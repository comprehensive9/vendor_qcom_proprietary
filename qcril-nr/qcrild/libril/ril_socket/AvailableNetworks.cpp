/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>

#include <interfaces/nas/RilRequestQueryAvailNetworkMessage.h>
#include <interfaces/nas/nas_types.h>
#undef TAG
#define TAG "RILQ"

namespace ril::socket::api {

RIL_OperatorStatus convertStatusType(qcril::interfaces::NetworkStatus in) {
    RIL_OperatorStatus ret = RIL_OPERATOR_UNKNOWN;
    switch (in)
    {
      case qcril::interfaces::NetworkStatus::UNKNOWN:
        ret = RIL_OPERATOR_UNKNOWN;
        break;
      case qcril::interfaces::NetworkStatus::AVAILABLE:
        ret = RIL_OPERATOR_AVAILABLE;
        break;
      case qcril::interfaces::NetworkStatus::CURRENT:
        ret = RIL_OPERATOR_CURRENT;
        break;
      case qcril::interfaces::NetworkStatus::FORBIDDEN:
        ret = RIL_OPERATOR_FORBIDDEN;
        break;
      default:
        ret = RIL_OPERATOR_UNKNOWN;
    }
    return ret;
}


void dispatchQueryAvailableNetworks(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  bool sendFailure = false;

  do {
    auto msg = std::make_shared<RilRequestQueryAvailNetworkMessage>(context);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            std::vector<RIL_OperatorInfo> networks;
            auto rilGetNetworksResult =
                std::static_pointer_cast<qcril::interfaces::RilGetAvailNetworkResult_t>(resp->data);
            auto p = std::make_shared<Marshal>();
            if (rilGetNetworksResult) {
              for(auto &i:rilGetNetworksResult->info) {
                RIL_OperatorInfo info{nullptr, nullptr, nullptr, RIL_OPERATOR_UNKNOWN};
                info.alphaLong = new char[i.alphaLong.size()+1]{0};
                if(info.alphaLong) {
                  i.alphaLong.copy(info.alphaLong, i.alphaLong.size()+1);
                }
                info.alphaShort = new char[i.alphaShort.size()+1]{0};
                if(info.alphaShort) {
                  i.alphaShort.copy(info.alphaShort, i.alphaShort.size()+1);
                }
                info.operatorNumeric = new char[i.operatorNumeric.size()+1]{0};
                if(info.operatorNumeric) {
                  i.operatorNumeric.copy(info.operatorNumeric, i.operatorNumeric.size()+1);
                }
                info.status = convertStatusType(i.status);
                networks.push_back(info);
              }
              int32_t numOfNetworks = networks.size();
              if (numOfNetworks) {
                if(p) {
                  p->write(numOfNetworks);
                  for (auto nw : networks) {
                    p->write(nw);
                  }
                }

                // delete allocated memory
                for(auto nw : networks) {
                  delete[] nw.alphaLong;
                  delete[] nw.alphaShort;
                  delete[] nw.operatorNumeric;
                }
              }
            }
            sendResponse(context, errorCode, p);
          } else {
            sendResponse(context, errorCode, nullptr);
          }
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace ril::socket::api
