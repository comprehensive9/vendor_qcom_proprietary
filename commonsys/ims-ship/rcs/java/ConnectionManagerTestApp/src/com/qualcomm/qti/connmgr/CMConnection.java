/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.connmgr;

import android.os.RemoteException;
import android.util.Log;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.Toast;
import android.widget.Spinner;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import com.qualcomm.qti.imscmservice.V2_0.IImsCMConnectionListener;
import com.qualcomm.qti.imscmservice.V2_0.IImsCMConnection;
import com.qualcomm.qti.imscmservice.V2_0.connectionEvent;
import com.qualcomm.qti.imscmservice.V2_0.connectionEventData;
import com.qualcomm.qti.imscmservice.V2_0.messageType;
import com.qualcomm.qti.imscmservice.V2_0.incomingMessage;
import com.qualcomm.qti.imscmservice.V2_0.incomingMessageKeys;
import com.qualcomm.qti.imscmservice.V2_0.keyValuePairStringType;
import com.qualcomm.qti.imscmservice.V2_0.keyValuePairBufferType;
import com.qualcomm.qti.imscmservice.V2_0.outgoingMessage;
import com.qualcomm.qti.imscmservice.V2_0.outgoingMessageKeys;
import com.qualcomm.qti.imscmservice.V2_0.userConfigKeys;
import com.qualcomm.qti.imscmservice.V2_2.deviceConfigKeys;

public class CMConnection {
    final static String LOG_TAG = "CMTestApp:Connection";
    public static final int VERSION_2_0 = 0;
    public static final int VERSION_2_1 = 1;
    public static final int VERSION_2_2 = 2;

    com.qualcomm.qti.imscmservice.V2_0.IImsCMConnection conn_2_0 = null;
    long connectionHandle = 0;
    int mTabInstance = -1;
    boolean isFtRegistered = false;
    ConnectionListenerImpl mConnListener = new ConnectionListenerImpl();
    String mTagName = "";
    String mUriStr = "";
    List<String> mConnectionStatusList = new ArrayList<String>();
    View mCmServiceView;

    private class ConnectionListenerImpl extends
           IImsCMConnectionListener.Stub {
        final static String TAG = "CMTestApp:ConnectionListenerImpl";

        private long listenerToken = 0;
        Handler mHandler = null;

        public void setListenerToken(long listenerToken) {
          this.listenerToken = listenerToken;
        }
        public long getListenerToken() {
          return this.listenerToken;
        }
        public ConnectionListenerImpl() {
          mHandler = new Handler(Looper.getMainLooper());
        }
        /*handleEventReceived: Callback function to inform clients about a
          registration status change, changes in service allowed by policy
          manager because of a RAT change,and any forceful terminations
          of the connection object by the QTI framework because of PDP
          status changes.
        */
        @Override
        public void onEventReceived(connectionEventData event) {
          Log.d(TAG, "onEventReceived event is "+event.toString());
          if(event.eEvent == connectionEvent.SERVICE_REGISTERED) {
            Log.d(TAG, "onEventReceived Connection for FT: " +
                       mTagName +
                       "\t is Registered: on Sub" +
                       mTabInstance);

            isFtRegistered = true;

            if(!MainActivity.globaldata[mTabInstance].
            registeredConnectionSpinnerList.contains(mTagName)){
              MainActivity.globaldata[mTabInstance].
              registeredConnectionSpinnerList.add(mTagName);
            }
          }
          else {
            isFtRegistered = false;
            if(event.eEvent == connectionEvent.SERVICE_CREATED) {
              if(MainActivity.globaldata[mTabInstance].
              connectionSpinnerList.contains(mTagName)){
              MainActivity.globaldata[mTabInstance].
              connectionSpinnerList.remove(mTagName);
              }
            }else if(event.eEvent == connectionEvent.SERVICE_NOTREGISTERED) {
              //TO CHECK
              //MainActivity.globaldata[mTabInstance].
              //registeredConnectionSpinnerList.remove(mTagName);
            }
          }
          Runnable msg = new Runnable() {
            @Override
            public void run() {
              if(event.eEvent == connectionEvent.SERVICE_REGISTERED) {
                Toast.makeText(MainActivity.mCtx,
                        "onEventReceived: SERVICE_REGISTERED For FT[" +
                        mTagName + "]",
                        Toast.LENGTH_SHORT).show();

                String status = "Registered FT: ["+
                                mTagName +
                                "]";
                MainActivity.globaldata[mTabInstance].
                connMgrStatusList.add(status);
                MainActivity.mSectionsPagerAdapter.mSubsTabs[mTabInstance].
                statusAdapter.notifyDataSetChanged();
                MainActivity.mSectionsPagerAdapter.mSubsTabs[mTabInstance].
                    updateRegisteredFtSpinnerElement(mCmServiceView);
              }else if(event.eEvent == connectionEvent.SERVICE_CREATED) {
                Toast.makeText(MainActivity.mCtx,
                        "onEventReceived: SERVICE_CREATED For FT[" +
                        mTagName + "]",
                        Toast.LENGTH_SHORT).show();
                MainActivity.mSectionsPagerAdapter.mSubsTabs[mTabInstance].
                                 updateFTSpinnerElement(mCmServiceView);
              }else if(event.eEvent == connectionEvent.SERVICE_NOTREGISTERED) {
                Toast.makeText(MainActivity.mCtx,
                        "onEventReceived: SERVICE_NOTREGISTERED For FT["+
                        mTagName + "]",
                        Toast.LENGTH_SHORT).show();
                String status = "Deregistered FT: ["+
                                 mTagName +
                                "]";
                MainActivity.globaldata[mTabInstance].
                  connMgrStatusList.add(status);
                MainActivity.mSectionsPagerAdapter.mSubsTabs[mTabInstance].
                statusAdapter.notifyDataSetChanged();
              }else {
                //All other events: SERVICE_NOTALLOWED,
                //SERVICE_FORCEFUL_CLOSE,
                //SERVICE_TERMINATE_CONNECTION
                MainActivity.mSectionsPagerAdapter.
                mSubsTabs[mTabInstance].clearConnection(mTagName);
              }
            }
          };
          mHandler.post(msg);
        }

        /*handleIncomingMessage: This
        callback indicates the incoming message to the client.*/
        @Override
        public void handleIncomingMessage(incomingMessage data) {
          String sipMessage = "";
          Log.d(TAG, "handleIncomingMessage: on Sub" + mTabInstance);
          if(data.data.size() > 0) {
            for(keyValuePairStringType var : data.data) {
              Log.d(TAG, "handleIncomingMessage: key[" +
               incomingMessageKeys.toString(var.key) +
                          "] Value[" + var.value +"]");
            }
          }
          else {
            Log.d(TAG, "handleIncomingMessage: no String Type data received");
          }
          //TextViewobj.setText(ImcomignMsg);

          if(data.bufferData.size() > 0) {
            for(keyValuePairBufferType var : data.bufferData) {
                char[] byteData = new char[var.value.size()];
                for(int i=0; i< var.value.size(); i++)
                {
                    byteData[i] = (char)var.value.get(i).byteValue();
                }
                sipMessage = new String(byteData);
                Log.d(TAG, "handleIncomingMessage: key[" +
                 incomingMessageKeys.toString(var.key) +
                "] Value[" + sipMessage +"]");
            }
          }
          else {
            Log.d(TAG, "handleIncomingMessage: no Buffer Type data received");
          }

          final String sipPacket = sipMessage;
          Runnable msg = new Runnable() {
            @Override
            public void run() {
              Toast.makeText(MainActivity.mCtx,
                      "handleIncomingMessage: For instance[" +
                      mTabInstance + "]",
                      Toast.LENGTH_SHORT).show();
              outgoingMessage msgResponse = parseIncomingMessage(sipPacket);
              if(msgResponse != null){
                  sendMessage(
                    msgResponse,
                    MainActivity.globaldata[mTabInstance].
                     userDataArray[mTabInstance]);
              }
            }
          };
          mHandler.post(msg);

        }

        /*onCommandStatus: Status of the sendMessage
          (whether or not the message was transmitted to network)
          is returned asynchronously via
          the onCommandStatus callback with messageID as a parameter.
        */
        @Override
        public void onCommandStatus(int status, int userdata) {
          for(int i=0; i<=1; i++) {
            if(MainActivity.globaldata[mTabInstance].userDataArray[i] == userdata) {
              Log.d(TAG, "onCommandStatus: on Sub" + i);
              Log.d(TAG, "onCommandStatus status is ["+
                status + "] userdata " + userdata);
            }
          }
          Runnable msg = new Runnable() {
            @Override
            public void run() {
              Toast.makeText(MainActivity.mCtx,
                  "onCommandStatus: status " + status +
                  "For instance[" + mTabInstance +
                  "] userdata: " + userdata,
                  Toast.LENGTH_SHORT).show();
            }
          };
          mHandler.post(msg);
        }

        private outgoingMessage parseIncomingMessage(String msgString) {
          outgoingMessage msg = new outgoingMessage();
          String response = "";
          String[] headers = msgString.split("\r\n");
          String fromHeader ="";
          String cseqHeader ="";
          String callidHeader ="";
          String senderBranch ="";
          String protocol ="";
          String contactHeader ="";
          String portHeader ="";
          String status = "Incoming Msg["+ headers[0] + "]";
          mConnectionStatusList.add(status);
          FeatureOperationActivity.ftAdapter.notifyDataSetChanged();
          Log.d(LOG_TAG,"parseIncomingMessage: Num of headers is ["+
                headers.length+"]");

          if(headers[0].contains("MESSAGE ")){
            for(int i=0; i<headers.length; i++){
              if(headers[i].contains("From: ")) {
                int left = 6;
                fromHeader = headers[i].substring(left, headers[i].length());
                Log.d(LOG_TAG,"From: "+fromHeader);
              } else if(headers[i].contains("CSeq: ")) {
                int left = 6;
                cseqHeader = headers[i].substring(left, headers[i].length());
                Log.d(LOG_TAG,"cseq: "+cseqHeader);
              } else if(headers[i].contains("Call-ID: ")) {
                int left = 9;
                callidHeader = headers[i].substring(left, headers[i].length());
                Log.d(LOG_TAG,"callid: "+callidHeader);
                int leftIndex = headers[i].indexOf("@");
                leftIndex +=1;
                contactHeader = headers[i].substring(
                   leftIndex, headers[i].length());
                Log.d(LOG_TAG,"contactHeader: "+contactHeader);
              } else if(headers[i].contains("Via: ")) {
                int left = headers[i].indexOf("branch=");
                left += 7;
                senderBranch = headers[i].substring(left, headers[i].length());
                Log.d(LOG_TAG,"branch: "+senderBranch);
                int leftindex = 13;
                int right = headers[i].indexOf("[");
                protocol = headers[i].substring(leftindex, right);
                Log.d(LOG_TAG,"protocol: "+protocol);
                int portleftIndex = headers[i].indexOf("]:");
                portleftIndex +=2;
                int portrightIndex = headers[i].indexOf(";branch");
                portHeader = headers[i].substring(
                  portleftIndex, portrightIndex);
                Log.d(LOG_TAG,"portHeader: "+portHeader);
              }
            }
            String lcprotocol = protocol.toLowerCase();
            lcprotocol = lcprotocol.replaceAll("\\s", "");
            String localIpAddress = MainActivity.globaldata[mTabInstance].
              userConfigData.get(userConfigKeys.LocalHostIPAddress);
            String outboundProxy = MainActivity.globaldata[mTabInstance].
              deviceConfigData.get(deviceConfigKeys.StrSipOutBoundProxyName);
            String outboundProxyPort = MainActivity.globaldata[mTabInstance].
              deviceConfigData.get(deviceConfigKeys.SipOutBoundProxyPort);
            String publicUserId = MainActivity.globaldata[mTabInstance].
              userConfigData.get(userConfigKeys.SipPublicUserId);
            response = "SIP/2.0 200 OK\r\n" +
              "Via: SIP/2.0/"+ protocol + "[" + localIpAddress + "]:" +
              outboundProxyPort + ";branch=" + senderBranch + "\r\n" +
              "Contact: <sip:[" + contactHeader + "]:" + portHeader +
              ";transport=" +lcprotocol+">\r\n" +
              "To: " + fromHeader + "\r\n"+
              "From: <" + publicUserId + ">;tag=3476455352\r\n" +
              "Call-ID: " + callidHeader +"\r\n"+
              "CSeq: " + cseqHeader + "\r\n" +
              "Content-Length: 0\r\n\r\n";

            keyValuePairStringType callIdType = new keyValuePairStringType();
            callIdType.key = outgoingMessageKeys.CallId;
            callIdType.value = callidHeader;
            msg.data.add(callIdType);

            keyValuePairStringType messagetypeVal = new
              keyValuePairStringType();
            messagetypeVal.key = outgoingMessageKeys.MessageType;
            messagetypeVal.value = Integer.toString(messageType.TYPE_RESPONSE);
            msg.data.add(messagetypeVal);

            keyValuePairStringType protocolType = new
              keyValuePairStringType();
            protocolType.key = outgoingMessageKeys.Protocol;
            protocolType.value = protocol.equals("UDP")? "0" : "1";
            msg.data.add(protocolType);

            keyValuePairStringType outboundProxyVal = new
              keyValuePairStringType();
            outboundProxyVal.key = outgoingMessageKeys.OutboundProxy;
            outboundProxyVal.value = "["+ outboundProxy+ "]";
            msg.data.add(outboundProxyVal);

            keyValuePairStringType remotePortVal = new
              keyValuePairStringType();
            remotePortVal.key = outgoingMessageKeys.RemotePort;
            remotePortVal.value = Integer.toString(5060);
            //remotePortVal.value = portHeader;
            msg.data.add(remotePortVal);

            keyValuePairBufferType messageData = new
              keyValuePairBufferType();
            messageData.key = outgoingMessageKeys.Message;

            Log.d(TAG,"parseIncomingMessage:response" +
                  "Message string:["+ response +"]");
            byte[] messageByteArray = response.getBytes();
            for(int i = 0; i < messageByteArray.length; i++) {
              messageData.value.add(messageByteArray[i]);
            }
            msg.bufferData.add(messageData);
          }
          else{
            Log.d(LOG_TAG,"parseIncomingMessage: non-incoming msg");
            return null;
          }
          return msg;
        }
    };

    CMConnection(int instance,
                 String TagName,
                 String uriStr,
                 View cmServiceView) {
        mTabInstance = instance;
        mTagName = TagName;
        mUriStr = uriStr;
        mCmServiceView = cmServiceView;
        Log.d(LOG_TAG,"CMConnection ctor FT: "+mTagName);
    }

    public ConnectionListenerImpl getConnListnerImpl() {
        return mConnListener;
    }
    public boolean isFtRegistered() {
        return isFtRegistered;
    }
    public String getTagName() {
        return mTagName;
    }

    public void setIConnectionData(
       com.qualcomm.qti.imscmservice.V2_0.IImsCMConnection connection,
       long connectionHandle,
       long listenerToken) {
        conn_2_0 = connection;
        this.connectionHandle = connectionHandle;
        mConnListener.setListenerToken(listenerToken);
    }

    public void setConnectionStatusList(String statusText){
      mConnectionStatusList.add(statusText);
    }

    public List<String> getConnectionStatusList() {
      return mConnectionStatusList;
    }

    public boolean closeConnection(long connectionManager) {
      boolean status = false;
      boolean removeListenerstatus = removeListener(
             mConnListener.getListenerToken());
       if(removeListenerstatus){
        status = MainActivity.globaldata[mTabInstance].connMgr.closeConnection(
            connectionManager,
          connectionHandle);
       }
       return status;
    }

    public boolean removeListener(long listenerToken) {
      try{
        conn_2_0.removeListener(listenerToken);
      }catch(RemoteException e) {
        return false;
      }
      return true;
    }

    public boolean sendMessage(outgoingMessage data, int userdata) {
       try{
          conn_2_0.sendMessage(
                        data,
                        userdata);
        }catch(RemoteException e) {
          return false;
        }
        return true;
    }

        public boolean closeTransaction(String callID, int userdata) {
            try{
          conn_2_0.closeTransaction(
                        callID,
                        userdata);
        }catch(RemoteException e) {
                return false;
            }
            return true;
        }

        public boolean closeAllTransactions(int userdata) {
            try{
        conn_2_0.closeAllTransactions(
                  userdata);
      }catch(RemoteException e) {
                return false;
            }
            return true;
        }

    public boolean setStatus(int status) {
            try{
        conn_2_0.setStatus(
          mUriStr,
                        status);
            }
            catch(RemoteException e) {
                return false;
      }
            return true;
    }
}
