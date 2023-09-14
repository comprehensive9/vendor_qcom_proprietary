#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for sensors
#
# Process this file with autoconf to produce a configure script
# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)

# Initialize the sensors package version 1.0.0
AC_INIT([sensors-see-test], 1.0.0)

# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE

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

# Checks for libraries
PKG_CHECK_MODULES([QMIFRAMEWORK], [qmi-framework])
AC_SUBST([QMIFRAMEWORK_CFLAGS])
AC_SUBST([QMIFRAMEWORK_LIBS])

PKG_CHECK_MODULES([LIBHARDWARE], [hardware])
AC_SUBST([LIBHARDWARE_CFLAGS])
AC_SUBST([LIBHARDWARE_LIBS])

PKG_CHECK_MODULES([PROTOBUF], [protobuf])
AC_SUBST([PROTOBUF_CFLAGS])
AC_SUBST([PROTOBUF_LIBS])
AC_SUBST(PROTOBUF_VERSION)

PKG_CHECK_MODULES([SENSORSEE], [sensors-see])
AC_SUBST([SENSORSEE_CFLAGS])
AC_SUBST([SENSORSEE_LIBS])

PKG_CHECK_MODULES([LIBUTILS], [libutils])
AC_SUBST([LIBUTILS_CFLAGS])
AC_SUBST([LIBUTILS_LIBS])

PKG_CHECK_MODULES([LIBCUTILS], [libcutils])
AC_SUBST([LIBCUTILS_CFLAGS])
AC_SUBST([LIBCUTILS_LIBS])

AC_CHECK_PROG([PROTOC], [protoc], [protoc])
AS_IF([test "x${PROTOC}" == "x"],
      [AC_MSG_ERROR([ProtoBuf compiler "protoc" not found.])])
AC_PATH_PROG([PROTOCPATH], [protoc])
AC_SUBST(PROTOC)

AC_ARG_WITH(sanitized-headers,
        AS_HELP_STRING([--with-sanitized-headers=DIR],
                [Specify the location of the sanitized Linux headers]),
        [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

AC_ARG_WITH([glib],
       AS_HELP_STRING([--with-glib],
          [enable glib, building HLOS systems which use glib]))

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

AC_ARG_WITH([common_includes],
      AS_HELP_STRING([--with-common-includes=@<:@dir@:>@],
         [Specify the location of the common headers]),
            [common_incdir=$withval],
               with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${common_incdir}"
fi

AC_ARG_ENABLE(sns-direct-channel,
        AS_HELP_STRING([--enable-sns-direct-channel], [enables sensors direct channel]),
        [sns_direct_channel_support="yes"],
        [sns_direct_channel_support="no"])

if test "x${sns_direct_channel_support}" = "xyes"; then
  CFLAGS="${CFLAGS} -DSNS_SUPPORT_DIRECT_CHANNEL"
  CPPFLAGS="${CPPFLAGS} -DSNS_SUPPORT_DIRECT_CHANNEL"
fi

AM_CONDITIONAL([ENABLE_SNS_DIRECT_CHANNEL], [test "x${sns_direct_channel_support}" = "xyes"])

AC_ARG_ENABLE(sns-target-le-platform,
      AS_HELP_STRING([--enable-sns-target-le-platform], [Enable conditional compilation for target apq8053]),
         [sns_target_le_platform=${enableval}])
AM_CONDITIONAL(SNS_TARGET_LE_PLATFORM, [test "x${sns_target_le_platform}" = "xyes"])

AC_ARG_WITH(systemd,
        AS_HELP_STRING([--with-systemd], [include services in systemd]),
        [SYSTEMD="yes"],
        [SYSTEMD="no"])
AM_CONDITIONAL([USE_SYSTEMD], [test "x${SYSTEMD}" != "xno"])

AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])

AC_CONFIG_FILES([
        Makefile                                   \
        ssc_drva_test/Makefile                     \
        ssc_sensor_info/Makefile                   \
        see_salt/Makefile                          \
        see_workhorse/Makefile                     \
        see_selftest/Makefile                      \
        see_resampler/Makefile                     \
        sns_client_example/Makefile                \
        sns_client_example_cpp/Makefile            \
        sns_client_test_cpp_cmdline_args/Makefile  \
        sensors-see-test.pc
        ])
AC_OUTPUT

if test "x${sns_direct_channel_support}" = "xyes"; then
AC_CONFIG_FILES([                                       \
        sns_direct_channel_client_example/Makefile \
               ])
AC_OUTPUT
fi
