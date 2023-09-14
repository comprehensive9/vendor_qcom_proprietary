/*===========================================================================

  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef RADIODATACALLLISTCHANGEINDMESSAGE
#define RADIODATACALLLISTCHANGEINDMESSAGE

#include "MessageCommon.h"

namespace rildata {

class RadioDataCallListChangeIndMessage : public UnSolicitedMessage {
  protected:
    const std::vector<DataCallResult_t> mDCList;

  public:
    RadioDataCallListChangeIndMessage() = delete;
    inline RadioDataCallListChangeIndMessage(message_id_t id,
      const std::vector<DataCallResult_t>& dcList): UnSolicitedMessage(id), mDCList(dcList) {}
    ~RadioDataCallListChangeIndMessage() = default;

    string dump() {
      std::stringstream ss;
      ss << mName << " {";
      for (const DataCallResult_t& call : mDCList) {
        call.dump("", ss);
        ss << ",";
      }
      ss << "}";
      return ss.str();
    };

    std::vector<DataCallResult_t> getDCList() {return mDCList;};
};

class RadioDataCallListChangeIndMessage_1_4 : public RadioDataCallListChangeIndMessage,
                          public add_message_id<RadioDataCallListChangeIndMessage_1_4> {
public:
    static constexpr const char *MESSAGE_NAME = "RadioDataCallListChangeIndMessage_1_4";
    RadioDataCallListChangeIndMessage_1_4(const std::vector<DataCallResult_t>& dcList):
      RadioDataCallListChangeIndMessage(get_class_message_id(), dcList) {
        mName = MESSAGE_NAME;
      }
    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<RadioDataCallListChangeIndMessage_1_4>(mDCList);
    };
};

class RadioDataCallListChangeIndMessage_1_5 : public RadioDataCallListChangeIndMessage,
                          public add_message_id<RadioDataCallListChangeIndMessage_1_5> {
public:
    static constexpr const char *MESSAGE_NAME = "RadioDataCallListChangeIndMessage_1_5";
    RadioDataCallListChangeIndMessage_1_5(const std::vector<DataCallResult_t>& dcList):
      RadioDataCallListChangeIndMessage(get_class_message_id(), dcList) {
        mName = MESSAGE_NAME;
      }
    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<RadioDataCallListChangeIndMessage_1_5>(mDCList);
    };
};

#ifdef RIL_FOR_LOW_RAM
class RadioDataCallListChangeIndMessage_1_0 : public RadioDataCallListChangeIndMessage,
                          public add_message_id<RadioDataCallListChangeIndMessage_1_0> {
public:
    static constexpr const char *MESSAGE_NAME = "RadioDataCallListChangeIndMessage_1_0";
    RadioDataCallListChangeIndMessage_1_0(const std::vector<DataCallResult_t>& dcList):
      RadioDataCallListChangeIndMessage(get_class_message_id(), dcList) {
        mName = MESSAGE_NAME;
      }
    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<RadioDataCallListChangeIndMessage_1_0>(mDCList);
    };
};
#endif
}

#endif
