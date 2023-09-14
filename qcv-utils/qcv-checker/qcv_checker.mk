QCV_DEPS_VENDOR_IMAGE := $(PRODUCT_OUT)/vendor.img
QCV_DEPS_MISC_INFO_TXT := $(PRODUCT_OUT)/misc_info.txt
QCV_CHECKER_ENFORCEMENT_MODE := 1

.PHONY: qcv_checker
qcv_checker: $(QCV_DEPS_VENDOR_IMAGE) $(QCV_DEPS_MISC_INFO_TXT)
	python vendor/qcom/proprietary/qcv-utils/qcv-checker/qcv_checker.py \
	--target $(TARGET_BOARD_PLATFORM) --enforce $(QCV_CHECKER_ENFORCEMENT_MODE) --product_out $(PRODUCT_OUT)

ifeq ($(TARGET_USES_QCV),true)
droidcore:qcv_checker
endif
