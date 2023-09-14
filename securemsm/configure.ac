AC_PREREQ(2.61)
AC_INIT([securemsm], 1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AM_MAINTAINER_MODE
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AS
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_CXX

#LT_INIT

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([SECUREMSMNOSHIP], [securemsm-noship])
AC_SUBST([SECUREMSMNOSHIP_LIBS])
AC_SUBST([SECUREMSMNOSHIP_CFLAGS])

AC_ARG_WITH(sanitized-headers,
AS_HELP_STRING([--with-sanitized-headers=DIR],
[Specify the location of the sanitized Linux headers]),
[CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([kernel],
AC_HELP_STRING([--with-kernel=@<:@dir@:>@],
[Specify the location of the Linux kernel headers]),
[kerneldir=$withval],
with_kernel=no)

AC_ARG_ENABLE([trustedui],
              AC_HELP_STRING([--enable-trustedui],
                             [Enable trustedui components]),
                             [trustedui="${enableval}"],
                             trustedui=no)

AM_CONDITIONAL([TRUSTED_UI], [test "x$trustedui" = xyes])

AC_ARG_ENABLE(hyp,
    AC_HELP_STRING( [--enable-hyp], [Enable hyp]),
                    [hyp_build="${enableval}"], [hyp_build="no"])

AM_CONDITIONAL([ENABLE_HYP],[test "x${hyp_build}" = "xyes"])

if test "x$with_kernel" != "xno"; then
   CFLAGS="${CFLAGS} -I${kerneldir}/include -I${kerneldir}/arch/arm/include"
fi

AC_ARG_ENABLE(cmnlib-load,
    AC_HELP_STRING([--enable-cmnlib-load], [Enable cmnlib loading]),
            [cmnlib_load=${enableval}], [cmnlib_load="yes"])

AM_CONDITIONAL([LOAD_CMNLIB],[test "x${cmnlib_load}" = "xyes"])

AS_CASE([$host],
    [arm*], [ARM=yes],
    [ARM=no]
)

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

AC_ARG_WITH([lv-build],
    AC_HELP_STRING([--with-lv-build],
                   [enable LV build, building HLOS systems on LV platform]),
    [with_lv="${withval}"],
    [with_lv="no"])

AM_CONDITIONAL(LV_BUILD, test "x${with_lv}" = "xyes")

#Conditional compilation for Ubuntu Core
AC_ARG_ENABLE([ubuntucore],
              AC_HELP_STRING([--enable-ubuntucore],
                             [Enable ubuntucore components]),
                             [ubuntucore="${enableval}"],
                             ubuntucore=no)
AM_CONDITIONAL([UC_BUILD], [test "x$ubuntucore" = xyes])

AC_ARG_ENABLE([legacy-km],
               AC_HELP_STRING([--enable-legacy-km],
                              [Enable legacy km]),
                              [legacy_km="${enableval}"],
                              legacy_km=no)
AM_CONDITIONAL([USE_LEGACY_KM], [test "x$legacy_km" = xyes])

AC_ARG_WITH([le-qseecom],
    AC_HELP_STRING([--with-le-qseecom], [Enable LE QSEECOM build]),
            [with_le_qseecom="${withval}"], [with_le_qseecom="no"])

AM_CONDITIONAL([LE_QSEECOM_ENABLED], [test "x${with_le_qseecom}" = "xyes"])

AC_ARG_WITH([cmnlib-preload],
    AC_HELP_STRING([--with-cmnlib-preload], [Enable cmnlib preload build]),
            [with_cmnlib_preload="${withval}"], [with_cmnlib_preload="no"])

AM_CONDITIONAL([LOAD_CMNLIB], [test "x${with_cmnlib_preload}" = "xyes"])

AC_ARG_WITH([norpmb-listener],
   AC_HELP_STRING([--with-norpmb-listener], [Enable norpmb listener build]),
	   [with_norpmb_listener="${withval}"], [with_norpmb_listener="no"])

AM_CONDITIONAL([NORPMB_SUPPORT], [test "x${with_norpmb_listener}" = "xyes"])

AC_ARG_WITH([nossd-listener],
   AC_HELP_STRING([--with-nossd-listener], [Enable nossd listener build]),
	   [with_nossd_listener="${withval}"], [with_nossd_listener="no"])

AM_CONDITIONAL([NOSSD_SUPPORT], [test "x${with_nossd_listener}" = "xyes"])

AM_CONDITIONAL(ARM, [test "x$ARM" = "xyes"])
AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])
AC_CONFIG_FILES([Makefile \
            sampleclient/Makefile \
            daemon/Makefile \
            qapi/build/Makefile \
            km_lite/tests/Makefile \
            securemsm.pc])
AM_COND_IF([TRUSTED_UI],
           [AC_CONFIG_FILES([ \
           TrustedUI/TrustedVM/TrustedInputVM/Makefile \
           TrustedUI/TrustedVM/Composer/LayoutManager/Makefile \
           TrustedUI/TrustedVM/libTrustedUI/Makefile \
           TrustedUI/TrustedVM/TUIApp/Makefile \
           TrustedUI/TrustedVM/EIDTUIApp/Makefile \
           TrustedUI/TrustedVM/TVMAppLoader/Makefile
        ])])
AM_COND_IF([LV_BUILD],
           [AC_CONFIG_FILES([ \
            GPTEE_Sample_client/Makefile \
            tlsauth/Makefile \
            ])])
AC_OUTPUT
