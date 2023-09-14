/******************************************************************************
#  Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_service_1_5.h>
#include <framework/Log.h>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <RilServiceModule_1_4.h>
#include "interfaces/nas/RilUnsolUiccAppsStatusChangedMessage.h"
#include "interfaces/NwRegistration/RilUnsolNwRegistrationRejectMessage.h"
#include <UnSolMessages/RadioDataCallListChangeIndMessage.h>
#include <UnSolMessages/GetDataCallListResponseIndMessage.h>
#include <request/SetupDataCallRequestMessage.h>

class RilServiceModule_1_5: public RilServiceModule_1_4 {
    private:
        RadioImpl_1_5 &mRadioImpl_1_5;

    public:
        RilServiceModule_1_5(qcril_instance_id_e_type instance, RadioImpl_1_5 &radioImpl);
        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
        void handleUiccAppsStatusChangedMessage(
                std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg);
        void handleNwRegistrationRejectMessage(
                std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg);
        void handleSetupDataCallRadioResponseIndMessage(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage_1_5> msg);
        void handleRadioDataCallListChangeIndMessage(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage_1_5> msg);
        void handleGetDataCallListResponseIndMessage(std::shared_ptr<rildata::GetDataCallListResponseIndMessage> msg);
};

