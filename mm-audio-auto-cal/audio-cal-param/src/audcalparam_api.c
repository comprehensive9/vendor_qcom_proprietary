/**
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "audcalparam_api"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "audcalparam_api_priv.h"
// #include "audcalparam_api.h"
#include "audcalparam_serv_con.h"
#include "audcalparam_util.h"


/*! allocate memory for the commands set */
static audcalparam_command_set_t * audcalparam_cmd_set_alloc(void);
static audcalparam_usecase_list_t * audcalparam_usecase_list_alloc(void);

static audcalparam_err_t audcalparam_cmd_set_free(audcalparam_command_set_t *cmd_set);

static audcalparam_err_t audcalparam_cmd_free(audcalparam_command_t *cmd);

static audcalparam_err_t audcalparam_usecase_list_free(audcalparam_usecase_list_t * use_cases);

/*! parsing cfg file, fill internal cmd data structures */
static audcalparam_err_t audcalparam_cmd_set_init(audcalparam_session_t *h, json_object* jo);

static audcalparam_command_t * audcalparam_cmd_type_init(audcalparam_session_t *h, json_object* jo);

static audcalparam_err_t audcalparam_cmd_inst_init(audcalparam_command_t *cmd, json_object* jo);

static audcalparam_err_t audcalparam_usecase_list_init(audcalparam_session_t *h, json_object* jo);

static audcalparam_command_t * audcalparam_cmd_alloc(char *name);

static audcalparam_err_t audcalparam_cmd_print(audcalparam_command_t *cmd);

static audcalparam_usecase_t * audcalparam_usecase_get_byname(audcalparam_usecase_list_t *usecases_list, char* name);

static audcalparam_command_instance_t * audcalparam_cmd_inst_get_byname(audcalparam_command_t *cmd, char* name);

static audcalparam_err_t audcalparam_usecase_valid_check(audcalparam_usecase_list_t *usecases_list, char *name);

static audcalparam_err_t audcalparam_cmd_inst_valid_check(audcalparam_session_t *h, audcalparam_command_instance_t *cmd_inst);

static audcalparam_err_t audcalparam_check_and_tunnel_cmd_build(audcalparam_session_t* h,
                                                     audcalparam_command_t *cmd,
                                                     char* cmd_instance_name,
                                                     audcalparam_cmd_dir_t dir,
                                                     audcalparam_cmd_base_cfg_t *cfg,
                                                     audcalparam_cmd_tunnel_cfg_t *tunnel_cmd);

static uint32_t audcalparam_get_topoid(audcalparam_session_t* h, audcalparam_cmd_tunnel_cfg_t *tunnel_cmd);


static audcalparam_command_set_t * audcalparam_cmd_set_alloc(void){

    audcalparam_command_set_t * cmd_set=NULL;

    if ((cmd_set=(audcalparam_command_set_t*)malloc(sizeof(audcalparam_command_set_t)))==NULL)
        goto exit;
    // allocate memory for sub-struct
    if ((cmd_set->cmd_bmt=audcalparam_cmd_alloc("BMT"))==NULL)
        goto exit;
    if ((cmd_set->cmd_delay=audcalparam_cmd_alloc("Delay"))==NULL)
        goto exit;
    if ((cmd_set->cmd_fnb=audcalparam_cmd_alloc("FNB"))==NULL)
        goto exit;
    if ((cmd_set->cmd_mute=audcalparam_cmd_alloc("Mute"))==NULL)
        goto exit;
    if ((cmd_set->cmd_volume_idx=audcalparam_cmd_alloc("Volume_idx"))==NULL)
        goto exit;
    if ((cmd_set->cmd_gain=audcalparam_cmd_alloc("Gain"))==NULL)
        goto exit;
    if ((cmd_set->cmd_avc=audcalparam_cmd_alloc("AVC"))==NULL)
        goto exit;
    if ((cmd_set->cmd_sumx=audcalparam_cmd_alloc("SUMX"))==NULL)
        goto exit;
    if ((cmd_set->cmd_synth=audcalparam_cmd_alloc("SYNTH"))==NULL)
        goto exit;
    if ((cmd_set->cmd_module_param=audcalparam_cmd_alloc("Module_param"))==NULL)
        goto exit;

exit:
    return cmd_set;
}

static audcalparam_command_t * audcalparam_cmd_alloc(char * name){
    audcalparam_command_t * cmd = NULL;

    cmd = (audcalparam_command_t*)malloc(sizeof(audcalparam_command_t));
    if (cmd==NULL)
        return NULL;

    cmd->type_name=strdup(name);
    cmd->command_instance_list=NULL;
    cmd->inst_num = 0;

    return cmd;
}

static audcalparam_err_t audcalparam_cmd_set_free(audcalparam_command_set_t *cmd_set){
    audcalparam_err_t r=0;

    if (cmd_set==NULL)
        return AUDCALPARAM_HDLERR;

    // free each command set separately
    r=audcalparam_cmd_free(cmd_set->cmd_bmt);
    r|=audcalparam_cmd_free(cmd_set->cmd_delay);
    r|=audcalparam_cmd_free(cmd_set->cmd_fnb);
    r|=audcalparam_cmd_free(cmd_set->cmd_mute);
    r|=audcalparam_cmd_free(cmd_set->cmd_volume_idx);
    r|=audcalparam_cmd_free(cmd_set->cmd_gain);
    r|=audcalparam_cmd_free(cmd_set->cmd_avc);
    r|=audcalparam_cmd_free(cmd_set->cmd_sumx);
    r|=audcalparam_cmd_free(cmd_set->cmd_synth);
    r|=audcalparam_cmd_free(cmd_set->cmd_module_param);

    free(cmd_set);

    return r;
}

static audcalparam_err_t audcalparam_cmd_free(audcalparam_command_t *cmd){
    int32_t i,j;

    if (cmd==NULL || cmd->type_name ==NULL)
        return AUDCALPARAM_HDLERR;
    if (cmd->command_instance_list==NULL)
        goto free_cmd;//empty list,nothing to free

    // free each command set
    audcalparam_command_instance_t *cmd_inst=cmd->command_instance_list;
    audcalparam_command_instance_t *nxt_cmd_inst;

    for (i=0;i<cmd->inst_num;i++){
        if (cmd_inst->inst_name!=NULL)
            free(cmd_inst->inst_name);
        if (cmd_inst->use_case_name!=NULL)
            free(cmd_inst->use_case_name);
        if (cmd_inst->parambuf!=NULL)
            free(cmd_inst->parambuf);
        nxt_cmd_inst=cmd_inst->nxt_command_instance;
        free(cmd_inst);
        cmd_inst=nxt_cmd_inst;
    }
free_cmd:
#ifdef AUDCALPARAM_DBG
    ALOGD("Freeing cmd %s",cmd->type_name);
#endif
    free(cmd->type_name);
    free(cmd);
    return AUDCALPARAM_OK;
}

static audcalparam_usecase_list_t *audcalparam_usecase_list_alloc(void){

    audcalparam_usecase_list_t * use_cases=NULL;

    use_cases=(audcalparam_usecase_list_t *)malloc(sizeof(audcalparam_usecase_list_t));
    if (use_cases!=NULL){
        use_cases->use_case_num = 0;
        use_cases->usecase_list = NULL;
    }

    return use_cases;

}

static audcalparam_err_t audcalparam_cfg_parse(audcalparam_session_t *h, char* cfg_file_name){

    FILE *fp=NULL;
    struct stat filestat;
    int32_t fsize;
    char* fcont=NULL;
    void *fmap;
    audcalparam_err_t r=AUDCALPARAM_OK;;
    char* json;
    struct json_object *fjson, *root, *sct, *elt, *tmp;

    if (stat(cfg_file_name, &filestat) != 0) {
        ALOGE("Cfg file %s not found", cfg_file_name);
        return AUDCALPARAM_CMDPARSERR;
    }
    // filesize
    fsize = filestat.st_size;
    // allocate memory for file
    fcont = (char*)malloc(filestat.st_size);

    if (fcont == NULL) {
        ALOGE("Unable to allocate %d bytes for cfg file", fsize);
        return AUDCALPARAM_CMDPARSERR;
    }

    fp = fopen(cfg_file_name, "rt");
    if (fp == NULL) {
        ALOGE("Unable to open cfg file %s", cfg_file_name);
        r=AUDCALPARAM_CMDPARSERR;
        goto exit;
    }
    if (fread(fcont, fsize, 1, fp) != 1 ) {
        ALOGE("Unable t read content of cfg file%s", cfg_file_name);
        r=AUDCALPARAM_CMDPARSERR;
        goto exit;
    }

    /* Start parsing JSON file */
    fjson = json_tokener_parse (fcont);
    //check keyword config
    json_object_object_get_ex (fjson, "config", &root);
    if (!fjson || !root) {
        ALOGE("Format not known:config not found");
        r=AUDCALPARAM_CMDPARSERR;
        goto exit;
    }

    // init use-cases
    json_object_object_get_ex (root, "usecase_map", &sct);
    if(!sct){
        ALOGE("Format not known:usecase_map not found");
        r=AUDCALPARAM_CMDPARSERR;
        goto exit;
    }
    if ((r=audcalparam_usecase_list_init(h,sct))!=AUDCALPARAM_OK){
        ALOGE("Usecase list init ret%d",r);
        r=AUDCALPARAM_CMDPARSERR;
        goto exit;
    }
    // init cmd
    json_object_object_get_ex (root, "commands", &sct);
    if(!sct){
        ALOGE("Format not known:commands not found");
        r=AUDCALPARAM_CMDPARSERR;
        goto exit;
    }
    if ((r=audcalparam_cmd_set_init(h,sct))!=AUDCALPARAM_OK){
        r=AUDCALPARAM_CMDPARSERR;
        goto exit;
    }

exit:
    if (fp!=NULL)
        fclose(fp);
    free(fcont);
    return r;
}

static audcalparam_err_t audcalparam_usecase_list_init(audcalparam_session_t *h, json_object* jo){
    uint32_t i,len;
    int32_t hexstr2dec;
    struct json_object *elt, *tmp;
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;

    if (jo == NULL){
        ALOGE("Unable to init use-cases:jval not valid");
        return AUDCALPARAM_CMDUSECASEERR;
    }
    if (p_ses==NULL || p_ses->use_cases==NULL){
        ALOGE("Unable to init use-cases:use_cases not init");
        return AUDCALPARAM_CMDUSECASEERR;
    }

    len = json_object_array_length (jo);
    audcalparam_usecase_t* new_use_case=NULL;
    audcalparam_usecase_t* last_use_case=p_ses->use_cases->usecase_list;

    for (i=0; i<len; i++) {
        const char *name,*topoid,*type;
        int32_t acdb_dev_id,app_type,topo_id,itype=0;
        int id;
        elt = json_object_array_get_idx (jo, i);
        json_object_object_get_ex (elt, "name", &tmp);
        if(tmp)
            name = json_object_get_string (tmp);
        else
            // name = "";
            continue;
        json_object_object_get_ex (elt, "acdb_dev_id", &tmp);
        if(tmp)
            acdb_dev_id = json_object_get_int (tmp);
        else
            // acdb_dev_id=0;
            continue;
        json_object_object_get_ex (elt, "app_type", &tmp);
        if(tmp)
            app_type = json_object_get_int (tmp);
        else
            // app_type =0;
            continue;
        json_object_object_get_ex (elt, "topo_id", &tmp);
        if(tmp)
            topoid = json_object_get_string (tmp);
        else
            topoid="";

        if (topoid[0]!='\0'){
            hexstr2dec=(int32_t)strtol(topoid,NULL,0);
        }else{
            hexstr2dec=0;
        }
        topo_id=hexstr2dec;

        json_object_object_get_ex (elt, "type", &tmp);
        if(tmp)
            type=json_object_get_string (tmp);
        else
            type="";
        itype=1;//default
        if(type[0]!='\0'){
            if(!strcmp(type,"RX"))
                itype=1;
            else if (!strcmp(type,"TX"))
                itype=2;
        }
        // create and fill the use-case list
        new_use_case = (audcalparam_usecase_t*)malloc(sizeof(audcalparam_usecase_t));
        if (new_use_case==NULL)
            return AUDCALPARAM_CMDALLOCERR;
        if (last_use_case!=NULL){
            // next
            last_use_case->next = new_use_case;
            p_ses->use_cases->use_case_num++;
        } else {
            // first one
            p_ses->use_cases->usecase_list=new_use_case;//
            p_ses->use_cases->use_case_num=1;
        }
        last_use_case=new_use_case;
        last_use_case->next = NULL;//next of the last one always NULL
        new_use_case->name=strdup(name);
        new_use_case->acdb_dev_id=acdb_dev_id;
        new_use_case->app_type=app_type;
        new_use_case->topo_id=topo_id;
        new_use_case->type=itype;

#if AUDCALPARAM_DBG
        ALOGD("usecase found:idx=%d,name=%s",i,name);
        ALOGD("...acdb_dev_id=%d,app_type=%d,topo_id=%d",acdb_dev_id,app_type,topo_id);
#endif
    }
    return AUDCALPARAM_OK;
}

static audcalparam_err_t audcalparam_usecase_list_free(audcalparam_usecase_list_t * list){

    audcalparam_usecase_t *next_use_case, *use_case;

    int32_t i;

    if (list==NULL){
        ALOGE("...usecase list in NULL");
        return AUDCALPARAM_CMDUSECASEERR;
    }
    use_case=list->usecase_list;
    for (i=0;i<list->use_case_num;i++){
        if (use_case==NULL){
            return AUDCALPARAM_CMDUSECASEERR;
        }
#ifdef AUDCALPARAM_DBG
        ALOGD("Freeing usecase %s", use_case->name);
#endif
        free(use_case->name);
        next_use_case=use_case->next;
        free(use_case);
        use_case=next_use_case;
    }
    free (list);

    return AUDCALPARAM_OK;

}

audcalparam_err_t audcalparam_usecase_list_print(audcalparam_session_t *h){

    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;

    audcalparam_usecase_t* use_case;

    int32_t i;

    if (p_ses==NULL || p_ses->use_cases->usecase_list==NULL){
        ALOGE("...p_ses in NULL");
        return AUDCALPARAM_HDLERR;
    }
    use_case=p_ses->use_cases->usecase_list;
    for (i=0;i<p_ses->use_cases->use_case_num;i++){
        ALOGE("Usecase idx =%d",i);
        ALOGE("Usecase name=%s", use_case->name);
        ALOGE("Usecase app_type=%d", use_case->app_type);
        ALOGE("Usecase acdb_dev_id=%d", use_case->acdb_dev_id);
        ALOGE("Usecase topo_id=%d", use_case->topo_id);
        use_case=use_case->next;
    }

    return AUDCALPARAM_OK;
}

audcalparam_err_t audcalparam_cmd_set_print(audcalparam_session_t *h){

    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;

    //print each cmd
    audcalparam_cmd_print(p_ses->cmd_set->cmd_bmt);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_delay);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_fnb);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_mute);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_volume_idx);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_gain);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_avc);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_sumx);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_synth);
    audcalparam_cmd_print(p_ses->cmd_set->cmd_module_param);

    return AUDCALPARAM_OK;
}

static audcalparam_err_t audcalparam_cmd_print(audcalparam_command_t *cmd){

    int32_t i;
    audcalparam_command_instance_t *cmd_inst;
    if (cmd==NULL || cmd->command_instance_list==NULL){
        ALOGE("...Command or instance list is empty");
        return AUDCALPARAM_OK;
    }
    //print cmd
    ALOGE("Command %s",cmd->type_name);
    cmd_inst=cmd->command_instance_list;
    for (i=0;i<cmd->inst_num;i++){
        ALOGE("...instance %d:",i+1);
        ALOGE("....name=%s", cmd_inst->inst_name!=NULL ? cmd_inst->inst_name: "");
        ALOGE("....usecase=%s", cmd_inst->use_case_name!=NULL ? cmd_inst->use_case_name: "");
        ALOGE("....module_id=%d",cmd_inst->module_id);
        ALOGE("....param_id=%d",cmd_inst->param_id);
        ALOGE("....param_id=%d",cmd_inst->topo_id);
        ALOGE("....params:");
        if (cmd_inst->parambuflen>0 && cmd_inst->parambuf!=NULL){
            uint32_t j, *ppar=(uint32_t*)cmd_inst->parambuf;
            for (j=0;j<cmd_inst->parambuflen;j++){
                ALOGE("0x%x,",ppar[j]);
            }
        }
        //next
        cmd_inst=cmd_inst->nxt_command_instance;
    }

    return AUDCALPARAM_OK;
}

static audcalparam_err_t audcalparam_cmd_set_init(audcalparam_session_t *h, json_object* jo) {
    int32_t len, i;
    audcalparam_command_t *cmd=NULL;
    struct json_object *elt, *tmp;
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r=0;

    if (jo == NULL){
        ALOGE("Unable to init cmd set:json object not valid");
        return AUDCALPARAM_CMDUSECASEERR;
    }
    if (p_ses==NULL || p_ses->cmd_set==NULL){
        ALOGE("Unable to init cmd set:cmd_set not init");
        return AUDCALPARAM_CMDUSECASEERR;
    }

    len = json_object_array_length (jo);
#ifdef AUDCALPARAM_DBG
    ALOGD("Cmd set array len=%d",len);
#endif
    for (i=0;i<len;i++){
        // for each cmd
        elt = json_object_array_get_idx (jo, i);

        cmd = audcalparam_cmd_type_init(h,elt);
        if (cmd!=NULL)
            r&=audcalparam_cmd_inst_init(cmd,elt);//return error if all functions call not ok

    }
    return r;
}
static audcalparam_command_t * audcalparam_cmd_type_init(audcalparam_session_t *h, json_object* jo) {
    audcalparam_command_t *cmd=NULL;
    struct json_object *elt, *tmp=NULL;

    if (jo == NULL)
        return NULL;

    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;

    // set type
    const char *type=NULL;

    json_object_object_get_ex (jo, "type", &tmp);
    if(!tmp)
        goto exit;
    type = json_object_get_string (tmp);
    if (type){
        // make function init_cmd_type
        if (!strcmp(type, "BMT")){
            cmd = p_ses->cmd_set->cmd_bmt;
        } else if (!strcmp(type, "Delay")){
            cmd = p_ses->cmd_set->cmd_delay;
        } else if (!strcmp(type, "FNB")){
            cmd = p_ses->cmd_set->cmd_fnb;
        } else if (!strcmp(type, "Mute")){
            cmd = p_ses->cmd_set->cmd_mute;
        } else if (!strcmp(type, "Volume_idx")){
            cmd = p_ses->cmd_set->cmd_volume_idx;
        } else if (!strcmp(type, "Gain")){
            cmd = p_ses->cmd_set->cmd_gain;
        } else if (!strcmp(type, "AVC")){
            cmd = p_ses->cmd_set->cmd_avc;
        } else if (!strcmp(type, "SUMX")){
            cmd = p_ses->cmd_set->cmd_sumx;
        } else if (!strcmp(type, "SYNTH")){
            cmd = p_ses->cmd_set->cmd_synth;
        } else if (!strcmp(type, "Module_param")){
            cmd = p_ses->cmd_set->cmd_module_param;
        } else {
            ALOGE("New command type added %s",type);
        }
    }

exit:
    return cmd;
}
static audcalparam_err_t audcalparam_cmd_inst_init(audcalparam_command_t *cmd, json_object* jo) {
    struct json_object  *sct, *elt, *tmp, *tmp1;
    int len, i;
    const char *type_param, *moduleid, *paramid;
    int32_t module_id=0, param_id=0, hexstr2dec=0;
    audcalparam_command_instance_t *cmd_inst=NULL;
    void *pmem;
    uint32_t parambuflen=0;
    uint8_t *parambuf=NULL;

    // type_param
    json_object_object_get_ex (jo, "type_param", &sct);
    if(sct){
        // parse type param
        json_object_object_get_ex (sct, "module_id", &tmp);
        if (tmp){
            moduleid=json_object_get_string (tmp);
            if (moduleid[0]!='\0')
                hexstr2dec=(int32_t)strtol(moduleid,NULL,0);
            else
                hexstr2dec=0;
        }
        module_id=hexstr2dec;

        json_object_object_get_ex (sct, "param_id", &tmp);
        hexstr2dec=0;
        if (tmp){
            paramid=json_object_get_string (tmp);
            if (paramid[0]!='\0')
                hexstr2dec=(int32_t)strtol(paramid,NULL,0);
            else
                hexstr2dec=0;
        }
        param_id=hexstr2dec;
    }
    // instances
    json_object_object_get_ex (jo, "instances", &sct);
    if(!sct)
        return AUDCALPARAM_CMDPARSERR;
    len = json_object_array_length (sct);
    cmd_inst=cmd->command_instance_list;
    for (i=0; i<len; i++) {
        // for each instance
        const char *name, *usecase=NULL, *topo_type=NULL, *instanceid;
        uint32_t instance_id;
        elt = json_object_array_get_idx (sct, i);
        json_object_object_get_ex(elt, "name", &tmp);
        if(!tmp)
            continue;//skip skip parsing current instance when name not given
        name=json_object_get_string(tmp);
        json_object_object_get_ex(elt, "usecase", &tmp);
        if (tmp)
            usecase=json_object_get_string(tmp);
        json_object_object_get_ex(elt, "topo_type", &tmp);
        if (tmp)
            topo_type=json_object_get_string(tmp);
        json_object_object_get_ex(elt, "instance_id", &tmp);
        if (tmp)
            instance_id=json_object_get_int(tmp);
        else
            instance_id=0;

        // set module id and param id if given in the instance
        json_object_object_get_ex (elt, "module_id", &tmp);
        hexstr2dec=0;
        if (tmp){
            moduleid=json_object_get_string (tmp);
            if (moduleid[0]!='\0')
                hexstr2dec=(int32_t)strtol(moduleid,NULL,0);
            else
                hexstr2dec=0;
        }
        if (hexstr2dec)
            module_id=hexstr2dec;

        json_object_object_get_ex (elt, "param_id", &tmp);
        hexstr2dec=0;
        if (tmp){
            paramid=json_object_get_string (tmp);
            if (paramid[0]!='\0')
                hexstr2dec=(int32_t)strtol(paramid,NULL,0);
            else
                hexstr2dec=0;
        }
        if (hexstr2dec)
            param_id=hexstr2dec;

        json_object_object_get_ex(elt, "param", &tmp);
        if (tmp){
            parambuflen=json_object_array_length (tmp);
            if (parambuflen>0 && parambuflen<32){
                parambuf=malloc(parambuflen*sizeof(uint32_t));
                if (parambuf != NULL){
                    uint32_t j, *ppar=(uint32_t*)parambuf;
                    for (j=0;j<parambuflen;j++){
                        elt = json_object_array_get_idx (tmp, j);
                        if (elt){
                            char *paramstr=json_object_get_string(elt);
                            if (paramstr[0]!='\0'){
                                hexstr2dec=(int32_t)strtol(paramstr,NULL,0);
                            }else{
                                hexstr2dec=0;
                            }
                            ppar[j]=hexstr2dec;
                        }
                        else
                            ppar[j]=0;
                    }
                } else
                    return AUDCALPARAM_CMDALLOCERR;
            }
        }

        // create new cmd instance
        pmem=(audcalparam_command_instance_t *)malloc(sizeof(audcalparam_command_instance_t));
        if (pmem!=NULL){
            if (cmd_inst==NULL){
                cmd_inst=(audcalparam_command_instance_t *)pmem;
                cmd->command_instance_list=cmd_inst;//init list
                cmd->inst_num=1;
            } else {
                cmd_inst->nxt_command_instance=(audcalparam_command_instance_t *)pmem;
                cmd_inst=cmd_inst->nxt_command_instance;
                cmd->inst_num++;
            }
            cmd_inst->nxt_command_instance=NULL;// next of the last one always NULL!
            // fill
            cmd_inst->inst_name=strdup(name);
            if (usecase!=NULL)
                cmd_inst->use_case_name=strdup(usecase);
            cmd_inst->cal_type=0;
            if(topo_type!=NULL && !strcmp(topo_type,"POPP"))
                cmd_inst->cal_type=1;

            cmd_inst->inst_id=instance_id;
            cmd_inst->module_id=module_id;
            cmd_inst->param_id=param_id;

            cmd_inst->parambuf=parambuf;
            cmd_inst->parambuflen=parambuflen;
        } else
            return AUDCALPARAM_CMDALLOCERR;

    }

    return AUDCALPARAM_OK;

}

audcalparam_usecase_t * audcalparam_usecase_get_byname(audcalparam_usecase_list_t *usecases_list, char* name){
    audcalparam_usecase_t *usecase=NULL;
    int32_t i,found=0;
    if (usecases_list==NULL || name==NULL)
        return NULL;

    usecase=usecases_list->usecase_list;
    for (i=0;i<usecases_list->use_case_num;i++){
        if (usecase->name!=NULL && !strcmp(usecase->name,name)){
            found=1;
            break;
        }
        usecase=usecase->next;
    }

    if (!found)
        usecase=NULL;

    return usecase;
}

audcalparam_command_instance_t * audcalparam_cmd_inst_get_byname(audcalparam_command_t *cmd, char *inst_name){

    audcalparam_command_instance_t *cmd_inst=NULL;
    int32_t i,found=0;

    if (cmd==NULL || inst_name==NULL || cmd->command_instance_list==NULL)
        return NULL;

    cmd_inst =  cmd->command_instance_list;
    for (i=0;i<cmd->inst_num;i++){
        if (cmd_inst->inst_name!=NULL && !strcmp(cmd_inst->inst_name,inst_name)){
             found=1;
             break;//found!
        }
        cmd_inst=cmd_inst->nxt_command_instance;
    }
    if (!found)
        cmd_inst=NULL;

    return cmd_inst;
}

audcalparam_command_instance_t * audcalparam_cmd_inst_get_byidx(audcalparam_command_t *cmd, int idx){

    audcalparam_command_instance_t *cmd_inst=NULL;
    int32_t i,found=0;

    if (cmd==NULL || cmd->command_instance_list==NULL)
        return NULL;

    cmd_inst =  cmd->command_instance_list;
    for (i=0;i<cmd->inst_num;i++){
        if (i==idx){
             found=1;
             break;//found!
        }
        cmd_inst=cmd_inst->nxt_command_instance;
    }
    if (!found)
        cmd_inst=NULL;

    return cmd_inst;
}

static audcalparam_err_t audcalparam_usecase_valid_check(audcalparam_usecase_list_t *usecases_list, char *name){

    audcalparam_usecase_t *usecase=NULL;

    if (usecases_list==NULL || usecases_list->use_case_num==0 || name==NULL)
        return AUDCALPARAM_CMDUSECASEERR;

    usecase=audcalparam_usecase_get_byname(usecases_list,name);

    if (usecase==NULL || usecase->acdb_dev_id<=0 || usecase->app_type==0){
        return AUDCALPARAM_CMDUSECASEERR;
    }

    return AUDCALPARAM_OK;
}

static audcalparam_err_t audcalparam_cmd_inst_valid_check(audcalparam_session_t *h, audcalparam_command_instance_t *cmd_inst){

    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;

    if (p_ses==NULL || cmd_inst==NULL)
        return AUDCALPARAM_CMDINSTERR;

    //cmd instance has valid usecase
    r = audcalparam_usecase_valid_check(p_ses->use_cases,cmd_inst->use_case_name);

    return r;
}

audcalparam_err_t audcalparam_session_init(audcalparam_session_t **h, char* cfg_file_name, char *snd_card_name){
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_err_t r=AUDCALPARAM_OK;
    audcalparam_api_session_t * p_ses=NULL;

    p_ses = (audcalparam_api_session_t *)malloc(sizeof(audcalparam_api_session_t));

    if  (p_ses==NULL){
        ALOGE("Error while ses init!");
        return AUDCALPARAM_SESINITERR;
    }

    p_ses->use_cases=NULL;
    p_ses->cmd_set=NULL;
    p_ses->con_hdl=NULL;

    if ((p_ses->cmd_set = audcalparam_cmd_set_alloc())==NULL){
        ALOGE("Error while cmd set alloc!");
        r=AUDCALPARAM_CMDALLOCERR;
        goto exit;
    }
    if ((p_ses->use_cases = audcalparam_usecase_list_alloc())==NULL){
        ALOGE("Error while usecase list alloc!");
        r=AUDCALPARAM_UCALLOCERR;
        goto exit;
    }

    if ((p_ses->con_hdl = audcalparam_serv_con_init(snd_card_name))==NULL){
        ALOGE("Error while serv con init!");
#ifdef USE_LIBACDBLOADER
        r=AUDCALPARAM_SERVCONERR;
        goto exit;
#endif
     }
    //parse provided cfg file
    if (cfg_file_name[0] != '\0'){
        if ((r=audcalparam_cfg_parse((audcalparam_session_t *)p_ses, cfg_file_name))!=AUDCALPARAM_OK)
            goto exit;
    } else {
        //TODO: add defaults
        r=AUDCALPARAM_CMDCFGFILEERR;
        goto exit;
    }

exit:
    //init session handle to allocated structures
    *h=(audcalparam_session_t *)p_ses;

    return r;

}


audcalparam_err_t audcalparam_session_deinit(audcalparam_session_t *h){
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r=AUDCALPARAM_OK;

    if (p_ses==NULL)
        return AUDCALPARAM_HDLERR;

    if ((r=audcalparam_cmd_set_free(p_ses->cmd_set))!=AUDCALPARAM_OK){
        ALOGE("Cmd-set free error %d", r);
        r=AUDCALPARAM_HDLERR;
    }

    if ((r=audcalparam_usecase_list_free(p_ses->use_cases))!=AUDCALPARAM_OK){
        ALOGE("Usecase free error=%d", r);
        r|=AUDCALPARAM_HDLERR;
    }

    if ((r=audcalparam_serv_con_close(p_ses->con_hdl))!=AUDCALPARAM_OK){
        ALOGE("Serv con close error=%d", r);
        r|=AUDCALPARAM_HDLERR;
    }

    free(p_ses);

    return r;

}

static uint32_t audcalparam_get_topoid(audcalparam_session_t* h, audcalparam_cmd_tunnel_cfg_t *tunnel_cmd)
{
    uint32_t topo_id=0;
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    if (tunnel_cmd->cal_type==0){
        //copp
        struct audio_cal_adm_top adm_top;
        adm_top.cal_type.cal_info.acdb_id=tunnel_cmd->acdb_dev_id;
        adm_top.cal_type.cal_info.app_type=tunnel_cmd->app_type;
        if (audcalparam_serv_con_get_adm_topology(p_ses->con_hdl, &adm_top)==0)
            topo_id=adm_top.cal_type.cal_info.topology;
    }
    else {
        //popp
        struct audio_cal_asm_top asm_top;
        // asm_top.cal_type.cal_info.acdb_id=tunnel_cmd->acdb_dev_id;
        asm_top.cal_type.cal_info.app_type=tunnel_cmd->app_type;
        if (audcalparam_serv_con_get_asm_topology(p_ses->con_hdl, &asm_top)==0)
            topo_id=asm_top.cal_type.cal_info.topology;
    }

    return topo_id;

}

static audcalparam_err_t audcalparam_check_and_tunnel_cmd_build(audcalparam_session_t* h,
                                                     audcalparam_command_t *cmd,
                                                     char* cmd_instance_name,
                                                     audcalparam_cmd_dir_t dir,
                                                     audcalparam_cmd_base_cfg_t *cfg,
                                                     audcalparam_cmd_tunnel_cfg_t *tunnel_cmd)
{
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;
    audcalparam_command_instance_t *cmd_inst;
    audcalparam_usecase_t *use_case;

    //move check in one function check(h,cmd_type,cmd_inst_name,dir,value,cfg)

    if (h==NULL)
        return AUDCALPARAM_HDLERR;

    if (cmd==NULL || cmd->inst_num==0){
        ALOGE("%s: cmd inst error",__func__);
        return AUDCALPARAM_CMDTYPERR;
    }

    cmd_inst=audcalparam_cmd_inst_get_byname(cmd, cmd_instance_name);
    if (cmd_inst==NULL){
        ALOGE("Cmd instance %s not found", cmd_instance_name);
        return AUDCALPARAM_CMDINSTERR;
    }

    // check if other param(usecase) OK
    r=audcalparam_cmd_inst_valid_check(h, cmd_inst);
    if (r)
        return r;
    if (dir!=AUDCALPARAM_SET && dir!=AUDCALPARAM_GET)
        return AUDCALPARAM_CMDDIRERR;
#ifdef AUDCALPARAM_DBG
    ALOGD("CMD Instance found name = %s, usecase=%s",cmd_inst->inst_name, cmd_inst->use_case_name);
#endif
    //end check
    //build tunnel_cmd: fill audcalparam_cmd_tunnel_cfg_t
    if (tunnel_cmd==NULL)
        return AUDCALPARAM_CMDALLOCERR;
    use_case=audcalparam_usecase_get_byname(p_ses->use_cases, cmd_inst->use_case_name);

    // TODO: check these if valid
    tunnel_cmd->persist=cfg->cache;
    tunnel_cmd->sampling_rate=cfg->sampling_rate;
    if (use_case!=NULL){
        tunnel_cmd->acdb_dev_id= 60;//use_case->acdb_dev_id;
        tunnel_cmd->app_type=use_case->app_type;
    }
    fprintf(stderr,"cmd_inst->acdb_dev_id %0x\n", use_case->acdb_dev_id);
    tunnel_cmd->cal_type=cmd_inst->cal_type;
    tunnel_cmd->module_id=cmd_inst->module_id;
    tunnel_cmd->instance_id=cmd_inst->inst_id;
    tunnel_cmd->reserved=cmd_inst->reserved;
    tunnel_cmd->param_id=cmd_inst->param_id;
    tunnel_cmd->topo_id=use_case->topo_id;//audcalparam_get_topoid(h, tunnel_cmd);
    if (tunnel_cmd->topo_id==0)
        return AUDCALPARAM_CMDINSTERR;
    fprintf(stderr,"cmd_inst->topo_id %0x\n", cmd_inst->topo_id);
    ALOGD("CMD Instance found name = %s, usecase=%s  return",cmd_inst->inst_name, cmd_inst->use_case_name);

    return AUDCALPARAM_OK;
}

audcalparam_err_t audcalparam_cmd_module_param(audcalparam_session_t *h,
                                                char *cmd_instance_name,
                                                audcalparam_cmd_dir_t dir,
                                                uint8_t *pbuf,
                                                uint32_t *pbuf_len,
                                                audcalparam_cmd_module_param_cfg_t *cfg)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;

    audcalparam_cmd_tunnel_cfg_t tunnel_cmd;// on the stack!

    r=audcalparam_check_and_tunnel_cmd_build(h,p_ses->cmd_set->cmd_module_param,cmd_instance_name, dir, &cfg->base_cfg, &tunnel_cmd);
    if (r)
        goto exit;
    if (cfg->module_id!=0 && tunnel_cmd.module_id==0)
        tunnel_cmd.module_id=cfg->module_id;
    if (cfg->param_id && tunnel_cmd.param_id==0)
        tunnel_cmd.param_id=cfg->param_id;
    //optionally: check value and cfg

    // get/send command
    if (dir==AUDCALPARAM_GET){
        r=audcalparam_serv_con_param_get(p_ses->con_hdl,&tunnel_cmd,pbuf,pbuf_len, (int8_t)1);
    }
    else{
        r=audcalparam_serv_con_param_set(p_ses->con_hdl,&tunnel_cmd,pbuf,pbuf_len);

    }

exit:
    return r;
}

audcalparam_err_t audcalparam_cmd_bmt(audcalparam_session_t* h,
                                        char* cmd_instance_name,
                                        audcalparam_cmd_dir_t dir,
                                        audcalparam_cmd_bmt_t* value,
                                        audcalparam_cmd_base_cfg_t* cfg)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;
    //fields in parameter
    // int16_t bass_step; ///< bass setting - range [-step .. step], step provided in bmt_engine_config_t
    // int16_t mid_step; ///< mid setting
    // int16_t treble_step; ///< treble setting
    // int16_t reserved; ///< align to 32 bit
#define BASS_STEP_BYTE_BUF_POS 0
#define MID_STEP_BYTE_BUF_POS (BASS_STEP_BYTE_BUF_POS+2)
#define TREBLE_STEP_BYTE_BUF_POS (MID_STEP_BYTE_BUF_POS+2)
#define RESERVED_BUTE_BUF_POS (TREBLE_STEP_BYTE_BUF_POS+2)
#undef BYTE_BUF_BYTE_HEADROOM
#define BYTE_BUF_BYTE_HEADROOM 20
#undef CMD_BYTE_BUF_LEN
#define CMD_BYTE_BUF_LEN (RESERVED_BUTE_BUF_POS+2+BYTE_BUF_BYTE_HEADROOM)
    uint8_t byte_buf[CMD_BYTE_BUF_LEN]={0};
    uint32_t buf_len=sizeof(byte_buf);
    uint8_t *pbufOut=byte_buf;

    if (value==NULL)
        return AUDCALPARAM_CMDCFGERR;

    audcalparam_cmd_tunnel_cfg_t tunnel_cmd;// on the stack!

    r=audcalparam_check_and_tunnel_cmd_build(h,p_ses->cmd_set->cmd_bmt,cmd_instance_name, dir, cfg, &tunnel_cmd);
    if (r)
        goto exit;

    //optionally: check value and cfg

    // send command
    // GET & mask
    r=audcalparam_serv_con_param_get(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len, (int8_t)1);//get buf parsing
    // parse value GET
    if (dir==AUDCALPARAM_GET){
        if (value->filter_mask&0x1)
            value->bass=(int32_t)(*(int16_t*)&pbufOut[BASS_STEP_BYTE_BUF_POS]);
        if (value->filter_mask&0x2)
            value->mid=(int32_t)(*(int16_t*)&pbufOut[MID_STEP_BYTE_BUF_POS]);
        if (value->filter_mask&0x4)
            value->treble=(int32_t)(*(int16_t*)&pbufOut[TREBLE_STEP_BYTE_BUF_POS]);
    }else{
        // SET sequence: get, mask, set
        // parse value SET
        if (value->filter_mask&0x1)
            *(uint16_t*)&pbufOut[BASS_STEP_BYTE_BUF_POS]=(uint16_t)value->bass;
        if (value->filter_mask&0x2)
            *(uint16_t*)&pbufOut[MID_STEP_BYTE_BUF_POS]=(uint16_t)value->mid;
        if (value->filter_mask&0x4)
            *(uint16_t*)&pbufOut[TREBLE_STEP_BYTE_BUF_POS]=(uint16_t)value->treble;
        buf_len = RESERVED_BUTE_BUF_POS+2;
        r=audcalparam_serv_con_param_set(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len);

    }

exit:
    return r;
}

audcalparam_err_t audcalparam_cmd_delay(audcalparam_session_t* h,
                                        char* cmd_instance_name,
                                        audcalparam_cmd_dir_t dir,
                                        audcalparam_cmd_delay_t* value,
                                        audcalparam_cmd_base_cfg_t* cfg)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;

    uint32_t buf_len;
    uint8_t *pbufOut;
    uint32_t num_cfg=0;
    uint32_t i,n;
    uint32_t *pval, *pmsb, *plsb;
    if (value==NULL || /*value->num_el!=AUDCALPARAM_CMD_DELAY_EL_NUM_MAX ||*/ value->value==NULL){
        ALOGE("Not enough memory provided. Expected %d elements in the structure!",AUDCALPARAM_CMD_DELAY_EL_NUM_MAX);
        return AUDCALPARAM_CMDCFGERR;
    }

    //allocate memory
    buf_len=3*sizeof(value->value[0])*AUDCALPARAM_CMD_DELAY_EL_NUM_MAX+sizeof(num_cfg)+5*sizeof(uint32_t);//header is included
    pbufOut=(uint8_t*)malloc(buf_len);
    if (pbufOut == NULL)
        return AUDCALPARAM_CMDALLOCERR;

    audcalparam_cmd_tunnel_cfg_t tunnel_cmd;// on the stack!
    r=audcalparam_check_and_tunnel_cmd_build(h,p_ses->cmd_set->cmd_delay,cmd_instance_name, dir, cfg, &tunnel_cmd);

    if (r)
        goto exit;

    //optionally: check value and cfg

    // send command
#define CMD_DELAY_NUM_CFG_POS 0
#define CMD_DELAY_NUM_CFG_LEN (sizeof(int32_t))
#define CMD_DELAY_MSK_LSB_POS (CMD_DELAY_NUM_CFG_POS+CMD_DELAY_NUM_CFG_LEN)
#define CMD_DELAY_MSK_MSB_POS (CMD_DELAY_MSK_LSB_POS+sizeof(uint32_t))
#define CMD_DELAY_DELAY_POS (CMD_DELAY_MSK_MSB_POS+sizeof(uint32_t))
    plsb=(uint32_t*)&pbufOut[CMD_DELAY_MSK_LSB_POS];
    pmsb=(uint32_t*)&pbufOut[CMD_DELAY_MSK_MSB_POS];
    pval=(uint32_t*)&pbufOut[CMD_DELAY_DELAY_POS];
    if (dir==AUDCALPARAM_GET){
        if (value->num_el<AUDCALPARAM_CMD_DELAY_EL_NUM_MAX){
            ALOGE("Not enough memory provided. Expected %d elements in the structure!",AUDCALPARAM_CMD_DELAY_EL_NUM_MAX);
            r= AUDCALPARAM_CMDCFGERR;
            goto exit;
        }
        r=audcalparam_serv_con_param_get(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len, (uint8_t)1); //get buf parsing currently
        if (r==AUDCALPARAM_OK){
            num_cfg=*(uint32_t*)&pbufOut[CMD_DELAY_NUM_CFG_POS];
            value->num_el=num_cfg;
            if (((num_cfg*3+1)*sizeof(uint32_t))!=buf_len){
                ALOGE("Unknown return format: too many num_cfg (%d) !",num_cfg);
                return AUDCALPARAM_CMDPARAMERR;
            }
            for (i=0;i<num_cfg;i++){
                value->value[i]=pval[3*i];
                value->mask_msb[i]=pmsb[3*i];
                value->mask_lsb[i]=plsb[3*i];
#ifdef AUDCALPARAM_DBG
                ALOGD("cfg=%d, lsb=%d, msb=%d, delay=%d",i,plsb[i*3], pmsb[i*3],pval[3*i]);
#endif
            }
        }
    } else if (dir==AUDCALPARAM_SET){
        if (value->num_el>AUDCALPARAM_CMD_DELAY_EL_NUM_MAX){
            ALOGE("Not enough memory provided. Expected %d elements in the structure!",AUDCALPARAM_CMD_DELAY_EL_NUM_MAX);
            r= AUDCALPARAM_CMDCFGERR;
            goto exit;
        }
        num_cfg=value->num_el;
        *(uint32_t*)&pbufOut[CMD_DELAY_NUM_CFG_POS]=num_cfg;
        for (i=0;i<num_cfg;i++){
            plsb[3*i]=value->mask_lsb[i];
            pmsb[3*i]=value->mask_msb[i];
            pval[3*i]=value->value[i];
        }
        *(uint32_t*)&pbufOut[CMD_DELAY_NUM_CFG_POS]=num_cfg;
        buf_len=3*num_cfg*sizeof(value->value[0])+sizeof(num_cfg);
#ifdef AUDCALPARAM_BUF_DBG
        for (i=0;i<buf_len;i++){
            ALOGD("%d",pbufOut[i]);
        }
#endif
        r=audcalparam_serv_con_param_set(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len);
    }

exit:
    free(pbufOut);
    return r;
}


audcalparam_err_t audcalparam_cmd_fnb(audcalparam_session_t *h,
                                        char* cmd_instance_name,
                                        audcalparam_cmd_dir_t dir,
                                        audcalparam_cmd_fnb_t* value,
                                        audcalparam_cmd_base_cfg_t* cfg)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;

    //fields in parameter
    // uint16_t effective;
    // int16_t balance_step;
    // int16_t fading_step;
    // uint16_t hard_change;
#define EFFECTIVE_FIELD_BYTE_BUF_POS 0
#define BALANCE_STEP_BYTE_BUF_POS (EFFECTIVE_FIELD_BYTE_BUF_POS+2)
#define FADING_STEP_BYTE_BUF_POS (BALANCE_STEP_BYTE_BUF_POS+2)
#define HARD_CHANGE_BYTE_BUF_POS (FADING_STEP_BYTE_BUF_POS+2)
#undef BYTE_BUF_BYTE_HEADROOM
#define BYTE_BUF_BYTE_HEADROOM 20
#undef CMD_BYTE_BUF_LEN
#define CMD_BYTE_BUF_LEN (HARD_CHANGE_BYTE_BUF_POS+2+BYTE_BUF_BYTE_HEADROOM)
    uint8_t byte_buf[CMD_BYTE_BUF_LEN]={0};
    uint32_t buf_len=sizeof(byte_buf);
    uint8_t *pbufOut=byte_buf;

    if (value==NULL)
        return AUDCALPARAM_CMDCFGERR;

    audcalparam_cmd_tunnel_cfg_t tunnel_cmd;// on the stack!

    r=audcalparam_check_and_tunnel_cmd_build(h,p_ses->cmd_set->cmd_fnb,cmd_instance_name, dir, cfg, &tunnel_cmd);
    if (r)
        goto exit;

    //optionally: check value and cfg

    // send command
    if (dir==AUDCALPARAM_GET){
        r=audcalparam_serv_con_param_get(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len, (int8_t)1);//get buf parsing
        // parse value GET
        value->filter_mask=(uint32_t)(*(uint16_t*)&pbufOut[EFFECTIVE_FIELD_BYTE_BUF_POS]);
        value->balance=(int32_t)(*(int16_t*)&pbufOut[BALANCE_STEP_BYTE_BUF_POS]);
        value->fade=(int32_t)(*(int16_t*)&pbufOut[FADING_STEP_BYTE_BUF_POS]);
    }
    else{
        // parse value SET
        *(uint16_t*)&pbufOut[EFFECTIVE_FIELD_BYTE_BUF_POS]=(uint16_t)value->filter_mask;
        *(uint16_t*)&pbufOut[BALANCE_STEP_BYTE_BUF_POS]=(uint16_t)value->balance;
        *(uint16_t*)&pbufOut[FADING_STEP_BYTE_BUF_POS]=(uint16_t)value->fade;
        *(uint16_t*)&pbufOut[HARD_CHANGE_BYTE_BUF_POS]=(uint16_t)0;
        buf_len = HARD_CHANGE_BYTE_BUF_POS+2;
        r=audcalparam_serv_con_param_set(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len);
    }

exit:
    return r;
}


audcalparam_err_t audcalparam_cmd_mute(audcalparam_session_t* h,
                                        char* cmd_instance_name,
                                        audcalparam_cmd_dir_t dir,
                                        audcalparam_cmd_mute_t* value,
                                        audcalparam_cmd_base_cfg_t *cfg)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;

    uint8_t byte_buf[288]={0};//long enough buffer for 6+1+64 integers
    uint32_t buf_len=sizeof(byte_buf);
    uint8_t *pbufOut=byte_buf;

    uint32_t num_ch=0;
    uint32_t i=0,n=0;
    uint32_t *pval;

    if (value==NULL || value->num_el==0 || value->value==NULL || value->type==NULL)
        return AUDCALPARAM_CMDCFGERR;

    audcalparam_cmd_tunnel_cfg_t tunnel_cmd;// on the stack!
    r=audcalparam_check_and_tunnel_cmd_build(h,p_ses->cmd_set->cmd_mute,cmd_instance_name, dir, cfg, &tunnel_cmd);

    if (r)
        goto exit;

    //optionally: check value and cfg
    // send command
#define CMD_MUTE_NUM_CH_POS 0
#define CMD_MUTE_NUM_CH_LEN (sizeof(int32_t))
#define CMD_MUTE_MUTE_PAIR_CH_TYPE_POS (CMD_MUTE_NUM_CH_POS+CMD_MUTE_NUM_CH_LEN)
#define CMD_MUTE_MUTE_PAIR_MUTE_POS (CMD_MUTE_MUTE_PAIR_CH_TYPE_POS+sizeof(uint32_t))
#define CMD_MUTE_MUTE_PAIR_LEN (2*sizeof(int32_t))
    pval=(uint32_t*)&pbufOut[CMD_MUTE_MUTE_PAIR_MUTE_POS];
    uint8_t* ptype=&pbufOut[CMD_MUTE_MUTE_PAIR_CH_TYPE_POS];
    if (dir==AUDCALPARAM_GET){
        if (value->num_el<AUDCALPARAM_CMD_MUTE_EL_NUM_MAX){
            ALOGE("Not enough memory provided. Expected %d elements in the structure!",AUDCALPARAM_CMD_MUTE_EL_NUM_MAX);
            return AUDCALPARAM_CMDCFGERR;
        }
        r=audcalparam_serv_con_param_get(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len, (uint8_t)1); //get buf parsing currently
        if (r==AUDCALPARAM_OK){
            num_ch=*(uint32_t*)&pbufOut[CMD_MUTE_NUM_CH_POS];
            if (((num_ch*2+1)*sizeof(uint32_t))>buf_len){
                ALOGE("Unknown return format: too many num_ch (%d) with buf_len=%d !",num_ch,buf_len);
                return AUDCALPARAM_CMDPARAMERR;
            }
            value->num_el=num_ch;
            for (i=0;i<num_ch;i++){
                value->value[i]=pval[2*i];
                value->type[i]=(uint32_t)ptype[i*8];
            }

        }
    } else  if (dir==AUDCALPARAM_SET){
        num_ch=value->num_el;
        if (num_ch>AUDCALPARAM_CMD_MUTE_EL_NUM_MAX){
            ALOGE("Too much number of elements requested. Expected %d elements in the structure!",AUDCALPARAM_CMD_MUTE_EL_NUM_MAX);
            return AUDCALPARAM_CMDCFGERR;
        }
        for (i=0;i<num_ch;i++){
            pval[2*i]=value->value[i];
            ptype[i*8]=(uint8_t)value->type[i];
        }
        buf_len=(num_ch*2+1)*sizeof(uint32_t);
        pbufOut[CMD_MUTE_NUM_CH_POS]=num_ch;
#ifdef AUDCALPARAM_BUF_DBG
        for (i=0;i<buf_len;i++){
            ALOGD("%d",pbufOut[i]);
        }
#endif
        r=audcalparam_serv_con_param_set(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len);

    }

exit:
    return r;
}

audcalparam_err_t audcalparam_cmd_volume_idx(audcalparam_session_t* h,
                                            char* cmd_instance_name,
                                            audcalparam_cmd_dir_t dir,
                                            audcalparam_cmd_volume_idx_t* value,
                                            audcalparam_cmd_base_cfg_t *cfg)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;

    uint8_t byte_buf[64]={0};//long enough buffer for 6+1+32 integers
    uint32_t buf_len=sizeof(byte_buf);
    uint8_t *pbufOut=byte_buf;

    if (value==NULL || value->num_el!=1 || value->value==NULL)
        return AUDCALPARAM_CMDCFGERR;
    if (cfg==NULL || cfg->cache!=0)
        return AUDCALPARAM_CMDCFGERR;
    audcalparam_command_instance_t *cmd_inst=NULL;
    audcalparam_cmd_tunnel_cfg_t tunnel_cmd;// on the stack!
    r=audcalparam_check_and_tunnel_cmd_build(h,p_ses->cmd_set->cmd_volume_idx,cmd_instance_name, dir, cfg, &tunnel_cmd);
    ALOGD("%s: 000 r %d",__func__, r);

    if (r)
        goto exit;
    ALOGD("%s: 111",__func__);

    //optionally: check value and cfg
    // check if topology is up and running
    if (tunnel_cmd.topo_id==0)
        return AUDCALPARAM_CMDUSECASEERR;
    cmd_inst=audcalparam_cmd_inst_get_byname(p_ses->cmd_set->cmd_volume_idx,cmd_instance_name);

    ALOGD("%s: 222",__func__);

    if (dir==AUDCALPARAM_GET){
        r=audcalparam_serv_con_param_get(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len, (uint8_t)1);
        if (!r && cmd_inst!=NULL && cmd_inst->parambuf!=NULL ){
            uint32_t *pvol=(uint32_t *)cmd_inst->parambuf;
            uint32_t i;
            // search for first index for which pvol[i]<=*(uint32_t*)pbufOut, pvol[0]==max
            // Assumtion: array in ascending order (if not TODO)
            for (i=30; i>0; i--){
                if (pvol[i]<=*(uint32_t*)pbufOut) {
                    //return index
                    value->value[0]=i;
                    goto exit;
                } else {
                    continue;
                }

            }
        }
    }
    else if (dir==AUDCALPARAM_SET){
        // SET
        ALOGD("%s: 333",__func__);
        uint32_t idx=value->value[0];
        if (cmd_inst!=NULL && cmd_inst->parambuf!=NULL){
            if (idx >= 31)
                idx = 30;
            uint32_t *pvol=(uint32_t *)cmd_inst->parambuf;
            *(uint32_t*)pbufOut=pvol[idx];
#ifdef AUDCALPARAM_DBG
            ALOGD("%s:Setting idx=%d, gain=%d",__func__,idx,pvol[idx]);
#endif
        }
        else
            return AUDCALPARAM_CMDCFGERR;
        buf_len=sizeof(uint32_t);

        ALOGD("%s: 444",__func__);

        r=audcalparam_serv_con_param_set(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len);

        ALOGD("%s: 555 r %d",__func__, r);

    }

exit:
    return r;
}

audcalparam_err_t audcalparam_cmd_gain(audcalparam_session_t* h,
                                        char* cmd_instance_name,
                                        audcalparam_cmd_dir_t dir,
                                        audcalparam_cmd_gain_t* value,
                                        audcalparam_cmd_base_cfg_t *cfg)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r;

    uint8_t byte_buf[288]={0};//long enough buffer for 6+1+64 integers
    uint32_t buf_len=sizeof(byte_buf);
    uint8_t *pbufOut=byte_buf;

    uint32_t num_ch=0,num_ch_eff=0;
    uint32_t i=0,n=0;
    uint32_t *pval;
    if (value==NULL || value->num_el==0 || value->value==NULL)
        return AUDCALPARAM_CMDCFGERR;

    audcalparam_cmd_tunnel_cfg_t tunnel_cmd;// on the stack!
    r=audcalparam_check_and_tunnel_cmd_build(h,p_ses->cmd_set->cmd_gain,cmd_instance_name, dir, cfg, &tunnel_cmd);

    if (r)
        goto exit;

    //optionally: check value and cfg

    // send command
#define CMD_GAIN_NUM_CH_POS 0
#define CMD_GAIN_NUM_CH_LEN (sizeof(int32_t))
#define CMD_GAIN_GAIN_PAIR_CH_TYPE_POS (CMD_GAIN_NUM_CH_POS+CMD_GAIN_NUM_CH_LEN)
#define CMD_GAIN_GAIN_PAIR_GAIN_POS (CMD_GAIN_GAIN_PAIR_CH_TYPE_POS+sizeof(uint32_t))
#define CMD_GAIN_GAIN_PAIR_LEN (2*sizeof(int32_t))
    pval=(uint32_t*)&pbufOut[CMD_GAIN_GAIN_PAIR_GAIN_POS];
    uint8_t* ptype=&pbufOut[CMD_GAIN_GAIN_PAIR_CH_TYPE_POS];
    if (dir==AUDCALPARAM_GET){
        if (value->num_el<AUDCALPARAM_CMD_GAIN_EL_NUM_MAX){
            ALOGE("Not enough memory provided. Expected %d elements in the structure!",AUDCALPARAM_CMD_GAIN_EL_NUM_MAX);
            return AUDCALPARAM_CMDCFGERR;
        }
        r=audcalparam_serv_con_param_get(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len, (uint8_t)1); //get buf parsing currently
        if (r==AUDCALPARAM_OK){
            num_ch=*(uint32_t*)&pbufOut[CMD_GAIN_NUM_CH_POS];
            if (((num_ch*2+1)*sizeof(uint32_t))>buf_len){
                ALOGE("Unknown return format: too many num_ch (%d)!",num_ch);
                return AUDCALPARAM_CMDPARAMERR;
            }
            value->num_el=num_ch;
            for (i=0;i<num_ch;i++){
                value->value[i]=pval[2*i];
                value->type[i]=(uint32_t)ptype[i*8];
#ifdef AUDCALPARAM_DBG
                ALOGD("ch=%d, type=%d, gain=0x%x",i,ptype[i*8],pval[2*i]);
#endif
            }

        }
    } else  if (dir==AUDCALPARAM_SET){
        num_ch=value->num_el;
        if (num_ch>AUDCALPARAM_CMD_GAIN_EL_NUM_MAX){
            ALOGE("Too much number of elements requested. Expected %d elements in the structure!",AUDCALPARAM_CMD_GAIN_EL_NUM_MAX);
            return AUDCALPARAM_CMDCFGERR;
        }
#ifdef AUDCALPARAM_DBG
        ALOGD("Setting first %d channels from %",value->num_el,num_ch);
#endif
        for (i=0;i<num_ch;i++){
            pval[2*i]=value->value[i];
            ptype[i*8]=(uint8_t)value->type[i];
        }
        buf_len=(num_ch*2+1)*sizeof(uint32_t);
        pbufOut[CMD_GAIN_NUM_CH_POS]=num_ch;
        r=audcalparam_serv_con_param_set(p_ses->con_hdl,&tunnel_cmd,pbufOut,&buf_len);

    }

exit:
    return r;
}


audcalparam_err_t audcalparam_util_parser_init(audcalparam_session_t **h, char* cfg_file_name)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_err_t r=AUDCALPARAM_OK;
    audcalparam_api_session_t * p_ses=NULL;

    p_ses = (audcalparam_api_session_t *)calloc(1, sizeof(audcalparam_api_session_t));

    if  (p_ses==NULL){
        ALOGE("Error while ses init!");
        return AUDCALPARAM_SESINITERR;
    }

    p_ses->use_cases=NULL;
    p_ses->cmd_set=NULL;
    p_ses->con_hdl=NULL;

    if ((p_ses->cmd_set = audcalparam_cmd_set_alloc())==NULL){
        ALOGE("Error while cmd set alloc!");
        r=AUDCALPARAM_CMDALLOCERR;
        goto exit;
    }
    if ((p_ses->use_cases = audcalparam_usecase_list_alloc())==NULL){
        ALOGE("Error while usecase list alloc!");
        r=AUDCALPARAM_UCALLOCERR;
        goto exit;
    }

    //parse provided cfg file
    if (cfg_file_name[0] != '\0'){
        if ((r=audcalparam_cfg_parse((audcalparam_session_t *)p_ses, cfg_file_name))!=AUDCALPARAM_OK)
            goto exit;
    } else {
        //TODO: add defaults
        r=AUDCALPARAM_CMDCFGFILEERR;
        goto exit;
    }

exit:
    //init session handle to allocated structures
    *h=(audcalparam_session_t *)p_ses;

    return r;

}

audcalparam_err_t audcalparam_util_parser_deinit(audcalparam_session_t *h)
{
#ifdef AUDCALPARAM_DBG
    ALOGD("%s:",__func__);
#endif
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_err_t r=AUDCALPARAM_OK;

    if (p_ses==NULL)
        return AUDCALPARAM_HDLERR;

    if ((r=audcalparam_cmd_set_free(p_ses->cmd_set))!=AUDCALPARAM_OK){
        ALOGE("Cmd-set free error %d", r);
        r=AUDCALPARAM_HDLERR;
    }

    if ((r=audcalparam_usecase_list_free(p_ses->use_cases))!=AUDCALPARAM_OK){
        ALOGE("Usecase free error=%d", r);
        r|=AUDCALPARAM_HDLERR;
    }

    free(p_ses);

    return r;

}

audcalparam_err_t  audcalparam_util_parser_get_caldata_for_cmd_inst(audcalparam_session_t *h, char* cmd_name, int idx, audcalparam_util_parser_caldata_t *caldata)
{
     if (h==NULL || cmd_name == NULL || caldata == NULL)
        return AUDCALPARAM_SESINITERR;
    audcalparam_api_session_t *p_ses=(audcalparam_api_session_t *)h;
    audcalparam_command_instance_t *cmd_inst=NULL;
    audcalparam_command_t *cmd=NULL;
    int32_t i,found=0;

    if (p_ses->cmd_set == NULL)
        return AUDCALPARAM_CMDTYPERR;

    if (strcmp(cmd_name, "BMT")==0){
        cmd = p_ses->cmd_set->cmd_bmt;
    } else if (strcmp(cmd_name, "FNB")==0){
        cmd = p_ses->cmd_set->cmd_fnb;
    } else if (strcmp(cmd_name, "Delay")==0){
        cmd = p_ses->cmd_set->cmd_delay;
    } else if (strcmp(cmd_name, "Gain")==0){
        cmd = p_ses->cmd_set->cmd_gain;
    } else if (strcmp(cmd_name, "Volume_idx")==0){
        cmd = p_ses->cmd_set->cmd_volume_idx;
    } else if (strcmp(cmd_name, "Mute")==0){
        cmd = p_ses->cmd_set->cmd_mute;
    } else if (strcmp(cmd_name, "Module_param")==0){
        cmd = p_ses->cmd_set->cmd_module_param;
    } else if (strcmp(cmd_name, "AVC")==0){
        cmd = p_ses->cmd_set->cmd_avc;
    } else if (strcmp(cmd_name, "SUMX")==0){
        cmd = p_ses->cmd_set->cmd_sumx;
    } else if (strcmp(cmd_name, "SYNTH")==0){
        cmd = p_ses->cmd_set->cmd_synth;
    }
    if (cmd==NULL || cmd->command_instance_list==NULL)
        return AUDCALPARAM_CMDTYPERR;

    cmd_inst =  cmd->command_instance_list;
    for (i=0;i<cmd->inst_num;i++){
        if (i==idx){
             found=1;
             break;//found!
        }
        cmd_inst=cmd_inst->nxt_command_instance;
    }
    if (!found)
        return AUDCALPARAM_CMDINSTERR;

    audcalparam_usecase_t *usecase=audcalparam_usecase_get_byname(p_ses->use_cases, cmd_inst->use_case_name);

    if (usecase==NULL)
        return AUDCALPARAM_CMDUSECASEERR;

    caldata->acdb_dev_id = usecase->acdb_dev_id;
    caldata->app_type = usecase->app_type;
    caldata->topo_id = usecase->topo_id;
    caldata->cal_type = cmd_inst->cal_type;
    caldata->module_id = cmd_inst->module_id;
    caldata->instance_id = cmd_inst->inst_id;
    caldata->reserved = cmd_inst->reserved;
    caldata->param_id = cmd_inst->param_id;

    return AUDCALPARAM_OK;
}
