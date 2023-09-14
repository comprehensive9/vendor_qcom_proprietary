/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/SMS_3GPP.h>
#include <stdint.h>

template <>
Marshal::Result Marshal::write<RIL_GsmSmsMessage>(const RIL_GsmSmsMessage &arg) {
    int32_t smscPresnt = 0;
    if (arg.smscPdu) {
        smscPresnt = 1;
        write(smscPresnt);
        write(arg.smscPdu);
    } else {
        write(smscPresnt);
    }
    write(arg.pdu);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_GsmSmsMessage>(RIL_GsmSmsMessage &arg) const {
    int32_t smscPresnt = 0;
    read(smscPresnt);
    if (smscPresnt) {
        read(arg.smscPdu);
    }
    read(arg.pdu);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_GsmSmsAck>(const RIL_GsmSmsAck &arg) {
    write(arg.result);
    write(arg.cause);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_GsmSmsAck>(RIL_GsmSmsAck &arg) const {
    read(arg.result);
    read(arg.cause);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SMS_Response>(const RIL_SMS_Response &arg) {
    write(arg.messageRef);
    write(arg.ackPDU);
    write(arg.errorCode);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SMS_Response>(RIL_SMS_Response &arg) const {
    read(arg.messageRef);
    read(arg.ackPDU);
    read(arg.errorCode);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SMS_Response>(RIL_SMS_Response &arg) {
    release(arg.ackPDU);
    return Result::SUCCESS;
}

template<>
Marshal::Result Marshal::write<RIL_SMS_WriteArgs>(const RIL_SMS_WriteArgs& arg) {
    WRITE_AND_CHECK(arg.status);
    WRITE_AND_CHECK(arg.pdu);
    WRITE_AND_CHECK(arg.smsc);
    return Marshal::Result::SUCCESS;
}

template<>
Marshal::Result Marshal::read<RIL_SMS_WriteArgs>(RIL_SMS_WriteArgs& arg) const {
    READ_AND_CHECK(arg.status);

    size_t length = 0;
    RUN_AND_CHECK(readAndAlloc(arg.pdu, length));

    length = 0;
    RUN_AND_CHECK(readAndAlloc(arg.smsc, length));

    return Marshal::Result::SUCCESS;
}
