/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimGetCardStatusRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#include <telephony/ril.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimGetSimStatusReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    (void)p;
    QCRIL_LOG_INFO("Dispatching Get Sim Status request.");

    auto msg = std::make_shared<UimGetCardStatusRequestMsg>(static_cast<uint8_t>(context->getRilInstanceId()));
    if (msg != nullptr) {

        GenericCallback<RIL_UIM_CardStatus> cb(
            [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<RIL_UIM_CardStatus> resp) -> void {

            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

            if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {

                errorCode = static_cast<RIL_Errno>(resp->err);

                auto p = std::make_shared<Marshal>();
                if(p && resp)
                {
                    RIL_CardStatus_v6 cardStatusInfo{};

                    cardStatusInfo.card_state = static_cast<RIL_CardState>(resp->card_state);
                    cardStatusInfo.physical_slot_id = resp->physical_slot_id;
                    cardStatusInfo.atr = const_cast<char *>(resp->atr.c_str());
                    cardStatusInfo.iccid = const_cast<char *>(resp->iccid.c_str());
                    cardStatusInfo.universal_pin_state = static_cast<RIL_PinState>(resp->universal_pin_state);
                    cardStatusInfo.gsm_umts_subscription_app_index =
                    resp->gsm_umts_subscription_app_index;
                    cardStatusInfo.cdma_subscription_app_index =
                    resp->cdma_subscription_app_index;
                    cardStatusInfo.ims_subscription_app_index =
                    resp->ims_subscription_app_index;
                    cardStatusInfo.num_applications = resp->num_applications;

                    for(uint32_t cnt = 0; cnt < cardStatusInfo.num_applications; cnt++)
                    {
                        cardStatusInfo.applications[cnt].app_type =
                        static_cast<RIL_AppType>(resp->applications[cnt].app_type);

                        cardStatusInfo.applications[cnt].app_state =
                        static_cast<RIL_AppState>(resp->applications[cnt].app_state);

                        cardStatusInfo.applications[cnt].perso_substate =
                        static_cast<RIL_PersoSubstate>(resp->applications[cnt].perso_substate);

                        cardStatusInfo.applications[cnt].aid_ptr =
                        const_cast<char *>(resp->applications[cnt].aid_ptr.c_str());

                        cardStatusInfo.applications[cnt].app_label_ptr =
                        const_cast<char *>(resp->applications[cnt].app_label_ptr.c_str());

                        cardStatusInfo.applications[cnt].pin1_replaced =
                        resp->applications[cnt].pin1_replaced;

                        cardStatusInfo.applications[cnt].pin1 =
                        static_cast<RIL_PinState>(resp->applications[cnt].pin1);

                        cardStatusInfo.applications[cnt].pin2 =
                        static_cast<RIL_PinState>(resp->applications[cnt].pin2);
                    }
                    cardStatusInfo.eid = const_cast<char*>(resp->eid.c_str());
                    QCRIL_LOG_INFO("sim status error code %d",errorCode);
                    p->write(cardStatusInfo);
                }
                sendResponse(context, errorCode, p);
            } else {
                sendResponse(context, errorCode, nullptr);
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
    }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
