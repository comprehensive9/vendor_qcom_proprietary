/*
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.smq.receiver;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;

import com.qualcomm.qti.smq.R;
import com.qualcomm.qti.smq.ui.QtiFeedbackActivity;
import com.qualcomm.qti.smq.ui.util.SMQUiUtils;

public class QtiNotificationReceiver extends BroadcastReceiver {
    private static final String LOG_TAG = "QCC-TR-UI";
    /*QDMA_UI Notification action and key-name*/
    public static final String QCC_UI_NOTI_ACTION = "com.qti.smq.Feedback.notification";
    private static final String QCC_UI_EXTRA_KEY_NAME = "bFirstBoot";
    private Context mContext;
    private final String notificationChannelID = "QCC_Noti_Channel";
    private static NotificationManager mNotificationManager;
    public static final int QCC_UI_NOTIFICATION_ID = Integer.MAX_VALUE - 100;
    private static final int QCC_UI_INTENT_REQ = Integer.MAX_VALUE - 100;
    private static String mSharedPreferences = "SharedPreferences";
    private static final String enabledModeKey = "enabledMode";

    //private static final long DURATION_TIMES = 86400000;    // one day
    private static final long DURATION_TIMES = 3600000;    // one hour
    //private static final long DURATION_TIMES = 180000;    // test 3min

    @Override
    public void onReceive(Context context, Intent intent) {

        if (! SMQUiUtils.isVendorEnhancedFramework())
		{
			Log.i(LOG_TAG, "isVendorEnhancedFramework - false");
            return;
		}

        Context sharedContext = context.createDeviceProtectedStorageContext();
        SharedPreferences enabledSet = sharedContext.getSharedPreferences(mSharedPreferences, Context.MODE_PRIVATE );
        int enabled = enabledSet.getInt(enabledModeKey, 1);
        Log.d(LOG_TAG,"QtiFeedbackActivity enabled : "+enabled);
        if(enabled == 0){
            return;
        }

        String act = intent.getAction();
        Log.i(LOG_TAG, "QtiNotificationReceiver... act : "+act);
        mContext = context;
        if (mNotificationManager == null) {
            mNotificationManager = (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);
        }
        if(QCC_UI_NOTI_ACTION.equals(act)){
            boolean bFirstBoot = intent.getBooleanExtra(QCC_UI_EXTRA_KEY_NAME, false);
            Log.i(LOG_TAG, "QtiNotificationReceiver / extra : "+bFirstBoot);
            if(bFirstBoot){
                notificationForQtiFeedback();
            }else{
                cancelNotificationForQtiFeedback();
            }
        }
    }

    private void notificationForQtiFeedback(){
        Log.i(LOG_TAG, "calling notificationForQtiFeedback");
        Intent notifyIntent = new Intent(mContext, QtiFeedbackActivity.class);
        notifyIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        PendingIntent notifyPendingIntent = PendingIntent.getActivity(mContext,QCC_UI_INTENT_REQ,notifyIntent,PendingIntent.FLAG_UPDATE_CURRENT);
        Notification.Builder noti =
                new Notification.Builder(mContext, notificationChannelID)
                        .setSmallIcon(R.drawable.ic_launcher)
                        .setContentTitle(mContext.getString(R.string.notification_title))
                        .setContentText(mContext.getString(R.string.notification_notice))
                        .setStyle(new Notification.BigTextStyle().bigText(mContext.getString(R.string.notification_notice)))
                        .setTimeoutAfter(DURATION_TIMES)
                        .setContentIntent(notifyPendingIntent);

        NotificationChannel channel = new NotificationChannel(
                notificationChannelID,
                mContext.getString(R.string.notification_title),
                NotificationManager.IMPORTANCE_HIGH);

        mNotificationManager.createNotificationChannel(channel);
        mNotificationManager.notify(QCC_UI_NOTIFICATION_ID, noti.build());
    }

    private void cancelNotificationForQtiFeedback(){
        Log.i(LOG_TAG, "calling cancelNotificationForQtiFeedback");
        mNotificationManager.cancel(QCC_UI_NOTIFICATION_ID);
    }
}
