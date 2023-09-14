/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * Copyright (c) 2014, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *            notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in the
 *            documentation and/or other materials provided with the distribution.
 *        * Neither the name of The Linux Foundation nor
 *            the names of its contributors may be used to endorse or promote
 *            products derived from this software without specific prior written
 *            permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FM_IOCTL_INTERFACE_H__
#define __FM_IOCTL_INTERFACE_H__

#include <linux/videodev2.h>
#include <sys/stat.h>

typedef  unsigned int UINT;
typedef  unsigned long ULINT;
const int IOCTL_SUCC = 0;
const int FM_SUCCESS = 0;
const int FM_FAILURE = -1;
#define TUNE_MULT  16
const UINT CAL_DATA_SIZE = 23;
const char *const CALIB_DATA_NAME = "/data/vendor/fm/Riva_fm_cal";
#define RADIO_DEVICE_PATH "/dev/radio0"
#define CAL_DATA_BUF 10
#define V4L2_BUF_LENGTH 128
#define STD_BUF_SIZE  256
#define V4L2_CID_AUDIO_MUTE    (V4L2_CID_BASE+9)
#define EBADF    9 /* Bad file number */

enum AUDIO_MODE
{
     MONO,
     STEREO,
};

//V4L2 CONTROLS FOR FM DRIVER
enum FM_V4L2_PRV_CONTROLS
{
    V4L2_CID_PRV_BASE = 0x8000000,
    V4L2_CID_PRIVATE_FM_SRCHMODE = (0x08000000 + 1),
    V4L2_CID_PRIVATE_FM_SCANDWELL,
    V4L2_CID_PRIVATE_FM_SRCHON,
    V4L2_CID_PRIVATE_FM_STATE,
    V4L2_CID_PRIVATE_FM_TRANSMIT_MODE,
    V4L2_CID_PRIVATE_FM_RDSGROUP_MASK,
    V4L2_CID_PRIVATE_FM_REGION,
    V4L2_CID_PRIVATE_FM_SIGNAL_TH,
    V4L2_CID_PRIVATE_FM_SRCH_PTY,
    V4L2_CID_PRIVATE_FM_SRCH_PI,
    V4L2_CID_PRIVATE_FM_SRCH_CNT,
    V4L2_CID_PRIVATE_FM_EMPHASIS,
    V4L2_CID_PRIVATE_FM_RDS_STD,
    V4L2_CID_PRIVATE_FM_SPACING,
    V4L2_CID_PRIVATE_FM_RDSON,
    V4L2_CID_PRIVATE_FM_RDSGROUP_PROC,
    V4L2_CID_PRIVATE_FM_LP_MODE,
    V4L2_CID_PRIVATE_FM_ANTENNA,
    V4L2_CID_PRIVATE_FM_RDSD_BUF,
    V4L2_CID_PRIVATE_FM_PSALL,  /*0x8000014*/

    /*v4l2 Tx controls*/
    V4L2_CID_PRIVATE_FM_TX_SETPSREPEATCOUNT,
    V4L2_CID_PRIVATE_FM_STOP_RDS_TX_PS_NAME,
    V4L2_CID_PRIVATE_FM_STOP_RDS_TX_RT,
    V4L2_CID_PRIVATE_FM_IOVERC,
    V4L2_CID_PRIVATE_FM_INTDET,
    V4L2_CID_PRIVATE_FM_MPX_DCC,
    V4L2_CID_PRIVATE_FM_AF_JUMP,
    V4L2_CID_PRIVATE_FM_RSSI_DELTA,
    V4L2_CID_PRIVATE_FM_HLSI, /*0x800001d*/

    /*Diagnostic commands*/
    V4L2_CID_PRIVATE_FM_SOFT_MUTE,
    V4L2_CID_PRIVATE_FM_RIVA_ACCS_ADDR,
    V4L2_CID_PRIVATE_FM_RIVA_ACCS_LEN,
    V4L2_CID_PRIVATE_FM_RIVA_PEEK,
    V4L2_CID_PRIVATE_FM_RIVA_POKE,
    V4L2_CID_PRIVATE_FM_SSBI_ACCS_ADDR,
    V4L2_CID_PRIVATE_FM_SSBI_PEEK,
    V4L2_CID_PRIVATE_FM_SSBI_POKE,
    V4L2_CID_PRIVATE_FM_TX_TONE,
    V4L2_CID_PRIVATE_FM_RDS_GRP_COUNTERS,
    V4L2_CID_PRIVATE_FM_SET_NOTCH_FILTER, /* 0x8000028 */
    V4L2_CID_PRIVATE_FM_SET_AUDIO_PATH, /* TAVARUA specific command */
    V4L2_CID_PRIVATE_FM_DO_CALIBRATION,
    V4L2_CID_PRIVATE_FM_SRCH_ALGORITHM, /* TAVARUA specific command */
    V4L2_CID_PRIVATE_FM_GET_SINR,
    V4L2_CID_PRIVATE_INTF_LOW_THRESHOLD,
    V4L2_CID_PRIVATE_INTF_HIGH_THRESHOLD,
    V4L2_CID_PRIVATE_SINR_THRESHOLD,
    V4L2_CID_PRIVATE_SINR_SAMPLES,
    V4L2_CID_PRIVATE_SPUR_FREQ,
    V4L2_CID_PRIVATE_SPUR_FREQ_RMSSI,
    V4L2_CID_PRIVATE_SPUR_SELECTION,
    V4L2_CID_PRIVATE_UPDATE_SPUR_TABLE,
    V4L2_CID_PRIVATE_VALID_CHANNEL,
    V4L2_CID_PRIVATE_AF_RMSSI_TH,
    V4L2_CID_PRIVATE_AF_RMSSI_SAMPLES,
    V4L2_CID_PRIVATE_GOOD_CH_RMSSI_TH,
    V4L2_CID_PRIVATE_SRCHALGOTYPE,
    V4L2_CID_PRIVATE_CF0TH12,
    V4L2_CID_PRIVATE_SINRFIRSTSTAGE,
    V4L2_CID_PRIVATE_RMSSIFIRSTSTAGE,
    V4L2_CID_PRIVATE_RXREPEATCOUNT,
    V4L2_CID_PRIVATE_FM_RSSI_TH,
    V4L2_CID_PRIVATE_FM_AF_JUMP_RSSI_TH,
    V4L2_CID_PRIVATE_BLEND_SINRHI,
    V4L2_CID_PRIVATE_BLEND_RMSSIHI,

    /*using private CIDs under userclass*/
    V4L2_CID_PRIVATE_FM_READ_DEFAULT = 0x00980928,
    V4L2_CID_PRIVATE_FM_WRITE_DEFAULT,
    V4L2_CID_PRIVATE_FM_SET_CALIBRATION,
    V4L2_CID_PRIVATE_FM_SET_SPURTABLE = 0x0098092D,
    V4L2_CID_PRIVATE_FM_GET_SPUR_TBL    = 0x0098092E,

    V4L2_CID_PRV_ENABLE_SLIMBUS = 0x00980940,
};

class FmIoctlsInterface
{
    private:
        static char const * const LOGTAG;
    public:
        static int start_fm_patch_dl(UINT fd);
        static int close_fm_patch_dl(void);
        static int get_cur_freq(UINT fd, long &freq);
        static int set_freq(UINT fd, ULINT freq);
        static int set_control(UINT fd, UINT id, int val);
        static int set_calibration(UINT fd);
        static int get_control(UINT fd, UINT id, long &val);
        static int start_search(UINT fd, UINT dir);
        static int set_band(UINT fd, ULINT low, ULINT high);
        static int get_upperband_limit(UINT fd, ULINT &freq);
        static int get_lowerband_limit(UINT fd, ULINT &freq);
        static int set_audio_mode(UINT fd, enum AUDIO_MODE mode);
        static int get_buffer(UINT fd, char *buff, UINT len, UINT index);
        static int get_rmssi(UINT fd, long &rmssi);
        static int set_ext_control(UINT fd, struct v4l2_ext_controls *v4l2_ctls);
        static int do_riva_calibration();
};

//char const *FmIoctlsInterface::LOGTAG = "FmIoctlsInterface";

#endif //__FM_IOCTL_INTERFACE_H__
