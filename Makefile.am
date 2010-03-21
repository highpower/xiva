SUBDIRS = include libltdl library stdext utils

if HAVE_DAEMON
SUBDIRS += daemon
endif

if HAVE_PYTHON
SUBDIRS += python
endif

if HAVE_BOOST_UNIT_TEST
SUBDIRS += tests
endif

ACLOCAL_AMFLAGS = -I config

EXTRA_DIST = autogen.sh AUTHORS ChangeLog COPYING INSTALL NEWS README \
	config/ax_boost_header.m4 config/ax_boost_lib.m4 \
	config/ax_boost_multi_index.m4 config/ax_boost_prefix.m4 \
	config/ax_boost_python.m4 config/ax_boost_regex.m4 \
	config/ax_boost_required.m4 config/ax_boost_system.m4 config/ax_boost_thread.m4 \
	config/ax_boost_unit_test.m4 config/ax_check_compiler_flags.m4 \
	config/ax_prefix_config_h.m4 config/ax_python_devel.m4 pkg.m4
