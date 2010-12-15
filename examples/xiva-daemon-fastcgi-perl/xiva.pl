#!/usr/bin/perl

use strict;
use warnings;

use FCGI;
use Socket;

use Encode;
use Date::Format;
use utf8;

my $request = FCGI::Request();
my $sockpath = '/tmp/xivadaemon.sock';

my $log_dir = '/var/log/xiva-daemon-fastcgi';

sub json_decode {
  my $ss = $_[0];
  return if ($ss eq "");
  $ss =~ s/\\\\u/}\\x{/g;
  $ss = '\\' . substr($ss, 2) . '}';
  my $sss = eval('"' . $ss . '"');
  return Encode::encode('utf8', $sss);
}

sub wrap {
  my $ss = $_[0];
  return if ($ss eq "");
  my $str = json_decode($ss);
  $str =~ s/\&/\&amp;/g;
  $str =~ s/\</\&lt;/g;
  $str =~ s/\>/\&gt;/g;
  return $str;
}

sub log_message {
  my $s = $_[0];
  my %h;
  foreach my $i (split(',', $s)) {
    #print $i, "\n";
    my ($n, $v) = split(':', $i);
    my ($n1, $name, $n2) = split('"', $n);
    my ($v1, $value, $v2) = split('"', $v);
    $h{$name} = $value;
  }
  if (defined($h{'username'}) && defined($h{'type'})) {
    if ($h{'type'} eq 'message') {
      if (open(my $fh, ">>", $log_dir."/history_".$h{'room'}.".log")) {
        if (chmod(0644, $fh)) {
          my @lt = localtime(time);
          print $fh "<td>", strftime("%Y-%m-%d %H:%M:%S", @lt), "</td><td>", wrap($h{'username'}), "</td><td>", wrap($h{'text'}), "</td>\n";
        }
        close $fh;
      }
    }
  }
}

sub process_history {
  my $q = $_[0];

  my $fl;
  if ($q =~ /^history\/(.*)$/ && open($fl, "<", $log_dir.'/history_'.$1)) {
    print "Status: 200\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
    print '<!DOCTYPE html>
<head>
<meta http-equiv="content-type" content="text/html;charset=utf-8"/>
</head>
<body>
<table border="1" cellpadding="5px">
<tr style="font-weight:normal;background-color:#dddddd"><td><b>Date</b></td><td><b>User</b></td><td><b>Message</b></td></tr>
';
    local $_;
    my $i = 0;
    while (<$fl>) {
      chomp;
      my $tr = $i ? '<tr style="font-weight:normal;background-color:#e8e8e8">' : '<tr>';
      print $tr.$_."</tr>\n";
      $i = 1 - $i;
    }
    close $fl;
    print "</table></body></html>\n";
  }
  else {
    print "Status: 404\r\nContent-Type: text/plain\r\n\r\nNot Found\n";
  }
}

sub URLDecode {
  my $theURL = $_[0];
  $theURL =~ tr/+/ /;
  $theURL =~ s/%([a-fA-F0-9]{2,2})/chr(hex($1))/eg;
  $theURL =~ s/<!.(.|\n)*.>//g;
  return $theURL;
}

while ($request->Accept() >= 0) {
  my $q = defined $ENV{'QUERY_STRING'} ? URLDecode($ENV{'QUERY_STRING'}) : '';
  if ($q ne '') {
    process_history($q);
    next;
  }

  my $status = 500;
  my $type = 'text/plain';
  my $content = 'failed';
  if ($q eq '') {
    my $stdin_passthrough ='';
    my $req_len = 0 + $ENV{'CONTENT_LENGTH'};
    if (($ENV{'REQUEST_METHOD'} eq 'POST') && ($req_len != 0) ){
      my $bytes_read = 0;
      while ($bytes_read < $req_len) {
        my $data = '';
        my $bytes = read(STDIN, $data, ($req_len - $bytes_read));
        last if ($bytes == 0 || !defined($bytes));
        $stdin_passthrough .= $data;
        $bytes_read += $bytes;
      }
    }
    $q = URLDecode($1) if $stdin_passthrough =~ /^cmd=(.*)$/;
  }
  if ($q ne '') {
    socket(S, AF_UNIX, SOCK_STREAM, 0);
    if (connect(S, sockaddr_un($sockpath))) {
      if (send(S, $q, 0)) {
        if (recv(S, my $data, 1042 * 1024, 0)) {
          $content = $data;
	  $status = 200;
	  log_message($q);
        }
      }
    }
    close S;
  }

  print "Status: $status\r\nContent-Type: $type\r\n";
  #print "$header: $path\r\n" if $header;
  print "\r\n$content\n";
}

