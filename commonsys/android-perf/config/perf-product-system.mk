ifneq ($(TARGET_BOARD_AUTO),true)
# Preloading QPerformance jar to ensure faster perflocks in Boost Framework
PRODUCT_BOOT_JARS += QPerformance
# Preloading UxPerformance jar to ensure faster UX invoke in Boost Framework
PRODUCT_BOOT_JARS += UxPerformance
endif

PRODUCT_PACKAGES += \
    perfservice \
    vendor.qti.hardware.perf@2.0.vendor \
    vendor.qti.hardware.perf@2.1.vendor \
    vendor.qti.hardware.perf@2.2.vendor \

ifneq ($(TARGET_BOARD_AUTO),true)
PRODUCT_PACKAGES += \
    UxPerformance \
    QPerformance \
    libqti_performance \
    libqti-at \
    libqti-perfd-client_system \
    libqti-util_system \
    libqti-iopd-client_system
endif

#Below libs are needed by WorkloadClassifier and are specific to HY11 Builds
ifneq ($(TARGET_BOARD_AUTO),true)
ifneq ($(TARGET_HAS_LOW_RAM),true)
PRODUCT_PACKAGES += \
    libtflite \
    libtextclassifier_hash \
    libtflite_context \
    libtflite_framework \
    libtflite_kernels \
    libtextclassifier \
    libtextclassifier_hash_defaults \
    libc++_static
endif
endif
