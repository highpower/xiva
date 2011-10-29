bin_PROGRAMS = xiva-util

xiva_util_SOURCES = main.cpp
xiva_util_LDFLAGS = @BOOST_THREAD_LDFLAGS@ @BOOST_SYSTEM_LDFLAGS@

AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config @BOOST_CPPFLAGS@
AM_CXXFLAGS = -pthread
AM_LDFLAGS = @BOOST_LDFLAGS@
