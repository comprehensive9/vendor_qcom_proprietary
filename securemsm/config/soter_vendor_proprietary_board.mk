ifneq ($(TARGET_USES_QMAA), true)
COMPILE_SOTER_MODULES := true
endif #TARGET_USES_QMAA

ifeq ($(TARGET_USES_QMAA_OVERRIDE_SOTER), true)
COMPILE_SOTER_MODULES := true
endif #TARGET_USES_QMAA_OVERRIDE_SOTER

ifeq ($(ENABLE_HYP),true)
COMPILE_SOTER_MODULES := false
endif #ENABLE_HYP

ifeq ($(COMPILE_SOTER_MODULES),true)
TARGET_USES_SOTER := true

#Disable soter in 32go builds, such as msm8937_32go
ifeq ($(TARGET_BOARD_SUFFIX),_32go)
TARGET_USES_SOTER := false
endif

#Disable soter in go_aon builds, such as monaco_go_aon
ifeq ($(TARGET_BOARD_SUFFIX),_go_aon)
TARGET_USES_SOTER := false
endif

#Disable soter in go builds.
ifeq ($(TARGET_BOARD_SUFFIX),_go)
TARGET_USES_SOTER := false
endif

#Disable soter in 32go builds, such as msm8937_lily
ifeq ($(TARGET_BOARD_SUFFIX),_lily)
TARGET_USES_SOTER := false
endif

endif #COMPILE_SOTER_MODULES
