/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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

#pragma once

#include <iomanip>
#include <ostream>

#include "DebugTrace/DebugTraceService.h"

#ifdef _WINDOWS
    #pragma warning(push)
    #pragma warning(disable: 4702) // suppress false unreachable code warning
#endif

#define TRACE_WITH_PREFIX(module, SEV)                                                  \
    DebugTraceService::GetInstance().StartMessage(module, SEV, __FILE__, __LINE__) &&   \
    DebugTraceService::GetInstance().GetStream()

#define LOG_ERROR   TRACE_WITH_PREFIX(DFLT, LOG_SEV_ERROR)
#define LOG_WARNING TRACE_WITH_PREFIX(DFLT, LOG_SEV_WARNING)
#define LOG_INFO    TRACE_WITH_PREFIX(DFLT, LOG_SEV_INFO)
#define LOG_DEBUG   TRACE_WITH_PREFIX(DFLT, LOG_SEV_DEBUG)
#define LOG_VERBOSE TRACE_WITH_PREFIX(DFLT, LOG_SEV_VERBOSE)

#define LOG_ERR(module)   TRACE_WITH_PREFIX(module, LOG_SEV_ERROR)
#define LOG_WRN(module) TRACE_WITH_PREFIX(module, LOG_SEV_WARNING)
#define LOG_INF(module)    TRACE_WITH_PREFIX(module, LOG_SEV_INFO)
#define LOG_DBG(module)   TRACE_WITH_PREFIX(module, LOG_SEV_DEBUG)
#define LOG_VRB(module) TRACE_WITH_PREFIX(module, LOG_SEV_VERBOSE)

// These macros are intended for low-level mechanisms where a logger cannot be used
#define LOG_INFO_COUT   std::cout << "[INF] "
#define LOG_DEBUG_COUT  std::cout << "[DBG] "
#define LOG_WARNING_COUT  std::cout << "[WRN] "
#define LOG_ERROR_COUT  std::cerr << "[ERR] "

inline bool ShouldPrint(ModuleId dtm, LogSeverity severity)
{
    return severity <= DebugTraceService::GetInstance().GetModuleMaxSeverity(dtm);
}

inline bool ShouldPrint(LogSeverity severity)
{
    return ShouldPrint(DFLT, severity);
}

// Decoupling from system assert allows to print an error message when
// assert is disabled.
#define LOG_ASSERT(CONDITION)                                                                \
    do {                                                                                     \
        if (!(CONDITION)) {                                                                  \
            LOG_ERROR << "ASSERTION FAILURE: " << #CONDITION                                 \
                << " at " << __FILE__ << ':' << __LINE__                                     \
                << std::endl;                                                                \
                if (DebugTraceService::GetInstance().ShouldExitOnAssert()) exit(1);          \
        }                                                               \
    } while (false)

// *************************************************************************************************
// Stream Formatters
// *************************************************************************************************

// Print a boolean value as a string
struct BoolStr
{
    explicit BoolStr(bool value): Value(value) {}
    const bool Value;
};

std::ostream& operator<<(std::ostream& os, const BoolStr& boolStr);

// *************************************************************************************************

// Print a boolean value as a Success/Failure string
struct SuccessStr
{
    explicit SuccessStr(bool value): Value(value) {}
    const bool Value;
};

std::ostream& operator<<(std::ostream& os, const SuccessStr& successStr);

// *************************************************************************************************

// Print a string while displaying newline characters
struct PlainStr
{
    explicit PlainStr(const std::string& value): Value(value) {}
    const std::string& Value;
};

std::ostream& operator<<(std::ostream& os, const PlainStr& plainStr);

// *************************************************************************************************

// Helper class to format hexadecimal numbers
template <int WIDTH = 0> struct Hex
{
    explicit Hex(uint32_t value): Value(value) {}
    static const unsigned Width = WIDTH;
    const uint32_t Value;
};

template<int WIDTH>
std::ostream& operator<<(std::ostream& os, const Hex<WIDTH>& hex)
{
    int width = hex.Width;
    std::ios::fmtflags f(os.flags());

    os << "0x";
    if (0 != width) os << std::setw(width) << std::right << std::setfill('0');
    os << std::hex << std::uppercase << hex.Value;
    os.flags( f );
    return os;
}

// *************************************************************************************************

struct MacAddress
{
    MacAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5);
    explicit MacAddress(uint64_t val);

    const uint8_t Byte0;
    const uint8_t Byte1;
    const uint8_t Byte2;
    const uint8_t Byte3;
    const uint8_t Byte4;
    const uint8_t Byte5;
};

std::ostream& operator<<(std::ostream& os, const MacAddress& mac);

// *************************************************************************************************

// Prints address as Hex<8>
struct Address: public Hex<8>
{
    explicit Address(uint32_t value): Hex(value) {}
};

// *************************************************************************************************

struct AddressValue
{
    const uint32_t Address;
    const uint32_t Value;
    AddressValue(uint32_t address, uint32_t value): Address(address), Value(value) {}
};

std::ostream& operator<<(std::ostream& os, const struct AddressValue& addressValue);

// *************************************************************************************************

struct DeviceAddressBlock
{
    const std::string DeviceName;
    const uint32_t Address;
    const uint32_t SizeBytes;

    DeviceAddressBlock(std::string deviceName, uint32_t address, uint32_t sizeBytes) :
        DeviceName(std::move(deviceName)),
        Address(address),
        SizeBytes(sizeBytes)
    {}
};

std::ostream& operator<<(std::ostream& os, const struct DeviceAddressBlock& deviceAddressBlock);

// **************************************************************************************************

// Helper class to print memory content (new line, for large content)
class MemoryDump
{
    friend std::ostream& operator<<(std::ostream& os, const MemoryDump& memoryDump);

public:
    MemoryDump(const uint32_t* pData, size_t lengthDwords, bool printHeader = true): m_pData(pData), m_lengthDwords(lengthDwords), m_printHeader(printHeader) {}

private:
    const uint32_t* const m_pData;
    const size_t m_lengthDwords;
    const bool m_printHeader;
};

// *************************************************************************************************

// Helper class to print memory buffer as a byte array (embedded in the message)
class ByteBuffer
{
    friend std::ostream& operator<<(std::ostream& os, const ByteBuffer& buffer);

public:
    ByteBuffer(const unsigned char* pData, size_t length): m_pData(pData), m_Length(length) {}

private:
    const unsigned char* const m_pData;
    const size_t m_Length;
    const size_t m_MaxPrintedElements = 10;
};

// *************************************************************************************************

// Helper class to print memory buffer as a dword array (embedded in the message)
class DwordBuffer
{
    friend std::ostream& operator<<(std::ostream& os, const DwordBuffer& buffer);

public:
    DwordBuffer(const uint32_t* pData, size_t lengthInDwords): m_pData(pData), m_LengthInDwords(lengthInDwords) {}

private:
    const uint32_t* const m_pData;
    const size_t m_LengthInDwords;
};

// *************************************************************************************************
//
// Display ratio as percentage with one digit after a decimal point
class Percentage
{
    friend std::ostream& operator<<(std::ostream& os, const Percentage& percentage);
public:
    Percentage(double part, double whole, bool printPercentSign = true)
        : m_part(part), m_whole(whole), m_printPercentSign(printPercentSign) {}

private:
    const double m_part;
    const double m_whole;
    const bool m_printPercentSign;
};

// *************************************************************************************************

#ifdef _WINDOWS
    #pragma warning(pop)
#endif
