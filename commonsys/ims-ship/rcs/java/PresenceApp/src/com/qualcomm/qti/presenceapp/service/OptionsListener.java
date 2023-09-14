/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp.service;

import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;

import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.options.IOptionsListener;
import com.android.ims.internal.uce.options.OptionsCapInfo;
import com.android.ims.internal.uce.options.OptionsCmdStatus;
import com.android.ims.internal.uce.options.OptionsSipResponse;
import com.android.ims.internal.uce.presence.PresTupleInfo;
import com.qualcomm.qti.PresenceApp.CapabilityInfo;
import com.qualcomm.qti.PresenceApp.Contact;
import com.qualcomm.qti.PresenceApp.MainActivity;

public class OptionsListener extends IOptionsListener.Stub {
    Handler mHandler = null;
    int mTabInstance = -1;
    TaskListener mTaskListener;
    final static String LOG_TAG = MainActivity.LOG_TAG + ": OptionsListener:";
    public void setTaskListener(TaskListener l) {
        mTaskListener = l;
    }

    public OptionsListener(int instance) {
        mTabInstance = instance;
        mHandler = new Handler(Looper.getMainLooper());
    }
    @Override
    public void getVersionCb(String s) throws RemoteException {

    }

    @Override
    public void serviceAvailable(StatusCode statusCode) throws RemoteException {

    }

    @Override
    public void serviceUnavailable(StatusCode statusCode) throws RemoteException {

    }

    @Override
    public void sipResponseReceived(final String s, final OptionsSipResponse optionsSipResponse,
                                    final OptionsCapInfo optionsCapInfo) throws RemoteException {
        Uri uri = Uri.parse(s);
        String s2 = uri.getSchemeSpecificPart();
        String[] numberParts = s2.split("[@;:]");
        final String phoneNumber = numberParts[0];
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                String message = "Options SipResp:for["+ s + "][ SIP Code: " + optionsSipResponse.getSipResponseCode()
                        + ", RequestId: " + optionsSipResponse.getRequestId()+ "]";
                Log.d(LOG_TAG, message);
                Toast.makeText(MainActivity.mCtx,
                        message,
                        Toast.LENGTH_SHORT).show();


                CapabilityInfo caps = new CapabilityInfo(optionsCapInfo.getCapInfo());
                Contact c = MainActivity.contacts.get(phoneNumber);
                if(c == null) {
                    Log.e(LOG_TAG, "[" + phoneNumber+"] is not found in ContactList");
                    return;
                }
                Log.i(LOG_TAG, "[" + phoneNumber+"] Capabilities Received start");
                caps.printCaps();
                Log.i(LOG_TAG, "[" + phoneNumber+"] Capabilities Received End");
                c.setCapabilities(caps);
                c.setSubscribed(true);
                MainActivity.contacts.put(phoneNumber, c);
                mTaskListener.onSipResponse(optionsSipResponse.getSipResponseCode(),
                        optionsSipResponse.getReasonPhrase());
                mTaskListener.onContactCapabilitesUpdated(phoneNumber);
            }
        };
        mHandler.post(msg);

    }

    @Override
    public void cmdStatus(OptionsCmdStatus optionsCmdStatus) throws RemoteException {

    }

    @Override
    public void incomingOptions(final String s, final OptionsCapInfo optionsCapInfo,
                                final int i) throws RemoteException {
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.mCtx,
                        "Incoming Options received" + "From ["+ s +"]",
                        Toast.LENGTH_SHORT).show();
                Uri uri = Uri.parse(s);
                String s2 = uri.getSchemeSpecificPart();
                String[] numberParts = s2.split("[@;:]");
                final String phoneNumber = numberParts[0];
                Log.i(LOG_TAG, "incoming options [" + phoneNumber+"] Capabilities Received start");
                CapabilityInfo caps = new CapabilityInfo(optionsCapInfo.getCapInfo());
                caps.printCaps();
                Contact c = MainActivity.contacts.get(phoneNumber);
                c.setCapabilities(caps);
                c.setSubscribed(true);
                Log.i(LOG_TAG, "incoming options [" + phoneNumber+"] Capabilities Received End");
            }
        };
        mHandler.post(msg);
        // New thread to respond to this Incoming Options Request
        Thread t = new Thread() {
            @Override
            public void run() {
                super.run();
                Uri uri = Uri.parse(s);
                String s2 = uri.getSchemeSpecificPart();
                String[] numberParts = s2.split("[@;:]");
                final String phoneNumber = numberParts[0];
                Log.i(LOG_TAG, "reply to incoming options [" + phoneNumber+"] tid["+i+"]");
                if(MainActivity.mCapabilities.get(mTabInstance) == null)
                    return;

                CapabilityInfo caps = MainActivity.mCapabilities.get(mTabInstance);
                MainActivity.mServiceWrapper.replyToOptions(mTabInstance, i, caps);
            }
        };
        t.start();
    }

}
