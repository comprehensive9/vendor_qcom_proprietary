/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   This file contains the data structure definitions for DGNSS Correction
   Data framework API

   Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef QDGNSSDATATYPE_H
#define QDGNSSDATATYPE_H

#include <stdint.h>

#define CODE_BIAS_COUNT_MAX       32
#define DGNSS_MEASUREMENT_MAX     64
#define DGNSS_SATELLITE_MAX       64

typedef enum {
    /** GPS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_GPS     = 0,
    /** SBAS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_SBAS    = 1,
    /** QZSS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_QZSS    = 2,
    /** GALILEO identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_GALILEO = 3,
    /** GLONASS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_GLONASS = 4,
    /** BDS identifier */
    DGNSS_CONSTELLATION_IDENTIFIER_BDS     = 5
} DGnssConstellationIdentifier;

typedef uint16_t  DGnssConstellationBitMask;
typedef enum {
    /** GPS constellation */
    DGNSS_CONSTELLATION_GPS_BIT     = (1<<DGNSS_CONSTELLATION_IDENTIFIER_GPS),
    /** SBAS constellation */
    DGNSS_CONSTELLATION_SBAS_BIT    = (1<<DGNSS_CONSTELLATION_IDENTIFIER_SBAS),
    /** QZSS constellation */
    DGNSS_CONSTELLATION_QZSS_BIT    = (1<<DGNSS_CONSTELLATION_IDENTIFIER_QZSS),
    /** GALILEO constellation */
    DGNSS_CONSTELLATION_GALILEO_BIT = (1<<DGNSS_CONSTELLATION_IDENTIFIER_GALILEO),
    /** GLONASS constellation */
    DGNSS_CONSTELLATION_GLONASS_BIT = (1<<DGNSS_CONSTELLATION_IDENTIFIER_GLONASS),
    /** BDS constellation */
    DGNSS_CONSTELLATION_BDS_BIT     = (1<<DGNSS_CONSTELLATION_IDENTIFIER_BDS)
} DGnssConstellationBits;

typedef enum {
    /** GPS L1 C/A signal */
    DGNSS_SIGNAL_GPS_L1CA  = 0,
    /** GPS L1 P signal */
    DGNSS_SIGNAL_GPS_L1P   = 1,
    /** GPS L1 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L1Z   = 2,
    /** GPS L1 codeless signal */
    DGNSS_SIGNAL_GPS_L1N   = 3,
    /** GPS L2 C/A signal */
    DGNSS_SIGNAL_GPS_L2CA  = 4,
    /** GPS L2 P signal */
    DGNSS_SIGNAL_GPS_L2P   = 5,
    /** GPS L2 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L2Z   = 6,
    /** GPS L2C (M) signal */
    DGNSS_SIGNAL_GPS_L2CM  = 7,
    /** GPS L2C (L) signal */
    DGNSS_SIGNAL_GPS_L2CL  = 8,
    /** GPS L2C (M+L) signal */
    DGNSS_SIGNAL_GPS_L2CML = 9,
    /** GPS L2 semi-codeless signal */
    DGNSS_SIGNAL_GPS_L2D   = 10,
    /** GPS L2 codeless signal */
    DGNSS_SIGNAL_GPS_L2N   = 11,
    /** GPS L5 I signal */
    DGNSS_SIGNAL_GPS_L5I   = 12,
    /** GPS L5 Q signal */
    DGNSS_SIGNAL_GPS_L5Q   = 13,
    /** GPS L5 I+Q signal */
    DGNSS_SIGNAL_GPS_L5IQ  = 14,
    /** GPS L1C-D signal */
    DGNSS_SIGNAL_GPS_L1CD  = 15,
    /** GPS L1C-P signal */
    DGNSS_SIGNAL_GPS_L1CP  = 16,
    /** GPS L1C-(D+P) signal */
    DGNSS_SIGNAL_GPS_L1CDP = 17
} DGnssGpsSignalIdentifier;

typedef uint32_t  DGnssGpsSignalBitMask;
typedef enum {
    /** GPS L1 C/A signal */
    DGNSS_SIGNAL_GPS_L1CA_BIT  = (1<<DGNSS_SIGNAL_GPS_L1CA),
    /** GPS L1 P signal */
    DGNSS_SIGNAL_GPS_L1P_BIT   = (1<<DGNSS_SIGNAL_GPS_L1P),
    /** GPS L1 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L1Z_BIT   = (1<<DGNSS_SIGNAL_GPS_L1Z),
    /** GPS L1 codeless signal */
    DGNSS_SIGNAL_GPS_L1N_BIT   = (1<<DGNSS_SIGNAL_GPS_L1N),
    /** GPS L2 C/A signal */
    DGNSS_SIGNAL_GPS_L2CA_BIT  = (1<<DGNSS_SIGNAL_GPS_L2CA),
    /** GPS L2 P signal */
    DGNSS_SIGNAL_GPS_L2P_BIT   = (1<<DGNSS_SIGNAL_GPS_L2P),
    /** GPS L2 Z tracking or similar signal */
    DGNSS_SIGNAL_GPS_L2Z_BIT   = (1<<DGNSS_SIGNAL_GPS_L2Z),
    /** GPS L2C (M) signal */
    DGNSS_SIGNAL_GPS_L2CM_BIT  = (1<<DGNSS_SIGNAL_GPS_L2CM),
    /** GPS L2C (L) signal */
    DGNSS_SIGNAL_GPS_L2CL_BIT  = (1<<DGNSS_SIGNAL_GPS_L2CL),
    /** GPS L2C (M+L) signal */
    DGNSS_SIGNAL_GPS_L2CML_BIT = (1<<DGNSS_SIGNAL_GPS_L2CML),
    /** GPS L2 semi-codeless signal */
    DGNSS_SIGNAL_GPS_L2D_BIT   = (1<<DGNSS_SIGNAL_GPS_L2D),
    /** GPS L2 codeless signal */
    DGNSS_SIGNAL_GPS_L2N_BIT   = (1<<DGNSS_SIGNAL_GPS_L2N),
    /** GPS L5 I signal */
    DGNSS_SIGNAL_GPS_L5I_BIT   = (1<<DGNSS_SIGNAL_GPS_L5I),
    /** GPS L5 Q signal */
    DGNSS_SIGNAL_GPS_L5Q_BIT   = (1<<DGNSS_SIGNAL_GPS_L5Q),
    /** GPS L5 I+Q signal */
    DGNSS_SIGNAL_GPS_L5IQ_BIT  = (1<<DGNSS_SIGNAL_GPS_L5IQ),
    /** GPS L1C-D signal */
    DGNSS_SIGNAL_GPS_L1CD_BIT  = (1<<DGNSS_SIGNAL_GPS_L1CD),
    /** GPS L1C-P signal */
    DGNSS_SIGNAL_GPS_L1CP_BIT  = (1<<DGNSS_SIGNAL_GPS_L1CP),
    /** GPS L1C-(D+P) signal */
    DGNSS_SIGNAL_GPS_L1CDP_BIT = (1<<DGNSS_SIGNAL_GPS_L1CDP)
} DGnssGpsSignalBits;

typedef enum {
    /** GLONASS G1 C/A signal */
    DGNSS_SIGNAL_GLONASS_G1CA  = 0,
    /** GLONASS G1 P signal */
    DGNSS_SIGNAL_GLONASS_G1P   = 1,
    /** GLONASS G2 C/A signal */
    DGNSS_SIGNAL_GLONASS_G2CA  = 2,
    /** GLONASS G2 P signal */
    DGNSS_SIGNAL_GLONASS_G2P   = 3,
    /** GLONASS G3-I signal */
    DGNSS_SIGNAL_GLONASS_G3I   = 4,
    /** GLONASS G3-Q signal */
    DGNSS_SIGNAL_GLONASS_G3Q   = 5,
    /** GLONASS G3-(I+Q) signal */
    DGNSS_SIGNAL_GLONASS_G3IQ  = 6
} DGnssGlonassSignalIdentifier;

typedef uint32_t DGnssGlonassSignalBitMask;
typedef enum {
    /** GLONASS G1 C/A signal */
    DGNSS_SIGNAL_GLONASS_G1CA_BIT  = (1<<DGNSS_SIGNAL_GLONASS_G1CA),
    /** GLONASS G1 P signal */
    DGNSS_SIGNAL_GLONASS_G1P_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G1P),
    /** GLONASS G2 C/A signal */
    DGNSS_SIGNAL_GLONASS_G2CA_BIT  = (1<<DGNSS_SIGNAL_GLONASS_G2CA),
    /** GLONASS G2 P signal */
    DGNSS_SIGNAL_GLONASS_G2P_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G2P),
    /** GLONASS G3-I signal */
    DGNSS_SIGNAL_GLONASS_G3I_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G3I),
    /** GLONASS G3-Q signal */
    DGNSS_SIGNAL_GLONASS_G3Q_BIT   = (1<<DGNSS_SIGNAL_GLONASS_G3Q),
    /** GLONASS G3-(I+Q) signal */
    DGNSS_SIGNAL_GLONASS_G3IQ_BIT  = (1<<DGNSS_SIGNAL_GLONASS_G3IQ)
} DGnssGlonassSignalBits;

typedef enum {
    /** Galileo E1 C signal */
    DGNSS_SIGNAL_GALILEO_E1C    = 0,
    /** Galileo E1 A signal */
    DGNSS_SIGNAL_GALILEO_E1A    = 1,
    /** Galileo E1 B (I/NAV OS/SC/SOL) signal */
    DGNSS_SIGNAL_GALILEO_E1B    = 2,
    /** Galileo E1 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1BC   = 3,
    /** Galileo E1 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1ABC  = 4,
    /** Galileo E6 C signal */
    DGNSS_SIGNAL_GALILEO_E6C    = 5,
    /** Galileo E6 A signal */
    DGNSS_SIGNAL_GALILEO_E6A    = 6,
    /** Galileo E6 B signal */
    DGNSS_SIGNAL_GALILEO_E6B    = 7,
    /** Galileo E6 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6BC   = 8,
    /** Galileo E6 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6ABC  = 9,
    /** Galileo E5 B-I signal */
    DGNSS_SIGNAL_GALILEO_E5BI   = 10,
    /** Galileo E5 B-Q signal */
    DGNSS_SIGNAL_GALILEO_E5BQ   = 11,
    /** Galileo E5 B-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5BIQ  = 12,
    /** Galileo E5 (A+B)-I signal */
    DGNSS_SIGNAL_GALILEO_E5ABI  = 13,
    /** Galileo E5 (A+B)-Q signal */
    DGNSS_SIGNAL_GALILEO_E5ABQ  = 14,
    /** Galileo E5 (A+B)-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5ABIQ = 15,
    /** Galileo E5 A-I signal */
    DGNSS_SIGNAL_GALILEO_E5AI   = 16,
    /** Galileo E5 A-Q signal */
    DGNSS_SIGNAL_GALILEO_E5AQ   = 17,
    /** Galileo E5 A-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5AIQ  = 18
} DGnssGalileoSignalIdentifier;

typedef uint32_t DGnssGalileoSignalBitMask;
typedef enum {
    /** Galileo E1 C signal */
    DGNSS_SIGNAL_GALILEO_E1C_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E1C),
    /** Galileo E1 A signal */
    DGNSS_SIGNAL_GALILEO_E1A_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E1A),
    /** Galileo E1 B (I/NAV OS/SC/SOL) signal */
    DGNSS_SIGNAL_GALILEO_E1B_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E1B),
    /** Galileo E1 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1BC_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E1BC),
    /** Galileo E1 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E1ABC_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E1ABC),
    /** Galileo E6 C signal */
    DGNSS_SIGNAL_GALILEO_E6C_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E6C),
    /** Galileo E6 A signal */
    DGNSS_SIGNAL_GALILEO_E6A_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E6A),
    /** Galileo E6 B signal */
    DGNSS_SIGNAL_GALILEO_E6B_BIT    = (1<<DGNSS_SIGNAL_GALILEO_E6B),
    /** Galileo E6 (B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6BC_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E6BC),
    /** Galileo E6 (A+B+C) signal */
    DGNSS_SIGNAL_GALILEO_E6ABC_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E6ABC),
    /** Galileo E5 B-I signal */
    DGNSS_SIGNAL_GALILEO_E5BI_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5BI),
    /** Galileo E5 B-Q signal */
    DGNSS_SIGNAL_GALILEO_E5BQ_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5BQ),
    /** Galileo E5 B-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5BIQ_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5BIQ),
    /** Galileo E5 (A+B)-I signal */
    DGNSS_SIGNAL_GALILEO_E5ABI_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5ABI),
    /** Galileo E5 (A+B)-Q signal */
    DGNSS_SIGNAL_GALILEO_E5ABQ_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5ABQ),
    /** Galileo E5 (A+B)-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5ABIQ_BIT = (1<<DGNSS_SIGNAL_GALILEO_E5ABIQ),
    /** Galileo E5 A-I signal */
    DGNSS_SIGNAL_GALILEO_E5AI_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5AI),
    /** Galileo E5 A-Q signal */
    DGNSS_SIGNAL_GALILEO_E5AQ_BIT   = (1<<DGNSS_SIGNAL_GALILEO_E5AQ),
    /** Galileo E5 A-(I+Q) signal */
    DGNSS_SIGNAL_GALILEO_E5AIQ_BIT  = (1<<DGNSS_SIGNAL_GALILEO_E5AIQ)
} DGnssGalileoSignalBits;

typedef enum {
    /** QZSS L1 C/A signal */
    DGNSS_SIGNAL_QZSS_L1CA  = 0,
    /** QZSS LEX-S signal */
    DGNSS_SIGNAL_QZSS_LEXS  = 1,
    /** QZSS LEX-L signal */
    DGNSS_SIGNAL_QZSS_LEXL  = 2,
    /** QZSS LEX-(S+L) signal */
    DGNSS_SIGNAL_QZSS_LEXSL = 3,
    /** QZSS L2C (M) signal */
    DGNSS_SIGNAL_QZSS_L2CM  = 4,
    /** QZSS L2C (L) signal */
    DGNSS_SIGNAL_QZSS_L2CL  = 5,
    /** QZSS L2C (M+L) signal */
    DGNSS_SIGNAL_QZSS_L2CML = 6,
    /** QZSS L5 I signal */
    DGNSS_SIGNAL_QZSS_L5I   = 7,
    /** QZSS L5 Q signal */
    DGNSS_SIGNAL_QZSS_L5Q   = 8,
    /** QZSS L5 I+Q signal */
    DGNSS_SIGNAL_QZSS_L5IQ  = 9,
    /** QZSS L1C-D signal */
    DGNSS_SIGNAL_QZSS_L1CD  = 10,
    /** QZSS L1C-P signal */
    DGNSS_SIGNAL_QZSS_L1CP  = 11,
    /** QZSS L1C-(D+P) signal */
    DGNSS_SIGNAL_QZSS_L1CDP = 12,
    /** QZSS L1S/L1-SAIF signal */
    DGNSS_SIGNAL_QZSS_L1S   = 13
} DGnssQzssSignalIdentifier;

typedef uint32_t DGnssQzssSignalBitMask;
typedef enum {
    /** QZSS L1 C/A signal */
    DGNSS_SIGNAL_QZSS_L1CA_BIT  = (1<<DGNSS_SIGNAL_QZSS_L1CA),
    /** QZSS LEX-S signal */
    DGNSS_SIGNAL_QZSS_LEXS_BIT  = (1<<DGNSS_SIGNAL_QZSS_LEXS),
    /** QZSS LEX-L signal */
    DGNSS_SIGNAL_QZSS_LEXL_BIT  = (1<<DGNSS_SIGNAL_QZSS_LEXL),
    /** QZSS LEX-(S+L) signal */
    DGNSS_SIGNAL_QZSS_LEXSL_BIT = (1<<DGNSS_SIGNAL_QZSS_LEXSL),
    /** QZSS L2C (M) signal */
    DGNSS_SIGNAL_QZSS_L2CM_BIT  = (1<<DGNSS_SIGNAL_QZSS_L2CM),
    /** QZSS L2C (L) signal */
    DGNSS_SIGNAL_QZSS_L2CL_BIT  = (1<<DGNSS_SIGNAL_QZSS_L2CL),
    /** QZSS L2C (M+L) signal */
    DGNSS_SIGNAL_QZSS_L2CML_BIT = (1<<DGNSS_SIGNAL_QZSS_L2CML),
    /** QZSS L5 I signal */
    DGNSS_SIGNAL_QZSS_L5I_BIT   = (1<<DGNSS_SIGNAL_QZSS_L5I),
    /** QZSS L5 Q signal */
    DGNSS_SIGNAL_QZSS_L5Q_BIT   = (1<<DGNSS_SIGNAL_QZSS_L5Q),
    /** QZSS L5 I+Q signal */
    DGNSS_SIGNAL_QZSS_L5IQ_BIT  = (1<<DGNSS_SIGNAL_QZSS_L5IQ),
    /** QZSS L1C-D signal */
    DGNSS_SIGNAL_QZSS_L1CD_BIT  = (1<<DGNSS_SIGNAL_QZSS_L1CD),
    /** QZSS L1C-P signal */
    DGNSS_SIGNAL_QZSS_L1CP_BIT  = (1<<DGNSS_SIGNAL_QZSS_L1CP),
    /** QZSS L1C-(D+P) signal */
    DGNSS_SIGNAL_QZSS_L1CDP_BIT = (1<<DGNSS_SIGNAL_QZSS_L1CDP),
    /** QZSS L1S/L1-SAIF signal */
    DGNSS_SIGNAL_QZSS_L1S_BIT   = (1<<DGNSS_SIGNAL_QZSS_L1S)
} DGnssQzssSignalBits;

typedef enum {
    /** BDS B1-2 I signal */
    DGNSS_SIGNAL_BDS_B12I   = 0,
    /** BDS B1-2 Q signal */
    DGNSS_SIGNAL_BDS_B12Q   = 1,
    /** BDS B1-2 (I+Q) signal */
    DGNSS_SIGNAL_BDS_B12IQ  = 2,
    /** BDS B3-I signal */
    DGNSS_SIGNAL_BDS_B3I    = 3,
    /** BDS B3-Q signal */
    DGNSS_SIGNAL_BDS_B3Q    = 4,
    /** BDS B3-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B3IQ   = 5,
    /** BDS B3A signal */
    DGNSS_SIGNAL_BDS_B3A    = 6,
    /** BDS B2-I signal */
    DGNSS_SIGNAL_BDS_B2I    = 7,
    /** BDS B2-Q signal */
    DGNSS_SIGNAL_BDS_B2Q    = 8,
    /** BDS B2-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B2IQ   = 9,
    /** BDS B2a Data signal */
    DGNSS_SIGNAL_BDS_B2AD   = 10,
    /** BDS B2a Pilot signal */
    DGNSS_SIGNAL_BDS_B2AP   = 11,
    /** BDS B2a (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2ADP  = 12,
    /** BDS B1C Data signal */
    DGNSS_SIGNAL_BDS_B1CD   = 13,
    /** BDS B1C Pilot signal */
    DGNSS_SIGNAL_BDS_B1CP   = 14,
    /** BDS B1C (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B1CDP  = 15,
    /** BDS B1A signal */
    DGNSS_SIGNAL_BDS_B1A    = 16,
    /** BDS B1 Codeless signal */
    DGNSS_SIGNAL_BDS_B1N    = 17,
    /** BDS B2b Data signal */
    DGNSS_SIGNAL_BDS_B2BD   = 18,
    /** BDS B2b Pilot signal */
    DGNSS_SIGNAL_BDS_B2BP   = 19,
    /** BDS B2b (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2BDP  = 20,
    /** BDS B2(B2a+B2b)-D signal */
    DGNSS_SIGNAL_BDS_B2D    = 21,
    /** BDS B2(B2a+B2b)-P signal */
    DGNSS_SIGNAL_BDS_B2P    = 22,
    /** BDS B2(B2a+B2b)-(D+P) signal */
    DGNSS_SIGNAL_BDS_B2DP   = 23
} DGnssBdsSignalIdentifier;

typedef uint32_t DGnssBdsSignalBitMask;
typedef enum {
    /** BDS B1-2 I signal */
    DGNSS_SIGNAL_BDS_B12I_BIT   = (1<<DGNSS_SIGNAL_BDS_B12I),
    /** BDS B1-2 Q signal */
    DGNSS_SIGNAL_BDS_B12Q_BIT   = (1<<DGNSS_SIGNAL_BDS_B12Q),
    /** BDS B1-2 (I+Q) signal */
    DGNSS_SIGNAL_BDS_B12IQ_BIT  = (1<<DGNSS_SIGNAL_BDS_B12IQ),
    /** BDS B3-I signal */
    DGNSS_SIGNAL_BDS_B3I_BIT    = (1<<DGNSS_SIGNAL_BDS_B3I),
    /** BDS B3-Q signal */
    DGNSS_SIGNAL_BDS_B3Q_BIT    = (1<<DGNSS_SIGNAL_BDS_B3Q),
    /** BDS B3-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B3IQ_BIT   = (1<<DGNSS_SIGNAL_BDS_B3IQ),
    /** BDS B3A signal */
    DGNSS_SIGNAL_BDS_B3A_BIT    = (1<<DGNSS_SIGNAL_BDS_B3A),
    /** BDS B2-I signal */
    DGNSS_SIGNAL_BDS_B2I_BIT    = (1<<DGNSS_SIGNAL_BDS_B2I),
    /** BDS B2-Q signal */
    DGNSS_SIGNAL_BDS_B2Q_BIT    = (1<<DGNSS_SIGNAL_BDS_B2Q),
    /** BDS B2-(I+Q) signal */
    DGNSS_SIGNAL_BDS_B2IQ_BIT   = (1<<DGNSS_SIGNAL_BDS_B2IQ),
    /** BDS B2a Data signal */
    DGNSS_SIGNAL_BDS_B2AD_BIT   = (1<<DGNSS_SIGNAL_BDS_B2AD),
    /** BDS B2a Pilot signal */
    DGNSS_SIGNAL_BDS_B2AP_BIT   = (1<<DGNSS_SIGNAL_BDS_B2AP),
    /** BDS B2a (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2ADP_BIT  = (1<<DGNSS_SIGNAL_BDS_B2ADP),
    /** BDS B1C Data signal */
    DGNSS_SIGNAL_BDS_B1CD_BIT   = (1<<DGNSS_SIGNAL_BDS_B1CD),
    /** BDS B1C Pilot signal */
    DGNSS_SIGNAL_BDS_B1CP_BIT   = (1<<DGNSS_SIGNAL_BDS_B1CP),
    /** BDS B1C (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B1CDP_BIT  = (1<<DGNSS_SIGNAL_BDS_B1CDP),
    /** BDS B1A signal */
    DGNSS_SIGNAL_BDS_B1A_BIT    = (1<<DGNSS_SIGNAL_BDS_B1A),
    /** BDS B1 Codeless signal */
    DGNSS_SIGNAL_BDS_B1N_BIT    = (1<<DGNSS_SIGNAL_BDS_B1N),
    /** BDS B2b Data signal */
    DGNSS_SIGNAL_BDS_B2BD_BIT   = (1<<DGNSS_SIGNAL_BDS_B2BD),
    /** BDS B2b Pilot signal */
    DGNSS_SIGNAL_BDS_B2BP_BIT   = (1<<DGNSS_SIGNAL_BDS_B2BP),
    /** BDS B2b (Data+Pilot) signal */
    DGNSS_SIGNAL_BDS_B2BDP_BIT  = (1<<DGNSS_SIGNAL_BDS_B2BDP),
    /** BDS B2(B2a+B2b)-D signal */
    DGNSS_SIGNAL_BDS_B2D_BIT    = (1<<DGNSS_SIGNAL_BDS_B2D),
    /** BDS B2(B2a+B2b)-P signal */
    DGNSS_SIGNAL_BDS_B2P_BIT    = (1<<DGNSS_SIGNAL_BDS_B2P),
    /** BDS B2(B2a+B2b)-(D+P) signal */
    DGNSS_SIGNAL_BDS_B2DP_BIT   = (1<<DGNSS_SIGNAL_BDS_B2DP)
} DGnssBdsSignalBits;

typedef enum {
    /** SBAS L1 C/A signal */
    DGNSS_SIGNAL_SBAS_L1CA  = 0,
    /** SBAS L5 I signal */
    DGNSS_SIGNAL_SBAS_L5I   = 1,
    /** SBAS L5 Q signal */
    DGNSS_SIGNAL_SBAS_L5Q   = 2,
    /** SBAS L5 I+Q signal */
    DGNSS_SIGNAL_SBAS_L5IQ  = 3
} DGnssSbasSignalIdentifier;

typedef uint32_t  DGnssSbasSignalBitMask;
typedef enum {
    /** SBAS L1 C/A signal */
    DGNSS_SIGNAL_SBAS_L1CA_BIT  = (1<<DGNSS_SIGNAL_SBAS_L1CA),
    /** SBAS L5 I signal */
    DGNSS_SIGNAL_SBAS_L5I_BIT   = (1<<DGNSS_SIGNAL_SBAS_L5I),
    /** SBAS L5 Q signal */
    DGNSS_SIGNAL_SBAS_L5Q_BIT   = (1<<DGNSS_SIGNAL_SBAS_L5Q),
    /** SBAS L5 I+Q signal */
    DGNSS_SIGNAL_SBAS_L5IQ_BIT  = (1<<DGNSS_SIGNAL_SBAS_L5IQ)
} DGnssSbasSignalBits;

/** Reference Station */
typedef struct {
    /** 0 - Real, physical reference station
     *  1 - Non-physical or computed reference station */
    uint8_t  stationIndicator          : 1;
    /** 11 - all raw observations are measured at the same instant
     *  10 - all raw observations may be measured at different instants
     *  00 - unknown / not available */
    uint8_t  oscillatorIndicator       : 2;
    /** 00 - correction status unspecified or not available
     *  01 - phase observations are corrected for quarter cycle bias
     *  10 - phase observation are not corrected for qualter cycle bias
     *  11 - reserved */
    uint8_t  quarterCycleIndicator     : 2;
    /** antenna height available
     *  0 - not availiable
     *  1 - availiable */
    uint8_t  heightAvailability        : 1;
    /** antenna reference point uncertainty avilable
     *  0 - not availiable
     *  1 - availiable */
    uint8_t  referencePointUncertainty : 1;
} StationStatusIndicatorMask;

typedef struct {
    /** reference station ID, valid range: 0-4095 */
    uint16_t stationID;
    /** reference station status indicators */
    StationStatusIndicatorMask  statusIndicator;
    /** linked reference station ID, valid range: 0-4095 */
    uint16_t linkedStationID;
    /** ITRF realization year, valid range: 0-63 */
    uint8_t  ITRFYear;
    /** DGnss constellation bit mask */
    DGnssConstellationBitMask constellationBitMask;
    /** antenna reference point: ECEF-X, valid range: -13743895.3472m ~ 13743895.3472m */
    double   ecefX;
    /** antenna reference point: ECEF-Y, valid range: -13743895.3472m ~ 13743895.3472m */
    double   ecefY;
    /** antenna reference point: ECEF-Z, valid range: -13743895.3472m ~ 13743895.3472m */
    double   ecefZ;
    /** antenna height, valid range: 0-6.5535m */
    float    antennaHeight;
    /** antenna reference point: uncertainty-X */
    float    uncertaintyX;
    /** antenna reference point: uncertainty-Y */
    float    uncertaintyY;
    /** antenna reference point: uncertainty-Z */
    float    uncertaintyZ;
    /** antenna reference point: uncertainty confidence-X, valid range: 0-100 */
    uint8_t  uncertaintyConfidenceX;
    /** antenna reference point: uncertainty confidence-Y, valid range: 0-100 */
    uint8_t  uncertaintyConfidenceY;
    /** antenna reference point: uncertainty confidence-Z, valid range: 0-100 */
    uint8_t  uncertaintyConfidenceZ;
} ReferenceStation;

/** Gnss Time */
union GnssEpochTime {
    /** gps time of week, valid range: 0 - 604799999 ms */
    uint32_t  timeOfWeek;
    struct {
        /** glonass time of day, valid range: 0 - 86400999 ms */
        uint32_t timeOfDay : 27;
        /** glonass day of week, valid range: 0 - 6, set to 7 if unknown or not available */
        uint32_t dayOfWeek : 3;
    } glonassEpochTime;
};

/** GLONASS Bias Information */
/** GLONASS code phase bias indicator and FDMA signal mask */
typedef uint8_t BiasIndicatorFDMASignalMask;
typedef enum {
    /** GLONASS pseudorange and phaserange measurements are aligned to the same epoch */
    GLONASS_PSEUDORANGE_PHASERANGE_SAME_EPOCH = (1<<0),
    /** GLONASS L2-P code phase bias is valid */
    GLONASS_CODE_PHASE_BIAS_L2P_VALID         = (1<<1),
    /** GLONASS L2-C/A code phase bias is valid */
    GLONASS_CODE_PHASE_BIAS_L2CA_VALID        = (1<<2),
    /** GLONASS L1-P code phase is valid */
    GLONASS_CODE_PHASE_BIAS_L1P_VALID         = (1<<3),
    /** GLONASS L1-C/A code phase bias is valid */
    GLONASS_CODE_PHASE_BIAS_L1CA_VALID        = (1<<4)
} BiasIndicatorFdmaSignalBits;

typedef struct {
    /** reference station ID, valid range: 0-4095 */
    uint16_t                      stationID;
    /** GLONASS code phase bias indicator and FDMA signals mask */
    BiasIndicatorFDMASignalMask   biasIndicatorSignalMask;
    /** GLONASS L1 C/A code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL1CA;
    /** GLONASS L1 P code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL1P;
    /** GLONASS L2 C/A code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL2CA;
    /** GLONASS L2 P code phase bias, valid range: -655.34 ~ 655.34 m */
    float                         biasL2P;
} GlonassBias;

/** State Space Representation */
typedef struct {
    /** 0 - last set of corrections available for this type and epoch time
     *  1 - more corrections available for this type and epoch time*/
    uint8_t multipleMessageIndicator : 1;
    /** 0 - SSR provider information invalid/not available
     *  1 - SSR provider information valid */
    uint8_t providerInfoValidity     : 1;
    /** orbit corrections refer to satellite reference datum
     *  0 - ITRF      1 - Regional */
    uint8_t datumType                : 1;
    /** reserved */
    uint8_t                          : 1;
    /** Issue of Data SSR */
    uint8_t issueOfData              : 4;
} SsrStatusFlags;

typedef struct {
    /** DGnss constellation identifier */
    DGnssConstellationIdentifier   constellationIdentifier;
    /** GNSS epoch time */
    GnssEpochTime                  epochTime;
    /** SSR status flags */
    SsrStatusFlags                 statusFlags;
    /** SSR update interval, valid range: 0-15 */
    uint8_t                        updateInterval;
    /** SSR provider ID, valid range: 0-65535 */
    uint16_t                       providerID;
    /** SSR solution ID, valid range: 0-15 */
    uint8_t                        solutionID;
    /** number of satellites, valid range: 1-255 */
    uint8_t                        numberOfSatellites;
} SsrHeader;

/** Satellite specific part of SSR orbit corrections */
typedef struct {
    /** GPS SV ID, range: 1-32
     *  GLONASS slot number, range: 0-63
     *  0:unknown, 1-24:GLONASS slot number, >32: reserved for SBAS
     */
    uint8_t   svId;
    /** IODE */
    uint16_t  IODE;
    /** Delta radial, valid range: -209.7151m ~ 209.7151m */
    float     deltaRadial;
    /** Delta along-track, valid range: -209.7148 ~ 209.7148m */
    float     deltaAlongTrack;
    /** Delta cross-track, valid range: -209.7148m ~ 209.7148m */
    float     deltaCrossTrack;
    /** Dot delta radial, valid range: -1.048575 ~ 1.048575 m/s */
    float     dotDeltaRadial;
    /** Dot delta along-track, valid range: -1.048575 ~ 1.048575 m/s */
    float     dotDeltaAlongTrack;
    /** Dot delta cross-track, valid range: -1.048575 ~ 1.048575 m/s */
    float     dotDeltaCrossTrack;
} SsrSatelliteOrbitCorrections;

/** SSR orbit corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteOrbitCorrections satOrbitCorrections[DGNSS_SATELLITE_MAX];
} SsrOrbitCorrections;

/** Satellite specific part of SSR clock corrections */
typedef struct {
    /** GPS SV ID, range: 1-32
     *  GLONASS slot number, range: 0-63
     *  0:unknown, 1-24:GLONASS slot number, >32: reserved for SBAS
     */
    uint8_t svId;
    /** Delta clock C0, valid range: -209.7151 ~ 209.7151 m */
    float   deltaClockC0;
    /** Delta clock C1, valid range: -1.048575 ~ 1.048575 m/s */
    float   deltaClockC1;
    /** Delta clock C2, valid range: -1.34217726 ~ 1.34217726 m/s^2 */
    double  deltaClockC2;
} SsrSatelliteClockCorrections;

/** SSR clock corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteClockCorrections  satClockCorrections[DGNSS_SATELLITE_MAX];
} SsrClockCorrections;

/** SSR combined orbit and clock corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteOrbitCorrections  satOrbitCorrections[DGNSS_SATELLITE_MAX];
    SsrSatelliteClockCorrections  satClockCorrections[DGNSS_SATELLITE_MAX];
} SsrOrbitAndClockCorrections;

/** Satellite specific part of SSR high-rate clock corrections */
typedef struct {
    /** GPS SV ID, range: 1-32
     *  GLONASS slot number, range: 0-63
     *  0:unknown, 1-24:GLONASS slot number, >32: reserved for SBAS
     */
    uint8_t  svId;
    /** high rate clock correction, valid range: -209.7151 ~ 209.7151 m */
    float    highRateClockCorrection;
} SsrSatelliteHighRateClockCorrections;

/** SSR high-rate clock corrections */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteHighRateClockCorrections  satHighRateClockCorrections[DGNSS_SATELLITE_MAX];
} SsrHighRateClockCorrections;

/** specific code bias */
typedef struct {
    /** GNSS signal identifier */
    uint8_t   signalIdentifier;
    /** Code bias, valid range: -81.91 ~ 81.91 m */
    float     codeBias;
} CodeBias;

/* satellite specific part of SSR code bias */
typedef struct {
    /** GPS SV ID, range: 1-32
     *  GLONASS slot number, range: 0-63
     *  0:unknown, 1-24:GLONASS slot number, >32: reserved for SBAS
     */
    uint8_t       svId;
    /** number of  code biases processed, valid range: 1-255 */
    uint8_t       numberOfCodeBias;
    CodeBias      codeBias[CODE_BIAS_COUNT_MAX];
} SsrSatelliteCodeBias;

/** SSR code bias */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteCodeBias  satCodeBias[DGNSS_SATELLITE_MAX];
} SsrCodeBias;

/** Satellite specific SSR user range accuracy */
typedef struct {
    /** GPS SV ID, range: 1-32
     *  GLONASS slot number, range: 0-63
     *  0:unknown, 1-24:GLONASS slot number, >32: reserved for SBAS
     */
    uint8_t  svId;
    /** SSR user range accuracy, valid range: 0-63 */
    uint8_t  userRangeAccuracy;
} SsrSatelliteUserRangeAccuracy;

/** SSR user range accuracy */
typedef struct {
    SsrHeader  ssrHeader;
    SsrSatelliteUserRangeAccuracy  satUserRangeAccuracy[DGNSS_SATELLITE_MAX];
} SsrUserRangeAccuracy;

/*RTK Observations*/
typedef struct {
    /** 0 - no further observations are expected for this reference station ID and epoch time
     *  1 - more observations are expected for this reference station ID and epoch time */
    uint16_t  synchronousFlag                  : 1;
    /** 0 - Divergence-Free smoothing not used
     *  1 - Divergence-Free smoothing used */
    uint16_t  smoothingIndicator               : 1;
    /** 000 - no smoothing is used
     *  001 - less than 30 seconds
     *  010 - 30-60 seconds
     *  011 - 1 to 2 minutes
     *  100 - 2 to 4 minutes
     *  101 - 4 to 8 minutes
     *  110 - more than 8 minutes
     *  111 - unlimited smoothing interval */
    uint16_t  smoothingInterval                : 3;
    /** 00 - clock steering not applied
     *  01 - clock steering has been applied
     *  10 - unknown clock steering status
     *  11 - reserved */
    uint16_t  clockSteeringIndicator           : 2;
    /** 00 - internal clock is used
     *  01 - external clock is used, status is locked
     *  10 - external clock is used, status is not locked
     *  11 - unknown clock is used */
    uint16_t  externalClockIndicator           : 2;
    /** reserved */
    uint16_t                                   : 7;
} DGnssObservationStatusFlags;

typedef uint8_t  DGnssSignalStatusFlags;
typedef enum {
    /** bit0: 1: fully reconstructed measurements available including integer-milliseconds
     *        0: modulo 1-ms measurements */
    MEASUREMENT_COMPLETE         = (1<<0),
    /** bit1: 1: pseudorange measurements valid
     *        0: pseudorange measurements invalid */
    PSEUDORANGE_VALID            = (1<<1),
    /** bit2: 1: Carrier phase measurements valid
              0: Carrier phase messurements invalid */
    CARRIER_PHASE_VALID          = (1<<2),
    /** bit3: 1: Phase range rate measurements valid
              0: Phase ragne rate measurements invalid */
    PHASE_RANGE_RATE_VALID       = (1<<3),
    /** Half-cycle ambiguity indicator, valid only if carrier phase measurements are set to valid
     *  bit5 bit4: Half cycle ambiguity
     *  00 - No Half-cycle ambiguity
     *  01 - Half-cycle ambiguity
     *  1x - Unknown */
    HALF_CYCLE_AMBIGUITY_VALID   = (1<<4),
    /** Half cycle ambiguity unknown */
    HALF_CYCLE_AMBIGUITY_UNKNOWN = (1<<5)
} DGnssSignalStatusBits;

typedef enum {
    /** nominal value of frequency: L1 1598.0625 MHz, L2 1242.9375 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_7  = -7,
    /** nominal value of frequency: L1 1598.6250 MHz, L2 1243.3750 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_6  = -6,
    /** nominal value of frequency: L1 1599.1875 MHz, L2 1243.8125 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_5  = -5,
    /** nominal value of frequency: L1 1599.7500 MHz, L2 1244.2500 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_4  = -4,
    /** nominal value of frequency: L1 1600.3125 MHz, L2 1244.6875 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_3  = -3,
    /** nominal value of frequency: L1 1600.8750 MHz, L2 1245.1250 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_2  = -2,
    /** nominal value of frequency: L1 1601.4375 MHz, L2 1245.5625 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_1  = -1,
    /** nominal value of frequency: L1 1602.0 MHz, L2 1246.0 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_0        = 0,
    /** nominal value of frequency: L1 1602.5625 MHz, L2 1246.4375 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_1        = 1,
    /** nominal value of frequency: L1 1603.125 MHz, L2 1246.875 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_2        = 2,
    /** nominal value of frequency: L1 1603.6875 MHz, L2 1247.3125 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_3        = 3,
    /** nominal value of frequency: L1 1604.25 MHz, L2 1247.75 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_4        = 4,
    /** nominal value of frequency: L1 1604.8125 MHz, L2 1248.1875 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_5        = 5,
    /** nominal value of frequency: L1 1605.375 MHz, L2 1248.625 MHz*/
    DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_6        = 6,
    /** reserved */
    DGNSS_GLONASS_FREQUENCY_CHANNEL_RESERVED        = 14,
    /** frequency channel number is not known or not available */
    DGNSS_GLONASS_FREQUENCY_CHANNEL_UNKNOWN         = 15
} ExtendedSatelliteInformation;

typedef struct {
    /** GPS SV ID, range: 1-64, 1-63: GPS PRN, 64:reserved
     *  GLONASS SV slot number, range: 1-64, 1-24: glonass slot number, 25-64:reserved
     *  GALILEO SV ID, range: 1-64, 1-52: GALILEO PRN, 53-64: reserved
     *  SBAS SV ID, range:120-158
     *  QZSS SV ID, range:193-202
     *  BDS SV ID, range:1-64, 1-37: BDS PRN, 38-64:reserved */
    uint8_t svId;
    /** Extended satellite information */
    ExtendedSatelliteInformation extendedSatelliteInformation;
    /** GNSS signal identifier */
    union {
        DGnssGpsSignalIdentifier     gpsSignalIdentifier;
        DGnssGlonassSignalIdentifier glonassSignalIdentifier;
        DGnssGalileoSignalIdentifier galileoSignalIdentifier;
        DGnssQzssSignalIdentifier    qzssSignalIdentifier;
        DGnssBdsSignalIdentifier     bdsSignalIdentifier;
        DGnssSbasSignalIdentifier    sbasSignalIdentifier;
    } gnssSignalIdentifier;
    /** GNSS signal specific observation status */
    DGnssSignalStatusFlags signalObservationStatus;
    /** valid range: 0-76447076.7894m, full pseudorange, represented in meters with
     *               a precision of 0.0006m
     *               0-299792.4574m, modulo 1-ms pseudorange, represented in meters
     *               with a precision of 0.0006m*/
    double  pseudorange;
    /** valid range: 0-76447976.16723m, full phase range, represented in meters with
     *               a precision of 0.00014m
     *               0-299792.45786m, modulo 1-ms phase range, represented in meters
     *               with a precision of 0.00014m*/
    double  phaseRange;
    /** Phaserange-Rate, valid range: -8192.6383 ~ 8192.6383 m */
    double  phaseRangeRate;
    /** Phase range lock time indicator, valid range: 0-1023 */
    uint16_t phaseRangeLockTimeIndicator;
    /** GNSS signal CNR, valid range: 0.0625-63.9375 dB-Hz */
    float signalCNR;
} DGnssMeasurement;

typedef struct {
    /** Reference station ID, valid range: 0-4095 */
    uint16_t                      stationID;
    /** GNSS epoch time */
    GnssEpochTime                 epochTime;
    /** common observation status flags */
    DGnssObservationStatusFlags   statusFlags;
    /** issue of Data Station, valid range: 0-7 */
    uint8_t                       issueOfDataStation;
    /** DGNSS constellaton identifier */
    DGnssConstellationIdentifier  constellationIdentifier;
    /** number of measurements, valid range: 1-64 */
    uint8_t                       numberOfMeasurements;
    DGnssMeasurement dGnssMeasurement[DGNSS_MEASUREMENT_MAX];
} DGnssObservation;

/*Ephemeris data*/
typedef struct {
    /** GPS SV ID, range: 1-32,
     *  QZSS SV ID, range: 193-202 */
    uint8_t    svId;
    /** status for L2 frequency (2-bits) */
    uint8_t    codeL2;
    /** User Range Accuracy (4-bits) */
    uint8_t    accuracy;
    /** SV health (6-bits) */
    uint8_t    health;
    /** issue of data clock (10-bits) */
    uint16_t   iodc;
    /** P-Code indication flag (1-bit) */
    uint8_t    pcodeFlag;
    /** True if P-code indication is valid */
    bool       isPcodeFlagValid;
    /** Reserved bits (87-bits, First bit in LSB) */
    uint32_t   sf1ResvBits[3];
    /** True if SF1 Reserved bits are valid */
    bool       areSf1BitsValid;
    /** Time of Group Delay (8-bits) */
    uint8_t    tgd;
    /** Time of Group Delay vadidity flag */
    bool       isTgdValid;
    /** Time of clock */
    uint16_t   toc;
    /** clock acceleration coefficient */
    uint8_t    af2;
    /** clock drift coefficient */
    uint16_t   af1;
    /** clock bias coefficient */
    uint32_t   af0;
    /** Issue of data ephemeris tag */
    uint8_t    iode;
    /** Amplitude of the sine harmonic correction term to Orbit Radius */
    uint16_t   Crs;
    /** mean motion difference from computed value */
    uint16_t   deltaN;
    /** mean anomaly at reference time */
    uint32_t   M0;
    /** Amplitude of the cosine harmonic correction term to argument of latitude */
    uint16_t   Cuc;
    /** orbital eccentricity */
    uint32_t   e;
    /** Amplitude of the sine harmonic correction term  to argument of latitude */
    uint16_t   Cus;
    /** square-root of the semi-major axis */
    uint32_t   sqrtA;
    /** Reference time of Ephemeris */
    uint16_t   toe;
    /** Fit interval */
    uint8_t    fitIntervalFlag;
    /** Fit interval validity flag */
    bool       isFitIntervalFlagValid;
    /** Amplitude of the cosine harmonic correction term to angle of inclination */
    uint16_t   Cic;
    /** longitude of the ascending node of orbit plane at weekly epoch */
    uint32_t   Omega0;
    /** Amplitude of the sine harmonic correction term to angel of inclination */
    uint16_t   Cis;
    /** inclination angle at reference time */
    uint32_t   i0;
    /** Amplitude of the cosine harmonic correction term to orbit radius */
    uint16_t   Crc;
    /** argument of Perigee */
    uint32_t   omega;
    /** rate of right ascension */
    uint32_t   omegaDot;
    /** rate of inclination angle */
    uint16_t   idot;
    /** aodo validity flag*/
    bool       isAodoValid;
    /** age of data offset*/
    uint8_t    aodo;
    /** TRUE if the week number specified is valid*/
    bool       isGpsWeekValid;
    /** unambiguous gps week */
    uint16_t   gpsWeek;
} gpsEphemerisType;

typedef struct {
    /** flags for optional data */
    union {
        uint8_t  filledMask;
        struct {
            uint8_t flagN4      : 1;
            uint8_t flagKp      : 1;
            uint8_t flagNt      : 1;
            uint8_t flagTaoGps  : 1;
            uint8_t flagFreqNum : 1;
            uint8_t flagTaoC    : 1;
            uint8_t             : 2;
        } filledFlagMask;
    } mask;
    /** number of 4-year cycle since 1996, starting from 1 */
    uint8_t  N4;
    /** leap sec change notice, 1: will be changed */
    uint8_t  Kp;
    /** number of days in a 4-year cycle, starting from 1 */
    uint8_t  Nt;
    /** TaoGps: unscaled */
    uint8_t  taoGps;
    /** should be ignored if the type is NOT OTA */
    uint8_t  freqNumber;
    /** Glonass time scale correction to UTC time, for GLO-M */
    uint8_t  taoC;
    /** GLOUTC_SRC_INVALID, GLOUTC_SRC_OTA, GLOUTC_SRC_XTRA,
     *  GLOUTC_SRC_ASSIST */
    uint8_t  src;
} glonassUtcType;

typedef struct {
    /** GLONASS satellite slot number, range: 0-63
     *  0:unknown, 1-24:GLONASS slot number, >32:reserved for SBAS */
    uint8_t    svId;
    /** flags for optional data */
    union {
        uint8_t  filledMask;
        struct {
            uint8_t  flagGloType  : 1;
            uint8_t  flagTimeSrcP : 1;
            uint8_t  flagFt       : 1;
            uint8_t  flagNt       : 1;
            uint8_t  flagP4       : 1;
            uint8_t  flagLn       : 1;
            uint8_t               : 2;
        }filledFlagMask;
    } mask;
    /** glonass eph reference time tb */
    uint8_t   tb;
    /** glonass-type: 00-GLONASS, 01-GLONASS-M, 02-GLONASS-K */
    uint8_t   gloType;
    /** indication the satellite operation mode in repect fo time parameters P */
    uint8_t   timeSrcP;
    /** health flag Bn */
    uint8_t   Bn;
    /** time diff between the L2 and L1 transmitters: Delta_Tao_n */
    uint8_t   timeDiffL1L2;
    /** indication of time interval between adjacent eph P1 */
    uint8_t   P1;
    /** interval indicator P2 */
    uint8_t   P2;
    /** additional health flag only available on GLONASS-M satellite */
    uint8_t   ln;
    /** frequency index */
    uint8_t   freqNumber;
    /** FT is a parameter shown the URE at time tb */
    uint8_t   Ft;
    /** acceleration due to lunar-solar gravitational perturbation */
    uint8_t   acceleration[3];
    /** calendar number of day within four-year interval starting from the 1st of January in a leap year */
    uint16_t  Nt;
    /** (f(pre)-f(nom))/f(nom), Gamma_n */
    uint16_t  freqOff;
    /** satellite clock correction: Tao_n */
    uint32_t  clockCorrection;
    /** satellite position */
    uint32_t  satellitePosition[3];
    /** satellite velocity */
    uint32_t  satelliteVelocity[3];
    /** age of eph in days */
    uint8_t   En;
    /** flag P4 */
    uint8_t   P4;
} glonassEphemerisType;

typedef struct {
    /** Galileo SV ID, range: 0-63, 1-36:I/NAV and F/NAV, >36:reserved for future use */
    uint16_t    svId;
    /** signal in space accuracy */
    uint8_t     sisa;
    /** issue of data ephemeris tag */
    uint16_t    iode;
    /** reference time of ephemeris */
    uint16_t    toe;
    /** mean anomaly at reference time */
    uint32_t    M0;
    /** mean motion difference from computed value */
    uint16_t    deltaN;
    /** orbital eccentricity */
    uint32_t    e;
    /** square-root of the semi-major axis */
    uint32_t    sqrtA;
    /** longitude of the ascending node of orbit plane computed according to ref. time */
    uint32_t    Omega0;
    /** argument of perigee */
    uint32_t    omega;
    /** rate of right ascension */
    uint32_t    omegaDot;
    /** inclination angle at reference time */
    uint32_t    i0;
    /** rate of inclination angle */
    uint16_t    idot;
    /** Amplitude of the cosine harmonic correction term to angle of inclination */
    uint16_t    Cic;
    /** Amplitude of the sine harmonic correction term to angel of inclination */
    uint16_t    Cis;
    /** Amplitude of the cosine harmonic correction term to orbit radius */
    uint16_t    Crc;
    /** Amplitude of the sine harmonic correction term to Orbit Radius */
    uint16_t    Crs;
    /** Amplitude of the cosine harmonic correction term to argument of latitude */
    uint16_t    Cuc;
    /** Amplitude of the sine harmonic correction term  to argument of latitude */
    uint16_t    Cus;
    /** week number */
    uint16_t    galileoWeek;
    /** 6 bits for INAV: E5bHS(2-bit), E1bHS(2-bit), E5bDVS(1-bit), E1bDVS(1-bit,LSB)
     *  3 bits for FNAV: E5aHS(2-bit), E5aDVS(1-bit, LSB)
     */
    uint8_t     health;
    /** time of clock */
    uint16_t    toc;
    /** clock acceleration coefficient */
    uint8_t     af2;
    /** clock drift coefficient */
    uint32_t    af1;
    /** clock bias coefficient */
    uint32_t    af0;
    /** BGD (E1, E5a) */
    uint16_t    bgdE1E5a;
    /** BGD (E1, E5b) */
    uint16_t    bgdE1E5b;
    /** Tgd flag */
    uint16_t    tgd;
    /** TRUE if it is from FNAV */
    bool        isFnav;
} galileoEphemerisType;

typedef struct {
    /** BDS SV ID, range: 1-63, 1-37: BDS PRN, >37: reserved */
    uint8_t    svId;
    /** unambiguous BDS week number */
    uint16_t   bdsWeek;
    /** reference time of ephemeris */
    uint32_t   toe;
    /** time of clock */
    uint32_t   toc;
    /** age of data ephemeris */
    uint8_t    aode;
    /** age of data clock */
    uint8_t    aodc;
    /** SV health */
    uint8_t    health;
    /** user range accuracy index */
    uint8_t    URAI;
    /** square-root of the semi-major axis */
    uint32_t   sqrtA;
    /** orbital eccentricity */
    uint32_t   e;
    /** argument of perigee */
    uint32_t   omega;
    /** mean motion difference from computed value */
    uint16_t   deltaN;
    /** mean anomaly at reference time */
    uint32_t   M0;
    /** longitude of the ascending node of orbit plane computed according to ref. time */
    uint32_t   Omega0;
    /** rate of right ascension */
    uint32_t   omegaDot;
    /** inclination angle at reference time */
    uint32_t   i0;
    /** rate of inclination angle */
    uint16_t   idot;
    /** Amplitude of the sine harmonic correction term to Orbit Radius */
    uint32_t   Crs;
    /** Amplitude of the cosine harmonic correction term to argument of latitude */
    uint32_t   Cuc;
    /** Amplitude of the sine harmonic correction term  to argument of latitude */
    uint32_t   Cus;
    /** Amplitude of the cosine harmonic correction term to angle of inclination */
    uint32_t   Cic;
    /** Amplitude of the sine harmonic correction term to angel of inclination */
    uint32_t   Cis;
    /** Amplitude of the cosine harmonic correction term to orbit radius */
    uint32_t   Crc;
    /** clock acceleration coefficient */
    uint16_t   a2;
    /** clock drift coefficient */
    uint32_t   a1;
    /** clock bias coefficient */
    uint32_t   a0;
    /** time of group delay */
    uint16_t   tgd;
} bdsEphemerisType;

typedef struct {
    /** single band BDS raw ephemeris */
    bdsEphemerisType bdsEph;
    /** Tgd2 */
    uint16_t         tgd2;
    /** Tgd2 validity flag */
    bool             isTgd2Valid;
} bdsMbRawEphemerisType;

typedef struct {
    /** GEO satellite PRN, range:120-158 */
    uint16_t   geoPrn;
    /** issue of data GEO */
    uint8_t    iodg;
    /** user range accuracy */
    uint8_t    ura;
    /** X of GEO (ECEF) meters */
    float      xg;
    /** Y of GEO (ECEF) meters */
    float      yg;
    /** Z of GEO (ECEF) meters */
    float      zg;
    /** X of GEO rate-of-change meters/sec */
    float      xgDot;
    /** Y of GEO rate-of-change meters/sec */
    float      ygDot;
    /** Z of GEO rate-of-change meters/sec */
    float      zgDot;
    /** X of GEO acceleration meters/sec^2 */
    float      xgAcceleration;
    /** Y of GEO acceleration meters/sec^2 */
    float      ygAcceleration;
    /** Z of GEO acceleration meters/sec^2 */
    float      zgAcceleration;
    /** clock offset of GEO seconds */
    double     clockOffset;
    /** clock drift of GEO seconds/sec */
    double     clockDrift;
    /** time of applicability */
    uint32_t   timeOfApplicability;
    /** validity of GEO ephemeris */
    uint8_t    valid;
    /** cumulative GPS time of the update */
    double     updateTimeMsec;
} sbasGeoEphemerisType;

#endif
