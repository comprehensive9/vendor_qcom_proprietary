#Add system entries from base.mk
MM_WFD_BASE_SYSTEM := libwfdaac
PRODUCT_PACKAGES += $(MM_WFD_BASE_SYSTEM)

WFD_PREBUILT_HY22_STATS_PATH   := $(TOP)/vendor/qcom/proprietary/prebuilt_HY22
WFD_HAS_PREBUILT_PATH := false
WFD_HAS_PREBUILT_PATH := $(shell if [[ -d $(WFD_PREBUILT_HY22_STATS_PATH) ]] ; then echo true; fi)


#Add system entries from target.mk
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
ifneq ($(strip $(QCPATH)),)
ifneq ($(WFD_HAS_PREBUILT_PATH),true)
    PRODUCT_BOOT_JARS += WfdCommon
endif
endif
endif

MM_RTP_SYSTEM := libmmrtpdecoder
MM_RTP_SYSTEM += libmmrtpencoder
PRODUCT_PACKAGES += $(MM_RTP_SYSTEM)
ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
WFD_SYSTEM := libwfdavenhancements
WFD_SYSTEM += WfdCommon
WFD_SYSTEM += WfdService
WFD_SYSTEM += libwfdnative
WFD_SYSTEM += libwfdconfigutils
WFD_SYSTEM += wfdconfigsink.xml
WFD_SYSTEM += libmiracast
WFD_SYSTEM += libwfdsinksm
WFD_SYSTEM += libwfdmmsink
WFD_SYSTEM += libwfdmmsrc_system
WFD_SYSTEM += libwfdclient
WFD_SYSTEM += libwfdservice
WFD_SYSTEM += wfdservice
WFD_SYSTEM += libwfduibcinterface
WFD_SYSTEM += libwfduibcsinkinterface
WFD_SYSTEM += libwfduibcsink
WFD_SYSTEM += libwfduibcsrcinterface
WFD_SYSTEM += libwfduibcsrc
WFD_SYSTEM += libwfdsm
WFD_SYSTEM += libwfdmminterface
WFD_SYSTEM += libwfdrtsp
WFD_SYSTEM += rtspclient
WFD_SYSTEM += rtspserver
WFD_SYSTEM += libwfdcommonutils
WFD_SYSTEM += com.qualcomm.qti.wifidisplayhal@1.0
WFD_SYSTEM += com.qualcomm.qti.wifidisplayhal@1.0_system
WFD_SYSTEM += wfdservice.rc
WFD_SYSTEM += vendor.qti.hardware.wifidisplaysession@1.0
WFD_SYSTEM += vendor.qti.hardware.wifidisplaysession@1.0_system
WFD_SYSTEM += libmiracastsystem
WFD_SYSTEM += sigma_miracasthalservice
WFD_SYSTEM += vendor.qti.hardware.sigma.miracast@1.0
WFD_SYSTEM += vendor.qti.hardware.sigma_miracast@1.0-impl
WFD_SYSTEM += vendor.qti.hardware.sigma_miracast@1.0-halimpl
WFD_SYSTEM += com.qualcomm.qti.sigma_miracast@1.0-service.rc
WFD_SYSTEM += libwfddisplayconfig
WFD_SYSTEM += wfd-system-ext-privapp-permissions-qti.xml
WFD_SYSTEM += wfdservice.policy
endif
PRODUCT_PACKAGES += $(WFD_SYSTEM)

