/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp.service;

import android.os.Handler;
import android.os.Looper;
import android.content.Context;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;
import java.util.HashMap;
import java.util.Map;

import com.android.ims.internal.uce.common.CapInfo;
import com.android.ims.internal.uce.common.UceLong;
import com.android.ims.internal.uce.options.IOptionsService;
import com.android.ims.internal.uce.options.OptionsCapInfo;
import com.android.ims.internal.uce.presence.IPresenceService;
import com.android.ims.internal.uce.presence.PresCapInfo;
import com.android.ims.internal.uce.uceservice.IUceService;
import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.qualcomm.qti.PresenceApp.CapabilityInfo;
import com.qualcomm.qti.PresenceApp.MainActivity;

public class  UceService {
    private static IUceService mService = null;
    private static Map<Integer, PresenceServiceWrapper> presenceServices = new HashMap<Integer, PresenceServiceWrapper>();
    private static Map<Integer, OptionsServiceWrapper> optionsServices = new HashMap<Integer, OptionsServiceWrapper>();
    private static int iUceManagerId = 11001;
    private static int mUserData = 11002;
    private static String LOG_TAG = MainActivity.LOG_TAG + ":UceServiceWrapper";
    public static Map<Integer, Boolean> isServiceInit = new HashMap<Integer, Boolean>();

    private class PresenceServiceWrapper {
        PresenceListener mListener;
        IPresenceService mPresence;
        int mServiceHandle;
        UceLong mClientHandle = new UceLong();
        public PresenceServiceWrapper(int instance) {
            mClientHandle.setUceLong((iUceManagerId << instance) | instance);
            mListener = new PresenceListener(instance);
        }
    };

    private class OptionsServiceWrapper {
        OptionsListener mListener;
        int mServiceHandle;
        IOptionsService mOptions;
        UceLong mClientHandle = new UceLong();
        public OptionsServiceWrapper(int instance) {
            mClientHandle.setUceLong((iUceManagerId << instance) | instance);
            mListener = new OptionsListener(instance);
        }
    };

    private int getUserData () {
        return mUserData++;
    }

    public void configureForSlot(int slotId) {
        /*presenceServices = new PresenceServiceWrapper[maxSubsSupported];
        optionsServices = new OptionsServiceWrapper[maxSubsSupported];
        isServiceInit = new boolean[maxSubsSupported];*/
    }
    public boolean initialize(Context ctx, int instance, String IccId, TaskListener publishTaskListener) {
        Log.d(LOG_TAG, "Initialize Start");
        final ImsUceManager uceManager = ImsUceManager.getInstance(ctx);
        if(uceManager == null){
            Log.e(LOG_TAG, "UceManager is NULL");
            return false;
        }
        uceManager.createUceService(false);
        mService = uceManager.getUceServiceInstance();
        if(mService == null) {
            Log.e(LOG_TAG, "Create UceService Failed");
            return false;
        }
        try {
            if(presenceServices.get(instance) == null) {
                presenceServices.put(instance, new PresenceServiceWrapper(instance));
            }
            presenceServices.get(instance).mServiceHandle = mService.createPresenceServiceForSubscription(
                    presenceServices.get(instance).mListener, presenceServices.get(instance).mClientHandle, IccId);
            presenceServices.get(instance).mPresence = mService.getPresenceServiceForSubscription(IccId);
            presenceServices.get(instance).mListener.setPublishTaskListener(publishTaskListener);
            Log.d(LOG_TAG, "PresenceServiceHandle [" +
                    presenceServices.get(instance).mServiceHandle+"] for Instance["+instance+"]");
        } catch (RemoteException e) {
            Log.e(LOG_TAG, "Create Presence Failed");
            return false;
        }

        try {
            if(optionsServices.get(instance) == null) {
                optionsServices.put(instance, new OptionsServiceWrapper(instance));
            }
            optionsServices.get(instance).mServiceHandle = mService.createOptionsServiceForSubscription(
                    optionsServices.get(instance).mListener, optionsServices.get(instance).mClientHandle, IccId);
            optionsServices.get(instance).mOptions = mService.getOptionsServiceForSubscription(IccId);
            Log.d(LOG_TAG, "OptionsServiceHandle [" +
                    optionsServices.get(instance).mServiceHandle+"] for Instance["+instance+"]");

        } catch (RemoteException e) {
            Log.e(LOG_TAG, "Create options Failed");
            return  false;
        }
        isServiceInit.put(instance, true);
        return true;
    }

    public void release(int instance) {
        try {
            Log.i(LOG_TAG, "destroy Presence start");
            if(presenceServices.get(instance) == null) {
              Log.e(LOG_TAG, "destroy Presence return since instance does not exist");
              return;
            } else if(presenceServices.get(instance).mPresence != null) {
                presenceServices.get(instance).mPresence.removeListener(
                    presenceServices.get(instance).mServiceHandle,
                    presenceServices.get(instance).mClientHandle
                );
            }
            mService.destroyPresenceService(presenceServices.get(instance).mServiceHandle);
        }catch (RemoteException | RuntimeException e) {
            Log.e(LOG_TAG, "destroy Presence Failed");
        }

        try {
            Log.i(LOG_TAG, "destroy Options start");
            if(optionsServices.get(instance) == null) {
              Log.e(LOG_TAG, "destroy Presence return since instance does not exist");
              return;
            } else if(optionsServices.get(instance).mOptions != null) {
                optionsServices.get(instance).mOptions.removeListener(
                    optionsServices.get(instance).mServiceHandle,
                    optionsServices.get(instance).mClientHandle
                );
            }
            mService.destroyOptionsService(optionsServices.get(instance).mServiceHandle);
        }catch (RemoteException | RuntimeException e) {
            Log.e(LOG_TAG, "destroy Options Failed");
        }
        presenceServices.remove(instance);
        optionsServices.remove(instance);
        isServiceInit.remove(instance);
        postServiceDown(instance);
    }
    private void postServiceDown(int instance) {
        Handler handler = new Handler(Looper.getMainLooper());
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.mCtx,
                "UCE Service Down for slot: "+instance,
                Toast.LENGTH_SHORT).show();
            }
        };
        handler.post(msg);
    }

    public void publish(int instance, CapInfo caps) {
        Log.e(LOG_TAG, "Publish Triggered");
        if(!isServiceInit.get(instance)) {
            postServiceDown(instance);
            return;
        }
        PresCapInfo presCaps = new PresCapInfo();
        presCaps.setCapInfo(caps);

        if(presenceServices.get(instance) == null || optionsServices.get(instance) == null) {
            Log.e(LOG_TAG, "publish : instance does not exist");
            return;
        }

        try {
            presenceServices.get(instance).mPresence.publishMyCap(presenceServices.get(instance).mServiceHandle,
                    presCaps, getUserData());
            optionsServices.get(instance).mOptions.setMyInfo(optionsServices.get(instance).mServiceHandle,
                    caps, getUserData());
        } catch (RemoteException e) {
            Log.e(LOG_TAG, "Publish Failed with Exception");
            e.printStackTrace();
        }
    }

    public void availabilityFetch(int instance, String remoteContact, TaskListener Listener) {
        Log.e(LOG_TAG, "availabilityFetch Triggered");
        if(!isServiceInit.get(instance)) {
            postServiceDown(instance);
            return;
        }
        if(presenceServices.get(instance) == null || optionsServices.get(instance) == null) {
            Log.e(LOG_TAG, "availabilityFetch : instance does not exist");
            return;
        }

        try {
            presenceServices.get(instance).mPresence.getContactCap(presenceServices.get(instance).mServiceHandle,
                    remoteContact, getUserData());
            presenceServices.get(instance).mListener.setSubscribeTaskListener(Listener);
        }catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void capabilityPolling(int instance, String[] uris,TaskListener Listener) {
        Log.e(LOG_TAG, "capabilityPolling Triggered");
        if(!isServiceInit.get(instance)) {
            postServiceDown(instance);
            return;
        }

        if(presenceServices.get(instance) == null || optionsServices.get(instance) == null) {
            Log.e(LOG_TAG, "capabilityPolling : instance does not exist");
            return;
        }

        try {
            presenceServices.get(instance).mPresence.getContactListCap(presenceServices.get(instance).mServiceHandle,
                    uris, getUserData());
            presenceServices.get(instance).mListener.setCapPollTaskListener(Listener);
        }catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void sendOptionsRequest(int instance, String uri, TaskListener listener) {
        Log.e(LOG_TAG, "sendOptionsRequest Triggered");
        if(!isServiceInit.get(instance)) {
            postServiceDown(instance);
            return;
        }

        if(optionsServices.get(instance) == null) {
            Log.e(LOG_TAG, "sendOptionsRequest : instance does not exist");
        }

        try {
            optionsServices.get(instance).mOptions.getContactCap(optionsServices.get(instance).mServiceHandle,
                    uri, getUserData());
            optionsServices.get(instance).mListener.setTaskListener(listener);

        }catch (RemoteException e) {
            e.printStackTrace();
        }

    }

    public void replyToOptions(int instance, int tid, CapabilityInfo caps) {
        Log.e(LOG_TAG, "replyToOptions Triggered tabInstance["+instance+"], tid["+tid+"]");
        if(!isServiceInit.get(instance)) {
            postServiceDown(instance);
            return;
        }

        if(optionsServices.get(instance) == null) {
            Log.e(LOG_TAG, "replyToOptions : instance does not exist");
        }

        try {
            OptionsCapInfo opCap = new OptionsCapInfo();
            opCap.setCapInfo(caps.getCapInfo());
            //send 200OK by default
            optionsServices.get(instance).mOptions.responseIncomingOptions(optionsServices.get(instance).mServiceHandle,
                    tid, 200, "OK", opCap, false);

        }catch (RemoteException e) {
            e.printStackTrace();
        }
    }
}
