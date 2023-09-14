/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/SMS_3GPP2.h>

template <>
Marshal::Result Marshal::write<RIL_CDMA_SMS_Address>(const RIL_CDMA_SMS_Address &arg) {
    write<int32_t>(arg.digit_mode);
    write<int32_t>(arg.number_mode);
    write<int32_t>(arg.number_type);
    write<int32_t>(arg.number_plan);
    write<unsigned char>((const unsigned char*)arg.digits, arg.number_of_digits);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_SMS_Subaddress>(
        const RIL_CDMA_SMS_Subaddress &arg) {
    write<int32_t>(arg.subaddressType);
    write(arg.odd);
    write<unsigned char>((const unsigned char*)arg.digits, arg.number_of_digits);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_SMS_Message>(const RIL_CDMA_SMS_Message &arg) {
    write(arg.uTeleserviceID);
    write(arg.bIsServicePresent);
    write(arg.uServicecategory);
    write<RIL_CDMA_SMS_Address>(arg.sAddress);
    write<RIL_CDMA_SMS_Subaddress>(arg.sSubAddress);
    write<unsigned char>((const unsigned char*)arg.aBearerData, arg.uBearerDataLen);
    write(arg.expectMore);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_SMS_Subaddress>(RIL_CDMA_SMS_Subaddress &arg) const {
    int32_t tmp32;
    read(tmp32);
    arg.subaddressType = static_cast<RIL_CDMA_SMS_SubaddressType>(tmp32);
    read(arg.odd);

    read(tmp32);
    char* buf = reinterpret_cast<char*>(arg.digits);
    read<char>(buf, (size_t)tmp32);
    arg.number_of_digits = tmp32;

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_SMS_Address>(RIL_CDMA_SMS_Address &arg) const {
    int32_t tmp32;
    read(tmp32);
    arg.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(tmp32);
    read(tmp32);
    arg.number_mode = static_cast<RIL_CDMA_SMS_NumberMode>(tmp32);
    read(tmp32);
    arg.number_type = static_cast<RIL_CDMA_SMS_NumberType>(tmp32);
    read(tmp32);
    arg.number_plan = static_cast<RIL_CDMA_SMS_NumberPlan>(tmp32);

    read(tmp32);
    char* buf = reinterpret_cast<char*>(arg.digits);
    read<char>(buf, (size_t)tmp32);
    arg.number_of_digits = tmp32;

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_SMS_Message>(RIL_CDMA_SMS_Message &arg) const {
    read(arg.uTeleserviceID);
    read(arg.bIsServicePresent);
    read(arg.uServicecategory);
    read<RIL_CDMA_SMS_Address>(arg.sAddress);
    read<RIL_CDMA_SMS_Subaddress>(arg.sSubAddress);
    int32_t tmp32;
    read<int32_t>(tmp32);
    char* buf = reinterpret_cast<char*>(arg.aBearerData);
    read<char>(buf, (size_t)tmp32);
    arg.uBearerDataLen = tmp32;
    read(arg.expectMore);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_SMS_Ack>(const RIL_CDMA_SMS_Ack &arg) {
    write<int32_t>(arg.uErrorClass);
    write<int>(arg.uSMSCauseCode);

    return Result::SUCCESS;
}

template<>
Marshal::Result Marshal::read<RIL_CDMA_SMS_Ack>(RIL_CDMA_SMS_Ack &arg) const {
    int32_t tmp32;
    read<int32_t>(tmp32);
    arg.uErrorClass = static_cast<RIL_CDMA_SMS_ErrorClass>(tmp32);
    read<int>(arg.uSMSCauseCode);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_BroadcastSmsConfigInfo>(const RIL_CDMA_BroadcastSmsConfigInfo &arg) {
    write(arg.service_category);
    write(arg.language);
    write(arg.selected);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_BroadcastSmsConfigInfo>(RIL_CDMA_BroadcastSmsConfigInfo &arg) const {
    read(arg.service_category);
    read(arg.language);
    read(arg.selected);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CDMA_SMS_WriteArgs>(const RIL_CDMA_SMS_WriteArgs &arg) {
    write(arg.status);
    write(arg.message);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CDMA_SMS_WriteArgs>(RIL_CDMA_SMS_WriteArgs& arg) const {
    read(arg.status);
    read(arg.message);

    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::readAndAlloc<RIL_CDMA_BroadcastSmsConfigInfo>(
        RIL_CDMA_BroadcastSmsConfigInfo* &arg, size_t &sz) const {
    int32_t size;
    read(size);
    if (size == -1) {
        arg = nullptr;
        sz = 0;
        return Result::SUCCESS;
    }
    auto out = new RIL_CDMA_BroadcastSmsConfigInfo[size]();
    if (out == nullptr) {
        arg = nullptr;
        sz = 0;
        return Result::FAILURE;
    }
    sz = size;
    std::for_each(out, out + sz, [this] (RIL_CDMA_BroadcastSmsConfigInfo &val) {
            read(val); });
    arg = out;

    return Result::SUCCESS;
}

