/******************************************************************************
  @file    Target.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <log/log.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <cutils/properties.h>

#include "Request.h"
#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"
#include "BoostConfigReader.h"
#include "XmlParser.h"
#include "ResourceInfo.h"

Target Target::cur_target;

#define PMQOSWFI_LATENCY_NODE "/sys/devices/system/cpu/cpu%d/cpuidle/state0/latency"

/*
 * Definition for various get functions
 * */
int Target::getPhysicalCluster(unsigned short logicalCluster) {
    int cluster = -1;
    // If logicalCluster lies  within the range of the
    // physical clusters, then return the cluster from
    // the physicalCLusterMap.
    if (logicalCluster < mTargetConfig.getNumCluster()) {
        if (mPhysicalClusterMap) {
            cluster = mPhysicalClusterMap[logicalCluster];
        }
    } else if ((logicalCluster >= START_PRE_DEFINE_CLUSTER) &&
               (logicalCluster < MAX_PRE_DEFINE_CLUSTER)) {
        //logical cluster lies in pre-defined cluster range
        if (mPredefineClusterMap) {
            cluster = mPredefineClusterMap[logicalCluster - START_PRE_DEFINE_CLUSTER];
        }
    } else {
        QLOGE("Error: Invalid logical cluster id %d", logicalCluster);
    }

    if (cluster > mTargetConfig.getNumCluster()) {
        cluster = -1;
    }
    return cluster;
}

int Target::getFirstCoreOfPerfCluster() {
    int perfCluster = -1;
    int supportedCore = -1;

    if ((NULL == mPhysicalClusterMap) || (NULL == mLastCoreIndex)) {
        return supportedCore;
    }

    perfCluster = mPhysicalClusterMap[0];
    if ((perfCluster >= 0) && (perfCluster < mTargetConfig.getNumCluster())) {
        if (perfCluster == 0) {
            supportedCore = 0;
        } else {
            supportedCore = mLastCoreIndex[perfCluster-1] + 1;
        }
    }
    return supportedCore;
}

int Target::getPhysicalCore(int cluster, unsigned short logicalCore) {
    int physicalCore = -1;

    if(cluster < 0 || cluster >= mTargetConfig.getNumCluster())
        return physicalCore;

    if (cluster == 0) {
        physicalCore = logicalCore;
    } else {
        if (NULL != mLastCoreIndex) {
            physicalCore = logicalCore + mLastCoreIndex[cluster-1] + 1;
        }
    }
    if ((NULL != mLastCoreIndex) && (physicalCore > mLastCoreIndex[cluster])) {
        physicalCore = -1;
    }

    return physicalCore;
}

int Target::getLastCoreIndex(int cluster) {
    if (mLastCoreIndex) {
        if (cluster >=0 && cluster < mTargetConfig.getNumCluster()) {
            return mLastCoreIndex[cluster];
        } else {
            QLOGE("Error: Invalid cluster id %d", cluster);
        }
    } else {
        QLOGE("Eror: Target not initialized");
    }
    return -1;
}

int Target::getFirstCoreIndex(int cluster) {
    if (mLastCoreIndex) {
        if (cluster == 0) {
            return 0;
        } else if (cluster >=1 && cluster < mTargetConfig.getNumCluster()) {
            return mLastCoreIndex[cluster-1] + 1;
        } else {
            QLOGE("Error: Invalid cluster id %d", cluster);
        }
    } else {
        QLOGE("Eror: Target not initialized");
    }
    return -1;
}

Target::Target() {
    QLOGI("Target constructor");
    mPhysicalClusterMap = NULL;
    mPredefineClusterMap = NULL;
    mLastCoreIndex = NULL;
    memset(mResourceSupported, 0x00, sizeof(mResourceSupported));
    mPmQosWfi = 0;
    memset(mStorageNode, '\0', sizeof(mStorageNode));
    mTotalGpuFreq = 0;
    mTotalGpuBusFreq = 0;
    mTraceFlag = false;
}

Target::~Target() {
    QLOGI("Target destructor");
    if (mPhysicalClusterMap){
        delete [] mPhysicalClusterMap;
        mPhysicalClusterMap = NULL;
    }

    if (mPredefineClusterMap){
        mPredefineClusterMap = NULL;
    }

    if (mLastCoreIndex) {
        delete [] mLastCoreIndex;
        mLastCoreIndex = NULL;
    }
}

/* Create a logical to physical cluster
 * mapping, in which Logical cluster "0"
 * maps to physical cluster "0". Useful
 * for the targets in which cluster0 is
 * the perf cluster.
 * */
void Target::mLogicalPerfMapPerfCluster() {
    int i;
    int num_clusters = mTargetConfig.getNumCluster();
    mPhysicalClusterMap = new(std::nothrow) int[num_clusters];
    if(mPhysicalClusterMap) {
        for(i=0; i < num_clusters; i++) {
            mPhysicalClusterMap[i] = i ;
        }
   } else {
     QLOGE("Error: Could not map Logical perf cluster to perf\n");
   }
}

/* Create a logical to physical cluster mapping
 * in which logical cluster "0" maps to power
 * cluster
 * */
void Target::mLogicalPerfMapPowerCluster() {
    int num_clusters = mTargetConfig.getNumCluster();
    int i, cluster = (num_clusters > MIN_CLUSTERS) ? MIN_CLUSTERS - 1 : num_clusters-1;
    int clustersToMap = (num_clusters > MIN_CLUSTERS) ? MIN_CLUSTERS : num_clusters;
    mPhysicalClusterMap = new(std::nothrow) int[num_clusters];
    if(mPhysicalClusterMap) {
        //This will retain the Big/little mapping similar to 2-cluster
        for(i=0; i < clustersToMap && cluster >= 0; i++,cluster--) {
            mPhysicalClusterMap[i] = cluster;
        }
        //This will keep 1:1 mapping for cluster after 2.
        for(i=MIN_CLUSTERS; i < num_clusters; i++) {
            mPhysicalClusterMap[i] = i ;
        }

    } else {
        QLOGE("Error: Could not map logical perf to power cluster\n");
    }
}

/* Calculate the number/index for last
 * core in this cluster
 * */
void Target::mCalculateCoreIdx() {
    int cumlative_core, i;
    mLastCoreIndex = new(std::nothrow) int[mTargetConfig.getNumCluster()];
    if(mLastCoreIndex && mTargetConfig.getCoresInCluster(0)) {
        cumlative_core = 0;
        for(i =0;i < mTargetConfig.getNumCluster(); i++) {
            mLastCoreIndex[i] = cumlative_core + (mTargetConfig.getCoresInCluster(i) - 1);
            cumlative_core = mLastCoreIndex[i] + 1;
        }
    } else {
        QLOGE("Error: Initializing core index failed\n");
    }
}

/* Resource support functions. */

/* Return the bitmap value for the asked resource.
 * If Resource is supported it will return 1,
 * else 0.
 * */
bool Target::isResourceSupported(unsigned short major, unsigned short minor) {
    uint64_t tmp = 0;
    bool ret = false;

    if (major < MAX_MAJOR_RESOURCES) {
        tmp = mResourceSupported[major];
        ret = ((tmp >> minor) & 0x1);
    }

    return ret;
}

/* This function sets all the minor resources for a
 * major resource to be supported.
 * */
void Target::mSetAllResourceSupported() {
    unsigned int minorIdx,rshIdx;
    for (unsigned int i = 0; i < MAX_MAJOR_RESOURCES; i++) {
        minorIdx = ResourceInfo::GetMinorIndex(i);
        rshIdx = sizeof(uint64_t)*8 - minorIdx;
        mResourceSupported[i] = ((uint64_t)~0) >> rshIdx;
    }
}

/* This function resets the minor rsource in the specified
 * major resource to false (not supported).
 * */
void Target::mResetResourceSupported(unsigned short major, unsigned short minor) {
    unsigned long tmp;
    if (major < MAX_MAJOR_RESOURCES) {
        if (minor < ResourceInfo::GetMinorIndex(major)) {
            mResourceSupported[major] &= ~(1 << minor);
        }
    } /*if (major < MAX_MAJOR_RESOURCES)*/
}

/** This function returns the cluster number to which a
 * given cpu belongs. It also updates input parameters
 * with first cpu and last cpu in that cluster.
 * On error the return value is -1
 */
int Target::getClusterForCpu(int cpu, int &startcpu, int &endcpu) {
    int i, cluster = -1;
    bool success = false;

    startcpu = 0;
    endcpu = -1;

    if ((cpu < 0) || (cpu > mTargetConfig.getTotalNumCores()) || (NULL == mLastCoreIndex)) {
        QLOGE("Invalid cpu number %d. Cannot find cluster.", cpu);
        return -1;
    }

    for (i = 0 ; i < mTargetConfig.getNumCluster() && !success; i++) {
        endcpu = mLastCoreIndex[i];
        if (cpu >= startcpu && cpu <= endcpu) {
            cluster = i;
            success = true;
        }
        else {
            startcpu = mLastCoreIndex[i] + 1;
        }
    }

    if (!success){
        cluster = -1;
        startcpu = -1;
        endcpu = -1;
    }

    return cluster;
}


/* Functions to support value maps. */

/* This function initializes the valueMap for all
 * the resources. The default is "false" and
 * no map is specified.
 *
 * Whichever resource needs a map and has to be
 * specified should be done in specifc target
 * functions.
 * */
void Target::mInitAllResourceValueMap() {
    int i = 0;
    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        mValueMap[i].mapSize = -1;
        mValueMap[i].map = NULL;
    }
}

void Target::readPmQosWfiValue() {
    int fd = -1, i, n;
    char tmp[NODE_MAX];
    int latency = 0, max_latency = 0;
    const int MAX_BUF_SIZE = 16;
    char buf[MAX_BUF_SIZE];

    //read the PmQosWfi latency for all cpus to find the max value.
    for(i = 0; i < mTargetConfig.getTotalNumCores(); i++) {
        snprintf(tmp, NODE_MAX, PMQOSWFI_LATENCY_NODE, i);
        fd = open(tmp, O_RDONLY);
        if (fd != -1) {
            memset(buf, 0x0, sizeof(buf));
            n = read(fd, buf, MAX_BUF_SIZE-1);
            if (n > 0) {
                latency = strtol(&buf[0], NULL, 0);
                max_latency = max(latency, max_latency);
            }
            close(fd);
        } else {
            QLOGW("Unable to read node = %s", tmp);
        }
    }

    if(max_latency == 0) {
        QLOGE("Unable to get mPmQosWfi latency, So initializing to default 0 value");
    } else {
        mPmQosWfi = max_latency + 1;
        QLOGI("mPmQosWfi latency caluculated as = %u", mPmQosWfi);
    }
}
/* This function returns the mapped value for
 * a major resource and minor resource, specified
 * through the index.
 * */
int Target::getMappedValue(int resrcIndex, int val) {
    if ((resrcIndex >= 0) && (resrcIndex < MAX_MINOR_RESOURCES)
           && (resrcIndex != UNSUPPORTED_Q_INDEX)) {
        if(mValueMap[resrcIndex].map != NULL) {
            if (val >= 0 && val < mValueMap[resrcIndex].mapSize) {
                return mValueMap[resrcIndex].map[val];
            } else {
                QLOGE("Value Map not defined for this %d index for the %d value", resrcIndex, val);
            }
        } else {
            QLOGE("Value map not defined for this %d index", resrcIndex);
        }
    } else {
        QLOGE("Error: Cannot find mapped value for the resource with index %d", resrcIndex);
    }
    return -1;
}

/* Calculate bitmask of all CPUs in target.
 * For example, if total number of CPUs is 4,
 * then bitmask will be 0b1111 (or 0xF).
 * */
int Target::getAllCpusBitmask() {
    if(mTargetConfig.getTotalNumCores()) {
        return (1 << mTargetConfig.getTotalNumCores()) - 1;
    } else {
        QLOGE("Error: Initializing total cores failed\n");
        return -1;
    }
}

/* Store all the available GPU freq in an integer array */
void Target::mInitGpuAvailableFreq() {
    char listf[FREQLIST_STR] = "";
    int rc = -1;
    char *cFreqL = NULL, *pcFreqL = NULL;

    FREAD_STR(GPU_AVAILABLE_FREQ, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        QLOGI("Initializing GPU available freq as %s", listf);
        cFreqL = strtok_r(listf, " ", &pcFreqL);
        if (cFreqL) {
            do {
                if(mTotalGpuFreq >= GPU_FREQ_LVL) {
                    QLOGE("Number of frequency is more than the size of the array.Exiting");
                    return;
                }
                mGpuAvailFreq[mTotalGpuFreq++] = atoi(cFreqL);
            } while ((cFreqL = strtok_r(NULL, " ", &pcFreqL)) != NULL);
        }
        sort(mGpuAvailFreq, mGpuAvailFreq + mTotalGpuFreq);
    } else {
        QLOGE("Initialization of GPU available freq failed, as %s not present", GPU_AVAILABLE_FREQ);
    }
}

/* Returns nearest >= input freq level,
   or max freq level if input too high. */
int Target::findNextGpuFreq(int freq) {
    int i = 0;

    for (i = 0; i < mTotalGpuFreq; i++) {
        if (mGpuAvailFreq[i] >= freq) {
            return mGpuAvailFreq[i];
        }
    }

    if ((mTotalGpuFreq != 0) && (i ==  mTotalGpuFreq)) {
        return mGpuAvailFreq[mTotalGpuFreq-1];
    }

    return FAILED;
}

/* Store all the available GPU bus freq in an integer array */
void Target::mInitGpuBusAvailableFreq() {
    char listf[FREQLIST_STR] = "";
    int rc = -1;
    char *cFreqL = NULL, *pcFreqL = NULL;

    FREAD_STR(GPU_BUS_AVAILABLE_FREQ, listf, FREQLIST_STR, rc);
    if (rc > 0) {
        QLOGI("Initializing GPU available freq as %s", listf);
        cFreqL = strtok_r(listf, " ", &pcFreqL);
        if (cFreqL) {
            do {
                if(mTotalGpuBusFreq >= GPU_FREQ_LVL) {
                    QLOGE("Number of frequency is more than the size of the array.Exiting");
                    return;
                }
                mGpuBusAvailFreq[mTotalGpuBusFreq++] = atoi(cFreqL);
            } while ((cFreqL = strtok_r(NULL, " ", &pcFreqL)) != NULL);
        }
        sort(mGpuBusAvailFreq, mGpuBusAvailFreq + mTotalGpuBusFreq);
    } else {
        QLOGE("Initialization of GPU Bus available freq failed, as %s not present", GPU_BUS_AVAILABLE_FREQ);
    }
}

/* Returns nearest >= input freq level,
   or max freq level if input too high. */
int Target::findNextGpuBusFreq(int freq) {
    int i = 0;

    for (i = 0; i < mTotalGpuBusFreq; i++) {
        if (mGpuBusAvailFreq[i] >= freq) {
            return mGpuBusAvailFreq[i];
        }
    }

    if ((mTotalGpuBusFreq != 0) && (i ==  mTotalGpuBusFreq)) {
        return mGpuBusAvailFreq[mTotalGpuBusFreq-1];
    }

    return FAILED;
}
int Target::getBoostConfig(int hintId, int type, int *mapArray, int *timeout, int fps) {
    int size = 0;

    if (NULL == mapArray) {
        return size;
    }

    if (NULL != mPerfDataStore) {
        size = mPerfDataStore->GetBoostConfig(hintId, type, mapArray, timeout,
                                                 mTargetConfig.getTargetName().c_str(), mTargetConfig.getResolution(), fps);
    }
    return size;
}

int Target::getConfigPerflockNode(int resId, char *node, bool &supported) {
    int ret = FAILED;

    if (resId < 0) {
        return 0;
    }

    if (NULL != mPerfDataStore) {
        ret = mPerfDataStore->GetResourceConfigNode(resId, node, supported);
    }
    return ret;
}

void Target::Dump() {
    int i;
    int num_clusters = mTargetConfig.getNumCluster();
    for (i =0; i < num_clusters; i++) {
        if (mPhysicalClusterMap) {
            QLOGI("Logical cluster %d is mapped to physical cluster %d", i, mPhysicalClusterMap[i]);
        }
        if (mLastCoreIndex) {
            QLOGI("End index for physical cluster %d is %d",i, mLastCoreIndex[i]);
        }
    }
    for (i=0; i < MAX_PRE_DEFINE_CLUSTER-START_PRE_DEFINE_CLUSTER; i++) {
        if (mPredefineClusterMap) {
            QLOGI("Logical pre-defined cluster %d is mapped to physical cluster %d",
                  i+START_PRE_DEFINE_CLUSTER, mPredefineClusterMap[i]);
        }
    }
    QLOGI("PmQosWfi_latency %u", mPmQosWfi);

    return;
}
