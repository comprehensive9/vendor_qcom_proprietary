/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  This file contains the abstract interface definitions of QTI DGNSS Correction
  Data framework.

  Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IQDGNSS_H
#define IQDGNSS_H

#include <stdint.h>
#include <functional>
#include <memory>

using namespace std;

enum QDgnssDataSource {
    /** Source inject application is unknown */
    CDFW_DATA_SOURCE_UNKNOWN    = 0,
    /** Source inject application is Cellular
     *  Vehicle-to-Everything */
    CDFW_DATA_SOURCE_CV2X       = 1,
    /** Internal source injector 1 */
    CDFW_DATA_SOURCE_INTERNAL_1 = 2
};

enum QDgnssInjectDataFormat{
    /** Source data format is unknown */
    CDFW_DATA_FORMAT_UNKNOWN                = 0,
    /** Source data format is RTCM_3 */
    CDFW_DATA_FORMAT_RTCM_3                 = 1
};

enum QDgnssSourceStatus{
    /** CDFW advises the source to inject data */
    CDFW_READY_TO_ACCEPT_DATA               = 0,
    /** CDFW does not support the data source */
    CDFW_DATA_SOURCE_NOT_SUPPORTED          = 1,
    /** CDFW does not support the data format */
    CDFW_DATA_FORMAT_NOT_SUPPORTED          = 2,
    /** There is another higher priority source injecting the data
     *  at the same time, the data injected by this source will be
     *  dropped */
    CDFW_OTHER_SOURCE_IN_USE                = 3,
    /** There is a parsing error such as unrecognized format, CRC
     *  check failure, value range check failure, etc.; the injected
     *  data is dropped */
    CDFW_MESSAGE_PARSE_ERROR                = 4,
    /** The injected DGNSS data is usable */
    CDFW_DATA_USABLE                        = 5,
    /** The injected DGNSS data is not usable, for example,
    the referece station is too far away to improve the position
    accuracy */
    CDFW_DATA_NOT_USABLE                    = 6
};

enum QDgnssMsgId {
    QDGNSS_REFERENCE_STATION_MSG_ID                 = 0,
    QDGNSS_OBSERVATIONS_MSG_ID                      = 1,
    QDGNSS_GLONASS_BIAS_MSG_ID                      = 2,
    QDGNSS_SSR_ORBIT_CORRECTIONS_MSG_ID             = 3,
    QDGNSS_SSR_CLOCK_CORRECTIONS_MSG_ID             = 4,
    QDGNSS_SSR_HIGHRATE_CLOCK_CORRECTIONS_MSG_ID    = 5,
    QDGNSS_SSR_CODE_BIAS_MSG_ID                     = 6,
    QDGNSS_SSR_USER_RANGE_ACCURACY_MSG_ID           = 7,
    QDGNSS_SSR_ORBIT_AND_CLOCK_MSG_ID               = 8,
    QDGNSS_GPS_EPHEMERIS_MSG_ID                     = 9,
    QDGNSS_GLONASS_EPHEMERIS_MSG_ID                 = 10,
    QDGNSS_GALILEO_EPHEMERIS_MSG_ID                 = 11,
    QDGNSS_BDS_EPHEMERIS_MSG_ID                     = 12,
    QDGNSS_QZSS_EPHEMERIS_MSG_ID                    = 13,
    QDGNSS_SBAS_EPHEMERIS_MSG_ID                    = 14,
    QDGNSS_DGNSS_MSG_MAX_ID                         = 15,
    QDGNSS_SESSION_ACTIVE_MSG_ID                    = 63
};

#define QDGNSS_MSG_ID_TO_MSG_MASK(ID)   ((QDgnssMsgMask)(0x1ULL << (ID)))

typedef uint64_t QDgnssMsgMask;
/** DGNSS messages */
#define QDGNSS_REFERENCE_STATION_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_REFERENCE_STATION_MSG_ID)
#define QDGNSS_OBSERVATIONS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_OBSERVATIONS_MSG_ID)
#define QDGNSS_GLONASS_BIAS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_GLONASS_BIAS_MSG_ID)
#define QDGNSS_SSR_ORBIT_CORRECTIONS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SSR_ORBIT_CORRECTIONS_MSG_ID)
#define QDGNSS_SSR_CLOCK_CORRECTIONS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SSR_CLOCK_CORRECTIONS_MSG_ID)
#define QDGNSS_SSR_HIGHRATE_CLOCK_CORRECTIONS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SSR_HIGHRATE_CLOCK_CORRECTIONS_MSG_ID)
#define QDGNSS_SSR_CODE_BIAS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SSR_CODE_BIAS_MSG_ID)
#define QDGNSS_SSR_USER_RANGE_ACCURACY_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SSR_USER_RANGE_ACCURACY_MSG_ID)
#define QDGNSS_SSR_ORBIT_AND_CLOCK_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SSR_ORBIT_AND_CLOCK_MSG_ID)
#define QDGNSS_GPS_EPHEMERIS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_GPS_EPHEMERIS_MSG_ID)
#define QDGNSS_GLONASS_EPHEMERIS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_GLONASS_EPHEMERIS_MSG_ID)
#define QDGNSS_GALILEO_EPHEMERIS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_GALILEO_EPHEMERIS_MSG_ID)
#define QDGNSS_BDS_EPHEMERIS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_BDS_EPHEMERIS_MSG_ID)
#define QDGNSS_QZSS_EPHEMERIS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_QZSS_EPHEMERIS_MSG_ID)
#define QDGNSS_SBAS_EPHEMERIS_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SBAS_EPHEMERIS_MSG_ID)
#define QDGNSS_DGNSS_MSG_MAX_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_DGNSS_MSG_MAX_ID)

/** Event message */
#define QDGNSS_SESSION_ACTIVE_MSG_BIT \
        QDGNSS_MSG_ID_TO_MSG_MASK(QDGNSS_SESSION_ACTIVE_MSG_ID)

/** BIT OR from QDGNSS_REFERENCE_STATION_MSG_BIT to QDGNSS_SBAS_EPHEMERIS_MSG_BIT */
#define QDGNSS_ALL_DGNSS_MSG_MASK (QDGNSS_DGNSS_MSG_MAX_BIT - 1)

typedef struct {
    bool    sourceDataUsable;
} SourceDataUsageMsg;

typedef struct {
    bool    sessionActive;
} SessionActiveMsg;

/** @fn
    @brief Correction Data Framework sends status
    to the QDgnssSource object(s)

    @param statusUpdate:  status of the source
*/
typedef std::function<void(
    QDgnssSourceStatus    statusUpdate
)> QDgnssSourceStatusCb;

class IQDgnssMsg
{
public:
    virtual QDgnssMsgId getMsgId() const = 0;
    virtual uint32_t getMsgSize() const = 0;
    virtual QDgnssDataSource getDataSource() const = 0;
    virtual QDgnssInjectDataFormat getDataSourceFormat() const = 0;
    virtual uint32_t getDataSourceId() const = 0;
    virtual void* getMsg() const = 0;
    virtual ~IQDgnssMsg() {}
};

class IQDgnssSource
{
public:
    /** @brief Default destructor */
    virtual ~IQDgnssSource(){}

    /** @brief Inject the raw correction data to the correction data framework.
        The correction data framework will parse the injected buffer into
        QDGNss messages.

        @param buffer
        The raw correction data message buffer
        @param bufferSize
        The buffer size
    */
    virtual void injectCorrectionData(const uint8_t* buffer, uint32_t bufferSize) = 0;
};

class IQDgnssListener
{
public:
    /** @brief Default destructor */
    virtual ~IQDgnssListener(){}

    /** @brief QDgnssListener object receives QDgnss messages.

        @param dGnssMsg: DGnss message pointer
        The correction data framework sends QDgnss messages to the
        listener. The dGnssMsg is owned by the correction data
        framework. The listener must not use the dGnssMsg after
        onReceiveMsg returns.
    */
    virtual void onReceiveMsg(unique_ptr<IQDgnssMsg>& dGnssMsg) = 0;

    /** @brief QDgnssListener object report DGNSS data usage
        status to the correction data framework.

        @param  sourceDataUsage
        DGNSS data usage status */
    virtual void reportDataUsage(const SourceDataUsageMsg& sourceDataUsage) = 0;
};

#endif /* IQDGNSS_H */
