/* ==============================================================================
 * QtiWifiServiceiImpl.java
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.qti.server.qtiwifi;

import android.content.Context;
import android.os.Binder;
import android.os.IBinder;
import android.os.SystemProperties;
import android.util.Log;
import android.net.wifi.WifiManager;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;

import com.qualcomm.qti.qtiwifi.ICsiCallback;
import com.qualcomm.qti.qtiwifi.IQtiWifiManager;

public final class QtiWifiServiceImpl extends IQtiWifiManager.Stub {
    private static final String TAG = "QtiWifiServiceImpl";
    private static final boolean DBG = true;
    private boolean mServiceStarted = false;
    private boolean mInitializeHals = false;
    private WifiManager mWifiManager;

    private final Context mContext;
    private Context mServiceContext = null;
    private Object mLock = new Object();
    private final IntentFilter mQtiIntentFilter;

    QtiWifiCsiHal qtiWifiCsiHal;
    QtiSupplicantStaIfaceHal qtiSupplicantStaIfaceHal;

    public QtiWifiServiceImpl(Context context) {
        Log.d(TAG, "QtiWifiServiceImpl ctor");
        mContext = context;
        if (mServiceStarted == false) {
            mServiceContext = context;
            Intent serviceIntent = new Intent(context, QtiWifiService.class);
            context.startForegroundService(serviceIntent);
            Log.d(TAG, "QtiWifiService has started");
            mServiceStarted = true;
        }
        mQtiIntentFilter = new IntentFilter("android.net.wifi.supplicant.STATE_CHANGE");
        mQtiIntentFilter.addAction("android.net.wifi.WIFI_STATE_CHANGED");
        mContext.registerReceiver(mQtiReceiver, mQtiIntentFilter);
        mWifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        if (mWifiManager.isWifiEnabled()) {
            Log.d(TAG, "isWifiEnabled true");
            mInitializeHals = true;
            checkAndInitCfrHal();
            checkAndInitSupplicantStaIfaceHal();
        }
    }

    protected void destroyService() {
        Log.d(TAG, "destroyService()");
        mServiceStarted = false;
    }

    public void checkAndInitCfrHal() {
        Log.i(TAG, "checkAndInitCfrHal");
        qtiWifiCsiHal = new QtiWifiCsiHal();
        qtiWifiCsiHal.initialize();
    }

    public void checkAndInitSupplicantStaIfaceHal() {
        Log.i(TAG, "checkAndInitSupplicantStaIfaceHal");
        qtiSupplicantStaIfaceHal = new QtiSupplicantStaIfaceHal();
        qtiSupplicantStaIfaceHal.initialize();
        if (!qtiSupplicantStaIfaceHal.vendor_setupIface("wlan0")) {
            Log.e(TAG, "Failed to setup iface in supplicant on wlan0");
        }
    }

    private final BroadcastReceiver mQtiReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)) {
                 int state = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN);
                 if ((state == WifiManager.WIFI_STATE_ENABLED) && !mInitializeHals) {
                     Log.i(TAG, "Didn't iniltailze the hals, now initializing");
                     checkAndInitCfrHal();
                     checkAndInitSupplicantStaIfaceHal();
                 } else if (state == WifiManager.WIFI_STATE_DISABLED) {
                     mInitializeHals = false;
                 }
            }
        }
    };

    //@Override
    public void registerCsiCallback(IBinder binder, ICsiCallback callback,
            int callbackIdentifier) {
        // verify arguments
        if (binder == null) {
            throw new IllegalArgumentException("Binder must not be null");
        }
        if (callback == null) {
            throw new IllegalArgumentException("Callback must not be null");
        }
        enforceAccessPermission();
        if (DBG) {
            Log.i(TAG, "registerCsiCallback uid=%" + Binder.getCallingUid());
        }
        qtiWifiCsiHal.registerCsiCallback(binder, callback, callbackIdentifier);
    }

    //@Override
    public void unregisterCsiCallback(int callbackIdentifier) {
        enforceAccessPermission();
        if (DBG) {
            Log.i(TAG, "unregisterCsiCallback uid=%" + Binder.getCallingUid());
        }
        qtiWifiCsiHal.unregisterCsiCallback(callbackIdentifier);
    }

    @Override
    public void doDriverCmd(String command)
    {
        qtiSupplicantStaIfaceHal.doDriverCmd(command);
        return;
    }

    /**
     * see {@link com.qualcomm.qti.qtiwifi.QtiWifiManager#startCsi}
     */
    public void startCsi() {
        enforceChangePermission();
        Log.i(TAG, "startCsi");
        qtiWifiCsiHal.startCsi();
    }

    /**
     * see {@link com.qualcomm.qti.qtiwifi.QtiWifiManager#stopCsi}
     */
    public void stopCsi() {
        enforceChangePermission();
        Log.i(TAG, "stopCsi");
        qtiWifiCsiHal.stopCsi();
    }

    private void enforceAccessPermission() {
        mContext.enforceCallingOrSelfPermission(android.Manifest.permission.ACCESS_WIFI_STATE,
                "QtiWifiServiceImpl");
    }

    private void enforceChangePermission() {
        mContext.enforceCallingOrSelfPermission(android.Manifest.permission.CHANGE_WIFI_STATE,
                "QtiWifiServiceImpl");
    }
}
