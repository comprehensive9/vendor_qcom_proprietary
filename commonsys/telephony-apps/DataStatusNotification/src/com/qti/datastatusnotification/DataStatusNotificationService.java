/*
 * Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qti.qualcomm.datastatusnotification;

import java.lang.Exception;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.IBinder;
import android.os.SystemProperties;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.provider.Telephony;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import com.android.internal.telephony.PhoneConstants;
import com.qualcomm.qcrilhook.IQcRilHook;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;

public class DataStatusNotificationService extends Service {
    private static final String TAG = "QcDataStatusNotification Service";
    private static final boolean DBG = true;
    private static final String ALL_APNS = "default,mms,dun,hipri,fota,ims,cbs,ia,supl";

    private static final String MOBILE_DATA = Settings.Global.MOBILE_DATA;
    private static final String DATA_ROAMING = Settings.Global.DATA_ROAMING;
    private static final String CARRIERS = "carriers";
    private static final Uri CARRIERS_URI = Telephony.Carriers.CONTENT_URI;
    private static boolean mEnableMsimLldd =
            SystemProperties.getBoolean("persist.vendor.radio.msim.lldd", false);
    private QcRilHook mQcRilHook;
    private TelephonyManager mTm;
    private SubscriptionManager mSubscriptionManager;
    private int mPhoneCount;
    private DataSettingsObserver mDataSettingsObserver;
    private ApnCache[] mApnCache;
    private HashMap<Integer, Boolean> mSubEnabledMap;
    private ContentResolver mResolver;
    boolean mIsQcRilHookReady = false;
    private PhoneStateListener[] mPhoneStateListeners;
    private boolean mCallActive = false;

    private class Apn {
        private String mType;
        private String mApn;

        public Apn(String type, String apn) {
            mType = type;
            mApn = apn;
        }

        public String getType() {
            return mType;
        }

        public String getName() {
            return mApn;
        }

        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("[ Type=");
            sb.append(mType);
            sb.append(", Apn=");
            sb.append(mApn);
            sb.append("]");
            return sb.toString();
        }
    }

    private QcRilHookCallback mQcRilHookCb = new QcRilHookCallback() {
        public void onQcRilHookReady() {
            mIsQcRilHookReady = true;
            log("mPhoneCount = " + mPhoneCount);
            mDataSettingsObserver = new DataSettingsObserver();
            mSubEnabledMap = new HashMap<Integer, Boolean>();
            // Register content observer for carriers DB for changes in APN
            mResolver.registerContentObserver(CARRIERS_URI, false, mDataSettingsObserver);

            for (int i = 0; i < mPhoneCount; i++) {
                int[] subIds = mSubscriptionManager.getSubscriptionIds(i);
                if (subIds != null) {
                    if (SubscriptionManager.isUsableSubscriptionId(subIds[0])) {
                        Uri dataUri = formUri(MOBILE_DATA, subIds[0]);
                        Uri roamingUri = formUri(DATA_ROAMING, subIds[0]);
                        log("enableContentObservers for subId = " + subIds[0]);
                        enableContentObservers(dataUri, roamingUri);
                        // Explicitly trigger onChange() at start of service
                        // since modem needs to know the values at start of service
                        triggerOnChange(dataUri, roamingUri);
                        mSubEnabledMap.put(subIds[0], true);
                        registerPhoneStateListener(i, subIds[0]);
                    } else {
                        log("Subscription id is not usable: " + subIds[0]);
                    }
                } else {
                    log("getSubId returned null array");
                }
            }
            registerSimStateReceiver();
        }

        public void onQcRilHookDisconnected() {
            mIsQcRilHookReady = false;
        }
    };

    /* Register sim state receiver for loaded event */
    private void registerSimStateReceiver() {
        registerReceiver(mSimStateReceiver, new IntentFilter(
                TelephonyManager.ACTION_SIM_APPLICATION_STATE_CHANGED));
    }

    private BroadcastReceiver mSimStateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();
            if (action.equals(TelephonyManager.ACTION_SIM_APPLICATION_STATE_CHANGED)) {
                int stateExtra = intent.getIntExtra(TelephonyManager.EXTRA_SIM_STATE,
                        TelephonyManager.SIM_STATE_UNKNOWN);
                int subId = intent.getIntExtra(SubscriptionManager.EXTRA_SUBSCRIPTION_INDEX,
                        SubscriptionManager.INVALID_SUBSCRIPTION_ID);
                int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                        SubscriptionManager.INVALID_PHONE_INDEX);
                log("onReceive state=" + stateExtra + ", subId = " + subId +
                        ", phoneId=" + phoneId);
                if (subId != SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
                    if (stateExtra == TelephonyManager.SIM_STATE_LOADED) {
                        Uri dataUri = formUri(MOBILE_DATA, subId);
                        Uri roamingUri = formUri(DATA_ROAMING, subId);
                        if (!mSubEnabledMap.containsKey(subId) || !mSubEnabledMap.get(subId)) {
                            mSubEnabledMap.put(subId, true);
                            log("enableContentObservers subId = " + subId);
                            enableContentObservers(dataUri, roamingUri);
                        }
                        triggerOnChange(dataUri, roamingUri);
                        registerPhoneStateListener(phoneId, subId);
                    }
                }
                if ((stateExtra == TelephonyManager.SIM_STATE_NOT_READY ||
                        stateExtra == TelephonyManager.SIM_STATE_UNKNOWN) &&
                        (phoneId >= 0 && phoneId < mPhoneCount)) {
                    unRegisterPhoneStateListener(phoneId);
                }
            }
        }
    };

    private class DataSettingsObserver extends ContentObserver {
        DataSettingsObserver() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            if (!mIsQcRilHookReady) {
                Log.e(TAG, "QcRilhook not read. Bail out");
                return;
            }

            if (uri != null) {
                String authority = uri.getAuthority();
                String uriLastSegment = uri.getLastPathSegment();
                int slotId = 0;
                int subId = 0;
                if (authority.equals("settings")) { // authority for mobile_data and roaming
                    if (mPhoneCount > 1) {
                        /*
                         * For multi-sim, the URI last segment contains subId in the format
                         * mobile_data0
                         */
                        String[] lastSegmentParts = uriLastSegment.split("\\d");
                        int uriLength = uriLastSegment.length();
                        int keyLength = lastSegmentParts[0].length();
                        subId = Integer.parseInt(uriLastSegment.substring(keyLength, uriLength));
                        slotId = SubscriptionManager.getSlotIndex(subId);
                        uriLastSegment = uriLastSegment.substring(0, keyLength);
                        log("MultiSim onChange(): subId = " + subId);
                    }
                }
                log("onChange():uri=" + uri.toString() + " authority=" + authority
                            + " path=" + uri.getPath() + " segments=" + uri.getPathSegments()
                            + " uriLastSegment=" + uriLastSegment);
                switch (uriLastSegment) {
                    case MOBILE_DATA:
                        TelephonyManager tm = mTm.createForSubscriptionId(subId);
                        int mobile_data_status = tm.isDataEnabled() ? 1 : 0;
                        log("handleMessage: Data Enable changed to "
                                    + mobile_data_status
                                    + " on slot = " + slotId);
                        mQcRilHook.qcRilSendDataEnableStatus(mobile_data_status, slotId);
                        break;
                    case DATA_ROAMING:
                        int data_roaming_status = 0;
                        try {
                            if (mPhoneCount > 1) {
                                data_roaming_status = Settings.Global.getInt(
                                        mResolver, DATA_ROAMING + subId);
                            } else {
                                data_roaming_status = Settings.Global.getInt(
                                        mResolver, DATA_ROAMING);
                            }
                        } catch (SettingNotFoundException ex) {
                            Log.e(TAG, ex.getMessage());
                        }
                        log("handleMessage: Data Roaming changed to "
                                    + data_roaming_status
                                    + " on slot = " + slotId);
                        mQcRilHook.qcRilSendDataRoamingEnableStatus(data_roaming_status, slotId);
                        break;
                    case CARRIERS:
                        for (int i = 0; i < mPhoneCount; i++) {
                            onApnChanged(i);
                        }
                        break;
                    default:
                        Log.e(TAG, "Received unsupported uri");
                }
            } else
                Log.e(TAG, "Received uri is null");
        }
    }

    private void triggerOnChange(Uri dataUri, Uri roamingUri) {
        mDataSettingsObserver.onChange(false, dataUri);
        mDataSettingsObserver.onChange(false, roamingUri);
        mDataSettingsObserver.onChange(false, CARRIERS_URI);
    }

    private Uri formUri(String uriStr, int subId) {
        Uri result = null;
        if (mPhoneCount > 1) {
            result = Settings.Global.getUriFor(uriStr + subId);
        } else {
            result = Settings.Global.getUriFor(uriStr);
        }
        return result;
    }

    private void enableContentObservers(Uri dataUri, Uri roamingUri) {
        // Data Enable Content Observer
        mResolver.registerContentObserver(dataUri, false, mDataSettingsObserver);

        // Data Roaming Content Observer
        mResolver.registerContentObserver(roamingUri, false, mDataSettingsObserver);
    }

    private void onApnChanged(int slotId) {
        Apn newApn = null;
        Apn newImsApn = null;
        Apn preferredApn = null;
        // Get subIds for a particular slotId. Only first subId is relevant for now
        int[] subIdList = mSubscriptionManager.getSubscriptionIds(slotId);
        if (subIdList == null)
            return;
        int subId = subIdList[0];

        // Find operator of the subId where APN has changed
        String operator = mTm.createForSubscriptionId(subId).getSimOperator();
        log("onApnChanged: slot = " + slotId + " subId = "
                + subId + " operator = " + operator);
        // For operator, get cursor from DB
        if (operator != null && !operator.isEmpty()) {
            ApnCache currentApnCache = new ApnCache();

            String selection = "numeric = '" + operator + "'";
            selection += " and carrier_enabled = 1";
            Cursor cursor = mResolver.query(getUri(Telephony.Carriers.CONTENT_URI, subId),
                    new String[] {
                        "apn", "numeric", "type", "carrier_enabled"
                    },
                    selection, null, null);

            log("APN change URI is  "
                    + getUri(Telephony.Carriers.CONTENT_URI, subId).toString()
                    + " count = " + cursor.getCount());

            if (cursor != null && cursor.getCount() > 0) {
                // Try to get preferredApn if preferred APN is set, which can handle
                // default type and has the correct operator
                // else get first APN that can handle default type
                preferredApn = getPreferredApn(subId, selection);
                if (preferredApn != null) {
                    log("Found preferred APN: " + preferredApn.toString());
                } else {
                    preferredApn = getDefaultApn(cursor);
                    if (preferredApn != null) {
                        log("Found default APN: " + preferredApn.toString());
                    } else {
                        log("Preferred or default APN not found");
                    }
                }
                if (cursor.moveToFirst()) {
                    do {
                        newApn = extractApn(cursor);
                        //Store in new cache instance.
                        currentApnCache.put(newApn.getName(), newApn.getType());
                    } while (cursor.moveToNext());
                }
            } else {
                log("No rows in Carriers DB for current operator");
            }
            if (cursor != null) {
                cursor.close();
            }

            /**
             * Inform modem of apns which are no longer relevant because
             * user removed them.
             */
            ApnCache apnCache = mApnCache[slotId];
            if (apnCache != null) {
                for (String apn : apnCache.getMissingKeys(currentApnCache)) {
                    log("Apn = " + apn + " removed, inform modem.");

                    /** Inform modem about apn removal via oem hook */
                    mQcRilHook.qcRilSendApnInfo("", apn, 0 /* disable */,
                                slotId);
                }
            }

            /**
             * Update apn cache to current snapshot
             */
            mApnCache[slotId] = currentApnCache;

            /**
             * Inform modem of current apns snapshot if L+L property is enabled
             */
            if (!mEnableMsimLldd) {
                informCurrentApnsToModemForPhoneId(slotId, preferredApn);
            } else {
                mQcRilHook.qcRilSendApnInfo(preferredApn.getType(), preferredApn.getName(),
                                        1 /* enable */, slotId);
            }

        } else {
            log("Could not get current operator");
        }

    }

    private Apn getApnFound(Cursor cursor, String type) {
        Apn apn = null;
        String[] typesSupported = parseTypes(cursor.getString(
                cursor.getColumnIndexOrThrow(Telephony.Carriers.TYPE)));
        log("getApnFound: typesSupported = " + Arrays.toString(typesSupported) +
            " type requested = " + type);
        for (String t : typesSupported) {
            if (t.equalsIgnoreCase(PhoneConstants.APN_TYPE_ALL) ||
                    t.equalsIgnoreCase(type)) {
                apn = new Apn(
                        type,
                        cursor.getString(cursor.getColumnIndexOrThrow(
                                        Telephony.Carriers.APN)));
                log("getApnFound: Apn = " + apn.toString());
                break;
            }
        }
        return apn;
    }

    private Apn getPreferredApn(int subId, String selection) {
        Uri PREFERAPN_URI = Uri.parse("content://telephony/carriers/preferapn");
        Apn preferredApn = null;
        Cursor cursor = mResolver.query(getUri(PREFERAPN_URI, subId),
                new String[] {
                    "apn", "numeric", "type", "carrier_enabled"
                },
                selection, null, null);
        if (cursor != null && cursor.getCount() > 0) {
            cursor.moveToFirst();
            preferredApn = getApnFound(cursor, PhoneConstants.APN_TYPE_DEFAULT);
        }
        if(cursor != null) {
            cursor.close();
        }
        return preferredApn;
    }

    private Apn getDefaultApn(Cursor cursor) {
        Apn defaultApn = null;
        while (defaultApn == null && cursor.moveToNext()) {
            defaultApn = getApnFound(cursor, PhoneConstants.APN_TYPE_DEFAULT);
        }
        return defaultApn;
    }

    private void dump(ArrayList<Apn> apns) {
        for (Apn apn : apns) {
            log(apn.toString());
        }
    }

    private void informCurrentApnsToModemForPhoneId(int slotId, Apn preferredApn) {
        log("InformCurrentApnsToModemForPhoneId: " + slotId);
        ApnCache apnCache = mApnCache[slotId];
        if (apnCache != null) {
            ArrayList<Apn> apns= new ArrayList<Apn>();
            for (String apn : apnCache.keySet()) {
                String type = apnCache.get(apn);
                apns.add(new Apn(type, apn));
                if (preferredApn != null && preferredApn.getName().equals(apn)) {
                    log("Adding preferred type to: " + preferredApn.getName());
                    type += ",preferred";
                }

                /*
                * Inform modem about apn name and type via oem hook
                */
                mQcRilHook.qcRilSendApnInfo(type, apn,
                        1 /* enable */, slotId);
            }

            log("***********************");
            log("Current APNs for PhoneId = " + slotId);
            dump(apns);
            log("***********************");
        }
    }

    private Apn extractApn(Cursor cursor) {
        Apn apn = null;
        String typesSupported = cursor.getString(cursor
                .getColumnIndexOrThrow(Telephony.Carriers.TYPE));
        if (typesSupported.isEmpty() ||
                typesSupported.equalsIgnoreCase(PhoneConstants.APN_TYPE_ALL)) {
            typesSupported = ALL_APNS;
        }

        apn = new Apn(
                typesSupported,
                cursor.getString(
                        cursor.getColumnIndexOrThrow(Telephony.Carriers.APN)));
        return apn;
    }

    private String[] parseTypes(String types) {
        String[] result;
        // If unset, set to DEFAULT.
        if (TextUtils.isEmpty(types)) {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_ALL;
        } else {
            result = types.split(",");
        }
        return result;
    }

    private Uri getUri(Uri uri, int subId) {
        return Uri.withAppendedPath(uri, "subId/" + subId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        log("onStartCommand");
        mTm = (TelephonyManager) DataStatusNotificationService.this.
                getSystemService(Context.TELEPHONY_SERVICE);
        mSubscriptionManager = (SubscriptionManager) DataStatusNotificationService.this.
                getSystemService(Context.TELEPHONY_SUBSCRIPTION_SERVICE);
        mPhoneCount = mTm.getPhoneCount();
        mPhoneStateListeners = new PhoneStateListener[mPhoneCount];
        mResolver = DataStatusNotificationService.this.getContentResolver();
        if (mQcRilHook == null) {
            mQcRilHook = new QcRilHook(this, mQcRilHookCb);
            mApnCache = new ApnCache[mPhoneCount];
        }
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        log("onDestroy");
        if (mDataSettingsObserver != null) {
            mResolver.unregisterContentObserver(mDataSettingsObserver);
            unregisterReceiver(mSimStateReceiver);
        }
        if (mQcRilHook != null) {
            mQcRilHook.dispose();
        }
        unRegisterPhoneStateListeners();
        mSubEnabledMap.clear();
        mTm = null;
        mPhoneCount = 0;
        mApnCache = null;
    }

    private void registerPhoneStateListener(int phoneId, int subId) {
        if (mPhoneStateListeners[phoneId] == null) {
            log("registerPhoneStateListener: phoneId=" + phoneId);
            TelephonyManager tm = mTm.createForSubscriptionId(subId);
            tm.listen(getPhoneStateListener(phoneId, subId),
                    PhoneStateListener.LISTEN_CALL_STATE);
        }
    }
    private void unRegisterPhoneStateListener(int phoneId) {
        if (mPhoneStateListeners[phoneId] != null) {
            log("unRegisterPhoneStateListener: phoneId=" + phoneId);
            mTm.listen(mPhoneStateListeners[phoneId], PhoneStateListener.LISTEN_NONE);
            mPhoneStateListeners[phoneId] = null;
        }
    }

    private void unRegisterPhoneStateListeners() {
        log("unRegisterPhoneStateListeners");
        for (int i=0; i < mPhoneCount; i++) {
            unRegisterPhoneStateListener(i);
        }
        mPhoneStateListeners = null;
    }

    private PhoneStateListener getPhoneStateListener(int phoneId, int subId) {
        final int i = phoneId;
        final int subscriptionId = subId;
        if (mPhoneStateListeners[phoneId] == null) {
            mPhoneStateListeners[phoneId] = new PhoneStateListener() {
                @Override
                public void onCallStateChanged(int state, String incomingNumber) {
                    log("onCallStateChange: state=" + state);
                    updateDataStatus(i, subscriptionId, state);
                }
            };
        }
        return mPhoneStateListeners[phoneId];
    }

    /*
     * Method to overwrite mobile data setting for non dds sub
     * if Data during calls is enabled
     */
    private void updateDataStatus(int phoneId, int subId, int state) {
        int ddsSubId = mSubscriptionManager.getDefaultDataSubscriptionId();
        log("updateDataStatus: phoneId=" + phoneId + ", subId=" + subId +
                ", ddsSubId=" + ddsSubId);
        if (state == TelephonyManager.CALL_STATE_IDLE) {
            //Reset this flag for call end
            mCallActive = false;
        }

        if (mPhoneCount <= 1 || subId == ddsSubId) {
            log("updateDataStatus: skip processing");
            return;
        }

        if (!mCallActive) {
            try {
                int mobileDataStatus = Settings.Global.getInt(
                        mResolver, MOBILE_DATA + subId);
                //Override mobile data setting only if it is disabled
                if (mobileDataStatus == 0) {
                    mCallActive = (state != TelephonyManager.CALL_STATE_IDLE);
                    TelephonyManager tm = mTm.createForSubscriptionId(subId);
                    boolean isDataAllowedInVoiceCall = tm.isDataAllowedInVoiceCall();
                    log("updateDataStatus: isDataAllowedInVoiceCall=" + isDataAllowedInVoiceCall);
                    if (isDataAllowedInVoiceCall) {
                        mobileDataStatus = mCallActive ? 1 : 0;
                        log("updateDataStatus: Data Enable changed to " + mobileDataStatus
                                + " on phone" + phoneId);
                        mQcRilHook.qcRilSendDataEnableStatus(mobileDataStatus, phoneId);
                    }
                }
            } catch (SettingNotFoundException ex) {
                Log.e(TAG, ex.getMessage());
            }
        }
    }

    private void log(String str) {
        if (DBG) {
            Log.d(TAG, str);
        }
    }
}
