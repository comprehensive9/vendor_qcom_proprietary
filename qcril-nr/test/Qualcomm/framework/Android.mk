LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE                          := $(qcril_sanitize)
endif
LOCAL_MODULE                            := QtiRilFwkTest
LOCAL_VENDOR_MODULE                     := true
LOCAL_SHARED_LIBRARIES                  += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES                  += liblog
LOCAL_SHARED_LIBRARIES                  += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES                  += libqcrilNrLogger
LOCAL_STATIC_LIBRARIES                  += libgtest
LOCAL_HEADER_LIBRARIES                  += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES                  += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES                  += libqcrilNrLogger-headers
LOCAL_CFLAGS                            += $(qcril_cflags)
LOCAL_CPPFLAGS                          += $(qcril_cppflags)
LOCAL_LDFLAGS                           += $(qcril_cppflags)
LOCAL_SRC_FILES                         += $(call all-cpp-files-under,src)
include $(BUILD_HOST_EXECUTABLE)
