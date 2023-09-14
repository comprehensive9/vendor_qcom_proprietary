/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GnssLocationClientAPI.h"
#include <dlfcn.h>
#include <loc_pla.h>
#include <log_util.h>
#include <fcntl.h>
#include <sstream>
#include "GardenUtil.h"
#include <mutex>
#include <algorithm>

using std::mutex;
using std::lock_guard;
using namespace location_client;
#define NMEA_PORT "/dev/at_usb1"
static int ttyFd = -1;
// debug events counter
static uint32_t numLocationCb = 0;
static uint32_t numGnssLocationCb = 0;
static uint32_t numEngLocationCb = 0;
static uint32_t numGnssSvCb = 0;
static uint32_t numGnssNmeaCb = 0;
static uint32_t numDataCb = 0;
static uint32_t numMeasurementsCb = 0;
static uint32_t numBatchingCb = 0;
static uint32_t numGeofenceCb = 0;
static uint32_t numGfBreachCb = 0;

namespace garden {

using GeofenceBreachTypeMask = ::GeofenceBreachTypeMask;
using Location = ::Location;

static GnssCbBase* sGnssCbs = nullptr;
static GnssCbBase* sFlpCbs = nullptr;
static GnssCbBase* sGeofenceCbs = nullptr;
static std::mutex sGnssLock;
static std::mutex sFlpLock;
static std::mutex sGeofenceLock;
static class GnssLocationClientAPI* mPlocationClient;
static ClientReportsCbs* sClientReportsCbs = nullptr;

static bool openPort(void)
{
    bool retVal = true;

    gardenPrint("opening NMEA port %s ", NMEA_PORT);
    ttyFd = open(NMEA_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (ttyFd == -1) {
        /* Could not open the port. */
        gardenPrint("Unable to open %s \n", NMEA_PORT);
        retVal = false;
    } else {
        gardenPrint("openPort success ttyFd: %d\n", ttyFd);
    }
    return retVal;
}

static bool sendNMEAToTty(const std::string& nmea)
{
    int n;
    char buffer[201] = { 0 };
    bool retVal = true;
    strlcpy(buffer, nmea.c_str(), sizeof(buffer));
    /* NMEA strings contains "\r\n" at end of the string, remove '\r' and send it */
    char* p = strchr(buffer, '\r');
    if ((NULL != p) && (*(p + 1) == '\n')) {
        *p = '\n';
        *(p + 1) = '\0';
    }
    if (1 < nmea.length() && sizeof(buffer) > nmea.length()) {
        n = write(ttyFd, buffer, nmea.length() - 1);
        if (n < 0) {
            gardenPrint("write() of %d bytes failed!\n", n);
            retVal = false;
        } else if (0 == n) {
            gardenPrint("write() of %d bytes returned 0, errno:%d [%s]\n",
                nmea.length(), errno, strerror(errno));
            /* Sleep of 0.1 msec and reattempt to write*/
            usleep(100);
            n = write(ttyFd, buffer, nmea.length() - 1);
            if (n < 0) {
                gardenPrint("reattempt write() failed! errno:%d [%s] \n", errno, strerror(errno));
                retVal = false;
            } else if (0 == n) {
                gardenPrint("reattempt write() of %d bytes returned 0, errno:%d [%s]\n",
                    nmea.length(), errno, strerror(errno));
            }
        }
    } else {
        gardenPrint("Failed to write Len: %d %s \n", nmea.length(), nmea.c_str());
    }
    return true;
}

static void onCapabilitiesCb(location_client::LocationCapabilitiesMask mask) {
    gardenPrint("<<< onCapabilitiesCb mask=0x%" PRIx64, mask);
    gardenPrint("<<< onCapabilitiesCb mask string=%s",
            LocationClientApi::capabilitiesToString(mask).c_str());
}

static void onResponseCb(location_client::LocationResponse response) {
    gardenPrint("<<< onResponseCb err=%u", response);
}

static void convertLocation(const location_client::Location& LCALoc, ::Location& location) {
    location.timestamp = LCALoc.timestamp;
    location.latitude = LCALoc.latitude;
    location.longitude = LCALoc.longitude;
    location.altitude = LCALoc.altitude;
    location.speed = LCALoc.speed;
    location.bearing = LCALoc.bearing;
    location.accuracy = LCALoc.horizontalAccuracy;
    location.verticalAccuracy = LCALoc.verticalAccuracy;
    location.speedAccuracy = LCALoc.speedAccuracy;
    location.bearingAccuracy = LCALoc.bearingAccuracy;
    location.flags = (::LocationFlagsMask)LCALoc.flags;
    location.techMask = (::LocationTechnologyMask)LCALoc.techMask;
}

static void convertGnssLocation(const location_client::GnssLocation& gnssLoc,
        ::Location& location) {
    location.timestamp = gnssLoc.timestamp;
    location.latitude = gnssLoc.latitude;
    location.longitude = gnssLoc.longitude;
    location.altitude = gnssLoc.altitude;
    location.speed = gnssLoc.speed;
    location.bearing = gnssLoc.bearing;
    location.accuracy = gnssLoc.horizontalAccuracy;
    location.verticalAccuracy = gnssLoc.verticalAccuracy;
    location.speedAccuracy = gnssLoc.speedAccuracy;
    location.bearingAccuracy = gnssLoc.bearingAccuracy;
    location.flags = (::LocationFlagsMask)gnssLoc.flags;
    location.techMask = (::LocationTechnologyMask)gnssLoc.techMask;
}

static void convertGnssSv(const std::vector<location_client::GnssSv>& gnssSvs,
        ::GnssSvNotification& svNotify) {

    int i = 0;
    svNotify.count = gnssSvs.size();
    for (auto iter = gnssSvs.begin(); iter != gnssSvs.end(); ++iter) {
        svNotify.gnssSvs[i].size = sizeof(svNotify.gnssSvs);
        svNotify.gnssSvs[i].svId = (*iter).svId;
        svNotify.gnssSvs[i].type = (::GnssSvType)((*iter).type);
        svNotify.gnssSvs[i].cN0Dbhz = (*iter).cN0Dbhz;
        svNotify.gnssSvs[i].elevation = (*iter).elevation;
        svNotify.gnssSvs[i].azimuth = (*iter).azimuth;
        svNotify.gnssSvs[i].gnssSvOptionsMask = (::GnssSvOptionsMask)((*iter).gnssSvOptionsMask);
        svNotify.gnssSvs[i].carrierFrequencyHz = (::GnssSvOptionsMask)((*iter).carrierFrequencyHz);
        svNotify.gnssSvs[i].gnssSignalTypeMask = (::GnssSignalTypeMask)((*iter).gnssSignalTypeMask);
        i++;
    }
}

static GeofenceBreachType translateGeofenceBreachTypeMask(
        location_client::GeofenceBreachTypeMask type) {
    GeofenceBreachType transition = GEOFENCE_BREACH_UNKNOWN;
    switch (type) {
        case location_client::GEOFENCE_BREACH_ENTER_BIT:
            transition = GEOFENCE_BREACH_ENTER;
            break;
        case location_client::GEOFENCE_BREACH_EXIT_BIT:
            transition = GEOFENCE_BREACH_EXIT;
            break;
        case location_client::GEOFENCE_BREACH_DWELL_IN_BIT:
            transition = GEOFENCE_BREACH_DWELL_IN;
            break;
        case location_client::GEOFENCE_BREACH_DWELL_OUT_BIT:
            transition = GEOFENCE_BREACH_DWELL_OUT;
            break;
    }
    return transition;
}

static void locationCb(const location_client::Location& location) {
    numLocationCb++;
    if (mPlocationClient->getCbOutput()) {
        gardenPrint("<<< locationCb cnt=%u time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f",
            numLocationCb,
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
    }
    if (sGnssCbs != nullptr) {
        Location loc;
        convertLocation(location, loc);
        sGnssCbs->gnssLocationCb(loc);
    }
}

static void printGnssLocation(const location_client::GnssLocation& gnssLocation) {
    gardenPrint("<<< GNSS location: time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f "
                "info mask=0x%x eng type %d, eng mask 0x%x, conformityIndex %f, "
                "llaVrpbased (%f %f %f), enu velocity (%f %f %f), "
                "enu velocity vrp based (%f %f %f)",
                gnssLocation.timestamp,
                gnssLocation.flags,
                gnssLocation.latitude,
                gnssLocation.longitude,
                gnssLocation.altitude,
                gnssLocation.gnssInfoFlags,
                gnssLocation.locOutputEngType,
                gnssLocation.locOutputEngMask,
                gnssLocation.conformityIndex,
                gnssLocation.llaVRPBased.latitude,
                gnssLocation.llaVRPBased.longitude,
                gnssLocation.llaVRPBased.altitude,
                gnssLocation.eastVelocity,
                gnssLocation.northVelocity,
                gnssLocation.upVelocity,
                gnssLocation.enuVelocityVRPBased[0],
                gnssLocation.enuVelocityVRPBased[1],
                gnssLocation.enuVelocityVRPBased[2]);
    gardenPrint("<<< GNSS location: body frame data mask=0x%x, roll %f, roll rate %f, "
                "pitch %f, pitch rate %f, yaw %f, yaw rate %f",
                gnssLocation.bodyFrameData.bodyFrameDataMask,
                gnssLocation.bodyFrameData.roll,
                gnssLocation.bodyFrameData.rollRate,
                gnssLocation.bodyFrameData.pitch,
                gnssLocation.bodyFrameData.pitchRate,
                gnssLocation.bodyFrameData.yaw,
                gnssLocation.bodyFrameData.yawRate);
}

static void gnssLocationCb(const location_client::GnssLocation& gnssLocation) {
    numGnssLocationCb++;
    if (mPlocationClient->getCbOutput()) {
        gardenPrint("<<< gnssLocationCb numCbs = %d ", numGnssLocationCb);
        printGnssLocation(gnssLocation);
    }

    if (mPlocationClient->getAutoMode()) {
        if (sClientReportsCbs) {
            sClientReportsCbs->locationReceivedCallback();
        }
    }
    if (sGnssCbs != nullptr) {
        Location loc;
        convertGnssLocation(gnssLocation, loc);
        sGnssCbs->gnssLocationCb(loc);
    }
}

static void gnssSvCb(const std::vector<location_client::GnssSv>& gnssSvs) {
    numGnssSvCb++;
    if (mPlocationClient->getCbOutput()) {
        stringstream ss;
        ss << "<<< gnssSvCb c=" << numGnssSvCb << " s=" << gnssSvs.size();
        for (auto sv : gnssSvs) {
            ss << " " << sv.type << ":" << sv.svId << "/" << (uint32_t)sv.cN0Dbhz;
        }
        string s = ss.str();
        gardenPrint("%s", s.c_str());
    }
    if (sGnssCbs != nullptr) {
        ::GnssSvNotification svNotify;
        convertGnssSv(gnssSvs, svNotify);
        sGnssCbs->gnssSvStatusCb(svNotify);
    }
}

static void gnssNmeaCb(uint64_t timestamp, const std::string& nmea) {
    numGnssNmeaCb++;
    if (mPlocationClient->getCbOutput()) {
        gardenPrint("<<< gnssNmeaCb cnt=%u time=%" PRIu64" nmea=%s",
                numGnssNmeaCb, timestamp, nmea.c_str());
    }

    if (mPlocationClient->getRouteToNmeaPort()) {
        if (-1 == ttyFd) {
            if (openPort()) {
                sendNMEAToTty(nmea);
            }
        } else {
            sendNMEAToTty(nmea);
        }
    }
}

static void gnssEnergyConsumedInfoCb(const GnssEnergyConsumedInfo& gnssEneryConsumed) {
    gardenPrint("<<< onGnssEnergyConsumedInfoCb energy: (valid=%d, value=%" PRIu64"",
            (gnssEneryConsumed.flags & ENERGY_CONSUMED_SINCE_FIRST_BOOT_BIT) != 0,
            gnssEneryConsumed.totalEnergyConsumedSinceFirstBoot);
}

static void gnssEnergyConsumedResponseCb(location_client::LocationResponse response) {
    gardenPrint("<<< onGnssEnergyConsumedResponseCb err=%u\n", response);
}

static void locationSystemInfoCb(const location_client::LocationSystemInfo& systemInfo) {
    gardenPrint("<<< onLocationSystemInfoCb: "
        "(system info mask=0x%x, leap second info mask=0x%x"
        " gps timestamp of leap second change (%d week, %d msec), "
        " leap seconds before change %d, leap seconds after change %d,"
        " leap second current %d",
        systemInfo.systemInfoMask,
        systemInfo.leapSecondSysInfo.leapSecondInfoMask,
        systemInfo.leapSecondSysInfo.leapSecondChangeInfo.gpsTimestampLsChange.systemWeek,
        systemInfo.leapSecondSysInfo.leapSecondChangeInfo.gpsTimestampLsChange.systemMsec,
        systemInfo.leapSecondSysInfo.leapSecondChangeInfo.leapSecondsBeforeChange,
        systemInfo.leapSecondSysInfo.leapSecondChangeInfo.leapSecondsAfterChange,
        systemInfo.leapSecondSysInfo.leapSecondCurrent);
}

static void locationSystemInfoResponseCb(location_client::LocationResponse response) {
    gardenPrint("<<< onLocationSystemInfoCb err=%u\n", response);
}

static void gnssDataCb(const location_client::GnssData& gnssData) {
    numDataCb++;
    if (mPlocationClient->getCbOutput()) {
        gardenPrint("<<< gnssDataCb cnt=%u mPlocationClient=%p", numDataCb, mPlocationClient);
        for (int sig = GNSS_LOC_SIGNAL_TYPE_GPS_L1CA;
            sig < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES; sig++) {
            if (0 != gnssData.gnssDataMask[sig]) {
                gardenPrint("gnssDataMask[%d]=0x%X", sig, gnssData.gnssDataMask[sig]);
                gardenPrint("jammerInd[%d]=%f", sig, gnssData.jammerInd[sig]);
                gardenPrint("agc[%d]=%f", sig, gnssData.agc[sig]);
            }
        }
    }
}

static void gnssMeasurementsCb(const location_client::GnssMeasurements& gnssMeasurements) {
    numMeasurementsCb++;
    if (mPlocationClient->getCbOutput()) {
        gardenPrint("<<< gnssMeasurementsCb cnt=%u mPlocationClient=%p",
                    numMeasurementsCb,
                    mPlocationClient);
        gardenPrint(" Clocks Info");
        gardenPrint(" time_ns: %" PRId64 " full_bias_ns: %" PRId64 ""
            " bias_ns: %f, bias_uncertainty_ns: %f,"
            " drift_nsps: %f, drift_uncertainty_nsps: %f,"
            " hw_clock_discontinuity_count: %d, flags: 0x%04x",
            gnssMeasurements.clock.timeNs,
            gnssMeasurements.clock.fullBiasNs,
            gnssMeasurements.clock.biasNs,
            gnssMeasurements.clock.biasUncertaintyNs,
            gnssMeasurements.clock.driftNsps,
            gnssMeasurements.clock.driftUncertaintyNsps,
            gnssMeasurements.clock.hwClockDiscontinuityCount,
            gnssMeasurements.clock.flags);
        for (int i = 0;
             i < gnssMeasurements.measurements.size() && i < GNSS_MEASUREMENTS_MAX; i++) {
            gardenPrint("%02d : Const: %d,  svid: %d,"
                " time_offset_ns: %f, state %d,"
                " c_n0_dbhz: %f, c_freq_Hz: %f,"
                " pseudorange_rate_mps: %f,"
                " pseudorange_rate_uncertainty_mps: %f,"
                " agcLevelDb: %f, flags: 0x%08x",
                i + 1,
                gnssMeasurements.measurements[i].svType,
                gnssMeasurements.measurements[i].svId,
                gnssMeasurements.measurements[i].timeOffsetNs,
                gnssMeasurements.measurements[i].stateMask,
                gnssMeasurements.measurements[i].carrierToNoiseDbHz,
                gnssMeasurements.measurements[i].carrierFrequencyHz,
                gnssMeasurements.measurements[i].pseudorangeRateMps,
                gnssMeasurements.measurements[i].pseudorangeRateUncertaintyMps,
                gnssMeasurements.measurements[i].agcLevelDb,
                gnssMeasurements.measurements[i].flags);
        }
    }
}

static void engLocationsCb(const std::vector<location_client::GnssLocation>& locations) {
    numEngLocationCb++;
    if (mPlocationClient->getCbOutput()) {
        gardenPrint("<<< engLocationsCb numCbs = %d, numOfReports =%d",
                    numEngLocationCb, locations.size());
        for (auto location : locations) {
            printGnssLocation(location);
        }
    }

    if (mPlocationClient->getAutoMode()) {
        if (sClientReportsCbs) {
            sClientReportsCbs->locationReceivedCallback();
        }
    }
}

static void pingTestCb(uint32_t response) {
    if (sClientReportsCbs) {
        sClientReportsCbs->pingReceivedCallback();
    }
}

static void batchingCb(const std::vector<location_client::Location>& locations,
    location_client::BatchingStatus status) {
    numBatchingCb++;
    gardenPrint("<<< onBatchingCb batchingStatus=%d", status);
    if (location_client::BATCHING_STATUS_ACTIVE == status) {
        for (auto each : locations) {
            gardenPrint("<<< batchingCb cnt=%d time=%" PRIu64 " mask=0x%x lat=%f lon=%f alt=%f",
                numBatchingCb,
                each.timestamp,
                each.flags,
                each.latitude,
                each.longitude,
                each.altitude);
        }
    }
    if (sFlpCbs != nullptr) {
        Location* locationArr = new Location[locations.size()];
        for (int i=0; i<locations.size(); ++i) {
            Location loc;
            convertLocation(locations[i], loc);
            locationArr[i] = loc;
        }
        sFlpCbs->batchingCb(locations.size(), locationArr);
        delete[] locationArr;
    }
}

static void onGeofenceBreachCb(const std::vector<Geofence>& geofences,
    location_client::Location location, location_client::GeofenceBreachTypeMask type,
    uint64_t timestamp) {
    numGfBreachCb++;
    gardenPrint("<<< onGeofenceBreachCallback, cnt=%u, breachType=%d", numGfBreachCb, type);
    gardenPrint("<<< time=%" PRIu64" lat=%f lon=%f alt=%f",
        timestamp,
        location.latitude,
        location.longitude,
        location.altitude);
    if (sGeofenceCbs != nullptr) {
        Location loc;
        convertLocation(location, loc);
        GeofenceBreachType transition = translateGeofenceBreachTypeMask(type);
        sGeofenceCbs->geofenceBreachCb(-1, loc, transition, timestamp);
    }
}

static void onCollectiveResponseCb(std::vector<pair<Geofence, LocationResponse>>& responses) {
    numGeofenceCb++;
    for (int i = 0; i<responses.size(); ++i) {
        gardenPrint("<<< onCollectiveResponseCb cnt=%u lat=%f lon=%f radius=%f, response=%u",
            numGeofenceCb,
            responses[i].first.getLatitude(),
            responses[i].first.getLongitude(),
            responses[i].first.getRadius(),
            responses[i].second);
    }
}

static void onConfigResponseCb(location_integration::LocConfigTypeEnum    requestType,
                               location_integration::LocIntegrationResponse response) {
    gardenPrint("<<< onConfigResponseCb, type %d, err %d\n", requestType, response);
}

void GnssLocationClientAPI::setGnssCbs(GnssCbBase* callbacks) {
    std::lock_guard<std::mutex> guard(sGnssLock);
    sGnssCbs = callbacks;
}

void GnssLocationClientAPI::setFlpCbs(GnssCbBase* callbacks) {
    std::lock_guard<std::mutex> guard(sFlpLock);
    sFlpCbs = callbacks;
}

void GnssLocationClientAPI::setGeofenceCbs(GnssCbBase* callbacks) {
    std::lock_guard<std::mutex> guard(sGeofenceLock);
    sGeofenceCbs = callbacks;
    numGeofenceCb = 0;
    numGfBreachCb = 0;
}

GnssLocationClientAPI::GnssLocationClientAPI() :
    mStartType(GNSS_REPORT),
    mIntervalInMs(1000),
    mDistanceInMeters(0),
    mEngMask(::LOC_REQ_ENGINE_FUSED_BIT),
    mEnableOutput(true),
    mAutoMode(false),
    mRouteToNmeaPort(false) {
    mPClient = new LocationClientApi(onCapabilitiesCb);
    mPClientImpl = new LocationClientApiImpl(onCapabilitiesCb);
    mPlocationClient = this;
    memset(&mReportCbs, 0, sizeof(mReportCbs));
    mGeofences.clear();

    mIntApiCbs.configCb = LocConfigCb(onConfigResponseCb);
    mPIntClient = new LocationIntegrationApi(mPriorityMap, mIntApiCbs);
}

GnssLocationClientAPI::~GnssLocationClientAPI() {
    if (mPClient) {
        delete mPClient;
        mPClient = nullptr;
    }
    if (mPIntClient) {
        delete mPIntClient;
        mPIntClient = nullptr;
    }
    if (mPClientImpl) {
        mPClientImpl->destroy();
    }
}

int GnssLocationClientAPI::gnssStart() {
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;

    numGnssSvCb = 0;
    numGnssNmeaCb = 0;
    // start tracking session
    switch (mStartType) {
    case LOCATION_REPORT:
        numLocationCb = 0;
        numGnssLocationCb = 0;
        retVal = mPClient->startPositionSession(mIntervalInMs, mDistanceInMeters,
                locationCb, onResponseCb);
        break;
    case ENGINE_REPORT:
        numEngLocationCb = 0;
        retVal = mPClient->startPositionSession(mIntervalInMs,
                (location_client::LocReqEngineTypeMask)mEngMask,
                mEngineReportCbs, onResponseCb);
        break;
    case GNSS_REPORT:
    default:
        numLocationCb = 0;
        numGnssLocationCb = 0;
        memset(&mReportCbs, 0, sizeof(mReportCbs));
        mReportCbs.gnssLocationCallback = GnssLocationCb(gnssLocationCb);
        mReportCbs.gnssSvCallback = GnssSvCb(gnssSvCb);
        retVal = mPClient->startPositionSession(mIntervalInMs, mReportCbs, onResponseCb);
        break;
    }
    return retVal;
}

int GnssLocationClientAPI::gnssStop() {
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;
    mPClient->stopPositionSession();
    return retVal;
}

void GnssLocationClientAPI::gnssCleanup() {
}

int GnssLocationClientAPI::gnssInjectLocation(double latitude, double longitude,
        float accuracy) {
    ENTRY_LOG_CALLFLOW();
    return 0;
}

void GnssLocationClientAPI::gnssDeleteAidingData(LocGpsAidingData flags) {
    LOC_LOGD("%s]: (%02x)", __func__, flags);

    mPIntClient->deleteAllAidingData();
}

int GnssLocationClientAPI::gnssSetPositionMode(LocGpsPositionMode mode,
        LocGpsPositionRecurrence recurrence, uint32_t min_interval,
        uint32_t preferred_accuracy, uint32_t preferred_time, bool lowPowerMode) {
    LOC_LOGD("%s]: (%d %d %d %d %d)", __func__,
            mode, recurrence, min_interval, preferred_accuracy, preferred_time);

    return 0;
}

void GnssLocationClientAPI::configurationUpdate(const char* config_data, int32_t length) {
    LOC_LOGD("%s]: (%s %d)", __func__, config_data, length);
}

void GnssLocationClientAPI::updateNetworkAvailability(int available, const char* /*apn*/) {
    mPClient->updateNetworkAvailability(available);
}

void GnssLocationClientAPI::gnssNiResponse(uint32_t id, GnssNiResponse response) {
}

int GnssLocationClientAPI::gnssInjectBestLocation(Location location) {
    ENTRY_LOG_CALLFLOW();
    return 0;
}

//flp
int GnssLocationClientAPI::flpStartSession(uint32_t mode, LocationOptions& options, uint32_t id) {
    gardenPrint("%s ()]:, id: %u, mode: %d, size: %d, minInterval: %d, minDistance: %d,"
           " suplMode: %d", __func__, id, mode, options.size, options.minInterval,
            options.minDistance, options.mode);

    numBatchingCb = 0;
    return mPClient->startRoutineBatchingSession(
        options.minInterval, options.minDistance, batchingCb, onResponseCb);
}

int GnssLocationClientAPI::flpStopSession(uint32_t id) {
    mPClient->stopBatchingSession();
    return 0;
}

int GnssLocationClientAPI::flpGetBatchSize() {
    int batchSize = 0;
    gardenPrint("calling %s...", __func__);
    return 0; // TODO
}

int GnssLocationClientAPI::flpUpdateSession(uint32_t sessionMode, LocationOptions& options, uint32_t id) {
    LOC_LOGD("%s]: (%u %ld %d)", __FUNCTION__,
            id, options.minInterval*1000000L, sessionMode);
    return 0; // TODO
}

int GnssLocationClientAPI::flpGetBatchedLocation(int lastNLocations, uint32_t id) {
    LOC_LOGD("%s]: (%u %d)", __FUNCTION__, id, lastNLocations);
    return 0; // TODO
}

void GnssLocationClientAPI::flpCleanUp() {
    LOC_LOGD("%s]:", __FUNCTION__);
}

//geofence
void GnssLocationClientAPI::addGeofence(uint32_t geofenceId, double latitudeDegrees,
            double longitudeDegrees, double radiusMeters,
            GeofenceBreachTypeMask monitorTransitions,
            uint32_t notificationResponsivenessMs,
            uint32_t dwellTimeSeconds) {
    LOC_LOGd("Lat:%f Lon:%f Rad:%f BreachTypeMask:%d NotifRespMs%d DwellTimeSec:%d",
            latitudeDegrees, longitudeDegrees, radiusMeters, monitorTransitions,
            notificationResponsivenessMs, dwellTimeSeconds);

    vector<Geofence> addGf;
    Geofence gf(latitudeDegrees, longitudeDegrees,
                radiusMeters,
                (location_client::GeofenceBreachTypeMask)monitorTransitions,
                notificationResponsivenessMs, dwellTimeSeconds);
    addGf.push_back(gf);
    mPClient->addGeofences(addGf, onGeofenceBreachCb, onCollectiveResponseCb);
    // push the new entry to master list. Need to add this after addGeofences
    // call, since GeofenceImpl is created inside addGeofences api.
    // Get the updatedGf from addGf
    mGeofences.push_back(addGf[0]);
}

void GnssLocationClientAPI::removeGeofence(uint32_t geofenceId) {
    LOC_LOGd("Gf id: %d", geofenceId);

    if (geofenceId > 0 && geofenceId <= mGeofences.size()) {
        vector<Geofence> removeGf;
        int id = geofenceId - 1;
        removeGf.push_back(mGeofences[id]);
        gardenPrint("Remove Gf - Lat:%f Lon: %f Rad:%f", mGeofences[id].getLatitude(),
            mGeofences[id].getLongitude(), mGeofences[id].getRadius());
        mPClient->removeGeofences(removeGf);

        // remove from master list
        mGeofences.erase(mGeofences.begin()+id);
    } else {
        gardenPrint("Remove Gf - Invalid Gf id: %d", geofenceId);
    }
}

void GnssLocationClientAPI::modifyGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitorTransitions,
        uint32_t notificationResponsivenessMs,
        uint32_t dwellTimeSeconds) {

    if (geofenceId > 0 && geofenceId <= mGeofences.size()) {
        vector<Geofence> modifyGf;
        int id = geofenceId - 1;

        LOC_LOGd("Modify Gf - id: %d,  %d %d %d )", geofenceId,
                monitorTransitions, notificationResponsivenessMs,
                dwellTimeSeconds);

        gardenPrint("Modify Gf - id: %d,  %d %d %d )", geofenceId,
                monitorTransitions, notificationResponsivenessMs,
                dwellTimeSeconds);

        mGeofences[id].setBreachType(
                (location_client::GeofenceBreachTypeMask)monitorTransitions);
        mGeofences[id].setResponsiveness(notificationResponsivenessMs);
        mGeofences[id].setDwellTime(dwellTimeSeconds);
        modifyGf.push_back(mGeofences[id]);

        mPClient->modifyGeofences(modifyGf);
    } else {
        gardenPrint("Modify Gf - Invalid Gf id: %d", geofenceId);
    }
}

void GnssLocationClientAPI::pauseGeofence(uint32_t geofenceId) {
    LOC_LOGd("Gf id: %d", geofenceId);

    if (geofenceId > 0 && geofenceId <= mGeofences.size()) {
        vector<Geofence> pauseGf;
        int id = geofenceId - 1;
        pauseGf.push_back(mGeofences[id]);
        gardenPrint("Pause Gf - Lat:%f Lon: %f Rad:%f", mGeofences[id].getLatitude(),
            mGeofences[id].getLongitude(), mGeofences[id].getRadius());
        mPClient->pauseGeofences(pauseGf);
    } else {
        gardenPrint("Pause Gf - Invalid Gf id: %d", geofenceId);
    }
}

void GnssLocationClientAPI::resumeGeofence(uint32_t geofenceId,
        GeofenceBreachTypeMask monitorTransitions) {
    LOC_LOGd("(%d %d)", geofenceId, monitorTransitions);
    vector<Geofence> resumeGf;

    if (geofenceId > 0 && geofenceId <= mGeofences.size()) {
        int id = geofenceId - 1;
        resumeGf.push_back(mGeofences[id]);
        gardenPrint("Resume Gf - Lat:%f Lon: %f Rad:%f", mGeofences[id].getLatitude(),
            mGeofences[id].getLongitude(), mGeofences[id].getRadius());
        mPClient->resumeGeofences(resumeGf);
    } else {
        gardenPrint("Resume Gf - Invalid Gf id: %d", geofenceId);
    }
}

void GnssLocationClientAPI::removeAllGeofences() {
    LOC_LOGd("Remove All geofences");
}

uint32_t GnssLocationClientAPI::listAllActiveGeofences() {
    if (0 == mGeofences.size()) {
        gardenPrint("No active geofences !!!!!");
        return 0;
    }
    else {
        gardenPrint("Listing all active geofences");
        gardenPrint("============================");
        gardenPrint("GfId    Lat    Long    Radius    Respons    Dwell");
        int i = 1;
        for (auto gf : mGeofences) {
            gardenPrint("%d    %f    %f    %f    %d    %d", i, gf.getLatitude(),
                    gf.getLongitude(), gf.getRadius(), gf.getResponsiveness(), gf.getDwellTime());
            i++;
        }
        return mGeofences.size();
    }
}

void GnssLocationClientAPI::gnssStartOptions(GnssStartType startType,
        uint32_t intervalInMs, uint32_t distanceInMeters,
        ::LocReqEngineTypeMask engMask, int gnssReportOptions) {

    mStartType = startType;
    mIntervalInMs = intervalInMs;
    mDistanceInMeters = distanceInMeters;
    mEngMask = engMask;

    if (GNSS_REPORT == mStartType) {
        memset(&mReportCbs, 0, sizeof(mReportCbs));
        if (gnssReportOptions & LOCATION_BIT) {
            mReportCbs.gnssLocationCallback = GnssLocationCb(gnssLocationCb);
        }
        if (gnssReportOptions & GNSS_SV_BIT) {
            mReportCbs.gnssSvCallback = GnssSvCb(gnssSvCb);
        }
        if (gnssReportOptions & GNSS_NMEA_BIT) {
            mReportCbs.gnssNmeaCallback = GnssNmeaCb(gnssNmeaCb);
        }
        if (gnssReportOptions & GNSS_DATA_BIT) {
            numDataCb = 0;
            mReportCbs.gnssDataCallback = GnssDataCb(gnssDataCb);
        }
        if (gnssReportOptions & GNSS_MEAS_BIT) {
            numMeasurementsCb = 0;
            mReportCbs.gnssMeasurementsCallback = GnssMeasurementsCb(gnssMeasurementsCb);
        }
    } else if (ENGINE_REPORT == mStartType) {
        memset(&mEngineReportCbs, 0, sizeof(mEngineReportCbs));
        if (gnssReportOptions & LOCATION_BIT) {
            mEngineReportCbs.engLocationsCallback = EngineLocationsCb(engLocationsCb);
        }
        if (gnssReportOptions & GNSS_SV_BIT) {
            mEngineReportCbs.gnssSvCallback = GnssSvCb(gnssSvCb);
        }
        if (gnssReportOptions & GNSS_NMEA_BIT) {
            mEngineReportCbs.gnssNmeaCallback = GnssNmeaCb(gnssNmeaCb);
        }
        if (gnssReportOptions & GNSS_DATA_BIT) {
            numDataCb = 0;
            mEngineReportCbs.gnssDataCallback = GnssDataCb(gnssDataCb);
        }
        if (gnssReportOptions & GNSS_MEAS_BIT) {
            numMeasurementsCb = 0;
            mEngineReportCbs.gnssMeasurementsCallback = GnssMeasurementsCb(gnssMeasurementsCb);
        }
    }
}

uint32_t GnssLocationClientAPI::gnssDeleteAidingData(GnssAidingData& data) {
    mPIntClient->deleteAllAidingData();
    return 0;
}

void GnssLocationClientAPI::testGetEnergy() {
    mPClient->getGnssEnergyConsumed(gnssEnergyConsumedInfoCb,
            gnssEnergyConsumedResponseCb);
}

void GnssLocationClientAPI::testRegSystemInfo() {
    mPClient->updateLocationSystemInfoListener(locationSystemInfoCb,
            locationSystemInfoResponseCb);
}

void GnssLocationClientAPI::testUnregSystemInfo() {
    mPClient->updateLocationSystemInfoListener(nullptr,
            nullptr);
}

void GnssLocationClientAPI::pingTest() {
    mPClientImpl->pingTest(pingTestCb);
}

uint16_t GnssLocationClientAPI::getYearOfHw() {
    return mPClient->getYearOfHw();
}

void GnssLocationClientAPI::setClientReportsCbs(ClientReportsCbs* callbacks) {
    sClientReportsCbs = callbacks;
}

} // namespace garden
