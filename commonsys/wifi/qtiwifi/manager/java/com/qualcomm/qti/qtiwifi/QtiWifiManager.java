/* ==============================================================================
 * QtiWifiManager.java
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.qti.qtiwifi;

import android.content.Context;
import android.os.Handler;
import android.os.Binder;
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.Intent;
import java.util.List;

public class QtiWifiManager {
    private static final String TAG = "QtiWifiManager";
    private static ApplicationBinderCallback mApplicationCallback = null;
    private static Context mContext;
    private static boolean mServiceAlreadyBound = false;
    private static IQtiWifiManager mUniqueInstance = null;
    IQtiWifiManager mService;

    private QtiWifiManager(Context context, IQtiWifiManager service) {
        mContext = context;
        mService = service;
        Log.i(TAG, "QtiWifiManager created");
    }

    public static void initialize(Context context, ApplicationBinderCallback cb) {
        try {
            bindService(context);
        } catch (ServiceFailedToBindException e) {
            Log.e(TAG, "ServiceFailedToBindException received");
        }
        mApplicationCallback = cb;
        mContext = context;
    }

    private static void bindService(Context context)
        throws ServiceFailedToBindException {
        if (!mServiceAlreadyBound  || mUniqueInstance == null) {
            Log.d(TAG, "bindService- !mServiceAlreadyBound  || uniqueInstance == null");
            Intent serviceIntent = new Intent("com.qualcomm.qti.server.qtiwifi.QtiWifiService");
            serviceIntent.setPackage("com.qualcomm.qti.server.qtiwifi");
            if (!context.bindService(serviceIntent, mConnection, Context.BIND_AUTO_CREATE)) {
                Log.e(TAG,"Failed to connect to Provider service");
                throw new ServiceFailedToBindException("Failed to connect to Provider service");
            }
        }
    }

    public static void unbindService(Context context) {
        if(mServiceAlreadyBound) {
            context.unbindService(mConnection);
            mServiceAlreadyBound = false;
            mUniqueInstance = null;
        }
    }

    protected static ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            Log.d(TAG, "Connection object created");
            mServiceAlreadyBound = true;
            if (service == null) {
                Log.e(TAG, "qtiwifi service not available");
                return;
            }
            mUniqueInstance = IQtiWifiManager.Stub.asInterface(service);
            new QtiWifiManager(mContext, mUniqueInstance);
            mApplicationCallback.onAvailable(new QtiWifiManager(mContext, mUniqueInstance));
        }
        public void onServiceDisconnected(ComponentName className) {
            Log.d(TAG, "Remote service disconnected");
            mServiceAlreadyBound = false;
            mUniqueInstance = null;
        }
    };

    public static class ServiceFailedToBindException extends Exception {
        public static final long serialVersionUID = 1L;

        private ServiceFailedToBindException(String inString) {
            super(inString);
        }
    }

    public interface ApplicationBinderCallback {
        public abstract void onAvailable(QtiWifiManager manager);
    }

    /**
     * Base class for Csi callback. Should be extended by applications and set when calling
     * {@link QtiWifiManager#registerCsiCallback(CsiCallback, Handler)}.
     *
     */
    public interface CsiCallback {
        public abstract void onCsiUpdate(byte[] info);
    }

    /**
     * Callback proxy for CsiCallback objects.
     *
     */

    private static class CsiCallbackProxy extends ICsiCallback.Stub {
        private final Handler mHandler;
        private final CsiCallback mCallback;

        CsiCallbackProxy(Looper looper, CsiCallback callback) {
            mHandler = new Handler(looper);
            mCallback = callback;
        }

        @Override
        public void onCsiUpdate(byte[] info) throws RemoteException {
            mHandler.post(() -> {
                mCallback.onCsiUpdate(info);
            });
        }
    }

    public void registerCsiCallback(CsiCallback callback, Handler handler) {
        if (callback == null) throw new IllegalArgumentException("callback cannot be null");
        Log.v(TAG, "registerCsiCallback: callback=" + callback + ", handler=" + handler);

        Looper looper = (handler == null) ? mContext.getMainLooper() : handler.getLooper();
        Binder binder = new Binder();
        try {
            mService.registerCsiCallback(binder, new CsiCallbackProxy(looper, callback),
                    callback.hashCode());
        } catch (RemoteException e) {
            throw e.rethrowFromSystemServer();
        }
    }

    /**
     * Allow callers to unregister a previously registered callback. After calling this method,
     * applications will no longer receive csi events.
     *
     * @param callback Callback to unregister for csi events
     *
     */
    public void unregisterCsiCallback(CsiCallback callback) {
        if (callback == null) throw new IllegalArgumentException("callback cannot be null");
        Log.v(TAG, "unregisterCsiCallback: callback=" + callback);

        try {
            mService.unregisterCsiCallback(callback.hashCode());
        } catch (RemoteException e) {
            throw e.rethrowFromSystemServer();
        }
    }

    /**
     * Run driver command from user space
     */
    public void doDriverCmd(String command)
    {
        try {
            mService.doDriverCmd(command);
            return;
        } catch (RemoteException e) {
            throw e.rethrowFromSystemServer();
        }
    }

    public boolean startCsi(CsiCallback callback, Handler handler) {
        registerCsiCallback(callback, handler);
        try {
            mService.startCsi();
            return true;
        } catch (RemoteException e) {
            Log.e(TAG, "startCsi: " + e);
            return false;
        }
    }

    public boolean stopCsi(CsiCallback callback) {
        unregisterCsiCallback(callback);
        try {
            mService.stopCsi();
            return true;
        } catch (RemoteException e) {
            Log.e(TAG, "stopCsi: " + e);
            return false;
        }
    }
}
