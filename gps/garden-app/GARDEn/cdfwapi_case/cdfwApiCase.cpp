/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "cdfwApiCase.h"
#include "QDgnssDataType.h"
#include <memory.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <qsocket.h>
#include <qsocket_ipcr.h>
#include <loc_cfg.h>
#include <loc_misc_utils.h>

#define BUFFER_SIZE  2000
static uint8_t  cdfwBuffer[BUFFER_SIZE];
static uint8_t  truncBuffer[BUFFER_SIZE];
#define RTCM_SOURCE_FILE "/etc/rtcmSourceFile.dat"
#define QDGNSS_OUTPUT_FILE "/data/vendor/location/qDgnssOutput.log"
#define RTCM_DOWNLOAD_FILE "/data/vendor/location/rtcmSourceFile.dat"

#define ACK_STRING "ICY 200 OK\r\n"
#define ACK_STRING_LEN  12
#define RTCM_FRAME_LEN  6

static const char gGardenString[] = "garden";

static FILE *fQDgnss = nullptr;
static FILE *fDownload = nullptr;
static bool gRemoteApi = false;
static bool gSourceReady = false;
static bool gSocket = false;

namespace garden {

void printStatusCb(const char* source, QDgnssSourceStatus statusUpdate) {
    switch (statusUpdate) {
    case CDFW_READY_TO_ACCEPT_DATA:
        gardenPrint("<<<%s CDFW_READY_TO_ACCEPT_DATA", source);
        gSourceReady = true;
        break;
    case CDFW_DATA_SOURCE_NOT_SUPPORTED:
        gardenPrint("<<<%s CDFW_DATA_SOURCE_NOT_SUPPORTED", source);
        gSourceReady = false;
        break;
    case CDFW_DATA_FORMAT_NOT_SUPPORTED:
        gardenPrint("<<<%s CDFW_DATA_FORMAT_NOT_SUPPORTED", source);
        gSourceReady = false;
        break;
    case CDFW_OTHER_SOURCE_IN_USE:
        gardenPrint("<<<%s CDFW_OTHER_SOURCE_IN_USE", source);
        gSourceReady = false;
        break;
    case CDFW_MESSAGE_PARSE_ERROR:
        gardenPrint("<<<%s CDFW_MESSAGE_PARSE_ERROR", source);
        //do not change gSourceReady on purpose;
         break;
    default:
        gardenPrint("<<<%s unknows status %d", source, statusUpdate);
    }
}

void fileSourceStatusCb(QDgnssSourceStatus  statusUpdate) {
    printStatusCb("fileSourceStatusCb", statusUpdate);
}

void internetSourceStatusCb(QDgnssSourceStatus  statusUpdate) {
    printStatusCb("internetSourceStatusCb", statusUpdate);
}

void writeObservationMsg(DGnssObservation* msg) {
    char buffer [300];
    int n;

    DGnssMeasurement* pMeasurement = msg->dGnssMeasurement;
    uint16_t* pUint16 = reinterpret_cast<uint16_t *>(&(msg->statusFlags));
    uint32_t* pUint32 = nullptr;
    uint8_t*  pUint8 = nullptr;
    n = snprintf (buffer, sizeof(buffer), "DGnss Observation: \n" \
                                 "Station ID: %u\n" \
                                 "Epoch Time: %u\n" \
                                 "Status Flags: 0X%hX\n" \
                                 "Issue Of Data Station: %u\n" \
                                 "Constellation Identifier: %u\n" \
                                 "Number Of Measurements: %u \n",
                  msg->stationID, msg->epochTime.timeOfWeek, *pUint16,
                  msg->issueOfDataStation, msg->constellationIdentifier,
                  msg->numberOfMeasurements);
    fwrite(buffer, n, 1, fQDgnss);
    for (int i=0; i < msg->numberOfMeasurements; i++) {
        memset(buffer, 0, sizeof(buffer));
        pUint32 = reinterpret_cast<uint32_t *>(&(pMeasurement->gnssSignalIdentifier));
        pUint8 = reinterpret_cast<uint8_t *>(&(pMeasurement->signalObservationStatus));
        n = snprintf (buffer, sizeof(buffer), "Measurement %u: \n" \
                            "\tSV Id: %u\n" \
                            "\tExtended Satellite Information: %u\n" \
                            "\tGnss Signal Identifier: %u\n" \
                            "\tSignal Observation Status: 0x%X\n" \
                            "\tPseudorange: %lf\n" \
                            "\tPhaserange: %lf\n" \
                            "\tPhaserange Rate: %lf\n" \
                            "\tLock Time Indicator: %u\n" \
                            "\tSignal CNR: %f\n",
                i+1, pMeasurement->svId, pMeasurement->extendedSatelliteInformation,
                *pUint32, *pUint8,
                pMeasurement->pseudorange, pMeasurement->phaseRange,
                pMeasurement->phaseRangeRate, pMeasurement->phaseRangeLockTimeIndicator,
                pMeasurement->signalCNR);
        fwrite(buffer, n, 1, fQDgnss);
        pMeasurement++;
    }
}

void writeReferenceStationMsg(ReferenceStation* pMsg)
{
    char buffer[600];
    int num;
    uint8_t* pUint8 = reinterpret_cast<uint8_t *>(&(pMsg->statusIndicator));

    num = snprintf(buffer, sizeof(buffer), "Reference Station Information:\n" \
                           "\tReference Station ID: %u\n" \
                           "\tReference Station Status Indicator: 0x%X\n" \
                           "\tLinked Reference Station ID: %u\n" \
                           "\tITRF-yy: %u\n" \
                           "\tConstellation Bit Mask: 0x%X\n" \
                           "\tARP ECEF-X: %lf\n" \
                           "\tARP ECEF-Y: %lf\n" \
                           "\tARP ECEF-Z: %lf\n" \
                           "\tAntenna height: %f\n" \
                           "\tARP Uncertainty-X: %f\n" \
                           "\tARP Uncertainty-Y: %f\n" \
                           "\tARP Uncertainty-Z: %f\n" \
                           "\tARP Uncertainty Confidence-X: %u\n" \
                           "\tARP Uncertainty Confidence-Y: %u\n" \
                           "\tARP Uncertainty Confidence-Z: %u\n",
                           pMsg->stationID, *pUint8,
                           pMsg->linkedStationID,
                           pMsg->ITRFYear, pMsg->constellationBitMask, pMsg->ecefX,
                           pMsg->ecefY, pMsg->ecefZ, pMsg->antennaHeight,
                           pMsg->uncertaintyX, pMsg->uncertaintyY, pMsg->uncertaintyZ,
                           pMsg->uncertaintyConfidenceX, pMsg->uncertaintyConfidenceY,
                           pMsg->uncertaintyConfidenceZ);
    fwrite(buffer, num, 1, fQDgnss);
}

void writeGlonassBiasMsg(GlonassBias* pMsg)
{
    char buffer[300];
    int num;

    num = snprintf(buffer, sizeof(buffer), "Glonass Bias Information:\n" \
                          "\tReference Station ID: %u\n" \
                          "\tGlonass Code Phase Bias Indicator and  FDMA Signal Mask: 0x%X\n" \
                          "\tGlonass L1 C/A Code Phase Bias: %f\n" \
                          "\tGlonass L1 P   Code Phase Bias: %f\n" \
                          "\tGlonass L2 C/A Code Phase Bias: %f\n" \
                          "\tGlonass L2 P   Code Phase Bias: %f\n",
                          pMsg->stationID, pMsg->biasIndicatorSignalMask, pMsg->biasL1CA,
                          pMsg->biasL1P, pMsg->biasL2CA, pMsg->biasL2P);
    fwrite(buffer, num, 1, fQDgnss);
}

void writeSSROrbitCorrectionsMsg(SsrOrbitCorrections* pMsg)
{
    char buffer[300];
    int num;
    int i;

    SsrSatelliteOrbitCorrections* pOrbitCorrections = pMsg->satOrbitCorrections;
    uint8_t* pUint8 = reinterpret_cast<uint8_t *>(&(pMsg->ssrHeader.statusFlags));
    num = snprintf(buffer, sizeof(buffer), "SSR Orbit Corrections:\n" \
                          "Constellation Identifier: %u\n" \
                          "Epoch Time: %u\n" \
                          "SSR Status Flags: 0x%X\n" \
                          "SSR Update Interval: %u\n" \
                          "SSR Provider ID: %u\n" \
                          "SSR Solution ID: %u\n" \
                          "Number Of Satellites: %u\n",
                          pMsg->ssrHeader.constellationIdentifier,
                          pMsg->ssrHeader.epochTime.timeOfWeek,
                          *pUint8,
                          pMsg->ssrHeader.updateInterval,
                          pMsg->ssrHeader.providerID, pMsg->ssrHeader.solutionID,
                          pMsg->ssrHeader.numberOfSatellites);
    fwrite(buffer, num, 1, fQDgnss);
    for (i = 0; i < pMsg->ssrHeader.numberOfSatellites; i++) {
        memset(buffer, 0, sizeof(buffer));
        num = snprintf(buffer, sizeof(buffer), "\tSV ID: %u\n" \
                          "\tIODE: %u\n" \
                          "\tDelta Radial: %f\n" \
                          "\tDelta Along-track: %f\n" \
                          "\tDelta Cross-track: %f\n" \
                          "\tDot Delta Radial: %f\n" \
                          "\tDot Delta Along-track: %f\n" \
                          "\tDot Delta Cross-track: %f\n",
                          pOrbitCorrections->svId, pOrbitCorrections->IODE,
                          pOrbitCorrections->deltaRadial, pOrbitCorrections->deltaAlongTrack,
                          pOrbitCorrections->deltaCrossTrack, pOrbitCorrections->dotDeltaRadial,
                          pOrbitCorrections->dotDeltaAlongTrack,
                          pOrbitCorrections->dotDeltaCrossTrack);
        fwrite(buffer, num, 1, fQDgnss);
        pOrbitCorrections++;
    }
}

void writeSSRClockCorrectionsMsg(SsrClockCorrections* pMsg)
{
    char buffer[300];
    int num;
    int i;

    SsrSatelliteClockCorrections* pClockCorrections = pMsg->satClockCorrections;
    uint8_t* pUint8 = reinterpret_cast<uint8_t *>(&(pMsg->ssrHeader.statusFlags));
    num = snprintf(buffer, sizeof(buffer), "SSR Clock Corrections:\n" \
                          "Constellation Identifier: %u\n" \
                          "Epoch Time: %u\n" \
                          "SSR Status Flags: 0x%X\n" \
                          "SSR Update Interval: %u\n" \
                          "SSR Provider ID: %u\n" \
                          "SSR Solution ID: %u\n" \
                          "Number Of Satellites: %u\n",
                          pMsg->ssrHeader.constellationIdentifier,
                          pMsg->ssrHeader.epochTime.timeOfWeek,
                          *pUint8,
                          pMsg->ssrHeader.updateInterval,
                          pMsg->ssrHeader.providerID, pMsg->ssrHeader.solutionID,
                          pMsg->ssrHeader.numberOfSatellites);
    fwrite(buffer, num, 1, fQDgnss);
    for (i = 0; i < pMsg->ssrHeader.numberOfSatellites; i++) {
        memset(buffer, 0, sizeof(buffer));
        num = snprintf(buffer, sizeof(buffer), "\tSV ID: %u\n" \
                          "\tDelta Clock C0: %f\n" \
                          "\tDelta Clock C1: %f\n" \
                          "\tDelta Clock C2: %lf\n",
                          pClockCorrections->svId, pClockCorrections->deltaClockC0,
                          pClockCorrections->deltaClockC1,
                          pClockCorrections->deltaClockC2);
        fwrite(buffer, num, 1, fQDgnss);
        pClockCorrections++;
    }
}

void writeSSRHighRateClockCorrectionsMsg(SsrHighRateClockCorrections* pMsg)
{
    char buffer[300];
    int num;
    int i;

    SsrSatelliteHighRateClockCorrections* pHighRateClockCorrections = \
        pMsg->satHighRateClockCorrections;
    uint8_t* pUint8 = reinterpret_cast<uint8_t *>(&(pMsg->ssrHeader.statusFlags));
    num = snprintf(buffer, sizeof(buffer), "SSR HighRate Clock Corrections:\n" \
                          "Constellation Identifier: %u\n" \
                          "Epoch Time: %u\n" \
                          "SSR Status Flags: 0x%X\n" \
                          "SSR Update Interval: %u\n" \
                          "SSR Provider ID: %u\n" \
                          "SSR Solution ID: %u\n" \
                          "Number Of Satellites: %u\n",
                          pMsg->ssrHeader.constellationIdentifier,
                          pMsg->ssrHeader.epochTime.timeOfWeek,
                          *pUint8,
                          pMsg->ssrHeader.updateInterval,
                          pMsg->ssrHeader.providerID, pMsg->ssrHeader.solutionID,
                          pMsg->ssrHeader.numberOfSatellites);
    fwrite(buffer, num, 1, fQDgnss);
    for (i = 0; i < pMsg->ssrHeader.numberOfSatellites; i++) {
        memset(buffer, 0, sizeof(buffer));
        num = snprintf(buffer, sizeof(buffer), "\tSV ID: %u\n" \
                          "\tHigh Rate Clock Correction: %f\n",
                          pHighRateClockCorrections->svId,
                          pHighRateClockCorrections->highRateClockCorrection);
        fwrite(buffer, num, 1, fQDgnss);
        pHighRateClockCorrections++;
    }
}

void writeSSRCodeBiasMsg(SsrCodeBias* pMsg)
{
    char buffer[300];
    int num;
    int i, j;

    SsrSatelliteCodeBias* pSatCodeBias = pMsg->satCodeBias;
    uint8_t* pUint8 = reinterpret_cast<uint8_t *>(&(pMsg->ssrHeader.statusFlags));
    num = snprintf(buffer, sizeof(buffer), "SSR Code Bias:\n" \
                          "Constellation Identifier: %u\n" \
                          "Epoch Time: %u\n" \
                          "SSR Status Flags: 0x%X\n" \
                          "SSR Update Interval: %u\n" \
                          "SSR Provider ID: %u\n" \
                          "SSR Solution ID: %u\n" \
                          "Number Of Satellites: %u\n",
                          pMsg->ssrHeader.constellationIdentifier,
                          pMsg->ssrHeader.epochTime.timeOfWeek,
                          *pUint8,
                          pMsg->ssrHeader.updateInterval,
                          pMsg->ssrHeader.providerID, pMsg->ssrHeader.solutionID,
                          pMsg->ssrHeader.numberOfSatellites);
    fwrite(buffer, num, 1, fQDgnss);
    for (i = 0; i < pMsg->ssrHeader.numberOfSatellites; i++) {
        memset(buffer, 0, sizeof(buffer));
        num = snprintf(buffer, sizeof(buffer), "\tSV ID: %u\n" \
                          "\tNumber Of Code Biases Processed: %u\n",
                          pSatCodeBias->svId, pSatCodeBias->numberOfCodeBias);
        fwrite(buffer, num, 1, fQDgnss);
        for (j = 0; j < pSatCodeBias->numberOfCodeBias; j++) {
            memset(buffer, 0, sizeof(buffer));
            num = snprintf(buffer, sizeof(buffer), "\t\tSignal Identifier: %u, Code Bias: %f\n",
                          pSatCodeBias->codeBias[j].signalIdentifier,
                          pSatCodeBias->codeBias[j].codeBias);
            fwrite(buffer, num, 1, fQDgnss);
        }
        pSatCodeBias++;
    }
}

void writeSSRUserRangeAccuracyMsg(SsrUserRangeAccuracy* pMsg)
{
    char buffer[300];
    int num;
    int i;

    SsrSatelliteUserRangeAccuracy* pUserRangeAccuracy = pMsg->satUserRangeAccuracy;
    uint8_t* pUint8 = reinterpret_cast<uint8_t *>(&(pMsg->ssrHeader.statusFlags));
    num = snprintf(buffer, sizeof(buffer), "SSR User Range Accuracy:\n" \
                          "Constellation Identifier: %u\n" \
                          "Epoch Time: %u\n" \
                          "SSR Status Flags: 0x%X\n" \
                          "SSR Update Interval: %u\n" \
                          "SSR Provider ID: %u\n" \
                          "SSR Solution ID: %u\n" \
                          "Number Of Satellites: %u\n",
                          pMsg->ssrHeader.constellationIdentifier,
                          pMsg->ssrHeader.epochTime.timeOfWeek,
                          *pUint8,
                          pMsg->ssrHeader.updateInterval,
                          pMsg->ssrHeader.providerID, pMsg->ssrHeader.solutionID,
                          pMsg->ssrHeader.numberOfSatellites);
    fwrite(buffer, num, 1, fQDgnss);
    for (i = 0; i < pMsg->ssrHeader.numberOfSatellites; i++) {
        memset(buffer, 0, sizeof(buffer));
        num = snprintf(buffer, sizeof(buffer), "\tSV ID: %u  URA: %u\n",
                          pUserRangeAccuracy->svId,
                          pUserRangeAccuracy->userRangeAccuracy);
        fwrite(buffer, num, 1, fQDgnss);
        pUserRangeAccuracy++;
    }
}

void writeSSROrbitAndClockCorrectionsMsg(SsrOrbitAndClockCorrections* pMsg)
{
    char buffer[300];
    int num;
    int i;

    SsrSatelliteOrbitCorrections* pOrbitCorrections = pMsg->satOrbitCorrections;
    SsrSatelliteClockCorrections* pClockCorrections = pMsg->satClockCorrections;
    uint8_t* pUint8 = reinterpret_cast<uint8_t *>(&(pMsg->ssrHeader.statusFlags));
    num = snprintf(buffer, sizeof(buffer), "SSR Combined Orbit And Clock Corrections:\n" \
                          "Constellation Identifier: %u\n" \
                          "Epoch Time: %u\n" \
                          "SSR Status Flags: 0x%X\n" \
                          "SSR Update Interval: %u\n" \
                          "SSR Provider ID: %u\n" \
                          "SSR Solution ID: %u\n" \
                          "Number Of Satellites: %u\n",
                          pMsg->ssrHeader.constellationIdentifier,
                          pMsg->ssrHeader.epochTime.timeOfWeek,
                          *pUint8,
                          pMsg->ssrHeader.updateInterval,
                          pMsg->ssrHeader.providerID, pMsg->ssrHeader.solutionID,
                          pMsg->ssrHeader.numberOfSatellites);
    fwrite(buffer, num, 1, fQDgnss);
    for (i = 0; i < pMsg->ssrHeader.numberOfSatellites; i++) {
        memset(buffer, 0, sizeof(buffer));
        num = snprintf(buffer, sizeof(buffer), "\tSV ID: %u\n" \
                          "\tIODE: %u\n" \
                          "\tDelta Radial: %f\n" \
                          "\tDelta Along-track: %f\n" \
                          "\tDelta Cross-track: %f\n" \
                          "\tDot Delta Radial: %f\n" \
                          "\tDot Delta Along-track: %f\n" \
                          "\tDot Delta Cross-track: %f\n" \
                          "\tDelta Clock C0: %f\n" \
                          "\tDelta Clock C1: %f\n" \
                          "\tDelta Clock C2: %lf\n",
                          pOrbitCorrections->svId, pOrbitCorrections->IODE,
                          pOrbitCorrections->deltaRadial, pOrbitCorrections->deltaAlongTrack,
                          pOrbitCorrections->deltaCrossTrack, pOrbitCorrections->dotDeltaRadial,
                          pOrbitCorrections->dotDeltaAlongTrack,
                          pOrbitCorrections->dotDeltaCrossTrack,
                          pClockCorrections->deltaClockC0,
                          pClockCorrections->deltaClockC1,
                          pClockCorrections->deltaClockC2);
        fwrite(buffer, num, 1, fQDgnss);
        pOrbitCorrections++;
        pClockCorrections++;
    }
}

void writeGpsEphemerisMsg(gpsEphemerisType* pMsg)
{
    char buffer[600];
    int num;

    num = snprintf(buffer, sizeof(buffer), "Gps Ephemeris:\n" \
                          "\tSV ID: %u\n" \
                          "\tCode L2: 0x%X\n" \
                          "\tAccuracy: %u\n" \
                          "\tHealth: %u\n" \
                          "\tIodc: %u\n" \
                          "\tP Code Flag : %u\n" \
                          "\tIs PCode Flag Valid: %u\n" \
                          "\tAre SF1 bits valid: %u\n" \
                          "\tTgd: %u\n" \
                          "\tIs Tgd Valid: %u\n" \
                          "\tToc: %u\n" \
                          "\tAf2: %u\n" \
                          "\tAf1: %u\n" \
                          "\tAf0: %u\n" \
                          "\tIode: %u\n" \
                          "\tCrs: %u\n" \
                          "\tDeltaN: %u\n" \
                          "\tM0: %u\n" \
                          "\tCuc: %u\n" \
                          "\te: %u\n" \
                          "\tCus: %u\n" \
                          "\tSqrtA: %u\n" \
                          "\tToe: %u\n" \
                          "\tFit Interval Flag: %u\n" \
                          "\tIs Fit Interval Flag Valid: %u\n" \
                          "\tCic: %u\n" \
                          "\tOmega0: %u\n" \
                          "\tCis: %u\n" \
                          "\tI0: %u\n" \
                          "\tCrc: %u\n" \
                          "\tOmega %u\n" \
                          "\tOmegaDot: %u\n" \
                          "\tiDot: %u\n" \
                          "\tGps Week: %u\n",
                          pMsg->svId, pMsg->codeL2, pMsg->accuracy, pMsg->health, pMsg->iodc,
                          pMsg->pcodeFlag, pMsg->isPcodeFlagValid, pMsg->areSf1BitsValid,
                          pMsg->tgd, pMsg->isTgdValid, pMsg->toc, pMsg->af2, pMsg->af1,
                          pMsg->af0, pMsg->iode, pMsg->Crs, pMsg->deltaN, pMsg->M0, pMsg->Cuc,
                          pMsg->e, pMsg->Cus, pMsg->sqrtA, pMsg->toe, pMsg->fitIntervalFlag,
                          pMsg->isFitIntervalFlagValid, pMsg->Cic, pMsg->Omega0, pMsg->Cis,
                          pMsg->i0, pMsg->Crc, pMsg->omega, pMsg->omegaDot,
                          pMsg->idot, pMsg->gpsWeek);
    fwrite(buffer, num, 1, fQDgnss);
}

void writeGlonassEphemerisMsg(glonassEphemerisType* pMsg)
{
    char buffer[600];
    int num;

    num = snprintf(buffer, sizeof(buffer), "Glonass Ephemeris:\n"
                          "\tSV ID: %u\n"
                          "\tFilled Mask: 0x%X\n"
                          "\ttb: %u\n"
                          "\tGlonass Type: %u\n"
                          "\ttimeSrcP : %u\n"
                          "\tBn: %u\n"
                          "\ttimeDiffL1L2: %u\n"
                          "\tP1: %u\n"
                          "\tP2: %u\n"
                          "\tln: %u\n"
                          "\tFrequency Number: %u\n"
                          "\tFt: %u\n"
                          "\tacceleration: %u %u %u\n"
                          "\tNt: %u\n"
                          "\tFreqOff: %u\n"
                          "\tclock Correction: %u\n"
                          "\tsatellite Position: %u %u %u\n"
                          "\tsatellite Velocity: %u %u %u\n"
                          "\tEn: %u\n"
                          "\tP4: %u\n",
                          pMsg->svId, pMsg->mask.filledMask, pMsg->tb, pMsg->gloType,
                          pMsg->timeSrcP, pMsg->Bn, pMsg->timeDiffL1L2, pMsg->P1,
                          pMsg->P2, pMsg->ln, pMsg->freqNumber, pMsg->Ft, pMsg->acceleration[0],
                          pMsg->acceleration[1], pMsg->acceleration[2], pMsg->Nt, pMsg->freqOff,
                          pMsg->clockCorrection, pMsg->satellitePosition[0],
                          pMsg->satellitePosition[1], pMsg->satellitePosition[2],
                          pMsg->satelliteVelocity[0], pMsg->satelliteVelocity[1],
                          pMsg->satelliteVelocity[2], pMsg->En, pMsg->P4);
    fwrite(buffer, num, 1, fQDgnss);
}

void writeGalileoEphemerisMsg(galileoEphemerisType* pMsg)
{
    char buffer[600];
    int num;

    num = snprintf(buffer, sizeof(buffer), "Galileo Ephemeris:\n"
                          "\tSV ID: %u\n"
                          "\tSisa: %u\n"
                          "\tIode: %u\n"
                          "\tToe: %u\n"
                          "\tM0: %u\n"
                          "\tDeltaN: %u\n"
                          "\te: %u\n"
                          "\tSqrtA: %u\n"
                          "\tOmega0: %u\n"
                          "\tOmega %u\n"
                          "\tOmegaDot: %u\n"
                          "\tI0: %u\n"
                          "\tiDot: %u\n"
                          "\tCic: %u\n"
                          "\tCis: %u\n"
                          "\tCrc: %u\n"
                          "\tCrs: %u\n"
                          "\tCuc: %u\n"
                          "\tCus: %u\n"
                          "\tGalileo Week: %u\n"
                          "\tHealth : %u\n"
                          "\tToc: %u\n"
                          "\tAf2: %u\n"
                          "\tAf1: %u\n"
                          "\tAf0: %u\n"
                          "\tbgdE1E5a: %u\n"
                          "\tbgdE1E5b: %u\n"
                          "\tTgd: %u\n"
                          "\tIs Fnav: %u\n",
                          pMsg->svId, pMsg->sisa, pMsg->iode, pMsg->toe, pMsg->M0,
                          pMsg->deltaN, pMsg->e, pMsg->sqrtA, pMsg->Omega0, pMsg->omega,
                          pMsg->omegaDot, pMsg->i0, pMsg->idot, pMsg->Cic, pMsg->Cis,
                          pMsg->Crc, pMsg->Crs, pMsg->Cuc, pMsg->Cus, pMsg->galileoWeek,
                          pMsg->health, pMsg->toc, pMsg->af2, pMsg->af1, pMsg->af0,
                          pMsg->bgdE1E5a, pMsg->bgdE1E5b, pMsg->tgd, pMsg->isFnav);
    fwrite(buffer, num, 1, fQDgnss);
}

void writeBdsEphemerisMsg(bdsMbRawEphemerisType* pMsg)
{
    char buffer[600];
    int num;

    num = snprintf(buffer, sizeof(buffer), "BDS Ephemeris:\n"
                          "\tSV ID: %u\n"
                          "\tBds Week: %u\n"
                          "\ttoe: %u\n"
                          "\ttoc: %u\n"
                          "\taode: %u\n"
                          "\taodc: %u\n"
                          "\thealth: %u\n"
                          "\tURAI: %u\n"
                          "\tSqrtA: %u\n"
                          "\te: %u\n"
                          "\tOmega %u\n"
                          "\tDeltaN: %u\n"
                          "\tM0: %u\n"
                          "\tOmega0: %u\n"
                          "\tOmegaDot: %u\n"
                          "\tI0: %u\n"
                          "\tiDot: %u\n"
                          "\tCrs: %u\n"
                          "\tCuc: %u\n"
                          "\tCus: %u\n"
                          "\tCic: %u\n"
                          "\tCis: %u\n"
                          "\tCrc: %u\n"
                          "\tA2: %u\n"
                          "\tA1: %u\n"
                          "\tA0: %u\n"
                          "\tTgd: %u\n"
                          "\tTgd2: %u\n"
                          "\tIs Tgd2 Valid: %u\n",
                          pMsg->bdsEph.svId, pMsg->bdsEph.bdsWeek, pMsg->bdsEph.toe,
                          pMsg->bdsEph.toc, pMsg->bdsEph.aode, pMsg->bdsEph.aodc,
                          pMsg->bdsEph.health, pMsg->bdsEph.URAI, pMsg->bdsEph.sqrtA,
                          pMsg->bdsEph.e, pMsg->bdsEph.omega, pMsg->bdsEph.deltaN,
                          pMsg->bdsEph.M0, pMsg->bdsEph.Omega0, pMsg->bdsEph.omegaDot,
                          pMsg->bdsEph.i0, pMsg->bdsEph.idot, pMsg->bdsEph.Crs,
                          pMsg->bdsEph.Cuc, pMsg->bdsEph.Cus, pMsg->bdsEph.Cic,
                          pMsg->bdsEph.Cis, pMsg->bdsEph.Crc, pMsg->bdsEph.a2,
                          pMsg->bdsEph.a1, pMsg->bdsEph.a0, pMsg->bdsEph.tgd,
                          pMsg->tgd2, pMsg->isTgd2Valid);
    fwrite(buffer, num, 1, fQDgnss);
}


void QDgnssFileListener::onReceiveMsg(unique_ptr<IQDgnssMsg>& dGnssMsg) {
    if (nullptr == dGnssMsg) {
        gardenPrint("QDgnssFileListener null dGnssMsg");
        return;
    }
    gardenPrint("<<< QDgnssFileListener MsgID=0x%x, size=%d",
                dGnssMsg->getMsgId(), dGnssMsg->getMsgSize());
    if (nullptr == fQDgnss) {
        fQDgnss = fopen(QDGNSS_OUTPUT_FILE, "w");
        if (nullptr == fQDgnss) {
            gardenPrint("open output file %s failed: %s", QDGNSS_OUTPUT_FILE, strerror(errno));
            return;
        } else {
            gardenPrint("output file %s opened", QDGNSS_OUTPUT_FILE);
        }
    }

    void* pDgnssMsg = dGnssMsg->getMsg();
    uint32_t msgId = dGnssMsg->getMsgId();

    switch (msgId) {
    case QDGNSS_REFERENCE_STATION_MSG_ID:
        writeReferenceStationMsg((ReferenceStation*)pDgnssMsg);
        break;
    case QDGNSS_OBSERVATIONS_MSG_ID:
        writeObservationMsg((DGnssObservation*)pDgnssMsg);
        break;
    case QDGNSS_GLONASS_BIAS_MSG_ID:
        writeGlonassBiasMsg((GlonassBias*)pDgnssMsg);
        break;
    case QDGNSS_SSR_ORBIT_CORRECTIONS_MSG_ID:
        writeSSROrbitCorrectionsMsg((SsrOrbitCorrections*)pDgnssMsg);
        break;
    case QDGNSS_SSR_CLOCK_CORRECTIONS_MSG_ID:
        writeSSRClockCorrectionsMsg((SsrClockCorrections*)pDgnssMsg);
        break;
    case QDGNSS_SSR_HIGHRATE_CLOCK_CORRECTIONS_MSG_ID:
        writeSSRHighRateClockCorrectionsMsg((SsrHighRateClockCorrections*)pDgnssMsg);
        break;
    case QDGNSS_SSR_CODE_BIAS_MSG_ID:
        writeSSRCodeBiasMsg((SsrCodeBias*)pDgnssMsg);
        break;
    case QDGNSS_SSR_USER_RANGE_ACCURACY_MSG_ID:
        writeSSRUserRangeAccuracyMsg((SsrUserRangeAccuracy*)pDgnssMsg);
        break;
    case QDGNSS_SSR_ORBIT_AND_CLOCK_MSG_ID:
        writeSSROrbitAndClockCorrectionsMsg((SsrOrbitAndClockCorrections*)pDgnssMsg);
        break;
    case QDGNSS_GPS_EPHEMERIS_MSG_ID:
    case QDGNSS_QZSS_EPHEMERIS_MSG_ID:
        writeGpsEphemerisMsg((gpsEphemerisType*)pDgnssMsg);
        break;
    case QDGNSS_GLONASS_EPHEMERIS_MSG_ID:
        writeGlonassEphemerisMsg((glonassEphemerisType*)pDgnssMsg);
        break;
    case QDGNSS_GALILEO_EPHEMERIS_MSG_ID:
        writeGalileoEphemerisMsg((galileoEphemerisType*)pDgnssMsg);
        break;
    case QDGNSS_BDS_EPHEMERIS_MSG_ID:
        writeBdsEphemerisMsg((bdsMbRawEphemerisType*)pDgnssMsg);
        break;
    default:
        break;
    }
}

void QDgnssInternetListener::onReceiveMsg(unique_ptr<IQDgnssMsg>& dGnssMsg) {
    gardenPrint("<<< QDgnssInternetListener MsgID=0x%x, size=%d",
                dGnssMsg->getMsgId(), dGnssMsg->getMsgSize());
}

void InjectDataTimer::timeOutCallback() {
    struct ReadFileTimerMsg : public LocMsg {
        CDFWApiCase& mCase;
        inline ReadFileTimerMsg(CDFWApiCase& apiCase) :
            LocMsg(), mCase(apiCase) {};
        inline virtual void proc() const {
            mCase.injectDataFromFile();
        }
    };

    mMsgTask.sendMsg(new ReadFileTimerMsg(mApiCase));
}

MsgTask* CDFWApiCase::mMsgTask = nullptr;

CDFWApiCase::CDFWApiCase()
{
    mNtripClientStarted = false;
    mNtripClientStartDataStreamingApi = nullptr;
    mNtripClientStopDataStreamingApi = nullptr;
    if (nullptr == mMsgTask) {
        mMsgTask = new MsgTask("cdfwApiCase");
    }
    mInjectDataTimer = new InjectDataTimer(*this, *mMsgTask);
    mNtripClientConfFile = "/etc/izat.conf";
    static loc_param_s_type ntrip_client_conf_param_table[] = {
    {"GARDEN_NTRIP_HOST_NAME", mHostName, NULL, 's'},
    {"GARDEN_NTRIP_MOUNT_POINT", mMountPoint, NULL, 's'},
    {"GARDEN_NTRIP_USERNAME_PWD_BASE64_FORMAT", mUserNamePwdInBase64Format, NULL, 's'},
    {"GARDEN_NTRIP_PORT_NUMBER", &mPort, NULL, 'n'},
    {"GARDEN_NTRIP_DATA_LOGGING_TO_FILE", &mDataLoggingToFile, NULL, 'n'}
    };
    UTIL_READ_CONF(mNtripClientConfFile,ntrip_client_conf_param_table);
    mNtripConfigs.paramsMask = 1; //NTRIP_CONFIG_PARAMS_HAS_MAX_CONTENT_DATA_BUFFER_SIZE_BIT
    mNtripConfigs.maxContentDataBufferSizeInBytes = BUFFER_SIZE;

}

CDFWApiCase::~CDFWApiCase()
{
    closeFile();
    if (nullptr != mpSource) {
        mpSource->unsubscribe();
    }
    gardenPrint("~CDFWApiCase");
}

GARDEN_RESULT CDFWApiCase::preRun()
{
    gardenPrint("preRun");
    if (mpSource) {
        mpSource->unsubscribe();
    }
    return GARDEN_RESULT_PASSED;
}

GARDEN_RESULT CDFWApiCase::postRun()
{
    gardenPrint("postRun");
    return GARDEN_RESULT_PASSED;
}

void CDFWApiCase::closeFile()
{
    if (mFrtcm) {
        fclose (mFrtcm);
        gardenPrint("close %s", RTCM_SOURCE_FILE);
        mFrtcm = nullptr;
        mIsFileOpen == false;
        mTotalReadSize = 0;
        mInjectDataTimer->stop();
    }
    if (fQDgnss) {
        fclose (fQDgnss);
        gardenPrint("close %s", QDGNSS_OUTPUT_FILE);
        fQDgnss = nullptr;
    }
    if (fDownload) {
        fclose (fDownload);
        gardenPrint("close %s", RTCM_DOWNLOAD_FILE);
        fDownload = nullptr;
    }
}

void CDFWApiCase::injectDataFromFile()
{
    int c;
    uint32_t readSize = 0;
    uint8_t* pTemp = cdfwBuffer;
    uint8_t  tempData[2];
    int ret;

    if (false == gRemoteApi) {
        if (nullptr == mFileSource) {
            mFileSource = make_shared<QDgnssSource>(
            CDFW_DATA_SOURCE_CV2X, CDFW_DATA_FORMAT_RTCM_3, fileSourceStatusCb);
            gardenPrint("shared ptr mFileSource");
        }
        if (nullptr == mFileListener) {
            mFileListener = unique_ptr<QDgnssFileListener>(
               new QDgnssFileListener(*this, *mMsgTask));
        }
    } else {
        if (nullptr == mpSource) {
            mpSource = QDgnssRemoteSource::createSource(
                CDFW_DATA_SOURCE_CV2X, CDFW_DATA_FORMAT_RTCM_3,
                fileSourceStatusCb, gGardenString);
            if (nullptr == mpSource) {
                gardenPrint("Remote API createSource failed");
                return;
            }
        }
    }

    if (false == mIsFileOpen) {
        mFrtcm = fopen(RTCM_SOURCE_FILE, "rb");
        if (nullptr == mFrtcm) {
            gardenPrint("open rtcm file %s failed: %s", RTCM_SOURCE_FILE, strerror(errno));
            return;
        } else {
            gardenPrint("rtcm file %s opened", RTCM_SOURCE_FILE);
        }
        mIsFileOpen = true;
    }

    memset(cdfwBuffer, 0, BUFFER_SIZE);
    do {
        ret = fread(tempData, 1, 2, mFrtcm);
        if (tempData[0] == '\r' && tempData[1] == '\n'){
            break;
        } else {
           *pTemp = tempData[0];
           *(pTemp+1) = tempData[1];
           pTemp += 2;
           readSize += 2;
        }
    } while (readSize < BUFFER_SIZE);

    //if end of file, close file
    if (ret < 2) {
        gardenPrint("injectDataFromFile is done");
        closeFile();
        return;
    }

    // injectData, startTime
    if (readSize) {
        gardenPrint("read in %d bytes cdfw buffer", readSize);
        mTotalReadSize += readSize;
        if (false == gRemoteApi) {
          mFileSource->injectCorrectionData(cdfwBuffer, readSize);
        } else {
            if (!gSourceReady) {
                gardenPrint("source ready not received yet");
            }
            mpSource->injectCorrectionData(cdfwBuffer, readSize);
        }

        if (mIsFileOpen) {
            mInjectDataTimer->start(200, false);
        }
    }
}

void CDFWApiCase::injectDataFromInternet()
{
    if (false == gRemoteApi) {
        if (nullptr == mInternetSource) {
            mInternetSource = unique_ptr<QDgnssSource>(new QDgnssSource(
                CDFW_DATA_SOURCE_CV2X, CDFW_DATA_FORMAT_RTCM_3, internetSourceStatusCb));
        }
        if (nullptr == mInternetListener) {
            mInternetListener = unique_ptr<QDgnssInternetListener>(
               new QDgnssInternetListener());
        }
    } else {
        if (nullptr == mpSource) {
            mpSource = QDgnssRemoteSource::createSource(
                CDFW_DATA_SOURCE_CV2X, CDFW_DATA_FORMAT_RTCM_3,
                internetSourceStatusCb, gGardenString);
            if (nullptr == mpSource) {
                gardenPrint("Remote API createSource failed");
                return;
            }
        }
    }

    if (nullptr == fDownload) {
        fDownload = fopen(RTCM_DOWNLOAD_FILE, "wb");
        if (nullptr == fDownload) {
            gardenPrint("open download file %s failed: %s",
                        RTCM_DOWNLOAD_FILE, strerror(errno));
            return;
        } else {
            gardenPrint("download file %s opened",RTCM_DOWNLOAD_FILE);
        }
    }

    gardenPrint("HOST %s:%d %s", mHostName, mPort, mMountPoint);
    if (false == gSocket) {
        ntripClientApiConnect();
    } else {

        struct SocketConnectMsg : public LocMsg {
            CDFWApiCase& mCase;
            inline SocketConnectMsg(CDFWApiCase& apiCase) :
                LocMsg(), mCase(apiCase) {};
            inline virtual void proc() const {
                mCase.socketConnect();
            }
        };

        mMsgTask->sendMsg(new SocketConnectMsg(*this));
    }
}

void CDFWApiCase::ntripClientApiConnect() {
    void* libhandle = nullptr;
    const char* libName = "libntripclient.so";
    if (nullptr == mNtripClientStartDataStreamingApi) {
        mNtripClientStartDataStreamingApi =
            (NtripClientStartDataStreamingApi*)dlGetSymFromLib(libhandle, libName,
                                                 "startCorrectionDataStreamingInt");
    }
    if (nullptr == mNtripClientStopDataStreamingApi) {
        mNtripClientStopDataStreamingApi =
            (NtripClientStopDataStreamingApi*)dlGetSymFromLib(libhandle, libName,
                                                 "stopCorrectionDataStreamingInt");
    }
    if ((nullptr == mNtripClientStopDataStreamingApi) ||
            (nullptr == mNtripClientStartDataStreamingApi)) {
        gardenPrint("dlGetSymFromLib failed");
        return;
    }

    CorrectionDataCb corrDataCb = [&] (uint8_t *revBuffer, uint32_t lengthInBytes) {
        static bool ackStringReceived = false;
        if (false == ackStringReceived) {
            if (0 == strncmp(ACK_STRING, (char*)revBuffer, ACK_STRING_LEN)) {
                gardenPrint("ICY 200 OK ACK_STRING is found");
                ackStringReceived = true;
                revBuffer += ACK_STRING_LEN;
                lengthInBytes -= ACK_STRING_LEN;
            }
        }
        gardenPrint("\nReceived %lu bytes cdfw buffer", lengthInBytes);
        if (lengthInBytes >0 && lengthInBytes <= BUFFER_SIZE ) {
            concateAndInject(revBuffer, lengthInBytes);
        }
    };

    if (false == mNtripClientStarted) {
        mNtripClientStartDataStreamingApi(false, mHostName, mMountPoint,
            mUserNamePwdInBase64Format, mPort, mNtripConfigs, corrDataCb);
    }
    mNtripClientStarted = true;
}

void CDFWApiCase::injectDataToCdfw(uint8_t* buffer, uint32_t bufferSize) {
    char newline[] = {'\r','\n'};

    if (!gSourceReady) {
        gardenPrint("source ready not received yet");
    }

    if (false == gRemoteApi) {
        mInternetSource->injectCorrectionData(buffer, bufferSize);
    } else {
        mpSource->injectCorrectionData(buffer, bufferSize);
    }

    if (mDataLoggingToFile) {
        fwrite(buffer, bufferSize, 1, fDownload);
        fwrite(newline, 2, 1, fDownload);
    }
}

void CDFWApiCase::concateAndInject(uint8_t* buffer, uint32_t recvBytes) {

    static bool     appendBuffer = false;
    static int      msgLength, appendOffset;
    int             i, msgId;

    if (appendBuffer) {
        appendBuffer = false;
        i = msgLength - appendOffset + RTCM_FRAME_LEN;
        if (i < recvBytes) {
            gardenPrint("Append %d bytes to truncBuffer, recvBytes = %d\n",
                        i, recvBytes);

            memcpy(truncBuffer+appendOffset, buffer, i);
            injectDataToCdfw(truncBuffer, msgLength + RTCM_FRAME_LEN);
        } else {
            gardenPrint("Error: recvBytes %d < %d",
                        recvBytes, msgLength + RTCM_FRAME_LEN);
            return;
        }
    } else {
        i = 0;
    }

    for(; i< (recvBytes - 4);) {
        if((buffer[i] == 0xD3) && ((buffer[i+1] & 0xFC) == 0x00)) {
            msgLength = buffer[i+2];
            msgLength |= (buffer[i+1] & 0x03) << 8;
            msgId = buffer[i+3] << 4;
            msgId |= buffer[i+4] >> 4;
            gardenPrint("Found RTCM msgId %d, length %d at buffer[%d]",
                        msgId, msgLength, i);

            if (i + msgLength + RTCM_FRAME_LEN > recvBytes) {
                memset(truncBuffer, 0, sizeof(truncBuffer));
                memcpy(truncBuffer, buffer+i, recvBytes-i);
                appendBuffer = true;
                appendOffset = recvBytes-i;
                gardenPrint("Copy %d bytes to truncBuffer", recvBytes-i);
                i += appendOffset;
            } else {
                injectDataToCdfw(buffer+i, msgLength+RTCM_FRAME_LEN);
                i += (msgLength + RTCM_FRAME_LEN);
            }
         } else {
            i += 1;
         }
    }
}

void CDFWApiCase::socketConnect() {
    int socket_fd;
    struct sockaddr_in serv_addr;
    int ret;
    char con_request[256];

    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        gardenPrint("socket creat failed!\n");
    }
    else {
        gardenPrint("socket creat sucess");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(mPort);
    serv_addr.sin_addr.s_addr = inet_addr(mHostName);

    gardenPrint("conneting to server...");
    ret = connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        gardenPrint("connect failed!\n");
    }
    else {
        gardenPrint("connect sucess \n");
    }

    memset(&con_request, 0, sizeof(con_request));
    memset(&cdfwBuffer, 0 , sizeof(cdfwBuffer));

    strlcat(con_request, "GET /", sizeof(con_request));
    strlcat(con_request, mMountPoint, sizeof(con_request));
    strlcat(con_request, " HTTP/1.1\r\n", sizeof(con_request));
    strlcat(con_request, "User-Agent: NTRIP GNR/1.0.0 (Win32)\r\n", sizeof(con_request));
    strlcat(con_request, "Authorization: Basic ", sizeof(con_request));
    strlcat(con_request, mUserNamePwdInBase64Format, sizeof(con_request));
    strlcat(con_request, "\r\nConnection: close\r\n\r\n", sizeof(con_request));

    ret = send(socket_fd, con_request, strlen(con_request), 0);
    gardenPrint("con_request=%s\n", con_request);
    gardenPrint("send ret =%d.\n",ret);
    ret = recv(socket_fd, cdfwBuffer, sizeof(cdfwBuffer), 0);
    if(ret < 0 ) {
        gardenPrint("recv failed.\n");
    }
    else if (ret > 0 && !strncmp(ACK_STRING, (char*)cdfwBuffer, ACK_STRING_LEN)) {
        while(1) {
            memset(cdfwBuffer, 0, sizeof(cdfwBuffer));
            ret = recv(socket_fd, cdfwBuffer, sizeof(cdfwBuffer),  0);
            if(ret < 0) {
                gardenPrint("fetchOnePacket:recv failed!\n");
            } else {
                gardenPrint("fetchOnePacket %d bytes\n", ret);
                concateAndInject(cdfwBuffer, ret);
            }
        }
    } else {
        gardenPrint("could not get the request data");
    }
}

GARDEN_RESULT CDFWApiCase::menuCase()
{
    char buf[16], *p;
    bool exit_loop = false;

    while (!exit_loop) {
        menuPrint("\n\n"
                  "1: Inject data from file\n"
                  "2: Inject data from server\n"
                  "3: Remote Inject data from file\n"
                  "4: Remote Inject data from server(socket)\n"
                  "5: Remote Inject data from server(ntrip client API)\n"
                  "b: back\n"
                  "q: quit\n"
                  "Enter Command: \n");
        fflush(stdout);
        p = fgets(buf, 16, stdin);
        if (nullptr == p) {
            GARDEN_ERROR("ERROR:fgets returned nullptr!");
        }
        switch (p[0]) {
            case '1':
                gRemoteApi = false;
                injectDataFromFile();
                break;
            case '2':
                gRemoteApi = false;
                gSocket = true;
                injectDataFromInternet();
                break;
            case '3':
                gRemoteApi = true;
                injectDataFromFile();
                break;
            case '4':
                gRemoteApi = true;
                gSocket = true;
                injectDataFromInternet();
                break;
            case '5':
                gRemoteApi = true;
                gSocket = false;
                injectDataFromInternet();
                break;
            case 'b':
                exit_loop = true;
                break;
            case 'q':
                if (mNtripClientStarted) {
                    if (nullptr != mNtripClientStopDataStreamingApi) {
                        mNtripClientStopDataStreamingApi();
                    } else {
                        gardenPrint("mNtripClientStopDataStreamingApi is null");
                    }
                    mNtripClientStarted = false;
                }
                closeFile();
                return GARDEN_RESULT_ABORT;
                break;
            default:
                gardenPrint("\nInvalid command\n");
        }
    }
    return GARDEN_RESULT_PASSED;
}
} /* namespace */

