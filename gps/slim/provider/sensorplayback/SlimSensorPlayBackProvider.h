/*
   Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
   */
/**
  @file
  @brief Sensor PlayBack provider interface file.

  This file declares Sensor PlayBack provider object.

  @ingroup slim_SensorPlayBackProvider
  */
#ifndef SLIM_SENSOR_PLAYBACK_H_INCLUDED
#define SLIM_SENSOR_PLAYBACK_H_INCLUDED

#include <MultiplexingProvider.h>
#include <slim_api.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <loc_cfg.h>
#include <inttypes.h>
#include <slim_os_log_api.h>
#include <slim_os_api.h>
#include "SlimSensorPlayBackProvider.h"
#include <loc_cfg.h>


#include <linux/input.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <algorithm>
#include <fstream>
#include <dlfcn.h>
#include <loc_pla.h>
#include <loc_cfg.h>
#include "diag_lsm.h"
//#include "diag_lsm_dci.h"
//#include <platform_lib_macros.h>

#include <utils/SystemClock.h>
#include <utils/Timers.h>
#include <signal.h>

void* GetDCIInitPtr();
void* GetCreateTimerPtr();
void* GetGpsTimeRxdPtr();
void* GetDCIDeInitPtr();
void* GetTimerOffsetMutexPtr();
void* GetReadPtr();
void* GetRetrievePtr();
void* GetSessionStartedPtr();
void* GetTimerIdPtr();
void* GetPlayBackTimerMutexPtr();
void* GetPlayBackTimerCondPtr();
void* GetTimerElapsedPtr();
typedef void*(*getsymptr)();


namespace slim {

  /**
    @brief Sensor PlayBack provider class.

    SensorPlayBackProvider class implements methods specific for Sensor on PlayBack
    connection.
    */

  /** Max number of GNSS SV measurement */
#define EP_DIAG_GNSS_MAX_MEAS                  128
#define EP_DIAG_ID_LENGTH   128
  typedef uint16_t epDiagGnssConstellationTypeMask;
  class SensorPlayBackProvider: public MultiplexingProvider {
    /* Diag PKT 0x1478 definition with all inner structs */
    typedef PACKED struct PACKED_POST
    {
      uint16_t w_GpsWeek; /* GPS week number at reference tick [weeks] */
      uint32_t q_GpsMsec; /* GPS msec at reference tick [msecs] */
      float f_ClkTimeBias; /* Clock bias [msecs] */
      float f_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */
      uint8_t u_GpsTimeSrc; /* Source of gps clock*/
    }
    mc_GpsTimeStructType_ForDmLog;

    typedef PACKED struct PACKED_POST
    { /*Keep this order this is how DM log defined*/
      uint8_t u_FourYear;
      uint16_t w_Days;
      uint32_t q_GloMsec;
      float f_ClkTimeBias; /* Clock bias [msecs] */
      float f_ClkTimeUncMs;/* 1 sided max time bias uncertainty [msecs] */
      uint8_t u_GloTimeSrc; /* Source of Glo clock*/
    }
    mc_GloTimeStructType_ForDmLog;

    /* BDS time struct for DM log */
    typedef PACKED struct PACKED_POST
    {
      uint16_t w_BdsWeek; /* BDS week number at reference tick [weeks] */
      uint32_t q_BdsMsec; /* BDS msec at reference tick [msecs] */
      float f_BdsTimeBias; /* Clock bias [msecs] */
      float f_BdsTimeUnc; /* 1 sided max time bias uncertainty [msecs] */
      uint8_t u_BdsClkSrc; /* Source of BDS clock*/
    }
    mc_BdsTimeStructType_ForDmLog;

    /* GAL time struct for DM log */
    typedef PACKED struct PACKED_POST
    {
      uint16_t w_GalWeek; /* GAL week number at reference tick [weeks] */
      uint32_t q_GalMsec; /* GAL msec at reference tick [msecs] */
      float f_GalTimeBias; /* Clock bias [msecs]. Constrait to >-0.5 and <0.5 */
      float f_GalTimeUnc; /* 1 sided max time bias uncertainty [msecs] */
      uint8_t u_GalClkSrc; /* Source of GAL clock*/
    }
    mc_GalTimeStructType_ForDmLog;

    typedef PACKED struct PACKED_POST
    {
      float f_ClkFreqBias; /* Clock frequency bias. [m/s] */
      float f_ClkFreqUnc; /* 1 sided max freq bias uncertainty [m/s] */
      uint8_t u_ClkFreqSrc; /* The source of gnss clock frequency*/
    }
    mc_GnssFreqStructType_ForDmLog;

    typedef PACKED struct PACKED_POST
    {
      uint8_t u_DeltaLeapSec; /* GPS time leap second delta to UTC time */
      uint8_t u_DeltaLeapSecUnc; /* Uncertainty for GPS leap second */
      uint8_t u_DeltaLeapSecSrc; /* Source of Leap second, see LS definition above */
      float f_GpsGloTBMs; /* GPS to Glonass Time Bias */
      float f_GpsGloTBUncMs; /* Uncertainty for GPS to Glonass Time Bias */
      float f_GpsBdsTBMs; /* GPS to BDS Time Bias */
      float f_GpsBdsTBUncMs; /* Uncertainty for GPS to BDS Time Bias */
      float f_BdsGloTBMs; /* BDS to GLO Time Bias */
      float f_BdsGloTBUncMs; /* Uncertainty for BDS to GLO Time Bias */
      float f_GpsGalTBMs; /* GPS to GAL Time Bias */
      float f_GpsGalTBUncMs; /* Uncertainty for GPS to GAL Time Bias */
      float f_GalGloTBMs; /* GAL to Glonass Time Bias */
      float f_GalGloTBUncMs; /* Uncertainty for GAL to Glonass Time Bias */
      float f_GalBdsTBMs; /* GAL to BDS Time Bias */
      float f_GalBdsTBUncMs; /* Uncertainty for GAL to BDS Time Bias */

    }
    mc_GnssTimeBiasStructType_ForDmLogV2;

    typedef PACKED struct PACKED_POST
    {
      uint8_t u_Version;
      uint16_t w_ValidFlag;
      uint32_t q_RefFCount; /* FCount value at reference tick */
      mc_GpsTimeStructType_ForDmLog z_GpsTimeInfo;
      mc_GloTimeStructType_ForDmLog z_GloTimeInfo;
      mc_BdsTimeStructType_ForDmLog z_BdsTimeInfo;
      mc_GalTimeStructType_ForDmLog z_GalTimeInfo;
      mc_GnssFreqStructType_ForDmLog z_GnssFreqInfo;
      mc_GnssTimeBiasStructType_ForDmLogV2 z_GnssBiasInfo;
      uint32_t q_SystemRtcMs; /* System RTC value, Note: In gps_Common.h this field is 64 bit */

      /* Not care others from here*/
      uint32_t othersNotCare;
    }
    mc_GnssClockDmLogV3;

    typedef PACKED struct PACKED_POST {
      uint16_t isSystemWeekValid : 1;
      uint16_t isSystemWeekMsecValid : 1;
      uint16_t isSystemClkTimeBiasValid : 1;
      uint16_t isSystemClkTimeUncMsValid : 1;
      uint16_t isRefFCountValid : 1;
      uint16_t isNumClockResetsValid : 1;
      uint16_t isIntraSystemTimeBias1AndUncValid : 1;
      uint16_t isIntraSystemTimeBias2AndUncValid : 1;
uint16_t : 8;
    }
    epDiagSystemTimeValidity;

    typedef PACKED struct PACKED_POST {
      uint8_t lossOfContinousLock : 1;
      uint8_t halfCycleAmbiguity : 1;
uint8_t : 6;
    }
    epDiagGnssMeasSvLLI;


    typedef uint32_t epGNSSInterSystemBiasImplStatusMask;
    typedef PACKED struct PACKED_POST {
      /* Validity mask */
      epGNSSInterSystemBiasImplStatusMask validMask;
      /** System-1 to System-2 Time Bias
        - Units: msec  */
      float timeBias;
      /** System-1 to System-2 Time Bias uncertainty
        - Units: msec  */
      float timeBiasUnc;
    }
    epDiagGnssInterSystemBiasStructType;

    static boolean timerElapsed;
    typedef PACKED struct PACKED_POST
    {
      uint16 len; /* Specifies the length, in bytes of the
                     entry, including this header. */

      uint16 code; /* Specifies the log code for the entry as
                      enumerated above. Note: This is
                      specified as word to guarantee size. */
      // removed AMSS specific code
      //qword ts;          The system timestamp for the log entry. The
      /*upper 48 bits represent elapsed time since
        6 Jan 1980 00:00:00 in 1.25 ms units. The
        low order 16 bits represent elapsed time
        since the last 1.25 ms tick in 1/32 chip
        units (this 16 bit counter wraps at the
        value 49152). */
      uint32 ts_lo; /* Time stamp */
      uint32 ts_hi;
    }
    log_hdr_type;

    typedef PACKED struct PACKED_POST {
      /** Used by Logging Module */
      log_hdr_type z_LogHeader;
      /** EP Message Version */
      uint8_t u_Version;
      /** Engine Plugin identifier */
      char epID[EP_DIAG_ID_LENGTH];
      /** Source Nname */
      char source[EP_DIAG_ID_LENGTH];
      /** Process identification */
      uint32_t u_Process_id;
      /** time at logging in milliseconds */
      uint64_t t_TimeTickMsec;

      uint8_t eventType;
      uint32_t reserved2;
    }epDiagLogGenericHeader;

    typedef PACKED struct PACKED_POST {
      /** Validity mask for below fields */
      epDiagSystemTimeValidity validityMask;
      /** Extended week number at reference tick.
Unit: Week.
Set to 65535 if week number is unknown.
For GPS:
Calculated from midnight, Jan. 6, 1980.
OTA decoded 10 bit GPS week is extended to map between:
[NV6264 to (NV6264 + 1023)].
NV6264: Minimum GPS week number configuration.
Default value of NV6264: 1738
For BDS:
Calculated from 00:00:00 on January 1, 2006 of Coordinated Universal Time (UTC).
For GAL:
Calculated from 00:00 UT on Sunday August 22, 1999 (midnight between August 21 and August 22).
*/
      uint16_t systemWeek;
      /** Time in to the current week at reference tick.
Unit: Millisecond. Range: 0 to 604799999.
Check for systemClkTimeUncMs before use */
      uint32_t systemMsec;
      /** System clock time bias (sub-millisecond)
Units: Millisecond
Note: System time (TOW Millisecond) = systemMsec - systemClkTimeBias.
Check for systemClkTimeUncMs before use. */
      float systemClkTimeBias;
      /** Single sided maximum time bias uncertainty
Units: Millisecond */
      float systemClkTimeUncMs;
      /** FCount (free running HW timer) value. Don't use for relative time purpose
        due to possible discontinuities.
Unit: Millisecond */
      uint32_t refFCount;
      /** Number of clock resets/discontinuities detected, affecting the local
       * hardware counter value. */
      uint32_t numClockResets;
    /**  For GPS: GPS L1 to L5 time bias (L5-L1)
         For BDS: BDS B1I to B2a time bias (B2a-B1I)
         For GAL: GAL E1 to E5a time bias (E5a-E1)
         For QZSS: QZSS L1 to L5 time bias (L5-L1)
Unit: millisecond  */
      float intraSystemTimeBias1;
      /**  Uncertainty of intraSystemTimeBias1
Unit: millisecond  */
      float intraSystemTimeBias1Unc;
    /**  For GPS: GPS L1 to L2C time bias (L2C-L1)
         For BDS: BDS B1I to B1C time bias (B1C-B1I)
         For GAL: GAL E1 to E5b time bias (E5b-E1)
         For QZSS: reserved
Unit: millisecond  */
    float intraSystemTimeBias2;
    /**  Uncertainty of intraSystemTimeBias2
Unit: millisecond  */
    float intraSystemTimeBias2Unc;
    }epDiagGnssSystemTimeStructType;

    typedef PACKED struct PACKED_POST {
      uint16_t isGloClkTimeBiasValid : 1;
      uint16_t isGloClkTimeUncMsValid : 1;
      uint16_t isRefFCountValid : 1;
      uint16_t isNumClockResetsValid : 1;
      uint16_t isGloFourYearValid: 1;
      uint16_t isGloDaysValid: 1;
      uint16_t isGloMsecValid: 1;
      uint16_t isIntraSystemTimeBias1AndUncValid : 1;
uint16_t : 8;
    } epDiagGloTimeValidity;
    typedef PACKED struct PACKED_POST {
      /** GLONASS day number in four years. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 65535
        Mandatory Field */
      uint16_t gloDays;
      /** GLONASS time of day in Millisecond. Refer to GLONASS ICD.
Units: Millisecond
Check for gloClkTimeUncMs before use */
      /** Validity mask for below fields */
      epDiagGloTimeValidity validityMask;
      uint32_t gloMsec;
      /** GLONASS clock time bias (sub-millisecond)
Units: Millisecond
Note: GLO time (TOD Millisecond) = gloMsec - gloClkTimeBias.
Check for gloClkTimeUncMs before use. */
      float gloClkTimeBias;
      /** Single sided maximum time bias uncertainty
Units: Millisecond */
      float gloClkTimeUncMs;
      /** FCount (free running HW timer) value. Don't use for relative time purpose
        due to possible discontinuities.
Unit: Millisecond */
      uint32_t refFCount;
      /** Number of clock resets/discontinuities detected, affecting the local
       * hardware counter value. */
      uint32_t numClockResets;
      /** GLONASS four year number from 1996. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 255
        Mandatory Field */
      uint8_t gloFourYear;
    /**  For GLO: GLO G1 to G2 time bias (G2-G1)
Unit: millisecond  */
      float intraSystemTimeBias1;
      /**  Uncertainty of intraSystemTimeBias1
Unit: millisecond  */
      float intraSystemTimeBias1Unc;
    }
    epDiagGnssGloTimeStructType;


    typedef PACKED struct PACKED_POST {
      uint32_t isHealthStatusValid : 1;
      uint32_t isSvInfoMaskValid : 1;
      uint32_t isLossOfLockValid : 1;
      uint32_t isFineSpeedValid : 1;
      uint32_t isFineSpeedUncValid : 1;
      uint32_t isCarrierPhaseValid : 1;
      uint32_t isCarrierPhaseUncValid : 1;
      uint32_t isCycleSlipCountValid : 1;
      uint32_t isPseudorangeValid : 1;
      uint32_t isPseudorangeUncValid : 1;
      uint32_t isDopplerValid : 1;
      uint32_t isDopplerUncValid : 1;
      uint32_t isCNoValid : 1;
      uint32_t isSVMSValid : 1;
      uint32_t isSVSubMSValid : 1;
      uint32_t isSVTimeUncValid : 1;
uint32_t : 16;
    }
    gnssDiagSVMeasurementValidity;

    typedef PACKED struct PACKED_POST {
      uint16_t isLeapSecValid : 1;
      uint16_t isClockDriftValid : 1;
      uint16_t isClockDriftStdDeviationValid : 1;
      uint16_t isGpsSystemTimeValid : 1;
      uint16_t isGalSystemTimeValid : 1;
      uint16_t isBdsSystemTimeValid : 1;
      uint16_t isQzssSystemTimeValid : 1;
      uint16_t isGloSystemTimeValid : 1;
uint16_t : 8;
    } epDiagGnssMeasurementReportValidity;

    typedef uint32_t epDiagGnssSignalTypeMask;
    typedef uint8_t epDiagGnssMeasSvSearchState;
    typedef uint64_t epDiagGNSSMeasurementStatusMask;
    typedef uint8_t epDiagGnssMeasSvInfoMask;
    typedef PACKED struct PACKED_POST {
      /** Specifies GNSS signal type. Mandatory field */
      epDiagGnssSignalTypeMask gnssSignal;
      /** Specifies GNSS system. Mandatory field */
      epDiagGnssConstellationTypeMask gnssSystem;
      /** Specifies GNSS SV ID.
        For GPS:             1 to 32
        For GLONASS:     65 to 96. When slot-number to SV ID mapping is unknown, set as 255.
        For SBAS:           120 to 151
        For QZSS-L1CA:  193 to 197
        For BDS:             201 to 237
        For GAL:              301 to 336
        Mandatory Field */
      uint16_t gnssSvId;
      /** GLONASS frequency number + 7.
        Valid only for a GLONASS system and
        is to be ignored for all other systems.
        - Range: 1 to 14 */
      uint8_t gloFrequency;
      /** Specifies satellite search state. Mandatory field */
      epDiagGnssMeasSvSearchState svSearchState;
      /**  Carrier to Noise ratio
        - Units: 0.1 dBHz
        Mandatory field */
      float cN0dbHz;
      /**  Bit-mask indicating SV measurement status.
        Valid bit-masks:
        If any MSB bit in 0xFFC0000000000000 DONT_USE is set, the measurement
        must not be used by the client.
        Mandatory field */
      epDiagGNSSMeasurementStatusMask measurementStatus;
      /**  Satellite time Millisecond.
        When measurementStatus bit-0 (sub-ms valid),
        1 (sub-bit known) & 2 (SV time known) are set, for:
        GPS, BDS, GAL, QZSS: Range of 0 through (604800000-1).
GLO: Range of 0 through (86400000-1).
Unit:Millisecond
Note: All SV times in the current measurement block are already
propagated to common reference time epoch.
Mandatory field */
      uint32_t svMs;
      /**  Satellite time sub-millisecond.
        When measurementStatus bit-0 "Sub-millisecond is valid" is set,
        Range of 0 through 0.99999 [Milliseconds]
        Total SV Time = svMs + svSubMs
        - Units: Millisecond
        Mandatory field */
      float svSubMs;
      /**  Satellite Time uncertainty
        - Units: Millisecond
        Mandatory field */
      float svTimeUncMs;
      /** Satellite Doppler
        - Units: meter per sec
        Mandatory field */
      float dopplerShift;
      /** Satellite Doppler uncertainty
        - Units: meter per sec */
      float dopplerShiftUnc;
      /** Validity mask for below fields */
      gnssDiagSVMeasurementValidity validityMask;
      /** Health status.
Range: 0 to 2;
0 = unhealthy, 1 = healthy, 2 = unknown */
      uint8_t healthStatus;
      /** Indicates whether almanac and ephemeris information is available. */
      epDiagGnssMeasSvInfoMask svInfoMask;
      /** Loss of signal lock indicator */
      epDiagGnssMeasSvLLI lossOfLock;
      /** Increments when a CSlip is detected */
      uint8_t cycleSlipCount;
      /** Carrier phase derived speed
Units: m/s */
      /** Estimated pseudorange uncertainty
Unit: Meters */
      float pseudorange_uncertinty;
      /** Carrier phase measurement [Carrier cycles]  */
      double carrierPhase;
      /** Carrier phase measurement Unc [Carrier cycles]  */
      float carrierPhaseUnc;
      /** Estimated pseudorange
Unit: Meters */
      double pseudorange;
    }
    epDiagGnssSVMeasurementStructType;
    typedef struct {
      uint32_t tv_sec;
      uint32_t tv_nsec;
    } timeSpec;
    typedef PACKED struct PACKED_POST {
      /** Boot time received from pps-ktimer
        Mandatory Field */
      timeSpec apTimeStamp;
      /** Time-stamp uncertainty in Milliseconds
        Mandatory Field */
      float apTimeStampUncertaintyMs;
    }epDiagGnssApTimeStampStructType;
    typedef enum {
      /** GNSS Measurement 1 Hz Report */
      EP_DIAG_MEASUREMENT_1HZ_REPORT = 1,
      /** GNSS Measurement 10 Hz Report */
      EP_DIAG_MEASUREMENT_NHZ_REPORT
    } epDiagGnssMeasurementReportType;


    typedef PACKED struct PACKED_POST {
      /** Receiver clock Drift
Units: meter per sec */
      float clockDrift;
      /** Receiver clock drift std deviation
Units: Meter per sec */
      float clockDriftStdDeviation;
      /** GPS system time information. Check validity before using */
      epDiagGnssSystemTimeStructType gpsSystemTime;
      /** GAL system time information. Check validity before using */
      epDiagGnssSystemTimeStructType galSystemTime;
      /** BDS system time information. Check validity before using */
      epDiagGnssSystemTimeStructType bdsSystemTime;
      /** QZSS system time information. Check validity before using */
      epDiagGnssSystemTimeStructType qzssSystemTime;
      /** GLO system time information. Check validity before using */
      epDiagGnssGloTimeStructType gloSystemTime;
      /** Validity mask */
      epDiagGnssMeasurementReportValidity validityMask;
      /** GPS time leap second delta to UTC time. Check validity before using
Units: sec */
      uint8_t leapSec;
      /** Number of Measurements in this report block.
        Mandatory field */
      uint8_t numMeas;
      /** Satellite measurement Information
        Mandatory field */
      epDiagGnssSVMeasurementStructType svMeasurement[EP_DIAG_GNSS_MAX_MEAS];
      /** Local time of last PPS pulse, associated with GNSS Measurement report
        Mandatory field */
      epDiagGnssApTimeStampStructType lastPpsLocaltimeStamp;
      /** GNSS Measurement Report Type */
      epDiagGnssMeasurementReportType gnssMeasReportType;
    }
    epDiagGnssMeasurementReportStruct;

    typedef PACKED struct PACKED_POST {
      epDiagLogGenericHeader header;
      uint8_t segmentNo;
      uint8_t maxSegmentNo;
      epDiagGnssMeasurementReportStruct msrInfo;
    }
    epDiagGnssMeasurementReport;

    typedef uint32_t epDiagGNSSMeasurementImplStatusMask;typedef PACKED struct PACKED_POST {
      /** Specifies GNSS system. Mandatory field */
      epDiagGnssConstellationTypeMask gnssSystem;
      /** Specifies GNSS SV ID. Mandatory field
        For GPS:       1 to 32
        For GLONASS: 65 to 96. When slot-number to SV ID mapping is unknown,set as 255.
        For SBAS:      120 to 151
        For QZSS-L1CA: 193 to 197
        For BDS:       201 to 237
        For GAL:       301 to 336 */
      uint16_t gnssSvId;
      /**  Bitmask indicating SV measurement status.
        Valid bitmasks:
        If any MSB bit in 0xFFC0000000000000 DONT_USE is set, the measurement
        must not be used by the client.
        Mandatory field */
      epDiagGNSSMeasurementImplStatusMask msrImplStatusMask;
      /**  Satellite Doppler Accelertion
        - Units: Hz/s */
      float dopplerAccel;
      /**  Estimate of multipath in measurement
        - Units: Meters */
      float multipathEstimate;
      /**  Rf loss reference to Antenna.
        - Units: dB, Scale: 0.1 */
      uint16_t rfLoss;
      /**  Age of the measurement. Positive value means measurement precedes ref time.
        - Units: msec */
      int16_t measLatency;
      /**  Satellite Azimuth
        - Units: radians */
      float svAzimuth;
      /**  Satellite Elevation
        - Units: radians */
      float svElevation;
      /**  Carrier phase derived speed
        - Units: m/s */
      float fineSpeed;
      /**  Carrier phase derived speed uncertainty
        - Units: m/s */
      float fineSpeedUnc;

    }
    epDiagGnssSVImplMeasurementStructType;

    typedef PACKED struct PACKED_POST {
      /**  Validity indicator for System RTC */
      uint8_t systemRtc_valid;
      /**  Platform system RTC value
        - Units: msec */
      uint64_t systemRtcMs;

    }epDiagGnssSystemTimeImplStruct;

    typedef enum {
      /** Source of the frequency is invalid */
      EP_DIAG_GNSS_FREQ_SOURCE_INVALID = 0,
      /** Source of the frequency is from external injection */
      EP_DIAG_GNSS_FREQ_SOURCE_EXTERNAL = 1,
      /** Source of the frequency is from Navigation engine */
      EP_DIAG_GNSS_FREQ_SOURCE_PE_CLK_REPORT = 2,
      /** Source of the frequency is unknown */
      EP_DIAG_GNSS_FREQ_SOURCE_UNKNOWN = 3
    } epDiagGnssSourceofFreqEnumType;
    typedef PACKED struct PACKED_POST {
      epDiagGnssMeasurementReport msrReport;
      epDiagGnssSVImplMeasurementStructType svImplMeasurement[EP_DIAG_GNSS_MAX_MEAS];
      /** GPS system time information. */
      epDiagGnssSystemTimeImplStruct gpsSystemTime;
      /** GAL system time information. */
      epDiagGnssSystemTimeImplStruct galSystemTime;
      /** BDS system time information. */
      epDiagGnssSystemTimeImplStruct bdsSystemTime;
      /** QZSS system time information. */
      epDiagGnssSystemTimeImplStruct qzssSystemTime;

      epDiagGnssSourceofFreqEnumType sourceOfFreq;
      epDiagGnssInterSystemBiasStructType gpsGloInterSystemBias;
      epDiagGnssInterSystemBiasStructType gpsBdsInterSystemBias;
      epDiagGnssInterSystemBiasStructType gpsGalInterSystemBias;
      epDiagGnssInterSystemBiasStructType bdsGloInterSystemBias;
      epDiagGnssInterSystemBiasStructType galGloInterSystemBias;
      epDiagGnssInterSystemBiasStructType galBdsInterSystemBias;
    }
    epDiagGnssImplMeasurementReport;


    /* Buffer will use the Sensor info in this struct format  */
    struct SensorData {
      unsigned int GpsMsec;
      unsigned int RTCMs;
      unsigned int SensorType;
      double SensorXaxis;
      double SensorYaxis;
      double SensorZaxis;
      SensorData(unsigned int GpsMsecKey) :
        GpsMsec(GpsMsecKey) {
        }

      bool operator ()(const SensorData& m) const {
        return GpsMsec <= m.GpsMsec;
      }

      bool operator <(const SensorData& m) const {
        return GpsMsec < m.GpsMsec;
      }
    };

    /* Singleton static object and its associates */
    static SensorPlayBackProvider *s_pzInstance;
    static Mutex s_zInstanceMutex;
    static const uint32_t s_qThreadId;
    static const slimAvailableServiceMaskT s_qSupportedServices;

    /* Other static objects */
    /* Thread ID for Sensor PlackBack File Read */
    static const uint32_t s_qFileReadThreadId;
    /* When Timer elapses it will signal this condition */
    static pthread_cond_t s_zPlayBackTimerCond;
    static pthread_mutex_t s_zPlayBackTimerMutex;
    /* GPS2QTimerOffset protector Mutex */
    static pthread_mutex_t s_zGPS2APSSTimerOffsetMutex;
    /* Timer for Senor batch interval */
    static timer_t s_ztimer_id;
    /* Senor batch interval */
    static long mTimer_Interval;
    /* Senor Look Ahead Buffer Size */
    static unsigned long Buffer_Size;
    /* Indicator for Session start */
    static uint8 s_uSession_Started;
    /* Indicator for Valid GPS Time */
    static boolean s_bValidGpsTimeRxd;
    /* Indicator for Valid GPS Time */
    static long long s_qGPS2APSSTimerOffset;
    /* Buffer to keep GpsToQTimer Offsets before Calibration */
    static std::vector<unsigned int> m_zGpsToQTimerVector;

    struct HalLibInfo
    {
      uint8_t sensorType;
      uint8_t sensorHalLibPathStrSet;
      /* As this is read using loc cfg routine, the buffer size
         need to conform to what is supported by loc cfg module. */
      char    sensorHalLibPath[LOC_MAX_PARAM_STRING];
    };

    static HalLibInfo m_zLibInfo;

    /* Main Thread responsible to send the Sensor Packets */
    slim_ThreadT m_zThread;
    /* Sensor PlayBack File read Thread */
    slim_ThreadT m_zFileReadThread;
    /*! @brief Mutex for atomic operations */
    mutable Mutex m_zLock;

    /* When Enable Sensor data request comes from clients it will signal this condition */
    pthread_cond_t m_zPlayBackStartCond;
    pthread_mutex_t m_zPlayBackStartMutex;

    pthread_mutex_t m_zPlayBackStartMutex2;
    /* From Main thread when the Buffer does not have data or Buffer is not full,
     * it will signal this condition */
    pthread_cond_t m_zPlayBackFileReadCond;
    pthread_mutex_t m_zPlayBackFileReadMutex;

    /* Only in case of Lookup fails from the buffer this is needed.
       In that case after finding the data from file, File Read thread will signal this condition */

    /* Single Look ahead Buffer for all Sensor data */
    std::vector<SensorData> m_zSensorDataVector;

    /* Temporary SensorDat structs which will be accepted by RouteIndication Function */
    slimSensorDataStructT m_zAccSensorData;
    slimSensorDataStructT m_zGyroSensorData;
    slimSensorDataStructT m_zMagSensorData;
    /*  DIAG_PROC ID */
    int mDci_proc;
    /* Diag Client ID  */
    int mClient_id;
    /*  Key GPS Time to search from File */
    unsigned int m_qKeyGpsMsec = 0;
    /*  Flags to keep the Senor is enabled or not */
    boolean m_bACCELService = FALSE;
    boolean m_bGYROService = FALSE;
    boolean m_bMAGService = FALSE;
    /*  Target Latency Configured  */
    unsigned int m_qTargetLatency = 0;
    /*  Inject Latency Configured  */
    unsigned int m_qInjectLatency = 0;
    /*  Senor Play Back File name  */
    char mqSensorPlayBackFile[LOC_MAX_PARAM_STRING];
    /*  Indicator to search from the Senor Play Back File */
    boolean m_bSearchFile;
    /*  Indicator to tell Time not found in the Senor Play Back File */
    boolean m_bNotFoundInFile = FALSE;
    FILE * fPTR;
    /*  Indicator to tell LowerLimit should be calculated again */
    boolean m_bSearchLowerLimit = TRUE;
    boolean mfileReadCondWait = FALSE;
    static unsigned int m_ProcessClkCntr;

    private:
    /*! Sensor PlayBack provider main thread function */
    static void threadMain(void *pData);

    /*! Sensor Plackback Record File Read thread function */
    static void FileReadthreadMain(void *pData);

    /**
      @brief Class constructor

      Creates and initializes new instance of the class.

*/
    SensorPlayBackProvider();

    /**
      @brief Class destructor

      Destroys current instance

*/
    ~SensorPlayBackProvider();

    /**
      @brief Class initializor

      initializes the Singleton object of SensorPlayBackProvider

*/
    bool initialize();

    /**
      @brief Main Thread function

      Responsible to send the Sensor Packets

*/
    void runEventLoop();

    /**
      @brief File Thread function

      Responsible to fill the Look ahead buffer

*/
    void runFileReadLoop();

    /**
      @brief Initiates time offset request.

      Function for making the time request. Successful response enable SLIM to
      calculate the offset between modem time and sensor time.

      @param[in] lTxnId Service transaction id.
      @return eSLIM_SUCCESS if time request is made successfully.
      */
    //virtual slimErrorEnumT getTimeUpdate(int32_t lTxnId);

    /**
      @brief Method for enabling or disabling sensor service.

      Generic function to start/stop a sensor based on provided sampling rate,
      batching rate, mounted state, and sensor information using Sensor1 Buffering
      API.

      @todo The sampling frequency is a mere suggestion to the sensor1 daemon.
      Sensor 1 will stream at the requested minimum sampling frequency requested
      by all AP clients combined. So we do see cases where buffering API does not
      help and we get single sensor data for every indication. In that case should
      SLIM AP do the batching?

      @param[in] uEnable       Flag that indicates if the service sPlayBackl be enabled
      or disabled.
      @param[in] wReportRateHz Requested reporting rate in Hz.
      @param[in] wSampleCount  Requested sample count in one batch.
      @param[in] eService      Service to control.

      @return eSLIM_SUCCESS is operation succeeded.
      */
    virtual slimErrorEnumT doUpdateSensorStatus(bool uEnable,
        uint16_t wReportRateHz, uint16_t wSampleCount,
        slimServiceEnumT eService);

    /**
      @brief Diag stream register function

Diag_Stream_Config: Set log & event masks

@param[in] client_id      client ID

@return eSLIM_SUCCESS is operation succeeded.
*/
    int Diag_Stream_Config(int mClient_id);

    /**
      @brief Diag call back function for Clock report

      Call back function to decode the 0x1478 Clock report

      @param[in] pRaw      Raw Byte array

*/
    static void processClkRptLog(unsigned char *pRaw);

    static void processGPS2APSSClkLog(unsigned char *pRaw);
    /**
      @brief Diag call back function for set of Logs

      Call back function to Process DCI log stream

      @param[in] ptr      Raw Byte array
      @param[in] len      Raw Byte array length

*/
    static void process_dci_log_stream(unsigned char *ptr, int len);

    /**
      @brief Diag call back function for set of events

      Call back function to Process DCI events stream

      @param[in] ptr      Raw Byte array
      @param[in] len      Raw Byte array length

*/
    static void process_dci_event_stream(unsigned char *ptr, int len);

    /**
      @brief Diag Initializer

      Diag Initializing function

      @param[out] client_id      DCI client ID

      @return eSLIM_SUCCESS is operation succeeded.
      */
    slimErrorEnumT Diag_DCI_Init(int *pmClient_id);

    /**
      @brief Diag De-Initializer

      Diag De-Initializing function

      @param[in] client_id      client ID

      @return eSLIM_SUCCESS is operation succeeded.
      */
    slimErrorEnumT Diag_DCI_DeInit(int mClient_id);

    /**
      @brief Time Converter

      Convert Time from CDMA Time format to UTC Ms
      Start from 1970/1/1 0:0:0

      @param[in] High and Low DWORDS

      @return Time in Ms.
      */
    static double CDMATimeToUTCMs(uint32 ts_hi, uint32 ts_low);

    /**
      @brief Time Converter

      Convert Time from CDMA Time format to Ms
      Start from 1970/1/1 0:0:0

      @param[in] High and Low DWORDS

      @return Time in Ms.
      */

    static double CDMATimeToMs(uint32 ts_hi, uint32 ts_low);

    /**
      @brief Notify Function Handler

      Gets called when there is a change in the channel connection

      @param[in] signal ID
      @param[in] signal Info

*/
    static void notify_handler(int signal, siginfo_t *info, void* /*unused*/
        );

    /**
      @brief Timer Creation function

      Responsible to create the Timer for Sensor sampling.

*/
    static void create_timer(const long& Timer_Interval);

    /**
      @brief Timer Creation function

      Responsible to create the Timer for Sensor sampling.

*/
    static void set_timer(const long& Timer_Interval);

    /**
      @brief Timer Thread main function

      Responsible to signal Main thread to prepare and send sensor data

*/
    static void timer_thread(union sigval arg);

    /**
      @brief Main Thread subroutine function

      Responsible to fill the SensorData and Invoke the routeIndication Function

*/
    void SendSensorData(slimSensorDataStructT& SensorData, uint8 sensorType);

    /**
      @brief Main Thread subroutine function

      Responsible to get sensor data from the Look ahead buffer

*/
    void RetrieveSensorData();

    uint32_t calcMsTimeDiffTick(uint64_t tTs1Ms, uint64_t tTs2Ms);

/**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.
*/
  virtual slimErrorEnumT getTimeUpdate(int32_t lTxnId);

public:
  typedef slimErrorEnumT (SensorPlayBackProvider::*fptr)(int*);
  fptr DCIInitPtr;
typedef void (*fptr1)(const long& Timer_Interval);
  fptr1 create_timerPtr;
 typedef uint8 *VPtr1;
 VPtr1 SessionStartedPtr;
typedef boolean *VPtr2;
 VPtr2 ValidGpsTimeRxdPtr;
 VPtr2 TimerElapsedPtr;

typedef slimErrorEnumT (SensorPlayBackProvider::*fptr2)(int mClient_id);
 fptr2 DCIDeInitPtr;

typedef pthread_mutex_t *VPtr3;
VPtr3 TimerOffsetMutexPtr;

typedef void (*fptr3)(void*);
fptr3 ReadDataPtr;

typedef void(SensorPlayBackProvider::*fptr4)(void);
fptr4 RetrieveData;

typedef timer_t* VPtr4;
VPtr4 TimerIdPtr;

VPtr3 PlaybackTimeMutexPtr;
typedef pthread_cond_t* VPtr5;
VPtr5 PlayBackTimerCondPtr;
friend void* ::GetDCIInitPtr();
friend void* ::GetCreateTimerPtr();
friend void* ::GetGpsTimeRxdPtr();
friend void* ::GetDCIDeInitPtr();
friend void* ::GetTimerOffsetMutexPtr();
friend void* ::GetReadPtr();
friend void* ::GetRetrievePtr();
friend void* ::GetSessionStartedPtr();
friend void* ::GetTimerIdPtr();
friend void* ::GetPlayBackTimerMutexPtr();
friend void* ::GetPlayBackTimerCondPtr();
friend void* ::GetTimerElapsedPtr();
/*!
     * @brief Returns provider instance.
     *
     * Method provides access to provider instance. If necessary, the instance
     * is created and initialized.
     *
     * @return Provider instance.
     * @retval 0 On error.
     */
    static slim_ServiceProviderInterfaceType *getProvider();
  };


}

#endif /* SLIM_SENSOR_PLAYBACK_H_INCLUDED */
