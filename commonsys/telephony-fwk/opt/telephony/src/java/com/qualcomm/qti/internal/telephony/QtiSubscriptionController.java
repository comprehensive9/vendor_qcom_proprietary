/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
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
import android.os.RemoteException;
import android.os.SystemProperties;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.util.Log;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.RIL;
import com.android.internal.telephony.vendor.VendorMultiSimSettingController;
import com.android.internal.telephony.vendor.VendorSubscriptionController;

import java.util.List;

/*
 * Extending VendorSubscriptionController here:
 * To implement fall back of sms/data user preferred subId value to next
 * available subId when current preferred SIM deactivated or removed.
 */
public class QtiSubscriptionController extends VendorSubscriptionController {
    static final String LOG_TAG = "QtiSubscriptionController";

    private static final int PROVISIONED = 1;
    private static final int NOT_PROVISIONED = 0;

    private static final String APM_SIM_NOT_PWDN_PROPERTY = "persist.vendor.radio.apm_sim_not_pwdn";

    // FIXME remove below CT Class A specific code
    // check current carrier is CT class A, C or not set
    private static final String CARRIER_MODE_CT_CLASS_A = "ct_class_a";
    private String mCarrierMode = SystemProperties.get("persist.vendor.radio.carrier_mode",
            "default");
    private boolean mIsCTClassA = mCarrierMode.equals(CARRIER_MODE_CT_CLASS_A);

    private static final int DEFAULT_PHONE_INDEX = 0;

    public static QtiSubscriptionController init(Context c) {
        synchronized (QtiSubscriptionController.class) {
            if (sInstance == null) {
                sInstance = new QtiSubscriptionController(c);
            } else {
                Log.wtf(LOG_TAG, "init() called multiple times!  sInstance = " + sInstance);
            }
            return (QtiSubscriptionController)sInstance;
        }
    }

    public static QtiSubscriptionController getInstance() {
        if (sInstance == null) {
           Log.wtf(LOG_TAG, "getInstance null");
        }

        return (QtiSubscriptionController)sInstance;
    }

    private QtiSubscriptionController(Context c) {
        super(c);
        if (DBG) logd(" init by Context");
    }

    @Override
    public void setDefaultDataSubId(int subId) {
        enforceModifyPhoneState("setDefaultDataSubId");
        String flexMapSupportType =
                SystemProperties.get("persist.vendor.radio.flexmap_type", "dds");

        // When flex map type is set as "dds" and both Phones bind to same stackId, start initiate
        // normal mapping request.
        if (SubscriptionManager.isValidSubscriptionId(subId) && flexMapSupportType.equals("dds")) {
            QtiRadioCapabilityController radioCapController =
                    QtiRadioCapabilityController.getInstance();

            if (radioCapController.isBothPhonesMappedToSameStack()) {
                radioCapController.initNormalMappingRequest();
                logd(" setDefaultDataSubId init normal mapping: " + subId);
            }
            super.setDefaultDataSubId(subId);
        } else {
            int previousDefaultSub = getDefaultSubId();
            Settings.Global.putInt(mContext.getContentResolver(),
                    Settings.Global.MULTI_SIM_DATA_CALL_SUBSCRIPTION, subId);
            VendorMultiSimSettingController.getInstance().notifyDefaultDataSubChanged();
            broadcastDefaultDataSubIdChanged(subId);
            if (previousDefaultSub != getDefaultSubId()) {
                sendDefaultChangedBroadcast(getDefaultSubId());
            }
        }
    }

    @Override
    public boolean isRadioInValidState() {
        int simNotPwrDown = 0;
        try {
            simNotPwrDown = QtiTelephonyComponentFactory.getInstance().getRil(DEFAULT_PHONE_INDEX).
                    getPropertyValueInt(APM_SIM_NOT_PWDN_PROPERTY, 0);
        } catch (RemoteException|NullPointerException ex) {
            loge("Exception: " + ex);
        }
        boolean isApmSimNotPwrDown = (simNotPwrDown == 1);
        int isAPMOn = Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0);

        // Do not updateUserPrefs, SIM is pwdn due to APM, as apm_sim_not_pwdn flag is not set.
        if ((isAPMOn == 1) && (!isApmSimNotPwrDown)) {
            logd("isRadioInValidState, isApmSimNotPwrDown = " + isApmSimNotPwrDown
                    + ", isAPMOn:" + isAPMOn);
            return false;
        }

        return super.isRadioInValidState();
    }

    @Override
    protected void handleDataPreference(int nextActiveSubId) {
        List<SubscriptionInfo> subInfoList =
                getActiveSubscriptionInfoList(mContext.getOpPackageName());
        if (subInfoList == null) {
            return;
        }
        logd(" handleDataPreference ");

        // FIXME re-design and move below change out of Phone process
        // switch DDS to sub1 for CT classA mode.
        if (mIsCTClassA && isSubProvisioned(getSubId(PhoneConstants.SUB1)[0])) {
            logd("set dds to slot0 for ct classA mode");
            setDefaultDataSubId(getSubId(PhoneConstants.SUB1)[0]);
        } else {
            super.handleDataPreference(nextActiveSubId);
        }

        // Check and set DDS after sub activation
        QtiRadioCapabilityController radioCapController =
                QtiRadioCapabilityController.getInstance();
        radioCapController.setDdsIfRequired(false);
    }

    private int getUiccProvisionStatus(int slotId) {
        QtiUiccCardProvisioner uiccCardProvisioner = QtiUiccCardProvisioner.getInstance();
        if (uiccCardProvisioner != null) {
            return uiccCardProvisioner.getCurrentUiccCardProvisioningStatus(slotId);
        } else {
            return NOT_PROVISIONED;
        }
    }

    // This method returns true if subId and corresponding slotId is in valid
    // range and the Uicc card corresponds to this slot is provisioned.
    @Override
    protected boolean isSubProvisioned(int subId) {
        boolean isSubIdUsable = SubscriptionManager.isUsableSubIdValue(subId);

        if (isSubIdUsable) {
            int slotId = getSlotIndex(subId);
            if (!SubscriptionManager.isValidSlotIndex(slotId)) {
                loge(" Invalid slotId " + slotId + " or subId = " + subId);
                isSubIdUsable = false;
            } else {
                if (getUiccProvisionStatus(slotId) != PROVISIONED) {
                    isSubIdUsable = false;
                }
                loge("isSubProvisioned, state = " + isSubIdUsable + " subId = " + subId);
            }
        }
        return isSubIdUsable;
    }

    // FIXME remove this code
    /* Returns User SMS Prompt property,  enabled or not */
    public boolean isSMSPromptEnabled() {
        boolean prompt = false;
        int value = 0;
        try {
            value = Settings.Global.getInt(mContext.getContentResolver(),
                    Settings.Global.MULTI_SIM_SMS_PROMPT);
        } catch (SettingNotFoundException snfe) {
            loge("Settings Exception Reading Dual Sim SMS Prompt Values");
        }
        prompt = (value == 0) ? false : true ;
        if (VDBG) logd("SMS Prompt option:" + prompt);

       return prompt;
    }

    /*Sets User SMS Prompt property,  enable or not */
    public void setSMSPromptEnabled(boolean enabled) {
        enforceModifyPhoneState("setSMSPromptEnabled");
        int value = (enabled == false) ? 0 : 1;
        Settings.Global.putInt(mContext.getContentResolver(),
                Settings.Global.MULTI_SIM_SMS_PROMPT, value);
        logi("setSMSPromptOption to " + enabled);
    }

    @Override
    public boolean setSubscriptionEnabled(boolean enable, int subId) {
        enforceModifyPhoneState("setSubscriptionEnabled");

        int slotId = getSlotIndex(subId);
        int simState = getUiccProvisionStatus(slotId);
        boolean currentStatus = (simState == PROVISIONED);

        if (enable == currentStatus) {
            if (DBG) logd("setSubscriptionEnabled: block request, enable=" + enable +
                    ", currentStatus=" + currentStatus);
            return false;
        } else {
            return super.setSubscriptionEnabled(enable, subId);
        }
    }

    @Override
    public int setUiccApplicationsEnabled(boolean enabled, int subId) {
        if (DBG) logd("[setUiccApplicationsEnabled]+ enabled:" + enabled + " subId:" + subId);
        int result;

        Phone phone = PhoneFactory.getPhone(SubscriptionManager.getPhoneId(subId));

        if (phone != null && phone.getHalVersion().greaterOrEqual(RIL.RADIO_HAL_VERSION_1_5)) {
            if (DBG) logd("setUiccApplicationsEnabled HAL is 1.5");
            result = super.setUiccApplicationsEnabled(enabled, subId);
        } else {
            int slotId = getSlotIndex(subId);
            if (enabled) {
                if (DBG) logd("setUiccApplicationsEnabled: using legacy api activateUiccCard");
                result = QtiUiccCardProvisioner.getInstance().activateUiccCard(slotId);
            } else {
                if (DBG) logd("setUiccApplicationsEnabled: using legacy api deactivateUiccCard");
                result = QtiUiccCardProvisioner.getInstance().deactivateUiccCard(slotId);
            }
        }

        return result;
    }

    @Override
    protected boolean isUiccProvisioned(int slotId) {
        int[] subId = getSubId(slotId);

        if (subId != null && (subId.length > 0)) {
            return isSubProvisioned(subId[0]);
        }
        return false;
    }

    private void logd(String string) {
        if (DBG) Rlog.d(LOG_TAG, string);
    }

    private void logi(String string) {
        Rlog.i(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}
