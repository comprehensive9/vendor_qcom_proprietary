LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

LOCAL_MODULE						:= qtiril-utils
LOCAL_VENDOR_MODULE					:= true
LOCAL_MODULE_OWNER					:= qti
LOCAL_PROPRIETARY_MODULE			:= true
LOCAL_SRC_FILES						:= $(call all-cpp-files-under, src)
LOCAL_HEADER_LIBRARIES				:= qtiril-utils-headers
LOCAL_CFLAGS						+= $(qcril_cflags)
LOCAL_CPPFLAGS						+= $(qcril_cppflags)
LOCAL_LDFLAGS						+= $(qcril_ldflags)
LOCAL_HEADER_LIBRARIES				+= libqcrilNrQtiMutex-headers
LOCAL_SHARED_LIBRARIES				+= libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES				+= libqcrilNrLogger
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
#LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
#LOCAL_SANITIZE := $(qcril_sanitize)
endif

LOCAL_MODULE						:= qtiril-utils
LOCAL_VENDOR_MODULE					:= true
LOCAL_MODULE_OWNER					:= qti
LOCAL_PROPRIETARY_MODULE			:= true
LOCAL_SRC_FILES						:= $(call all-cpp-files-under, src)
LOCAL_C_INCLUDES                    += $(LOCAL_PATH)/src
LOCAL_HEADER_LIBRARIES				:= qtiril-utils-headers
LOCAL_CFLAGS						+= $(qcril_cflags) -DHOST_EXECUTABLE_BUILD
LOCAL_CPPFLAGS						+= $(qcril_cppflags)
LOCAL_LDFLAGS						+= $(qcril_ldflags)
LOCAL_HEADER_LIBRARIES				+= libqcrilNrQtiMutex-headers
LOCAL_SHARED_LIBRARIES				+= libqcrilNrLogger
LOCAL_SHARED_LIBRARIES				+= libqcrilNrQtiMutex
include $(BUILD_HOST_STATIC_LIBRARY)

