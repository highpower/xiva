AC_DEFUN([AX_BOOST_LIB],
[
	AC_REQUIRE([AX_BOOST_PREFIX])
	AC_MSG_CHECKING([for $2])
	ax_check=$2
	ax_library="$ax_check"
	ax_value=`echo $CPPFLAGS $CXXFLAGS | grep -- -pthread`
	if test "f$ax_value" != "f"; then
		ax_check="$ax_check-mt"
	fi
	if test -f "$ax_boost_prefix/lib/lib$ax_check.so"; then
		ax_library="$ax_check"
	fi
	ax_value=`echo $CPPFLAGS $CXXFLAGS | grep -- -g`
	if test "f$ax_value" != "f"; then
		ax_check="$ax_check-d"
	fi
	if test -f "$ax_boost_prefix/lib/lib$ax_check.so"; then
		ax_library="$ax_check"
	fi
	$1="$ax_library"
	AC_MSG_RESULT([$ax_library])
])
