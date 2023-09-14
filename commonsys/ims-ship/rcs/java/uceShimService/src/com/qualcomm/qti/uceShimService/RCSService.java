/*********************************************************************
 Copyright (c) 2017-2018,2020-2021 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.uceShimService;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;
import java.util.ArrayList;
import java.util.Date;
import java.util.NoSuchElementException;

import com.qualcomm.qti.uceservice.V2_0.*;

import com.android.ims.internal.uce.uceservice.IUceService;
import com.android.ims.internal.uce.UceServiceBase;
import com.android.ims.internal.uce.uceservice.IUceListener;
import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.android.ims.internal.uce.common.UceLong;
import android.app.Activity;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IHwBinder;
import android.os.Message;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.Bundle;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.os.SystemClock;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.content.Intent;
import android.content.IntentFilter;
import android.telephony.TelephonyManager;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.IccCardConstants;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.content.BroadcastReceiver;
import android.telephony.TelephonyManager;
import android.telephony.PhoneStateListener;

import android.os.RemoteException;
import static android.telephony.PhoneStateListener.LISTEN_ACTIVE_DATA_SUBSCRIPTION_ID_CHANGE;

public class RCSService extends Service {

    /** Service Initialize Parameters  */
    public final static String INTENT_BOOT_COMPLETE_STR = "android.intent.action.BOOT_COMPLETED";
    public final static String INTENT_SIM_APPLICATION_STATE_CHANGED_STR = "android.telephony.action.SIM_APPLICATION_STATE_CHANGED";
    public final static String INTENT_SIM_STATE_CHANGED_STR = "android.telephony.action.SIM_CARD_STATE_CHANGED";
    public final static String INTENT_PROCESS_ID = "PROCESS_ID";
    public final static String INTENT_ONE_TIME = "ONE_TIME";
    public static final int DEFAULT_DPL_MAX_RETRY = 3;
    private static final long DEFAULT_DPL_INIT_TIMEOUT = 10000;
    private static final long DEFAULT_MODEM_STATUS_CHECK_RETRY =  12000;
    public static final long DEFAULT_SERVICE_RECOVERY_TIMER =  8000;
    private final static String UCE_SERVICE_VERSION2 = "com.qualcomm.qti.uceservice@2.0::IUceService";
    private final static String UCE_SERVICE_VERSION2_1 = "com.qualcomm.qti.uceservice@2.1::IUceService";
    private final static String UCE_SERVICE_VERSION2_2 = "com.qualcomm.qti.uceservice@2.2::IUceService";
    private final static String UCE_SERVICE_VERSION2_3 = "com.qualcomm.qti.uceservice@2.3::IUceService";
    private final static String UCE_HIDL_SERVICE_NAME = "com.qualcomm.qti.uceservice";
    public static final int RCS_CONFIG_DISABLED = 0;
    public static final int RCS_CONFIG_ENABLED = 1;
    public static final int RCS_MODEM_IMS_DISABLED = 2;
    public static final int RCS_PERSIST_DISABLED = 3;

    //Service Registered boolean variable
    private boolean isHidlServiceUp = false;
    private boolean isSimInserted = true;
    //Death REcipient boolean variable
    private boolean isHIDLDead = true;
    //death Binder Cookie
    private final long mDeathBinderCookie = 1011010;

    /** check support Type */
    private static int qRcsSupportType = 0;

    /** Initialization Parameters */
    public static RCSServiceStub.HalServiceWrapper mHidlService = null;
    public static RCSServiceStub objQRCSServiceStub = null;
    public static Context context; // context usage
    public static Context ctx;
    private static boolean isRCSServiceStartCalled = false;
    public Activity serviceConfigActivity = new Activity();
    private static boolean isLoadLibrary = false;

    /** Logging Tag */
    private final static String logTAG = "QRCSService";

    /** Service Manager Identity to register with */
    private static final String UCE_SERVICE_NAME = "uce";
    private RCSListener mHidlUceListener = null;

    private final Object mLock = new Object();

   private HandlerThread mEventThread;
   private Handler mHandler;
   private int currDDSId = -1;
   private TelephonyManager mTelephonyManager;
   private static final int EVENT_UPDATE_ACTIVE_DATA_SUBID = 1;

   private int MAX_SLOTS_SUPPORTED = 2;
   private boolean[] isSimRcsEnabled = new boolean[MAX_SLOTS_SUPPORTED];
   private Map<String, Boolean> serviceUpIntentPendingMap = new HashMap<String, Boolean>();

    private IHwBinder.DeathRecipient mDeathRecipient = new IHwBinder.DeathRecipient() {
        @Override
        public void serviceDied(long cookie) {
            Log.d(logTAG,"HIDL SErvice Died");
            if (cookie == mDeathBinderCookie) {
                try {
                    mHidlService.unlinkToDeath(mDeathRecipient);
                } catch (RemoteException e) {
                    Log.i(logTAG, "RemoteException : unable to unlink DeathRecipient");
                } catch (NoSuchElementException e) {
                    Log.i(logTAG, "NoSuchElementException : DeathRecipient has not been registered");
                }
                performCleanUp();
                isSimRcsEnabled[0] = false;
                isSimRcsEnabled[1] = false;
                //Broadcast Service Intent : UceService DOWN
                postServiceDown();
                isHidlServiceUp = false;
                isHIDLDead = true;
                startService();
        }
        }
    };

    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
         @Override
         public void onActiveDataSubscriptionIdChanged(int subId) {
             Log.d(logTAG,"onActiveSubscriptionsChanged currDDSId: "+ currDDSId + " new DDS " + subId);

             if(subId != currDDSId) {
                 mHandler.sendMessage(mHandler.obtainMessage
                                              (EVENT_UPDATE_ACTIVE_DATA_SUBID, subId, -1));
                 currDDSId = subId;
             }
         }
    };

    synchronized private void startService() {
        Log.i(logTAG, "start service isHidlServiceUp["+isHidlServiceUp+"], isSimInserted["+
                 isSimInserted+"], mHidlService is ["+(mHidlService != null)+"]");

        if (mHidlService == null) {
            Log.i(logTAG, "Attempting to get HIDL-IUceService");
            //blocking getService call
            mHidlService = objQRCSServiceStub.getService();

            if(mHidlService != null && objQRCSServiceStub != null) {
                Log.i(logTAG, "Received HIDL-UCEService :" + mHidlService.getVersion());
                isHidlServiceUp = true;
                objQRCSServiceStub.setHidlUceService(mHidlService);
                try {
                    mHidlService.addUceListener(mHidlUceListener);
                } catch (RemoteException | RuntimeException e) {
                    Log.i(logTAG, "Unable to add HIDL Listener Exception :" + e);
                }
                try{
                    //register to Service Manager
                    ServiceManager.addService(UCE_SERVICE_NAME, objQRCSServiceStub.getBinder());
                    objQRCSServiceStub.setServiceStatus(true);

                    //Broadcast Service Intent
                    //Firing intent to Open source Client
                    Log.i(logTAG, "startService: attempt BroadCast ServieUpIntent");

                    //Register to Death Recipient
                    boolean isLinkSuccesful= mHidlService.linkToDeath(mDeathRecipient,
                                                                    mDeathBinderCookie);
                    isHIDLDead = !(isLinkSuccesful);
                } catch (RemoteException e){
                    Log.i(logTAG, "Caught Exception :" + e);
                }
            }
        }
    }

    synchronized private void performCleanUp() {
        if(isHidlServiceUp) {
            objQRCSServiceStub.setServiceStatus(false);
            mHidlService = null;
            objQRCSServiceStub.setHidlUceService(mHidlService);
            objQRCSServiceStub.onDestroyAllServices();
        }
    }

    synchronized private void postServiceDown() {
        if(isHidlServiceUp) {
            Intent intent = new Intent();
            Bundle bundle = new Bundle();
            for(int i = 0; i < MAX_SLOTS_SUPPORTED; i++)
            {
               if(isSimRcsEnabled[i])
                 bundle.putBoolean("SLOT_"+i, true);
               else
                 bundle.putBoolean("SLOT_"+i, false);
            }

            Log.d(logTAG,"postServiceDown: Sending ACTION_UCE_SERVICE_DOWN");
            intent.setAction(ImsUceManager.ACTION_UCE_SERVICE_DOWN);
            intent.putExtras(bundle);

            context.sendBroadcast(intent);
        }
    }

    public static void setServiceStatus(boolean bStatus) {
        isRCSServiceStartCalled = bStatus;
    }

    public static boolean getServiceStatus() {
        return isRCSServiceStartCalled;
    }


    BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            if (intent == null) {
               Log.i(logTAG, "received null intent");
               return;
            }

            final String action = intent.getAction();
            if (action == null) {
               Log.i(logTAG, "received null intent action");
               return;
            }

            if (action.equals(INTENT_SIM_STATE_CHANGED_STR)||action.equals(INTENT_SIM_APPLICATION_STATE_CHANGED_STR)) {
                int slotIdx = intent.getIntExtra(PhoneConstants.PHONE_KEY, SubscriptionManager.INVALID_SIM_SLOT_INDEX);
                Log.d(logTAG, "SIM State Changed Event Received for slot Idx ["+slotIdx+"]");
                int state = intent.getIntExtra(TelephonyManager.EXTRA_SIM_STATE,TelephonyManager.SIM_STATE_UNKNOWN);

                /* If SIM is inserted and ready then it should be either READY
                   LOADED any other state means that SIM is removed or
                   not READY yet  */
                isSimInserted = (TelephonyManager.SIM_STATE_READY == state
                                         || TelephonyManager.SIM_STATE_LOADED == state);

                Log.i(logTAG, " Sim State received: "+state);

                /* Start the service if SIM is inserted and ICCID is ready. */
                if (isSimInserted) {
                    startService();
                    String[] iccIdArr = serviceUpIntentPendingMap.
                                           keySet().toArray(new String[0]);
                    for(String tempIccId : iccIdArr) {
                        if( serviceUpIntentPendingMap.getOrDefault(tempIccId, false)) {
                           handleHidlStatus(true, tempIccId);
                       }
                    }
                }
                else {
                 if(isSimRcsEnabled[slotIdx]) {
                   isSimRcsEnabled[slotIdx] = false;
                   postServiceDown();
                 }
                }
            }
        }
    };

//refactor
    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(logTAG, "onCreate: Attempt to start service");
        ctx = getApplicationContext();
        context = this;
        mHidlUceListener = new RCSListener(this);
        /* Register for SIM state intent */
        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyManager.ACTION_SIM_CARD_STATE_CHANGED);
        filter.addAction(TelephonyManager.ACTION_SIM_APPLICATION_STATE_CHANGED );
        context.registerReceiver(mIntentReceiver, filter, null, mHandler);

        //broadcastedDownEventIntent = null;
        isSimRcsEnabled[0] = false;
        isSimRcsEnabled[1] = false;

        mEventThread = new HandlerThread("MainEventThread");
        mEventThread.start();
        mHandler = new Handler(mEventThread.getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch(msg.what) {
                    case EVENT_UPDATE_ACTIVE_DATA_SUBID:
                        int ddsId = msg.arg1;
                        if ((objQRCSServiceStub.updateDefaultIccId(ddsId))) {
                            currDDSId = ddsId;
                            Log.d(logTAG, "Updated Default ICCID");
                        }
                        break;
                    default:
                       Log.e(logTAG, "Unexpected message " + msg.what);
                }
            }
        };

        if(objQRCSServiceStub == null) {
            objQRCSServiceStub = new RCSServiceStub();
        } else {
            Log.i(logTAG, "onCreate: objQRCSServiceStub already exists: check whether OnCreate Called Multiple Times");
        }
        objQRCSServiceStub.SetContext(context);
        mTelephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        if(mTelephonyManager != null) {
            mTelephonyManager.listen(mPhoneStateListener, LISTEN_ACTIVE_DATA_SUBSCRIPTION_ID_CHANGE);
            isSimInserted = (TelephonyManager.SIM_STATE_READY == mTelephonyManager.getSimState());
            Log.i(logTAG, "onCreate: isSimInserted["+isSimInserted+"]");
        } else {
             Log.e(logTAG, "onCreate: mTelephonyManager is null, so not listening to active_data_sub event");
        }

        Process proc = null;
    }

    @Override
    public void onStart(Intent intent, int startId) {
        Log.i(logTAG, "onStart: Attempt to start Service");
        super.onStart(intent, startId);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.i(logTAG, "onStartCommand- Call");
        Thread serviceStatusCheckThread = new Thread(){
                public void run(){
                    try {
                        if(!isHidlServiceUp) {
                            startService();
                        }
                        else {
                            Log.i(logTAG, "Already HIDL-IUceService is Started ");
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
        };
        serviceStatusCheckThread.start();
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        isSimRcsEnabled[0] = false;
        isSimRcsEnabled[1] = false;

        //Broadcast Service Intent that Uce Service is Down
        postServiceDown();
        performCleanUp();

        Log.i(logTAG, "onDestroy: Call stopRCSService");
        try {
            if (getServiceStatus() == true) {
                RCSListener.isRCSRegistered = false;
            }
        } catch (UnsatisfiedLinkError e) {
            Log.i(logTAG, "onDestroy- stopRCSService error = " + e);
        } catch (Exception e) {
            Log.i(logTAG, "onDestroy- stopRCSService exception = " + e);
        }
        if(mHidlService != null) {
            try {
                mHidlService.unlinkToDeath(mDeathRecipient);
            } catch (RemoteException e) {
                Log.i(logTAG, "OnDestroy: RemoteException : unable to unlink DeathRecipient");
            } catch (NoSuchElementException e) {
                Log.i(logTAG, "NoSuchElementException : DeathRecipient has not been registered");
            }

            mHidlService = null;
        }

    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(logTAG, "onBind: Received: return NULL as this feature is not supported");
        return null;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.i(logTAG, "onUnbind Received: Nothing to do");
        return false;
    }

    public IUceService.Stub getRCSServiceImpl() {
        try {
            return objQRCSServiceStub.getBinder();
        } catch (Exception e) {
            return null;
        }
    }

    public boolean getRcsServiceStatus() {
        Log.i(logTAG, "getRcsServiceStatus  isRCSRegistered ["+ RCSListener.isRCSRegistered +"]");
        return RCSListener.isRCSRegistered;
    }

    void handleHidlStatus(boolean isUp, String iccId) {
        Log.i(logTAG, "handleHidlStatus  hidlStatus ["+ isUp +"]");
        SubscriptionInfo subInfo = null;
        SubscriptionManager subMgr =  SubscriptionManager.from(context);
        int slotId = SubscriptionManager.INVALID_SIM_SLOT_INDEX;

        if(subMgr != null)
          subInfo = subMgr.getActiveSubscriptionInfoForIcc(iccId);

        if(subInfo != null)
          slotId = subInfo.getSimSlotIndex();

        if(isUp) {
            if(slotId != SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
              startService();
              isSimRcsEnabled[slotId] = true;
              sendServiceUpIndication();
               removeIccIdFromPendingIntent(iccId);
            } else {
              Log.i(logTAG, " handleHidlStatus received true but invalid slotId");
              serviceUpIntentPendingMap.put(iccId, true);
            }
        } else {
            removeIccIdFromPendingIntent(iccId);

            if(slotId != SubscriptionManager.INVALID_SIM_SLOT_INDEX &&
                                                isSimRcsEnabled[slotId]) {
                Log.i(logTAG, "Sending ACTION_UCE_SERVICE_DOWN for slot: "+slotId);
                isSimRcsEnabled[slotId] = false;
                postServiceDown();
            } else {
                Log.i(logTAG, " handleHidlStatus received false but invalid slotId");
            }

            if (objQRCSServiceStub != null) {
              objQRCSServiceStub.destroyAllServicesForIccId(iccId);
            }
        }
    }

    void sendServiceUpIndication() {
        Intent uceServiceIntent = new Intent();
        Bundle bundle = new Bundle();
        for(int i = 0; i < MAX_SLOTS_SUPPORTED; i++)
        {
           if(isSimRcsEnabled[i])
             bundle.putBoolean("SLOT_"+i, true);
           else
             bundle.putBoolean("SLOT_"+i, false);
        }

        Log.d(logTAG,"Sending ACTION_UCE_SERVICE_UP");
        uceServiceIntent.setAction(ImsUceManager.ACTION_UCE_SERVICE_UP);
        uceServiceIntent.putExtras(bundle);

        context.sendStickyBroadcast(uceServiceIntent);
    }

    private void removeIccIdFromPendingIntent(String tempIccId) {
        if(serviceUpIntentPendingMap.getOrDefault(tempIccId,false)) {
            serviceUpIntentPendingMap.remove(tempIccId);
        }
    }

}
