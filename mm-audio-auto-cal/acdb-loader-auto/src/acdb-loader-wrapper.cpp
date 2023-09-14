/*
 *
 * This library contains the API of acdb loader when C/S mode is enabled.
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#define LOG_TAG "acdb-loader-wrapper"
#define LOG_NDEBUG 0

#include <stdio.h>
#include <stdlib.h>

#include <binder/ProcessState.h>

#ifdef _ANDROID_
/* definitions for Android logging */
#include <utils/Log.h>
#include "common_log.h"
#else
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

#include "acdb-loader-server.h"
#include "acdb-loader-server-client.h"

using namespace audiocal;

/* acdb loader service handle */
sp<Iacdb_loader_server> g_als = NULL;

/* Flag to indicate als status */
static bool g_als_died = false;
/* Death notification handle */
sp<death_notifier> g_death_notifier = NULL;
/* Handle for client context*/
void* g_ctxt = NULL;
/* Client callback handle */
acdb_loader_callback g_acdb_loader_cb = NULL;

void death_notifier::binderDied(const wp<IBinder>& who)
{
    ALOGD("%s %d: default cb", __func__, __LINE__);
    g_als_died = true;
    g_als = NULL;
    // reinit when service death notified
    //acdb_loader_init_v2(NULL, NULL, 0);
    if (g_acdb_loader_cb) {
        ALOGD("%s %d: registered cb", __func__, __LINE__);
        g_acdb_loader_cb(g_ctxt);
    }
}

void acdb_loader_register_als_death_notify_cb(acdb_loader_callback cb, void* context)
{
    ALOGD("%s %d", __func__, __LINE__);
    g_acdb_loader_cb = cb;
    g_ctxt = context;
}

death_notifier::death_notifier()
{
    ALOGV("%s %d", __func__, __LINE__);
    sp<ProcessState> proc(ProcessState::self());
    proc->startThreadPool();
}

sp<Iacdb_loader_server> get_acdb_loader_service() {
    sp<IServiceManager> sm;
    sp<IBinder> binder;
    int retry_cnt = 5;

    if (g_als == 0) {
        sm = android::defaultServiceManager();
        if (sm != NULL) {
            do {
                binder = sm->getService(android::String16(ACDB_LOADER_SERVICE));
                if (binder != 0)
                    break;
                else
                    ALOGE("%d:%s: get acdb loader service failed",__LINE__, __func__);

                ALOGW("acdb_loader_server not published, waiting...");
                usleep(500000);
            } while (--retry_cnt);
        } else {
            ALOGE("%d:%s: defaultServiceManager failed",__LINE__, __func__);
        }
        if (binder == NULL)
            return NULL;

        if (g_death_notifier == NULL) {
            g_death_notifier = new death_notifier();
            if (g_death_notifier == NULL) {
                ALOGE("%d: %s() unable to allocate death notifier", __LINE__, __func__);
                return NULL;
            }
        }
        binder->linkToDeath(g_death_notifier);
        g_als = android::interface_cast<Iacdb_loader_server>(binder);
        assert(g_als != 0);
    }
    return g_als;
}

extern "C" {

int acdb_loader_init_v2(char *snd_card_name, char *cvd_version, int metaInfoKey) {
    int ret = 0;
    ALOGV("%d:%s",__LINE__, __func__);

    sp<Iacdb_loader_server> als = get_acdb_loader_service();
    if (als == 0) {
       ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
       return -ENODEV;
    }
    ret = als->acdb_loader_init_v2(snd_card_name, cvd_version, metaInfoKey);
    ALOGV("%d:%s: exit",__LINE__, __func__);
    if (g_als_died) {
      g_als_died = false;
    }
    return ret;
}

int acdb_loader_init_v3(char *snd_card_name, char *cvd_version, struct listnode *metaKey_list) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_init_v3(snd_card_name, cvd_version, metaKey_list);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_init_v4(void* acdb_init_data, int acdb_loader_init_version) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_init_v4(acdb_init_data, acdb_loader_init_version);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_init_ACDB(void) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_init_ACDB();
    } else {
        return -ENODEV;
    }
}

int acdb_loader_get_default_app_type(void) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_get_default_app_type();
    } else {
        return -ENODEV;
    }
}

int acdb_loader_send_common_custom_topology(void) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_send_common_custom_topology();
    } else {
        return -ENODEV;
    }
}

void acdb_loader_deallocate_ACDB(void) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_deallocate_ACDB();
    } else {
        return;
    }
}

void acdb_loader_send_voice_cal_v2(int rxacdb_id, int txacdb_id, int feature_set) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_send_voice_cal_v2(rxacdb_id, txacdb_id, feature_set);
    } else {
        return;
    }
}

void acdb_loader_send_voice_cal(int rxacdb_id, int txacdb_id) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_send_voice_cal(rxacdb_id, txacdb_id);
    } else {
        return;
    }
}

int acdb_loader_reload_vocvoltable(int feature_set) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_reload_vocvoltable(feature_set);
    } else {
        return -ENODEV;
    }
}

void acdb_loader_send_audio_cal(int acdb_id, int capability) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_send_audio_cal(acdb_id, capability);
    } else {
        return;
    }
}

void acdb_loader_send_audio_cal_v2(int acdb_id, int capability, int app_id, int sample_rate) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_send_audio_cal_v2(acdb_id, capability, app_id, sample_rate);
    } else {
        return;
    }
}

void acdb_loader_send_audio_cal_v3(int acdb_id, int capability, int app_id, int sample_rate, int use_case) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_send_audio_cal_v3(acdb_id, capability, app_id, sample_rate, use_case);
    } else {
        return;
    }
}

void acdb_loader_send_audio_cal_v4(int acdb_id, int capability, int app_id,
    int sample_rate, int use_case, int afe_sample_rate) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_send_audio_cal_v4(acdb_id, capability, app_id, sample_rate, use_case, afe_sample_rate);
    } else {
        return;
    }
}

void acdb_loader_send_listen_device_cal(int acdb_id, int type, int app_id, int sample_rate) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->acdb_loader_send_listen_device_cal(acdb_id, type, app_id, sample_rate);
    } else {
        return;
    }
}

int acdb_loader_send_listen_lsm_cal(int acdb_id, int app_id, int mode, int type) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_send_listen_lsm_cal(acdb_id, app_id, mode, type);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_send_listen_lsm_cal_v1(int acdb_id, int app_id, int mode, int type, int buff_idx) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_send_listen_lsm_cal_v1(acdb_id, app_id, mode, type, buff_idx);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_send_anc_cal(int acdb_id) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_send_anc_cal(acdb_id);
    } else {
        return -ENODEV;
    }
}

void send_tabla_anc_data(void) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return;
        }
        als->send_tabla_anc_data();
    } else {
        return;
    }
}

int acdb_loader_get_aud_volume_steps(void) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_get_aud_volume_steps();
    } else {
        return -ENODEV;
    }
}

int acdb_loader_send_gain_dep_cal(int acdb_id, int app_id,
    int capability, int mode, int vol_index) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_send_gain_dep_cal(acdb_id, app_id, capability, mode, vol_index);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_get_remote_acdb_id(unsigned int native_acdb_id) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_get_remote_acdb_id(native_acdb_id);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_get_ecrx_device(int acdb_id) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_get_ecrx_device(acdb_id);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_get_calibration(char *attr, int size, void *data) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_get_calibration(attr, size, data);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_set_audio_cal_v2(void *caldata, void* data,
    unsigned int datalen) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_set_audio_cal_v2(caldata, data, datalen);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_get_audio_cal_v2(void *caldata, void* data,
    unsigned int *datalen) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_get_audio_cal_v2(caldata, data, datalen);
    } else {
        return -ENODEV;
    }
}

int send_meta_info(int metaInfoKey, int buf_idx) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->send_meta_info(metaInfoKey, buf_idx);
    } else {
        return -ENODEV;
    }
}

int send_meta_info_list(struct listnode *key_list) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->send_meta_info_list(key_list);
    } else {
        return -ENODEV;
    }
}

int acdb_loader_set_codec_data(void *data, char *attr) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return -ENODEV;
        }
        return als->acdb_loader_set_codec_data(data, attr);
    } else {
        return -ENODEV;
    }
}

bool acdb_loader_is_initialized(void) {
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return false;
        }
        return als->acdb_loader_is_initialized();
    } else {
        return false;
    }
}

int acdb_loader_reload_acdb_files(char *new_acdb_file_path, char *snd_card_name, char *cvd_version, int metaInfoKey){
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return false;
        }
        return als->acdb_loader_reload_acdb_files(new_acdb_file_path, snd_card_name, cvd_version, metaInfoKey);
    } else {
        return false;
    }
}

int acdb_loader_reload_acdb_files_v2(char *new_acdb_file_path, char *snd_card_name, char *cvd_version, struct listnode *meta_key_list){
    ALOGV("%d:%s",__LINE__, __func__);
    if (!g_als_died) {
        sp<Iacdb_loader_server> als = get_acdb_loader_service();
        if (als == 0) {
           ALOGE("%d:%s: invalid acdb loader service handle",__LINE__, __func__);
           return false;
        }
        return als->acdb_loader_reload_acdb_files_v2(new_acdb_file_path, snd_card_name, cvd_version, meta_key_list);
    } else {
        return false;
    }
}
}

