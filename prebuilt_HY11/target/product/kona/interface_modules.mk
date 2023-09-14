PREBUILT_PATH := $(call my-dir)
LOCAL_PATH := $(PREBUILT_PATH)

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := fastrpc/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/fastrpc/inc/AEEStdDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := fastrpc/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/fastrpc/inc/AEEStdErr.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := fastrpc/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/fastrpc/inc/remote.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qualcomm.qti.dpm.api@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := com.qualcomm.qti.dpm.api@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/com.qualcomm.qti.dpm.api@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccvndhal@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libcutils libfmq libhidlbase liblog libutils vendor.qti.hardware.qccvndhal@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/hw/vendor.qti.hardware.qccvndhal@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdsprpc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libhardware libutils vendor.qti.hardware.dsp@1.0 liblog libc libcutils libion libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcdsprpc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmdsprpc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libhardware libutils vendor.qti.hardware.dsp@1.0 liblog libc libcutils libion libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmdsprpc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsubsystem_control
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libqmi_cci libqmi_common_so libmdmdetect libperipheral_client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsubsystem_control.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfddisplayconfig_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libcutils liblog vendor.display.config@2.0 libdisplayconfig.qti libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libwfddisplayconfig_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.data.factory@2.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.data.factory@2.0.vendor vendor.qti.data.slm@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.data.factory@2.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.2.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.2.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.mwqem@1.0.vendor vendor.qti.data.slm@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.data.factory@2.2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.3.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.3.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.factory@2.2.vendor vendor.qti.data.mwqem@1.0.vendor vendor.qti.data.slm@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.lce@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.data.factory@2.3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.cacert@1.0
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.cacert@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.api@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cne.internal.api@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.data.cne.internal.constants@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.data.cne.internal.api@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.constants@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cne.internal.constants@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.data.cne.internal.constants@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.server@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cne.internal.server@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.data.cne.internal.constants@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.data.cne.internal.server@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.qmi@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.qmi@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.data.qmi@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.embmssl@1.0-adapter-helper.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.embmssl@1.0-adapter-helper.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libcutils libhidlbase liblog libutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.embmssl@1.0-adapter-helper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.embmssl@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.embmssl@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.embmssl@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.embmssl@1.1-adapter-helper.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.embmssl@1.1-adapter-helper.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libcutils libhidlbase liblog libutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.embmssl@1.1-adapter-helper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.embmssl@1.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.embmssl@1.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.embmssl@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.embmssl@1.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.mwqemadapter@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.mwqemadapter@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.mwqemadapter@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccsyshal@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.qccsyshal@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.qccsyshal@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccvndhal@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.qccvndhal@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.qccvndhal@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-interface),)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.sigma_miracast@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.sigma_miracast@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.sigma_miracast@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.slmadapter@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.slmadapter@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.slmadapter@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-interface),)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.wifidisplaysession@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.wifidisplaysession@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.wifidisplaysession@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.factory@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.ims.callcapability@1.0.vendor vendor.qti.ims.rcsconfig@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.ims.factory@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory@1.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.factory@1.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.ims.callcapability@1.0.vendor vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.rcsconfig@2.0.vendor vendor.qti.ims.rcsconfig@2.1.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.ims.factory@1.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.imsrtpservice@3.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.imsrtpservice@3.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.imsrtpservice@3.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qualcomm.qti.dpm.api@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := com.qualcomm.qti.dpm.api@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/com.qualcomm.qti.dpm.api@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccvndhal@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libcutils libfmq libhidlbase liblog libutils vendor.qti.hardware.qccvndhal@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/hw/vendor.qti.hardware.qccvndhal@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdsprpc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libhardware libutils vendor.qti.hardware.dsp@1.0 liblog libc libcutils libion libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcdsprpc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmdsprpc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libhardware libutils vendor.qti.hardware.dsp@1.0 liblog libc libcutils libion libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmdsprpc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/mm-rtp),)

include $(CLEAR_VARS)
LOCAL_MODULE := libmmrtpdecoder_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libmmosal libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmmrtpdecoder_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/mm-rtp),)

include $(CLEAR_VARS)
LOCAL_MODULE := libmmrtpencoder_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmmrtpencoder_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libsubsystem_control
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libqmi_cci libqmi_common_so libmdmdetect libperipheral_client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsubsystem_control.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfddisplayconfig_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libcutils liblog vendor.display.config@2.0 libdisplayconfig.qti libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfddisplayconfig_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdrtsp_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils libutils libmmosal libwfdcommonutils_proprietary libwfdconfigutils_proprietary liblog libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdrtsp_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfduibcinterface_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libwfduibcsrcinterface_proprietary libwfduibcsinkinterface_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfduibcinterface_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfduibcsinkinterface_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libwfduibcsink_proprietary libwfdcommonutils_proprietary libwfdconfigutils_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfduibcsinkinterface_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfduibcsink_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfduibcsink_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfduibcsrcinterface_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libwfduibcsrc_proprietary libwfdcommonutils_proprietary libwfdconfigutils_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfduibcsrcinterface_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfduibcsrc_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal libwfdcommonutils_proprietary liblog libutils libcutils libwfdconfigutils_proprietary libwfddisplayconfig_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfduibcsrc_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.data.factory@2.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.data.factory@2.0.vendor vendor.qti.data.slm@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.data.factory@2.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.2.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.2.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.mwqem@1.0.vendor vendor.qti.data.slm@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.data.factory@2.2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory@2.3.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.data.factory@2.3.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.factory@2.2.vendor vendor.qti.data.mwqem@1.0.vendor vendor.qti.data.slm@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.hardware.data.lce@1.0.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.data.factory@2.3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.cacert@1.0
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.cacert@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.api@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cne.internal.api@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.data.cne.internal.constants@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.data.cne.internal.api@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.constants@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cne.internal.constants@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.data.cne.internal.constants@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.server@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.cne.internal.server@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.data.cne.internal.constants@1.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.data.cne.internal.server@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.qmi@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.data.qmi@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.data.qmi@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.mwqemadapter@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.mwqemadapter@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.mwqemadapter@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccsyshal@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.qccsyshal@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.qccsyshal@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccvndhal@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.qccvndhal@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.qccvndhal@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-interface),)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.sigma_miracast@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.sigma_miracast@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.sigma_miracast@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.slmadapter@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.slmadapter@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.slmadapter@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/commonsys-intf/wfd-interface),)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.wifidisplaysession@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.hardware.wifidisplaysession@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.wifidisplaysession@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory@1.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.factory@1.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.ims.callcapability@1.0.vendor vendor.qti.ims.rcsconfig@2.0.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.ims.factory@1.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory@1.1.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.ims.factory@1.1.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.ims.callcapability@1.0.vendor vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.rcsconfig@2.0.vendor vendor.qti.ims.rcsconfig@2.1.vendor libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.ims.factory@1.1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.imsrtpservice@3.0.vendor
LOCAL_INSTALLED_MODULE_STEM := vendor.qti.imsrtpservice@3.0.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.imsrtpservice@3.0.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

$(shell mkdir -p $(PRODUCT_OUT)/vendor/app/CneApp/lib/arm64//vendor/lib64 && pushd $(PRODUCT_OUT)/vendor/app/CneApp/lib/arm64 > /dev/null && ln -s /vendor/lib64/libvndfwk_detect_jni.qti.so libvndfwk_detect_jni.qti.so && popd > /dev/null)
