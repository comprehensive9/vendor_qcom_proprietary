LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE			   := $(qcril_sanitize)
endif
LOCAL_SANITIZE			   += cfi address
LOCAL_SANITIZE_DIAG		   += cfi address
LOCAL_MODULE                            := libQtiMarshallingTests
LOCAL_VENDOR_MODULE                     := true
LOCAL_HEADER_LIBRARIES					+= qcrilInterfaces-headers
LOCAL_SHARED_LIBRARIES					+= qcrilMarshal
LOCAL_SHARED_LIBRARIES					+= liblog
LOCAL_SHARED_LIBRARIES                  += libqcrilNrQtiMutex
LOCAL_HEADER_LIBRARIES					+= libgtest
LOCAL_CFLAGS                            += $(qcril_cflags)
LOCAL_CPPFLAGS                          += $(qcril_cppflags)
LOCAL_LDFLAGS                           += $(qcril_ldflags)
LOCAL_SRC_FILES							+= $(filter Test%.cpp,$(call all-cpp-files-under,src))
include $(BUILD_HOST_SHARED_LIBRARY)

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE			   := $(qcril_sanitize)
endif
LOCAL_SANITIZE			   += cfi address
LOCAL_SANITIZE_DIAG		   += cfi address
LOCAL_MODULE                            := libQtiMarshallingTests
LOCAL_VENDOR_MODULE                     := true
LOCAL_HEADER_LIBRARIES					+= qcrilMarshal
LOCAL_SHARED_LIBRARIES					+= liblog
LOCAL_SHARED_LIBRARIES                  += libqcrilNrQtiMutex
LOCAL_HEADER_LIBRARIES      += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES					+= libgtest
LOCAL_CFLAGS                            += $(qcril_cflags)
LOCAL_CPPFLAGS                          += $(qcril_cppflags)
LOCAL_LDFLAGS                           += $(qcril_ldflags)
LOCAL_SRC_FILES							+= $(filter Test%.cpp,$(call all-cpp-files-under,src))
include $(BUILD_HOST_STATIC_LIBRARY)


include $(CLEAR_VARS)
ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE			   := $(qcril_sanitize)
endif
LOCAL_MODULE                            := QtiMarshallingTest
LOCAL_VENDOR_MODULE                     := true
LOCAL_SHARED_LIBRARIES					+= qcrilMarshal
LOCAL_SHARED_LIBRARIES					+= liblog
LOCAL_SHARED_LIBRARIES                  += libqcrilNrQtiMutex
LOCAL_STATIC_LIBRARIES					+= libgtest
LOCAL_SHARED_LIBRARIES					+= libQtiMarshallingTests
LOCAL_HEADER_LIBRARIES      += qcrilInterfaces-headers
LOCAL_CFLAGS                            += $(qcril_cflags)
LOCAL_CPPFLAGS                          += $(qcril_cppflags)
LOCAL_LDFLAGS                           += $(qcril_cppflags)
LOCAL_SRC_FILES							+= $(filter-out Test%.cpp,$(call all-cpp-files-under,src))
include $(BUILD_HOST_EXECUTABLE)
