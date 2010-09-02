#!/usr/bin/perl -w

use strict;
use Socket;

my $argc = $#ARGV;
if ($argc < 1) {
  print "Usage: xiva_cmd.pl <socket> <cmd-params>\n";
  exit(1);
}
my $sockpath = $ARGV[0];
shift;
my $cmd = join(" ", @ARGV);
print "CMD: '$cmd'\n";
my $ret = 2;
socket(S, AF_UNIX, SOCK_STREAM, 0);
if (connect(S, sockaddr_un($sockpath))) {
  if (send(S, $cmd, 0)) {
    if (recv(S, my $data, 1042 * 1024, 0)) {
      print $data, "\n";
      $ret = 0;
    }
  }
}
print "Fail\n" if $ret;
close S;
exit(2);
