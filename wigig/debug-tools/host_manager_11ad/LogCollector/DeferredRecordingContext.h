/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "LogCollectorDefinitions.h"
#include "LogCollectorModuleVerbosity.h"
#include "DebugLogger.h"

#include <vector>
#include <ostream>

class DeferredRecordingContext
{
public:
    DeferredRecordingContext(
        bool isCompress, bool isUpload, bool debugMode, LoggingType loggingType, const std::vector<LogCollectorModuleVerbosity>& enforcedVerbosity)
        : m_isCompress(isCompress)
        , m_isUpload(isUpload)
        , m_debugMode(debugMode)
        , m_loggingType(loggingType)
        , m_enforcedVerbosity(enforcedVerbosity)
    {}

    bool IsCompress() const { return m_isCompress; }
    bool IsUpload() const { return m_isUpload; }
    bool IsDebugMode() const { return m_debugMode; }
    LoggingType GetLoggingType() const { return m_loggingType; }
    const std::vector<LogCollectorModuleVerbosity>& GetModuleVerbosity() const { return m_enforcedVerbosity; }

private:
    const bool m_isCompress;
    const bool m_isUpload;
    const bool m_debugMode;
    const LoggingType m_loggingType;
    const std::vector<LogCollectorModuleVerbosity> m_enforcedVerbosity;
};

inline std::ostream& operator<<(std::ostream& os, const DeferredRecordingContext& ctx)
{
    return os << "Compress: " << BoolStr(ctx.IsCompress())
        << " Upload: " << BoolStr(ctx.IsUpload())
        << " Debug mode:" << BoolStr(ctx.IsDebugMode())
        << " Type: " << ctx.GetLoggingType()
        << " Enforced Verbosity: " << ctx.GetModuleVerbosity();
}
