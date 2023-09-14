/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "NwRegistrationModule"

#include <limits.h>

// Framework headers
#include <framework/Module.h>
#include <framework/add_message_id.h>
#include <framework/SolicitedMessage.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/AddPendingMessageList.h>

// Module headers
#include "telephony/ril.h"
#include <modules/nas/qcril_qmi_nas.h>
#include <modules/nas/NasRegRejUnsolMessage.h>
#include <modules/qmi/QmiIndMessage.h>
#include <modules/android/RilRequestMessage.h>
#include <modules/qmi/EndpointStatusIndMessage.h>

#include <interfaces/NwRegistration/RilUnsolNwRegistrationRejectMessage.h>

// Data headers
#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"

// QMI headers
#include <network_access_service_v01.h>

class NwRegistrationModule: public Module, public AddPendingMessageList
{
    private:
        bool mReady = false;
        bool reportRejInd = false;
        RilNwRegistrationRejectInfo_t rejInfo = {};
        qtimutex::QtiRecursiveMutex mMutex;
        void qcril_qmi_nwreg_process_nas_registration_info(
            RilNwRegistrationRejectInfo_t &nas_reg_info);
#ifndef QMI_RIL_UTF
        void qcril_qmi_nwreg_handle_data_reg_failure_cause_code_unsol_message(
            std::shared_ptr<rildata::DataRegistrationFailureCauseMessage> msg);
        void handleRegistrationFailureIndicationFilterMessage(
            std::shared_ptr<rildata::RegistrationFailureReportingStatusMessage> msg);
#endif
    public:
        NwRegistrationModule();
        void handleNasEndpointStatusIndMessage(std::shared_ptr<Message> msg);
        void handleNasRegRejUnsolMessage(
            std::shared_ptr<Message> msg);
        void handleDataRegistrationFailureCauseMessage(
            std::shared_ptr<rildata::DataRegistrationFailureCauseMessage> msg);
};

static load_module<NwRegistrationModule> the_module;

NwRegistrationModule::NwRegistrationModule() : AddPendingMessageList("NwRegistrationModule")
{
    rejInfo.causeCode = INT32_MAX;
    rejInfo.additionalCauseCode = INT32_MAX;
    mName = "NwRegistrationModule";
    using std::placeholders::_1;

    mMessageHandler = {
        {REG_MSG("NAS_ENDPOINT_STATUS_IND"),
            std::bind(&NwRegistrationModule::handleNasEndpointStatusIndMessage, this, _1)},
         HANDLER(rildata::DataRegistrationFailureCauseMessage,
                 NwRegistrationModule::handleDataRegistrationFailureCauseMessage),
#ifndef QMI_RIL_UTF
         HANDLER(rildata::RegistrationFailureReportingStatusMessage,
                 NwRegistrationModule::handleRegistrationFailureIndicationFilterMessage),
#endif
         HANDLER(NasRegRejUnsolMessage,
                 NwRegistrationModule::handleNasRegRejUnsolMessage),
    };
}

void NwRegistrationModule::handleNasEndpointStatusIndMessage(
    std::shared_ptr<Message> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

    if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
        if (!mReady) {
            mReady = true;
        }
    } else {
        // state is not operational
        mReady = false;
        // invalidate reg reject information
        memset(&rejInfo, 0, sizeof(rejInfo));
        rejInfo.causeCode = INT32_MAX;
        rejInfo.additionalCauseCode = INT32_MAX;
    }
}

void NwRegistrationModule::handleNasRegRejUnsolMessage(
    std::shared_ptr<Message> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    auto shared_indMsg(std::static_pointer_cast<NasRegRejUnsolMessage>(msg));

    if (shared_indMsg != nullptr) {
        qcril_qmi_nwreg_process_nas_registration_info(
            shared_indMsg->getNasRegistrationInfo());
    }
}

#ifndef QMI_RIL_UTF
void NwRegistrationModule::handleRegistrationFailureIndicationFilterMessage(
    std::shared_ptr<rildata::RegistrationFailureReportingStatusMessage> msg){
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    reportRejInd = msg->getReportingStatus();
}
#endif

void NwRegistrationModule::qcril_qmi_nwreg_process_nas_registration_info(
    RilNwRegistrationRejectInfo_t &nas_reg_info) {

    RilNwRegistrationRejectInfo_t oldRejInfo;
    memcpy(&oldRejInfo, &rejInfo, sizeof(oldRejInfo));

    if (nas_reg_info.rilCellIdentity_valid) {
        // Fill cellIdentity, choosen plmn information from NAS information
        memcpy(&rejInfo.rilCellIdentity,
            &nas_reg_info.rilCellIdentity,
            sizeof(nas_reg_info.rilCellIdentity));

        snprintf(rejInfo.choosenPlmn,
            sizeof(rejInfo.choosenPlmn),
            "%s%s",
            rejInfo.rilCellIdentity.cellIdentityGsm.mcc,
            rejInfo.rilCellIdentity.cellIdentityGsm.mnc);
    }

    if (nas_reg_info.causeCode_valid) {
        rejInfo.causeCode_valid = true;
        rejInfo.causeCode = nas_reg_info.causeCode;
        rejInfo.domain = nas_reg_info.domain;
    } else {
        rejInfo.causeCode_valid = false;
        rejInfo.causeCode = INT32_MAX;
        rejInfo.domain &= ~((uint64_t)ServiceDomain::CS_DOMAIN);
    }

    if (nas_reg_info.additionalCauseCode_valid) {
        rejInfo.additionalCauseCode_valid = true;
        rejInfo.additionalCauseCode = nas_reg_info.additionalCauseCode;
    }

    // Don't send unsol unless atleasat one cause code is valid and there is change
    if( reportRejInd &&
        (rejInfo.causeCode_valid || rejInfo.additionalCauseCode_valid) &&
        memcmp(&oldRejInfo, &rejInfo, sizeof(oldRejInfo)) != 0) {
        auto msg = std::make_shared<RilUnsolNwRegistrationRejectMessage>(rejInfo);
        if (msg != nullptr) {
            Dispatcher::getInstance().dispatchSync(msg);
        } else {
            Log::getInstance().d("[" + mName + "]: No Memory! ");
        }
    }
}

#ifndef QMI_RIL_UTF
void NwRegistrationModule::qcril_qmi_nwreg_handle_data_reg_failure_cause_code_unsol_message(
    std::shared_ptr<rildata::DataRegistrationFailureCauseMessage> msg) {

    RilNwRegistrationRejectInfo_t oldRejInfo;
    memcpy(&oldRejInfo, &rejInfo, sizeof(oldRejInfo));

    if ((msg != nullptr)) {
        if (msg->isRejectCauseValid()) {
            rejInfo.domain |= (uint64_t)ServiceDomain::PS_DOMAIN;
            rejInfo.additionalCauseCode_valid = true;
            rejInfo.additionalCauseCode = msg->getRejectCause();

            // set these flags to ensure that we don't send invalid cellIdentity to telephony
            rejInfo.rilCellIdentity_valid = TRUE;
            rejInfo.rilCellIdentity.cellInfoType = RIL_CELL_INFO_TYPE_LTE;
        } else {
            rejInfo.additionalCauseCode_valid = false;
            rejInfo.additionalCauseCode = INT32_MAX;
            // clear PS_DOMAIN
            rejInfo.domain &= ~((uint64_t)ServiceDomain::PS_DOMAIN);
        }

        if (reportRejInd &&
            (rejInfo.causeCode_valid || rejInfo.additionalCauseCode_valid) &&
            (memcmp(&oldRejInfo, &rejInfo, sizeof(oldRejInfo)) != 0)){
            auto msg = std::make_shared<RilUnsolNwRegistrationRejectMessage>(rejInfo);
            if (msg != nullptr) {
                Dispatcher::getInstance().dispatchSync(msg);
            } else {
                Log::getInstance().d("[" + mName + "]: No Memory! ");
            }
        }
    }
}
#endif

void NwRegistrationModule::handleDataRegistrationFailureCauseMessage(
    std::shared_ptr<rildata::DataRegistrationFailureCauseMessage> shared_msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + shared_msg->dump());
#ifndef QMI_RIL_UTF
    qcril_qmi_nwreg_handle_data_reg_failure_cause_code_unsol_message(shared_msg);
#endif
}
