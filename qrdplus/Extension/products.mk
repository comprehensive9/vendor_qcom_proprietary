ifneq ($(TARGET_HAS_LOW_RAM),true)
    ifeq ($(TARGET_FWK_SUPPORTS_FULL_VALUEADDS),true)
        ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
           PRODUCT_PACKAGES += tracing_config.sh
           PRODUCT_PACKAGES += smart_trace.rc
        endif
    endif
endif
