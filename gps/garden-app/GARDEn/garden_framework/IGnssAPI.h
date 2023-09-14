/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IGNSSAPI_H
#define IGNSSAPI_H
#include <unordered_map>
#include <loc_gps.h>
#include <gps_extended.h>
#include "GnssCbBase.h"
#include "GardenUtil.h"
#include "LocationAPI.h"

#ifndef FEATURE_EXTERNAL_AP
#include "DataItemConcreteTypes.h"
#include "IDataItemCore.h"
#include "SystemStatus.h"
#include "IzatWiFiDBReceiver.h"
#include "IzatWiFiDBProvider.h"
#include "wwan_db_provider.h"
#include "wwan_db_receiver.h"
#endif //FEATURE_EXTERNAL_AP

namespace garden {

using getDebugDataCb = ::std::function<void(GnssDebugReport& debugData)>;
typedef std::unordered_map<LeverArmTypeMask, LeverArmParams> LeverArmParamsMap;
enum GnssStartType {
    GNSS_REPORT     = 0,
    LOCATION_REPORT = 1,
    ENGINE_REPORT   = 2
};


class IGnssAPI : public Waitable {
public:
    IGnssAPI() = default;
    virtual ~IGnssAPI() = default;
    /*****************************Android/LE common APIs***************************/
    virtual void setGnssCbs(GnssCbBase* callbacks) = 0;
    virtual void setFlpCbs(GnssCbBase* callbacks) = 0;
    virtual void setGeofenceCbs(GnssCbBase* callbacks) = 0;
    virtual int gnssStart(void) = 0;
    virtual int gnssStop(void) = 0;
    virtual void gnssCleanup(void) = 0;
    virtual int gnssInjectLocation(double latitude, double longitude, float accuracy) = 0;
    virtual void gnssDeleteAidingData(LocGpsAidingData flags) = 0;
    virtual int gnssSetPositionMode(LocGpsPositionMode mode, LocGpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time,
            bool lowPowerMode=false) = 0;

    virtual void configurationUpdate(const char* config_data, int32_t length) = 0;
    virtual void updateNetworkAvailability(int available, const char* apn) = 0;
    virtual void gnssNiResponse(uint32_t id, GnssNiResponse response) = 0;
    virtual int gnssInjectBestLocation(Location location) = 0;

    //flp
    virtual int flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id=1) = 0;
    virtual int flpStopSession(uint32_t id=1) = 0;
    virtual int flpGetBatchSize() = 0;
    virtual int flpUpdateSession(uint32_t sessionMode, LocationOptions& options, uint32_t id=1) = 0;
    virtual int flpGetBatchedLocation(int lastNLocations, uint32_t id=1) = 0;
    virtual void flpCleanUp() = 0;

    // geofence
    virtual void addGeofence(uint32_t geofenceId, double latitudeDegrees,
            double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs,
            uint32_t dwellTimeSeconds=0) = 0;
    virtual void removeGeofence(uint32_t geofenceId) = 0;
    virtual void modifyGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs,
            uint32_t dwellTimeSeconds=0) = 0;
    virtual void pauseGeofence(uint32_t geofenceId) = 0;
    virtual void resumeGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask monitorTransitions) = 0;
    virtual void removeAllGeofences() = 0;


#ifndef FEATURE_EXTERNAL_AP
    /**********************************Android specific IGNSS API**************************/
    virtual void setAGnssCbs(GnssCbBase* callbacks) {}
    virtual void setAGnssRilCbs(GnssCbBase* callbacks) {}
    virtual void setGnssMeasurementCbs(GnssCbBase* callbacks) {}
    virtual void setGnssVisibilityControlCbs(GnssCbBase* callbacks) {}

    //IAGnss
    virtual bool agnssDataConnClosed() { return true; }
    virtual bool agnssDataConnFailed() { return true; }
    virtual bool agnssSetServer(int32_t type, const ::std::string& hostname, int32_t port) {
            return true; }
    virtual bool agnssDataConnOpen(uint64_t networkHandle, const ::std::string& apn,
            int32_t apnIpType) { return true; }

    //IAGnssRil
    virtual void agnssRilSetRefLocation(int type, int mcc, int mnc, int lac, int cid) {}
    virtual bool agnssRilSetSetId(int type, const ::std::string& setid) { return true; }
    virtual bool agnssRilUpdateNetworkAvailability(bool available, const ::std::string& apn) {
            return true; }
    virtual bool agnssRilUpdateNetworkState(uint64_t networkHandle, bool connected,
            uint16_t attributes, const ::std::string& apn) { return true; }

    //IGnssConfiguration
    virtual bool gnssConfigSetBlacklist(
            const std::vector<int>& constellations, const std::vector<int>& svIDs) { return true; }

    //IGnssDebug
    virtual void gnssDebugGetDebugData(getDebugDataCb _hidl_cb) {}

    //IGnssMeasurement
    virtual void gnssMeasurementClose() {}

    //IGnssVisibilityControl
    virtual bool gnssVisibilityCtrlEnableNfwLocationAccess(
            const ::std::vector<::std::string>& proxyApps) { return true; }

    //IGnss
    virtual bool gnssInjectTime(int64_t timeMs, int64_t timeReferenceMs, int32_t uncertaintyMs) {
            return true; }


    /******************************Android specific LocHIDL API********************************/
    //update ILocHidlCallback
    virtual void setIzatSubscriptionCbs(GnssCbBase* callbacks) {}
    virtual void setIzatProviderCbs(GnssCbBase* callbacks) {}
    virtual void setOsNpCbs(GnssCbBase* callbacks) {}
    virtual void setGnssConfigCbs(GnssCbBase* callbacks) {}
    virtual void setIzatConfigCbs(GnssCbBase* callbacks) {}
    virtual void setAGnssExtCbs(GnssCbBase* callbacks) {}
    virtual void setWiFiDBReceiverCbs(GnssCbBase* callbacks) {}
    virtual void setWWanDBReceiverCbs(GnssCbBase* callbacks) {}
    virtual void setWiFiDBProviderCbs(GnssCbBase* callbacks) {}
    virtual void setWWanDBProviderCbs(GnssCbBase* callbacks) {}
    //Izat subscription
    virtual void dataItemsUpdate (::std::list<IDataItemCore *> & dlist) {}
    virtual void dataItemUpdate (IDataItemCore* dataItem) {}
    virtual void izatSubscriptionCleanUp() {}

    //Ril info monitor
    virtual void rilInfoMonitorInit() {}
    virtual void cinfoInject(int32_t cid, int32_t lac, int32_t mnc, int32_t mcc, bool roaming) {}
    virtual void oosInform() {}
    virtual void niSuplInit(::std::string& str) {}
    virtual void chargerStatusInject(int32_t status) {}

    //Izatprovider
    virtual bool onEnable() { return true; }
    virtual bool onDisable() { return true; }
    virtual bool onAddRequest(int providerType, int32_t numFixes, int32_t tbf,
            float displacement, int accuracy) { return true; }
    virtual bool onRemoveRequest(int providerType, int32_t numFixes, int32_t tbf,
            float displacement, int accuracy) { return true; }

    //OsNpGlue
    virtual void OsNpLocationChanged(Location& location) {}

    //XT
    virtual void xtInit() {}
    virtual void setUserPref(bool userPref) {}

    //Gnss config
    virtual void getGnssSvTypeConfig() {}
    virtual void setGnssSvTypeConfig(::std::vector<uint8_t> svTypeVec) {}
    virtual void resetGnssSvTypeConfig() {}
    virtual void getRobustLocationConfig() {}
    virtual void setRobustLocationConfig(bool enable, bool enableForE911) {}
    //Izat config
    virtual bool readIzatConfig() { return true; }
    //AGNSS
    virtual void dataConnOpenExt(uint8_t agpsType, ::std::string apn, int bearerType) {}
    virtual void dataConnClosedExt(uint8_t agpsType) {}
    virtual void dataConnFailedExt(uint8_t agpsType) {}

    //Debug report
    virtual void getDebugReport(int maxReports, loc_core::SystemStatusReports& systemReports) {}

    //Wifi/Wwan DB receiver
    virtual void unregisterWiFiDBUpdater() {}
    virtual void sendAPListRequest(int32_t expireInDays) {}
    virtual void pushWiFiDB(
            ::std::vector<izat_remote_api::WiFiDBUpdater::APLocationData> apLocationData,
            ::std::vector<izat_remote_api::WiFiDBUpdater::APSpecialInfo> apSpecialInfo,
            int32_t daysValid, bool isLookup) {}

    virtual void unregisterWWANDBUpdater() {}
    virtual void sendBSListRequest(int32_t expireInDays) {}
    virtual void pushBSWWANDB(
            ::std::vector<BSLocationData_s> bsLocationDataList,
            ::std::vector<BSSpecialInfo_s> bsSpecialInfoList,
            int32_t daysValid) {}

    virtual void unregisterWiFiDBProvider() {}
    virtual void sendAPObsLocDataRequest() {}

    virtual void unregisterWWANDBProvider() {}
    virtual void sendBSObsLocDataRequest() {}
#endif //FEATURE_EXTERNAL_AP


    /****************************Location Client API specific*********************************/
    virtual void setClientReportsCbs(GnssCbBase* callbacks) {}
    virtual void gnssStartOptions(GnssStartType startType, uint32_t intervalInMs,
            uint32_t distanceInMeters, LocReqEngineTypeMask engMask,
            int gnssReportOptions) {}
    virtual void testGetEnergy() {}
    virtual void testRegSystemInfo() {}
    virtual void testUnregSystemInfo() {}
    virtual void pingTest() {}
    virtual uint16_t getYearOfHw() { return 0; }


    /****************************Location Integration API specific***************************/
    virtual bool configConstrainedTimeUncertainty(bool enable,
                                          float tuncThresholdMs = 0.0,
                                          uint32_t energyBudget = 0) { return true; }

    virtual bool configPositionAssistedClockEstimator(bool enable) { return true; }

    virtual bool deleteAllAidingData() { return true; }

    virtual bool configLeverArm(const LeverArmParamsMap& configInfo) { return true; }

    virtual bool configMinGpsWeek(uint16_t minGpsWeek) { return true; }

    virtual bool getMinGpsWeek() { return true; }

    virtual bool configBodyToSensorMountParams(const BodyToSensorMountParams& b2sParams) {
            return true; }

    virtual bool configMinSvElevation(uint8_t minSvElevation) { return true; }

    virtual bool getMinSvElevation() { return true; }
};
}// namespace garden
#endif //IGNSSAPI_H
