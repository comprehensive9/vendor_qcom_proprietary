/*
 * Copyright (c) 2016, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2015 The Android Open Source Project
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

import static android.telephony.TelephonyManager.RADIO_POWER_UNAVAILABLE;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;

import com.android.internal.telephony.Call;
import com.android.internal.telephony.Connection;
import com.android.internal.telephony.dataconnection.DataEnabledSettings;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.vendor.VendorPhoneSwitcher;

import com.qualcomm.qcrilhook.QcRilHook;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class QtiPhoneSwitcher extends VendorPhoneSwitcher {

    private QtiRilInterface mQtiRilInterface;

    private final int USER_INITIATED_SWITCH = 0;
    private final int NONUSER_INITIATED_SWITCH = 1;

    private static final int DEFAULT_PHONE_INDEX = 0;

    public QtiPhoneSwitcher(int maxActivePhones, Context context, Looper looper) {
        super (maxActivePhones, context, looper);
        mQtiRilInterface = QtiRilInterface.getInstance(context);
        mQtiRilInterface.registerForUnsol(this, EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED, null);
    }

    public static QtiPhoneSwitcher make(int maxActivePhones, Context context, Looper looper) {
        if (sPhoneSwitcher == null) {
            sPhoneSwitcher = new QtiPhoneSwitcher(maxActivePhones, context, looper);
        }

        return (QtiPhoneSwitcher)sPhoneSwitcher;
    }

    private void queryMaxDataAllowed() {
        mMaxDataAttachModemCount = mQtiRilInterface.getMaxDataAllowed();
    }

    private void handleUnsolMaxDataAllowedChange(Message msg) {
        if (msg == null ||  msg.obj == null) {
            log("Null data received in handleUnsolMaxDataAllowedChange");
            return;
        }
        ByteBuffer payload = ByteBuffer.wrap((byte[])msg.obj);
        payload.order(ByteOrder.nativeOrder());
        int rspId = payload.getInt();
        if ((rspId == QcRilHook.QCRILHOOK_UNSOL_MAX_DATA_ALLOWED_CHANGED)) {
            int response_size = payload.getInt();
            if (response_size < 0 ) {
                log("Response size is Invalid " + response_size);
                return;
            }
            mMaxDataAttachModemCount = payload.get();
            log(" Unsol Max Data Changed to: " + mMaxDataAttachModemCount);
        }
    }

    @Override
    public void handleMessage(Message msg) {
        final int ddsSubId = mSubscriptionController.getDefaultDataSubId();
        final int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);

        log("handle event - " + msg.what);
        AsyncResult ar = null;
        switch (msg.what) {
            case EVENT_RADIO_AVAILABLE: {
                if (mQtiRilInterface.isServiceReady()) {
                    queryMaxDataAllowed();
                } else {
                    log("Oem hook service is not ready");
                }
                super.handleMessage(msg);
                break;
            }
            case EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED: {
                org.codeaurora.telephony.utils.AsyncResult asyncresult =
                       (org.codeaurora.telephony.utils.AsyncResult)msg.obj;
                if (asyncresult.result != null) {
                    handleUnsolMaxDataAllowedChange((Message)asyncresult.result);
                } else {
                    log("Error: empty result, EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED");
                }
                break;
            }
            default:
                super.handleMessage(msg);
        }
    }

    protected boolean isUiccProvisioned(int phoneId) {
        QtiUiccCardProvisioner uiccProvisioner = QtiUiccCardProvisioner.getInstance();
        boolean status = (uiccProvisioner.getCurrentUiccCardProvisioningStatus(phoneId) > 0) ?
                true : false;
        log("isUiccProvisioned = " + status);

        return status;
    }

    /*
     * Method to check if any of the calls are started
     */
    @Override
    protected boolean isPhoneInVoiceCall(Phone phone) {
        if (phone == null) {
            log("isPhoneInVoiceCall: phone is null");
            return false;
        }

        try {
            boolean tmpSwitchProperty = QtiTelephonyComponentFactory.getInstance().
                    getRil(DEFAULT_PHONE_INDEX).getPropertyValueBool(
                    PROPERTY_TEMP_DDSSWITCH, false);
            boolean dataDuringCallsEnabled = false;
            DataEnabledSettings dataEnabledSettings = phone.getDataEnabledSettings();
            if (dataEnabledSettings != null) {
                dataDuringCallsEnabled = dataEnabledSettings.isDataAllowedInVoiceCall();
            }

            if (!tmpSwitchProperty && !dataDuringCallsEnabled) {
                log("isPhoneInVoiceCall: tmpSwitchProperty=" + tmpSwitchProperty +
                        ", dataDuringCallsEnabled=" + dataDuringCallsEnabled);
                return false;
            }
        } catch (RemoteException ex) {
            log("isPhoneInVoiceCall: Exception" + ex);
            return false;
        }

        int phoneId = phone.getPhoneId();
        return (isCallAlive(mFgCsCalls[phoneId])
                || isCallAlive(mBgCsCalls[phoneId])
                || isCallAlive(mRiCsCalls[phoneId])
                || isCallAlive(mFgImsCalls[phoneId])
                || isCallAlive(mBgImsCalls[phoneId])
                || isCallAlive(mRiImsCalls[phoneId]));
    }

    private boolean isCallAlive(Call call) {
        if (call != null) {
            Call.State state = call.getState();
            boolean hasEmergencyCall = hasEmergencyConnection(call);
            log("isCallActive: callstate=" + state + ", hasEmergencyCall=" + hasEmergencyCall);
            return !(hasEmergencyCall && state == Call.State.DIALING)
                    && !(state == Call.State.IDLE || state == Call.State.DISCONNECTED);
        }
        return false;
    }

    private boolean hasEmergencyConnection(Call call) {
        if (call != null && call.hasConnections()) {
            ArrayList<Connection> connections = call.getConnections();
            for (Connection c : connections) {
                if (c.isEmergencyCall()) {
                    return true;
                }
            }
        }
        return false;
    }

    private void informDdsToRil(int ddsSubId) {
        int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);

        if (!mQtiRilInterface.isServiceReady()) {
            log("Oem hook service is not ready yet");
            return;
        }
        if (!updateHalCommandToUse()) {
           log("sendRilCommands: waiting for HAL command update, may be radio is unavailable");
           return;
        }

        for (int i = 0; i < mActiveModemCount; i++) {
            log("InformDdsToRil rild= " + i + ", DDS=" + ddsPhoneId);
            if (isCallInProgress()) {
                mQtiRilInterface.qcRilSendDDSInfo(ddsPhoneId,
                        NONUSER_INITIATED_SWITCH, i);
            } else {
                mQtiRilInterface.qcRilSendDDSInfo(ddsPhoneId,
                        USER_INITIATED_SWITCH, i);
            }
        }
    }

    /*
     * Returns true if mPhoneIdInVoiceCall is set for active calls
     */
    private boolean isCallInProgress() {
        return SubscriptionManager.isValidPhoneId(mPhoneIdInVoiceCall);
    }

    @Override
    protected boolean onEvaluate(boolean requestsChanged, String reason) {
        if (!updateHalCommandToUse()) {
            log("Wait for HAL command update");
            return false;
        }

        return super.onEvaluate(requestsChanged, reason);
    }

    @Override
    protected void sendRilCommands(int phoneId) {
        if (!updateHalCommandToUse()) {
            log("Wait for HAL command update");
            return;
        }

        super.sendRilCommands(phoneId);
    }

    private boolean updateHalCommandToUse() {
        if (mHalCommandToUse == HAL_COMMAND_UNKNOWN) {
            boolean isRadioAvailable = true;
            for (int i = 0; i < mActiveModemCount; i++) {
                isRadioAvailable &= PhoneFactory.getPhone(i).mCi.getRadioState()
                                != RADIO_POWER_UNAVAILABLE;
            }
            if (isRadioAvailable) {
                log("update HAL command");
                mHalCommandToUse = mRadioConfig.isSetPreferredDataCommandSupported()
                        ? HAL_COMMAND_PREFERRED_DATA : HAL_COMMAND_ALLOW_DATA;
                return true;
            } else {
                log("radio is unavailable");
            }
        }
        return mHalCommandToUse != HAL_COMMAND_UNKNOWN;
    }
}
