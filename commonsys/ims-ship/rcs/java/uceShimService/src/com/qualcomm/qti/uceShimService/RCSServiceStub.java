/*********************************************************************
 Copyright (c) 2017-2018,2019,2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.uceShimService;


import android.os.Parcel;
import android.os.RemoteException;
import android.util.Log;
import android.os.IBinder;
import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.os.ServiceManager;
import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionInfo;
import com.qualcomm.qti.uceservice.V2_0.UceStatus; // or .*
import com.qualcomm.qti.uceservice.V2_0.UceStatusCode;

import com.android.ims.internal.uce.uceservice.IUceService;
import com.android.ims.internal.uce.UceServiceBase;
import com.android.ims.internal.uce.uceservice.IUceListener;
import com.android.ims.internal.uce.presence.IPresenceService;
import com.android.ims.internal.uce.presence.IPresenceListener;
import com.android.ims.internal.uce.options.IOptionsService;
import com.android.ims.internal.uce.options.IOptionsListener;
import com.android.ims.internal.uce.common.*;

import com.qualcomm.qti.presence.PresListener;
import com.qualcomm.qti.options.OptionsListener;

import com.qualcomm.qti.presence.PresService;
import com.qualcomm.qti.options.OptionsService;
import java.util.*;
import android.os.IBinder;
import android.os.IHwBinder;
public class RCSServiceStub extends UceServiceBase { //extend AIDL- IUCEService

    private HalServiceWrapper mhidlUceService = null;
    private final String ACCESS_UCE_PRES_SERVICE = Manifest.permission.ACCESS_UCE_PRESENCE_SERVICE;
    private final String ACCESS_UCE_OPTION_SERVICE = Manifest.permission.ACCESS_UCE_OPTIONS_SERVICE;

    private final static String logTAG = "Aidl-shim UceServiceStub";

    private static Map<String, PresService> mPresenceSvcMap = new HashMap<String, PresService>();
    private static Map<String, OptionsService> mOptionsServicMap = new HashMap<String, OptionsService>();

    private String mIccId = "";
    private int mDefaultSlotIdx = -1;
    private Context servContext = null;

    public HalServiceWrapper getService() {
        HalServiceWrapper temp = null;
        Log.d(logTAG, "getService start");
        temp = new Hal_2_3();
        if(temp.bindToHal()){
            return temp;
        }
        Log.d(logTAG, "getService 2_3 bind failed");
        temp = new Hal_2_2();
        if(temp.bindToHal()){
            return temp;
        }
        temp = new Hal_2_1();
        if(temp.bindToHal()){
            return temp;
        }
        temp = new Hal_2_0();
        if(temp.bindToHal()){
            return temp;
        }
        return null;
    }

    public abstract class HalServiceWrapper {
        public final static int VERSION_2_0 = 0;
        public final static int VERSION_2_1 = 1;
        public final static int VERSION_2_2 = 2;
        public final static int VERSION_2_3 = 3;

        private int modemPresenceVersion = VERSION_2_3;

        private int modemOptionsVersion = VERSION_2_3;

        public String getVersion() { return "VERSION_2_3"; }

        public int getModemPresenceVersion() { return modemPresenceVersion; }

        public int getModemOptionsVersion() { return modemOptionsVersion; }

        public void setModemPresenceVersion(int presenceVer) { modemPresenceVersion = presenceVer; }

        public void setModemOptionsVersion(int optionsVer) { modemOptionsVersion = optionsVer; }

        public boolean bindToHal() { return false;}

        public void addUceListener(RCSListener listener)throws RuntimeException,RemoteException  {}

        public UceStatus createOptionsService(OptionsListener optionsListener,
                                      long optionsServiceListenerHdl,
                                      String iccId) throws RuntimeException,RemoteException { return null;}
        public void destroyOptionsService(int optionsServiceHandle)throws RuntimeException,RemoteException {}

        public com.qualcomm.qti.uceservice.V2_3.IOptionsService getOptionsService_2_3(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_2.IOptionsService getOptionsService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_1.IOptionsService getOptionsService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_0.IOptionsService getOptionsService(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public UceStatus createPresenceService(PresListener presServiceListener,
                            long presServiceListenerHdl,
                            String iccId)throws RuntimeException,RemoteException{ return null;}
        public void destroyPresenceService(int pPresServiceHdl)throws RuntimeException,RemoteException {}
        public com.qualcomm.qti.uceservice.V2_3.IPresenceService getPresenceService_2_3(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_2.IPresenceService getPresenceService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_1.IPresenceService getPresenceService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_0.IPresenceService getPresenceService(int handle)
            throws RuntimeException,RemoteException,RemoteException{
            return null;
        }

        public void unlinkToDeath(IHwBinder.DeathRecipient d) throws RemoteException, NoSuchElementException {}
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) throws RemoteException, NoSuchElementException {
            return false;
        }
    };


    private class Hal_2_3 extends HalServiceWrapper {
        com.qualcomm.qti.uceservice.V2_3.IUceService mSvc = null;
        public String getVersion() {
            return "VERSION_2_3";
        }

        public boolean bindToHal() {
            try {
                Log.d(logTAG, "Hal_2_3 bindToHal");
                mSvc = com.qualcomm.qti.uceservice.V2_3.IUceService.getService(
                "com.qualcomm.qti.uceservice",true);
            } catch (NoSuchElementException | RemoteException e) {
                //Log.e();
            }
            return (mSvc != null);
        }
        public void addUceListener(RCSListener listener)throws RuntimeException,RemoteException  {
            mSvc.addUceListener(listener);
        }

        public UceStatus createOptionsService(OptionsListener listener,
                                      long handle,
                                      String iccId) throws RuntimeException ,RemoteException{
            UceStatus hidlUceStatus = mSvc.createOptionsService_2_3(listener, handle, iccId);
            mhidlUceService.setModemOptionsVersion(VERSION_2_3);

            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createOptionsService_2_2(listener, handle, iccId);
                mhidlUceService.setModemOptionsVersion(VERSION_2_2);
            }
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createOptionsService_2_1(listener, handle, iccId);
                mhidlUceService.setModemOptionsVersion(VERSION_2_1);
            }
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createOptionsService(listener, handle, iccId);
                mhidlUceService .setModemOptionsVersion(VERSION_2_0);
            }
            return hidlUceStatus;
        }
        public void destroyOptionsService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyOptionsService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_3.IOptionsService getOptionsService_2_3(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService_2_3(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IOptionsService getOptionsService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService_2_2(handle);
        }
        public com.qualcomm.qti.uceservice.V2_1.IOptionsService getOptionsService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService_2_1(handle);
        }
        public com.qualcomm.qti.uceservice.V2_0.IOptionsService getOptionsService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService(handle);
        }
        public UceStatus createPresenceService(PresListener listener,
                            long handle,
                            String iccId)throws RuntimeException,RemoteException {
            UceStatus hidlUceStatus = mSvc.createPresenceService_2_3(listener, handle, iccId);
            mhidlUceService.setModemPresenceVersion(VERSION_2_3);

            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createPresenceService_2_2(listener, handle, iccId);
                mhidlUceService.setModemPresenceVersion(VERSION_2_2);
            }
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createPresenceService_2_1(listener, handle, iccId);
                mhidlUceService.setModemPresenceVersion(VERSION_2_1);
            }
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createPresenceService(listener, handle, iccId);
                mhidlUceService.setModemPresenceVersion(VERSION_2_0);
            }
            return hidlUceStatus;
        }
        public void destroyPresenceService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyPresenceService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_3.IPresenceService getPresenceService_2_3(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService_2_3(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IPresenceService getPresenceService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService_2_2(handle);
        }
        public com.qualcomm.qti.uceservice.V2_1.IPresenceService getPresenceService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService_2_1(handle);
        }
        public com.qualcomm.qti.uceservice.V2_0.IPresenceService getPresenceService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService(handle);
        }

        public void unlinkToDeath(IHwBinder.DeathRecipient d) throws RemoteException, NoSuchElementException {
            mSvc.unlinkToDeath(d);
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) throws RemoteException, NoSuchElementException {
            return mSvc.linkToDeath(d, cookie);
        }
    }

    private class Hal_2_2 extends HalServiceWrapper {
        com.qualcomm.qti.uceservice.V2_2.IUceService mSvc = null;
        public String getVersion() {
            return "VERSION_2_2";
        }

        public boolean bindToHal() {
            try {
                Log.d(logTAG, "Hal_2_2 bindToHal");
                mSvc = com.qualcomm.qti.uceservice.V2_2.IUceService.getService(
                "com.qualcomm.qti.uceservice",true);
            } catch (NoSuchElementException | RemoteException e) {
                //Log.e();
            }
            return (mSvc != null);
        }
        public void addUceListener(RCSListener listener)throws RuntimeException,RemoteException  {
            mSvc.addUceListener(listener);
        }

        public UceStatus createOptionsService(OptionsListener listener,
                                      long handle,
                                      String iccId) throws RuntimeException ,RemoteException{
            UceStatus hidlUceStatus = mSvc.createOptionsService_2_2(listener, handle, iccId);
            mhidlUceService.setModemOptionsVersion(VERSION_2_2);
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createOptionsService_2_1(listener, handle, iccId);
                mhidlUceService.setModemOptionsVersion(VERSION_2_1);
            }
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createOptionsService(listener, handle, iccId);
                mhidlUceService.setModemOptionsVersion(VERSION_2_0);
            }
            return hidlUceStatus;
        }
        public void destroyOptionsService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyOptionsService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IOptionsService getOptionsService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService_2_2(handle);
        }
        public com.qualcomm.qti.uceservice.V2_1.IOptionsService getOptionsService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService_2_1(handle);
        }
        public com.qualcomm.qti.uceservice.V2_0.IOptionsService getOptionsService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService(handle);
        }
        public UceStatus createPresenceService(PresListener listener,
                            long handle,
                            String iccId)throws RuntimeException,RemoteException {
            UceStatus hidlUceStatus = mSvc.createPresenceService_2_2(listener, handle, iccId);
            mhidlUceService.setModemPresenceVersion(VERSION_2_2);
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createPresenceService_2_1(listener, handle, iccId);
                mhidlUceService.setModemPresenceVersion(VERSION_2_1);
            }
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createPresenceService(listener, handle, iccId);
                mhidlUceService.setModemPresenceVersion(VERSION_2_0);
            }
            return hidlUceStatus;
        }
        public void destroyPresenceService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyPresenceService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IPresenceService getPresenceService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService_2_2(handle);
        }
        public com.qualcomm.qti.uceservice.V2_1.IPresenceService getPresenceService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService_2_1(handle);
        }
        public com.qualcomm.qti.uceservice.V2_0.IPresenceService getPresenceService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService(handle);
        }

        public void unlinkToDeath(IHwBinder.DeathRecipient d) throws RemoteException, NoSuchElementException {
            mSvc.unlinkToDeath(d);
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) throws RemoteException, NoSuchElementException {
            return mSvc.linkToDeath(d, cookie);
        }
    }

    private class Hal_2_1 extends HalServiceWrapper {
        com.qualcomm.qti.uceservice.V2_1.IUceService mSvc = null;
        public String getVersion() {
            return "VERSION_2_1";
        }

        public boolean bindToHal() {
            try {
                Log.d(logTAG, "Hal_2_1 bindToHal");
                mSvc = com.qualcomm.qti.uceservice.V2_1.IUceService.getService(
                "com.qualcomm.qti.uceservice",true);
            } catch (NoSuchElementException|RemoteException e) {
                //Log.e();
            }
            return (mSvc != null);
        }
        public void addUceListener(RCSListener listener)throws RuntimeException,RemoteException  {
            mSvc.addUceListener(listener);
        }

        public UceStatus createOptionsService(OptionsListener listener,
                                      long handle,
                                      String iccId) throws RuntimeException,RemoteException {
            UceStatus hidlUceStatus = mSvc.createOptionsService_2_1(listener, handle, iccId);
            mhidlUceService.setModemOptionsVersion(VERSION_2_1);
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createOptionsService(listener, handle, iccId);
                mhidlUceService.setModemOptionsVersion(VERSION_2_0);
            }
            return hidlUceStatus;
        }
        public void destroyOptionsService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyOptionsService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IOptionsService getOptionsService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_1.IOptionsService getOptionsService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService_2_1(handle);
        }
        public com.qualcomm.qti.uceservice.V2_0.IOptionsService getOptionsService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService(handle);
        }
        public UceStatus createPresenceService(PresListener listener,
                            long handle,
                            String iccId)throws RuntimeException,RemoteException {
            UceStatus hidlUceStatus = mSvc.createPresenceService_2_1(listener, handle, iccId);
            mhidlUceService.setModemPresenceVersion(VERSION_2_1);
            if (hidlUceStatus.status == UceStatusCode.NOT_SUPPORTED) {
                hidlUceStatus = mSvc.createPresenceService(listener, handle, iccId);
                mhidlUceService.setModemPresenceVersion(VERSION_2_0);
            }
            return hidlUceStatus;
        }
        public void destroyPresenceService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyPresenceService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IPresenceService getPresenceService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_1.IPresenceService getPresenceService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService_2_1(handle);
        }
        public com.qualcomm.qti.uceservice.V2_0.IPresenceService getPresenceService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService(handle);
        }

        public void unlinkToDeath(IHwBinder.DeathRecipient d) throws RemoteException, NoSuchElementException {
            mSvc.unlinkToDeath(d);
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) throws RemoteException, NoSuchElementException {
            return mSvc.linkToDeath(d, cookie);
        }
    }

    private class Hal_2_0 extends HalServiceWrapper {
        com.qualcomm.qti.uceservice.V2_0.IUceService mSvc = null;
        public String getVersion() {
            return "VERSION_2_0";
        }

        public boolean bindToHal() {
            try {
                Log.d(logTAG, "Hal_2_0 bindToHal");
                mSvc = com.qualcomm.qti.uceservice.V2_0.IUceService.getService(
                "com.qualcomm.qti.uceservice",true);
            } catch (NoSuchElementException|RemoteException e) {
                //Log.e();
            }
            return (mSvc != null);
        }
        public void addUceListener(RCSListener listener)throws RuntimeException,RemoteException  {
            mSvc.addUceListener(listener);
        }

        public UceStatus createOptionsService(OptionsListener listener,
                                      long handle,
                                      String iccId) throws RuntimeException,RemoteException {
            mhidlUceService.setModemOptionsVersion(VERSION_2_0);
            return mSvc.createOptionsService(listener, handle, iccId);
        }
        public void destroyOptionsService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyOptionsService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IOptionsService getOptionsService_2_2(int handle)
            throws RuntimeException,RemoteException {
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_1.IOptionsService getOptionsService_2_1(int handle)
            throws RuntimeException,RemoteException {
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_0.IOptionsService getOptionsService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getOptionsService(handle);
        }

        public UceStatus createPresenceService(PresListener listener,
                            long handle,
                            String iccId)throws RuntimeException,RemoteException {
            mhidlUceService.setModemPresenceVersion(VERSION_2_0);
            return mSvc.createPresenceService(listener, handle, iccId);
        }
        public void destroyPresenceService(int handle)throws RuntimeException,RemoteException {
            mSvc.destroyPresenceService(handle);
        }
        public com.qualcomm.qti.uceservice.V2_2.IPresenceService getPresenceService_2_2(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_1.IPresenceService getPresenceService_2_1(int handle)
            throws RuntimeException,RemoteException{
            return null;
        }
        public com.qualcomm.qti.uceservice.V2_0.IPresenceService getPresenceService(int handle)
            throws RuntimeException,RemoteException{
            return mSvc.getPresenceService(handle);
        }

        public void unlinkToDeath(IHwBinder.DeathRecipient d) throws RemoteException, NoSuchElementException {
            mSvc.unlinkToDeath(d);
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) throws RemoteException, NoSuchElementException {
            return mSvc.linkToDeath(d, cookie);
        }
    }

    public boolean setHidlUceService(HalServiceWrapper hidlService) {
        Log.d(logTAG, "RCSStartRCSService setHidlUceService");
        mhidlUceService = hidlService;
        return (updateDefaultIccId(SubscriptionManager.INVALID_SUBSCRIPTION_ID));
    }

    public boolean startService(IUceListener uceListener)
            throws RemoteException {
        Log.i(logTAG, "RCSStartRCSService Start Service call");
        return true;
    }

    public boolean stopService() throws RemoteException {
        return true;
    }

    public boolean isServiceStarted() throws RemoteException {
        return true;
    }

    public int getDefaultSlotIdx() {
        synchronized (mIccId) {
            return mDefaultSlotIdx;
        }
    }

    public boolean updateDefaultIccId(int defaultSubId) {
        boolean retval = false;
        //Retrieve defaultSubscriptionInfo
        SubscriptionManager subMgr =  SubscriptionManager.from(servContext);
        if (defaultSubId == SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
            defaultSubId = subMgr.getDefaultDataSubscriptionId();
        }
        SubscriptionInfo defaultSubInfo = subMgr.getActiveSubscriptionInfo(defaultSubId);
        if (defaultSubInfo != null) {
            synchronized (mIccId) {
                mIccId = defaultSubInfo.getIccId();
                mDefaultSlotIdx = subMgr.getSlotIndex(defaultSubId);
                Log.i(logTAG, "getDefaultIccId default mDefaultSlotId [" +mDefaultSlotIdx+"] defaultSubId [" +defaultSubId+"]");
            }
            retval = true;
        }

        return retval;
    }

    protected int onCreateOptionsService(IOptionsListener optionsListener,
                                    UceLong optionsServiceListenerHdl) {
        int serviceHandle = -1;
        String defaultIccId = null;

        if(mIccId == null)
            return serviceHandle;

        /* Its possible that after retrieving IccId DDS change happens.
           For such corner cases legacy API would use the defaultIccId
           which was stored in uceShimService before DDS change.
           For accuracy the newer API which includes IccId should be
           used */
        synchronized (mIccId) {
           defaultIccId = mIccId;
           Log.i(logTAG, "onCreateOptionsService legacy API hit");
        }

        if (defaultIccId  == null) {
            Log.i(logTAG, "onCreateOptionsService FAILED ICCID not found");
            return serviceHandle;
        }
        try {
            serviceHandle = onCreateOptionsService(optionsListener,
                                                   optionsServiceListenerHdl,
                                                   defaultIccId);
        } /*  catch (RemoteException e) {
            Log.d(logTAG, "createPresenceService failed with Exception:" + e);
        }*/  catch (RuntimeException e) {
            Log.d(logTAG, "createPresenceService failed with Exception:" + e);
        }

        return serviceHandle;
    }

    protected int onCreateOptionsService(IOptionsListener optionsListener,
                                      UceLong optionsServiceListenerHdl,
                                      String iccId)  {
       servContext.enforceCallingOrSelfPermission(ACCESS_UCE_OPTION_SERVICE, "open");
       int serviceHandle = -1;
       UceStatus hidlUceStatus = null;
       long hidlOptionsSrvcListenerHdl = 1;

       /* Create the thread and set service handle to -1 */
       UceThreadHandler optionsHandler = new UceThreadHandler();
       optionsHandler.setHandle(serviceHandle);

       boolean bSuspendThread = true;
       OptionsListener hidlOptionLister = new OptionsListener();

       optionsHandler.setThread(Thread.currentThread());
       hidlOptionLister.setAidlOptionsServiceListener(optionsListener);
       hidlOptionLister.setOptionsHandler(optionsHandler);

       hidlOptionsSrvcListenerHdl = optionsServiceListenerHdl.getUceLong();
       try {
            hidlUceStatus = mhidlUceService.createOptionsService(hidlOptionLister,
                                                                    hidlOptionsSrvcListenerHdl,
                                                                    iccId);
       }catch (RemoteException | RuntimeException e) {
            Log.d(logTAG, "createPresenceService failed with Exception:" + e);
            bSuspendThread = false;
       }
       //if createoptionsservice returns any failure, service handle is returned as -1
       if(hidlUceStatus != null && hidlUceStatus.status != UceStatusCode.SUCCESS) {
            Log.d(logTAG, "createOptionsService returning service handle -1, hidlUceStatus: " + hidlUceStatus.status);
            return serviceHandle;
       }
       OptionsService optionsSvc = mOptionsServicMap.get(iccId);
       if ( optionsSvc == null) {
           optionsSvc = new OptionsService();
           mOptionsServicMap.put(iccId, optionsSvc);
       }
       optionsSvc.addHidlListener(hidlOptionLister);

       /* Get the service handle to check whether its already avaiable.
           This is done to avoid calling thread suspend when service handle
           is already available */
       serviceHandle = optionsHandler.getHandle();

       if (bSuspendThread && (serviceHandle == -1)) {
           optionsHandler.suspend();
       }
       if (hidlUceStatus != null && hidlUceStatus.status == 0) {
            serviceHandle = optionsHandler.getHandle();
            optionsSvc.setServiceHandle(serviceHandle);
        }
        OptionsListenerDeathRecipient optionsListenerDeathRecipient =
                  new OptionsListenerDeathRecipient(serviceHandle, optionsListener);
        try{
            optionsListener.asBinder().linkToDeath(optionsListenerDeathRecipient,1);
        }
        catch(RemoteException e){
            Log.i(logTAG, "Caught Exception :" + e);
        }
       return serviceHandle;
    }

    protected void onDestroyOptionsService(int optionsServiceHandle) {
        servContext.enforceCallingOrSelfPermission(ACCESS_UCE_OPTION_SERVICE, "close");
        try {
            if(mhidlUceService != null) {
                mhidlUceService.destroyOptionsService(optionsServiceHandle);
            } else {
              Log.d(logTAG, "onDestroyOptionsService: mhidlUceService is null");
            }
        } catch (RemoteException | RuntimeException e) {
            Log.d(logTAG, "onDestroyOptionsService failed with Exception:" + e);
        }
    }

    protected int onCreatePresService(
            IPresenceListener pPresServiceListener,
            UceLong pPresServiceListenerHdl) {
        int serviceHandle = -1;
        String defaultIccId = null;

        if(mIccId == null)
            return serviceHandle;

        /* Its possible that after retrieving IccId DDS change happens.
           For such corner cases legacy API would use the defaultIccId
           which was stored in uceShimService before DDS change.
           For accuracy the newer API which includes IccId should be
           used */
        synchronized (mIccId) {
            defaultIccId = mIccId;
            Log.i(logTAG, "onCreatePresService legacy API hit");
        }

        if (defaultIccId == null) {
            Log.i(logTAG, "onCreatePresService FAILED ICCID not found");
            return serviceHandle;
        }
        try {
            serviceHandle = onCreatePresService(pPresServiceListener,
                                                pPresServiceListenerHdl,
                                                defaultIccId);
        } catch (RuntimeException  e) {
            Log.d(logTAG, "createPresenceService failed with Exception:" + e);
        }

        return serviceHandle;

    }
    protected int onCreatePresService(IPresenceListener presServiceListener,
                                                    UceLong presServiceListenerHdl,
                                                    String iccId)  {
        servContext.enforceCallingOrSelfPermission(ACCESS_UCE_PRES_SERVICE, "open");
        int serviceHandle = -1;
        UceStatus hidlUceStatus = null;
        long hidlPresServiceListenerHdl = -1;
        UceThreadHandler presenceHandler = new UceThreadHandler();
        boolean bSuspendThread = true;
        PresListener hidlPresListener = new PresListener();

        /* Create the thread and set service handle to -1 */
        presenceHandler.setThread(Thread.currentThread());
        presenceHandler.setHandle(serviceHandle);

        hidlPresListener.setAidlPresListener(presServiceListener);
        hidlPresListener.setCreatePresenceHandler(presenceHandler);

        hidlPresServiceListenerHdl = presServiceListenerHdl.getUceLong();
        try {
            Log.d(logTAG, "createPresenceService HAL");
            hidlUceStatus = mhidlUceService.createPresenceService(hidlPresListener,
                                                                hidlPresServiceListenerHdl,
                                                                iccId);
        }
        catch (RemoteException | RuntimeException e) {
            Log.d(logTAG, "createPresenceService failed with Exception:" + e);
            bSuspendThread = false;
        }
         //if createpresencesservice returns any failure, service handle is returned as -1
        if(hidlUceStatus != null && hidlUceStatus.status != UceStatusCode.SUCCESS) {
            Log.d(logTAG, "createPresenceService returning service handle -1, hidlUceStatus: " + hidlUceStatus.status);
            return serviceHandle;
        }
        PresService presSvc = mPresenceSvcMap.get(iccId);
        if ( presSvc == null) {
            Log.i(logTAG, "onCreatePresService creating new PresService");
            presSvc = new PresService();
            mPresenceSvcMap.put(iccId, presSvc);
        }
        presSvc.addHidlListener(hidlPresListener);

        /* Get the service handle to check whether its already avaiable.
           This is done to avoid calling thread suspend when service handle
           is already available */
        serviceHandle = presenceHandler.getHandle();

        if (bSuspendThread && (serviceHandle == -1)) {
            presenceHandler.suspend();
        }

        if (hidlUceStatus != null && hidlUceStatus.status == 0) {
            serviceHandle = presenceHandler.getHandle();
            presSvc.setServiceHandle(serviceHandle);
        }
        PresenceListenerDeathRecipient presenceListenerDeathRecipient =
                new PresenceListenerDeathRecipient(serviceHandle, presServiceListener);
        try{
            presServiceListener.asBinder().linkToDeath(presenceListenerDeathRecipient, 1);
        }
        catch (RemoteException e){
            Log.i(logTAG, "Caught Exception :" + e);
        }
        return serviceHandle;
    }

    protected void onDestroyPresService(int pPresServiceHdl) {
        servContext.enforceCallingOrSelfPermission(ACCESS_UCE_PRES_SERVICE, "close");
        try {
            if(mhidlUceService != null) {
                mhidlUceService.destroyPresenceService(pPresServiceHdl);
            } else {
              Log.d(logTAG, "onDestroyPresService: mhidlUceService is null");
            }
        }
        catch (RemoteException | RuntimeException e) {
            Log.d(logTAG, "destroyPresenceService failed with Exception:" + e);
        }
    }

    protected IPresenceService onGetPresenceService()  {
        if(mIccId == null)
            return null;

        /* Its possible that after retrieving IccId DDS change happens.
           For such corner cases legacy API would use the defaultIccId
           which was stored in uceShimService before DDS change.
           For accuracy the newer API which includes IccId should be
           used */
        synchronized (mIccId) {
            Log.i(logTAG, "onGetPresenceService default");
            return onGetPresenceService(mIccId);
        }
    }

    protected IPresenceService onGetPresenceService(String iccId) {
        PresService presService = mPresenceSvcMap.get(iccId);
        if(presService == null) {
            Log.i(logTAG, "onGetPresenceService presService is NULL");
        }
        return presService;
    }

    protected IOptionsService onGetOptionsService() {
        if(mIccId == null)
            return null;

        /* Its possible that after retrieving IccId DDS change happens.
           For such corner cases legacy API would use the defaultIccId
           which was stored in uceShimService before DDS change.
           For accuracy the newer API which includes IccId should be
           used */
        synchronized (mIccId) {
            Log.i(logTAG, "onGetOptionsService default mIccId");
            return onGetOptionsService(mIccId);
        }
    }

    protected IOptionsService onGetOptionsService (String iccId) {
        return mOptionsServicMap.get(iccId);
    }

    protected boolean onGetServiceStatus()
    {
        Log.i(logTAG, "ServiceStatus isRCSRegistered ["
                + RCSListener.isRCSRegistered + "]");
        return RCSListener.isRCSRegistered;
    }

    public void setServiceStatus( boolean status) {
        RCSListener.isRCSRegistered = status;
        Log.i(logTAG, "ServiceStatus isRCSRegistered ["
                + RCSListener.isRCSRegistered + "]");
    }

    public void SetContext(Context servContext) {
        this.servContext = servContext;

    }

    public void onDestroyAllServices() {
        for (Map.Entry el : mPresenceSvcMap.entrySet()) {
            PresService ps = (PresService) el.getValue();
            ps.destroyServiceHandles();
        }
        for (Map.Entry el : mOptionsServicMap.entrySet()) {
            OptionsService os = (OptionsService) el.getValue();
            os.destroyServiceHandles();
        }
        mPresenceSvcMap.clear();
        mOptionsServicMap.clear();
    }

    public void destroyAllServicesForIccId(String iccId) {
        PresService presSvc = mPresenceSvcMap.get(iccId);
        if ( presSvc != null) {
          presSvc.destroyServiceHandles();
        }

        OptionsService optionsSvc = mOptionsServicMap.get(iccId);
        if ( optionsSvc != null) {
          optionsSvc.destroyServiceHandles();
        }

        mPresenceSvcMap.remove(iccId);
        mOptionsServicMap.remove(iccId);
    }

    private class OptionsListenerDeathRecipient implements IBinder.DeathRecipient{
        private IOptionsListener listener = null;
        private int serviceHandle = 0;
        //constructor
        public OptionsListenerDeathRecipient(int serviceHandle, IOptionsListener listener) {
          this.serviceHandle = serviceHandle;
          this.listener = listener;
        }
        @Override
        public void binderDied(){
            Log.i(logTAG, "Options listener died");
            try{
                if (serviceHandle != 0 && mhidlUceService != null){
                        mhidlUceService.destroyOptionsService(serviceHandle);
                }
                if (listener != null){
                    listener.asBinder().unlinkToDeath(this, 1);
                }
            }
            catch(RemoteException e){
                Log.i(logTAG, "Remote exception : " +e);
            }
        }
    };

    private class PresenceListenerDeathRecipient implements IBinder.DeathRecipient{
        private IPresenceListener listener = null;
        private int serviceHandle = 0;
        //constructor
        public PresenceListenerDeathRecipient(int serviceHandle, IPresenceListener listener) {
          this.serviceHandle = serviceHandle;
          this.listener = listener;
        }
        @Override
        public void binderDied(){
            Log.i(logTAG, "Presence listener died");
            try{
                if (serviceHandle != 0 && mhidlUceService != null){
                        mhidlUceService.destroyPresenceService(serviceHandle);
                }
                if (listener != null) {
                    listener.asBinder().unlinkToDeath(this,1);
                }
            }
            catch(RemoteException e){
                Log.i(logTAG, "Remote exception : " +e);
            }
        }
    };

}
