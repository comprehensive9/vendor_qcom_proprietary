/* Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#pragma once

#include <stdlib.h>
#include <utils/RefBase.h>
#include <binder/TextOutput.h>

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryDealer.h>
#include <cutils/list.h>

#include <pthread.h>
#include <linux/msm_audio_calibration.h>

#include "anc_map_api.h"
#include "vbat_map_api.h"
#include "acdb-loader-server-interface.h"

#define ACDB_LOADER_SERVICE "acdb_loader_service"

#ifdef _ANDROID_
#include <cutils/properties.h>
/* definitions for Android logging */
#include <utils/Log.h>
#include "common_log.h"
#else
#undef ALOGI
#undef ALOGE
#undef ALOGV
#undef ALOGD

#define ALOGI(...)     fprintf(stdout,__VA_ARGS__);\
                       fprintf(stdout, "\n")
#define ALOGE(...)     fprintf(stderr,__VA_ARGS__);\
                       fprintf(stderr, "\n")
#ifdef ACDB_LOADER_DEBUG
#define ALOGV(...)     fprintf(stderr,__VA_ARGS__);\
                       fprintf(stderr, "\n")
#define ALOGD(...)     fprintf(stderr,__VA_ARGS__);\
                       fprintf(stderr, "\n")
#else
#define ALOGV(...)     ((void)0)
#define ALOGD(...)     ((void)0)
#endif /* ACDB_LOADER_DEBUG */
#endif /* _ANDROID_ */

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef memscpy
#define memscpy(dst, dst_size, src, bytes_to_copy) (void) memcpy(dst, src, MIN(dst_size, bytes_to_copy))
#endif

typedef void (*acdb_loader_callback)(void* context);

typedef int  (*acdb_init_v2_t)(const char *, char *, int);
typedef int  (*acdb_init_v3_t)(const char *, char *, struct listnode *);
typedef int  (*acdb_init_v4_t)(void *, int);
typedef int  (*acdb_init_t)(void);
typedef int  (*acdb_get_default_app_type_t)(void);
typedef int  (*acdb_send_common_custom_topology_t)(void);
typedef void (*acdb_deallocate_t)(void);
typedef void (*acdb_send_voice_cal_v2_t)(int, int, int);
typedef void (*acdb_send_voice_cal_t)(int, int);
typedef int  (*acdb_reload_vocvoltable_t)(int);
typedef void (*acdb_send_audio_cal_t)(int, int);
typedef void (*acdb_send_audio_cal_v2_t)(int, int, int, int);
typedef void (*acdb_send_audio_cal_v3_t)(int, int, int, int, int);
typedef void (*acdb_send_audio_cal_v4_t)(int, int, int, int, int, int);
typedef void (*acdb_send_listen_device_cal_t)(int, int, int, int);
typedef int  (*acdb_send_listen_lsm_cal_t)(int, int, int, int);
typedef int  (*acdb_send_listen_lsm_cal_v1_t)(int, int, int, int, int);
typedef int  (*acdb_send_anc_cal_t)(int);
typedef void (*acdb_send_tabla_anc_data_t)(void);
typedef int (*acdb_get_aud_volume_steps_t)(void);
typedef int (*acdb_send_gain_dep_cal_t)(int, int, int, int, int);
typedef int (*acdb_get_remote_acdb_id_t)(unsigned int);
typedef int (*acdb_get_ecrx_device_t)(int);
typedef int (*acdb_get_calibration_t)(char *, int, void *);
typedef int (*acdb_set_audio_cal_v2_t)(void *, void*, unsigned int);
typedef int (*acdb_get_audio_cal_v2_t)(void *, void*, unsigned int *);
typedef int (*acdb_send_meta_info_t)(int, int);
typedef int (*acdb_send_meta_info_list_t)(struct listnode *);
typedef int (*acdb_set_codec_data_t)(void *, char *);
typedef bool (*acdb_is_initialized_t)(void);
typedef int (*acdb_reload_acdb_files_t)(char *, char *, char *, int);
typedef int (*acdb_reload_acdb_files_v2_t)(char *, char *, char *, struct listnode *);

typedef struct acdb_service_data {
    void                               *acdb_handle;
    acdb_init_v2_t                     acdb_init_v2;
    acdb_init_v3_t                     acdb_init_v3;
    acdb_init_v4_t                     acdb_init_v4;
    acdb_init_t                        acdb_init;
    acdb_get_default_app_type_t        acdb_get_default_app_type;
    acdb_send_common_custom_topology_t acdb_send_common_custom_topology;
    acdb_deallocate_t                  acdb_deallocate;
    acdb_send_voice_cal_v2_t           acdb_send_voice_cal_v2;
    acdb_send_voice_cal_t              acdb_send_voice_cal;
    acdb_reload_vocvoltable_t          acdb_reload_vocvoltable;
    acdb_send_audio_cal_t              acdb_send_audio_cal;
    acdb_send_audio_cal_v2_t           acdb_send_audio_cal_v2;
    acdb_send_audio_cal_v3_t           acdb_send_audio_cal_v3;
    acdb_send_audio_cal_v4_t           acdb_send_audio_cal_v4;
    acdb_send_listen_device_cal_t      acdb_send_listen_device_cal;
    acdb_send_listen_lsm_cal_t         acdb_send_listen_lsm_cal;
    acdb_send_listen_lsm_cal_v1_t      acdb_send_listen_lsm_cal_v1;
    acdb_send_anc_cal_t                acdb_send_anc_cal;
    acdb_send_tabla_anc_data_t         acdb_send_tabla_anc_data;
    acdb_get_aud_volume_steps_t        acdb_get_aud_volume_steps;
    acdb_send_gain_dep_cal_t           acdb_send_gain_dep_cal;
    acdb_get_remote_acdb_id_t          acdb_get_remote_acdb_id;
    acdb_get_ecrx_device_t             acdb_get_ecrx_device;
    acdb_get_calibration_t             acdb_get_calibration;
    acdb_set_audio_cal_v2_t            acdb_set_audio_cal_v2;
    acdb_get_audio_cal_v2_t            acdb_get_audio_cal_v2;
    acdb_send_meta_info_t              acdb_send_meta_info;
    acdb_send_meta_info_list_t         acdb_send_meta_info_list;
    acdb_set_codec_data_t              acdb_set_codec_data;
    acdb_is_initialized_t              acdb_is_initialized;
    acdb_reload_acdb_files_t           acdb_reload_acdb_files;
    acdb_reload_acdb_files_v2_t        acdb_reload_acdb_files_v2;
}acdb_service_data;


namespace audiocal {

using namespace android;

class acdb_loader_server : public ::android::BnInterface<Iacdb_loader_server> {
public:
    acdb_loader_server();
    ~acdb_loader_server();

private:
    acdb_service_data *mydata;
    bool init_status;

    int32_t onTransact(uint32_t code, const ::android::Parcel& data,
                 ::android::Parcel* reply, uint32_t flags = 0) override;
    int acdb_loader_init_v2(char *snd_card_name, char *cvd_version, int metaInfoKey) override;
    int acdb_loader_init_v3(char *snd_card_name, char *cvd_version, struct listnode *metaKey_list) override;
	int acdb_loader_init_v4(void* acdb_init_data, int acdb_loader_init_version) override;
    int acdb_loader_init_ACDB(void) override;
    int acdb_loader_get_default_app_type(void) override;
    int acdb_loader_send_common_custom_topology(void) override;
    void acdb_loader_deallocate_ACDB(void) override;
    void acdb_loader_send_voice_cal_v2(int rxacdb_id, int txacdb_id, int feature_set) override;
    void acdb_loader_send_voice_cal(int rxacdb_id, int txacdb_id) override;
    int acdb_loader_reload_vocvoltable(int feature_set) override;
    void acdb_loader_send_audio_cal(int acdb_id, int capability) override;
    void acdb_loader_send_audio_cal_v2(int acdb_id, int capability, int app_id, int sample_rate) override;
    void acdb_loader_send_audio_cal_v3(int acdb_id, int capability, int app_id, int sample_rate, int use_case) override;
    void acdb_loader_send_audio_cal_v4(int acdb_id, int capability, int app_id,
            int sample_rate, int use_case, int afe_sample_rate) override;
    void acdb_loader_send_listen_device_cal(int acdb_id, int type, int app_id, int sample_rate) override;
    int acdb_loader_send_listen_lsm_cal(int acdb_id, int app_id, int mode, int type) override;
    int acdb_loader_send_listen_lsm_cal_v1(int acdb_id, int app_id, int mode, int type, int buff_idx) override;
    int acdb_loader_send_anc_cal(int acdb_id) override;
    void send_tabla_anc_data(void) override;
    int acdb_loader_get_aud_volume_steps(void) override;
    int acdb_loader_send_gain_dep_cal(int acdb_id, int app_id,
        int capability, int mode, int vol_index) override;
    int acdb_loader_get_remote_acdb_id(unsigned int native_acdb_id) override;
    int acdb_loader_get_ecrx_device(int acdb_id) override;
    int acdb_loader_get_calibration(char *attr, int size, void *data) override;
    int acdb_loader_set_audio_cal_v2(void *caldata, void* data,
        unsigned int datalen) override;
    int acdb_loader_get_audio_cal_v2(void *caldata, void* data,
        unsigned int *datalen) override;
    int send_meta_info(int metaInfoKey, int buf_idx) override;
    int send_meta_info_list(struct listnode *key_list) override;
    int acdb_loader_set_codec_data(void *data, char *attr) override;
    bool acdb_loader_is_initialized(void) override;
    int acdb_loader_reload_acdb_files(char *new_acdb_file_path, char *snd_card_name, char *cvd_version, int metaInfoKey) override;
    int acdb_loader_reload_acdb_files_v2(char *new_acdb_file_path, char *snd_card_name , char *cvd_version, struct listnode *meta_key_list) override;
};
}; // namespace audiocal

