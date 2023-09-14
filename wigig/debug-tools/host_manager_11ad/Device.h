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

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <map>

#include "DeviceTypes.h"
#include "OperationStatus.h"

struct StringNameValuePair;
class DriverAPI;
enum class DeviceType;
struct StaInfo;
class FieldDescription;
class FwStateProvider;
class PmcContext;

class Device
{
public:
    explicit Device(std::unique_ptr<DriverAPI>&& deviceDriver);

    // cannot be inline because incompleteness of FieldDescription and other types
    ~Device();

    const std::string& GetDeviceName() const { return m_deviceName; }
    BasebandType GetBasebandType() const { return m_basebandType; }
    BasebandRevision GetBasebandRevision() const { return m_basebandRevision; }
    DeviceType GetDeviceType() const;
    bool IsMonitored() const;

    bool GetSilenceMode() const { return m_isSilent.load(); }
    void SetSilenceMode(bool silentMode) { m_isSilent.store(silentMode); }

    uint32_t GetCapabilityMask() const;
    OperationStatus GetDriverMode(int& currentState) const;
    OperationStatus SetDriverMode(int newState, int& oldState) const;
    OperationStatus DriverControl(uint32_t Id, const void *inBuf, uint32_t inBufSize, void *outBuf, uint32_t outBufSize) const;
    OperationStatus SendWmiWithEvent(const void* inBuf, uint32_t inBufSize, uint32_t eventId, unsigned timeout, std::vector<uint8_t>& eventBinaryData,
        uint32_t commandId, uint32_t moduleId = 0, uint32_t subtypeId = 0) const;

    const FwStateProvider* GetFwStateProvider() const { return m_fwStateProvider.get(); }

    bool IsContinuousPmcSupported() const;

    // ************************ Custom Regs *************************//
    OperationStatus AddCustomRegister(const std::string& name, uint32_t address, uint32_t firstBit, uint32_t lastBit);

    OperationStatus RemoveCustomRegister(const std::string& name);

    // Read values for all defined custom registers
    // Returns true when read operation succeeded for all registers
    OperationStatus ReadCustomRegisters(std::vector<StringNameValuePair>& customRegisters) const;
    // *********************** [END] Custom Regs *******************//

    // invalid address constant to be used during memory access
    static const uint32_t sc_invalidAddress = 0U;

    OperationStatus Read(uint32_t address, uint32_t& value) const;
    OperationStatus Write(uint32_t address, uint32_t value) const;
    OperationStatus ReadBlock(uint32_t address, uint32_t blockSize, std::vector<uint32_t>& values) const;
    OperationStatus ReadBlock(uint32_t address, uint32_t blockSizeInBytes, char *arrBlock) const;
    OperationStatus ReadRadarData(uint32_t maxBlockSize, std::vector<uint32_t>& values) const;
    OperationStatus WriteBlock(uint32_t address, const std::vector<uint32_t>& values) const;
    OperationStatus WriteBlock(uint32_t address, uint32_t blockSizeInBytes, const char* valuesToWrite) const;
    OperationStatus InterfaceReset() const;

    OperationStatus AllocPmc(unsigned descSize, unsigned descNum) const;
    OperationStatus DeallocPmc(bool bSuppressError) const;
    OperationStatus CreatePmcFiles(unsigned refNumber) const;
    OperationStatus FindPmcDataFile(unsigned refNumber, std::string& foundPmcFilePath) const;
    OperationStatus FindPmcRingFile(unsigned refNumber, std::string& foundPmcFilePath) const;

    OperationStatus PmcGetData(uint32_t& bufLenBytesInOut, uint8_t* pBuffer, bool& hasMoreData) const;

    OperationStatus GetStaInfo(std::vector<StaInfo>& staInfo) const;
    OperationStatus GetFwCapa(std::vector<uint32_t>& fwCapa) const;
    PmcContext& GetPmcContext() const { return *m_upPmcContext; }

private:

    // initialized in ctor body and never changed after
    BasebandType m_basebandType;
    BasebandRevision m_basebandRevision;
    const std::unique_ptr<DriverAPI> m_driver;
    std::unique_ptr<FwStateProvider> m_fwStateProvider;
    const std::string& m_deviceName; // alias to the Driver interface name

    std::atomic<bool> m_isSilent;
    std::unique_ptr<PmcContext> m_upPmcContext;
    std::map<std::string, std::unique_ptr<FieldDescription>> m_customRegistersMap;

    void InitializeBasebandRevision();
    void InitializeDriver(bool pollingRequired);
    OperationStatus ValidateIoOperation(uint32_t address) const;
};
