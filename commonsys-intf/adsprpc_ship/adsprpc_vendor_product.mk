ADSPRPC_VENDOR := libadsprpc
ADSPRPC_VENDOR += libmdsprpc
ADSPRPC_VENDOR += libsdsprpc
ADSPRPC_VENDOR += libcdsprpc
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0.vendor
ADSPRPC_VENDOR += vendor.qti.hardware.dsp@1.0_vendor
ADSPRPC_VENDOR += com.qualcomm.qti.FASTRPCPRIVILEGE.xml
$(call add_soong_config_namespace,fastrpc_QMAA_config)
ifeq ($(TARGET_USES_QMAA),true)
$(call add_soong_config_var_value,fastrpc_QMAA_config,fastrpc_QMAA,enabled)
else
$(call add_soong_config_var_value,fastrpc_QMAA_config,fastrpc_QMAA,disabled)
endif
$(call add_soong_config_namespace,fastrpc_HYP_config)
ifeq ($(ENABLE_HYP),true)
$(call add_soong_config_var_value,fastrpc_HYP_config,fastrpc_HYP_vendor,enabled)
endif

PRODUCT_PACKAGES += $(ADSPRPC_VENDOR)
