/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include <interfaces/voice/QcRilRequestRejectIncomingCallMessage.h>
#include <Marshal.h>
#include "qcril_qmi_oem_reqlist.h"
#include "qcril_qmi_oemhook_utils.h"
#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

RIL_Errno rejectIncomingCallWithCause21(std::shared_ptr<SocketRequestContext> context,
                                                uint8_t * /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<QcRilRequestRejectIncomingCallMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

/*
 * Oemhook Request has the following message format
 *  [OEMNAME (8 bytes MANDATORY)]
 *  [COMMAND ID (4 bytes MANDATORY)]
 *  [REQ DATA LENGTH (4 bytes OPTIONAL)]
 *  [REQ DATA (REQ DATA LENGTH bytes OPTIONAL)]
 */
void dispatchOemhookRaw(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno errorCode = RIL_E_SUCCESS;
  uint8_t *data = nullptr;
  size_t dataLen = 0;
  do {
    if (p.dataAvail()) {
      p.readAndAlloc(data, dataLen);
    }

    uint8_t *dataPtr = data;
    if (dataPtr == nullptr) {
      QCRIL_LOG_ERROR("Invalid parameters; data is null");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    // [OEMNAME (8 bytes MANDATORY)]
    if (dataLen < QCRIL_OTHER_OEM_NAME_LENGTH) {
      QCRIL_LOG_ERROR("Invalid parameters; invalid data length");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    dataLen -= QCRIL_OTHER_OEM_NAME_LENGTH;
    dataPtr += QCRIL_OTHER_OEM_NAME_LENGTH;

    uint32_t cmd_id = 0;
    // [COMMAND ID (4 bytes MANDATORY)]
    if (dataLen < QCRIL_OTHER_OEM_REQUEST_ID_LEN) {
      QCRIL_LOG_ERROR("Invalid parameters; invalid data length");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      return;
    }
    memcpy(&cmd_id, dataPtr, QCRIL_OTHER_OEM_REQUEST_ID_LEN);
    dataLen -= QCRIL_OTHER_OEM_REQUEST_ID_LEN;
    dataPtr += QCRIL_OTHER_OEM_REQUEST_ID_LEN;

    uint32_t reqLen = 0;
    uint8_t *reqData = nullptr;
    // [REQ DATA LENGTH (4 bytes OPTIONAL)]
    if (dataLen > QCRIL_OTHER_OEM_REQUEST_DATA_LEN) {
      memcpy(&reqLen, dataPtr, QCRIL_OTHER_OEM_REQUEST_DATA_LEN);
      dataLen -= QCRIL_OTHER_OEM_REQUEST_DATA_LEN;
      dataPtr += QCRIL_OTHER_OEM_REQUEST_DATA_LEN;
      // [REQ DATA (REQ DATA LENGTH bytes OPTIONAL)]
      reqData = dataPtr;
    }

    // Return failure if REQ DATA LENGTH is larger than the actual data length.
    if (reqLen > dataLen) {
      QCRIL_LOG_ERROR("Invalid parameters; invalid data length");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }

    switch (cmd_id) {
      case QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21:
        errorCode = rejectIncomingCallWithCause21(context, reqData, reqLen);
        break;
      default:
        errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
        break;
    }
  } while (FALSE);

  p.release(data);

  if (errorCode != RIL_E_SUCCESS) {
    sendResponse(context, errorCode, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
