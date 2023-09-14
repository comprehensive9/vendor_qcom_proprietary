/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/nas/RilRequestSetPrefNetworkTypeMessage.h>
#include <interfaces/nas/RilRequestGetPrefNetworkTypeMessage.h>
#include <interfaces/nas/nas_types.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

uint32_t convertNwTypeToRadioAccessFamily(RIL_PreferredNetworkType nwType) {
    uint32_t radioAccessFamily = 0;
    switch (nwType) {
        case PREF_NET_TYPE_GSM_WCDMA:
            radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_GSM_ONLY:
            radioAccessFamily = QCRIL_RAF_GSM;
            break;
        case PREF_NET_TYPE_WCDMA:
            radioAccessFamily = QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_GSM_WCDMA_AUTO:
            radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_AUTO;
            break;
        case PREF_NET_TYPE_CDMA_EVDO_AUTO:
            radioAccessFamily = QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
            break;
        case PREF_NET_TYPE_CDMA_ONLY:
            radioAccessFamily = QCRIL_RAF_CDMA;
            break;
        case PREF_NET_TYPE_EVDO_ONLY:
            radioAccessFamily = QCRIL_RAF_EVDO;
            break;
        case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
            break;
        case PREF_NET_TYPE_LTE_CDMA_EVDO:
            radioAccessFamily = QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
            break;
        case PREF_NET_TYPE_LTE_GSM_WCDMA:
            radioAccessFamily = QCRIL_RAF_LTE | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
            radioAccessFamily = QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_LTE_ONLY:
            radioAccessFamily = QCRIL_RAF_LTE;
            break;
        case PREF_NET_TYPE_LTE_WCDMA:
            radioAccessFamily = QCRIL_RAF_LTE | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_TD_SCDMA_ONLY:
            radioAccessFamily = RAF_TD_SCDMA;
            break;
        case PREF_NET_TYPE_TD_SCDMA_WCDMA:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_TD_SCDMA_LTE:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_LTE;
            break;
        case PREF_NET_TYPE_TD_SCDMA_GSM:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM;
            break;
        case PREF_NET_TYPE_TD_SCDMA_GSM_LTE:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_LTE;
            break;
        case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE;
            break;
        case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE;
            break;
        case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO;
            break;
        case PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
            radioAccessFamily = RAF_TD_SCDMA | QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_NR5G_ONLY:
            radioAccessFamily = RAF_5G;
            break;
        case PREF_NET_TYPE_NR5G_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
            radioAccessFamily = RAF_5G | RAF_TD_SCDMA | QCRIL_RAF_LTE | QCRIL_RAF_CDMA |
                QCRIL_RAF_EVDO | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA;
            break;
        case PREF_NET_TYPE_GSM_WCDMA_LTE_NR5G:
            radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE | RAF_5G;
            break;
        case PREF_NET_TYPE_GSM_WCDMA_NR5G:
            radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | RAF_5G;
            break;
        case PREF_NET_TYPE_GSM_LTE_NR5G:
            radioAccessFamily = QCRIL_RAF_GSM | QCRIL_RAF_LTE | RAF_5G;
            break;
        case PREF_NET_TYPE_WCDMA_LTE_NR5G:
            radioAccessFamily = QCRIL_RAF_WCDMA | QCRIL_RAF_LTE | RAF_5G;
            break;
        case PREF_NET_TYPE_GSM_NR5G:
            radioAccessFamily = QCRIL_RAF_GSM | RAF_5G;
            break;
        case PREF_NET_TYPE_WCDMA_NR5G:
            radioAccessFamily = QCRIL_RAF_WCDMA | RAF_5G;
            break;
        case PREF_NET_TYPE_LTE_NR5G:
            radioAccessFamily = QCRIL_RAF_LTE | RAF_5G;
            break;
        default:
            radioAccessFamily = RAF_UNKNOWN;
            break;
    }
    QCRIL_LOG_INFO("radioAccessFamily - %d", radioAccessFamily);
    return radioAccessFamily;
}

RIL_PreferredNetworkType convertRadioAccessFamilyToNwType(int radioAccessFamily) {
  RIL_PreferredNetworkType nwType = PREF_NET_TYPE_GSM_WCDMA;

  switch (radioAccessFamily) {
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_GSM_WCDMA;
      break;
    case QCRIL_RAF_GSM:
      nwType = PREF_NET_TYPE_GSM_ONLY;
      break;
    case QCRIL_RAF_WCDMA:
      nwType = PREF_NET_TYPE_WCDMA;
      break;
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_AUTO):
      nwType = PREF_NET_TYPE_GSM_WCDMA_AUTO;
      break;
    case (QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
      break;
    case QCRIL_RAF_CDMA:
      nwType = PREF_NET_TYPE_CDMA_ONLY;
      break;
    case QCRIL_RAF_EVDO:
      nwType = PREF_NET_TYPE_EVDO_ONLY;
      break;
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = PREF_NET_TYPE_LTE_CDMA_EVDO;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_LTE_GSM_WCDMA;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA;
      break;
    case (QCRIL_RAF_LTE):
      nwType = PREF_NET_TYPE_LTE_ONLY;
      break;
    case (QCRIL_RAF_LTE | QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_LTE_WCDMA;
      break;
    case RAF_TD_SCDMA:
      nwType = PREF_NET_TYPE_TD_SCDMA_ONLY;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_TD_SCDMA_WCDMA;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_LTE):
      nwType = PREF_NET_TYPE_TD_SCDMA_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM):
      nwType = PREF_NET_TYPE_TD_SCDMA_GSM;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_LTE):
      nwType = PREF_NET_TYPE_TD_SCDMA_GSM_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE):
      nwType = PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE):
      nwType = PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO):
      nwType = PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO;
      break;
    case (RAF_TD_SCDMA | QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM |
          QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
      break;
    case RAF_5G:
      nwType = PREF_NET_TYPE_NR5G_ONLY;
      break;
    case (RAF_5G | RAF_TD_SCDMA | QCRIL_RAF_LTE | QCRIL_RAF_CDMA | QCRIL_RAF_EVDO | QCRIL_RAF_GSM | QCRIL_RAF_WCDMA):
      nwType = PREF_NET_TYPE_NR5G_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
      break;
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | QCRIL_RAF_LTE | RAF_5G):
      nwType = PREF_NET_TYPE_GSM_WCDMA_LTE_NR5G;
      break;
    case (QCRIL_RAF_GSM | QCRIL_RAF_WCDMA | RAF_5G):
      nwType = PREF_NET_TYPE_GSM_WCDMA_NR5G;
      break;
    case (QCRIL_RAF_GSM | QCRIL_RAF_LTE | RAF_5G):
      nwType = PREF_NET_TYPE_GSM_LTE_NR5G;
      break;
    case (QCRIL_RAF_WCDMA | QCRIL_RAF_LTE | RAF_5G):
      nwType = PREF_NET_TYPE_WCDMA_LTE_NR5G;
      break;
    case (QCRIL_RAF_GSM | RAF_5G):
      nwType = PREF_NET_TYPE_GSM_NR5G;
      break;
    case (QCRIL_RAF_WCDMA | RAF_5G):
      nwType = PREF_NET_TYPE_WCDMA_NR5G;
      break;
    case (QCRIL_RAF_LTE | RAF_5G):
      nwType = PREF_NET_TYPE_LTE_NR5G;
      break;
    default:
      nwType = PREF_NET_TYPE_GSM_WCDMA;
      break;
  }
  QCRIL_LOG_INFO("nwType - %d", nwType);
  return nwType;
}

void dispatchSetNetworkType(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_INFO("Dispatching set_preferred_network_type request.");
  bool sendFailure = false;
  RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

  do {
    int bitMap;
    p.read(bitMap);
    uint32_t radioAccessFamily = convertNwTypeToRadioAccessFamily((RIL_PreferredNetworkType)bitMap);
    auto msg = std::make_shared<RilRequestSetPrefNetworkTypeMessage>(context, radioAccessFamily);
    if (msg == nullptr) {
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (false);

  if (sendFailure) {
    sendResponse(context, errorCode, nullptr);
  }
}

void dispatchGetNetworkType(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_INFO("Dispatching set_preferred_network_type request.");
  bool sendFailure = false;
  RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;

  do {
    auto msg = std::make_shared<RilRequestGetPrefNetworkTypeMessage>(context);
    if (msg == nullptr) {
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> msg, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {

          std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult;
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            RIL_PreferredNetworkType nwType = PREF_NET_TYPE_GSM_WCDMA;
            prefResult =
                std::static_pointer_cast<qcril::interfaces::RilGetModePrefResult_t>(resp->data);
            if (prefResult != nullptr) {
              nwType = convertRadioAccessFamilyToNwType(prefResult->pref);
            }
            auto p = std::make_shared<Marshal>();
            p->write(static_cast<int>(nwType));
            sendResponse(context, errorCode, p);
          } else {
            sendResponse(context, errorCode, nullptr);
          }
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (false);

  if (sendFailure) {
    sendResponse(context, errorCode, nullptr);
  }
}

}  // namespace api
}  // namespace socket
}  // namespace ril
