/******************************************************************************
 *   @file    Feature.cpp
 *   @brief   Feature default implementation
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the Base implementation
 *   for Feature implementers to derive.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include <cutils/trace.h>
#include "Feature.h"
#include "DebugLog.h"

#define LOG_TAG_FEATURE "Feature"
using namespace std;
void Feature::runAsync(AsyncTriggerPayload payload) {
    DEBUGV(LOG_TAG_FEATURE, "runAsync %s", mFeatureName.c_str());
    if (mMetaMeter) {
        /*
         * Check if the metameter is interested in this trigger:
         * We need to send the trigger if DEFAULT_HINTTYPE is present
         * for this hintID (metameter is interested in all hintTypes)
         * or if current hintType is present for this hintID in the
         * FeatureInfo
         *  */
        auto currHintIDTypes = mfeatureInfo.asyncIdList[payload.hintID];
        if (currHintIDTypes.count(DEFAULT_HINTTYPE) == 1
                || currHintIDTypes.count(payload.hintType) == 1) {
            DEBUGV(LOG_TAG_FEATURE, "calling handleAsync() %s", mFeatureName.c_str());
            mMetaMeter->handleAsync(payload);
        }
    }
}

void Feature::runPeriodic() {
     int sleepDurationInMs = -1;
     while (runPeriodicThreadInterrupt == false) {
         sleepDurationInMs = -1;
         DEBUGV(LOG_TAG_FEATURE, "Starting threads, feature name = %s", this->getFeatureName().c_str());

         unique_lock<mutex> lk(this->mFeatureRunner.mWaitForThreads);
         DEBUGV(LOG_TAG_FEATURE, "Waiting for threads to signal");
         this->mFeatureRunner.mRunPeriodic.wait(lk, [this]{return ((mSleepDurationInMs != -1) && mRunFlag);});
         sleepDurationInMs = this->mSleepDurationInMs;
         DEBUGV(LOG_TAG_FEATURE, "Signal received from thread");
         lk.unlock();

         unique_lock<mutex> lck(this->mFeatureRunner.mWaitForInterrupt);
         this->mFeatureRunner.mInterruptPeriodic.wait_for(lck, chrono::milliseconds(sleepDurationInMs));
         lck.unlock();

         if(this->mRunFlag) {
            DEBUGV(LOG_TAG_FEATURE,"Calling runAsyncPeriodic for %s", this->getFeatureName().c_str());
            if(mEnableTracing) {
                string traceLog = "LM-runPeriodic() " + this->getFeatureName();
                ATRACE_ASYNC_BEGIN(traceLog.c_str(), 0);
            }
            this->runAsyncPeriodic(false);
            if(mEnableTracing) {
                string traceLog = "LM-runPeriodic() " + this->getFeatureName();
                ATRACE_ASYNC_END(traceLog.c_str(), 0);
            }
            DEBUGV(LOG_TAG_FEATURE,"runPeriodic for %s returned", this->getFeatureName().c_str());
         }
     }
 }

void Feature::runAsyncPeriodic(const std::atomic_bool& interrupt) {
    /* Each feature should implement it's own */
}

void Feature::runAsyncIdle(const atomic_bool& interrupt) {
    DEBUGV(LOG_TAG_FEATURE, "runAsyncIdle %s", mFeatureName.c_str());
    if(mAlgorithm) {
        DEBUGV(LOG_TAG_FEATURE, "calling runAlgo() %s", mFeatureName.c_str());
        mAlgorithm->runAlgo(interrupt);
    }
}


Feature* FeatureLocator::lookup(std::string& featureName) {
    Feature *currFeature = nullptr;
    int featureFound = 0;
    for (std::vector <Feature *>::size_type i = 0; i < mFeatureList.size(); i++) {
        if (mFeatureList[i]->getFeatureName() == featureName) {
            currFeature = mFeatureList[i];
            featureFound = 1;
            break;
        }
    }
    DEBUGD(LOG_TAG_FEATURE, "Feature Name = %s, total featuers = %lu, featureFound = %d",
           featureName.c_str(), mFeatureList.size(), featureFound);
    return currFeature;
}

int FeatureLocator::getData(std::string& featureName, std::string& appName, featureData &data, int type) {
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->getData(appName, data, type);
    }
    DEBUGD(LOG_TAG_FEATURE, "data.data = %d", data.data);
    return ret;
}

int FeatureLocator::getData(std::string& featureName, std::string& appName, featureData &data, int type, int duration) {
    DEBUGD(LOG_TAG_FEATURE, "Feature.cpp FeatureLocator::getData featureName: %s", featureName.c_str());
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->getData(appName, data, type, duration);
    }
    DEBUGD(LOG_TAG_FEATURE, "data.data = %d", data.data);
    return ret;
}

int FeatureLocator::setData(std::string& featureName, std::string& appName, std::vector<int> data) {
    Feature *currFeature;
    int ret = -1;
    currFeature = lookup(featureName);
    if (currFeature) {
        ret = currFeature->setData(appName, data);
    }
    return ret;
}

void FeatureLocator::registerFeatureList(std::vector<Feature*> featureList) {
    mFeatureList = featureList;
}
