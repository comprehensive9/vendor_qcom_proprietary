/**
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATANRICONTYPEINDMESSAGE
#define DATANRICONTYPEINDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

/********************** Class Definitions *************************/
class DataNrIconTypeIndMessage: public UnSolicitedMessage,
                           public add_message_id<DataNrIconTypeIndMessage> {

private:
    NrIconType_t mIcon;
public:
    static constexpr const char *MESSAGE_NAME = "DataNrIconTypeIndMessage";

    DataNrIconTypeIndMessage() = delete;
   ~DataNrIconTypeIndMessage() = default;
    DataNrIconTypeIndMessage(NrIconType_t setIcon):
    UnSolicitedMessage(get_class_message_id()), mIcon(setIcon)
    {
      mName = MESSAGE_NAME;
    }
    bool isUwb()
    {
      return mIcon.isUwb();
    }

    bool isBasic()
    {
      return mIcon.isBasic();
    }

    bool isNone()
    {
      return mIcon.isNone();
    }

    std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::make_shared<DataNrIconTypeIndMessage>(mIcon);
    }

    string dump()
    {
      string type = "NONE";
      if (isUwb()) {
        type = "UWB";
      } else if (isBasic()) {
        type = "BASIC";
      }
      std::stringstream ss;
      ss << MESSAGE_NAME << " " << type;
      return ss.str();
    }

};

} //namespace

#endif