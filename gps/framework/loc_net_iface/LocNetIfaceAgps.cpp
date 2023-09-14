/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#include <LocNetIfaceAgps.h>
#include <loc_pla.h>
#include <log_util.h>

#define LOG_TAG "LocSvc_LocNetIfaceHolder"

/* LocNetIfaceAgps members */
LocNetIface* LocNetIfaceAgps::sLocNetIfaceAgpsInternet = NULL;
LocNetIface* LocNetIfaceAgps::sLocNetIfaceAgpsSupl = NULL;
LocNetAgpsState LocNetIfaceAgps::sAgpsStateInternet = LOC_NET_AGPS_STATE_CLOSED;
LocNetAgpsState LocNetIfaceAgps::sAgpsStateSupl = LOC_NET_AGPS_STATE_CLOSED;
LocAgpsOpenResultCb LocNetIfaceAgps::sAgpsOpenResultCb = NULL;
LocAgpsCloseResultCb LocNetIfaceAgps::sAgpsCloseResultCb = NULL;
void* LocNetIfaceAgps::sUserDataPtr = NULL;
AgpsCbInfo LocNetIfaceAgps::sAgpsCbInfo = {};

#define LOCNETIFACE_AGPS_CLIENT    "agps-client"

/* Method accessed from HAL */
AgpsCbInfo& LocNetIfaceAgps_getAgpsCbInfo(
        LocAgpsOpenResultCb openResultCb,
        LocAgpsCloseResultCb closeResultCb, void* userDataPtr) {

    ENTRY_LOG();

    /* Save callbacks and userDataPtr */
    LocNetIfaceAgps::sAgpsOpenResultCb = openResultCb;
    LocNetIfaceAgps::sAgpsCloseResultCb = closeResultCb;
    LocNetIfaceAgps::sUserDataPtr = userDataPtr;

    /* Create LocNetIface instances */
    if (LocNetIfaceAgps::sLocNetIfaceAgpsInternet == NULL) {
        LocNetIfaceAgps::sLocNetIfaceAgpsInternet =
                new LocNetIface(LOC_NET_CONN_TYPE_WWAN_INTERNET);
        LocNetIfaceAgps::sLocNetIfaceAgpsInternet->
        registerWwanCallStatusCallback(
                LocNetIfaceAgps::wwanStatusCallback,
                LocNetIfaceAgps::sLocNetIfaceAgpsInternet);
    } else {
        LOC_LOGE("sLocNetIfaceAgpsInternet not NULL");
    }

    if (LocNetIfaceAgps::sLocNetIfaceAgpsSupl == NULL) {
        LocNetIfaceAgps::sLocNetIfaceAgpsSupl =
                new LocNetIface(LOC_NET_CONN_TYPE_WWAN_SUPL);
        LocNetIfaceAgps::sLocNetIfaceAgpsSupl->
        registerWwanCallStatusCallback(
                LocNetIfaceAgps::wwanStatusCallback,
                LocNetIfaceAgps::sLocNetIfaceAgpsSupl);
    } else {
        LOC_LOGE("sLocNetIfaceAgpsSupl not NULL");
    }

    /* Return our callback */
    LocNetIfaceAgps::sAgpsCbInfo.statusV4Cb = (void*)LocNetIfaceAgps::agpsStatusCb;
    return LocNetIfaceAgps::sAgpsCbInfo;
}

void LocNetIfaceAgps::agpsStatusCb(AGnssExtStatusIpV4 status){

    ENTRY_LOG();

    /* Validate */
    if (sLocNetIfaceAgpsInternet == NULL) {
        LOC_LOGE("Not init'd");
        return;
    }

    if (status.status == LOC_GPS_REQUEST_AGPS_DATA_CONN) {

        if (status.type == LOC_AGPS_TYPE_SUPL) {

            LOC_LOGV("REQUEST LOC_AGPS_TYPE_SUPL");
            sAgpsStateSupl = LOC_NET_AGPS_STATE_OPEN_PENDING;
            if (!sLocNetIfaceAgpsSupl->setupWwanCall()) {
                LOC_LOGE("Setup wwan call failed !");
                wwanStatusCallback(
                        sLocNetIfaceAgpsSupl,
                        LOC_NET_WWAN_CALL_EVT_OPEN_FAILED,
                        NULL, LOC_NET_CONN_IP_TYPE_INVALID);
                sAgpsStateSupl = LOC_NET_AGPS_STATE_CLOSED;
            }
        } else if (status.type == LOC_AGPS_TYPE_WWAN_ANY) {

            LOC_LOGV("REQUEST LOC_AGPS_TYPE_WWAN_ANY");
            sAgpsStateInternet = LOC_NET_AGPS_STATE_OPEN_PENDING;
            if (!sLocNetIfaceAgpsInternet->connectBackhaul(LOCNETIFACE_AGPS_CLIENT, false)) {
                LOC_LOGE("Connect Backhaul failed");
                wwanStatusCallback(
                        sLocNetIfaceAgpsInternet,
                        LOC_NET_WWAN_CALL_EVT_OPEN_FAILED,
                        NULL, LOC_NET_CONN_IP_TYPE_INVALID);
                sAgpsStateInternet = LOC_NET_AGPS_STATE_CLOSED;
            }
        } else {

            LOC_LOGE("Unsupported AGPS type %d", status.type);
        }
    }
    else if (status.status == LOC_GPS_RELEASE_AGPS_DATA_CONN) {

        if (status.type == LOC_AGPS_TYPE_SUPL) {

            LOC_LOGV("RELEASE LOC_AGPS_TYPE_SUPL");
            sAgpsStateSupl = LOC_NET_AGPS_STATE_CLOSE_PENDING;
            if (!sLocNetIfaceAgpsSupl->stopWwanCall()) {
                LOC_LOGE("Stop wwan call failed !");
                wwanStatusCallback(
                        sLocNetIfaceAgpsSupl,
                        LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED,
                        NULL, LOC_NET_CONN_IP_TYPE_INVALID);
                sAgpsStateSupl = LOC_NET_AGPS_STATE_CLOSED;
            }
        } else if (status.type == LOC_AGPS_TYPE_WWAN_ANY) {

            LOC_LOGV("RELEASE LOC_AGPS_TYPE_WWAN_ANY");
            sAgpsStateInternet = LOC_NET_AGPS_STATE_CLOSE_PENDING;
            if (!sLocNetIfaceAgpsInternet->disconnectBackhaul(LOCNETIFACE_AGPS_CLIENT)) {
                LOC_LOGE("Disconnect backhaul failed !");
                wwanStatusCallback(
                        sLocNetIfaceAgpsInternet,
                        LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED,
                        NULL, LOC_NET_CONN_IP_TYPE_INVALID);
                sAgpsStateInternet = LOC_NET_AGPS_STATE_CLOSED;
            }
        } else {

            LOC_LOGE("Unsupported AGPS type %d", status.type);
        }
    }
    else {
        LOC_LOGE("Unsupported AGPS action %d", status.status);
    }
}

void LocNetIfaceAgps::wwanStatusCallback(
            void* userDataPtr, LocNetWwanCallEvent event,
            const char* apn, LocNetConnIpType apnIpType){

    ENTRY_LOG();
    LOC_LOGV("event: %d, apnIpType: %d", event, apnIpType);

    /* Derive bearer type */
    AGpsBearerType bearerType = AGPS_APN_BEARER_INVALID;
    switch (apnIpType) {
        case LOC_NET_CONN_IP_TYPE_V4:
            bearerType = AGPS_APN_BEARER_IPV4;
            break;
        case LOC_NET_CONN_IP_TYPE_V6:
            bearerType = AGPS_APN_BEARER_IPV6;
            break;
        case LOC_NET_CONN_IP_TYPE_V4V6:
            bearerType = AGPS_APN_BEARER_IPV4V6;
            break;
        default:
            LOC_LOGE("Invalid APN IP type %d", apnIpType);
    }

    /* Derive AGPS type */
    AGpsExtType agpsType = LOC_AGPS_TYPE_INVALID;
    LocNetAgpsState* agpsStatePtr = NULL;
    if (userDataPtr == (void*)sLocNetIfaceAgpsInternet) {
        agpsType = LOC_AGPS_TYPE_WWAN_ANY;
        agpsStatePtr = &sAgpsStateInternet;
    }
    else if (userDataPtr == (void*)sLocNetIfaceAgpsSupl) {
        agpsType = LOC_AGPS_TYPE_SUPL;
        agpsStatePtr = &sAgpsStateSupl;
    }
    else {
        LOC_LOGE("Invalid user data ptr %p", userDataPtr);
        return;
    }

    /* Complete AGPS call flow */
    if (event == LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS &&
            *agpsStatePtr == LOC_NET_AGPS_STATE_OPEN_PENDING) {
        LOC_LOGV("LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS");
        sAgpsOpenResultCb(
                true, agpsType, apn, bearerType, sUserDataPtr);
        *agpsStatePtr = LOC_NET_AGPS_STATE_OPENED;
    }
    else if (event == LOC_NET_WWAN_CALL_EVT_OPEN_FAILED &&
            *agpsStatePtr == LOC_NET_AGPS_STATE_OPEN_PENDING) {
        LOC_LOGE("LOC_NET_WWAN_CALL_EVT_OPEN_FAILED");
        sAgpsOpenResultCb(
                false, agpsType, apn, bearerType, sUserDataPtr);
        *agpsStatePtr = LOC_NET_AGPS_STATE_CLOSED;
    }
    else if (event == LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS &&
            *agpsStatePtr == LOC_NET_AGPS_STATE_CLOSE_PENDING) {
        LOC_LOGV("LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS");
        sAgpsCloseResultCb(true, agpsType, sUserDataPtr);
        *agpsStatePtr = LOC_NET_AGPS_STATE_CLOSED;
    }
    else if (event == LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED &&
            *agpsStatePtr == LOC_NET_AGPS_STATE_CLOSE_PENDING) {
        LOC_LOGE("LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED");
        sAgpsCloseResultCb(false, agpsType, sUserDataPtr);
        *agpsStatePtr = LOC_NET_AGPS_STATE_CLOSED;
    }
    else {
        LOC_LOGE("Unsupported event %d, type %d, state %d",
                event, agpsType, *agpsStatePtr);
    }
}
