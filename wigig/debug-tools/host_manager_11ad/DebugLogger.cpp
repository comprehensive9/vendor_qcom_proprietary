/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "DebugLogger.h"

#include <ostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& os, const BoolStr& boolStr)
{
    return os << std::boolalpha << boolStr.Value << std::noboolalpha;
}

std::ostream& operator<<(std::ostream& os, const SuccessStr& successStr)
{
    return os << (successStr.Value ? "Success" : "Failure");
}

std::ostream& operator<<(std::ostream& os, const PlainStr& plainStr)
{
    for (std::string::const_iterator it = plainStr.Value.begin(); it != plainStr.Value.end(); ++it)
    {
        switch (*it)
        {
        case '\r': os << "\\r"; break;
        case '\n': os << "\\n"; break;
        case '\t': os << "\\t"; break;
        default:   os << *it; break;
        }
    }

    return os;
}

MacAddress::MacAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5)
    : Byte0(b0)
    , Byte1(b1)
    , Byte2(b2)
    , Byte3(b3)
    , Byte4(b4)
    , Byte5(b5)
{
}

MacAddress::MacAddress(uint64_t val)
    : Byte0(static_cast<uint8_t>((val >> 0) & 0xFF))
    , Byte1(static_cast<uint8_t>((val >> 8) & 0xFF))
    , Byte2(static_cast<uint8_t>((val >> 16) & 0xFF))
    , Byte3(static_cast<uint8_t>((val >> 24) & 0xFF))
    , Byte4(static_cast<uint8_t>((val >> 32) & 0xFF))
    , Byte5(static_cast<uint8_t>((val >> 40) & 0xFF))
{}

std::ostream& operator<<(std::ostream& os, const MacAddress& mac)
{
    std::ios::fmtflags f(os.flags());

    os << std::setw(2) << std::hex << std::right << std::setfill('0') << static_cast<unsigned>(mac.Byte0) << ':'
        << std::setw(2) << std::hex << std::right << std::setfill('0') << static_cast<unsigned>(mac.Byte1) << ':'
        << std::setw(2) << std::hex << std::right << std::setfill('0') << static_cast<unsigned>(mac.Byte2) << ':'
        << std::setw(2) << std::hex << std::right << std::setfill('0') << static_cast<unsigned>(mac.Byte3) << ':'
        << std::setw(2) << std::hex << std::right << std::setfill('0') << static_cast<unsigned>(mac.Byte4) << ':'
        << std::setw(2) << std::hex << std::right << std::setfill('0') << static_cast<unsigned>(mac.Byte5);

    os.flags(f);
    return os;
}

std::ostream& operator<<(std::ostream& os, const struct AddressValue& addressValue)
{
    return os << "[" << Address(addressValue.Address) << "] = " << Hex<>(addressValue.Value);
}

std::ostream& operator<<(std::ostream& os, const struct DeviceAddressBlock& deviceAddressBlock)
{
    return os << "Device: " << deviceAddressBlock.DeviceName
        << ", Addr: " << Address(deviceAddressBlock.Address)
        << ", Size (Bytes): " << deviceAddressBlock.SizeBytes;
}

std::ostream& operator<<(std::ostream& os, const MemoryDump& memoryDump)
{
    for (unsigned i = 0; i < memoryDump.m_lengthDwords; ++i)
    {
        if (i % 12 == 0 && memoryDump.m_printHeader)
        {
            os << "\n" << std::setfill('0') << std::setw(5) << i << "    ";
        }
        os << Hex<8>(memoryDump.m_pData[i]) << " ";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const ByteBuffer& buffer)
{
    std::ios::fmtflags f(os.flags());
    os << std::hex << std::uppercase << std::setfill('0') << '[';

    for (size_t i = 0; i < buffer.m_Length; ++i)
    {
        if (i >= buffer.m_MaxPrintedElements)
        {
            os << "(" << buffer.m_Length - i << " more...)";
            break;
        }

        os << "0x" << std::setw(2) << static_cast<unsigned>(buffer.m_pData[i]);
        if (i != buffer.m_Length - 1) os << ' ';
    }

    os.flags(f);
    return os << ']';
}

std::ostream& operator<<(std::ostream& os, const DwordBuffer& buffer)
{
    std::ios::fmtflags f(os.flags());
    os << std::hex << std::uppercase << std::setfill('0') << '[';

    for (size_t i = 0; i < buffer.m_LengthInDwords; ++i)
    {
        os << Hex<8>(buffer.m_pData[i]);
        if (i != buffer.m_LengthInDwords - 1) os << ',';
    }

    os.flags(f);
    return os << ']';
};

std::ostream& operator<<(std::ostream& os, const Percentage& percentage)
{
    std::ios::fmtflags f(os.flags());
    if (percentage.m_whole != 0)
    {
        os << std::fixed << std::showpoint << std::setprecision(1) << (percentage.m_part * 100) / percentage.m_whole;
    }
    else
    {
        os << "0";
    }

    if (percentage.m_printPercentSign)
    {
        os << '%';
    }

    os.flags(f);
    return os;
}