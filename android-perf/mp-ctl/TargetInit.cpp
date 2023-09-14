/******************************************************************************
  @file    TargetInit.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET-INIT"
#include <cutils/log.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <sys/utsname.h>

#include "PerfController.h"
#include "Request.h"
#include "client.h"
#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"
#include "BoostConfigReader.h"
#include "HintExtHandler.h"

//Default frequency map for value mapping
int msmDefaultFreqMap[FREQ_MAP_MAX] = {
    800,    /*LOWEST_FREQ*/
    1100,   /*LEVEL1_FREQ*/
    1300,   /*LEVEL2_FREQ*/
    1500,   /*LEVEL3_FREQ*/
    1650    /*HIGHEST_FREQ*/
};

//Default pre-defined cluster map .
int msmDefaultPreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,  /*LAUNCH_CLUSTER*/
    1,  /*SCROLL_CLUSTER*/
    1,  /*ANIMATION_CLUSTER*/
};

/* Init Sequence
 * 1. Populate the target resources config
 * 2. Init the resources node
 */

void Target::InitializeTarget() {
    int ret = 0;
    int socid = 0;
    char tmp_s[NODE_MAX];
    int rc = 0;
    int qindx = 0;
    char clk_scaling_s[NODE_MAX];
    int res = 0;
    int kernelMajor = -1, KernelMinor = -1;
    const char* target_name = mTargetConfig.getTargetName().c_str();
    ResourceInfo tmpr;

    QLOGI("Inside InitializeTarget");
    mSetAllResourceSupported();
    mInitAllResourceValueMap();
    mResetResourceSupported(MISC_MAJOR_OPCODE, UNSUPPORTED_OPCODE);
    mInitGpuAvailableFreq();
    mInitGpuBusAvailableFreq();

    // Identifying storage device type. (UFS/EMMC)
    FREAD_STR(STORAGE_UFS_CLK_SCALING_DISABLE, clk_scaling_s, NODE_MAX, rc);
    if (rc > 0) {
        strlcpy(mStorageNode, STORAGE_UFS_CLK_SCALING_DISABLE,
                strlen(STORAGE_UFS_CLK_SCALING_DISABLE)+1);
    } else {
        strlcpy(mStorageNode, STORAGE_EMMC_CLK_SCALING_DISABLE,
                strlen(STORAGE_EMMC_CLK_SCALING_DISABLE)+1);
    }


    rc = 0;
    //populate boost conigs, target configs  & params mapping tables
    mPerfDataStore = PerfDataStore::getPerfDataStore();
    mPerfDataStore->Init();

    /*intialize hint extnsion handler and register actions
     * Note: Number of hint registrations allowed is limited.
     * Modify the MAX_HANDLERS limit in HintExtension as needed
    */
    HintExtHandler &hintExt = HintExtHandler::getInstance();
    hintExt.Reset();
    hintExt.Register(VENDOR_HINT_FIRST_LAUNCH_BOOST, NULL, TaskBoostAction::TaskBoostPostAction);
    hintExt.Register(VENDOR_HINT_FPS_UPDATE, FpsUpdateAction::FpsUpdatePreAction,
                     FpsUpdateAction::FpsUpdatePostAction);
    hintExt.Register(VENDOR_HINT_BOOST_RENDERTHREAD, NULL, TaskBoostAction::TaskBoostPostAction);
    hintExt.Register(VENDOR_HINT_PICARD_GPU_DISPLAY_MODE,
                                DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction, NULL);
    hintExt.Register(VENDOR_HINT_PICARD_GPU_ONLY_MODE,
                                DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction, NULL);
    hintExt.Register(VENDOR_HINT_PICARD_DISPLAY_ONLY_MODE,
                                DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction, NULL);

    //modify the scroll hint only if feature is enabled
    string property = perf_get_prop("ro.vendor.perf.splh", "none").value;
    if (!property.compare("sw") || !property.compare("hw")) {
        hintExt.Register(VENDOR_HINT_SCROLL_BOOST, NULL, CPUFreqAction::CPUFreqPostAction);
    }
    hintExt.Register(VENDOR_HINT_LARGER_COMP_CYCLE, LargeComp::LargeCompPreAction, NULL);
    hintExt.Register(VENDOR_HINT_PASS_PID, StorePID::StorePIDPreAction, NULL);


    /* All the target related information parsed from XML file are initialized in the TargetInit()
    function which remains same for all the targets. For any target specific initializations provided
    an ExtendedTargetInit() function, which works on SocId. */

    TargetInit();
    ExtendedTargetInit();

    /* Init for per target resource file.
     * Moved the call after TargetInit as it need target name
     */
    mPerfDataStore->TargetResourcesInit();

    /* Moved Init KPM Nodes after Target Resource as the values read from config files
     * are needed in KPM nodes init method
     */
    InitializeKPMnodes();

    kernelMajor = mTargetConfig.getKernelMajorVersion();
    KernelMinor = mTargetConfig.getKernelMinorVersion();

    //cluster map from xml
    ret = mPerfDataStore->GetClusterMap(&mPredefineClusterMap, target_name);

    //default cluster map if not available
    if ((NULL == mPredefineClusterMap) || !ret) {
        mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    }

    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();

    mValueMap[qindx].mapSize = mPerfDataStore->GetFreqMap(res, &mValueMap[qindx].map, target_name);

    //default it to a map if no mappings exists
    if ((NULL == mValueMap[qindx].map) || !mValueMap[qindx].mapSize) {
        mValueMap[qindx].mapSize = FREQ_MAP_MAX;
        mValueMap[qindx].map = msmDefaultFreqMap;
    }

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();

    mValueMap[qindx].mapSize = mPerfDataStore->GetFreqMap(res,
                                           &mValueMap[qindx].map, target_name);

    //default it to a map if no mappings exists
    if ((NULL == mValueMap[qindx].map) || !mValueMap[qindx].mapSize) {
        mValueMap[qindx].mapSize = FREQ_MAP_MAX;
        mValueMap[qindx].map = msmDefaultFreqMap;
    }
}

void invoke_wa_libs() {
#define KERNEL_WA_NODE "/sys/module/app_setting/parameters/lib_name"
   int rc;
   const char *wl_libs[] = {
      "libvoH264Dec_v7_OSMP.so",
      "libvossl_OSMP.so",
      "libTango.so"
   };
   int i;

   int len = sizeof (wl_libs) / sizeof (*wl_libs);

   for(i = 0; i < len; i++) {
      FWRITE_STR(KERNEL_WA_NODE, wl_libs[i], strlen(wl_libs[i]), rc);
      QLOGI("Writing to node (%s)  (%s) rc:%d\n", KERNEL_WA_NODE, wl_libs[i], rc);
   }

}

void invoke_tinyu_wa(int socid) {
  static struct {
       char* node;
       char* val;
  }tinyu_wa_detl [] = {
      {"/proc/sys/kernel/sched_lib_mask_check", "0x80"},
      {"/proc/sys/kernel/sched_lib_mask_force", "0xf0"},
      {"/proc/sys/kernel/sched_lib_name", "libunity.so, libfb.so"},
  };
  int rc;
  int len = sizeof (tinyu_wa_detl) / sizeof (tinyu_wa_detl[0]);

  /*for lito,atoll,lagoon,holi,orchid we have only two gold cors, so mask is changed accrodingly*/
  if(socid == 400 || socid == 440 || socid == 407 || socid == 434 ||  socid == 454 || socid == 459 || socid == 476 || socid == 507 || socid == 578) {
       tinyu_wa_detl[1].val = "0xc0";
  }

  for(int i = 0; i < len; i++) {
      FWRITE_STR(tinyu_wa_detl[i].node, tinyu_wa_detl[i].val, strlen(tinyu_wa_detl[i].val), rc);
      QLOGE("Writing to node (%s)  (%s) rc:%d\n", tinyu_wa_detl[i].node, tinyu_wa_detl[i].val, rc);
   }
}

void Target::TargetInit() {

    readPmQosWfiValue();
    mCalculateCoreIdx();
    if (mTargetConfig.getType() == 0) {
        //cluster 0 is big
        mLogicalPerfMapPerfCluster();
    } else {
        //cluster 0 is little
        mLogicalPerfMapPowerCluster();
    }
}

//KPM node initialization
void Target::InitializeKPMnodes() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    PerfDataStore *store = PerfDataStore::getPerfDataStore();
    char kpmNumClustersNode[NODE_MAX];
    char kpmManagedCpusNode[NODE_MAX];
    char kpmCpuMaxFreqNode[NODE_MAX];
    memset(kpmNumClustersNode, 0, sizeof(kpmNumClustersNode));
    memset(kpmManagedCpusNode, 0, sizeof(kpmManagedCpusNode));
    memset(kpmCpuMaxFreqNode, 0, sizeof(kpmCpuMaxFreqNode));

    store->GetSysNode(KPM_NUM_CLUSTERS, kpmNumClustersNode);
    store->GetSysNode(KPM_MANAGED_CPUS, kpmManagedCpusNode);
    store->GetSysNode(KPM_CPU_MAX_FREQ_NODE, kpmCpuMaxFreqNode);

    FREAD_STR(kpmManagedCpusNode, tmp_s, NODE_MAX, rc);
    if (rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
        snprintf(tmp_s, NODE_MAX , "%d", mTargetConfig.getNumCluster());
        FWRITE_STR(kpmNumClustersNode, tmp_s, strlen(tmp_s), rc);

        if(mTargetConfig.getCoresInCluster(0) == -1)
            return;

        //Initializing KPM nodes for each cluster, with their cpu ranges and max possible frequencies.
        for (int i=0, prevcores = 0; i < mTargetConfig.getNumCluster(); i++) {
            snprintf(tmp_s, NODE_MAX , "%d-%d", prevcores, prevcores + mTargetConfig.getCoresInCluster(i)-1);
            FWRITE_STR(kpmManagedCpusNode, tmp_s, strlen(tmp_s), rc);

            rc = update_freq_node(prevcores, prevcores + mTargetConfig.getCoresInCluster(i) - 1 , mTargetConfig.getCpuMaxFreqResetVal(i) , tmp_s, NODE_MAX);
            if (rc >= 0) {
                FWRITE_STR(kpmCpuMaxFreqNode, tmp_s, strlen(tmp_s), rc);
            }
            prevcores += mTargetConfig.getCoresInCluster(i);
        }
    }
}

void Target::ExtendedTargetInit() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    switch (mTargetConfig.getSocID()) {
    case 246: /*8996*/
    case 291: /*8096*/
        invoke_wa_libs();
        break;

    case 305: /*8996Pro*/
    case 312: /*8096Pro*/
        invoke_wa_libs();
        break;

    case 339: /*msmnile*/
    case 361: /*msmnile without modem and SDX55*/
    case 356: /*kona*/
    case 394: /*trinket*/
    case 417: /*bengal*/
    case 420: /*bengal*/
    case 444: /*bengal*/
    case 445: /*bengal*/
    case 518: /*khaje*/
    case 561: /*khaje*/
    case 585: /*khaje*/
    case 586: /*khaje*/
    case 400: /*lito*/
    case 440: /*lito*/
    case 407: /*atoll*/
    case 415: /*lahaina*/
    case 439: /*lahaina*/
    case 456: /*lahaina-APQ*/
    case 501: /*lahaina - SM8345 - APQ */
    case 502: /*lahaina - SM8325p */
    case 434: /*lagoon*/
    case 459: /*lagoon*/
    case 454: /*holi*/
    case 507: /*holi*/
    case 450: /*shima*/
    case 476: /*orchid*/
    case 475: /*yupik*/
    case 497: /*yupik*/
    case 498: /*yupik*/
    case 499: /*yupik*/
    case 515: /*yupik*/
    case 578: /*holi*/
    case 575: /*katmai*/
    case 576: /*katmai*/
       invoke_tinyu_wa(mTargetConfig.getSocID());
       break;
    case 457: /*taro*/
       mUpDownComb = true;
       mGrpUpDownComb = true;
       break;
    }

    Dump();
}
