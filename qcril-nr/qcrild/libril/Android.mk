# Copyright 2006 The Android Open Source Project

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true

LOCAL_SRC_FILES := \
    ril.cpp \
    ril_event.cpp \
    ril_service.cpp \
    ril_utils.cpp \
    ril_service_1_2.cpp \
    RilServiceModule.cpp \
    RilServiceModule_1_2.cpp \


LOCAL_SRC_FILES += ril_service_1_3.cpp
LOCAL_SRC_FILES += RilServiceModule_1_3.cpp
LOCAL_SRC_FILES += ril_service_1_4.cpp
LOCAL_SRC_FILES += RilServiceModule_1_4.cpp
LOCAL_SRC_FILES += ril_service_1_5.cpp
LOCAL_SRC_FILES += RilServiceModule_1_5.cpp

ifeq ($(TARGET_BUILD_VARIANT),userdebug)
LOCAL_CFLAGS += -DRIL_ENABLE_SOCKET_INTERFACE
LOCAL_SRC_FILES += \
    ril_socket_api.cpp \
    RilSocketIndicationModule.cpp \
    ril_socket/Answer.cpp \
    ril_socket/Conference.cpp \
    ril_socket/GetCurrentCalls.cpp \
    ril_socket/GsmBroadcastSms.cpp \
    ril_socket/Hangup.cpp \
    ril_socket/Dtmf.cpp \
    ril_socket/EnterNetworkDepersonalization.cpp \
    ril_socket/lastCallFailCause.cpp \
    ril_socket/NetworkType.cpp \
    ril_socket/QueryAvailableBandMode.cpp \
    ril_socket/SeparateConnection.cpp \
    ril_socket/SignalStrength.cpp \
    ril_socket/Stk.cpp \
    ril_socket/SwitchCalls.cpp \
    ril_socket/Udub.cpp \
    ril_socket/USSD.cpp \
    ril_socket/RadioCapability.cpp \
    ril_socket/RadioPower.cpp \
    ril_socket/AllowData.cpp \
    ril_socket/EnableSim.cpp \
    ril_socket/GetActivityInfo.cpp \
    ril_socket/BasebandVersion.cpp \
    ril_socket/DeviceIdentity.cpp \
    ril_socket/OperatorName.cpp \
    ril_socket/QueryNetworkSelectionMode.cpp \
    ril_socket/SetNetworkSelection.cpp \
    ril_socket/SetLocationUpdates.cpp \
    ril_socket/VoiceRadioTech.cpp \
    ril_socket/ExplicitCallTransfer.cpp \
    ril_socket/SetMute.cpp \
    ril_socket/GetMute.cpp \
    ril_socket/SetClir.cpp \
    ril_socket/GetClir.cpp \
    ril_socket/ImsSendRttMessage.cpp \
    ril_socket/ImsGeolocationInfoMessage.cpp \
    ril_socket/SetUnsolCellInfoListRate.cpp \
    ril_socket/CdmaPreferredVoicePrivacy.cpp \
    ril_socket/TtyMode.cpp \
    ril_socket/CdmaFlash.cpp \
    ril_socket/ImsRegistrationState.cpp \
    ril_socket/AvailableNetworks.cpp \
    ril_socket/NetworkScan.cpp \
    ril_socket/SetSystemSelectionChannels.cpp \
    ril_socket/CallForwardStatus.cpp\
    ril_socket/CallWaiting.cpp\
    ril_socket/ChangeBarringPassword.cpp\
    ril_socket/QueryClip.cpp\
    ril_socket/SetSuppSvcNotification.cpp\
    ril_socket/ImsDtmf.cpp \
    ril_socket/ImsUtils.cpp \
    ril_socket/ImsGetRegistrationState.cpp \
    ril_socket/ImsQueryServiceStatus.cpp \
    ril_socket/ImsRegistrationChange.cpp \
    ril_socket/ImsSetServiceStatus.cpp \
    ril_socket/ImsGetImsSubConfig.cpp \
    ril_socket/ImsDial.cpp \
    ril_socket/ImsAnswer.cpp \
    ril_socket/ImsHangup.cpp \
    ril_socket/ImsAddParticipant.cpp \
    ril_socket/ImsCallForwardStatus.cpp \
    ril_socket/ImsConfig.cpp \
    ril_socket/ImsModifyCall.cpp \
    ril_socket/ImsExplicitCallTransfer.cpp \
    ril_socket/ImsClir.cpp \
    ril_socket/ImsDeflectCall.cpp \
    ril_socket/ImsSendUiTtyMode.cpp \
    ril_socket/ImsQueryClip.cpp \
    ril_socket/ImsSetSuppSvcNotification.cpp \
    ril_socket/ImsSuppSvcStatus.cpp \
    ril_socket/ImsRequestConference.cpp \
    ril_socket/ImsRequestHoldResume.cpp \
    ril_socket/ImsRequestEmergencyCallbackMode.cpp \
    ril_socket/ImsColr.cpp \
    ril_socket/ImsVirtualLine.cpp \
    ril_socket/ImsSms.cpp \
    ril_socket/ImsCallWaiting.cpp \
    ril_socket/UimSimIo.cpp \
    ril_socket/UimGetSimStatus.cpp \
    ril_socket/UimOpenChannel.cpp \
    ril_socket/UimCloseChannel.cpp \
    ril_socket/UimTransmitApduBasic.cpp \
    ril_socket/UimTransmitApduChannel.cpp \
    ril_socket/UimGetImsi.cpp \
    ril_socket/UimChangePin.cpp \
    ril_socket/UimChangePin2.cpp \
    ril_socket/UimEnterPuk.cpp \
    ril_socket/UimEnterPuk2.cpp \
    ril_socket/UimEnterPin.cpp \
    ril_socket/UimEnterPin2.cpp \
    ril_socket/UimQueryFacilityLock.cpp \
    ril_socket/UimSetFacilityLock.cpp \
    ril_socket/UimIsimAuthentication.cpp \
    ril_socket/UimSimAuthentication.cpp \
    ril_socket/UimStkSendEnvelopeWithStatus.cpp\
    ril_socket/StartKeepAlive.cpp\
    ril_socket/SetPreferredDataModem.cpp\
    ril_socket/StopKeepAlive.cpp\
    ril_socket/RegisterDataUnsolIndicationFilter.cpp\
    ril_socket/CarrierInfoImsiEncryption.cpp\
    ril_socket/Sms.cpp \
    ril_socket/Oemhook.cpp\

endif

LOCAL_HEADER_LIBRARIES := qcrild_libqcrilnrutils-headers
LOCAL_HEADER_LIBRARIES += libqcrilNr-headers libqcrilNrQtiMutex-headers qcrilNrRadioconfig-headers qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNRLogger-headers
LOCAL_HEADER_LIBRARIES += libril-db-headers
LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libcutils \
    android.hardware.radio@1.0 \
    android.hardware.radio@1.1 \
    android.hardware.radio@1.2 \
    android.hardware.radio@1.3 \
    android.hardware.radio@1.4 \
    android.hardware.radio.deprecated@1.0 \
    android.hardware.radio.config@1.0 \
    android.hardware.radio.config@1.1 \
    libhidlbase \
    qcrild_libqcrilnrutils \
    libqcrilNrFramework \
    libqcrilNrLogger \
    libqcrilNrQtiMutex \
    qtiwakelock \
    qcrilMarshal

LOCAL_SHARED_LIBRARIES += android.hardware.radio@1.5

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE := $(qcril_sanitize)
LOCAL_NOSANITIZE := cfi
endif
LOCAL_CFLAGS += -Wno-unused-parameter -DRIL_SHLIB $(qcril_cflags)
LOCAL_CPPFLAGS += -std=c++17 $(qcril_cppflags) -fexceptions
LOCAL_CXXFLAGS += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS += $(qcril_ldflags)

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif

ifeq ($(SIM_COUNT), 2)
    LOCAL_CFLAGS += -DANDROID_MULTI_SIM -DDSDA_RILD1
    LOCAL_CFLAGS += -DANDROID_SIM_COUNT_2
endif

ifneq ($(DISABLE_RILD_OEM_HOOK), false)
    LOCAL_CFLAGS += -DOEM_HOOK_DISABLED
endif

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qcril-common/interfaces/inc/
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/../include

LOCAL_MODULE := qcrild_libqcrilnr
LOCAL_CLANG := true
LOCAL_SANITIZE += integer

include $(BUILD_STATIC_LIBRARY)

