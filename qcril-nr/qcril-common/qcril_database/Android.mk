LOCAL_PATH := $(call my-dir)
QCRIL_COMMON_DIR := ${LOCAL_PATH}/../

## Build header library
include $(CLEAR_VARS)
LOCAL_MODULE                := libril-db-headers
# here the class is only used to specify directory names in output directory
LOCAL_MODULE_CLASS          := SHARED_LIBRARIES_HEADERS
GEN_HEADER_DIR := $(call local-generated-sources-dir)

GEN_HEADER_FILE := $(GEN_HEADER_DIR)/__config_key.h
$(GEN_HEADER_FILE): PRIVATE_CUSTOM_TOOL = python $(QCRIL_COMMON_DIR)/qcril_database/tools/config_gen.py \
    $(QCRIL_COMMON_DIR)/qcril_database/src/configs.xml --header $@
$(GEN_HEADER_FILE): $(QCRIL_COMMON_DIR)/qcril_database/tools/config_gen.py $(QCRIL_COMMON_DIR)/qcril_database/src/configs.xml
$(GEN_HEADER_FILE):
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES     := $(GEN_HEADER_FILE)

LOCAL_VENDOR_MODULE         := true
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/inc $(GEN_HEADER_DIR)

include $(BUILD_HEADER_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE                := libril-db-headers
# here the class is only used to specify directory names in output directory
LOCAL_MODULE_CLASS          := SHARED_LIBRARIES_HEADERS
LOCAL_IS_HOST_MODULE        := true
GEN_HEADER_DIR := $(call local-generated-sources-dir)

GEN_HEADER_FILE := $(GEN_HEADER_DIR)/__config_key.h
$(GEN_HEADER_FILE): PRIVATE_CUSTOM_TOOL = python $(QCRIL_COMMON_DIR)/qcril_database/tools/config_gen.py \
    $(QCRIL_COMMON_DIR)/qcril_database/src/configs.xml --header $@
$(GEN_HEADER_FILE): $(QCRIL_COMMON_DIR)/qcril_database/tools/config_gen.py $(QCRIL_COMMON_DIR)/qcril_database/src/configs.xml
$(GEN_HEADER_FILE):
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES     := $(GEN_HEADER_FILE)

LOCAL_VENDOR_MODULE         := true
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/inc $(GEN_HEADER_DIR)

include $(BUILD_HEADER_LIBRARY)

## Build shared library
include $(CLEAR_VARS)

LOCAL_CFLAGS               += -DRIL_SHLIB $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_C_INCLUDES           += $(QCRIL_COMMON_DIR)/include
ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             := $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)

LOCAL_MODULE               := libril-db
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_HEADER_LIBRARIES     := libril-db-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += qtiril-utils-headers
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES     += libsqlite
LOCAL_SHARED_LIBRARIES     += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += qtiril-utils

LOCAL_HEADER_LIBRARIES     += qcrilDataInterfaces_headers

include $(BUILD_SHARED_LIBRARY)

###########################################################################
DB_UPGRADES_OTHER_SOURCE := $(sort $(call all-named-files-under,*.sql,upgrade/other))

# Build qcril.db
include $(CLEAR_VARS)
LOCAL_MODULE := qcrilNr.db
LOCAL_MODULE_CLASS := ETC
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/qcril_database

$(foreach script, $(DB_UPGRADES_OTHER_SOURCE), \
$(eval LOCAL_REQUIRED_MODULES += $(subst /,_,$(script))) \
)
LOCAL_REQUIRED_MODULES += qtigetprop
LOCAL_REQUIRED_MODULES += qtisetprop

include $(BUILD_SYSTEM)/base_rules.mk

# DB dependencies: 1) other source of SQL file (like ecc number), 2) XML file (to generate the config SQLs)
# As part of rule, config SQLs will be generated and COPIED. (Better way may be to dynamically generate the
# PREBUILT MODULES, which has dependencies on this rule)
# Since 1) we always generate new versions of SQL and never delete existed files or change existed names, 2)
# copying happens before "target" DB is generated (in case that compiling is aborted by user or some error),
# copying and overwriting directly should be fine.
DB_OTHER_UPGRADES_FILES := upgrade/other/0_initial_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/1_version_intro_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/2_version_add_wps_config_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/3_version_update_wps_config_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/4_version_update_ecc_table_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/5_version_update_ecc_table_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/6_version_change_property_table_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/7_version_update_ecc_table_qcrilnr.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/8_version_update_ecc_table.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/9_version_update_ecc_table.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/10_version_update_ecc_table.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/11_version_update_ecc_table.sql
DB_OTHER_UPGRADES_FILES += upgrade/other/12_version_update_ecc_table.sql
DB_OTHER_UPGRADES_PATH := $(addprefix $(LOCAL_PATH)/, $(DB_OTHER_UPGRADES_FILES))
$(info DB_OTHER_UPGRADES_PATH: $(DB_OTHER_UPGRADES_PATH))
DB_CONFIG_UPGRADES_FILES := upgrade/config/6.0_config.sql
DB_CONFIG_UPGRADES_FILES += upgrade/config/7.0_config.sql
DB_CONFIG_UPGRADES_FILES += upgrade/config/8.0_config.sql
DB_CONFIG_UPGRADES_FILES += upgrade/config/9.0_config.sql
DB_CONFIG_UPGRADES_FILES += upgrade/config/10.0_config.sql
DB_CONFIG_UPGRADES_FILES += upgrade/config/11.0_config.sql
DB_CONFIG_UPGRADES_FILES += upgrade/config/12.0_config.sql
DB_CONFIG_UPGRADES_PATH := $(addprefix $(LOCAL_PATH)/, $(DB_CONFIG_UPGRADES_FILES))
$(info DB_CONFIG_UPGRADES_PATH: $(DB_CONFIG_UPGRADES_PATH))
$(LOCAL_BUILT_MODULE): $(DB_OTHER_UPGRADES_PATH) $(QCRIL_COMMON_DIR)/qcril_database/src/configs.xml $(HOST_OUT_EXECUTABLES)/sqlite3
	$(hide) rm -f $@
	$(hide) mkdir -p $(TARGET_OUT_VENDOR)/etc/qcril_database/upgrade/config/
	$(hide) python $(QCRIL_COMMON_DIR)/qcril_database/tools/config_gen.py $(QCRIL_COMMON_DIR)/qcril_database/src/configs.xml --sql $(QCRIL_COMMON_DIR)/qcril_database/upgrade/config/
	$(hide) cp -rp $(QCRIL_COMMON_DIR)/qcril_database/upgrade/config/ $(TARGET_OUT_VENDOR)/etc/qcril_database/upgrade/
	$(hide) (echo BEGIN TRANSACTION\;; cat $(DB_OTHER_UPGRADES_PATH); echo COMMIT TRANSACTION\;) | $(HOST_OUT_EXECUTABLES)/sqlite3 $@
	$(hide) (echo BEGIN TRANSACTION\;; cat $(DB_CONFIG_UPGRADES_PATH); echo COMMIT TRANSACTION\;) | $(HOST_OUT_EXECUTABLES)/sqlite3 $@

# Copy SQL files
#$(foreach script,$(DB_UPGRADES_CONFIG_SOURCE), \
#$(eval include $(CLEAR_VARS)) \
#$(eval LOCAL_MODULE := $(subst /,_,$(script))) \
#$(eval LOCAL_MODULE_STEM :=  $(notdir $(script))) \
#$(eval LOCAL_PROPRIETARY_MODULE := true) \
#$(eval LOCAL_MODULE_OWNER := qti) \
#$(eval LOCAL_MODULE_CLASS := SCRIPT) \
#$(eval LOCAL_MODULE_TAGS := optional) \
#$(eval LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/qcril_database/upgrade/config/) \
#$(eval LOCAL_SRC_FILES := $(script)) \
#$(eval include $(BUILD_PREBUILT)) \
#)

# Copy SQL files
$(foreach script,$(DB_UPGRADES_OTHER_SOURCE), \
$(eval include $(CLEAR_VARS)) \
$(eval LOCAL_MODULE := $(subst /,_,$(script))) \
$(eval LOCAL_MODULE_STEM := $(notdir $(script))) \
$(eval LOCAL_PROPRIETARY_MODULE := true) \
$(eval LOCAL_MODULE_OWNER := qti) \
$(eval LOCAL_MODULE_CLASS := SCRIPT) \
$(eval LOCAL_MODULE_TAGS := optional) \
$(eval LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/qcril_database/upgrade/other) \
$(eval LOCAL_SRC_FILES := $(script)) \
$(eval include $(BUILD_PREBUILT)) \
)

# Copy the command line tools
include $(CLEAR_VARS)
LOCAL_MODULE := qtisetprop
LOCAL_SRC_FILES := tools/qtisetprop
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_CLASS := SCRIPT
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/bin/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := qtigetprop
LOCAL_SRC_FILES := tools/qtigetprop
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_CLASS := SCRIPT
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/bin
include $(BUILD_PREBUILT)
