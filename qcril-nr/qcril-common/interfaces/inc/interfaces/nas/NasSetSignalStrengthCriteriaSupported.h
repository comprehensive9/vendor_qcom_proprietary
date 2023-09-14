/******************************************************************************
#  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "interfaces/nas/nas_types.h"
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/** NasSetSignalStrengthCriteriaSupported.
 * This message informs the Nas module that the
 * SetSignalStrengthReportingCriteria request is supported.
 * If the parameter "supported" is true, it will adjust its
 * call to QMI_NAS_CONFIG_SIG_INFO2 to prevent overriding
 * user-provided values.
 */
class NasSetSignalStrengthCriteriaSupported : public SolicitedMessage<qcril::interfaces::NasSettingResultCode>,
                              public add_message_id<NasSetSignalStrengthCriteriaSupported>
{
 private:
  bool mSupported;

 public:
 static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.SetSignalStrengthCriteriaSupported";
 NasSetSignalStrengthCriteriaSupported() = delete;
  ~NasSetSignalStrengthCriteriaSupported() {}

  inline NasSetSignalStrengthCriteriaSupported(bool supported):
      SolicitedMessage<qcril::interfaces::NasSettingResultCode>(get_class_message_id()),
      mSupported(supported)
  {
    mName = MESSAGE_NAME;
  }

  inline bool isSupported() {
    return mSupported;
  }

  string dump() {
      return mName + "[ mSupported = " + (mSupported ? "true" : "false") + "]";
  }
};
