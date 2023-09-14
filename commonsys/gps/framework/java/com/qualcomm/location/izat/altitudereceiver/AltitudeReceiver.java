/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.location.izat.altitudereceiver;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.SystemClock;
import android.util.Log;
import android.os.Handler;
import android.os.Message;

import com.qti.altitudereceiver.*;

import com.qualcomm.location.izat.IzatService;
import com.qualcomm.location.izat.GTPClientHelper;
import com.qualcomm.location.utils.IZatServiceContext;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Iterator;
import java.util.concurrent.TimeUnit;
import java.lang.InterruptedException;
import java.lang.Runnable;
import java.lang.Thread;

public class AltitudeReceiver implements IzatService.ISystemEventListener, Handler.Callback {
    private static final String TAG = "AltitudeReceiver";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    private final Context mContext;
    private Handler mHandler;

    private static final int ALTITUDE_LOOK_UP = IZatServiceContext.MSG_ALTITUDE_RECEIVER +1;
    private static final int REPORT_ALTITUDE = IZatServiceContext.MSG_ALTITUDE_RECEIVER +2;
    private static final int LOOK_UP_TIME_OUT = IZatServiceContext.MSG_ALTITUDE_RECEIVER +3;
    private static final int SET_ALTITUDE_CB = IZatServiceContext.MSG_ALTITUDE_RECEIVER +4;
    private static final int RECOVER_QUERY_ALTITUDE_STATUS =
            IZatServiceContext.MSG_ALTITUDE_RECEIVER +5;

    private static final int TIME_OUT_IN_MS = 5000;
    private static final int TIME_WAIT_FOR_RECOVER = 60000;
    private static final int NUM_THRESHOLD_TO_STOP_QUERY = 10;

    private boolean mHasAltStatusForPreviousFix = true;
    private int mContinuousFailedFixesNum = 1;
    private boolean mStopQueryStatus = false;

    private IAltitudeReceiverResponseListener mAltitudeReceiverResponseListener = null;
    private PendingIntent mListenerIntent = null;
    private static final Object sCallBacksLock = new Object();

    private IAltitudeReportCb mAltitudeReportCb;
    private List<Location> mLocationsQueue;

    private static AltitudeReceiver sInstance = null;
    public static AltitudeReceiver getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new AltitudeReceiver(ctx);
        }
        return sInstance;
    }

    private AltitudeReceiver(Context ctx) {
        if (VERBOSE) {
            Log.d(TAG, "AltitudeReceiver construction");
        }
        mContext = ctx;
        mHandler = new Handler(IZatServiceContext.getInstance(mContext).getLooper(), this);
        mLocationsQueue = new LinkedList<Location>();
    }

    @Override
    public void onAidlClientDied(String packageName) {
        Log.d(TAG, "aidl client crash: " + packageName);
        synchronized (sCallBacksLock) {
            mAltitudeReceiverResponseListener = null;
        }
    }

    @Override
    public boolean handleMessage(Message msg) {
        int msgID = msg.what;
        Location loc;
        switch(msgID) {
            case ALTITUDE_LOOK_UP:
                loc = (Location)msg.obj;
                if (mStopQueryStatus) {
                    Log.d(TAG, "failed to get altitude more than 10 times, directly return");
                    //Report all the fixes in queue
                    if ( 0 != mLocationsQueue.size()) {
                        Iterator<Location> iter = mLocationsQueue.iterator();
                        while(iter.hasNext()) {
                            Location item = iter.next();
                            if (mAltitudeReportCb != null) {
                                mAltitudeReportCb.onLocationReportWithAlt(item, false);
                            }
                        }
                        mLocationsQueue.clear();
                    }

                    if (mAltitudeReportCb != null) {
                        mAltitudeReportCb.onLocationReportWithAlt(loc, false);
                    }
                } else {
                    mLocationsQueue.add(loc);
                    boolean isEmergency = (msg.arg1 == 1);
                    synchronized (sCallBacksLock) {
                        try {
                            Location newLoc = new Location(loc);
                            newLoc.setProvider("gps");
                            //Remove the altitude flag before sending it to 3rd party Z-Provider
                            newLoc.removeAltitude();
                            mAltitudeReceiverResponseListener
                                    .onAltitudeLookupRequest(newLoc, isEmergency);
                        } catch (RemoteException e) {
                            Log.e(TAG, "error happens when try to get altitude, sending intent");
                            GTPClientHelper.SendPendingIntent(mContext, mListenerIntent,
                                    "AltitudeReceiver");
                        }
                    }
                }
                break;
            case REPORT_ALTITUDE:
                    //Get location object in response from 3rd party Z-Provider,
                    //1, Report all the locations in queue whose timestamp is smaller
                    //   than the timestamp of location in response;
                    //2, If Altitude is valid in location in response, set the altitude
                    //   in latest location in queue and its time stamp is smaller than
                    //   the timestamp of location in response.
                    //3, Remove all reported locations in queue.
                    loc = (Location)msg.obj;
                    int i = 0;
                    for(; i < mLocationsQueue.size(); i++) {
                        Location item = mLocationsQueue.get(i);
                        if (item.getElapsedRealtimeNanos() > loc.getElapsedRealtimeNanos()) {
                            // If first element in queue's timestamp is larger
                            // than the timestamp of location in response,
                            // Do nothing, set i = -1 to avoid remove any element in queue
                            i = -1;
                            break;
                        } else if ((item.getElapsedRealtimeNanos() < loc.getElapsedRealtimeNanos())
                                && (i < (mLocationsQueue.size() - 1))
                                && (mLocationsQueue.get(i+1).getElapsedRealtimeNanos()
                                <= loc.getElapsedRealtimeNanos())) {
                            reportAltitude(item, false);
                        } else if (i == (mLocationsQueue.size() - 1) ||
                                (item.getElapsedRealtimeNanos() <= loc.getElapsedRealtimeNanos())
                                && (mLocationsQueue.get(i+1).getElapsedRealtimeNanos() >
                                 loc.getElapsedRealtimeNanos())) {
                            if (loc.hasAltitude()) {
                                item.setAltitude(loc.getAltitude());
                            }
                            if (loc.hasVerticalAccuracy()) {
                                item.setVerticalAccuracyMeters(loc.getVerticalAccuracyMeters());
                            }
                            reportAltitude(item, loc.hasAltitude());
                            break;
                        }
                    }
                    //Remove reported elements
                    if (i >= 0 && i < mLocationsQueue.size()) {
                        mLocationsQueue.subList(0, i+1).clear();
                    }
                break;
            case LOOK_UP_TIME_OUT:
                if (mLocationsQueue.size() > 0) {
                    loc = (Location)msg.obj;
                    Location cachedLoc = mLocationsQueue.get(0);
                    if (cachedLoc.getElapsedRealtimeNanos() == loc.getElapsedRealtimeNanos()) {
                        reportAltitude(loc, false);
                        mLocationsQueue.remove(0);
                    }
                }
                break;
            case SET_ALTITUDE_CB:
                Log.d(TAG, "AltitudeReceiver SetCallback");
                if (null == msg.obj) {
                    mLocationsQueue.clear();
                }
                mAltitudeReportCb = (IAltitudeReportCb)msg.obj;
            case RECOVER_QUERY_ALTITUDE_STATUS:
                Log.d(TAG, "AltitudeReceiver status reset");
                mStopQueryStatus = false;
                mContinuousFailedFixesNum = 1;
                mHasAltStatusForPreviousFix = true;
                break;
        }
        return true;
    }

    //If 3rd party Z-Provider returns invalid altitude or timeout
    //for 10 continuous fixes, stop to query Altitude in 60s and then recover
    private void reportAltitude(Location loc, boolean status) {
        if (mAltitudeReportCb != null) {
            Log.d(TAG, "onLocationReportWithAlt, location: " + loc.toString()
                    + ", status: " + status);
            mAltitudeReportCb.onLocationReportWithAlt(loc, status);
        }
        if (!status && (mHasAltStatusForPreviousFix == status)) {
            mContinuousFailedFixesNum++;
        } else {
            mContinuousFailedFixesNum = 1;
        }
        mHasAltStatusForPreviousFix = status;
        if (mContinuousFailedFixesNum >= NUM_THRESHOLD_TO_STOP_QUERY) {
            mStopQueryStatus = true;
            mContinuousFailedFixesNum = 1;

            mHandler.sendMessageDelayed(mHandler.obtainMessage(RECOVER_QUERY_ALTITUDE_STATUS),
                    TIME_WAIT_FOR_RECOVER);
        }
    }

    public interface IAltitudeReportCb {
        void onLocationReportWithAlt(Location location, boolean isAltitudeAvail);
    }

    public void setAltitudeCallback(IAltitudeReportCb callback) {
        mHandler.obtainMessage(SET_ALTITUDE_CB, callback).sendToTarget();
    }

    public boolean isPresent() {
        return !(mAltitudeReceiverResponseListener == null);
    }

    public void getAltitudeFromLocation(Location loc, boolean isEmergency) {
        mHandler.obtainMessage(ALTITUDE_LOOK_UP, isEmergency ? 1:0, 0, loc).sendToTarget();
        mHandler.sendMessageDelayed(mHandler.obtainMessage(LOOK_UP_TIME_OUT, loc), TIME_OUT_IN_MS);
    }

    /* Remote binder */
    private final IAltitudeReceiver.Stub mBinder = new IAltitudeReceiver.Stub() {

        @Override
        public boolean registerResponseListener(IAltitudeReceiverResponseListener callback,
                android.app.PendingIntent notifyIntent) throws android.os.RemoteException {
            Log.d(TAG, "registerResponseListener");
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return false;
            }

            if (notifyIntent == null) {
                Log.w(TAG, "notifyIntent is null");
            }

            if (null != mAltitudeReceiverResponseListener) {
                Log.e(TAG, "Response listener already provided.");
                return false;
            }

            synchronized (sCallBacksLock) {
                mAltitudeReceiverResponseListener = callback;
                mListenerIntent = notifyIntent;
            }
            return true;
        }

        @Override
        public boolean removeResponseListener(IAltitudeReceiverResponseListener callback)
                throws android.os.RemoteException {
            Log.d(TAG, "removeResponseListener");
            synchronized (sCallBacksLock) {
                mAltitudeReceiverResponseListener = null;
                mListenerIntent = null;
            }
            return true;
        }

        @Override
        public void pushAltitude(Location location) throws android.os.RemoteException {
            Log.d(TAG, "pushAltitude, location: " + location.toString());
            mHandler.obtainMessage(REPORT_ALTITUDE, location).sendToTarget();
        }
    };


    public IAltitudeReceiver getAltitudeReceiverBinder() {
        return mBinder;
    }
}
