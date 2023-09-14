/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#ifndef _11AD_UNIX_RTL_DRIVER_H_
#define _11AD_UNIX_RTL_DRIVER_H_

#include "DriverAPI.h"
#include "RtlSimCommand.h"

#include <string>
#include <set>
#include <vector>

// *************************************************************************************************

class UnixRtlDriver : public DriverAPI
{
public:

    explicit UnixRtlDriver(const std::string& interfaceName);

    OperationStatus Read(uint32_t address, uint32_t& value) override;
    OperationStatus ReadBlock(uint32_t address, uint32_t blockSize, std::vector<uint32_t>& values) override;
    OperationStatus Write(uint32_t address, uint32_t value) override;
    OperationStatus WriteBlock(uint32_t addr, std::vector<uint32_t> values) override;

private:

    static const int SIMULATOR_REPLY_TIMEOUT_MSEC = 500000;

    const std::string m_RtlDirectory;
    const std::string m_InterfacePath;
    const std::string m_ToRtlCmdFileName;
    const std::string m_ToRtlDoneFileName;
    const std::string m_FromRtlDoneFileName;
    const std::string m_FromRtlReplyFileName;

    bool ExecuteCommand(const IRtlSimCommand& cmd, std::vector<uint32_t>& refReply) const;
    bool WriteCommandFile(const IRtlSimCommand& cmd) const;
    bool WriteDoneFile() const;
    bool WaitForSimulatorDoneFile(const IRtlSimCommand& cmd) const;
    bool SimulatorDoneFileExists() const;
    bool ReadSimulatorReply(const IRtlSimCommand& cmd, std::vector<uint32_t>& reply) const;
};

#endif //_11AD_UNIX_PCI_DRIVER_H_
