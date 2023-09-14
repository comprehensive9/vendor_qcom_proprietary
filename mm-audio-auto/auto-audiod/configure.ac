#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for audiod
#

# Process this file with autoconf to produce a configure script.

# Requires autoconf tool later than 2.61
AC_PREREQ([2.61])
# Initialize the audiod package version 1.0.0
AC_INIT([audiod], 1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign subdir-objects])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([AutoAudioDaemon.cpp])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Checks for libraries.

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
        [enable glib, Build against glib. Use this when building for HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,
                          AC_MSG_ERROR(GThread >= 2.16 is required))
        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,
                          AC_MSG_ERROR(GLib >= 2.16 is required))
        GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
fi

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_ARG_ENABLE(target,
              [AS_HELP_STRING([--enable-target=TARGET_BOARD_PLATFORM],
                              [Specify the target product to build])],
              [AS_CASE([$enableval],
                       [qtiquingvm], [TARGET_BOARD_PLATFORM=PLATFORM_MSMNILE],
                       [sa81*],      [TARGET_BOARD_PLATFORM=PLATFORM_MSMNILE],
                       [sa61*],      [TARGET_BOARD_PLATFORM=PLATFORM_MSMSTEPPE],
                       [TARGET_BOARD_PLATFORM=none])],
              [TARGET_BOARD_PLATFORM=none]
)
AC_SUBST([TARGET_BOARD_PLATFORM])

AC_SUBST([CFLAGS])
AC_SUBST([CC])
AC_CONFIG_FILES([ \
        Makefile \
	audiod.pc
        ])
AC_OUTPUT
