/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <cstdint>

#ifndef SOL_NETLINK
#define SOL_NETLINK    270
#endif

#define OUI_QCA 0x001374
#define NL_MSG_SIZE_MAX (8 * 1024)

enum qca_nl80211_vendor_subcmds
{
    QCA_NL80211_VENDOR_SUBCMD_UNSPEC = 0,
};

// attributes set when sending a command
enum qca_wlan_vendor_attr
{
    QCA_WLAN_VENDOR_ATTR_TYPE = 7,
    QCA_WLAN_VENDOR_ATTR_BUF  = 8,
    QCA_WLAN_VENDOR_ATTR_MAX,
};

// attribute for NL_60G_GEN_GET_DRIVER_CAPA
enum qca_wlan_vendor_driver_capa
{
    QCA_WLAN_VENDOR_ATTR_DRIVER_CAPA,
};

// attribute for NL_60G_GEN_GET_STA_INFO
enum qca_wlan_vendor_nl60g_sta_info
{
    QCA_WLAN_VENDOR_ATTR_STA_INFO,
};

// attribute for NL_60G_GEN_GET_FW_STATE
enum qca_wlan_vendor_driver_fw_state
{
    QCA_WLAN_VENDOR_ATTR_DRIVER_FW_STATE, /* wil_fw_state */
};

// attributes for NL_60G_GEN_PMC_GET_DATA and NL_60G_GEN_PMC_GET_DESC_DATA
enum qca_wlan_vendor_pmc_get_data
{
    QCA_WLAN_VENDOR_ATTR_PMC_DATA_LENGTH,       // U32, total length of data returned, in bytes, max 32 KB
    QCA_WLAN_VENDOR_ATTR_PMC_DATA,              // payload data
    QCA_WLAN_VENDOR_ATTR_PMC_MORE_DATA,         // boolean flag (true when attribute exists)
    QCA_WLAN_VENDOR_ATTR_PMC_MIN_DATA_LENGTH,   // U32, minimum retrieve size (Bytes)
};

/* Definition of WMI command/event payload. In case of event, it's the content of buffer in driver_event_report. */
struct wil_nl_60g_send_receive_wmi
{
    uint32_t cmd_id;    /* command or event id */
    uint16_t reserved;  /* reserved for context id, not in use */
    uint8_t  dev_id;    /* mid, not in use */
    uint16_t buf_len;
    uint8_t  buf[];
} __attribute__((packed));

struct wil_nl_60g_send_receive_rmi
{
    uint8_t version;
    uint8_t reserved_1[3];
    uint16_t cmd_id;    /* command or event id */
    uint16_t buf_len;   /* payload length in bytes, without header */
    uint32_t token;     /* command or associated command token, zero means NA */
    uint32_t timestamp;
    uint32_t reserved_2[2];
    uint8_t  buf[];
} __attribute__((packed));

// NL_60G_CMD_PMC payload structures
struct nl60g_generic
{
    uint32_t cmd_id;
};

struct nl60g_pmc_alloc
{
    uint32_t num_desc; /* number of descriptors in the PMC ring */
    uint32_t payload_size;
};

/* used for NL_60G_PMC_GET_DATA and NL_60G_PMC_GET_DESC_DATA */
struct nl60g_pmc_get_data
{
    uint32_t bytes_num; /* number of bytes in the host buffer */
};

union nl60g_pmc_cmd
{
    struct nl60g_pmc_alloc alloc;
    struct nl60g_pmc_get_data get_data;
};

struct nl60g_pmc
{
    struct nl60g_generic hdr;
    union nl60g_pmc_cmd cmd;
};

enum qca_wlan_vendor_nl60g_memio
{
    QCA_WLAN_VENDOR_ATTR_MEMIO,
    QCA_WLAN_VENDOR_ATTR_MEMIO_BLOCK,
};

enum qca_wlan_vendor_fw_capa
{
    QCA_WLAN_VENDOR_ATTR_FW_CAPA,
};

/*
 * for reading/writing memory dword and block
 */
enum wil_memio_op
{
    wil_mmio_read = 0,
    wil_mmio_write = 1,
    wil_mmio_op_mask = 0xff,
    wil_mmio_addr_linker = 0 << 8,
    wil_mmio_addr_ahb = 1 << 8,
    wil_mmio_addr_bar = 2 << 8,
    wil_mmio_addr_mask = 0xff00,
};

//temp - alex
struct nl_memio
{
    uint32_t op;
    uint32_t addr; /* should be 32-bit aligned */
    uint32_t val;
};

struct nl_memioBlock
{
    uint32_t op;
    uint32_t addr; /* should be 32-bit aligned */
    uint32_t size; /* represents the size in bytes. should be multiple of 4 */
    uint8_t buf[]; /* block address */
};
