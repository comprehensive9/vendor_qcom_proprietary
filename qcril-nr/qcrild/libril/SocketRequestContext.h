/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/MessageContext.h>

class SocketRequestContext: public MessageContextBase<SocketRequestContext>
{
    private:
        int32_t requestId;
        uint64_t requestToken;
        uint8_t rilInstanceId;

    public:
        // TODO: type rilInstanceId as RIL_SOCKET_ID
        SocketRequestContext(uint8_t rilInstanceId, int32_t requestId, uint64_t requestToken):
            MessageContextBase<SocketRequestContext>(static_cast<qcril_instance_id_e_type>(rilInstanceId)),
            requestId(requestId), requestToken(requestToken), rilInstanceId(rilInstanceId) {}

        std::string toString() {
            return std::string("RIL Instance: ") + std::to_string(rilInstanceId) + std::string(" Request Token: ") + std::to_string(requestToken);
        }

        uint64_t getRequestToken() { return requestToken; }

        int32_t getRequestId() { return requestId; }
        qcril_instance_id_e_type getRilInstanceId() { return static_cast<qcril_instance_id_e_type>(rilInstanceId); }
};

