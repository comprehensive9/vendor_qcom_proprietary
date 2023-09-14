/******************************************************************************
  @file    TargetConfig.cpp
  @brief   Implementation of TargetConfig

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGETCONFIG"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <log/log.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <cutils/properties.h>
#include <sys/utsname.h>
#include "TargetConfig.h"

#define QC_DEBUG_ERRORS 1
#ifdef QC_DEBUG_ERRORS
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#else
#define QLOGE(...)
#endif

#if QC_DEBUG
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#define QCLOGE(...)   ALOGE(__VA_ARGS__)
#else
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#define QCLOGE(...)
#endif
#define FAILED -1
TargetConfig TargetConfig::cur_target;

//resolution nodes
#define RESOLUTION_DRM_NODE "/sys/class/drm/card0-DSI-1/modes"
#define RESOLUTION_GRAPHICS_NODE "/sys/class/graphics/fb0/modes"

//targetconfig xml
#define TARGET_CONFIGS_XML "/vendor/etc/perf/targetconfig.xml"
#define KERNEL_VERSION_NODE  "/proc/sys/kernel/osrelease"

//target config tags
#define TARGET_CONFIGS_XML_ROOT "TargetConfig"
#define TARGET_CONFIGS_XML_CHILD_CONFIG "Config%d"
#define TARGET_CONFIGS_XML_ELEM_TARGETINFO_TAG "TargetInfo"
#define TARGET_CONFIGS_XML_ELEM_NUMCLUSTERS_TAG "NumClusters"
#define TARGET_CONFIGS_XML_ELEM_TOTALNUMCORES_TAG "TotalNumCores"
#define TARGET_CONFIGS_XML_ELEM_CORECTLCPU_TAG "CoreCtlCpu"
#define TARGET_CONFIGS_XML_ELEM_SYNCORE_TAG "SynCore"
#define TARGET_CONFIGS_XML_ELEM_MINCOREONLINE_TAG "MinCoreOnline"
#define TARGET_CONFIGS_XML_ELEM_GOVINSTANCETYPE_TAG "GovInstanceType"
#define TARGET_CONFIGS_XML_ELEM_CPUFREQGOV_TAG "CpufreqGov"
#define TARGET_CONFIGS_XML_ELEM_TARGET_TAG "Target"
#define TARGET_CONFIGS_XML_ELEM_SOCIDS_TAG "SocIds"
#define TARGET_CONFIGS_XML_ELEM_CLUSTER_TAG "ClustersInfo"
#define TARGET_CONFIGS_XML_ELEM_ID_TAG "Id"
#define TARGET_CONFIGS_XML_ELEM_NUMCORES_TAG "NumCores"
#define TARGET_CONFIGS_XML_ELEM_TYPE_TAG "Type"
#define TARGET_CONFIGS_XML_ELEM_MAXFREQUENCY_TAG "MaxFrequency"
#define TARGET_CONFIGS_XML_ELEM_MINFPSTUNING_TAG "MinFpsForTuning"


/*
 * Definition for various get functions
 * */
int TargetConfig::getCoresInCluster(int cluster) {
    if (mCorePerCluster) {
        if ((cluster >= 0) && (cluster < mNumCluster)) {
            int tmp = mCorePerCluster[cluster];
            if (tmp <= mTotalNumCores) {
                return tmp;
            }
        } else {
            QLOGE("Error: Invalid cluster id %d",cluster);
        }
    } else {
        QLOGE("Error: TargetConfig not initialized");
    }
    return FAILED;
}

/* Returns the value to be written on cpu max freq
   sysfs node after perflock release.
*/
int TargetConfig::getCpuMaxFreqResetVal(int cluster) {
    if (cluster >= 0 && cluster < MAX_CLUSTER) {
        return mCpuMaxFreqResetVal[cluster];
    }
    return FAILED;
}

TargetConfig::TargetConfig() {
    QLOGI("TargetConfig constructor");
    mSyncCore = true;
    mNumCluster = 0;
    mTotalNumCores = 0;
    mCorePerCluster = NULL;
    mCoreCtlCpu = -1;
    mMinCoreOnline = 0;
    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;
    mCpufreqGov = INTERACTIVE;
    mType = 0;
    mRam = -1;
    mRamKB = -1;
    mMinFpsTuning = 0;
    mInitCompleted.store(false);
    /* Setting reset value to UINT_MAX to avoid race condition with thermal engine.
     * UINT_MAX work as vote removed from perfd side */
    for (int i = 0; i < MAX_CLUSTER; i++)
         mCpuMaxFreqResetVal[i] = UINT_MAX;
    mResolution = MAP_RES_TYPE_ANY;
    QLOGI("TargetConfig constructor exit");
}

TargetConfig::~TargetConfig() {
    QLOGI("TargetConfig destructor");
    if (mCorePerCluster) {
        delete [] mCorePerCluster;
        mCorePerCluster = NULL;
    }

    //delete target configs
    while (!mTargetConfigs.empty()) {
        TargetConfigInfo *tmp = mTargetConfigs.back();
        if (tmp) {
            delete tmp;
        }
        mTargetConfigs.pop_back();
    }
}

int TargetConfig::readRamSize(void) {
    FILE *memInfo = fopen("/proc/meminfo", "r");
    int ram = -1;
    if (memInfo == NULL) {
        return -1;
    }
    char line[256];
    while (fgets(line, sizeof(line), memInfo)) {
        int memTotal = -1;
        char* end = NULL;
        char* token = strtok_r(line, ":", &end);
        if ((end != NULL) && (token != NULL) && (strncmp(token, "MemTotal", 8) == 0)) {
            token = strtok_r(NULL, " ", &end);
            if ((end != NULL) && (token != NULL)) {
                memTotal = strtol(token, &end, 10);
                mRamKB = memTotal;
                if (memTotal < MEM_1GB) {
                    ram = RAM_1GB;
                } else if (memTotal < MEM_2GB) {
                    ram = RAM_2GB;
                } else if (memTotal < MEM_3GB) {
                    ram = RAM_3GB;
                } else if (memTotal < MEM_4GB) {
                    ram = RAM_4GB;
                } else if (memTotal < MEM_6GB) {
                    ram = RAM_6GB;
                } else if (memTotal < MEM_8GB) {
                    ram = RAM_8GB;
                } else if (memTotal < MEM_10GB) {
                    ram = RAM_10GB;
                } else {
                    ram = RAM_12GB;
                }
                if(ram != -1) {
                    break;
                }
            }
        }
    }
    fclose(memInfo);
    return ram;
}

int TargetConfig::readSocID() {
    int fd;
    int soc = -1;
    if (!access("/sys/devices/soc0/soc_id", F_OK)) {
        fd = open("/sys/devices/soc0/soc_id", O_RDONLY);
    } else {
        fd = open("/sys/devices/system/soc/soc0/id", O_RDONLY);
    }
    if (fd != -1)
    {
        char raw_buf[5];
        read(fd, raw_buf,4);
        raw_buf[4] = 0;
        soc = atoi(raw_buf);
        close(fd);
    }
    mSocID = soc; /* set target socid */
    QLOGI("socid of the device: %d", soc);
    return soc;
}

void TargetConfig::readKernelVersion(){
    int fd = -1, n;
    int rc = 0;
    const int MAX_BUF_SIZE = 16;
    char kernel_version_buf[MAX_BUF_SIZE];
    mKernelMinorVersion = -1;
    mKernelMajorVersion = -1;
    fd = open(KERNEL_VERSION_NODE, O_RDONLY);
    if (fd != -1) {
        memset(kernel_version_buf, 0x0, sizeof(kernel_version_buf));
        n = read(fd, kernel_version_buf, MAX_BUF_SIZE-1);
        if (n > 0) {
            rc = sscanf(kernel_version_buf, "%d.%d.", &mKernelMajorVersion,
                    &mKernelMinorVersion);
            if (rc != 2) {
                mKernelMinorVersion = -1;
                mKernelMajorVersion = -1;
                QLOGE("sscanf failed, kernel version set to -1");
            } else {
                snprintf(kernel_version_buf, MAX_BUF_SIZE, "%d.%d", mKernelMajorVersion, mKernelMinorVersion);
            }
            mFullKernelVersion = string(kernel_version_buf);
        } else
            ALOGE("Kernel Version node not present");
    close(fd);
    }
}

void TargetConfig::readVariant() {
    char target_name_var[PROPERTY_VALUE_MAX];
    if (property_get("ro.product.name", target_name_var, "Undefined") > 0) {
        target_name_var[PROPERTY_VALUE_MAX-1]='\0';
    }
    mVariant = string(target_name_var);
}

/* A single InitializeTargetConfig function for all the targets which
   parses XML file for target configs.
 * For adding a new TargetConfig, create a targetconfig.xml file.
*/


/* Init Sequence
 * 1. Pouplate from targetconfig.xml
 * 2. Init the target
 */
void TargetConfig::InitializeTargetConfig() {
    int socid = 0;
    int res = 0;

    QLOGI("Inside InitializeTargetConfig");

    socid = readSocID();
    mRam = readRamSize();
    readVariant();

    //get resolution
    (void) readResolution();
    res = getResolution();
    /*
     * Initialize kernel version
     */
    readKernelVersion();
    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;

    InitTargetConfigXML();
    TargetConfigInit();
    QLOGI("Init complete for: %s", getTargetName().c_str());

    // view stored info
    DumpAll();
}

void TargetConfig::TargetConfigInit() {

    QLOGI("TargetConfigInit start");
    TargetConfigInfo *config = getTargetConfigInfo(getSocID());
    if (NULL == config) {
        QLOGE("Initialization of TargetConfigInfo Object failed");
        return;
    }

    if (!config->mUpdateTargetInfo) {
        QLOGE("Target Initialized with default available values, as mistake in target config XML file");
        return;
    }

    if(config->mCalculatedCores != config->mTotalNumCores) {
        QLOGE("Target Initialized with default values, as mismatch between the TotalNumCores and CalculatedCores.");
        return;
    }
    mTargetName = string(config->mTargetName);
    QLOGI("Init %s",!mTargetName.empty() ? mTargetName.c_str() : "Target");

    mNumCluster = config->mNumCluster;
    mTotalNumCores = config->mTotalNumCores;
    mCorePerCluster = new(std::nothrow) int[mNumCluster];
    if (mCorePerCluster) {
        for (int i=0;i<mNumCluster;i++) {
            mCorePerCluster[i] = config->mCorepercluster[i];
            mCpuMaxFreqResetVal[i] = config->mCpumaxfrequency[i];
        }
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }

    mSyncCore = config->mSyncCore;
    mType = config->mType;
    if (config->mCoreCtlCpu < 0 || config->mCoreCtlCpu >= mTotalNumCores) {
        QLOGW("CoreCtlCpu is incorrectly specified in XML file, So Initializing to -1");
    } else {
        mCoreCtlCpu = config->mCoreCtlCpu;
    }

    if (config->mMinCoreOnline < 0 || config->mMinCoreOnline > mTotalNumCores) {
        QLOGW("MinCoreOnline is incorrectly specified in XML file, So Initializing to 0");
    } else {
        mMinCoreOnline = config->mMinCoreOnline;
    }

    if (config->mGovInstanceType >= MAX_GOVINSTANCETYPE) {
        QLOGW("GovInstanceType is incorrectly specified in XML file, So Initializing to CLUSTER_BASED_GOV_INSTANCE");
    } else {
        mGovInstanceType = config->mGovInstanceType;
    }

    if (config->mCpufreqGov >= MAX_CPUFREQGOV) {
        QLOGW("CpufreqGov is incorrectly specified in XML file, So Initializing to INTERACTIVE");
    } else {
        mCpufreqGov = config->mCpufreqGov;
    }

    mMinFpsTuning = config->mMinFpsTuning;
    /*Deleting target configs vector, after target initialized with the required values
      which are parsed from the XML file. As the vector is not needed anymore.*/
    while (mTargetConfigs.empty()) {
        config = mTargetConfigs.back();
        if (config) {
            delete config;
        }
        mTargetConfigs.pop_back();
    }
    QLOGI("TargetConfigInit end");

}

void TargetConfig::InitTargetConfigXML() {
    const string fTargetConfigName(TARGET_CONFIGS_XML);
    const string xmlTargetConfigRoot(TARGET_CONFIGS_XML_ROOT);
    int idnum, i;
    char TargetConfigXMLtag[NODE_MAX] = "";
    string xmlChildTargetConfig;
    AppsListXmlParser *xmlParser = new(std::nothrow) AppsListXmlParser();
    if (NULL == xmlParser) {
        return;
    }
    QLOGI("InitTargetConfigXML start");

    //target configs
    /*Multitple targets can share the same platform. So, these multple target configs
      are differentiated with the config index number in the configs tag. Parsing of XML
      file is done by identifying these configs.*/
    for(i = 1; i <= MAX_CONFIGS_SUPPORTED_PER_PLATFORM; i++) {
        snprintf(TargetConfigXMLtag, NODE_MAX, TARGET_CONFIGS_XML_CHILD_CONFIG, i);
        xmlChildTargetConfig = TargetConfigXMLtag;
        idnum = xmlParser->Register(xmlTargetConfigRoot, xmlChildTargetConfig, TargetConfigsCB, &i);
        xmlParser->Parse(fTargetConfigName);
        xmlParser->DeRegister(idnum);
    }
    delete xmlParser;
    QLOGI("InitTargetConfigXML end");

    return;
}

int TargetConfig::ConvertToIntArray(char *str, int intArray[], int size) {
    int i = 0;
    char *pch = NULL;
    char *end = NULL;
    char *endPtr = NULL;

    if ((NULL == str) || (NULL == intArray)) {
        return i;
    }

    end = str + strlen(str);

    do {
        pch = strchr(str, ',');
        intArray[i] = strtol(str, &endPtr, 0);
        i++;
        str = pch;
        if (NULL != pch) {
            str++;
        }
    } while ((NULL != str) && (str < end) && (i < size));

    return i;
}

long int TargetConfig::ConvertNodeValueToInt(xmlNodePtr node, const char *tag, long int defaultvalue) {
    /* For a given XML node pointer, checks the presence of tag.
       If tag present, returns the converted numeric of tags value.
       else returns the default value of that tag.
    */
    char *idPtr = NULL;

    if (xmlHasProp(node, BAD_CAST tag)) {
        idPtr = (char *)xmlGetProp(node, BAD_CAST tag);
        if (NULL != idPtr) {
            defaultvalue = strtol(idPtr, NULL, 0);
            xmlFree(idPtr);
        }
    }
    return defaultvalue;
}

void TargetConfig::TargetConfigsCB(xmlNodePtr node, void *index) {

    char *idPtr = NULL;
    char *tmp_target = NULL;
    int id = 0, core_per_cluster = 0, target_index = 0;
    long int frequency = UINT_MAX;
    bool cluster_type_present = false;

    TargetConfig &tc = TargetConfig::getTargetConfig();

    if (NULL == index) {
        QLOGE("Unable to get the target config index of XML.");
        return;
    }
    target_index = *((int*)index) - 1;
    if((target_index < 0) || (target_index >= MAX_CONFIGS_SUPPORTED_PER_PLATFORM)) {
        QLOGE("Invalid config index value while parsing the XML file.");
        return;
    }


    /*Parsing of a config tag is done by multiple calls to the parsing function, so for
    the first call to the function we create a config object and for all the next calls
    we use the same object. */
    if (tc.mTargetConfigs.size() <= target_index) {
        auto tmp = new(std::nothrow) TargetConfigInfo;
        if (tmp != NULL)
            tc.mTargetConfigs.push_back(tmp);
    }
    TargetConfigInfo *config = tc.mTargetConfigs[target_index];
    if (NULL == config) {
        QLOGE("Initialization of TargetConfigInfo object failed");
        return;
    }

    //Parsing Targetconfig tag which has all the target related information.
    if (!xmlStrcmp(node->name, BAD_CAST TARGET_CONFIGS_XML_ELEM_TARGETINFO_TAG)) {
        if (xmlHasProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TARGET_TAG)) {
            tmp_target = (char *)xmlGetProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TARGET_TAG);
            if(tmp_target != NULL) {
                config->mTargetName = string(tmp_target);
            }
        }

        //third argument defaultvalue is needed to retain the same value in case of any error.
        config->mSyncCore = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_SYNCORE_TAG, config->mSyncCore);
        config->mNumCluster = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_NUMCLUSTERS_TAG, config->mNumCluster);
        config->mTotalNumCores = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_TOTALNUMCORES_TAG, config->mTotalNumCores);
        config->mCoreCtlCpu = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_CORECTLCPU_TAG, config->mCoreCtlCpu);
        config->mMinCoreOnline = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_MINCOREONLINE_TAG, config->mMinCoreOnline);
        config->mGovInstanceType = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_GOVINSTANCETYPE_TAG, config->mGovInstanceType);
        config->mCpufreqGov = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_CPUFREQGOV_TAG, config->mCpufreqGov);
        config->mMinFpsTuning = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_MINFPSTUNING_TAG, config->mMinFpsTuning);

        if (xmlHasProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_SOCIDS_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_SOCIDS_TAG);
            if (NULL != idPtr) {
                config->mNumSocids = ConvertToIntArray(idPtr,config->mSupportedSocids, MAX_SUPPORTED_SOCIDS);
                xmlFree(idPtr);
            }
        }

        //checks to ensure the target configs are mentioned correctly in XML file.
        if ((config->mNumCluster <= 0) || (config->mNumCluster > MAX_CLUSTER)) {
            config->mUpdateTargetInfo = false;
            QLOGE("Number of clusters are not mentioned correctly in targetconfig xml file");
        }

        if (config->mTotalNumCores <= 0) {
            config->mUpdateTargetInfo = false;
            QLOGE("Number of cores are not mentioned correctly in targetconfig xml file");
        }

        if (config->mNumSocids <= 0) {
            config->mUpdateTargetInfo = false;
            QLOGE("Supported Socids of the target are not mentioned correctly in targetconfig xml file.");
        }

        QLOGI("Identified Target with clusters=%u cores=%u core_ctlcpu=%d min_coreonline=%d target_name=%s",
              config->mNumCluster, config->mTotalNumCores, config->mCoreCtlCpu,
              config->mMinCoreOnline, !config->mTargetName.empty() ? config->mTargetName.c_str() : "Not mentioned");
    }

    /* Parsing the clusterinfo tag, which has all the cluster based information.
       Each cluster has a seperate clusterinfo tag. */
    if (!xmlStrcmp(node->name, BAD_CAST TARGET_CONFIGS_XML_ELEM_CLUSTER_TAG)) {
        id = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_ID_TAG, id);
        core_per_cluster = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_NUMCORES_TAG, core_per_cluster);

        if (xmlHasProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TYPE_TAG)) {
            idPtr = (char *)xmlGetProp(node, BAD_CAST TARGET_CONFIGS_XML_ELEM_TYPE_TAG);
            if (NULL != idPtr) {
                cluster_type_present = true;
                if (id == 0) {
                    if (strncmp("little",idPtr,strlen(idPtr))== 0) {
                        config->mType = 1;
                    } else {
                        config->mType = 0;
                    }
                }
                xmlFree(idPtr);
            }
        }

        frequency = ConvertNodeValueToInt(node, TARGET_CONFIGS_XML_ELEM_MAXFREQUENCY_TAG, frequency);
        if (frequency <= 0 || frequency > UINT_MAX) {
            QLOGW("Cluster %d max possible frequency is mentioned incorrectly, assigning UNIT_MAX", id);
            frequency = UINT_MAX;
        }

        //ensuring the cluster_id is in the range
        if ((id < 0) || (id >= config->mNumCluster) || (id >= MAX_CLUSTER)) {
            config->mUpdateTargetInfo = false;
            QLOGE("Cluster id is not mentioned correctly in targetconfig.xml file, aborting parsing of XML");
            return;
        } else {
            config->mCorepercluster[id] = core_per_cluster;
            config->mCpumaxfrequency[id] = frequency;
            config->mCalculatedCores += core_per_cluster;
        }

        //need this check to decide the mapping of logical clusters to physical clusters.
        if ((config->mNumCluster > 1) && (!cluster_type_present)) {
            config->mUpdateTargetInfo = false;
            QLOGE("Number of clusters is more than 1 but the type is not mentioned for cluster id=%d",id);
        }

        QLOGI("Identified CPU cluster with id=%d cores=%d max_freq=%u", id, core_per_cluster, frequency);
    }

    return;
}

bool TargetConfig::TargetConfigInfo::CheckSocID(int SocId) {
    bool valid_socid = false;
    for (int i = 0; i < mNumSocids; i++) {
        if (mSupportedSocids[i] == SocId) {
            valid_socid = true;
            break;
        }
    }
    return valid_socid;
}

bool TargetConfig::TargetConfigInfo::CheckClusterInfo() {
    bool valid_target_info = false;
    int total_cores = 0;
    for (int i = 0; i < mNumCluster; i++) {
        total_cores += mCorepercluster[i];
    }
    return total_cores == mTotalNumCores?true:false;
}

/*Depending on the socid of device, we select the target configs from the multiple
configs in the XML file. */
TargetConfig::TargetConfigInfo* TargetConfig::getTargetConfigInfo(int socId) {
    vector<TargetConfigInfo*>::iterator itbegin = mTargetConfigs.begin();
    vector<TargetConfigInfo*>::iterator itend = mTargetConfigs.end();

    for (vector<TargetConfigInfo*>::iterator it = itbegin; it != itend; ++it) {
        if ((NULL != *it) && ((*it)->mNumSocids > 0)) {
            if ((*it)->CheckSocID(socId)) {
                if(!(*it)->CheckClusterInfo()) {
                    (*it)->mUpdateTargetInfo = false;
                    QLOGE("Target Info with maching SocId have invalid cluster info");
                }
                return (*it);
            }
        }
    }
    QLOGE("Target Initializes with default values, as Target socid not supported");
    return NULL;
}

int TargetConfig::readResolution() {
    int fd = -1;
    int ret = MAP_RES_TYPE_ANY, n = 0;
    int w = 0, h = 0, t = 0;
    char *pch = NULL;
    const int MAX_BUF_SIZE = 16;

    //use the new node if present to get the resolution.
    if (!access(RESOLUTION_DRM_NODE, F_OK)) {
        fd = open(RESOLUTION_DRM_NODE, O_RDONLY);
    } else {
        fd = open(RESOLUTION_GRAPHICS_NODE, O_RDONLY);
    }

    if (fd != -1) {
        char buf[MAX_BUF_SIZE];
        memset(buf, 0x0, sizeof(buf));
        n = read(fd, buf, MAX_BUF_SIZE-1);
        if (n > 0) {
            //parse U:1080x1920p-60 (or) 1440x2560
            if (isdigit(buf[0])) {
                w = atoi(&buf[0]); //for new node
            } else {
                w = atoi(&buf[2]);//for old node
            }
            pch = strchr(buf, 'x');
            if (NULL != pch) {
                h = atoi(pch+1);
            }

            if (h && (w/h > 0)) {
                t = w;
                w = h;
                h = t;
            }

            //convert resolution to enum
            if (w <= 800 && h <= 1280) {
                ret = MAP_RES_TYPE_720P;
            }
            else if (w <= 800 && h <= 1440) {
                ret = MAP_RES_TYPE_HD_PLUS;
            }
            else if (w <= 1080 && h <= 1920) {
                ret = MAP_RES_TYPE_1080P;
            }
            else if (w <= 1440 && h <= 2560) {
                ret = MAP_RES_TYPE_2560;
            }
        }
        close(fd);
    }
    mResolution = ret; /* set the resolution*/
    QLOGI("Resolution of the device: %d", ret);
    return ret;
}
void TargetConfig::DumpAll() {
    int i;
    QLOGI("SocId: %d \n", getSocID());
    QLOGI("Resolution: %d \n", getResolution());
    QLOGI("Ram: %d GB\n", getRamSize());
    QLOGI("Total Ram: %d KB \n", getRamInKB());
    QLOGI("Kernel Major Version: %d \n", getKernelMajorVersion());
    QLOGI("Kernel Minor Version: %d \n", getKernelMinorVersion());
    QLOGI("Full Kernel Version: %s \n", getFullKernelVersion().c_str());
    QLOGI("TargetName: %s \n", getTargetName().c_str());
    QLOGI("Variant: %s \n", getVariant().c_str());
    QLOGI("Type: %d \n", getType());
    QLOGI("Number of cluster %d \n", getNumCluster());
    QLOGI("Number of cores %d \n", getTotalNumCores());
    for (i =0; i < getNumCluster(); i++) {
        if (getCoresInCluster(0)) {
            QLOGI("Cluster %d has %d cores", i, getCoresInCluster(i));
        }
        QLOGI("Cluster %d can have max limit frequency of %u ", i, getCpuMaxFreqResetVal(i));
    }
    QLOGI("Core_ctl_cpu %d", getCoreCtlCpu());
    QLOGI("min_core_online %d", getMinCoreOnline());
    QLOGI("SyncCore_value %d", isSyncCore());
    QLOGI("GovInstance_type %d", getGovInstanceType());
    QLOGI("CpufreqGov_type %d", getCpufreqGov());

    return;
}
