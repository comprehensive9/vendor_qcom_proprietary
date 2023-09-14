/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vendor/qti/hardware/fm/1.0/IFmHci.h>
#include "FmIoctlsInterface.h"
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <mutex>              // std::mutex, std::unique_lock
#include <hidl/Status.h>
#include <utils/Log.h>
#include <unistd.h>
#include <thread>
#include "network_access_service_v01.h"
#include <errno.h>
#include "qmi_client.h"
#include "qmi_client_instance_defs.h"
#include "qmi_cci_target_ext.h"

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using vendor::qti::hardware::fm::V1_0::IFmHci;
using vendor::qti::hardware::fm::V1_0::IFmHciCallbacks;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::wp;
using vendor::qti::hardware::fm::V1_0::HciPacket;
using vendor::qti::hardware::fm::V1_0::Status;
using ::android::hardware::hidl_death_recipient;

#define BUF_SIZE  255
#define AF_JUMP_ENABLE  (1 << 4)
#define MAX_RT_LENGTH   (64)
#define RDS_OFFSET 5
#define MAX_PS_LEN 8
#define RDS_PS_DATA_OFFSET 8
#define RDS_PS_LENGTH_OFFSET 7
#define RDS_PTYPE 2
#define RDS_PID_LOWER 1
#define RDS_PID_HIGHER 0
/* Search direction */
#define SRCH_DIR_UP      (1)
#define SRCH_DIR_DOWN    (0)
#define RIVA_PEEK_OPCODE 0x0D
#define RIVA_POKE_OPCODE 0x0C
#define DEFAULT_DATA_SIZE 249
#define DEFAULT_DATA_READ_SIZE 6
#define STATUS_BYTE 1
#define DATA_LENGTH_PARAM 1
#define AF_RMSSI_TH_OFFSET  1
#define GD_CH_RMSSI_TH_OFFSET   0x03
#define AF_RMSSI_SAMPLES_OFFSET 2
#define RX_REPEATE_BYTE_OFFSET 0x05
#define AF_ALGO_OFFSET      0
#define MAX_BLEND_INDEX 0x31
#define STN_NUM_OFFSET     0x01
#define DEFAULT_CMD_LEN 6
#define SRCH_ALGO_TYPE_OFFSET  0x02
#define SINRFIRSTSTAGE_OFFSET  0x03
#define RMSSIFIRSTSTAGE_OFFSET 0x04
#define RDS_AVAIL 1
#define RDS_NOT_AVAIL 0
#define FM_SET_SPUR_TABLE           0x0008
#define HIGH_BAND 2
#define LOW_BAND  1
#define RESET_NOTCH_FILTER 0
#define WA_ENABLE 1
#define WA_DISABLE 0
#define SOC_NAME_LENGTH 16
#define MODULE_PARAM_CALL "/sys/module/radio_iris_transport/parameters/fmsmd_set"
#define DEFAULT_PARAM_LEN 3
#define DEFAULT_EVE_SIZE 2
#define QMI_CLIENT_INIT_TIMEOUT 4

enum state_t {
    FM_OFF,
    FM_ON,
};

/* Search options */
enum search_t {
    SEEK,
    SCAN,
    SCAN_FOR_STRONG,
    SCAN_FOR_WEAK,
    RDS_SEEK_PTY,
    RDS_SCAN_PTY,
    RDS_SEEK_PI,
    RDS_AF_JUMP,
};

enum fm_buf_t {
FM_BUF_SRCH_LIST,
FM_BUF_EVENTS,
FM_BUF_RT_RDS,
FM_BUF_PS_RDS,
FM_BUF_RAW_RDS,
FM_BUF_AF_LIST,
FM_BUF_PEEK,
FM_BUF_SSBI_PEEK,
FM_BUF_RDS_CNTRS,
FM_BUF_RD_DEFAULT,
FM_BUF_CAL_DATA,
FM_BUF_RT_PLUS,
FM_BUF_ERT,
FM_BUF_SPUR,
FM_BUF_MAX,
};

struct hci_fm_mute_mode_req {
    char  hard_mute;
    char  soft_mute;
} ;

enum COMMAND_OPCODE
{
    FM_BASE_OPCODE = 0x4c00,
    FM_ENABLE_RECV_REQ,
    FM_DISABLE_RECV_REQ,
    FM_GET_RECV_CONF_REQ,
    FM_SET_RECV_CONF_REQ,
    FM_SET_MUTE_MODE_REQ,
    FM_SET_STEREO_MODE_REQ,
    FM_SET_ANTENNA,
    FM_SET_SIGNAL_THRESHOLD,
    FM_GET_SIGNAL_THRESHOLD,
    FM_GET_STATION_PARAM_REQ,
    FM_GET_PROGRAM_SERVICE_REQ,
    FM_GET_RADIO_TEXT_REQ,
    FM_GET_AF_LIST_REQ,
    FM_SEARCH_STATIONS,
    FM_SEARCH_RDS_STATIONS,
    FM_SEARCH_STATIONS_LIST,
    FM_CANCEL_SEARCH,
    FM_RDS_GRP,
    FM_RDS_GRP_PROCESS,
    FM_EN_WAN_AVD_CTRL,
    FM_EN_NOTCH_CTRL,
    FM_SET_EVENT_MASK,
    FM_SET_CH_DET_THRESHOLD,
    FM_GET_CH_DET_THRESHOLD,

    FM_SET_BLND_TBL =           0x4c1B,
    FM_GET_BLND_TBL =           0x4c1C,
    FM_LOW_PASS_FILTER_CTRL =   0x4c1F,

    FM_ENABLE_SLIMBUS = 0xfc0e,
    FM_PEEK_DATA = 0xfc02,
    FM_POKE_DATA,
    FM_SSBI_PEEK_REG,
    FM_SSBI_POKE_REG,
    FM_STATION_DBG_PARAM = 0xfc07,

    FM_TUNE_STATION_REQ = 0x5401,
    FM_DEFAULT_DATA_READ,
    FM_DEFAULT_DATA_WRITE,

    /*HCI Status parameters commands*/
    FM_READ_GRP_COUNTERS = 0x5801,
    FM_READ_GRP_COUNTERS_EXT = 0x5802,
};
/* ----- HCI Command request ----- */
struct hci_fm_recv_conf_req {
    char  emphasis;
    char  ch_spacing;
    char  rds_std;
    char  hlsi;
    int   band_low_limit;
    int   band_high_limit;
} ;

struct hci_fm_conf_rsp {
     char    status;
     struct hci_fm_recv_conf_req recv_conf_rsp;
};

struct hci_fm_rds_grp_req {
    int   rds_grp_enable_mask;
    int   rds_buf_size;
    char    en_rds_change_filter;
} ;

struct hci_fm_search_station_req {
    char    srch_mode;
    char    scan_time;
    char    srch_dir;
} ;

struct hci_fm_search_rds_station_req {
    struct hci_fm_search_station_req srch_station;
    char    srch_pty;
    short   srch_pi;
} ;

struct hci_fm_search_station_list_req {
    char    srch_list_mode;
    char    srch_list_dir;
    int   srch_list_max;
    char    srch_pty;
} ;

struct fm_command_header_t {
    uint16_t opcode;
    uint8_t  len;
    uint8_t  params[];
}__attribute__((packed));

struct fm_event_header_t {
    uint8_t evt_code;
    uint8_t evt_len;
    uint8_t params[];
}__attribute__((packed));

struct hci_fm_ssbi_req {
    short   start_addr;
    char    data;
} ;

struct hci_fm_ssbi_peek {
    short start_address;
} ;

struct hci_fm_ch_det_threshold {
    char sinr;
    char sinr_samples;
    char low_th;
    char high_th;
} ;

struct hci_fm_blend_table {
    char BlendType;
    char BlendRampRateUp;
    char BlendDebounceNumSampleUp;
    char BlendDebounceIdxUp;
    char BlendSinrIdxSkipStep;
    char BlendSinrHi;
    char BlendRmssiHi;
    char BlendIndexHi;
    char BlendIndex[MAX_BLEND_INDEX];
} ;

struct hci_fm_def_data_wr_req {
    char    mode;
    char    length;
    char   data[DEFAULT_DATA_SIZE];
} ;

struct hci_ev_tune_status {
    char    sub_event;
    int     station_freq;
    char    serv_avble;
    char    rssi;
    char    stereo_prg;
    char    rds_sync_status;
    char    mute_mode;
    char    sinr;
    char    intf_det_th;
}__attribute__((packed)) ;

struct hci_fm_station_rsp {
    int     station_freq;
    char    serv_avble;
    char    rssi;
    char    stereo_prg;
    char    rds_sync_status;
    char    mute_mode;
    char    sinr;
    char    intf_det_th;
};

struct hci_fm_dbg_param_rsp {
    char    blend;
    char    soft_mute;
    char    inf_blend;
    char    inf_soft_mute;
    char    pilot_pil;
    char    io_verc;
    char    in_det_out;
} ;

struct hci_ev_rel_freq {
    char  rel_freq_msb;
    char  rel_freq_lsb;

} ;

struct hci_ev_srch_list_compl {
    char    num_stations_found;
    struct hci_ev_rel_freq  rel_freq[20];
} ;

enum fm_evt_t {
FM_EVT_RADIO_READY,
FM_EVT_TUNE_SUCC,
FM_EVT_SEEK_COMPLETE,
FM_EVT_SCAN_NEXT,
FM_EVT_NEW_RAW_RDS,
FM_EVT_NEW_RT_RDS,
FM_EVT_NEW_PS_RDS,
FM_EVT_ERROR,
FM_EVT_BELOW_TH,
FM_EVT_ABOVE_TH,
FM_EVT_STEREO,
FM_EVT_MONO,
FM_EVT_RDS_AVAIL,
FM_EVT_RDS_NOT_AVAIL,
FM_EVT_NEW_SRCH_LIST,
FM_EVT_NEW_AF_LIST,
FM_EVT_TXRDSDAT,
FM_EVT_TXRDSDONE,
FM_EVT_RADIO_DISABLED,
FM_EVT_NEW_ODA,
FM_EVT_NEW_RT_PLUS,
FM_EVT_NEW_ERT,
FM_EVT_SPUR_TBL,
};

/*HCI events*/
#define HCI_EV_TUNE_STATUS              0x01
#define HCI_EV_RDS_LOCK_STATUS          0x02
#define HCI_EV_STEREO_STATUS            0x03
#define HCI_EV_SERVICE_AVAILABLE        0x04
#define HCI_EV_SEARCH_PROGRESS          0x05
#define HCI_EV_SEARCH_RDS_PROGRESS      0x06
#define HCI_EV_SEARCH_LIST_PROGRESS     0x07
#define HCI_EV_RDS_RX_DATA              0x08
#define HCI_EV_PROGRAM_SERVICE          0x09
#define HCI_EV_RADIO_TEXT               0x0A
#define HCI_EV_FM_AF_LIST               0x0B
#define HCI_EV_TX_RDS_GRP_AVBLE         0x0C
#define HCI_EV_TX_RDS_GRP_COMPL         0x0D
#define HCI_EV_TX_RDS_CONT_GRP_COMPL    0x0E
#define HCI_EV_CMD_COMPLETE             0x0F
#define HCI_EV_CMD_STATUS               0x10
#define HCI_EV_TUNE_COMPLETE            0x11
#define HCI_EV_SEARCH_COMPLETE          0x12
#define HCI_EV_SEARCH_RDS_COMPLETE      0x13
#define HCI_EV_SEARCH_LIST_COMPLETE     0x14

#define HCI_EV_EXT_COUNTRY_CODE         0x17
#define HCI_EV_RADIO_TEXT_PLUS_ID       0x18
#define HCI_EV_RADIO_TEXT_PLUS_TAG      0x19
#define HCI_EV_HW_ERR_EVENT             0x1A
/*HCI event opcode for fm driver RDS support*/
#define HCI_EV_DRIVER_RDS_EVENT         0x1B
#define HCI_EV_E_RADIO_TEXT             0x1C

typedef enum {
    FM_HAL_DISABLED,
    FM_HAL_DISABLING,
    FM_HAL_ENABLED,
    FM_HAL_ENABLING
} fm_power_state_t;

struct fm_hal_t {
    int fd;
    fm_power_state_t state;

    std::condition_variable tx_cond;
    std::mutex tx_cond_mtx;

    std::thread doioctl_thread;
    std::thread reader_thread;

    struct fm_command_header_t *cmd_hdr;
};

class FmIoctlDeathRecipient;

struct FmIoctlHal : public IFmHci {

private:
    static ::android::sp<IFmHciCallbacks> event_cb;
    static struct fm_hal_t *fmhal;
    ::android::sp<FmIoctlDeathRecipient> death_recipient_;
    std::function<void(sp<FmIoctlDeathRecipient>&)> unlink_cb_;
    static void sendHciEvent(struct fm_event_header_t *event_hdr_, int len);
    static void commandCompleteEvent(uint16_t opcode, int status);
    static void commandStatusEvent(uint16_t opcode, int status);
    static void DoFmIoctl();
    static void fmDoIoctlThread();
    static void fmReaderThread(int fd);
    static int fmRdsGroupProcess();
    static int fmSetAntenna();
    static int FmSetConfiguration();
    static int FmSetMonoStereo();
    static int FmTurnOn();
    static int FmTurnOff();
    static int FmTune();
    static int FmSearchStations();
    static int FmCancelSearch();
    static int FmRdsGrpMaskReq();
    static int FmSetSignalThreshold();
    static int fmGetRdsGrpCountersReq();
    static int fmSetNotchFilterReq();
    static int fmPeekData();
    static int FmSsbiPokeReg();
    static int FmSsbiPeekReg();
    static int FmGetChDetTh();
    static int FmSetChDetTh();
    static int fmDefaultDataWrite();
    static int fmDefaultDataRead();
    static int fmSetBlendTbl();
    static int fmGetBlendTbl();
    static int fmGetStationParamReq();
    static int FmStationDbgParam();
    static int fmMuteModeReq();
    static int fmSearchRdsStations();
    static int fmSearchList();
    static int fmDeviceNodeInit();
    static int initializationCompleteCallBack(int status);
    static void fmEnableEvent();
    static void fmTuneEvent(int fd);
    static void fmStereoMonoEvent(bool stereo);
    static void fmSeekCompleteEvent(int fd);
    static void fmScanNextEvent();
    static void fmRtEvent(int fd);
    static void fmPsEvent(int fd);
    static void fmNewAFList(int fd);
    static void fmDisableEvent(int fd);
    static void fmRTPlusEvent(int fd);
    static void fmSrchStListComplEvent(int fd);
    static void fmRawRdsEvent(int fd);
    static void fmHwErrorEvent();
    static void fmRdsStatusEvent(uint8_t status);
    static void fmNewErtEvent(int fd);
    static int fmBandMayAffectFmRf(int band);
    static int fmSetDesiredNotchFilter(int wcm_mode);

public:

    // Methods from ::vendor::qti::hardware::fm::V1_0::IFmHci follow.
    Return<void> initialize(const sp<IFmHciCallbacks>& callback) override;
    Return<void> sendHciCommand(const hidl_vec<uint8_t>& command) override;
    Return<void> close() override;
    FmIoctlHal(char SocProp[]);
};

