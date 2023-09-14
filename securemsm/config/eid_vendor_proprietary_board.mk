ifneq ($(TARGET_USES_QMAA), true)
COMPILE_EID_MODULES := true
endif #TARGET_USES_QMAA

ifeq ($(TARGET_USES_QMAA_OVERRIDE_EID), true)
COMPILE_EID_MODULES := true
endif #TARGET_USES_QMAA_OVERRIDE_EID

ifeq ($(COMPILE_EID_MODULES),true)

#Lahain specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),lahaina)
TARGET_USES_EID := true
endif

#Strait specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),holi)
TARGET_USES_EID := true
endif

endif #COMPILE_EID_MODULES
