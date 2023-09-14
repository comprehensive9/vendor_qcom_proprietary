/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
*  Copyright (c) 2012-2014, 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*  2012-2014 Qualcomm Atheros, Inc.
*  All Rights Reserved.
*  Qualcomm Atheros Confidential and Proprietary.
=============================================================================*/

package com.qualcomm.location;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.HwBinder;
import android.provider.Telephony.Sms.Intents;
import android.telephony.CellInfo;
import android.telephony.CellLocation;
import android.telephony.CellInfoGsm;
import android.telephony.CellIdentityGsm;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.SmsMessage;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import android.telephony.gsm.GsmCellLocation;
import com.android.internal.telephony.TelephonyIntents;
import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.lang.ClassCastException;
import java.lang.ArrayIndexOutOfBoundsException;
import com.qualcomm.location.hidlclient.BaseHidlClient;
import com.qualcomm.location.hidlclient.HidlClientUtils;
import com.qualcomm.location.utils.IZatServiceContext;
import com.qualcomm.location.izat.IzatService;

import vendor.qti.gnss.V1_0.ILocHidlRilInfoMonitor;
import vendor.qti.gnss.V3_0.ILocHidlGnss;
public class RilInfoMonitor implements IzatService.ISystemEventListener, Handler.Callback {
    private static final String TAG = "RilInfoMonitor";
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);
    private static final int MSG_START =            IZatServiceContext.MSG_RILINFO_MONITOR_BASE + 1;
    private static final int MSG_CID_INJECT =       IZatServiceContext.MSG_RILINFO_MONITOR_BASE + 2;
    private static final int MSG_OOS_INJECT =       IZatServiceContext.MSG_RILINFO_MONITOR_BASE + 3;
    private static final int MSG_SMS_INJECT =       IZatServiceContext.MSG_RILINFO_MONITOR_BASE + 4;
    private static final int MSG_SMS_MULTI_INJECT = IZatServiceContext.MSG_RILINFO_MONITOR_BASE + 5;
    private static final int MSG_MAX =              5;
    private TelephonyManager mTelMgr;
    private RilInfoMonitorHidlClient mRemoteServiceClient;
    private final IZatServiceContext mIZatServiceCtx;
    private final Handler mHandler;

    @Override
    public void notify(int msgId, Object... args) {
        if (MSG_RIL_INFO == msgId) {
            Intent intent = (Intent)args[0];
            String action = intent.getAction();

            logv("mBroadcastReceiver - " + action);

            if (action.equals(Intents.DATA_SMS_RECEIVED_ACTION)) {
                SmsMessage[] messages = Intents.getMessagesFromIntent(intent);
                if (messages != null) {
                    for (int i=0; i <messages.length; i++) {
                        sendMessage(MSG_SMS_INJECT, 0, 0, messages[i].getUserData());
                    }
                }
            } else if (action.equals(Intents.WAP_PUSH_RECEIVED_ACTION)) {
                sendMessage(MSG_SMS_INJECT, 0, 0, intent.getByteArrayExtra("data"));
            } else if (action.equals(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED)) {
                logv("Received intent TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED.");
                injectCellInfo();
            }
        } else if (MSG_CELL_ID_CHANGE == msgId) {
            CellLocation location = (CellLocation)args[0];
            Log.v(TAG, "onCellLocationChanged() called, location:  " + location);
            injectCellInfo();
        } else if (MSG_SERVICE_STATE_CHANGE == msgId) {
            ServiceState serviceState = (ServiceState)args[0];
            Log.v(TAG, "onServiceStateChanged () called, serviceState: " + serviceState);
            if (ServiceState.STATE_OUT_OF_SERVICE == serviceState.getState() &&
                ServiceState.STATE_OUT_OF_SERVICE == serviceState.getDataRegState()) {
                if (VERBOSE_DBG) {
                    logv("onServiceStateChanged - OOS");
                }

                sendMessage(MSG_OOS_INJECT, 0, 0, null);
            }
        } else if (MSG_CALL_STATE_CHANGE == msgId) {
            int state = (int)args[0];
            Log.v(TAG, "onCallStateChanged(): "
                    +"state is "+ state);
            // listening for emergency call ends
            if (TelephonyManager.CALL_STATE_IDLE == state) {
                Log.d(TAG, "state == TelephonyManager.CALL_STATE_IDLE");
                if (null == GpsNetInitiatedHandler.obj) {
                    Log.e(TAG, "GpsNetInitiatedHandler is NULL!");
                } else {
                    Log.d(TAG, "calling GpsNetInitiatedHandler"
                         +".obj.updateEmergencySUPLStatus(false)");
                    GpsNetInitiatedHandler.obj.updateEmergencySUPLStatus(false);
                }
            }
        }
    }

    private void injectCellInfo() {
        int dds = SubscriptionManager.getDefaultDataSubscriptionId();
        if(SubscriptionManager.getPhoneId(dds) == 0) {
            List<CellInfo> cellInfoValue = mTelMgr.getAllCellInfo();
            logv("PhoneId 0 is DDS. CellInfoValue: "+cellInfoValue);
            if (cellInfoValue != null) {
                for (CellInfo ci : cellInfoValue) {
                    if (ci instanceof CellInfoGsm && ci.isRegistered()) {
                        logv("ci instanceof CellInfoGsm && ci.isRegistered()"
                             +"Inject will happen.");
                        CellInfoGsm gsmCell = (CellInfoGsm)ci;
                        CellIdentityGsm cellIdentityGsm = gsmCell.getCellIdentity();
                        //only inject GSM info
                        if (cellIdentityGsm != null) {
                            logv("sendMessage(MSG_CID_INJECT, 0, 0, cellIdentityGsm)");
                            sendMessage(MSG_CID_INJECT, 0, 0, cellIdentityGsm);
                        }
                        break;
                    }
                }
            } else {
                logv("cellInfoValue == null");
            }
        }
    }

    public RilInfoMonitor(Context context, int config) {
        mHandler = new Handler(IZatServiceContext.getInstance(context).getLooper(), this);
        mRemoteServiceClient = new RilInfoMonitorHidlClient();
        mIZatServiceCtx = IZatServiceContext.getInstance(context);
        mTelMgr = (TelephonyManager)mIZatServiceCtx.getContext().getSystemService(
                Context.TELEPHONY_SERVICE);
        mIZatServiceCtx.registerSystemEventListener(MSG_CELL_ID_CHANGE, this);
        mIZatServiceCtx.registerSystemEventListener(MSG_CALL_STATE_CHANGE, this);
        mIZatServiceCtx.registerSystemEventListener(MSG_SERVICE_STATE_CHANGE, this);
        sendMessage(MSG_START, config, 0, null);
    }

    private void startMonitor(int config) {
        int mask = 0;

        switch (config) {
        case LocationService.LOC_RIL_SGLTE_NO_ES_SUPL:
            // SGLTE without ES SUPL
            Log.d(TAG, "SGLTE without ES SUPL");
            mask = PhoneStateListener.LISTEN_CELL_LOCATION |
                   PhoneStateListener.LISTEN_SERVICE_STATE;
            break;
        case LocationService.LOC_RIL_SGLTE_WITH_ES_SUPL:
            // SGLTE with ES SUPL
            Log.d(TAG, "SGLTE with ES SUPL");
            mask = PhoneStateListener.LISTEN_CELL_LOCATION |
                   PhoneStateListener.LISTEN_SERVICE_STATE |
                   PhoneStateListener.LISTEN_CALL_STATE;
            break;
        case LocationService.LOC_RIL_NON_SGLTE_WITH_ES_SUPL:
            // nonSGLTE device with ES SUPL
            Log.d(TAG, "nonSGLTE device with ES SUPL");
            mask = PhoneStateListener.LISTEN_CALL_STATE;
            break;
        }

        if (mask != 0) {
            if ((mask & (PhoneStateListener.LISTEN_CELL_LOCATION |
                         PhoneStateListener.LISTEN_SERVICE_STATE)) != 0) {
                mIZatServiceCtx.registerSystemEventListener(MSG_RIL_INFO, this);
            }
        }
    }

    private void sendMessage(int what, int arg1, int arg2, Object obj) {
        Message msg = Message.obtain(mHandler, what, arg1, arg2, obj);
        mHandler.sendMessage(msg);
    }


    @Override
    public boolean handleMessage(Message msg) {
        int message = msg.what;
        Log.d(TAG, "handleMessage what - " + message);

        try {
            switch (message) {
            case MSG_START:
                startMonitor(msg.arg1);
                mRemoteServiceClient.init();
                break;
            case MSG_CID_INJECT:
                CellIdentityGsm cellIdentityGsm = (CellIdentityGsm) msg.obj;
                int cid = cellIdentityGsm.getCid();
                int lac = cellIdentityGsm.getLac();
                int mnc = cellIdentityGsm.getMnc();
                int mcc = cellIdentityGsm.getMcc();

                if (cid == -1) {
                    logv("GSM is OOS");
                    lac = -1; // overwrite lac when GSM is OOS
                }
                logv("onCellInfoChanged: cid - " + cid +
                      "; lac - " + lac + "; mnc - " + mnc +
                      "; mcc - " + mcc);
                if (mnc >= 0) {
                    mRemoteServiceClient.cinfoInject(cid, lac, mnc, mcc, getRoaming());
                }
                break;
            case MSG_OOS_INJECT:
                mRemoteServiceClient.oosInform();
                break;
            case MSG_SMS_INJECT:
                byte[] data = (byte[]) msg.obj;
                mRemoteServiceClient.niSuplInit(data, data.length);
                break;
            case MSG_SMS_MULTI_INJECT:
                break;
            default:
                break;
            }
        } catch (ClassCastException cce) {
            Log.w(TAG, "ClassCastException on " + message);
        }
        return true;
    }

    private boolean getRoaming() {
        boolean roaming = mTelMgr.isNetworkRoaming();
        logv("isNetworkRoaming() - "+roaming);
        return roaming;
    }

    static private void logv(String s) {
        if (VERBOSE_DBG) Log.v(TAG, s);
    }

    /* =================================================
     *   HIDL Client
     * =================================================*/
    public class RilInfoMonitorHidlClient extends BaseHidlClient
            implements BaseHidlClient.IServiceDeathCb {
        private static final int GPS_SV_TYPE = 1;
        private static final int GALILEO_SV_TYPE = 6;
        private ILocHidlRilInfoMonitor mRilInfoMonitor;

        private ILocHidlRilInfoMonitor getRilInfoMonitorIface() {
            if (null == mRilInfoMonitor) {
                ILocHidlGnss service = (ILocHidlGnss)getGnssService();
                if (null == service) {
                    Log.e(TAG, "ILocHidlGnss handle is null");
                    return null;
                }
                Log.d(TAG, "ILocHidlGnss.getService(), service = " + service);
                try {
                    mRilInfoMonitor = service.getExtensionLocHidlRilInfoMonitor();
                    if (null == mRilInfoMonitor) {
                        Log.e(TAG, "ILocHidlRilInfoMonitor handle is null");
                        return null;
                    }
                    Log.d(TAG, "getRilInfoMonitorIface, mRilInfoMonitor=" +
                            mRilInfoMonitor);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception getting gnss config service extension: " + e);
                }
            }

            return mRilInfoMonitor;
        }
        private RilInfoMonitorHidlClient() {
            getRilInfoMonitorIface();
            if (null != mRilInfoMonitor) {
                try {
                    mRilInfoMonitor.init();
                    registerServiceDiedCb(this);
                    Log.d(TAG, "RilInfoMonitorHidlClient hidl init");
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception init RilInfoMonitor: " + e);
                }
            }
        }


        private int validGnssConstellationTypeCount(ArrayList<Byte> disabledSvTypeList) {
            int validTypes = 0;
            for (Byte type: disabledSvTypeList) {
                if ((type >= GPS_SV_TYPE && type <= GALILEO_SV_TYPE) ||
                        (~type >= GPS_SV_TYPE && ~type <= GALILEO_SV_TYPE)) {
                    ++validTypes;
                }
            }
            return validTypes;
        }

        @Override
        public void onServiceDied() {
            Log.e(TAG, "ILocHidlRilInfoMonitor died");
            mRilInfoMonitor = null;
            init();
        }

        // Methods from ::vendor::qti::gnss::V1_0::ILocHidlRilInfoMonitor follow.
        public void init() {
            HidlClientUtils.toHidlService(TAG);
            if (null != mRilInfoMonitor) {
                try {
                    ILocHidlRilInfoMonitor rilInfoMonitorIface = getRilInfoMonitorIface();
                    rilInfoMonitorIface.init();
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception ILocHidlRilInfoMonitor::init(): " + e);
                }
            }
        }

        public void cinfoInject(int cid, int lac, int mnc, int mcc, boolean roaming) {
            HidlClientUtils.toHidlService(TAG);
            if (null != mRilInfoMonitor) {
                try {
                    ILocHidlRilInfoMonitor rilInfoMonitorIface = getRilInfoMonitorIface();
                    rilInfoMonitorIface.cinfoInject(cid, lac, mnc, mnc, roaming);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception ILocHidlRilInfoMonitor::cinfoInject() " + e);
                }
            }
        }

        public void oosInform() {
            HidlClientUtils.toHidlService(TAG);
            if (null != mRilInfoMonitor) {
                try {
                    ILocHidlRilInfoMonitor rilInfoMonitorIface = getRilInfoMonitorIface();
                    rilInfoMonitorIface.oosInform();
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception ILocHidlRilInfoMonitor::oosInform(): " + e);
                }
            }
        }

        public void niSuplInit(byte[] data, int length) {
            HidlClientUtils.toHidlService(TAG);
            StringBuilder strBuilder = new StringBuilder();
            for (int i=0; i<length; ++i) {
                strBuilder.append(data[i]);
            }
            if (null != mRilInfoMonitor) {
                try {
                    ILocHidlRilInfoMonitor rilInfoMonitorIface = getRilInfoMonitorIface();
                    rilInfoMonitorIface.niSuplInit(strBuilder.toString());
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception ILocHidlRilInfoMonitor::niSuplInit(): " + e);
                }
            }
        }

        public void chargerStatusInject(int status) {
            HidlClientUtils.toHidlService(TAG);
            if (null != mRilInfoMonitor) {
                try {
                    ILocHidlRilInfoMonitor rilInfoMonitorIface = getRilInfoMonitorIface();
                    rilInfoMonitorIface.chargerStatusInject(status);
                } catch (RemoteException e) {
                    Log.e(TAG, "Exception ILocHidlRilInfoMonitor::chargerStatusInject(): " + e);
                }
            }
        }
    }
}
