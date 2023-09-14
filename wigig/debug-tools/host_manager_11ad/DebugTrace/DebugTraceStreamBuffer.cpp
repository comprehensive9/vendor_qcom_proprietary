/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "DebugTraceStreamBuffer.h"
#include "DebugTraceTarget.h"
#include "DebugTraceService.h"

#include <cassert>

using namespace std;

DebugTraceStreamBuffer::DebugTraceStreamBuffer()
    : m_buffer(s_reservedBufLen + 1)
{
    char* pBufferBegin = &m_buffer.front();
    setp(pBufferBegin, pBufferBegin + m_buffer.size() - 1); // -1 to make overflow() easier
}

std::streambuf::int_type DebugTraceStreamBuffer::overflow(int_type ch)
{
    if (ch != traits_type::eof())
    {
        assert(less_equal<char *>()(pptr(), epptr()));
        *pptr() = static_cast<char>(ch);
        pbump(1);
        if (FlushBuffer()) return ch;
    }

    return traits_type::eof();
}

int DebugTraceStreamBuffer::sync()
{
    return FlushBuffer() ? 0 : -1;
}

bool DebugTraceStreamBuffer::FlushBuffer()
{
    const int nChars = static_cast<int>(pptr() - pbase());
    if (0 == nChars)
    {
        return true;
    }

    pbump(-nChars);

    DebugTraceService::GetInstance().SubmitBuffer(pbase(), nChars);
    return true;
}