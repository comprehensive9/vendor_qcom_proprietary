/******************************************************************************
#  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "framework/add_message_id.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/QcRilRequestMessage.h"

/** NasEnablePhysChanConfigReporting.
 * This message informs the Nas module whether it should
 * report the physical channel config information.
 * If the parameter "enabled" is true, the indication will be
 * enabled.
 */
class NasEnablePhysChanConfigReporting : public SolicitedMessage<qcril::interfaces::NasSettingResultCode>,
                              public add_message_id<NasEnablePhysChanConfigReporting>
{
 private:
  bool mEnable;

 public:
 using cb_t = GenericCallback<qcril::interfaces::NasSettingResultCode>::cb_t;
 static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.EnablePhysChanConfigReporting";
 NasEnablePhysChanConfigReporting() = delete;
  ~NasEnablePhysChanConfigReporting() {}

  inline NasEnablePhysChanConfigReporting(bool enabled, std::weak_ptr<MessageContext> ctx, cb_t callbackfn):
      SolicitedMessage<qcril::interfaces::NasSettingResultCode>(MESSAGE_NAME, ctx, get_class_message_id()),
      mEnable(enabled)
  {
      GenericCallback<qcril::interfaces::NasSettingResultCode> cb(callbackfn);
      setCallback(&cb);
  }

  inline bool isEnable() {
    return mEnable;
  }

  string dump() {
      return mName + "[ mEnable = " + (mEnable ? "true" : "false") + "]";
  }
};
