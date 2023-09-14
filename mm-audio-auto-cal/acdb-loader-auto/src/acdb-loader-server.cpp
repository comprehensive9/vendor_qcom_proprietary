/* Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * acdb_loader_server
 *
 * Manages multiple acdb loader clients
 * This is implemented as a Singleton Class !
 * First client that requests to be initialized, instantiates acdb_loader_server
 */

#define LOG_NDDEBUG 0
#define LOG_TAG "acdb_loader_server"

#include <stdlib.h>
#include <mutex>
#include <utils/RefBase.h>
#include <binder/TextOutput.h>

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryDealer.h>
#include <dlfcn.h>
#include <fcntl.h>

#include "acdb-loader-server.h"
extern "C"
{
#include "acdb.h"
#include "acdb-loader.h"
}

namespace audiocal {

using namespace android;

#define LIB_ACDB_LOADER "libacdbloader.so"

std::mutex acdb_loader_mutex;

int32_t acdb_loader_server::onTransact(uint32_t code,
                                   const Parcel& data,
                                   Parcel* reply,
                                   uint32_t flags) {
    ALOGV("acdb_loader_server::onTransact(%i) %i", code, flags);
    data.checkInterface(this);

    switch(code) {
        case INIT_V2: {
            const char *snd_card_name = data.readCString();
            const char *cvd_version = data.readCString();
            if (snd_card_name == NULL || cvd_version == NULL) {
                ALOGE("Bad Binder transaction: INIT_V2");
                reply->writeInt32(static_cast<int32_t> (BAD_VALUE));
                return NO_ERROR;
            }
            int metaInfoKey;
            data.readInt32(&metaInfoKey);
            ALOGD("%s INIT_V2: snd(%s), cvd(%s), metaInfoKey(%d)", __func__, snd_card_name, cvd_version, metaInfoKey);
            if(strcmp("null", snd_card_name) == 0) {
                snd_card_name = NULL;
            }
            if(strcmp("null", cvd_version) == 0) {
                cvd_version = NULL;
            }
            int ret = acdb_loader_init_v2((char *)snd_card_name, (char *)cvd_version, metaInfoKey);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case INIT_V3: {
            const char *snd_card_name = data.readCString();
            const char *cvd_version = data.readCString();
            if (snd_card_name == NULL || cvd_version == NULL) {
                ALOGE("Bad Binder transaction: INIT_V3");
                reply->writeInt32(static_cast<int32_t> (BAD_VALUE));
                return NO_ERROR;
            }

            struct listnode key_list;
            struct meta_key_list key_info;
            data.readUint32(&key_info.cal_info.nKey);
            list_init(&key_list);
            list_add_tail(&key_list, &key_info.list);

            ALOGD("%s INIT_V3: snd(%s), cvd(%s), key_info = %u", __func__, snd_card_name, cvd_version, key_info.cal_info.nKey);
            if(strcmp("null", snd_card_name) == 0) {
                snd_card_name = NULL;
            }
            if(strcmp("null", cvd_version) == 0) {
                cvd_version = NULL;
            }
            int ret = acdb_loader_init_v3((char *)snd_card_name, (char*)cvd_version, &key_list);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case INIT_V4: {
            int32_t init_version = ACDB_LOADER_INIT_V4;
            bool is_instance_id_supported;
            struct listnode key_list;
            struct meta_key_list key_info;
            struct acdb_init_data_v4   acdb_init_data;

            const char *snd_card_name = data.readCString();
            const char *cvd_version = data.readCString();
            if (snd_card_name == NULL || cvd_version == NULL) {
                ALOGE("Bad Binder transaction: INIT_V4");
                reply->writeInt32(static_cast<int32_t> (BAD_VALUE));
                return NO_ERROR;
            }
            ALOGD("%s INIT_V4: acdb_loader_init_version(%d), cvd_version = %s, snd_card_name = %s", __func__, init_version, cvd_version, snd_card_name);

            if(strcmp("null", snd_card_name) == 0) {
                snd_card_name = NULL;
            }
            if(strcmp("null", cvd_version) == 0) {
                cvd_version = NULL;
            }
            data.read(&is_instance_id_supported, sizeof(is_instance_id_supported));

            data.readUint32(&key_info.cal_info.nKey);
            list_init(&key_list);
            list_add_tail(&key_list, &key_info.list);

            acdb_init_data.cvd_version = strdup(cvd_version);
            acdb_init_data.snd_card_name = strdup(snd_card_name);
            acdb_init_data.meta_key_list = &key_list;
            acdb_init_data.is_instance_id_supported = &is_instance_id_supported;

            int ret = acdb_loader_init_v4((void*)&acdb_init_data, init_version);
            reply->writeBool(is_instance_id_supported);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case INIT_ACDB: {
            ALOGD("%s INIT_ACDB", __func__);
            int ret = acdb_loader_init_ACDB();
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case GET_DEF_APP_TYPE: {
            ALOGD("%s GET_DEF_APP_TYPE", __func__);
            int ret = acdb_loader_get_default_app_type();
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SEND_COM_CUSTOM_TOP: {
            ALOGD("%s SEND_COM_CUSTOM_TOP", __func__);
            int ret = acdb_loader_send_common_custom_topology();
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case DEALLOCATE_ACDB: {
            ALOGD("%s DEALLOCATE_ACDB", __func__);
            acdb_loader_deallocate_ACDB();
            return NO_ERROR;
        }
        break;

        case SEND_VOICE_CAL_V2: {
            int rxacdb_id;
            data.readInt32(&rxacdb_id);
            int txacdb_id;
            data.readInt32(&txacdb_id);
            int feature_set;
            data.readInt32(&feature_set);
            ALOGD("%s SEND_VOICE_CAL_V2: rxacdb_id(%d), txacdb_id(%d), feature_set(%d)", __func__, rxacdb_id, txacdb_id, feature_set);
            acdb_loader_send_voice_cal_v2(rxacdb_id, txacdb_id, feature_set);
            return NO_ERROR;
        }
        break;

        case SEND_VOICE_CAL: {
            int rxacdb_id;
            data.readInt32(&rxacdb_id);
            int txacdb_id;
            data.readInt32(&txacdb_id);
            ALOGD("%s SEND_VOICE_CAL: rxacdb_id(%d), txacdb_id(%d)", __func__, rxacdb_id, txacdb_id);
            acdb_loader_send_voice_cal(rxacdb_id, txacdb_id);
            return NO_ERROR;
        }
        break;

        case RELOAD_VOICE_VOL_TABLE: {
            int feature_set;
            data.readInt32(&feature_set);
            ALOGD("%s RELOAD_VOICE_VOL_TABLE: feature_set(%d)", __func__, feature_set);
            int ret = acdb_loader_reload_vocvoltable(feature_set);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SEND_AUDIO_CAL: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int capability;
            data.readInt32(&capability);
            ALOGD("%s SEND_AUDIO_CAL: acdb_id(%d), capability(%d)", __func__, acdb_id, capability);
            acdb_loader_send_audio_cal(acdb_id, capability);
            return NO_ERROR;
        }
        break;

        case SEND_AUDIO_CAL_V2: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int capability;
            data.readInt32(&capability);
            int app_id;
            data.readInt32(&app_id);
            int sample_rate;
            data.readInt32(&sample_rate);
            ALOGD("%s SEND_AUDIO_CAL_V2: acdb_id(%d), capability(%d), app_id(%d), sample_rate(%d)", __func__, acdb_id, capability, app_id, sample_rate);
            acdb_loader_send_audio_cal_v2(acdb_id, capability, app_id, sample_rate);
            return NO_ERROR;
        }
        break;

        case SEND_AUDIO_CAL_V3: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int capability;
            data.readInt32(&capability);
            int app_id;
            data.readInt32(&app_id);
            int sample_rate;
            data.readInt32(&sample_rate);
            int use_case;
            data.readInt32(&use_case);
            ALOGD("%s SEND_AUDIO_CAL_V3: acdb_id(%d), capability(%d), app_id(%d), sample_rate(%d), use_case(%d)", __func__, acdb_id, capability, app_id, sample_rate, use_case);
            acdb_loader_send_audio_cal_v3(acdb_id, capability, app_id, sample_rate, use_case);
            return NO_ERROR;
        }
        break;

        case SEND_AUDIO_CAL_V4: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int capability;
            data.readInt32(&capability);
            int app_id;
            data.readInt32(&app_id);
            int sample_rate;
            data.readInt32(&sample_rate);
            int use_case;
            data.readInt32(&use_case);
            int afe_sample_rate;
            data.readInt32(&afe_sample_rate);
            ALOGD("%s SEND_AUDIO_CAL_V4: acdb_id(%d), capability(%d), app_id(%d), sample_rate(%d), use_case(%d), afe_sample_rate(%d)", __func__, acdb_id, capability, app_id, sample_rate, use_case, afe_sample_rate);
            acdb_loader_send_audio_cal_v4(acdb_id, capability, app_id, sample_rate, use_case, afe_sample_rate);
            return NO_ERROR;
        }
        break;

        case SEND_LISTEN_DEV_CAL: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int type;
            data.readInt32(&type);
            int app_id;
            data.readInt32(&app_id);
            int sample_rate;
            data.readInt32(&sample_rate);
            ALOGD("%s SEND_LISTEN_DEV_CAL: acdb_id(%d), type(%d), app_id(%d), sample_rate(%d)", __func__, acdb_id, type, app_id, sample_rate);
            acdb_loader_send_listen_device_cal(acdb_id, type, app_id, sample_rate);
            return NO_ERROR;
        }
        break;

        case SEND_LISTEN_LSM_CAL: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int app_id;
            data.readInt32(&app_id);
            int mode;
            data.readInt32(&mode);
            int type;
            data.readInt32(&type);
            ALOGD("%s SEND_LISTEN_LSM_CAL: acdb_id(%d), app_id(%d), mode(%d), type(%d)", __func__, acdb_id, app_id, mode, type);
            int ret = acdb_loader_send_listen_lsm_cal(acdb_id, app_id, mode, type);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SEND_LISTEN_LSM_CAL_V1: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int app_id;
            data.readInt32(&app_id);
            int mode;
            data.readInt32(&mode);
            int type;
            data.readInt32(&type);
            int buff_idx;
            data.readInt32(&buff_idx);
            ALOGD("%s SEND_LISTEN_LSM_CAL_V1: acdb_id(%d), app_id(%d), mode(%d), type(%d), buff_idx(%d)", __func__, acdb_id, app_id, mode, type, buff_idx);
            int ret = acdb_loader_send_listen_lsm_cal_v1(acdb_id, app_id, mode, type, buff_idx);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SEND_ANC_CAL: {
            int acdb_id;
            data.readInt32(&acdb_id);
            ALOGD("%s SEND_ANC_CAL: acdb_id(%d)", __func__, acdb_id);
            int ret = acdb_loader_send_anc_cal(acdb_id);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SEND_ANC_DATA: {
            ALOGD("%s SEND_ANC_DATA", __func__);
            send_tabla_anc_data();
            return NO_ERROR;
        }
        break;

        case GET_AUD_VOL_STEPS: {
            ALOGD("%s GET_AUD_VOL_STEPS:", __func__);
            int ret = acdb_loader_get_aud_volume_steps();
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SEND_GAIN_DEP_CAL: {
            int acdb_id;
            data.readInt32(&acdb_id);
            int app_id;
            data.readInt32(&app_id);
            int capability;
            data.readInt32(&capability);
            int mode;
            data.readInt32(&mode);
            int vol_index;
            data.readInt32(&vol_index);
            ALOGD("%s SEND_GAIN_DEP_CAL: acdb_id(%d), app_id(%d), capability(%d), mode(%d), vol_index(%d)", __func__, acdb_id, app_id, capability, mode, vol_index);
            int ret = acdb_loader_send_gain_dep_cal(acdb_id, app_id, capability, mode, vol_index);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case GET_REMOTE_ACDB_ID: {
            unsigned int native_acdb_id;
            data.readUint32(&native_acdb_id);
            ALOGD("%s GET_REMOTE_ACDB_ID: native_acdb_id(%d)", __func__, native_acdb_id);
            int ret = acdb_loader_get_remote_acdb_id(native_acdb_id);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case GET_ECRX_DEVICE: {
            int acdb_id;
            data.readInt32(&acdb_id);
            ALOGD("%s GET_ECRX_DEVICE: acdb_id(%d)", __func__, acdb_id);
            int ret = acdb_loader_get_ecrx_device(acdb_id);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case GET_CALIBRATION: {
            const char *attr = data.readCString();
            if (attr == NULL) {
                ALOGE("Bad Binder transaction: GET_CALIBRATION");
                reply->writeInt32(static_cast<int32_t> (BAD_VALUE));
                return NO_ERROR;
            }
            int size;
            data.readInt32(&size);
            ALOGD("%s GET_CALIBRATION: attr(%s), size(%d)", __func__, attr, size);
            struct param_data params;
            void *pdata = (void *)&params;
            data.read(pdata, sizeof(struct param_data));
            android::Parcel::WritableBlob w_blob;
            if(params.get_size == 0) {
                params.buff = calloc(1, params.buff_size);
                if (params.buff == NULL) {
                    reply->writeInt32(NO_MEMORY);
                    return NO_ERROR;
                }
            }
            int ret = acdb_loader_get_calibration((char *)attr, size, pdata);
            reply->write(pdata, sizeof(struct param_data));
            if(params.get_size == 0) {
                reply->writeBlob(params.buff_size, false, &w_blob);
                memset(w_blob.data(), 0x0, params.buff_size);
                memscpy(w_blob.data(), params.buff_size, params.buff, params.buff_size);
            }
            reply->writeInt32(ret);
            if(params.get_size == 0) {
                free(params.buff);
                w_blob.release();
            }
            return NO_ERROR;
        }
        break;

        case SET_AUDIO_CAL_V2: {
            unsigned int datalen;
            data.readUint32(&datalen);
            acdb_audio_cal_cfg_t cfg;
            void *caldata = (void *)&cfg;
            data.read(caldata, sizeof(acdb_audio_cal_cfg_t));
            void *pdata = calloc(1, datalen);
            if (pdata == NULL) {
                reply->writeInt32(NO_MEMORY);
                return NO_ERROR;
            }
            data.read(pdata, datalen);
            ALOGD("%s SET_AUDIO_CAL_V2: datalen(%d)", __func__, datalen);
            int ret = acdb_loader_set_audio_cal_v2(caldata, pdata, datalen);
            reply->writeInt32(ret);
            free(pdata);
            return NO_ERROR;
        }
        break;

        case GET_AUDIO_CAL_V2: {
            ALOGD("%s GET_AUDIO_CAL_V2", __func__);
            android::Parcel::WritableBlob w_blob;
            acdb_audio_cal_cfg_t cfg;
            void *caldata = (void *)&cfg;
            data.read(caldata, sizeof(acdb_audio_cal_cfg_t));
            uint32_t datalen;
            data.read(&datalen, sizeof(datalen));
            void *pdata = calloc(1, datalen);
            if (pdata == NULL) {
                reply->writeInt32(NO_MEMORY);
                return NO_ERROR;
            }
            ALOGD("%s: provided datalen(%d)", __func__, datalen);
            int ret = acdb_loader_get_audio_cal_v2(caldata, pdata, &datalen);
            ALOGD("%s: returned datalen(%d)", __func__, datalen);
            reply->writeUint32(datalen);
            reply->writeBlob(datalen, false, &w_blob);
            memset(w_blob.data(), 0x0, datalen);
            memscpy(w_blob.data(), datalen, pdata, datalen);
            reply->writeInt32(ret);
            free(pdata);
            w_blob.release();
            return NO_ERROR;
        }
        break;

        case SEND_META_INFO: {
            int metaInfoKey;
            data.readInt32(&metaInfoKey);
            int buf_idx;
            data.readInt32(&buf_idx);
            ALOGD("%s SEND_META_INFO: metaInfoKey(%d), buf_idx(%d)", __func__, metaInfoKey, buf_idx);
            int ret = send_meta_info(metaInfoKey, buf_idx);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SEND_META_INFO_LIST: {
            struct listnode key_list;
            struct meta_key_list key_info;
            data.readUint32(&key_info.cal_info.nKey);
            list_init(&key_list);
            list_add_tail(&key_list, &key_info.list);

            ALOGD("%s SEND_META_INFO_LIST key_info = %u", __func__, key_info.cal_info.nKey);
            int ret = send_meta_info_list(&key_list);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case SET_CODEC_DATA: {
            vbat_adc_data_t params;
            void *pdata = (void *)&params;
            data.read(pdata, sizeof(vbat_adc_data_t));
            const char *attr = data.readCString();
            if (attr == NULL) {
                ALOGE("Bad Binder transaction: SET_CODEC_DATA");
                reply->writeInt32(static_cast<int32_t> (BAD_VALUE));
                return NO_ERROR;
            }
            ALOGD("%s SET_CODEC_DATA: attr(%s)", __func__, attr);
            int ret = acdb_loader_set_codec_data(pdata, (char *)attr);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case IS_INITIALIZED: {
            ALOGD("%s IS_INITIALIZED", __func__);
            bool ret = acdb_loader_is_initialized();
            reply->writeBool(ret);
            return NO_ERROR;
        }
        break;

        case RELOAD_ACDB_FILES: {
            const char* new_acdb_file_path = data.readCString();
            const char *snd_card_name = data.readCString();
            const char *cvd_version = data.readCString();
            if (new_acdb_file_path == NULL || snd_card_name == NULL || cvd_version == NULL) {
                ALOGE("Bad Binder transaction: RELOAD_ACDB_FILES");
                reply->writeInt32(static_cast<int32_t> (BAD_VALUE));
                return NO_ERROR;
            }
            int metaInfoKey;
            data.readInt32(&metaInfoKey);
            ALOGD("%s RELOAD_ACDB_FILES: new_acdb_file_path(%s), snd(%s), cvd(%s), metaInfoKey(%d)", __func__, new_acdb_file_path, snd_card_name, cvd_version, metaInfoKey);
            if(strcmp("null", new_acdb_file_path) == 0) {
                new_acdb_file_path = NULL;
            }
            if(strcmp("null", snd_card_name) == 0) {
                snd_card_name = NULL;
            }
            if(strcmp("null", cvd_version) == 0) {
                cvd_version = NULL;
            }
            int ret = acdb_loader_reload_acdb_files((char *)new_acdb_file_path, (char *)snd_card_name, (char *)cvd_version, metaInfoKey);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        case RELOAD_ACDB_FILES_V2: {
            const char* new_acdb_file_path = data.readCString();
            const char *snd_card_name = data.readCString();
            const char *cvd_version = data.readCString();
            if (new_acdb_file_path == NULL || snd_card_name == NULL || cvd_version == NULL) {
                ALOGE("Bad Binder transaction: RELOAD_ACDB_FILES_V2");
                reply->writeInt32(static_cast<int32_t> (BAD_VALUE));
                return NO_ERROR;
            }
            struct listnode key_list;
            struct meta_key_list key_info;
            data.readUint32(&key_info.cal_info.nKey);
            list_init(&key_list);
            list_add_tail(&key_list, &key_info.list);

            ALOGD("%s RELOAD_ACDB_FILES_V2: new_acdb_file_path(%s), snd(%s), cvd(%s), metaInfoKey(%d)", __func__, new_acdb_file_path, snd_card_name, cvd_version, key_info.cal_info.nKey);
            if(strcmp("null", new_acdb_file_path) == 0) {
                new_acdb_file_path = NULL;
            }
            if(strcmp("null", snd_card_name) == 0) {
                snd_card_name = NULL;
            }
            if(strcmp("null", cvd_version) == 0) {
                cvd_version = NULL;
            }
            int ret = acdb_loader_reload_acdb_files_v2((char *)new_acdb_file_path, (char *)snd_card_name, (char *)cvd_version, &key_list);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
    return 0;
}

acdb_loader_server::acdb_loader_server() {
    ALOGD("acdb_loader_server::acdb_loader_server()");

    init_status = TRUE;

    mydata = new acdb_service_data;
    if(mydata == NULL) {
        init_status = FALSE;
        return;
    }
    mydata->acdb_handle = dlopen(LIB_ACDB_LOADER, RTLD_NOW);
    if (mydata->acdb_handle == NULL) {
        ALOGE("%s: DLOPEN failed for %s", __func__, LIB_ACDB_LOADER);
        init_status = FALSE;
        delete(mydata);
        mydata = NULL;
    } else {
        ALOGV("%s: DLOPEN successful for %s", __func__, LIB_ACDB_LOADER);

        mydata->acdb_init_v2 = (acdb_init_v2_t)dlsym(mydata->acdb_handle, "acdb_loader_init_v2");
        if (mydata->acdb_init_v2 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_init_v2", __func__, dlerror());

        mydata->acdb_init_v3 = (acdb_init_v3_t)dlsym(mydata->acdb_handle, "acdb_loader_init_v3");
        if (mydata->acdb_init_v3 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_init_v3", __func__, dlerror());

        mydata->acdb_init_v4 = (acdb_init_v4_t)dlsym(mydata->acdb_handle, "acdb_loader_init_v4");
        if (mydata->acdb_init_v4 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_init_v4", __func__, dlerror());

        mydata->acdb_init = (acdb_init_t)dlsym(mydata->acdb_handle, "acdb_loader_init_ACDB");
        if (mydata->acdb_init == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_init_ACDB", __func__, dlerror());

        mydata->acdb_get_default_app_type = (acdb_get_default_app_type_t)dlsym(mydata->acdb_handle, "acdb_loader_get_default_app_type");
        if (mydata->acdb_get_default_app_type == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_get_default_app_type", __func__, dlerror());

        mydata->acdb_send_common_custom_topology = (acdb_send_common_custom_topology_t)dlsym(mydata->acdb_handle, "acdb_loader_send_common_custom_topology");
        if (mydata->acdb_send_common_custom_topology == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_common_custom_topology", __func__, dlerror());

        mydata->acdb_deallocate = (acdb_deallocate_t)dlsym(mydata->acdb_handle, "acdb_loader_deallocate_ACDB");
        if (mydata->acdb_deallocate == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_deallocate_ACDB", __func__, dlerror());

        mydata->acdb_send_voice_cal_v2 = (acdb_send_voice_cal_v2_t)dlsym(mydata->acdb_handle, "acdb_loader_send_voice_cal_v2");
        if (mydata->acdb_send_voice_cal_v2 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_voice_cal_v2", __func__, dlerror());

        mydata->acdb_send_voice_cal = (acdb_send_voice_cal_t)dlsym(mydata->acdb_handle, "acdb_loader_send_voice_cal");
        if (mydata->acdb_send_voice_cal == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_voice_cal", __func__, dlerror());

        mydata->acdb_reload_vocvoltable = (acdb_reload_vocvoltable_t)dlsym(mydata->acdb_handle, "acdb_loader_reload_vocvoltable");
        if (mydata->acdb_reload_vocvoltable == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_reload_vocvoltable", __func__, dlerror());

        mydata->acdb_send_audio_cal = (acdb_send_audio_cal_t)dlsym(mydata->acdb_handle, "acdb_loader_send_audio_cal");
        if (mydata->acdb_send_audio_cal == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_audio_cal", __func__, dlerror());

        mydata->acdb_send_audio_cal_v2 = (acdb_send_audio_cal_v2_t)dlsym(mydata->acdb_handle, "acdb_loader_send_audio_cal_v2");
        if (mydata->acdb_send_audio_cal_v2 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_audio_cal_v2", __func__, dlerror());

        mydata->acdb_send_audio_cal_v3 = (acdb_send_audio_cal_v3_t)dlsym(mydata->acdb_handle, "acdb_loader_send_audio_cal_v3");
        if (mydata->acdb_send_audio_cal_v3 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_audio_cal_v3", __func__, dlerror());

        mydata->acdb_send_audio_cal_v4 = (acdb_send_audio_cal_v4_t)dlsym(mydata->acdb_handle, "acdb_loader_send_audio_cal_v4");
        if (mydata->acdb_send_audio_cal_v4 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_audio_cal_v4", __func__, dlerror());

        mydata->acdb_send_listen_device_cal = (acdb_send_listen_device_cal_t)dlsym(mydata->acdb_handle, "acdb_loader_send_listen_device_cal");
        if (mydata->acdb_send_listen_device_cal == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_listen_device_cal", __func__, dlerror());

        mydata->acdb_send_listen_lsm_cal = (acdb_send_listen_lsm_cal_t)dlsym(mydata->acdb_handle, "acdb_loader_send_listen_lsm_cal");
        if (mydata->acdb_send_listen_lsm_cal == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_listen_lsm_cal", __func__, dlerror());

        mydata->acdb_send_listen_lsm_cal_v1 = (acdb_send_listen_lsm_cal_v1_t)dlsym(mydata->acdb_handle, "acdb_loader_send_listen_lsm_cal_v1");
        if (mydata->acdb_send_listen_lsm_cal_v1 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_listen_lsm_cal_v1", __func__, dlerror());

        mydata->acdb_send_anc_cal = (acdb_send_anc_cal_t)dlsym(mydata->acdb_handle, "acdb_loader_send_anc_cal");
        if (mydata->acdb_send_anc_cal == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_anc_cal", __func__, dlerror());

        mydata->acdb_send_tabla_anc_data = (acdb_send_tabla_anc_data_t)dlsym(mydata->acdb_handle, "send_tabla_anc_data");
        if (mydata->acdb_send_tabla_anc_data == NULL)
            ALOGE("%s: dlsym error %s for send_tabla_anc_data", __func__, dlerror());

        mydata->acdb_get_aud_volume_steps = (acdb_get_aud_volume_steps_t)dlsym(mydata->acdb_handle, "acdb_loader_get_aud_volume_steps");
        if (mydata->acdb_get_aud_volume_steps == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_get_aud_volume_steps", __func__, dlerror());

        mydata->acdb_send_gain_dep_cal = (acdb_send_gain_dep_cal_t)dlsym(mydata->acdb_handle, "acdb_loader_send_gain_dep_cal");
        if (mydata->acdb_send_gain_dep_cal == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_send_gain_dep_cal", __func__, dlerror());

        mydata->acdb_get_remote_acdb_id = (acdb_get_remote_acdb_id_t)dlsym(mydata->acdb_handle, "acdb_loader_get_remote_acdb_id");
        if (mydata->acdb_get_remote_acdb_id == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_get_remote_acdb_id", __func__, dlerror());

        mydata->acdb_get_ecrx_device = (acdb_get_ecrx_device_t)dlsym(mydata->acdb_handle, "acdb_loader_get_ecrx_device");
        if (mydata->acdb_get_ecrx_device == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_get_ecrx_device", __func__, dlerror());

        mydata->acdb_get_calibration = (acdb_get_calibration_t)dlsym(mydata->acdb_handle, "acdb_loader_get_calibration");
        if (mydata->acdb_get_calibration == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_get_calibration", __func__, dlerror());

        mydata->acdb_set_audio_cal_v2 = (acdb_set_audio_cal_v2_t)dlsym(mydata->acdb_handle, "acdb_loader_set_audio_cal_v2");
        if (mydata->acdb_set_audio_cal_v2 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_set_audio_cal_v2", __func__, dlerror());

        mydata->acdb_get_audio_cal_v2 = (acdb_get_audio_cal_v2_t)dlsym(mydata->acdb_handle, "acdb_loader_get_audio_cal_v2");
        if (mydata->acdb_get_audio_cal_v2 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_get_audio_cal_v2", __func__, dlerror());

        mydata->acdb_send_meta_info = (acdb_send_meta_info_t)dlsym(mydata->acdb_handle, "send_meta_info");
        if (mydata->acdb_send_meta_info == NULL)
            ALOGE("%s: dlsym error %s for send_meta_info", __func__, dlerror());

        mydata->acdb_send_meta_info_list = (acdb_send_meta_info_list_t)dlsym(mydata->acdb_handle, "send_meta_info_list");
        if (mydata->acdb_send_meta_info_list == NULL)
            ALOGE("%s: dlsym error %s for send_meta_info_list", __func__, dlerror());

        mydata->acdb_set_codec_data = (acdb_set_codec_data_t)dlsym(mydata->acdb_handle, "acdb_loader_set_codec_data");
        if (mydata->acdb_set_codec_data == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_set_codec_data", __func__, dlerror());

        mydata->acdb_is_initialized = (acdb_is_initialized_t)dlsym(mydata->acdb_handle, "acdb_loader_is_initialized");
        if (mydata->acdb_is_initialized == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_is_initialized", __func__, dlerror());

        mydata->acdb_reload_acdb_files = (acdb_reload_acdb_files_t)dlsym(mydata->acdb_handle, "acdb_loader_reload_acdb_files");
        if (mydata->acdb_reload_acdb_files == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_reload_acdb_files", __func__, dlerror());

        mydata->acdb_reload_acdb_files_v2 = (acdb_reload_acdb_files_v2_t)dlsym(mydata->acdb_handle, "acdb_loader_reload_acdb_files_v2");
        if (mydata->acdb_reload_acdb_files_v2 == NULL)
            ALOGE("%s: dlsym error %s for acdb_loader_reload_acdb_files_v2", __func__, dlerror());
    }
}

acdb_loader_server::~acdb_loader_server() {
    ALOGD("acdb_loader_server::~acdb_loader_server()");
    if(init_status) {
        dlclose(mydata->acdb_handle);
        delete(mydata);
        mydata = NULL;
    }
}

int acdb_loader_server::acdb_loader_init_v2(char *snd_card_name,
                                        char *cvd_version,
                                        int metaInfoKey) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_init_v2)
        return mydata->acdb_init_v2(snd_card_name, cvd_version, metaInfoKey);

    return result;
}

int acdb_loader_server::acdb_loader_init_v3(char *snd_card_name, char *cvd_version, struct listnode *metaKey_list) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_init_v3)
        return mydata->acdb_init_v3(snd_card_name, cvd_version, metaKey_list);

    return result;
}

int acdb_loader_server::acdb_loader_init_v4(void* acdb_init_data, int acdb_loader_init_version) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_init_v4)
        return mydata->acdb_init_v4(acdb_init_data, acdb_loader_init_version);

    return result;
}

int acdb_loader_server::acdb_loader_init_ACDB() {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_init)
        return mydata->acdb_init();

    return result;
}

int acdb_loader_server::acdb_loader_get_default_app_type(void) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_get_default_app_type)
        return mydata->acdb_get_default_app_type();

    return result;
}

int acdb_loader_server::acdb_loader_send_common_custom_topology(void) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_send_common_custom_topology)
        return mydata->acdb_send_common_custom_topology();

    return result;
}

void acdb_loader_server::acdb_loader_deallocate_ACDB(void) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_deallocate)
        return mydata->acdb_deallocate();
}

void acdb_loader_server::acdb_loader_send_voice_cal_v2(int rxacdb_id, int txacdb_id, int feature_set) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_voice_cal_v2)
        return mydata->acdb_send_voice_cal_v2(rxacdb_id, txacdb_id, feature_set);
}

void acdb_loader_server::acdb_loader_send_voice_cal(int rxacdb_id, int txacdb_id) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_voice_cal)
        return mydata->acdb_send_voice_cal(rxacdb_id, txacdb_id);
}

int acdb_loader_server::acdb_loader_reload_vocvoltable(int feature_set) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_reload_vocvoltable)
        return mydata->acdb_reload_vocvoltable(feature_set);

    return result;
}

void acdb_loader_server::acdb_loader_send_audio_cal(int acdb_id, int capability) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_audio_cal)
        return mydata->acdb_send_audio_cal(acdb_id, capability);
}

void acdb_loader_server::acdb_loader_send_audio_cal_v2(int acdb_id, int capability, int app_id, int sample_rate) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_audio_cal_v2)
        return mydata->acdb_send_audio_cal_v2(acdb_id, capability, app_id, sample_rate);
}

void acdb_loader_server::acdb_loader_send_audio_cal_v3(int acdb_id, int capability, int app_id, int sample_rate, int use_case) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_audio_cal_v3)
        return mydata->acdb_send_audio_cal_v3(acdb_id, capability, app_id, sample_rate, use_case);
}

void acdb_loader_server::acdb_loader_send_audio_cal_v4(int acdb_id, int capability, int app_id,
    int sample_rate, int use_case, int afe_sample_rate) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_audio_cal_v4)
        return mydata->acdb_send_audio_cal_v4(acdb_id, capability, app_id, sample_rate, use_case, afe_sample_rate);
}

void acdb_loader_server::acdb_loader_send_listen_device_cal(int acdb_id, int type, int app_id, int sample_rate) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_listen_device_cal)
        return mydata->acdb_send_listen_device_cal(acdb_id, type, app_id, sample_rate);
}

int acdb_loader_server::acdb_loader_send_listen_lsm_cal(int acdb_id, int app_id, int mode, int type) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_send_listen_lsm_cal)
        return mydata->acdb_send_listen_lsm_cal(acdb_id, app_id, mode, type);

    return result;
}

int acdb_loader_server::acdb_loader_send_listen_lsm_cal_v1(int acdb_id, int app_id, int mode, int type, int buff_idx) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_send_listen_lsm_cal_v1)
        return mydata->acdb_send_listen_lsm_cal_v1(acdb_id, app_id, mode, type, buff_idx);

    return result;
}

int acdb_loader_server::acdb_loader_send_anc_cal(int acdb_id) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_send_anc_cal)
        return mydata->acdb_send_anc_cal(acdb_id);

    return result;
}

void acdb_loader_server::send_tabla_anc_data(void) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);

    if(mydata->acdb_send_tabla_anc_data)
        return mydata->acdb_send_tabla_anc_data();
}

int acdb_loader_server::acdb_loader_get_aud_volume_steps(void) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_get_aud_volume_steps)
        return mydata->acdb_get_aud_volume_steps();

    return result;
}

int acdb_loader_server::acdb_loader_send_gain_dep_cal(int acdb_id, int app_id,
    int capability, int mode, int vol_index) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_send_gain_dep_cal)
        return mydata->acdb_send_gain_dep_cal(acdb_id, app_id, capability, mode, vol_index);

    return result;
}

int acdb_loader_server::acdb_loader_get_remote_acdb_id(unsigned int native_acdb_id) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_get_remote_acdb_id)
        return mydata->acdb_get_remote_acdb_id(native_acdb_id);

    return result;
}

int acdb_loader_server::acdb_loader_get_ecrx_device(int acdb_id) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_get_ecrx_device)
        return mydata->acdb_get_ecrx_device(acdb_id);

    return result;
}

int acdb_loader_server::acdb_loader_get_calibration(char *attr, int size, void *data) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_get_calibration)
        return mydata->acdb_get_calibration(attr, size, data);

    return result;
}

int acdb_loader_server::acdb_loader_set_audio_cal_v2(void *caldata, void* data, unsigned int datalen) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_set_audio_cal_v2)
        return mydata->acdb_set_audio_cal_v2(caldata, data, datalen);

    return result;
}

int acdb_loader_server::acdb_loader_get_audio_cal_v2(void *caldata, void* data, unsigned int *datalen) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_get_audio_cal_v2)
        return mydata->acdb_get_audio_cal_v2(caldata, data, datalen);

    return result;
}

int acdb_loader_server::send_meta_info(int metaInfoKey, int buf_idx) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_send_meta_info)
        return mydata->acdb_send_meta_info(metaInfoKey, buf_idx);

    return result;
}

int acdb_loader_server::send_meta_info_list(struct listnode *key_list) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_send_meta_info_list)
        return mydata->acdb_send_meta_info_list(key_list);

    return result;
}

int acdb_loader_server::acdb_loader_set_codec_data(void *data, char *attr) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_set_codec_data)
        return mydata->acdb_set_codec_data(data, attr);

    return result;
}

bool acdb_loader_server::acdb_loader_is_initialized(void) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    bool result = FALSE;

    if(mydata->acdb_is_initialized)
        return mydata->acdb_is_initialized();

    return result;
}

int acdb_loader_server::acdb_loader_reload_acdb_files(char *new_acdb_file_path, char *snd_card_name,
                                        char *cvd_version,
                                        int metaInfoKey) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_reload_acdb_files)
        return mydata->acdb_reload_acdb_files(new_acdb_file_path, snd_card_name, cvd_version, metaInfoKey);

    return result;
}

int acdb_loader_server::acdb_loader_reload_acdb_files_v2(char *new_acdb_file_path, char *snd_card_name, char *cvd_version, struct listnode *meta_key_list) {
    std::lock_guard<std::mutex> guard(acdb_loader_mutex);
    int result = NO_INIT;

    if(mydata->acdb_reload_acdb_files_v2)
        return mydata->acdb_reload_acdb_files_v2(new_acdb_file_path, snd_card_name, cvd_version, meta_key_list);

    return result;
}
}; // namespace audiocal

