/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Light;

import android.content.Intent;
import android.os.IBinder;
import android.os.Binder;
import android.os.RemoteException;
import android.os.ServiceManager;
import java.util.function.Supplier;

import android.hardware.light.HwLightState;
import android.hardware.light.ILights;
import android.hardware.light.LightType;
import android.hardware.light.FlashMode;
import android.hardware.light.BrightnessMode;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.model.HidlManager;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class LightService extends BaseService {
    private HidlManager hidlManager = null;
    private Supplier<ILights> mLightService = null;
    private ILights mVintfLights = null;

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi("onStartCommand");

        mLightService =  new VintfHalCache();
        if (mLightService == null) {
            LogUtils.loge( "No Light AIDL Service here");
        } else {
            mVintfLights = mLightService.get();
        }

        hidlManager = HidlManager.getInstance();
        if (hidlManager == null) {
            LogUtils.loge("No hidl manager found");
        }
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void register() { }

    @Override
    public int stop(TestCase testCase) {
        int type = Integer.valueOf(testCase.getParameter().get("type"));
        try {
            if (type != LightType.BACKLIGHT)
                setLight(testCase, false);
        } catch (Throwable t) {
            //catch NoClassDefFoundError if HIDL not exist
            LogUtils.logi( "NoClassDefFoundError occur!");
        }

        //disable charger before test light "rgb"
        if (type == LightType.NOTIFICATIONS) {
            if (hidlManager != null) {
                hidlManager.chargerEnable(true);
            }
        }
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        int type = Integer.valueOf(testCase.getParameter().get("type"));
        //disable charger before test light "rgb"
        if (type == LightType.NOTIFICATIONS) {
            if (hidlManager != null) {
                hidlManager.chargerEnable(false);
            }
        }

        setLight(testCase, true);
        if (mVintfLights != null) {
            updateView(testCase.getName(), this.getResources().getString(R.string.light_on));
        } else {
            updateView(testCase.getName(), this.getResources().getString(R.string.light_service_miss));
        }
        return 0;
    }

    private void setLight(TestCase testCase, boolean on) {

        int type = Integer.valueOf(testCase.getParameter().get("type"));
        String color = testCase.getParameter().get("color");
        try {
            LogUtils.logi("LightService service run for type:" + type);

            int colorValue = 0;
            if (on) {
                if (type == LightType.BUTTONS) {   //Button light test
                    colorValue = 0xFF020202;
                } else if (type == LightType.NOTIFICATIONS) {  // LED light test
                    if ("red".equalsIgnoreCase(color)){
                        colorValue = 0xFF0000;
                    } else if ("blue".equalsIgnoreCase(color)) {
                        colorValue = 0x0000FF;
                    } else if ("green".equalsIgnoreCase(color)) {
                        colorValue = 0x00FF00;
                    } else {
                        LogUtils.loge("LIGHT: Unknow LED color");
                    }
                } else if (type == LightType.BACKLIGHT) {
                    colorValue = 0xFFFFFF;
                }
            } else {
                colorValue = 0x00000000;
            }

            if (mVintfLights != null) {
                LogUtils.logi("LightService set light on for:" + type);
                HwLightState lightState = new HwLightState();
                lightState.color = colorValue;
                lightState.flashMode = (byte)FlashMode.NONE;
                lightState.flashOnMs = 0;
                lightState.flashOffMs = 0;
                lightState.brightnessMode = (byte) BrightnessMode.USER;
                mVintfLights.setLightState(type, lightState);
            }

        } catch (Exception e) {
            LogUtils.loge("Exception in light service" + e.toString());
            e.printStackTrace();
        } catch (Throwable t) {
            //catch NoClassDefFoundError if HIDL not exist
            LogUtils.loge("NoClassDefFoundError occur!");
        }
    }

    private static class VintfHalCache implements Supplier<ILights>, IBinder.DeathRecipient {
        private ILights mInstance = null;

        @Override
        public synchronized ILights get() {
            if (mInstance == null) {
                IBinder binder = Binder.allowBlocking(ServiceManager.waitForDeclaredService(
                        "android.hardware.light.ILights/default"));
                if (binder != null) {
                    mInstance = ILights.Stub.asInterface(binder);
                    try {
                        binder.linkToDeath(this, 0);
                    } catch (RemoteException e) {
                        LogUtils.loge("Unable to register DeathRecipient for " + mInstance);
                    }
                }
            }
            return mInstance;
        }

        @Override
        public synchronized void binderDied() {
            mInstance = null;
        }
    }
}
