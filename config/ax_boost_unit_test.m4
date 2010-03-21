AC_DEFUN([AX_BOOST_UNIT_TEST],
[
	ax_boost_unit_test_stored_ldflags="$LDFLAGS"
	
	AC_REQUIRE([AX_BOOST_PREFIX])
	AX_BOOST_LIB([ax_boost_unit_test_lib], [boost_unit_test_framework])
	
	LDFLAGS="$ax_boost_unit_test_stored_ldflags $BOOST_LDFLAGS -l$ax_boost_unit_test_lib"
	
	ax_have_boost_unit_test="yes"
	AX_BOOST_HEADER([test/unit_test.hpp], [], [ax_have_boost_unit_test="no"])
	AX_BOOST_HEADER([test/test_case_template.hpp], [], [ax_have_boost_unit_test="no"])
	
	AC_MSG_CHECKING([trying to link with boost::test])
	AC_LINK_IFELSE(
		[ AC_LANG_PROGRAM([#include <boost/test/unit_test.hpp>], []) ],
		[ AC_MSG_RESULT(yes) ],
		[ AC_MSG_RESULT(no); ax_have_boost_unit_test="no" ])
	
	LDFLAGS="$ax_boost_unit_test_stored_ldflags"
	
	if test "f$ax_have_boost_unit_test" = "fyes"; then
		ifelse([$1], , :, [$1])
		AC_SUBST([BOOST_UNIT_TEST_LDFLAGS], ["-l$ax_boost_unit_test_lib"])
	else
		ifelse([$2], , :, [$2])
	fi	
])
