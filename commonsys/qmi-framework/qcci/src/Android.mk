LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_COPY_HEADERS_TO   := qmi-framework/inc
LOCAL_COPY_HEADERS      += ../inc/qmi_cci_target.h
LOCAL_COPY_HEADERS      += ../inc/qmi_cci_common.h
LOCAL_COPY_HEADERS      += ../../inc/common_v01.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_cci_target_ext.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_client.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_client_deprecated.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_csi.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_csi_target_ext.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_idl_lib.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_idl_lib_internal.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_idl_lib_target.h
LOCAL_COPY_HEADERS      += ../../inc/qmi_client_instance_defs.h

include $(BUILD_COPY_HEADERS)