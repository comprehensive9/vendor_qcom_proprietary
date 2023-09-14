/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "DriverCommandHandlers.h"
#include "DebugLogger.h"
#include "DriverContracts.h"
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>

// Legacy command handler, assumes a single attribute of 32 bit
LegacyCmdHandler::LegacyCmdHandler(uint32_t& response)
    : IDriverCmdHandler()
    , m_response(response)
{}

int LegacyCmdHandler::HandleResponse(nlattr ** tb_nested)
{
    LOG_VERBOSE << "LegacyCmdHandler::HandleResponse" << std::endl;

    static const int attrIndex = 0; // assumes single attribute
    if (!tb_nested[attrIndex])
    {
        LOG_ERROR << "missing attribute #" << attrIndex << " for driver command response" << std::endl;
        return NL_SKIP;
    }

    const size_t len = nla_len(tb_nested[attrIndex]);
    if (len != sizeof(m_response))
    {
        LOG_ERROR << "invalid driver command response: expected " << sizeof(m_response) << " Bytes, but got " << len << std::endl;
        return NL_SKIP;
    }

    m_response = nla_get_u32(tb_nested[attrIndex]);
    return NL_SKIP;
}

// NL_60G_GEN_GET_DRIVER_CAPA command handler
// Note: Currently assumes that capabilities are encoded as a single DW.
//       Driver reply is wil_nl_60g_driver_capabilities_reply containing a dynamic array of u32.
DriverCapaCmdHandler::DriverCapaCmdHandler(uint32_t& capabilities)
    : IDriverCmdHandler()
    , m_capabilities(capabilities)
{}

int DriverCapaCmdHandler::HandleResponse(nlattr ** tb_nested)
{
    LOG_VERBOSE << "DriverCapaCmdHandler::HandleResponse" << std::endl;

    nlattr* driverCapaAttr = tb_nested[QCA_WLAN_VENDOR_ATTR_DRIVER_CAPA]; // cannot be const to comply with Android's nla_get_u32
    if (!driverCapaAttr)
    {
        LOG_ERROR << "missing QCA_WLAN_VENDOR_ATTR_DRIVER_CAPA attribute for NL_60G_GEN_GET_DRIVER_CAPA response" << std::endl;
        return NL_SKIP;
    }
    const size_t len = nla_len(driverCapaAttr);
    if (len != sizeof(m_capabilities))
    {
        LOG_ERROR << "NL_60G_GEN_GET_DRIVER_CAPA response invalid: expected " << sizeof(m_capabilities) << " Bytes, but got " << len << std::endl;
        return NL_SKIP;
    }

    m_capabilities = nla_get_u32(driverCapaAttr);
    return NL_SKIP;
}

// NL_60G_GEN_PMC_GET_DATA/NL_60G_GEN_PMC_GET_DESC_DATA command handler
PmcGetDataCmdHandler::PmcGetDataCmdHandler(uint32_t bufLenBytesIn, uint32_t& bufLenBytesOut, uint8_t* pBuffer, uint32_t& minDataLengthBytes, bool& hasMoreData)
    : IDriverCmdHandler()
    , m_bufLenBytesIn(bufLenBytesIn)
    , m_bufLenBytesOut(bufLenBytesOut)
    , m_pBuffer(pBuffer)
    , m_minDataLengthBytes(minDataLengthBytes)
    , m_hasMoreData(hasMoreData)
{}

int PmcGetDataCmdHandler::HandleResponse(nlattr ** tb_nested)
{
    LOG_VERBOSE << "PmcGetDataCmdHandler::HandleResponse" << std::endl;

    m_bufLenBytesOut = 0U;
    m_minDataLengthBytes = 0U;
    m_hasMoreData = false;

    // MIN_DATA_LENGTH
    // attribute exists only if the allocated buffer is too small
    nlattr* pmcMinDataLenAttr = tb_nested[QCA_WLAN_VENDOR_ATTR_PMC_MIN_DATA_LENGTH]; // cannot be const to comply with Android's nla_get_u32
    if (pmcMinDataLenAttr)
    {
        size_t len = nla_len(pmcMinDataLenAttr);
        if (len != sizeof(m_minDataLengthBytes))
        {
            LOG_ERROR << "QCA_WLAN_VENDOR_ATTR_PMC_MIN_DATA_LENGTH attribute invalid: expected "
                << sizeof(m_minDataLengthBytes) << " Bytes, but got " << len << std::endl;
            return NL_SKIP;
        }

        m_minDataLengthBytes = nla_get_u32(pmcMinDataLenAttr);
    }

    // PMC_DATA
    const nlattr* pmcDataAttr = tb_nested[QCA_WLAN_VENDOR_ATTR_PMC_DATA];
    if (pmcDataAttr)
    {
        m_bufLenBytesOut = nla_len(pmcDataAttr); // actual length of the buffer

        // ensure we don't exceed the allocated array boundaries before copy
        // note: non zero m_bufLenBytesOut bigger than bufLenBytesIn marks an error, detected by an upper level
        if (m_bufLenBytesOut <= m_bufLenBytesIn)
        {
            memcpy(m_pBuffer, nla_data(pmcDataAttr), m_bufLenBytesOut);
        }
    }
    // otherwise, m_bufLenBytesOut stays zero

    // QCA_WLAN_VENDOR_ATTR_PMC_DATA_LENGTH is set only if zero buffer length was requested
    // In that case the total data size available in the ring is reported, currently unused

    // MORE_DATA (boolean flag attribute)
    m_hasMoreData = tb_nested[QCA_WLAN_VENDOR_ATTR_PMC_MORE_DATA] != nullptr;

    return NL_SKIP;
}

DriverMemioCmdHandler::DriverMemioCmdHandler(uint32_t& value)
    : IDriverCmdHandler()
    , m_value(value)
{}

int DriverMemioCmdHandler::HandleResponse(nlattr ** tb_nested)
{
    LOG_VERBOSE << "DriverMemioCmdHandler::HandleResponse" << std::endl;

    nlattr* driverMemioAttr = tb_nested[QCA_WLAN_VENDOR_ATTR_MEMIO]; // cannot be const to comply with Android's nla_get_u32
    if (!driverMemioAttr)
    {
        LOG_ERROR << "missing QCA_WLAN_VENDOR_ATTR_MEMIO attribute for DRIVER_CMD_MEMIO response" << std::endl;
        return NL_SKIP;
    }
    const size_t len = nla_len(driverMemioAttr);
    if (len != sizeof(struct nl_memio))
    {
        LOG_ERROR << "DRIVER_CMD_MEMIO response invalid: expected " << sizeof(driverMemioAttr) << " Bytes, but got " << len << std::endl;
        return NL_SKIP;
    }

    m_value = reinterpret_cast<struct nl_memio *>(nla_data(driverMemioAttr))->val;
    return NL_SKIP;
}

DriverMemioBlkCmdHandler::DriverMemioBlkCmdHandler(uint32_t size, char *arrBlock)
    : IDriverCmdHandler()
    , m_size(size)
    , m_arrBlock(arrBlock)
{}

int DriverMemioBlkCmdHandler::HandleResponse(nlattr ** tb_nested)
{
    LOG_VERBOSE << "DriverMemioCmdHandler::HandleResponse" << std::endl;

    nlattr* driverMemioBlkAttr = tb_nested[QCA_WLAN_VENDOR_ATTR_MEMIO_BLOCK]; // cannot be const to comply with Android's nla_get_u32
    if (!driverMemioBlkAttr)
    {
        LOG_ERROR << "missing QCA_WLAN_VENDOR_ATTR_MEMIO_BLOCK attribute for DRIVER_CMD_MEMIO_BLOCK response" << std::endl;
        return NL_SKIP;
    }
    const size_t len = nla_len(driverMemioBlkAttr);
    if (len < sizeof(struct nl_memioBlock))
    {
        LOG_ERROR << "DRIVER_CMD_MEMIO_BLOCK response invalid: expected " << sizeof(driverMemioBlkAttr) << " Bytes, but got " << len << std::endl;
        return NL_SKIP;
    }

    memcpy(m_arrBlock, reinterpret_cast<struct nl_memioBlock *>(nla_data(driverMemioBlkAttr))->buf, m_size);
    return NL_SKIP;
}

DriverGetStaInfoCmdHandler::DriverGetStaInfoCmdHandler(std::vector<struct nl_60g_sta_info_entry>& staInfo)
    : IDriverCmdHandler()
    , m_staInfo(staInfo)
{}

int DriverGetStaInfoCmdHandler::HandleResponse(nlattr ** tb_nested)
{
    LOG_VERBOSE << "DriverGetStaInfoCmdHandler::HandleResponse" << std::endl;

    nlattr* driverGetStaInfoAttr = tb_nested[QCA_WLAN_VENDOR_ATTR_STA_INFO]; // cannot be const to comply with Android's nla_get_u32
    if (!driverGetStaInfoAttr)
    {
        LOG_ERROR << "missing QCA_WLAN_VENDOR_ATTR_STA_INFO attribute for NL_60G_GEN_GET_STA_INFO response" << std::endl;
        return NL_SKIP;
    }
    const size_t len = nla_len(driverGetStaInfoAttr);
    if (len < sizeof(struct nl_60g_sta_info))
    {
        LOG_ERROR << "very short stations info reply len = " << len << std::endl;
        return NL_SKIP;
    }

    struct nl_60g_sta_info *si = reinterpret_cast<struct nl_60g_sta_info *>(nla_data(driverGetStaInfoAttr));

    size_t expected_len = sizeof(struct nl_60g_sta_info) + si->num_cids * sizeof(struct nl_60g_sta_info_entry);
    if (len < expected_len)
    {
        LOG_ERROR << "stations info reply too short. Need " << expected_len << " have " << len << std::endl;
        return NL_SKIP;
    }

    m_staInfo.clear();
    for (uint16_t i = 0; i < si->num_cids; i++)
    {
        m_staInfo.emplace_back(si->stations[i]);
    }

    return NL_SKIP;
}

DriverFwCapaCmdHandler::DriverFwCapaCmdHandler(std::vector<uint32_t>& fwCapa)
    : IDriverCmdHandler()
    , m_fwCapa(fwCapa)
{}

int DriverFwCapaCmdHandler::HandleResponse(nlattr ** tb_nested)
{
    LOG_VERBOSE << "DriverFwCapaCmdHandler::HandleResponse" << std::endl;

    nlattr* driverFwCapaAttr = tb_nested[QCA_WLAN_VENDOR_ATTR_FW_CAPA]; // cannot be const to comply with Android's nla_get_u32
    if (!driverFwCapaAttr)
    {
        LOG_ERROR << "missing QCA_WLAN_VENDOR_ATTR_FW_CAPA attribute for DRIVER_CMD_MEMIO_BLOCK response" << std::endl;
        return NL_SKIP;
    }
    const size_t len = nla_len(driverFwCapaAttr);
    if (len == 0)
    {
        LOG_ERROR << "NL_60G_GEN_GET_FW_CAPA response invalid: expected " << sizeof(driverFwCapaAttr) << " Bytes, but got " << len << std::endl;
        return NL_SKIP;
    }

    uint32_t *buf = reinterpret_cast<uint32_t *>(nla_data(driverFwCapaAttr));
    const size_t dlen = len / sizeof(uint32_t);

    m_fwCapa.clear();
    m_fwCapa = std::vector<uint32_t>(buf, buf + dlen);

    return NL_SKIP;
}