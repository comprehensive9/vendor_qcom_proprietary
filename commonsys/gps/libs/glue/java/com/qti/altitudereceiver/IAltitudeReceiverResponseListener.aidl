/* ======================================================================
*  Copyright (c) 2021 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.altitudereceiver;

import android.location.Location;

oneway interface IAltitudeReceiverResponseListener {
    void onAltitudeLookupRequest(in Location location, in boolean isEmergency);
}
