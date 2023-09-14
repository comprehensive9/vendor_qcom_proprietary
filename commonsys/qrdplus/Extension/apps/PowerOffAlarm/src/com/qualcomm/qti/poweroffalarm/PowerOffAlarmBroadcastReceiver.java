/*******************************************************************************
@file    PowerOffAlarmBroadcastReceiver.java
@brief   Receive "org.codeaurora.poweroffalarm.action.SET_ALARM" action to set
         power off alarm and receive "org.codeaurora.poweroffalarm.action.CANCEL_ALARM"
         action to cancel alarm.
---------------------------------------------------------------------------

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.poweroffalarm;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.util.Log;

import java.io.File;

public class PowerOffAlarmBroadcastReceiver extends BroadcastReceiver {
    private static final String TAG = "PowerOffAlarm";

    private static final String ACTION_SET_POWEROFF_ALARM =
            "org.codeaurora.poweroffalarm.action.SET_ALARM";

    private static final String ACTION_CANCEL_POWEROFF_ALARM =
            "org.codeaurora.poweroffalarm.action.CANCEL_ALARM";

    private static final String ACTION_UPDATE_ALARM_STATUS =
            "org.codeaurora.poweroffalarm.action.UPDATE_ALARM";

    private static final String ENCRYPTING_STATE = "trigger_restart_min_framework";

    private static final String PERSIST_ALARM_FOLDER = "/mnt/vendor/persist/alarm/";

    private static final String ALARM_BOOT_PROP = "ro.vendor.alarm_boot";

    private static final String STATUS = "status";

    private static final String TIME = "time";

    private static final String SNOOZE_TIME = "snooze_time";

    private static final int FAILURE = -1;

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, "Receive " + intent.getAction() + " intent. ");

        String action = intent.getAction();

        // Set or cancel power off alarm
        if (ACTION_SET_POWEROFF_ALARM.equals(action)) {
            long alarmTime = intent.getLongExtra(TIME,
                            PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
            long alarmInPref = PowerOffAlarmUtils.getAlarmFromPreference(context);

            Log.d(TAG, "Set power off alarm : alarm time " + alarmTime +
                    " time in pref " + alarmInPref);

            PowerOffAlarmUtils.saveAlarmToPreference(context, alarmTime);
            long alarmTimeToRtc = PowerOffAlarmUtils.setAlarmToRtc(alarmTime);
            if (alarmTimeToRtc != FAILURE) {
                PowerOffAlarmUtils.saveRtcAlarmToPreference(context, alarmTimeToRtc);
            }

        } else if (ACTION_CANCEL_POWEROFF_ALARM.equals(action)){
            long alarmTime = intent.getLongExtra(TIME,
                            PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
            long alarmInPref = PowerOffAlarmUtils.getAlarmFromPreference(context);

            if (alarmTime == alarmInPref) {
                PowerOffAlarmUtils.saveAlarmToPreference(context,
                                PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                PowerOffAlarmUtils.saveRtcAlarmToPreference(context,
                                PowerOffAlarmUtils.DEFAULT_ALARM_TIME);
                int rtc = PowerOffAlarmUtils.cancelAlarmInRtc();
                if (rtc < 0) {
                    Log.d(TAG, "Cancel alarm time in rtc failed ");
                }
            }
        }

    }

}
