/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "interfaces/uim/qcril_uim_types.h"

/*===========================================================================*/
/*!
    @brief
    UIM Message to notify deep sleep entry and exit.

    @params
    mDeepSleepState  - Deep Sleep state

    @return
    None.
*/
/*=========================================================================*/
class UimDeepSleepNotifyMsg : public SolicitedSyncMessage<RIL_UIM_Errno>,
                           public add_message_id<UimDeepSleepNotifyMsg>
{
  private:
    RIL_UIM_DeepSleepState       mDeepSleepState;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.deep_sleep_notify";
    UimDeepSleepNotifyMsg() = delete;
    ~UimDeepSleepNotifyMsg() = default;

    inline UimDeepSleepNotifyMsg(RIL_UIM_DeepSleepState deep_sleep_state) :
                              SolicitedSyncMessage<RIL_UIM_Errno>(get_class_message_id())
    {
      mDeepSleepState = deep_sleep_state;
      mName = MESSAGE_NAME;
    }

    inline string dump()
    {
      return mName + "{}" + " Deep Sleep state:" + std::to_string(mDeepSleepState);
    }

    inline RIL_UIM_DeepSleepState get_deep_sleep_state(void)
    {
      return mDeepSleepState;
    }

}; /* UimDeepSleepNotifyMsg */
