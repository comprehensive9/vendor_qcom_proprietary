/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

class NasGetRfBandInfoMessage : public SolicitedMessage<NasActiveBand_t>,
                                   public add_message_id<NasGetRfBandInfoMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME =
            "QCRIL_DATA_GET_RF_BAND_INFO";

        inline NasGetRfBandInfoMessage():
            SolicitedMessage<NasActiveBand_t>(get_class_message_id())
        {
          mName = MESSAGE_NAME;
        }

        inline ~NasGetRfBandInfoMessage() {}

        string dump()
        {
            return mName;
        }
};
}