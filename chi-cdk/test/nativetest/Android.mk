ifeq ($(CAMX_TDEV_PATH),)
NATIVETEST_PATH := $(abspath $(call my-dir))
else
NATIVETEST_PATH := $(CAMX_TDEV_PATH)/nativetest
endif

#Defining shared libraries
NATIVETEST_SHARED_LIBRARIES :=              \
    libutils                                \
    libc++                                  \
    libcutils                               \
    libhardware                             \
    libcamera_metadata                      \
    liblog

# Take backup of sdllvm lto flag and common defs as other modules
# also maintain their own version of it.
OLD_SDCLANG_FLAG_DEFS    := $(SDCLANG_FLAG_DEFS)
OLD_SDCLANG_VERSION_DEFS := $(SDCLANG_VERSION_DEFS)

#Defining common c flags when not doing a VGDB build
NATIVETEST_C_FLAGS += -Wall -Wextra -DOS_ANDROID -DCAMX_ANDROID_API=28

#Needed for some of the camx definition includes
NATIVETEST_C_FLAGS += -D_LINUX

#Default is camx, use DRIVER=legacy flag to compile for legacy driver
ifeq ($(DRIVER), legacy)
    $(info Defining MACRO for: $(DRIVER))
    NATIVETEST_C_FLAGS += -DLEGACY
endif

ifeq ($(ENABLE3A), true)
    $(info Defining MACRO for enabling 3A routines)
    NATIVETEST_C_FLAGS += -DENABLE3A
endif

ifeq ($(METADATA), old)
    $(info Defining MACRO for using old metadata API)
    NATIVETEST_C_FLAGS += -DOLD_METADATA
endif

NCHI_TITAN_TOPOLOGY        := $(NATIVETEST_PATH)/assets/topology/chitopologies/nchi_titan_topology.xml
NCHI_MIMAS_TOPOLOGY        := $(NATIVETEST_PATH)/assets/topology/chitopologies/nchi_mimas_topology.xml

#Defining targets
#GTEST_LEGACY should be used for Hana and previous platforms
ifeq ($(TARGET_PRODUCT), kona)
    $(info Defining MACRO for TARGET_PRODUCT Kona)
    NATIVETEST_C_FLAGS += -DTARGET_KONA
endif

ifeq ($(TARGET_PRODUCT), msmnile)
    $(info Defining MACRO for TARGET_PRODUCT Hana)
    NATIVETEST_C_FLAGS += -DTARGET_HANA -DGTEST_LEGACY
endif

ifeq ($(TARGET_PRODUCT), sm6150)
    $(info Defining MACRO for TARGET_PRODUCT Talos/Moorea)
    NATIVETEST_C_FLAGS += -DTARGET_TALOSMOOREA -DGTEST_LEGACY
endif

ifeq ($(TARGET_PRODUCT), sdm845)
    $(info Defining MACRO for TARGET_PRODUCT Napali)
    NATIVETEST_C_FLAGS += -DTARGET_NAPALI -DGTEST_LEGACY
endif

# Defining Android platform
ifeq ($(ANDROID_FLAVOR), $(ANDROID_FLAVOR_Q))
    LOCAL_CFLAGS += -DPLATFORM_VERSION_Q
    NATIVETEST_C_FLAGS += -DPLATFORM_VERSION_Q
endif

ifeq ($(ANDROID_FLAVOR), $(ANDROID_FLAVOR_R))
    LOCAL_CFLAGS += -DPLATFORM_VERSION_R
    NATIVETEST_C_FLAGS += -DPLATFORM_VERSION_R
endif

NCHI_PIPELINE_HEADER := $(NATIVETEST_PATH)/nativechitest/g_testpipelines.h

NCHI_CONVERTER_CMD   := perl $(CAMX_CHICDK_PATH)/tools/usecaseconverter/usecaseconverter.pl $(NCHI_TITAN_TOPOLOGY) $(NCHI_MIMAS_TOPOLOGY) $(NCHI_PIPELINE_HEADER)

$(info INFO: Command beind used to generate native chi header: $(NCHI_CONVERTER_CMD))

# Run UsecaseConverter to generate g_pipelines.h
NCHI_CONVERTER_STATUS := $(shell $(NCHI_CONVERTER_CMD)|| echo [NativeChiUsecaseConverterFailure])
$(info $(NCHI_CONVERTER_STATUS))
ifeq ($(lastword $(NCHI_CONVERTER_STATUS)),[NativeChiUsecaseConverterFailure])
    # Stop the build when error status code detected
    $(error $(NCHI_PIPELINE_HEADER) was not generated)
endif

# Build native tests
include $(NATIVETEST_PATH)/nativechitest/common/build/android/Android.mk
include $(NATIVETEST_PATH)/nativehaltest/common/build/android/Android.mk

# Restore previous value of sdllvm flag and version defs
SDCLANG_FLAG_DEFS    := $(OLD_SDCLANG_FLAG_DEFS)
SDCLANG_VERSION_DEFS := $(OLD_SDCLANG_VERSION_DEFS)
