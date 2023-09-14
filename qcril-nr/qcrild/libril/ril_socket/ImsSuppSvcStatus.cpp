/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <Marshal.h>
#include <interfaces/voice/QcRilRequestSetSupsServiceMessage.h>
#include <interfaces/voice/QcRilRequestQueryColpMessage.h>
#include <interfaces/voice/QcRilRequestGetCallBarringMessage.h>
#include <interfaces/ims/ims.h>
#include <ril_socket_api.h>
#include <ril_utils.h>
#include <telephony/ril_ims.h>
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

qcril::interfaces::FacilityType convertFacilityType(RIL_IMS_FacilityType& in) {
  qcril::interfaces::FacilityType out;
  switch (in) {
    case RIL_IMS_FACILITY_CLIP:
      out = qcril::interfaces::FacilityType::CLIP;
      break;
    case RIL_IMS_FACILITY_COLP:
      out = qcril::interfaces::FacilityType::COLP;
      break;
    case RIL_IMS_FACILITY_BAOC:
      out = qcril::interfaces::FacilityType::BAOC;
      break;
    case RIL_IMS_FACILITY_BAOIC:
      out = qcril::interfaces::FacilityType::BAOIC;
      break;
    case RIL_IMS_FACILITY_BAOICxH:
      out = qcril::interfaces::FacilityType::BAOICxH;
      break;
    case RIL_IMS_FACILITY_BAIC:
      out = qcril::interfaces::FacilityType::BAIC;
      break;
    case RIL_IMS_FACILITY_BAICr:
      out = qcril::interfaces::FacilityType::BAICr;
      break;
    case RIL_IMS_FACILITY_BA_ALL:
      out = qcril::interfaces::FacilityType::BA_ALL;
      break;
    case RIL_IMS_FACILITY_BA_MO:
      out = qcril::interfaces::FacilityType::BA_MO;
      break;
    case RIL_IMS_FACILITY_BA_MT:
      out = qcril::interfaces::FacilityType::BA_MT;
      break;
    case RIL_IMS_FACILITY_BS_MT:
      out = qcril::interfaces::FacilityType::BS_MT;
      break;
    case RIL_IMS_FACILITY_BAICa:
      out = qcril::interfaces::FacilityType::BAICa;
      break;
    default:
      out = qcril::interfaces::FacilityType::UNKNOWN;
      break;
  }
  return out;
}

void dispatchImsSuppSvcStatus(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  bool sendFailure = false;
  std::shared_ptr<QcRilRequestMessage> msg = nullptr;
  do {
    RIL_IMS_SuppSvcRequest suppSvcReq{};
    if (!p.dataAvail()) {
      QCRIL_LOG_ERROR("Empty data");
      sendFailure = true;
      break;
    }
    Marshal::Result ret = p.read(suppSvcReq);
    if (ret != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to read parameter");
      sendFailure = true;
      break;
    }

    switch (suppSvcReq.operationType) {
      case RIL_IMS_SUPP_OPERATION_REGISTER:
      case RIL_IMS_SUPP_OPERATION_ERASURE:
        if (suppSvcReq.facilityType != RIL_IMS_FACILITY_BS_MT) {
          QCRIL_LOG_INFO("Unsupported facility type %d for reg or erase", suppSvcReq.facilityType);
          break;
        }
        // fallthrough; use QcRilRequestSetSupsServiceMessage for REGISTER/ERASURE of BS_MT
        [[fallthrough]];
      case RIL_IMS_SUPP_OPERATION_ACTIVATE:
      case RIL_IMS_SUPP_OPERATION_DEACTIVATE:
      {
        auto setSupsMsg = std::make_shared<QcRilRequestSetSupsServiceMessage>(context);
        if (setSupsMsg == nullptr) {
          QCRIL_LOG_ERROR("setSupsMsg is nullptr");
          sendFailure = true;
          break;
        }
        // Set parameters
        setSupsMsg->setOperationType(suppSvcReq.operationType);
        setSupsMsg->setFacilityType(convertFacilityType(suppSvcReq.facilityType));
        const auto cbNumListInfo = suppSvcReq.callBarringNumbersListInfo;
        if (cbNumListInfo) {
          setSupsMsg->setServiceClass(cbNumListInfo->serviceClass);
          if (suppSvcReq.facilityType == RIL_IMS_FACILITY_BS_MT) {
            if (cbNumListInfo->callBarringNumbersInfoLen > 0 &&
                cbNumListInfo->callBarringNumbersInfo != nullptr) {
              std::vector<std::string> cbNumList;
              for (uint32_t i = 0; i < cbNumListInfo->callBarringNumbersInfoLen; ++i) {
                if (cbNumListInfo->callBarringNumbersInfo[i].number) {
                  std::string num = cbNumListInfo->callBarringNumbersInfo[i].number;
                  cbNumList.push_back(num);
                }
              }
              setSupsMsg->setCallBarringNumberList(cbNumList);
            }
          }
        }
        if (suppSvcReq.password != nullptr) {
          setSupsMsg->setPassword(suppSvcReq.password);
        }
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
              RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
              RIL_IMS_SuppSvcResponse suppSvcResp{};
              if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                errorCode = resp->errorCode;
                std::shared_ptr<qcril::interfaces::SipErrorInfo> errorDetails =
                    std::static_pointer_cast<qcril::interfaces::SipErrorInfo>(resp->data);
                if (errorDetails) {
                  ril::socket::utils::convertToSocket(suppSvcResp.errorDetails, errorDetails);
                }
              }
              QCRIL_LOG_DEBUG("dispatchImsSuppSvcStatus:resp: errorCode=%d", errorCode);
              auto p = std::make_shared<Marshal>();
              p->write(suppSvcResp);
              sendResponse(context, errorCode, p);
            });
        setSupsMsg->setCallback(&cb);
        msg = setSupsMsg;
      }
      break;
      case RIL_IMS_SUPP_OPERATION_QUERY:
        if (suppSvcReq.facilityType == RIL_IMS_FACILITY_COLP) {
          // QcRilRequestQueryColpMessage
          auto queryColp = std::make_shared<QcRilRequestQueryColpMessage>(context);
          if (queryColp == nullptr) {
            QCRIL_LOG_ERROR("queryColp is nullptr");
            sendFailure = true;
            break;
          }
          GenericCallback<QcRilRequestMessageCallbackPayload> cb(
              [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                RIL_IMS_SuppSvcResponse suppSvcResp{};
                std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> data = nullptr;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                  errorCode = resp->errorCode;
                  data = std::static_pointer_cast<qcril::interfaces::SuppServiceStatusInfo>(
                              resp->data);
                }
                ril::socket::utils::convertToSocket(suppSvcResp, data);
                QCRIL_LOG_DEBUG("dispatchImsSuppSvcStatus:resp: errorCode=%d", errorCode);
                auto p = std::make_shared<Marshal>();
                p->write(suppSvcResp);
                sendResponse(context, errorCode, p);
              });
          queryColp->setCallback(&cb);
          msg = queryColp;
        } else if (suppSvcReq.facilityType != RIL_IMS_FACILITY_CLIP) {
          // QcRilRequestGetCallBarringMessage
          auto queryMsg = std::make_shared<QcRilRequestGetCallBarringMessage>(context);
          if (queryMsg == nullptr) {
            QCRIL_LOG_ERROR("queryMsg is nullptr");
            sendFailure = true;
            break;
          }
          // Set parameters
          queryMsg->setFacilityType(convertFacilityType(suppSvcReq.facilityType));
          const auto cbNumListInfo = suppSvcReq.callBarringNumbersListInfo;
          if (cbNumListInfo) {
            queryMsg->setServiceClass(cbNumListInfo->serviceClass);
          }
          GenericCallback<QcRilRequestMessageCallbackPayload> cb(
              [context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                RIL_IMS_SuppSvcResponse suppSvcResp{};
                std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> data = nullptr;
                if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                  errorCode = resp->errorCode;
                  data = std::static_pointer_cast<qcril::interfaces::SuppServiceStatusInfo>(
                              resp->data);
                }
                ril::socket::utils::convertToSocket(suppSvcResp, data);
                QCRIL_LOG_DEBUG("dispatchImsSuppSvcStatus:resp: errorCode=%d", errorCode);
                auto p = std::make_shared<Marshal>();
                p->write(suppSvcResp);
                sendResponse(context, errorCode, p);
              });
          queryMsg->setCallback(&cb);
          msg = queryMsg;
        }
        break;
      default:
        break;
    }
    if (msg) {
      msg->setIsImsRequest(true);
      msg->dispatch();
    }
  } while (FALSE);

  if (sendFailure) {
    sendResponse(context, RIL_E_NO_MEMORY, nullptr);
  }

  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
