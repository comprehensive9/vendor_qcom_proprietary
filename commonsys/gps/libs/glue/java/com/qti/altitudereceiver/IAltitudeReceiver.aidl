/* ======================================================================
*  Copyright (c) 2021 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.altitudereceiver;

import android.location.Location;
import android.app.PendingIntent;
import java.com.qti.altitudereceiver.IAltitudeReceiverResponseListener;

interface IAltitudeReceiver {

    boolean registerResponseListener(in IAltitudeReceiverResponseListener callback,
            in PendingIntent notifyIntent);

    boolean removeResponseListener(in IAltitudeReceiverResponseListener callback);

    void pushAltitude(in Location location);
}
