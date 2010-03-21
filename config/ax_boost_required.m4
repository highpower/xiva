AC_DEFUN([AX_BOOST_REQUIRED],
[
	ax_header_found="yes"
	AC_REQUIRE([AX_BOOST_PREFIX])
	
	ax_boost_requeired_stored_cppflags="$CPPFLAGS"
	CPPFLAGS="$ax_boost_requeired_stored_cppflags $BOOST_CPPFLAGS"
	AC_CHECK_HEADER([boost/$1], [], [ax_header_found="no"])
	CPPFLAGS="$ax_boost_requeired_stored_cppflags"
	
	if test "f$ax_header_found" != "fyes"; then
		AC_MSG_ERROR([required header boost/$1 is not found in $ax_boost_prefix/include])
	fi
])
