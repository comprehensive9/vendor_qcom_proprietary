LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS               += -DRIL_SHLIB $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_ldflags)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src) $(call all-c-files-under, src)

LOCAL_MODULE               := qcrilNrAndroidModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers \
                              libcutils_headers \
                              libqcrilNrQtiMutex-headers \
                              libril-db-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_SHARED_LIBRARIES     := libqcrilNrQtiMutex

include $(BUILD_STATIC_LIBRARY)
