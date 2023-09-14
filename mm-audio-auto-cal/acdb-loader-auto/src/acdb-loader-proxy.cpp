/* Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_NDDEBUG 0
#define LOG_TAG "acdb_loader_proxy"

#include <stdlib.h>
#include <pthread.h>
#include <utils/RefBase.h>

#include <binder/TextOutput.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryDealer.h>

#include "acdb-loader-server.h"
extern "C"
{
#include "acdb.h"
#include "acdb-loader.h"
}

namespace audiocal {

using namespace android;

static uint32_t nResBufLen_Gain_Levels = 0;

class acdb_loader_proxy: public ::android:: BpInterface<Iacdb_loader_server> {

public:
acdb_loader_proxy(const sp<IBinder>& impl) : BpInterface<Iacdb_loader_server>(impl) {
    ALOGD("acdb_loader_proxy::acdb_loader_proxy()");
}

~acdb_loader_proxy() {
    ALOGD("acdb_loader_proxy::~acdb_loader_proxy()");
}

int acdb_loader_init_v2(char *snd_card_name, char *cvd_version, int metaInfoKey) {
    ALOGD("%s: snd(%s), cvd(%s), metaInfoKey(%d)", __func__, snd_card_name, cvd_version, metaInfoKey);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    const char dummy[] = "null";
    if (snd_card_name != NULL) {
        data.writeCString(snd_card_name);
    } else {
        data.writeCString(dummy);
    }
    if (cvd_version != NULL) {
        data.writeCString(cvd_version);
    } else {
        data.writeCString(dummy);
    }
    data.writeInt32(metaInfoKey);

    remote()->transact(INIT_V2, data, &reply);
    return reply.readInt32();
}

int acdb_loader_init_v3(char *snd_card_name, char *cvd_version, struct listnode *metaKey_list) {
    ALOGD("%s: snd(%s), cvd(%s)", __func__, snd_card_name, cvd_version);

    struct listnode *node;
    struct meta_key_list *key_info;
    int count = 0;
    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    const char dummy[] = "null";
    if (snd_card_name != NULL) {
        data.writeCString(snd_card_name);
    } else {
        data.writeCString(dummy);
    }
    if (cvd_version != NULL) {
        data.writeCString(cvd_version);
    } else {
        data.writeCString(dummy);
    }

    list_for_each(node, metaKey_list) {
        if(count > 0) {
            ALOGE("%s: Only a single value for key_list is supported in the current implementation", __func__);
            goto FAIL;
        }
        key_info = node_to_item(node, struct meta_key_list, list);
        ALOGV("%s: acdb key_info = %u", __func__, key_info->cal_info.nKey);
        data.writeUint32(key_info->cal_info.nKey);
        count++;
    }

    remote()->transact(INIT_V3, data, &reply);
    return reply.readInt32();

FAIL:
    return -EINVAL;
}

int acdb_loader_init_v4(void* acdb_init_data, int acdb_loader_init_version) {
    ALOGD("%s: acdb_loader_init_version(%d)", __func__, acdb_loader_init_version);

    Parcel data, reply;
    struct listnode *node;
    struct meta_key_list *key_info;
    int count = 0;
    struct acdb_init_data_v4 *init_data = (struct acdb_init_data_v4*) acdb_init_data;
    char *cvd_version = init_data->cvd_version;;
    char *snd_card_name = init_data->snd_card_name;
    bool *is_instance_id_supported = init_data->is_instance_id_supported;
    struct listnode *metaKey_list = init_data->meta_key_list;

    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());

    const char dummy[] = "null";
    if (snd_card_name != NULL) {
        data.writeCString(snd_card_name);
    } else {
        data.writeCString(dummy);
    }
    if (cvd_version != NULL) {
        data.writeCString(cvd_version);
    } else {
        data.writeCString(dummy);
    }
    data.write(is_instance_id_supported, sizeof(*is_instance_id_supported));

    list_for_each(node, metaKey_list) {
        if(count > 0) {
            ALOGE("%s: Only a single value for key_list is supported in the current implementation", __func__);
            goto FAIL;
        }
        key_info = node_to_item(node, struct meta_key_list, list);
        ALOGV("%s: acdb key_info = %u", __func__, key_info->cal_info.nKey);
        data.writeUint32(key_info->cal_info.nKey);
        count++;
    }
    remote()->transact(INIT_V4, data, &reply);

    reply.readBool(is_instance_id_supported);
    ALOGD("%s: returned is_instance_id_supported : %d", __func__, *is_instance_id_supported);
    return reply.readInt32();

FAIL:
    return -EINVAL;

}

int acdb_loader_init_ACDB(void) {
    ALOGD("%s", __func__);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    remote()->transact(INIT_ACDB, data, &reply);
    return reply.readInt32();
}

int acdb_loader_get_default_app_type(void) {
    ALOGD("%s", __func__);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    remote()->transact(GET_DEF_APP_TYPE, data, &reply);
    return reply.readInt32();
}

int acdb_loader_send_common_custom_topology(void) {
    ALOGD("%s", __func__);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    remote()->transact(SEND_COM_CUSTOM_TOP, data, &reply);
    return reply.readInt32();
}

void acdb_loader_deallocate_ACDB(void) {
    ALOGD("%s", __func__);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    remote()->transact(DEALLOCATE_ACDB, data, &reply);
    return;
}

void acdb_loader_send_voice_cal_v2(int rxacdb_id, int txacdb_id, int feature_set) {
    ALOGD("%s: rxacdb_id(%d), txacdb_id(%d), feature_set(%d)", __func__, rxacdb_id, txacdb_id, feature_set);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(rxacdb_id);
    data.writeInt32(txacdb_id);
    data.writeInt32(feature_set);
    remote()->transact(SEND_VOICE_CAL_V2, data, &reply);
    return;
}

void acdb_loader_send_voice_cal(int rxacdb_id, int txacdb_id) {
    ALOGD("%s: rxacdb_id(%d), txacdb_id(%d)", __func__, rxacdb_id, txacdb_id);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(rxacdb_id);
    data.writeInt32(txacdb_id);
    remote()->transact(SEND_VOICE_CAL, data, &reply);
    return;
}

int acdb_loader_reload_vocvoltable(int feature_set) {
    ALOGD("%s: feature_set(%d)", __func__,feature_set);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(feature_set);
    remote()->transact(RELOAD_VOICE_VOL_TABLE, data, &reply);
    return reply.readInt32();
}

void acdb_loader_send_audio_cal(int acdb_id, int capability) {
    ALOGD("%s: acdb_id(%d), capability(%d)", __func__, acdb_id, capability);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(capability);
    remote()->transact(SEND_AUDIO_CAL, data, &reply);
    return;
}

void acdb_loader_send_audio_cal_v2(int acdb_id, int capability, int app_id, int sample_rate) {
    ALOGD("%s: acdb_id(%d), capability(%d), app_id(%d), sample_rate(%d)", __func__, acdb_id, capability, app_id, sample_rate);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(capability);
    data.writeInt32(app_id);
    data.writeInt32(sample_rate);
    remote()->transact(SEND_AUDIO_CAL_V2, data, &reply);
    return;
}

void acdb_loader_send_audio_cal_v3(int acdb_id, int capability, int app_id, int sample_rate, int use_case) {
    ALOGD("%s: acdb_id(%d), capability(%d), app_id(%d), sample_rate(%d), use_case(%d)", __func__, acdb_id, capability, app_id, sample_rate, use_case);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(capability);
    data.writeInt32(app_id);
    data.writeInt32(sample_rate);
    data.writeInt32(use_case);
    remote()->transact(SEND_AUDIO_CAL_V3, data, &reply);
    return;
}

void acdb_loader_send_audio_cal_v4(int acdb_id, int capability, int app_id,
    int sample_rate, int use_case, int afe_sample_rate) {
    ALOGD("%s: acdb_id(%d), capability(%d), app_id(%d), sample_rate(%d), use_case(%d), afe_sample_rate(%d)", __func__, acdb_id, capability, app_id, sample_rate, use_case, afe_sample_rate);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(capability);
    data.writeInt32(app_id);
    data.writeInt32(sample_rate);
    data.writeInt32(use_case);
    data.writeInt32(afe_sample_rate);
    remote()->transact(SEND_AUDIO_CAL_V4, data, &reply);
    return;
}

void acdb_loader_send_listen_device_cal(int acdb_id, int type, int app_id, int sample_rate) {
    ALOGD("%s: acdb_id(%d), type(%d), app_id(%d), sample_rate(%d)", __func__, acdb_id, type, app_id, sample_rate);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(type);
    data.writeInt32(app_id);
    data.writeInt32(sample_rate);
    remote()->transact(SEND_LISTEN_DEV_CAL, data, &reply);
    return;
}

int acdb_loader_send_listen_lsm_cal(int acdb_id, int app_id, int mode, int type) {
    ALOGD("%s: acdb_id(%d), app_id(%d), mode(%d), type(%d)", __func__, acdb_id, app_id, mode, type);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(app_id);
    data.writeInt32(mode);
    data.writeInt32(type);
    remote()->transact(SEND_LISTEN_LSM_CAL, data, &reply);
    return reply.readInt32();
}

int acdb_loader_send_listen_lsm_cal_v1(int acdb_id, int app_id, int mode, int type, int buff_idx) {
    ALOGD("%s: acdb_id(%d), app_id(%d), mode(%d), type(%d), buff_idx(%d)", __func__, acdb_id, app_id, mode, type, buff_idx);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(app_id);
    data.writeInt32(mode);
    data.writeInt32(type);
    data.writeInt32(buff_idx);
    remote()->transact(SEND_LISTEN_LSM_CAL_V1, data, &reply);
    return reply.readInt32();
}

int acdb_loader_send_anc_cal(int acdb_id) {
    ALOGD("%s: acdb_id(%d)", __func__, acdb_id);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    remote()->transact(SEND_ANC_CAL, data, &reply);
    return reply.readInt32();
}

void send_tabla_anc_data(void) {
    ALOGD("%s", __func__);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    remote()->transact(SEND_ANC_DATA, data, &reply);
    return;
}

int acdb_loader_get_aud_volume_steps(void) {
    ALOGD("%s", __func__);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    remote()->transact(GET_AUD_VOL_STEPS, data, &reply);
    return reply.readInt32();
}

int acdb_loader_send_gain_dep_cal(int acdb_id, int app_id,
    int capability, int mode, int vol_index) {
    ALOGD("%s: acdb_id(%d), app_id(%d), capability(%d), mode(%d), vol_index(%d)", __func__, acdb_id, app_id, capability, mode, vol_index);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    data.writeInt32(app_id);
    data.writeInt32(capability);
    data.writeInt32(mode);
    data.writeInt32(vol_index);
    remote()->transact(SEND_GAIN_DEP_CAL, data, &reply);
    return reply.readInt32();
}

int acdb_loader_get_remote_acdb_id(unsigned int native_acdb_id) {
    ALOGD("%s: native_acdb_id(%d)", __func__, native_acdb_id);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeUint32(native_acdb_id);
    remote()->transact(GET_REMOTE_ACDB_ID, data, &reply);
    return reply.readInt32();
}

int acdb_loader_get_ecrx_device(int acdb_id) {
    ALOGD("%s: acdb_id(%d)", __func__, acdb_id);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(acdb_id);
    remote()->transact(GET_ECRX_DEVICE, data, &reply);
    return reply.readInt32();
}

int acdb_loader_get_calibration(char *attr, int size, void *pdata) {
    ALOGD("%s: attr(%s), size(%d)", __func__, attr, size);

    Parcel data, reply;
    const char dummy[] = "null";
    android::Parcel::ReadableBlob r_blob;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    if (attr != NULL) {
        data.writeCString(attr);;
    } else {
        data.writeCString(dummy);
    }
    data.writeInt32(size);
    data.write(pdata, sizeof(struct param_data));
    remote()->transact(GET_CALIBRATION, data, &reply);
    struct param_data * params = (struct param_data *)pdata;
    // backup buffer of proxy
    void * pbuff = params->buff;
    reply.read(pdata, sizeof(struct param_data));
    // restore buffer of proxy
    params->buff = pbuff;
    if(params->get_size == 0) {
        reply.readBlob(params->buff_size, &r_blob);
        memscpy(params->buff, params->buff_size, r_blob.data(), params->buff_size);
        r_blob.release();
    }
    return reply.readInt32();
}

int acdb_loader_set_audio_cal_v2(void *caldata, void* pdata,
    unsigned int datalen) {
    ALOGD("%s: datalen(%d)", __func__, datalen);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    // transfer datalen first
    data.writeUint32(datalen);
    data.write(caldata, sizeof(acdb_audio_cal_cfg_t));
    data.write(pdata, datalen);
    remote()->transact(SET_AUDIO_CAL_V2, data, &reply);
    return reply.readInt32();
}

int acdb_loader_get_audio_cal_v2(void *caldata, void* pdata,
    unsigned int *datalen) {
    ALOGD("%s: provided datalen(%d)", __func__, *datalen);

    Parcel data, reply;
    android::Parcel::ReadableBlob r_blob;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.write(caldata, sizeof(acdb_audio_cal_cfg_t));
    data.write(datalen, sizeof(*datalen));
    remote()->transact(GET_AUDIO_CAL_V2, data, &reply);
    reply.readUint32(datalen);
    ALOGD("%s: returned datalen(%d)", __func__, *datalen);
    reply.readBlob(*datalen, &r_blob);
    memscpy(pdata, *datalen, r_blob.data(), *datalen);
    r_blob.release();
    return reply.readInt32();
}

int send_meta_info(int metaInfoKey, int buf_idx) {
    ALOGD("%s: metaInfoKey(%d), buf_idx(%d)", __func__, metaInfoKey, buf_idx);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.writeInt32(metaInfoKey);
    data.writeInt32(buf_idx);
    remote()->transact(SEND_META_INFO, data, &reply);
    return reply.readInt32();
}

int send_meta_info_list(struct listnode *metaKey_list) {
    ALOGD("%s", __func__);

    struct listnode *node;
    struct meta_key_list *key_info;
    int count = 0;
    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());

    list_for_each(node, metaKey_list) {
        if(count > 0) {
            ALOGE("%s: Only a single value for key_list is supported in the current implementation", __func__);
            goto FAIL;
        }
        key_info = node_to_item(node, struct meta_key_list, list);
        ALOGV("%s: acdb key_info = %u", __func__, key_info->cal_info.nKey);
        data.writeUint32(key_info->cal_info.nKey);
        count++;
    }

    remote()->transact(SEND_META_INFO_LIST, data, &reply);
    return reply.readInt32();

FAIL:
    return -EINVAL;

}

int acdb_loader_set_codec_data(void *pdata, char *attr) {
    ALOGD("%s: attr(%s)", __func__, attr);

    Parcel data, reply;
    const char dummy[] = "null";
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    data.write(pdata, sizeof(vbat_adc_data_t));
    if (attr != NULL) {
        data.writeCString(attr);;
    } else {
        data.writeCString(dummy);
    }
    remote()->transact(SET_CODEC_DATA, data, &reply);
    return reply.readInt32();
}

bool acdb_loader_is_initialized(void) {
    ALOGD("%s", __func__);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    remote()->transact(IS_INITIALIZED, data, &reply);
    return reply.readBool();
}

int acdb_loader_reload_acdb_files(char *new_acdb_file_path, char *snd_card_name, char *cvd_version, int metaInfoKey) {
    ALOGD("%s: new_acdb_file_path(%s), snd(%s), cvd(%s), metaInfoKey(%d)", __func__, new_acdb_file_path, snd_card_name, cvd_version, metaInfoKey);

    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    const char dummy[] = "null";
    if (new_acdb_file_path != NULL) {
        data.writeCString(new_acdb_file_path);
    } else {
        data.writeCString(dummy);
    }
    if (snd_card_name != NULL) {
        data.writeCString(snd_card_name);
    } else {
        data.writeCString(dummy);
    }
    if (cvd_version != NULL) {
        data.writeCString(cvd_version);
    } else {
        data.writeCString(dummy);
    }
    data.writeInt32(metaInfoKey);

    remote()->transact(RELOAD_ACDB_FILES, data, &reply);
    return reply.readInt32();
}

int acdb_loader_reload_acdb_files_v2(char *new_acdb_file_path, char *snd_card_name, char *cvd_version, struct listnode *metaKey_list) {
    ALOGD("%s: new_acdb_file_path(%s), snd(%s), cvd(%s)", __func__, new_acdb_file_path, snd_card_name, cvd_version);

    struct listnode *node;
    struct meta_key_list *key_info;
    int count = 0;
    Parcel data, reply;
    data.writeInterfaceToken(Iacdb_loader_server::getInterfaceDescriptor());
    const char dummy[] = "null";
    if (new_acdb_file_path != NULL) {
        data.writeCString(new_acdb_file_path);
    } else {
        data.writeCString(dummy);
    }
    if (snd_card_name != NULL) {
        data.writeCString(snd_card_name);
    } else {
        data.writeCString(dummy);
    }
    if (cvd_version != NULL) {
        data.writeCString(cvd_version);
    } else {
        data.writeCString(dummy);
    }

    list_for_each(node, metaKey_list) {
        if(count > 0) {
            ALOGE("%s: Only a single value for key_list is supported in the current implementation", __func__);
            goto FAIL;
        }
        key_info = node_to_item(node, struct meta_key_list, list);
        ALOGV("%s: acdb key_info = %u", __func__, key_info->cal_info.nKey);
        data.writeUint32(key_info->cal_info.nKey);
        count++;
    }

    remote()->transact(RELOAD_ACDB_FILES_V2, data, &reply);
    return reply.readInt32();

FAIL:
    return -EINVAL;
}
};


//IMPLEMENT_META_INTERFACE(acdb_loader_server, "acdb_loader_server");
// Macro above expands to code below. Doing it by hand so we can log ctor and destructor calls.
const android::String16 Iacdb_loader_server::descriptor("Iacdb_loader_server");
const android::String16& Iacdb_loader_server::getInterfaceDescriptor() const {
    return Iacdb_loader_server::descriptor;
}

android::sp<Iacdb_loader_server> Iacdb_loader_server::asInterface
    (const android::sp<android::IBinder>& obj) {
    ALOGD("Iacdb_loader_server::asInterface()");
    android::sp<Iacdb_loader_server> intr;
    if (obj != NULL) {
        intr = static_cast<Iacdb_loader_server*>(obj->queryLocalInterface(Iacdb_loader_server::descriptor).get());
        ALOGD("Iacdb_loader_server::asInterface() interface %s",
            ((intr == 0)?"zero":"non zero"));

        if (intr == NULL) {
            intr = new acdb_loader_proxy(obj);
        }
    }
    return intr;
}

Iacdb_loader_server::Iacdb_loader_server()
    { ALOGD("Iacdb_loader_server::Iacdb_loader_server()"); }
Iacdb_loader_server::~Iacdb_loader_server()
    { ALOGD("Iacdb_loader_server::~Iacdb_loader_server()"); }

}; // namespace audiocal

