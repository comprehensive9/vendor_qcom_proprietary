/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "OperationStatus.h"
#include <vector>
#include <string>

class Device;

enum class PmcModeE { LegacyPmcMode = 0, FwPmcMode, HostPmcMode }; // similar to FW wmi_pmc_status

// Helper struct to hold FW PMC ring spec
struct PmcRingSpec
{
    PmcRingSpec(PmcModeE pmcMode,
        uint32_t ringDescStartAddress, uint32_t ringPayloadStartAddress, uint32_t ringSwHeadAddress,
        uint32_t ringDescriptorsCount, uint32_t ringPayloadSizeBytes);

    const PmcModeE PmcMode;                     // current PMC mode
    const uint32_t PmcRingDescStartAddress;     // AHB addr. of the FW ring descriptors
    const uint32_t PmcRingPayloadStartAddress;  // AHB addr. of the FW ring payloads
    const uint32_t PmcRingSwHeadAddress;        // AHB addr. of the PMC SW_Head register
    const uint32_t PmcRingDescriptorsCount;     // number of descriptors
    const uint32_t PmcRingPayloadSizeBytes;     // payload buffer size (Bytes)
};

// DMA descriptor layout
// defined here to be used by RingReader in the FW PMC mode
#ifdef _WINDOWS
__pragma(pack(push, 1))
#endif
struct DmaDescriptor
{
    uint32_t Unused1[5];
    uint32_t PayloadAddr; // Linker addr. of the related payload buffer
    uint32_t Unused2;
    uint8_t  ErrorType;
    uint8_t  Status; // 0 - not in use
    uint16_t Length; // actual size in use
#ifdef _WINDOWS
}; __pragma(pack(pop))
#else
 } __attribute__((packed));
#endif

/*
PMC state machine:
++-------+-------+-------+-------+--------+
||INIT   |START  |PAUSE  |STOP   |SHUTDOWN| <- Action
+=======++=======+=======+=======+=======+========+
|IDLE   ||READY  |ACTIVE*|IDLE   |IDLE   |IDLE    |
+-------++-------+-------+-------+-------+--------+
|READY  ||READY  |ACTIVE |READY  |READY  |IDLE    |
+-------++-------+-------+-------+-------+--------+
|ACTIVE ||ACTIVE |ACTIVE |PAUSED |IDLE   |IDLE    |
+-------++-------+-------+-------+-------+--------+
|PAUSED ||PAUSED |PAUSED |PAUSED |IDLE   |IDLE    |
+-------++-------+-------+-------+-------+--------+
^
|
state
*/

class PmcSequenceBase
{
public:
    explicit PmcSequenceBase(const Device& device) : m_device(device) {}
    virtual ~PmcSequenceBase() = default;

    // actions requiring HW specific sequences
    OperationStatus Shutdown() const;
    OperationStatus Init() const;
    OperationStatus Start() const;
    OperationStatus Pause() const;
    OperationStatus SetDefaultConfiguration() const;
    // actions requiring different data retrieval methods
    OperationStatus StopForOnTarget() const;
    OperationStatus StopForDmTools(std::vector<uint8_t>& pmcData) const;

    // common implementations for Legacy and Continuous PMC
    OperationStatus IsPmcRecordingActive(bool& isRecordingActive) const;
    OperationStatus ReadPmcHead(uint32_t& pmcHead) const;
    OperationStatus FindPmcHeadAddress(uint32_t& pmcHeadAddress, uint32_t pmcRingIndex = 0) const;

    // Remark: may be null if the initialization failed
    // When recalculate is true, ring spec. is recreated according to the current mode
    const PmcRingSpec* GetPmcRingSpec(bool recalculate = false) const;

protected:
    const Device& m_device;
    OperationStatus DisableEventCollection(uint32_t pmcSamplingMask) const;
    OperationStatus FindPmcRingIndex(uint32_t& ringIndex) const;
    OperationStatus DisplayCurrentTsf(const char* szTitle) const;

private:
    virtual OperationStatus InitImpl() const = 0;
    virtual OperationStatus ShutdownImpl() const = 0;
    virtual OperationStatus PausePmcRecordingImpl() const = 0;
    virtual OperationStatus ValidateRecordingStatusImpl() const = 0;
    virtual OperationStatus ActivateRecordingImpl() const = 0;
    virtual OperationStatus SetDefaultConfigurationImpl() const = 0;
    virtual OperationStatus DeactivateRecordingImpl() const = 0;
    virtual OperationStatus FinalizeRecordingImpl(bool onTargetClient, std::vector<uint8_t>& pmcData, std::string& pmcDataPath) const = 0;
    virtual const PmcRingSpec* InitializePmcRingSpecImpl() const = 0;

    OperationStatus ApplyCurrentConfiguration() const;
    OperationStatus StopInternal(bool onTargetClient, std::vector<uint8_t>& pmcData) const;

};
