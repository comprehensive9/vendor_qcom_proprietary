LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)
#needed for backward compatability
LOCAL_CFLAGS := -Wno-missing-field-initializers

LOCAL_COPY_HEADERS_TO := qmi/inc

LOCAL_COPY_HEADERS += wireless_data_service_common_v01.h
LOCAL_COPY_HEADERS += wireless_data_service_v01.h
LOCAL_COPY_HEADERS += wireless_data_administrative_service_v01.h
LOCAL_COPY_HEADERS += data_system_determination_v01.h
LOCAL_COPY_HEADERS += data_filter_service_v01.h
LOCAL_COPY_HEADERS += data_port_mapper_v01.h
LOCAL_COPY_HEADERS += data_common_v01.h
LOCAL_COPY_HEADERS += quality_of_service_v01.h
LOCAL_COPY_HEADERS += over_the_top_v01.h
LOCAL_COPY_HEADERS += application_traffic_pairing_v01.h
LOCAL_COPY_HEADERS += authentication_service_v01.h

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_COPY_HEADERS)


include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wno-missing-field-initializers

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES += wireless_data_service_common_v01.c
LOCAL_SRC_FILES += wireless_data_service_v01.c
LOCAL_SRC_FILES += wireless_data_administrative_service_v01.c
LOCAL_SRC_FILES += data_system_determination_v01.c
LOCAL_SRC_FILES += data_port_mapper_v01.c
LOCAL_SRC_FILES += data_common_v01.c
LOCAL_SRC_FILES += data_filter_service_v01.c
LOCAL_SRC_FILES += quality_of_service_v01.c
LOCAL_SRC_FILES += over_the_top_v01.c
LOCAL_SRC_FILES += application_traffic_pairing_v01.c
LOCAL_SRC_FILES += authentication_service_v01.c

LOCAL_HEADER_LIBRARIES := libqmi_common_headers
LOCAL_HEADER_LIBRARIES += vendor_common_inc
LOCAL_HEADER_LIBRARIES += libdataqmiservices_headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libdataqmiservices_headers

LOCAL_MODULE:= libdataqmiservices
LOCAL_SANITIZE=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_HOST_STATIC_LIBRARY)

