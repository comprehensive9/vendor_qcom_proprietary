LOCAL_PATH := $(call my-dir)

DATA_HAL_DIR := ${LOCAL_PATH}/..

LOCAL_NR_PATH := $(DATA_HAL_DIR)/../qcril-nr

MESSAGES_DIR := ${LOCAL_PATH}




################# USED FOR QCRIL-HAL##################################
###################### Target ########################################

#Interfaces

include $(CLEAR_VARS)

LOCAL_MODULE               := libqcrilDataInterfaces
LOCAL_SANITIZE             := integer_overflow
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_CFLAGS               += -Wall -Werror -Wno-macro-redefined -DUSE_QCRIL_HAL
LOCAL_CXXFLAGS             += -std=c++17

LOCAL_SRC_FILES            := $(call all-cpp-files-under, src/Interfaces)

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_SRC_FILES            := $(filter-out %EmbmsActivateDeactivateTmgiTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %EmbmsActivateTmgiTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %EmbmsContentDescUpdateMessageTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %EmbmsDeactivateTmgiTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %EmbmsEnableDataReqTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %EmbmsGetActiveTmgiTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %EmbmsGetAvailTmgiTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %EmbmsSendIntTmgiListTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_CFLAGS               += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS             += -DRIL_FOR_LOW_RAM
endif

LOCAL_C_INCLUDES += $(DATA_HAL_DIR)/datamodule/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc/
#KERNEL_OBJ needed for correct definition of __kernel_sockaddr_storage
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#to evade cnt_error
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += libutils

LOCAL_HEADER_LIBRARIES := qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES += libril-qc-qmi-services-headers
LOCAL_HEADER_LIBRARIES += libqmi_common_headers
LOCAL_HEADER_LIBRARIES += libqmi_headers
LOCAL_HEADER_LIBRARIES += vendor_common_inc

LOCAL_SHARED_LIBRARIES += libril-qc-hal-qmi
LOCAL_HEADER_LIBRARIES += qcrilDataModule_headers

$(info Done building libqcrilDataInterfaces for target...)

include $(BUILD_STATIC_LIBRARY)

#DataInternal

include $(CLEAR_VARS)

LOCAL_MODULE               := libqcrilDataInternal
LOCAL_SANITIZE             := integer_overflow
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_CFLAGS               += -Wall -Werror -Wno-macro-redefined -DUSE_QCRIL_HAL
LOCAL_CXXFLAGS             += -std=c++17

LOCAL_SRC_FILES            := $(call all-cpp-files-under, src/Internal)

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_SRC_FILES            := $(filter-out %DataCallListTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %DeactivateDataCallTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %LastDataCallFailCauseTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %PullLceDataTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %SetDataProfileTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %SetupDataCallTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %StartKeepaliveTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %StopKeepaliveTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %StartLceTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES            := $(filter-out %StopLceTranslator.cpp,$(LOCAL_SRC_FILES))
LOCAL_CFLAGS               += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS             += -DRIL_FOR_LOW_RAM
endif

LOCAL_C_INCLUDES += $(DATA_HAL_DIR)/lqe/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc/
#KERNEL_OBJ needed for correct definition of __kernel_sockaddr_storage
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#to evade cnt_error
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libril-qc-hal-qmi
LOCAL_SHARED_LIBRARIES += libqdp

LOCAL_HEADER_LIBRARIES += libqmi_common_headers
LOCAL_HEADER_LIBRARIES += libqmi_headers
LOCAL_HEADER_LIBRARIES += vendor_common_inc
LOCAL_HEADER_LIBRARIES += libdataqmiservices_headers

LOCAL_HEADER_LIBRARIES += qcrilDataInternal_headers
LOCAL_HEADER_LIBRARIES += qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES += qcrilDataModule_headers

$(info Done building libqcrilDataInternal for target...)

include $(BUILD_STATIC_LIBRARY)

###################### Host ########################################

#Interfaces

include $(CLEAR_VARS)

LOCAL_MODULE               := libqcrilNRDataInterfaces
LOCAL_SANITIZE             := integer_overflow
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

#LOCAL_CFLAGS               += -Wall -Werror -Wno-macro-redefined -DUSE_QCRIL_HAL
LOCAL_CFLAGS               += -Wall -DHOST_EXECUTABLE_BUILD -g -fprofile-arcs -ftest-coverage --coverage -fexceptions


# This is for development only
LOCAL_CFLAGS += -DQCRIL_DATA_MODULE_DEV

LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_NETCTRL
LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS
LOCAL_CFLAGS += -DFEATURE_DATA_LQE
LOCAL_CFLAGS += -DQCRIL_DATA_OFFTARGET
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_STDERR
LOCAL_CFLAGS += -DFEATURE_UNIT_TEST

LOCAL_CXXFLAGS             += -std=c++17

LOCAL_SRC_FILES            := $(call all-cpp-files-under, src/Interfaces)

LOCAL_C_INCLUDES += $(DATA_HAL_DIR)/datamodule/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc/
#KERNEL_OBJ needed for correct definition of __kernel_sockaddr_storage
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#to evade cnt_error
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr


LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += libutils

LOCAL_HEADER_LIBRARIES := qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES += libril-qc-qmi-services-headers
LOCAL_HEADER_LIBRARIES += vendor_common_inc
LOCAL_SHARED_LIBRARIES += libril-qc-hal-qmi
LOCAL_HEADER_LIBRARIES += qcrilDataModule_headers


#$(info Done building libqcrilDataInterfaces for host...)

#include $(BUILD_HOST_STATIC_LIBRARY)

#DataInternal

include $(CLEAR_VARS)

LOCAL_MODULE               := libqcrilNRDataInternal
LOCAL_SANITIZE             := integer_overflow
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

#LOCAL_CFLAGS               += -Wall -Werror -Wno-macro-redefined -DUSE_QCRIL_HAL
LOCAL_CFLAGS               += -Wall -DHOST_EXECUTABLE_BUILD -g -fprofile-arcs -ftest-coverage --coverage -fexceptions

 # This is for development only
 LOCAL_CFLAGS += -DQCRIL_DATA_MODULE_DEV

 LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_NETCTRL
 LOCAL_CFLAGS += -DRIL_REQUEST_SET_INITIAL_ATTACH_APN
 LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS
 LOCAL_CFLAGS += -DFEATURE_DATA_LQE
 LOCAL_CFLAGS += -DQCRIL_DATA_OFFTARGET
 LOCAL_CFLAGS += -DFEATURE_DATA_LOG_STDERR
 LOCAL_CFLAGS += -DFEATURE_UNIT_TEST

LOCAL_CXXFLAGS             += -std=c++17

LOCAL_SRC_FILES            := $(call all-cpp-files-under, src/Internal)

LOCAL_C_INCLUDES += $(DATA_HAL_DIR)/lqe/inc

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc/
#KERNEL_OBJ needed for correct definition of __kernel_sockaddr_storage
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#to evade cnt_error
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libril-qc-hal-qmi
LOCAL_SHARED_LIBRARIES += libqdp

LOCAL_HEADER_LIBRARIES += qcrilDataInternal_headers
LOCAL_HEADER_LIBRARIES += qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES += vendor_common_inc
LOCAL_HEADER_LIBRARIES += qcrilDataModule_headers

#$(info Done building libqcrilDataInternal for host...)

#include $(BUILD_HOST_STATIC_LIBRARY)
