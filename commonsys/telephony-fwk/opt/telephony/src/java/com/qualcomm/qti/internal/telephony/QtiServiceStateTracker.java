/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.os.Message;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.vendor.VendorServiceStateTracker;


public class QtiServiceStateTracker extends VendorServiceStateTracker {
    private static final String LOG_TAG = "QtiServiceStateTracker";
    private boolean mIsImsCallingEnabled;

    public QtiServiceStateTracker(GsmCdmaPhone phone, CommandsInterface ci) {
        super(phone,ci);
        mIsImsCallingEnabled = isImsCallingEnabled();
    }

    @Override
    public void handleMessage(Message msg) {
        if (msg.what == EVENT_RADIO_STATE_CHANGED) {
            if (mPhone.mCi.getRadioState() == TelephonyManager.RADIO_POWER_OFF) {
                setPowerStateToDesired();
                log("Trigger as manual polling");
                pollState();
            } else {
                super.handleMessage(msg);
            }
        } else if (msg.what == EVENT_IMS_CAPABILITY_CHANGED) {
            super.handleMessage(msg);

            final boolean oldImsCallingEnabled = mIsImsCallingEnabled;
            mIsImsCallingEnabled = isImsCallingEnabled();

            if (mSS.getState() != ServiceState.STATE_IN_SERVICE
                    && oldImsCallingEnabled != mIsImsCallingEnabled) {
                log("Notify service state as IMS caps will only affect the merged service state");
                mPhone.notifyServiceStateChanged(mPhone.getServiceState());
            }
        } else {
            super.handleMessage(msg);
        }
    }

    // Because UE moves into OOS environment, IMS registration state can't get updated in time
    // based on timer's settings from modem as per 3GPP spec, so Telephony has to combine parts of
    // IMS capbilities to determine if service state needs to be notified.
    private boolean isImsCallingEnabled() {
        return mPhone != null
                && (mPhone.isVolteEnabled() || mPhone.isWifiCallingEnabled()
                        || mPhone.isVideoEnabled());
    }
}
