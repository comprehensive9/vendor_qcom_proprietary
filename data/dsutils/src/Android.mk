LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true

LOCAL_COPY_HEADERS_TO   := data/inc
LOCAL_COPY_HEADERS      += ../inc/ds_cmdq.h
LOCAL_COPY_HEADERS      += ../inc/ds_list.h
LOCAL_COPY_HEADERS      += ../inc/ds_string.h
LOCAL_COPY_HEADERS      += ../inc/ds_util.h
LOCAL_COPY_HEADERS      += ../inc/ds_sl_list.h
LOCAL_COPY_HEADERS      += ../inc/ds_trace.h
LOCAL_COPY_HEADERS      += ../inc/queue.h
LOCAL_COPY_HEADERS      += ../inc/stm2.h
LOCAL_COPY_HEADERS      += ../inc/stm2_os.h

include $(BUILD_COPY_HEADERS)
