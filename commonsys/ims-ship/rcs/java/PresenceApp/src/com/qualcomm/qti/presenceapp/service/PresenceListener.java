/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp.service;

import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Toast;

import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.presence.IPresenceListener;
import com.android.ims.internal.uce.presence.PresCmdId;
import com.android.ims.internal.uce.presence.PresCmdStatus;
import com.android.ims.internal.uce.presence.PresPublishTriggerType;
import com.android.ims.internal.uce.presence.PresResInfo;
import com.android.ims.internal.uce.presence.PresRlmiInfo;
import com.android.ims.internal.uce.presence.PresSipResponse;
import com.android.ims.internal.uce.presence.PresTupleInfo;
import com.qualcomm.qti.PresenceApp.CapabilityInfo;
import com.qualcomm.qti.PresenceApp.Contact;
import com.qualcomm.qti.PresenceApp.MainActivity;
import com.qualcomm.qti.PresenceApp.tasks.PublishTask;

import java.util.List;

public class PresenceListener extends IPresenceListener.Stub {

    int mTabInstance = -1;
    Handler mHandler = null;
    TaskListener mSubscribeTaskListener = null;
    TaskListener mPublishTaskListener = null;
    TaskListener mCapPollTaskListener = null;
    final String LOG_TAG = MainActivity.LOG_TAG + ":PresenceListener";
    public PresenceListener(int instance) {
        mTabInstance = instance;
        // This is UI Thread
        mHandler = new Handler(Looper.getMainLooper());
    }
    @Override
    public void getVersionCb(String s) throws RemoteException {

    }
    public void setSubscribeTaskListener(TaskListener listener) {
        mSubscribeTaskListener = listener;
    }
    public void setCapPollTaskListener(TaskListener listener) {
        mCapPollTaskListener = listener;
    }
    public void setPublishTaskListener(TaskListener listener) {
        mPublishTaskListener = listener;
    }
    @Override
    public void serviceAvailable(StatusCode statusCode) throws RemoteException {
        Log.d(LOG_TAG, "Service Available Indication");
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.mCtx,
                        "Presence Service Available" + "For instance["+ mTabInstance+"]",
                        Toast.LENGTH_SHORT).show();
                mPublishTaskListener.onRegistrationChange("TRUE" );
            }
        };
        mHandler.post(msg);

    }

    @Override
    public void serviceUnAvailable(StatusCode statusCode) throws RemoteException {
        Log.d(LOG_TAG, "Service Un-available Indication");
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.mCtx,
                        "Presence Service UnAvailable" + "For instance["+ mTabInstance+"]",
                        Toast.LENGTH_SHORT).show();
                mPublishTaskListener.onRegistrationChange( "FALSE" );
            }
        };
        mHandler.post(msg);
    }

    @Override
    public void publishTriggering(final PresPublishTriggerType presPublishTriggerType) throws RemoteException {
        final String message = "PublishTrigger Received [" +
                TriggerTypeToString(presPublishTriggerType.getPublishTrigeerType()) +
                "] For instance["+ mTabInstance+"]" +"]";
        Log.d(LOG_TAG, message);
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.mCtx,
                        message,
                        Toast.LENGTH_SHORT).show();
                mPublishTaskListener.onPublishTrigger(TriggerTypeToString(presPublishTriggerType.getPublishTrigeerType()));
                PublishTask task = new PublishTask(MainActivity.mCtx, mTabInstance);
                task.execute();
            }
        };
        mHandler.post(msg);
    }
    private String TriggerTypeToString(int type) {
        String sType = "UNKNOWN";
        switch(type) {
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_ETAG_EXPIRED:
                sType = "ETAG_EXPIRED";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_DISABLED:
                sType = "LTE_VOPS_DISABLED";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_ENABLED:
                sType = "LTE_VOPS_ENABLED";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_EHRPD:
                sType = "EHRPD";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS:
                sType = "HSPAPLUS";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_3G:
                sType = "3G";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_2G:
                sType = "2G";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN:
                sType = "WLAN";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN:
                sType = "IWLAN";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_UNKNOWN:
                sType = "UNKNOWN";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_NR5G_VOPS_DISABLED:
                sType = "NR5G_VOPS_DISABLED";
                break;
            case PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_NR5G_VOPS_ENABLED:
                sType = "NR5G_VOPS_ENABLED";
                break;
        }
        return sType;
    }

    @Override
    public void cmdStatus(final PresCmdStatus presCmdStatus) throws RemoteException {
        final int Status = presCmdStatus.getStatus().getStatusCode();
        final int ReqId = presCmdStatus.getRequestId();
        final String sStatus = (Status == StatusCode.UCE_SUCCESS) ? "Success" : "Failure";
        final String message = "Cmd Status:[ Status Code: " + Status
                + ", RequestId: " + ReqId + "For instance["+ mTabInstance+"]"+ "]";
        Log.d(LOG_TAG, message);
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                String cmd = "Publish";
                if (presCmdStatus.getCmdId().getCmdId() == PresCmdId.UCE_PRES_CMD_GETCONTACTCAP) {
                    cmd = "Availability Fetch";
                    if(mSubscribeTaskListener != null) {
                      mSubscribeTaskListener.onCommandStatus(sStatus,ReqId);
                    } else {
                      Log.e(LOG_TAG, "mSubscribeTaskListener is null, therefore not calling onCommandStatus for instance["+ mTabInstance+"]");
                    }
                }
                else if(presCmdStatus.getCmdId().getCmdId() == PresCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP) {
                    cmd = "Capability Poll";
                }
                else if(presCmdStatus.getCmdId().getCmdId() == PresCmdId.UCE_PRES_CMD_PUBLISHMYCAP) {
                    cmd = "Capability Poll";
                    mPublishTaskListener.onCommandStatus(sStatus,ReqId);
                }
                else {
                    cmd = "Unknown";
                }
                Toast.makeText(MainActivity.mCtx,
                        message,
                        Toast.LENGTH_SHORT).show();
            }
        };
        mHandler.post(msg);
    }

    @Override
    public void sipResponseReceived(final PresSipResponse presSipResponse) throws RemoteException {
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                String cmd = "Publish";
                if (presSipResponse.getCmdId().getCmdId() == PresCmdId.UCE_PRES_CMD_GETCONTACTCAP) {
                    cmd = "Availability Fetch";
                    if(mSubscribeTaskListener != null) {
                      mSubscribeTaskListener.onSipResponse(presSipResponse.getSipResponseCode(),
                            presSipResponse.getReasonPhrase());
                    } else {
                      Log.e(LOG_TAG, "mSubscribeTaskListener is null, therefore not calling onSipResponse for instance["+ mTabInstance+"]");
                    }
                }
                else if(presSipResponse.getCmdId().getCmdId() == PresCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP) {
                    cmd = "Capability Poll";
                }
                else if(presSipResponse.getCmdId().getCmdId() == PresCmdId.UCE_PRES_CMD_PUBLISHMYCAP) {
                    mPublishTaskListener.onSipResponse(presSipResponse.getSipResponseCode(),
                             presSipResponse.getReasonPhrase());
                }
                final String message = cmd + " SipResp:[ SIP Code: " + presSipResponse.getSipResponseCode()
                        + ", RequestId: " + presSipResponse.getRequestId()+ "]";
                Log.d(LOG_TAG, message);

                Toast.makeText(MainActivity.mCtx,
                        message,
                        Toast.LENGTH_SHORT).show();
            }
        };
        mHandler.post(msg);
    }

    @Override
    public void capInfoReceived(String s, PresTupleInfo[] presTupleInfos) throws RemoteException {
        final String message = "CapInfoReceived of size [" + presTupleInfos.length +
                "] Uri[" + s+ "]";
        Log.d(LOG_TAG, message);
        if(presTupleInfos.length == 0) {
            Log.e(LOG_TAG, message);
            return;
        }
        Uri contactString = Uri.parse(s);
        String s2 = contactString.getSchemeSpecificPart();
        String[] numberParts = s2.split("[@;:]");
        final String phoneNumber = numberParts[0];
        Log.e(LOG_TAG, "Extracted Phone number: " + phoneNumber);
        //final ListView capInfoListView = (ListView)mCapInfoView;
        //final int instance = mTabInstance;
        final PresTupleInfo[] presTupleInfoArray = presTupleInfos;
        Runnable msg = new Runnable() {
            @Override
            public void run() {

                CapabilityInfo caps = new CapabilityInfo();
                Contact c = MainActivity.contacts.get(phoneNumber);
                if(c == null) {
                    Log.i(LOG_TAG, "CapInfo [" + phoneNumber +"] not found in list");
                } else {
                    for(PresTupleInfo tupleInfo : presTupleInfoArray) {
                        caps.decodeFeatureTagToCap(tupleInfo.getFeatureTag(),
                                                   tupleInfo.getVersion());
                        caps.toString();
                    }
                    c.setCapabilities(caps);
                    c.setSubscribed(true);
                    MainActivity.contacts.put(phoneNumber, c);
                    Log.i(LOG_TAG, "CapInfo ["+ phoneNumber+"] Capabilities Received start");
                    caps.printCaps();
                    Log.i(LOG_TAG, "CapInfo ["+ phoneNumber+"] Capabilities Received End");
                    Toast.makeText(MainActivity.mCtx,
                            "Notify for Availabilty Fetch Received",
                            Toast.LENGTH_SHORT).show();
                    if(mSubscribeTaskListener != null) {
                      mSubscribeTaskListener.onContactCapabilitesUpdated(phoneNumber);
                    } else {
                      Log.e(LOG_TAG, "mSubscribeTaskListener is null, therefore not calling onContactCapabilitesUpdated for instance["+ mTabInstance+"]");
                    }
                }
            }
        };
        mHandler.post(msg);
    }

    @Override
    public void listCapInfoReceived(PresRlmiInfo presRlmiInfo, PresResInfo[] presResInfos) throws RemoteException {

        final PresResInfo[] tempResInfoArray = presResInfos;

        Runnable msg = new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.mCtx,
                        "Notify for Capability Poll Received",
                        Toast.LENGTH_SHORT).show();
                int i=0;
                for(PresResInfo resInfo : tempResInfoArray) {
                    Uri contactString = Uri.parse(resInfo.getInstanceInfo().getPresentityUri());
                    String s2 = contactString.getSchemeSpecificPart();
                    String[] numberParts = s2.split("[@;:]");
                    String phoneNumber = numberParts[0];
                    Log.i(LOG_TAG, "ListCapInfo ["+ i+ "][" + phoneNumber+"] Capabilities Received start");
                    Contact c = MainActivity.contacts.get(phoneNumber);
                    if(c == null) {
                        Log.i(LOG_TAG, "ListCapInfo ["+ i+ "][" + phoneNumber +"] not found in list");
                    } else {
                        c.setSubscribed(true);
                        CapabilityInfo caps = new CapabilityInfo();
                        for(PresTupleInfo tuple : resInfo.getInstanceInfo().getTupleInfo()) {
                            caps.decodeFeatureTagToCap(tuple.getFeatureTag(),
                                                       tuple.getVersion());
                        }
                        c.setCapabilities(caps);
                        MainActivity.contacts.put(phoneNumber, c);
                        caps.printCaps();
                    }
                    Log.i(LOG_TAG, "ListCapInfo ["+ i+ "][" + phoneNumber+"] Capabilities Received End");
                    i++;
                }
                if(mCapPollTaskListener != null) {
                  mCapPollTaskListener.onNotifyListReceived();
                } else {
                  Log.e(LOG_TAG, "mCapPollTaskListener is null therefore not calling onNotifyListReceived for instance["+ mTabInstance+"]");
                }
            }
        };
        mHandler.post(msg);
    }

    @Override
    public void unpublishMessageSent() throws RemoteException {
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.mCtx,
                        "UnPublish Received",
                        Toast.LENGTH_SHORT).show();
            }
        };
        mHandler.post(msg);
    }
}
