/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GNSSLOCATIONCLIENTAPI_H
#define GNSSLOCATIONCLIENTAPI_H

#include <vector>
#include <gps_extended_c.h>
#include <loc_gps.h>
#include "IGnssAPI.h"
#include "GnssCbBase.h"
#include "LocationClientApi.h"
#include "LocationClientApiImpl.h"
#include "LocationIntegrationApi.h"

using namespace location_integration;

namespace garden {

enum GnssCallbackOptionsMask {
    LOCATION_BIT =      (1 << 0),
    GNSS_SV_BIT =       (1 << 1),
    GNSS_NMEA_BIT =     (1 << 2),
    GNSS_DATA_BIT =     (1 << 3),
    GNSS_MEAS_BIT =     (1 << 4),
    LOC_SYS_INFO_BIT =  (1 << 5),
};

typedef std::function<void(
    void
)> LocationReceivedCb;

typedef std::function<void(
    void
)> PingReceivedCb;

struct ClientReportsCbs {
    LocationReceivedCb      locationReceivedCallback;
    PingReceivedCb          pingReceivedCallback;
};

class GnssLocationClientAPI : public IGnssAPI {
private:
    location_client::LocationClientApi*      mPClient;
    location_client::LocationClientApiImpl*  mPClientImpl;
    LocationIntegrationApi*  mPIntClient;
    location_client::GnssReportCbs           mReportCbs;
    location_client::EngineReportCbs         mEngineReportCbs;
    LocIntegrationCbs       mIntApiCbs;
    LocConfigPriorityMap    mPriorityMap;
    GnssStartType           mStartType;
    uint32_t                mIntervalInMs;
    uint32_t                mDistanceInMeters;
    LocReqEngineTypeMask  mEngMask;
    vector<location_client::Geofence>        mGeofences;
    PositioningEngineMask mPosEngineMask;
    bool                    mEnableOutput;
    bool                    mRouteToNmeaPort;
    bool                    mAutoMode;

public:
    GnssLocationClientAPI();
    virtual ~GnssLocationClientAPI();
    void setGnssCbs(GnssCbBase* callbacks) override;
    void setFlpCbs(GnssCbBase* callbacks) override;
    void setGeofenceCbs(GnssCbBase* callbacks) override;
    int gnssStart(void) override;
    int gnssStop(void) override;
    void gnssCleanup(void) override;
    int gnssInjectLocation(double latitude, double longitude,
            float accuracy) override;
    void gnssDeleteAidingData(LocGpsAidingData flags) override;
    int gnssSetPositionMode(LocGpsPositionMode mode,
            LocGpsPositionRecurrence recurrence, uint32_t min_interval,
            uint32_t preferred_accuracy, uint32_t preferred_time,
            bool lowPowerMode=false) override;
    void configurationUpdate(const char* config_data, int32_t length) override;
    void updateNetworkAvailability(int available, const char* apn) override;
    void gnssNiResponse(uint32_t id, GnssNiResponse response) override;
    int gnssInjectBestLocation(Location location) override;

    //flp
    int flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id=1) override;
    int flpStopSession(uint32_t id=1) override;
    int flpGetBatchSize() override;
    int flpUpdateSession(uint32_t sessionMode, LocationOptions& options, uint32_t id=1) override;
    int flpGetBatchedLocation(int lastNLocations, uint32_t id=1) override;
    void flpCleanUp() override;

    // geofence
    virtual void addGeofence(uint32_t geofenceId, double latitudeDegrees,
            double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs,
            uint32_t dwellTimeSeconds) override;
    virtual void removeGeofence(uint32_t geofenceId) override;
    virtual void modifyGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs,
            uint32_t dwellTimeSeconds) override;
    virtual void pauseGeofence(uint32_t geofenceId) override;
    virtual void resumeGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions) override;
    virtual void removeAllGeofences() override;

    // location client api specific
    //Deprecated, use setClientReportsCbs(GnssCbBase* callbacks) for instead
    void setClientReportsCbs(ClientReportsCbs* callbacks);
    void gnssStartOptions(GnssStartType startType, uint32_t intervalInMs,
            uint32_t distanceInMeters, LocReqEngineTypeMask engMask,
            int gnssReportOptions) override;
    //Deprecated, use gnssDeleteAidingData(LocGpsAidingData flags) for instead
    uint32_t gnssDeleteAidingData(GnssAidingData& data);
    void testGetEnergy() override;
    void testRegSystemInfo() override;
    void testUnregSystemInfo() override;
    void pingTest() override;
    uint16_t getYearOfHw() override;
    inline void setCbOutput(bool enableOutput) { mEnableOutput = enableOutput; }
    inline bool getCbOutput() { return mEnableOutput; }
    inline void setAutoModeOptions(bool autoMode, bool routeToNmeaPort) {
        mAutoMode = autoMode;
        mRouteToNmeaPort = routeToNmeaPort;
    }
    inline bool getAutoMode() { return mAutoMode; }
    inline bool getRouteToNmeaPort() { return mRouteToNmeaPort; }
    void setEngineMask(PositioningEngineMask posEngineMask) {
        mPosEngineMask = posEngineMask;
    }
    // list all active geofences and returns the number of active gf's
    uint32_t listAllActiveGeofences();
};

} // namespace garden

#endif //GNSSLOCATIONCLIENTAPI_H
