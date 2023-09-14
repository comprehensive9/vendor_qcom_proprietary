# Makefile.am - Automake script for sensors-see

ACLOCAL_AMFLAGS = -I m4

dirs = ssc_drva_test                    \
        ssc_sensor_info                  \
        see_salt                         \
        see_workhorse                    \
        see_selftest                     \
        see_resampler                    \
        sns_client_example               \
        sns_client_example_cpp           \
        sns_client_test_cpp_cmdline_args

if ENABLE_SNS_DIRECT_CHANNEL
  dirs += sns_direct_channel_client_example
endif

SUBDIRS = $(dirs)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sensors-see-test.pc

EXTRA_DIST = $(pkgconfig_DATA)
