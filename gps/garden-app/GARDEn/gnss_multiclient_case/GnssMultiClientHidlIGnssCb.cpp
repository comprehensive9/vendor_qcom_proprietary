/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_HidlIGnssCb"

#include "GnssMultiClientHidlIGnssCb.h"
#include "GnssMultiClientCaseUtils.h"
#include "GnssMultiClientHidlIGnss.h"

namespace garden {

GnssMultiClientHidlIGnssCb::GnssMultiClientHidlIGnssCb(
        GnssMultiClientHidlIGnss* hidlGnss)
{
    mHidlGnss = hidlGnss;
}

GnssMultiClientHidlIGnssCb::~GnssMultiClientHidlIGnssCb() {}

Return<void>
GnssMultiClientHidlIGnssCb::gnssMeasurementCb(
        const IGnssMeasurementCallback_V1_1::GnssData& )
{
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::GnssMeasurementCb(
        const IGnssMeasurementCallback_V1_0::GnssData& )
{
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssMeasurementCb_2_0(const GnssData_V2_0& data) {
    CONDPRINTLN("## %s]: () ##, num of measurements: %zu", __FUNCTION__, data.measurements.size());
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssMeasurementCb_2_1(const GnssData_V2_1& data) {
    /*
    1.0
    bitfield<GnssMeasurementFlags> flags;   OBSOLOTE, see 2.1
    int16_t svid;
    GnssConstellationType constellation; OBSOLOTE, see 2.0
    double timeOffsetNs;
    bitfield<GnssMeasurementState> state;  OBSOLOTE, see 2.0
    int64_t receivedSvTimeInNs;
    int64_t receivedSvTimeUncertaintyInNs;
    double cN0DbHz;
    double pseudorangeRateMps;
    double pseudorangeRateUncertaintyMps;
    bitfield<GnssAccumulatedDeltaRangeState> accumulatedDeltaRangeState; OBSOLOTE, see 1.1
    double accumulatedDeltaRangeM;
    double accumulatedDeltaRangeUncertaintyM;
    float carrierFrequencyHz;
    int64_t carrierCycles;  OBSOLOTE
    double carrierPhase;    OBSOLOTE
    double carrierPhaseUncertainty; OBSOLOTE
    GnssMultipathIndicator multipathIndicator;
    double snrDb;
    double agcLevelDb;

    1.1
    bitfield<GnssAccumulatedDeltaRangeState> accumulatedDeltaRangeState;

    2.0
    string codeType;
    bitfield<GnssMeasurementState> state;
    GnssConstellationType constellation;

    2.1
    bitfield<GnssMeasurementFlags> flags;
    double fullInterSignalBiasNs;
    double fullInterSignalBiasUncertaintyNs;
    double satelliteInterSignalBiasNs;
    double satelliteInterSignalBiasUncertaintyNs;
    double basebandCN0DbHz;
    */

    CONDPRINTLN("## %s]: () ##, num of measurements: %zu", __FUNCTION__, data.measurements.size());
    for (int i = 0; i < data.measurements.size(); i++) {
        gardenPrint("%02d : flags: 0x%08x, svid: %d,"
                    " Const: %u, timeOffsetNs: %.2f"
                    " state: 0x%08x"
                    " receivedSvTimeInNs: %" PRIu64
                    " receivedSvTimeUncertaintyInNs: %" PRIu64
                    " cN0DbHz: %.2f, pseudorangeRateMps : %.2f,"
                    " pseudorangeRateUncertaintyMps : %.2f,"
                    " accumulatedDeltaRangeState: 0x%08x"
                    " accumulatedDeltaRangeM: %.2f, "
                    " accumulatedDeltaRangeUncertaintyM : %.2f, "
                    " carrierFrequencyHz: %.2f,"
                    " agcLevelDb: %.2f, codeType: %s,"
                    " ISBns: %.2f, ISBUncNs: %.2f, "
                    " baseband_c_n0_dbhz: %.2f, ",
                    i + 1,
                    data.measurements[i].flags,
                    data.measurements[i].v2_0.v1_1.v1_0.svid,
                    data.measurements[i].v2_0.constellation,
                    data.measurements[i].v2_0.v1_1.v1_0.timeOffsetNs,
                    data.measurements[i].v2_0.state,
                    data.measurements[i].v2_0.v1_1.v1_0.receivedSvTimeInNs,
                    data.measurements[i].v2_0.v1_1.v1_0.receivedSvTimeUncertaintyInNs,
                    data.measurements[i].v2_0.v1_1.v1_0.cN0DbHz,
                    data.measurements[i].v2_0.v1_1.v1_0.pseudorangeRateMps,
                    data.measurements[i].v2_0.v1_1.v1_0.pseudorangeRateUncertaintyMps,
                    data.measurements[i].v2_0.v1_1.accumulatedDeltaRangeState,
                    data.measurements[i].v2_0.v1_1.v1_0.accumulatedDeltaRangeM,
                    data.measurements[i].v2_0.v1_1.v1_0.accumulatedDeltaRangeUncertaintyM,
                    data.measurements[i].v2_0.v1_1.v1_0.carrierFrequencyHz,
                    data.measurements[i].v2_0.v1_1.v1_0.agcLevelDb,
                    data.measurements[i].v2_0.codeType.c_str(),
                    data.measurements[i].fullInterSignalBiasNs,
                    data.measurements[i].fullInterSignalBiasUncertaintyNs,
                    data.measurements[i].basebandCN0DbHz);
    }
    gardenPrint(" Clocks Info");
    gardenPrint(" flags: 0x%04x,"
                " timeNs: %" PRId64,
                " timeUncertaintyNs: %f,"
                " fullBiasNs: %" PRId64 ""
                " biasNs: %g, biasUncertaintyNs: %g,"
                " driftNsps: %g, driftUncertaintyNsps: %g,"
                " hwClockDiscontinuityCount: %u"
                "referenceSignalTypeForIsb.constellation: %u"
                "referenceSignalTypeForIsb.carrierFrequencyHz: %f"
                "referenceSignalTypeForIsb.codeType: %s",
                data.clock.v1_0.gnssClockFlags,
                data.clock.v1_0.timeNs,
                data.clock.v1_0.timeUncertaintyNs,
                data.clock.v1_0.fullBiasNs,
                data.clock.v1_0.biasNs,
                data.clock.v1_0.biasUncertaintyNs,
                data.clock.v1_0.driftNsps,
                data.clock.v1_0.driftUncertaintyNsps,
                data.clock.v1_0.hwClockDiscontinuityCount,
                data.clock.referenceSignalTypeForIsb.constellation,
                data.clock.referenceSignalTypeForIsb.carrierFrequencyHz,
                data.clock.referenceSignalTypeForIsb.codeType.c_str());
    gardenPrint(" ElapsedRealtime");
    gardenPrint(" flags: 0x%08x, timestampNs: %" PRId64, " timeUncertaintyNs: %" PRId64 "",
                data.elapsedRealtime.flags,
                data.elapsedRealtime.timestampNs,
                data.elapsedRealtime.timeUncertaintyNs);

    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::nfwNotifyCb(
        const IGnssVisibilityControlCallback::NfwNotification& notification)
{
    CONDPRINTLN("## %s]: () ##, app: %s, PROT: %d, PNAME: %s REQ: %d, REQID: %s"
        ", RESP: %d, EM: %d, CA: %d",
        __FUNCTION__, notification.proxyAppPackageName.c_str(), notification.protocolStack,
        notification.otherProtocolStackName.c_str(), notification.requestor,
        notification.requestorId.c_str(), notification.responseType,
        notification.inEmergencyMode, notification.isCachedLocation);
    return Void();
}

Return<bool> GnssMultiClientHidlIGnssCb::isInEmergencySession()
{
    return false;
}

Return<void> GnssMultiClientHidlIGnssCb::setCapabilitiesCb(
        android::hardware::hidl_bitfield<mcCapabilities> capabilities)
{
    CONDPRINTLN("## %s]: () ##, cap %d", __FUNCTION__, capabilities);
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssLocationBatchCb(
        const android::hardware::hidl_vec<GnssLocation>& locations) {
    CONDPRINTLN("## %s]: () ##, num of batched locations: %u" ,
        __FUNCTION__, locations.size());
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssLocationCb(
        const ::android::hardware::gnss::V1_0::GnssLocation& location)
{
    CONDPRINTLN("HIDL Client (gnssLocationCb) - LAT: %f, LON: %f, ALT: %f ACC: %f, TIME: %llu",
             location.latitudeDegrees, location.longitudeDegrees, location.altitudeMeters,
            location.horizontalAccuracyMeters, (long long)location.timestamp);

    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssStatusCb(
        ::android::hardware::gnss::V1_0::IGnssCallback::GnssStatusValue status)
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssSvStatusCb(
        const ::android::hardware::gnss::V1_0::IGnssCallback::GnssSvStatus& svInfo)
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssNmeaCb(int64_t timestamp, const ::android::hardware::hidl_string& nmea)
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssSetCapabilitesCb(
        ::android::hardware::hidl_bitfield<::android::hardware::gnss::V1_0::IGnssCallback::Capabilities> capabilities)
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssAcquireWakelockCb()
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssReleaseWakelockCb()
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssRequestTimeCb()
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssSetSystemInfoCb(
        const ::android::hardware::gnss::V1_0::IGnssCallback::GnssSystemInfo& info)
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssNameCb(
        const ::android::hardware::hidl_string& name)
{
    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssRequestLocationCb(
        bool independentFromGnss)
{
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssSetCapabilitiesCb_2_0(
        ::android::hardware::hidl_bitfield<gnssCapabilities_V2_0> capabilities) {
    CONDPRINTLN("%s: capabilities=%u\n", __func__, capabilities);
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssSetCapabilitiesCb_2_1(
        ::android::hardware::hidl_bitfield<gnssCapabilities_V2_1> capabilities) {
    CONDPRINTLN("%s: capabilities=%u\n", __func__, capabilities);
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssLocationCb_2_0(const GnssLocation& location) {
    CONDPRINTLN("## %s]: () ##, latitudeDegrees: %f, longitudeDegrees: %f, gnssLocationFlags: %u,"
            " ElapsedRealtimeFlags: %u" ,
        __FUNCTION__, location.v1_0.latitudeDegrees, location.v1_0.longitudeDegrees,
        location.v1_0.gnssLocationFlags, location.elapsedRealtime.flags);

    if (mHidlGnss->mMeasurementCorrectionsAllowed) {
        mHidlGnss->GnssMeasurementCorrections_setCorrections_1_1(location);
    }

    return Void();
}
Return<void> GnssMultiClientHidlIGnssCb::gnssRequestLocationCb_2_0(bool independentFromGnss,
        bool isUserEmergency) {
    CONDPRINTLN("## %s]: () ##, independentFromGnss: %d, isUserEmergency: %d",
        __FUNCTION__, independentFromGnss, isUserEmergency);
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssSvStatusCb_2_0(
        const android::hardware::hidl_vec<GnssSvInfo_2_0>& svInfoList) {
    CONDPRINTLN("## %s]: () ##, SV Count: %u", __FUNCTION__, svInfoList.size());

    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssSvStatusCb_2_1(
    const android::hardware::hidl_vec<GnssSvInfo_2_1>& svInfoList) {
    CONDPRINTLN("## %s]: () ##, SV Count: %u", __FUNCTION__, svInfoList.size());
    uint32_t noOfSatellites;

    noOfSatellites = svInfoList.size();
    for (uint32_t sat = 0; sat < noOfSatellites; sat++) {
        CONDPRINTLN("## %s]: () ##, basebandCN0[%d]: %.2f",
                __FUNCTION__,
                sat, svInfoList[sat].basebandCN0DbHz);
    }

    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::agnssStatusCb(IAGnssCallback_V2_0::AGnssType type,
        IAGnssCallback_V2_0::AGnssStatusValue status) {
    CONDPRINTLN("## %s]: () ##, AGnssType: %u, AGnssStatus: %u", __FUNCTION__, type, status);
    return Void();
}

Return<void> GnssMultiClientHidlIGnssCb::gnssAntennaInfoCb(
        const hidl_vec<GnssAntennaInfo>& antInfos) {
    uint32_t noOfRows, nofColumns;

    CONDPRINTLN("## %s]: () ## ", __FUNCTION__);
    for (uint32_t i = 0; i < antInfos.size(); i++) {
        CONDPRINT("Entry %d of vector\n", i);
        CONDPRINT("    carrierFrequencyMHz = %f\n", antInfos[i].carrierFrequencyMHz);
        CONDPRINT("    PCOC\n");
        CONDPRINT("      PCOC.x    = %f\n",
                antInfos[i].phaseCenterOffsetCoordinateMillimeters.x);
        CONDPRINT("      PCOC.xUnc = %f\n",
                antInfos[i].phaseCenterOffsetCoordinateMillimeters.xUncertainty);
        CONDPRINT("      PCOC.y    = %f\n",
                antInfos[i].phaseCenterOffsetCoordinateMillimeters.y);
        CONDPRINT("      PCOC.yUnc = %f\n",
                antInfos[i].phaseCenterOffsetCoordinateMillimeters.yUncertainty);
        CONDPRINT("      PCOC.z    = %f\n",
                antInfos[i].phaseCenterOffsetCoordinateMillimeters.z);
        CONDPRINT("      PCOC.zUnc = %f\n",
                antInfos[i].phaseCenterOffsetCoordinateMillimeters.zUncertainty);

        CONDPRINT("    PCVC\n");
        noOfRows = antInfos[i].phaseCenterVariationCorrectionMillimeters.size();
        for (uint32_t j = 0; j < noOfRows; j++) {
            CONDPRINT("      %d: ", j);
            nofColumns = antInfos[i].phaseCenterVariationCorrectionMillimeters[j].row.size();
            for (uint32_t k = 0; k < nofColumns; k++) {
                CONDPRINT("%f ",
                        antInfos[i].phaseCenterVariationCorrectionMillimeters[j].row[k]);
            }
            CONDPRINT("\n");
        }

        CONDPRINT("    PCVC Uncertainty\n");
        noOfRows = antInfos[i].phaseCenterVariationCorrectionUncertaintyMillimeters.size();
        for (uint32_t j = 0; j < noOfRows; j++) {
            CONDPRINT("      %d: ", j);
            nofColumns =
                    antInfos[i].phaseCenterVariationCorrectionUncertaintyMillimeters[j].row.size();
            for (uint32_t k = 0; k < nofColumns; k++) {
                CONDPRINT("%f ",
                        antInfos[i].phaseCenterVariationCorrectionUncertaintyMillimeters[j].row[k]);
            }
            CONDPRINT("\n");
        }

        CONDPRINT("    SGC\n");
        noOfRows = antInfos[i].signalGainCorrectionDbi.size();
        for (uint32_t j = 0; j < noOfRows; j++) {
            CONDPRINT("      %d: ", j);
            nofColumns = antInfos[i].signalGainCorrectionDbi[j].row.size();
            for (uint32_t k = 0; k < nofColumns; k++) {
                CONDPRINT("%f ", antInfos[i].signalGainCorrectionDbi[j].row[k]);
            }
            CONDPRINT("\n");
        }

        CONDPRINT("    SGC Uncertainty\n");
        noOfRows = antInfos[i].signalGainCorrectionUncertaintyDbi.size();
        for (uint32_t j = 0; j < noOfRows; j++) {
            CONDPRINT("      %d: ", j);
            nofColumns = antInfos[i].signalGainCorrectionUncertaintyDbi[j].row.size();
            for (uint32_t k = 0; k < nofColumns; k++) {
                CONDPRINT("%f ", antInfos[i].signalGainCorrectionUncertaintyDbi[j].row[k]);
            }
            CONDPRINT("\n");
        }
    }
    return Void();
}

} // namespace garden
