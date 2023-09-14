/**
* Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef LINKPROPERTIESCHANGEDMESSAGE
#define LINKPROPERTIESCHANGEDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>

#include "MessageCommon.h"

namespace rildata {

enum class LinkPropertiesChangedType_t {
  None = 0,
  PcscfAddressChanged,
  LinkActiveStateChanged,
  QosDataReceived,
};
inline std::ostream& operator<<(std::ostream& os, LinkPropertiesChangedType_t lp)
{
  switch (lp) {
    case LinkPropertiesChangedType_t::None:
      os << "None";
      break;
    case LinkPropertiesChangedType_t::PcscfAddressChanged:
      os << "PcscfAddressChanged";
      break;
    case LinkPropertiesChangedType_t::LinkActiveStateChanged:
      os << "LinkActiveStateChanged";
      break;
    case LinkPropertiesChangedType_t::QosDataReceived:
      os << "QosDataReceived";
      break;
    default:
      os << std::to_string((int)lp);
      break;
  }
  return os;
}

class LinkPropertiesChangedMessage : public UnSolicitedMessage,
                             public add_message_id<LinkPropertiesChangedMessage> {
private:
  LinkPropertiesChangedType_t    mEvent;
  int                            mCid;
  std::optional<LinkActiveState> mActiveState;
  vector<QosSession_t> mQosSessions;

public:
  static constexpr const char *MESSAGE_NAME = "LinkPropertiesChangedMessage";
  ~LinkPropertiesChangedMessage() = default;

  inline LinkPropertiesChangedMessage(int cid, LinkPropertiesChangedType_t evt): UnSolicitedMessage(get_class_message_id())
  {
    mCid = cid;
    mEvent = evt;
    mName = MESSAGE_NAME;
  }

  inline std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<LinkPropertiesChangedMessage>(mCid, mEvent));
  }

  inline string dump()
  {
    std::stringstream ss;
    ss << mName << "{cid=" << mCid << " " << mEvent;
    if (hasActiveState()) {
      ss << " " << (int)getActiveState();
    }
    ss << "}";
    return ss.str();
  }

  inline LinkPropertiesChangedType_t getLinkPropertiesChanged() {
    return mEvent;
  }

  inline int getCid() {
    return mCid;
  }

  void setQosSessions(vector<QosSession_t>& qosSessions)
  {
    mQosSessions = qosSessions;
  }

  vector<QosSession_t> getQosSessions()
  {
    return mQosSessions;
  }

  inline bool hasActiveState() { return mActiveState ? true : false; }
  inline LinkActiveState getActiveState() { return *mActiveState; }
  inline void setActiveState(LinkActiveState active) { mActiveState = active; }
};

} //namespace

#endif
