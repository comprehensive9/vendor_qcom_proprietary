ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

LOCHIDL :=
LOCHIDL += vendor.qti.gnss@1.0
LOCHIDL += vendor.qti.gnss@1.1
LOCHIDL += vendor.qti.gnss@1.2
LOCHIDL += vendor.qti.gnss@2.0
LOCHIDL += vendor.qti.gnss@2.1
LOCHIDL += vendor.qti.gnss@3.0
LOCHIDL += vendor.qti.gnss@4.0

LOC_HIDL_VERSION = 4.0

PRODUCT_PACKAGES += $(LOCHIDL)

## VNDK_GNSS_LIBS
## Below libs are added explicitly in PRODUCT PACKAGES beacuse
## server(vendor) code is shipped as prebuilt and in customer
## variant no module is depends  on these libraries and
## compilation of vendor variant of these libraries will be
## skipped.
VNDK_GNSS_LIBS := $(patsubst vendor.qti.gnss%,vendor.qti.gnss%.vendor,$(LOCHIDL))
PRODUCT_PACKAGES += $(VNDK_GNSS_LIBS)

# List of supported board platform targets
LOC_BOARD_PLATFORM_LIST += msm8937
LOC_BOARD_PLATFORM_LIST += msm8953
LOC_BOARD_PLATFORM_LIST += msm8998
LOC_BOARD_PLATFORM_LIST += apq8098_latv
LOC_BOARD_PLATFORM_LIST += sdm660
LOC_BOARD_PLATFORM_LIST += sdm845
LOC_BOARD_PLATFORM_LIST += msmpeafowl
LOC_BOARD_PLATFORM_LIST += sdm710
LOC_BOARD_PLATFORM_LIST += qcs605
LOC_BOARD_PLATFORM_LIST += msmnile
LOC_BOARD_PLATFORM_LIST += sdmshrike
LOC_BOARD_PLATFORM_LIST += $(MSMSTEPPE)
LOC_BOARD_PLATFORM_LIST += $(TRINKET)
LOC_BOARD_PLATFORM_LIST += kona
LOC_BOARD_PLATFORM_LIST += atoll
LOC_BOARD_PLATFORM_LIST += lito
LOC_BOARD_PLATFORM_LIST += bengal
LOC_BOARD_PLATFORM_LIST += lahaina
LOC_BOARD_PLATFORM_LIST += holi

ifneq (,$(filter $(LOC_BOARD_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))

PRODUCT_PACKAGES += vendor.qti.gnss@4.0-impl
PRODUCT_PACKAGES += vendor.qti.gnss@4.0-service

PRODUCT_PACKAGES += loc_api_app
PRODUCT_PACKAGES += test_loc_api_client
PRODUCT_PACKAGES += test_agps_server
PRODUCT_PACKAGES += izat.conf
PRODUCT_PACKAGES += sap.conf
PRODUCT_PACKAGES += com.qualcomm.location.vzw_library.xml
PRODUCT_PACKAGES += slim_daemon
PRODUCT_PACKAGES += libslimclient
PRODUCT_PACKAGES += libulp2
PRODUCT_PACKAGES += loc_parameter.ini
PRODUCT_PACKAGES += pf_test_app
PRODUCT_PACKAGES += garden_app
PRODUCT_PACKAGES += libalarmservice_jni
PRODUCT_PACKAGES += liblocationservice_glue
PRODUCT_PACKAGES += liblocationservice
PRODUCT_PACKAGES += libdataitems
PRODUCT_PACKAGES += libizat_core
PRODUCT_PACKAGES += liblbs_core
PRODUCT_PACKAGES += libloc_socket
PRODUCT_PACKAGES += libquipc_os_api
PRODUCT_PACKAGES += liblowi_client
PRODUCT_PACKAGES += liblowi_wifihal
PRODUCT_PACKAGES += liblowi_wifihal_nl
PRODUCT_PACKAGES += mlid
PRODUCT_PACKAGES += loc_launcher
PRODUCT_PACKAGES += libgdtap
PRODUCT_PACKAGES += libizat_client_api
PRODUCT_PACKAGES += libloc_pla
PRODUCT_PACKAGES += libloc_stub
PRODUCT_PACKAGES += xtra-daemon
PRODUCT_PACKAGES += libxtadapter
PRODUCT_PACKAGES += libcdfw
PRODUCT_PACKAGES += libcdfw_remote_api
PRODUCT_PACKAGES += libloc_api_wds

#GPS-XTWiFi
PRODUCT_PACKAGES += cacert_location.pem
PRODUCT_PACKAGES += libxtwifi_ulp_adaptor
PRODUCT_PACKAGES += libasn1crt
PRODUCT_PACKAGES += libasn1crtx
PRODUCT_PACKAGES += libasn1cper
PRODUCT_PACKAGES += libxtwifi_server_protocol
PRODUCT_PACKAGES += libxtwifi_server_protocol_uri_v3
PRODUCT_PACKAGES += xtra_root_cert.pem
PRODUCT_PACKAGES += xtwifi-client
PRODUCT_PACKAGES += xtwifi.conf
PRODUCT_PACKAGES += xtwifi-inet-agent
PRODUCT_PACKAGES += xtwifi-upload-test
PRODUCT_PACKAGES += test-fake-ap
PRODUCT_PACKAGES += test-pos-tx
PRODUCT_PACKAGES += test-version
PRODUCT_PACKAGES += lowi-server
PRODUCT_PACKAGES += lowi.conf
PRODUCT_PACKAGES += liblowi_client

#GPS-Automotive
PRODUCT_PACKAGES += libDRPlugin
PRODUCT_PACKAGES += libdrplugin_client
PRODUCT_PACKAGES += lib_drplugin_server
PRODUCT_PACKAGES += DR_AP_Service
PRODUCT_PACKAGES += libloc_externalDr
PRODUCT_PACKAGES += libloc_externalDrcore
PRODUCT_PACKAGES += apdr.conf

#PRODUCT_PACKAGES += libloc_epDr
#PRODUCT_PACKAGES += libloc_externalDrcoreqdr3

ifeq ($(strip $(TARGET_BOARD_AUTO)),true)

SSV_SLL_SW_PLATFORM_LIST += msmnile
SSV_SLL_SW_PLATFORM_LIST += $(MSMSTEPPE)

ifneq (,$(filter $(SSV_SLL_SW_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
PRODUCT_PACKAGES += libloc_sll_impl
PRODUCT_PACKAGES += libloc_sll_sim
PRODUCT_PACKAGES += libsll
endif # FEATURE_SSV_SLL_SW := true

PRODUCT_PACKAGES += sll.conf
PRODUCT_PACKAGES += prgflash_uart
PRODUCT_PACKAGES += GSD5XP_FLASH_5.7.12-P3.bin
PRODUCT_PACKAGES += gnss_ssv_factorytest

endif # ifeq ($(strip $(TARGET_BOARD_AUTO)),true)

# GPS_DBG
PRODUCT_PACKAGES_DEBUG += lowi_test
PRODUCT_PACKAGES_DEBUG += test-fdal
PRODUCT_PACKAGES_DEBUG += gps-test.sh
PRODUCT_PACKAGES_DEBUG += libposlog
PRODUCT_PACKAGES_DEBUG += IZatSample
PRODUCT_PACKAGES_DEBUG += QCLocSvcTests
PRODUCT_PACKAGES_DEBUG += sftc
PRODUCT_PACKAGES_DEBUG += libLocModemEmulator
PRODUCT_PACKAGES_DEBUG += LocModemEmulatorCli
PRODUCT_PACKAGES_DEBUG += OptInAppOverlay
PRODUCT_PACKAGES_DEBUG += libizat_api_pds

ifneq ($(FEATURE_SLIM_AP),false)
PRODUCT_PACKAGES_DEBUG += slim_client
PRODUCT_PACKAGES_DEBUG += qmislim_client
PRODUCT_PACKAGES_DEBUG += qmislim_service
endif

PRODUCT_PACKAGES += libasn1cper_static
PRODUCT_PACKAGES += libasn1crt_static
PRODUCT_PACKAGES += libasn1crtx_static

endif # ifneq (,$(filter $(LOC_BOARD_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
