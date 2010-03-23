noinst_LTLIBRARIES = libxiva-daemon.la

libxiva_daemon_la_LIBADD = ${top_srcdir}/library/libxiva.la
libxiva_daemon_la_SOURCES = command_line.cpp xml.cpp xml_settings.cpp \
	variable_map.cpp

sbin_PROGRAMS = xiva-daemon
xiva_daemon_SOURCES = main.cpp 
xiva_daemon_LDADD = libxiva-daemon.la ${top_srcdir}/library/libxiva-loader.la

AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config \
	@XML_CPPFLAGS@ @BOOST_CPPFLAGS@ @INCLTDL@

AM_CXXFLAGS = -pthread
AM_LDFLAGS = @BOOST_LDFLAGS@ @XML_LIBS@

noinst_HEADERS = command_line.hpp xml.hpp xml_settings.hpp \
	variable_map.hpp

dist_sysconf_DATA = xiva.conf.example
