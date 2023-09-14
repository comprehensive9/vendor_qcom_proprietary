/*
 * Copyright (c) 2015, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
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

package com.qualcomm.qti.internal.telephony.dataconnection;

import android.telephony.AccessNetworkConstants;
import android.os.AsyncResult;
import android.os.Message;
import android.telephony.CarrierConfigManager;
import android.telephony.data.ApnSetting;
import android.telephony.Rlog;
import com.android.internal.telephony.DctConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.vendor.dataconnection.VendorDcTracker;
import com.qualcomm.qti.internal.telephony.QtiTelephonyComponentFactory;
import com.qualcomm.qti.internal.telephony.QtiRIL;
import java.util.ArrayList;

public final class QtiDcTracker extends VendorDcTracker {
    private String LOG_TAG = "QtiDCT";

    private static final int GID = 0;
    private static final int APN_TYPE = 1;
    private static final int DEVICE_CAPABILITY = 2;
    private static final int APN_NAME = 3;
    private static final int KEY_MULTI_APN_ARRAY_FOR_SAME_GID_ENTRY_LENGTH = 4;

    private QtiRadioCapabilityResponse mQtiRadioCapability;

    private void initQtiRadioCapability() {
        mQtiRadioCapability = new QtiRadioCapabilityResponse();
        mPhone.mCi.getQtiRadioCapability(obtainMessage(
                DctConstants.EVENT_GET_RADIO_CAPABILITY));
    }

    // Constructor
    public QtiDcTracker(Phone phone, int transportType) {
        super(phone, transportType);
        LOG_TAG +=
                 (transportType == AccessNetworkConstants.TRANSPORT_TYPE_WWAN) ? "-C" : "-I";
        initQtiRadioCapability();
    }

    @Override
    protected void log(String s) {
        Rlog.d(LOG_TAG, "[" + mPhone.getPhoneId() + "]" + s);
    }

    @Override
    public void handleMessage (Message msg) {
        if (VDBG) log("handleMessage msg=" + msg);
        AsyncResult ar;

        switch (msg.what) {
            case DctConstants.EVENT_GET_RADIO_CAPABILITY:
                log("EVENT_GET_RADIO_CAPABILITY");
                ar = (AsyncResult) msg.obj;
                if (ar == null || ar.result == null || ar.exception != null) {
                    loge("get radio capability: null");
                    return;
                }
                int raf = (int) ar.result;
                mQtiRadioCapability.updateQtiRadioCapability(raf);
                break;
            default:
                super.handleMessage(msg);
        }
    }

    /*Filters out multipe apns based on radio capability if the APN's GID value is listed in
    CarrierConfigManager#KEY_MULTI_APN_ARRAY_FOR_SAME_GID as per the operator requirement*/
    @Override
    protected void filterApnSettingsWithRadioCapability() {
        if (VDBG) {
            log(" filterApnSettingsWithRadioCapability start: mAllApnSettings:" + mAllApnSettings);
        }
        int i = 0;
        while (i < mAllApnSettings.size()) {
            ApnSetting apn = mAllApnSettings.get(i);
            String apnType = ApnSetting.getApnTypesStringFromBitmask(apn.getApnTypeBitmask());
            if(apn.hasMvnoParams() && (apn.getMvnoType() == ApnSetting.MVNO_TYPE_GID) &&
                    isApnFilteringRequired(apn.getMvnoMatchData(), apnType)) {
                String apnName = getApnBasedOnRadioCapability(apn.getMvnoMatchData(),
                        apnType, mQtiRadioCapability.getQtiRadioCapability());
                if(apnName != null && !apnName.equals(apn.getApnName())) {
                    mAllApnSettings.remove(i);
                    log("filterApnSettingsWithRadioCapability: removed not supported apn:" + apn);
                } else {
                    i++;
                }
            } else {
                i++;
            }
        }
        if(VDBG) {
            log("filterApnSettingsWithRadioCapability: end: mAllApnSettings:" + mAllApnSettings);
        }
    }

     /**
     * Check if APN filtering is required
     *
     * @param gid gid value of the apn.
     * @param apnType  apn type.
     * @return True if multipe apns present in CarrierConfigManager#KEY_MULTI_APN_ARRAY_FOR_SAME_GID
               for this gid .
     */
    private boolean isApnFilteringRequired(String gid, String apnType) {
        final String[] apnConfig =
                CarrierConfigManager.getDefaultConfig().
                getStringArray(CarrierConfigManager.KEY_MULTI_APN_ARRAY_FOR_SAME_GID);
        for(String apnEntry: apnConfig) {
            String[] split = apnEntry.split(":");
            if (split.length == KEY_MULTI_APN_ARRAY_FOR_SAME_GID_ENTRY_LENGTH) {
                if(gid.equals(split[GID]) && apnType.equals(split[APN_TYPE])) {
                    return true;
                }
            }
        }

        return false;
    }

     /**
     * Return apn name based on the device capability if the coresponding
     * entry present in CarrierConfigManager#KEY_MULTI_APN_ARRAY_FOR_SAME_GID
     *
     * @param gid gid value of the apn.
     * @param apnType  apn type.
     * @param deviceCapability  device capability, Ex: SA, NSA, LTE etc..
     * @return apn name from the entry matches with all the above three params.
     */

    private String getApnBasedOnRadioCapability(String gid,String apnType,
            String deviceCapability) {
        if(deviceCapability == null) {
            loge("getApnBasedOnRadioCapability: deviceCapability is null");
            return null;
        }
        final String[] apnConfig =
                CarrierConfigManager.getDefaultConfig().
                getStringArray(CarrierConfigManager.KEY_MULTI_APN_ARRAY_FOR_SAME_GID);
        for(String apnEntry: apnConfig) {
            String[] split = apnEntry.split(":");
            if (split.length == KEY_MULTI_APN_ARRAY_FOR_SAME_GID_ENTRY_LENGTH &&
                    deviceCapability != null) {
                if(gid.equals(split[GID]) && apnType.equals(split[APN_TYPE]) &&
                        deviceCapability.equals(split[DEVICE_CAPABILITY])) {
                    return split[APN_NAME];
                }
            }
        }
        return null;
    }
}
