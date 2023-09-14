/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  This file contains the application interface definitions of QTI DGNSS
  Correction Data framework.

  Copyright (c) 2019 - 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef QDGNSSAPI_H
#define QDGNSSAPI_H

#include "IQDgnss.h"

class QDgnssSource : public IQDgnssSource
{
public:
    /** @brief Create an instance of an QDgnssSource object.
        It is strongly recommended that the application creates a
        separate QDgnssSource object for each source.

        For example, the application downloads RTCM messages from
        Ntrip caster 1, creating a QDgnssSource object. Later, when
        the application connects to Ntrip caster 2, the application
        shall create a different QDgnssSource object.

        Failure to follow the rule would affect the measurement and
        position accuracy which are calculated by the QC position
        engine.

        @param dataSource
        Tell correction data framework who is going to inject the data
        @param dataFormat
        Tell correction data framework the injected data format
        @param statusCallback
        Correction data framework updates the status to the source object
        through this call back function.
    */
    QDgnssSource(QDgnssDataSource dataSource,
                 QDgnssInjectDataFormat dataFormat,
                 QDgnssSourceStatusCb statusCallback);

    /** @brief Default destructor */
    virtual ~QDgnssSource();

    /** @brief Inject the raw correction data to the correction data framework.
        The correction data framework will parse the injected buffer into
        QDGNss messages.

        @param buffer
        The raw correction data message buffer
        @param bufferSize
        The buffer size

    */
    void injectCorrectionData(const uint8_t* buffer, uint32_t bufferSize) override;

private:
    QDgnssSource() = delete;
    QDgnssSource(const QDgnssSource& source) = delete;
    QDgnssSource& operator=(const QDgnssSource& source) = delete;
};

class QDgnssListener : public IQDgnssListener
{
public:
    /** @brief QDgnssListener object receives QDgnss messages.

        @param dGnssMsg: DGnss message pointer
        The correction data framework sends QDgnss messages to the
        listener. The dGnssMsg is owned by the correction data
        framework. The listener must not use the dGnssMsg after
        onReceiveMsg returns.
    */
    virtual void onReceiveMsg(unique_ptr<IQDgnssMsg>& dGnssMsg) = 0;

    /** @brief Default destructor */
    virtual ~QDgnssListener();

    void reportDataUsage(const SourceDataUsageMsg& sourceDataUsage) override;

protected:
    /** @brief Create an instance of QDgnssListener object.

        @param registeredMsgs
        Tell the correction data framework which QDgnss messages the
        listener is interested in listening to.
    */
    QDgnssListener(QDgnssMsgMask registeredMsgs);

    /** @brief Default constructor, listens to all DGnss messages */
    QDgnssListener(): QDgnssListener(QDGNSS_ALL_DGNSS_MSG_MASK) {}

private:
    QDgnssListener(const QDgnssListener& listener) = delete;
    QDgnssListener& operator=(const QDgnssListener& listener) = delete;
};

#endif /* QDGNSSAPI_H */
