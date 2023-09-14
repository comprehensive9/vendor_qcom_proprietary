/******************************************************************************
  @file    TargetConfig.h
  @brief   Implementation of target

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PERF_TARGETCONFIG_H__
#define __PERF_TARGETCONFIG_H__

#include <string>
#include "XmlParser.h"
#include <vector>
#include <atomic>

#define NODE_MAX 150
#define MAX_CLUSTER 3
#define MAX_SUPPORTED_SOCIDS 15
#define MAX_CONFIGS_SUPPORTED_PER_PLATFORM 5

using namespace std;

enum {
    CLUSTER_BASED_GOV_INSTANCE, /*both cluster based/core based*/
    SINGLE_GOV_INSTANCE,
    MAX_GOVINSTANCETYPE,
}IntGovInstanceType;

enum {
    INTERACTIVE, /*both cluster based/core based*/
    SCHEDUTIL,
    MAX_CPUFREQGOV,
}CpufreqGov;

typedef enum {
        MAP_RES_TYPE_ANY   = 0,
        MAP_RES_TYPE_720P  = 720,
        MAP_RES_TYPE_HD_PLUS = 721, //Using 721 for HD+ to align with existing implementation
        MAP_RES_TYPE_1080P = 1080,
        MAP_RES_TYPE_2560  = 2560
}ValueMapResType;

#define MEM_1GB (1*1024*1024)
#define MEM_2GB (2*1024*1024)
#define MEM_3GB (3*1024*1024)
#define MEM_4GB (4*1024*1024)
#define MEM_6GB (6*1024*1024)
#define MEM_8GB (8*1024*1024)
#define MEM_10GB (10*1024*1024)
#define MEM_12GB (12*1024*1024)

enum {
    RAM_1GB = 1, /*Ram Size*/
    RAM_2GB,
    RAM_3GB,
    RAM_4GB,
    RAM_6GB = 6,
    RAM_8GB = 8,
    RAM_10GB = 10,
    RAM_12GB = 12/*For 12 and 12+ GB Ram size targets*/
}RamVar;

/* TargetConfig class: It contains target
 * specific information. It is a singleton
 * class. On perf-hal a object for this
 * class is initialized with the current target
 * specific information.
 * */
class TargetConfig {

private:
    class TargetConfigInfo {
    public:
        bool mSyncCore = true, mUpdateTargetInfo = true;
        unsigned short mNumCluster = 0, mTotalNumCores = 0, mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE, mCpufreqGov = INTERACTIVE;
        string mTargetName;
        int mType = 0, mCoreCtlCpu = -1,mMinCoreOnline = 0, mNumSocids = 0, mCalculatedCores = 0, mMinFpsTuning = 0;
        unsigned int mCpumaxfrequency[MAX_CLUSTER];
        int mSupportedSocids[MAX_SUPPORTED_SOCIDS], mCorepercluster[MAX_CLUSTER];
        bool CheckSocID(int SocId);
        bool CheckClusterInfo();
    };

private:
    int mSocID;
    int mRam;
    int mRamKB;
    bool mSyncCore;  /*true- if target is sync core*/
    unsigned short mNumCluster;  /*total number of cluster*/
    unsigned short mTotalNumCores;    /*total number of cores*/
    int *mCorePerCluster; /*number of cores per cluster*/
    int mResolution;
    string mTargetName; /*Store the name of that target*/
    string mVariant; /*Store the Variant of current target (go/32/64 variant)*/
    /* Array for storing cpu max frequency reset value
     *   for each cluster. */
    unsigned int mCpuMaxFreqResetVal[MAX_CLUSTER];
    int mCoreCtlCpu; /*core_ctl is enabled on which physical core*/
    int mMinCoreOnline; /*Minimum number of cores needed to be online*/
    //defines which type of int gov instance target has
    //paths will vary based on this
    unsigned short mGovInstanceType;
    //returns cpufreq governor
    unsigned short mCpufreqGov;
    int mType;
    int mMinFpsTuning; /*Minmum FPS for which system tuning is supported*/

    // Singelton object of this class
    static TargetConfig cur_target;
    //target config object
    vector<TargetConfigInfo*> mTargetConfigs;

    /*
     * kernel base version
     */
    string mFullKernelVersion;
    int mKernelMajorVersion;
    int mKernelMinorVersion;
    //init support routines
    int readSocID();
    int readResolution();
    int readRamSize(void);
    //read Kernel Version of the Target
    void readKernelVersion();
    void readVariant();
    //target specific configc xml CB
    static void TargetConfigsCB(xmlNodePtr node, void *index);
    static int ConvertToIntArray(char *mappings, int mapArray[], int msize);
    static long int ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue);
    void InitTargetConfigXML();
    atomic<bool> mInitCompleted;

    //ctor, copy ctor, assignment overloading
    TargetConfig();
    TargetConfig(TargetConfig const& oh);
    TargetConfig& operator=(TargetConfig const& oh);

public:
    ~TargetConfig();

    void InitializeTargetConfig();
    void TargetConfigInit();

    bool isSyncCore() {
        return mSyncCore;
    }
    unsigned short getNumCluster() {
        return mNumCluster;
    }
    unsigned short getTotalNumCores()  {
        return mTotalNumCores;
    }
    int getCoresInCluster(int cluster);
    int getCpuMaxFreqResetVal(int cluster);

    int getSocID() {
    return mSocID;
    }
    int getResolution() {
        return mResolution;
    }
    int getCoreCtlCpu() {
        return mCoreCtlCpu;
    }
    int getMinCoreOnline() {
        return mMinCoreOnline;
    }
    int getGovInstanceType() {
        return mGovInstanceType;
    }
    int getCpufreqGov() {
        return mCpufreqGov;
    }
    string &getTargetName() {
        return mTargetName;
    }
    string &getVariant() {
        return mVariant;
    }
    int getKernelMinorVersion() {
        return mKernelMinorVersion;
    }
    int getKernelMajorVersion() {
        return mKernelMajorVersion;
    }
    string &getFullKernelVersion() {
        return mFullKernelVersion;
    }
    int getRamSize() {
        return mRam;
    }
    int getRamInKB() {
        return mRamKB;
    }

    int getType() {
        return mType;
    }

    int getMinFpsTuning() {
        return mMinFpsTuning;
    }

    void setInitCompleted(bool flag) {
        mInitCompleted.store(flag);
    }
    bool getInitCompleted() {
        return mInitCompleted.load();
    }
    //Initialize cur_target
    static TargetConfig &getTargetConfig() {
        return cur_target;
    }
    TargetConfigInfo *getTargetConfigInfo(int socId);

    void DumpAll();

};

#endif /*__PERF_TARGETCONFIG_H__*/
