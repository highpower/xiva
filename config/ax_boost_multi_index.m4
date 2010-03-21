AC_DEFUN([AX_BOOST_MULTI_INDEX],
[
	ax_boost_multi_index_stored_cppflags="$CPPFLAGS"
	AC_REQUIRE([AX_BOOST_PREFIX])
	CPPFLAGS="$ax_boost_multi_index_stored_cppflags $BOOST_CPPFLAGS -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION"
	
	ax_have_boost_multi_index="yes"
	AX_BOOST_HEADER([multi_index_container.hpp], [], [ax_have_boost_multi_index="no"])
	AX_BOOST_HEADER([multi_index/hashed_index.hpp], [], [ax_have_boost_multi_index="no"])
	AX_BOOST_HEADER([multi_index/ordered_index.hpp], [], [ax_have_boost_multi_index="no"])
	CPPFLAGS="$ax_boost_multi_index_stored_cppflags"
	if test "f$ax_have_boost_multi_index" = "fyes"; then
		ifelse([$1], , :, [$1])
	else
		ifelse([$2], , :, [$2])
	fi	
])
