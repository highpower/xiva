#!/usr/bin/env python
#
# -*- coding: utf-8 -*-

import socket, os, sys, logging, xiva
from threading import Thread


def printUsage():
	print 'Usage:\n' + sys.argv[0] + ' --config=<filepath>'
	exit(1)

def splitChannel(channel, sep_key='_', sep_data=':'):
	p1 = channel.partition(sep_data)
	p2 = p1[0].partition(sep_key)
	return [p2[0], p2[2], p1[2]]


class XivaSettings(object):
	def __init__(self, config_fpath, log):
		self._cfg = eval(file(config_fpath).read())
		self.__log = log

	def initLogger(self, log):
		log_cfg = self._cfg.get('log')
		if type(log_cfg) is dict:
			logtype = log_cfg.get('type')
			if logtype and logtype == 'syslog':
				from logging.handlers import SysLogHandler
				syslog = SysLogHandler(address='/dev/log', facility=SysLogHandler.LOG_DAEMON)
				formatter = logging.Formatter('%(name)s: %(levelname)s %(message)s')
				syslog.setFormatter(formatter)
				log.addHandler(syslog)
			else:
				logging.basicConfig()
			loglevel_name = log_cfg.get('level')
			if loglevel_name:
				loglevel = logging.getLevelName(loglevel_name)
				if loglevel:
					log.setLevel(loglevel)
			logname = log_cfg.get('name')
			if logname:
				log.name = logname

	def value(self, name):
		v = self._cfg.get(name)
		if v is not None:
			self.__log.debug('config value("' + name + '"): ' + str(v))
			return v
		self.__log.warning('config value("' + name + '") was not found')
		return None # use default


class XivaHandler(object):
	def has_enough_data(self, req):
		if req.uri() == "/ping":
			return True
		return len(req.param('id')) > 0

	def receiver(self, req):
		if req.uri() == "/ping":
			return ""
		return req.param('id')

	def handle_response(self, req, resp):
		if req.uri() == "/ping":
			resp.content_type('text/plain; charset=utf-8')
			resp.content('pong\n')
			return
		resp.content_type('application/x-javascript; charset=utf-8')
		resp.single_message(req.has_param("single"))
		fmt_id = "simple"
		resp.formatter_id(fmt_id)
		channels = req.param("channels")
		if len(channels) > 0:
			for ch in channels.split(','):
				pp = splitChannel(ch)
				resp.formatter_by_channel(pp[0], pp[1], pp[2], fmt_id)


class XivaApp(Thread):
	def __init__ (self, server, settings):
		Thread.__init__(self)
		self._server = server
		self._started = False
		self._server.init(settings)
		self._server.list_channels_enable()

	def run(self):
		if not self._started:
			self._started = True
			self._server.start()

	def stop(self):
		self._server.stop()
		if self._started:
			self.join()
			self._started = False


class XivaListenerLog(object):
	def __init__(self, log):
		self.__log = log

	def connection_opened(self, to, id):
		self.__log.info('listener: con opened to=%s id=%d' % (to, id))

	def connection_closed(self, to, id):
		self.__log.info('listener: con closed to=%s id=%d' % (to, id))

	def disconnected(self, to):
		self.__log.info('listener: a message was dropped because disconnected to=' + to)


class XivaFormatterSimple(object):
	def __init__(self, req):
		pass

	def wrap_message(self, content):
		return content + "\n" #"{'" + content + "'}\n" #simple version

	def ping_message(self):
		return "ping\n"


class XivaMessageFilter(object):
	def allow_message(self, prefix, key, msg_data, ch_data):
		return len(msg_data) == 0 or len(ch_data) == 0 or long(msg_data) > long(ch_data)


class XivaFormatterCreatorSimple(object):
	def create(self, req):
		return XivaFormatterSimple(req)

	def name(self):
	    return 'simple'


class UdsServer(object):
	def __init__(self, uds_path, log, xiva_server, input_msg_size=10240):
		self.__uds = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		self.__log = log
		self.__uds_path = uds_path
		self.__input_msg_size = input_msg_size

	def listen(self, backlog=10):
		self.__unlink_path()
		self.__xiva_server = xiva_server
		self.__uds.bind(self.__uds_path)
		os.chmod(self.__uds_path, 0666)
		self.__uds.listen(backlog)

	def __unlink_path(self):
		try:
			os.unlink(self.__uds_path)
		except:
			pass

	def run(self):
		try:
			try:
				while 1:
					self.__run_one()
			except KeyboardInterrupt:
				pass
		finally:
			self.__uds.close()
			self.__unlink_path()

	def __run_one(self):
		try:
			sock, address = self.__uds.accept()
			self.handle(sock)
		except socket.error, e:
			if e[0] == os.errno.EPIPE:
				pass # It's happens when the client closes the connection

	def handle(self, sock):
		data = sock.recv(self.__input_msg_size)
		pp = data.partition(' ')
		if pp[0] == 'list':
			self.handle_list(sock, pp[2])
		elif pp[0] == 'msg':
			self.handle_msg(sock, pp[2])
		else:
			sock.send('unknown command "' + pp[0] + '"')

	def handle_list(self, sock, channel):
		res = []; l = 0; out = ''
		if len(channel) > 0:
			res = self.__xiva_server.list_channel(channel)
		else:
			res = self.__xiva_server.list_channels()
		for i in res:
			out += i + '\n'
			l += 1
		msg = str(l) + ':\n' + out
		sock.send(msg)

	def handle_msg(self, sock, data):
		err_msg = self.send_msg(data)
		if err_msg and len(err_msg) > 0:
			self.__log.error(err_msg)
			sock.send(err_msg)
		else:
			sock.send('OK')

	def send_msg(self, data):
		pp = data.partition('/')
		id = pp[0].strip()
		if not id or len(id) == 0:
			return 'empty id'
		channel, sep, msg = pp[2].partition(' ')
		if not msg or len(msg) == 0:
			return 'empty msg for' + id
		if len(channel) > 0:
			pp = splitChannel(channel)
			self.__xiva_server.send_to_channel(id, msg, pp[0], pp[1], pp[2])
		else:
			self.__xiva_server.send(id, msg)


if __name__ == "__main__":
	if len(sys.argv) != 2:
		printUsage()

	pp = sys.argv[1].rpartition('=')
	if pp[0] != '--config':
		printUsage()
	config_fpath = pp[2].strip()
	if len(config_fpath) == 0:
		printUsage()

	log = logging.getLogger()
	log.setLevel(logging.INFO)

	xiva_settings = XivaSettings(config_fpath, log)
	xiva_settings.initLogger(log)
	log.debug('xiva settings init')

	uds_path = xiva_settings.value('socket');
	if (uds_path is None) or len(uds_path) == 0:
		print 'undefined "socket" in config'
		exit(2)

	pid_path = xiva_settings.value('pidfile');
	if (pid_path is None) or len(pid_path) == 0:
		print 'undefined "socket" in config'
		exit(3)

	xiva_handler = XivaHandler()
	xiva_listener = XivaListenerLog(log)
	xiva_formatter_creator = XivaFormatterCreatorSimple()
	xiva_message_filter = XivaMessageFilter()

	xiva_server = xiva.server()
	xiva_server.attach_logger(log)
	xiva_server.add_connection_listener(xiva_listener)
	xiva_server.attach_response_handler(xiva_handler)
	xiva_server.attach_formatter_creator(xiva_formatter_creator.name(), xiva_formatter_creator)
	xiva_server.attach_message_filter(xiva_message_filter)

	uds_server = UdsServer(uds_path, log, xiva_server)

	xiva_app = XivaApp(xiva_server, xiva_settings)
	log.info('xiva init')

	uds_server.listen()
	try:
		log.debug('xiva starting')
		xiva_app.start()
		log.info('xiva started')
		open(pid_path, 'w').write(str(os.getpid()))
		uds_server.run()
	finally:
		log.debug('xiva stopping')
		xiva_app.stop()
		log.info('xiva stopped')
		try:
			os.unlink(pid_path)
		except:
			pass
