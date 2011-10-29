AC_DEFUN([AX_BOOST_HEADER],
[
	ax_header_found="yes"
	AC_REQUIRE([AX_BOOST_PREFIX])

	ax_boost_header_stored_cppflags="$CPPFLAGS"
	CPPFLAGS="$ax_boost_header_stored_cppflags $BOOST_CPPFLAGS"
	AC_CHECK_HEADER([boost/$1], [], [ax_header_found="no"])
	CPPFLAGS="$ax_boost_header_stored_cppflags"
	if test "f$ax_header_found" = "fyes"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$3], , :, [$3])
	fi
])
