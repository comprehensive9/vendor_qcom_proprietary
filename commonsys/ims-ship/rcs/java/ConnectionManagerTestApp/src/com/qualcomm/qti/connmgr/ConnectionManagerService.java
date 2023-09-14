/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.connmgr;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.os.IHwBinder;

import com.qualcomm.qti.imscmservice.V2_0.IImsCMConnectionListener;
import com.qualcomm.qti.imscmservice.V2_2.IImsCmService;
import com.qualcomm.qti.imscmservice.V2_0.IImsCmServiceListener;
import com.qualcomm.qti.imscmservice.V2_0.methodResponseData;

public class ConnectionManagerService {

    final static String LOG_TAG = "CMTestApp:ConnectionManagerService";
    public static final int VERSION_2_0 = 0;
    public static final int VERSION_2_1 = 1;
    public static final int VERSION_2_2 = 2;

    public CMHalVersionWrapper getConnectionManagerService() {
        CMHalVersionWrapper halWrapper = null;
        halWrapper = new Version2_2();
        if(halWrapper.bind() == true)
        {
            Log.d(LOG_TAG,"Version 2_2");
            return halWrapper;
        }
        halWrapper = new Version2_1();
        if(halWrapper.bind() == true){
            Log.d(LOG_TAG,"Version 2_1");
            return halWrapper;
        }

        halWrapper = new Version2_0();
        if(halWrapper.bind() == true){
            Log.d(LOG_TAG,"Version 2_0");
            return halWrapper;
        }

        return null;
    }


    public abstract class CMHalVersionWrapper {

        //private int halVersion = VERSION_2_0;

        public int getHalVersion() {
            return VERSION_2_0;
        }


        public boolean bind(){return false;}

        public boolean InitializeService(String iccId,
          ConnectionManagerListenerImpl cmListener, int userData){
            return false;
        }

        public boolean removeListener(long connectionManager, long listenerId){
            return false;
        }

        public CMConnection createConnection(long connectionManager,
          int instance, String tagName, String uriStr, View v) {
            return null;
        }

        public boolean closeConnection(long connectionManager,
          long connectionHandle) {
            return false;
        }

        /*triggerRegistration---- Triggers registration.
        This must be done after all the connections are created,
        enabling registration triggering with all the required
        feature tags simultaneously*/
        public boolean triggerRegistration(long connectionManager,
          int userdata) {
            return false;
        }

        /*triggerDeRegistration---- Triggers a deregistration.
        This API removes all FTs, performs a PDN release,
        and brings up the PDN*/
        public boolean triggerDeRegistration(long connectionManager,
          int userdata){
            return false;
        }

        public boolean getConfiguration(long connectionManager,
          int configType, int userdata) {
            return false;
        }

        /* Closes the Connection Manager.
        Closing the manager forces pending connection objects to be
        immediately deleted regardless of what state they are in*/
        public boolean closeService(long connectionManager){
            return false;
        }

        public boolean triggerACSRequest(long connectionManager,
          int autoConfigReasonType,
          int userdata){
            return false;
        }

        public boolean methodResponse(long connectionManager,
          methodResponseData data,
          int userdata){
            return false;
        }
        public boolean unlinkToDeath(IHwBinder.DeathRecipient d){
            return false;
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) {
            return false;
        }
    };

    private class Version2_0 extends CMHalVersionWrapper {

        com.qualcomm.qti.imscmservice.V2_0.IImsCmService connMgrV2_0 = null;
        CMConnection conn = null;

        protected void setBinder(
            com.qualcomm.qti.imscmservice.V2_0.IImsCmService svc) {
            Log.d(LOG_TAG, "Inside Version_2_0::setBinder");
            connMgrV2_0 = svc;
        }

        @Override
        public int getHalVersion() {
            return VERSION_2_0;
        }

        @Override
        public boolean bind() {
            try {
                connMgrV2_0 = com.qualcomm.qti.imscmservice.V2_0.IImsCmService.
                getService("qti.ims.connectionmanagerservice");
            }
            catch (RemoteException exc) {
                Log.d(LOG_TAG, "Exception raised" + exc);
            }
            return (connMgrV2_0 != null);
        }

        @Override
        public boolean InitializeService(
            String iccId,
            ConnectionManagerListenerImpl cmListener,
            int userData)
        {
            Log.d(LOG_TAG, "Inside Version_2_0::InitializeService");
            try{
                connMgrV2_0.InitializeService(
                        iccId,
                        cmListener,
                        userData, //0 or 1
                        (int status, long listenerId) -> {
                            //initStatus2 = status;
                            cmListener.setListenerId(listenerId);
                        });
            }
            catch(RemoteException exc) {
                return false;
            }
            return true;
        }

        @Override
        public boolean removeListener(
            long connectionManager,
            long listenerId)
        {
            try{
                connMgrV2_0.removeListener(
                        connectionManager,
                        listenerId);
            }
            catch(RemoteException e) {
                return false;
            }
            return true;
        }

        @Override
        public CMConnection createConnection(
            long connectionManager,
            int instance,
            String tagName,
            String uriStr,
            View cmServiceView)
        {
            Log.d(LOG_TAG, "Inside Version_2_0::createConnection");
            CMConnection conn = new CMConnection(
                instance,
                tagName,
                uriStr,
                cmServiceView);
            try{
                connMgrV2_0.createConnection(
                        connectionManager,
                        conn.getConnListnerImpl(),
                        uriStr,
                        (com.qualcomm.qti.imscmservice.V2_0.IImsCMConnection
                        connection,
                        long connectionHandle,
                        long listenerToken) -> {
                            conn.setIConnectionData(
                                connection,
                                connectionHandle,
                                listenerToken);
                        });
            }
            catch(RemoteException e) {
                return null;
            }
            return conn;
        }

        @Override
        public boolean closeConnection(
            long connectionManager,
            long connectionHandle)
        {
            try {
                connMgrV2_0.closeConnection(
                    connectionManager,
                    connectionHandle);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }

        @Override
        public boolean triggerRegistration(
            long connectionManager,
            int userdata)
        {
            try {
                connMgrV2_0.triggerRegistration(connectionManager, userdata);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }

        @Override
        public boolean triggerDeRegistration(
            long connectionManager,
            int userdata)
        {
            try {
                connMgrV2_0.triggerDeRegistration(connectionManager, userdata);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }

        @Override
        public boolean getConfiguration(
            long connectionManager,
            int configType,
            int userdata)
        {
            try {
                connMgrV2_0.getConfiguration(connectionManager,
                                             configType,
                                             userdata);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }

        @Override
        public boolean closeService(long connectionManager) {
            try {
                connMgrV2_0.closeService(connectionManager);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }

        @Override
        public boolean methodResponse(
            long connectionManager,
            methodResponseData data,
            int userdata)
        {
            try {
                connMgrV2_0.methodResponse(
                    connectionManager,
                    data,
                    userdata);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }

        public boolean unlinkToDeath(IHwBinder.DeathRecipient d){
            try {
                connMgrV2_0.unlinkToDeath(d);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) {
            try {
                connMgrV2_0.linkToDeath(d,cookie);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }

    };

    private class Version2_1 extends Version2_0 {

        com.qualcomm.qti.imscmservice.V2_1.IImsCmService connMgrV2_1 = null;

        @Override
        public int getHalVersion() {
            return VERSION_2_1;
        }

        @Override
        public boolean bind() {
            try {
                connMgrV2_1 = com.qualcomm.qti.imscmservice.V2_1.IImsCmService.
                getService("qti.ims.connectionmanagerservice");
            }
            catch (RemoteException exc) {
                Log.d(LOG_TAG, "Exception raised" + exc);
            }
            super.setBinder(connMgrV2_1);
            return (connMgrV2_1 != null);
        }
        protected void setBinder(com.qualcomm.qti.imscmservice.V2_1.
          IImsCmService svc) {
            Log.d(LOG_TAG, "Inside Version_2_1::setBinder");
            connMgrV2_1 = svc;
            super.setBinder(connMgrV2_1);
        }

        @Override
        public boolean InitializeService(
            String iccId,
            ConnectionManagerListenerImpl cmListener,
            int userData)
        {
            try{
                connMgrV2_1.InitializeService_2_1(
                        iccId,
                        cmListener,
                        userData, //0 or 1
                        (int status, long listenerId) -> {
                            //setCmServiceStatus(status);
                            cmListener.setListenerId(listenerId);
                        });
            }
            catch(RemoteException exc) {
                return false;
            }
            return true;
        }

        @Override
        public boolean triggerACSRequest(
            long connectionManager,
            int autoConfigReasonType,
            int userdata)
        {
            try{
                connMgrV2_1.triggerACSRequest(
                        connectionManager,
                        autoConfigReasonType,
                        userdata);
            }
            catch(RemoteException exc) {
                return false;
            }
            return true;
        }
        public boolean unlinkToDeath(IHwBinder.DeathRecipient d){
            try {
                connMgrV2_1.unlinkToDeath(d);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) {
            try {
                connMgrV2_1.linkToDeath(d,cookie);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }
    };

    public class Version2_2 extends Version2_1 {

        com.qualcomm.qti.imscmservice.V2_2.IImsCmService connMgrV2_2 = null;
        CMConnection conn = null;

        @Override
        public int getHalVersion() {
            return VERSION_2_2;
        }

        @Override
        public boolean bind() {
            try {
                connMgrV2_2 = com.qualcomm.qti.imscmservice.V2_2.IImsCmService.
                getService("qti.ims.connectionmanagerservice");
            }
            catch (RemoteException exc) {
                Log.d(LOG_TAG, "Exception raised" + exc);
            }
            super.setBinder(connMgrV2_2);
            return (connMgrV2_2 != null);
        }

        @Override
        public boolean InitializeService(
            String iccId,
            ConnectionManagerListenerImpl cmListener,
            int userData)
        {
            try{
                connMgrV2_2.InitializeService_2_2(
                        iccId,
                        cmListener,
                        userData, //0 or 1
                        (int status, long listenerId) -> {
                            //setCmServiceStatus(status);
                            cmListener.setListenerId(listenerId);
                        });
            }
            catch(RemoteException exc) {
                return false;
            }
            return true;
        }

        public boolean unlinkToDeath(IHwBinder.DeathRecipient d){
            try {
                connMgrV2_2.unlinkToDeath(d);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }
        public boolean linkToDeath(IHwBinder.DeathRecipient d, long cookie) {
            try {
                connMgrV2_2.linkToDeath(d,cookie);
            } catch (RemoteException e) {
                Log.d(LOG_TAG, "Exception raised" + e);
                return false;
            }
            return true;
        }
    };
}
