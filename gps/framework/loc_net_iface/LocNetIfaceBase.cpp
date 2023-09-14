/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#define LOG_TAG "LocSvc_LocNetIfaceBase"

#include <LocNetIfaceBase.h>
#include <loc_pla.h>
#include <log_util.h>
#include <loc_cfg.h>

/* Data Item notify callback
 * Only one instance of LocNetIfaceBase can register this callback.
 * No support for multiple clients */
LocNetStatusChangeCb LocNetIfaceBase::sNotifyCb = NULL;
void* LocNetIfaceBase::sNotifyCbUserDataPtr = NULL;

void LocNetIfaceBase::registerWwanCallStatusCallback(
        LocWwanCallStatusCb wwanCallStatusCb, void* userDataPtr) {

    ENTRY_LOG();

    mWwanCallStatusCb = wwanCallStatusCb;
    mWwanCbUserDataPtr = userDataPtr;
}

void LocNetIfaceBase::registerDataItemNotifyCallback(
        LocNetStatusChangeCb callback, void* userDataPtr) {

    ENTRY_LOG();

    if (LocNetIfaceBase::sNotifyCb != NULL) {
        LOC_LOGE("Notify cb already registered !");
        return;
    }

    LocNetIfaceBase::sNotifyCb = callback;
    LocNetIfaceBase::sNotifyCbUserDataPtr = userDataPtr;
}

bool LocNetIfaceBase::updateSubscribedItemList(
        const std::list<DataItemId>& itemList, bool addOrDelete){

    ENTRY_LOG();
    bool anyUpdatesToList = false;

    /* Scroll through specified item list */
    std::list<DataItemId>::const_iterator it = itemList.begin();
    for (; it != itemList.end(); it++) {

        DataItemId itemId = *it;

        bool itemInSubscribedList = isItemSubscribed(itemId);

        /* Request to add */
        if (addOrDelete == true && !itemInSubscribedList) {

            mSubscribedItemList.push_back(itemId);
            anyUpdatesToList = true;

        } else if (addOrDelete == false && itemInSubscribedList) {
            /* Request to delete */
            mSubscribedItemList.erase(
                    std::remove(
                            mSubscribedItemList.begin(),
                            mSubscribedItemList.end(), itemId),
                            mSubscribedItemList.end());
            anyUpdatesToList = true;
        }
    }

    return anyUpdatesToList;
}

char* LocNetIfaceBase::getApnNameFromConfig(){

    return mApnName;
}

LocNetConnIpType LocNetIfaceBase::getIpTypeFromConfig(){

    /* Convert config value to LocNetConnIpType */
    if (mIpType == 4) {
        return LOC_NET_CONN_IP_TYPE_V4;
    } else if (mIpType == 6) {
        return LOC_NET_CONN_IP_TYPE_V6;
    } else if (mIpType == 10) {
        return LOC_NET_CONN_IP_TYPE_V4V6;
    }
    return LOC_NET_CONN_IP_TYPE_INVALID;
}

void LocNetIfaceBase::fetchConfigItems(){

    ENTRY_LOG();

    /* Fetch config items from gps.conf */
    if (mLocNetConnType == LOC_NET_CONN_TYPE_WWAN_INTERNET) {
        loc_param_s_type confItemsToFetchArray[] = {
                { "INTERNET_APN",     &mApnName, NULL, 's' },
                { "INTERNET_IP_TYPE", &mIpType,  NULL, 'n' } };
        UTIL_READ_CONF(LOC_PATH_GPS_CONF, confItemsToFetchArray);

    } else if (mLocNetConnType == LOC_NET_CONN_TYPE_WWAN_SUPL) {
        loc_param_s_type confItemsToFetchArray[] = {
                { "SUPL_APN",     &mApnName, NULL, 's' },
                { "SUPL_IP_TYPE", &mIpType,  NULL, 'n' } };
        UTIL_READ_CONF(LOC_PATH_GPS_CONF, confItemsToFetchArray);

    } else {
        LOC_LOGE("Invalid connType %d", mLocNetConnType);
    }
}
