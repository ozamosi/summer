m4_define([summer_major_version], [0])
m4_define([summer_minor_version], [2])
m4_define([summer_micro_version], [0])
m4_define([summer_version],
          [summer_major_version.summer_minor_version.summer_micro_version])

AC_INIT([libsummer], [summer_version], [http://wrya.net/services/trac/summer], [libsummer])
AC_CONFIG_SRCDIR([libsummer/summer.h])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])

SUMMER_MAJOR_VERSION=summer_major_version
SUMMER_MINOR_VERSION=summer_minor_version
SUMMER_MICRO_VERSION=summer_micro_version
SUMMER_VERSION=summer_version

AC_SUBST(SUMMER_MAJOR_VERSION)
AC_SUBST(SUMMER_MINOR_VERSION)
AC_SUBST(SUMMER_MICRO_VERSION)
AC_SUBST(SUMMER_VERSION)

AC_DEFINE(SUMMER_MAJOR_VERSION, [summer_major_version],
      [Define to the SUMMER major version])
AC_DEFINE(SUMMER_MINOR_VERSION, [summer_minor_version],
      [Define to the SUMMER minor version])
AC_DEFINE(SUMMER_MICRO_VERSION, [summer_micro_version],
      [Define to the SUMMER micro version])

AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O

AC_PROG_LIBTOOL
DOLT

GNOME_COMMON_INIT
GNOME_DOC_INIT
GNOME_MAINTAINER_MODE_DEFINES
GNOME_COMPILE_WARNINGS
GNOME_CXX_WARNINGS

GTK_DOC_CHECK(1.9)

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

GLIB_REQUIRED=2.18.0
LIBSOUP_REQUIRED=2.24
LIBTORRENT_REQUIRED=0.14
LIBXML_REQUIRED=2.6
VALA_REQUIRED=0.5.0

AM_PATH_GLIB_2_0($GLIB_REQUIRED,,,gobject gio gmodule)
if test "$GLIB_LIBS" = ""; then
	AC_MSG_ERROR(GLIB $GLIB_REQUIRED or later is required to build libsummer)
fi
GLIB_CFLAGS="$GLIB_CFLAGS -DG_DISABLE_SINGLE_INCLUDES"

PKG_CHECK_MODULES(SOUP, libsoup-2.4 >= $LIBSOUP_REQUIRED)
PKG_CHECK_MODULES(LIBXML, libxml-2.0 >= $LIBXML_REQUIRED)

# ***************
# Torrent backend
# ***************
AC_MSG_CHECKING([whether bittorrent support is requested])
AC_ARG_ENABLE([bittorrent],
	AS_HELP_STRING([--enable-bittorrent],[Enable bittorrent support]),
	[enable_bittorrent=$enableval],
	[enable_bittorrent=autodetect])
AC_MSG_RESULT([$enable_bittorrent])

if test "x$enable_bittorrent" != "xno"; then
	PKG_CHECK_MODULES(LIBTORRENT, libtorrent-rasterbar >= $LIBTORRENT_REQUIRED, [
		enable_bittorrent=yes
		SUMMER_CFLAGS+=-DENABLE_BITTORRENT
	], [
		if test "x$enable_bittorrent" = "xyes"; then
			AC_MSG_ERROR([libtorrent-rasterbar not found])
		else
			AC_MSG_WARN([libtorrent-rasterbar not found, disabling bittorrent support])
			enable_bittorrent=no
		fi
	])
fi

AM_CONDITIONAL([ENABLE_BITTORRENT],[test "x$enable_bittorrent" = "xyes"])

# ******
# Python
# ******

AC_MSG_CHECKING([whether python support is requested])
AC_ARG_ENABLE([python],
	AS_HELP_STRING([--enable-python],[Enable python support]),
	[enable_python=$enableval have_python=$enableval],
	[enable_python=autodetect have_python=yes])
AC_MSG_RESULT([$enable_python])

if test "x$have_python" != "xno"; then
	AM_PATH_PYTHON([2.3],[],[no])
	if test "x$PYTHON" = "x:"; then
		have_python=no
	fi
fi

if test "x$have_python" != "xno"; then
	AM_CHECK_PYTHON_HEADERS([],[have_python=no])
fi

if test "x$have_python" != "xno"; then
	PY_PREFIX=`$PYTHON -c 'import sys ; print sys.prefix'`
	PY_EXEC_PREFIX=`$PYTHON -c 'import sys ; print sys.exec_prefix'`
	PYTHON_LIBS="-lpython$PYTHON_VERSION"
	PYTHON_LIB_LOC="-L$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config"
	PYTHON_CFLAGS="-I$PY_PREFIX/include/python$PYTHON_VERSION"
	PYTHON_MAKEFILE="$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config/Makefile"
	PYTHON_LOCALMODLIBS=`sed -n -e 's/^LOCALMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_BASEMODLIBS=`sed -n -e 's/^BASEMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_OTHER_LIBS=`sed -n -e 's/^LIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_EXTRA_LIBS="$PYTHON_LOCALMODLIBS $PYTHON_BASEMODLIBS $PYTHON_OTHER_LIBS"
	AC_SUBST([PYTHON_LIBS])
	AC_SUBST([PYTHON_LIB_LOC])
	AC_SUBST([PYTHON_CFLAGS])
	AC_SUBST([PYTHON_EXTRA_LIBS])
fi 

if test "x$have_python" != "xyes"; then
	if test "x$enable_python" = "xyes"; then
		AC_MSG_ERROR([Python not found])
	elif test "x$enable_python" = "xautodetect"; then
		enable_python=no
		AC_MSG_WARN([Python not found, disabling python support])
	fi
fi

if test "x$have_python" != "xno"; then
	PYGOBJECT_REQUIRED=2.15.2

	PKG_CHECK_MODULES([PYGOBJECT], [pygobject-2.0 >= $PYGOBJECT_REQUIRED], [], [
		have_python=no
		if test "x$enable_python" = "xyes"; then
			AC_MSG_ERROR([$PYGOBJECT_PKG_ERRORS])
		elif test "x$enable_python" = "xautodetect"; then
			enable_python=no
			AC_MSG_WARN([$PYGOBJECT_PKG_ERRORS])
			AC_MSG_WARN([Disabling python support])
		fi
	])

	PYGLIB_LIBS=
	BASE=$(pkg-config --variable=libdir gobject-2.0);
	if test -f $BASE/libpyglib-2.0-python$PYTHON_VERSION.so; then
		PYGLIB_LIBS="-L$BASE -lpyglib-2.0-python$PYTHON_VERSION";
	else
		PYGLIB_LIBS="-L$BASE -lpyglib-2.0";
	fi
	AC_SUBST([PYGLIB_LIBS])

	AC_SUBST([PYGOBJECT_CFLAGS])
	AC_SUBST([PYGOBJECT_LIBS])
fi

if test "x$have_python" != "xno"; then
	AC_MSG_CHECKING([for pygobject defs])
	PYGOBJECT_DEFSDIR=`$PKG_CONFIG --variable=defsdir pygobject-2.0`
	AC_MSG_RESULT([$PYGOBJECT_DEFSDIR])

	AC_MSG_CHECKING([for pygobject codegen])
	PYGOBJECT_CODEGEN="$PYTHON `$PKG_CONFIG --variable=codegendir pygobject-2.0`/codegen.py"
	AC_MSG_RESULT([$PYGOBJECT_CODEGEN])
	
	AC_SUBST([PYGOBJECT_DEFSDIR])
	AC_SUBST([PYGOBJECT_CODEGEN])
fi

if test "x$have_python" != "xno" -a "x$enable_python" != "xno"; then
	enable_python=yes
	AC_DEFINE([ENABLE_PYTHON],[1],[Define to compile with python support])
fi 
AM_CONDITIONAL([ENABLE_PYTHON],[test "x$enable_python" = "xyes"]) 

# ***
# Vala
# ***

AC_MSG_CHECKING([whether vala support is requested])
AC_ARG_ENABLE([vala],
	AS_HELP_STRING([--enable-vala],[Enable vala support]),
	[enable_vala=$enableval],
	[enable_vala=autodetect])
AC_MSG_RESULT([$enable_vala])

if test "x$enable_vala" != "xno"; then
	PKG_CHECK_MODULES(VALA, vala-1.0 >= $VALA_REQUIRED, [
		VAPIGEN="$($PKG_CONFIG --variable=vapigen vala-1.0)"
		VALA_GEN_INTROSPECT="$($PKG_CONFIG --variable=vala_gen_introspect vala-1.0)"
		AC_SUBST(VAPIGEN)
		AC_SUBST(VALA_GEN_INTROSPECT)
		enable_vala=yes
	], [
		if test "x$enable_vala" = "xyes"; then
			AC_MSG_ERROR([Vala not found])
		else
			AC_MSG_WARN([Vala not found, disabling vala support])
			enable_vala=no
		fi
	])
fi

AM_CONDITIONAL([ENABLE_VALA],[test "x$enable_vala" = "xyes"])


# *****************
# coverage analysis
# *****************

AC_MSG_CHECKING(whether to enable coverage analysis)
AC_ARG_ENABLE(
	coverage,
	AS_HELP_STRING([--enable-coverage],[enable coverage analysis (default=no)]),
	,
	[enable_coverage="no"])
AC_MSG_RESULT($enable_coverage)
if test "$enable_coverage" = "yes"; then
	COVERAGE_CFLAGS="-O0 -g -fprofile-arcs -ftest-coverage"
	COVERAGE_LIBS="-lgcov"

	AC_PATH_PROG(lcov_path, [lcov], [no])
	if test "x$lcov_path" = "xno" ; then
		AC_MSG_ERROR([lcov not found])
	else
		LCOV_VERSION=`lcov --version|head -n 1| $SED 's/^.* version //'`
		LCOV_MAJOR=`echo $LCOV_VERSION | cut -d. -f1 | $SED s/[a-zA-Z\-].*//g`
		LCOV_MINOR=`echo $LCOV_VERSION | cut -d. -f2 | $SED s/[a-zA-Z\-].*//g`
		if [[ "$LCOV_MAJOR" -lt "1" ]]; then
			AC_MSG_WARN([lcov >= 1.6 is required, you have $LCOV_VERSION])
			lcov_path="no"
		elif [[ "$LCOV_MAJOR" -eq "1" ] && \
					[ "$LCOV_MINOR" -lt "6" ]]; then
			AC_MSG_WARN([lcov >= 1.6 is required, you have $LCOV_VERSION])
			lcov_path="no"
		fi
	fi
else
	COVERAGE_CFLAGS=""
	COVERAGE_LIBS=""
fi
AM_CONDITIONAL(USE_LCOV, test ! x$lcov_path = xyes)

SUMMER_CFLAGS+=" $COVERAGE_CFLAGS"
SUMMER_LIBS+=" $COVERAGE_LIBS"

AC_SUBST(SUMMER_CFLAGS)
AC_SUBST(SUMMER_LIBS)

SHAVE_INIT(., enable)

AC_CONFIG_FILES([
	Makefile
	shave
	shave-libtool
	libsummer-1.0.pc
	libsummer-1.0-uninstalled.pc
	bindings/python/Makefile
	docs/reference/summer/Makefile
	docs/reference/summer/version.xml])

AC_OUTPUT

echo "
Libsummer was configured with the following options:

	Bittorrent support:     $enable_bittorrent
	Python bindings:        $enable_python
	Vala bindings:          $enable_vala
	Gtk-Doc Support:        $enable_gtk_doc
	Test Coverage Analysis: $enable_coverage
"
