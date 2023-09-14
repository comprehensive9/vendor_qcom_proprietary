#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for sensors
#
# Process this file with autoconf to produce a configure script
# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)

# Initialize the sensors package version 1.0.0
AC_INIT([sensors-see], 1.0.0)

# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE

# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([ssc])

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

PKG_CHECK_MODULES([DIAG], [diag])
AC_SUBST([DIAG_CFLAGS])
AC_SUBST([DIAG_LIBS])

PKG_CHECK_MODULES([LIBHARDWARE], [hardware])
AC_SUBST([LIBHARDWARE_CFLAGS])
AC_SUBST([LIBHARDWARE_LIBS])

PKG_CHECK_MODULES([PROTOBUF], [protobuf])
AC_SUBST([PROTOBUF_CFLAGS])
AC_SUBST([PROTOBUF_LIBS])
AC_SUBST(PROTOBUF_VERSION)

PKG_CHECK_MODULES([LIBCUTILS], [libcutils])
AC_SUBST([LIBCUTILS_CFLAGS])
AC_SUBST([LIBCUTILS_LIBS])

AC_SUBST([PROTOBUF_CFLAGS])
AC_SUBST([PROTOBUF_LIBS])
AC_SUBST(PROTOBUF_VERSION)


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


AC_ARG_WITH(sns-target-proto-path,
        AS_HELP_STRING([--with-sns-target-proto-path], [Specify the location of sensors proto on target]),
        [TARGET_PROTO_PATH="$withval"],
        [TARGET_PROTO_PATH="no"])

AM_CONDITIONAL([TARGET_PROTO_PATH_SPECIFIED], [test "x${TARGET_PROTO_PATH}" != "xno"])
AC_SUBST(TARGET_PROTO_PATH)

AC_ARG_ENABLE(sns-target-le-platform,
      AS_HELP_STRING([--enable-sns-target-le-platform], [Enable conditional compilation for target apq8053]),
         [sns_target_le_platform=${enableval}])
AM_CONDITIONAL(SNS_TARGET_LE_PLATFORM, [test "x${sns_target_le_platform}" = "xyes"])

AC_ARG_WITH(systemd,
        AS_HELP_STRING([--with-systemd], [include services in systemd]),
        [SYSTEMD="yes"],
        [SYSTEMD="no"])
AM_CONDITIONAL([USE_SYSTEMD], [test "x${SYSTEMD}" != "xno"])

AC_ARG_WITH(qti-slpi,
        AS_HELP_STRING([--with-qti-slpi], [include sscrpd services]),
        [QRTSLPI="yes"],
        [QRTSLPI="no"])
AM_CONDITIONAL([USE_QTI_SLPI], [test "x${QRTSLPI}" != "xno"])

AC_ARG_ENABLE(target-sdm845,
        AS_HELP_STRING([--enable-target-sdm845], [enable for sdm845 target]),
        [SDM845="yes"],
        [SDM845="no"])
AM_CONDITIONAL([SDM845_TARGET], [test "x${SDM845}" != "xno"])

AC_ARG_ENABLE(target-qcs605,
        AS_HELP_STRING([--enable-target-qcs605], [enable for qcs605 target]),
        [QCS605="yes"],
        [QCS605="no"])
AM_CONDITIONAL([QCS605_TARGET], [test "x${QCS605}" != "xno"])

AC_ARG_ENABLE(target-qcs40x,
        AS_HELP_STRING([--enable-target-qcs40x], [enable for qcs40x target]),
        [QCX40X="yes"],
        [QCX40X="no"])
AM_CONDITIONAL([QCS40X_TARGET], [test "x${QCX40X}" != "xno"])

AC_ARG_ENABLE(target-sdmsteppe,
        AS_HELP_STRING([--enable-target-sdmsteppe], [enable for sdmsteppe target]),
        [SDMSTEPPE="yes"],
        [SDMSTEPPE="no"])
AM_CONDITIONAL([SDMSTEPPE_TARGET], [test "x${SDMSTEPPE}" != "xno"])

AC_ARG_ENABLE(target-kona,
        AS_HELP_STRING([--enable-target-kona], [enable for kona target]),
        [KONA="yes"],
        [KONA="no"])
AM_CONDITIONAL([KONA_TARGET], [test "x${KONA}" != "xno"])

AC_ARG_ENABLE(target-qrb5165,
        AS_HELP_STRING([--enable-target-qrb5165], [enable for qrb5165 target]),
        [QRB5165="yes"],
        [QRB5165="no"])
AM_CONDITIONAL([QRB5165_TARGET], [test "x${QRB5165}" != "xno"])

AC_ARG_ENABLE(target-sxr2130,
        AS_HELP_STRING([--enable-target-sxr2130], [enable for sxr2130 target]),
        [SXR2130="yes"],
        [SXR2130="no"])
AM_CONDITIONAL([SXR2130_TARGET], [test "x${SXR2130}" != "xno"])

AC_ARG_ENABLE(target-qrbx210,
        AS_HELP_STRING([--enable-target-qrbx210], [enable for qrbx210 target]),
        [QRBX210="yes"],
        [QRBX210="no"])
AM_CONDITIONAL([QRBX210_TARGET], [test "x${QRBX210}" != "xno"])

AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])

AC_CONFIG_FILES([                                       \
        Makefile                                        \
        nanopb/Makefile                                 \
        sensors-log/Makefile                            \
        ssc/nanopb_api/Makefile                         \
        ssc/Makefile                                    \
        sensors-diag-log/Makefile                       \
        sensordaemon/Makefile                           \
        sscrpcd/Makefile                                \
        sns_fastRPC_utils/Makefile                      \
        reverserpc/Makefile                             \
        registry/Makefile                               \
        USTA/native/Makefile                            \
        sensors-see.pc
        ])
AC_OUTPUT

if test "x${sns_direct_channel_support}" = "xyes"; then
AC_CONFIG_FILES([                                       \
        direct_channel_stub/Makefile                    \
               ])
AC_OUTPUT
fi
