/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 - 2016, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.geofence;

import android.content.Intent;
import android.content.Context;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.location.Location;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.HwBinder;

import android.util.Log;

import android.app.PendingIntent;

import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;

import com.qti.geofence.*;
import com.qualcomm.location.izat.DataPerPackageAndUser;
import com.qualcomm.location.izat.CallbackData;
import com.qualcomm.location.izat.IzatService;

import vendor.qti.gnss.V1_0.ILocHidlGeofenceService;
import vendor.qti.gnss.V1_0.ILocHidlGeofenceServiceCallback;
import vendor.qti.gnss.V2_1.ILocHidlGnss;
import com.qualcomm.location.hidlclient.BaseHidlClient;
import com.qualcomm.location.hidlclient.HidlClientUtils;
import com.qualcomm.location.izat.IzatService;
import com.qualcomm.location.utils.IZatServiceContext;

public class GeofenceServiceProvider implements IzatService.ISystemEventListener {
    private static final String TAG = "GeofenceServiceProvider";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    public static final int GEOFENCE_RESULT_SUCCESS = 0;
    public static final int GEOFENCE_RESULT_ERROR_TOO_MANY_GEOFENCES = -100;
    public static final int GEOFENCE_RESULT_ERROR_ID_EXISTS = -101;
    public static final int GEOFENCE_RESULT_ERROR_ID_UNKNOWN = -102;
    public static final int GEOFENCE_RESULT_ERROR_INVALID_TRANSITION = -103;
    public static final int GEOFENCE_RESULT_ERROR_GENERIC = -149;

    public static final int GEOFENCE_REQUEST_TYPE_ADD = 1;
    public static final int GEOFENCE_REQUEST_TYPE_REMOVE = 2;
    public static final int GEOFENCE_REQUEST_TYPE_PAUSE = 3;
    public static final int GEOFENCE_REQUEST_TYPE_RESUME = 4;
    private static int sGeofenceId = 1;
    private static final Object sCallBacksLock = new Object();
    private PendingIntent mPendingIntent;
    private final Context mContext;
    private GeofenceServiceHidlClient mGeofenceServiceHidlClient;
    private IZatServiceContext mIZatServiceCtx;

    private class ClientGeofenceData extends CallbackData {
        private IGeofenceCallback mCallback;
        private PendingIntent mPendingIntent;
        private Map<Integer, GeofenceData> mGeofenceMap;

        public ClientGeofenceData(IGeofenceCallback callback) {
            mCallback = callback;
            super.mCallback = callback;
            mGeofenceMap = new HashMap<Integer, GeofenceData> ();
        }

        public ClientGeofenceData(PendingIntent notifyIntent) {
            mPendingIntent = notifyIntent;
            mGeofenceMap = new HashMap<Integer, GeofenceData> ();
        }

        public void setCallback(IGeofenceCallback callback) {
            mCallback = callback;
            super.mCallback = callback;
        }
    }

    private DataPerPackageAndUser<ClientGeofenceData> mDataPerPackageAndUser;

    private RemoteCallbackList<IGeofenceCallback> mGeofenceCallbacks
        = new RemoteCallbackList<IGeofenceCallback>();

    private static GeofenceServiceProvider sInstance = null;
    public static GeofenceServiceProvider getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new GeofenceServiceProvider(ctx);
        }
        return sInstance;
    }

    private GeofenceServiceProvider(Context ctx) {
        if (VERBOSE) {
            Log.d(TAG, "GeofenceServiceProvider construction");
        }

        mContext = ctx;
        mIZatServiceCtx = IZatServiceContext.getInstance(mContext);
        mDataPerPackageAndUser = new DataPerPackageAndUser<ClientGeofenceData>(mContext,
                new UserChanged());
        mGeofenceServiceHidlClient = new GeofenceServiceHidlClient(this);

        mIZatServiceCtx. registerSystemEventListener(MSG_PKG_REMOVED, this);
        IzatService.AidlClientDeathNotifier.getInstance().registerAidlClientDeathCb(this);
    }

    @Override
    public void notify(int msgId, Object... args) {
        if (msgId == MSG_PKG_REMOVED) {
            Intent intent = (Intent)args[0];
            synchronized (sCallBacksLock)  {
                Log.d(TAG, "Package uninstalled, removing its geofences: " +
                        intent.getData().getSchemeSpecificPart());
                for (ClientGeofenceData clData:
                        mDataPerPackageAndUser.getAllDataPerPackageName(
                            intent.getData().getSchemeSpecificPart())) {
                    removeAllGeofences(clData);
                }
            }
        }
    }

    @Override
    public void onAidlClientDied(String packageName) {
        Log.d(TAG, "aidl client crash: " + packageName);
        synchronized (sCallBacksLock) {
            ClientGeofenceData clData =
                    mDataPerPackageAndUser.getDataByPkgName(packageName);

            if (null != clData) {
                mGeofenceCallbacks.unregister(clData.mCallback);
                clData.mCallback = null;
                if (null == clData.mPendingIntent) {
                    if (VERBOSE) {
                        Log.d(TAG, "Client died, no intent:" +
                              mDataPerPackageAndUser.getPackageName(null) +
                              " remove all geofences");
                    }
                    removeAllGeofences(clData);
                } else {
                    if (VERBOSE) {
                        Log.d(TAG, "Client died:" +
                              mDataPerPackageAndUser.getPackageName(null) +
                              " notify on breach by intent");
                    }
                }
            } else {
                Log.e(TAG, "Client died but no clientData found!: " +
                      mDataPerPackageAndUser.getPackageName(null));
            }

        }
    }

    /* Remote binder */
    private final IGeofenceService.Stub mBinder = new IGeofenceService.Stub() {

        public void registerCallback(final IGeofenceCallback callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }

            synchronized (sCallBacksLock) {
                ClientGeofenceData clData = mDataPerPackageAndUser.getData();
                if (null == clData) {
                    clData = new ClientGeofenceData(callback);
                    clData.mCallback = callback;

                    mDataPerPackageAndUser.setData(clData);
                } else {
                    if (null != clData.mCallback) {
                        mGeofenceCallbacks.unregister(clData.mCallback);
                    }
                    clData.setCallback(callback);
                }
                mGeofenceCallbacks.register(callback);
            }
        }

        public void unregisterCallback(IGeofenceCallback callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }
            synchronized (sCallBacksLock) {
                removeAllGeofences(mDataPerPackageAndUser.getData());
                mGeofenceCallbacks.unregister(callback);
            }
        }

        public void registerPendingIntent(final PendingIntent notifyIntent) {
            if (notifyIntent == null) {
                Log.e(TAG,
                    "in registerPendingIntent() notifyIntent is null");
                return;
            }

            IzatService.SsrHandler.get().
                    updateClientPackageName(mContext, notifyIntent, true);

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null == clData) {
                clData = new ClientGeofenceData(notifyIntent);
                mDataPerPackageAndUser.setData(clData, notifyIntent);
            } else {
                clData.mPendingIntent = notifyIntent;
            }
        }

        public void unregisterPendingIntent(PendingIntent notifyIntent) {
            if (notifyIntent == null) {
                Log.e(TAG,
                    "in unregisterPendingIntent() notifyIntent is null");
                return;
            }

            IzatService.SsrHandler.get().
                    updateClientPackageName(mContext, notifyIntent, false);

            String callingPackageAndUser = notifyIntent.getCreatorPackage();
            if (VERBOSE) {
                Log.d(TAG,
                    "unregisterPendingIntent() for package:" + callingPackageAndUser);
            }
            removeAllGeofences(mDataPerPackageAndUser.getData());
        }

        public int addGeofence(double latitude,
                               double longitude,
                               double radius, // in meters
                               int transitionTypes,
                               int responsiveness, // in milliseconds
                               int confidence,
                               int dwellTime, // in seconds
                               int dwellTimeMask) {

            final int geofenceId = sGeofenceId++;

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                GeofenceData gfData = new GeofenceData(responsiveness, latitude, longitude,
                    radius, transitionTypes, confidence, dwellTimeMask, dwellTime,
                    null, null, geofenceId);
                synchronized (clData.mGeofenceMap) {
                    clData.mGeofenceMap.put(geofenceId, gfData);
                }
            }

            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): addGeofence()" +
                           "; Calling package is " +
                           mDataPerPackageAndUser.getPackageName(null) +
                           "; geofenceId is " + geofenceId +
                           "; latitude is " + latitude +
                           "; longitude is " + longitude +
                           "; radius is " + radius +
                           "; transitionTypes is " + transitionTypes +
                           "; responsiveness is " + responsiveness +
                           "; confidence is " + confidence +
                           "; dwellTime is " + dwellTime +
                           "; dwellTimeMask is " + dwellTimeMask);
            }
            mGeofenceServiceHidlClient.addGeofence(geofenceId, latitude, longitude, radius,
                                transitionTypes, responsiveness, confidence,
                                dwellTime, dwellTimeMask);
            return geofenceId;
        }


        public int addGeofenceObj(GeofenceData gfData) {
            if (gfData == null) {
                Log.e(TAG,
                    "in addGeofence() gfData is null");
                return -1;
            }

            double latitude = gfData.getLatitude();
            double longitude = gfData.getLongitude();
            double radius = gfData.getRadius();
            int transitionTypes = gfData.getTransitionType().getValue();
            int responsiveness = gfData.getNotifyResponsiveness();
            int confidence = gfData.getConfidence().getValue();
            int dwellTime = gfData.getDwellTime();
            int dwellTimeMask = gfData.getDwellType().getValue();
            Object appBundleData = gfData.getAppBundleData();
            String appTextData = gfData.getAppTextData();

            final int geofenceId = sGeofenceId++;

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                // geofence Id is provided by service, hence set it here.
                gfData.setGeofenceId(geofenceId);
                synchronized (clData.mGeofenceMap) {
                    clData.mGeofenceMap.put(geofenceId, gfData);
                }
            }

            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): addGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId +
                        "; latitude is " + latitude +
                        "; longitude is " + longitude +
                        "; radius is " + radius +
                        "; transitionTypes is " + transitionTypes +
                        "; responsiveness is " + responsiveness +
                        "; confidence is " + confidence +
                        "; dwellTime is " + dwellTime +
                        "; dwellTimeMask is " + dwellTimeMask +
                        "; appTextData is " + appTextData);
            }
            mGeofenceServiceHidlClient.addGeofence(geofenceId, latitude, longitude, radius,
                                transitionTypes, responsiveness, confidence,
                                dwellTime, dwellTimeMask);
            return geofenceId;
        }

        public void removeGeofence(int geofenceId) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): removeGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                synchronized (clData.mGeofenceMap) {
                    clData.mGeofenceMap.remove(Integer.valueOf(geofenceId));
                }
            }

            mGeofenceServiceHidlClient.removeGeofence(geofenceId);
        }

        public void updateGeofence(int geofenceId,
                                   int transitionTypes,
                                   int notifyResponsiveness) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): updateGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    gfData.setTransitionType(transitionTypes);
                    gfData.setNotifyResponsiveness(notifyResponsiveness);
                }
            }

            mGeofenceServiceHidlClient.updateGeofence(geofenceId,
                                   transitionTypes,
                                   notifyResponsiveness);
        }

        public void updateGeofenceData(int geofenceId,
                                       double latitude,
                                       double longitude,
                                       double radius,
                                       int transitionTypes,
                                       int responsiveness,
                                       int confidence,
                                       int dwellTime,
                                       int dwellTimeMask,
                                       int updatedFieldsFlags) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): updateGeofenceData()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId +
                        "; latitude is " + latitude +
                        "; longitude is " + longitude +
                        "; radius is " + radius +
                        "; transitionTypes is " + transitionTypes +
                        "; responsiveness is " + responsiveness +
                        "; confidence is " + confidence +
                        "; dwellTime is " + dwellTime +
                        "; dwellTimeMask is " + dwellTimeMask +
                        "; updatedFieldsFlags is " + updatedFieldsFlags);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData =
                        clData.mGeofenceMap.get(Integer.valueOf(geofenceId));

                    if ((updatedFieldsFlags & GeofenceData.TRANSITION_TYPES_IS_SET) != 0) {
                        gfData.setTransitionType(transitionTypes);
                    }
                    if ((updatedFieldsFlags & GeofenceData.RESPONSIVENESS_IS_SET) != 0) {
                        gfData.setNotifyResponsiveness(responsiveness);
                    }
                    if ((updatedFieldsFlags & GeofenceData.LATITUDE_IS_SET) != 0) {
                        gfData.setLatitude(latitude);
                    }
                    if ((updatedFieldsFlags & GeofenceData.LONGITUDE_IS_SET) != 0) {
                        gfData.setLongitude(longitude);
                    }
                    if ((updatedFieldsFlags & GeofenceData.RADIUS_IS_SET) != 0) {
                        gfData.setRadius(radius);
                    }
                    if ((updatedFieldsFlags & GeofenceData.TRANSITION_TYPES_IS_SET) != 0) {
                        gfData.setTransitionType(transitionTypes);
                    }
                    if ((updatedFieldsFlags & GeofenceData.CONFIDENCE_IS_SET) != 0) {
                        gfData.setConfidence(confidence);
                    }
                    if ((updatedFieldsFlags & GeofenceData.DWELL_NOTIFY_IS_SET) != 0) {
                        gfData.setDwellTime(dwellTime);
                        gfData.setDwellType(dwellTimeMask);
                    }

                    mGeofenceServiceHidlClient.removeGeofence(geofenceId);
                    mGeofenceServiceHidlClient.addGeofence(geofenceId, latitude, longitude, radius,
                            transitionTypes, responsiveness, confidence,
                            dwellTime, dwellTimeMask);

                    if (gfData.isPaused()) {
                        mGeofenceServiceHidlClient.pauseGeofence(geofenceId);
                    }

                } else {
                    Log.e(TAG, "Geofence id not found: " + geofenceId);
                }
            }
        }

        public void pauseGeofence(int geofenceId) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): pauseGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    gfData.setPausedStatus(true);
                    mGeofenceServiceHidlClient.pauseGeofence(geofenceId);
                }
            }
        }

        public void resumeGeofence(int geofenceId) {
            if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): resumeGeofence()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null) +
                        "; geofenceId is " + geofenceId);
            }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData.GeofenceTransitionTypes transitionTypes;
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    transitionTypes = gfData.getTransitionType();
                    gfData.setPausedStatus(false);

                    // resume the geofence with the original transition type
                    mGeofenceServiceHidlClient.resumeGeofence(geofenceId, transitionTypes.getValue());
                }
            }
        }

        public void recoverGeofences(List<GeofenceData> gfList) {
             if (VERBOSE) {
                Log.d(TAG, "in IGeofenceService.Stub(): recoverGeofences()" +
                        "; Calling package is " +
                        mDataPerPackageAndUser.getPackageName(null));
             }

            ClientGeofenceData clData = mDataPerPackageAndUser.getData();
            if (null != clData) {
                gfList.addAll(clData.mGeofenceMap.values());
             }
        }
    };

    public void reportGeofenceTransition(int geofenceId,
                                          int transition,
                                          Location location) {
        if (VERBOSE) {
            Log.d(TAG, "reportGeofenceTransition id : " + geofenceId +
                    "; transition : " + transition);
        }
        location.setElapsedRealtimeNanos(SystemClock.elapsedRealtimeNanos());
        synchronized (sCallBacksLock) {
            for (ClientGeofenceData clData : mDataPerPackageAndUser.getAllData()) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    GeofenceData gfData = clData.mGeofenceMap.get(Integer.valueOf(geofenceId));
                    if (gfData.isPaused()) {
                        if (VERBOSE) {
                            Log.w(TAG, "Breach event with paused geofence: " + geofenceId);
                        }
                        break;
                    }

                    if (VERBOSE) {
                        Log.d(TAG, "Sending breach event to: " + geofenceId);
                    }

                    try {
                        if (clData.mPendingIntent != null) {
                            Intent pdIntent = new Intent();
                            String gfCtxStringData = gfData.getAppTextData();
                            if (gfCtxStringData != null) {
                                pdIntent.putExtra("context-data", gfCtxStringData);
                            } else {
                                Bundle gfCtxBundleData = gfData.getAppBundleData();
                                if (gfCtxBundleData != null) {
                                    pdIntent.putExtra("context-data", gfCtxBundleData);
                                }
                            }
                            pdIntent.putExtra("transition-location", location);
                            pdIntent.putExtra("transition-event", transition);

                            clData.mPendingIntent.send(mContext, 0, pdIntent);
                        }
                    } catch (PendingIntent.CanceledException e) {
                        clData.mPendingIntent = null;
                    }

                    try {
                        if (clData.mCallback != null) {
                            clData.mCallback.onTransitionEvent(geofenceId,
                                                               transition,
                                                               location);
                        }
                    } catch (RemoteException e) {
                        clData.mCallback = null;
                    }

                    if ((clData.mPendingIntent == null) &&
                        (clData.mCallback == null)) {
                        removeAllGeofences(clData);
                    }

                    break;
                }
            };
        }
    }

    public void reportGeofenceRequestStatus(int requestType,
                                             int geofenceId,
                                             int status) {
        if (VERBOSE) {
            Log.d(TAG, "reportGeofenceRequestStatus requestType: " +
                    requestType + "; id : " + geofenceId +
                    "; status : " + status);
        }
        synchronized (sCallBacksLock) {
            for (ClientGeofenceData clData : mDataPerPackageAndUser.getAllData()) {
                if (clData.mGeofenceMap.containsKey(geofenceId)) {
                    try {
                        clData.mCallback.onRequestResultReturned(geofenceId,
                                                                 requestType,
                                                                 status);

                        if ((GEOFENCE_REQUEST_TYPE_ADD == requestType) &&
                                (GEOFENCE_RESULT_SUCCESS != status)) {
                            synchronized (clData.mGeofenceMap) {
                                clData.mGeofenceMap.remove(Integer.valueOf(geofenceId));
                            }
                        }
                    } catch (RemoteException e) {
                        // do nothing
                    }
                    return;
                }
            };
        }
    }

    public void reportGeofenceStatus(int status,
                                      Location location) {
        if (VERBOSE) {
            Log.d(TAG, "reportGeofenceStatus - status : " + status);
        }
        synchronized (sCallBacksLock) {
            int index = mGeofenceCallbacks.beginBroadcast();
            for (int i = 0; i < index; i++) {
                try {
                    mGeofenceCallbacks.getBroadcastItem(i).onEngineReportStatus(status, location);
                } catch (RemoteException e) {
                    // do nothing
                }
            }
            mGeofenceCallbacks.finishBroadcast();
        }
    }

    private void removeAllGeofences(ClientGeofenceData clData)
    {
        if (VERBOSE) {
            Log.d(TAG, "removing all geofences for package and user: " +
                    clData.mPackageName);
        }
        synchronized (clData.mGeofenceMap) {
            for(Integer key : clData.mGeofenceMap.keySet()) {
                mGeofenceServiceHidlClient.removeGeofence(key);
            }
            clData.mGeofenceMap.clear();
        }

        mDataPerPackageAndUser.removeData(clData);
    }

    public IGeofenceService getGeofenceBinder() {
        return mBinder;
    }

    class UserChanged implements DataPerPackageAndUser.UserChangeListener<ClientGeofenceData> {
        @Override
        public void onUserChange(Map<String, ClientGeofenceData> prevUserData,
                                 Map<String, ClientGeofenceData> currentUserData) {
            if (VERBOSE) {
                Log.d(TAG, "Active user has changed, updating geofences...");
            }

            // Remove prevUser geofences
            for (ClientGeofenceData geofenceData: prevUserData.values()) {
                synchronized (geofenceData.mGeofenceMap) {
                    for(GeofenceData geofence: geofenceData.mGeofenceMap.values()) {
                        if (VERBOSE) {
                            Log.d(TAG, "Removing geofence id: " + geofence.getGeofenceId());
                        }
                        mGeofenceServiceHidlClient.removeGeofence(geofence.getGeofenceId());
                    }
                }
            }

            // Add back current user Geofences
            for (ClientGeofenceData geofenceData: currentUserData.values()) {
                synchronized (geofenceData.mGeofenceMap) {
                    for(GeofenceData geofence: geofenceData.mGeofenceMap.values()) {
                        if (VERBOSE) {
                            Log.d(TAG, "Adding geofence id: " + geofence.getGeofenceId());
                        }
                        mGeofenceServiceHidlClient.addGeofence(geofence.getGeofenceId(),
                                geofence.getLatitude(), geofence.getLongitude(),
                                geofence.getRadius(),
                                geofence.getTransitionType().getValue(),
                                geofence.getNotifyResponsiveness(),
                                geofence.getConfidence().getValue(),
                                geofence.getDwellTime(),
                                geofence.getDwellType().getValue());
                    }
                }
            }
        }
    }

    /**
     * Called from native code to report HIDL death
     */
    public void reportServiceDied() {
        if (VERBOSE) {
            Log.d(TAG, "GeofenceServiceDied, initializing...");
        }

        // Restart all geofences
        for (ClientGeofenceData clData : mDataPerPackageAndUser.getAllData()) {
            for(Integer geofenceId : clData.mGeofenceMap.keySet()) {
                GeofenceData gfData = clData.mGeofenceMap.get(geofenceId);

                mGeofenceServiceHidlClient.removeGeofence(geofenceId);
                mGeofenceServiceHidlClient.addGeofence(geofenceId, gfData.getLatitude(), gfData.getLongitude(),
                        gfData.getRadius(), gfData.getTransitionType().getValue(),
                        gfData.getNotifyResponsiveness(), gfData.getConfidence().getValue(),
                        gfData.getDwellTime(), gfData.getDwellType().getValue());

                if (gfData.isPaused()) {
                    mGeofenceServiceHidlClient.pauseGeofence(geofenceId);
                }
            }
        }
    }

    /* =================================================
     *   HIDL Client
     * =================================================*/
    private class GeofenceServiceHidlClient extends BaseHidlClient
            implements BaseHidlClient.IServiceDeathCb {
        private static final int LOC_GPS_GEOFENCE_ENTERED        = 1<<0;
        private static final int LOC_GPS_GEOFENCE_EXITED         = 1<<1;
        private static final int LOC_GPS_GEOFENCE_UNCERTAIN      = 1<<2;
        private static final int LOC_GPS_GEOFENCE_DWELL_INSIDE   = 1<<3;
        private static final int LOC_GPS_GEOFENCE_DWELL_OUTSIDE  = 1<<4;

        private ILocHidlGeofenceService mGeofenceService;
        private GeofenceServiceProvider mGeofenceServiceProvider;
        private ILocHidlGeofenceServiceCallback mGeofenceServiceCallback =
                new GeofenceServiceCallback();

        private ILocHidlGeofenceService getGeofenceServiceIface() {
            if (null == mGeofenceService) {
                ILocHidlGnss service = (ILocHidlGnss)getGnssService();
                if (null == service) {
                    Log.e(TAG, "ILocHidlGnss handle is null");
                    return null;
                }
                Log.d(TAG, "ILocHidlGnss.getService(), service = " + service);
                try {
                    if (service instanceof vendor.qti.gnss.V1_1.ILocHidlGnss) {
                        mGeofenceService = service.getExtensionLocHidlGeofenceService_1_1();
                    } else {
                        mGeofenceService = service.getExtensionLocHidlGeofenceService();
                    }
                    Log.d(TAG, "getGeofenceServiceIface, mGeofenceService=" +
                            mGeofenceService);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception getting geofence service extension: " + e);
                }
                if (null == mGeofenceService) {
                    Log.e(TAG, "ILocHidlGeofenceService handle is null");
                    return null;
                }
            }

            return mGeofenceService;
        }
        private GeofenceServiceHidlClient(GeofenceServiceProvider provider) {
            getGeofenceServiceIface();
            mGeofenceServiceProvider = provider;
            try {
                boolean ret = mGeofenceService.init(mGeofenceServiceCallback);
                if (!ret) {
                    Log.e(TAG, "Failed to init LocHidlGeofenceService");
                }
                registerServiceDiedCb(this);
                Log.d(TAG, "GeofenceServiceHidlClient hidl init");
            } catch (RemoteException e) {
                Log.e(TAG, "Exception init LocHidlGeofenceService: " + e);
            }
        }

        private int translateHidlLocError(int locationErr) {
            int geofenceResult = GEOFENCE_RESULT_ERROR_GENERIC;
            switch (locationErr) {
                case 0:
                    geofenceResult = GEOFENCE_RESULT_SUCCESS;
                    break;
                case 3:
                    geofenceResult = GEOFENCE_RESULT_ERROR_INVALID_TRANSITION;
                    break;
                case 4:
                    geofenceResult = GEOFENCE_RESULT_ERROR_ID_EXISTS;
                    break;
                case 5:
                    geofenceResult = GEOFENCE_RESULT_ERROR_ID_UNKNOWN;
                    break;
                case 7:
                    geofenceResult = GEOFENCE_RESULT_ERROR_TOO_MANY_GEOFENCES;
                    break;
                default:
                    geofenceResult = GEOFENCE_RESULT_ERROR_GENERIC;
                    break;
            }
            return geofenceResult;
        }

        private class GeofenceServiceCallback extends ILocHidlGeofenceServiceCallback.Stub {
            @Override
            public void gnssGeofenceBreachCb(int count, ArrayList<Integer> idList,
                    vendor.qti.gnss.V1_0.LocHidlLocation location, int breachType,
                    long timestamp) throws android.os.RemoteException {
                HidlClientUtils.fromHidlService(TAG);
                int transition = LOC_GPS_GEOFENCE_UNCERTAIN;
                for (int i = 0; i < count; i++) {
                    switch (breachType) {
                        case 0:
                            transition = LOC_GPS_GEOFENCE_ENTERED;
                            break;
                        case 1:
                            transition = LOC_GPS_GEOFENCE_EXITED;
                            break;
                        case 2:
                            transition = LOC_GPS_GEOFENCE_DWELL_INSIDE;
                            break;
                        case 3:
                            transition = LOC_GPS_GEOFENCE_DWELL_OUTSIDE;
                            break;
                        default:
                            transition = LOC_GPS_GEOFENCE_UNCERTAIN;
                            break;
                    }
                    Location locationObj = HidlClientUtils.translateHidlLocation(location);

                    mGeofenceServiceProvider.reportGeofenceTransition(idList.get(i),
                            transition, locationObj);
                }
            }
            @Override
            public void gnssGeofenceStatusCb(int statusAvailable, int locTechType)
                throws android.os.RemoteException {
                    HidlClientUtils.fromHidlService(TAG);
                    int status = 1 << statusAvailable;
                    mGeofenceServiceProvider.reportGeofenceStatus(status, null);
                }

            @Override
            public void gnssAddGeofencesCb(int count, ArrayList<Integer> locationErrorList,
                    ArrayList<Integer> idList) throws android.os.RemoteException {
                HidlClientUtils.fromHidlService(TAG);
                for (int i = 0; i < count; i++) {
                    int status = translateHidlLocError(locationErrorList.get(i));
                    mGeofenceServiceProvider.reportGeofenceRequestStatus(
                            GEOFENCE_REQUEST_TYPE_ADD, idList.get(i), status);
                }
            }

            @Override
            public void gnssRemoveGeofencesCb(int count, ArrayList<Integer> locationErrorList,
                    ArrayList<Integer> idList) throws android.os.RemoteException {
                HidlClientUtils.fromHidlService(TAG);
                for (int i = 0; i < count; i++) {
                    int status = translateHidlLocError(locationErrorList.get(i));
                    mGeofenceServiceProvider.reportGeofenceRequestStatus(
                            GEOFENCE_REQUEST_TYPE_REMOVE, idList.get(i), status);
                }
            }

            @Override
            public void gnssPauseGeofencesCb(int count, ArrayList<Integer> locationErrorList,
                    ArrayList<Integer> idList) throws android.os.RemoteException {
                HidlClientUtils.fromHidlService(TAG);
                for (int i = 0; i < count; i++) {
                    int status = translateHidlLocError(locationErrorList.get(i));
                    mGeofenceServiceProvider.reportGeofenceRequestStatus(
                            GEOFENCE_REQUEST_TYPE_PAUSE, idList.get(i), status);
                }
            }

            @Override
            public void gnssResumeGeofencesCb(int count, ArrayList<Integer> locationErrorList,
                    ArrayList<Integer> idList) throws android.os.RemoteException {
                HidlClientUtils.fromHidlService(TAG);
                for (int i = 0; i < count; i++) {
                    int status = translateHidlLocError(locationErrorList.get(i));
                    mGeofenceServiceProvider.reportGeofenceRequestStatus(
                            GEOFENCE_REQUEST_TYPE_RESUME, idList.get(i), status);
                }
            }
        }

        @Override
        public void onServiceDied() {
            Log.e(TAG, "ILocHidlGeofenceService died");
            mGeofenceService = null;
            ILocHidlGeofenceService geofenceIface = getGeofenceServiceIface();
            try {
                geofenceIface.init(mGeofenceServiceCallback);
                mGeofenceServiceProvider.reportServiceDied();
            } catch (RemoteException e) {
                Log.e(TAG, "Exception GeofenceService serviceDied: " + e);
            }
        }

        public void addGeofence(int id, double latitude, double longitude, double radius,
                int transitionTypes, int responsiveness, int confidence, int dwellTime,
                int dwellTimeMask) {
            HidlClientUtils.toHidlService(TAG);
            if (null != mGeofenceService) {
                try {
                    ILocHidlGeofenceService geofenceIface = getGeofenceServiceIface();
                    geofenceIface.addGeofence(id, latitude, longitude, radius, transitionTypes,
                            responsiveness, confidence, dwellTime, dwellTimeMask);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception addGeofence: " + e);
                }
            }
        }

        public void removeGeofence(int id) {
            HidlClientUtils.toHidlService(TAG);
            if (null != mGeofenceService) {
                try {
                    ILocHidlGeofenceService geofenceIface = getGeofenceServiceIface();
                    geofenceIface.removeGeofence(id);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception removeGeofence: " + e);
                }
            }
        }

        public void updateGeofence(int id, int transitionTypes, int responsiveness) {
            HidlClientUtils.toHidlService(TAG);
            if (null != mGeofenceService) {
                try {
                    ILocHidlGeofenceService geofenceIface = getGeofenceServiceIface();
                    geofenceIface.updateGeofence(id, transitionTypes, responsiveness);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception updateGeofence: " + e);
                }
            }
        }

        public void pauseGeofence(int id) {
            HidlClientUtils.toHidlService(TAG);
            if (null != mGeofenceService) {
                try {
                    ILocHidlGeofenceService geofenceIface = getGeofenceServiceIface();
                    geofenceIface.pauseGeofence(id);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception pauseGeofence: " + e);
                }
            }
        }

        public void resumeGeofence(int id, int transitionTypes) {
            HidlClientUtils.toHidlService(TAG);
            if (null != mGeofenceService) {
                try {
                    ILocHidlGeofenceService geofenceIface = getGeofenceServiceIface();
                    geofenceIface.resumeGeofence(id, transitionTypes);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception resumeGeofence: " + e);
                }
            }
        }
    }
}
