PREBUILT_PATH := $(call my-dir)
LOCAL_PATH := $(PREBUILT_PATH)
-include $(LOCAL_PATH)/interface_modules.mk

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.awb.dll
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := ../../.././target/product/kona/data/prebuilt/camera/com.qti.stats.awb.dll
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data/prebuilt/camera
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.awbsimulationwrapper.dll
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := DATA
LOCAL_SRC_FILES := ../../.././target/product/kona/data/prebuilt/camera/com.qti.stats.awbsimulationwrapper.dll
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data/prebuilt/camera
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdrmMinimalfs
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj_arm/STATIC_LIBRARIES/libdrmMinimalfs_intermediates/libdrmMinimalfs.a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnc
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj_arm/STATIC_LIBRARIES/libnc_intermediates/libnc.a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqsappsver
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj_arm/STATIC_LIBRARIES/libqsappsver_intermediates/libqsappsver.a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQSEEComAPIStatic
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj_arm/STATIC_LIBRARIES/libQSEEComAPIStatic_intermediates/libQSEEComAPIStatic.a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librpmbStatic
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj_arm/STATIC_LIBRARIES/librpmbStatic_intermediates/librpmbStatic.a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libslimcommon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj_arm/STATIC_LIBRARIES/libslimcommon_intermediates/libslimcommon.a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libssdStatic
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj_arm/STATIC_LIBRARIES/libssdStatic_intermediates/libssdStatic.a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/anr10regcmd17x.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/anr10regcmd480.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/anrbaseregcmd.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ANR_Chromatix.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ANR_Registers17x.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ANR_Registers480.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/bitmlengine
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/bitmlengine/bitmlengineinterface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/bps_data.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/bpsStripingLib.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/camxifestripinginterface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/camxstatsdebugdatatypes.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/camxstatsdebugdatawriter.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/cdmpayload.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/CommonDefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/CVP_Chromatix.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/dbgdefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/DS4to1_Chromatix.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/DSX_Chromatix.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/DSX_Registers.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/eis2_interface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/eis3_interface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/CommonDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/DetectionInfo.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/DetectorComDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcCtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcDtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcFrAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcGbAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcPcAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcPtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcSmAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v5.x/FaceProcTime.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v6.x/DetectionInfo.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v6.x/DetectorComDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v6.x/FaceProcCoAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v6.x/FaceProcCoDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v6.x/FaceProcCoStatus.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/faceproc/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/faceproc/v6.x/FaceProcDtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/CommonDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/DetectionInfo.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/DetectorComDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcCtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcDtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcFrAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcGbAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcPcAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcPtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcSmAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v5.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v5.x/FaceProcTime.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v6.x/DetectionInfo.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v6.x/DetectorComDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v6.x/FaceProcCoAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v6.x/FaceProcCoDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v6.x/FaceProcCoStatus.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/algo/v6.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/algo/v6.x/FaceProcDtAPI.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/fd/fdengine
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fd/fdengine/camxfdengineinterface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/fwdefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/GeoLib.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/GeoLibUtils.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/hfidefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ICA_Chromatix.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ICA_Registers_v30.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/icpcomdef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/icpdefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ImageTransform.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ipdefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ipebps_data.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ipebpsdefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ipe_data.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/ipeStripingLib.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/is_interface_error_codes.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/is_interface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/is_interface_utils.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/localhistogramalgo
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/localhistogramalgo/camxhclibrary.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/LRME_Chromatix.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/LRME_Registers.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/NcLibChipInfo.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/NcLibContext.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/NcLibWarpCommonDef.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/NcLibWarp.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_AnrPass17x.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_AnrPass480.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_AnrPass.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_CVP.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_DSX.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_ICA.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_LRME.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_TF17x.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_TF480.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/Process_TF.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/registrationalgo/v1.x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/registrationalgo/v1.x/registration.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/stripedebug.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/stripe.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/stripingLib.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/swprocessalgo
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/swprocessalgo/camxswprocessalgo.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/tf10regcmd17x.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/tf20regcmd480.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/tfbaseregcmd.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/TF_Chromatix.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/TF_Registers17x.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/TF_Registers480.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/ori_titan170_base.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_apu32q2n7s1v1_2_cl36.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_base.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_bps.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cam_noc.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cci.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cdm_3_wrapper.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cdm.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cm_mipi_csi_2lane.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cm_mipi_csi_3phase.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cpas_top.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_cpas_top_secure.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_fd_wrapper.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_icp.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_ife.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_ife_lite.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_ipe.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_jpeg_dma.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_jpeg_encode.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan17x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan17x/titan170_lrme.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/ori_titan480_base.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_APU32Q2N7S1V1_2_CL36.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_base.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_bps.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_cam_noc.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_cci.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_cdm.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_clc_cdm_g1r2s0_c1n1.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_cm_mipi_csi_3phase.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_cpas_top.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_cpas_top_secure.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_fd_wrapper.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_icp.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_ife.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_ife_lite_wrapper.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_ipe.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_jpeg_dma.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_jpeg_encode.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx/titan48x
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/titan48x/titan480_sbi.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := camx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/camx/TransformEstimation.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneCet.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneCom.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneConfigs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneDefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/Cne.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneLog.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneMascInterface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneMsg.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneQmi.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneResourceInfo.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneSrmDefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneSrm.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneSupplicantWrapper.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneTimer.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneUtils.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/CneWqeInterface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/EventDispatcher.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/InetAddr.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/queue.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/SocketWrapperClient.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/SwimNetlinkSocket.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/common/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/common/inc/SwimSyncWrapperDefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/wqe/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/wqe/inc/IBitrateEstimator.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/wqe/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/wqe/inc/IWqeFactory.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cne/wqe/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cne/wqe/inc/WqePolicyTypes.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpDcm.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpDfs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpDme.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpFeaturePoint.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpMem.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpNcc.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpObjectDetection.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpOdt.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpOpticalFlow.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpPyramid.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpScale.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpSession.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpTypes.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpUtils.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := cvp/v2.0
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/cvp/v2.0/cvpWarp.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := face3d
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/face3d/Face3DManager.hpp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := face3d
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/face3d/Face3DManagerTypes.hpp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := fastcv
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/fastcv/fastcvExt.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := fastcv
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/fastcv/fastcv.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := hdr-intf
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/hdr-intf/hdr_interface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := hta/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/hta/inc/hta_hexagon_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := hta/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/hta/inc/hta_hexagon_nn_ops.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := hta/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/hta/inc/npu_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := hta/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/hta/inc/npu_sts.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libcdfw
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libcdfw/IQDgnss.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libcdfw
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libcdfw/QDgnssApi.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libcdfw
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libcdfw/QDgnssDataType.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libdatahalfactory
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libdatahalfactory/Creatable.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libizat_core
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libizat_core/IzatAdapterBase.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libizat_core
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libizat_core/IzatApiBase.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libizat_core
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libizat_core/IzatApiRpc.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libizat_core
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libizat_core/IzatApiV02.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libizat_core
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libizat_core/IzatPcid.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_client.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_const.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_defines.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_mac_address.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_request.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_response.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_scan_measurement.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblowi_client/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/liblowi_client/inc/lowi_ssid.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libnlnetmgr/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libnlnetmgr/inc/netmgr_rtnetlink.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libnlnetmgr/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libnlnetmgr/inc/netmgr_xfrm.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libperipheralclient/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libperipheralclient/inc/pm-service.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_common_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_provider_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_qmi_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_qmi_client_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_qmi_dm_log_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_qmi_monitor_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_qmi_service_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/qmislim_task_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/sensor_location_interface_manager_v01.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_client_types.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_core.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_internal_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_internal_client_types.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_internal.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_provider_conf.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_provider_data.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_service_status.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_task.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_timesync.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libslimcommon
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libslimcommon/slim_utils.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libxtadapter
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/libxtadapter/XtAdapter.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/audio-flac
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-audio/audio-flac/FLACDec_API.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/audio-flac
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-audio/audio-flac/FLACDec_BitStream.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/audio-flac
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-audio/audio-flac/FLACDec_MetaData.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/audio-flac
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-audio/audio-flac/FLACDec_Struct.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/audio-flac
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-audio/audio-flac/FLACDec_typedefs.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/audio-flac
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-audio/audio-flac/FLACDec_Wrapper.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-audio/surround_sound_3mic
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-audio/surround_sound_3mic/surround_rec_interface.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-video/streamparser
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-video/streamparser/VideoStreamParser.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-video/swvdec
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-video/swvdec/swvdec_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-video/swvdec
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-video/swvdec/swvdec_types.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-video/swvenc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-video/swvenc/swvenc_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mm-video/swvenc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mm-video/swvenc/swvenc_types.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := mpegh-enc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/mpegh-enc/omx_aenc_logger.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vx_api.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vx_compatibility.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vx.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vx_kernels.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vx_nodes.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vx_types.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vxu.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := openvx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/openvx/vx_vendors.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := qmi-ext/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/qmi-ext/inc/network_access_service_ext_v01.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := qmi-ext/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/qmi-ext/inc/voice_service_common_v01.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := qmi-ext/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/qmi-ext/inc/voice_service_ims_ext_v02.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := scve
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/scve/scveContext.hpp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := scve
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/scve/scveObjectSegmentation.hpp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := scve
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/scve/scveObjectTracker.hpp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := scve
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/scve/scveTypes.hpp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := scve
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/scve/scveUtils.hpp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := ssg
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/ssg/ITzdRegister.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := ssg
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/ssg/minkipc.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := ssg
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/ssg/object.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := synx
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/synx/synx.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := xmllib/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/xmllib/inc/xmllib_common.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := xmllib/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/xmllib/inc/xmllib_parser.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX),kona)
ifeq ($(filter qssi qssi_32 qssi_32go,$(TARGET_PRODUCT)),)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := xmllib/inc
LOCAL_COPY_HEADERS := ../../.././target/product/kona/obj/include/xmllib/inc/xmllib_tok.h
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
endif
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libdrmMinimalfs
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/libdrmMinimalfs_intermediates/libdrmMinimalfs.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libisdbtmm
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/libisdbtmm_intermediates/libisdbtmm.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnc
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/libnc_intermediates/libnc.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqsappsver
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/libqsappsver_intermediates/libqsappsver.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQSEEComAPIStatic
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/libQSEEComAPIStatic_intermediates/libQSEEComAPIStatic.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librpmbStatic
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/librpmbStatic_intermediates/librpmbStatic.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libslimcommon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/libslimcommon_intermediates/libslimcommon.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libssdStatic
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := ../../.././target/product/kona/obj/STATIC_LIBRARIES/libssdStatic_intermediates/libssdStatic.a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := CACertService
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/app/CACertService/CACertService.apk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/app
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := CneApp
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/app/CneApp/CneApp.apk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/app
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ConnectionSecurityService
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/app/ConnectionSecurityService/ConnectionSecurityService.apk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/app
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := pasrservice
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/app/pasrservice/pasrservice.apk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/app
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/biometrics/fingerprint),)

include $(CLEAR_VARS)
LOCAL_MODULE := QFingerprintService
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/app/QFingerprintService/QFingerprintService.apk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/app
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/biometrics/fingerprint),)

include $(CLEAR_VARS)
LOCAL_MODULE := QFPCalibration
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/app/QFPCalibration/QFPCalibration.apk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/app
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := TrustZoneAccessService
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := APPS
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/app/TrustZoneAccessService/TrustZoneAccessService.apk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/app
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := adsprpcd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/adsprpcd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := AnchorTest
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libnativewindow libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/AnchorTest
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := audioflacapp
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libFlacSwDec liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/audioflacapp
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := cdsprpcd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/cdsprpcd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := cnd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libdiag libcutils liblog libnetutils libqti_vndfwk_detect libutils libcne libcneapiclient libqmiservices libhidlbase libandroid_net vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.latency@1.0 vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.factory@2.2.vendor vendor.qti.data.factory@2.3.vendor vendor.qti.latency@2.0.vendor vendor.qti.latency@2.1.vendor vendor.qti.hardware.mwqemadapter@1.0.vendor libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/cnd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := dpmQmiMgr
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils liblog libbase libutils libhardware_legacy libhardware libhidlbase libqmi_cci libqmi_client_helper libqmiservices libdpmqmihal com.qualcomm.qti.dpm.api@1.0.vendor libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/dpmQmiMgr
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := dspservice
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libhidlbase libhardware libutils libcutils liblog libavservices_minijail vendor.qti.hardware.dsp@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/dspservice
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := embmsslServer
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libembmsservice liblog libutils libhidlbase vendor.qti.hardware.embmssl@1.0.vendor vendor.qti.hardware.embmssl@1.1.vendor libqmiservices libqmi_cci libqmi_encdec libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/embmsslServer
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := fastrpc_tests_apps
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libadsprpc liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/fastrpc_tests_apps
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := fastrpc_tests_apps32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := libadsprpc liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/fastrpc_tests_apps32
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := feature_enabler_client
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libhidlbase libhidltransport libminkdescriptor libQSEEComAPI libbinder libdrm libqdutils libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/feature_enabler_client
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := hdcp1prov
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libdiag liblog libhdcp1prov libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hdcp1prov
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := hdcp2p2prov
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libdiag liblog libhdcp2p2prov libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hdcp2p2prov
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := hvdcp_opti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hvdcp_opti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.authsecret@1.0-service-qti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libcrypto libcutils libbase libutils libhardware libhwbinder libhidlbase libhidltransport libkeymasterdeviceutils libspcom android.hardware.authsecret@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/android.hardware.authsecret@1.0-service-qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.gatekeeper@1.0-service-qti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := android.hardware.gatekeeper@1.0 libhardware libhidlbase libutils liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/android.hardware.gatekeeper@1.0-service-qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.keymaster@4.0-service-qti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware libhidlbase android.hardware.keymaster@4.0 libqtikeymaster4 libcrypto libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/android.hardware.keymaster@4.0-service-qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.keymaster@4.0-strongbox-service-qti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware libhidlbase android.hardware.keymaster@4.0 libqtikeymaster4 libcrypto libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/android.hardware.keymaster@4.0-strongbox-service-qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.keymaster@4.1-service-qti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware libhidlbase android.hardware.keymaster@4.0 android.hardware.keymaster@4.1 libqtikeymaster4 libcrypto libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/android.hardware.keymaster@4.1-service-qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.neuralnetworks@1.3-service-qti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libbase libhardware libhidlbase libhidlmemory liblog libutils libcutils libfmq libui libcrypto android.hardware.neuralnetworks@1.0 android.hardware.neuralnetworks@1.1 android.hardware.neuralnetworks@1.2 android.hardware.neuralnetworks@1.3 android.hidl.safe_union@1.0 android.hidl.allocator@1.0 android.hidl.memory@1.0 libprotobuf-cpp-full-3.9.1 libsync libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/android.hardware.neuralnetworks@1.3-service-qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qvrservice
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libqvrservice liblog libnativewindow android.hardware.graphics.allocator@4.0 libavservices_minijail libcutils libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/qvrservice
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := sxrservice
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.qxr-V1-ndk_platform libbinder_ndk libbase liblog libutils libcutils libbinder libhardware libnativewindow libsxrservice libavservices_minijail libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/sxrservice
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.display.color@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libbinder libhardware_legacy libhardware libhidlbase libhidltransport vendor.display.color@1.0 vendor.display.color@1.1 vendor.display.color@1.2 vendor.display.color@1.3 vendor.display.color@1.4 vendor.display.color@1.5 vendor.display.color@1.6 vendor.display.color@1.7 vendor.display.color@1.8 vendor.display.postproc@1.0 libsdm-disp-vndapis libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.display.color@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qseecom@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libbase libhidlbase libutils liblog vendor.qti.hardware.qseecom@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.qti.hardware.qseecom@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qteeconnector@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils liblog vendor.qti.hardware.qteeconnector@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.qti.hardware.qteeconnector@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.soter@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.soter@1.0 libhidlbase liblog libutils libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.qti.hardware.soter@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.tui_comm@1.0-service-qti
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware libhidlbase libbinder vendor.qti.hardware.tui_comm@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.qti.hardware.tui_comm@1.0-service-qti
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.media.c2@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libqcodec2_utils libqcodec2_basecodec libqcodec2_platform libqcodec2_core android.hardware.media.c2@1.0 libavservices_minijail libbinder libhidlbase libvndksupport libcodec2_hidl@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.qti.media.c2@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.power.pasrmanager@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libhardware_legacy libbase libhardware libhidlbase vendor.qti.power.pasrmanager@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.qti.power.pasrmanager@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.spu@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libcutils liblog libspcom libqti-utils libhidlmemory vendor.qti.spu@1.0 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/hw/vendor.qti.spu@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := imsdatadaemon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libqmi_cci libcutils liblog libqmiservices libqmi_csi libdsi_netctrl libdiag libcneapiclient libandroid_net libutils libhidlbase libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/imsdatadaemon
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := imsqmidaemon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libqmiservices libqmi_cci libidl libcutils libutils liblog libdiag libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/imsqmidaemon
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := imsrcsd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libbase libhidlbase libcutils libutils libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/imsrcsd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ims_rtp_daemon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libdiag libcutils liblog lib-imsvtcore lib-imsdpl lib-rtpsl lib-rtpcore lib-rtpcommon libqmiservices libqmi_csi libqmi_cci vendor.qti.imsrtpservice@3.0.vendor vendor.qti.imsrtpservice@3.0-service-Impl libutils libhidlbase libavservices_minijail libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/ims_rtp_daemon
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := init.qti.chg_policy.sh
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES :=
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/init.qti.chg_policy.sh
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := keymasterd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libhidlbase libQSEEComAPI libkeymasterdeviceutils libkeymasterutils libbase android.hardware.keymaster@4.0 libcrypto libspcom libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/keymasterd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ks
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libc libcutils libutils liblog libc++ libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/ks
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lowi-server
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libgps.utils liblog libnl libwpa_client libqmi_cci libqmi_common_so libdiag libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/lowi-server
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lowi_test
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils liblog liblowi_client libxml2 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/lowi_test
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := mlid
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils liblowi_client libminksocket libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/mlid
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := mm-adec-omxMpegh-test
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := libOmxMpeghDecSw liblog libmm-omxcore libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/mm-adec-omxMpegh-test
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := msm_irqbalance
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/msm_irqbalance
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := mutualex
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils liblog libdiag libnetutils libutils libhardware_legacy libqmiservices libqmi_cci libnl libidl libandroid_net libhidlbase libwpa_client libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/mutualex
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := nn_device_test
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/nn_device_test
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := npu_launcher
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libutils liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/npu_launcher
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := pm-proxy
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libperipheral_client libcutils liblog libmdmdetect libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/pm-proxy
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := pm-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbinder libqmi_cci libqmi_common_so libqmi_encdec libqmi_csi libmdmdetect libperipheral_client libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/pm-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := poweropt-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware_legacy libhardware libhwbinder libbinder libhidlbase libhidltransport libpowercore libtinyxml2 vendor.qti.hardware.perf@2.1 libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/poweropt-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qcc-trd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_CHECK_ELF_FILES := false
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qcc-trd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qcom-system-daemon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libdiag libxml2 libsubsystem_control libavservices_minijail libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qcom-system-daemon
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qdcmss
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libutils libqservice libsdmutils libbinder liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qdcmss
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qdss_qmi_helper
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_common_so libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qdss_qmi_helper
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/biometrics/fingerprint),)

include $(CLEAR_VARS)
LOCAL_MODULE := qfp-daemon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libutils libcutils libqfp-service liblog android.hardware.biometrics.fingerprint@2.1 vendor.qti.hardware.fingerprint@1.0 libhidlbase libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qfp-daemon
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := qmipriod
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmiservices libqmi_client_helper libcutils libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qmipriod
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qvrcameratest
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libnativewindow liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qvrcameratest
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qvrdatacapture
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := liblog libqvrservice_client.qti libnativewindow libutils libcutils libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qvrdatacapture
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qvrdatalogger
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libui libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qvrdatalogger
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qvrservicetest
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := libc++ liblog libutils libcutils libqvrservice_client.qti libnativewindow libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qvrservicetest
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qvrservicetest64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libc++ liblog libutils libcutils libqvrservice_client.qti libnativewindow libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/qvrservicetest64
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := sec_nvm
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libspcom libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/sec_nvm
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := shsusrd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcrypto libssl libqmiservices libqmi_cci libqmi_common_so libqmi_client_qmux libqmi_client_helper libdsutils libconfigdb libcutils liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/shsusrd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ssgqmigd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := liblog libm libc libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/ssgqmigd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ssgtzd
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libqcbor libminksocket libminkdescriptor liblog libjsoncpp libcutils libQSEEComAPI libcurl libqmi libqmi_common_so libqmi_csi libqmi_cci libdsi_netctrl libcrypto libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/ssgtzd
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := StoreKeybox
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := liboemcrypto libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/StoreKeybox
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := testapp_diag_senddata
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libdiag liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/testapp_diag_senddata
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := tftp_server
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libqsocket libqrtr liblog libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/tftp_server
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := tloc_daemon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils libQSEEComAPI liblog libloc_api_v02 libqsocket libqrtr libminkdescriptor libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/tloc_daemon
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.qspmhal@1.0-service
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := vendor.qti.qspmhal@1.0 vendor.qti.qspmhal@1.0-impl libhidlbase liblog libutils libavservices_minijail libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/vendor.qti.qspmhal@1.0-service
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := wfdhdcphalservice
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := android.hardware.drm@1.0 android.hardware.drm@1.1 android.hardware.drm@1.2 android.hardware.drm@1.3 android.hidl.memory@1.0 libhidlmemory libc++ libhidlbase libbinder libbase liblog libutils libcutils libwfdhdcpcp libwfdhdcpservice_proprietary libwfdcommonutils_proprietary libwfdutils_proprietary libqdMetaData libavservices_minijail libwfddisplayconfig_proprietary libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/wfdhdcphalservice
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := wfdvndservice
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := libutils liblog libbinder libwfdhdcpservice_proprietary libavservices_minijail libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/wfdvndservice
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := wifidisplayhalservice
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.wifidisplaysession@1.0.vendor vendor.qti.hardware.wifidisplaysessionl@1.0-halimpl libhidlbase libutils liblog libbinder libavservices_minijail libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/wifidisplayhalservice
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := xtra-daemon
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libutils libcutils libcrypto libssl libsqlite libgps.utils libloc_core libizat_core liblbs_core liblog libandroid_net libavservices_minijail libcdfw_remote_api libqcc_file_agent libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/xtra-daemon
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := xtwifi-client
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libcutils liblog libcrypto libz libsqlite libqmiservices libqmi_cci libqmi_common_so libgps.utils libloc_core libizat_core liblbs_core libavservices_minijail libgdtap libasn1cper libasn1crt libasn1crtx libxtwifi_server_protocol libxtwifi_server_protocol_uri_v3 liblowi_client libc++ libc libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/xtwifi-client
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := xtwifi-inet-agent
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SHARED_LIBRARIES := libc libcutils liblog libcurl libgps.utils libavservices_minijail libc++ libm libdl
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/bin/xtwifi-inet-agent
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/biometrics/fingerprint),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/biometrics/fingerprint),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.neuralnetworks@1.3-service-qti.xml
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/etc/vintf/manifest/android.hardware.neuralnetworks@1.3-service-qti.xml
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/etc/vintf/manifest
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := c2_manifest_vendor.xml
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/etc/vintf/manifest/c2_manifest_vendor.xml
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/etc/vintf/manifest
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qxr-service.xml
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/etc/vintf/manifest/vendor.qti.hardware.qxr-service.xml
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/etc/vintf/manifest
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.eisv2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libsync libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.eisv2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.eisv3
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libsync libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.eisv3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.aec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.aec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.afd
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.afd.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.af
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.af.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.asd
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.asd.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.awb
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.awb.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.haf
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.haf.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.pdlib
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.pdlib.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.pdlibsony
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.pdlibsony.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.pdlibwrapper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.pdlibwrapper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.tracker
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libfastcvopt libfastcvdsp_stub libscveCommon libscveCommon_stub libscveObjectTracker libscveObjectTracker_stub libscveObjectSegmentation libscveObjectSegmentation_stub libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/camera/components/com.qti.stats.tracker.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := deviceInfoServiceModule
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := vendor.qti.hardware.radio.internal.deviceinfo@1.0 libqcrilFramework libril-qc-hal-qmi libril-qc-logger libutils librilqmiservices libqmi_cci libhidlbase liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/deviceInfoServiceModule.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.gatekeeper@1.0-impl-qti
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := android.hardware.gatekeeper@1.0 libhidlbase libutils liblog libcutils libkeymasterdeviceutils libqcbor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/hw/android.hardware.gatekeeper@1.0-impl-qti.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qseecom@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libhidlbase libhidlmemory libutils libcutils liblog vendor.qti.hardware.qseecom@1.0 libQSEEComAPI android.hidl.allocator@1.0 android.hidl.memory@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/hw/vendor.qti.hardware.qseecom@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qteeconnector@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils liblog vendor.qti.hardware.qteeconnector@1.0 libQSEEComAPI libxml2 libion libGPreqcancel_svc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/hw/vendor.qti.hardware.qteeconnector@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.soter@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhwbinder libhidlbase libhidltransport libutils libcutils libcrypto libbase liblog vendor.qti.hardware.soter@1.0 libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/hw/vendor.qti.hardware.soter@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.power.pasrmanager@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbase libhidlbase vendor.qti.power.pasrmanager@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/hw/vendor.qti.power.pasrmanager@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libadaptlaunch
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils liblearningmodule libmeters-ns libsqlite libqti-perfd-client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libadaptlaunch.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libadsp_default_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libadsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libadsp_default_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libAlacSwDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libAlacSwDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libaoa
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libaoa.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libApeSwDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libApeSwDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libappclassifier
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils liblearningmodule libsqlite libqti-perfd-client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libappclassifier.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasn1cper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libasn1crtx libasn1crt liblog libgps.utils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libasn1cper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasn1crt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libasn1crtx libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libasn1crt.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasn1crtx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libgps.utils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libasn1crtx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libbase64
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libbase64.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libbitmlengine
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libcdsprpc libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libbitmlengine.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcacertclient
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libhidlbase vendor.qti.hardware.cacert@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcacertclient.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamera_nn_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils liblog libofflinelog libsync libcdsprpc libqdMetaData libhardware libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamera_nn_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxfacialfeatures
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcamxfdalgo libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamxfacialfeatures.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxfdalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamxfdalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxfdengine
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbitmlengine libcamera_nn_stub libcamxfdalgo libcamxfacialfeatures libcdsprpc libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamxfdengine.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxifestriping
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils liblog libofflinelog libsync libcdsprpc libqdMetaData libhardware libz libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamxifestriping.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxstatscore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamxstatscore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxswprocessalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamxswprocessalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxtintlessalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcamxtintlessalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcapiv2svacnn
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcapiv2svacnn.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcapiv2svarnn
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcapiv2svarnn.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcapiv2vop
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcapiv2vop.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdfw_remote_api
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libgps.utils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcdfw_remote_api.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdfw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libgps.utils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcdfw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdsp_default_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcdsp_default_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcneapiclient
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libnetutils libutils libdiag liblog libhidlbase vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.factory@2.2.vendor libandroid_net libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcneapiclient.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcne
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libdiag libnetutils libutils libcneapiclient libqmiservices libqmi_cci libqti_vndfwk_detect libhardware_legacy libhidlbase libnl libidl vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.latency@1.0 vendor.qti.hardware.data.lce@1.0.vendor libandroid_net vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.factory@2.2.vendor vendor.qti.data.factory@2.3.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor vendor.qti.latency@2.1.vendor vendor.qti.hardware.mwqemadapter@1.0.vendor vendor.qti.data.mwqem@1.0.vendor libwpa_client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcne.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcpion
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcpion.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvp2_hfi
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcvp2_hfi.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvp2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libcvp_common libcvp2_hfi libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcvp2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvp_common
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcvp_common.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvpcpuRev_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libcdsprpc libion libcvp_common libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libcvpcpuRev_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdepthapi
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libSNPE libc++_shared libc libm libdl liblog
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdepthapi.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdisp-aba
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdisp-aba.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdisplayqos
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdisplayqos.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdisplayskuutils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdisplayskuutils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdpmqmihal
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libcutils libbase libutils libhardware_legacy libhardware com.qualcomm.qti.dpm.api@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdpmqmihal.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdpps
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libdisp-aba libqdutils libutils liblog libbinder libdrm libtinyxml2 libhidlbase libhidltransport android.frameworks.sensorservice@1.0 libtinyxml libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdpps.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdrmfs
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libxml2 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdrmfs.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdrmtime
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libtime_genoff libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libdrmtime.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libembmsservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libhidlbase vendor.qti.hardware.embmssl@1.0.vendor libqmi libqmi_client_helper libqmi_cci libqmiservices libidl libdsutils libtime_genoff libdiag libdsi_netctrl libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libembmsservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libeye_tracking_dsp_sample_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libeye_tracking_dsp_sample_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libface3d_dev
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libbinder liblog libcutils libui libion libFace3D_hlos libFace3DTA libQ6MSFR_manager_stub libcamera2ndk_vendor libmediandk libnativewindow libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libface3d_dev.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libFace3D_hlos
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libFace3DTA libQ6MSFR_manager_stub libscveCommon_stub liblog libcdsprpc libutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libFace3D_hlos.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libFace3DTA
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libQSEEComAPI libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libFace3DTA.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastcvdsp_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libfastcvdsp_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastcvopt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libfastcvdsp_stub libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libfastcvopt.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libFlacSwDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libFlacSwDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libgameoptfeature
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils liblearningmodule libmeters-ns libsqlite libqti-perfd-client vendor.display.config@1.0 libhidlbase libutils libthermalclient android.hidl.allocator@1.0 vendor.qti.qspmhal@1.0 android.hidl.memory@1.0 libhidlmemory libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libgameoptfeature.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libgdtap
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libgps.utils liblog libloc_core libizat_core liblbs_core libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libgdtap.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPQTEEC_vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc libcutils libutils liblog libhidlbase libGPTEE_vendor libQTEEConnector_vendor vendor.qti.hardware.qteeconnector@1.0 libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libGPQTEEC_vendor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPreqcancel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libGPreqcancel_svc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libGPreqcancel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPreqcancel_svc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libGPreqcancel_svc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPTEE_vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc libcutils libutils liblog libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libGPTEE_vendor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhalide_hexagon_host
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libhalide_hexagon_host.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdcp1prov
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libhdcp1prov.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdcp2p2prov
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libhdcp2p2prov.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdcpsrm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libhdcpsrm.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdr_tm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libsdm-color libtinyxml2_1 libcutils libutils libdisplayskuutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libhdr_tm.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhexagon_nn_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libhexagon_nn_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhta_hexagon_runtime
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libhta_hexagon_runtime.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libI420colorconvert
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libmm-color-convertor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libI420colorconvert.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imscmservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhidlbase com.qualcomm.qti.imscmservice@2.0.vendor lib-imsdpl lib-imsqimf lib-imsxml libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-imscmservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsdpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libqmiservices libcutils libutils liblog libtime_genoff libcrypto libssl libandroid_net libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-imsdpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsqimf
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl lib-imsxml liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-imsqimf.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsrcsbaseimpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware_legacy libhardware libhidlbase libqmiservices libqmi_cci libqti_vndfwk_detect libidl com.qualcomm.qti.imscmservice@2.0.vendor com.qualcomm.qti.imscmservice@2.1.vendor com.qualcomm.qti.imscmservice@2.2.vendor com.qualcomm.qti.uceservice@2.0.vendor com.qualcomm.qti.uceservice@2.1.vendor com.qualcomm.qti.uceservice@2.2.vendor com.qualcomm.qti.uceservice@2.3.vendor vendor.qti.ims.callinfo@1.0 vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.factory@1.1.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-imsrcsbaseimpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsrcs-v2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl lib-imsqimf lib-imsxml lib-siputility libqmi_cci liblog libcutils libbase libutils libhardware_legacy libhardware libhidlbase libqmiservices libqti_vndfwk_detect lib-rcsconfig libidl com.qualcomm.qti.imscmservice@2.0.vendor com.qualcomm.qti.uceservice@2.0.vendor com.qualcomm.qti.uceservice@2.1.vendor com.qualcomm.qti.uceservice@2.2.vendor com.qualcomm.qti.uceservice@2.3.vendor com.qualcomm.qti.imscmservice@2.1.vendor com.qualcomm.qti.imscmservice@2.2.vendor vendor.qti.ims.callinfo@1.0 vendor.qti.ims.rcsconfig@2.0.vendor vendor.qti.ims.rcsconfig@2.1.vendor vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.factory@1.1.vendor vendor.qti.ims.callcapability@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-imsrcs-v2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsvtcore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := librcc libion libhidlbase vendor.qti.imsrtpservice@3.0.vendor vendor.qti.imsrtpservice@3.0-service-Impl libdiag libcutils libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-imsvtcore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsxml
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-imsxml.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libipebpsstriping
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils liblog libofflinelog libsync libcdsprpc libqdMetaData libhardware libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libipebpsstriping.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libizat_core
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libloc_core libgps.utils libloc_api_v02 libsqlite liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libizat_core.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libjnihelper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libqmiservices libqmi_cci libcacertclient libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libjnihelper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libkeymasterdeviceutils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion libcutils libQSEEComAPI liblog libcrypto libspcom libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libkeymasterdeviceutils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libkeymasterprovision
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcrypto libkeymasterdeviceutils libkeymasterutils libqcbor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libkeymasterprovision.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liblistensoundmodel2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/liblistensoundmodel2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liblowi_client
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/liblowi_client.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liblowi_wifihal
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblowi_client libcutils libutils liblog libgps.utils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/liblowi_wifihal.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libMapService
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libdl liblog libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libMapService.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmeters-ns
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils liblearningmodule libqti-perfd-client libqti-perfd libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmeters-ns.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libminksocket
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libqrtr libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libminksocket.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libmiracast
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase vendor.qti.hardware.sigma_miracast@1.0.vendor libmmosal liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmiracast.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libmmcamera_faceproc2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libdl libc libm
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmmcamera_faceproc2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmmcamera_faceproc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_SHARED_LIBRARIES := libc++ libdl libc libm
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmmcamera_faceproc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmm-color-convertor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmm-color-convertor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmm-hdcpmgr
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmm-hdcpmgr.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libMpeg4SwEncoder
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libGLESv2 libEGL liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libMpeg4SwEncoder.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libMpeghSwEnc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libMpeghSwEnc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmulawdec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libmulawdec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnative-api
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libminksocket liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libnative-api.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnetmgr_nr_fusion
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdsutils libconfigdb libdiag libcutils liblog libnetmgr libnetmgr_common libqmiservices libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libnetmgr_nr_fusion.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnetmgr_rmnet_ext
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdsutils libconfigdb libdiag libcutils liblog libnetmgr libnetmgr_common librmnetctl libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libnetmgr_rmnet_ext.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnlnetmgr
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdsutils libconfigdb libdiag libcutils liblog libnetmgr libnetmgr_common libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libnlnetmgr.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnpu
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libnpu.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liboemcrypto
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libQSEEComAPI libhidlbase libcpion libminkdescriptor libcutils libtrustedapploader libdisplayconfig.qti libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/liboemcrypto.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxAlacDecSw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libOmxAlacDecSw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxApeDecSw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libOmxApeDecSw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxEvrcDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libOmxEvrcDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxMpeghEncSw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libMpeghSwEnc liblog libutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libOmxMpeghEncSw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxQcelp13Dec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libOmxQcelp13Dec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libopenvx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libopenvx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libops
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libQSEEComAPI liblog libdrm libhidlbase libdisplayconfig.qti libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libops.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libos
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libos.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libperipheral_client
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbinder libmdmdetect libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libperipheral_client.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libpowercallback
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware_legacy libhardware libhwbinder libbinder libhidlbase libhidltransport libpowercore libtinyxml2 vendor.qti.hardware.perf@2.1 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libpowercallback.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libpowercore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbinder libtinyxml2 vendor.display.config@1.0 libhidlbase libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libpowercore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libpvr
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libpvr.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQ6MSFR_manager_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libQ6MSFR_manager_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcc_file_agent
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libutils libbase libfmq libhidlbase vendor.qti.hardware.qccsyshal@1.0.vendor vendor.qti.hardware.qccsyshal@1.1 vendor.qti.hardware.qccsyshal@1.2 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcc_file_agent.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_basecodec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libqcodec2_utils libqcodec2_platform libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcodec2_basecodec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_base
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcodec2_base.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_core
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libqcodec2_utils libqcodec2_platform libqcodec2_basecodec libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcodec2_core.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_platform
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libjsoncpp libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcodec2_platform.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_utils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libqcodec2_platform libhidlbase android.hardware.graphics.mapper@3.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcodec2_utils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_v4l2codec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libqcodec2_base libqcodec2_platform libqcodec2_utils libqcodec2_basecodec libcodec2_vndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcodec2_v4l2codec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libqcrildatactl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmiservices libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqcrildatactl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/biometrics/fingerprint),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqfp-service
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libutils libhidlbase libhidltransport android.hardware.biometrics.fingerprint@2.1 vendor.qti.hardware.fingerprint@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqfp-service.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libqisl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqisl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqmiextservices
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libidl libqmiservices libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqmiextservices.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQSEEComAPI
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion libcutils libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libQSEEComAPI.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqseed3
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqseed3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQTEEConnector_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libcutils liblog libQSEEComAPI libxml2 libbinder libcrypto libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libQTEEConnector_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQTEEConnector_vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libcutils libutils vendor.qti.hardware.qteeconnector@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libQTEEConnector_vendor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqtikeymaster4
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcrypto libhidlbase libutils libcutils libkeymasterdeviceutils libkeymasterutils libbase libqcbor android.hardware.keymaster@4.0 android.hardware.keymaster@4.1 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqtikeymaster4.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_cam_cdsp_driver_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqvr_cam_cdsp_driver_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvrcamera_client.qti
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libnativewindow libui libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqvrcamera_client.qti.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_cdsp_driver_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqvr_cdsp_driver_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_eyetracking_plugin
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqvr_eyetracking_plugin.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_hostcontroller_plugin
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libqvrservice libhidlbase libnativewindow android.frameworks.sensorservice@1.0 android.hardware.graphics.allocator@4.0 android.hardware.graphics.mapper@4.0 android.hardware.sensors@1.0 android.hardware.sensors@2.0 libc++ libcutils liblog libutils libjsoncpp libprotobuf-cpp-full-3.9.1 libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqvr_hostcontroller_plugin.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvrservice_client.qti
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libdrm libutils libnativewindow libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqvrservice_client.qti.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvrservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libnativewindow libjsoncpp android.hardware.graphics.mapper@4.0 android.hardware.graphics.allocator@4.0 libprotobuf-cpp-full-3.9.1 libhidlbase libhardware libbinder libbase libbinder_ndk vendor.qti.hardware.qxr-V1-ndk_platform android.hardware.sensors@1.0 android.frameworks.sensorservice@1.0 android.hardware.sensors@2.0 libdrm vendor.display.config@2.0 libdisplayconfig.qti libhwbinder libui libfmq libcamera_metadata android.hardware.camera.provider@2.4 android.hardware.camera.device@3.2 android.hardware.camera.device@3.4 android.hardware.camera.common@1.0 libqvrcamera_client.qti android.hardware.thermal@1.0 android.hardware.thermal@2.0 libusbhost libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libqvrservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librcc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libdiag libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/librcc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rcsconfig
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl libqmi_cci libcutils libutils liblog libqmiservices libqmi_csi libdiag libhidlbase libhardware_legacy libqti_vndfwk_detect vendor.qti.ims.rcsconfig@2.0.vendor vendor.qti.ims.rcsconfig@2.1.vendor vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.factory@1.1.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-rcsconfig.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libreverse_rpc_tests_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libadsprpc liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libreverse_rpc_tests_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librmp
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/librmp.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librpmb
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libQSEEComAPI liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/librpmb.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rtpcommon
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libcutils libutils liblog lib-imsdpl libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-rtpcommon.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rtpcore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libcutils libutils liblog lib-imsdpl lib-imsvtcore lib-rtpcommon libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-rtpcore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rtpsl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog lib-imsdpl lib-rtpcore lib-rtpcommon libqti_vndfwk_detect libdiag libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-rtpsl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libScenescape
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdl liblog libm libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libScenescape.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveCommon
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libscveCommon_stub libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libscveCommon.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveCommon_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libscveCommon_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectSegmentation
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libfastcvopt libscveCommon libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libscveObjectSegmentation.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectSegmentation_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libscveCommon libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libscveObjectSegmentation_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectTracker
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libfastcvopt libscveCommon libscveObjectSegmentation libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libscveObjectTracker.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectTracker_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libfastcvopt liblog libscveCommon libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libscveObjectTracker_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-colormgr-algo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsdm-colormgr-algo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-color
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbinder libutils libdisplaydebug libtinyxml2_1 libsdmutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsdm-color.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-diag
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libsdmutils libbinder libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsdm-diag.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-disp-vndapis
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdisplaydebug libqservice libbinder libcutils libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsdm-disp-vndapis.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdmextension
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdisplaydebug libsdmutils libdisplayqos libsdm-color libdisplayskuutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsdmextension.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libSecureUILib
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libStDrvInt libQSEEComAPI libsecureui_svcsock libhidlbase libbinder libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libSecureUILib.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsecureui
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libQSEEComAPI libStDrvInt libhidlbase libbinder libdisplayconfig.qti libion libsecureui_svcsock vendor.display.config@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsecureui.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsensor_calibration
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsensor_calibration.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-siputility
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl lib-imsxml lib-imsqimf liblog libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-siputility.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsi
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libpng libQSEEComAPI libtrustedapploader libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsi.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libskewknob
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc libcutils libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libskewknob.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libspcom
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion libutils libcutils liblog libcrypto libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libspcom.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libspl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libspl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libssc_default_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libsdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libssc_default_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libssd
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libssd.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libStereoRectifyWrapper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libdl libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libStereoRectifyWrapper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libswregistrationalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libswregistrationalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libswvdec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libcdsprpc liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libswvdec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsxrcryptoimpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libui liblog libutils libGLESv2 libion libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsxrcryptoimpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsxrservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libui liblog libutils libEGL libGLESv2 libusbhost libnativewindow libhidlbase libhidltransport libfmq libtinyalsa libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsxrservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsynx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libos libthreadutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsynx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsysmon_cdsp_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libsysmon_cdsp_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libthreadutils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libos libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libthreadutils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtinyxml2_1
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libtinyxml2_1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtracker_6dof_impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libtracker_6dof_impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtrustedapploader
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libminkdescriptor libminksocket libqcbor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libtrustedapploader.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtzdrmgenprov
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libtzdrmgenprov.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-uceservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhidlbase lib-imsdpl lib-imsqimf lib-imsxml lib-siputility libqti_vndfwk_detect lib-imscmservice com.qualcomm.qti.imscmservice@2.0.vendor com.qualcomm.qti.uceservice@2.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/lib-uceservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvideooptfeature
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbinder libtinyxml2 libpowercore libdisplayconfig.qti libhidlbase libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libvideooptfeature.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvpphcp
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libvpphcp.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvpphvx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libvpphvx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvppimmotion
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libadsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libvppimmotion.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libwms
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcne libqmiservices libqmi_cci liblog libcneoplookup libandroid_net libutils vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.latency@1.0 vendor.qti.latency@2.0.vendor vendor.qti.hardware.mwqemadapter@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libwms.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libwqe
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder libcutils libnetutils libutils liblog libsqlite libcneapiclient libcne libnl libxml libxml2 libandroid_net libqti_vndfwk_detect vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.mwqemadapter@1.0.vendor libwpa_client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libwqe.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxml
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libdiag libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libxml.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxtadapter
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libgps.utils liblog libloc_core libizat_core libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libxtadapter.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxtwifi_server_protocol
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libasn1cper libasn1crt libasn1crtx libloc_core libgps.utils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libxtwifi_server_protocol.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxtwifi_server_protocol_uri_v3
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libgps.utils libizat_core libasn1cper libasn1crt libasn1crtx libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/libxtwifi_server_protocol_uri_v3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasphere
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/soundfx/libasphere.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcbassboost
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/soundfx/libqcbassboost.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcreverb
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/soundfx/libqcreverb.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcvirt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/soundfx/libqcvirt.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libshoebox
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/soundfx/libshoebox.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := unnhal-acc-adreno
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libbase libprotobuf-cpp-full-3.9.1 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/unnhal-acc-adreno.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := unnhal-acc-common
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libhardware libhidlbase libhidlmemory libui liblog libnativewindow libutils libcutils libfmq android.hardware.neuralnetworks@1.0 android.hardware.neuralnetworks@1.1 android.hardware.neuralnetworks@1.2 android.hardware.neuralnetworks@1.3 android.hidl.safe_union@1.0 android.hidl.allocator@1.0 android.hidl.memory@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/unnhal-acc-common.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := unnhal-acc-hvx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libhardware libhidlbase libhidlmemory liblog libutils libcutils android.hidl.allocator@1.0 android.hidl.memory@1.0 libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/unnhal-acc-hvx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccvndhal@1.0-halimpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libfmq libcutils liblog vendor.qti.hardware.qccvndhal@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.hardware.qccvndhal@1.0-halimpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.imsrtpservice@3.0-service-Impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware_legacy libhardware libhidlbase libdiag vendor.qti.imsrtpservice@3.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.imsrtpservice@3.0-service-Impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.qspmhal@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils vendor.qti.qspmhal@1.0 android.hidl.allocator@1.0 android.hidl.memory@1.0 libhidlbase libhidlmemory liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 64
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib64/vendor.qti.qspmhal@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib64
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.eisv2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libsync libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.eisv2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.eisv3
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libsync libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.eisv3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.aec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.aec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.afd
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.afd.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.af
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.af.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.asd
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.asd.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.awb
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.awb.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.haf
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.haf.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.pdlib
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.pdlib.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.pdlibsony
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.pdlibsony.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.pdlibwrapper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.pdlibwrapper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/camx-lib-stats),)

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.stats.tracker
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libfastcvopt libfastcvdsp_stub libscveCommon libscveCommon_stub libscveObjectTracker libscveObjectTracker_stub libscveObjectSegmentation libscveObjectSegmentation_stub libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/camera/components/com.qti.stats.tracker.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/camera/components
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.gatekeeper@1.0-impl-qti
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := android.hardware.gatekeeper@1.0 libhidlbase libutils liblog libcutils libkeymasterdeviceutils libqcbor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/hw/android.hardware.gatekeeper@1.0-impl-qti.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qseecom@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbase libhidlbase libhidlmemory libutils libcutils liblog vendor.qti.hardware.qseecom@1.0 libQSEEComAPI android.hidl.allocator@1.0 android.hidl.memory@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/hw/vendor.qti.hardware.qseecom@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qteeconnector@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils liblog vendor.qti.hardware.qteeconnector@1.0 libQSEEComAPI libxml2 libion libGPreqcancel_svc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/hw/vendor.qti.hardware.qteeconnector@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.soter@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhwbinder libhidlbase libhidltransport libutils libcutils libcrypto libbase liblog vendor.qti.hardware.soter@1.0 libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/hw/vendor.qti.hardware.soter@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.power.pasrmanager@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbase libhidlbase vendor.qti.power.pasrmanager@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/hw/vendor.qti.power.pasrmanager@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/hw
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libadm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libtinyalsa libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libadm.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libadpcmdec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libadpcmdec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libadsp_default_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libadsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libadsp_default_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libAlacSwDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libAlacSwDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libaoa
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libaoa.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libApeSwDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libApeSwDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasn1cper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libasn1crtx libasn1crt liblog libgps.utils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libasn1cper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasn1crt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libasn1crtx libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libasn1crt.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasn1crtx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libgps.utils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libasn1crtx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libbase64
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libbase64.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libbitmlengine
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libcdsprpc libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libbitmlengine.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcacertclient
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libhidlbase vendor.qti.hardware.cacert@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcacertclient.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamera_nn_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils liblog libofflinelog libsync libcdsprpc libqdMetaData libhardware libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamera_nn_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxfacialfeatures
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcamxfdalgo libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamxfacialfeatures.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxfdalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamxfdalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxfdengine
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbitmlengine libcamera_nn_stub libcamxfdalgo libcamxfacialfeatures libcdsprpc libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamxfdengine.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxifestriping
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils liblog libofflinelog libsync libcdsprpc libqdMetaData libhardware libz libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamxifestriping.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxstatscore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamxstatscore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxswprocessalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamxswprocessalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamxtintlessalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libofflinelog libsync libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcamxtintlessalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcapiv2svacnn
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcapiv2svacnn.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcapiv2svarnn
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcapiv2svarnn.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcapiv2vop
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcapiv2vop.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdfw_remote_api
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libgps.utils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcdfw_remote_api.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdfw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libgps.utils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcdfw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcdsp_default_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcdsp_default_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcneapiclient
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libnetutils libutils libdiag liblog libhidlbase vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.factory@2.2.vendor libandroid_net libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcneapiclient.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcne
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libdiag libnetutils libutils libcneapiclient libqmiservices libqmi_cci libqti_vndfwk_detect libhardware_legacy libhidlbase libnl libidl vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.constants@1.0.vendor vendor.qti.hardware.data.latency@1.0 vendor.qti.hardware.data.lce@1.0.vendor libandroid_net vendor.qti.data.factory@2.0.vendor vendor.qti.data.factory@2.1.vendor vendor.qti.data.factory@2.2.vendor vendor.qti.data.factory@2.3.vendor vendor.qti.hardware.data.qmi@1.0.vendor vendor.qti.hardware.data.dynamicdds@1.0.vendor vendor.qti.ims.rcsconfig@1.0.vendor vendor.qti.ims.rcsconfig@1.1.vendor vendor.qti.latency@2.0.vendor vendor.qti.latency@2.1.vendor vendor.qti.hardware.mwqemadapter@1.0.vendor vendor.qti.data.mwqem@1.0.vendor libwpa_client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcne.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcpion
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcpion.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcppf
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcppf.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvp2_hfi
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcvp2_hfi.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvp2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libcvp_common libcvp2_hfi libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcvp2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvp_common
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcvp_common.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvpcpuRev_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libcdsprpc libion libcvp_common libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libcvpcpuRev_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdepthapi
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdl libSNPE libc++_shared libc libm liblog
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdepthapi.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdisp-aba
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdisp-aba.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdisplayqos
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdisplayqos.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdisplayskuutils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdisplayskuutils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdpmqmihal
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libcutils libbase libutils libhardware_legacy libhardware com.qualcomm.qti.dpm.api@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdpmqmihal.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdpps
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libdisp-aba libqdutils libutils liblog libbinder libdrm libtinyxml2 libhidlbase libhidltransport android.frameworks.sensorservice@1.0 libtinyxml libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdpps.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdrc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdrc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdrmfs
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libxml2 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdrmfs.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdrmtime
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libtime_genoff libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdrmtime.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdsd2pcm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libdsd2pcm.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libeye_tracking_dsp_sample_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libeye_tracking_dsp_sample_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libface3d_dev
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libbinder liblog libcutils libui libion libFace3D_hlos libFace3DTA libQ6MSFR_manager_stub libcamera2ndk_vendor libmediandk libnativewindow libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libface3d_dev.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libFace3D_hlos
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libFace3DTA libQ6MSFR_manager_stub libscveCommon_stub liblog libcdsprpc libutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libFace3D_hlos.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libFace3DTA
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libQSEEComAPI libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libFace3DTA.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastcrc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libfastcrc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastcvdsp_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libfastcvdsp_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastcvopt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libfastcvdsp_stub libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libfastcvopt.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/mm-mux),)

include $(CLEAR_VARS)
LOCAL_MODULE := libFileMux_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libFileMux_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libFlacSwDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libFlacSwDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libgdtap
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libgps.utils liblog libloc_core libizat_core liblbs_core libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libgdtap.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPQTEEC_vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc libcutils libutils liblog libhidlbase libGPTEE_vendor libQTEEConnector_vendor vendor.qti.hardware.qteeconnector@1.0 libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libGPQTEEC_vendor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPreqcancel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libGPreqcancel_svc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libGPreqcancel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPreqcancel_svc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libGPreqcancel_svc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libGPTEE_vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc libcutils libutils liblog libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libGPTEE_vendor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhalide_hexagon_host
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libhalide_hexagon_host.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdcp1prov
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libhdcp1prov.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdcp2p2prov
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libhdcp2p2prov.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdcpsrm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libhdcpsrm.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhdr_tm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libsdm-color libtinyxml2_1 libcutils libutils libdisplayskuutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libhdr_tm.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhta_hexagon_runtime
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libhta_hexagon_runtime.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libI420colorconvert
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libmm-color-convertor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libI420colorconvert.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imscmservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhidlbase com.qualcomm.qti.imscmservice@2.0.vendor lib-imsdpl lib-imsqimf lib-imsxml libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-imscmservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsdpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libqmiservices libcutils libutils liblog libtime_genoff libcrypto libssl libandroid_net libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-imsdpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsqimf
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl lib-imsxml liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-imsqimf.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsrcsbaseimpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware_legacy libhardware libhidlbase libqmiservices libqmi_cci libqti_vndfwk_detect libidl com.qualcomm.qti.imscmservice@2.0.vendor com.qualcomm.qti.imscmservice@2.1.vendor com.qualcomm.qti.imscmservice@2.2.vendor com.qualcomm.qti.uceservice@2.0.vendor com.qualcomm.qti.uceservice@2.1.vendor com.qualcomm.qti.uceservice@2.2.vendor com.qualcomm.qti.uceservice@2.3.vendor vendor.qti.ims.callinfo@1.0 vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.factory@1.1.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-imsrcsbaseimpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsrcs-v2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl lib-imsqimf lib-imsxml lib-siputility libqmi_cci liblog libcutils libbase libutils libhardware_legacy libhardware libhidlbase libqmiservices libqti_vndfwk_detect lib-rcsconfig libidl com.qualcomm.qti.imscmservice@2.0.vendor com.qualcomm.qti.uceservice@2.0.vendor com.qualcomm.qti.uceservice@2.1.vendor com.qualcomm.qti.uceservice@2.2.vendor com.qualcomm.qti.uceservice@2.3.vendor com.qualcomm.qti.imscmservice@2.1.vendor com.qualcomm.qti.imscmservice@2.2.vendor vendor.qti.ims.callinfo@1.0 vendor.qti.ims.rcsconfig@2.0.vendor vendor.qti.ims.rcsconfig@2.1.vendor vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.factory@1.1.vendor vendor.qti.ims.callcapability@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-imsrcs-v2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsvtcore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := librcc libion libhidlbase vendor.qti.imsrtpservice@3.0.vendor vendor.qti.imsrtpservice@3.0-service-Impl libdiag libcutils libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-imsvtcore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-imsxml
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-imsxml.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libipebpsstriping
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils liblog libofflinelog libsync libcdsprpc libqdMetaData libhardware libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libipebpsstriping.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libizat_core
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libloc_core libgps.utils libloc_api_v02 libsqlite liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libizat_core.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libjnihelper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libqmiservices libqmi_cci libcacertclient libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libjnihelper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libkeymasterdeviceutils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion libcutils libQSEEComAPI liblog libcrypto libspcom libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libkeymasterdeviceutils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libkeymasterprovision
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcrypto libkeymasterdeviceutils libkeymasterutils libqcbor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libkeymasterprovision.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liblistensoundmodel2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/liblistensoundmodel2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liblowi_client
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/liblowi_client.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liblowi_wifihal
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblowi_client libcutils libutils liblog libgps.utils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/liblowi_wifihal.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libMapService
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libdl liblog libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libMapService.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libminksocket
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libqrtr libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libminksocket.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libmiracast
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase vendor.qti.hardware.sigma_miracast@1.0.vendor libmmosal liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmiracast.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libmmcamera_faceproc2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libdl libc libm
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmmcamera_faceproc2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmmcamera_faceproc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_SHARED_LIBRARIES := libc++ libdl libc libm
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmmcamera_faceproc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmm-color-convertor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmm-color-convertor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmm-hdcpmgr
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmm-hdcpmgr.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libMpeg4SwEncoder
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libGLESv2 libEGL liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libMpeg4SwEncoder.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libMpeghSwEnc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libMpeghSwEnc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmulawdec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libmulawdec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnative-api
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libminksocket liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libnative-api.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnetmgr_nr_fusion
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdsutils libconfigdb libdiag libcutils liblog libnetmgr libnetmgr_common libqmiservices libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libnetmgr_nr_fusion.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnetmgr_rmnet_ext
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdsutils libconfigdb libdiag libcutils liblog libnetmgr libnetmgr_common librmnetctl libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libnetmgr_rmnet_ext.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnlnetmgr
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdsutils libconfigdb libdiag libcutils liblog libnetmgr libnetmgr_common libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libnlnetmgr.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libnpu
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libnpu.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liboemcrypto
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libQSEEComAPI libhidlbase libcpion libminkdescriptor libcutils libtrustedapploader libdisplayconfig.qti libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/liboemcrypto.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxAlacDecSw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxAlacDecSw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxApeDecSw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxApeDecSw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxDsdDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxDsdDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxEvrcDec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxEvrcDec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxMpeghDecSw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxMpeghDecSw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxMpeghEncSw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libMpeghSwEnc liblog libutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxMpeghEncSw.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxQcelp13Dec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxQcelp13Dec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libOmxVideoDSMode
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libmmosal libutils libOmxCore libwfdcommonutils_proprietary libwfdmmservice_proprietary libbinder libwfdutils_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libOmxVideoDSMode.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libopenvx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libopenvx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libops
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libQSEEComAPI liblog libdrm libhidlbase libdisplayconfig.qti libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libops.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libos
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libos.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libperipheral_client
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbinder libmdmdetect libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libperipheral_client.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libpowercallback
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware_legacy libhardware libhwbinder libbinder libhidlbase libhidltransport libpowercore libtinyxml2 vendor.qti.hardware.perf@2.1 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libpowercallback.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libpowercore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbinder libtinyxml2 vendor.display.config@1.0 libhidlbase libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libpowercore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libpvr
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libpvr.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQ6MSFR_manager_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libQ6MSFR_manager_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcc_file_agent
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libutils libbase libfmq libhidlbase vendor.qti.hardware.qccsyshal@1.0.vendor vendor.qti.hardware.qccsyshal@1.1 vendor.qti.hardware.qccsyshal@1.2 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcc_file_agent.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcc_sdk
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libutils libprotobuf-cpp-full-3.9.1 libbinder libbase libz vendor.qti.hardware.qccsyshal@1.2 libhidlbase libfmq libcrypto libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcc_sdk.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_basecodec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libqcodec2_utils libqcodec2_platform libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcodec2_basecodec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_base
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcodec2_base.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_core
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libqcodec2_utils libqcodec2_platform libqcodec2_basecodec libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcodec2_core.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_platform
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libjsoncpp libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcodec2_platform.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_utils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcodec2_vndk libqcodec2_base libqcodec2_platform libhidlbase android.hardware.graphics.mapper@3.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcodec2_utils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/media),)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcodec2_v4l2codec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libqcodec2_base libqcodec2_platform libqcodec2_utils libqcodec2_basecodec libcodec2_vndk libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcodec2_v4l2codec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libqcrildatactl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libqmi_cci libqmiservices libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqcrildatactl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqisl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqisl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQSEEComAPI
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion libcutils libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libQSEEComAPI.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqseed3
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqseed3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQTEEConnector_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libcutils liblog libQSEEComAPI libxml2 libbinder libcrypto libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libQTEEConnector_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQTEEConnector_vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase liblog libcutils libutils vendor.qti.hardware.qteeconnector@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libQTEEConnector_vendor.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqtikeymaster4
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcrypto libhidlbase libutils libcutils libkeymasterdeviceutils libkeymasterutils libbase libqcbor android.hardware.keymaster@4.0 android.hardware.keymaster@4.1 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqtikeymaster4.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_cam_cdsp_driver_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqvr_cam_cdsp_driver_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvrcamera_client.qti
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libnativewindow libui libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqvrcamera_client.qti.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_cdsp_driver_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqvr_cdsp_driver_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_eyetracking_plugin
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqvr_eyetracking_plugin.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_hostcontroller_plugin
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libqvrservice libhidlbase libnativewindow android.frameworks.sensorservice@1.0 android.hardware.graphics.allocator@4.0 android.hardware.graphics.mapper@4.0 android.hardware.sensors@1.0 android.hardware.sensors@2.0 libc++ libcutils liblog libutils libjsoncpp libprotobuf-cpp-full-3.9.1 libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqvr_hostcontroller_plugin.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvrservice_client.qti
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libdrm libutils libnativewindow libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqvrservice_client.qti.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvrservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libnativewindow libjsoncpp android.hardware.graphics.mapper@4.0 android.hardware.graphics.allocator@4.0 libprotobuf-cpp-full-3.9.1 libhidlbase libhardware libbinder libbase libbinder_ndk vendor.qti.hardware.qxr-V1-ndk_platform android.hardware.sensors@1.0 android.frameworks.sensorservice@1.0 android.hardware.sensors@2.0 libdrm vendor.display.config@2.0 libdisplayconfig.qti libhwbinder libui libfmq libcamera_metadata android.hardware.camera.provider@2.4 android.hardware.camera.device@3.2 android.hardware.camera.device@3.4 android.hardware.camera.common@1.0 libqvrcamera_client.qti android.hardware.thermal@1.0 android.hardware.thermal@2.0 libusbhost libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libqvrservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librcc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libdiag libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/librcc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rcsconfig
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl libqmi_cci libcutils libutils liblog libqmiservices libqmi_csi libdiag libhidlbase libhardware_legacy libqti_vndfwk_detect vendor.qti.ims.rcsconfig@2.0.vendor vendor.qti.ims.rcsconfig@2.1.vendor vendor.qti.ims.factory@1.0.vendor vendor.qti.ims.factory@1.1.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-rcsconfig.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libreverse_rpc_tests_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libadsprpc liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libreverse_rpc_tests_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librmp
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/librmp.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librpmb
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libQSEEComAPI liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/librpmb.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rtpcommon
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libcutils libutils liblog lib-imsdpl libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-rtpcommon.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rtpcore
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libcutils libutils liblog lib-imsdpl lib-imsvtcore lib-rtpcommon libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-rtpcore.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-rtpsl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog lib-imsdpl lib-rtpcore lib-rtpcommon libqti_vndfwk_detect libdiag libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-rtpsl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libScenescape
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdl liblog libm libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libScenescape.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveCommon
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libion libscveCommon_stub libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libscveCommon.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveCommon_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libscveCommon_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectSegmentation
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libfastcvopt libscveCommon libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libscveObjectSegmentation.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectSegmentation_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libscveCommon libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libscveObjectSegmentation_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectTracker
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libfastcvopt libscveCommon libscveObjectSegmentation libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libscveObjectTracker.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectTracker_stub
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libfastcvopt liblog libscveCommon libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libscveObjectTracker_stub.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-colormgr-algo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsdm-colormgr-algo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-color
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbinder libutils libdisplaydebug libtinyxml2_1 libsdmutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsdm-color.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-diag
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdiag libsdmutils libbinder libdisplaydebug libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsdm-diag.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdm-disp-vndapis
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdisplaydebug libqservice libbinder libcutils libutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsdm-disp-vndapis.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsdmextension
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdisplaydebug libsdmutils libdisplayqos libsdm-color libdisplayskuutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsdmextension.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libSecureUILib
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libStDrvInt libQSEEComAPI libsecureui_svcsock libhidlbase libbinder libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libSecureUILib.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsecureui
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libQSEEComAPI libStDrvInt libhidlbase libbinder libdisplayconfig.qti libion libsecureui_svcsock vendor.display.config@1.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsecureui.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsensor_calibration
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsensor_calibration.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-siputility
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := lib-imsdpl lib-imsxml lib-imsqimf liblog libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-siputility.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsi
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libpng libQSEEComAPI libtrustedapploader libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsi.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libskewknob
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc libcutils libc++ libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libskewknob.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libspcom
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libion libutils libcutils liblog libcrypto libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libspcom.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libspl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libspl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libssc_default_listener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libsdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libssc_default_listener.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libssd
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils libdiag liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libssd.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libStereoRectifyWrapper
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libdl liblog libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libStereoRectifyWrapper.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libstreamparser
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libvideoutils liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libstreamparser.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsurround_3mic_proc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsurround_3mic_proc.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libswregistrationalgo
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libz libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libswregistrationalgo.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libswvdec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libcdsprpc liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libswvdec.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsynx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libos libthreadutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsynx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libsysmon_cdsp_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libsysmon_cdsp_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libthreadutils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libos libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libthreadutils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtinyxml2_1
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libtinyxml2_1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtracker_6dof_impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcdsprpc libdl libc
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libtracker_6dof_impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtrustedapploader
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libminkdescriptor libminksocket libqcbor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libtrustedapploader.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtzdrmgenprov
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libQSEEComAPI libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libtzdrmgenprov.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lib-uceservice
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhidlbase lib-imsdpl lib-imsqimf lib-imsxml lib-siputility libqti_vndfwk_detect lib-imscmservice com.qualcomm.qti.imscmservice@2.0.vendor com.qualcomm.qti.uceservice@2.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/lib-uceservice.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvideooptfeature
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libbinder libtinyxml2 libpowercore libdisplayconfig.qti libhidlbase libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libvideooptfeature.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvideoutils
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libvideoutils.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvpphcp
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libvpphcp.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvpphvx
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libcdsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libvpphvx.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libvppimmotion
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libadsprpc libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libvppimmotion.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdcodecv4l2_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libwfdcommonutils_proprietary libwfdmminterface_proprietary libwfdutils_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdcodecv4l2_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdcommonutils_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libion libprocessgroup libwfddisplayconfig_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdcommonutils_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdhdcpservice_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libbinder libwfdhdcpcp libc++ libclang_rt.ubsan_standalone-arm-android libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdhdcpservice_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdmminterface_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdmminterface_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdmmservice_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libcutils libbinder libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdmmservice_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdmmsrc_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder libmmosal liblog libutils libmmrtpencoder_proprietary libFileMux_proprietary libcutils libtinyalsa libwfdcommonutils_proprietary libwfdutils_proprietary libwfdconfigutils_proprietary libwfdaac_vendor libpng libwfdmminterface_proprietary libwfdcodecv4l2_proprietary libwfdmmservice_proprietary libhidlbase android.hardware.graphics.mapper@2.0 libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdmmsrc_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdmodulehdcpsession
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libwfdhdcpcp libwfdcommonutils_proprietary libhidlbase libwfdutils_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdmodulehdcpsession.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdsessionmodule
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libutils libwfdsourcesession_proprietary vendor.qti.hardware.wifidisplaysession@1.0.vendor libwfdcommonutils_proprietary libhidlbase libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdsessionmodule.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdsourcesession_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libbinder libmmosal libwfdcommonutils_proprietary libwfdconfigutils_proprietary libwfdmminterface_proprietary libmedia_helper libwfdmmsrc_proprietary libqdMetaData libhidlbase libwfdsourcesm_proprietary libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdsourcesession_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdsourcesm_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libc++ libcutils libutils libwfdrtsp_proprietary libmmrtpencoder_proprietary libmmrtpdecoder_proprietary libwfduibcinterface_proprietary libwfdmmsrc_proprietary libmmosal libwfdcommonutils_proprietary libwfdconfigutils_proprietary libqdMetaData libwfdmminterface_proprietary libwfdmodulehdcpsession libhidlbase libwfddisplayconfig_proprietary liblog libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdsourcesm_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := libwfdutils_proprietary
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libmmosal liblog libion libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwfdutils_proprietary.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libwms
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcne libqmiservices libqmi_cci liblog libcneoplookup libandroid_net libutils vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.latency@1.0 vendor.qti.latency@2.0.vendor vendor.qti.hardware.mwqemadapter@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwms.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libwqe
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libbinder libcutils libnetutils libutils liblog libsqlite libcneapiclient libcne libnl libxml libxml2 libandroid_net libqti_vndfwk_detect vendor.qti.hardware.data.cne.internal.server@1.0.vendor vendor.qti.hardware.data.cne.internal.api@1.0.vendor vendor.qti.hardware.mwqemadapter@1.0.vendor libwpa_client libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libwqe.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxml
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libdiag libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libxml.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxtadapter
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libgps.utils liblog libloc_core libizat_core libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libxtadapter.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxtwifi_server_protocol
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils libasn1cper libasn1crt libasn1crtx libloc_core libgps.utils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libxtwifi_server_protocol.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libxtwifi_server_protocol_uri_v3
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libgps.utils libizat_core libasn1cper libasn1crt libasn1crtx libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/libxtwifi_server_protocol_uri_v3.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libapps_mem_heap
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libapps_mem_heap.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libbitml_nsp_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libbitml_nsp_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamera_nn_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libcamera_nn_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libcvpdsp_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libcvpdsp_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdspCV_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libdspCV_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libdsp_streamer_qvrcam_receiver
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libdsp_streamer_qvrcam_receiver.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libeye_tracking_dsp_sample_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libeye_tracking_dsp_sample_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastcvadsp
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libfastcvadsp.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastcvdsp_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libfastcvdsp_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhalide_hexagon_remote_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libhalide_hexagon_remote_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhexagon_nn_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libhexagon_nn_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libQ6MSFR_manager_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libQ6MSFR_manager_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_3dof_tracker_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libqvr_3dof_tracker_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_cam_dsp_driver_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libqvr_cam_dsp_driver_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqvr_dsp_driver_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libqvr_dsp_driver_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveObjectSegmentation_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libscveObjectSegmentation_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libscveT2T_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libscveT2T_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtracker_6dof_skel_1130
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libtracker_6dof_skel_1130.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtracker_6dof_skel_8250
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libtracker_6dof_skel_8250.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtracker_6dof_skel_8350
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libtracker_6dof_skel_8350.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtracker_6dof_skel_8450
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES :=
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/libtracker_6dof_skel_8450.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libbenchmark_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libbenchmark_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastrpc_tests1_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libfastrpc_tests1_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libfastrpc_tests_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libfastrpc_tests_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libhap_power_test_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libhap_power_test_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librpcperf1_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/librpcperf1_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := librpcperf_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/librpcperf_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtest_dl1_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libtest_dl1_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtest_dl_perf1_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libtest_dl_perf1_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtest_dl_perf_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libtest_dl_perf_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libtest_dl_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libtest_dl_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libunsigned_pd_test_skel
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/libunsigned_pd_test_skel.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_dep1
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_dep1.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_dep2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_dep2.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_nop
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_nop.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_128k
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_128k.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_16k
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_16k.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_1MB
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_1MB.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_256k
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_256k.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_32k
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_32k.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_512k
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_512k.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_64k
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_64k.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := test_elf_size_768k
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/rfsa/adsp/tests/test_elf_size_768k.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/rfsa/adsp/tests
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libasphere
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/soundfx/libasphere.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcbassboost
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/soundfx/libqcbassboost.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcreverb
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/soundfx/libqcreverb.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libqcvirt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/soundfx/libqcvirt.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libshoebox
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libcutils liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/soundfx/libshoebox.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib/soundfx
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.qccvndhal@1.0-halimpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libfmq libcutils liblog vendor.qti.hardware.qccvndhal@1.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.qccvndhal@1.0-halimpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

ifeq ($(wildcard vendor/qcom/proprietary/wfd-framework),)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.wifidisplaysessionl@1.0-halimpl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libhidlbase libutils libcutils vendor.qti.hardware.wifidisplaysession@1.0.vendor libwfdsessionmodule liblog libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.hardware.wifidisplaysessionl@1.0-halimpl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.imsrtpservice@3.0-service-Impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libutils libhardware_legacy libhardware libhidlbase libdiag vendor.qti.imsrtpservice@3.0.vendor libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.imsrtpservice@3.0-service-Impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.qspmhal@1.0-impl
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SHARED_LIBRARIES := libutils vendor.qti.qspmhal@1.0 android.hidl.allocator@1.0 android.hidl.memory@1.0 libhidlbase libhidlmemory liblog libcutils libc++ libc libm libdl
LOCAL_MODULE_SUFFIX := .so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ../../.././target/product/kona/vendor/lib/vendor.qti.qspmhal@1.0-impl.so
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/lib
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
LOCAL_PATH := $(PREBUILT_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V2.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V2.1-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V2.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V2.1-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V2.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V2.2-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V2.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V2.2-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcsuce-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcsuce-V1.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcsuce-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcsuce-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcsuce-V1.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcsuce-V1.1-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcsuce-V1.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcsuce-V1.1-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcsuce-V1.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcsuce-V1.2-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcsuce-V1.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcsuce-V1.2-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcssip-V1.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcssip-V1.2-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcssip-V1.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcssip-V1.2-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.mwqemadapter-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.mwqemadapter-V1.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.mwqemadapter-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.mwqemadapter-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.api-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.api-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.api-V1.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.api-V1.1-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.api-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.data.cne.internal.api-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.server-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.server-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.server-V2.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.server-V2.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.server-V2.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.server-V2.1-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.connection-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.connection-V1.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.connection-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.connection-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcssip-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcssip-V1.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcssip-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcssip-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcssip-V1.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcssip-V1.1-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.rcssip-V1.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.rcssip-V1.1-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.constants-V2.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.constants-V2.1-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.constants-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.data.cne.internal.constants-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.data.factory-V1.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.data.factory-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory-V2.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.data.factory-V2.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory-V2.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.data.factory-V2.1-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory-V2.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.data.factory-V2.2-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.data.factory-V2.3-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.data.factory-V2.3-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.qmi-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.data.qmi-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V1.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V1.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V1.1-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V1.1-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V1.1-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V2.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V2.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.ims.factory-V2.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.ims.factory-V2.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := izat.xt.srv
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/izat.xt.srv_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.qti.dpmframework
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.qti.dpmframework_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := dpmapi
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/dpmapi_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := dpmapi
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/dpmapi_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := tcmclient
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/tcmclient_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.slmadapter-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.slmadapter-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.slmadapter-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.slmadapter-V1.0-java_intermediates/classes.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.server-V2.2-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.server-V2.2-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.data.cne.internal.server-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/vendor.qti.hardware.data.cne.internal.server-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.constants-V1.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.constants-V1.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := com.quicinc.cne.constants-V2.0-java
LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_SUFFIX := $(COMMON_JAVA_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := ../../.././target/common/obj/JAVA_LIBRARIES/com.quicinc.cne.constants-V2.0-java_intermediates/classes-header.jar
LOCAL_UNINSTALLABLE_MODULE := true
ifeq ($(MODULE.TARGET.$(LOCAL_MODULE_CLASS).$(LOCAL_MODULE)),)
include $(BUILD_PREBUILT)
endif

$(shell mkdir -p $(PRODUCT_OUT)/vendor/app/CneApp/lib/arm64//vendor/lib64 && pushd $(PRODUCT_OUT)/vendor/app/CneApp/lib/arm64 > /dev/null && ln -s /vendor/lib64/libvndfwk_detect_jni.qti.so libvndfwk_detect_jni.qti.so && popd > /dev/null)
# create symbolic links for qvrservice_config file
$(shell mkdir -p $(PRODUCT_OUT)/vendor/etc/qvr)
$(shell ln -sf /mnt/vendor/persist/qvr/qvrservice_config.txt $(PRODUCT_OUT)/vendor/etc/qvr/qvrservice_config.txt)
