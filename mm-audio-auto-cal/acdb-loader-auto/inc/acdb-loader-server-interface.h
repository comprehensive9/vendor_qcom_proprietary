/* Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#pragma once

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <binder/TextOutput.h>

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <linux/msm_audio_calibration.h>

namespace audiocal {

using namespace android;

enum {
        ACDB_LOADER_INIT_V1 = 1,
        ACDB_LOADER_INIT_V2,
        ACDB_LOADER_INIT_V3,
        ACDB_LOADER_INIT_V4,
};

typedef enum{
    INIT_V2 = ::android::IBinder::FIRST_CALL_TRANSACTION,
    INIT_V3,
    INIT_V4,
    INIT_ACDB,
    GET_DEF_APP_TYPE,
    SEND_COM_CUSTOM_TOP,
    DEALLOCATE_ACDB,
    SEND_VOICE_CAL_V2,
    SEND_VOICE_CAL,
    RELOAD_VOICE_VOL_TABLE,
    SEND_AUDIO_CAL,
    SEND_AUDIO_CAL_V2,
    SEND_AUDIO_CAL_V3,
    SEND_AUDIO_CAL_V4,
    SEND_LISTEN_DEV_CAL,
    SEND_LISTEN_LSM_CAL,
    SEND_LISTEN_LSM_CAL_V1,
    SEND_ANC_CAL,
    SEND_ANC_DATA,
    GET_AUD_VOL_STEPS,
    SEND_GAIN_DEP_CAL,
    GET_REMOTE_ACDB_ID,
    GET_ECRX_DEVICE,
    GET_CALIBRATION,
    SET_AUDIO_CAL_V2,
    GET_AUDIO_CAL_V2,
    SEND_META_INFO,
    SEND_META_INFO_LIST,
    SET_CODEC_DATA,
    IS_INITIALIZED,
    RELOAD_ACDB_FILES,
    RELOAD_ACDB_FILES_V2
}acdb_command;

struct param_data {
        int     use_case;
        int     acdb_id;
        int     get_size;
        int     buff_size;
        int     data_size;
        void    *buff;
};

typedef struct acdb_audio_cal_cfg {
        uint32_t persist;
        uint32_t snd_dev_id;
        uint32_t dev_id;
        int32_t acdb_dev_id;
        uint32_t app_type;
        uint32_t topo_id;
        uint32_t sampling_rate;
        uint32_t cal_type;
        uint32_t module_id;
        uint16_t instance_id;
        uint16_t reserved;
        uint32_t param_id;
} acdb_audio_cal_cfg_t;

struct meta_key_list {
    struct listnode list;
    struct audio_cal_info_metainfo cal_info;
};

struct acdb_init_data_v4 {
        char                   *cvd_version;
        char                   *snd_card_name;
        struct listnode        *meta_key_list;
        bool                   *is_instance_id_supported;
};

class Iacdb_loader_server: public ::android::IInterface {
    public:
        DECLARE_META_INTERFACE(acdb_loader_server);

        virtual int acdb_loader_init_v2(char *snd_card_name, char *cvd_version, int metaInfoKey) = 0;
        virtual int acdb_loader_init_v3(char *snd_card_name, char *cvd_version, struct listnode *metaKey_list) = 0;
        virtual int acdb_loader_init_v4(void* acdb_init_data, int acdb_loader_init_version) = 0;
        virtual int acdb_loader_init_ACDB(void) = 0;
        virtual int acdb_loader_get_default_app_type(void) = 0;
        virtual int acdb_loader_send_common_custom_topology(void) = 0;
        virtual void acdb_loader_deallocate_ACDB(void) = 0;
        virtual void acdb_loader_send_voice_cal_v2(int rxacdb_id, int txacdb_id, int feature_set) = 0;
        virtual void acdb_loader_send_voice_cal(int rxacdb_id, int txacdb_id) = 0;
        virtual int acdb_loader_reload_vocvoltable(int feature_set) = 0;
        virtual void acdb_loader_send_audio_cal(int acdb_id, int capability) = 0;
        virtual void acdb_loader_send_audio_cal_v2(int acdb_id, int capability, int app_id, int sample_rate) = 0;
        virtual void acdb_loader_send_audio_cal_v3(int acdb_id, int capability, int app_id, int sample_rate, int use_case) = 0;
        virtual void acdb_loader_send_audio_cal_v4(int acdb_id, int capability, int app_id,
            int sample_rate, int use_case, int afe_sample_rate) = 0;
        virtual void acdb_loader_send_listen_device_cal(int acdb_id, int type, int app_id, int sample_rate) = 0;
        virtual int acdb_loader_send_listen_lsm_cal(int acdb_id, int app_id, int mode, int type) = 0;
        virtual int acdb_loader_send_listen_lsm_cal_v1(int acdb_id, int app_id, int mode, int type, int buff_idx) = 0;
        virtual int acdb_loader_send_anc_cal(int acdb_id) = 0;
        virtual void send_tabla_anc_data(void) = 0;
        virtual int acdb_loader_get_aud_volume_steps(void) = 0;
        virtual int acdb_loader_send_gain_dep_cal(int acdb_id, int app_id,
            int capability, int mode, int vol_index) = 0;
        virtual int acdb_loader_get_remote_acdb_id(unsigned int native_acdb_id) = 0;
        virtual int acdb_loader_get_ecrx_device(int acdb_id) = 0;
        virtual int acdb_loader_get_calibration(char *attr, int size, void *data) = 0;
        virtual int acdb_loader_set_audio_cal_v2(void *caldata, void* data,
                unsigned int datalen) = 0;
        virtual int acdb_loader_get_audio_cal_v2(void *caldata, void* data,
                unsigned int *datalen) = 0;
        virtual int send_meta_info(int metaInfoKey, int buf_idx) = 0;
        virtual int send_meta_info_list(struct listnode *key_list) = 0;
        virtual int acdb_loader_set_codec_data(void *data, char *attr) = 0;
        virtual bool acdb_loader_is_initialized(void) = 0;
        virtual int acdb_loader_reload_acdb_files(char *new_acdb_file_path, char *snd_card_name, char *cvd_version, int metaInfoKey) = 0;
        virtual int acdb_loader_reload_acdb_files_v2(char *new_acdb_file_path, char *snd_card_name , char *cvd_version, struct listnode *meta_key_list) = 0;
};
};  // namespace audiocal

