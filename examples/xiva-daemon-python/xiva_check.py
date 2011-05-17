#!/usr/bin/env python
#
# -*- coding: utf-8 -*-

# ERROR_CODE:
# 0 - OK
# 1 - USAGE ERROR
# 2 - FAILURE, INIT ERROR, ERROR ON CONNECT TO XIVA DAEMON (unix socket)
# 3 - HTTP CONNECT ERROR, XIVA DAEMON (http port)
# 4 - HTTP READ ERROR, XIVA DAEMON (http port)
# 5 - EMPTY RESPONSE
# 6 - INVALID RESPONSE

import socket, sys, urllib2
from threading import Thread, Event

event_ready = Event()
event_result = Event()

USER='www-test'
CHANNEL='test'
TEST_MSG = 'TEST-MSG-OK'

class Checker(Thread):
	def __init__ (self, port, debug = False):
		Thread.__init__(self)
		self._port = port
		self._msg = None
		self._failed = False
		self._debug = debug

	def msg(self):
		return self._msg

	def run(self):
		if self._debug:
			print "Start checker thread"
		try:
			url = 'http://localhost:' + self._port + '/?single&id=' + USER + '&channel=' + CHANNEL
			if self._debug:
				print "Connect to xiva-server: " + url
			stream = urllib2.urlopen(url)
			event_ready.set()
			if self._debug:
				print "Try get data"
			self._msg = stream.read()
		except:
			self._failed = True
		event_result.set()

	def waitEvent(self, event, timeout=1):
		for i in xrange(0, timeout * 20):
			if self._failed:
				return False
			if event.isSet():
				return True
			event.wait(0.05)
		return False


if __name__ == "__main__":
	argc = len(sys.argv)
	if argc != 3:
		print 'Usage:\n' + sys.argv[0] + ' <unix-socket> <http-port>';
		exit(1)

	try:
		# init
		debug = False
		uds = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		uds.connect(sys.argv[1])
		ch = Checker(sys.argv[2], debug)

		# start thread and wait ready event (1s)
		ch.start()
		if not ch.waitEvent(event_ready):
			print "HTTP CONNECT ERROR"
			exit(3)

		# send message and wait result event (1s)
		uds.send('msg ' + USER + '/' + CHANNEL + ' ' + TEST_MSG)
		data = uds.recv(1024*1024)
		if debug:
			print 'Recv from xiva:', data
		if not ch.waitEvent(event_result):
			print "HTTP READ ERROR"
			exit(4)

		# dump result message and analyse
		result = ch.msg()
		if result is None:
			print "EMPTY RESPONSE"
			exit(5)

		print "Http result: ", result
		if result != TEST_MSG + '\n':
			print "INVALID RESPONSE"
			exit(6)

	except Exception, e:
		print "FAILURE: ", e
		exit(2)

	exit(0)
