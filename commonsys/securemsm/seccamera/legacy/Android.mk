ifneq ($(BUILD_QEMU_IMAGES),true)
    include $(call all-subdir-makefiles)
endif
