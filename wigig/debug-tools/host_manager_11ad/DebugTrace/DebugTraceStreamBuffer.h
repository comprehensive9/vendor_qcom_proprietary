/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <streambuf>
#include <vector>

class DebugTraceStreamBuffer final : public std::streambuf
{
    static const size_t s_reservedBufLen = 512;

public:
    DebugTraceStreamBuffer();

    // Non copyable
    DebugTraceStreamBuffer(const DebugTraceStreamBuffer&) = delete;
    DebugTraceStreamBuffer &operator=(const DebugTraceStreamBuffer&) = delete;

protected:
    std::streambuf::int_type overflow(int_type ch) override;
    int sync() override;

private:
    std::vector<char> m_buffer;

    bool FlushBuffer();
};