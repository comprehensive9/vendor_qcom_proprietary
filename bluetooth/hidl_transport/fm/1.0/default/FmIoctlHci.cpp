/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "FmIoctlHci.h"

#define LOG_TAG "vendor.qti.hardware.fm@1.0-FmHci"
char SocName[SOC_NAME_LENGTH];

class FmIoctlDeathRecipient : public hidl_death_recipient {
    public:
        FmIoctlDeathRecipient(const sp<IFmHci> hci) : mHci(hci) {}

        virtual void serviceDied(
            uint64_t /*cookie*/,
            const wp<::android::hidl::base::V1_0::IBase>& /*who*/) {
                ALOGE("FmIoctlDeathRecipient::serviceDied - Fm service died");
                has_died_ = true;
                mHci->close();
            }
        sp<IFmHci> mHci;
        bool getHasDied() const { return has_died_; }
        void setHasDied(bool has_died) { has_died_ = has_died; }

    private:
        bool has_died_;
};

FmIoctlHal::FmIoctlHal(char SocProp[]) {
    death_recipient_ = new FmIoctlDeathRecipient(this);
    strlcpy(SocName, SocProp, sizeof(SocName));
}

::android::sp<IFmHciCallbacks> FmIoctlHal :: event_cb = nullptr;
struct fm_hal_t * FmIoctlHal :: fmhal = nullptr;

static inline void release(int fd)
{
    close(fd);
}

static inline bool IsSocPronto()
{
    return (strncmp(SocName, "pronto",strlen("pronto")) == 0)? true : false;
}

void FmIoctlHal :: sendHciEvent(struct fm_event_header_t *event_hdr_, int len)
{
    HciPacket data;

    if (event_cb != nullptr) {
        data.setToExternal((uint8_t *)event_hdr_, len);
        auto hidl_status = event_cb->hciEventReceived(
                (hidl_vec<unsigned char> )data);
        if (!hidl_status.isOk()) {
            ALOGE(" Unable to call hciEventReceived()");
        }

    } else {
        ALOGD("%s: fmHci is NULL", __func__);
    }
}

void FmIoctlHal :: commandCompleteEvent(uint16_t opcode, int status)
{
    struct fm_event_header_t *event_hdr;

    event_hdr = (struct fm_event_header_t *) malloc(DEFAULT_CMD_LEN);
    if (event_hdr == nullptr) {
        ALOGE("event_hdr allocation failed");
        return;
    }

    memset(event_hdr, 0, DEFAULT_CMD_LEN);

    event_hdr->evt_code = HCI_EV_CMD_COMPLETE;
    event_hdr->evt_len = 0x04;
    event_hdr->params[0] = 0x01;
    event_hdr->params[1] = opcode;
    event_hdr->params[2] = opcode >>8;
    //commad status byte
    event_hdr->params[3] = status;

    sendHciEvent(event_hdr, DEFAULT_CMD_LEN);
    free(event_hdr);
}

void FmIoctlHal :: commandStatusEvent(uint16_t opcode, int status)
{
    struct fm_event_header_t *cmdhdr;
    cmdhdr = (struct fm_event_header_t *) malloc(DEFAULT_CMD_LEN);
    if (cmdhdr == nullptr) {
        ALOGE("hdr allocation failed");
        return;
    }

    memset(cmdhdr, 0, DEFAULT_CMD_LEN);

    cmdhdr->evt_code = HCI_EV_CMD_STATUS;
    cmdhdr->evt_len = 0x04;
    cmdhdr->params[0] = status;
    cmdhdr->params[1] = 0x01;
    cmdhdr->params[2] = opcode;
    cmdhdr->params[3] = opcode >>8;

    sendHciEvent(cmdhdr, DEFAULT_CMD_LEN);
    free(cmdhdr);
}

int FmIoctlHal :: fmRdsGroupProcess()
{
    int ret;
    uint16_t rds_grps_proc;
    memcpy((void *)&rds_grps_proc, &fmhal->cmd_hdr->params, sizeof(uint16_t));
    uint8_t rds_grps = rds_grps_proc;

    if(rds_grps_proc == 0xff) {
        ALOGD("%s: lowpower mode disable ", __func__);
        ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_FM_LP_MODE, 0);
        if(ret == FM_FAILURE) {
            ALOGE("%s:%s low power mode disable failed", LOG_TAG, __func__);
        }
    } else if (rds_grps == 0x00 || rds_grps == AF_JUMP_ENABLE) {
        int af_jump_enable;
        if((rds_grps & AF_JUMP_ENABLE) == AF_JUMP_ENABLE)
            af_jump_enable = 1;
        else
            af_jump_enable = 0;

        ret = FmIoctlsInterface :: set_control(
                    fmhal->fd, V4L2_CID_PRIVATE_FM_AF_JUMP, af_jump_enable);
        if(ret == FM_FAILURE) {
            ALOGE("%s:%s AF Jump enable failed", LOG_TAG, __func__);
        }
        ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_FM_LP_MODE, 1);
        if(ret == FM_FAILURE) {
            ALOGE("%s:%s low power mode enable failed", LOG_TAG, __func__);
        }
    }

    ret = FmIoctlsInterface :: set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_RDSGROUP_PROC, rds_grps);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s RDS Group proc failed", LOG_TAG, __func__);
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: fmSetAntenna()
{
    int ret;
    if(fmhal->cmd_hdr->params[0] == 0)
    {
        ret = FmIoctlsInterface :: set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_ANTENNA, 0);
        if(ret == FM_FAILURE) {
            ALOGE("%s:%s reset antena failed", LOG_TAG, __func__);
        }
    } else {
        ret = FmIoctlsInterface :: set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_ANTENNA, 1);
        if(ret == FM_FAILURE) {
            ALOGE("%s:%s set antena failed", LOG_TAG, __func__);
        }
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: FmSetConfiguration()
{
    int ret;
    struct hci_fm_recv_conf_req recv_conf;
    memcpy((void*)&recv_conf, &fmhal->cmd_hdr->params,
                sizeof(struct hci_fm_recv_conf_req));

    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_EMPHASIS, recv_conf.emphasis);
    if(ret == FM_FAILURE) {
        ALOGE("set emphasis failed");
        goto END;
    }

    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_SPACING,
                    recv_conf.ch_spacing);
    if(ret == FM_FAILURE) {
        ALOGE("set spacing failed");
        goto END;
    }

    ret = FmIoctlsInterface :: set_control(fmhal->fd, V4L2_CID_PRIVATE_FM_RDS_STD,
                    recv_conf.rds_std);
    if(ret == FM_FAILURE) {
        ALOGE("set RDS STD failed");
        goto END;

    }

    if ((recv_conf.band_low_limit > 0) &&
            (recv_conf.band_high_limit > 0)) {
        ret  = FmIoctlsInterface :: set_band(fmhal->fd,
        recv_conf.band_low_limit, recv_conf.band_high_limit);
        if (ret < 0) {
            ALOGE("set band failed, low: %d, high: %d\n",
                   recv_conf.band_low_limit, recv_conf.band_high_limit);
        }
    }

END:
    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: FmSetMonoStereo()
{
    int ret;

    int val = fmhal->cmd_hdr->params[0] ? 0:1;
    ALOGD("Mono/Stereo command VAL %d",val);
    ret = FmIoctlsInterface :: set_audio_mode(
            fmhal->fd, (enum AUDIO_MODE)val);
    /*update the credit to fm client*/
    commandCompleteEvent(FM_SET_STEREO_MODE_REQ, FM_SUCCESS);
    return ret;
}

int FmIoctlHal :: fmMuteModeReq()
{
    int ret;
    struct hci_fm_mute_mode_req mute;

    memcpy((void*)&mute, &fmhal->cmd_hdr->params,
               sizeof(struct hci_fm_mute_mode_req));

    ret = FmIoctlsInterface :: set_control(fmhal->fd,
        V4L2_CID_PRIVATE_FM_SOFT_MUTE, mute.soft_mute);
    if(ret == FM_FAILURE) {
        ALOGE("%s failed to set soft mute mode",__func__);
        goto END;
    }

    ret = FmIoctlsInterface :: set_control(fmhal->fd,
        V4L2_CID_AUDIO_MUTE, mute.hard_mute);
    if(ret == FM_FAILURE) {
        ALOGE("%s failed to set hard mute mode",__func__);

    }

END:
    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}
int FmIoctlHal :: FmTurnOn()
{
    int ret;
    ret =FmIoctlsInterface :: set_control(fmhal->fd, V4L2_CID_PRIVATE_FM_STATE, FM_ON);
    if(ret == FM_SUCCESS) {
        /* start the reader thread to read event from driver */
        fmhal->reader_thread = std::thread(fmReaderThread, fmhal->fd);
        /*calibration for pronto soc*/
        if(IsSocPronto()) {
            ret = FmIoctlsInterface :: set_calibration(fmhal->fd);
            if (ret == FM_FAILURE) {
                ALOGE("%s: set calibration failed\n", __func__);
            }
        }
    }

    return ret;
}

inline int FmIoctlHal :: FmTurnOff()
{
    return FmIoctlsInterface :: set_control(fmhal->fd, V4L2_CID_PRIVATE_FM_STATE, FM_OFF);
}

int FmIoctlHal :: FmTune()
{
    int ret = FM_SUCCESS;
    int freq;
    memcpy(&freq, &fmhal->cmd_hdr->params, sizeof(int));
    ALOGD("Tune command set freq: %d\n", freq);
    if (freq > 0) {
        ret = FmIoctlsInterface :: set_freq(fmhal->fd, freq);
        if (ret < 0) {
            ALOGE("set freq failed, freq: %d\n", freq);
            ret = FM_FAILURE;
        }
    } else {
        ALOGE("set freq failed because freq is negative,\
                freq: %d", freq);
        ret = FM_FAILURE;
    }

    commandStatusEvent(FM_TUNE_STATION_REQ, ret);
    return ret;
}

int FmIoctlHal :: fmSearchRdsStations()
{
    int ret = FM_SUCCESS;
    struct hci_fm_search_rds_station_req rds_search;

    memcpy((void*)&rds_search, &fmhal->cmd_hdr->params,
                sizeof(struct hci_fm_search_rds_station_req));

    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SRCHMODE,
                    rds_search.srch_station.srch_mode);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s search mode failed", LOG_TAG, __func__);
        goto END;
    }
    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SCANDWELL,
                    rds_search.srch_station.scan_time);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s scan time failed", LOG_TAG, __func__);
        goto END;
    }
    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SRCH_PTY,
                    rds_search.srch_pty);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s search pty failed", LOG_TAG, __func__);
        goto END;
    }

    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SRCH_PI, rds_search.srch_pi);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s search pi failed", LOG_TAG, __func__);
    }
    if (rds_search.srch_station.srch_dir >= 0) {
        int dir;
        if (rds_search.srch_station.srch_dir)
            dir = SRCH_DIR_DOWN;
        else
            dir = SRCH_DIR_UP;
        ret = FmIoctlsInterface :: start_search(fmhal->fd, dir);
        if (ret == FM_FAILURE) {
            ALOGE("%s: VIDIOC_S_HW_FREQ_SEEK failed, dir: %d\n",
                    __func__, rds_search.srch_station.srch_dir);
        }
    }

END:
    commandStatusEvent(FM_SEARCH_RDS_STATIONS, ret);
    if(ret == FM_FAILURE) {
        fmSeekCompleteEvent(fmhal->fd);
    }
    return ret;
}

int FmIoctlHal :: FmSearchStations()
{
    int ret;
    struct hci_fm_search_station_req srch;

    memcpy((void*)&srch, &fmhal->cmd_hdr->params,
                sizeof(struct hci_fm_search_station_req));

    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SRCHMODE, srch.srch_mode);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s search mode failed", LOG_TAG, __func__);
        goto END;
    }

    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SCANDWELL, srch.scan_time);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s scan time failed", LOG_TAG, __func__);
        goto END;
    }

    if (srch.srch_dir >= 0) {
        int dir;
        if (srch.srch_dir)
            dir = SRCH_DIR_DOWN;
        else
            dir = SRCH_DIR_UP;
        ALOGD("startSearchNative: Issuing the VIDIOC_S_HW_FREQ_SEEK %d", dir);
        ret = FmIoctlsInterface :: start_search(fmhal->fd, dir);
        if (ret == FM_FAILURE) {
            ALOGE("%s: VIDIOC_S_HW_FREQ_SEEK failed, dir: %d\n",
                    __func__, srch.srch_dir);
        }
    }

END:
    commandStatusEvent(FM_SEARCH_STATIONS, ret);
    if(ret == FM_FAILURE) {
        fmSeekCompleteEvent(fmhal->fd);
    }
    return ret;
}

int FmIoctlHal :: FmCancelSearch()
{
    int ret;
    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SRCHON, 0);
    if (ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: FmRdsGrpMaskReq()
{
    int ret;
    struct hci_fm_rds_grp_req rds_grp;
    memcpy((void *)&rds_grp, &fmhal->cmd_hdr->params,
                    sizeof(struct hci_fm_rds_grp_req));
    ret =FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_RDSGROUP_MASK, rds_grp.rds_grp_enable_mask);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: FmSetSignalThreshold()
{
    int ret;
    ret =FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_SIGNAL_TH, fmhal->cmd_hdr->params[0]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: fmGetRdsGrpCountersReq()
{
    int ret;
    int val;

    memcpy((void *)&val, &fmhal->cmd_hdr->params,
                    sizeof(int));
    ret =FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_RDS_GRP_COUNTERS, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: fmBandMayAffectFmRf( int current_band )
{
    int retval = 0;

    switch(current_band)    {
        case 4:
        case 6:
        case 8:
        case 14:
        case 15:
        case 16:
        case 17:
        case 47:
        case 48:
        case 80:
        case 81:
        case 82:
        case 83:
        case 88:
        case 90:
        case 110:
        case 111:
        case 112:
        case 113:
        case 114:
        case 115:
        case 116:
        case 117:
        case 118:
        case 119:
        case 120:
        case 121:
        case 122:
        case 123:
        case 126:
        case 128:
        case 129:
        case 130:
        case 135:
        case 136:
        case 137:
        case 138:
        case 139:
        case 140:
        case 141:
        case 142:/*In all these case, band may cross 1GHZ*/
            retval = 1;
            break;
        default:
            retval = 0;
        }
        return retval;
}

int FmIoctlHal ::fmSetDesiredNotchFilter(int wcm_mode)
{
    qmi_client_error_type qmi_client_err;
    qmi_client_type my_qmi_client_handle;
    nas_get_rf_band_info_resp_msg_v01 resp_msg;
    qmi_idl_service_object_type nas_service;
    qmi_service_info qmi_svc_info;
    qmi_client_os_params qmi_at_os_params;
    int i = 0;
    memset(&resp_msg, 0, sizeof(resp_msg));

    if(wcm_mode == WA_DISABLE) {
        ALOGD("Resetting the notchfilters");
        return RESET_NOTCH_FILTER;
    }

    nas_service = nas_get_service_object_v01();
    if(nas_service == NULL) {
        ALOGE("%s: Not able to get nas service handle\n", __func__);
        return FM_FAILURE;
    }

    qmi_client_err = qmi_client_init_instance(nas_service, QMI_CLIENT_INSTANCE_ANY,
                        NULL, NULL, &qmi_at_os_params, QMI_CLIENT_INIT_TIMEOUT,
                        &my_qmi_client_handle);
    if(qmi_client_err != QMI_NO_ERR){
        ALOGE("qmi_client_init_instance failed error %d" ,qmi_client_err);
        if(my_qmi_client_handle != NULL) {
            free(my_qmi_client_handle);
        }
        return FM_FAILURE;
    }

    qmi_client_err = qmi_client_send_msg_sync(my_qmi_client_handle,
    QMI_NAS_GET_RF_BAND_INFO_RESP_MSG_V01, NULL, 0, &resp_msg,
                    sizeof(resp_msg), QMI_CLIENT_INIT_TIMEOUT);

    if(qmi_client_err != QMI_NO_ERR){
        ALOGE("qmi_client_send_msg_sync failed error %d" ,qmi_client_err);
        return FM_FAILURE;
    }

    //Got the band info
    if(resp_msg.rf_band_info_list_len > 1 ||
            resp_msg.rf_band_info_list_len == 0) {
        ALOGE("%s: band info in not valid", __func__);
        return FM_FAILURE;
    }

    for ( i = 0; i < (int)resp_msg.rf_band_info_list_len; i++ )
    {
        ALOGD("%d: radio_if: %d  active_band: %d\n",
          i, resp_msg.rf_band_info_list[i].radio_if,
            resp_msg.rf_band_info_list[i].active_band);
    }

    qmi_client_err = qmi_client_release(my_qmi_client_handle);
    if (qmi_client_err != QMI_NO_ERR){
        ALOGE("%s: Error while releasing qmi_client: %d",
            __func__, qmi_client_err);
    }

    if(fmBandMayAffectFmRf(
                resp_msg.rf_band_info_list[resp_msg.rf_band_info_list_len-1].
                active_band)) {
        ALOGD("%s: It is High band", __func__);
        return HIGH_BAND;
    } else    {
        ALOGD("%s: It is Low band", __func__);
        return LOW_BAND;
    }
}

int FmIoctlHal :: fmSetNotchFilterReq()
{
    int ret = FM_FAILURE;
    int notchval;
    int band;

    if(IsSocPronto()) {
        memcpy((void *)&notchval, &fmhal->cmd_hdr->params,
                        sizeof(int));
        band = fmSetDesiredNotchFilter(notchval);
        if(band != FM_FAILURE) {
            ret =FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_FM_SET_NOTCH_FILTER, band);
            if(ret == FM_FAILURE) {
                ALOGE("%s:%s failed", LOG_TAG, __func__);
            }
        } else {
            ALOGE("%s fmSetDesiredNotchFilter failed", __func__);
        }
    }
    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: fmPeekData()
{
    int ret;

    ret =FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_RIVA_PEEK, RIVA_PEEK_OPCODE);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: FmSsbiPokeReg()
{
    int ret;
    struct hci_fm_ssbi_req ssbi;
    memcpy((void *)&ssbi, &fmhal->cmd_hdr->params,
                    sizeof(struct hci_fm_ssbi_req));
    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_SSBI_ACCS_ADDR, ssbi.start_addr);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        goto END;
    }

    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_SSBI_POKE, ssbi.data);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
    }

END:
    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: FmSsbiPeekReg()
{
    int ret;
    struct hci_fm_ssbi_peek ssbi;
    memcpy((void *)&ssbi, &fmhal->cmd_hdr->params,
                    sizeof(struct hci_fm_ssbi_peek));
    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_SSBI_PEEK, ssbi.start_address);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
    }

    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: FmGetChDetTh()
{
    int ret;
    int packet_len;
    struct hci_fm_ch_det_threshold ch_det_threshold;
    struct fm_event_header_t *hdr;
    long val = 0;

    memset(&ch_det_threshold, 0, sizeof(struct hci_fm_ch_det_threshold));

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_INTF_HIGH_THRESHOLD, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s INTF_HIGH_THRESHOLD failed", LOG_TAG, __func__);
    } else {
        ch_det_threshold.high_th = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_INTF_LOW_THRESHOLD, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s INTF_LOW_THRESHOLD failed", LOG_TAG, __func__);
    } else {
        ch_det_threshold.low_th = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_SINR_THRESHOLD, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s SINR_THRESHOLD failed", LOG_TAG, __func__);
    } else {
        ch_det_threshold.sinr = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_SINR_SAMPLES, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s SINR_SAMPLES failed", LOG_TAG, __func__);
    } else {
        ch_det_threshold.sinr_samples = (char)val;
    }

    packet_len = DEFAULT_CMD_LEN + sizeof(struct hci_fm_ch_det_threshold);

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s:%s hdr allocation failed", LOG_TAG, __func__);
        return FM_FAILURE;
    }
    memset(hdr, 0, packet_len);
    hdr->evt_code = HCI_EV_CMD_COMPLETE;
    hdr->evt_len = 4 + sizeof(struct hci_fm_ch_det_threshold);
    hdr->params[0] = 0x01;
    hdr->params[1] = 0x18;
    hdr->params[2] = 0x4c;
    hdr->params[3] = 0x00;
    memcpy((void*)&(hdr->params[4]), &ch_det_threshold,
            sizeof(struct hci_fm_ch_det_threshold));

    sendHciEvent(hdr, packet_len);
    free(hdr);

    return ret;
}

int FmIoctlHal :: FmSetChDetTh()
{
    int ret = FM_SUCCESS;
    struct hci_fm_ch_det_threshold ch_det_threshold;
    memcpy((void *)&ch_det_threshold, &fmhal->cmd_hdr->params,
                    sizeof(struct hci_fm_ch_det_threshold));

    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_INTF_HIGH_THRESHOLD, ch_det_threshold.high_th);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s INTF_HIGH_THRESHOLD failed", LOG_TAG, __func__);
        goto END;
    }

    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_INTF_LOW_THRESHOLD, ch_det_threshold.low_th);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s INTF_LOW_THRESHOLD failed", LOG_TAG, __func__);
        goto END;
    }

    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_SINR_THRESHOLD, ch_det_threshold.sinr);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s SINR_THRESHOLD failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
        fmhal->fd, V4L2_CID_PRIVATE_SINR_SAMPLES, ch_det_threshold.sinr_samples);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s SINR_SAMPLES failed", LOG_TAG, __func__);
    }

END:
    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: fmDefaultDataWrite()
{
    int ret = FM_SUCCESS;
    struct hci_fm_def_data_wr_req data_wrt;
    memcpy((void *)&data_wrt, &fmhal->cmd_hdr->params,
                    sizeof(struct hci_fm_def_data_wr_req));

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_SINRFIRSTSTAGE,
                    data_wrt.data[SINRFIRSTSTAGE_OFFSET]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s SINRFIRSTSTAGE failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_RMSSIFIRSTSTAGE,
                    data_wrt.data[RMSSIFIRSTSTAGE_OFFSET]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s RMSSIFIRSTSTAGE failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_FM_SRCH_ALGORITHM,
                        data_wrt.data[SRCH_ALGO_TYPE_OFFSET]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s SRCH_ALGORITHM failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_AF_RMSSI_TH,
                        data_wrt.data[AF_RMSSI_TH_OFFSET]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s AF_RMSSI_TH failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_GOOD_CH_RMSSI_TH,
                        data_wrt.data[GD_CH_RMSSI_TH_OFFSET]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s GOOD_CH_RMSSI_TH failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_AF_RMSSI_SAMPLES,
                        data_wrt.data[AF_RMSSI_SAMPLES_OFFSET]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s AF_RMSSI_SAMPLES failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_RXREPEATCOUNT,
                        data_wrt.data[RX_REPEATE_BYTE_OFFSET]);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s RXREPEATCOUNT failed", LOG_TAG, __func__);
    }

END:
    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}

int FmIoctlHal :: fmDefaultDataRead()
{
    int packet_len, ret;
    struct fm_event_header_t *hdr;
    long val;
    char data[DEFAULT_DATA_READ_SIZE] = {0};

    ALOGD("%s: Sending Data Read event", __func__);

    /* Packet len fills the HCI event type, params for opcode and credit,
       event data with its length and status byte */
    packet_len = DEFAULT_EVE_SIZE + DEFAULT_PARAM_LEN
               + DEFAULT_DATA_READ_SIZE + STATUS_BYTE + DATA_LENGTH_PARAM;
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s:%s hdr allocation failed", LOG_TAG, __func__);
        return FM_FAILURE;
    }
    memset(hdr, 0, packet_len);

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_SINRFIRSTSTAGE, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s get SINRFIRSTSTAGE failed", LOG_TAG, __func__);
    } else {
        data[SINRFIRSTSTAGE_OFFSET] = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_RMSSIFIRSTSTAGE, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s get RMSSIFIRSTSTAGE failed", LOG_TAG, __func__);
    } else {
        data[RMSSIFIRSTSTAGE_OFFSET] = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_AF_RMSSI_TH, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s get AF_RMSSI_TH failed", LOG_TAG, __func__);
    } else {
        data[AF_RMSSI_TH_OFFSET] = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_AF_RMSSI_SAMPLES, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s get AF_RMSSI_SAMPLES failed", LOG_TAG, __func__);
    } else {
        data[AF_RMSSI_SAMPLES_OFFSET] = (char)val;
    }

    hdr->evt_code = HCI_EV_CMD_COMPLETE;
    hdr->evt_len =  (DEFAULT_PARAM_LEN + DEFAULT_DATA_READ_SIZE +
                        STATUS_BYTE + DATA_LENGTH_PARAM);
    hdr->params[0] = 0x01; //credit
    hdr->params[1] = 0x02; //Opcode Value Param 1 and 2
    hdr->params[2] = 0x54;
    hdr->params[3] = 0x00; //status Byte
    hdr->params[4] = DEFAULT_DATA_READ_SIZE;
    memcpy((void*)&(hdr->params[5]), &data,
            DEFAULT_DATA_READ_SIZE);

    sendHciEvent(hdr, packet_len);
    free(hdr);
    return FM_SUCCESS;
}

int FmIoctlHal :: fmSetBlendTbl()
{
    int ret;
    struct hci_fm_blend_table blend_tbl;
    memcpy((void *)&blend_tbl, &fmhal->cmd_hdr->params,
                    sizeof(struct hci_fm_blend_table));

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_BLEND_SINRHI,
                        blend_tbl.BlendSinrHi);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s BLEND_SINRHI failed", LOG_TAG, __func__);
            goto END;
    }

    ret = FmIoctlsInterface :: set_control(
                fmhal->fd, V4L2_CID_PRIVATE_BLEND_RMSSIHI,
                        blend_tbl.BlendRmssiHi);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s BLEND_RMSSIHI failed", LOG_TAG, __func__);
    }

END:
    commandCompleteEvent(fmhal->cmd_hdr->opcode, ret);
    return ret;
}
int FmIoctlHal :: fmGetBlendTbl()
{
    int ret;
    int packet_len;
    long val;
    struct fm_event_header_t *hdr;
    struct hci_fm_blend_table fm_blend_tbl;

    memset(&fm_blend_tbl, 0, sizeof(struct hci_fm_blend_table));
    /* Packet len fills the HCI event type, params for opcode and credit,
       event data and status byte */
    packet_len = DEFAULT_EVE_SIZE + DEFAULT_PARAM_LEN
               + sizeof(struct hci_fm_blend_table) + STATUS_BYTE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s:%s hdr allocation failed", LOG_TAG, __func__);
        return FM_FAILURE;
    }
    memset(hdr, 0, packet_len);

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_BLEND_SINRHI, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s BLEND_SINRHI failed", LOG_TAG, __func__);
    } else {
        fm_blend_tbl.BlendSinrHi = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
    fmhal->fd, V4L2_CID_PRIVATE_BLEND_RMSSIHI, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s BLEND_RMSSIHI failed", LOG_TAG, __func__);
    } else {
        fm_blend_tbl.BlendRmssiHi = (char)val;
    }

    hdr->evt_code  = HCI_EV_CMD_COMPLETE;
    hdr->evt_len   = DEFAULT_PARAM_LEN + STATUS_BYTE
                          + sizeof(struct hci_fm_blend_table);
    hdr->params[0] = 0x01; // Credit
    hdr->params[1] = 0x1C; //Opcode value param 1 & 2
    hdr->params[2] = 0x4C;
    hdr->params[3] = 0x00; //Status Byte
    memcpy((void*)&(hdr->params[4]), &fm_blend_tbl,
            sizeof(struct hci_fm_blend_table));

    sendHciEvent(hdr, packet_len);
    free(hdr);
    return FM_SUCCESS;
}
int FmIoctlHal :: fmGetStationParamReq()
{
    int ret = FM_SUCCESS;
    int packet_len;
    long val;
    struct fm_event_header_t *hdr;
    struct hci_fm_station_rsp fm_st_rsp;

    memset(&fm_st_rsp, 0, sizeof(struct hci_fm_station_rsp));

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_GET_SINR, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s get sinr failed", LOG_TAG, __func__);
    } else {
        fm_st_rsp.sinr = (char)val;
    }

    ret = FmIoctlsInterface :: get_rmssi(fmhal->fd, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s get_rmssi failed", LOG_TAG, __func__);
    } else {
        fm_st_rsp.rssi = (char)val;
    }

    packet_len = DEFAULT_CMD_LEN + sizeof(struct hci_fm_station_rsp);
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s:%s hdr allocation failed", LOG_TAG, __func__);
        return FM_FAILURE;
    }

    memset(hdr, 0, packet_len);
    hdr->evt_code = HCI_EV_CMD_COMPLETE;
    hdr->evt_len = 4 + sizeof(struct hci_fm_station_rsp);
    hdr->params[0] = 0x01;
    hdr->params[1] = 0x0A;
    hdr->params[2] = 0x4c;
    hdr->params[3] = 0x00;

    memcpy((void*)&(hdr->params[4]), &fm_st_rsp,
            sizeof(struct hci_fm_station_rsp));

    sendHciEvent(hdr, packet_len);
    free(hdr);

    return ret;
}

int FmIoctlHal :: FmStationDbgParam()
{
    int ret = FM_SUCCESS;
    int packet_len;
    long val;
    struct fm_event_header_t *hdr;
    struct hci_fm_dbg_param_rsp dbg_param_rsp;

    memset(&dbg_param_rsp, 0, sizeof(struct hci_fm_dbg_param_rsp));

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_IOVERC, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s FM_IOVERC failed", LOG_TAG, __func__);
    } else {
        dbg_param_rsp.io_verc = (char)val;
    }

    ret = FmIoctlsInterface :: get_control(
        fmhal->fd, V4L2_CID_PRIVATE_FM_INTDET, val);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s FM_INTDET failed", LOG_TAG, __func__);
    } else {
        dbg_param_rsp.in_det_out = (char)val;
    }

    packet_len = DEFAULT_CMD_LEN + sizeof(struct hci_fm_dbg_param_rsp);
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s:%s hdr allocation failed", LOG_TAG, __func__);
        return FM_FAILURE;
    }

    memset(hdr, 0, packet_len);
    hdr->evt_code = HCI_EV_CMD_COMPLETE;
    hdr->evt_len = 4 + sizeof(struct hci_fm_dbg_param_rsp);
    hdr->params[0] = 0x01;
    hdr->params[1] = 0x07;
    hdr->params[2] = 0xfc;
    hdr->params[3] = 0x00;

    memcpy((void*)&(hdr->params[4]), &dbg_param_rsp,
            sizeof(struct hci_fm_dbg_param_rsp));

    sendHciEvent(hdr, packet_len);
    free(hdr);

    return ret;
}

int FmIoctlHal :: fmSearchList()
{
    int ret = FM_SUCCESS;
    struct hci_fm_search_station_list_req list;

    memcpy((void*)&list, &fmhal->cmd_hdr->params,
                sizeof(struct hci_fm_search_station_list_req));

    ret = FmIoctlsInterface ::set_control(
            fmhal->fd, V4L2_CID_PRIVATE_FM_SRCHMODE, list.srch_list_mode);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s mode failed", LOG_TAG, __func__);
    }

    if (list.srch_list_dir >= 0) {
        int dir;
        if (list.srch_list_dir)
            dir = SRCH_DIR_DOWN;
        else
            dir = SRCH_DIR_UP;
        ALOGD("startSearchNative: Issuing the VIDIOC_S_HW_FREQ_SEEK %d", dir);
        ret = FmIoctlsInterface :: start_search(fmhal->fd, dir);
        if (ret == FM_FAILURE) {
            ALOGE("%s: VIDIOC_S_HW_FREQ_SEEK failed, dir: %d\n",
                    __func__, list.srch_list_dir);
        }
    }

    commandStatusEvent(FM_SEARCH_STATIONS_LIST, ret);
    return ret;
}

void FmIoctlHal :: DoFmIoctl()
{
    int ret = FM_SUCCESS;

    if(fmhal->cmd_hdr == nullptr || fmhal->fd < 0) {
        ALOGE("%s: No more FM CMDs are available ", __func__ );
        return ;
    }

    switch(fmhal->cmd_hdr->opcode) {
        // low power mode/ AF Jump command
        case FM_RDS_GRP_PROCESS:
            ret = fmRdsGroupProcess();
            break;

        // set ntenna command
        case FM_SET_ANTENNA:
            ret = fmSetAntenna();
            break;

        // FM configuration commad
        case FM_SET_RECV_CONF_REQ:
            ret = FmSetConfiguration();
            break;

        // cherokee specific enable/disable slimbus command
        case FM_ENABLE_SLIMBUS:
        case FM_GET_RECV_CONF_REQ:
        case FM_GET_SIGNAL_THRESHOLD:
        //intentional fall throguh
        // event mask command
        case FM_SET_EVENT_MASK:
            commandCompleteEvent(fmhal->cmd_hdr->opcode, FM_SUCCESS);
            break;

        // enablle/disable soft or hard mute command
        case FM_SET_MUTE_MODE_REQ:
            ret = fmMuteModeReq();
            break;

        //set Mono/Stereo command
        case FM_SET_STEREO_MODE_REQ:
            ret = FmSetMonoStereo();
            break;

        // FM Turn off command
        case FM_DISABLE_RECV_REQ:
            ret = FmTurnOff();
            break;

        // FM Turn on command
        case FM_ENABLE_RECV_REQ:
            ret = FmTurnOn();
            break;

        // FM Tune command
        case FM_TUNE_STATION_REQ:
            ret = FmTune();
            break;

        //FM Search Command
        case FM_SEARCH_STATIONS:
            ret = FmSearchStations();
            break;

        // FM RDS based search
        case FM_SEARCH_RDS_STATIONS:
            ret = fmSearchRdsStations();
            break;

        // FM Cancel Search
        case FM_CANCEL_SEARCH:
            ret = FmCancelSearch();
            break;

        // rds grp mask req
        case FM_RDS_GRP:
            ret = FmRdsGrpMaskReq();
            break;

        // set signal threshold
        case FM_SET_SIGNAL_THRESHOLD:
            ret = FmSetSignalThreshold();
            break;

        case FM_READ_GRP_COUNTERS:
            ret = fmGetRdsGrpCountersReq();
            break;

        case FM_EN_NOTCH_CTRL:
            ret = fmSetNotchFilterReq();
            break;

        case FM_PEEK_DATA:
            ret = fmPeekData();
            break;

        case FM_SSBI_POKE_REG:
            ret = FmSsbiPokeReg();
            break;

        case FM_SSBI_PEEK_REG:
            ret = FmSsbiPeekReg();
            break;

        case FM_GET_CH_DET_THRESHOLD:
            ret = FmGetChDetTh();
            break;

        case FM_SET_CH_DET_THRESHOLD:
            ret = FmSetChDetTh();
            break;

        case FM_DEFAULT_DATA_WRITE:
            ret = fmDefaultDataWrite();
            break;

        case FM_DEFAULT_DATA_READ:
            ret = fmDefaultDataRead();
            break;

        case FM_SET_BLND_TBL:
            ret = fmSetBlendTbl();
            break;

        case FM_GET_BLND_TBL:
            ret = fmGetBlendTbl();
            break;

        case FM_GET_STATION_PARAM_REQ:
            ret = fmGetStationParamReq();
            break;

        case FM_STATION_DBG_PARAM:
            ret = FmStationDbgParam();
            break;

        case FM_SEARCH_STATIONS_LIST:
            ret = fmSearchList();
            break;

        default:
        {
            ALOGE("default case opcode 0x%x",fmhal->cmd_hdr->opcode);
            commandCompleteEvent(fmhal->cmd_hdr->opcode, FM_SUCCESS);
            break;
        }
    }

    if(ret != FM_SUCCESS) {
        ALOGE("ioctl opcode 0x%x failed ret %d", fmhal->cmd_hdr->opcode, ret);
    }
    free(fmhal->cmd_hdr);
    fmhal->cmd_hdr = nullptr;

}

int FmIoctlHal :: fmDeviceNodeInit()
{
    v4l2_capability cap;
    fmhal->fd = open(RADIO_DEVICE_PATH, O_RDONLY, O_NONBLOCK);
    ALOGD("%s:fmDoIoctlThread: Opened %d", __func__, fmhal->fd);

    if(fmhal->fd < 0) {
        return FM_FAILURE;
    }

    int ret = ioctl(fmhal->fd, VIDIOC_QUERYCAP, &cap);
    ALOGD("%s:VIDIOC_QUERYCAP returns :%d: version: %d \n", __func__,
                    ret , cap.version );

    /*soc download for pronto based target*/
    if(IsSocPronto()) {
        ALOGD("%s: inserting the radio transport module", __func__);
        int fd = open(MODULE_PARAM_CALL, (O_RDWR | O_NOCTTY));
        if(fd < 0) {
            ALOGE("fmsmd_set device node open failed");
            return FM_FAILURE;
        }

        char paramVal = '1';
        int ret = write(fd, &paramVal, 1);
        if(ret < 0) {
            ALOGE("%s: write error: %d (%s)", __func__, ret, strerror(ret));
            release(fd);
            return ret;
        }
        /*add the 50ms delay so that iris driver can be register with SMD*/
        usleep(50 * 1000); /* 50 ms delay */
        release(fd);
        ALOGD("%s: inserted the radio transport module", __func__);

        FILE *cal_fp = NULL;
        unsigned char cal_data[CAL_DATA_SIZE] = {0};
        cal_fp = fopen(CALIB_DATA_NAME,"r" );
        if (!cal_fp ||
                fread(cal_data, 1, CAL_DATA_SIZE, cal_fp) < CAL_DATA_SIZE ) {
            if (cal_fp)
                fclose(cal_fp);
            /* get_cal_data writes the calibration data into the file*/
            return FmIoctlsInterface ::do_riva_calibration();

        } else {
            fclose(cal_fp);
        }
    }
    return FM_SUCCESS;
}

int FmIoctlHal :: initializationCompleteCallBack(int status)
{
    if(status == FM_SUCCESS) {
        fmhal->state = FM_HAL_ENABLED;
        auto hidl_status = event_cb->initializationComplete(
                Status::SUCCESS);
        if (!hidl_status.isOk()) {
            ALOGE("%s:Unable to call initializationComplete()", __func__);
        }
        return FM_SUCCESS;
    } else {
        auto hidl_status = event_cb->initializationComplete(
                    Status::INITIALIZATION_ERROR);
        if (!hidl_status.isOk()) {
            ALOGE("%s:Unable to call initializationComplete(ERR)", __func__);
        }
        return FM_FAILURE;
    }
}

void FmIoctlHal :: fmDoIoctlThread()
{
    ALOGD("%s:Starting", __func__);
    int ret = FM_SUCCESS;

    std::unique_lock<std::mutex> Lock(fmhal->tx_cond_mtx);

    ret = fmDeviceNodeInit();
    if(ret == FM_FAILURE) {
        ALOGE("%s: failed to intialize radio device node", __func__);
    }

    if(initializationCompleteCallBack(ret) == FM_FAILURE) {
        return;
    }

    while (fmhal->state == FM_HAL_ENABLED) {
        fmhal->tx_cond.wait(Lock);
        DoFmIoctl();
    }

    ALOGE("Exiting fmDoIoctlThread Worker thread");
}

void FmIoctlHal :: fmEnableEvent()
{
    int packet_len;
    struct fm_event_header_t *hdr;

    ALOGD("%s: Got READY_EVENT", __func__);

    packet_len = DEFAULT_EVE_SIZE + DEFAULT_PARAM_LEN
                     + sizeof(struct hci_fm_conf_rsp);
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s:%s hdr allocation failed", LOG_TAG, __func__);
        return;
    }
    memset(hdr, 0, packet_len);
    hdr->evt_code = HCI_EV_CMD_COMPLETE;
    hdr->evt_len =  DEFAULT_PARAM_LEN + sizeof(struct hci_fm_conf_rsp);
    hdr->params[0] = 0x01;
    hdr->params[1] = 0x01;
    hdr->params[2] = 0x4c;

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmTuneEvent(int fd)
{
    long freq;
    int packet_len;
    struct fm_event_header_t *hdr;
    struct hci_ev_tune_status fm_tune_evt;

    memset(&fm_tune_evt, 0, sizeof(struct hci_ev_tune_status));

    ALOGD("%s: Got TUNE_EVENT", __func__);

    //Send HCI_EV_TUNE_STATUS EVENT for opcode: 0x5401
    // reading the frequency from driver
    int err = FmIoctlsInterface :: get_cur_freq(fd, freq);
    if(err < 0) {
       ALOGE("get freq failed\n");
    }

    packet_len = DEFAULT_EVE_SIZE + sizeof(struct hci_ev_tune_status);
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s: hdr allocation failed", __func__);
        return;
    } else {
        fm_tune_evt.station_freq = freq;
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_TUNE_STATUS;
        hdr->evt_len = sizeof(struct hci_ev_tune_status);
        memcpy((void*)&(hdr->params[0]), (const void*)&fm_tune_evt, sizeof(struct hci_ev_tune_status));
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmStereoMonoEvent(bool stereo)
{
    int packet_len;
    struct fm_event_header_t *hdr;

    packet_len = DEFAULT_PARAM_LEN;
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s: hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_STEREO_STATUS;
        hdr->evt_len = 0x01;
        if (stereo) {
            hdr->params[0] = 0x01;
        } else {
            hdr->params[0] = 0x00;
        }
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmSeekCompleteEvent(int fd)
{
    long freq;
    int packet_len;
    struct fm_event_header_t *hdr;
    struct hci_ev_tune_status fm_seek_evt;

    memset(&fm_seek_evt, 0, sizeof(struct hci_ev_tune_status));

    ALOGD("%s: Got SEEK_COMPLETE_EVENT", __func__);

    int err = FmIoctlsInterface :: get_cur_freq(fd, freq);
    if(err < 0) {
       ALOGE("get freq failed\n");
       return;
    }
    packet_len = DEFAULT_EVE_SIZE + sizeof(struct hci_ev_tune_status);
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s: hdr allocation failed", __func__);
        return;
    } else {
        fm_seek_evt.station_freq = freq;
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_SEARCH_COMPLETE;
        hdr->evt_len = sizeof(struct hci_ev_tune_status);
        memcpy((void*)&(hdr->params[0]), (const void*)&fm_seek_evt, sizeof(struct hci_ev_tune_status));
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmScanNextEvent()
{
    int packet_len;
    struct fm_event_header_t *hdr;

    ALOGD("%s: Got SCAN_NEXT_EVENT", __func__);
    packet_len = DEFAULT_EVE_SIZE;
    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_SEARCH_PROGRESS;
        hdr->evt_len = 0x00;
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmRawRdsEvent(int fd)
{
    int packet_len;
    int ret = FM_SUCCESS;
    struct fm_event_header_t *hdr;

    ALOGD("%s: Got RAW_RDS_EVENT", __func__);
    char raw_data[STD_BUF_SIZE];
    ret = FmIoctlsInterface :: get_buffer(fd, raw_data, STD_BUF_SIZE, FM_BUF_RAW_RDS);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        return;
    }
    packet_len = 2 + STD_BUF_SIZE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_RDS_RX_DATA;
        hdr->evt_len = BUF_SIZE;
        memcpy((void*)&(hdr->params), (const void *)&raw_data, STD_BUF_SIZE);
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmRtEvent(int fd)
{
    int packet_len;
    int ret = FM_SUCCESS;
    struct fm_event_header_t *hdr;

    ALOGD("%s: Got RT_EVENT", __func__);
    char radio_txt[STD_BUF_SIZE];
    ret = FmIoctlsInterface :: get_buffer(fd, radio_txt, STD_BUF_SIZE, FM_BUF_RT_RDS);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        return;
    }
    packet_len = 3 + STD_BUF_SIZE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_DRIVER_RDS_EVENT;
        hdr->evt_len = BUF_SIZE;
        hdr->params[0] = HCI_EV_RADIO_TEXT;
        memcpy((void*)&(hdr->params[1]), (const void *)&radio_txt, STD_BUF_SIZE);
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmPsEvent(int fd)
{
    int packet_len;
    int ret = FM_SUCCESS;
    struct fm_event_header_t *hdr;
    char buff[STD_BUF_SIZE];

    ALOGD("%s: Got PS_EVENT", __func__);

    ret = FmIoctlsInterface :: get_buffer(fd, buff, STD_BUF_SIZE, FM_BUF_PS_RDS);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        return;
    }

    packet_len = 3 + STD_BUF_SIZE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_DRIVER_RDS_EVENT;
        hdr->evt_len = BUF_SIZE;
        hdr->params[0] = HCI_EV_PROGRAM_SERVICE;
        memcpy((void*)&(hdr->params[1]), (const void *)&buff, STD_BUF_SIZE);
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmNewAFList(int fd)
{
    int packet_len;
    int ret = FM_SUCCESS;
    struct fm_event_header_t *hdr;

    ALOGD("%s: Got NEW_AF_LIST", __func__);
    char aflistbuff[STD_BUF_SIZE] = {0};
    ret = FmIoctlsInterface :: get_buffer(fd, aflistbuff,
            STD_BUF_SIZE, FM_BUF_AF_LIST);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        return;
    }

    packet_len = 3 + STD_BUF_SIZE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_DRIVER_RDS_EVENT;
        hdr->evt_len = BUF_SIZE;
        hdr->params[0] = HCI_EV_FM_AF_LIST;
        memcpy((void*)&(hdr->params[1]), (const void *)&aflistbuff, STD_BUF_SIZE);
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmDisableEvent(int fd)
{
    ALOGD("%s: Got RADIO_DISABLED", __func__);
    commandCompleteEvent(FM_DISABLE_RECV_REQ, FM_SUCCESS);
}

void FmIoctlHal :: fmRTPlusEvent(int fd)
{
    int packet_len;
    int ret = FM_SUCCESS;
    struct fm_event_header_t *hdr;

    ALOGD("%s: got RT plus event", __func__);
    char rtplusbuff[STD_BUF_SIZE] = {0};
    ret = FmIoctlsInterface :: get_buffer(fd, rtplusbuff,
                STD_BUF_SIZE, FM_BUF_RT_PLUS);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        return;
    }

    packet_len = 3 + STD_BUF_SIZE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_DRIVER_RDS_EVENT;
        hdr->evt_len = BUF_SIZE;
        hdr->params[0] = HCI_EV_RADIO_TEXT_PLUS_TAG;
        memcpy((void*)&(hdr->params), (const void *)&rtplusbuff, STD_BUF_SIZE);
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmNewErtEvent(int fd)
{
    int packet_len;
    int ret = FM_SUCCESS;
    struct fm_event_header_t *hdr;

    ALOGD("%s: got eRT event", __func__);

    char ertbuff[STD_BUF_SIZE] = {0};
    ret = FmIoctlsInterface :: get_buffer(fd, ertbuff,
                STD_BUF_SIZE, FM_BUF_ERT);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        return;
    }

    packet_len = 3 + STD_BUF_SIZE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_DRIVER_RDS_EVENT;
        hdr->evt_len = BUF_SIZE;
        hdr->params[0] = HCI_EV_E_RADIO_TEXT;
        memcpy((void*)&(hdr->params), (const void *)&ertbuff, STD_BUF_SIZE);
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);

}

void FmIoctlHal :: fmSrchStListComplEvent(int fd)
{
    int packet_len;
    struct hci_ev_srch_list_compl tempev ;
    struct fm_event_header_t *hdr;

    ALOGD("%s: Got NEW_SRCH_LIST", __func__);
    char stlistbuff[STD_BUF_SIZE] = {0};
    int ret = FmIoctlsInterface :: get_buffer(fd, stlistbuff,
                STD_BUF_SIZE, FM_BUF_SRCH_LIST);
    if(ret == FM_FAILURE) {
        ALOGE("%s:%s failed", LOG_TAG, __func__);
        return;
    }

    memcpy((void*)&(tempev), (const void *)&stlistbuff,
                    sizeof(struct hci_ev_srch_list_compl));

    packet_len = 2 + STD_BUF_SIZE;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_SEARCH_LIST_COMPLETE;
        hdr->evt_len = BUF_SIZE;
        hdr->params[STN_NUM_OFFSET] = tempev.num_stations_found;
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmHwErrorEvent()
{
    int packet_len;
    struct fm_event_header_t *hdr;

    ALOGD("%s: Got ERROR_EVENT", __func__);

    packet_len = 2;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_HW_ERR_EVENT;
        hdr->evt_len = 0;
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmRdsStatusEvent(uint8_t status)
{
    int packet_len;
    struct fm_event_header_t *hdr;

    packet_len = 3;

    hdr = (struct fm_event_header_t *) malloc(packet_len);
    if (hdr == nullptr) {
        ALOGE("%s hdr allocation failed", __func__);
        return;
    } else {
        memset(hdr, 0, packet_len);
        hdr->evt_code = HCI_EV_RDS_LOCK_STATUS;
        hdr->evt_len = 1;
        hdr->params[0] = status;
    }

    sendHciEvent(hdr, packet_len);
    free(hdr);
}

void FmIoctlHal :: fmReaderThread(int fd)
{
    char buff[STD_BUF_SIZE] = {0};
    bool isRadioDriverDisabled = false;

    ALOGD("%s:Starting fmReaderThread:", __func__);

    while(isRadioDriverDisabled == false) {
        memset(&buff, 0, sizeof(buff));
        int eventCount = FmIoctlsInterface :: get_buffer(
                fd, buff, STD_BUF_SIZE, 1);
        if(eventCount == FM_FAILURE) {
            ALOGE("%s get_buffer returned failure", __func__);
            break;
        }

        for (int index = 0; index < eventCount; index++ ) {
            switch(buff[index]){
                case FM_EVT_RADIO_READY:
                    fmEnableEvent();
                    break;

                case FM_EVT_TUNE_SUCC:
                    fmTuneEvent(fd);
                    break;

                case FM_EVT_SEEK_COMPLETE:
                    fmSeekCompleteEvent(fd);
                    break;

                case FM_EVT_SCAN_NEXT:
                    fmScanNextEvent();
                    break;

                case FM_EVT_NEW_RAW_RDS:
                    fmRawRdsEvent(fd);
                    break;

                case FM_EVT_NEW_RT_RDS:
                    fmRtEvent(fd);
                    break;

                case FM_EVT_NEW_PS_RDS:
                    fmPsEvent(fd);
                    break;

                case FM_EVT_ERROR:
                    fmHwErrorEvent();
                    isRadioDriverDisabled = true;
                    break;

                case FM_EVT_BELOW_TH:
                    //Got BELOW_TH_EVENT
                    break;

                case FM_EVT_ABOVE_TH:
                    //Got ABOVE_TH_EVENT
                    break;

                case FM_EVT_STEREO:
                    ALOGD("%s: Got STEREO_EVENT", __func__);
                    fmStereoMonoEvent(true);
                    break;
                case FM_EVT_MONO:
                    ALOGD("%s: Got MONO_EVENT", __func__);
                    fmStereoMonoEvent(false);
                    break;
                case FM_EVT_RDS_AVAIL:
                    ALOGD("%s: Got RDS_AVAL_EVENT", __func__);
                    fmRdsStatusEvent(RDS_AVAIL);
                    break;

                case FM_EVT_RDS_NOT_AVAIL:
                    ALOGD("%s: Got RDS_NOT_AVAL_EVENT", __func__);
                    fmRdsStatusEvent(RDS_NOT_AVAIL);
                    break;

                case FM_EVT_NEW_SRCH_LIST:
                    fmSrchStListComplEvent(fd);
                    break;

                case FM_EVT_NEW_AF_LIST:
                    fmNewAFList(fd);
                    break;

                case FM_EVT_RADIO_DISABLED:
                    fmDisableEvent(fd);
                    isRadioDriverDisabled = true;
                    break;

                case FM_EVT_NEW_ODA:
                    /*ODA event posting to FM app is not required */
                    break;

                case FM_EVT_NEW_RT_PLUS:
                    fmRTPlusEvent(fd);
                    break;

                case FM_EVT_NEW_ERT:
                    fmNewErtEvent(fd);
                    break;

                case FM_EVT_SPUR_TBL:
                    ALOGD("%s: got EVT_SPUR_TBL event", __func__);
                    commandCompleteEvent(FM_SET_SPUR_TABLE, FM_SUCCESS);
                    break;

                default:
                    ALOGD("%s: Unknown event", __func__);
                    break;
            }
        }
    }

    ALOGE("Exiting fmReaderThread Worker thread");
}

Return<void> FmIoctlHal :: initialize(const sp<IFmHciCallbacks>& cb)
{
    ALOGD("%s:", __func__);
    if(cb == nullptr) {
        ALOGE("%s Callback is Null", __func__);
        return Void();
    }
    event_cb = cb;

    fmhal = (struct fm_hal_t *)malloc(sizeof(struct fm_hal_t));
    if(fmhal == nullptr) {
        ALOGE("%s fmhal allocation failed", __func__);
        return Void();
    }
    memset(fmhal, 0, sizeof(struct fm_hal_t));
    fmhal->state = FM_HAL_DISABLED;

    fmhal->doioctl_thread = std::thread(fmDoIoctlThread);

    death_recipient_->setHasDied(false);
    event_cb->linkToDeath(death_recipient_, 0);

    unlink_cb_ = [this](sp<FmIoctlDeathRecipient>& death_recipient) {
        if (death_recipient->getHasDied()){
            ALOGD("Skipping unlink call, service died.");
        } else {
            event_cb->unlinkToDeath(death_recipient);
            ALOGD(" unlink  to death recipient ");
        }
    };

    return Void();
}

Return<void> FmIoctlHal :: sendHciCommand(const hidl_vec<uint8_t>& command)
{
    ALOGD("%s:", __func__);

    if((fmhal == nullptr) || ((fmhal != nullptr) &&
                 (fmhal->state == FM_HAL_DISABLED))) {
        ALOGE("%s fmhal is not initialized, Igonre the command", __func__);
        return Void();
    }

    fmhal->tx_cond_mtx.lock();
    if(fmhal->cmd_hdr) {
        free(fmhal->cmd_hdr);
        fmhal->cmd_hdr = nullptr;
    }

    fmhal->cmd_hdr = (struct fm_command_header_t *) malloc(command.size());
    if (fmhal->cmd_hdr != nullptr) {
        memcpy(fmhal->cmd_hdr, command.data(), command.size());
    } else {
        ALOGE("%s hdr allocation failed", __func__);
        fmhal->tx_cond_mtx.unlock();
        return Void();
    }

    fmhal->tx_cond.notify_one();
    fmhal->tx_cond_mtx.unlock();

    return Void();
}

Return<void> FmIoctlHal :: close()
{
    ALOGD("%s:", __func__);

    /* check for fmhal pointer here, This case can happen when death_recipient
     * called during close execution */
    if(fmhal == nullptr)
    {
        ALOGE("fmhal in null");
        return Void();
    }

    if (death_recipient_->getHasDied())
    {
        /* FM App would have died and hidl death recipient
         * called close turn off fm.
         */
        fmhal->tx_cond_mtx.lock();
        fmhal->cmd_hdr = (struct fm_command_header_t *) malloc(sizeof(struct fm_command_header_t));
        if (fmhal->cmd_hdr != nullptr) {
            fmhal->cmd_hdr->opcode = FM_DISABLE_RECV_REQ;
        } else {
            ALOGE("%s hdr allocation failed", __func__);
        }
        fmhal->tx_cond.notify_one();
        fmhal->tx_cond_mtx.unlock();
    }

    // wait for fmReaderThread to exit
    if (fmhal->reader_thread.joinable()) {
        fmhal->reader_thread.join();
    }

    //release the fd
    release(fmhal->fd);
    fmhal->state = FM_HAL_DISABLED;

    // notify fmDoIoctlThread and wait for fmDoIoctlThread to exit
    fmhal->tx_cond_mtx.lock();
    fmhal->tx_cond.notify_one();
    fmhal->tx_cond_mtx.unlock();
    fmhal->doioctl_thread.join();

    unlink_cb_(death_recipient_);
    free(fmhal);
    fmhal = nullptr;
    event_cb = nullptr;

    ALOGD("close done for ioctl based hal");
    return Void();
}
