/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.telephony.Rlog;

import com.android.internal.telephony.uicc.IccFileHandler;
import com.android.internal.telephony.uicc.UiccCard;
import com.android.internal.telephony.uicc.UiccController;
import com.android.internal.telephony.uicc.UiccCardApplication;

public class QtiUiccEfHelper extends Handler {
    private static final String LOG_TAG = "QtiUiccEfHelper";

    private static final int MESSAGE_GET_EF_DONE = 1;
    private static final int MESSAGE_SET_EF_DONE = 2;
    protected static final String ACTION_READ_EF_BROADCAST =
            "com.qualcomm.qti.intent.action.ACTION_READ_EF_RESULT";
    protected static final String ACTION_WRITE_EF_BROADCAST =
            "com.qualcomm.qti.intent.action.ACTION_WRITE_EF_RESULT";

    private static Context mContext;

    public QtiUiccEfHelper(Context context) {
        super(Looper.getMainLooper());

        mContext = context;
    }

    public void handleMessage(Message msg) {
        Rlog.d(LOG_TAG, "handleMessage what = " + msg.what);

        AsyncResult ar = (AsyncResult)msg.obj;
        switch (msg.what) {
            case MESSAGE_GET_EF_DONE:
                Intent getEfIntent = new Intent(ACTION_READ_EF_BROADCAST);
                Rlog.d(LOG_TAG, "ef content = " + ar.result + " on slot = " + msg.arg1);
                if(ar.exception != null) {
                    getEfIntent.putExtra("exception", true);
                } else {
                    getEfIntent.putExtra("payload", (byte[])(ar.result));
                }
                getEfIntent.putExtra("slot", (int)(msg.arg1));
                mContext.sendBroadcast(getEfIntent);
                break;
            case MESSAGE_SET_EF_DONE:
                Intent setEfIntent = new Intent(ACTION_WRITE_EF_BROADCAST);
                if(ar.exception != null) {
                    setEfIntent.putExtra("exception", true);
                }
                setEfIntent.putExtra("slot", (int)(msg.arg1));
                mContext.sendBroadcast(setEfIntent);
                break;
            default:
                Rlog.w(LOG_TAG, "MessageHandler: unexpected message code: " + msg.what);
                break;
        }
    }

    public IccFileHandler loadIccFileHandler(int slotId, int family) {
        UiccCard card = UiccController.getInstance().getUiccCard(slotId);
        Rlog.d(LOG_TAG, "card = " + card);
        IccFileHandler iccFileHandler = null;
        if (card != null) {
            UiccCardApplication newUiccApplication = card.getApplication(family);
            Rlog.d(LOG_TAG, "newUiccApplication = " + newUiccApplication);
            if (newUiccApplication != null) {
                iccFileHandler = newUiccApplication.getIccFileHandler();
                Rlog.d(LOG_TAG, "fh = " + iccFileHandler);
            }
        }
        return iccFileHandler;
    }

    public boolean readUiccEf(final int slotId, final int family, final int efId) {
       IccFileHandler fh = loadIccFileHandler(slotId, family);
       if(fh != null) {
           fh.loadEFTransparent(efId, obtainMessage(MESSAGE_GET_EF_DONE, slotId, -1));
           return true;
       } else {
           return false;
       }
    }

    public boolean writeUiccEf(final int slotId, final int family,
                            final int efId, final byte[] efData) {
       IccFileHandler fh = loadIccFileHandler(slotId, family);
       if(fh != null) {
           fh.updateEFTransparent(efId, efData,
                obtainMessage(MESSAGE_SET_EF_DONE, slotId, -1));
           return true;
       } else {
           return false;
       }
    }
}
