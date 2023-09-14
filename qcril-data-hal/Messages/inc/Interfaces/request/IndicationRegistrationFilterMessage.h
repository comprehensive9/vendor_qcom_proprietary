/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef INDICATIONREGISTRATIONFILTERMESSAGE
#define INDICATIONREGISTRATIONFILTERMESSAGE

#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"

#define UI_INFO_REGISTRATION_FLAG 1

namespace rildata {

class IndicationRegistrationFilterMessage : public SolicitedMessage<int>,
                                        public add_message_id<IndicationRegistrationFilterMessage> {
  private:
    int mFilter;
  public:
    static constexpr const char *MESSAGE_NAME = "IndicationRegistrationFilterMessage";
    IndicationRegistrationFilterMessage() = delete;

    inline IndicationRegistrationFilterMessage( int filter ):
                     SolicitedMessage<int>(get_class_message_id()) {
        mName = MESSAGE_NAME;
        mFilter = filter;
    }
    ~IndicationRegistrationFilterMessage(){}
    string dump(){return mName + " Filter Value : " + std::to_string(mFilter);}
    int getFilterValue() { return mFilter;}
};
}

#endif
