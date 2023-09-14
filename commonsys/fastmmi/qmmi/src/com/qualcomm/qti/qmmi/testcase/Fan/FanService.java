/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Fan;

import android.content.Intent;
import android.os.SystemProperties;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.R;

public class FanService extends BaseService {

    private static final String FAN_TURN_ON = "fan_control_turn_on";
    private static final String FAN_TURN_OFF = "fan_control_turn_off";

    public int onStartCommand(Intent intent, int flags, int startId) {
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void register() {
    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        String fanAction = testCase.getParameter().get("init_service");

        try {
            switch(fanAction.toLowerCase()){
                case FAN_TURN_ON:
                    SystemProperties.set("ctl.start", fanAction);
                    updateView(testCase.getName(), this.getResources().getString(R.string.fan_turn_on_and_check));
                    LogUtils.logi("FAN TURN ON case");
                    break;
                case FAN_TURN_OFF:
                    SystemProperties.set("ctl.start", fanAction);
                    updateView(testCase.getName(), this.getResources().getString(R.string.fan_turn_off_and_check));
                    LogUtils.logi("FAN TURN OFF case");
                    break;
                default:
                    updateView(testCase.getName(), this.getResources().getString(R.string.fan_wrong_params));
            }
        } catch (RuntimeException e) {
            updateView(testCase.getName(), this.getResources().getString(R.string.fan_internal_error));
            LogUtils.loge("RuntimeException while start init service");
            e.printStackTrace();
        }

        return 0;
    }
}
