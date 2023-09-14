/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "DriverAPI.h"
#include "OperationStatus.h"
#include "UnixDriverEventsTransport.h"
#include <string>
#include <vector>

class UnixDPDKDriver : public DriverAPI
{
public:
    explicit UnixDPDKDriver(const std::string& interfaceName);

    ~UnixDPDKDriver() override;

    // Base access functions (to be implemented by specific device)
    OperationStatus Read(uint32_t address, uint32_t& value) override;
    OperationStatus Read(uint32_t address, uint32_t& value, bool& retriableError);
    OperationStatus ReadBlock(uint32_t address, uint32_t blockSizeInDwords, std::vector<uint32_t>& values) override;
    OperationStatus ReadBlock(uint32_t addr, uint32_t blockSizeInBytes, char *arrBlock) override;
    OperationStatus Write(uint32_t address, uint32_t value) override;
    OperationStatus WriteBlock(uint32_t addr, std::vector<uint32_t> values) override;
    OperationStatus WriteBlock(uint32_t address, uint32_t blockSizeInBytes, const char *valuesToWrite) override;
    OperationStatus RetryWhenBusyRead(uint32_t /*address*/, uint32_t& /*value*/) override;
    OperationStatus GetStaInfo(std::vector<StaInfo>& staInfo) override;
    OperationStatus GetFwState(wil_fw_state& fwState) override;
    OperationStatus GetFwCapa(std::vector<uint32_t>& fwCapa) override;

    OperationStatus DriverControl(uint32_t id, const void *inBuf, uint32_t inBufSize, void *outBuf, uint32_t outBufSize) override;
    OperationStatus SendWmiWithEvent(const void *inBuf, uint32_t inBufSize, uint32_t commandId, uint32_t moduleId, uint32_t subtypeId,
        uint32_t eventId, unsigned timeoutSec, std::vector<uint8_t>& eventBinaryData) override;

    OperationStatus AllocPmc(unsigned descSize, unsigned descNum) override;
    OperationStatus DeallocPmc(bool bSuppressError) override;
    OperationStatus CreatePmcFiles(unsigned refNumber) override;
    OperationStatus FindPmcDataFile(unsigned refNumber, std::string& foundPmcFilePath) override;
    OperationStatus FindPmcRingFile(unsigned refNumber, std::string& foundPmcFilePath) override;

    bool InitializeTransports() override;
    void RegisterDriverControlEvents(FwStateProvider* fwStateProvider) override;
    void StopDriverControlEvents() override;

    OperationStatus InterfaceReset() override;

private:
    struct IoctlIO
    {
        uint32_t op;
        uint32_t addr; /* should be 32-bit aligned */
        uint32_t val;
    };

    struct IoctlIOBlock
    {
        uint32_t op;
        uint32_t addr; /* should be 32-bit aligned */
        uint32_t size; /* represents the size in bytes. should be multiple of 4 */
        void* buf; /* block address */
    };

    struct IoctlRadarBlock
    {
        uint32_t max_size; /* maximal buffer size in bytes. should be multiple of 4 */
        void* buf;
    };

    bool IsReady() const;
    OperationStatus PmcGetDataInternal(PmcDataType dataType, std::vector<uint8_t>& pmcData);

    int m_fileDescriptor;
    unsigned m_lastPmcDscSize;
    unsigned m_lastPmcDescNum;
    UnixDriverEventsTransport m_driverEventsHandler; // internal class for operations related to send/receive of Driver commands/events
};
