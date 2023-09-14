/******************************************************************************
  @file    PerfConfig.h
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef VENDOR_PERF_CONFIG_H
#define VENDOR_PERF_CONFIG_H

#include <vector>
#include <log/log.h>
#include "PerfController.h"
#include "XmlParser.h"
#include "TargetConfig.h"

using namespace std;
class PerfConfigDataStore {
private:
    /*Perf Cofig Store Info class*/
    class PerfConfigStoreInfo {
    public:
        explicit PerfConfigStoreInfo(char *prop_name, char *prop_value);
        char mName[PROP_VAL_LENGTH];
        char mValue[PROP_VAL_LENGTH];
    };

private:
    //perf config store
    vector<PerfConfigStoreInfo*> mPerfConfigStore;
    // Singelton object of this class
    static PerfConfigDataStore mPerfStorage;
public:
    static PerfConfigDataStore &getPerfDataStore() {
        return mPerfStorage;
    }

private:
    //perf config store xml CB
    static void PerfConfigStoreCB(xmlNodePtr node, void *);
    //ctor, copy ctor, assignment overloading
    PerfConfigDataStore();
    PerfConfigDataStore(PerfConfigDataStore const& oh);
    PerfConfigDataStore& operator=(PerfConfigDataStore const& oh);
    //Update property value based on target/kernel
    static void UpdatePerfConfig(char *name, char *value);
public:
    char* GetProperty(const char *name, char *value, const int value_length);
    ValueMapResType ConvertToEnumResolutionType(char *res);
    void ConfigStoreInit();
    ~PerfConfigDataStore();
};
#endif  // VENDOR_PERF_CONFIG_H
