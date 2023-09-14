/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.wifidbreceiver;

import java.com.qti.wifidbreceiver.APInfo;
import java.com.qti.wifidbreceiver.APInfoExt;
import java.util.List;
import android.location.Location;

interface IWiFiDBReceiverResponseListener {

    // Decprecated, for backwards compatibility with static linked SDK
    oneway void onAPListAvailable(in List<APInfo> apInfoList);

    oneway void onStatusUpdate(in boolean isSuccess, in String error);

    oneway void onServiceRequest();

    oneway void onAPListAvailableExt(in List<APInfoExt> apInfoList, in int apStatus, in Location location);
    oneway void onLookupRequest(in List<APInfoExt> apInfoList, in Location location);
    boolean onServiceRequestES(in boolean isEmergency);
    boolean onLookupRequestES(in List<APInfoExt> apInfoList, in Location location, in boolean isEmergency);
}
