#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([SciTECO], [0.1],
	[robin.haberkorn@googlemail.com],
	[sciteco],
	[https://github.com/rhaberkorn/sciteco])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# SciTECO library macro directory
scitecolibdir=$datadir/$PACKAGE/lib
AC_SUBST(scitecolibdir)

# Checks for programs.
LT_INIT
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_INSTALL

AC_CHECK_PROG(DATE, date, date)
if [[ x$DATE = x ]]; then
	AC_MSG_ERROR([Required tool date not found!])
fi

AC_CHECK_PROG(READLINK, readlink, readlink)
if [[ x$READLINK = x ]]; then
	AC_MSG_ERROR([Required tool readlink not found!])
fi

# not necessarily required (depends on --enable-developer-doc)
AC_CHECK_PROG(DOXYGEN, doxygen, doxygen)
AC_CHECK_PROG(DOT, dot, dot)
DOXYGEN_HAVE_DOT=YES
if [[ x$DOT = x ]]; then DOXYGEN_HAVE_DOT=NO; fi
AC_SUBST(DOXYGEN_HAVE_DOT)

# For bootstrapping via installed sciteco
AC_CHECK_PROG(SCITECO, sciteco, sciteco)
if [[ x$SCITECO = x ]]; then
	bootstrap=yes
fi

# Checks for libraries.
PKG_CHECK_MODULES(LIBGLIB, [glib-2.0], [
	CFLAGS="$CFLAGS $LIBGLIB_CFLAGS"
	CXXFLAGS="$CXXFLAGS $LIBGLIB_CFLAGS"
	LIBS="$LIBS $LIBGLIB_LIBS"
])

# Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADERS([bsd/sys/queue.h])
AM_CONDITIONAL(NEED_COMPAT, [test $ac_cv_header_bsd_sys_queue_h = no])

case $host in
*-mingw*)
	AC_CHECK_HEADERS([windows.h], , [
		AC_MSG_ERROR([Missing Windows headers!])
	])
	;;
esac

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset setlocale strchr strrchr])

case $host in
*-*-linux*)
	AC_CHECK_FUNC([realpath])
	;;
esac

#
# Config options
#
AC_ARG_WITH(scintilla,
	AS_HELP_STRING([--with-scintilla=PATH],
		       [Specify Scintilla's path [default=..]]),
	[SCINTILLA_PATH=$withval], [SCINTILLA_PATH=..])
# eval necessary to perform necessary expansions that may not
# have been done by the calling shell
SCINTILLA_PATH=`eval $READLINK -e $SCINTILLA_PATH`
AC_SUBST(SCINTILLA_PATH)

CPPFLAGS="$CPPFLAGS -I$SCINTILLA_PATH/include -DSCI_LEXER"

AC_ARG_WITH(scinterm,
	AS_HELP_STRING([--with-scinterm=PATH],
		       [Specify Scinterm's path [default=SCINTILLA_PATH/scinterm]]),
	[SCINTERM_PATH=$withval], [SCINTERM_PATH=$SCINTILLA_PATH/scinterm])
SCINTERM_PATH=`eval $READLINK -e $SCINTERM_PATH`

AC_ARG_WITH(interface,
	AS_HELP_STRING([--with-interface=ncurses|pdcurses|gtk],
		       [Specify user interface [default=ncurses]]),
	[INTERFACE=$withval], [INTERFACE=ncurses])

case $INTERFACE in
*curses)
	case $INTERFACE in
	ncurses)
		AC_CHECK_LIB(ncurses, initscr, , [
			AC_MSG_ERROR([libncurses missing!])
		])
		;;
	pdcurses)
		AC_CHECK_LIB(pdcurses, initscr, , [
			AC_MSG_ERROR([libpdcurses missing!])
		])
		AC_CHECK_FUNC([PDC_set_resize_limits], [
			AC_DEFINE(PDCURSES_WIN32A, , [PDCurses supports Win32a extensions])
		])
		;;
	esac

	AC_CHECK_HEADERS([curses.h], , [
		AC_MSG_ERROR([Curses header missing!])
	])
	AC_DEFINE(INTERFACE_NCURSES, , [Build with curses support])

	# For Scintilla:
	CPPFLAGS="$CPPFLAGS -DNCURSES -I$SCINTERM_PATH"
	;;

gtk)
	PKG_CHECK_MODULES(LIBGTK, [gtk+-2.0], [
		CFLAGS="$CFLAGS $LIBGTK_CFLAGS"
		CXXFLAGS="$CXXFLAGS $LIBGTK_CFLAGS"
		LIBS="$LIBS $LIBGTK_LIBS"
	])
	AC_CHECK_PROG(GOB2, gob2, gob2)
	if [[ x$GOB2 = x ]]; then
		AC_MSG_ERROR([Gob2 (GObject Builder) not found!])
	fi
	AC_DEFINE(INTERFACE_GTK, , [Build with GTK+ 2.0 support])

	# For Scintilla:
	CPPFLAGS="$CPPFLAGS -DGTK"
	;;

*)
	AC_MSG_ERROR([Invalid user interface specified!])
	;;
esac

AM_CONDITIONAL(INTERFACE_GTK, [test x$INTERFACE = xgtk])

AC_ARG_WITH(default-scitecopath,
	AS_HELP_STRING([--with-default-scitecopath=PATH],
		       [Specify the default value of the $SCITECOPATH variable
		        [default=DATADIR/sciteco/lib]]),
	[DEFAULT_SCITECOPATH=$withval], [DEFAULT_SCITECOPATH=$scitecolibdir])
AC_SUBST(DEFAULT_SCITECOPATH)

AC_ARG_WITH(teco-integer,
	AS_HELP_STRING([--with-teco-integer=SIZE],
		       [Storage size of TECO integers in bits [default=64]]),
	[TECO_INTEGER=$withval], [TECO_INTEGER=64])
AC_DEFINE_UNQUOTED(TECO_INTEGER, $TECO_INTEGER, [Storage size of TECO integers])

AC_ARG_ENABLE(developer-doc,
	AS_HELP_STRING([--enable-developer-doc],
		       [Generate developer documentation using Doxygen [default=no]]),
	[dev_doc=$enableval], [dev_doc=no])
if [[ $dev_doc = yes -a x$DOXYGEN = x ]]; then
        AC_MSG_ERROR([Enabled generating developer documentation,
		      but Doxygen cannot be found!
		      Try --disable-developer-doc.])
fi
AM_CONDITIONAL(BUILD_DEVDOC, [test $dev_doc = yes])

AC_ARG_ENABLE(bootstrap,
	AS_HELP_STRING([--enable-bootstrap],
		       [Bootstrap using sciteco-minimal,
		        otherwise use preinstalled sciteco [default=check]]),
	[bootstrap=$enableval])
AM_CONDITIONAL(BOOTSTRAP, [test x$bootstrap = xyes])

AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile])
AC_CONFIG_FILES([doc/Makefile doc/Doxyfile])
AC_OUTPUT
