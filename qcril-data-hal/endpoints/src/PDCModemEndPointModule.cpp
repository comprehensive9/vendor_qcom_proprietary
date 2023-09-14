/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string>
#include "PDCModemEndPointModule.h"
#include "PDCRefreshIndication.h"
#include <modules/qmi/EndpointStatusIndMessage.h>
#include <modules/qmi/QmiIndMessage.h>

using namespace rildata;
extern int global_instance_id;

PDCModemEndPointModule::PDCModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner)
{
  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;

  mMessageHandler = {
      {REG_MSG("PDCModemEndPoint_QMI_IND"), std::bind(&PDCModemEndPointModule::handleQmiPDCIndMessage, this, _1)},
  };
}

PDCModemEndPointModule::~PDCModemEndPointModule()
{
    mLooper = nullptr;
}

void PDCModemEndPointModule::init() {
    ModemEndPointModule::init();
}

qmi_idl_service_object_type PDCModemEndPointModule::getServiceObject() {
    return pdc_get_service_object_v01();
}

void PDCModemEndPointModule::handleQmiPDCIndMessage(std::shared_ptr<Message> msg)
{
    auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
    Log::getInstance().d("[PDCModemEndPointModule]: Handling msg = " + msg->dump());

    QmiIndMsgDataStruct *indData = shared_indMsg->getData();
    if (indData != nullptr) {
        indicationHandler(indData->msgId, indData->indData, indData->indSize);
    }
    else {
        Log::getInstance().d("Unexpected, null data from message");
    }
}

void PDCModemEndPointModule::indicationHandler
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
) {
    (void)decoded_payload_len;
    Log::getInstance().d("[PDCModemEndPointModule]: indicationHandler ENTRY");
    Log::getInstance().d("msg_id = "+ std::to_string(msg_id));
    switch(msg_id)
    {
    case QMI_PDC_REFRESH_IND_V01:
    {
        pdc_refresh_ind_msg_v01* ind = (pdc_refresh_ind_msg_v01*)decoded_payload;
        if (ind == nullptr) {
            Log::getInstance().d("ind is nullptr");
            return;
        }
        handlePDCRefreshInd(*ind);
        break;
    }
    default:
        Log::getInstance().d("Unexpected pdc indication, ignoring = "+ std::to_string(msg_id));
        break;
    }
}

bool isCurrentSubscription(uint32_t subscription_id)
{
  uint32_t currentSub = 0;
  switch (global_instance_id) {
      case QCRIL_DEFAULT_INSTANCE_ID: currentSub = 0; break;
      case QCRIL_SECOND_INSTANCE_ID: currentSub = 1; break;
      case QCRIL_THIRD_INSTANCE_ID: currentSub = 2; break;
      default: break;
  }
  return currentSub == subscription_id;
}

void PDCModemEndPointModule::handlePDCRefreshInd(const pdc_refresh_ind_msg_v01& ind)
{
    std::stringstream ss;
    ss << "handlePDCRefreshInd():";
    if (ind.subscription_id_valid) {
        ss << " sub=" << ind.subscription_id;
    }
    if (ind.slot_id_valid) {
        ss << " slot=" << ind.slot_id;
    }
    Log::getInstance().d(ss.str());
    switch (ind.refresh_event) {
        case PDC_EVENT_REFRESH_START_V01:
        case PDC_EVENT_REFRESH_COMPLETE_V01:
            if (ind.subscription_id_valid && isCurrentSubscription(ind.subscription_id)) {
                auto msg = std::make_shared<PDCRefreshIndication>(ind.refresh_event);
                msg->broadcast();
            }
            break;
        case PDC_EVENT_CLIENT_REFRESH_V01:
            sendPdcNotificationAck(ind);
            break;
        default:
            Log::getInstance().d("[PDCModemEndPointModule]: Unhandled event type");
            break;
    }
}

void PDCModemEndPointModule::sendPdcNotificationAck(const pdc_refresh_ind_msg_v01& ind)
{
    if (mQmiSvcClient != nullptr)
    {
        pdc_notification_req_msg_v01 req;
        pdc_notification_resp_msg_v01 resp;

        memset(&req, 0, sizeof(req));
        memset(&resp, 0, sizeof(resp));

        req.client_refresh_done_valid = TRUE;
        req.client_refresh_done = 1;
        req.subscription_id_valid = ind.subscription_id_valid;
        req.subscription_id = ind.subscription_id;
        req.slot_id_valid = ind.slot_id_valid;
        req.slot_id = ind.slot_id;

        auto rc = qmi_client_send_msg_sync (mQmiSvcClient,
                                           QMI_PDC_NOTIFICATION_REQ_V01,
                                           &req,
                                           sizeof(req),
                                           (void*)&resp,
                                           sizeof(resp),
                                           DEFAULT_SYNC_TIMEOUT);

        if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
        {
           Log::getInstance().d("sendPdcNotificationAck(): failed with rc="+ std::to_string(rc) +", qmi_err=" + std::to_string(resp.resp.error));
        }
    }
}
