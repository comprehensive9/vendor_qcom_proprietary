#                                               -*- Autoconf -*-

# configure.ac -- Autoconf script for QMI Framework
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the qmi-framework package version 1.0.0
AC_INIT([qmi-framework],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([qcci/src/qmi_cci_common.c])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE([qmux],
      AS_HELP_STRING([--disable-qmux], [Disable qcci over qmux]))

AC_ARG_WITH([qmilog],
       AC_HELP_STRING([--with-qmilog], [Enable qmilog]))

# Checks for libraries.
AC_ARG_WITH([glib],
       AC_HELP_STRING([--with-glib],
          [enable glib, building HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
         AC_DEFINE(ENABLE_USEGLIB, 1, [Define if HLOS systems uses glib])
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

AM_CONDITIONAL(NO_BPF, test "x${with_no_bpf}" = "xyes")
#Enable QMI FW logging
AM_CONDITIONAL(ENABLE_QMI_LOG, test "x${with_qmilog}" = "xyes")

AC_CONFIG_FILES([ \
        Makefile \
        irsc_util/Makefile \
        qrtr/Makefile \
        qsocket/Makefile \
	common/src/Makefile \
	encdec/Makefile \
	qcci/src/Makefile \
	qcsi/src/Makefile \
	test_service/Makefile \
	qsap/src/Makefile \
        qmi-framework.pc
        ])
AC_OUTPUT
