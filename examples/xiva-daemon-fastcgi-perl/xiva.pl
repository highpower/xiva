#!/usr/bin/perl

use strict;
use warnings;

use FCGI;
use Socket;

my $request = FCGI::Request();
my $sockpath = '/tmp/xivadaemon.sock';

sub URLDecode {
  my $theURL = $_[0];
  $theURL =~ tr/+/ /;
  $theURL =~ s/%([a-fA-F0-9]{2,2})/chr(hex($1))/eg;
  $theURL =~ s/<!.(.|\n)*.>//g;
  return $theURL;
}

while ($request->Accept() >= 0) {
  my $status = 500;
  my $type = 'text/plain';
  my $content = 'failed';
  my $q = defined $ENV{'QUERY_STRING'} ? URLDecode($ENV{'QUERY_STRING'}) : '';
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
        }
      }
    }
    close S;
  }

  print "Status: $status\r\nContent-Type: $type\r\n";
  #print "$header: $path\r\n" if $header;
  print "\r\n$content\n";
}

