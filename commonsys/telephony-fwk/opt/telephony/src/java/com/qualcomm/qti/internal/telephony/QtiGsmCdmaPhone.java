/*
 * Copyright (c) 2016, 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2006 The Android Open Source Project
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
import com.android.ims.FeatureConnector;
import com.android.ims.ImsException;
import com.android.ims.ImsManager;
import android.content.ContentValues;
import android.content.SharedPreferences;
import android.os.AsyncResult;
import android.os.Message;
import android.os.RemoteException;
import android.preference.PreferenceManager;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;
import android.util.Pair;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.vendor.VendorGsmCdmaPhone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.PhoneNotifier;
import com.android.internal.telephony.RadioCapability;
import com.android.internal.telephony.TelephonyComponentFactory;
import com.android.internal.telephony.CarrierInfoManager;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.uicc.IccCardStatus;
import com.android.internal.telephony.uicc.IccUtils;
import com.android.internal.telephony.uicc.UiccSlot;
import android.telephony.ImsiEncryptionInfo;

import com.qualcomm.qti.internal.telephony.primarycard.SubsidyLockSettingsObserver;
import com.qualcomm.qti.internal.telephony.QtiRadioCapabilityController;

import java.util.BitSet;


public class QtiGsmCdmaPhone extends VendorGsmCdmaPhone {
    private static final String LOG_TAG = "QtiGsmCdmaPhone";
    private static final int PROP_EVENT_START = EVENT_LAST;
    private static final int EVENT_OEM_HOOK_SERVICE_READY = PROP_EVENT_START + 1;
    private static final int EVENT_RESET_CARRIER_KEY_IMSI_ENCRYPTION = PROP_EVENT_START + 2;
    private static final int DEFAULT_PHONE_INDEX = 0;

    private BaseRilInterface mQtiRilInterface;
    private static final int READY = 1;
    private static final int BIT_TOTAL_COUNT = 4;
    private static final int BIT_PHONE_READY_REQUIRED = 0;
    private static final int BIT_RIL_CONNECTED = 1;
    private static final int BIT_OEM_HOOK_READY = 2;
    private static final int BIT_IMS_STACK_UP = 3;
    /**
     * Bit 0 indicates if phone is required to send ready to RIL
     * Bit 1 indicates if RIL is connected
     * Bit 2 indicates if Oem hook is ready
     * Bit 3 indicates if IMS stack is up
     */
    private final BitSet mPhoneReadyBits = new BitSet(BIT_TOTAL_COUNT);
    private FeatureConnector<ImsManager> mImsManagerConnector;

    private int imsiToken = 0;

    public QtiGsmCdmaPhone(Context context,
            CommandsInterface ci, PhoneNotifier notifier, int phoneId,
            int precisePhoneType, TelephonyComponentFactory telephonyComponentFactory) {
        this(context, ci, notifier, false, phoneId, precisePhoneType,
                telephonyComponentFactory);
    }

    public QtiGsmCdmaPhone(Context context,
            CommandsInterface ci, PhoneNotifier notifier, boolean unitTestMode, int phoneId,
            int precisePhoneType, TelephonyComponentFactory telephonyComponentFactory) {
        super(context, ci, notifier, unitTestMode, phoneId, precisePhoneType,
                telephonyComponentFactory);
        Rlog.d(LOG_TAG, "Constructor");
        mQtiRilInterface = getQtiRilInterface();
        mQtiRilInterface.registerForServiceReadyEvent(this, EVENT_OEM_HOOK_SERVICE_READY, null);
        if (ImsManager.isImsSupportedOnDevice(context)) {
            mImsManagerConnector = new FeatureConnector(context, phoneId,
                    new FeatureConnector.Listener<ImsManager>() {
                        @Override
                        public void connectionReady(ImsManager manager) throws ImsException {
                            mPhoneReadyBits.set(BIT_IMS_STACK_UP);
                            // Allow again as IMS module may have exceptions
                            mPhoneReadyBits.set(BIT_PHONE_READY_REQUIRED);
                            updatePhoneReady(phoneId);
                            logd("IMS stack is ready.");
                        }

                        @Override
                        public void connectionUnavailable() {
                            mPhoneReadyBits.clear(BIT_IMS_STACK_UP);
                            logd("IMS stack is unready.");
                        }

                        @Override
                        public ImsManager getFeatureManager() {
                            return ImsManager.getInstance(context, phoneId);
                        }
            }, "QtiGsmCdmaPhone");
            mImsManagerConnector.connect();
        } else {
            mPhoneReadyBits.set(BIT_IMS_STACK_UP); // IMS status is ignored
        }
        mCi.registerForCarrierInfoForImsiEncryption(this,
                EVENT_RESET_CARRIER_KEY_IMSI_ENCRYPTION, null);
    }

    @Override
    public void setPreferredNetworkType(int networkType, Message response) {
        QtiRadioCapabilityController radioCapController =
                QtiRadioCapabilityController.getInstance();
        if (radioCapController != null
                && radioCapController.isFlexMappingRequired(getPhoneId())) {
            radioCapController.setPreferredNetworkType(getPhoneId(), networkType, response);
        } else {
            Rlog.d(LOG_TAG, "Set preferred network type without flex mapping support");
            super.setPreferredNetworkType(networkType, response);
        }
    }

    private void updatePhoneReady(int phoneId) {
        if (!mPhoneReadyBits.get(BIT_PHONE_READY_REQUIRED)) return;

        int radioPowerOpt = 0;
        try {
            radioPowerOpt = QtiTelephonyComponentFactory.getInstance().getRil(DEFAULT_PHONE_INDEX).
                    getPropertyValueInt("persist.vendor.radio.poweron_opt", 0);
        } catch (RemoteException|NullPointerException ex) {
            Rlog.e(LOG_TAG, "Exception: " + ex);
        }
        // To send ready to RIL needs to wait for
        // 1. RIL is connected
        // 2. OEM HOOK is ready
        // 3. IMS stack is ready
        if (mPhoneReadyBits.cardinality() == BIT_TOTAL_COUNT
                && (radioPowerOpt == 1)) {
            logd("Sending Phone Ready to RIL.");
            if (phoneId != DEFAULT_PHONE_INDEX) {
                // IMS stack is not up on phone0, but Telephony needs to send
                // UI READY as bootup optimization.
                ImsManager imsManager = ImsManager.getInstance(getContext(), DEFAULT_PHONE_INDEX);
                if (!imsManager.isServiceReady()) {
                    logd("Sending Phone Ready to RIL0 as required.");
                    mQtiRilInterface.sendPhoneStatus(READY, DEFAULT_PHONE_INDEX);
                }
            }
            mQtiRilInterface.sendPhoneStatus(READY, phoneId);
            mPhoneReadyBits.clear(BIT_PHONE_READY_REQUIRED);
        }
    }

    @Override
    protected void phoneObjectUpdater(int newVoiceTech) {
        super.phoneObjectUpdater(newVoiceTech);
        updatePhoneReady(mPhoneId);
    }

    @Override
    public void radioCapabilityUpdated(RadioCapability rc) {
        // Called when radios first become available or after a capability switch
        // Update the cached value
        mRadioCapability.set(rc);

        QtiRadioCapabilityController radioCapController =
                QtiRadioCapabilityController.getInstance();
        if (radioCapController != null) {
            // update new radio capabilities to QtiRadioCapabilityController
            // This would be called in below scenarios
            // 1. After completion of flex map.
            // 2. When radio state moved from UNAVAILABLE to AVAILABLE
            // 3. Whenever phone object switch happens.
            radioCapController.radioCapabilityUpdated(getPhoneId(), rc);
        }
    }

    @Override
    public boolean getCallForwardingIndicator() {
        if (!isCurrentSubValid()) {
            return false;
        }
        return super.getCallForwardingIndicator();
    }

    private boolean isCurrentSubValid() {
        int provisionStatus = QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED;
        SubscriptionManager subscriptionManager = SubscriptionManager.from(mContext);
        try {
            provisionStatus = QtiUiccCardProvisioner.getInstance()
                    .getCurrentUiccCardProvisioningStatus(mPhoneId);
        } catch (NullPointerException ex) {
            provisionStatus = QtiUiccCardProvisioner.UiccProvisionStatus.NOT_PROVISIONED;
        }
        Rlog.d(LOG_TAG, "ProvisionStatus: " + provisionStatus + " phone id:" + mPhoneId);
        return subscriptionManager.isActiveSubscriptionId(getSubId()) &&
                (provisionStatus == QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED);
    }

    public boolean setLocalCallHold(boolean enable) {
        if (!mQtiRilInterface.isServiceReady()) {
            Rlog.e(LOG_TAG, "mQtiRilInterface is not ready yet");
            return false;
        }
        return mQtiRilInterface.setLocalCallHold(mPhoneId, enable);
    }

    @Override
    public void dispose() {
        mQtiRilInterface.unRegisterForServiceReadyEvent(this);
        mQtiRilInterface = null;
        mPhoneReadyBits.clear();
        if (mImsManagerConnector != null) mImsManagerConnector.disconnect();
        super.dispose();
    }

    @Override
    public void handleMessage(Message msg) {
        Rlog.d(LOG_TAG, "handleMessage: Event: " + msg.what);
        AsyncResult ar;
        switch(msg.what) {
            case EVENT_OEM_HOOK_SERVICE_READY:
                ar = (AsyncResult)msg.obj;
                if ((ar != null) && ar.result != null) {
                    boolean isServiceReady = (boolean)ar.result;
                    if (isServiceReady) {
                        mPhoneReadyBits.set(BIT_OEM_HOOK_READY);
                        Rlog.d(LOG_TAG, "EVENT_OEM_HOOK_SERVICE_READY received");
                        updatePhoneReady(mPhoneId);
                    } else {
                        Rlog.d(LOG_TAG, "EVENT_OEM_HOOK_SERVICE_READY: service not ready");
                        mPhoneReadyBits.clear(BIT_OEM_HOOK_READY);
                    }
                } else {
                    Rlog.e(LOG_TAG, "Error: empty result, EVENT_OEM_HOOK_SERVICE_READY");
                }
                // Do not call super.handleMessage().
                // EVENT_OEM_HOOK_SERVICE_READY is non-aosp event.
                break;

            case EVENT_SIM_RECORDS_LOADED:
                if(isPhoneTypeGsm()) {
                    Rlog.d(LOG_TAG, "notify call forward indication, phone id:" + mPhoneId);
                    notifyCallForwardingIndicator();
                }

                super.handleMessage(msg);
                break;

            case EVENT_RADIO_AVAILABLE:
                mPhoneReadyBits.set(BIT_PHONE_READY_REQUIRED);
                updatePhoneReady(mPhoneId);
                super.handleMessage(msg);
                break;

            case EVENT_RIL_CONNECTED:
                mPhoneReadyBits.set(BIT_PHONE_READY_REQUIRED);
                mPhoneReadyBits.set(BIT_RIL_CONNECTED);
                updatePhoneReady(mPhoneId);
                super.handleMessage(msg);
                break;

            case EVENT_NV_READY:{
                Rlog.d(LOG_TAG, "Event EVENT_NV_READY Received");
                mSST.pollState();
                // Notify voicemails.
                Rlog.d(LOG_TAG, "notifyMessageWaitingChanged");
                mNotifier.notifyMessageWaitingChanged(this);
                updateVoiceMail();
                // Do not call super.handleMessage().
                // AOSP do not handle EVENT_NV_READY.
                break;
            }
            case EVENT_RESET_CARRIER_KEY_IMSI_ENCRYPTION:
                Rlog.d(LOG_TAG, "Event EVENT_RESET_CARRIER_KEY_IMSI_ENCRYPTION");
                super.resetCarrierKeysForImsiEncryption();
                break;

            default: {
                super.handleMessage(msg);
            }

        }
    }

    @Override
    public void sendSubscriptionSettings(boolean restoreNetworkSelection) {
         ExtTelephonyServiceImpl serviceImpl = ExtTelephonyServiceImpl.getInstance();

        // When subsidy lock feature enabled, device in subsidy locked/LOCKED state,
        // set network selection mode to AUTOMATIC for primary carrier SIM
        if (SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled() &&
                !SubsidyLockSettingsObserver.isSubsidyUnlocked(mContext) &&
                (serviceImpl != null & serviceImpl.isPrimaryCarrierSlotId(getPhoneId()))) {
            // Send settings down
            int type = PhoneFactory.calculatePreferredNetworkType(mContext, getSubId());
            setPreferredNetworkType(type, null);

            logd(" settings network selection mode to AUTO ");
            setNetworkSelectionModeAutomatic(null);
        } else {
            super.sendSubscriptionSettings(restoreNetworkSelection);
        }
    }

    private BaseRilInterface getQtiRilInterface() {
        BaseRilInterface qtiRilInterface;
        if (getUnitTestMode()) {
            logd("getQtiRilInterface, unitTestMode = true");
            qtiRilInterface = SimulatedQtiRilInterface.getInstance(mContext);
        } else {
            qtiRilInterface = QtiRilInterface.getInstance(mContext);
        }
        return qtiRilInterface;
    }

    @Override
    public boolean isEmergencyNumber(String address) {
        return QtiEmergencyCallHelper.isEmergencyNumber(mContext, address);
    }

    @Override
    public void setCarrierInfoForImsiEncryption(ImsiEncryptionInfo imsiEncryptionInfo) {
        CarrierInfoManager.setCarrierInfoForImsiEncryption(imsiEncryptionInfo, mContext, mPhoneId);
        QtiTelephonyComponentFactory.getInstance().getRil(mPhoneId)
            .setCarrierInfoForImsiEncryption(++imsiToken, imsiEncryptionInfo);
    }
    /**
     * Retrieves the full serial number of the ICC (including hex digits), if applicable.
     */
    @Override
    public String getFullIccSerialNumber() {
        String iccId = super.getFullIccSerialNumber();

        if (TextUtils.isEmpty(iccId)) {
            iccId = QtiUiccCardProvisioner.getInstance().getUiccFullIccId(mPhoneId);
        }
        return iccId;
    }

    private static final String SIM_STATUS = "sim_status-%d";
    private static final int INVALID_SIM_STATUS = -1;

    /*
     * Skip reapply if sim status is never set by Telephony
     */
    @Override
    protected void reapplyUiccAppsEnablementIfNeeded(int retries) {
        UiccSlot slot = mUiccController.getUiccSlotForPhone(mPhoneId);

        // If no card is present or we don't have mUiccApplicationsEnabled yet, do nothing.
        if (slot == null || slot.getCardState() != IccCardStatus.CardState.CARDSTATE_PRESENT
                || mUiccApplicationsEnabled == null) {
            return;
        }

        String iccId = IccUtils.stripTrailingFs(slot.getIccId());
        if (iccId == null) {
            logd("reapplyUiccAppsEnablementIfNeeded: iccid is null, do nothing");
            return;
        }

        SubscriptionInfo info = SubscriptionController.getInstance().getSubInfoForIccId(iccId);

        // If info is null, it could be a new subscription. By default we enable it.
        boolean expectedValue = info == null ? true : info.areUiccApplicationsEnabled();

        SharedPreferences sp
                = PreferenceManager.getDefaultSharedPreferences(getContext());
        String key = String.format(SIM_STATUS, mPhoneId);
        int simStatus = sp.getInt(key, INVALID_SIM_STATUS);
        if (simStatus == INVALID_SIM_STATUS) {
            logd("reapplyUiccAppsEnablementIfNeeded: Apply SIM status from RIL," +
                    "mUiccApplicationsEnabled=" + mUiccApplicationsEnabled);
            //Update SIM status in db
            ContentValues value = new ContentValues(1);
            value.put(SubscriptionManager.UICC_APPLICATIONS_ENABLED, mUiccApplicationsEnabled);
            mContext.getContentResolver().update(SubscriptionManager.CONTENT_URI, value,
                    SubscriptionManager.ICC_ID + "=\'" + iccId + "\'", null);

            //Update SIM status in shared preference
            SharedPreferences.Editor editor = sp.edit();
            editor.putInt(key, mUiccApplicationsEnabled ? 1 : 0);
            editor.commit();

            mCi.enableUiccApplications(mUiccApplicationsEnabled, Message.obtain(
                     this, EVENT_REAPPLY_UICC_APPS_ENABLEMENT_DONE,
                     new Pair<Boolean, Integer>(expectedValue, retries)));
        } else {
            // If for any reason current state is different from configured state, re-apply the
            // configured state.
            if (expectedValue != mUiccApplicationsEnabled) {
                mCi.enableUiccApplications(expectedValue, Message.obtain(
                        this, EVENT_REAPPLY_UICC_APPS_ENABLEMENT_DONE,
                        new Pair<Boolean, Integer>(expectedValue, retries)));

                //Update SIM status in shared preference
                SharedPreferences.Editor editor = sp.edit();
                editor.putInt(key, expectedValue ? 1 : 0);
                editor.commit();
            }
        }
    }

    /*
     * Store sim status in shared preference, this is to track sim state change by user
     */
    @Override
    public void enableUiccApplications(boolean enable, Message onCompleteMessage) {
        UiccSlot slot = mUiccController.getUiccSlotForPhone(mPhoneId);
        if (slot != null && slot.getCardState() == IccCardStatus.CardState.CARDSTATE_PRESENT) {
            SharedPreferences sp
                = PreferenceManager.getDefaultSharedPreferences(getContext());
            SharedPreferences.Editor editor = sp.edit();
            String key = String.format(SIM_STATUS, mPhoneId);
            editor.putInt(key, enable ? 1 : 0);
            editor.commit();
        }
        super.enableUiccApplications(enable, onCompleteMessage);
    }

    private void logd(String msg) {
        Rlog.d(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }

    private void loge(String msg) {
        Rlog.e(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }
}
