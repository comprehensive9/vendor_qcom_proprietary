/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATANASPSATTACHDETACHRESPONSE
#define DATANASPSATTACHDETACHRESPONSE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

/********************** Class Definitions *************************/
class DataNasPsAttachDetachResponse: public UnSolicitedMessage,
                           public add_message_id<DataNasPsAttachDetachResponse> {

private:
    RIL_Errno mError;
public:
    static constexpr const char *MESSAGE_NAME = "DataNasPsAttachDetachResponse";

   ~DataNasPsAttachDetachResponse() = default;
    DataNasPsAttachDetachResponse(RIL_Errno err):
    UnSolicitedMessage(get_class_message_id()), mError(err)
    {
      mName = MESSAGE_NAME;
    }
    std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::make_shared<DataNasPsAttachDetachResponse>(mError);
    }
    RIL_Errno getError() {
        return mError;
    }
    string dump()
    {
      return MESSAGE_NAME;
    }
};

#endif
