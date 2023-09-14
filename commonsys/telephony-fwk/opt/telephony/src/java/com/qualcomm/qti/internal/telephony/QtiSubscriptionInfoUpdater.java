/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2014 The Android Open Source Project
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

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Looper;
import android.os.Message;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.IccCard;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.uicc.IccRefreshResponse;
import com.android.internal.telephony.vendor.VendorSubscriptionInfoUpdater;

/**
 * This class extends AOSP SubscriptionInfoUpdater class to achieve below support
 *
 * 1. When SIM card not provisioned(i.e SIM state in DETECTED) add a record
 *    in subInfo table by using the ICCID queried from RIL.
 * 2. To reduce delay in SubInfo records availability to clients, add subInfo record
 *    to table without waiting for SIM state moves to LOADED.
 */
public class QtiSubscriptionInfoUpdater extends VendorSubscriptionInfoUpdater {
    private static final String LOG_TAG = "QtiSubscriptionInfoUpdater";

    private static final int EVENT_ADD_SUBINFO_RECORD = 100;
    private static final int EVENT_SIM_REFRESH = 101;
    private static final String ICCID_STRING_FOR_NO_SIM = "";
    private static Context sContext = null;
    private static final int SUPPORTED_MODEM_COUNT = TelephonyManager.getDefault()
            .getSupportedModemCount();

    // SubInfo record can be added from below two places:
    // 1. From base class when SIM state internal intent received
    //    with LOADED/SIM_LOCKED state.
    // 2. This class adds when it receives request from UiccCardProvisioner.
    // Since the request can come both places, we are using this flag to do not
    // update the SubInfo record second time(by calling updateSubscriptionInfoByIccId()).
    // Initially it set to 'true', when the SubInfo updated from this class
    // this moves to 'false', so subsequent request will be ignored.
    // When SIM becomes absent this will be reset.
    private static QtiSubscriptionInfoUpdater sInstance = null;

    static QtiSubscriptionInfoUpdater init(Looper looper, Context context,
            CommandsInterface[] ci) {
        synchronized (QtiSubscriptionInfoUpdater.class) {
            if (sInstance == null) {
                sInstance = new QtiSubscriptionInfoUpdater(looper, context, ci);
            } else {
                Log.wtf(LOG_TAG, "init() called multiple times!  sInstance = " + sInstance);
            }
            return sInstance;
        }
    }

    public static QtiSubscriptionInfoUpdater getInstance() {
        if (sInstance == null) {
            Log.wtf(LOG_TAG, "getInstance null");
        }
        return sInstance;
    }

    private QtiSubscriptionInfoUpdater(Looper looper, Context context,
            CommandsInterface[] ci) {
        super(looper, context, ci);
        sContext = context;

        for (int index = 0; index < SUPPORTED_MODEM_COUNT; index++) {
            mIsRecordUpdateRequired[index] = false;
        }

        for (int i = 0; i < ci.length; i++) {
            ci[i].registerForIccRefresh(this, EVENT_SIM_REFRESH, new Integer(i));
        }
    }

    @Override
    public void handleMessage(Message msg) {
        Rlog.d(LOG_TAG, " handleMessage: EVENT:  " + msg.what);
        switch (msg.what) {
            case EVENT_ADD_SUBINFO_RECORD:
                handleAddSubInfoRecordEvent(msg.arg1, (String) msg.obj);
                break;

            case EVENT_SIM_REFRESH:
                AsyncResult ar = (AsyncResult)msg.obj;
                if (ar != null) {
                    Integer phoneId = (Integer)ar.userObj;
                    if (ar.result != null) {
                        IccRefreshResponse refreshRsp = (IccRefreshResponse)ar.result;

                        // Clear the ICCID when SIM Refresh reset received so that next time when
                        // SIM LOADED event received proper SubId would be created with new ICCID.
                        if (refreshRsp.refreshResult == IccRefreshResponse.REFRESH_RESULT_RESET) {
                           sIccId[phoneId] = null;
                        }
                    } else {
                        Rlog.d(LOG_TAG, "SIM refresh Exception: " +
                                ar.exception + " phoneId " + phoneId);
                    }
                }
                break;

            default:
                super.handleMessage(msg);
                break;
        }
    }

    void addSubInfoRecord(int slotId, String iccId) {
         if ((iccId == null) || (slotId < 0 || slotId >= SUPPORTED_MODEM_COUNT)) {
             Rlog.e(LOG_TAG, "addSubInfoRecord, invalid input IccId[" + slotId + "] = " + iccId);
             return;
         }
        // Post message to handler to handle all record update events in handler thread
        sendMessage(obtainMessage(EVENT_ADD_SUBINFO_RECORD, slotId, -1, iccId));
    }

    synchronized private void handleAddSubInfoRecordEvent(int slotId, String iccId) {
        if ((sIccId[slotId] != null) && (!sIccId[slotId].equals(ICCID_STRING_FOR_NO_SIM))
                && (sIccId[slotId].equals(iccId))) {
            Rlog.d(LOG_TAG, "Record already exists ignore duplicate update, existing IccId = "
                    + sIccId[slotId] + " recvd iccId[" + slotId + "] = " + iccId);
            return;
        }

        if ((sIccId[slotId] == null) || (sIccId[slotId] == ICCID_STRING_FOR_NO_SIM)
               || (!sIccId[slotId].equals(iccId))) {
            mIsRecordUpdateRequired[slotId] = true;
        }

        sIccId[slotId] = iccId;
        Rlog.d(LOG_TAG, " slotId = " + slotId + ", iccId = " + iccId);

        updateSubscriptionInfoByIccId(slotId, true);
    }

    @Override
    protected void handleSimReady(int slotId) {
        if ((sIccId[slotId] == null) || (sIccId[slotId] == ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }
        super.handleSimReady(slotId);
    }

    @Override
    protected void handleSimLoaded(int slotId) {
        if ((sIccId[slotId] == null) || (sIccId[slotId] == ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }
        super.handleSimLoaded(slotId);
    }

    @Override
    protected void handleSimLocked(int slotId, String reason) {
        if ((sIccId[slotId] == null) || (sIccId[slotId] == ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }
        super.handleSimLocked(slotId, reason);
    }

    @Override
    protected void handleSimAbsent(int slotId) {
        if ((sIccId[slotId] == null) || (sIccId[slotId] != ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }

        super.handleSimAbsent(slotId);
    }

    @Override
    protected void handleSimError(int slotId) {
        if ((sIccId[slotId] == null) || (sIccId[slotId] != ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }

        super.handleSimError(slotId);
    }

    @Override
    protected void handleSimNotReady(int slotId) {
        IccCard iccCard = PhoneFactory.getPhone(slotId).getIccCard();
        String iccid = sIccId[slotId];
        if (iccCard != null && iccCard.isEmptyProfile()) {
            mIsRecordUpdateRequired[slotId] = true;
            super.handleSimNotReady(slotId);
        } else {
            super.handleSimNotReady(slotId);

            // Restore the Iccid value so that mIsRecordUpdateRequired[phoneId] flag would be
            // set to true in handleSimAbsent for removing subId info.
            sIccId[slotId] = iccid;
        }
    }

    @Override
    synchronized protected void updateSubscriptionInfoByIccId(int slotIndex,
        boolean updateEmbeddedSubs) {

        if (mIsRecordUpdateRequired[slotIndex] == true) {
            super.updateSubscriptionInfoByIccId(slotIndex, updateEmbeddedSubs);
            Rlog.d(LOG_TAG, "SIM state changed, Updating user preference " + slotIndex);
            // Calling updateUserPreferences() here to update user preference
            // 1. During device power-up
            // 2. When SIM hot swap removal/insertion performed.
            if (QtiUiccCardProvisioner.getInstance().isAllCardProvisionInfoReceived()
                    && isAllIccIdQueryDone()) {
                QtiSubscriptionController.getInstance().updateUserPreferences();
            }

            // Once subIfo record added/updated reset mIsRecordUpdateRequired flag
            mIsRecordUpdateRequired[slotIndex] = false;
        } else {
            Rlog.d(LOG_TAG, "Ignoring subscription update event " + slotIndex);
        }
    }

    synchronized void updateUserPreferences() {
        Rlog.d(LOG_TAG, " calling updateUserPreferences ");

        if (isAllIccIdQueryDone()) {
            QtiSubscriptionController.getInstance().updateUserPreferences();
         }
    }
}
