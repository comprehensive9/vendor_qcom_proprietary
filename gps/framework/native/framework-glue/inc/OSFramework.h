/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  OSFramework Class definition

  Copyright (c) 2015-2017, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_OSFRAMEWORK_H__
#define __IZAT_MANAGER_OSFRAMEWORK_H__

#include <MsgTask.h>
#include <IOSFramework.h>
#include <IIzatManager.h>
#include <log_util.h>
#include <LocationDataTypes.h>
#include "IOSListener.h"
#include "IzatNmea.h"
#include <unordered_set>

using namespace izat_manager;

class Listener : public IOSListener {
public:
    inline Listener() :
        IOSListener() {}
    inline ~Listener() { }

    inline virtual IzatListenerMask listensTo() const override {
        return IZAT_STREAM_NETWORK;
    }

    virtual void onLocationChanged(
            const izat_manager::IzatLocation* location,
            const izat_manager::IzatLocationStatus) override;

    virtual void onStatusChanged(
            const izat_manager::IzatProviderStatus status) override;

    inline virtual void onNmeaChanged(const IzatNmea*) override {}
    typedef unordered_set<trackingCallback*> TCallbackCol;
    TCallbackCol mCallbackSet;
};

class OSFramework : public IOSFramework {

public:
    static OSFramework* getOSFramework();
    static void destroyInstance();

    // IOSFramework overrides
    IDataItemSubscription* getDataItemSubscriptionObj();
    IFrameworkActionReq * getFrameworkActionReqObj();
    IOSLocationProvider* getOSNP();
    IOSFrameworkCleaner* getOSFrameworkCleaner();
    static uint32_t setOptInStatus(bool status, responseCallback* callback);
    static void enableNetworkProvider();
    static void disableNetworkProvider();
    static void startNetworkLocation(trackingCallback* callback);
    static void stopNetworkLocation(trackingCallback* callback);

private:
    static OSFramework* mOSFramework;

    Listener mListener;

    OSFramework();
    ~OSFramework();

    inline static loc_util::MsgTask* getIZatManagerMsgTask() {
        IIzatManager* izatManager = getIzatManager(OSFramework::getOSFramework());
        if (izatManager) {
            return izatManager->getMsgTask();
        } else {
            return nullptr;
        }
    };
};

#endif // #ifndef __IZAT_MANAGER_OSFRAMEWORK_H__
