/*
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.smq.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import com.qualcomm.qti.smq.ui.util.SMQUiUtils;

public class BootReceiver extends BroadcastReceiver {
    /** The Constant LOG_TAG. */
    private static final String LOG_TAG = "QCC-TR-UI";
    private QtiNotificationReceiver mQtiNotificationReceiver;
    @Override
    public void onReceive(Context context, Intent intent) {

        if (! SMQUiUtils.isVendorEnhancedFramework())
            return;

        Log.d(LOG_TAG,"BootReceiver");
        if(intent == null || intent.getAction() == null) {
            return;
        }
        String action = intent.getAction();
        if( Intent.ACTION_BOOT_COMPLETED.equals(action)
                || Intent.ACTION_LOCKED_BOOT_COMPLETED.equals(action))
        {
            Log.d(LOG_TAG, action + " intent received");

            //registerReceiver
            if(Intent.ACTION_LOCKED_BOOT_COMPLETED.equals(action)){
                Log.d(LOG_TAG, "bootReceiver : registerReceiver for QtiNotificationRecv");
                mQtiNotificationReceiver = new QtiNotificationReceiver();
                context.getApplicationContext().registerReceiver(mQtiNotificationReceiver, new IntentFilter(QtiNotificationReceiver.QCC_UI_NOTI_ACTION));
            }
        }
    }
}
