pkglib_LTLIBRARIES = xiva-syslog.la xiva-stdin-reader.la

xiva_syslog_la_LIBADD = ${top_srcdir}/library/libxiva.la
xiva_syslog_la_SOURCES = syslog_logger.cpp
xiva_syslog_la_LDFLAGS = -module

xiva_stdin_reader_la_LIBADD = ${top_srcdir}/library/libxiva.la
xiva_stdin_reader_la_SOURCES = stdin_reader.cpp
xiva_stdin_reader_la_LDFLAGS = -module

AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config @BOOST_CPPFLAGS@
AM_CXXFLAGS = -pthread
AM_LDFLAGS = @BOOST_LDFLAGS@
