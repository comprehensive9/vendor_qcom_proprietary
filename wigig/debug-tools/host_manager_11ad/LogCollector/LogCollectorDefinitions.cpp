/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <ostream>

#include "LogCollectorDefinitions.h"

const std::map<BasebandType, unsigned> baseband_to_peripheral_memory_start_address_linker =
{
    { BASEBAND_TYPE_SPARROW,    0x840000 },
    { BASEBAND_TYPE_TALYN,      0x840000 },
    { BASEBAND_TYPE_BORRELLY,   0x840000 }
};
const std::map<BasebandType, unsigned> baseband_to_peripheral_memory_start_address_ahb =
{
    { BASEBAND_TYPE_SPARROW,    0x908000 },
    { BASEBAND_TYPE_TALYN,      0xA20000 },
    { BASEBAND_TYPE_BORRELLY,   0x980000 },
};
const std::map<BasebandType, unsigned> baseband_to_ucode_dccm_start_address_linker =
{
    { BASEBAND_TYPE_SPARROW,    0x800000 },
    { BASEBAND_TYPE_TALYN,      0x800000 },
    { BASEBAND_TYPE_BORRELLY,   0x800000 }
};
const std::map<BasebandType, unsigned> baseband_to_ucode_dccm_start_address_ahb =
{
    { BASEBAND_TYPE_SPARROW,    0x940000 },
    { BASEBAND_TYPE_TALYN,      0xA78000 },
    { BASEBAND_TYPE_BORRELLY,   0x9e0000 },
};

std::ostream& operator<<(std::ostream& os, const module_level_info& moduleLevelInfo)
{
    os << "{ Sev: '";
    if (moduleLevelInfo.error_level_enable != 0) os << 'E';
    if (moduleLevelInfo.warn_level_enable != 0) os << 'W';
    if (moduleLevelInfo.info_level_enable != 0) os << 'I';
    if (moduleLevelInfo.verbose_level_enable != 0) os << 'V';
    os << "', ";
    os << "TPM: " << static_cast<int>(moduleLevelInfo.third_party_mode);
    return os << '}';
}

std::ostream& operator<<(std::ostream &output, const RecordingType &recordingType)
{
    switch (recordingType)
    {
    case RECORDING_TYPE_XML:
        output << "raw";
        break;
    case RECORDING_TYPE_TXT:
        output << "txt";
        break;
    case RECORDING_TYPE_PUBLISH:
        output << "publish";
        break;
    case RECORDING_TYPE_BOTH:
        output << "both";
        break;
    case RECORDING_TYPE_PMC:
        output << "pmc";
        break;
    default:
        output << "unknown recording type " << static_cast<int>(recordingType);
        break;
    }

    return output;
}

const char* CpuTypeToString(CpuType cpuType)
{
    switch (cpuType)
    {
    case CPU_TYPE_FW:       return "fw";
    case CPU_TYPE_UCODE:    return "ucode";
    case CPU_TYPE_BOTH:     return "both";
    default:                return "Unknown CPU type";
    }
}

std::ostream& operator<<(std::ostream &os, const CpuType &cpuType)
{
    return os << CpuTypeToString(cpuType);
}

const char* LoggingTypeToString(LoggingType loggingType)
{
    switch(loggingType)
    {
    case XmlRecorder:       return "RawRecorder";
    case TxtRecorder:       return "TxtRecorder";
    case RawPublisher:      return "RawPublisher";
    case PmcDataRecorder:   return "PmcDataRecorder";
    default:return "unknown Logging type";
    }
}

std::ostream &operator<<(std::ostream &os, const LoggingType &loggingType)
{
    return os << LoggingTypeToString(loggingType);
}

const char* LoggingStateToString(const LoggingState& ls)
{
    switch (ls)
    {
    case LoggingState::DISABLED:
        return "Disabled";
    case LoggingState::ENABLED:
        return "Enabled";
    case LoggingState::ACTIVE:
        return "Active";
    case LoggingState::ERROR_OCCURED:
        return "Error";
    default:
        return "Unknown state";
    }
}

std::ostream& operator<<(std::ostream& output, const LoggingState& ls)
{
    return output << LoggingStateToString(ls);
}

const char* RecordingTriggerToString(const RecordingTrigger& rt)
{
    switch (rt)
    {
    case RecordingTrigger::ExplicitStart:
        return "ExplicitStart";
    case RecordingTrigger::Deferred:
        return "Deferred";
    case RecordingTrigger::Persistent:
        return "Persistent";
    default:
        return "Unknown recording trigger";
    }
}
std::ostream &operator<<(std::ostream &output, const RecordingTrigger &rt)
{
    return output << RecordingTriggerToString(rt);
}
