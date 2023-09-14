/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#ifndef LOC_NET_IFACE_H
#define LOC_NET_IFACE_H

#include <functional>
#include <LocNetIfaceBase.h>
#include <QCMAP_Client.h>
#include <mutex>
#include <unordered_set>
#include "LocQmiWds.h"

using namespace std;

// Storing details of backhaul client requests
typedef unordered_set<string> ClientBackhaulRequest;

/*--------------------------------------------------------------------
 *  LE platform specific implementation for LocNetIface
 *-------------------------------------------------------------------*/
class LocNetIface : public LocNetIfaceBase {

public:
    /* Constructor */
    LocNetIface(LocNetConnType connType) :
        LocNetIfaceBase(connType), mQcmapClientPtr(NULL), mIsConnectReqSent(false),
        mIsConnectBackhaulPending(false), mIsDisconnectBackhaulPending(false),
        mLocNetBackHaulState(LOC_NET_CONN_STATE_INVALID),
        mLocNetBackHaulType(LOC_NET_CONN_TYPE_INVALID),
        mLocNetWlanState(LOC_NET_CONN_STATE_INVALID),
        mIsRoaming(false),
        mIsDsiInitDone(false), mDsiHandle(NULL), mIsDsiCallUp(false),
        mIsDsiStartCallPending(false), mIsDsiStopCallPending(false),
        mIsMobileApEnabled(false),
        mMutex() {}
        mSetRoaming(false),
        LocNetIface() : LocNetIface(LOC_NET_CONN_TYPE_WWAN_INTERNET),
        mLocQmiWds(LocQmiWds::getInstance()) {}

    /* Override base class pure virtual methods */
    bool setupWwanCall();
    bool stopWwanCall();
    void subscribe(const std::list<DataItemId>& itemListToSubscribe);
    void unsubscribe(const std::list<DataItemId>& itemListToUnsubscribe);
    void unsubscribeAll();
    void requestData(const std::list<DataItemId>& itemListToRequestData);

    /* Setup WWAN backhaul via QCMAP
     * This sets up IP routes as well for any AP socket */
    bool connectBackhaul(const string& clientName,
                         bool async = true, std::function<void (bool)> cb = nullptr);
    /* Disconnects the WWANbackhaul, only if it was setup by us */
    bool disconnectBackhaul(const string& clientName);

    /* APIs to fetch current WWAN status */
    bool isWwanConnected();
    /* APIs to fetch current Backhaul Network Interface status */
    bool isAnyBackHaulConnected();
    /* API to check if any non-metered backhaul type (eg: wifi, ethernet etc) status*/
    bool isNonMeteredBackHaulTypeConnected();
    /* API to check wwan roaming status */
    bool isWwanRoaming();
    qcmap_msgr_wwan_call_type_v01 getWwanCallType();

    recursive_mutex& getMutex(){ return mMutex; }

private:
    LocQmiWds* mLocQmiWds;
    /* QCMAP client handle
     * This will be set only for static sQcmapInstance. */
    QCMAP_Client* mQcmapClientPtr;

    /* Keep track of backhaul client requests */
    ClientBackhaulRequest mClientBackhaulReq;

    /* Flag to track whether we've setup QCMAP backhaul */
    bool mIsConnectReqSent;
    bool mIsConnectBackhaulPending;
    bool mIsDisconnectBackhaulPending;

    /* Maintain an exclusive instance for QCMAP interaction.
     * QCMAP does NOT support passing in/out a void user data pointer,
     * Hence we need to track the instance used internally. */
    static LocNetIface* sLocNetIfaceInstance;

    /* Current connection status */
    LocNetConnState mLocNetBackHaulState;
    /* Current Backhaul type include wwan, wifi, BT, USB cradle, Ethernet etc*/
    LocNetConnType  mLocNetBackHaulType;
    /* Check wifi hardware state */
    LocNetConnState mLocNetWlanState;
    /* Roaming status */
    bool mIsRoaming;
    /* Keep track of whether EnableMobileAP is done */
    bool mIsMobileApEnabled;

    // internal implementation for connectBackhaul
    bool connectBackhaulInternal(const string& clientName);
    /* Private APIs to interact with QCMAP module */
    void subscribeWithQcmap();
    void unsubscribeWithQcmap();
    void handleQcmapCallback(
            qcmap_msgr_wlan_status_ind_msg_v01 &wlanStatusIndData);
    void handleQcmapCallback(
            qcmap_msgr_bring_up_wwan_ind_msg_v01 &bringUpWwanIndData);
    void handleQcmapCallback(
            qcmap_msgr_tear_down_wwan_ind_msg_v01 &teardownWwanIndData);
#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
    void handleQcmapCallback(
            qcmap_msgr_backhaul_status_ind_msg_v01 &backhaulStatusIndData);
    void handleQcmapCallback(
            qcmap_msgr_wwan_roaming_status_ind_msg_v01 &roamingStatusIndData);
#else
    // Older Pls which does not have backhaul status and roaming status indications.
    void handleQcmapCallback(
            qcmap_msgr_station_mode_status_ind_msg_v01 &stationModeIndData);
    void handleQcmapCallback(
            qcmap_msgr_wwan_status_ind_msg_v01 &wwanStatusIndData);
#endif
    void notifyObserverForWlanStatus(bool isWlanEnabled);
    void notifyObserverForNetworkInfo(boolean isConnected, LocNetConnType connType);
    void notifyCurrentNetworkInfo(bool queryQcmap,
            LocNetConnType connType = LOC_NET_CONN_TYPE_INVALID);
    void notifyCurrentWifiHardwareState(bool queryQcmap);
#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
    void setCurrentBackHaulStatus(qcmap_msgr_backhaul_type_enum_v01  backhaulType,
            boolean backhaulIPv4Available, boolean backhaulIPv6Available);
#endif

    /* Callback registered with QCMAP */
    static void qcmapClientCallback
    (
      qmi_client_type user_handle,   /* QMI user handle. */
      unsigned int msg_id,           /* Indicator message ID. */
      void *ind_buf,                 /* Raw indication data. */
      unsigned int ind_buf_len,      /* Raw data length. */
      void *ind_cb_data              /* User callback handle. */
    );

    /* Data call setup specific members */
    bool mIsDsiInitDone;
    dsi_hndl_t mDsiHandle;
    bool mIsDsiCallUp;
    bool mIsDsiStartCallPending;
    bool mIsDsiStopCallPending;

    /* Callback registered with DSI */
    static void dsiNetEventCallback(
            dsi_hndl_t dsiHandle, void* userDataPtr, dsi_net_evt_t event,
            dsi_evt_payload_t* eventPayloadPtr);
    void handleDSCallback(dsi_hndl_t dsiHandle, bool isNetConnected);

    /* Mutex for synchronization */
    recursive_mutex mMutex;
};

#endif /* #ifndef LOC_NET_IFACE_H */
