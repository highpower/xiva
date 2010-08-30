#!/usr/bin/env python
#
# -*- coding: utf-8 -*-

import socket, sys

def print_usage():
	print 'Usage:\n' + sys.argv[0] + ' <socket> ( send <id>[/<channel>] <message>... | list [<channel>] )';
	exit(1)

if __name__ == "__main__":
	argc = len(sys.argv)
	if argc < 3:
		print_usage()
	msg = ''; uds_path = sys.argv[1]; cmd = sys.argv[2]
	if cmd == 'list':
		if argc == 3:
			msg = cmd
		elif argc == 4:
			msg = cmd + ' ' + sys.argv[3]
		else:
			print_usage()
	else:
		if argc < 5:
			print_usage()
		if cmd != 'send':
			print_usage()
		idx = 4
		msg = 'msg ' + sys.argv[3] # <id>[/channel] 
		for i in xrange(idx, argc):
			msg += ' ' + sys.argv[i]
	print 'Send:', msg
	uds = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
	try:
		try:
			uds.connect(uds_path)
			uds.send(msg)
			print 'Recv:', uds.recv(1024*1024)
		finally:
			uds.close()
	except Exception, e:
		print "Failed", e
		exit(2)
