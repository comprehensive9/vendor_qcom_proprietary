/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017,2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


#define LOG_TAG "OSFramework"
#define LOG_NDEBUG 0

#include "OSFramework.h"
#include <loc_pla.h>

#include "Subscription.h"
#include "FrameworkActionRequest.h"
#include "IzatDefines.h"
#include "OSFrameworkCleaner.h"
#include "OsNpGlue.h"
#include "IzatLocation.h"

using namespace loc_util;

void Listener::onLocationChanged(
        const izat_manager::IzatLocation * izatLocation,
        const izat_manager::IzatLocationStatus) {

    Location location = {};

    if (izatLocation->mHasLatitude && izatLocation->mHasLongitude) {
        location.flags |= LOCATION_HAS_LAT_LONG_BIT;
        location.latitude = izatLocation->mLatitude;
        location.longitude = izatLocation->mLongitude;
    }

    if (izatLocation->mHasAltitudeWrtEllipsoid) {
        location.flags |= LOCATION_HAS_ALTITUDE_BIT;
        location.altitude = izatLocation->mAltitudeWrtEllipsoid;
    }

    if (izatLocation->mHasSpeed) {
        location.flags |= LOCATION_HAS_SPEED_BIT;
        location.speed = izatLocation->mSpeed;
    }

    if (izatLocation->mHasBearing) {
        location.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
        location.bearing = izatLocation->mBearing;
    }

    if (izatLocation->mHasHorizontalAccuracy) {
        location.flags |= LOCATION_HAS_ACCURACY_BIT;
        location.accuracy = izatLocation->mHorizontalAccuracy;
    }

    if (izatLocation->mHasVertUnc) {
        location.flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
        location.verticalAccuracy = izatLocation->mVertUnc;
    }

    if (izatLocation->mHasSpeedUnc) {
        location.flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
        location.speedAccuracy = izatLocation->mSpeedUnc;
    }

    if (izatLocation->mHasBearingUnc) {
        location.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
        location.bearingAccuracy = izatLocation->mBearingUnc;
    }

    if (izatLocation->mHasPositionTechMask) {
        location.techMask = izatLocation->mPositionTechMask;
    }

    if (izatLocation->mHasUtcTimestampInMsec) {
        location.timestamp = izatLocation->mUtcTimestampInMsec;
    }

    TCallbackCol::const_iterator it = mCallbackSet.begin();
    while (it != mCallbackSet.end()) {
        (* *it)(location);
        ++it;
    }
}

void Listener::onStatusChanged(const izat_manager::IzatProviderStatus status) {
    LOC_LOGd("provider status: %d", status);
}

OSFramework* OSFramework::mOSFramework = NULL;

OSFramework::OSFramework()
{
    int result = -1;
    ENTRY_LOG();

    do {
        // Create IzatManager
        IIzatManager *pIzatManager = getIzatManager(this);
        BREAK_IF_ZERO(1, pIzatManager);
        pIzatManager->subscribeListener(&mListener);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

OSFramework::~OSFramework()
{
    ENTRY_LOG();

    IIzatManager *pIzatManager = getIzatManager(this);
    if (!pIzatManager) {
        LOC_LOGe("null izat manager");
        return;
    }
    pIzatManager->unsubscribeListener(&mListener);

    // Destroy IzatManager
    (void) destroyIzatManager();

    EXIT_LOG_WITH_ERROR("%d", 0);
}

OSFramework* OSFramework::getOSFramework()
{
    int result = -1;

    ENTRY_LOG();
    do {
          // already initialized
          BREAK_IF_NON_ZERO(0, mOSFramework);

          mOSFramework = new (std::nothrow) OSFramework();
          BREAK_IF_ZERO(2, mOSFramework);

          result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mOSFramework;
}

void OSFramework::destroyInstance()
{
    ENTRY_LOG();

    if (mOSFramework) {
        delete mOSFramework;
        mOSFramework = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

// For use in dload from Opensource modules
extern "C" void createOSFramework()
{
    OSFramework::getOSFramework();
}

extern "C" void destroyOSFramework()
{
    OSFramework::destroyInstance();
}

// IOSFramework overrides
IDataItemSubscription* OSFramework::getDataItemSubscriptionObj()
{
    return Subscription::getSubscriptionObj();
}

IFrameworkActionReq* OSFramework::getFrameworkActionReqObj()
{
    return FrameworkActionRequest::getFrameworkActionReqObj();
}

IOSLocationProvider * OSFramework::getOSNP()
{
    return OsNpGlue::getInstance();
}

IOSFrameworkCleaner * OSFramework::getOSFrameworkCleaner()
{
    return OSFrameworkCleaner::getInstance();
}

uint32_t OSFramework::setOptInStatus(bool status, responseCallback* callback)
{
    IIzatManager* izatManager = getIzatManager(OSFramework::getOSFramework());
    if (!izatManager) {
        LOC_LOGe("null izat manager");
        return 0;
    }
    return izatManager->propagateOptInStatus(status, callback);
}

void OSFramework::enableNetworkProvider()
{
    IIzatManager* izatManager = getIzatManager(OSFramework::getOSFramework());
    if (!izatManager) {
        LOC_LOGe("null izat manager");
        return ;
    }
    izatManager->enableProvider(IZAT_STREAM_NETWORK);
}

void OSFramework::disableNetworkProvider()
{
    IIzatManager* izatManager = getIzatManager(OSFramework::getOSFramework());
    if (!izatManager) {
        LOC_LOGe("null izat manager");
        return ;
    }
    izatManager->disableProvider(IZAT_STREAM_NETWORK);
}

void OSFramework::startNetworkLocation(trackingCallback* callback)
{
    struct StartNetworkLocationReqMsg : public LocMsg {
        StartNetworkLocationReqMsg(trackingCallback* callback) :
                mCallback(callback) {}
        virtual ~StartNetworkLocationReqMsg() {}
        void proc() const {
            OSFramework* pOSFramework = OSFramework::getOSFramework();
            IIzatManager* izatManager = getIzatManager(pOSFramework);
            if (pOSFramework && izatManager) {
                pOSFramework->mListener.mCallbackSet.insert(mCallback);
                IzatRequest* request = IzatRequest::createProviderRequest(
                        IZAT_STREAM_NETWORK, 0, 0, false);
                izatManager->addRequest(request);
            }
        }
        trackingCallback* mCallback;
    };

    MsgTask* pMsgTask = getIZatManagerMsgTask();
        if (pMsgTask) {
            pMsgTask->sendMsg(new (nothrow) StartNetworkLocationReqMsg(callback));
        }
    }

void OSFramework::stopNetworkLocation(trackingCallback* callback)
{
    struct StopNetworkLocationReqMsg : public LocMsg {
        StopNetworkLocationReqMsg(trackingCallback* callback) :
                mCallback(callback) {}
        virtual ~StopNetworkLocationReqMsg() {}
        void proc() const {
            OSFramework* pOSFramework = OSFramework::getOSFramework();
            IIzatManager* izatManager = getIzatManager(pOSFramework);
            if (pOSFramework && izatManager) {
                pOSFramework->mListener.mCallbackSet.erase(mCallback);
                if (0 == pOSFramework->mListener.mCallbackSet.size()) {
                        IzatRequest* request = IzatRequest::createProviderRequest(
                                IZAT_STREAM_NETWORK, 0, 0, false);
                        if (request) {
                            izatManager->removeRequest(request);
                        }
                    }
                }
            }
        trackingCallback* mCallback;
    };

    MsgTask* pMsgTask = getIZatManagerMsgTask();
        if (pMsgTask) {
            pMsgTask->sendMsg(new (nothrow) StopNetworkLocationReqMsg(callback));
        }
    }

#ifdef USE_GLIB
// For use in dload from Opensource modules
extern "C" uint32_t setOptInStatus(bool status, responseCallback* callback)
{
    return OSFramework::setOptInStatus(status, callback);
}

extern "C" void enableNetworkProvider()
{
    OSFramework::enableNetworkProvider();
}

extern "C" void disableNetworkProvider()
{
    OSFramework::disableNetworkProvider();
}

extern "C" void startNetworkLocation(trackingCallback* callback)
{
    OSFramework::startNetworkLocation(callback);
}

extern "C" void stopNetworkLocation(trackingCallback* callback)
{
    OSFramework::stopNetworkLocation(callback);
}
#endif
