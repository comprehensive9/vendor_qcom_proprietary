/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <iosfwd>
#include <vector>

// Used to communicate module severity to and out from log service.
class LogCollectorModuleVerbosity
{
public:
    LogCollectorModuleVerbosity(unsigned moduleIndex, bool isError, bool isWarning, bool isInfo, bool isVerbose);
    LogCollectorModuleVerbosity(const LogCollectorModuleVerbosity& other) = default;
    LogCollectorModuleVerbosity& operator=(const LogCollectorModuleVerbosity& other) = default;

    // Default c'tor to support dummy values in JsonValueBoxed
    LogCollectorModuleVerbosity();

    unsigned GetModuleIndex() const { return m_moduleIndex; }

    bool IsError() const { return m_isError; }
    bool IsWarning() const { return m_isWarning; }
    bool IsInfo() const { return m_isInfo; }
    bool IsVerbose() const { return m_isVerbose; }

    std::string ToString() const;

private:
    unsigned m_moduleIndex;

    bool m_isError;
    bool m_isWarning;
    bool m_isInfo;
    bool m_isVerbose;
};

std::ostream& operator<<(std::ostream& os, const LogCollectorModuleVerbosity& moduleVerbosity);
std::ostream& operator<<(std::ostream& os, const std::vector<LogCollectorModuleVerbosity>& v);