# LOCAL_PATH and the include path need to be hard-coded because wmsts is inside
# the qcril directory (bug in the Android makefile system).
# LOCAL_PATH reflects parent directory to ensure common objects are in
# separate pools for each RIL variant.


LOCAL_PATH := $(call my-dir)
PWD:=$(shell pwd)

include $(CLEAR_VARS)

LOCAL_MODULE:= librilqmimiscservices
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES += services/qmi_embms_v01.c
LOCAL_SRC_FILES += services/qtuner_v01.c
LOCAL_SRC_FILES += services/qmi_ims_vt_v01.c
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_SHARED_LIBRARIES += libqmi_common_so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcrilNr-headers
LOCAL_VENDOR_MODULE := true
LOCAL_EXPORT_C_INCLUDE_DIRS :=
ifeq ($(TARGET_PD_SERVICE_ENABLED), true)
    LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/libpdnotifier/inc
endif
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/nanopb_utils/
# IMS REFACTOR: TODO: export only required files
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/oem_socket/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/radio_config/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/services/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/control
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/list
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/log
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/memory
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/bit_field
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/synchronization
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/utilities/timer
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/../qcrild/include/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/../nanopb/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/../include/

LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/auth/inc/

LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/common/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/../common/data/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/data/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/libmdmdetect/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/libperipheralclient/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/diag/include/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/qmi/inc/
ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/system_health_monitor/inc/
endif
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/time-services/
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/libpdmapper/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TARGET_OUT_HEADERS)/libpdnotifier/inc


LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libqcrilnr-ims-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilNrSmsHeaders
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libqcrilNrFramework-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libqcrilNrLogger-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libril-qc-qmi-services-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilNrAndroidImsRadioHeaders
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libimsqmiservices_headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libqcrilNrQtiMutex-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libqcrilNrQtiBus-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qtiwakelock-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qtiril-utils-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qtiPeripheralMgr-headers

LOCAL_HEADER_LIBRARIES := libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES := libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES += qtiPeripheralMgr-headers
LOCAL_HEADER_LIBRARIES += libril-db-headers

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libqcrilNrQtiBus-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilInterfaces-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilIntermodulemsgs-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qtibus-headers

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilDataInternal_headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += qcrilDataInterfaces_headers

include $(BUILD_HEADER_LIBRARY)

include $(CLEAR_VARS)

##
## START build_qcril
##

LOCAL_CPP_EXTENSION := .cc

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif
LOCAL_CFLAGS += $(qcril_cflags)
LOCAL_CPPFLAGS += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

LOCAL_SRC_FILES += $(call all-c-files-under, .)

LOCAL_CFLAGS += -DPB_ENABLE_MALLOC # Needed by BT Sap
LOCAL_SRC_FILES += $(call all-c-files-under,../nanopb)
#LOCAL_SRC_FILES += $(call all-cpp-files-under, .)
LOCAL_SRC_FILES += $(call all-named-files-under,*.cc,.)

LOCAL_HEADER_LIBRARIES += libqcrilNr-headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libqcrilNr-headers
LOCAL_HEADER_LIBRARIES += libqcrilnr-ims-headers
LOCAL_HEADER_LIBRARIES += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES += libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES += libril-db-headers

# for asprinf
LOCAL_CFLAGS += -D_GNU_SOURCE

# for embms
LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS

# for LCE(LQE)
LOCAL_CFLAGS += -DFEATURE_DATA_LQE

LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidTranslators
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrVoiceModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrLpaModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrNasModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrNwRegistrationModule
LOCAL_WHOLE_STATIC_LIBRARIES += qtiPeripheralMgr
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrDmsModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrQmiModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrCellInfoModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrUimModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrGstkModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrUimServiceModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrImsModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrPbmModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrSapModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrUimRemoteClientModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrUimRemoteServerModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrSecureElementModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrQtiRadio
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrRadioConfigModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidImsRadio
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrAndroidAudioModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrild_libqcrilnr
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrOemHookModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrSmsModule
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilInterfaces
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilIntermodulemsgs
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrDataModule
ifneq ($(TARGET_HAS_LOW_RAM), true)
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrMbnModule
endif
ifeq ($(FEATURE_QCRIL_LTE_DIRECT_ENABLED),true)
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilLteDirectModule
endif
ifeq ($(TARGET_SUPPORTS_DS),true)
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrDeepSleepModule
LOCAL_SHARED_LIBRARIES       += vendor.qti.hardware.powerstateservice@1.0
endif

LOCAL_SHARED_LIBRARIES	   += libhidlbase
LOCAL_SHARED_LIBRARIES	   += vendor.qti.hardware.radio.ims@1.0
LOCAL_SHARED_LIBRARIES	   += vendor.qti.hardware.radio.ims@1.1
LOCAL_SHARED_LIBRARIES	   += vendor.qti.hardware.radio.ims@1.2
LOCAL_SHARED_LIBRARIES	   += vendor.qti.hardware.radio.ims@1.3
LOCAL_SHARED_LIBRARIES	   += vendor.qti.hardware.radio.ims@1.4
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.ims@1.5
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.ims@1.6
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.ims@1.7
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.am@1.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qcrilhook@1.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim@1.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim@1.1
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim@1.2
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim_remote_server@1.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim_remote_client@1.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim_remote_client@1.1
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.uim_remote_client@1.2
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.lpa@1.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.lpa@1.1
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.lpa@1.2
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@1.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.0
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.1
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.2
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.3
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.4
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.5
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.6
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qtiradio@2.7
LOCAL_SHARED_LIBRARIES     += android.hardware.radio@1.0
LOCAL_SHARED_LIBRARIES     += android.hardware.radio.config@1.0
LOCAL_SHARED_LIBRARIES     += android.hardware.radio.config@1.1
LOCAL_SHARED_LIBRARIES     += android.hardware.radio.config@1.2
LOCAL_SHARED_LIBRARIES += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES     += android.hardware.secure_element@1.0
LOCAL_SHARED_LIBRARIES     += android.hardware.secure_element@1.1
LOCAL_SHARED_LIBRARIES     += android.hardware.secure_element@1.2
ifneq ($(TARGET_HAS_LOW_RAM), true)
#LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.internal.deviceinfo@1.0
endif

LOCAL_SHARED_LIBRARIES += libdsutils  # must preceed libcutils in ICS build
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += qcrild_libqcrilnrutils
ifeq ($(FEATURE_QCRIL_RADIO_CONFIG_SOCKET_ENABLED),true)
LOCAL_SHARED_LIBRARIES += libril-qc-radioconfig
LOCAL_CFLAGS += -DFEATURE_QCRIL_RADIO_CONFIG_SOCKET
endif
ifeq ($(FEATURE_QCRIL_LTE_DIRECT_ENABLED),true)
LOCAL_SHARED_LIBRARIES += libril-qc-ltedirectdisc
endif

LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libhardware_legacy
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += librilqmimiscservices
LOCAL_SHARED_LIBRARIES += libqmi_client_helper
LOCAL_SHARED_LIBRARIES += libidl
LOCAL_SHARED_LIBRARIES += libtime_genoff
LOCAL_SHARED_LIBRARIES += libsqlite
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libmdmdetect
LOCAL_SHARED_LIBRARIES += libperipheral_client
LOCAL_SHARED_LIBRARIES += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libhardware_legacy
LOCAL_SHARED_LIBRARIES += android.hardware.radio@1.0
LOCAL_SHARED_LIBRARIES += android.hardware.radio@1.1
LOCAL_SHARED_LIBRARIES += android.hardware.radio@1.2
LOCAL_SHARED_LIBRARIES += android.hardware.radio@1.3
LOCAL_SHARED_LIBRARIES += android.hardware.radio@1.4
LOCAL_SHARED_LIBRARIES += android.hardware.radio@1.5

LOCAL_SHARED_LIBRARIES += android.hardware.radio.deprecated@1.0
LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += qcrild_libqcrilnrutils
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES += libprotobuf-cpp-full
LOCAL_SHARED_LIBRARIES += libvndksupport
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += libz
LOCAL_SHARED_LIBRARIES += libqdi
LOCAL_SHARED_LIBRARIES += libnetmgr
LOCAL_SHARED_LIBRARIES += libconfigdb
LOCAL_SHARED_LIBRARIES += libxml
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiBus
LOCAL_SHARED_LIBRARIES += qtiwakelock
LOCAL_SHARED_LIBRARIES += qtiril-utils
LOCAL_SHARED_LIBRARIES += qcrilMarshal
LOCAL_SHARED_LIBRARIES += libril-db

ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_SHARED_LIBRARIES += libsystem_health_mon
endif

ifeq ($(TARGET_PD_SERVICE_ENABLED), true)
    LOCAL_SHARED_LIBRARIES += libpdmapper
    LOCAL_SHARED_LIBRARIES += libpdnotifier
    LOCAL_CFLAGS += -DQCRIL_USE_PD_NOTIFIER
endif

# Assume by default that libxml2 is available
# This prevents touching existing device
# config files that already support #
# libxml2. Device files not supporting it,
# would have to explicitly define it, but it should be clear when it is
# required (compilation will fail)
ifneq (${LIBXML_SUPPORTED},false)
LOCAL_SHARED_LIBRARIES += libxml2
else
LOCAL_CFLAGS += -DLIBXML_UNSUPPORTED
endif

$(info is-board-platform-in-list=$is-board-platform-in-list)
# These targets have route look up available on modem
  LOCAL_SHARED_LIBRARIES += libdsi_netctrl
  LOCAL_SHARED_LIBRARIES += libqdp

ifneq ($(TARGET_HAS_LOW_RAM),true)
  LOCAL_SHARED_LIBRARIES     += liblqe
  LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.data.iwlan@1.0
  LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.data.connection@1.0
  LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.data.connection@1.1
endif

# For API Definitions and enables
LOCAL_CFLAGS   += $(remote_api_defines)
LOCAL_CFLAGS   += $(remote_api_enables)

# defines necessary for QCRIL code
LOCAL_CFLAGS += -DRIL_SHLIB
LOCAL_CFLAGS += -DFEATURE_MMGSDI_GSM
LOCAL_CFLAGS += -DFEATURE_AUTH
LOCAL_CFLAGS += -DPACKED=

ifdef FEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
LOCAL_CFLAGS += -DFEATURE_QCRIL_TOOLKIT_SKIP_SETUP_EVT_LIST_FILTER
endif

ifneq ($(SYS_HEALTH_MON_STATUS), false)
LOCAL_CFLAGS += -DFEATURE_QCRIL_SHM
endif
ifneq ($(TARGET_HAS_LOW_RAM), true)
LOCAL_CFLAGS += -DFEATURE_QCRIL_OEMHOOK_IMS_PRESENCE
LOCAL_CFLAGS += -DFEATURE_QCRIL_OEMHOOK_IMS_VT
LOCAL_CFLAGS += -DFEATURE_QCRIL_MBN
else
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif

LOCAL_CFLAGS += -DFEATURE_QCRIL_HDR_RELB
LOCAL_CFLAGS += -DFEATURE_QCRIL_NCELL

# defines common to all shared libraries
LOCAL_CFLAGS += \
  -DLOG_NDDEBUG=0 \
  -DIMAGE_APPS_PROC \
  -DFEATURE_Q_SINGLE_LINK \
  -DFEATURE_Q_NO_SELF_QPTR \
  -DFEATURE_NATIVELINUX \
  -DFEATURE_DSM_DUP_ITEMS \

# compiler options
LOCAL_CFLAGS += -g
LOCAL_CFLAGS += -O0
LOCAL_CFLAGS += -fno-inline
LOCAL_CFLAGS += -fno-short-enums

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

LOCAL_REQUIRED_MODULES     += qcrilNr.db

LOCAL_HEADER_LIBRARIES += qcrilDataInternal_headers
LOCAL_HEADER_LIBRARIES += qcrilDataInterfaces_headers
LOCAL_WHOLE_STATIC_LIBRARIES += libqcrilNRDataInternal
LOCAL_WHOLE_STATIC_LIBRARIES += libqcrilNRDataInterfaces

LOCAL_CLANG := true
ifndef QCRIL_DSDA_INSTANCE
   LOCAL_MODULE:= libqcrilNr
else
   LOCAL_CFLAGS += -DFEATURE_DSDA_RIL_INSTANCE=$(QCRIL_DSDA_INSTANCE)
   LOCAL_MODULE:= libqcrilnr-$(QCRIL_DSDA_INSTANCE)
endif
#LOCAL_EXPORT_CFLAGS += $(LOCAL_CFLAGS)
include $(BUILD_SHARED_LIBRARY)
##
## END build_qcril
##
