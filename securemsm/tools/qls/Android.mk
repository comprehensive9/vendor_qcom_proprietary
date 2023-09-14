LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifndef QC_PROP_ROOT
  QC_PROP_ROOT := vendor/qcom/proprietary
endif

$(shell mkdir -p $(PRODUCT_OUT))
ifeq ($(call is-board-platform-in-list, lahaina),true)

  # Generate UEFI license store "qweslicstore.bin"
  ECHO_RESULT := $(shell python $(LOCAL_PATH)/qlstool.py -i ./$(QC_PROP_ROOT)/securemsm/qapi/qwes/data/UEFI/ -o $(LOCAL_PATH))
  $(shell cp -rf $(LOCAL_PATH)/qweslicstore.bin $(PRODUCT_OUT))
  $(shell cp -rf $(LOCAL_PATH)/qlstool.py $(PRODUCT_OUT))

  # Generate HLOS license store "store_*"
  $(shell mkdir -p $(LOCAL_PATH)/licenses)
  $(shell mkdir -p $(LOCAL_PATH)/licstore)
  $(shell cp ./$(QC_PROP_ROOT)/securemsm/qapi/qwes/data/UEFI/*.pfm $(LOCAL_PATH)/licenses)
  $(shell cp ./$(QC_PROP_ROOT)/securemsm/qapi/qwes/data/*.pfm $(LOCAL_PATH)/licenses)
  ECHO_RESULT := $(shell python $(LOCAL_PATH)/qlstool.py -m hlos -i $(LOCAL_PATH)/licenses/ -o $(LOCAL_PATH)/licstore/)

  # Push HLOS license store "store_*" and licenses to /persist/
  $(shell mkdir -p $(PRODUCT_OUT)/persist/data/pfm/licenses)
  $(shell cp -rf $(LOCAL_PATH)/licstore/. $(PRODUCT_OUT)/persist/data/pfm/licenses)
  $(shell cp ./$(QC_PROP_ROOT)/securemsm/qapi/qwes/data/UEFI/*.pfm $(PRODUCT_OUT)/persist/data/pfm/licenses)
  $(shell cp ./$(QC_PROP_ROOT)/securemsm/qapi/qwes/data/*.pfm $(PRODUCT_OUT)/persist/data/pfm/licenses)

endif
