# Determine whether to build updatable drivers based on product name and api version
# and create package name for product variants.

# Using target board platform as product name starting from Android R
ifeq ($(call CHECK_VERSION_GE, $(ADRENO_PLATFORM_SDK_VERSION), $(PLATFORM_SDK_RPDK)),true)
    ADRENO_USE_UGD := lito
    ADRENO_USE_UGD += sdm855 msmnile
    ADRENO_USE_UGD += kona
    ADRENO_USE_UGD += lahaina
    ADRENO_USE_UGD += sm6150
    ifeq ($(call is-board-platform-in-list, $(ADRENO_USE_UGD)),true)
        ADRENO_PRODUCT_NAME := $(TARGET_BOARD_PLATFORM)
    endif
else
    # Chip names are decoded from chip list in ip catalog
    ifeq ($(call is-board-platform-in-list,lito),true)
        ADRENO_PRODUCT_NAME := sm7250
        else ifeq ($(call is-board-platform-in-list,sdm855 msmnile),true)
        ADRENO_PRODUCT_NAME := sm8150
        else ifeq ($(call is-board-platform-in-list,kona),true)
        ADRENO_PRODUCT_NAME := sm8250
        else ifeq ($(call is-board-platform-in-list,lahaina),true)
        ADRENO_PRODUCT_NAME := sm8350
        else ifeq ($(call is-board-platform-in-list,bengal),true)
        ADRENO_PRODUCT_NAME := $(TARGET_BOARD_PLATFORM)
    endif
endif

ADRENO_SDK_VERSION := $(ADRENO_PLATFORM_SDK_VERSION)
ifneq ($(ADRENO_BUILD_EMPTY_DRIVER),)
    # Optionally override product name and sdk version when manually building an empty driver apk
    ifneq ($(ADRENO_BUILD_EMPTY_DRIVER_PRODUCT_NAME),)
        ADRENO_PRODUCT_NAME := $(ADRENO_BUILD_EMPTY_DRIVER_PRODUCT_NAME)
    endif
    ifneq ($(ADRENO_BUILD_EMPTY_DRIVER_SDK_VERSION),)
        ADRENO_SDK_VERSION := $(ADRENO_BUILD_EMPTY_DRIVER_SDK_VERSION)
    endif
    ADRENO_BUILD_UPDATABLE_DRIVERS := 1
endif

ifneq ($(ADRENO_PRODUCT_NAME),)
ifeq ($(ADRENO_PLATFORM_SDK_VERSION),)
    $(error Project must define ADRENO_PLATFORM_SDK_VERSION for package name)
endif

# List of targets that supports updatable driver
ADRENO_UPDATABLE_MINAPI28_TARGETS := msmnile
ifeq ($(call is-board-platform-in-list,$(ADRENO_UPDATABLE_MINAPI28_TARGETS)),true)
    ifeq ($(ADRENO_PLATFORM_SDK_VERSION), $(PLATFORM_SDK_PPDK))
        ADRENO_BUILD_UPDATABLE_DRIVERS := 1
    endif
endif
ADRENO_UPDATABLE_MINAPI29_TARGETS := msmnile kona lito bengal
ifeq ($(call is-board-platform-in-list,$(ADRENO_UPDATABLE_MINAPI29_TARGETS)),true)
    ifeq ($(ADRENO_PLATFORM_SDK_VERSION), $(PLATFORM_SDK_QPDK))
        ADRENO_BUILD_UPDATABLE_DRIVERS := 1
    endif
endif
ADRENO_UPDATABLE_MINAPI30_TARGETS := kona lito lahaina msmnile sm6150
ifeq ($(call is-board-platform-in-list,$(ADRENO_UPDATABLE_MINAPI30_TARGETS)),true)
    ifeq ($(ADRENO_PLATFORM_SDK_VERSION), $(PLATFORM_SDK_RPDK))
        ADRENO_BUILD_UPDATABLE_DRIVERS := 1
    endif
endif
ADRENO_DRIVER_APPLICATION_ID := com.qualcomm.qti.gpudrivers

# Define product variants by appending product name and sdk version.
ADRENO_DRIVER_APPLICATION_ID_SUFFIX := $(ADRENO_PRODUCT_NAME).api$(ADRENO_SDK_VERSION)
ADRENO_DRIVER_PACKAGE_NAME := $(ADRENO_DRIVER_APPLICATION_ID).$(ADRENO_DRIVER_APPLICATION_ID_SUFFIX)

ifneq ($(ADRENO_USING_LEGACY_PACKAGE_NAME),)
# Using legacy package name
ADRENO_DRIVER_PACKAGE_NAME := com.qualcomm.qti.adrenographicsdrivers
endif

# Package name is used in apk naming scheme. Signed empty driver is prebuilt in build image and unsigned updatable driver will be generated.
ADRENO_EMPTY_DRIVER_NAME := $(ADRENO_DRIVER_PACKAGE_NAME)
ADRENO_UPDATABLE_DRIVER_NAME := $(ADRENO_DRIVER_PACKAGE_NAME).unsigned
endif # ($(ADRENO_PRODUCT_NAME),)
