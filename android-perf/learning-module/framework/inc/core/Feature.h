/******************************************************************************
 *   @file    Feature.h
 *   @brief   Feature abstract base class
 *
 *   DESCRIPTION
 *      "Feature"s take measurements, "learn" at runtime and apply actions
 *   based on such learning. Feature Class provides the abstract base class
 *   for Feature implementers to derive from.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#ifndef FEATURE_H
#define FEATURE_H

#include <string>
#include <atomic>
#include <thread>
#include "FeatureInfo.h"
#include "FeatureState.h"
#include "MetaMeter.h"
#include "Algorithm.h"
#include "Action.h"
#include "FeatureLocator.h"

class PeriodicFeatureRunner {
public:
    std::condition_variable mRunPeriodic;
    std::mutex mWaitForThreads;

    void notify() {
        mRunPeriodic.notify_all();
    }

    //support for interrupted wakeups
    std::condition_variable mInterruptPeriodic;
    std::mutex mWaitForInterrupt;

    void interruptNotifier() {
        mInterruptPeriodic.notify_all();
    }
};

class Feature {
private:
    std::string mFeatureName;
protected:
    /*
     * Features should implement allocation, maintenance and destruction of
     * resources for MetaMeter, Algorithm, Action and InputDb
     * Features should assign the respective pointer to base members
     * LM checks for null and calls the pointer whenever it wants to use
     * a Feature component.
     */
    MetaMeter* mMetaMeter;
    Algorithm* mAlgorithm;
    Action* mAction;
    FeatureInfo mfeatureInfo;
    FeatureState mFeatureState;
    FeatureLocator *mFeatureLocator;
    PeriodicFeatureRunner mFeatureRunner;
    bool mIsDisabled;

public:
    std::atomic_bool mRunFlag = false;
    std::atomic_int mSleepDurationInMs = -1;
    std::thread periodicAlgoThread;
    void runPeriodic();
    std::atomic_bool runPeriodicThreadInterrupt = false;
    bool enablePeriodic = false;
    bool mEnableTracing = false;
    Feature(FeatureInfo featureInfo) :
        mFeatureName(featureInfo.name),
        mMetaMeter(nullptr),
        mAlgorithm(nullptr),
        mAction(nullptr),
        mfeatureInfo(featureInfo),
        mFeatureState(featureInfo.name),
        mFeatureLocator(nullptr),
        mIsDisabled(false) {
        //property to enable tracing
        mEnableTracing = (property_get_bool("vendor.debug.trace.perf", 0) == 1);
    }

    virtual ~Feature() {}
    std::string getFeatureName() {
        return mFeatureName;
    }

    bool isEnabled() {
        return !mIsDisabled;
    }

    void Disable() {
        mIsDisabled = true;
    }

    /*
     * Features should implement the logic for Async Triggers in runAsync()
     * LM calls runAsync() for each AsyncTrigger the Feature is interested in
     */
    virtual void runAsync(AsyncTriggerPayload payload);


    /*
     * Features should implement the logic for Idle Triggers in runAsyncIdle()
     * This is generally used to call algorithms when the system is idle
     * LM calls runAsyncIdle() of the Feature when it decides the system is idle
     */
    virtual void runAsyncIdle(const std::atomic_bool& interrupt);
    /*
     * Features should implement the logic for Idle Triggers in runAsyncIdle()
     * This is generally used to call algorithms when the system is idle
     * LM calls runAsyncIdle() of the Feature when it decides the system is idle
     */
    virtual void runAsyncPeriodic(const std::atomic_bool& interrupt);

    void registerServiceLocator(FeatureLocator* FeatureLocatorService) {
        mFeatureLocator = FeatureLocatorService;
    }
    virtual int getData(std::string& key __attribute__((unused)), featureData &data, int type __attribute__((unused))) {
        data.data = {-1};
        return 0;
    }
    virtual int getData(std::string& key __attribute__((unused)), featureData &data, int type __attribute__((unused)), int duration __attribute__((unused))) {
        data.data = {-1};
        return 0;
    }
    virtual int setData(std::string& appName __attribute__((unused)), std::vector<int> data __attribute__((unused))) {
        return 0;
    }

};
#endif /* FEATURE_H */

