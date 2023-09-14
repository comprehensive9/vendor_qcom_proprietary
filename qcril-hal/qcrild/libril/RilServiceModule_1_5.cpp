/******************************************************************************
#  Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RilServiceModule_1_5"
#include "RilServiceModule_1_5.h"

// Move to common files */
using MessageHandlerTable = std::unordered_map<message_id_t,
        std::function<void(std::shared_ptr<Message>)>>;

extern void updateMessageHandler(MessageHandlerTable& consolidateList,
        MessageHandlerTable& updateList);

RilServiceModule_1_5::RilServiceModule_1_5(qcril_instance_id_e_type instance,
        RadioImpl_1_5 &radioImpl): RilServiceModule_1_4(instance, radioImpl), mRadioImpl_1_5(radioImpl) {
    mName = "RilServiceModule_1_5";

    using std::placeholders::_1;
    MessageHandlerTable updateList = {
        HANDLER(QcrilInitMessage, RilServiceModule_1_5::handleQcrilInit),
        HANDLER(RilUnsolUiccAppsStatusChangedMessage, RilServiceModule_1_5::handleUiccAppsStatusChangedMessage),
        HANDLER(RilUnsolNwRegistrationRejectMessage, RilServiceModule_1_5::handleNwRegistrationRejectMessage),
        HANDLER(rildata::SetupDataCallRadioResponseIndMessage_1_5, RilServiceModule_1_5::handleSetupDataCallRadioResponseIndMessage),
        HANDLER(rildata::RadioDataCallListChangeIndMessage_1_5, RilServiceModule_1_5::handleRadioDataCallListChangeIndMessage),
        HANDLER(rildata::GetDataCallListResponseIndMessage, RilServiceModule_1_5::handleGetDataCallListResponseIndMessage),
    };
    updateMessageHandler(mMessageHandler, updateList);

    (void)mRadioImpl_1_5;
}

void RilServiceModule_1_5::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
    QCRIL_LOG_DEBUG("Handling QcrilInitMessage %s",msg->dump().c_str());
#ifdef QMI_RIL_UTF
    mRadioImpl_1_5.sendRadioClientCapabilities();
#endif
}

void RilServiceModule_1_5::handleNwRegistrationRejectMessage(
        std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg) {
    auto ri = getRadioImpl();
    if (ri) {
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();

        auto radioServiceRwlockPtr = radio::getRadioServiceRwlock(ri->mSlotId);
        radioServiceRwlockPtr->lock_shared();

        mRadioImpl_1_5.sendNwRegistrationReject(msg);

        radioServiceRwlockPtr->unlock_shared();
    }
}

void RilServiceModule_1_5::handleUiccAppsStatusChangedMessage(
        std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg) {
    auto ri = getRadioImpl();
    if (ri) {
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();

        auto radioServiceRwlockPtr = radio::getRadioServiceRwlock(ri->mSlotId);
        radioServiceRwlockPtr->lock_shared();

        mRadioImpl_1_5.sendUiccAppsStatusChanged(msg);

        radioServiceRwlockPtr->unlock_shared();
    }
}

void RilServiceModule_1_5::handleSetupDataCallRadioResponseIndMessage(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage_1_5> msg) {
    if (msg) {
        QCRIL_LOG_DEBUG("Handling handleSetupDataCallRadioResponseIndMessage %s",msg->dump().c_str());
        auto ri = getRadioImpl();
        if (ri) {
            auto radioServiceRwlockPtr = radio::getRadioServiceRwlock(ri->mSlotId);
            radioServiceRwlockPtr->lock_shared();
            auto rsp = msg->getResponse();
            auto status = msg->getStatus();
            auto serial = msg->getSerial();

            RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::NO_MEMORY};
            V1_5::SetupDataCallResult dcResult = {};
            dcResult.cause = V1_4::DataCallFailCause::ERROR_UNSPECIFIED;
            dcResult.suggestedRetryTime = -1;
            RadioError e = RadioError::NONE;
            if (status == Message::Callback::Status::SUCCESS) {
                dcResult = mRadioImpl_1_5.convertDcResultToHidlDcResult_1_5(rsp.call);
            }
            else {
                switch(rsp.respErr) {
                    case rildata::ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
                    case rildata::ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                    default: e = RadioError::GENERIC_FAILURE; break;
                }
            }
            responseInfo = {.serial = serial, .error = e};
            mRadioImpl_1_5.setupDataCallResponse_1_5(responseInfo, dcResult);

            radioServiceRwlockPtr->unlock_shared();
        }
    }
}

void RilServiceModule_1_5::handleRadioDataCallListChangeIndMessage(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage_1_5> msg) {
    if(msg) {
        QCRIL_LOG_DEBUG("Handling handleRadioDataCallListChangeIndMessage %s",msg->dump().c_str());

        auto ri = getRadioImpl();
        if (ri) {
            auto radioServiceRwlockPtr = radio::getRadioServiceRwlock(ri->mSlotId);
            radioServiceRwlockPtr->lock_shared();

            std::vector<rildata::DataCallResult_t> dcList = msg->getDCList();
            ::android::hardware::hidl_vec<V1_5::SetupDataCallResult> dcResultList;
            QCRIL_LOG_DEBUG("dcList %d",dcList.size());
            dcResultList.resize(dcList.size());
            int i=0;
            for (rildata::DataCallResult_t entry: dcList) {
                dcResultList[i] = mRadioImpl_1_5.convertDcResultToHidlDcResult_1_5(entry);
                i++;
            }
            mRadioImpl_1_5.dataCallListChanged_1_5(RadioIndicationType::UNSOLICITED, dcResultList);

            radioServiceRwlockPtr->unlock_shared();
        }
    }
    else {
        QCRIL_LOG_DEBUG("Handling handleRadioDataCallListChangeIndMessage is null");
    }
}

void RilServiceModule_1_5::handleGetDataCallListResponseIndMessage(std::shared_ptr<rildata::GetDataCallListResponseIndMessage> msg) {
    if(msg) {
        QCRIL_LOG_DEBUG("Handling handleGetDataCallListResponseIndMessage %s", msg->dump().c_str());
        mRadioImpl_1_5.getDataCallListResponse_1_5(msg->getDCList(), msg->getSerial(), msg->getStatus());
    } else {
        QCRIL_LOG_DEBUG("GetDataCallListResponseIndMessage is null");
    }
}
