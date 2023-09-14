/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ImsSms.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SmsMessage>(const RIL_IMS_SmsMessage& arg) {
    WRITE_AND_CHECK(arg.messageRef);
    WRITE_AND_CHECK(arg.format);
    WRITE_AND_CHECK(arg.smsc, static_cast<std::size_t>(arg.smscLength));
    WRITE_AND_CHECK(arg.shallRetry);
    WRITE_AND_CHECK(arg.pdu, static_cast<std::size_t>(arg.pduLength));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SmsMessage>(RIL_IMS_SmsMessage& arg) const {
    READ_AND_CHECK(arg.messageRef);
    READ_AND_CHECK(arg.format);

    std::size_t len = 0;
    RUN_AND_CHECK(readAndAlloc(arg.smsc, len));
    arg.smscLength = len;

    READ_AND_CHECK(arg.shallRetry);

    len = 0;
    RUN_AND_CHECK(readAndAlloc(arg.pdu, len));
    arg.pduLength = len;

    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SendSmsResponse>(const RIL_IMS_SendSmsResponse& arg) {
    WRITE_AND_CHECK(arg.status);
    WRITE_AND_CHECK(arg.messageRef);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SendSmsResponse>(RIL_IMS_SendSmsResponse& arg) const {
    READ_AND_CHECK(arg.status);
    READ_AND_CHECK(arg.messageRef);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SmsAck>(const RIL_IMS_SmsAck& arg) {
    WRITE_AND_CHECK(arg.messageRef);
    WRITE_AND_CHECK(arg.deliveryStatus);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SmsAck>(RIL_IMS_SmsAck& arg) const {
    READ_AND_CHECK(arg.messageRef);
    READ_AND_CHECK(arg.deliveryStatus);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_IncomingSms>(const RIL_IMS_IncomingSms& arg) {
    WRITE_AND_CHECK(arg.format);
    WRITE_AND_CHECK(arg.pdu, static_cast<std::size_t>(arg.pduLength));
    WRITE_AND_CHECK(arg.verificationStatus);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_IncomingSms>(RIL_IMS_IncomingSms& arg) const {
    READ_AND_CHECK(arg.format);

    size_t len = 0;
    RUN_AND_CHECK(readAndAlloc(arg.pdu, len));
    arg.pduLength = len;

    READ_AND_CHECK(arg.verificationStatus);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_SmsStatusReport>(const RIL_IMS_SmsStatusReport& arg) {
    WRITE_AND_CHECK(arg.messageRef);
    WRITE_AND_CHECK(arg.format);
    WRITE_AND_CHECK(arg.pdu, static_cast<std::size_t>(arg.pduLength));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SmsStatusReport>(RIL_IMS_SmsStatusReport& arg) const {
    READ_AND_CHECK(arg.messageRef);
    READ_AND_CHECK(arg.format);

    size_t len = 0;
    RUN_AND_CHECK(readAndAlloc(arg.pdu, len));
    arg.pduLength = len;

    return Marshal::Result::SUCCESS;
}
