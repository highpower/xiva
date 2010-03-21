AC_DEFUN([AX_BOOST_REGEX],
[
	ax_boost_regex_stored_ldflags="$LDFLAGS"
	
	AC_REQUIRE([AX_BOOST_PREFIX])
	AX_BOOST_LIB([ax_boost_regex_lib], [boost_regex])
	
	LDFLAGS="$ax_boost_regex_stored_ldflags $BOOST_LDFLAGS -l$ax_boost_regex_lib"
	
	ax_have_boost_regex="yes"
	AX_BOOST_HEADER([regex.hpp], [], [ax_have_boost_regex="no"])
	
	AC_MSG_CHECKING([trying to link with boost::regex])
	AC_LINK_IFELSE(
		[ AC_LANG_PROGRAM([#include <boost/regex.hpp>], [ ;]) ],
		[ AC_MSG_RESULT(yes) ],
		[ AC_MSG_RESULT(no); ax_have_boost_regex="no" ])
	
	LDFLAGS="$ax_boost_regex_stored_ldflags"
	
	if test "f$ax_have_boost_regex" = "fyes"; then
		ifelse([$1], , :, [$1])
		AC_SUBST([BOOST_REGEX_LDFLAGS], ["-l$ax_boost_regex_lib"])
	else
		ifelse([$2], , :, [$2])
	fi	
])
