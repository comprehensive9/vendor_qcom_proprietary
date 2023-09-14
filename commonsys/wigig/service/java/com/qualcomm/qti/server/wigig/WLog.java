/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.server.wigig;

import android.content.ContentResolver;
import android.content.Context;
import android.provider.Settings;
import android.util.Log;

/* WIGIG verbosity printing log */
final public class WLog {
    public static final String TAG = "WLog";

    private static boolean mVerboseLoggingEnabled = false;

    private WLog() {
    }

    public static void setVerboseLogging(Context context) {
        boolean oldVerboseLoggingEnabled = mVerboseLoggingEnabled;
        if (Settings.Global.getInt(context.getContentResolver(),
                                   Settings.Global.WIFI_VERBOSE_LOGGING_ENABLED,
                                   0) > 0){
            mVerboseLoggingEnabled = true;
        } else{
            mVerboseLoggingEnabled = false;
        }

        if (mVerboseLoggingEnabled != oldVerboseLoggingEnabled) {
            Log.v(TAG, "WIGIG verbose logging was set to " + mVerboseLoggingEnabled);
        }
    }

    public static void v(String tag, String text) {
        if (mVerboseLoggingEnabled) {
                Log.i(tag, text);
        }
    }
}
