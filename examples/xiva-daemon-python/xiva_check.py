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
# 7 - UNIX SOCKET WRITE ERROR
# 8 - UNIX SOCKET READ ERROR
# 9 - UNIX SOCKET UNKNOWN ERROR

import socket, sys, urllib2
from threading import Thread, Event

event_ready = Event()
event_result = Event()
event_sender = Event()

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
		except Exception, e:
			if self._debug:
				print "Checker failure: ", e
			self._failed = True
		except:
			self._failed = True
		event_result.set()

	def waitReady(self, timeout=1):
		for i in xrange(0, timeout * 20):
			if self._failed:
				return False
			if event_ready.isSet():
				return True
			event_ready.wait(0.05)
		return False

	def waitResult(self, timeout=1):
		for i in xrange(0, timeout * 20):
			if event_result.isSet():
				return True
			event_result.wait(0.05)
		return False


class Sender(Thread):
	def __init__ (self, sock, debug = False):
		Thread.__init__(self)
		self._sock = sock
		self._result = None
		self._sent = False
		self._debug = debug

	def result(self):
		return self._result

	def isSentData(self):
		return self._sent

	def run(self):
		if self._debug:
			print "Start sender thread"
		try:
			self._sock.send('msg ' + USER + '/' + CHANNEL + ' ' + TEST_MSG)
			self._sent = True
			self._result = self._sock.recv(1024*1024)
			if self._debug:
				print 'Recv from xiva:', self._result
		except Exception, e:
			if self._debug:
				print "Sender failure: ", e
		except:
			pass
		event_sender.set()

	def waitResult(self, timeout=1):
		for i in xrange(0, timeout * 20):
			if event_sender.isSet():
				return True
			event_sender.wait(0.05)
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
		checker = Checker(sys.argv[2], debug)
		sender = Sender(uds, debug)

		# start thread and wait ready event (1s)
		checker.start()
		if not checker.waitReady():
			print "HTTP CONNECT ERROR"
			exit(3)

		# send message and wait result event (1s)
		sender.start()
		sender_result = sender.waitResult();
		if not sender.isSentData():
			print "UNIX SOCKET WRITE ERROR"
			exit(7)
		if sender.result() is None:
			print "UNIX SOCKET READ ERROR"
			exit(8)
		if not sender_result:
			print "UNIX SOCKET UNKNOWN ERROR"
			exit(9)

		if not checker.waitResult():
			print "HTTP READ ERROR"
			exit(4)

		# dump result message and analyse
		result = checker.msg()
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
