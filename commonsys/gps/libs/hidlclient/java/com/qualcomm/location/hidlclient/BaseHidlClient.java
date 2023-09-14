/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.location.hidlclient;

import android.util.Log;
import android.os.RemoteException;
import java.util.NoSuchElementException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.HashSet;
import android.os.HwBinder;
import vendor.qti.gnss.V4_1.ILocHidlGnss;

abstract public class BaseHidlClient {
    private static vendor.qti.gnss.V1_0.ILocHidlGnss mGnssService = null;
    private static LocHidlDeathRecipient mDeathRecipient = null;
    private static HidlServiceVersion mHidlServiceVer = HidlServiceVersion.V0_0;
    private static Runnable mGetGnssService = null;
    private static CountDownLatch mCountDownLatch;
    private static final String TAG = "BaseHidlClient";
    //Izat modules wait for gnss hidl service maximum 30s
    private static final long WAIT_GETSERVICE_TIME_MS = 30000;

    public interface IServiceDeathCb {
        void onServiceDied();
    }

    static final class LocHidlDeathRecipient implements HwBinder.DeathRecipient {
        HashSet<IServiceDeathCb> mServiceDiedCbs = new HashSet<IServiceDeathCb>();

        @Override
        public void serviceDied(long cookie) {
            mGnssService = null;
            mHidlServiceVer = HidlServiceVersion.V0_0;
            mCountDownLatch = new CountDownLatch(1);
            new Thread(mGetGnssService).start();
            for (IServiceDeathCb item : mServiceDiedCbs) {
                item.onServiceDied();
            }
        }

        public void registerServiceDiedCb(IServiceDeathCb cb) {
            mServiceDiedCbs.add(cb);
        }

        public void deRegisterServiceDiedCb(IServiceDeathCb cb) {
            mServiceDiedCbs.remove(cb);
        }
    }

    public static enum HidlServiceVersion {
        V0_0,
        V1_0,
        V1_1,
        V1_2,
        V2_0,
        V2_1,
        V3_0,
        V4_0,
        V4_1,
    }

    static {
        mCountDownLatch = new CountDownLatch(1);
        mDeathRecipient = new LocHidlDeathRecipient();
        mGetGnssService = new Runnable() {
            @Override
            public void run() {
                do {
                    Log.d(TAG, "try to get 4.1 service");
                    try {
                        mGnssService = vendor.qti.gnss.V4_1.ILocHidlGnss
                                .getService("gnss_vendor", true);
                    } catch (RemoteException e) {
                        mGnssService = null;
                        Log.d(TAG, "RemoteException: " + e);
                    } catch (NoSuchElementException e) {
                        mGnssService = null;
                        Log.d(TAG, "NoSuchElementException: " + e);
                    }
                    if (mGnssService != null) {
                        mHidlServiceVer = HidlServiceVersion.V4_1;
                        break;
                    }


                    Log.d(TAG, "try to get 4.0 service");
                    try {
                        mGnssService = vendor.qti.gnss.V4_0.ILocHidlGnss
                                .getService("gnss_vendor", true);
                    } catch (RemoteException e) {
                        mGnssService = null;
                        Log.d(TAG, "RemoteException: " + e);
                    } catch (NoSuchElementException e) {
                        mGnssService = null;
                        Log.d(TAG, "NoSuchElementException: " + e);
                    }
                    if (mGnssService != null) {
                        mHidlServiceVer = HidlServiceVersion.V4_0;
                        break;
                    }

                    Log.d(TAG, "try to get 3.0 service");
                    try {
                        mGnssService = vendor.qti.gnss.V3_0.ILocHidlGnss
                                .getService("gnss_vendor", true);
                    } catch (RemoteException e) {
                        mGnssService = null;
                        Log.d(TAG, "RemoteException: " + e);
                    } catch (NoSuchElementException e) {
                        mGnssService = null;
                        Log.d(TAG, "NoSuchElementException: " + e);
                    }
                    if (mGnssService != null) {
                        mHidlServiceVer = HidlServiceVersion.V3_0;
                        break;
                    }

                    Log.d(TAG, "try to get 2.1 service");
                    try {
                        mGnssService = vendor.qti.gnss.V2_1.ILocHidlGnss
                                .getService("gnss_vendor", true);
                    } catch (RemoteException e) {
                        mGnssService = null;
                        Log.d(TAG, "RemoteException: " + e);
                    } catch (NoSuchElementException e) {
                        mGnssService = null;
                        Log.d(TAG, "NoSuchElementException: " + e);
                    }
                    if (mGnssService != null) {
                        mHidlServiceVer = HidlServiceVersion.V2_1;
                        break;
                    }

                    Log.d(TAG, "try to get 1.2 service");
                    try {
                        mGnssService = vendor.qti.gnss.V2_0.ILocHidlGnss
                                .getService("gnss_vendor", true);
                    } catch (RemoteException e) {
                        mGnssService = null;
                        Log.d(TAG, "RemoteException: " + e);
                    } catch (NoSuchElementException e) {
                        mGnssService = null;
                        Log.d(TAG, "NoSuchElementException: " + e);
                    }
                    if (mGnssService != null) {
                        mHidlServiceVer = HidlServiceVersion.V2_0;
                        break;
                    }

                    Log.d(TAG, "try to get 1.1 service");
                    try {
                        mGnssService = vendor.qti.gnss.V1_1.ILocHidlGnss
                                .getService("gnss_vendor", true);
                    } catch (RemoteException e) {
                        mGnssService = null;
                        Log.d(TAG, "RemoteException: " + e);
                    } catch (NoSuchElementException e) {
                        mGnssService = null;
                        Log.d(TAG, "NoSuchElementException: " + e);
                    }
                    if (mGnssService != null) {
                        mHidlServiceVer = HidlServiceVersion.V1_1;
                        break;
                    }

                    Log.d(TAG, "try to get 1.0 service");
                    try {
                        mGnssService = vendor.qti.gnss.V1_0.ILocHidlGnss
                                .getService("gnss_vendor", true);
                    } catch (RemoteException e) {
                        mGnssService = null;
                        Log.d(TAG, "RemoteException: " + e);
                    } catch (NoSuchElementException e) {
                        mGnssService = null;
                        Log.d(TAG, "NoSuchElementException: " + e);
                    }
                    if (mGnssService != null) {
                        mHidlServiceVer = HidlServiceVersion.V1_0;
                        break;
                    }
                }
                while (false);

                try {
                    if (mGnssService != null) {
                        mGnssService.linkToDeath(mDeathRecipient, 0);
                    }
                } catch (RemoteException e) {
                    Log.d(TAG, "RemoteException: " + e);
                } catch (NoSuchElementException e) {
                    Log.d(TAG, "NoSuchElementException: " + e);
                }
                mCountDownLatch.countDown();
            }
        };
        new Thread(mGetGnssService).start();
    }

    public static vendor.qti.gnss.V1_0.ILocHidlGnss getGnssService() {
        try {
            mCountDownLatch.await(WAIT_GETSERVICE_TIME_MS, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
        }
        return mGnssService;
    }

    public static HidlServiceVersion getHidlServiceVersion() {
        try {
            mCountDownLatch.await(WAIT_GETSERVICE_TIME_MS, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
        }
        return mHidlServiceVer;
    }

    protected static void registerServiceDiedCb(IServiceDeathCb cb) {
        mDeathRecipient.registerServiceDiedCb(cb);
    }

    protected static void deRegisterServiceDiedCb(IServiceDeathCb cb) {
        mDeathRecipient.deRegisterServiceDiedCb(cb);
    }
}
