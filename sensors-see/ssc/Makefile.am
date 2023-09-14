# Makefile.am - Automake script for proto files to generate equivalent *.pb.cc, *.pb.h, snsproto.la & ssc.la

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^

nanopb.pb.cc nanopb.pb.h: nanopb.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/nanopb.pb.cc

sns_basic_gestures.pb.cc sns_basic_gestures.pb.h: sns_basic_gestures.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_basic_gestures.pb.cc

sns_client.pb.cc sns_client.pb.h: sns_client.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_client.pb.cc

sns_da_test.pb.cc sns_da_test.pb.h: sns_da_test.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_da_test.pb.cc

sns_device_mode.pb.cc sns_device_mode.pb.h: sns_device_mode.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_device_mode.pb.cc

sns_diag.pb.cc sns_diag.pb.h: sns_diag.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_diag.pb.cc

sns_ext_svc.pb.cc sns_ext_svc.pb.h: sns_ext_svc.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_ext_svc.pb.cc

sns_hall.pb.cc sns_hall.pb.h: sns_hall.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_hall.pb.cc

sns_proximity.pb.cc sns_proximity.pb.h: sns_proximity.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_proximity.pb.cc

sns_sar.pb.cc sns_sar.pb.h: sns_sar.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_sar.pb.cc

sns_std.pb.cc sns_std.pb.h: sns_std.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_std.pb.cc

sns_std_type.pb.cc sns_std_type.pb.h: sns_std_type.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_std_type.pb.cc

sns_direct_channel.pb.cc sns_direct_channel.pb.h: sns_direct_channel.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_direct_channel.pb.cc

sns_cal.pb.cc sns_cal.pb.h: sns_cal.proto
	$(PROTOC) --proto_path=$(srcdir) --cpp_out=$(srcdir) $^
	sed -i '1i\#pragma GCC diagnostic ignored "-Warray-bounds"' $(srcdir)/sns_cal.pb.cc

AM_CPPFLAGS = -Werror                   \
              -Wall                     \
              -Wno-unused-parameter     \
              -Wno-unused-variable      \
              -fexceptions              \
              -DSNS_TARGET_LE_PLATFORM  \
              -I./                      \
              -I./client_api/           \
              -I../sensors-log/

nodist_proto_SOURCES = $(srcdir)/nanopb.pb.cc                       \
                       $(srcdir)/sns_accel_cal.pb.cc                \
                       $(srcdir)/sns_accel.pb.cc                    \
                       $(srcdir)/sns_ambient_light.pb.cc            \
                       $(srcdir)/sns_ambient_temperature.pb.cc      \
                       $(srcdir)/sns_amd.pb.cc                      \
                       $(srcdir)/sns_aont.pb.cc                     \
                       $(srcdir)/sns_basic_gestures.pb.cc           \
                       $(srcdir)/sns_bring_to_ear.pb.cc             \
                       $(srcdir)/sns_cal.pb.cc                      \
                       $(srcdir)/sns_client.pb.cc                   \
                       $(srcdir)/sns_cmc.pb.cc                      \
                       $(srcdir)/sns_da_test.pb.cc                  \
                       $(srcdir)/sns_device_mode.pb.cc              \
                       $(srcdir)/sns_device_orient.pb.cc            \
                       $(srcdir)/sns_diag.pb.cc                     \
                       $(srcdir)/sns_diag_sensor.pb.cc              \
                       $(srcdir)/sns_direct_channel.pb.cc           \
                       $(srcdir)/sns_dpc.pb.cc                      \
                       $(srcdir)/sns_ext_svc.pb.cc                  \
                       $(srcdir)/sns_facing.pb.cc                   \
                       $(srcdir)/sns_fmv.pb.cc                      \
                       $(srcdir)/sns_fw.pb.cc                       \
                       $(srcdir)/sns_game_rv.pb.cc                  \
                       $(srcdir)/sns_geomag_rv.pb.cc                \
                       $(srcdir)/sns_gravity.pb.cc                  \
                       $(srcdir)/sns_gyro_cal.pb.cc                 \
                       $(srcdir)/sns_gyro.pb.cc                     \
                       $(srcdir)/sns_gyro_rot_matrix.pb.cc          \
                       $(srcdir)/sns_hall.pb.cc                     \
                       $(srcdir)/sns_heart_beat.pb.cc               \
                       $(srcdir)/sns_heart_rate.pb.cc               \
                       $(srcdir)/sns_humidity.pb.cc                 \
                       $(srcdir)/sns_mag_cal.pb.cc                  \
                       $(srcdir)/sns_mag.pb.cc                      \
                       $(srcdir)/sns_oem1.pb.cc                     \
                       $(srcdir)/sns_offbody_detect.pb.cc           \
                       $(srcdir)/sns_pedometer.pb.cc                \
                       $(srcdir)/sns_pedometer_wrist.pb.cc          \
                       $(srcdir)/sns_physical_sensor_test.pb.cc     \
                       $(srcdir)/sns_pose_6dof.pb.cc                \
                       $(srcdir)/sns_ppg.pb.cc                      \
                       $(srcdir)/sns_pressure.pb.cc                 \
                       $(srcdir)/sns_proximity.pb.cc                \
                       $(srcdir)/sns_psmd.pb.cc                     \
                       $(srcdir)/sns_registry.pb.cc                 \
                       $(srcdir)/sns_resampler.pb.cc                \
                       $(srcdir)/sns_rgb.pb.cc                      \
                       $(srcdir)/sns_rmd.pb.cc                      \
                       $(srcdir)/sns_rotv.pb.cc                     \
                       $(srcdir)/sns_sar.pb.cc                      \
                       $(srcdir)/sns_sensor_temperature.pb.cc       \
                       $(srcdir)/sns_sig_motion.pb.cc               \
                       $(srcdir)/sns_std_event_gated_sensor.pb.cc   \
                       $(srcdir)/sns_std.pb.cc                      \
                       $(srcdir)/sns_std_sensor.pb.cc               \
                       $(srcdir)/sns_std_type.pb.cc                 \
                       $(srcdir)/sns_suid.pb.cc                     \
                       $(srcdir)/sns_thermopile.pb.cc               \
                       $(srcdir)/sns_threshold.pb.cc                \
                       $(srcdir)/sns_tilt.pb.cc                     \
                       $(srcdir)/sns_tilt_to_wake.pb.cc             \
                       $(srcdir)/sns_ultra_violet.pb.cc             \
                       $(srcdir)/sns_wrist_tilt_gesture.pb.cc

BUILT_SOURCES = $(srcdir)/nanopb.pb.h                       \
                $(srcdir)/sns_accel_cal.pb.h                \
                $(srcdir)/sns_accel.pb.h                    \
                $(srcdir)/sns_ambient_light.pb.h            \
                $(srcdir)/sns_ambient_temperature.pb.h      \
                $(srcdir)/sns_amd.pb.h                      \
                $(srcdir)/sns_aont.pb.h                     \
                $(srcdir)/sns_basic_gestures.pb.h           \
                $(srcdir)/sns_bring_to_ear.pb.h             \
                $(srcdir)/sns_cal.pb.h                      \
                $(srcdir)/sns_client.pb.h                   \
                $(srcdir)/sns_cmc.pb.h                      \
                $(srcdir)/sns_da_test.pb.h                  \
                $(srcdir)/sns_device_mode.pb.h              \
                $(srcdir)/sns_device_orient.pb.h            \
                $(srcdir)/sns_diag.pb.h                     \
                $(srcdir)/sns_diag_sensor.pb.h              \
                $(srcdir)/sns_direct_channel.pb.h           \
                $(srcdir)/sns_dpc.pb.h                      \
                $(srcdir)/sns_ext_svc.pb.h                  \
                $(srcdir)/sns_facing.pb.h                   \
                $(srcdir)/sns_fmv.pb.h                      \
                $(srcdir)/sns_fw.pb.h                       \
                $(srcdir)/sns_game_rv.pb.h                  \
                $(srcdir)/sns_geomag_rv.pb.h                \
                $(srcdir)/sns_gravity.pb.h                  \
                $(srcdir)/sns_gyro_cal.pb.h                 \
                $(srcdir)/sns_gyro.pb.h                     \
                $(srcdir)/sns_gyro_rot_matrix.pb.h          \
                $(srcdir)/sns_hall.pb.h                     \
                $(srcdir)/sns_heart_beat.pb.h               \
                $(srcdir)/sns_heart_rate.pb.h               \
                $(srcdir)/sns_humidity.pb.h                 \
                $(srcdir)/sns_mag_cal.pb.h                  \
                $(srcdir)/sns_mag.pb.h                      \
                $(srcdir)/sns_oem1.pb.h                     \
                $(srcdir)/sns_offbody_detect.pb.h           \
                $(srcdir)/sns_pedometer.pb.h                \
                $(srcdir)/sns_pedometer_wrist.pb.h          \
                $(srcdir)/sns_physical_sensor_test.pb.h     \
                $(srcdir)/sns_pose_6dof.pb.h                \
                $(srcdir)/sns_ppg.pb.h                      \
                $(srcdir)/sns_pressure.pb.h                 \
                $(srcdir)/sns_proximity.pb.h                \
                $(srcdir)/sns_psmd.pb.h                     \
                $(srcdir)/sns_registry.pb.h                 \
                $(srcdir)/sns_resampler.pb.h                \
                $(srcdir)/sns_rgb.pb.h                      \
                $(srcdir)/sns_rmd.pb.h                      \
                $(srcdir)/sns_rotv.pb.h                     \
                $(srcdir)/sns_sar.pb.h                      \
                $(srcdir)/sns_sensor_temperature.pb.h       \
                $(srcdir)/sns_sig_motion.pb.h               \
                $(srcdir)/sns_std_event_gated_sensor.pb.h   \
                $(srcdir)/sns_std.pb.h                      \
                $(srcdir)/sns_std_sensor.pb.h               \
                $(srcdir)/sns_std_type.pb.h                 \
                $(srcdir)/sns_suid.pb.h                     \
                $(srcdir)/sns_thermopile.pb.h               \
                $(srcdir)/sns_threshold.pb.h                \
                $(srcdir)/sns_tilt.pb.h                     \
                $(srcdir)/sns_tilt_to_wake.pb.h             \
                $(srcdir)/sns_ultra_violet.pb.h             \
                $(srcdir)/sns_wrist_tilt_gesture.pb.h

sources_snsapi = $(nodist_proto_SOURCES) \
                 ./client_api/sns_client_api_v01.c

include_HEADERS = $(BUILT_SOURCES)       \
                  ./client_api/sns_client_api_v01.h

library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libsnsapi.la libssc.la

requiredlibs_snsapi = @DIAG_LIBS@          \
                      @QMIFRAMEWORK_LIBS@  \
                      @GLIB_LIBS@          \
                      @PROTOBUF_LIBS@

libsnsapi_la_CC = @CC@
libsnsapi_la_SOURCES = $(sources_snsapi)
libsnsapi_la_CFLAGS = $(AM_CFLAGS) @DIAG_CFLAGS@ @QMIFRAMEWORK_CFLAGS@ @GLIB_CFLAGS@ @PROTOBUF_CFLAGS@
libsnsapi_la_LDFLAGS = -shared -version-info 1:0:0 @LDFLAGS@
libsnsapi_la_LIBADD = $(requiredlibs_snsapi)

cpp_sources_ssc = ./wakelock_utils.cpp  \
                  ./ssc_connection.cpp  \
                  ./ssc_utils.cpp

requiredlibs_ssc = ./libsnsapi.la                   \
                   ../sensors-log/libsensorslog.la  \
                   -lrt                             \
                   @DIAG_LIBS@                     \
                   @QMIFRAMEWORK_LIBS@             \
                   @PROTOBUF_LIBS@             \
                   @LIBCUTILS_LIBS@

include_HEADERS += ./ssc_connection.h \
                   ./ssc_utils.h      \
                   ./wakelock_utils.h \
                   ./worker.h

libssc_la_CC = @CC@
libssc_la_SOURCES = $(cpp_sources_ssc)
libssc_la_CPPFLAGS = $(AM_CPPFLAGS) @DIAG_CFLAGS@ @QMIFRAMEWORK_CFLAGS@ @PROTOBUF_CFLAGS@ @LIBCUTILS_CFLAGS@
libssc_la_LDFLAGS = -shared -version-info 1:0:0 @LDFLAGS@
if USE_GLIB
 libssc_la_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 libssc_la_LDFLAGS += @GLIB_LIBS@
endif
libssc_la_LIBADD = $(requiredlibs_ssc)

sns_proto_list = $(shell find . -iname '*.proto')
sns_protodir = $(root_system)/etc/sensors/proto
sns_proto_DATA = $(sns_proto_list)