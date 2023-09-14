/**
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#ifndef SETRADIOCLIENTCAPUNSOLMESSAGE
#define SETRADIOCLIENTCAPUNSOLMESSAGE
#include "framework/UnSolicitedMessage.h"
#include "DataCommon.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"

namespace rildata {

class SetRadioClientCapUnSolMessage : public UnSolicitedMessage,public add_message_id<SetRadioClientCapUnSolMessage> {
    private:
    bool mIsAPAssistCapable;
    std::function<void(const std::vector<rildata::DataCallResult_t>&)> mDcListChangedCallback;

    public:

    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_RADIO_CLIENT_CAP_UNSOL";

    SetRadioClientCapUnSolMessage() = delete;

    inline SetRadioClientCapUnSolMessage( bool isAPAssistCapable,
        std::function<void(const std::vector<rildata::DataCallResult_t>&)> dcListChangedCallback ):
        UnSolicitedMessage(get_class_message_id()) {
        mName = MESSAGE_NAME;
        mIsAPAssistCapable = isAPAssistCapable;
        mDcListChangedCallback = dcListChangedCallback;
    }
    ~SetRadioClientCapUnSolMessage(){ }

    bool isAPAssistCapable() {
        return mIsAPAssistCapable;
    }

    std::function<void(const std::vector<rildata::DataCallResult_t>&)> getDcListChangedCallback() {
        return mDcListChangedCallback;
    }

    std::shared_ptr<UnSolicitedMessage> clone() {
        return std::make_shared<SetRadioClientCapUnSolMessage>( mIsAPAssistCapable, mDcListChangedCallback );
    }

    string dump() {
        return mName;
    }
};
}

#endif