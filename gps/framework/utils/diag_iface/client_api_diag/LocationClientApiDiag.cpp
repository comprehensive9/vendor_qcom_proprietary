/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2019-2020 The Linux Foundation. All rights reserved
=============================================================================*/

#include "LocationClientApiDiag.h"
#include <errno.h>

#ifndef LOG_GNSS_CLIENT_API_LOCATION_REPORT_C
#define LOG_GNSS_CLIENT_API_LOCATION_REPORT_C (0x1C8F)
#endif
#ifndef LOG_GNSS_CLIENT_API_SV_REPORT_C
#define LOG_GNSS_CLIENT_API_SV_REPORT_C (0x1C90)
#endif
#ifndef LOG_GNSS_CLIENT_API_NMEA_REPORT_C
#define LOG_GNSS_CLIENT_API_NMEA_REPORT_C (0x1CB2)
#endif
#ifndef LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C
#define LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C (0x1CB7)
#endif
#ifndef LOG_GNSS_CLIENT_API_SV_POLY_REPORT_C
#define LOG_GNSS_CLIENT_API_SV_POLY_REPORT_C (0x1CC7)
#endif

namespace location_client {

LocationClientApiDiag::LocationClientApiDiag(): mDiagIface(LocDiagIface::getInstance()) {}

void GnssLocationReport::log() {
    if (!mDiagIface) {
        return;
    }

    LOC_LOGd("GnssLocationReport::log");
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssLocationStructType* diagGnssLocPtr = nullptr;
    diagGnssLocPtr = (clientDiagGnssLocationStructType*)
            mDiagIface->logAlloc(LOG_GNSS_CLIENT_API_LOCATION_REPORT_C,
                                 sizeof(clientDiagGnssLocationStructType), &bufferSrc);
    if (diagGnssLocPtr == NULL) {
        LOC_LOGv("diag memory alloc failed");
        return;
    }
    populateClientDiagLocation(diagGnssLocPtr, mGnssLocation, mCapMask);
    diagGnssLocPtr->version = LOG_CLIENT_LOCATION_DIAG_MSG_VERSION;

    mDiagIface->logCommit(diagGnssLocPtr, bufferSrc,
                          LOG_GNSS_CLIENT_API_LOCATION_REPORT_C,
                          sizeof(clientDiagGnssLocationStructType));
}

void GnssSvReport::log() {
    if (!mDiagIface) {
        return;
    }

    LOC_LOGd("GnssSvReport::log");
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssSvStructType* diagGnssSvPtr = nullptr;
    diagGnssSvPtr = (clientDiagGnssSvStructType*)mDiagIface->logAlloc(
            LOG_GNSS_CLIENT_API_SV_REPORT_C,
            sizeof(clientDiagGnssSvStructType), &bufferSrc);
    if (diagGnssSvPtr == NULL) {
        LOC_LOGv("memory alloc failed");
        return;
    }
    populateClientDiagGnssSv(diagGnssSvPtr, mGnssSvsVector);
    diagGnssSvPtr->version = LOG_CLIENT_SV_REPORT_DIAG_MSG_VERSION;

    mDiagIface->logCommit(diagGnssSvPtr, bufferSrc,
            LOG_GNSS_CLIENT_API_SV_REPORT_C,
            sizeof(clientDiagGnssSvStructType));
}

void GnssNmeaReport::log() {
    if (!mDiagIface) {
        return;
    }
    LOC_LOGd("GnssNmeaReport::log");
    size_t diagBufferSize = sizeof(clientDiagGnssNmeaStructType) + mLength - 1;
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssNmeaStructType* diagGnssNmeaPtr = nullptr;
    diagGnssNmeaPtr = (clientDiagGnssNmeaStructType*)
        mDiagIface->logAlloc(LOG_GNSS_CLIENT_API_NMEA_REPORT_C,
                diagBufferSize, &bufferSrc);
    if (diagGnssNmeaPtr == NULL) {
        LOC_LOGv("memory alloc failed");
        return;
    }
    diagGnssNmeaPtr->timestamp = mTimestamp;
    diagGnssNmeaPtr->nmeaLength = mLength;
    memcpy(&diagGnssNmeaPtr->nmea, mNmea, mLength);
    diagGnssNmeaPtr->version = LOG_CLIENT_NMEA_REPORT_DIAG_MSG_VERSION;

    mDiagIface->logCommit(diagGnssNmeaPtr, bufferSrc,
            LOG_GNSS_CLIENT_API_NMEA_REPORT_C,
            sizeof(clientDiagGnssNmeaStructType));
}

void GnssMeasReport::log() {
    if (!mDiagIface) {
        return;
    }
    LOC_LOGd("GnssMeasReport::log");
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssMeasurementsStructType* diagGnssMeasPtr = nullptr;
    diagGnssMeasPtr =
            (clientDiagGnssMeasurementsStructType*)mDiagIface->logAlloc(
                    LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C,
                    sizeof(clientDiagGnssMeasurementsStructType),
                    &bufferSrc);
    if (NULL == diagGnssMeasPtr) {
        LOC_LOGv("memory alloc failed");
        return;
    }
    diagGnssMeasPtr->count = mGnssMeasurements.measurements.size();
    if (diagGnssMeasPtr->count > CLIENT_DIAG_GNSS_MEASUREMENTS_MAX) {
        diagGnssMeasPtr->count = CLIENT_DIAG_GNSS_MEASUREMENTS_MAX;
    }
    diagGnssMeasPtr->maxSequence =
            (uint8)(((diagGnssMeasPtr->count - 0.5) /
                    CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) + 1);
    LOC_LOGv("maxSequence = %d, count = %d",
             diagGnssMeasPtr->maxSequence, diagGnssMeasPtr->count);
    diagGnssMeasPtr->version = LOG_CLIENT_MEASUREMENTS_DIAG_MSG_VERSION;
    diagGnssMeasPtr->clock.flags =
        (clientDiagGnssMeasurementsClockFlagsMask)mGnssMeasurements.clock.flags;
    diagGnssMeasPtr->clock.leapSecond = mGnssMeasurements.clock.leapSecond;
    diagGnssMeasPtr->clock.timeNs = mGnssMeasurements.clock.timeNs;
    diagGnssMeasPtr->clock.timeUncertaintyNs =
            mGnssMeasurements.clock.timeUncertaintyNs;
    diagGnssMeasPtr->clock.fullBiasNs = mGnssMeasurements.clock.fullBiasNs;
    diagGnssMeasPtr->clock.biasNs = mGnssMeasurements.clock.biasNs;
    diagGnssMeasPtr->clock.biasUncertaintyNs =
            mGnssMeasurements.clock.biasUncertaintyNs;
    diagGnssMeasPtr->clock.driftNsps = mGnssMeasurements.clock.driftNsps;
    diagGnssMeasPtr->clock.driftUncertaintyNsps =
            mGnssMeasurements.clock.driftUncertaintyNsps;
    diagGnssMeasPtr->clock.hwClockDiscontinuityCount =
            mGnssMeasurements.clock.hwClockDiscontinuityCount;
    for (uint8 i = 0; i < diagGnssMeasPtr->maxSequence; i++) {
        diagGnssMeasPtr->sequenceNumber = i + 1;
        LOC_LOGv("seqNumber = %d", diagGnssMeasPtr->sequenceNumber);
        populateClientDiagMeasurements(diagGnssMeasPtr, mGnssMeasurements);
        mDiagIface->logCommit(diagGnssMeasPtr, bufferSrc,
                LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C,
                sizeof(clientDiagGnssMeasurementsStructType));
    }
}

void GnssLatencyReport::log() {
    if (!mDiagIface) {
        return;
    }
    LOC_LOGv("GnssLatencyReport::log");

    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssLatencyStructType* diagGnssLatencyPtr = nullptr;
    diagGnssLatencyPtr = (clientDiagGnssLatencyStructType*)
            mDiagIface->logAlloc(
                    LOG_GNSS_LATENCY_REPORT_C,
                    sizeof(clientDiagGnssLatencyStructType),
                    &bufferSrc);
    if (diagGnssLatencyPtr == NULL) {
        LOC_LOGe("diag memory alloc failed");
        return;
    }

    diagGnssLatencyPtr->meQtimer1 = mGnssLatencyInfo.meQtimer1;
    diagGnssLatencyPtr->meQtimer2 = mGnssLatencyInfo.meQtimer2;
    diagGnssLatencyPtr->meQtimer3 = mGnssLatencyInfo.meQtimer3;
    diagGnssLatencyPtr->peQtimer1 = mGnssLatencyInfo.peQtimer1;
    diagGnssLatencyPtr->peQtimer2 = mGnssLatencyInfo.peQtimer2;
    diagGnssLatencyPtr->peQtimer3 = mGnssLatencyInfo.peQtimer3;
    diagGnssLatencyPtr->smQtimer1 = mGnssLatencyInfo.smQtimer1;
    diagGnssLatencyPtr->smQtimer2 = mGnssLatencyInfo.smQtimer2;
    diagGnssLatencyPtr->smQtimer3 = mGnssLatencyInfo.smQtimer3;
    diagGnssLatencyPtr->locMwQtimer = mGnssLatencyInfo.locMwQtimer;
    diagGnssLatencyPtr->hlosQtimer1 = mGnssLatencyInfo.hlosQtimer1;
    diagGnssLatencyPtr->hlosQtimer2 = mGnssLatencyInfo.hlosQtimer2;
    diagGnssLatencyPtr->hlosQtimer3 = mGnssLatencyInfo.hlosQtimer3;
    diagGnssLatencyPtr->hlosQtimer4 = mGnssLatencyInfo.hlosQtimer4;
    diagGnssLatencyPtr->hlosQtimer5 = mGnssLatencyInfo.hlosQtimer5;
    diagGnssLatencyPtr->version = LOG_CLIENT_LATENCY_DIAG_MSG_VERSION;

    uint64_t qTimerLatency = 0;

    if (diagGnssLatencyPtr->hlosQtimer2 >= diagGnssLatencyPtr->meQtimer2 &&
        0 != diagGnssLatencyPtr->meQtimer2) {
        /* The starting point to compute overall latency is meQtimer2, and this is because
           between meQtimer1 amd meQtimer2 there is internal processing in ME. Also, we
           need to adjust for the the time between hlosQtimer1 (measurements block arrival time
           to HLOS) and hlosQtimer2 (position arrival time to HLOS), hence below formula */
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer5 -
                (diagGnssLatencyPtr->hlosQtimer2 - diagGnssLatencyPtr->hlosQtimer1) -
                diagGnssLatencyPtr->meQtimer2;
        diagGnssLatencyPtr->overallLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("overallLatency=%" PRIi64 " ", diagGnssLatencyPtr->overallLatency);
    } else {
        LOC_LOGw("overallLatency can't be computed");
        diagGnssLatencyPtr->overallLatency = 0;
    }

    if (diagGnssLatencyPtr->meQtimer2 >= diagGnssLatencyPtr->meQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->meQtimer2 - diagGnssLatencyPtr->meQtimer1;
        diagGnssLatencyPtr->meLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("meLatency1=%" PRIi64 " ", diagGnssLatencyPtr->meLatency1);
    } else {
        LOC_LOGw("meLatency1 can't be computed");
        diagGnssLatencyPtr->meLatency1 = 0;
    }

    if (diagGnssLatencyPtr->meQtimer3 >= diagGnssLatencyPtr->meQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->meQtimer3 - diagGnssLatencyPtr->meQtimer2;
        diagGnssLatencyPtr->meLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("meLatency2=%" PRIi64 " ", diagGnssLatencyPtr->meLatency2);
    } else {
        LOC_LOGw("meLatency2 can't be computed");
        diagGnssLatencyPtr->meLatency2 = 0;
    }

    if (diagGnssLatencyPtr->peQtimer1 >= diagGnssLatencyPtr->meQtimer3) {
        qTimerLatency = diagGnssLatencyPtr->peQtimer1 - diagGnssLatencyPtr->meQtimer3;
        diagGnssLatencyPtr->meToPeLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("meToPeLatency=%" PRIi64 " ", diagGnssLatencyPtr->meToPeLatency);
    } else {
        LOC_LOGw("meToPeLatency can't be computed");
        diagGnssLatencyPtr->meToPeLatency = 0;
    }

    if (diagGnssLatencyPtr->peQtimer2 >= diagGnssLatencyPtr->peQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->peQtimer2 - diagGnssLatencyPtr->peQtimer1;
        diagGnssLatencyPtr->peLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("peLatency1=%" PRIi64 " ", diagGnssLatencyPtr->peLatency1);
    } else {
        LOC_LOGw("peLatency1 can't be computed");
        diagGnssLatencyPtr->peLatency1 = 0;
    }

    if (diagGnssLatencyPtr->peQtimer3 >= diagGnssLatencyPtr->peQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->peQtimer3 - diagGnssLatencyPtr->peQtimer2;
        diagGnssLatencyPtr->peLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("peLatency2=%" PRIi64 " ", diagGnssLatencyPtr->peLatency2);
    } else {
        LOC_LOGw("peLatency2 can't be computed");
        diagGnssLatencyPtr->peLatency2 = 0;
    }

    if (diagGnssLatencyPtr->smQtimer1 >= diagGnssLatencyPtr->peQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->smQtimer1 - diagGnssLatencyPtr->peQtimer1;
        diagGnssLatencyPtr->peToSmLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("peToSmLatency=%" PRIi64 " ", diagGnssLatencyPtr->peToSmLatency);
    } else {
        LOC_LOGw("peToSmLatency can't be computed");
        diagGnssLatencyPtr->peToSmLatency = 0;
    }

    if (diagGnssLatencyPtr->smQtimer2 >= diagGnssLatencyPtr->smQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->smQtimer2 - diagGnssLatencyPtr->smQtimer1;
        diagGnssLatencyPtr->smLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("smLatency1=%" PRIi64 " ", diagGnssLatencyPtr->smLatency1);
    } else {
        LOC_LOGw("smLatency1 can't be computed");
        diagGnssLatencyPtr->smLatency1 = 0;
    }

    if (diagGnssLatencyPtr->smQtimer3 >= diagGnssLatencyPtr->smQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->smQtimer3 - diagGnssLatencyPtr->smQtimer2;
        diagGnssLatencyPtr->smLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("smLatency2=%" PRIi64 " ", diagGnssLatencyPtr->smLatency2);
    } else {
        LOC_LOGw("smLatency2 can't be computed");
        diagGnssLatencyPtr->smLatency2 = 0;
    }

    if (diagGnssLatencyPtr->locMwQtimer >= diagGnssLatencyPtr->smQtimer3) {
        qTimerLatency = diagGnssLatencyPtr->locMwQtimer - diagGnssLatencyPtr->smQtimer3;
        diagGnssLatencyPtr->smToLocMwLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("smToLocMwLatency=%" PRIi64 " ", diagGnssLatencyPtr->smToLocMwLatency);
    } else {
        LOC_LOGw("smToLocMwLatency can't be computed");
        diagGnssLatencyPtr->smToLocMwLatency = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer1 >= diagGnssLatencyPtr->locMwQtimer) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer1 - diagGnssLatencyPtr->locMwQtimer;
        diagGnssLatencyPtr->locMwToHlosLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("locMwToHlosLatency=%" PRIi64 " ", diagGnssLatencyPtr->locMwToHlosLatency);
    } else {
        LOC_LOGw("locMwToHlosLatency can't be computed");
        diagGnssLatencyPtr->locMwToHlosLatency = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer2 >= diagGnssLatencyPtr->hlosQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer2 - diagGnssLatencyPtr->hlosQtimer1;
        diagGnssLatencyPtr->hlosLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency1=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency1);
    } else {
        LOC_LOGw("hlosLatency1 can't be computed");
        diagGnssLatencyPtr->hlosLatency1 = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer3 >= diagGnssLatencyPtr->hlosQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer3 - diagGnssLatencyPtr->hlosQtimer2;
        diagGnssLatencyPtr->hlosLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency2=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency2);
    } else {
        LOC_LOGw("hlosLatency2 can't be computed");
        diagGnssLatencyPtr->hlosLatency2 = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer4 >= diagGnssLatencyPtr->hlosQtimer3) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer4 - diagGnssLatencyPtr->hlosQtimer3;
        diagGnssLatencyPtr->hlosLatency3 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency3=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency3);
    } else {
        LOC_LOGw("hlosLatency3 can't be computed");
        diagGnssLatencyPtr->hlosLatency3 = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer5 >= diagGnssLatencyPtr->hlosQtimer4) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer5 - diagGnssLatencyPtr->hlosQtimer4;
        diagGnssLatencyPtr->hlosLatency4 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency4=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency3);
    } else {
        LOC_LOGw("hlosLatency4 can't be computed");
        diagGnssLatencyPtr->hlosLatency4 = 0;
    }

    mDiagIface->logCommit(diagGnssLatencyPtr, bufferSrc,
            LOG_GNSS_LATENCY_REPORT_C,
            sizeof(clientDiagGnssLatencyStructType));
}

void LocationClientApiDiag::translateDiagGnssLocationPositionDynamics(
        clientDiagGnssLocationPositionDynamics& out, const GnssLocationPositionDynamics& in) {
    out.bodyFrameDataMask = in.bodyFrameDataMask;
    out.longAccel = in.longAccel;
    out.latAccel = in.latAccel;
    out.vertAccel = in.vertAccel;
    out.longAccelUnc = in.longAccelUnc;
    out.latAccelUnc = in.latAccelUnc;
    out.vertAccelUnc = in.vertAccelUnc;

    out.roll = in.roll;
    out.rollUnc = in.rollUnc;
    out.rollRate = in.rollRate;
    out.rollRateUnc = in.rollRateUnc;

    out.pitch = in.pitch;
    out.pitchUnc = in.pitchUnc;
    out.pitchRate = in.pitchRate;
    out.pitchRateUnc = in.pitchRateUnc;

    out.yaw = in.yaw;
    out.yawUnc = in.yawUnc;
    out.yawRate = in.yawRate;
    out.yawRateUnc = in.yawRateUnc;
}

clientDiagGnssSystemTimeStructType LocationClientApiDiag::parseDiagGnssTime(
        const GnssSystemTimeStructType &halGnssTime) {
    clientDiagGnssSystemTimeStructType gnssTime;
    memset(&gnssTime, 0, sizeof(gnssTime));
    uint32_t gnssTimeFlags = 0;

    if (GNSS_SYSTEM_TIME_WEEK_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_VALID;
        gnssTime.systemWeek = halGnssTime.systemWeek;
    }
    if (GNSS_SYSTEM_TIME_WEEK_MS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_MS_VALID;
        gnssTime.systemMsec = halGnssTime.systemMsec;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
        gnssTime.systemClkTimeBias = halGnssTime.systemClkTimeBias;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
        gnssTime.systemClkTimeUncMs = halGnssTime.systemClkTimeUncMs;
    }
    if (GNSS_SYSTEM_REF_FCOUNT_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_REF_FCOUNT_VALID;
        gnssTime.refFCount = halGnssTime.refFCount;
    }
    if (GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
        gnssTime.numClockResets = halGnssTime.numClockResets;
    }

    gnssTime.validityMask = (clientDiagGnssSystemTimeStructTypeFlags)gnssTimeFlags;

    return gnssTime;
}

clientDiagGnssGloTimeStructType LocationClientApiDiag::parseDiagGloTime(
        const GnssGloTimeStructType &halGloTime) {

    clientDiagGnssGloTimeStructType gloTime;
    memset(&gloTime, 0, sizeof(gloTime));
    uint32_t gloTimeFlags = 0;

    if (GNSS_CLO_DAYS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_CLO_DAYS_VALID;
        gloTime.gloDays = halGloTime.gloDays;
    }
    if (GNSS_GLO_MSEC_VALID  & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_MSEC_VALID ;
        gloTime.gloMsec = halGloTime.gloMsec;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_VALID;
        gloTime.gloClkTimeBias = halGloTime.gloClkTimeBias;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_UNC_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
        gloTime.gloClkTimeUncMs = halGloTime.gloClkTimeUncMs;
    }
    if (GNSS_GLO_REF_FCOUNT_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_REF_FCOUNT_VALID;
        gloTime.refFCount = halGloTime.refFCount;
    }
    if (GNSS_GLO_NUM_CLOCK_RESETS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_NUM_CLOCK_RESETS_VALID;
        gloTime.numClockResets = halGloTime.numClockResets;
    }
    if (GNSS_GLO_FOUR_YEAR_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_FOUR_YEAR_VALID;
        gloTime.gloFourYear = halGloTime.gloFourYear;
    }

    gloTime.validityMask = (clientDiagGnssGloTimeStructTypeFlags)gloTimeFlags;

    return gloTime;
}

void LocationClientApiDiag::translateDiagSystemTime(
        clientDiagGnssSystemTime& out, const GnssSystemTime& in) {

    out.gnssSystemTimeSrc = (clientDiagGnss_LocSvSystemEnumType)in.gnssSystemTimeSrc;
    switch (in.gnssSystemTimeSrc) {
        case GNSS_LOC_SV_SYSTEM_GPS:
           out.u.gpsSystemTime = parseDiagGnssTime(in.u.gpsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
           out.u.galSystemTime = parseDiagGnssTime(in.u.galSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
           out.u.gloSystemTime = parseDiagGloTime(in.u.gloSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_BDS:
           out.u.bdsSystemTime = parseDiagGnssTime(in.u.bdsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
           out.u.qzssSystemTime = parseDiagGnssTime(in.u.qzssSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_NAVIC:
           out.u.navicSystemTime = parseDiagGnssTime(in.u.navicSystemTime);
           break;
        default:
           break;
    }
}

clientDiagGnssLocationSvUsedInPosition LocationClientApiDiag::parseDiagLocationSvUsedInPosition(
        const GnssLocationSvUsedInPosition &halSv) {

    clientDiagGnssLocationSvUsedInPosition clientSv;
    clientSv.gpsSvUsedIdsMask = halSv.gpsSvUsedIdsMask;
    clientSv.gloSvUsedIdsMask = halSv.gloSvUsedIdsMask;
    clientSv.galSvUsedIdsMask = halSv.galSvUsedIdsMask;
    clientSv.bdsSvUsedIdsMask = halSv.bdsSvUsedIdsMask;
    clientSv.qzssSvUsedIdsMask = halSv.qzssSvUsedIdsMask;
    clientSv.navicSvUsedIdsMask = halSv.navicSvUsedIdsMask;

    return clientSv;
}

void LocationClientApiDiag::translateDiagGnssSignalType(
        clientDiagGnssSignalTypeMask& out, GnssSignalTypeMask in) {

    out = (clientDiagGnssSignalTypeMask)0;
    if (in & GNSS_SIGNAL_GPS_L1CA_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L1CA;
    }
    if (in & GNSS_SIGNAL_GPS_L1C_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L1C;
    }
    if (in & GNSS_SIGNAL_GPS_L2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L2;
    }
    if (in & GNSS_SIGNAL_GPS_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L5;
    }
    if (in & GNSS_SIGNAL_GLONASS_G1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G1;
    }
    if (in & GNSS_SIGNAL_GLONASS_G2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G2;
    }
    if (in & GNSS_SIGNAL_GALILEO_E1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E1;
    }
    if (in & GNSS_SIGNAL_GALILEO_E5A_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5A;
    }
    if (in & GNSS_SIGNAL_GALILEO_E5B_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5B;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1I_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1I;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1C_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1C;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2I_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2I;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2AI_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AI;
    }
    if (in & GNSS_SIGNAL_QZSS_L1CA_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1CA;
    }
    if (in & GNSS_SIGNAL_QZSS_L1S_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1S;
    }
    if (in & GNSS_SIGNAL_QZSS_L2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L2;
    }
    if (in & GNSS_SIGNAL_QZSS_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L5;
    }
    if (in & GNSS_SIGNAL_SBAS_L1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_SBAS_L1;
    }
    if (in & GNSS_SIGNAL_NAVIC_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_NAVIC_L5;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2AQ_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AQ;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2;
    }
}

clientDiagGnss_LocSvSystemEnumType LocationClientApiDiag::parseDiagGnssConstellation(
        Gnss_LocSvSystemEnumType gnssConstellation) {

    clientDiagGnss_LocSvSystemEnumType constellation;
    switch (gnssConstellation) {
        case GNSS_LOC_SV_SYSTEM_GPS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GPS;
            break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GALILEO;
            break;
        case GNSS_LOC_SV_SYSTEM_SBAS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_SBAS;
            break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GLONASS;
            break;
        case GNSS_LOC_SV_SYSTEM_BDS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_BDS;
            break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_QZSS;
            break;
        case GNSS_LOC_SV_SYSTEM_NAVIC:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_NAVIC;
            break;
        default:
            constellation = (clientDiagGnss_LocSvSystemEnumType)~0;
            break;
    }
    return constellation;
}

void LocationClientApiDiag::translateDiagGnssMeasUsageInfo(clientDiagGnssMeasUsageInfo& out,
        const GnssMeasUsageInfo& in) {

    clientDiagGnssSignalTypeMask diagGnssSignalType;
    translateDiagGnssSignalType(diagGnssSignalType, in.gnssSignalType);
    out.gnssSignalType = diagGnssSignalType;
   /** Specifies GNSS Constellation Type */
    out.gnssConstellation = parseDiagGnssConstellation(in.gnssConstellation);
    /**  GNSS SV ID.
     For GPS:      1 to 32
     For GLONASS:  65 to 96. When slot-number to SV ID mapping is unknown, set as 255.
     For SBAS:     120 to 151
     For QZSS-L1CA:193 to 197
     For BDS:      201 to 237
     For GAL:      301 to 336
     For NAVIC:    401 to 414 */
    out.gnssSvId = in.gnssSvId;
}

void LocationClientApiDiag::populateClientDiagLocation(
        clientDiagGnssLocationStructType* diagGnssLocPtr,
        const GnssLocation& gnssLocation,
        const LocationCapabilitiesMask& capMask) {

    diagGnssLocPtr->timestamp = gnssLocation.timestamp;
    diagGnssLocPtr->latitude = gnssLocation.latitude;
    diagGnssLocPtr->longitude = gnssLocation.longitude;
    diagGnssLocPtr->altitude = gnssLocation.altitude;
    diagGnssLocPtr->speed = gnssLocation.speed;
    diagGnssLocPtr->bearing = gnssLocation.bearing;
    diagGnssLocPtr->horizontalAccuracy = gnssLocation.horizontalAccuracy;
    diagGnssLocPtr->verticalAccuracy = gnssLocation.verticalAccuracy;
    diagGnssLocPtr->speedAccuracy = gnssLocation.speedAccuracy;
    diagGnssLocPtr->bearingAccuracy = gnssLocation.bearingAccuracy;
    diagGnssLocPtr->flags = gnssLocation.flags;
    diagGnssLocPtr->techMask = gnssLocation.techMask;

    diagGnssLocPtr->gnssInfoFlags = gnssLocation.gnssInfoFlags;
    diagGnssLocPtr->altitudeMeanSeaLevel = gnssLocation.altitudeMeanSeaLevel;
    diagGnssLocPtr->pdop = gnssLocation.pdop;
    diagGnssLocPtr->hdop = gnssLocation.hdop;
    diagGnssLocPtr->vdop = gnssLocation.vdop;
    diagGnssLocPtr->gdop = gnssLocation.gdop;
    diagGnssLocPtr->tdop = gnssLocation.tdop;
    diagGnssLocPtr->magneticDeviation = gnssLocation.magneticDeviation;
    diagGnssLocPtr->horReliability = (clientDiagLocationReliability)gnssLocation.horReliability;
    diagGnssLocPtr->verReliability = (clientDiagLocationReliability)gnssLocation.verReliability;
    diagGnssLocPtr->horUncEllipseSemiMajor = gnssLocation.horUncEllipseSemiMajor;
    diagGnssLocPtr->horUncEllipseSemiMinor = gnssLocation.horUncEllipseSemiMinor;
    diagGnssLocPtr->horUncEllipseOrientAzimuth = gnssLocation.horUncEllipseOrientAzimuth;
    diagGnssLocPtr->northStdDeviation = gnssLocation.northStdDeviation;
    diagGnssLocPtr->eastStdDeviation = gnssLocation.eastStdDeviation;
    diagGnssLocPtr->northVelocity = gnssLocation.northVelocity;
    diagGnssLocPtr->eastVelocity = gnssLocation.eastVelocity;
    diagGnssLocPtr->upVelocity = gnssLocation.upVelocity;
    diagGnssLocPtr->northVelocityStdDeviation = gnssLocation.northVelocityStdDeviation;
    diagGnssLocPtr->eastVelocityStdDeviation = gnssLocation.eastVelocityStdDeviation;
    diagGnssLocPtr->upVelocityStdDeviation = gnssLocation.upVelocityStdDeviation;
    diagGnssLocPtr->svUsedInPosition =
            parseDiagLocationSvUsedInPosition(gnssLocation.svUsedInPosition);
    diagGnssLocPtr->navSolutionMask = gnssLocation.navSolutionMask;
    diagGnssLocPtr->posTechMask = gnssLocation.posTechMask;
    translateDiagGnssLocationPositionDynamics(diagGnssLocPtr->bodyFrameData,
            gnssLocation.bodyFrameData);
    translateDiagSystemTime(diagGnssLocPtr->gnssSystemTime, gnssLocation.gnssSystemTime);
    diagGnssLocPtr->numOfMeasReceived = (uint8_t)gnssLocation.measUsageInfo.size();
    clientDiagGnssMeasUsageInfo measUsage;
    memset(diagGnssLocPtr->measUsageInfo, 0, sizeof(diagGnssLocPtr->measUsageInfo));
    for (int idx = 0; idx < gnssLocation.measUsageInfo.size(); idx++) {
        translateDiagGnssMeasUsageInfo(measUsage, gnssLocation.measUsageInfo[idx]);
        diagGnssLocPtr->measUsageInfo[idx] = measUsage;
    }
    diagGnssLocPtr->leapSeconds = gnssLocation.leapSeconds;
    diagGnssLocPtr->timeUncMs = gnssLocation.timeUncMs;
    diagGnssLocPtr->numSvUsedInPosition = gnssLocation.numSvUsedInPosition;
    diagGnssLocPtr->calibrationConfidencePercent = gnssLocation.calibrationConfidencePercent;
    diagGnssLocPtr->calibrationStatus = gnssLocation.calibrationStatus;
    diagGnssLocPtr->conformityIndex = gnssLocation.conformityIndex;
    diagGnssLocPtr->llaVRPBased.latitude = gnssLocation.llaVRPBased.latitude;
    diagGnssLocPtr->llaVRPBased.longitude = gnssLocation.llaVRPBased.longitude;
    diagGnssLocPtr->llaVRPBased.altitude = gnssLocation.llaVRPBased.altitude;
    diagGnssLocPtr->enuVelocityVRPBased[0] = gnssLocation.enuVelocityVRPBased[0];
    diagGnssLocPtr->enuVelocityVRPBased[1] = gnssLocation.enuVelocityVRPBased[1];
    diagGnssLocPtr->enuVelocityVRPBased[2] = gnssLocation.enuVelocityVRPBased[2];

    diagGnssLocPtr->locOutputEngType =
            (clientDiagLocOutputEngineType) gnssLocation.locOutputEngType;
    diagGnssLocPtr->locOutputEngMask =
            (clientDiagPositioningEngineMask) gnssLocation.locOutputEngMask;

    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    diagGnssLocPtr->bootTimestampNs = (ts.tv_sec * 1000000000ULL + ts.tv_nsec);
    diagGnssLocPtr->qtimerTickCnt = getQTimerTickCount();

    diagGnssLocPtr->drSolutionStatusMask =
            (clientDiagDrSolutionStatusMask) gnssLocation.drSolutionStatusMask;
    strlcpy(diagGnssLocPtr->processName, program_invocation_short_name,
            sizeof(diagGnssLocPtr->processName));

    diagGnssLocPtr->altitudeAssumed = gnssLocation.altitudeAssumed;
    diagGnssLocPtr->capabilitiesMask = capMask;
    diagGnssLocPtr->sessionStatus = (clientDiagLocSessionStatus) gnssLocation.sessionStatus;
}

void LocationClientApiDiag::populateClientDiagMeasurements(
        clientDiagGnssMeasurementsStructType* diagGnssMeasPtr,
        const GnssMeasurements& gnssMeasurements) {

    uint8 adjust = (diagGnssMeasPtr->sequenceNumber - 1) * CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
    uint32_t count = diagGnssMeasPtr->count - adjust;
    if (count > CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) {
        count = CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
    }
    LOC_LOGv("adjust = %d count = %d", adjust, count);
    for (uint32_t idx = 0; idx < count; ++idx) {
        uint32_t gIdx = idx + adjust;
        diagGnssMeasPtr->measurements[idx].flags =
                (clientDiagGnssMeasurementsDataFlagsMask)gnssMeasurements.measurements[gIdx].flags;
        diagGnssMeasPtr->measurements[idx].svId = gnssMeasurements.measurements[gIdx].svId;
        diagGnssMeasPtr->measurements[idx].svType =
                (clientDiagGnssSvType)gnssMeasurements.measurements[gIdx].svType;
        diagGnssMeasPtr->measurements[idx].timeOffsetNs =
                gnssMeasurements.measurements[gIdx].timeOffsetNs;
        diagGnssMeasPtr->measurements[idx].stateMask =
                (clientDiagGnssMeasurementsStateMask)gnssMeasurements.measurements[gIdx].stateMask;
        diagGnssMeasPtr->measurements[idx].receivedSvTimeNs =
                gnssMeasurements.measurements[gIdx].receivedSvTimeNs;
        diagGnssMeasPtr->measurements[idx].receivedSvTimeUncertaintyNs =
                gnssMeasurements.measurements[gIdx].receivedSvTimeUncertaintyNs;
        diagGnssMeasPtr->measurements[idx].carrierToNoiseDbHz =
                gnssMeasurements.measurements[gIdx].carrierToNoiseDbHz;
        diagGnssMeasPtr->measurements[idx].pseudorangeRateMps =
                gnssMeasurements.measurements[gIdx].pseudorangeRateMps;
        diagGnssMeasPtr->measurements[idx].pseudorangeRateUncertaintyMps =
                gnssMeasurements.measurements[gIdx].pseudorangeRateUncertaintyMps;
        diagGnssMeasPtr->measurements[idx].adrStateMask =
                (clientDiagGnssMeasurementsAdrStateMask)
                        gnssMeasurements.measurements[gIdx].adrStateMask;
        diagGnssMeasPtr->measurements[idx].adrMeters =
                gnssMeasurements.measurements[gIdx].adrMeters;
        diagGnssMeasPtr->measurements[idx].adrUncertaintyMeters =
                gnssMeasurements.measurements[gIdx].adrUncertaintyMeters;
        diagGnssMeasPtr->measurements[idx].carrierFrequencyHz =
                gnssMeasurements.measurements[gIdx].carrierFrequencyHz;
        diagGnssMeasPtr->measurements[idx].carrierCycles =
                gnssMeasurements.measurements[gIdx].carrierCycles;
        diagGnssMeasPtr->measurements[idx].carrierPhase =
                gnssMeasurements.measurements[gIdx].carrierPhase;
        diagGnssMeasPtr->measurements[idx].carrierPhaseUncertainty =
                gnssMeasurements.measurements[gIdx].carrierPhaseUncertainty;
        diagGnssMeasPtr->measurements[idx].multipathIndicator =
                (clientDiagGnssMeasurementsMultipathIndicator)
                        gnssMeasurements.measurements[gIdx].multipathIndicator;
        diagGnssMeasPtr->measurements[idx].signalToNoiseRatioDb =
                gnssMeasurements.measurements[gIdx].signalToNoiseRatioDb;
        diagGnssMeasPtr->measurements[idx].agcLevelDb =
                gnssMeasurements.measurements[gIdx].agcLevelDb;
        diagGnssMeasPtr->measurements[idx].basebandCarrierToNoiseDbHz =
                gnssMeasurements.measurements[idx].basebandCarrierToNoiseDbHz;
        clientDiagGnssSignalTypeMask diagGnssSignalType;
        translateDiagGnssSignalType(diagGnssSignalType,
                gnssMeasurements.measurements[idx].gnssSignalType);
        diagGnssMeasPtr->measurements[idx].gnssSignalType = diagGnssSignalType;
        diagGnssMeasPtr->measurements[idx].interSignalBiasNs=
                gnssMeasurements.measurements[idx].interSignalBiasNs;
        diagGnssMeasPtr->measurements[idx].interSignalBiasUncertaintyNs=
                gnssMeasurements.measurements[idx].interSignalBiasUncertaintyNs;
        diagGnssMeasPtr->measurements[idx].cycleSlipCount =
                gnssMeasurements.measurements[idx].cycleSlipCount;
    }
}

void LocationClientApiDiag::translateDiagGnssSv(clientDiagGnssSv& out, const GnssSv& in) {

    /** Unique Identifier */
    out.svId = in.svId;
    /** type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO, NAVIC) */
    out.type = (clientDiagGnssSvType)in.type;
    /** signal strength */
    out.cN0Dbhz = in.cN0Dbhz;
    /** elevation of SV (in degrees) */
    out.elevation = in.elevation;
    /** azimuth of SV (in degrees) */
    out.azimuth = in.azimuth;
    /** Bitwise OR of GnssSvOptionsBits */
    out.gnssSvOptionsMask = in.gnssSvOptionsMask;
    /** carrier frequency of SV (in Hz) */
    out.carrierFrequencyHz = in.carrierFrequencyHz;
    /** Bitwise OR of clientDiagGnssSignalTypeBits */
    clientDiagGnssSignalTypeMask diagGnssSignalType;
    translateDiagGnssSignalType(diagGnssSignalType, in.gnssSignalTypeMask);
    out.gnssSignalTypeMask = diagGnssSignalType;
    /** baseband signal strength */
    out.basebandCarrierToNoiseDbHz = in.basebandCarrierToNoiseDbHz;
}

void LocationClientApiDiag::populateClientDiagGnssSv(clientDiagGnssSvStructType* diagGnssSvPtr,
        const std::vector<GnssSv>& gnssSvs) {

    clientDiagGnssSv diagGnssSv;
    diagGnssSvPtr->count = gnssSvs.size();
    for (int idx = 0; idx < gnssSvs.size(); ++idx) {
        translateDiagGnssSv(diagGnssSv, gnssSvs[idx]);
        diagGnssSvPtr->gnssSvs[idx] = diagGnssSv;
    }
}

extern "C" {
void LogGnssLocation(const GnssLocation& gnssLocation, const LocationCapabilitiesMask& capsMask) {
    GnssLocationReport report(gnssLocation, capsMask);
    report.log();
}

void LogGnssSv(const std::vector<GnssSv>& gnssSvsVector) {
    GnssSvReport report(gnssSvsVector);
    report.log();
}

void LogGnssNmea(uint64_t timestamp, uint32_t length, const char* nmea) {
    GnssNmeaReport report(timestamp, length, nmea);
    report.log();
}

void LogGnssMeas(const GnssMeasurements& gnssMeasurements) {
    GnssMeasReport report(gnssMeasurements);
    report.log();
}

void LogGnssLatency(const GnssLatencyInfo& gnssLatencyInfo) {
    GnssLatencyReport report(gnssLatencyInfo);
    report.log();
}
}

}
