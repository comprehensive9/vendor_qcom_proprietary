/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.location.izat.wifidbreceiver;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.Binder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

import com.qti.debugreport.*;
import com.qti.wifidbreceiver.*;

import com.qualcomm.location.hidlclient.BaseHidlClient;
import com.qualcomm.location.hidlclient.HidlClientUtils;
import com.qualcomm.location.izat.GTPClientHelper;
import com.qualcomm.location.izat.IzatService;
import com.qualcomm.location.izat.esstatusreceiver.EsStatusReceiver;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;

import vendor.qti.gnss.V1_0.ILocHidlGnss;
import vendor.qti.gnss.V1_0.ILocHidlWiFiDBReceiver.LocHidlApLocationData;
import vendor.qti.gnss.V1_0.ILocHidlWiFiDBReceiver.LocHidlApSpecialInfo;
import vendor.qti.gnss.V2_1.ILocHidlWiFiDBReceiver;
import vendor.qti.gnss.V2_1.ILocHidlWiFiDBReceiverCallback;
import vendor.qti.gnss.V2_1.LocHidlUlpLocation;
import vendor.qti.gnss.V2_1.LocHidlWifiDBListStatus;

public class WiFiDBReceiver implements IzatService.ISystemEventListener {
    private static final String TAG = "WiFiDBReceiver";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    // As defined in LocHidlWifiDBListStatus & GTP
    private static final int LOOKUP_STATUS = 3;

    private static final Object sCallBacksLock = new Object();
    private final Context mContext;
    // In case and old SDK client < 7.0 is used
    private boolean mIsLegacySDKClient = false;

    //Indicate whether the device is in emergency mode
    private boolean mIsEmergency = false;

    private IWiFiDBReceiverResponseListener mWiFiDBReceiverResponseListener = null;
    private PendingIntent mListenerIntent = null;
    private WiFiDBReceiverHidlClient mHidlClient = null;

    private String mPackageName;

    private static WiFiDBReceiver sInstance = null;
    public static WiFiDBReceiver getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new WiFiDBReceiver(ctx);
        }
        return sInstance;
    }

    private WiFiDBReceiver(Context ctx) {
        if (VERBOSE) {
            Log.d(TAG, "WiFiDBReceiver construction");
        }

        mHidlClient = new WiFiDBReceiverHidlClient(this);
        if (null == mHidlClient) {
            Log.e(TAG, "WiFiDBReceiver construction fail: " + mHidlClient);
        }

        EsStatusReceiver.getInstance(ctx).registerEsStatusListener(
                new EsStatusReceiver.IEsStatusListener() {
                    public void onStatusChanged(boolean isEmergencyMode) {
                        Log.d(TAG, "Emergency mode changed to : " + isEmergencyMode);
                        mIsEmergency = isEmergencyMode;
                    }});

        mContext = ctx;
        IzatService.AidlClientDeathNotifier.getInstance().registerAidlClientDeathCb(this);
    }

    @Override
    public void onAidlClientDied(String packageName) {
        if (mPackageName != null && mPackageName.equals(packageName)) {
            Log.d(TAG, "aidl client crash: " + packageName);
            synchronized (sCallBacksLock) {
                mWiFiDBReceiverResponseListener = null;
            }
        }
    }

    /* Remote binder */
    private final IWiFiDBReceiver.Stub mBinder = new IWiFiDBReceiver.Stub() {

        @Deprecated
        // For backwards compatibility with possible static-linked SDK
        public boolean registerResponseListener(final IWiFiDBReceiverResponseListener callback) {
            mIsLegacySDKClient = true;
            return registerResponseListenerExt(callback, null);
        }

        // For backwards compatibility with possible static-linked SDK
        public boolean registerResponseListenerExt(final IWiFiDBReceiverResponseListener callback,
                                                   PendingIntent notifyIntent) {
            mIsLegacySDKClient = false;
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return false;
            }

            if (notifyIntent == null) {
                Log.w(TAG, "notifyIntent is null");
            }

            if (null != mWiFiDBReceiverResponseListener) {
                Log.e(TAG, "Response listener already provided.");
                return false;
            }

            synchronized (sCallBacksLock) {
                mWiFiDBReceiverResponseListener = callback;
                mListenerIntent = notifyIntent;
            }

            mPackageName = mContext.getPackageManager().getNameForUid(Binder.getCallingUid());

            return true;
        }

        public void removeResponseListener(final IWiFiDBReceiverResponseListener callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }
            synchronized (sCallBacksLock) {
                mWiFiDBReceiverResponseListener = null;
                mListenerIntent = null;
            }

            mPackageName = null;
        }


        public void requestAPList(int expireInDays) {
            if (VERBOSE) {
                Log.d(TAG, "in IWiFiDBReceiver.Stub(): requestAPList()");
            }

            mHidlClient.sendAPListRequest(expireInDays);
        }

        public void requestScanList() {
            if (VERBOSE) {
                Log.d(TAG, "in IWiFiDBReceiver.Stub(): requestScanList()");
            }

            mHidlClient.sendScanListRequest();
        }

        public void pushWiFiDB(List<APLocationData> locData,
                              List<APSpecialInfo> splData,
                              int daysValid) {
            // Legacy SDK used reverse MAC as 445566112233 like original WiFi module
            if (mIsLegacySDKClient) {
                for (APLocationData apLocationData: locData) {
                    apLocationData.mMacAddress = reverseLo24Hi24(apLocationData.mMacAddress);
                }
                for (APSpecialInfo apSpecialInfo: splData) {
                    apSpecialInfo.mMacAddress = reverseLo24Hi24(apSpecialInfo.mMacAddress);
                }
            }

            mHidlClient.pushAPWiFiDB(locData, splData, daysValid, false);
        }

        public void pushLookupResult(List<APLocationData> locData,
                              List<APSpecialInfo> splData) {
            mHidlClient.pushAPWiFiDB(locData, splData, 0, true);
        }
    };

    protected void onAPListAvailable(ArrayList<APInfoExt> apInfoExtList, int status,
            Location location) {
        if (VERBOSE) {
            Log.d(TAG, "onAPListAvailable status: " + status);
        }
        synchronized (sCallBacksLock) {
            if (null != mWiFiDBReceiverResponseListener) {
                try {
                    if (mIsLegacySDKClient) {
                        List<APInfo> apInfoList = new ArrayList<APInfo>();
                        for (APInfoExt apInfoExt: apInfoExtList) {
                            APInfo apInfo = new APInfo();
                            apInfo.mMacAddress = apInfoExt.mMacAddress;
                            // Legacy SDK used reverse MAC as 445566112233 like original WiFi module
                            apInfo.mMacAddress = reverseLo24Hi24(apInfo.mMacAddress);
                            apInfo.mSSID = apInfoExt.mSSID;
                            apInfo.mCellType = apInfoExt.mCellType;
                            apInfo.mCellRegionID1 = apInfoExt.mCellRegionID1;
                            apInfo.mCellRegionID2 = apInfoExt.mCellRegionID2;
                            apInfo.mCellRegionID3 = apInfoExt.mCellRegionID3;
                            apInfo.mCellRegionID4 = apInfoExt.mCellRegionID4;

                            apInfoList.add(apInfo);
                        }

                        mWiFiDBReceiverResponseListener.onAPListAvailable(
                                apInfoList);
                    } else {
                        // Check special status to indicate it is the new onLookupRequest
                        if (LOOKUP_STATUS == status) {
                            if (!mWiFiDBReceiverResponseListener.onLookupRequestES(
                                    apInfoExtList, location, mIsEmergency)) {
                                Log.d(TAG, "Fall back to legacy onLookupRequest!");
                                mWiFiDBReceiverResponseListener.onLookupRequest(
                                        apInfoExtList, location);
                            }
                        } else {
                            mWiFiDBReceiverResponseListener.onAPListAvailableExt(
                                    apInfoExtList, status, location);
                        }
                    }
                } catch (RemoteException e) {
                    Log.w(TAG, "onAPListAvailable remote exception, sending intent");
                    GTPClientHelper.SendPendingIntent(mContext, mListenerIntent, "WiFiDBReceiver");
               }
            }
        }
    }

    protected void onStatusUpdate(boolean isSuccess, String error) {
        if (VERBOSE) {
            Log.d(TAG, "onStatusUpdate");
        }
        synchronized (sCallBacksLock) {
            if (null != mWiFiDBReceiverResponseListener) {
                try {
                    mWiFiDBReceiverResponseListener.onStatusUpdate(isSuccess,
                                                                                        error);
                    if (VERBOSE) {
                        Log.d(TAG, "onStatusUpdate: send update to listener");
                    }
                } catch (RemoteException e) {
                    Log.w(TAG, "onStatusUpdate remote exception, sending intent");
                    GTPClientHelper.SendPendingIntent(mContext, mListenerIntent, "WiFiDBReceiver");
                }
            }
        }
    }

    protected void onServiceRequest() {
        if (VERBOSE) {
            Log.d(TAG, "onServiceRequest");
        }
        synchronized (sCallBacksLock) {
            if (null != mWiFiDBReceiverResponseListener) {
                try {
                        if (!mWiFiDBReceiverResponseListener.onServiceRequestES(mIsEmergency)) {
                            Log.d(TAG, "Fall back to legacy onServiceRequest!");
                            mWiFiDBReceiverResponseListener.onServiceRequest();
                        }
                } catch (RemoteException e) {
                    Log.w(TAG, "onServiceRequest remote exception, sending intent");
                    GTPClientHelper.SendPendingIntent(mContext, mListenerIntent,
                            "WiFiDBReceiver");
                }
            }
        }
    }

    public IWiFiDBReceiver getWiFiDBReceiverBinder() {
        return mBinder;
    }

    private static String reverseLo24Hi24(String mac) {
        String res = mac;
        if (null != mac && mac.length() == 12) {
            res = mac.substring(6, 12) + mac.substring(0, 6);
        }
        return res;
    }

    // ======================================================================
    // HIDL client
    // ======================================================================

    static class WiFiDBReceiverHidlClient extends BaseHidlClient
            implements BaseHidlClient.IServiceDeathCb {
        private final String TAG = "WiFiDBReceiverHidlClient";
        private final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

        private LocHidlWiFiDBReceiverWrapper mWiFiDBReceiverWrapper;

        private WiFiDBReceiverHidlClient mWiFiDBReceiverHidlClient;

        private final int HAS_HORIZONTAL_COMPONENT = 1;
        private final int HAS_VERTICAL_COMPONENT = 2;
        private final int HAS_SOURCE = 4;

        private LocHidlWiFiDBReceiverCallback mLocHidlWiFiDBReceiverCallback;

        void getWiFiDBReceiverIface() {
            if (null == mWiFiDBReceiverWrapper) {
                try {
                    mWiFiDBReceiverWrapper = LocHidlWiFiDBReceiverWrapperFactory.getWrapper();
                } catch (RuntimeException e) {
                    Log.e(TAG, "Exception getting wifi db wrapper: " + e);
                    mWiFiDBReceiverWrapper = null;
                }
            }
        }

        @Override
        public void onServiceDied() {
            mWiFiDBReceiverWrapper = null;
            getWiFiDBReceiverIface();
            try {
                mWiFiDBReceiverWrapper.init(mLocHidlWiFiDBReceiverCallback);
                mWiFiDBReceiverWrapper.registerWiFiDBUpdater(mLocHidlWiFiDBReceiverCallback);
            } catch (RemoteException e) {
                Log.e(TAG, "Exception on receiver init: " + e);
            }
        }

        private WiFiDBReceiverHidlClient(WiFiDBReceiver receiver) {
            getWiFiDBReceiverIface();

            mLocHidlWiFiDBReceiverCallback = new LocHidlWiFiDBReceiverCallback(receiver);

            try {
                Log.d(TAG, "WiFiDBReceiverHidlClient hidl init");
                if (null != mWiFiDBReceiverWrapper) {
                    mWiFiDBReceiverWrapper.init(mLocHidlWiFiDBReceiverCallback);
                    registerServiceDiedCb(this);
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Exception on receiver init: " + e);
            }
        }

        public void sendAPListRequest(int expire_in_days) {
            try {
                if (null != mWiFiDBReceiverWrapper) {
                    mWiFiDBReceiverWrapper.sendAPListRequest(expire_in_days);
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Exception in sendAPListRequest: " + e);
            }
        }

        public void sendScanListRequest() {
            try {
                if (null != mWiFiDBReceiverWrapper) {
                    mWiFiDBReceiverWrapper.sendScanListRequest();
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Exception in sendScanListRequest: " + e);
            }
        }

        public int pushAPWiFiDB(List<APLocationData> locData, List<APSpecialInfo> splData,
                int daysValid, boolean isLookup) {

            int result = 0;

            ArrayList<LocHidlApLocationData> apLocList = new ArrayList<LocHidlApLocationData>();
            for (APLocationData loc: locData) {
                LocHidlApLocationData apLoc = new LocHidlApLocationData();
                apLoc.mac_R48b = HidlClientUtils.hexMacToLong(loc.mMacAddress);
                apLoc.latitude = loc.mLatitude;
                apLoc.longitude = loc.mLongitude;
                apLoc.max_antenna_range = loc.mMaxAntenaRange;
                apLoc.horizontal_error = loc.mHorizontalError;
                apLoc.reliability = (byte) loc.mReliability;
                apLoc.valid_bits = (byte) loc.mValidBits;

                apLocList.add(apLoc);
            }

            ArrayList<LocHidlApSpecialInfo> splList = new ArrayList<LocHidlApSpecialInfo>();
            for (APSpecialInfo sp: splData) {
                LocHidlApSpecialInfo spl = new LocHidlApSpecialInfo();
                spl.mac_R48b = HidlClientUtils.hexMacToLong(sp.mMacAddress);
                spl.info = (byte) sp.mInfo;

                splList.add(spl);
            }

            try {
                if (null != mWiFiDBReceiverWrapper) {
                    mWiFiDBReceiverWrapper.pushAPWiFiDB_2_1(apLocList, (short) apLocList.size(),
                            splList, (short) splList.size(), daysValid, isLookup);
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Exception pushing wifidb: " + e);
            }

            return result;
        }

        protected void finalize() throws Throwable {
            if (null != mWiFiDBReceiverWrapper) {
                mWiFiDBReceiverWrapper.unregisterWiFiDBUpdater();
            }
        }

        // ======================================================================
        // HIDL Wrapper factory
        // ======================================================================
        static class LocHidlWiFiDBReceiverWrapperFactory extends BaseHidlClient {
            private static final String TAG = "WiFiDBReceiverHIDLWrapperFactory";

            static LocHidlWiFiDBReceiverWrapper getWrapper() {
                Log.i(TAG, "LocHidlWiFiDBReceiverWrapper ftry");
                ILocHidlGnss gnssService = getGnssService();
                HidlServiceVersion hidlVer = getHidlServiceVersion();
                LocHidlWiFiDBReceiverWrapper instance = null;

                if (null != gnssService) {
                    try {
                        if (hidlVer.compareTo(HidlServiceVersion.V2_1) >= 0) {
                            Log.d(TAG, "LocHidlWiFiDBReceiverWrapper factory calling new 21");
                            ILocHidlWiFiDBReceiver wiFiDBReceiver =
                                    ((vendor.qti.gnss.V2_1.ILocHidlGnss)gnssService)
                                    .getExtensionLocHidlWiFiDBReceiver_2_1();
                            instance = new LocHidlWiFiDBReceiverWrapper21(wiFiDBReceiver);
                        } else {
                            Log.i(TAG, "LocHidlWiFiDBReceiverWrapper ftry calling new base 10");
                            ILocHidlWiFiDBReceiver wiFiDBReceiver =
                                    (vendor.qti.gnss.V2_1.ILocHidlWiFiDBReceiver)
                                    gnssService.getExtensionLocHidlWiFiDBReceiver();
                            instance = new LocHidlWiFiDBReceiverWrapper(wiFiDBReceiver);
                        }
                    } catch (RemoteException e) {
                        throw new RuntimeException("Exception getting wifidb receiver: " + e);
                    }
                } else {
                    throw new RuntimeException("gnssService is null!");
                }

                if (null == instance) {
                    throw new RuntimeException("wifidb wrapper is null!");
                }

                Log.i(TAG, "LocHidlWiFiDBReceiverWrapper factory all OK, returns instance");
                return instance;
            }
        }

        // ======================================================================
        // HIDL base version Wrapper
        // ======================================================================
        static class LocHidlWiFiDBReceiverWrapper extends ILocHidlWiFiDBReceiver.Stub {
            private static final String TAG = "WiFiDBReceiverHIDLWrapper";
            protected ILocHidlWiFiDBReceiver mWiFiDBReceiver = null;

            public LocHidlWiFiDBReceiverWrapper(ILocHidlWiFiDBReceiver wiFiDBReceiver)
                    throws RemoteException {
                mWiFiDBReceiver = wiFiDBReceiver;
                if (null == mWiFiDBReceiver) {
                    throw new RuntimeException("wifidb receiver is null!");
                }
            }

            public boolean init(vendor.qti.gnss.V1_0.ILocHidlWiFiDBReceiverCallback
                    locHidlWiFiDBReceiverCallback) throws RemoteException {
                try {
                    mWiFiDBReceiver.init(locHidlWiFiDBReceiverCallback);
                    mWiFiDBReceiver.registerWiFiDBUpdater(locHidlWiFiDBReceiverCallback);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception on receiver init: " + e);
                }
                return true;
            }

            public void registerWiFiDBUpdater(
                    vendor.qti.gnss.V1_0.ILocHidlWiFiDBReceiverCallback
                    locHidlWiFiDBReceiverCallback) throws RemoteException {
                mWiFiDBReceiver.registerWiFiDBUpdater(locHidlWiFiDBReceiverCallback);
            }

            public void sendAPListRequest(int expire_in_days) throws RemoteException {
                mWiFiDBReceiver.sendAPListRequest(expire_in_days);
            }

            public void sendScanListRequest() throws RemoteException {
                Log.e(TAG, "sendScanListRequest not supportded in this HIDL version (1.0)");
            }

            public void pushAPWiFiDB_2_1(
                    ArrayList<LocHidlApLocationData> apLocList, short apLocListSize,
                    ArrayList<LocHidlApSpecialInfo> splList, short splListSize, int daysValid,
                    boolean isLookup) throws RemoteException {
                pushAPWiFiDB(apLocList, apLocListSize, splList, splListSize, daysValid);
            }

            public void pushAPWiFiDB(
                    ArrayList<LocHidlApLocationData> apLocList, short apLocListSize,
                    ArrayList<LocHidlApSpecialInfo> splList, short splListSize, int daysValid)
                    throws RemoteException {
                mWiFiDBReceiver.pushAPWiFiDB(
                        apLocList, apLocListSize, splList, splListSize, daysValid);
            }

            public void unregisterWiFiDBUpdater() throws RemoteException {
                mWiFiDBReceiver.unregisterWiFiDBUpdater();
            }
        }

        // ======================================================================
        // HIDL version 2.1 Wrapper
        // ======================================================================
        static class LocHidlWiFiDBReceiverWrapper21 extends LocHidlWiFiDBReceiverWrapper implements
                vendor.qti.gnss.V2_1.ILocHidlWiFiDBReceiver {

            public LocHidlWiFiDBReceiverWrapper21(ILocHidlWiFiDBReceiver wiFiDBReceiver)
                    throws RemoteException {
                super(wiFiDBReceiver);
            }

            public void sendScanListRequest() throws RemoteException {
                mWiFiDBReceiver.sendScanListRequest();
            }

            public void pushAPWiFiDB_2_1(
                    ArrayList<LocHidlApLocationData> apLocList, short apLocListSize,
                    ArrayList<LocHidlApSpecialInfo> splList, short splListSize, int daysValid,
                    boolean isLookup) throws RemoteException {
                mWiFiDBReceiver.pushAPWiFiDB_2_1(apLocList, apLocListSize,
                        splList, splListSize, daysValid, isLookup);
            }
        }

        // ======================================================================
        // Callbacks
        // ======================================================================

        class LocHidlWiFiDBReceiverCallback extends ILocHidlWiFiDBReceiverCallback.Stub {

            private WiFiDBReceiver mWiFiDBReceiver;

            private LocHidlWiFiDBReceiverCallback(WiFiDBReceiver wiFiDBReceiver) {
                mWiFiDBReceiver = wiFiDBReceiver;
            }

            public void attachVmOnCallback() {
                // ???
            }

            public void serviceRequestCallback() {
                mWiFiDBReceiver.onServiceRequest();
            }

            public void statusUpdateCallback(boolean status, String reason) {
                mWiFiDBReceiver.onStatusUpdate(status, reason);
            }

            public void apListUpdateCallback(ArrayList<LocHidlApInfo> apInfoList, int apListSize) {
                // deprecated
            }

            public void apListUpdateCallback_2_1(ArrayList<LocHidlApInfoExt> apInfoList,
                    int apListSize, int apListStatus, LocHidlUlpLocation ulpLocation,
                    boolean ulpLocationValid) {

                ArrayList<APInfoExt> apInfoExtList = new ArrayList<APInfoExt>();

                for (LocHidlApInfoExt apInfoHidl: apInfoList) {
                    APInfoExt apInfo = new APInfoExt();
                    apInfo.mMacAddress =
                            HidlClientUtils.longMacToHex(apInfoHidl.base_apinfo_1_0.mac_R48b);
                    apInfo.mCellType = apInfoHidl.base_apinfo_1_0.cell_type;
                    apInfo.mCellRegionID1 = apInfoHidl.base_apinfo_1_0.cell_id1;
                    apInfo.mCellRegionID2 = apInfoHidl.base_apinfo_1_0.cell_id2;
                    apInfo.mCellRegionID3 = apInfoHidl.base_apinfo_1_0.cell_id3;
                    apInfo.mCellRegionID4 = apInfoHidl.base_apinfo_1_0.cell_id4;
                    apInfo.mSSID = apInfoHidl.base_apinfo_1_0.ssid.getBytes();
                    apInfo.mTimestamp = (int) apInfoHidl.utc_time;
                    apInfo.mFdalStatus = apInfoHidl.fdal_status;

                    apInfoExtList.add(apInfo);
                }

                mWiFiDBReceiver.onAPListAvailable(apInfoExtList, apListStatus, 
                        HidlClientUtils.translateHidlLocation(ulpLocation.gpsLocation));
            }
        }
    }
}
