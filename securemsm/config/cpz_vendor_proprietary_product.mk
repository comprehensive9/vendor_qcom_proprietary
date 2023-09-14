ifeq ($(strip $(ENABLE_SECCAM)),true)
#No seccam lib/bin in /vendor yet (except for hal service)
ifeq ($(strip $(ENABLE_SECCAM_QTI_SERVICE)),true)
SECUREMSM_SECCAM := vendor.qti.hardware.seccam@1.0.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.seccam@1.0_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.seccam@1.0-service-qti
SECUREMSM_SECCAM += vendor.qti.hardware.seccam@1.0-service-qti.rc
endif
endif

ifeq ($(strip $(ENABLE_SECURE_UI)),true)
SECUREMSM_SECDISP := libSecureUILib
SECUREMSM_SECDISP += libsi
SECUREMSM_SECDISP += libStDrvInt
SECUREMSM_SECDISP += secure_ui_sample_client
SECUREMSM_SECDISP += libsecureui_svcsock
SECUREMSM_SECDISP += libsecureui
SECUREMSM_SECDISP += vendor.qti.hardware.tui_comm@1.0.vendor
SECUREMSM_SECDISP += vendor.qti.hardware.tui_comm@1.0_vendor
SECUREMSM_SECDISP += vendor.qti.hardware.tui_comm@1.0-impl-qti
SECUREMSM_SECDISP += vendor.qti.hardware.tui_comm@1.0-service-qti
SECUREMSM_SECDISP += vendor.qti.hardware.tui_comm@1.0-service-qti.rc
SECUREMSM_SECDISP += vendor.qti.hardware.qdutils_disp@1.0.vendor
SECUREMSM_SECDISP += vendor.qti.hardware.qdutils_disp@1.0_vendor
SECUREMSM_SECDISP += vendor.qti.hardware.qdutils_disp@1.0-impl-qti
SECUREMSM_SECDISP += vendor.qti.hardware.qdutils_disp@1.0-service-qti
SECUREMSM_SECDISP += vendor.qti.hardware.qdutils_disp@1.0-service-qti.rc
endif

ifeq ($(strip $(ENABLE_TRUSTED_UI_2_0)),true)
SECUREMSM_SECDISP += vendor.qti.hardware.systemhelper@1.0
SECUREMSM_SECDISP += vendor.qti.hardware.systemhelper@1.0.vendor
SECUREMSM_SECDISP += vendor.qti.hardware.systemhelper@1.0_vendor
SECUREMSM_SECDISP += vendor.qti.hardware.systemhelper@1.0_32
SECUREMSM_SECDISP += libTrustedInput
SECUREMSM_SECDISP += TrustedUISampleTest
SECUREMSM_SECDISP += libTrustedUI
SECUREMSM_SECDISP += libTrustedUITZ
SECUREMSM_SECDISP += libTrustedInputTZ
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui@1.0-service-qti
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui@1.0-service-qti.rc
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui@1.0
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui@1.1
SECUREMSM_SECDISP += vendor.qti.hardware.trustedui@1.2
endif

ifeq ($(strip $(ENABLE_TRUSTED_UI_VM_3_0)),true)
SECUREMSM_SECDISP += libTrustedUIVM
SECUREMSM_SECDISP += libTouchInputVM
endif

ifeq ($(strip $(ENABLE_SECCAM_2_0)),true)
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config@1.0.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config@1.0.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.config@1.0.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device@1.0.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device@1.0.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.device@1.0.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0-helper.so
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0-helper.so.vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor.common@1.0-helper.so_vendor
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor@1.0
SECUREMSM_SECCAM += vendor.qti.hardware.secureprocessor@1.0.rc
endif

PRODUCT_PACKAGES += $(SECUREMSM_SECCAM)
PRODUCT_PACKAGES += $(SECUREMSM_SECDISP)

ifeq ($(strip $(ENABLE_WIDEVINE_DRM)), true)
SECUREMSM_WIDEVINE_DRM := liboemcrypto
SECUREMSM_WIDEVINE_DRM += libcppf
SECUREMSM_WIDEVINE_DRM += libcpion
SECUREMSM_WIDEVINE_DRM += libtrustedapploader
SECUREMSM_WIDEVINE_DRM += libminkdescriptor
SECUREMSM_WIDEVINE_DRM += libdrmfs
SECUREMSM_WIDEVINE_DRM += libdrmtime
SECUREMSM_WIDEVINE_DRM += libops
SECUREMSM_WIDEVINE_DRM += libhdcpsrm
SECUREMSM_WIDEVINE_DRM += libhdcp1prov
SECUREMSM_WIDEVINE_DRM += libops
SECUREMSM_WIDEVINE_DRM += libhdcp2p2prov
SECUREMSM_WIDEVINE_DRM += StoreKeybox
SECUREMSM_WIDEVINE_DRM += InstallKeybox
SECUREMSM_WIDEVINE_DRM += hdcp2p2prov
SECUREMSM_WIDEVINE_DRM += hdcp_srm
SECUREMSM_WIDEVINE_DRM += hdcp1prov

# Don't use dynamic DRM HAL for non-go SPs
# Add static and dynamic android.hardware.drm@1.3-service.widevine
# & android.hardware.drm@1.3-service.clearkey to PRODUCT_PACKAGES.
# These static and dynamic (lazy) services are needed for complete
# Widevine DRM use case on full Android & Go devices.
ifneq ($(TARGET_HAS_LOW_RAM),true)
SECUREMSM_WIDEVINE_DRM += android.hardware.drm@1.3-service.widevine
SECUREMSM_WIDEVINE_DRM += android.hardware.drm@1.3-service.clearkey
else
SECUREMSM_WIDEVINE_DRM += android.hardware.drm@1.3-service-lazy.widevine
SECUREMSM_WIDEVINE_DRM += android.hardware.drm@1.3-service-lazy.clearkey
endif

ifeq ($(strip $(OTA_FLAG_FOR_DRM)),true)
SECUREMSM_WIDEVINE_DRM += move_widevine_data.sh
endif

#
# Drminitialization is invoked during setDataSource and as a part
# initialization, DrmManagerClient is created which tries to get
# DrmManagerService(“drm.drmManager”).
# Earlier, if the service is not yet started & if the property
# “drm.service.enabled” is not set, then getService call returns NULL
# immediately and client creates a No-op-DrmClientManager.
# But now, AOSP change has disabled this service by default and it will
# only be started if the property  drm.service.enabled is set.
# Hence setting this property in vendor shippable config.mk
#
# Exclude wearable SPs as they don't need WV L1 enabled
#
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled = true
endif

#ENABLE_WIDEVINE_DRM
endif

PRODUCT_PACKAGES += $(SECUREMSM_WIDEVINE_DRM)

ifeq ($(strip $(ENABLE_PLAYREADY_DRM)), true)
SECUREMSM_PLAYREADY_DRM := drmtest
SECUREMSM_PLAYREADY_DRM += libdrmfs
SECUREMSM_PLAYREADY_DRM += libdrmMinimalfs
SECUREMSM_PLAYREADY_DRM += libdrmtime
SECUREMSM_PLAYREADY_DRM += libbase64
SECUREMSM_PLAYREADY_DRM += libprpk3
SECUREMSM_PLAYREADY_DRM += libprhidl
SECUREMSM_PLAYREADY_DRM += libprdrmengine
SECUREMSM_PLAYREADY_DRM += android.hardware.drm@1.3-service.playready-qti
SECUREMSM_PLAYREADY_DRM += android.hardware.drm@1.3-service.playready-qti.rc

SECUREMSM_PLAYREADY_DRM += prdrmkeyprov
SECUREMSM_PLAYREADY_DRM_DEBUG += prdrm_gtest
endif

PRODUCT_PACKAGES += $(SECUREMSM_PLAYREADY_DRM)
PRODUCT_PACKAGES_DEBUG += $(SECUREMSM_PLAYREADY_DRM_DEBUG)
