check_PROGRAMS = xiva-tests

xiva_tests_SOURCES = main.cpp mock_connection.cpp mock_listener.cpp \
	mock_logger.cpp checker_test.cpp date_test.cpp line_reader_test.cpp \
	listener_test.cpp manager_test.cpp matcher_test.cpp queue_test.cpp \
	range_test.cpp request_test.cpp string_test.cpp urlencode_test.cpp

xiva_tests_LDADD = \
	${top_srcdir}/library/libxiva.la \
	${top_srcdir}/library/libxiva-loader.la
	
AM_CXXFLAGS = -pthread
AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/config \
	-I${top_srcdir}/daemon @BOOST_CPPFLAGS@

AM_LDFLAGS = @BOOST_LDFLAGS@ @BOOST_THREAD_LDFLAGS@ @BOOST_UNIT_TEST_LDFLAGS@

if HAVE_DAEMON
AM_LDFLAGS += @XML_LIBS@
AM_CPPFLAGS += @XML_CPPFLAGS@
xiva_tests_SOURCES += settings_test.cpp
xiva_tests_LDADD += ${top_srcdir}/daemon/libxiva-daemon.la
endif

noinst_DATA = test.conf

noinst_HEADERS = mock_connection.hpp mock_listener.hpp mock_logger.hpp \
	test_utils.hpp
	
TESTS = xiva-tests
