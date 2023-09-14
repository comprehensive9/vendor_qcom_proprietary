/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.wifidbprovider;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.Binder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.HashMap;

import com.qti.wifidbprovider.*;
import com.qti.wwandbreceiver.*;
import com.qualcomm.location.hidlclient.BaseHidlClient;
import com.qualcomm.location.hidlclient.HidlClientUtils;
import com.qualcomm.location.izat.GTPClientHelper;
import com.qualcomm.location.izat.IzatService;

import vendor.qti.gnss.V2_1.ILocHidlGnss;
import vendor.qti.gnss.V2_1.ILocHidlWiFiDBProvider;
import vendor.qti.gnss.V2_1.ILocHidlWiFiDBProviderCallback;
import vendor.qti.gnss.V2_1.LocHidlWifiDBListStatus;

public class WiFiDBProvider implements IzatService.ISystemEventListener {
    private static final String TAG = "WiFiDBProvider";
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);

    private static final Object sCallBacksLock = new Object();
    private final Context mContext;

    private IWiFiDBProviderResponseListener mWiFiDBProviderResponseListener = null;

    private PendingIntent mListenerIntent = null;
    private WiFiDBProviderHidlClient mHidlClient = null;
    private String mPackageName;

    private static WiFiDBProvider sInstance = null;
    public static WiFiDBProvider getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new WiFiDBProvider(ctx);
        }
        return sInstance;
    }

    private WiFiDBProvider(Context ctx) {
        if (VERBOSE_DBG) {
            Log.d(TAG, "WiFiDBProvider construction");
        }

        mHidlClient = new WiFiDBProviderHidlClient(this);
        if (null == mHidlClient) {
            Log.e(TAG, "WiFiDBProvider construction fail: " + mHidlClient);
        }

        mContext = ctx;
        IzatService.AidlClientDeathNotifier.getInstance().registerAidlClientDeathCb(this);
    }

    @Override
    public void onAidlClientDied(String packageName) {
        if (mPackageName != null && mPackageName.equals(packageName)) {
            Log.d(TAG, "aidl client crash: " + packageName);
            synchronized (sCallBacksLock) {
                mWiFiDBProviderResponseListener = null;
            }
        }
    }

    /* Remote binder */
    private final IWiFiDBProvider.Stub mBinder = new IWiFiDBProvider.Stub() {

        public boolean registerResponseListener(final IWiFiDBProviderResponseListener callback,
                                                PendingIntent notifyIntent) {
            if (VERBOSE_DBG) {
                Log.d(TAG, "WiFiDBProvider registerResponseListener");
            }

            if (callback == null) {
                Log.e(TAG, "callback is null");
                return false;
            }

            if (notifyIntent == null) {
                Log.w(TAG, "notifyIntent is null");
            }

            if (null != mWiFiDBProviderResponseListener) {
                Log.e(TAG, "Response listener already provided.");
                return false;
            }

            synchronized (sCallBacksLock) {
                mWiFiDBProviderResponseListener = callback;
                mListenerIntent = notifyIntent;
            }

            mPackageName = mContext.getPackageManager().getNameForUid(Binder.getCallingUid());
            if (VERBOSE_DBG) {
                Log.d(TAG, "WiFiDBProvider GTPClientHelper.SetClientRegistrationStatus IN");
            }
            GTPClientHelper.SetClientRegistrationStatus(mContext,
                    GTPClientHelper.GTP_CLIENT_WIFI_PROVIDER, notifyIntent, true);

            return true;
        }

        public void removeResponseListener(final IWiFiDBProviderResponseListener callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }
            if (VERBOSE_DBG) {
                Log.d(TAG, "WiFiDBProvider GTPClientHelper.SetClientRegistrationStatus OUT");
            }
            GTPClientHelper.SetClientRegistrationStatus(mContext,
                    GTPClientHelper.GTP_CLIENT_WIFI_PROVIDER, mListenerIntent, false);
            synchronized (sCallBacksLock) {
                mWiFiDBProviderResponseListener = null;
                mListenerIntent = null;
            }
            mPackageName = null;
        }


        public void requestAPObsLocData() {
            if (VERBOSE_DBG) {
                Log.d(TAG, "in IWiFiDBProvider.Stub(): requestAPObsLocData()");
            }

            mHidlClient.requestApObsData();
        }

    };

    private void onApObsLocDataAvailable(List<APObsLocData> obsDataList, int status) {
        if (VERBOSE_DBG) {
            Log.d(TAG, "onApObsLocDataAvailable status: " + status);
        }

        Log.d(TAG, "onApObsLocDataAvailable status: " + status);
        Log.d(TAG, "onApObsLocDataAvailable obsDataList size: " + obsDataList.size());
        synchronized (sCallBacksLock) {
            if (null != mWiFiDBProviderResponseListener) {
                try {
                    mWiFiDBProviderResponseListener.onApObsLocDataAvailable(obsDataList, status);
                } catch (RemoteException e) {
                    Log.w(TAG, "onApObsLocDataAvailable remote exception, sending intent");
                    GTPClientHelper.SendPendingIntent(mContext, mListenerIntent, "WiFiDBProvider");
                }
            }
        }
    }

    private void onServiceRequest() {
        if (VERBOSE_DBG) {
            Log.d(TAG, "onServiceRequest");
        }
        synchronized (sCallBacksLock) {
            if (null != mWiFiDBProviderResponseListener) {
                try {
                    mWiFiDBProviderResponseListener.onServiceRequest();
                } catch (RemoteException e) {
                    Log.w(TAG, "onServiceRequest remote exception, sending intent");
                    GTPClientHelper.SendPendingIntent(mContext, mListenerIntent, "WiFiDBProvider");
                }
            }
        }
    }

    public IWiFiDBProvider getWiFiDBProviderBinder() {
        return mBinder;
    }

    // ======================================================================
    // HIDL client
    // ======================================================================
    static class WiFiDBProviderHidlClient extends BaseHidlClient
            implements BaseHidlClient.IServiceDeathCb {
        private final String TAG = "WiFiDBProviderHidlClient";
        private final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

        private LocHidlWiFiDBProviderCallback mLocHidlWiFiDBProvicerCallback;
        private ILocHidlWiFiDBProvider mLocHidlWiFiDBProvider;



        public WiFiDBProviderHidlClient(WiFiDBProvider provider) {
            getWiFiDBProviderIface();
            mLocHidlWiFiDBProvicerCallback = new LocHidlWiFiDBProviderCallback(provider);

            try {
                mLocHidlWiFiDBProvider.init(mLocHidlWiFiDBProvicerCallback);
                mLocHidlWiFiDBProvider.registerWiFiDBProvider(mLocHidlWiFiDBProvicerCallback);
                registerServiceDiedCb(this);
            } catch (RemoteException e) {
                Log.e(TAG, "Exception on provider init: " + e);
            }
        }

        void getWiFiDBProviderIface() {
            Log.i(TAG, "getWiFiDBProviderIface");
            ILocHidlGnss gnssService = (vendor.qti.gnss.V2_1.ILocHidlGnss) getGnssService();

            if (null != gnssService) {
                try {
                    mLocHidlWiFiDBProvider = gnssService.getExtensionLocHidlWiFiDBProvider();
                } catch (RemoteException e) {
                    throw new RuntimeException("Exception getting wifidb provider: " + e);
                }
            } else {
                throw new RuntimeException("gnssService is null!");
            }
        }

        @Override
        public void onServiceDied() {
            mLocHidlWiFiDBProvider = null;
            getWiFiDBProviderIface();
            try {
                mLocHidlWiFiDBProvider.init(mLocHidlWiFiDBProvicerCallback);
                mLocHidlWiFiDBProvider.registerWiFiDBProvider(mLocHidlWiFiDBProvicerCallback);
            } catch (RemoteException e) {
                Log.e(TAG, "Exception on provider init: " + e);
            }
        }

        public void requestApObsData() {
            if (null != mLocHidlWiFiDBProvider) {
                try {
                    mLocHidlWiFiDBProvider.sendAPObsLocDataRequest();
                } catch (RemoteException e) {
                    throw new RuntimeException("Exception on sendAPObsLocDataRequest: " + e);
                }
            } else {
                throw new RuntimeException("mLocHidlWiFiDBProvider is null!");
            }
        }

        // ======================================================================
        // Callbacks
        // ======================================================================

        class LocHidlWiFiDBProviderCallback extends ILocHidlWiFiDBProviderCallback.Stub {

            private WiFiDBProvider mWiFiDBProvider;

            private LocHidlWiFiDBProviderCallback(WiFiDBProvider wiFiDBProvider) {
                mWiFiDBProvider = wiFiDBProvider;
            }

            public void attachVmOnCallback() {
                // ???
            }

            public void serviceRequestCallback() {
                mWiFiDBProvider.onServiceRequest();
            }

            public void apObsLocDataUpdateCallback(ArrayList<LocHidlApObsData> apObsLocDataList,
                    int apObsLocDataListSize, int apListStatus) {

                ArrayList<APObsLocData> aPObsLocDataList = new ArrayList<APObsLocData>();

                for (LocHidlApObsData apObsData: apObsLocDataList) {
                    APObsLocData apObsLocData = new APObsLocData();
                    apObsLocData.mScanTimestamp = (int) (apObsData.scanTimestamp_ms / 1000);
                    apObsLocData.mLocation = HidlClientUtils.translateHidlLocation(
                            apObsData.gpsLoc.gpsLocation);

                    BSInfo bsInfo = new BSInfo();
                    bsInfo.mCellType = HidlClientUtils.RiltoIZatCellTypes(
                            apObsData.cellInfo.cell_type);
                    bsInfo.mCellRegionID1 = apObsData.cellInfo.cell_id1;
                    bsInfo.mCellRegionID2 = apObsData.cellInfo.cell_id2;
                    bsInfo.mCellRegionID3 = apObsData.cellInfo.cell_id3;
                    bsInfo.mCellRegionID4 = apObsData.cellInfo.cell_id4;

                    apObsLocData.mCellInfo = bsInfo;

                    ArrayList<APScan> apScanList = new ArrayList<APScan>();
                    for (LocHidlApScanData apHidlScan: apObsData.ap_scan_info) {
                        APScan apScan = new APScan();
                        apScan.mMacAddress = HidlClientUtils.longMacToHex(apHidlScan.mac_R48b);
                        apScan.mRssi = apHidlScan.rssi;
                        apScan.mDeltaTime = (int) apHidlScan.age_usec;
                        apScan.mChannelNumber = apHidlScan.channel_id;
                        apScan.mSSID = apHidlScan.ssid.getBytes();;

                        apScanList.add(apScan);
                    }

                    APScan[] apScanArray = new APScan[apScanList.size()];
                    apScanList.toArray(apScanArray);
                    apObsLocData.mScanList = apScanArray;

                    aPObsLocDataList.add(apObsLocData);
                }

                mWiFiDBProvider.onApObsLocDataAvailable(aPObsLocDataList, apListStatus);
            }
        }
    }
}
