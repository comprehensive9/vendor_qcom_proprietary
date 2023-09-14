/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "interfaces/sms/RilRequestGetCdmaBroadcastConfigMessage.h"
#include "telephony/ril.h"

RilRequestGetCdmaBroadcastConfigMessage::~RilRequestGetCdmaBroadcastConfigMessage() {
}

string RilRequestGetCdmaBroadcastConfigMessage::dump() {
  return mName;
}
