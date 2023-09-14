/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/uim/UimGetImsiRequestMsg.h>
#include <interfaces/uim/qcril_uim_types.h>
#undef TAG
#define TAG "RILQ"
#include <framework/Log.h>

namespace ril {
namespace socket {
namespace api {

void dispatchSimGetImsiReq(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
    QCRIL_LOG_INFO("Dispatching Get IMSI request.");

    char** params = nullptr;
    size_t sz = 0;
    p.readAndAlloc<char *>(params, sz);
    RIL_Errno errorCode = RIL_E_INTERNAL_ERR;

    if (params != nullptr && sz == 1) {
        std::string  aid = std::string(params[0]);
        QCRIL_LOG_INFO("Entered aid is %s",aid.c_str());
        auto msg = std::make_shared<UimGetImsiRequestMsg>(qmi_ril_get_process_instance_id(), aid);
        if (msg != nullptr) {

            GenericCallback<RIL_UIM_IMSI_Response> cb(
                [context] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<RIL_UIM_IMSI_Response> resp) -> void {
                std::shared_ptr<Marshal> p = nullptr;
                RIL_Errno errorCode = RIL_E_INTERNAL_ERR;

                if(status == Message::Callback::Status::SUCCESS && resp != nullptr) {


                  p = std::make_shared<Marshal>();
                  if(p && resp)
                  {
                    if (resp) {
                      if (p->write(resp->IMSI) == Marshal::Result::SUCCESS) {
                          errorCode = static_cast<RIL_Errno>(resp->err);
                      } else {
                          p = nullptr;
                      }
                    } else {
                      errorCode = RIL_E_NO_MEMORY;
                    }
                  }
                } else {
                  errorCode = RIL_E_NO_MEMORY;
                }
                QCRIL_LOG_INFO("Response code for Get imsi req %d",(RIL_Errno)errorCode);
                sendResponse(context, errorCode, p);
            });
            msg->setCallback(&cb);
            msg->dispatch();
        }
    } else {
        if (!params) {
            errorCode = RIL_E_NO_MEMORY;
        } else if (sz != 1) {
            errorCode = RIL_E_INVALID_ARGUMENTS;
        }
        sendResponse(context, errorCode, nullptr);
    }
    p.release(params,sz);
}

}  // namespace api
}  // namespace socket
}  // namespace ril
