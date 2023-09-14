/******************************************************************************
  @file    Boostconfigreader.h
  @brief   Implementation of boostconfigreader

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __BOOSTCONFIG_READER__H_
#define __BOOSTCONFIG_READER__H_

#include <vector>
#include "XmlParser.h"
#include "Target.h"

/* size of logical cluster map is 8, size of logical
 * freq map is 5, so taking max(8) */
#define MAX_MAP_TABLE_SIZE 8

#define TARG_NAME_LEN 32
#define MAX_OPCODE_VALUE_TABLE_SIZE 64

using namespace std;

class PerfDataStore {
public:
    typedef enum {
        MAP_TYPE_UNKNOWN,
        MAP_TYPE_FREQ,
        MAP_TYPE_CLUSTER
    }ValueMapType;

private:
    class ParamsMappingInfo {
    public:
        explicit ParamsMappingInfo(int mtype, const char *tname, int res, int maptable[], int mapsize);

        int mMapType;
        char mTName[TARG_NAME_LEN];
        int mResolution;
        int mMapTable[MAX_MAP_TABLE_SIZE];
        int mMapSize;
    };

    class BoostConfigInfo {
    public:
        explicit BoostConfigInfo(int idnum, int type, bool enable, int timeout, int fps,const char *tname, int res, char *resourcesPtr);
        int mId;
        int mType;
        bool mEnable;
        int mTimeout;
        int mFps;
        char mTName[TARG_NAME_LEN];
        int mResolution;
        int mConfigTable[MAX_OPCODE_VALUE_TABLE_SIZE];
        int mConfigsSize;
    };

    /*Following class is used to Store the information present in resource config XML files.
    mResId is calculated from Major and Minor values, mNodePath for sysfsnode path and mSupported
    flag to indicate whether this resource is supported for that target or not.*/
    class ResourceConfigInfo {
    public:
        explicit ResourceConfigInfo(int idx, char *rsrcPath, bool supported);
        int mResId;
        char mNodePath[NODE_MAX];
        bool mSupported;
    };

private:
    //perf boost configs
    vector<BoostConfigInfo*> mBoostConfigs;

    //power hint
    vector<BoostConfigInfo*> mPowerHint;

    //perf params mappings
    vector<ParamsMappingInfo*> mBoostParamsMappings;

    //perf resource configs
    vector<ResourceConfigInfo*> mResourceConfig;

    //perf sys nodes configs
    vector<ResourceConfigInfo*> mSysNodesConfig;

    // Singelton object of this class
    static PerfDataStore mPerfStore;

    friend void Target::TargetInit();

private:
    //xml open/read/close
    void XmlParserInit();

    //target specific boost params xml CBs
    static void BoostParamsMappingsCB(xmlNodePtr node, void *);

    //target specific boost configurations xml CB
    static void BoostConfigsCB(xmlNodePtr node, void *);

    //perflock resource configurations xml CB
    static void CommonResourcesCB(xmlNodePtr node, void *);
    static void TargetResourcesCB(xmlNodePtr node, void *);

    //common sysnodes config
    static void CommonSysNodesCB(xmlNodePtr node, void *);
    static void TargetSysNodesCB(xmlNodePtr node, void *);

    //support routines
    ValueMapType ConvertToEnumMappingType(char *maptype);
    ValueMapResType ConvertToEnumResolutionType(char *res);
    static int ConvertToIntArray(char *mappings, int mapArray[], int msize);
    static long int ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue);
    static void UpdateResourceConfig(int resId, const char *node, bool supported);
    static void UpdateSysNodeConfig(int resId, const char *node, bool supported);
    static void UpdateConfig(vector<ResourceConfigInfo*> &config, int resId, const char *node, bool supported);

    //ctor, copy ctor, assignment overloading
    PerfDataStore();
    PerfDataStore(PerfDataStore const& oh);
    PerfDataStore& operator=(PerfDataStore const& oh);

public:
    //interface to boost params mappings
    int GetFreqMap(int res, int **maparray, const char *tname);
    int GetClusterMap(int **maparray, const char *tname);

    //interface to boost configs
    int GetBoostConfig(int hintId, int type, int *mapArray, int *timeout,
                      const char *tName = NULL, int res = MAP_RES_TYPE_ANY, int fps = 0);

    int GetResourceConfigNode(int resId, char *node, bool &supported);

    int GetSysNode(const int idx_value, char *node_path);
    //Initialize cur_target
    void Init();
    void TargetResourcesInit();

    static PerfDataStore *getPerfDataStore() {
        return &mPerfStore;
    }

    //dtor
    ~PerfDataStore();
};
#endif /*__BOOSTCONFIG_READER__H_*/
