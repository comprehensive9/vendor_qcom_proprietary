/* ==============================================================================
 * QtiWifiService.java
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.qti.server.qtiwifi;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.res.Resources;
import android.os.Build;
import android.os.Process;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import com.qualcomm.qti.qtiwifi.IQtiWifiManager;

public final class QtiWifiService extends Service {
    private static final String TAG = "QtiWifiService";
    private static final boolean DBG = true;
    private IQtiWifiManager.Stub mBinder;

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand()");

        Notification.Builder nb = new Notification.Builder(
                getApplicationContext())
            .setContentTitle(getResources().getString(R.string.notif_title))
            .setContentText(getResources().getString(R.string.notif_text))
            .setSmallIcon(R.mipmap.icon);
            nb.setChannelId(createNotificationChannel(TAG,
                    getResources().getString(R.string.channel_name))
                    .getId());
        startForeground(Process.myPid(), nb.build());
        return super.onStartCommand(intent, flags, startId);
    }

    private NotificationChannel createNotificationChannel(
            String channelId, String channelName){
        Log.d(TAG, "createNotificationChannel called");
        NotificationChannel channel = new NotificationChannel(channelId,
                channelName, NotificationManager.IMPORTANCE_NONE);
        NotificationManager service = (NotificationManager)
            getSystemService(Context.NOTIFICATION_SERVICE);
        service.createNotificationChannel(channel);
        return channel;
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "onBind()");
        if (mBinder == null) {
            Context c = getApplicationContext();
            Log.d(TAG, "Creating QtiWifiServiceImpl with context:" + c);
            mBinder = new QtiWifiServiceImpl(c);
        }
        return mBinder;
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy()");
        super.onDestroy();
        if (mBinder != null && mBinder instanceof QtiWifiServiceImpl) {
            Log.d(TAG, "Unregister callbacks");
            ((QtiWifiServiceImpl) mBinder).destroyService();
            mBinder = null;
        }
    }
}
