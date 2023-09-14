/******************************************************************************
  @file    Target.h
  @brief   Implementation of target

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __TARGET_SPECIFICS__H_
#define __TARGET_SPECIFICS__H_

#include "ResourceInfo.h"
#include "PerfController.h"
#include "TargetConfig.h"
#include <PerfConfig.h>

#define GPU_FREQ_LVL 16
//TODO Remove the usage for this.
#define MAX_CORES 8

#define NODE_MAX 150

using namespace std;

class PerfDataStore;
/* Structure for defining value maps.
 * */
typedef struct value_map{
    int *map;
    int mapSize;
}value_map;

/* Target class: It contains target
 * specific information. It is a singleton
 * class. On mpctl-server-init a object for this
 * class is initialized with the current target
 * specific information.
 * */
class Target {

private:
    int *mPhysicalClusterMap; /*logical to physical cluster map*/
    int *mPredefineClusterMap; /*pre-defined logical cluster to physical cluster map*/
    int *mLastCoreIndex; /*Maintain end core index for each physical cluster*/

    /*resource supported bitmaps which will let us know which resources are
     *supported on this target. This is a bitmap of 64bits per Major        *
     *resource. Every bit representing the minor resource in the major      *
     *resource.                                                             */
    uint64_t mResourceSupported[MAX_MAJOR_RESOURCES];


    /*Holds the latency to restrict to WFI
     * This value is target specific. Set this to 1 greater than the WFI
     * of the Power Cluster (cluster 0 pm-cpu "wfi"). Look for "wfi". This will
     * ensure the requirement to restrict a vote to WFI for both clusters
     */
    unsigned int mPmQosWfi;

    char mStorageNode[NODE_MAX];

    /* Value map is declared for each resource and defined
     * by a target for specific resources.*/
    struct value_map mValueMap[MAX_MINOR_RESOURCES];


    // Singelton object of this class
    static Target cur_target;


    /* Array for storing available GPU frequencies */
    int mGpuAvailFreq[GPU_FREQ_LVL];
    int mGpuBusAvailFreq[GPU_FREQ_LVL];

    /* Store no. of gpu freq level */
    int mTotalGpuFreq;
    int mTotalGpuBusFreq;

    //boost configs, target configs and mappings store
    PerfDataStore *mPerfDataStore;

    bool mUpDownComb = false;
    bool mGrpUpDownComb = false;
    bool mTraceFlag;
    TargetConfig &mTargetConfig = TargetConfig::getTargetConfig();

    void ExtendedTargetInit();
    void InitializeKPMnodes();

    void mLogicalPerfMapPerfCluster();
    void mLogicalPerfMapPowerCluster();
    void mCalculateCoreIdx();
    void mSetAllResourceSupported();
    void mResetResourceSupported(unsigned short, unsigned short);
    void mInitAllResourceValueMap();
    void mInitGpuAvailableFreq();
    void mInitGpuBusAvailableFreq();

    //init support routines
    void readPmQosWfiValue();
    //ctor, copy ctor, assignment overloading
    Target();
    Target(Target const& oh);
    Target& operator=(Target const& oh);

public:
    ~Target();

    void InitializeTarget();
    void TargetInit();

    int getPhysicalCluster(unsigned short logicalCluster);
    int getFirstCoreOfPerfCluster();
    int getLastCoreIndex(int cluster);
    int getFirstCoreIndex(int cluster);
    int getPhysicalCore(int cluster, unsigned short logicalCore);
    bool isResourceSupported(unsigned short major, unsigned short minor);
    int getClusterForCpu(int cpu, int &startcpu, int &endcpu);
    int getMappedValue(int qindx, int val);
    int getAllCpusBitmask();
    int findNextGpuFreq(int freq);
    int findNextGpuBusFreq(int freq);
    int getBoostConfig(int hintId, int type, int *mapArray, int *timeout, int fps);
    int getConfigPerflockNode(int resId, char *node, bool &supported);

    char* getStorageNode() {
        return mStorageNode;
    }

    unsigned int getMinPmQosLatency() {
        return mPmQosWfi;
    }

    void Dump();

    //Initialize cur_target
    static Target &getCurTarget() {
        return cur_target;
    }

    bool isUpDownCombSupported() {
        return mUpDownComb;
    }
    void setTraceEnabled(bool flag) {
        mTraceFlag = flag;
    }
    bool isTraceEnabled() {
        return mTraceFlag;
    }

    bool isGrpUpDownCombSupported() {
        return mGrpUpDownComb;
    }
};

#endif /*__TARGET_SPECIFICS__H_*/
