/*
* Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "LogCollectorDefinitions.h"
#include "OperationStatus.h"
#include "LogFmtStringsContainer.h"
#include <string>

typedef union arg_t
{
    uintmax_t i;
    double f;
    const void *p;
} arg;



class LogTxtParser
{
public:
    LogTxtParser() : m_modulesInfo(nullptr), m_newLineRequired(true), m_prevModule(-1)
    {}
    ~LogTxtParser();

    std::string ParseLogMessage(bool possiblyCorrupted, unsigned stringOffset, const int* pParams, uint32_t numParams,
                                unsigned module, unsigned verbosityLevel, const std::string &timeStamp,
                                bool& invalidStringOffsetFound, bool& paramConversionError,
                                const char **fmt);
    OperationStatus LogParserInit(const char* stringConversionPath, CpuType cpuType);
    void SetModuleLevelVerbosity(const module_level_info* modulesInfo) { m_modulesInfo = modulesInfo; }
    const LogFmtStringsContainer& GetFmtStrings() const { return m_fmtStringsContainer;  }

private:
    static const unsigned MaxVerboseLevel = 4;
    const char* m_levels[MaxVerboseLevel] = { "[ERROR]","[WARN ]","[INFO ]","[VERBO]" };
    const unsigned str_mask = 0xFFFFF;
    const char *NNL = "[NNL]";

    const module_level_info* m_modulesInfo;
    unsigned char states[3]['z' - 'A' + 1] = { { 0 } };
    bool m_newLineRequired;
    int m_prevModule;
    LogFmtStringsContainer m_fmtStringsContainer;

    void SetStates();
    const char *GetVerbosityLevelByIndex(unsigned int index) const;

    std::string ParseMessageFmt(const std::string &prefix, int module, const char* fmt, const int* pParams, uint32_t numParams, bool& conversionError);
    int printf_core_ss(std::stringstream &ss, const char *fmt, arg_t *nl_arg,
                       int *nl_type, const std::string &module_string,
                       const int* pParams, uint32_t numParams);
    int print_trace_string(std::stringstream &ss, const char *fmt, const int* pParams, uint32_t numParams, const std::string & prefix);
    bool pop_arg(int type, const int* pParams, uint32_t numParams, unsigned int &lastParamIndex, arg_t& newArg);
};
