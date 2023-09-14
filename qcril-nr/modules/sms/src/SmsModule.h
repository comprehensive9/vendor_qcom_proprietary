/******************************************************************************
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#pragma once

#include <framework/Module.h>
#include <framework/Dispatcher.h>
#include <framework/AddPendingMessageList.h>
#include <modules/qmi/SmsModemEndPoint.h>
#include <modules/qmi/QmiAsyncResponseMessage.h>
#include <modules/android/RilRequestMessage.h>
#include <interfaces/sms/RilRequestSendSmsMessage.h>
#include <interfaces/sms/RilRequestWriteSmsToSimMessage.h>
#include <interfaces/sms/RilRequestGsmSetBroadcastSmsConfigMessage.h>
#include <interfaces/sms/RilRequestCdmaSendSmsMessage.h>
#include <interfaces/sms/RilRequestCdmaSetBroadcastSmsConfigMessage.h>
#include <interfaces/sms/RilRequestImsSendSmsMessage.h>
#include <interfaces/sms/RilRequestAckGsmSmsMessage.h>
#include <interfaces/sms/RilRequestAckCdmaSmsMessage.h>
#include <interfaces/sms/RilRequestDeleteSmsOnSimMessage.h>
#include <interfaces/sms/RilRequestCdmaWriteSmsToRuimMessage.h>
#include <interfaces/sms/RilRequestCdmaDeleteSmsOnRuimMessage.h>
#include <interfaces/sms/RilRequestGetSmscAddressMessage.h>
#include <interfaces/sms/RilRequestSetSmscAddressMessage.h>
#include <interfaces/sms/RilRequestGetGsmBroadcastConfigMessage.h>
#include <interfaces/sms/RilRequestGetCdmaBroadcastConfigMessage.h>
#include <interfaces/sms/RilRequestGsmSmsBroadcastActivateMessage.h>
#include <interfaces/sms/RilRequestCdmaSmsBroadcastActivateMessage.h>
#include <interfaces/sms/RilRequestReportSmsMemoryStatusMessage.h>
#include <interfaces/sms/RilRequestGetImsRegistrationMessage.h>
#include <interfaces/sms/RilRequestAckImsSmsMessage.h>
#include <modules/sms/SmsGetImsServiceStatusMessage.h>

class SmsModule: public Module, public AddPendingMessageList {
    public:
        SmsModule();
        ~SmsModule();
        void init();
#ifdef QMI_RIL_UTF
        void qcrilHalSmsModuleCleanup();
#endif

    private:
        bool mReady = false;
        qtimutex::QtiRecursiveMutex mMutex;

        void handleQcrilInit(std::shared_ptr<Message> msg);
        void handleSmsQmiIndMessage(std::shared_ptr<Message> msg);
        void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);

        void handleSendSms(std::shared_ptr<RilRequestSendSmsMessage> msg);
        void handleSmsAcknowledge(std::shared_ptr<RilRequestAckGsmSmsMessage> msg);
        void handleWriteSmsToSim(std::shared_ptr<RilRequestWriteSmsToSimMessage> msg);
        void handleDeleteSmsOnSim(std::shared_ptr<RilRequestDeleteSmsOnSimMessage> msg);
        void handleGetSmscAddress(std::shared_ptr<RilRequestGetSmscAddressMessage> msg);
        void handleSetSmscAddress(std::shared_ptr<RilRequestSetSmscAddressMessage> msg);
        void handleReportSmsMemoryStatus(std::shared_ptr<RilRequestReportSmsMemoryStatusMessage> msg);
        void handleGsmGetBroadcastSmsConfig(std::shared_ptr<RilRequestGetGsmBroadcastConfigMessage> msg);
        void handleGsmSetBroadcastSmsConfig(std::shared_ptr<RilRequestGsmSetBroadcastSmsConfigMessage> msg);
        void handleGsmSmsBroadcastActivation(std::shared_ptr<RilRequestGsmSmsBroadcastActivateMessage> msg);
        void handleCdmaSendSms(std::shared_ptr<RilRequestCdmaSendSmsMessage> msg);
        void handleCdmaSmsAcknowledge(std::shared_ptr<RilRequestAckCdmaSmsMessage> msg);
        void handleCdmaWriteSmsToRuim(std::shared_ptr<RilRequestCdmaWriteSmsToRuimMessage> msg);
        void handleCdmaDeleteSmsOnRuim(std::shared_ptr<RilRequestCdmaDeleteSmsOnRuimMessage> msg);
        void handleCdmaGetBroadcastSmsConfig(std::shared_ptr<RilRequestGetCdmaBroadcastConfigMessage> msg);
        void handleCdmaSetBroadcastSmsConfig(std::shared_ptr<RilRequestCdmaSetBroadcastSmsConfigMessage> msg);
        void handleCdmaSmsBroadcastActivation(std::shared_ptr<RilRequestCdmaSmsBroadcastActivateMessage> msg);
        void handleImsRegistrationState(std::shared_ptr<RilRequestGetImsRegistrationMessage> msg);
        void handleImsSendSms(std::shared_ptr<RilRequestImsSendSmsMessage> msg);
        void handleImsSmsAcknowledge(std::shared_ptr<RilRequestAckImsSmsMessage> msg);
        void handleSmsEndPointStatusIndMessage(std::shared_ptr<Message> msg);
        void handleSmsGetImsServiceStatusMessage(std::shared_ptr<SmsGetImsServiceStatusMessage> msg);
};

SmsModule& getSmsModule();
