lib_LTLIBRARIES = libxiva.la

libxiva_la_LDFLAGS = @BOOST_THREAD_LDFLAGS@ @BOOST_SYSTEM_LDFLAGS@
libxiva_la_SOURCES = acceptor_base.cpp component_set.cpp \
	compound_listener.cpp connection_base.cpp connection.cpp \
	connection_data.cpp connection_listener.cpp connection_manager_base.cpp \
	connection_traits_base.cpp error.cpp http_constants.cpp http.cpp \
	http_error.cpp logger.cpp matcher_invoker.cpp message.cpp \
	message_queue.cpp message_sender.cpp receiver_matcher.cpp \
	request.cpp request_impl.cpp response.cpp server.cpp server_impl.cpp \
	settings.cpp stdio_logger.cpp system_error.cpp threaded_connection.cpp \
	threaded_listener.cpp threaded_matcher_invoker.cpp url_matcher.cpp \
	websocket_info.cpp

noinst_LTLIBRARIES = libxiva-loader.la

libxiva_loader_la_LIBADD = libxiva.la
libxiva_loader_la_LDFLAGS = @LIBLTDL@
libxiva_loader_la_SOURCES = dynamic_loader.cpp

AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config \
	@INCLTDL@ @BOOST_CPPFLAGS@
AM_CXXFLAGS = -pthread
AM_LDFLAGS = @BOOST_LDFLAGS@ 
