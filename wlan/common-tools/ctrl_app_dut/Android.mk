# Role is dut or platform
ROLE = dut
# Package Version
VERSION = "1.1.0.61"

OBJS=main.c
OBJS += eloop.c
OBJS += indigo_api.c
OBJS += indigo_packet.c
OBJS += utils.c
OBJS += wpa_ctrl.c

# Initialize CFLAGS to limit to local module
CFLAGS =

# Define the app is for DUT or platform
ifeq ($(ROLE),dut)
OBJS += indigo_api_callback_dut.c
OBJS += vendor_specific_dut.c
CFLAGS += -D_DUT_
else
OBJS += indigo_api_callback_tp.c
OBJS += vendor_specific_tp.c
CFLAGS += -DCONFIG_CTRL_IFACE_UDP
CFLAGS += -D_TEST_PLATFORM_
endif

LOCAL_PATH := $(call my-dir)

# Define the package version
ifneq ($(VERSION),)
CFLAGS += -D_VERSION_='$(VERSION)'
endif
CFLAGS += -DCONFIG_CTRL_IFACE_CLIENT_DIR=\"/data/vendor/wifi/wpa/sockets\"

include $(CLEAR_VARS)
LOCAL_MODULE := ctrlapp_dut
#ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED), true)
LOCAL_VENDOR_MODULE := true
#endif
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) system/core/include/cutils \
	$(LOCAL_PATH) hardware/libhardware_legacy/include/hardware_legacy \
	$(TARGET_OUT_HEADERS)/common/inc \
	$(LOCAL_PATH) external/libnl/include

LOCAL_SHARED_LIBRARIES := libc libcutils libnl
LOCAL_SHARED_LIBRARIES += libnetutils
LOCAL_C_INCLUDES += $(LOCAL_PATH) system/core/include/netutils
LOCAL_SHARED_LIBRARIES += libhardware_legacy
CFLAGS += -Wno-unused-parameter
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SRC_FILES := $(OBJS)
LOCAL_HEADER_LIBRARIES := libcutils_headers
LOCAL_CFLAGS := $(CFLAGS)
include $(BUILD_EXECUTABLE)
