/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.smq.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;

import com.qualcomm.qti.smq.ui.util.SMQUiUtils;

public class EnablementReceiver extends BroadcastReceiver {
    /** The Constant LOG_TAG. */
    private static final String LOG_TAG = "QCC-TR-UI";
    private static final String QCC_UI_ENABLED_ACTION = "com.qti.smq.Feedback.ENABLED";
    private static final String QCC_UI_DISABLED_ACTION = "com.qti.smq.Feedback.DISABLED";
    private Context mContext;
    private static String mSharedPreferences = "SharedPreferences";
    private static final String enabledModeKey = "enabledMode";
    @Override
    public void onReceive(Context context, Intent intent) {
        if (! SMQUiUtils.isVendorEnhancedFramework())
        {
            Log.i(LOG_TAG, "isVendorEnhancedFramework - false");
            return;
        }

        String act = intent.getAction();
        Log.i(LOG_TAG, "EnablementReceiver... act : "+act);
        mContext = context;
        if(intent == null || intent.getAction() == null) {
            return;
        }
        Context sharedContext = context.createDeviceProtectedStorageContext();
        SharedPreferences enabledSet = sharedContext.getSharedPreferences(mSharedPreferences, Context.MODE_PRIVATE );
        int enabledMode = 1;
        if(QCC_UI_ENABLED_ACTION.equals(act)){
            Log.i(LOG_TAG, "EnablementReceiver... received : "+act);
            enabledMode = 1;
        }else if(QCC_UI_DISABLED_ACTION.equals(act)){
            Log.i(LOG_TAG, "EnablementReceiver... received : "+act);
            enabledMode = 0;
        }
        SharedPreferences.Editor editor = enabledSet.edit();
        editor.putInt(enabledModeKey, enabledMode);
        boolean ret = editor.commit();
        Log.i(LOG_TAG, "stored current enabledSet : "+enabledMode+" , commit : "+ret);
    }
}
