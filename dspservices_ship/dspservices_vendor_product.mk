ADSPRPC_VENDOR += libadsp_default_listener
ADSPRPC_VENDOR += libcdsp_default_listener
ADSPRPC_VENDOR += libssc_default_listener

TARGET_AUDIOPD_DISABLED := sdm845
TARGET_AUDIOPD_DISABLED += lahaina
TARGET_AUDIOPD_DISABLED += holi
TARGET_AUDIOPD_DISABLED += msm8953
TARGET_AUDIOPD_DISABLED += msm8998

TARGET_FASTRPC_SENSORSPD_ENABLED := sm6150
TARGET_FASTRPC_SENSORSPD_ENABLED += sdm710
TARGET_FASTRPC_SENSORSPD_ENABLED += trinket
TARGET_FASTRPC_SENSORSPD_ENABLED_AUDIOPD_DISABLED += msm8937

TARGET_CDSPRPCD_DISABLED := msm8937
TARGET_CDSPRPCD_DISABLED += msm8953
TARGET_CDSPRPCD_DISABLED += msm8998
TARGET_CDSPRPCD_DISABLED += monaco

ifneq ($(ENABLE_HYP), true)
ifeq ($(call is-board-platform-in-list, $(TARGET_FASTRPC_SENSORSPD_ENABLED)),true)
ADSPRPC_VENDOR += adsprpcd_guestos_audiopd_sscrrpc
else ifeq ($(call is-board-platform-in-list, $(TARGET_AUDIOPD_DISABLED)),true)
ADSPRPC_VENDOR += adsprpcd_guestos
else ifeq ($(call is-board-platform-in-list, $(TARGET_FASTRPC_SENSORSPD_ENABLED_AUDIOPD_DISABLED)),true)
ADSPRPC_VENDOR += adsprpcd_guestos_sensorspd
else
#targets where sscrpcd is enabled
ADSPRPC_VENDOR += adsprpcd_guestos_audiopd
endif
ifneq ($(call is-board-platform-in-list, $(TARGET_CDSPRPCD_DISABLED)),true)
ADSPRPC_VENDOR += cdsprpcd
ADSPRPC_VENDOR += libloadalgo_stub
ADSPRPC_VENDOR += loadalgo
ADSPRPC_VENDOR += libsysmon_cdsp_skel
ifneq ($(ENABLE_HYP),true)
ADSPRPC_VENDOR += vendor.qti.cdsprpc-service.rc
endif
endif
endif
ADSPRPC_VENDOR += dspservice
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0-service.rc
ADSPRPC_VENDOR += vendor.qti.hardware.dsp.policy
ifneq ($(ENABLE_HYP),true)
ADSPRPC_VENDOR += vendor.qti.adsprpc-service.rc
ADSPRPC_VENDOR += vendor.qti.adsprpc-sscrpc-service.rc
ADSPRPC_VENDOR += vendor.qti.adsprpc-guestos-sensorspd-service.rc
endif
ifeq ($(call is-vendor-board-platform,QCOM),true)
PRODUCT_PACKAGES += $(ADSPRPC_VENDOR)
endif

#ADSPRPC_DBG
ADSPRPC_DBG := fastrpc_tests_apps
ADSPRPC_DBG += fastrpc_tests_apps32
ADSPRPC_DBG += libreverse_rpc_tests_skel
ADSPRPC_DBG += libfastrpc_tests_skel
ADSPRPC_DBG += libhap_power_test_skel
ADSPRPC_DBG += libtest_dl_skel
ADSPRPC_DBG += libtest_dl_perf_skel
ADSPRPC_DBG += librpcperf_skel
ADSPRPC_DBG += libfastrpc_tests1_skel
ADSPRPC_DBG += libtest_dl1_skel
ADSPRPC_DBG += libtest_dl_perf1_skel
ADSPRPC_DBG += librpcperf1_skel
ADSPRPC_DBG += libunsigned_pd_test_skel
ADSPRPC_DBG += libbenchmark_skel
ADSPRPC_DBG += test_elf_size_16k
ADSPRPC_DBG += test_elf_size_32k
ADSPRPC_DBG += test_elf_size_64k
ADSPRPC_DBG += test_elf_size_128k
ADSPRPC_DBG += test_elf_size_256k
ADSPRPC_DBG += test_elf_size_512k
ADSPRPC_DBG += test_elf_size_768k
ADSPRPC_DBG += test_elf_size_1MB
ADSPRPC_DBG += test_elf_nop
ADSPRPC_DBG += test_elf_dep1
ADSPRPC_DBG += test_elf_dep2
ifeq ($(call is-vendor-board-platform,QCOM),true)
PRODUCT_PACKAGES_DEBUG += $(ADSPRPC_DBG)
endif
