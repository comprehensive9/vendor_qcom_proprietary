/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <ostream>

/*
 * The below are contracts between concrete driver class implementation and the network driver (kernel or DPDK).
 * These contracts are OS independent and are common for all supported drivers.
 */

// Enumeration for commands sent to the Driver
// Note: It is a contract with DmTools and the Driver, order is important!
enum class wil_nl_60g_cmd_type
{
    DRIVER_CMD_FW_WMI                   = 0, // NL_60G_CMD_FW_WMI
    DRIVER_CMD_GENERIC_COMMAND          = 1, // NL_60G_CMD_GENERIC
    DRIVER_CMD_GET_DRIVER_STATISTICS    = 2, // NL_60G_CMD_STATISTICS
    DRIVER_CMD_REGISTER                 = 3, // NL_60G_CMD_REGISTER
    DRIVER_CMD_FW_RMI                   = 4, // NL_60G_CMD_FW_RMI
    DRIVER_CMD_MEMIO                    = 5, // NL_60G_CMD_MEMIO
    DRIVER_CMD_MEMIO_BLOCK              = 6, // NL_60G_CMD_MEMIO_BLOCK
    DRIVER_CMD_PMC                      = 7, // NL_60G_CMD_PMC
};

// enumeration of generic commands supported by the Driver
enum class wil_nl_60g_generic_cmd
{
    NL_60G_GEN_FORCE_WMI_SEND       = 0,
    NL_60G_GEN_RADAR_ALLOC_BUFFER   = 1,
    NL_60G_GEN_FW_RESET             = 2,
    NL_60G_GEN_GET_DRIVER_CAPA      = 3,
    NL_60G_GEN_GET_FW_STATE         = 4,
    NL_60G_GEN_AUTO_RADAR_RX_CONFIG = 5,
    NL_60G_GEN_GET_STA_INFO         = 6,
    NL_60G_GEN_GET_FW_CAPA          = 7,
};

enum class wil_nl_60g_pmc_cmd
{
    NL_60G_GEN_PMC_ALLOC            = 0,
    NL_60G_GEN_PMC_FREE             = 1,
    NL_60G_GEN_PMC_GET_DATA         = 2,
    NL_60G_GEN_PMC_GET_DESC_DATA    = 3,
};

enum class wil_nl_60g_evt_type
{
    DRIVER_EVENT_DRIVER_ERROR           = 0, // NL_60G_EVT_DRIVER_ERROR
    DRIVER_EVENT_FW_ERROR               = 1, // NL_60G_EVT_FW_ERROR
    DRIVER_EVENT_FW_WMI_EVENT           = 2, // NL_60G_EVT_FW_WMI
    DRIVER_EVENT_DRIVER_SHUTDOWN        = 3, // NL_60G_EVT_DRIVER_SHUTOWN
    DRIVER_EVENT_DRIVER_DEBUG_EVENT     = 4, // NL_60G_EVT_DRIVER_DEBUG_EVENT
    DRIVER_EVENT_DRIVER_GENERIC_EVENT   = 5, // NL_60G_EVT_DRIVER_GENERIC
    DRIVER_EVENT_FW_RMI_EVENT           = 6, // NL_60G_EVT_FW_RMI
};

// enumeration for driver capabilities, keep same type name defined in the driver
enum class wil_nl_60g_driver_capa
{
    NL_60G_DRIVER_CAPA_WMI = 0,             // WMI transport to/from the Operational mailbox
    NL_60G_DRIVER_CAPA_FW_STATE,            // notifications of FW health state changes
    NL_60G_DRIVER_CAPA_IOCTL_WRITE,         // ioctl to write to the device address space
    NL_60G_DRIVER_CAPA_MEMIO,               // memio using NL, read dword/block
    NL_60G_DRIVER_CAPA_MEMIO_WRITE,         // memio using NL, write dword/block support
    NL_60G_DRIVER_CAPA_GET_STA_INFO,        // provides the GET_STA_INFO generic command
    NL_60G_DRIVER_CAPA_LEGACY_PMC_OVER_NL,  // Legacy PMC control is supported over netlink
    NL_60G_DRIVER_CAPA_CONTINUOUS_PMC,      // FW/uCode Logs through continuous PMC mechanism
};

// Returns FW capability name corresponding to given FW capability bit or the bit number when capability is not defined
const char* DriverCapaToString(int capability);


// Structure definition for the driver event report
#define DRIVER_MSG_MAX_LEN 2048 // buffer size for driver events, contract with the Driver - do NOT change!
#ifdef _WINDOWS
__pragma(pack(push, 1))
#endif
struct driver_event_report
{
    wil_nl_60g_evt_type evt_type;
    uint32_t buf_len;
    uint32_t evt_id;          // event index, keeping track of events in the queue, set by part of the drivers
    uint8_t  more_events_inQ; // more events in queue, set by part of the drivers
    uint32_t listener_id;     // set by part of the drivers
    unsigned char buf[DRIVER_MSG_MAX_LEN];
#ifdef _WINDOWS
}; __pragma(pack(pop))
#else
 } __attribute__((packed));
#endif

// Enumeration of FW health states as defined by wil6210 driver
enum class wil_fw_state
{
    WIL_FW_STATE_UNKNOWN = 0,           // uninitialized (also may mark debug_fw)
    WIL_FW_STATE_DOWN,                  // FW not loaded or not ready yet
    WIL_FW_STATE_READY,                 // FW is ready
    WIL_FW_STATE_ERROR_BEFORE_READY,    // Detected FW error before FW sent ready indication
    WIL_FW_STATE_ERROR,                 // Detected FW error after FW sent ready indication
    WIL_FW_STATE_UNRESPONSIVE           // FW operation error, does not exist in driver enumeration
                                        // (scan/connect timeout or PCIe link down detected by the driver)
};

std::ostream& operator<<(std::ostream &os, wil_fw_state fwHealthState);

enum class wil_nl_60g_generic_evt
{
    NL_60G_GEN_EVT_FW_STATE = 0,
    NL_60G_GEN_EVT_AUTO_RADAR_DATA_READY,
};

struct wil_nl_60g_fw_state_event
{
    wil_nl_60g_generic_evt evt_id;
    wil_fw_state fw_sts;
};

// a single station entry for NL_60G_GET_STA_INFO, the index in the array is the CID
#ifdef _WINDOWS
__pragma(pack(push, 1))
#endif
struct nl_60g_sta_info_entry
{
    uint8_t mac_addr[6];
    uint16_t aid;
    uint8_t status; /* enum wil_sta_status */
    uint8_t mid;
    uint8_t reserved[2];
#ifdef _WINDOWS
}; __pragma(pack(pop))
#else
} __attribute__((packed));
#endif

 struct nl_60g_sta_info
 {
     uint32_t num_cids;
     struct nl_60g_sta_info_entry stations[];
 };

enum class PmcDataType
{
    PMC_RING_PAYLOAD,
    PMC_RING_DESCRIPTORS
};

std::ostream& operator<<(std::ostream& os, const PmcDataType& dataType);

enum class IfindexState
{
    IFINDEX_STATE_MISSING   = 0,
    IFINDEX_STATE_MATCH     = 1,
    IFINDEX_STATE_MISMATCH  = 2,
    IFINDEX_STATE_DPDK      = 3,
};
