/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"
#include "DeviceTypes.h"

#include <map>
#include <string>

class Device;

//This class is responsible for reading and writing to and from PMC registers
class PmcRegisterInfo
{
    public:

        // Constructs default invalid instance
        PmcRegisterInfo() :
            m_address(0), m_bitStart(0), m_bitEnd(0)
        {
        }

        PmcRegisterInfo(uint32_t regAddress, uint32_t regBitStart, uint32_t regBitEnd) :
                m_address(regAddress), m_bitStart(regBitStart), m_bitEnd(regBitEnd)
        {
        }

        bool IsValid() const
        {
            return m_address != 0;
        }

        uint32_t GetRegisterAddress() const
        {
            return m_address;
        }

        uint32_t GetBitStart() const
        {
            return m_bitStart;
        }

        uint32_t GetBitEnd() const
        {
            return m_bitEnd;
        }

    private:

        const uint32_t m_address;
        const uint32_t m_bitStart;
        const uint32_t m_bitEnd;
};

std::ostream& operator<<(std::ostream& os, const PmcRegisterInfo& pmcRegInfo);

class PmcRegistersAccessor
{
    public:

        explicit PmcRegistersAccessor(const Device& device);

        OperationStatus WritePmcRegister(const std::string& registerName, uint32_t value) const;
        OperationStatus ReadPmcRegister(const std::string& registerName, uint32_t& registerValue) const;

        static uint32_t WriteToBitMask(uint32_t dataBufferToWriteTo, uint32_t index, uint32_t size, uint32_t valueToWriteInData);

        // expose register info to be able to cache mnemonic address for a periodic access
        static PmcRegisterInfo GetPmcRegisterInfo(const std::string& registerName, const BasebandRevision deviceType);
    private:

        const Device& m_device;

        //register name => {device name, registerInfo}
        static const std::map<std::string, std::map<BasebandRevision, PmcRegisterInfo>> s_pmcRegisterMap;

        static uint32_t GetBitMask(uint32_t index, uint32_t size);
        static uint32_t ReadFromBitMask(uint32_t data, uint32_t index, uint32_t size);

};