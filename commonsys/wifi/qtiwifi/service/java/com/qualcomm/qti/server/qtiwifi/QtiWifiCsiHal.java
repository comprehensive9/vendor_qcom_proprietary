/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.server.qtiwifi;

import android.annotation.NonNull;
import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;
import android.os.HwRemoteBinder;
import android.os.RemoteException;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import java.util.ArrayList;
import java.util.NoSuchElementException;
import java.util.HashMap;
import java.util.List;

import vendor.qti.hardware.wifi.wificfr.V1_0.IWificfr;
import vendor.qti.hardware.wifi.wificfr.V1_0.IWificfrDataCallback;
import vendor.qti.hardware.wifi.wificfr.V1_0.CaptureStatus;
import vendor.qti.hardware.wifi.wificfr.V1_0.CaptureStatusCode;

import com.qualcomm.qti.qtiwifi.ICsiCallback;

public class QtiWifiCsiHal {
    private static final String TAG = "QtiWifiCsiHal";

    private final Object mLock = new Object();
    private boolean mVerboseLoggingEnabled = true;

    public static final String HAL_INSTANCE_NAME = "wificfr";

    private IServiceManager mIServiceManager = null;
    private IWificfr mIWifiCfr;
    private QtiWifiCsiDeathEventHandler mDeathEventHandler;
    private final IWificfrDataCallback mIWifiCfrDataCallback;

    /* Limit on number of registered csi callbacks to track and prevent potential memory leak */
    private static final int NUM_CSI_CALLBACKS_WTF_LIMIT = 20;
    private final HashMap<Integer, ICsiCallback> mRegisteredCsiCallbacks;
    private ICsiCallback mCsiCallback;

    /********************************************************
     * WifiCsi operations
     ********************************************************/

    /**
     * Callback to notify WifiCtr death.
     */
    public interface QtiWifiCsiDeathEventHandler {
        /**
         * Invoked when the QtiWifi dies.
         */
        void onDeath();
    }


    /**
     * Callback for events on IWifiCfr interface.
     */
    private class WifiCfrDataCallback extends IWificfrDataCallback.Stub {
        @Override
        public void onCfrDataAvailable(ArrayList<Byte> info) {
            Log.i(TAG, "onCfrDataAvailable called");
            if (mCsiCallback != null) {
                try {
                    byte[] byteArray = new byte[(info).size()];
                    int i = 0;
                    for (Byte b : info) {
                        byteArray[i++] = b;
                    }
                    StringBuilder strBuilder = new StringBuilder();
                    for(byte val : byteArray) {
                        strBuilder.append(String.format("%02x", val&0xff));
                    }
                    mCsiCallback.onCsiUpdate(byteArray);
                } catch (RemoteException e) {
                    Log.e(TAG, "onCsiUpdate " + e);
                }

            }
        }
    }


    private final IServiceNotification mServiceNotificationCallback =
            new IServiceNotification.Stub() {
        public void onRegistration(String fqName, String name, boolean preexisting) {
            synchronized (mLock) {
                if (mVerboseLoggingEnabled) {
                    Log.i(TAG, "IServiceNotification.onRegistration for: " + fqName
                            + ", " + name + " preexisting=" + preexisting);
                }
                if (!initIWifiCfrService()) {
                    Log.e(TAG, "initializing WifiCfr failed.");
                    wifiCfrServiceDiedHandler();
                } else {
                    Log.i(TAG, "Completed initialization of IWifiCfr.");
                }
            }
        }
    };
    private final HwRemoteBinder.DeathRecipient mServiceManagerDeathRecipient =
            cookie -> {
                synchronized (mLock) {
                    Log.w(TAG, "IServiceManager died: cookie=" + cookie);
                    wifiCfrServiceDiedHandler();
                    mIServiceManager = null; // Will need to register a new ServiceNotification
                }
            };
    private final HwRemoteBinder.DeathRecipient mWifiCfrDeathRecipient =
            cookie -> {
                synchronized (mLock) {
                    Log.w(TAG, "IWifiCfr died: cookie=" + cookie);
                    wifiCfrServiceDiedHandler();
                }
            };


    public QtiWifiCsiHal() {
        mIWifiCfrDataCallback = new WifiCfrDataCallback();
        mRegisteredCsiCallbacks = new HashMap<>();
    }

    /**
     * Enable/Disable verbose logging.
     *
     * @param enable true to enable, false to disable.
     */
    void enableVerboseLogging(boolean enable) {
        synchronized (mLock) {
            mVerboseLoggingEnabled = enable;
        }
    }

    /**
     * Link to death for IServiceManager object.
     * @return true on success, false otherwise.
     */
    private boolean linkToServiceManagerDeath() {
        synchronized (mLock) {
            if (mIServiceManager == null) return false;
            try {
                if (!mIServiceManager.linkToDeath(mServiceManagerDeathRecipient, 0)) {
                    Log.wtf(TAG, "Error on linkToDeath on IServiceManager");
                    wifiCfrServiceDiedHandler();
                    mIServiceManager = null; // Will need to register a new ServiceNotification
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "IServiceManager.linkToDeath exception", e);
                mIServiceManager = null; // Will need to register a new ServiceNotification
                return false;
            }
            return true;
        }
    }

    /**
     * Registers a service notification for the IWifiCfr service, which triggers intialization of
     * the IWifiCfr
     * @return true if the service notification was successfully registered
     */
    public boolean initialize() {
        synchronized (mLock) {
            if (mVerboseLoggingEnabled) {
                Log.i(TAG, "Registering IWifiCfr service ready callback.");
            }

            if (mIServiceManager != null) {
                // Already have an IServiceManager and serviceNotification registered,
                // don't register another.
                return true;
            }
            try {
                mIServiceManager = getServiceManagerMockable();
                if (mIServiceManager == null) {
                    Log.e(TAG, "Failed to get HIDL Service Manager");
                    return false;
                }
                if (!linkToServiceManagerDeath()) {
                    return false;
                }
                if (!mIServiceManager.registerForNotifications(
                        IWificfr.kInterfaceName, "", mServiceNotificationCallback)) {
                    Log.e(TAG, "Failed to register for notifications to "
                            + IWificfr.kInterfaceName);
                    mIServiceManager = null; // Will need to register a new ServiceNotification
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Exception while trying to register a listener for IWifiCfr service: "
                        + e);
                wifiCfrServiceDiedHandler();
                mIServiceManager = null; // Will need to register a new ServiceNotification
                return false;
            }

            if (isSupportedInternal()) {
                return initIWifiCfrService();
            }

            return false;
        }
    }

    /**
     * Link to death for IWificfr object.
     * @return true on success, false otherwise.
     */
    private boolean linkToWifiCfrDeath() {
        synchronized (mLock) {
            if (mIWifiCfr == null) return false;
            try {
                if (!mIWifiCfr.linkToDeath(mWifiCfrDeathRecipient, 0)) {
                    Log.wtf(TAG, "Error on linkToDeath on IWifiCfr");
                    wifiCfrServiceDiedHandler();
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "IWifiCfr.linkToDeath exception", e);
                return false;
            }
            return true;
        }
    }

    /**
     * Uses the IServiceManager to query if the WifiCfr HAL is present in the VINTF for the device
     * or not.
     * @return true if supported, false otherwise.
     */
    private boolean isSupportedInternal() {
        synchronized (mLock) {
            if (mIServiceManager == null) {
                Log.e(TAG, "isSupported: called but mIServiceManager is null!?");
                return false;
            }
            try {
                return (mIServiceManager.getTransport(IWificfr.kInterfaceName, HAL_INSTANCE_NAME)
                        != IServiceManager.Transport.EMPTY);
            } catch (RemoteException e) {
                Log.wtf(TAG, "Exception while operating on IServiceManager: " + e);
                return false;
            }
        }
    }

    /**
     * Initialize the IWifiCfr object.
     * @return true on success, false otherwise.
     */
    private boolean initIWifiCfrService() {
        synchronized (mLock) {
            if (mIWifiCfr != null) {
                return true;
            }

            try {
                mIWifiCfr = getWifiCfrMockable();
            } catch (RemoteException e) {
                Log.e(TAG, "IWifiCfr.getService exception: " + e);
                return false;
            } catch (NoSuchElementException e) {
                Log.e(TAG, "IWifiCfr.getService exception: " + e);
                return false;
            }
            if (mIWifiCfr == null) {
                Log.e(TAG, "Got null IWifiCfr service. Stopping wificfr HIDL startup");
                return false;
            }
            if (!linkToWifiCfrDeath()) {
                return false;
            }
        }
        return true;
    }

    /**
     * register IWifiCfr event callback.
     *
     * @param callback .
     * @return true on success, false otherwise.
     */
    private boolean registerEventCallback() {
        synchronized (mLock) {
            final String methodStr = "registerEventCallback";
            if (!checkWifiCfrAndLogFailure(methodStr)) return false;
            try {
                mIWifiCfr.unregisterEventCallback(mIWifiCfrDataCallback);
                CaptureStatus status = mIWifiCfr.registerEventCallback(mIWifiCfrDataCallback);
                return checkStatusAndLogFailure(status, methodStr);
            } catch (RemoteException e) {
                handleRemoteException(e, methodStr);
                return false;
            }
        }
    }


    /**
     * unregister IWifiCfr event callback.
     *
     * @param callback .
     * @return true on success, false otherwise.
     */
    private boolean unregisterEventCallback() {
        synchronized (mLock) {
            final String methodStr = "unregisterEventCallback";
            if (!checkWifiCfrAndLogFailure(methodStr)) return false;
            try {
                CaptureStatus status = mIWifiCfr.unregisterEventCallback(mIWifiCfrDataCallback);
                return checkStatusAndLogFailure(status, methodStr);
            } catch (RemoteException e) {
                handleRemoteException(e, methodStr);
                return false;
            }
        }
    }


    /**
     * Registers a death notification for wificfr.
     * @return Returns true on success.
     */
    public boolean registerDeathHandler(@NonNull QtiWifiCsiDeathEventHandler handler) {
        if (mDeathEventHandler != null) {
            Log.e(TAG, "Death handler already present");
        }
        mDeathEventHandler = handler;
        return true;
    }

    /**
     * Deregisters a death notification for wifictr.
     * @return Returns true on success.
     */
    public boolean deregisterDeathHandler() {
        if (mDeathEventHandler == null) {
            Log.e(TAG, "No Death handler present");
        }
        mDeathEventHandler = null;
        return true;
    }

    /**
     * Clear internal state.
     */
    private void clearState() {
        synchronized (mLock) {
            mIWifiCfr = null;
        }
    }

    /**
     * Handle wificfr death.
     */
    private void wifiCfrServiceDiedHandler() {
        synchronized (mLock) {
            clearState();
            if (mDeathEventHandler != null) {
                mDeathEventHandler.onDeath();
            }
        }
    }

    /**
     * Signals whether Initialization completed successfully.
     */
    public boolean isInitializationStarted() {
        synchronized (mLock) {
            return mIServiceManager != null;
        }
    }

    /**
     * Signals whether Initialization completed successfully.
     */
    public boolean isInitializationComplete() {
        synchronized (mLock) {
            return mIWifiCfr != null;
        }
    }

    /**
     * Wrapper functions to access static HAL methods, created to be mockable in unit tests
     */
    protected IServiceManager getServiceManagerMockable() throws RemoteException {
        synchronized (mLock) {
            return IServiceManager.getService();
        }
    }

    protected IWificfr getWifiCfrMockable() throws RemoteException {
        synchronized (mLock) {
            try {
                return IWificfr.getService(HAL_INSTANCE_NAME);
            } catch (NoSuchElementException e) {
                Log.e(TAG, "Exception getting IWifiCfr service: " + e);
                return null;
            }

        }
    }

    /**
     * Returns false if WifiCfr is null, and logs failure to call methodStr
     */
    private boolean checkWifiCfrAndLogFailure(String methodStr) {
        synchronized (mLock) {
            if (mIWifiCfr == null) {
                Log.e(TAG, "Can't call " + methodStr + ", IWifiCfr is null");
                return false;
            }
            return true;
        }
    }

    /**
     * Returns true if provided status code is SUCCESS, logs debug message and returns false
     * otherwise
     */
    private boolean checkStatusAndLogFailure(CaptureStatus status,
            String methodStr) {
        synchronized (mLock) {
            if (status.code != CaptureStatusCode.SUCCESS) {
                Log.e(TAG, "IWifiCfr." + methodStr + " failed: " + status.code
                        + ", " + status.debugMessage);
                return false;
            } else {
                if (mVerboseLoggingEnabled) {
                    Log.d(TAG, "IWifiCfr." + methodStr + " succeeded");
                }
                return true;
            }
        }
    }

    private void handleRemoteException(RemoteException e, String methodStr) {
        synchronized (mLock) {
            wifiCfrServiceDiedHandler();
            Log.e(TAG, "IWifiCfr." + methodStr + " failed with exception", e);
        }
    }

    public void registerCsiCallback(IBinder binder, ICsiCallback callback,
            int callbackIdentifier) {
        // register for binder death
        IBinder.DeathRecipient dr = new IBinder.DeathRecipient() {
            @Override
            public void binderDied() {
                binder.unlinkToDeath(this, 0);
                mRegisteredCsiCallbacks.remove(callbackIdentifier);
            }
        };
        try {
            binder.linkToDeath(dr, 0);
        } catch (RemoteException e) {
            Log.e(TAG, "Error on linkToDeath - " + e);
            return;
        }

        mCsiCallback = callback;
        mRegisteredCsiCallbacks.put(callbackIdentifier, callback);

        if (mRegisteredCsiCallbacks.size() > NUM_CSI_CALLBACKS_WTF_LIMIT) {
            Log.e(TAG, "Too many csi callbacks: " + mRegisteredCsiCallbacks.size());
        }

    }

    public void unregisterCsiCallback(int callbackIdentifier) {
        if (mCsiCallback != null)
            mCsiCallback = null;
        mRegisteredCsiCallbacks.remove(callbackIdentifier);
    }

    public void startCsi() {
       Log.i(TAG, "startCsi Called");
       final String methodStr = "startCsi";
       try {
           if (!registerEventCallback()) {
               Log.e(TAG, "CFR tool is not running, make sure its running");
           } else {
               mIWifiCfr.csiCaptureStart();
               Log.i(TAG, "csiCaptureStart invoked through hidl client");
           }
       } catch (RemoteException e) {
           handleRemoteException(e, methodStr);
           return;
       }
    }

    public void stopCsi() {
       Log.i(TAG, "stopCsi Called");
       final String methodStr = "stopCsi";
       try {
           if (!unregisterEventCallback()) {
               Log.e(TAG, "CFR tool is not running, make sure its running");
           } else {
               mIWifiCfr.csiCaptureStop();
               Log.i(TAG, "csiCaptureStop invoked through hidl client");
           }
       } catch (RemoteException e) {
           handleRemoteException(e, methodStr);
           return;
       }
    }

}
