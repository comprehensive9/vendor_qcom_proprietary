possible_cvv_dirs = $(QCPATH)/prebuilt_HY11 $(QCPATH)/prebuilt_HY22 $(QCPATH)/prebuilt_grease
prebuilt_cvv_dirs =  $(wildcard $(possible_cvv_dirs))
TBP := $(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX)
ifneq ($(prebuilt_cvv_dirs),)
RET := $(shell for i in $(prebuilt_cvv_dirs);do bash vendor/qcom/proprietary/common/create_bp.sh $(TBP) $$i; bash vendor/qcom/proprietary/common/create_mk.sh $(TBP) $$i; done)
endif
$(warning Successfully Created Dynamic Android.bp/mk files.)
