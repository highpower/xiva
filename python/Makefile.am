pyexec_LTLIBRARIES = xiva.la

xiva_la_SOURCES = interpreter_lock.cpp module.cpp python_formatter.cpp \
	python_formatter_creator.cpp python_handler.cpp \
	python_listener.cpp python_logger.cpp python_request.cpp \
	python_response.cpp python_server.cpp python_settings.cpp

xiva_la_LDFLAGS = -module -avoid-version @BOOST_LDFLAGS@ @BOOST_PYTHON_LDFLAGS@ @BOOST_THREAD_LDFLAGS@ @PYTHON_LDFLAGS@
xiva_la_LIBADD = \
	${top_srcdir}/library/libxiva.la \
	${top_srcdir}/library/libxiva-loader.la

AM_CXXFLAGS = -pthread
AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config \
	@PYTHON_CPPFLAGS@ @BOOST_CPPFLAGS@ @INCLTDL@

noinst_HEADERS = interpreter_lock.hpp python_enumeration.hpp \
	python_formatter.hpp python_formatter_creator.hpp \
	python_handler.hpp python_listener.hpp python_logger.hpp \
	python_request.hpp python_response.hpp python_server.hpp \
	python_settings.hpp request_class.hpp response_class.hpp \
	server_class.hpp
