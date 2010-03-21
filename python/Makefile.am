pyexec_LTLIBRARIES = xiva.la

xiva_la_SOURCES = gil.cpp module.cpp python_listener.cpp python_logger.cpp \
	python_matcher.cpp python_settings.cpp server.cpp

xiva_la_LDFLAGS = -module -avoid-version
xiva_la_LIBADD = \
	${top_srcdir}/library/libxiva.la \
	${top_srcdir}/library/libxiva-loader.la

AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config \
	@PYTHON_CPPFLAGS@ @BOOST_CPPFLAGS@ @INCLTDL@
AM_CXXFLAGS = -pthread
AM_LDFLAGS = @BOOST_LDFLAGS@ @BOOST_PYTHON_LDFLAGS@ @BOOST_THREAD_LDFLAGS@ @PYTHON_LDFLAGS@

noinst_HEADERS = gil.hpp python_listener.hpp python_logger.hpp \
	python_matcher.hpp python_settings.hpp server_class.hpp server.hpp
