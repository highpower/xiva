AC_DEFUN([_AX_CHECK_BOOST_IN],
[
	ax_prefix=$1
	AC_MSG_CHECKING([whether boost is installed in $1])
	ax_have_boost_in="yes"
	if test ! -d "$ax_prefix/include/boost"; then
		ax_have_boost_in="no"
	fi
	if test ! -d "$ax_prefix/lib"; then
		ax_have_boost_in="no"
	fi
	AC_MSG_RESULT([$ax_have_boost_in])
	if test "f$ax_have_boost_in" = "fyes"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$3], , :, [$3])
	fi
])

AC_DEFUN([AX_BOOST_PREFIX],
[
	ax_boost_prefix=""
	AC_ARG_WITH([boost-prefix], 
		AS_HELP_STRING(--with-boost-prefix, specifies boost installation),
	[
		if test "f$withval" != "f"; then
			ax_boost_prefix="$withval"
		fi
	])
	if test "f$ax_boost_prefix" != "f"; then
		_AX_CHECK_BOOST_IN([$ax_boost_prefix], [], [ax_boost_prefix=""])
	else
		for i in /usr/local /usr; do
			_AX_CHECK_BOOST_IN([$i], [ax_boost_prefix="$i"], [])
		done;
	fi
	
	if test "f$ax_boost_prefix" != "f"; then
		AC_SUBST([BOOST_LDFLAGS], ["-I$ax_boost_prefix/lib"])
		AC_SUBST([BOOST_CPPFLAGS], ["-I$ax_boost_prefix/include"])
		ifelse([$1], , :, [$1])
	else
		ifelse([$2], , :, [$2])
	fi
])
