/**
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "audcalparam_serv_con"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "audcalparam_serv_con.h"
#include "audcalparam_api.h"

#ifdef USE_LIBACDBLOADER

#define LIB_ACDB_LOADER "libacdbloaderclient.so"
typedef int  (*acdb_init_t)(const char *, const char *, int);
typedef void (*acdb_deinit_t)(void);
typedef int (*acdb_get_cal_t)(void *caldata, void* data, unsigned int *datalen);
typedef int (*acdb_set_cal_t)(void *caldata, void* data, unsigned int datalen);
typedef int (*acdb_get_asm_top_t)(struct audio_cal_asm_top *audstrm_top);
typedef int (*acdb_get_adm_top_t)(struct audio_cal_adm_top *adm_top);

#endif

typedef struct {
    void *con;
    void *acdb_handle;
#ifdef USE_LIBACDBLOADER
    acdb_get_cal_t acdb_get_cal;
    acdb_set_cal_t acdb_set_cal;
    acdb_get_asm_top_t acdb_get_asm_top;
    acdb_get_adm_top_t acdb_get_adm_top;
    acdb_deinit_t acdb_deinit;
#endif
}audcalparam_serv_con_t;

void *audcalparam_serv_con_init(char *snd_card_name){

    audcalparam_serv_con_t * sh = NULL;
    char * perr=NULL;

#ifdef USE_LIBACDBLOADER
    //init con-handle
    int32_t r=AUDCALPARAM_SERVCONERR;
    sh = malloc(sizeof(audcalparam_serv_con_t));
    if (sh==NULL){
        ALOGE("%s: Memory allocation for %s", __func__, LIB_ACDB_LOADER);
        goto exit;
    }
    acdb_init_t acdb_init;
    sh->acdb_handle = dlopen(LIB_ACDB_LOADER, RTLD_NOW);
    if (sh->acdb_handle == NULL) {
        ALOGE("%s: ERROR. dlopen failed for %s", __func__, LIB_ACDB_LOADER);
        goto cleanup;
    }

    acdb_init = (acdb_init_t)dlsym(sh->acdb_handle,"acdb_loader_init_v2");
    if (acdb_init == NULL) {
        perr = dlerror();
        ALOGE("%s: dlsym error %s for acdb_init", __func__, perr ? perr:"none");
    goto cleanup;
    }

    sh->acdb_deinit = (acdb_deinit_t)dlsym(sh->acdb_handle, "acdb_loader_deallocate_ACDB");
    if (sh->acdb_deinit == NULL) {
        perr = dlerror();
        ALOGE("%s: dlsym error %s for acdb_loader_deallocate_ACDB", __func__, perr ? perr:"none");
        goto cleanup;
    }

    sh->acdb_get_cal = (acdb_get_cal_t)dlsym(sh->acdb_handle, "acdb_loader_get_audio_cal_v2");
    if (sh->acdb_get_cal == NULL) {
        perr = dlerror();
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_get_audio_cal_v2", __func__,perr ? perr:"none");
        goto cleanup;
    }

    sh->acdb_set_cal = (acdb_set_cal_t)dlsym(sh->acdb_handle, "acdb_loader_set_audio_cal_v2");
    if (sh->acdb_set_cal == NULL) {
        perr = dlerror();
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_set_audio_cal_v2", __func__, perr ? perr:"none");
        goto cleanup;
    }
/*
    sh->acdb_get_asm_top = (acdb_get_asm_top_t)dlsym(sh->acdb_handle, "acdb_loader_get_asm_topology");
    if (sh->acdb_get_asm_top == NULL) {
        perr = dlerror();
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_get_asm_topology", __func__, perr ? perr:"none");
        goto cleanup;
    }

    sh->acdb_get_adm_top = (acdb_get_adm_top_t)dlsym(sh->acdb_handle, "acdb_loader_get_adm_topology");
    if (sh->acdb_get_adm_top == NULL) {
        perr = dlerror();
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_get_adm_topology", __func__, perr ? perr:"none");
        goto cleanup;
    }
*/
    r=acdb_init(snd_card_name, NULL, 0);

    if (r){
        ALOGE("acdb_loader_init for %s returned %d",snd_card_name,r);
        goto cleanup;
    }
    goto exit;
cleanup:
    if (sh->acdb_handle)
        dlclose(sh->acdb_handle);
    free(sh);
    sh=NULL;
#endif
exit:
    return (void*)sh;
}


int32_t audcalparam_serv_con_param_set(void *h, audcalparam_cmd_tunnel_cfg_t *cfg, uint8_t *pbuf, uint32_t *pbuf_len){

    audcalparam_serv_con_t * sh=(audcalparam_serv_con_t *)h;
    int32_t r=AUDCALPARAM_SERVCONERR;
    if (sh==NULL || sh->acdb_set_cal==NULL)
        goto exit;
#ifdef USE_LIBACDBLOADER

    ALOGD("tunnel_cmd->persist=%d",cfg->persist);
    ALOGD("tunnel_cmd->sampling_rate=%d",cfg->sampling_rate);
    ALOGD("tunnel_cmd->acdb_dev_id=%d",cfg->acdb_dev_id);
    ALOGD("tunnel_cmd->topo=%0x",cfg->topo_id);
    ALOGD("tunnel_cmd->app_type=%0x",cfg->app_type);
    ALOGD("tunnel_cmd->cal_type=%d",cfg->cal_type);
    ALOGD("tunnel_cmd->module_id=%0x",cfg->module_id);
    ALOGD("tunnel_cmd->instance_id=%d",cfg->instance_id);
    ALOGD("tunnel_cmd->reserved=%d",cfg->reserved);
    ALOGD("tunnel_cmd->param_id=%0x",cfg->param_id);

    r=sh->acdb_set_cal(cfg, pbuf, *pbuf_len);

#endif
exit:
    return r;
}
#define AUDCAL_GET_BUF_MOD_ID_POS 0
#define AUDCAL_GET_BUF_INST_ID_POS (AUDCAL_GET_BUF_MOD_ID_POS+sizeof(int32_t))
#define AUDCAL_GET_BUF_PARAMID_POS (AUDCAL_GET_BUF_INST_ID_POS+sizeof(int32_t))
#define AUDCAL_GET_BUF_PAYLOAD_LEN_POS (AUDCAL_GET_BUF_PARAMID_POS+sizeof(int32_t))
#define AUDCAL_GET_BUF_PAYLOAD_POS (AUDCAL_GET_BUF_PAYLOAD_LEN_POS+sizeof(int32_t))

int32_t audcalparam_serv_con_param_get(void *h, audcalparam_cmd_tunnel_cfg_t *cfg, uint8_t *pbuf, uint32_t *pbuf_len,  uint8_t get_buf_pars_en){

    audcalparam_serv_con_t * sh=(audcalparam_serv_con_t *)h;
    int32_t r=AUDCALPARAM_SERVCONERR;
    if (sh==NULL || sh->acdb_get_cal==NULL)
        goto exit;
#ifdef USE_LIBACDBLOADER
    // uint8_t *pbuf=*ppbuf;
#ifdef AUDCALPARAM_DBG
    ALOGD("tunnel_cmd->persist=%d",cfg->persist);
    ALOGD("tunnel_cmd->sampling_rate=%d",cfg->sampling_rate);
    ALOGD("tunnel_cmd->acdb_dev_id=%d",cfg->acdb_dev_id);
    ALOGD("tunnel_cmd->topo=%0x",cfg->topo_id);
    ALOGD("tunnel_cmd->app_type=%0x",cfg->app_type);
    ALOGD("tunnel_cmd->cal_type=%d",cfg->cal_type);
    ALOGD("tunnel_cmd->module_id=%0x",cfg->module_id);
    ALOGD("tunnel_cmd->instance_id=%d",cfg->instance_id);
    ALOGD("tunnel_cmd->reserved=%d",cfg->reserved);
    ALOGD("tunnel_cmd->param_id=%0x",cfg->param_id);
#endif
    r=sh->acdb_get_cal(cfg, pbuf, pbuf_len);
#ifdef AUDCALPARAM_DBG
    ALOGD("Buf len=%d ret=%d",*pbuf_len,r);
    uint32_t i;
    for (i=0;i<*pbuf_len;i+=4){
        ALOGD("%04x",*(uint32_t*)&pbuf[i]);
    }
#endif
    if (r == AUDCALPARAM_OK && get_buf_pars_en && cfg->persist==0){
        if (*pbuf_len>AUDCAL_GET_BUF_PAYLOAD_POS){
            // parsing only when persist NOT set
            uint32_t modid=0;
            uint32_t instid=0;
            uint32_t parid=0;
            uint32_t payloadlen=0;
            modid=*(uint32_t*)(&pbuf[AUDCAL_GET_BUF_MOD_ID_POS]);
            instid=*(uint32_t*)(&pbuf[AUDCAL_GET_BUF_INST_ID_POS]);
            parid=*(uint32_t*)(&pbuf[AUDCAL_GET_BUF_PARAMID_POS]);
            // return only payload
            payloadlen=*(uint32_t*)(&pbuf[AUDCAL_GET_BUF_PAYLOAD_LEN_POS]);//return len of the payload only
#ifdef AUDCALPARAM_DBG
            ALOGD("%s parsing: modid=%d, instid=%d, parid=%d, payloadlen=%d",__func__,modid,instid,parid,payloadlen);
#endif
            //if (payloadlen>0 && payloadlen+AUDCAL_GET_BUF_PAYLOAD_POS==*pbuf_len){
            if (payloadlen>0){
                // *ppbuf=(uint8_t*)&pbuf[AUDCAL_GET_BUF_PAYLOAD_POS];
                if (payloadlen<AUDCAL_GET_BUF_PAYLOAD_POS){
                    //null first bytes
                    memset(&pbuf[0],0,AUDCAL_GET_BUF_PAYLOAD_POS);
                }
                memmove(&pbuf[0], &pbuf[AUDCAL_GET_BUF_PAYLOAD_POS], payloadlen);
                for(int h=0;h<payloadlen;h+=2)
                {
                    ALOGD("%d", *(uint16_t*)&pbuf[h]);
                }
                *pbuf_len=payloadlen;
            }else{
                *pbuf_len=0;
                ALOGE("Wrong payload length %d for get buffer!",payloadlen);
            }
        }
        else
            ALOGE("Provided buffer not long enough. Parsing format for get buffer not known!");
    }

#endif
exit:
    return r;
}

int32_t audcalparam_serv_con_get_asm_topology(void *h, struct audio_cal_asm_top *asm_top){
    audcalparam_serv_con_t * sh=(audcalparam_serv_con_t *)h;
    int32_t r=AUDCALPARAM_SERVCONERR;
    if (sh==NULL || sh->acdb_get_asm_top==NULL)
        goto exit;
#ifdef USE_LIBACDBLOADER
    r=sh->acdb_get_asm_top(asm_top);
#endif
exit:
    return r;
}

int32_t audcalparam_serv_con_get_adm_topology(void *h, struct audio_cal_adm_top *adm_top){
    audcalparam_serv_con_t * sh=(audcalparam_serv_con_t *)h;
    int32_t r=AUDCALPARAM_SERVCONERR;
    if (sh==NULL || sh->acdb_get_adm_top==NULL)
        goto exit;
#ifdef USE_LIBACDBLOADER
    r=sh->acdb_get_adm_top(adm_top);
#endif
exit:
    return r;
}

int32_t audcalparam_serv_con_close(void *h){

    audcalparam_serv_con_t * sh=(audcalparam_serv_con_t *)h;
    int32_t r=AUDCALPARAM_SERVCONERR;
    if (sh==NULL || sh->acdb_deinit==NULL)
        goto exit;
#ifdef USE_LIBACDBLOADER
        sh->acdb_deinit();
        dlclose(sh->acdb_handle);
        r=0;
#endif
    if (sh!=NULL)
        free(sh);
exit:
    return r;
}
