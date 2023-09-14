/*===========================================================================

  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef IWLANDATACALLLISTCHANGEINDMESSAGE
#define IWLANDATACALLLISTCHANGEINDMESSAGE

#include "MessageCommon.h"

using namespace std;

namespace rildata {

class IWlanDataCallListChangeIndMessage : public UnSolicitedMessage,
                          public add_message_id<IWlanDataCallListChangeIndMessage> {
  private:
    std::vector<DataCallResult_t> mDCList;
  public:
    static constexpr const char *MESSAGE_NAME = "IWlanDataCallListChangeIndMessage";

    inline IWlanDataCallListChangeIndMessage(std::vector<DataCallResult_t> dcList):
      UnSolicitedMessage(get_class_message_id())
    {
      mName = MESSAGE_NAME;
      mDCList = dcList;
    }
    ~IWlanDataCallListChangeIndMessage() = default;

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

    std::shared_ptr<UnSolicitedMessage> clone() {
      return std::make_shared<IWlanDataCallListChangeIndMessage>(mDCList);
    };

    std::vector<DataCallResult_t> getDCList() {return mDCList;};
};

}

#endif