/*===========================================================================
 *
 *    Copyright (c) 2019 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#pragma once

#include "android/hardware/radio/1.5/IRadio.h"
#include "telephony/ril.h"

using namespace ::android::hardware::radio;

int convertSimIoPayloadtoHidl(void *data, size_t datalen, V1_0::IccIo& iccIoData);
int convertSimIoRespHidltoRilPayload(const V1_0::IccIoResult& iccIoResult, RIL_SIM_IO_Response& data);
int convertGetIccCardStatusRespHidltoRilPayload(const V1_2::CardStatus& cardStatusData,
                                                RIL_CardStatus_v6& data);
int convertGetIccCardStatusRespHidltoRilPayload_1_5(const V1_5::CardStatus& cardStatusData,
                                                RIL_CardStatus_v6& data);

