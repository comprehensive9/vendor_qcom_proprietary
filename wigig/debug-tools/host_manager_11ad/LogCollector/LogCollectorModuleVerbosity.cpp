/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorModuleVerbosity.h"

#include <ostream>
#include <sstream>
#include <vector>

LogCollectorModuleVerbosity::LogCollectorModuleVerbosity(unsigned moduleIndex, bool isError, bool isWarning, bool isInfo, bool isVerbose)
    : m_moduleIndex(moduleIndex)
    , m_isError(isError)
    , m_isWarning(isWarning)
    , m_isInfo(isInfo)
    , m_isVerbose(isVerbose)
{}

// Default c'tor to support dummy values in JsonValueBoxed
LogCollectorModuleVerbosity::LogCollectorModuleVerbosity()
    : m_moduleIndex(0)
    , m_isError(false)
    , m_isWarning(false)
    , m_isInfo(false)
    , m_isVerbose(false)
{}

std::string LogCollectorModuleVerbosity::ToString() const
{
    std::ostringstream verbosityStringBuilder;
    if (IsError()) verbosityStringBuilder << 'E';
    if (IsWarning()) verbosityStringBuilder << 'W';
    if (IsInfo()) verbosityStringBuilder << 'I';
    if (IsVerbose()) verbosityStringBuilder << 'V';

    return verbosityStringBuilder.str();
}

std::ostream& operator<<(std::ostream& os, const LogCollectorModuleVerbosity& moduleVerbosity)
{
    os << "Module" << moduleVerbosity.GetModuleIndex() << ": \"";
    if (moduleVerbosity.IsError()) os << 'E';
    if (moduleVerbosity.IsWarning()) os << 'W';
    if (moduleVerbosity.IsInfo()) os << 'I';
    if (moduleVerbosity.IsVerbose()) os << 'V';

    return os << "\"";
}

std::ostream& operator<<(std::ostream& os, const std::vector<LogCollectorModuleVerbosity>& v)
{
    os << "[ ";
    for (const auto& moduleVerbosity : v)
    {
        os << moduleVerbosity << " ";
    }

    return os << ']';
}
