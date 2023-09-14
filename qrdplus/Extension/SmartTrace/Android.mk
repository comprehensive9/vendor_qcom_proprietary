ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
  LOCAL_PATH:= $(call my-dir)
  include $(CLEAR_VARS)
  LOCAL_MODULE       := tracing_config.sh
  LOCAL_MODULE_TAGS  := optional
  LOCAL_MODULE_CLASS := EXECUTABLES
  LOCAL_SRC_FILES    := shell/tracing_config.sh
  LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_EXECUTABLES)
  include $(BUILD_PREBUILT)

  include $(CLEAR_VARS)
  LOCAL_MODULE       := smart_trace.rc
  LOCAL_MODULE_TAGS  := optional
  LOCAL_MODULE_CLASS := ETC
  LOCAL_SRC_FILES    := smart_trace.rc
  LOCAL_MODULE_PATH  := $(TARGET_OUT_VENDOR_ETC)/init
  include $(BUILD_PREBUILT)
endif
