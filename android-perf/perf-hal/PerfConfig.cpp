/******************************************************************************
  @file    PerfConfig.cpp
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define PERF_CONFIG_STORE_ROOT "PerfConfigsStore"
#define PERF_CONFIG_STORE_CHILD "PerfConfigs"
#define PERF_CONFIG_STORE_PROP "Prop"
#define PERF_CONFIG_STORE_NAME "Name"
#define PERF_CONFIG_STORE_VALUE "Value"
#define PERF_CONFIG_STORE_TARGET "Target"
#define PERF_CONFIG_STORE_TARGET_VARIANT "Variant"
#define PERF_CONFIG_STORE_KERNEL "Kernel"
#define PERF_CONFIG_STORE_RESOLUTION "Resolution"
#define PERF_CONFIG_STORE_RAM "Ram"
#define PERF_CONFIG_STORE_XML "vendor/etc/perf/perfconfigstore.xml"

#include "PerfConfig.h"
#include <unistd.h>
#include <cutils/properties.h>
#include "MpctlUtils.h"

#define MAP_RES_TYPE_VAL_1080p "1080p"
#define MAP_RES_TYPE_VAL_2560 "2560"
#define MAP_RES_TYPE_VAL_720p "720p"
#define MAP_RES_TYPE_VAL_HD_PLUS "HD+"
using namespace std;

PerfConfigDataStore PerfConfigDataStore::mPerfStorage;

PerfConfigDataStore::PerfConfigDataStore() {
}

PerfConfigDataStore::~PerfConfigDataStore() {
    //delete mappings
    while (!mPerfConfigStore.empty()) {
        PerfConfigStoreInfo *tmp = mPerfConfigStore.back();
        if (tmp) {
            delete tmp;
        }
        mPerfConfigStore.pop_back();
    }
}

PerfConfigDataStore::PerfConfigStoreInfo::PerfConfigStoreInfo(char *prop_name, char *prop_value) {
    strlcpy(mName, prop_name, sizeof(mName));
    strlcpy(mValue, prop_value, sizeof(mValue));
}

ValueMapResType
PerfConfigDataStore::ConvertToEnumResolutionType(char *res) {
    ValueMapResType ret = MAP_RES_TYPE_ANY;

    if (NULL == res) {
        return ret;
    }

    switch(res[0]) {
    case '1':
        if (!strncmp(res, MAP_RES_TYPE_VAL_1080p, strlen(MAP_RES_TYPE_VAL_1080p))) {
            ret = MAP_RES_TYPE_1080P;
        }
        break;
    case '7':
        if (!strncmp(res, MAP_RES_TYPE_VAL_720p, strlen(MAP_RES_TYPE_VAL_720p))) {
            ret = MAP_RES_TYPE_720P;
        }
        break;
    case 'H':  //Denotes HD_PLUS Resolution ( 720x1440)
        if (!strncmp(res, MAP_RES_TYPE_VAL_HD_PLUS, strlen(MAP_RES_TYPE_VAL_HD_PLUS))) {
            ret = MAP_RES_TYPE_HD_PLUS;
        }
        break;
    case '2':
        if (!strncmp(res, MAP_RES_TYPE_VAL_2560, strlen(MAP_RES_TYPE_VAL_2560))) {
            ret = MAP_RES_TYPE_2560;
        }
    }
    return ret;
}

//perf config store xml CB
void PerfConfigDataStore::PerfConfigStoreCB(xmlNodePtr node, void *) {
    char *mName = NULL, *mValue = NULL, *mTarget = NULL, *mKernel = NULL,
         *mResolution = NULL, *mRam = NULL, *mVariant = NULL;
    PerfConfigDataStore &store = PerfConfigDataStore::getPerfDataStore();
    TargetConfig &tc = TargetConfig::getTargetConfig();
    const char *target_name = tc.getTargetName().c_str();
    const char *kernelVersion = tc.getFullKernelVersion().c_str();
    const char *target_name_variant = tc.getVariant().c_str();
    int tc_resolution = tc.getResolution();
    int tc_ram = tc.getRamSize();
    int res = 0, ram = 0;
    bool valid_target = true, valid_kernel = true, valid_ram = true,
         valid_resolution = true, valid_target_variant = true;
    char trace_prop[PROPERTY_VALUE_MAX];

    /* Enable traces by adding vendor.debug.trace.perf=1 into build.prop */
    if (property_get(PROP_NAME, trace_prop, NULL) > 0) {
        if (trace_prop[0] == '1') {
            perf_debug_output = PERF_SYSTRACE = atoi(trace_prop);
        }
    }
    if(!xmlStrcmp(node->name, BAD_CAST PERF_CONFIG_STORE_PROP)) {
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_NAME)) {
              mName = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_NAME);
        } else if (perf_debug_output) {
              ALOGI("Property not found Name=%s", mName ? mName : "NULL");
        }

        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_VALUE)) {
              mValue = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_VALUE);
        } else if (perf_debug_output) {
              ALOGI("Property not found Name=%s", mValue ? : "NULL");
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET)) {
            mTarget = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET);
            if (mTarget != NULL) {
                if(((strlen(mTarget) == strlen(target_name)) and (!strncmp(target_name, mTarget,strlen(mTarget))))) {
                    valid_target = true;
                } else {
                    valid_target = false;
                }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET_VARIANT)) {
            mVariant = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_TARGET_VARIANT);
            if (mVariant != NULL) {
                    if(((strlen(mVariant) == strlen(target_name_variant)) and (!strncmp(target_name_variant, mVariant,strlen(mVariant))))) {
                        valid_target_variant = true;
                    } else {
                        valid_target_variant = false;
                    }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_KERNEL)) {
            mKernel = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_KERNEL);
            if (mKernel != NULL) {
                if((strlen(mKernel) == strlen(kernelVersion)) and  !strncmp(kernelVersion, mKernel, strlen(mKernel)) ) {
                    valid_kernel = true;
                } else {
                    valid_kernel = false;
                }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_RESOLUTION)) {
            mResolution = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_RESOLUTION);
            if (mResolution != NULL) {
                res = store.ConvertToEnumResolutionType(mResolution);
                if (res == tc_resolution) {
                    valid_resolution = true;
                } else {
                    valid_resolution = false;
                }
            }
        }
        if(xmlHasProp(node, BAD_CAST PERF_CONFIG_STORE_RAM)) {
            mRam = (char *) xmlGetProp(node, BAD_CAST PERF_CONFIG_STORE_RAM);
            if (mRam != NULL) {
                ram = atoi(mRam);
                if (ram == tc_ram) {
                    valid_ram = true;
                } else {
                    valid_ram = false;
                }
            }
        }

       /* Enable this log by adding vendor.debug.trace.perf=1 into build.prop */
        if (perf_debug_output) {
            ALOGI("Identified Name=%s Value=%s for PerfConfigStore in table", mName ? mName : "NULL", mValue ? mValue : "NULL");
        }

        if (mName != NULL and mValue != NULL) {
            if ((valid_kernel and valid_target and valid_target_variant and valid_resolution and valid_ram)) {
                   UpdatePerfConfig(mName, mValue);
            } else if (!mTarget and !mVariant and !mKernel and !mResolution and !mRam) {
                auto tmp = new(std::nothrow) PerfConfigStoreInfo(mName, mValue);
                if (tmp != NULL)
                    store.mPerfConfigStore.push_back(tmp);
            }
        }

        if(mName)
             xmlFree(mName);
        if(mValue)
             xmlFree(mValue);
        if(mTarget)
             xmlFree(mTarget);
        if(mVariant)
            xmlFree(mVariant);
        if(mKernel)
             xmlFree(mKernel);
        if(mResolution)
             xmlFree(mResolution);
        if(mRam)
            xmlFree(mRam);
    }
    return;
}

void PerfConfigDataStore::UpdatePerfConfig(char *name, char *value) {
    PerfConfigDataStore &store = PerfConfigDataStore::getPerfDataStore();
    vector<PerfConfigStoreInfo*>::iterator itbegin = store.mPerfConfigStore.begin();
    vector<PerfConfigStoreInfo*>::iterator itend = store.mPerfConfigStore.end();
    for (vector<PerfConfigStoreInfo*>::iterator it = itbegin; it != itend; ++it){
         if ((*it != NULL) and (strlen(name) == strlen((*it)->mName))
                           and (!strncmp(name, (*it)->mName, strlen((*it)->mName)))) {
              strlcpy((*it)->mValue, value, PROP_VAL_LENGTH);
              return;
         }
    }
    auto tmp = new(std::nothrow) PerfConfigStoreInfo(name, value);
    if (tmp != NULL)
        store.mPerfConfigStore.push_back(tmp);
}

void PerfConfigDataStore::ConfigStoreInit() {
    int idnum;
    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if (NULL == xmlParser) {
        return;
    }
    //perf cofig store
    const string fPerfConfigStore(PERF_CONFIG_STORE_XML);
    const string xmlPerfConfigRoot(PERF_CONFIG_STORE_ROOT);
    const string xmlPerfConfigChild(PERF_CONFIG_STORE_CHILD);

    idnum = xmlParser->Register(xmlPerfConfigRoot, xmlPerfConfigChild, PerfConfigStoreCB, NULL);
    xmlParser->Parse(fPerfConfigStore);
    xmlParser->DeRegister(idnum);

    delete xmlParser;
    return;
}

char* PerfConfigDataStore::GetProperty(const char *name, char *value, const int value_length) {
    if (NULL == name or NULL == value) {
        if (perf_debug_output)
            ALOGI("Couldn't return property, no space");
        return NULL;
    }
    bool prop_found = false;
    vector<PerfConfigStoreInfo*>::iterator itbegin = mPerfConfigStore.begin();
    vector<PerfConfigStoreInfo*>::iterator itend = mPerfConfigStore.end();

    for (vector<PerfConfigStoreInfo*>::iterator it = itbegin; it != itend; ++it){
        if ((*it != NULL) and (strlen(name) == strlen((*it)->mName))
                          and !strncmp(name, (*it)->mName, strlen(name))) {
            strlcpy(value, (*it)->mValue, value_length);
            prop_found = true;
            break;
        }
    }
    if (!prop_found) {
        if (perf_debug_output)
            ALOGI("Property %s not found", name);
        return NULL;
    }
    return value;
}
