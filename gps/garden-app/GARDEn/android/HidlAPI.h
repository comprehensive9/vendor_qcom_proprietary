/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef HIDLAPI_H
#define HIDLAPI_H

#include "IGnssAPI.h"
#include <LocationAPI.h>
#ifdef LOCHIDL
#include "DataItemConcreteTypes.h"
#include "IDataItemCore.h"
#include "SystemStatus.h"
#include "IzatWiFiDBReceiver.h"
#include "IzatWiFiDBProvider.h"
#include "wwan_db_provider.h"
#include "wwan_db_receiver.h"
#endif

namespace garden {

//using getDebugData_2_0_cb = std::function<void(GnssDebugReport& debugData)>;
class HidlAPI : public IGnssAPI {
public:
    HidlAPI();
    virtual ~HidlAPI() = default;
    void setGnssCbs(GnssCbBase* callbacks) override;
    void setFlpCbs(GnssCbBase* callbacks) override;
    void setGeofenceCbs(GnssCbBase* callbacks) override;
#ifdef LOCHIDL
    void setIzatSubscriptionCbs(GnssCbBase* callbacks) override;
    void setIzatProviderCbs(GnssCbBase* callbacks) override;
    void setOsNpCbs(GnssCbBase* callbacks) override;
    void setGnssConfigCbs(GnssCbBase* callbacks) override;
    void setIzatConfigCbs(GnssCbBase* callbacks) override;
    void setAGnssExtCbs(GnssCbBase* callbacks) override;
    void setWiFiDBReceiverCbs(GnssCbBase* callbacks) override;
    void setWWanDBReceiverCbs(GnssCbBase* callbacks) override;
    void setWiFiDBProviderCbs(GnssCbBase* callbacks) override;
    void setWWanDBProviderCbs(GnssCbBase* callbacks) override;
#endif

    int gnssStart(void) override;
    int gnssStop(void) override;
    void gnssCleanup(void) override;
    int gnssInjectLocation(double latitude, double longitude, float accuracy) override;
    void gnssDeleteAidingData(LocGpsAidingData flags) override;
    int gnssSetPositionMode(LocGpsPositionMode mode,
            LocGpsPositionRecurrence recurrence, uint32_t min_interval,
            uint32_t preferred_accuracy, uint32_t preferred_time, bool lowPowerMode) override;
    void configurationUpdate(const char* config_data, int32_t length) override;
    void updateNetworkAvailability(int available, const char* apn) override;

    void gnssNiResponse(uint32_t id, GnssNiResponse response) override;
    int gnssInjectBestLocation(Location location) override;

    //flp
    int flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id=1) override;
    int flpStopSession(uint32_t id=1) override;
    int flpGetBatchSize() override;
    int flpUpdateSession(uint32_t /*sessionMode*/, LocationOptions& /*options*/,
            uint32_t id=1) override;
    int flpGetBatchedLocation(int /*lastNLocations*/, uint32_t id=1) override;
    void flpCleanUp() override;

    // geofence
    void addGeofence(uint32_t geofenceId, double latitudeDegrees, double longitudeDegrees,
            double radiusMeters, GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs,
            uint32_t dwellTimeSeconds = 0) override;
    void removeGeofence(uint32_t geofenceId) override;
    void modifyGeofence(uint32_t /*geofenceId*/,
            GeofenceBreachTypeMask /*monitorTransitions*/,
            uint32_t /*notificationResponsivenessMs*/,
            uint32_t dwellTimeSeconds = 0) override;
    void pauseGeofence(uint32_t geofenceId) override;
    void resumeGeofence(uint32_t geofenceId,
            GeofenceBreachTypeMask /*monitorTransitions*/) override;
    void removeAllGeofences() override;

    void setAGnssCbs(GnssCbBase* callbacks) override;
    void setAGnssRilCbs(GnssCbBase* callbacks) override;
    void setGnssMeasurementCbs(GnssCbBase* callbacks) override;
    void setGnssVisibilityControlCbs(GnssCbBase* callbacks) override;

    bool agnssDataConnClosed() override;
    bool agnssDataConnFailed() override;
    bool agnssSetServer(int32_t type, const std::string& hostname, int32_t port) override;
    bool agnssDataConnOpen(uint64_t networkHandle, const std::string& apn,
            int32_t apnIpType) override;

    //IAGnssRil
    void agnssRilSetRefLocation(int type, int mcc, int mnc, int lac, int cid) override;
    bool agnssRilSetSetId(int type, const std::string& setid) override;
    bool agnssRilUpdateNetworkAvailability(bool available, const std::string& apn) override;
    bool agnssRilUpdateNetworkState(uint64_t networkHandle, bool connected,
            uint16_t attributes, const std::string& apn) override;

    //IGnssConfiguration
    bool gnssConfigSetBlacklist(
            const std::vector<int>& constellations, const std::vector<int>& svIDs) override;

    //IGnssDebug
    void gnssDebugGetDebugData(getDebugDataCb _hidl_cb) override;

    //IGnssMeasurement
    void gnssMeasurementClose() override;

    //IGnssVisibilityControl
    bool gnssVisibilityCtrlEnableNfwLocationAccess(
            const std::vector<std::string>& proxyApps) override;

    //IGnss
    bool gnssInjectTime(int64_t timeMs, int64_t timeReferenceMs, int32_t uncertaintyMs) override;

#ifdef LOCHIDL
    //LOC HIDL APIs
    //Izat subscription
    void dataItemsUpdate (std::list<IDataItemCore *> & dlist) override;
    void dataItemUpdate (IDataItemCore* dataItem) override;
    void izatSubscriptionCleanUp() override;

    //Ril info monitor
    void rilInfoMonitorInit() override;
    void cinfoInject(int32_t cid, int32_t lac, int32_t mnc, int32_t mcc, bool roaming) override;
    void oosInform() override;
    void niSuplInit(std::string& str) override;
    void chargerStatusInject(int32_t status) override;

    //Izatprovider
    bool onEnable() override;
    bool onDisable() override;
    bool onAddRequest(int providerType, int32_t numFixes, int32_t tbf,
            float displacement, int accuracy) override;
    bool onRemoveRequest(int providerType, int32_t numFixes, int32_t tbf,
            float displacement, int accuracy) override;

    //OsNpGlue
    void OsNpLocationChanged(Location& location) override;

    //XT
    void xtInit() override;
    void setUserPref(bool userPref) override;

    //Gnss config
    void getGnssSvTypeConfig() override;
    void setGnssSvTypeConfig(std::vector<uint8_t> svTypeVec) override;
    void resetGnssSvTypeConfig() override;
    void getRobustLocationConfig() override;
    void setRobustLocationConfig(bool enable, bool enableForE911) override;
    //Izat config
    bool readIzatConfig() override;
    //AGNSS
    void dataConnOpenExt(uint8_t agpsType, std::string apn, int bearerType) override;
    void dataConnClosedExt(uint8_t agpsType) override;
    void dataConnFailedExt(uint8_t agpsType) override;

    //Debug report
    void getDebugReport(int maxReports, loc_core::SystemStatusReports& systemReports) override;

    //Wifi/Wwan DB receiver
    void unregisterWiFiDBUpdater() override;
    void sendAPListRequest(int32_t expireInDays) override;
    void pushWiFiDB(std::vector<izat_remote_api::WiFiDBUpdater::APLocationData> apLocationData,
            std::vector<izat_remote_api::WiFiDBUpdater::APSpecialInfo> apSpecialInfo,
            int32_t daysValid, bool isLookup) override;

    void unregisterWWANDBUpdater() override;
    void sendBSListRequest(int32_t expireInDays) override;
    void pushBSWWANDB(
            std::vector<BSLocationData_s> bsLocationDataList,
            std::vector<BSSpecialInfo_s> bsSpecialInfoList,
            int32_t daysValid) override;

    void unregisterWiFiDBProvider() override;
    void sendAPObsLocDataRequest() override;

    void unregisterWWANDBProvider() override;
    void sendBSObsLocDataRequest() override;
#endif
private:
    GnssCbBase* mGnssCallbacks;
    GnssCbBase* mFlpCbs;
    GnssCbBase* mGeofenceCbs;

};

} // namespace garden

#endif //GNSSLOCATION_H
