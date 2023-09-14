/**
* Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef RILEVENTDATACALLBACK
#define RILEVENTDATACALLBACK
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include <framework/add_message_id.h>
#include "dsi_netctrl.h"

namespace rildata {

typedef struct
{
  dsi_net_evt_t      evt;
  void              *data;
  int                data_len;
  void              *self;
  dsi_evt_payload_t  payload;
} EventDataType;

class RilEventDataCallback : public UnSolicitedMessage,
                             public add_message_id<RilEventDataCallback> {
private:
  EventDataType eventData;
  int mCid;

public:
  static constexpr const char *MESSAGE_NAME = "QCRIL_EVT_DATA_EVENT_CALLBACK";
  ~RilEventDataCallback() = default;

  inline RilEventDataCallback(EventDataType p): UnSolicitedMessage(get_class_message_id())
  {
    eventData = p;
    mName = MESSAGE_NAME;
  }

  inline std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<RilEventDataCallback>(eventData));
  }

  inline std::string getEventName()
  {
    switch (eventData.evt) {
      case DSI_EVT_INVALID:
        return "INVALID";
      case DSI_EVT_NET_IS_CONN:
        return "NET_IS_CONN";
      case DSI_EVT_NET_NO_NET:
        return "NET_NO_NET";
      case DSI_EVT_PHYSLINK_DOWN_STATE:
        return "PHYSLINK_DOWN_STATE";
      case DSI_EVT_PHYSLINK_UP_STATE:
        return "PHYSLINK_UP_STATE";
      case DSI_EVT_NET_RECONFIGURED:
        return "NET_RECONFIGURED";
      case DSI_EVT_QOS_STATUS_IND:
        return "QOS_STATUS_IND";
      case DSI_EVT_NET_NEWADDR:
        return "NET_NEWADDR";
      case DSI_EVT_NET_DELADDR:
        return "NET_DELADDR";
      case DSI_EVT_NET_PARTIAL_CONN:
        return "NET_PARTIAL_CONN";
      case DSI_NET_TMGI_ACTIVATED:
        return "TMGI_ACTIVATED";
      case DSI_NET_TMGI_DEACTIVATED:
        return "TMGI_DEACTIVATED";
      case DSI_NET_TMGI_ACTIVATED_DEACTIVATED:
        return "TMGI_ACTIVATED_DEACTIVATED";
      case DSI_NET_TMGI_LIST_CHANGED:
        return "TMGI_LIST_CHANGED";
      case DSI_NET_SAI_LIST_CHANGED:
        return "SAI_LIST_CHANGED";
      case DSI_NET_CONTENT_DESC_CONTROL:
        return "CONTENT_DESC_CONTROL";
      case DSI_NET_TMGI_SERVICE_INTERESTED:
        return "TMGI_SERVICE_INTERESTED";
      case DSI_EVT_NET_HANDOFF:
        return "NET_HANDOFF";
      case DSI_EVT_WDS_CONNECTED:
        return "WDS_CONNECTED";
      case DSI_EVT_NET_NEWMTU:
        return "NET_NEWMTU";
      default:
        return "UNKNOWN";
    }
  }

  inline string dump()
  {
    return mName + " {cid=" + std::to_string(mCid) + " " + getEventName() + "}";
  }

  inline EventDataType *getEventData() {
    return &eventData;
  }

  inline int getCid() {
    return mCid;
  }

  inline void setCid(int cid) {
    mCid = cid;
  }
};

} //namespace

#endif