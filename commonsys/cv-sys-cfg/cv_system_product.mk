
#CVP LIBRARIES
CVP_SYSTEM := libcvp_system
CVP_SYSTEM += libcvp_system.so
CVP_SYSTEM += libcvp_stub_system
CVP_SYSTEM += libcvp_stub_system.so
CVP_SYSTEM += libcvp_common_system
CVP_SYSTEM += libcvp_common_system.so
CVP_SYSTEM += libcvpcpuRev_skel_system
CVP_SYSTEM += libcvpcpuRev_skel_system.so
CVP_SYSTEM += vendor.qti.hardware.cvp@1.0-adapter-helper
CVP_SYSTEM += vendor.qti.hardware.cvp@1.0
CVP_SYSTEM += libcvp_hidl

PRODUCT_PACKAGES += $(CVP_SYSTEM)

SCVE_TESTS_DEBUG := 3DScanner
SCVE_TESTS_DEBUG += CleverCaptureDemo
SCVE_TESTS_DEBUG += ImageStudioRef
SCVE_TESTS_DEBUG += ObjectTrackerRef
SCVE_TESTS_DEBUG += PanoramaRef
SCVE_TESTS_DEBUG += SocialCamera
SCVE_TESTS_DEBUG += STA
SCVE_TESTS_DEBUG += Touch2Track
SCVE_TESTS_DEBUG += VideoSummaryRef
PRODUCT_PACKAGES_DEBUG += $(SCVE_TESTS_DEBUG)

#INTERNAL ONLY SCVE APK COMPILATION
SCVE_APK_ENABLE := true

