/******************************************************************************
    Copyright (c) 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "EnginePluginLibAPI.h"

#define EP_GNSS_ALL_CONSTELLATION (EP_GNSS_CONSTELLATION_GPS |\
        EP_GNSS_CONSTELLATION_GALILEO | EP_GNSS_CONSTELLATION_SBAS |\
        EP_GNSS_CONSTELLATION_GLONASS | EP_GNSS_CONSTELLATION_BEIDOU |\
        EP_GNSS_CONSTELLATION_QZSS)

#define EP_GNSS_ALL_SIGNAL_TYPE (EP_GNSS_SIGNAL_GPS_L1CA | EP_GNSS_SIGNAL_GPS_L1C |\
        EP_GNSS_SIGNAL_GPS_L2C_L | EP_GNSS_SIGNAL_GPS_L5_Q | EP_GNSS_SIGNAL_GLONASS_G1_CA |\
        EP_GNSS_SIGNAL_GLONASS_G2_CA | EP_GNSS_SIGNAL_GALILEO_E1_C |\
        EP_GNSS_SIGNAL_GALILEO_E5A_Q | EP_GNSS_SIGNAL_GALILIEO_E5B_Q |\
        EP_GNSS_SIGNAL_BEIDOU_B1_I | EP_GNSS_SIGNAL_BEIDOU_B1_C | EP_GNSS_SIGNAL_BEIDOU_B2_I |\
        EP_GNSS_SIGNAL_BEIDOU_B2A_I | EP_GNSS_SIGNAL_QZSS_L1CA | EP_GNSS_SIGNAL_QZSS_L1S |\
        EP_GNSS_SIGNAL_QZSS_L2C_L | EP_GNSS_SIGNAL_QZSS_L5_Q | EP_GNSS_SIGNAL_SBAS_L1_CA)

using namespace std;

EPInterface ePInterface = {};

bool handleEPSessionContrlCommand(const epCommandRequest *cmdParameters)
{
    bool returnValue = false;
    printf("---> \n");
    if (NULL != cmdParameters) {
        printf("command %d \n", cmdParameters->command );
        returnValue = true;
    }
    printf("<---- \n");
    return returnValue;
}

void handleEPGnssEphemeris(const epGnssEphemerisReport *ephemerisResport)
{
    printf("---> \n");
    if (NULL != ephemerisResport) {
        printf("Ephemeris Constellation %d \n", ephemerisResport->gnssConstellation);
    }
    printf("<---- \n");
}

void handleEPProvideGnssSvMeasurement(const epGnssMeasurementReport *msrReport)
{
    printf("---> \n");
    if (NULL != msrReport) {
        printf("Measurement# %d \n", msrReport->numMeas);
    }
    printf("<---- \n");
}

void handleEPProvidePosition(const epPVTReport *positionReport)
{
    printf("---> \n");
    if (NULL != positionReport) {
        printf("posFlags %x \n", positionReport->posFlags);
    }
    printf("<---- \n");
}

void initEpInterface(EPInterface* epIntrface)
{
    if (NULL == epIntrface) {
        return;
    }

    epIntrface->epInterfaceFileMajorVersion = EP_INTERFACE_FILE_MAJOR_VERSION;
    epIntrface->epInterfaceFileMinorVersion = EP_INTERFACE_FILE_MAJOR_VERSION;

    epIntrface->epSessionContrlCommand = handleEPSessionContrlCommand;
    epIntrface->epProvideGnssEphemeris = handleEPGnssEphemeris;
    epIntrface->epProvideGnssSvMeasurement = handleEPProvideGnssSvMeasurement;
    epIntrface->epProvidePosition = handleEPProvidePosition;

    return;
}

int main(int argc, char * argv[])
{
    int sleepTimer = 100;
    initEpInterface(&ePInterface);
    const struct EPCallbacks* pEpCbs = init_ep_lib_interface(&ePInterface);
    if (NULL == pEpCbs) {
        printf("failed to init EnginePlugin instance \n");
        return -1;
    }
    epSubscriptionInfo subscriptionMsg = {};
    subscriptionMsg.flags = (EP_SUBSCRIPTION_PVT_1HZ_REPORT | \
        EP_SUBSCRIPTION_MEASUREMENT_NHZ_REPORT | \
        EP_SUBSCRIPTION_MEASUREMENT_1HZ_REPORT | \
        EP_SUBSCRIPTION_EPHEMERIS_REPORT);
    subscriptionMsg.subscriptionModMeasurement.constellationType = EP_GNSS_ALL_CONSTELLATION;
    subscriptionMsg.subscriptionModMeasurement.gnssSignal = EP_GNSS_ALL_SIGNAL_TYPE;
    subscriptionMsg.subscriptionModMeasurementNHz.constellationType = EP_GNSS_ALL_CONSTELLATION;
    subscriptionMsg.subscriptionModMeasurementNHz.gnssSignal = EP_GNSS_ALL_SIGNAL_TYPE;
    subscriptionMsg.subscriptionModEphemeris.constellationType = EP_GNSS_ALL_CONSTELLATION;
    subscriptionMsg.subscriptionModEphemeris.gnssSignal = EP_GNSS_ALL_SIGNAL_TYPE;
    if (nullptr != pEpCbs->epRequestReportSubscription) {
        printf("calling ReportSubscription.. flags: %x \n", subscriptionMsg.flags);
        pEpCbs->epRequestReportSubscription(&subscriptionMsg);
    }
    printf("Waiting for %d seconds \n", sleepTimer);
    sleep(sleepTimer);

    deinit_ep_lib_interface();

    printf("exiting.. \n ");
    return 0;
}
