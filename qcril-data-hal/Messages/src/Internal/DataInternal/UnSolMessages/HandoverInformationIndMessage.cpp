/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "UnSolMessages/HandoverInformationIndMessage.h"

using namespace rildata;

HandoverInformationIndMessage::HandoverInformationIndMessage
(HandoffState_t setState, IpFamilyType_t setIpType, int setCid) : UnSolicitedMessage(get_class_message_id()),
    state(setState), ipType(setIpType), cid(setCid)
{
    mName = MESSAGE_NAME;
}

HandoverInformationIndMessage::~HandoverInformationIndMessage()
{
}

HandoffState_t HandoverInformationIndMessage::getState()
{
    return state;
}

IpFamilyType_t HandoverInformationIndMessage::getIpType()
{
    return ipType;
}

int HandoverInformationIndMessage::getCid()
{
    return cid;
}

bool HandoverInformationIndMessage::hasFailReason()
{
    return failReason ? true : false;
}

void HandoverInformationIndMessage::setFailReason(wds_handoff_failure_reason_type_v01 setReason) {
    failReason = setReason;
}

wds_handoff_failure_reason_type_v01 HandoverInformationIndMessage::getFailReason()
{
    return *failReason;
}

bool HandoverInformationIndMessage::hasCurrentNetwork()
{
    return currNw ? true : false;
}

void HandoverInformationIndMessage::setCurrentNetwork(HandoffNetworkType_t setCurrNw) {
    currNw = setCurrNw;
}

HandoffNetworkType_t HandoverInformationIndMessage::getCurrentNetwork()
{
    return *currNw;
}

bool HandoverInformationIndMessage::hasPreferredNetwork()
{
    return prefNw ? true : false;
}

void HandoverInformationIndMessage::setPreferredNetwork(HandoffNetworkType_t setPrefNw) {
    prefNw = setPrefNw;
}

HandoffNetworkType_t HandoverInformationIndMessage::getPreferredNetwork()
{
    return *prefNw;
}

std::shared_ptr<UnSolicitedMessage> HandoverInformationIndMessage::clone()
{
    auto copy = std::make_shared<HandoverInformationIndMessage>(state, ipType, cid);
    if (copy != nullptr) {
        if (hasFailReason()) {
            copy->setFailReason(getFailReason());
        }
        if (hasCurrentNetwork()) {
            copy->setCurrentNetwork(getCurrentNetwork());
        }
        if (hasPreferredNetwork()) {
            copy->setPreferredNetwork(getPreferredNetwork());
        }
    }
    return copy;
}

string HandoverInformationIndMessage::dump()
{
    return std::string(MESSAGE_NAME) + " state=" + std::to_string(static_cast<int>(state)) +
            " ipType=" + std::to_string(static_cast<int>(ipType)) + " cid=" +
            std::to_string(cid) + " failreason: " + (hasFailReason() ? "true" : "false") +
            " currNw: " + std::to_string(hasCurrentNetwork() ? (int)getCurrentNetwork() : 0) +
            " prefNw: " + std::to_string(hasPreferredNetwork() ? (int)getPreferredNetwork() : 0);
}
