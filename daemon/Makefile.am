sbin_PROGRAMS = xiva-daemon

xiva_daemon_SOURCES = command_line.cpp main.cpp xml.cpp \
	xml_settings.cpp xml_variable_map.cpp
	
xiva_daemon_LDADD = \
	${top_srcdir}/library/libxiva.la \
	${top_srcdir}/library/libxiva-loader.la

AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config \
	@LIBXML_CFLAGS@ @BOOST_CPPFLAGS@ @INCLTDL@
AM_CXXFLAGS = -pthread
AM_LDFLAGS = @BOOST_LDFLAGS@ @BOOST_REGEX_LDFLAGS@ @LIBXML_LIBS@

noinst_HEADERS = command_line.hpp xml.hpp \
	xml_settings.hpp xml_variable_map.hpp

dist_sysconf_DATA = xiva.conf.example
