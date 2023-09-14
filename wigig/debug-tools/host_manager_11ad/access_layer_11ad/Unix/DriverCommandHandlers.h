/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <netlink/netlink.h>
#include <vector>
#include "nl_contracts.h"


 // Interface for driver command handler that allows to extract all relevant response parts
class IDriverCmdHandler
{
public:
    virtual ~IDriverCmdHandler() = default;

    virtual int HandleResponse(nlattr ** tb_nested) = 0;
};


// Legacy command handler, assumes a single attribute of 32 bit
class LegacyCmdHandler : public IDriverCmdHandler
{
public:
    LegacyCmdHandler(uint32_t& response);

    int HandleResponse(nlattr ** tb_nested) override;

private:
    uint32_t& m_response;
};

// NL_60G_GEN_GET_DRIVER_CAPA command handler
// Note: Currently assumes that capabilities are encoded as a single DW.
//       Driver reply is wil_nl_60g_driver_capabilities_reply containing a dynamic array of u32.
class DriverCapaCmdHandler : public IDriverCmdHandler
{
public:
    DriverCapaCmdHandler(uint32_t& capabilities);

    int HandleResponse(nlattr ** tb_nested) override;

private:
    uint32_t& m_capabilities;
};

// NL_60G_GEN_PMC_GET_DATA/NL_60G_GEN_PMC_GET_DESC_DATA command handler
class PmcGetDataCmdHandler : public IDriverCmdHandler
{
public:
    PmcGetDataCmdHandler(uint32_t bufLenBytesIn, uint32_t& bufLenBytesOut, uint8_t* pBuffer, uint32_t& minDataLengthBytes, bool& hasMoreData);

    int HandleResponse(nlattr ** tb_nested) override;

private:
    const uint32_t m_bufLenBytesIn; // length of the provided buffer
    uint32_t& m_bufLenBytesOut;     // updated with amount of retrieved data
    uint8_t* m_pBuffer;             // pointer to a preallocated memory
    uint32_t& m_minDataLengthBytes; // minimum retrieve size
    bool& m_hasMoreData;            // there are more data to retrieve
};

class DriverMemioCmdHandler : public IDriverCmdHandler
{
public:
    explicit DriverMemioCmdHandler(uint32_t& value);

    int HandleResponse(nlattr ** tb_nested) override;

private:
    uint32_t& m_value;
};

class DriverMemioBlkCmdHandler : public IDriverCmdHandler
{
public:
    DriverMemioBlkCmdHandler(uint32_t size, char *arrBlock);

    int HandleResponse(nlattr ** tb_nested) override;

private:
    uint32_t m_size;
    char *m_arrBlock;
};

class DriverGetStaInfoCmdHandler : public IDriverCmdHandler
{
public:
    DriverGetStaInfoCmdHandler(std::vector<struct nl_60g_sta_info_entry>& staInfo);

    int HandleResponse(nlattr ** tb_nested) override;

private:
    std::vector<struct nl_60g_sta_info_entry>& m_staInfo;
};

class DriverFwCapaCmdHandler : public IDriverCmdHandler
{
public:
    DriverFwCapaCmdHandler(std::vector<uint32_t>& fwCapa);

    int HandleResponse(nlattr ** tb_nested) override;

private:
    std::vector<uint32_t>& m_fwCapa;
};