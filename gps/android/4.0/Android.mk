ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.gnss@4.0-impl
# activate the following line for debug purposes only, comment out for production
#LOCAL_SANITIZE_DIAG += $(GNSS_SANITIZE_DIAG)

LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    LocHidlUtils.cpp \
    LocHidlGnss.cpp \
    LocHidlAGnss.cpp \
    LocHidlDebugReportService.cpp \
    LocHidlFlpService.cpp \
    location_api/LocHidlFlpClient.cpp \
    LocHidlGeofenceService.cpp \
    LocHidlIzatProvider.cpp \
    LocHidlGnssNi.cpp \
    LocHidlIzatOsNpGlue.cpp \
    LocHidlRilInfoMonitor.cpp \
    LocHidlIzatSubscription.cpp \
    LocHidlXT.cpp \
    LocHidlWiFiDBReceiver.cpp \
    LocHidlWWANDBReceiver.cpp \
    LocHidlGnssConfigService.cpp \
    LocHidlWiFiDBProvider.cpp \
    LocHidlWWANDBProvider.cpp \
    LocHidlIzatConfig.cpp \

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/location_api \
    $(LOCAL_PATH)/../ \
    $(TARGET_OUT_HEADERS)/common/inc \
    $(TARGET_OUT_HEADERS)/libizat_core \
    $(TARGET_OUT_HEADERS)/liblbs_core \

LOCAL_HEADER_LIBRARIES := \
    izat_remote_api_headers \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    liblocationservice_glue_headers \
    liblocationservice_headers \
    libdataitems_headers \
    izat_remote_api_prop_headers \
    libxtadapter_headers \
    libloc_mq_client_headers

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libhidlbase \
    libutils \
    android.hardware.gnss@1.0 \
    android.hardware.gnss@1.1 \
    vendor.qti.gnss@1.0 \
    vendor.qti.gnss@1.1 \
    vendor.qti.gnss@1.2 \
    vendor.qti.gnss@2.0 \
    vendor.qti.gnss@2.1 \
    vendor.qti.gnss@3.0 \
    vendor.qti.gnss@4.0 \

LOCAL_SHARED_LIBRARIES += \
    libloc_core \
    libgps.utils \
    libdl \
    liblocation_api \
    liblocationservice_glue \
    liblbs_core \
    libdataitems \
    liblocationservice \
    libxtadapter \
    libizat_core \
    libizat_client_api \

LOCAL_CFLAGS += -DGNSS_HIDL_VERSION='"$(GNSS_HIDL_VERSION)"'

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.gnss@4.0-service
# activate the following line for debug purposes only, comment out for production
#LOCAL_SANITIZE_DIAG += $(GNSS_SANITIZE_DIAG)

LOCAL_VINTF_FRAGMENTS := vendor.qti.gnss@4.0-service.xml
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := \
    service/service.cpp \

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/../ \

LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libgps.utils \
    libqti_vndfwk_detect \

LOCAL_SHARED_LIBRARIES += \
    libhidlbase \
    android.hardware.gnss@1.0 \
    android.hardware.gnss@1.1 \
    android.hardware.gnss@2.0 \
    vendor.qti.gnss@1.0 \
    vendor.qti.gnss@1.1 \
    vendor.qti.gnss@1.2 \
    vendor.qti.gnss@2.0 \
    vendor.qti.gnss@2.1 \
    vendor.qti.gnss@3.0 \
    vendor.qti.gnss@4.0 \

LOCAL_CFLAGS += $(GNSS_CFLAGS)


include $(BUILD_SHARED_LIBRARY)

endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
