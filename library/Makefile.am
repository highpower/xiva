lib_LTLIBRARIES = libxiva.la

libxiva_la_LDFLAGS = @BOOST_THREAD_LDFLAGS@ @BOOST_SYSTEM_LDFLAGS@
libxiva_la_SOURCES = acceptor_base.cpp channels_stat_impl.cpp channels_stat.cpp \
	component_set.cpp compound_listener.cpp connection.cpp connection_base.cpp \
	connection_socket.cpp connection_data.cpp connection_listener.cpp connection_manager_base.cpp \
	connection_traits_base.cpp error.cpp formatter.cpp formatter_creator.cpp \
	formatters_data.cpp formatters_data_channels.cpp formatters_data_simple.cpp \
	formatters_factory.cpp handler_invoker.cpp handler_invoker_base.cpp http_constants.cpp \
	http.cpp http_error.cpp logger.cpp message.cpp message_filter.cpp \
	message_queue.cpp message_sender.cpp request.cpp request_impl.cpp \
	response.cpp response_handler.cpp response_impl.cpp server.cpp \
	server_impl.cpp settings.cpp ssl_connection_socket.cpp \
	ssl_context.cpp stdio_logger.cpp system_error.cpp \
	threaded_connection.cpp threaded_handler_invoker.cpp \
	threaded_listener.cpp url_response_handler.cpp websocket_info.cpp

noinst_LTLIBRARIES = libxiva-loader.la

libxiva_loader_la_LIBADD = libxiva.la
libxiva_loader_la_LDFLAGS = @LIBLTDL@
libxiva_loader_la_SOURCES = dynamic_loader.cpp

AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config \
	@INCLTDL@ @BOOST_CPPFLAGS@
AM_CXXFLAGS = -pthread
AM_LDFLAGS = @BOOST_LDFLAGS@ 
