AC_PREREQ([2.69])
AC_INIT([fToxTun], [0.0.0], [mail@jschwab.org])
AC_CONFIG_SRCDIR([src/mainWindow.h])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.10 no-define foreign])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
PKG_CHECK_MODULES([TOXCORE], [libtoxcore])
PKG_CHECK_MODULES([TOXTUN], [libtoxtun])

# Check for fltk
AC_ARG_VAR(FLTK_CONFIG, fltk-config to use. Use this when fltk-config is not in PATH)
if test x"$FLTK_CONFIG" = x"";
then
	FLTK_CONFIG="fltk-config"
fi

FLTK_CFLAGS=`${FLTK_CONFIG} --cxxflags`
FLTK_LIBS=`${FLTK_CONFIG} --ldstaticflags`
AC_SUBST([FLTK_CFLAGS])
AC_SUBST([FLTK_LIBS])

# Check for mingw
AC_CANONICAL_HOST
DMINGW=""
case $host_os in
	*mingw*)
		DMINGW="-DMINGW -DPTW32_STATIC_LIB"
	;;
esac
AC_SUBST([DMINGW])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
