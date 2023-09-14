/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "UnSolMessages/VoiceIndMessage.h"

using namespace rildata;

VoiceIndMessage::VoiceIndMessage(std::vector<VoiceIndResp> voiceResp):UnSolicitedMessage(get_class_message_id())
{
  mVoiceResp.assign(voiceResp.begin(), voiceResp.end());
}

VoiceIndMessage::~VoiceIndMessage()
{
}

std::shared_ptr<UnSolicitedMessage> VoiceIndMessage::clone()
{
  return std::make_shared<VoiceIndMessage>(mVoiceResp);
}

string VoiceIndMessage::dump()
{
  return MESSAGE_NAME;
}

std::vector<VoiceIndResp> VoiceIndMessage::getParam()
{
  return mVoiceResp;
}