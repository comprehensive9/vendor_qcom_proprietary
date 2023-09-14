/*==========================================================================
Description
  It has implementation for logger class

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once

#include <hidl/HidlSupport.h>
#include <vector>
#include <string>
#include <stack>
#include "ring_buffer.h"
#include "uart_ipc.h"
#include "hci_internals.h"
#include "diag_interface.h"
#include "state_info.h"

#define LOG_YEAR_LENGTH 4
#define LOG_TS_UNIT_LENGTH 2
#define LOG_FILE_TS_LENGTH 23
#define LOGS_EXTN ".log"
#define SNOOP_EXTN ".cfa"
#define FW_DUMP_EXTN ".bin"
#define LOGS_TS           "%.04d-%.02d-%.02d_%.02d-%.02d-%.02d"
#define LOG_COLLECTION_DIR "/data/vendor/ssrdump/"
#define SOC_DUMP_PREFIX "ramdump_bt_fw_crashdump_"
#define SOC_DUMP_PATH LOG_COLLECTION_DIR SOC_DUMP_PREFIX LOGS_TS FW_DUMP_EXTN
#define SNOOP_FILE_PATH  LOG_COLLECTION_DIR SNOOP_FILE_NAME_PREFIX LOGS_TS SNOOP_EXTN
#define UART_IPC_TX_LOGS_DEST_PATH  LOG_COLLECTION_DIR UART_IPC_TX_LOG_PREFIX LOGS_TS LOGS_EXTN
#define UART_IPC_RX_LOGS_DEST_PATH LOG_COLLECTION_DIR UART_IPC_RX_LOG_PREFIX LOGS_TS LOGS_EXTN
#define UART_IPC_STATE_LOGS_DEST_PATH LOG_COLLECTION_DIR UART_IPC_STATE_LOG_PREFIX LOGS_TS LOGS_EXTN
#define UART_IPC_PWR_LOGS_DEST_PATH LOG_COLLECTION_DIR UART_IPC_PWR_LOG_PREFIX LOGS_TS LOGS_EXTN
#define BT_STATE_FILE_PATH LOG_COLLECTION_DIR BT_STATE_FILE_NAME_PREFIX LOGS_TS LOGS_EXTN


/* Packet types */
#define LOG_BT_CMD_PACKET_TYPE     0x01
#define LOG_BT_ACL_PACKET_TYPE     0x02
#define LOG_BT_SCO_PACKET_TYPE     0x03
#define LOG_BT_EVT_PACKET_TYPE     0x04
#define LOG_ANT_CTL_PACKET_TYPE    0x0c
#define LOG_ANT_DATA_PACKET_TYPE   0x0e
#define LOG_BT_EVT_VENDOR_SPECIFIC 0xFF
#define LOG_BT_EVT_CMD_CMPLT 0x0E

/* Message type of the log from controller */
#define LOG_BT_CONTROLLER_LOG        0x01
#define LOG_BT_MESSAGE_TYPE_VSTR     0x02
#define LOG_BT_MESSAGE_TYPE_PACKET   0x05
#define LOG_BT_MESSAGE_TYPE_MEM_DUMP 0x08
#define LOG_BT_MESSAGE_TYPE_HW_ERR   0x09
#define LOG_BT_HCI_BUFFER_DUMP       0x0A

/* Offset related to different crash dumps */
#define MINI_DUMP_ARM_REG_SIZE_OFFSET   10
#define MINI_DUMP_CALL_STK_SIZE_OFFSET  87
#define HCI_BUFFER_DUMP_SIZE_OFFSET     4
#define DUMP_PKT_SIZE_OFFSET            1

#define MAX_BQR_VS_PARAMS_SIZE          251
#define BQR_RIE_VS_PARAM_COUNT_OFFSET   6
#define BQR_RIE_VS_ERR_CODE_OFFSET      5
#define BQR_RIE_VSP_META_DATA_SIZE      2
#define MAX_BQR_RIE_PKT_SIZE            257

/* Sub log ID for the message type PACKET */
#define LOG_BT_HCI_CMD   0
#define LOG_BT_HCI_EVENT 1

#define LOG_BT_RX_LMP_PDU      18
#define LOG_BT_TX_LMP_PDU      19
#define LOG_BT_RX_LE_CTRL_PDU  20
#define LOG_BT_TX_LE_CTRL_PDU  21
#define LOG_BT_TX_LE_CONN_MNGR 22

#define LOG_BT_LINK_MANAGER_STATE    0x80
#define LOG_BT_CONN_MANAGER_STATE    0x81
#define LOG_BT_SECURITY_STATE        0x82
#define LOG_BT_LE_CONN_MANAGER_STATE 0x83
#define LOG_BT_LE_CHANNEL_MAP_STATE  0x84
#define LOG_BT_LE_ENCRYPTION_STATE   0x85

/* Sub log ID for the message type VSTR */
#define LOG_BT_VSTR_ERROR 0
#define LOG_BT_VSTR_HIGH  1
#define LOG_BT_VSTR_LOW   2

/* Diag ID for LMP packers */
#define LOG_BT_DIAG_LMP_LOG_ID 0x1041
#define LOG_BT_DIAG_LMP_RX_ID  0x1048
#define LOG_BT_DIAG_LMP_TX_ID  0x1049

/* To format LMP logs */
#define LOG_BT_QXDM_PKT_LENGTH_POS    0
#define LOG_BT_QXDM_PKT_LENGTH2_POS   1
#define LOG_BT_QXDM_DEVICE_IDX_POS    2
#define LOG_BT_QXDM_PKT_POS           3

#define LOG_BT_DBG_DEVICE_IDX_POS 0
#define LOG_BT_DBG_PKT_LENGTH_POS 1
#define LOG_BT_DBG_PKT_POS 2

/* Headed size of the log */
#define LOG_BT_HEADER_SIZE (sizeof(bt_log_pkt) - 1)
#define CRASH_SOURCE_FILE_PATH_LEN 50

#define MAX_RING_BUF_SIZE (8*1024)
#define OUT_RING_BUF 0
#define IN_RING_BUF 1

#define HOST_SPECIFIC_HW_ERR_EVT 0x0f
#define BT_HW_ERR_EVT            0x10
#define BT_HW_ERR_FRAME_SIZE     0x01

//Download rates (bytes/ms)
#define ELF_PATCH_SEQUENTIAL_DOWNLOAD_RATE  180
#define ELF_PATCH_SEQUENTIAL_DOWNLOAD_RATE_CC  120
#define RAM_PATCH_DOWNLOAD_RATE  228
#define RAM_PATCH_DOWNLOAD_RATE_WITH_CC 120
#define NVM_DOWNLOAD_RATE  35
#define XMEM_RAM_PATCH_DOWNLOAD_RATE  290
#define XMEM_RAM_PATCH_DOWNLOAD_RATE_WITH_CC 95
#define XMEM_NVM_PATCH_DOWNLOAD_RATE  23

//IDEAL DELAY TIME in ms
/* UART init will take approx 425ms.*/
#define UART_INIT_IDEAL_TIME_DELAY_CHE_BASED  500
#define UART_INIT_IDEAL_TIME_DELAY  20
#define PROPERTY_GET_IDEAL_TIME_DELAY  10
#define PROPERTY_SET_IDEAL_TIME_DELAY  10
#define RX_THREAD_START_IDEAL_TIME_DELAY  10
#define POWER_ON_REGS_IDEAL_TIME_DELAY  300
#define POWER_IOCTL_IDEAL_TIME_DELAY  20
#define POWER_ON_REGS_IDEAL_TIME_DELAY_CHE_BASED  70
#define FILE_SYSTEM_CALL_IDEAL_TIME_DELAY  30
#define UART_IOCTL_IDEAL_DELAY    30
#define DIAGINIT_IDEAL_TIME_DELAY  20
#define CMD_RSP_IDEAL_TIME_DELAY  20
#define BAUDRATE_CMD_RSP_IDEAL_TIME_DELAY  30
/* Variable time taken by below cmds are due additional
 * delay in calibration at SoC side when these cmd are processed.
 */
#define ADDON_FEATURE_CMD_RSP_IDEAL_TIME_DELAY  85
#define ADDON_FEATURE_CMD_RSP_IDEAL_TIME_DELAY_CHE3x  200
#define HCI_RESET_CMD_RSP_IDEAL_TIME_DELAY_CHE2x  130

#define DEFAULT_RAM_PATCH_TIME_DELAY  800
#define DEFAULT_NVM_PATCH_TIME_DELAY  150
#define DEFAULT_XMEM_RAM_PATCH_TIME_DELAY  3500
#define DEFAULT_XMEM_NVM_PATCH_TIME_DELAY  200
#define MAX_PATCH_DOWNLOAD_VARIABLE_DELAY  30

#define MAX_CONTINUOUS_SYSTEM_DELAY_CRASH_COUNT  10

// UART reason offset and bounds
#define UART_REASON_OFFSET         UART_REASON_DEFAULT
#define UART_ERR_CODE_LOWER_BOUND  UART_REASON_DEFAULT
#define UART_ERR_CODE_UPPER_BOUND  0x80

// SoC err mask
#define BT_SOC_ERR_MASK   0x80

#define LOG_I(tag, fmt, args...) ALOG(LOG_INFO, tag, fmt, ## args)
#define LOG_W(tag, fmt, args...) ALOG(LOG_WARN, tag, fmt, ## args)
#define LOG_E(tag, fmt, args...) ALOG(LOG_ERROR, tag, fmt, ## args)

#define IBS_LOG_INFO(tag, fmt, args...) if(Logger.GetIbsLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define IBS_LOG_WAR(tag, fmt, args...) if(Logger.GetIbsLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define IBS_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

#define RX_LOG_INFO(tag, fmt, args...) if(Logger.GetRxLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define RX_LOG_WAR(tag, fmt, args...) if(Logger.GetRxLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define RX_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

#define TX_LOG_INFO(tag, fmt, args...) if(Logger.GetTxLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define TX_LOG_WAR(tag, fmt, args...) if(Logger.GetTxLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define TX_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

#define GEN_LOG_INFO(tag, fmt, args...) if(Logger.GetGenLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define GEN_LOG_WAR(tag, fmt, args...) if(Logger.GetGenLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define GEN_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {
using CrashDumpStatusCallback = std::function<void(bool)>;
using HWEventCallback = std::function<void()>;

typedef enum {
  BT_SOC_REASON_DEFAULT                 =  0x00,
  // SoC Crash Reasons
  BT_SOC_REASON_UNKNOWN        =  0x81,
  BT_SOC_REASON_SW_REQUESTED   =  0x82,
  BT_SOC_REASON_STACK_OVERFLOW =  0x83,
  BT_SOC_REASON_EXCEPTION      =  0x84,
  BT_SOC_REASON_ASSERT         =  0x85,
  BT_SOC_REASON_TRAP           =  0x86,
  BT_SOC_REASON_OS_FATAL       =  0x87,
  BT_SOC_REASON_HCI_RESET      =  0x88,
  BT_SOC_REASON_PATCH_RESET    =  0x89,
  BT_SOC_REASON_ABT            =  0x8A,
  BT_SOC_REASON_RAMMASK        =  0x8B,
  BT_SOC_REASON_PREBARK        =  0x8C,
  BT_SOC_REASON_BUSERROR       =  0x8D,
  BT_SOC_REASON_IO_FATAL       =  0x8E,
  BT_SOC_REASON_SSR_CMD        =  0x8F,
  BT_SOC_REASON_POWERON        =  0x90,
  BT_SOC_REASON_WATCHDOG       =  0x91,
  BT_SOC_REASON_RAMMASK_RGN1   =  0x92,
  BT_SOC_REASON_RAMMASK_RGN0   =  0x93,
  BT_SOC_REASON_Q6_WATCHDOG    =  0x94,
  BT_SOC_REASON_ZEALIS_RAM_MASK_RGN0 =  0x95,
  BT_SOC_REASON_ZEALIS_RAM_MASK_RGN1 =  0x96,
  BT_SOC_REASON_APSS_RESET           =  0x97,
  BT_SOC_REASON_TIME_RESET           =  0x98,
  BT_SOC_REASON_AUDIOSS_RESET        =  0x99,
  BT_SOC_REASON_HOST_WARMRESET       =  0x9A,
  BT_SOC_REASON_HOST_NMI_INIT        =  0x9B,
  BT_SOC_REASON_TX_RX_INVALID_PKT_FATAL = 0xC0,
  BT_SOC_REASON_TX_RX_INVALID_LEN_FATAL = 0xC1,
  BT_SOC_REASON_INVALID_STACK  =  0xF0,

  // Transport Driver Crash Reasons
  BT_HOST_REASON_UARTINIT_STUCK        =  0x17,
  BT_HOST_REASON_GETVER_SEND_STUCK     =  0x18,
  BT_HOST_REASON_GETVER_NO_RSP_RCVD    =  0x19,
  BT_HOST_REASON_SETBAUDRATE_CMD_STUCK =  0x1A,
  BT_HOST_REASON_PATCH_DNLD_STUCK      =  0x1B,
  BT_HOST_REASON_GETBOARDID_CMD_STUCK  =  0x1C,
  BT_HOST_REASON_NVM_DNLD_STUCK        =  0x1D,
  BT_HOST_REASON_HCI_RESET_STUCK       =  0x1E,
  BT_HOST_REASON_GETBLDINFO_CMD_STUCK  =  0x1F,
  BT_HOST_REASON_ADDONFEAT_CMD_STUCK   =  0x20,
  BT_HOST_REASON_ENHLOG_CMD_STUCK      =  0x21,
  BT_HOST_REASON_DIAGINIT_STUCK        =  0x22,
  BT_HOST_REASON_DIAGDEINIT_STUCK      =  0x23,
  BT_HOST_REASON_XMEM_NVM_DNLD_STUCK   =  0x24,
  BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK =  0x25,
  BT_HOST_REASON_SECURE_BRIDGE_CMD_STUCK = 0x26,
  BT_HOST_REASON_FAILED_TO_SEND_CMD              =  0x27,
  BT_HOST_REASON_HCI_RESET_CC_NOT_RCVD           =  0x28,
  BT_HOST_REASON_HCI_PRE_SHUTDOWN_CC_NOT_RCVD    =  0x29,
  BT_HOST_REASON_HCI_SET_BD_ADDRESS_CC_NOT_RCVD  =  0x2A,
  BT_HOST_REASON_FAILED_TO_RECEIVE_SLEEP_IND     =  0x2B,
  BT_HOST_REASON_POWER_ON_REGS_STUCK             =  0x2C,
  BT_HOST_REASON_RX_THREAD_START_STUCK           =  0x2D,
  BT_HOST_REASON_GET_LOCALADDR_STUCK             =  0x2E,
  BT_HOST_REASON_OTP_INFO_GET_CMD_STUCK          =  0x2F,
  BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK          =  0x30,
  BT_HOST_REASON_PROPERTY_GET_STUCK              =  0x31,
  BT_HOST_REASON_PROPERTY_SET_STUCK              =  0x32,
  BT_HOST_REASON_RAM_PATCH_READ_STUCK            =  0x33,
  BT_HOST_REASON_NVM_PATCH_READ_STUCK            =  0x34,
  BT_HOST_REASON_UART_IOCTL_STUCK                =  0x35,
  BT_HOST_REASON_POWER_IOCTL_STUCK               =  0x36,
  BT_HOST_REASON_PATCH_CONFIG_CMD_STUCK          =  0x37,
  BT_HOST_REASON_GET_APP_VER_CMD_STUCK           =  0x38,
  // Exception in HOST reason, below is actually related to SoC.
  SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT =  0x3B,

  BT_HOST_REASON_GETVER_CMD_FAILED               = 0x3C,
  BT_HOST_REASON_BAUDRATE_CHANGE_FAILED          = 0x3D,
  BT_HOST_REASON_TLV_DOWNLOAD_FAILED             = 0x3E,
  BT_HOST_REASON_FW_BUILD_INFO_CMD_FAILED        = 0x3F,
  BT_HOST_REASON_HCI_RESET_CMD_FAILED            = 0x40,
  BT_HOST_REASON_UART_INIT_FAILED                = 0x41,
  BT_HOST_REASON_MEMORY_ALLOCATION_FAILED        = 0x42,
  BT_HOST_REASON_READ_THREAD_START_FAILED        = 0x43,
  BT_HOST_REASON_HW_FLOW_ON_FAILED               = 0x44,
  BT_HOST_REASON_NVM_FILE_NOT_FOUND              = 0x45,
  BT_HOST_REASON_UART_BAUDRATE_CHANGE_FAILED     = 0x46,
  BT_HOST_REASON_PATCH_CONFIG_FAILED             = 0x47,

  // UART driver crash reasons
  UART_REASON_DEFAULT                            =  0x51,
  UART_REASON_INVALID_FW_LOADED                  =  0x52,
  UART_REASON_CLK_GET_FAIL                       =  0x53,
  UART_REASON_SE_CLK_RATE_FIND_FAIL              =  0x54,
  UART_REASON_SE_RESOURCES_INIT_FAIL             =  0x55,
  UART_REASON_SE_RESOURCES_ON_FAIL               =  0x56,
  UART_REASON_SE_RESOURCES_OFF_FAIL              =  0x57,
  UART_REASON_TX_DMA_MAP_FAIL                    =  0x58,
  UART_REASON_TX_CANCEL_FAIL                     =  0x59,
  UART_REASON_TX_ABORT_FAIL                      =  0x5A,
  UART_REASON_TX_FSM_RESET_FAIL                  =  0x5B,
  UART_REASON_RX_CANCEL_FAIL                     =  0x5C,
  UART_REASON_RX_ABORT_FAIL                      =  0x5D,
  UART_REASON_RX_FSM_RESET_FAIL                  =  0x5E,
  UART_REASON_RX_TTY_INSET_FAIL                  =  0x5F,
  UART_REASON_ILLEGAL_INTERRUPT                  =  0x60,
  UART_REASON_BUFFER_OVERRUN                     =  0x61,
  UART_REASON_RX_PARITY_REASON                   =  0x62,
  UART_REASON_RX_BREAK_REASON                    =  0x63,
  UART_REASON_RX_SBE_REASON                      =  0x64,
  SOC_REASON_START_TX_IOS_SOC_RFR_HIGH           =  0x65,
} SecondaryReasonCode;

typedef struct {
  SecondaryReasonCode reason;
  char reasonstr[50];
} SecondaryReasonMap;

typedef struct {
  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t date = 0;
  uint8_t hour = 0;
  uint8_t min = 0;
  uint8_t sec = 0;
} LogFileTS;

struct LogFileName {
  std::string name;
  LogFileTS ts;
};

struct DelayListElement {
  SecondaryReasonCode activity_code;
  std::string activity_info;
  std::string start_time;
  std::string end_time;
  //in ms
  int time_diff;
  int time_taken;
  DelayListElement(SecondaryReasonCode code, std::string info, char* start_ts,
                    char* end_ts, int total_time, int delay) {
    activity_code = code;
    activity_info = info;
    start_time = std::string(start_ts);
    end_time = std::string(end_ts);
    time_taken = total_time;
    time_diff = delay;
  }
  bool operator>(const DelayListElement &activity2) const {
    return this->time_diff > activity2.time_diff;
  }
};

struct CurrentActivity {
  SecondaryReasonCode cur_activity_code;
  std::string cur_activity_info;
  struct timeval start_time_val;
  int recursive_overhead;
};

bool CompareLogicForDelayList(DelayListElement &activity1,
                                DelayListElement &activity2);

// BQR RIE vendor specific params IDs
enum BqrRieVsParamsOrder : uint8_t {
  PC_ADDRESS = 0x01,
  MINI_DUMP = 0x02,
  HCI_BUFFER_DUMP = 0x03,
};

static SecondaryReasonMap secReasonMap[] = {
  { BT_SOC_REASON_DEFAULT                      ,    "Default"},
  { BT_SOC_REASON_UNKNOWN                 ,    "Unknown"},
  { BT_SOC_REASON_TX_RX_INVALID_PKT_FATAL ,    "Tx/Rx invalid packet fatal error"},
  { BT_SOC_REASON_TX_RX_INVALID_LEN_FATAL ,    "Tx/Rx invalid length fatal error"},
  { BT_SOC_REASON_SW_REQUESTED            ,    "SW Requested"},
  { BT_SOC_REASON_STACK_OVERFLOW          ,    "Stack Overflow"},
  { BT_SOC_REASON_EXCEPTION               ,    "Exception"},
  { BT_SOC_REASON_ASSERT                  ,    "Assert"},
  { BT_SOC_REASON_TRAP                    ,    "Trap"},
  { BT_SOC_REASON_OS_FATAL                ,    "OS Fatal"},
  { BT_SOC_REASON_HCI_RESET               ,    "HCI Reset"},
  { BT_SOC_REASON_PATCH_RESET             ,    "Patch Reset"},
  { BT_SOC_REASON_ABT                     ,    "SoC Abort"},
  { BT_SOC_REASON_RAMMASK                 ,    "RAM MASK"},
  { BT_SOC_REASON_PREBARK                 ,    "PREBARK"},
  { BT_SOC_REASON_BUSERROR                ,    "Bus error"},
  { BT_SOC_REASON_IO_FATAL                ,    "IO fatal eror"},
  { BT_SOC_REASON_SSR_CMD                 ,    "SSR CMD"},
  { BT_SOC_REASON_POWERON                 ,    "Power ON"},
  { BT_SOC_REASON_WATCHDOG                ,    "Watchdog"},
  { BT_SOC_REASON_RAMMASK_RGN1            ,    "RAMMASK RGN1"},
  { BT_SOC_REASON_RAMMASK_RGN0            ,    "RAMMASK RGN0"},
  { BT_SOC_REASON_Q6_WATCHDOG             ,    "Q6 Watchdog"},
  { BT_SOC_REASON_ZEALIS_RAM_MASK_RGN0    ,    "ZEALIS RAM MASK RGN0"},
  { BT_SOC_REASON_ZEALIS_RAM_MASK_RGN1    ,    "ZEALIS RAM MASK RGN1"},
  { BT_SOC_REASON_APSS_RESET              ,    "APSS reset"},
  { BT_SOC_REASON_TIME_RESET              ,    "Time reset"},
  { BT_SOC_REASON_AUDIOSS_RESET           ,    "Audioss reset"},
  { BT_SOC_REASON_HOST_WARMRESET          ,    "Host warm reset"},
  { BT_SOC_REASON_HOST_NMI_INIT           ,    "Host NMI init"},
  { BT_SOC_REASON_INVALID_STACK           ,    "Invalid Stack"},
  { BT_HOST_REASON_UARTINIT_STUCK              ,    "UartInitStuck"},
  { BT_HOST_REASON_GETVER_SEND_STUCK           ,    "GetVerSendStuck"},
  { BT_HOST_REASON_GETVER_NO_RSP_RCVD          ,    "GetVerNoRspRcvd"},
  { BT_HOST_REASON_SETBAUDRATE_CMD_STUCK       ,    "SetBaudRateStuck"},
  { BT_HOST_REASON_PATCH_DNLD_STUCK            ,    "PatchDnldStuck"},
  { BT_HOST_REASON_GETBOARDID_CMD_STUCK        ,    "GetBoardIdStuck"},
  { BT_HOST_REASON_NVM_DNLD_STUCK              ,    "NvmDnldStuck"},
  { BT_HOST_REASON_HCI_RESET_STUCK             ,    "HciResetStuck"},
  { BT_HOST_REASON_GETBLDINFO_CMD_STUCK        ,    "GetBldInfoCmdStuck"},
  { BT_HOST_REASON_ADDONFEAT_CMD_STUCK         ,    "AddOnFeatCmdStuck"},
  { BT_HOST_REASON_ENHLOG_CMD_STUCK            ,    "EnhLogCmdStuck"},
  { BT_HOST_REASON_DIAGINIT_STUCK              ,    "DiagInitStuck"},
  { BT_HOST_REASON_DIAGDEINIT_STUCK            ,    "DiagDeinitStuck"},
  { BT_HOST_REASON_XMEM_NVM_DNLD_STUCK         ,    "XMEM NVM Download stuck"},
  { BT_HOST_REASON_XMEM_PATCH_DNLD_STUCK       ,    "XMEM patch download stuck"},
  { BT_HOST_REASON_SECURE_BRIDGE_CMD_STUCK     ,    "Secure bridge cmd stuck"},
  { BT_HOST_REASON_FAILED_TO_SEND_CMD          ,    "Failed to send internal cmd"},
  { BT_HOST_REASON_HCI_RESET_CC_NOT_RCVD       ,    "HCI Reset Cmd CC Not Rcvd"},
  { BT_HOST_REASON_HCI_PRE_SHUTDOWN_CC_NOT_RCVD    ,  "HCI Pre shutdown Cmd CC not Rcvd"},
  { BT_HOST_REASON_HCI_SET_BD_ADDRESS_CC_NOT_RCVD  ,  "HCI BD address CC not Rcvd"},
  { BT_HOST_REASON_FAILED_TO_RECEIVE_SLEEP_IND     ,  "Failed to receive SLEEP IND from SoC"},
  { BT_HOST_REASON_POWER_ON_REGS_STUCK             ,  "SoC Power ON Sequence stuck"},
  { BT_HOST_REASON_POWER_IOCTL_STUCK               ,  "Power driver IOCTL stuck"},
  { BT_HOST_REASON_RX_THREAD_START_STUCK           ,  "RX thread start stuck"},
  { BT_HOST_REASON_GET_LOCALADDR_STUCK             ,  "Get local BD address stuck"},
  { BT_HOST_REASON_OTP_INFO_GET_CMD_STUCK          ,  "Get OTP info. cmd stuck"},
  { BT_HOST_REASON_FILE_SYSTEM_CALL_STUCK          ,  "FILE system call stuck"},
  { BT_HOST_REASON_PROPERTY_GET_STUCK              ,  "Property get call stuck"},
  { BT_HOST_REASON_PROPERTY_SET_STUCK              ,  "Property set call stuck"},
  { BT_HOST_REASON_RAM_PATCH_READ_STUCK            ,  "RAM patch open/read stuck"},
  { BT_HOST_REASON_NVM_PATCH_READ_STUCK            ,  "NVM file open/read stuck"},
  { BT_HOST_REASON_UART_IOCTL_STUCK                ,  "UART IOCTL stuck"},
  { BT_HOST_REASON_PATCH_CONFIG_CMD_STUCK          ,  "Patch config cmd stuck"},
  { BT_HOST_REASON_GET_APP_VER_CMD_STUCK           ,  "Get APP version cmd stuck"},
  { SOC_REASON_START_TX_IOS_SOC_RFR_HIGH_DURING_INIT, "SoC RFR high during INIT"},
  { BT_HOST_REASON_GETVER_CMD_FAILED               ,  "Get Version cmd failed"},
  { BT_HOST_REASON_BAUDRATE_CHANGE_FAILED          ,  "Baudrate change failed"},
  { BT_HOST_REASON_TLV_DOWNLOAD_FAILED             ,  "TLV/NVM download failed"},
  { BT_HOST_REASON_FW_BUILD_INFO_CMD_FAILED        ,  "FW build info. cmd failed"},
  { BT_HOST_REASON_HCI_RESET_CMD_FAILED            ,  "HCI RESET cmd failed"},
  { BT_HOST_REASON_UART_INIT_FAILED                ,  "UART INIT failed"},
  { BT_HOST_REASON_MEMORY_ALLOCATION_FAILED        ,  "Memory allocation failed"},
  { BT_HOST_REASON_READ_THREAD_START_FAILED        ,  "Read thread start failed"},
  { BT_HOST_REASON_HW_FLOW_ON_FAILED               ,  "HW Flow ON failed"},
  { BT_HOST_REASON_NVM_FILE_NOT_FOUND              ,  "NVM file not found"},
  { BT_HOST_REASON_UART_BAUDRATE_CHANGE_FAILED     ,  "UART baudrate change failed"},
  { BT_HOST_REASON_PATCH_CONFIG_FAILED             ,  "Patch config cmd failed"},
  { UART_REASON_DEFAULT                            ,  "UART reason Default"},
  { UART_REASON_INVALID_FW_LOADED                  ,  "UART invalid FW loaded"},
  { UART_REASON_CLK_GET_FAIL                       ,  "UART CLK get failed"},
  { UART_REASON_SE_CLK_RATE_FIND_FAIL              ,  "UART CLK rate find failed"},
  { UART_REASON_SE_RESOURCES_INIT_FAIL             ,  "UART resources init failed"},
  { UART_REASON_SE_RESOURCES_ON_FAIL               ,  "UART resources ON failed"},
  { UART_REASON_SE_RESOURCES_OFF_FAIL              ,  "UART resources OFF failed"},
  { UART_REASON_TX_DMA_MAP_FAIL                    ,  "UART DMA map failed"},
  { UART_REASON_TX_CANCEL_FAIL                     ,  "UART tx cancel failed"},
  { UART_REASON_TX_ABORT_FAIL                      ,  "UART tx abort failed"},
  { UART_REASON_TX_FSM_RESET_FAIL                  ,  "UART tx FSM reset failed"},
  { UART_REASON_RX_CANCEL_FAIL                     ,  "UART rx cancel failed"},
  { UART_REASON_RX_ABORT_FAIL                      ,  "UART rx abort failed"},
  { UART_REASON_RX_FSM_RESET_FAIL                  ,  "UART rx FSM reset failed"},
  { UART_REASON_RX_TTY_INSET_FAIL                  ,  "UART rx tty inset failed"},
  { UART_REASON_ILLEGAL_INTERRUPT                  ,  "UART illegal interrupt"},
  { UART_REASON_BUFFER_OVERRUN                     ,  "UART buffer overrun"},
  { UART_REASON_RX_PARITY_REASON                   ,  "UART rx parity error"},
  { UART_REASON_RX_BREAK_REASON                    ,  "UART rx break error"},
  { UART_REASON_RX_SBE_REASON                      ,  "UART rx SBE error"},
  { SOC_REASON_START_TX_IOS_SOC_RFR_HIGH           ,  "SoC RFR high"},
};

/* This structure monitor_rx holds the information's
 * of incoming packets received  while sending wake
 * indications and time waited to receive wake ack.
 * lapsed_timeout: Hold the time waited to received
 * wake ack
 * num_valid_pkt_rx: counter will hold the information's
 * of valid packets received by HIDL when HIDL sends
 * wake indications.
 * prev_num_valid_pkt_rx: a temporary counter holds
 * information of number of valid packets received
 * before a new timeout.
 * is_monitor_enabled: set to true while sending
 * wake indications.
 */
struct monitor_rx {
  unsigned int lapsed_timeout;
  unsigned int num_valid_pkt_rx;
  unsigned int prev_num_valid_pkt_rx;
  bool is_monitor_enabled;
};

class Logger
{
 private:
  static Logger *instance_;
  static const diagpkt_user_table_entry_type ssr_bt_tbl_[];
  short int ibs_log_level_, rx_log_level_, tx_log_level_, gen_log_level_;
  int time_year, time_month, time_day, time_hour, time_min, time_sec;
  static bool any_reg_file;
  bool is_recovery_time_set_;
  PrimaryReasonCode dump_reason_;
  RingBuffer snoop_buff_;
  std::vector<std::string> dump_files;
  struct timeval ssrtime;
  std::vector<DelayListElement> delay_list;
  std::map<SecondaryReasonCode, int> ideal_time_mapper_;
  bool crashcmdneeded;
  bool is_activity_in_progress;
  bool is_delay_list_processed;
  std::stack<CurrentActivity> current_activities;
  std::mutex delay_list_mutex_;
  size_t hci_buffer_dump_size_;
  char hci_buffer_dump_buff_[HAL_DUMP_FILE_SIZE];
  size_t mini_dump_size_;
  char mini_dump_buff_[HAL_DUMP_FILE_SIZE];

#ifdef DUMP_IPC_LOG
  IpcLogs ipc_log_;
#endif
  DiagInterface diag_interface_;
  CrashDumpStatusCallback crash_dump_status_cb_;
  HWEventCallback hw_event_cb_;
  BtState *state_info_;
  SecondaryReasonCode uart_err_code_;
  SecondaryReasonCode init_failed_code_;

#ifdef DIAG_ENABLED
  static void *SsrBtHandler(void *, uint16);
#endif

  bool HandleHciEvent(HciPacketType, uint8_t *, uint16_t);
  void SaveSocMemDump(uint8_t *, uint16_t, PrimaryReasonCode);
  void DeleteDumpsIfRequired();
  void DeleteDumps(char *);
  void SetDumpTimestamp();
  void InitialIdealTimeTable();
  void CheckAndAddToDelayListHelper(struct timeval* tv_end);

  static bool isLastestLogFileName(const LogFileName& current, const LogFileName& latest);
  static LogFileName LastestLogFileName(const std::string& prefix, const std::string& dir);
  static void WriteToFd(int fd, const LogFileTS& latest_file_ts, const std::string prefix,
                        const std::string dir, const std::string log_sufix);
  time_t time_rx_call_back;
  uint32_t pc_address_;
  struct  monitor_rx rx_stats;
  bool is_this_fp_issue;
 public:
  Logger();
  enum {
    LOW = 0,
    MID,
    HIGH,
  } log_level;
  static std::recursive_mutex bugreport_mutex;
  static bool is_crash_dump_in_progress_;
  static pthread_mutex_t crash_dump_lock;
  static pthread_cond_t crash_dump_cond;
  static std::mutex bugreport_wait_mutex;
  static std::condition_variable bugreport_wait_cv;
  static bool is_bugreport_triggered_during_crash_dump;
  static bool is_bugreport_triggered;
  bool host_crash_during_init;
  bool stack_timeout_triggered;
  static Logger *Get(void);
  void UnlockDiagMutex();
  void GetCrashDumpFileName(char*);
#ifdef DUMP_RINGBUF_LOG
  void GetSnoopFileName(char*);
  void SetNormalSsrTrigered(bool);
  void SaveIbsToRingBuf(HciPacketType type,  uint8_t ibs_data) {
    /* IBS is a one byte command or event sent or received */
    snoop_buff_.AddLog(RingBuffer::IBS_BYTE, type, &ibs_data, 1);
  };
  void DumpRingBuffers(void) {
    snoop_buff_.DumpData();
  };
#endif

#ifdef DUMP_IPC_LOG
  void GetUartDumpFilename(short int, char*);
  void DumpUartIpcLogs(void) {
    ipc_log_.DumpUartLogs();
  };
#endif
  static void DumpLogsInBugreport(int fd);
  bool GetCleanupStatus(ProtocolType ptype);
  void StoreCrashReason(void);
  void FrameCrashEvent(hidl_vec<uint8_t>*bt_packet_);
  void FrameBqrRieEvent(hidl_vec<uint8_t>*bt_packet_);
  bool ProcessRx(HciPacketType type, const uint8_t *buff, uint16_t len);
  bool ProcessTx(HciPacketType, const uint8_t *, uint16_t);
  bool IsSnoopLogEnabled(void);
  bool IsHciFwSnoopEnabled(void);

  void Cleanup(void);

  bool IsControllerLogPkt(HciPacketType type, const uint8_t *buff, uint16_t len);
  bool IsCrashDumpStarted(void);
  bool RegisterCrashDumpCallback (CrashDumpStatusCallback crash_dump_cb);
  void RegisterHWEventCallback(HWEventCallback);
  bool PrepareDumpProcess();
  bool isSsrTriggered();
  bool isDiagSsrTriggered();
  void UpdateRxEventTag(RxThreadEventType rx_event) {
#ifdef DUMP_RINGBUF_LOG
    snoop_buff_.UpdateRxEventTag(rx_event);
#endif
  };
  void UpdateRxTimeStamp();
  void ResetSsrTriggeredFlag();
  bool SetSsrTriggeredFlag();
  void SetRecoveryStartTime();
  bool GetSsrTime(struct timeval* time);
  void UnlockRingbufferMutex();

  void SaveLastStackHciCommand(char * cmd) {
#ifdef DUMP_RINGBUF_LOG
    snoop_buff_.SaveLastStackHciCommand(cmd);
#endif
  };

  void ResetCrashReasons();
  void SetPrimaryCrashReason(PrimaryReasonCode);
  void SetSecondaryCrashReason(SecondaryReasonCode);
  char *get_reset_reason_str(SecondaryReasonCode);
  void DecodeHwErrorEvt(uint8_t *buff);
  uint8_t GetClientStatus(ProtocolType);
  void SetClientStatus(bool , ProtocolType);
  bool IsPreviousItrCrashed();
  char *GetSecondaryCrashReason();
  SecondaryReasonCode GetSecondaryCrashReasonCode();
  PrimaryReasonCode GetPrimaryReason();

  void Init(HciTransport *hci_transport);

  void GetStateFileName(char*);

  inline short int GetIbsLogLevel() {return ibs_log_level_;};
  inline short int GetRxLogLevel() {return rx_log_level_;};
  inline short int GetTxLogLevel() {return tx_log_level_;};
  inline short int GetGenLogLevel() {return gen_log_level_;};
  void ResetForceSsrTriggeredFlag();
  void CollectDumps(bool is_uart_ipc_enabled, bool is_state_log_enabled);
  void ResetCleanupflag();
  void SetInternalCmdTimeout();
  bool DiagInitOnGoing();
#ifdef DIAG_ENABLED
  bool GetDiagInitStatus();
#endif
  void DeleteCurrentDumpedFiles();
  int ReadSsrLevel();
  void CheckAndAddToDelayList(struct timeval* tv_end);
  void CheckDelayListAndSetCrashReason();
  bool CrashCmdNeeded();
  void SetCurrentactivityStartTime(struct timeval tv_start, SecondaryReasonCode activity_code,
                                    std::string activity_info);
  bool CheckActivityCodeForCrashCmd(SecondaryReasonCode activity_code);
  void UpdateNvmPatchIdealDelay(int size);
  void UpdateRamPatchIdealDelay(int size, bool fw_optimization);
  void UpdateElfPatchIdealDelay(int size, bool fw_optimization);
  void UpdateXmemRamPatchIdealDelay(int size, bool fw_optimization);
  void UpdateXmemNvmPatchIdealDelay(int size);
  void UpdateCheBasedIdealDelay();
  int PropertyGet(const char* property_name, char* value, const char* default_value);
  int PropertySet(const char* property_name, const char* value);
  void AddDelayListInfo();
  void UpdateIdealDelay(SecondaryReasonCode activity_code, int ideal_delay);
  void SetUartErrCode(SecondaryReasonCode uart_err);
  SecondaryReasonCode GetUartErrCode();
  char* GetPrimaryReasonString(PrimaryReasonCode code);
  void SetInitFailureReason(SecondaryReasonCode reason);
  SecondaryReasonCode GetInitFailureReason();
  void ResetRxStats();
  void inc_rx_stats_counter();
  struct monitor_rx *GetRxStats();
  void StartRxStatsMonitor();
  void SetasFpissue();
  bool isThisFpissue();
  static std::recursive_mutex monitor_rx_stats;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
