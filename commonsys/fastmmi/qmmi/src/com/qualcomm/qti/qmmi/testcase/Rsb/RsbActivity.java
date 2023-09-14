/*
 * Copyright (c) 2017,2021, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Rsb;

import android.app.ActivityManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.ShellUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import android.content.pm.PackageManager;

import com.android.internal.widget.LockPatternUtils;

public class RsbActivity extends BaseActivity {

    List<String> mKeys = new ArrayList<>(0);

    private HashMap<Integer, String> mKeyMap = new HashMap<Integer, String>();

    {
        mKeyMap.put(KeyEvent.KEYCODE_VOLUME_DOWN, "volumedown");
        mKeyMap.put(KeyEvent.KEYCODE_VOLUME_UP, "volumeup");
    }

    /**
     * Can't directly add into TestCase.TestData.
     * When test power/home key, will call BaseActivity.onResume() and will clear TestCase.TestData.
     */
    private Map<String, String> mKeysData = new HashMap<String, String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        int i = 0;
        for (Integer in : mKeyMap.keySet()) {
            mKeysData.put(mKeyMap.get(in), "not detected");
        }
        Utils.parseCases(mTestCase.getParameter().get("rsb"), mKeys);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.rsb_act;
    }

    @Override
    protected void onResume() {

        super.onResume();
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.assertTrueButton || view.getId() == R.id.assertFalseButton){
            //Write TestCase.TestData
            for (String key : mKeysData.keySet()) {
                mTestCase.addTestData(key, mKeysData.get(key));
                LogUtils.logi( "TestData: " + key + " : " + mKeysData.get(key));
            }
            super.onClick(view);
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // TODO Auto-generated method stub
        LogUtils.logi( "dispatchKeyEvent -- keyCode:" + event.getKeyCode());
        TextView keyText = null;
        switch (event.getKeyCode()) {
            case KeyEvent.KEYCODE_VOLUME_DOWN:
                keyText = (TextView) findViewById(R.id.volume_down);
                break;
            case KeyEvent.KEYCODE_VOLUME_UP:
                keyText = (TextView) findViewById(R.id.volume_up);
                break;
        }

        if ( null != keyText) {
            setKeyPass(keyText);
            mKeysData.put(mKeyMap.get(event.getKeyCode()), "detected");
            return true;
        }

        return super.dispatchKeyEvent(event);
    }

    private void setKeyPass(TextView keyText) {
        if (null != keyText) {
            keyText.setBackgroundResource(R.drawable.pass);
        }
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
    }

    @Override
    protected void onStop() {
        // TODO Auto-generated method stub
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
    }

}
