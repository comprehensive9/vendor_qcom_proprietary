/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.wwandbprovider;

import android.app.PendingIntent;
import android.content.Context;
import android.location.Location;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.os.Binder;
import android.util.Log;

import com.qualcomm.location.izat.GTPClientHelper;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import com.qti.wwandbprovider.*;

import com.qti.wwandbprovider.*;
import com.qti.wwandbreceiver.BSInfo;
import com.qualcomm.location.hidlclient.BaseHidlClient;
import com.qualcomm.location.hidlclient.HidlClientUtils;
import com.qualcomm.location.izat.IzatService;
import vendor.qti.gnss.V2_1.ILocHidlGnss;
import vendor.qti.gnss.V2_1.ILocHidlWWANDBProvider;
import vendor.qti.gnss.V2_1.ILocHidlWWANDBProviderCallback;
import vendor.qti.gnss.V2_1.ILocHidlWWANDBProviderCallback.LocHidlBSObsData;
import vendor.qti.gnss.V2_1.ILocHidlWWANDBProviderCallback.LocHidlBsCellInfo;
import vendor.qti.gnss.V2_1.LocHidlUlpLocation;

public class WWANDBProvider implements IzatService.ISystemEventListener {
    private static final String TAG = "WWANDBProvider";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    private static final Object sCallBacksLock = new Object();
    private final Context mContext;

    private IWWANDBProviderResponseListener mWWANDBProviderResponseListener = null;

    private PendingIntent mListenerIntent = null;
    private WWANDBProviderHidlClient mHidlClient = null;

    private String mPackageName;

    private static WWANDBProvider sInstance = null;
    public static WWANDBProvider getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new WWANDBProvider(ctx);
        }
        return sInstance;
    }

    private WWANDBProvider(Context ctx) {
        if (VERBOSE) {
            Log.d(TAG, "WWANDBProvider construction");
        }
        mContext = ctx;
        mHidlClient = new WWANDBProviderHidlClient(this);
        if (null == mHidlClient) {
            Log.e(TAG, "WWANDBProvider construction fail: " + mHidlClient);
        }
        IzatService.AidlClientDeathNotifier.getInstance().registerAidlClientDeathCb(this);
    }

    @Override
    public void onAidlClientDied(String packageName) {
        if (mPackageName != null && mPackageName.equals(packageName)) {
            Log.d(TAG, "aidl client crash: " + packageName);
            synchronized (sCallBacksLock) {
                mWWANDBProviderResponseListener = null;
            }
        }
    }

    /* Remote binder */
    private final IWWANDBProvider.Stub mBinder = new IWWANDBProvider.Stub() {

        public boolean registerResponseListener(final IWWANDBProviderResponseListener callback,
                                                PendingIntent notifyIntent) {
            if (VERBOSE) {
                Log.d(TAG, "WWANDBProvider registerResponseListener");
            }

            if (callback == null) {
                Log.e(TAG, "callback is null");
                return false;
            }

            if (notifyIntent == null) {
                Log.w(TAG, "notifyIntent is null");
            }

            if (null != mWWANDBProviderResponseListener) {
                Log.e(TAG, "Response listener already provided.");
                return false;
            }

            synchronized (sCallBacksLock) {
                mWWANDBProviderResponseListener = callback;
                mListenerIntent = notifyIntent;
            }

            mPackageName = mContext.getPackageManager().getNameForUid(Binder.getCallingUid());

            if (VERBOSE) {
                Log.d(TAG, "WWANDBProvider GTPClientHelper.SetClientRegistrationStatus IN");
            }
            GTPClientHelper.SetClientRegistrationStatus(mContext,
                    GTPClientHelper.GTP_CLIENT_WWAN_PROVIDER, notifyIntent, true);

            return true;
        }

        public void removeResponseListener(final IWWANDBProviderResponseListener callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }
            if (VERBOSE) {
                Log.d(TAG, "WWANDBProvider GTPClientHelper.SetClientRegistrationStatus OUT");
            }
            GTPClientHelper.SetClientRegistrationStatus(mContext,
                    GTPClientHelper.GTP_CLIENT_WWAN_PROVIDER, mListenerIntent, false);
            synchronized (sCallBacksLock) {
                mWWANDBProviderResponseListener = null;
                mListenerIntent = null;
            }
            mPackageName = null;
        }


        public int requestBSObsLocData() {
            if (VERBOSE) {
                Log.d(TAG, "in IWWANDBProvider.Stub(): requestBSObsLocData()");
            }

            mHidlClient.requestBSObsLocData();
            return 0;
        }
    };

    private void onBsObsLocDataAvailable(ArrayList<BSObsLocationData> obsDataList, int status) {
        if (VERBOSE) {
            Log.d(TAG, "onBsObsLocDataAvailable status: " + status);
        }

        synchronized (sCallBacksLock) {
            if (null != mWWANDBProviderResponseListener) {
                try {
                    mWWANDBProviderResponseListener.onBsObsLocDataAvailable(
                            obsDataList, status);
                } catch (RemoteException e) {
                    Log.w(TAG, "onBsObsLocDataAvailable remote exception, sending intent");
                    GTPClientHelper.SendPendingIntent(mContext, mListenerIntent, "WWANDBProvider");
                }
            }
        }
    }

    private void onServiceRequest() {
        if (VERBOSE) {
            Log.d(TAG, "onServiceRequest");
        }
        synchronized (sCallBacksLock) {
            if (null != mWWANDBProviderResponseListener) {
                try {
                    mWWANDBProviderResponseListener.onServiceRequest();
                } catch (RemoteException e) {
                    Log.w(TAG, "onServiceRequest remote exception, sending intent");
                    GTPClientHelper.SendPendingIntent(mContext, mListenerIntent, "WWANDBProvider");
                }
            }
        }
    }

    public IWWANDBProvider getWWANDBProviderBinder() {
        return mBinder;
    }

    // ======================================================================
    // HIDL client
    // ======================================================================
    static class WWANDBProviderHidlClient extends BaseHidlClient
            implements BaseHidlClient.IServiceDeathCb {
        private final String TAG = "WWANDBProviderHidlClient";
        private final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

        private LocHidlWWANDBProviderCallback mLocHidlWWANDBProvicerCallback;
        private ILocHidlWWANDBProvider mLocHidlWWANDBProvider;

        public WWANDBProviderHidlClient(WWANDBProvider Provider) {
            getWWANDBProviderIface();
            mLocHidlWWANDBProvicerCallback = new LocHidlWWANDBProviderCallback(Provider);

            try {
                mLocHidlWWANDBProvider.init(mLocHidlWWANDBProvicerCallback);
                mLocHidlWWANDBProvider.registerWWANDBProvider(mLocHidlWWANDBProvicerCallback);
                registerServiceDiedCb(this);
            } catch (RemoteException e) {
                Log.e(TAG, "Exception on Provider init: " + e);
            }
        }

        void getWWANDBProviderIface() {
            Log.i(TAG, "getWWANDBProviderIface");
            ILocHidlGnss gnssService = (vendor.qti.gnss.V2_1.ILocHidlGnss) getGnssService();

            if (null != gnssService) {
                try {
                    mLocHidlWWANDBProvider = gnssService.getExtensionLocHidlWWANDBProvider();
                } catch (RemoteException e) {
                    throw new RuntimeException("Exception getting wifidb Provider: " + e);
                }
            } else {
                throw new RuntimeException("gnssService is null!");
            }
        }

        @Override
        public void onServiceDied() {
            mLocHidlWWANDBProvider = null;
            getWWANDBProviderIface();
            try {
                mLocHidlWWANDBProvider.init(mLocHidlWWANDBProvicerCallback);
                mLocHidlWWANDBProvider.registerWWANDBProvider(mLocHidlWWANDBProvicerCallback);
            } catch (RemoteException e) {
                Log.e(TAG, "Exception on Provider init: " + e);
            }
        }

        public void requestBSObsLocData() {
            if (null != mLocHidlWWANDBProvider) {
                try {
                    mLocHidlWWANDBProvider.sendBSObsLocDataRequest();
                } catch (RemoteException e) {
                    throw new RuntimeException("Exception on sendAPObsLocDataRequest: " + e);
                }
            } else {
                throw new RuntimeException("mLocHidlWWANDBProvider is null!");
            }
        }

        // ======================================================================
        // Callbacks
        // ======================================================================
        class LocHidlWWANDBProviderCallback extends ILocHidlWWANDBProviderCallback.Stub {

            private WWANDBProvider mWWANDBProvider;

            private LocHidlWWANDBProviderCallback(WWANDBProvider wiFiDBProvider) {
                mWWANDBProvider = wiFiDBProvider;
            }

            public void attachVmOnCallback() {
                // ???
            }

            public void serviceRequestCallback() {
                mWWANDBProvider.onServiceRequest();
            }

            public void bsObsLocDataUpdateCallback(ArrayList<LocHidlBSObsData> bsInfoList,
                    int apObsLocDataListSize, byte bsListStatus) {

                Log.i(TAG, "TESTDEBUG bsObsLocDataUpdateCallback");

                ArrayList<BSObsLocationData> obsDataList = new ArrayList<BSObsLocationData>();

                for (LocHidlBSObsData bsObsData: bsInfoList) {
                    BSInfo cellInfo = new BSInfo();
                    cellInfo.mCellType =
                            HidlClientUtils.FDCLtoIZatCellTypes(bsObsData.cellInfo.cell_type);
                    cellInfo.mCellRegionID1 = bsObsData.cellInfo.cell_id1;
                    cellInfo.mCellRegionID2 = bsObsData.cellInfo.cell_id2;
                    cellInfo.mCellRegionID3 = bsObsData.cellInfo.cell_id3;
                    cellInfo.mCellRegionID4 = bsObsData.cellInfo.cell_id4;

                    BSObsLocationData obsData = new BSObsLocationData();
                    obsData.mScanTimestamp = (int) bsObsData.scanTimestamp_ms;
                    obsData.mLocation =
                            HidlClientUtils.translateHidlLocation(bsObsData.gpsLoc.gpsLocation);
                    obsData.mCellInfo = cellInfo;

                    obsDataList.add(obsData);
                }

                mWWANDBProvider.onBsObsLocDataAvailable(obsDataList, bsListStatus);
            }
        }
    }
}
