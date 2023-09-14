/*********************************************************************
 Copyright (c) 2017,2019,2020-2021 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.uceShimService;

import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.qualcomm.qti.uceservice.V2_0.IUceListener;
import com.qualcomm.qti.uceservice.V2_0.UceStatus;
import com.qualcomm.qti.uceservice.V2_0.UceStatusCode;

import com.qualcomm.qti.uceShimService.RCSService;
import android.util.Log;
import android.content.Context;
import android.content.Intent;
import android.os.RemoteException;
import android.os.Handler;
import android.os.Looper;

public class RCSListener extends IUceListener.Stub { //extends HIDL IUceListener

    public static boolean isRCSRegistered = false;
    com.android.ims.internal.uce.uceservice.IUceListener mAidlUceListener = null;
    private final static String logTAG = "RCSService Listener";
    private RCSService mSvc= null;

    public RCSListener(RCSService s) {
        mSvc = s;
    }

    public void onStatusChange(UceStatus status, String iccId) {
        //ToDo Notify Status change Ind
        //AIDL-UCE Listener currently does not have Subcription Specific Callback
        final int statuscode = status.status;
        final String iccID = iccId;
        Handler handler = new Handler(Looper.getMainLooper());
        Runnable msg = new Runnable() {
            @Override
            public void run() {
                switch(statuscode) {
                    case UceStatusCode.SERVICE_UP:
                        Log.i(logTAG,"update hidl status for ACTION_UCE_SERVICE_UP");
                        mSvc.handleHidlStatus(true, iccID);
                        break;
                    case UceStatusCode.SERVICE_DOWN:
                        Log.i(logTAG,"update hidl status for ACTION_UCE_SERVICE_DOWN");
                        mSvc.handleHidlStatus(false, iccID);
                        break;
                }
            }
        };
        handler.post(msg);
        Log.d(logTAG,"onStatusChange_1_1 callback received status code["+ statuscode+"]");
    }

    public void setAidlUceListener(com.android.ims.internal.uce.uceservice.IUceListener aidlUceListener) {
	    mAidlUceListener = aidlUceListener;
    }
}
