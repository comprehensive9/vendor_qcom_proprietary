PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so \
    ro.vendor.perf-hal.ver=2.2

SOONG_CONFIG_NAMESPACES += perf
SOONG_CONFIG_perf += QMAA_ENABLE_PERF_STUB_HALS


PRODUCT_PACKAGES += \
    vendor.qti.hardware.perf@2.2 \
    vendor.qti.hardware.perf@2.2-service.rc \
    vendor.qti.hardware.perf@2.2-service

#IOP Feature
ifneq ($(call is-board-platform-in-list, msm8937 msm8953 msm8998 qcs605 sdm660 sdm710 monaco ),true)
PRODUCT_PACKAGES += \
    vendor.qti.hardware.iop@2.0 \
    vendor.qti.hardware.iop@2.0-service.rc \
    vendor.qti.hardware.iop@2.0-service
endif

ifeq ($(TARGET_DISABLE_PERF_OPTIMIZATIONS),true)
SOONG_CONFIG_perf_QMAA_ENABLE_PERF_STUB_HALS := true
endif

ifneq ($(TARGET_DISABLE_PERF_OPTIMIZATIONS),true)
SOONG_CONFIG_perf_QMAA_ENABLE_PERF_STUB_HALS := false
# QC_PROP_ROOT is already set when we reach here:
# PATH for reference: QC_PROP_ROOT := vendor/qcom/proprietary
$(call inherit-product-if-exists, $(QC_PROP_ROOT)/android-perf/profiles.mk)

PRODUCT_PACKAGES += \
    libqti-perfd \
    libqti-perfd-client \
    libqti-util \
    android.hardware.tests.libhwbinder@1.0-impl \
    libperfgluelayer \
    libperfconfig


#IOP Feature
ifneq ($(call is-board-platform-in-list, msm8937 msm8953 msm8998 qcs605 sdm660 sdm710 monaco ),true)
PRODUCT_PACKAGES += \
    libqti-iopd \
    libqti-iopd-client
endif

#LM Feature
ifneq ($(call is-board-platform-in-list, msm8953 sdm660 sdm845 monaco ),true)
PRODUCT_PACKAGES += \
    liblearningmodule \
    libmeters \
    libreffeature \
    ReferenceFeature.xml \
    AdaptLaunchFeature.xml \
    AppClassifierFeature.xml \
    GameOptimizationFeature.xml
endif

# For Pixel Targets.
# ODM partition will be created and below rc files will go to ODM partition in pixel targets to disable the perf and IOP services.
ifeq ($(GENERIC_ODM_IMAGE),true)
PRODUCT_PACKAGES += \
    init.pixel.vendor.qti.hardware.perf@2.2-service.rc \
    init.pixel.vendor.qti.hardware.iop@2.0-service.rc
endif

PRODUCT_PACKAGES_DEBUG += \
    perflock_native_test \
    perflock_native_stress_test \
    perfunittests \
    boostConfigParser \
    libqti-tests-mod1 \
    libqti-tests-mod2 \
    libqti-tests-mod3 \
    libqti-tests-mod4 \
    libqti-tests-mod5 \
    libqti-tests-mod6

# Pre-render feature
ifeq ($(call is-board-platform-in-list, lahaina shima holi yupik bengal ),true)
PRODUCT_PROPERTY_OVERRIDES += \
ro.vendor.perf.scroll_opt=1
endif

endif # TARGET_DISABLE_PERF_OPTIMIZATIONS is false
