/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
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

#include "DeviceTypes.h"

#include <ostream>
#include <cstdint>
#include <map>

enum { MaxParams = 7 };
enum { LogLineHeaderSignature = 5 };
enum { NUM_MODULES = 16 };

enum RecordingType
{
    RECORDING_TYPE_XML     = 0,
    RECORDING_TYPE_TXT     = 1,
    RECORDING_TYPE_PUBLISH = 2, // corresponds to RawPublisher logging type, not exposed through CLI help
    RECORDING_TYPE_BOTH    = 3, // XML & TXT, not exposed through CLI help
    RECORDING_TYPE_PMC     = 4, // PMC Raw data recording, not exposed through CLI help
};

std::ostream &operator<<(std::ostream &output, const RecordingType &recordingType);

// This enum is also used as an array index, do NOT change the order
enum CpuType
{
    CPU_TYPE_FW = 0,
    CPU_TYPE_UCODE,
    CPU_TYPE_BOTH
};

const char* CpuTypeToString(CpuType cpuType);
std::ostream &operator<<(std::ostream &os, const CpuType &cpuType);

enum LoggingType
{
    XmlRecorder     = 0,
    TxtRecorder     = 1,
    RawPublisher    = 2,
    PmcDataRecorder = 3,
    MaxConsumers    = 4
};

const char* LoggingTypeToString(LoggingType loggingType);
std::ostream &operator<<(std::ostream &os, const LoggingType &loggingType);

enum class LoggingState
{
    ERROR_OCCURED,  // Something went wrong
    DISABLED,       // No recording
    ENABLED,        // The state right after start recording request by UI/CLI command (ready to activate recording)
    ACTIVE          // The recording is triggered by poll iteration
};

const char* LoggingStateToString(const LoggingState& ls);
std::ostream &operator<<(std::ostream &output, const LoggingState &ls);

enum class RecordingTrigger
{
    ExplicitStart,      // onTarget/DmTools start
    Deferred,           // deferred recording
    Persistent,         // device restarted
    NotDefinedYet
};

const char* RecordingTriggerToString(const RecordingTrigger& rt);
std::ostream &operator<<(std::ostream &output, const RecordingTrigger &rt);

/*
    module_level_info 8 bit representation:
    LSB                                 MSB
    +-----+-------+-----+-----+-----+-----+
    | 3:R | 1:TPM | 1:V | 1:I | 1:W | 1:E |
    +-----+-------+-----+-----+-----+-----+

*/

#ifdef _WINDOWS
__pragma(pack(push, 1))
#endif
struct module_level_info
{
    /* Little Endian */
    uint8_t error_level_enable : 1;   // E
    uint8_t warn_level_enable : 1;    // W
    uint8_t info_level_enable : 1;    // I
    uint8_t verbose_level_enable : 1; // V
    uint8_t third_party_mode : 1;     // TPM - This bit is set if we are on Third party mode and need to deal with logs with \n
    uint8_t reserved0 : 3;            // R

    module_level_info() :
        error_level_enable(0),
        warn_level_enable(0),
        info_level_enable(0),
        verbose_level_enable(0),
        third_party_mode(0),
        reserved0(0) {}

#ifdef _WINDOWS
}; __pragma(pack(pop));
#else
} __attribute__((packed));
#endif

/*
log_trace_header 32 bit representation:
LSB                                             MSB
+------+-------+-----+-----+-------+------+-------+
|2:DNM | 18:SO | 4:M | 2:V | 2:DNL | 1:IS | 3:SIG |
+------+-------+-----+-----+-------+------+-------+

*/

#ifdef _WINDOWS
__pragma(pack(push, 1))
#endif
struct log_trace_header
{
    /* Little Endian */
    //LSB
    uint32_t dword_num_msb : 2;     /* DNM */
    uint32_t string_offset : 18;    /* SO */
    uint32_t module : 4;            /* M - module that outputs the trace */
    uint32_t level : 2;             /* V - verbosity level [Error, Warning, Info, Verbose]*/
    uint32_t dword_num_lsb : 2;     /* DNL - to make the dw_num. If Old Mode just take this 2 bits*/
    uint32_t is_string : 1;         /* IS - (obsolete) indicate if the printf uses %s*/
    uint32_t signature : 3;         /* SIG - should be 5 (2'101 - LogLineHeaderSignature) in a valid header */
    //MSB
#ifdef _WINDOWS
}; __pragma(pack(pop));
#else
} __attribute__((packed));
#endif

std::ostream& operator<<(std::ostream& os, const module_level_info& moduleLevelInfo);

struct LogBufferHeader
{
    uint32_t WrPtr; /* incremented by trace producer every write */
    module_level_info ModuleLevel[NUM_MODULES];
};

extern const std::map<BasebandType, unsigned> baseband_to_peripheral_memory_start_address_linker;
extern const std::map<BasebandType, unsigned> baseband_to_peripheral_memory_start_address_ahb;
extern const std::map<BasebandType, unsigned> baseband_to_ucode_dccm_start_address_linker;
extern const std::map<BasebandType, unsigned> baseband_to_ucode_dccm_start_address_ahb;
