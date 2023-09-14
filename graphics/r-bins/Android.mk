#This file is generated using generate_bpx_mkx_for_a6xbins.py.

ADRENO := adreno
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_USE_VNDK := true
LOCAL_COPY_HEADERS_TO := $(ADRENO)
LOCAL_COPY_HEADERS := include/public/GLES3/gl3extQCOM.h
LOCAL_COPY_HEADERS += include/public/GLES2/gl2extQCOM.h
LOCAL_COPY_HEADERS += include/private/C2D/c2d2.h
LOCAL_COPY_HEADERS += c2d30/include/c2dExt.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_USE_VNDK := true
LOCAL_COPY_HEADERS_TO := $(ADRENO)/GLES
LOCAL_COPY_HEADERS := include/public/GLES/egl.h
LOCAL_COPY_HEADERS += include/public/GLES/gl.h
LOCAL_COPY_HEADERS += include/public/GLES/glext.h
LOCAL_COPY_HEADERS += include/public/GLES/glextQCOM.h
LOCAL_COPY_HEADERS += include/public/GLES/glplatform.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_USE_VNDK := true
LOCAL_COPY_HEADERS_TO := $(ADRENO)/GLES2
LOCAL_COPY_HEADERS := include/public/GLES2/gl2.h
LOCAL_COPY_HEADERS += include/public/GLES2/gl2ext.h
LOCAL_COPY_HEADERS += include/public/GLES2/gl2extQCOM.h
LOCAL_COPY_HEADERS += include/public/GLES2/gl2platform.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_USE_VNDK := true
LOCAL_COPY_HEADERS_TO := $(ADRENO)/GLES3
LOCAL_COPY_HEADERS := include/public/GLES3/gl3.h
LOCAL_COPY_HEADERS += include/public/GLES3/gl3extQCOM.h
LOCAL_COPY_HEADERS += include/public/GLES3/gl3platform.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_USE_VNDK := true
LOCAL_COPY_HEADERS_TO := $(ADRENO)/CL
LOCAL_COPY_HEADERS := include/public/CL/cl.h
LOCAL_COPY_HEADERS += include/public/CL/cl_ext.h
LOCAL_COPY_HEADERS += include/public/CL/cl_gl.h
LOCAL_COPY_HEADERS += include/public/CL/opencl.h
LOCAL_COPY_HEADERS += include/public/CL/cl.hpp
LOCAL_COPY_HEADERS += include/public/CL/cl_gl_ext.h
LOCAL_COPY_HEADERS += include/public/CL/cl_platform.h
LOCAL_COPY_HEADERS += include/public/CL/cl_ext_qcom.h
LOCAL_COPY_HEADERS += include/public/CL/cl_egl.h
LOCAL_COPY_HEADERS += include/public/CL/cl2.hpp
LOCAL_COPY_HEADERS += include/public/CL/opencl.hpp
LOCAL_COPY_HEADERS += include/public/CL/cl_version.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_USE_VNDK := true
LOCAL_COPY_HEADERS_TO := $(ADRENO)/EGL
LOCAL_COPY_HEADERS := include/public/EGL/egl.h
LOCAL_COPY_HEADERS += include/public/EGL/eglextQCOM.h
LOCAL_COPY_HEADERS += include/public/EGL/eglext.h
LOCAL_COPY_HEADERS += include/public/EGL/eglplatform.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_USE_VNDK := true
LOCAL_COPY_HEADERS_TO := $(ADRENO)/KHR
LOCAL_COPY_HEADERS := include/public/KHR/khrplatform.h
include $(BUILD_COPY_HEADERS)

ifneq ($(ADRENO_BUILD_UPDATABLE_DRIVERS),)
ifeq ($(ADRENO_BUILD_EMPTY_DRIVER),)
# Preinstalled (empty) driver package

include $(CLEAR_VARS)
LOCAL_MODULE             := $(ADRENO_EMPTY_DRIVER_NAME)
LOCAL_SRC_FILES          := $(LOCAL_DIR)/emptydrivers/$(ADRENO_EMPTY_DRIVER_NAME).apk
LOCAL_MODULE_SUFFIX      := .apk
LOCAL_DEX_PREOPT         := false
LOCAL_MODULE_OWNER       := qti
LOCAL_MODULE_TAGS        := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS       := APPS
LOCAL_CERTIFICATE        := PRESIGNED
LOCAL_MODULE_PATH        := $(TARGET_OUT_VENDOR)/app
include $(BUILD_PREBUILT)

endif # ADRENO_BUILD_EMPTY_DRIVER
endif # ADRENO_BUILD_UPDATABLE_DRIVERS


include $(CLEAR_VARS)
LOCAL_MODULE := libEGL_adreno
LOCAL_MULTILIB := both
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_32 := vendor/lib/egl/libEGL_adreno.so
LOCAL_SRC_FILES_64 := vendor/lib64/egl/libEGL_adreno.so
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := egl
LOCAL_STRIP_MODULE := false
include $(BUILD_PREBUILT)

SYM_LOCAL_PATH := $(LOCAL_PATH)

# Symlink for 32bit library
SYMLINKS32 := $(addprefix $(TARGET_OUT_VENDOR)/lib/,$(LOCAL_MODULE).so)
$(SYMLINKS32): BINARY := $(LOCAL_MODULE).so
$(SYMLINKS32): $(LOCAL_INSTALLED_MODULE) $(SYM_LOCAL_PATH)/Android.mk
	@echo "Symlink for 32bit: $@ -> egl/$(BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf egl/$(BINARY) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS32)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := 	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS32)

ifneq ($(filter %64,$(TARGET_ARCH)),)

# Symlink for 64bit library
SYMLINKS64 := $(addprefix $(TARGET_OUT_VENDOR)/lib64/,$(LOCAL_MODULE).so)
$(SYMLINKS64): BINARY := $(LOCAL_MODULE).so
$(SYMLINKS64): $(LOCAL_INSTALLED_MODULE) $(SYM_LOCAL_PATH)/Android.mk
	@echo "Symlink for 64bit: $@ -> egl/$(BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf egl/$(BINARY) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS64)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := 	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS64)

endif # ifneq ($(filter %64,$(TARGET_ARCH)),)

include $(CLEAR_VARS)
LOCAL_MODULE := libGLESv2_adreno
LOCAL_MULTILIB := both
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_32 := vendor/lib/egl/libGLESv2_adreno.so
LOCAL_SRC_FILES_64 := vendor/lib64/egl/libGLESv2_adreno.so
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := egl
LOCAL_STRIP_MODULE := false
include $(BUILD_PREBUILT)

SYM_LOCAL_PATH := $(LOCAL_PATH)

# Symlink for 32bit library
SYMLINKS32 := $(addprefix $(TARGET_OUT_VENDOR)/lib/,$(LOCAL_MODULE).so)
$(SYMLINKS32): BINARY := $(LOCAL_MODULE).so
$(SYMLINKS32): $(LOCAL_INSTALLED_MODULE) $(SYM_LOCAL_PATH)/Android.mk
	@echo "Symlink for 32bit: $@ -> egl/$(BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf egl/$(BINARY) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS32)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := 	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS32)

ifneq ($(filter %64,$(TARGET_ARCH)),)

# Symlink for 64bit library
SYMLINKS64 := $(addprefix $(TARGET_OUT_VENDOR)/lib64/,$(LOCAL_MODULE).so)
$(SYMLINKS64): BINARY := $(LOCAL_MODULE).so
$(SYMLINKS64): $(LOCAL_INSTALLED_MODULE) $(SYM_LOCAL_PATH)/Android.mk
	@echo "Symlink for 64bit: $@ -> egl/$(BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf egl/$(BINARY) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS64)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := 	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS64)

endif # ifneq ($(filter %64,$(TARGET_ARCH)),)

include $(CLEAR_VARS)
LOCAL_MODULE := libq3dtools_adreno
LOCAL_MULTILIB := both
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_32 := vendor/lib/egl/libq3dtools_adreno.so
LOCAL_SRC_FILES_64 := vendor/lib64/egl/libq3dtools_adreno.so
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := egl
LOCAL_STRIP_MODULE := false
include $(BUILD_PREBUILT)

SYM_LOCAL_PATH := $(LOCAL_PATH)

# Symlink for 32bit library
SYMLINKS32 := $(addprefix $(TARGET_OUT_VENDOR)/lib/,$(LOCAL_MODULE).so)
$(SYMLINKS32): BINARY := $(LOCAL_MODULE).so
$(SYMLINKS32): $(LOCAL_INSTALLED_MODULE) $(SYM_LOCAL_PATH)/Android.mk
	@echo "Symlink for 32bit: $@ -> egl/$(BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf egl/$(BINARY) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS32)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := 	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS32)

ifneq ($(filter %64,$(TARGET_ARCH)),)

# Symlink for 64bit library
SYMLINKS64 := $(addprefix $(TARGET_OUT_VENDOR)/lib64/,$(LOCAL_MODULE).so)
$(SYMLINKS64): BINARY := $(LOCAL_MODULE).so
$(SYMLINKS64): $(LOCAL_INSTALLED_MODULE) $(SYM_LOCAL_PATH)/Android.mk
	@echo "Symlink for 64bit: $@ -> egl/$(BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf egl/$(BINARY) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS64)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := 	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS64)

endif # ifneq ($(filter %64,$(TARGET_ARCH)),)
