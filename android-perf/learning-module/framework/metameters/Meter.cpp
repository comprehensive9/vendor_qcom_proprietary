/******************************************************************************
  @file    Meter.cpp
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "Meter.h"
#include "ThreadHelper.h"
#include "inc/MeterFwk.h"
#include "MeterRegistry.h"
#include "DebugLog.h"
#include <exception>

using namespace std;

#define LOG_TAG_METER "Meter"
Meter::Meter(){
    mMeterFwk = new(std::nothrow) MeterFwk();
    hasStop = false;
}

Meter::Meter(string& meterName) {
    mMeterName = meterName;
    hasStop = false;
    mMeterFwk = new(std::nothrow) MeterFwk();
}

Meter::Meter(MeterInfo meterInfo) {
    mInfo = meterInfo;
    hasStop = false;
    mMeterName = meterInfo.name;
    mMeterFwk = new(std::nothrow) MeterFwk();
}

Meter::~Meter() {
    if(mMeterFwk) {
        delete mMeterFwk;
        mMeterFwk = NULL;
    }
}

bool Meter::hasStopNotification() {
    return hasStop;
}

void Meter::asyncTriggerUpdate(const AsyncTriggerPayload& asyncUpdate) {
    if(asyncUpdate.hintID == 0 && asyncUpdate.hintType == DEFAULT_HINTTYPE) {
        hasStop = true;
        DEBUGV(LOG_TAG_METER," stop notification on %s",mMeterName.c_str());
        return;
    }

    if(hasStop) {
        hasStop = false;
    }

    DEBUGV(LOG_TAG_METER,"(hintID,hintType,meterName) = (%u, %d,%s)",asyncUpdate.hintID,asyncUpdate.hintType,mMeterName.c_str());
    asyncUpdates.emplace_back(asyncUpdate);
    return;
}

vector<AsyncTriggerPayload>& Meter::getAsyncUpdates() {
    return asyncUpdates;
}

void Meter::getAsyncUpdates(vector<AsyncTriggerPayload>& asyncUpdates_t) {
    try {
        asyncUpdates_t = asyncUpdates;
    } catch (std::exception &e) {
        DEBUGE(LOG_TAG_METER,"Caught Exception: %s in %s",e.what(), __func__);
    } catch (...) {
        DEBUGE(LOG_TAG_METER,"Caught Exception in %s", __func__);
    }
}

