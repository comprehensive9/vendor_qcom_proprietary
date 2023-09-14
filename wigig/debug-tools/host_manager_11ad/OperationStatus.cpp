/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "OperationStatus.h"
#include "DebugLogger.h"

#include <ostream>
#include <sstream>

using namespace std;

OperationStatus::OperationStatus(bool success, const char* szMsg)
    : OperationStatus(success, szMsg ? string(szMsg) : string())
{}

OperationStatus::OperationStatus(bool success, string msg)
    : m_success(success)
    , m_msg(move(msg))
{}

void OperationStatus::AddPrefix(const string& prefix, bool addSeparator)
{
    ostringstream msgBuilder;
    msgBuilder << prefix;
    if (addSeparator)
    {
        msgBuilder << ". ";
    }
    msgBuilder << m_msg;
    m_msg = msgBuilder.str();
}

void OperationStatus::AddSuffix(const string& suffix, bool addSeparator)
{
    ostringstream msgBuilder;
    msgBuilder << m_msg;
    if (addSeparator)
    {
        msgBuilder << ". ";
    }
    msgBuilder << suffix;
    m_msg = msgBuilder.str();
}

OperationStatus OperationStatus::Merge(const OperationStatus& lhs, const OperationStatus& rhs)
{
    string msg;
    if (lhs.GetStatusMessage().empty())
    {
        // also covers case when both messages are empty
        msg = rhs.GetStatusMessage();
    }
    else if (rhs.GetStatusMessage().empty())
    {
        msg = lhs.GetStatusMessage();
    }
    else
    {
        // use ";" as separation to distinguish from commas that are part of the message
        ostringstream msgBuilder;
        msgBuilder << lhs.GetStatusMessage() << "; " << rhs.GetStatusMessage();
        msg = msgBuilder.str();
    }

    return { lhs && rhs, msg };
}

ostream& operator<<(ostream& os, const OperationStatus& st)
{
    return os << "Completed: " << SuccessStr(st.IsSuccess())
        << " Message: [" << st.GetStatusMessage() << ']';
}