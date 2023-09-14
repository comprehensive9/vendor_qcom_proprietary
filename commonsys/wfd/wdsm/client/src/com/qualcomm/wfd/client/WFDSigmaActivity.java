/*==============================================================================
*  WFDSigmaActivity.java
*
*  DESCRIPTION:
*
*  The main UI activity on launch of Sigma app, responsible for creating and
*  interfacing with other helper classes and sub-activities. Carries out the
*  task of p2p discovery, connect and initiating start of wfd session.
*
*  Copyright (c) 2012 - 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*  Not a contribution
*
*  Copyright (C) 2011 The Android Open Source Project
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*===============================================================================
*/

package com.qualcomm.wfd.client;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.wifi.p2p.WifiP2pInfo;
import android.content.ServiceConnection;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.graphics.Color;
import android.net.Uri;
import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import java.lang.InterruptedException;
import java.lang.ref.WeakReference;
import java.util.Collection;
import java.util.Map;
import java.util.Stack;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.FrameLayout.LayoutParams;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.SimpleAdapter;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdStatus;
import com.qualcomm.wfd.WfdEnums.WFDDeviceType;
import com.qualcomm.wfd.client.R;
import com.qualcomm.wfd.client.ServiceUtil.ServiceFailedToBindException;
import com.qualcomm.wfd.client.WfdClientUtils.WfdOperation;
import com.qualcomm.wfd.client.net.CombinedNetManager;
import com.qualcomm.wfd.client.net.NetDeviceIF;
import com.qualcomm.wfd.client.net.NetManagerIF;
import com.qualcomm.wfd.client.net.ResultListener;
import com.qualcomm.wfd.client.net.ConnectionInfoIF;
import com.qualcomm.wfd.service.IWfdSession;
import com.qualcomm.wfd.service.IWfdActionListener;
import static com.qualcomm.wfd.client.WfdClientUtils.*;
import com.qualcomm.wfd.WfdEnums.AVPlaybackMode;

public class WFDSigmaActivity extends Activity {
    public static final String TAG = "SIGMA.WFDSigmaActivity";

    private TextView mynameTextView;
    private TextView mymacaddressTextView;
    private TextView mystatusTextView;

    private TextView mynameTextViewWigig;
    private TextView mymacaddressTextViewWigig;
    private TextView mystatusTextViewWigig;
    private Whisperer whisperer;
    private NetManagerIF netManager;
    private ClientEventHandler mEventHandler;

    public static final Object SigmaP2pStaticLock = new Object();
    private static final int DISCOVERY_TIMEOUT = 10000; // 10 secs

    boolean ipTaskSuccess = false;
    public IWfdActionListener mActionListener;
    boolean sigmaDiscoverySuccess= false;
    boolean sigmaP2pConnectSuccess= false;
    String sigmaPeerMacAddress;
    ConnectionInfoIF localDevInfo;
    private WfdEnums.WFDDeviceType mLocalDeviceType;
    NetDeviceIF mlocalDevice;
    NetDeviceIF sigmaPeerDevice;
    private WfdDevice localWfdDev, peerWfdDev;
    private final int START_SURFACE = 1;
    static DutSocketServer socketConnection;
    Thread socketServerThread;
    private IWfdSession wfdSession;
    private final Stack<IWfdSession> mSessionStack = new Stack<IWfdSession>();
    private boolean mSurfaceStared;
    private int localExtSupport = 0;
    private int peerExtSupprt = 0;
    private AVPlaybackMode avPlaybackMode;
    ProgressDialog ipDialog;

    /**
     * @param isWifiP2pEnabled
     *            the isWifiP2pEnabled to set
     */
    public void setIsWifiP2pEnabled(boolean isWifiP2pEnabled) {
        whisperer.setWifiP2pEnabled(isWifiP2pEnabled);
        if (!whisperer.isWifiP2pEnabled()) {
            Log.d(TAG, "Find selected from action bar while p2p off");
            Toast.makeText(WFDSigmaActivity.this,
            R.string.p2p_off_warning, Toast.LENGTH_SHORT)
            .show();
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(TAG, "onCreate() called");
        whisperer = Whisperer.getInstance();
        whisperer.setActivity(this);

        setContentView(R.layout.sigma_layout);
        mEventHandler = new ClientEventHandler();
        whisperer.setNetManager(new CombinedNetManager(mEventHandler, WfdEnums.NetType.WIFI_P2P, WfdEnums.NetType.WIGIG_P2P));
        netManager = whisperer.getNetManager();
        netManager.send_wfd_set(true, whisperer.getLocalDeviceType(),(localExtSupport == 1)?true:false);

        // SystemProperties.set("debug.wfd.sigma.activity","1");

        try {
            ServiceUtil.bindService(getApplicationContext(), mEventHandler);
        } catch (ServiceFailedToBindException e) {
            Log.e(TAG, "ServiceFailedToBindException received");
        }

        //start socket server thread that accepts and parses commands from sigma
        socketConnection = new DutSocketServer(this);
        socketServerThread = new Thread(socketConnection);
        socketServerThread.start();

        mynameTextView = (TextView) this.findViewById(R.id.tv_my_name);
        mymacaddressTextView = (TextView) this.findViewById(R.id.tv_my_mac_address);
        mystatusTextView = (TextView) this.findViewById(R.id.tv_my_status);
        whisperer.setMultiSessionMode(false);
        Whisperer.getInstance().registerHandler(mEventHandler, WFDSigmaActivity.class.getName());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(!mSessionStack.empty()) {
            IWfdSession currSesion = mSessionStack.pop();
            if(currSesion != null)
            {
                try {
                    currSesion.deinit();
                } catch (RemoteException e) {
                    Log.e(TAG, "RemoteException in deInit");
                }
            }
        }
        if (ServiceUtil.getmServiceAlreadyBound())
        {
            try {
                ServiceUtil.getInstance().deinit();
            } catch (RemoteException e) {
                Log.e(TAG, "RemoteException in deInit");
            }
        }
        try {
            unbindWfdService();
        } catch (IllegalArgumentException e) {
            Log.e(TAG,"Illegal Argument Exception ",e);
        }
        // SystemProperties.set("debug.wfd.sigma.activity","0");
        mSessionStack.clear();
        Whisperer.getInstance().unRegisterHandler(WFDSigmaActivity.class.getName());

        whisperer.getNetManager().destroy();
        whisperer.setNetManager(null);
        whisperer.clear();
        whisperer = null;
        //Close socket server
        socketConnection.StopServer();
        socketConnection = null;
        socketServerThread = null;
        Log.d(TAG, "onDestroy() called");
    }

    /** To prevent dynamic configuration changes from destroying activity */
    @Override
    public void onConfigurationChanged (Configuration newConfig) {
        Log.e(TAG, "onConfigurationChanged called due to"+ newConfig.toString());
        super.onConfigurationChanged(newConfig);
    }

    /** register the BroadcastReceiver with the intent values to be matched */
    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume() called");
        netManager.onMainActivityResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        netManager.onMainActivityPause();
        Log.d(TAG, "onPause() called");
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        super.onPrepareOptionsMenu(menu);
        whisperer.setModeDS(false);
        return true;
    }

    public void disconnect() {
        Log.d(TAG, "disconnect() called");
        netManager.disconnect(ResultListener.NullListener);
    }

    public void cancelDisconnect() {
    /*
     * A cancel abort request by user. Disconnect i.e. removeGroup if
     * already connected. Else, request WifiP2pManager to abort the ongoing
     * request
    */
        netManager.cancelConnect(new ResultListener() {
            @Override
            public void onSuccess(Object result) {
                Toast.makeText(WFDSigmaActivity.this,
                    "Aborting connection",
                    Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(Object err) {
                Toast.makeText(
                        WFDSigmaActivity.this,
                        "Connect abort request failed. Reason Code: "
                                + err,
                        Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.sigma_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        return super.onOptionsItemSelected(item);
    }

    private void configureDeviceAsSourceOrSink(int deviceType, String extSupport) {
        Log.d(TAG, "Configure Sigma App as Source or Sink");
        if (deviceType == WFDDeviceType.SOURCE.getCode()) {
            Log.d(TAG, "Local device being configured as Source");
            whisperer.setLocalDeviceType(WFDDeviceType.SOURCE);
               netManager.send_wfd_set(true, WFDDeviceType.SOURCE,(localExtSupport == 1)?true:false);

        } else {
            Log.d(TAG, "Local device being configured as Sink");
            whisperer.setLocalDeviceType(WFDDeviceType.PRIMARY_SINK);
               netManager.send_wfd_set(true, WFDDeviceType.PRIMARY_SINK,(localExtSupport == 1)?true:false);
        }
        mLocalDeviceType =  whisperer.getLocalDeviceType();

        //configure R1 / R2
        if (0 == extSupport.compareToIgnoreCase("R1")) {
            Log.d(TAG, "Configuring R1 session");
            localExtSupport = peerExtSupprt = 0;
        }
        else if (0 == extSupport.compareToIgnoreCase("R2")) {
            Log.d(TAG, "Configuring R2 session");
            localExtSupport = peerExtSupprt = 1;
        }
        else {
            Log.d(TAG, "Unknown / Invalid parameter, default to R1");
            localExtSupport = peerExtSupprt = 0;
        }
    }

    public void configureSessionAsSourceOrSink() {
        Log.d(TAG, "Configure Sigma session stack as Source or Sink");

        if (ServiceUtil.getmServiceAlreadyBound()) {
            try {
                for (IWfdSession session: mSessionStack) {
                    session.setDeviceType(mLocalDeviceType.getCode());
                }
            } catch (RemoteException e) {
                    Log.e(TAG, "Remote exception when setting device type", e);
            }
        } else {
            try {
                ServiceUtil.bindService(getApplicationContext(), mEventHandler);
            } catch (ServiceFailedToBindException e) {}
        }
    }


    public void onOptionSigmaVersion(MenuItem i) {
        Log.d(TAG, "Version selected from menu");
        try {
            String version = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
            AlertDialog alertDialog = new AlertDialog.Builder(this).create();
            alertDialog.setTitle("Version #");
            alertDialog.setMessage(version);
            alertDialog.show();
        } catch (NameNotFoundException e) {
            Log.e("tag", e.getMessage());
        }
    }

    void unbindWfdService() {
        Log.d(TAG, "unbindWfdService() called");
        if(ServiceUtil.getmServiceAlreadyBound()) {
            ServiceUtil.unbindService(getApplicationContext());
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
        String permissions[], int[] grantResults) {
        whisperer.getNetManager().updateSystemPermissions(requestCode, permissions, grantResults);
    }

    public void setupWfdSession (IWfdSession  session) {
        Log.d(TAG, "setupWfdSession() called");

        wfdSession = session;

        if( mLocalDeviceType == WfdEnums.WFDDeviceType.SOURCE) {
                Whisperer.getInstance().getNetManager().setMiracastMode(NetManagerIF.MIRACAST_SOURCE);
         }else {
                Whisperer.getInstance().getNetManager().setMiracastMode(NetManagerIF.MIRACAST_SINK);
        }

        if (avPlaybackMode != null) {
                try {
                    wfdSession.setAvPlaybackMode(avPlaybackMode.ordinal());
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
         }

        try {
                wfdSession.setDeviceType(mLocalDeviceType.getCode());
         } catch (RemoteException e) {
                e.printStackTrace();
         }

         if (!sigmaPeerDevice.supportWfd()) {
                Log.e(TAG, "Peer Device is missing wfdInfo");
          }

        if (localDevInfo != null) {
            if (localDevInfo.isGroupOwner()) {
                socketConnection.localWfdDev = localWfdDev = mlocalDevice.convertToWfdDevice();
                if (mLocalDeviceType  != WfdEnums.WFDDeviceType.SOURCE) {
                    final getIPTask Task = new getIPTask();
                    Task.execute(sigmaPeerDevice);
                    ipDialog = ProgressDialog.show(this,
                               "Awaiting IP allocation of peer from DHCP",
                               "Press back to cancel", true, true,
                                new DialogInterface.OnCancelListener() {
                                @Override
                                    public void onCancel(DialogInterface dialog) {
                                        Log.e(TAG,  "IP Dialog cancelled by listener");
                                        Task.cancel(false);
                                    }
                                });
                } else {
                    // Source doesn't require sink IP
                    socketConnection.peerWfdDev = peerWfdDev =  sigmaPeerDevice.convertToWfdDevice();
                    socketConnection.peerWfdDev.decoderLatency = peerWfdDev.decoderLatency = 0; //default

                }
            } else {
                Log.e(TAG, "Local Device is not a group owner");
                socketConnection.localWfdDev = localWfdDev = mlocalDevice.convertToWfdDevice();
                socketConnection.peerWfdDev = peerWfdDev = sigmaPeerDevice.convertToWfdDevice();
                socketConnection.peerWfdDev.decoderLatency =  peerWfdDev.decoderLatency = 0; //default
            }
        } else if (localDevInfo == null) {
            Log.e(TAG, "Something amiss!! Local Device Info is null.");
        }

        //allowing 2 combinations for now from sigma command - (1)  r1 source and r1 sink (2) r2 source and r2 sink

        socketConnection.localWfdDev.extSupport = localWfdDev.extSupport = localExtSupport;
        socketConnection.peerWfdDev.extSupport = peerWfdDev.extSupport = peerExtSupprt;

    }

    class getIPTask extends AsyncTask<NetDeviceIF, Integer, Boolean> {
        private int timeout = 0;
        String show = new String("Failed to get IP from lease file in attempt ");
        private NetDeviceIF peerDevice = null;
        @Override
        protected Boolean doInBackground(NetDeviceIF... devices) {
            peerDevice = devices[0];
            while (timeout++ < 60) { // try 60 seconds for IP to get populated else
                               // give up
                socketConnection.peerWfdDev = peerWfdDev = sigmaPeerDevice.convertToWfdDevice();
                if (peerWfdDev != null && peerWfdDev.ipAddress != null) {
                    return true;
                }
                if (isCancelled()) {
                    Log.e(TAG, "getIP Task was cancelled");
                    return false;
                }
                publishProgress(timeout);
                Log.e(TAG, show + timeout);
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    Log.e(TAG, "Background thread of async task interrupted");
                }
            }
            return false;
        }

        @Override
        protected void onProgressUpdate(Integer... progress) {
            ipDialog.setMessage(show + progress[0]);
        }

        @Override
        protected void onPostExecute(Boolean success) {
            if (ipDialog != null) {
                ipDialog.dismiss();
            }
            if (success) {
                Log.e(TAG, "Successful in obtaining Peer Device IP");
            } else {
                Log.e(TAG, "Unsuccessful in obtaining Peer Device IP");
            }
            ipTaskSuccess = success;
            notifyCompletion();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && whisperer.isInSession()) {
            Log.d(TAG, "Back button pressed while starting session");
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_BACK) {
            Log.d(TAG, "Back button pressed");
            AlertDialog.Builder builder = new AlertDialog.Builder(WFDSigmaActivity.this);
                builder.setMessage("Would you like to exit the application?")
                    .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            WFDSigmaActivity.this.finish();
                        }
                    })
                    .setNegativeButton("No", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            dialog.cancel();
                        }
                    });
            AlertDialog alert = builder.create();
            alert.show();
        }
        return super.onKeyDown(keyCode, event);
    }


    public void updateThisDevice(NetDeviceIF device) {
        Log.d(TAG, "updateThisDevice called");

        if (netManager instanceof CombinedNetManager) {
            Map<WfdEnums.NetType, NetDeviceIF> localDevices = ((CombinedNetManager) netManager).getLocalDevices();
            for (WfdEnums.NetType type: localDevices.keySet()) {
                NetDeviceIF dev = localDevices.get(type);
                if (!dev.hasDeviceInfo()) {
                    continue;
                }
                if (type == WfdEnums.NetType.WIFI_P2P) {
                    mynameTextView.setText(dev.getName());
                    mymacaddressTextView.setText(dev.getAddress());
                    mystatusTextView.setText(dev.getStatusString());
                } else if (type == WfdEnums.NetType.WIGIG_P2P) {
                    mynameTextViewWigig.setText(dev.getName());
                    mymacaddressTextViewWigig.setText(dev.getAddress());
                    mystatusTextViewWigig.setText(dev.getStatusString());
                } else {
                    Log.e(TAG, "unknown net type: " + type);
                }
            }
        }
    }

    public void deinitWFDService() {
        try {
            Log.e(TAG, "WiFi Direct deinit do in background started");
            ServiceUtil.getInstance().deinit();
            Log.e(TAG, "WiFi Direct deinit do in background finished");
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception", e);
        }
    }


    /**
     * Class for internal event handling in WFDSigmaActivity. Must run on UI thread.
     */
    class ClientEventHandler extends Handler {

        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "Event handler received: " + msg.what);

            if (WfdClientUtils.wfdOperationProgressDialog != null) {
                Log.d(TAG, "clientProgressDialog != null");
                if (WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                    Log.d(TAG, "clientProgressDialog isShowing");
                } else {
                    Log.d(TAG, "clientProgressDialog not isShowing");
                }
            } else {
                Log.d(TAG, "clientProgressDialog == null");
            }

            switch (msg.what) {

                case PLAY_CALLBACK: {
                    Log.d(TAG, "EventHandler: PLAY_CALLBACK");
                    /*Bundle negRes = new Bundle();
                    try {
                        int ret = wfdSession.getNegotiatedResolution(negRes);
                        if (ret == 0) {
                            int [] res = negRes.getIntArray("negRes");
                            Log.d(TAG, "Negotiated Resolution : H264 Profile= " + res[0] +" CEA Mode= " + res[1] + " VESA Mode= " + res[2] + " HH Mode= " + res[3] );
                            printResolutionFps(res[1],res[2],res[3]);
                        } else {
                            Log.e(TAG, "Failed to fetch negotiated resolution: " + ret);
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "EventHandler: PLAY_CALLBACK: Exception " + e);
                    }*/
                }
                break;
                case PAUSE_CALLBACK: {
                    Log.d(TAG, "EventHandler: PAUSE_CALLBACK");
                }
                break;
                case STANDBY_CALLBACK: {
                    Log.d(TAG, "EventHandler: STANDBY_CALLBACK");
                }
                break;
                case INVALID_WFD_DEVICE: {
                    AlertDialog.Builder builder = new AlertDialog.Builder(WFDSigmaActivity.this);
                    builder.setTitle("Selected device does not support WFD")
                        .setMessage("Please select another device to start a WFD session")
                        .setCancelable(false)
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                dialog.cancel();
                            }
                        });
                    AlertDialog alert = builder.create();
                    alert.show();
                }
                break;
                case SERVICE_BOUND: {
                    WFDSigmaActivity.this.mActionListener = WfdClientUtils.createActionListener(mEventHandler);
                    Toast.makeText(getApplicationContext(), "WFD service connected",
                        Toast.LENGTH_SHORT).show();
                }
                break;
                case CLEAR_UI: {
                    if(!whisperer.isSourceMode()) {
                        Log.e(TAG, "Calling finish on SurfaceActivity");
                            try {
                                finishActivity (START_SURFACE);
                            } catch (ActivityNotFoundException e) {
                                Log.e(TAG, "Surface Activity not yet started/already finished");
                            }
                        }
                }
                //fall through to TEARDOWN_CALLBACK case handling for cleanup
                case TEARDOWN_CALLBACK: {
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        Log.d(TAG, "EventHandler: teardownCallback- dismiss clientProgressDialog");
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                    }
                    cancelDisconnect();
                    netManager.setMiracastMode(NetManagerIF.MIRACAST_DISABLED);
                    if (!whisperer.isMultiSinkMode() ||
                        whisperer.getLocalDeviceType() == WfdEnums.WFDDeviceType.SOURCE) {
                        if (ServiceUtil.getmServiceAlreadyBound()) {
                            try {
                                ServiceUtil.getInstance().unregisterListener(
                                        mActionListener);
                            } catch (RemoteException e) {
                                Log.e(TAG, "RemoteException in un-registering listener" + e);
                            }
                        }
                        Log.d(TAG,"unbinding from service. Source or Not in multi-session mode");

                        if(!mSessionStack.empty()) {
                            IWfdSession currSesion = mSessionStack.pop();
                            if(currSesion != null)
                            {
                                try {
                                    currSesion.deinit();
                                } catch (RemoteException e) {
                                    Log.e(TAG, "RemoteException in deInit");
                                }
                            }
                        }

                        if (ServiceUtil.getmServiceAlreadyBound())
                        {
                            try {
                                ServiceUtil.getInstance().deinit();
                            } catch (RemoteException e) {
                                Log.e(TAG, "RemoteException in deInit");
                            }
                        }
                        mSessionStack.clear();
                        unbindWfdService();
                    } else {
                        Log.d(TAG,"Not unbinding from service in multi-session mode");
                    }
                    Log.d(TAG, "EventHandler: teardownCallback- completed");
                    whisperer.setInSession(false);
                    whisperer.setModeDS(false);

                    sigmaDiscoverySuccess = false;
                    sigmaP2pConnectSuccess = false;
                    sigmaPeerMacAddress = null;
                    localDevInfo = null;
                    mlocalDevice = null;
                    sigmaPeerDevice = null;
                    wfdSession = null;
                    localExtSupport = 0; peerExtSupprt = 0;
                }
                break;
                case START_SESSION_ON_UI: {
                    Log.d(TAG, "EventHandler: startSessionOnUI");
                }
                break;
                case UIBC_ACTION_COMPLETED: {
                    Log.d(TAG, "EventHandler: uibcActionCompleted- completed");
                }
                break;
                case REMOVE_WFD_SESSION:
                    IWfdSession wfdSession= IWfdSession.Stub.asInterface((android.os.IBinder) msg.obj);
                    if(mSessionStack.remove(wfdSession)) {
                        Log.d(TAG,"Removed WFD session");
                    } else {
                        Log.wtf(TAG,"Unable to find session in stack!");
                    }
                break;
                case RTP_TRANSPORT_NEGOTIATED: {
                    Log.d(TAG,"EventHandler : RTP Port Negotiation Successful");
                }
                break;
                case RTP_TRANSPORT_NEGOTIATED_FAIL: {
                    Log.d(TAG, "EventHandler : RTP Port Negotiation Failed");
                }
                break;
                case TCP_PLAYBACK_CONTROL_DONE: {
                    Log.d(TAG,"EventHandler : Playback control successful");
                }
                break;
                case NetManagerIF.WIFI_UTIL_RETRY_CHANNEL: {
                    Toast.makeText(WFDSigmaActivity.this, "Channel lost. Trying again",
                        Toast.LENGTH_LONG).show();
                    // resetData();
                }
                break;
                case NetManagerIF.WIFI_UTIL_CHANNAL_LOST_PERMANENTLY: {
                    Toast.makeText(
                            WFDSigmaActivity.this,
                            "Severe! Channel is probably lost permanently. Try Disable/Re-Enable P2P.",
                            Toast.LENGTH_LONG).show();
                }
                break;
                case NetManagerIF.WIFI_UTIL_P2P_STATE_CHANGED: {
                    setIsWifiP2pEnabled(netManager.isEnabled());
                }
                break;
                case NetManagerIF.WIFI_UTIL_DISCONNECTED: {
                    Log.d(TAG, "WIFI_UTIL_DISCONNECTED");
                    if ((sigmaDiscoverySuccess) && (sigmaPeerDevice != null)) {
                        Log.d(TAG, "P2P Disconnected abruptly, cleaning up..");
                        sigmaDiscoverySuccess = false;
                        sigmaP2pConnectSuccess = false;
                        sigmaPeerMacAddress = null;
                        localDevInfo = null;
                        mlocalDevice = null;
                        sigmaPeerDevice = null;
                        wfdSession = null;
                        localExtSupport = 0; peerExtSupprt = 0;
                        unbindWfdService();
                        notifyCompletion();
                    }
                }
                break;
                case NetManagerIF.WIFI_UTIL_CONNECTION_INFO: {
                    Log.d(TAG, "WIFI_UTIL_CONNECTION_INFO");
                    localDevInfo = (ConnectionInfoIF)msg.obj;
                }
                break;
                case NetManagerIF.WIFI_UTIL_PEERS_CHANGED: {
                    Collection<NetDeviceIF> peers = (Collection<NetDeviceIF>)msg.obj;
                    onPeersChanged(peers);
                }
                break;
                case NetManagerIF.WIFI_UTIL_CONNECTED_PEERS_CHANGED: {
                    Log.d(TAG, "WIFI_UTIL_CONNECTED_PEERS_CHANGED");
                    sigmaP2pConnectSuccess = true;
                    notifyCompletion();
                }
                break;
                case NetManagerIF.WIFI_UTIL_LOCAL_DEVICE_CHANGED: {
                    updateThisDevice((NetDeviceIF)msg.obj);
                }
                break;
                default:
                    Log.e(TAG, "Unknown event received: " + msg.what);
            }
        }
    }

    public void onPeersChanged(Collection<NetDeviceIF> peers) {
        if (whisperer.isMultiSinkMode()) {
            //onMultiSinkPeersChanged(peers);
        } else if (whisperer.isDiscoveryRequested()) {
            onPeersAvailable(peers);
        }
    }

    // update peers list in non-multisink mode
    private void onPeersAvailable(Collection<NetDeviceIF> peers) {
        Log.d(TAG, "onPeersAvailable called");
        if (peers == null || peers.size() == 0 ||sigmaPeerMacAddress == null) {
            Log.d(TAG, "No devices found");
            sigmaDiscoverySuccess = false;
            return;
        }

        if(sigmaDiscoverySuccess == true) {
            // Prevent processing any newer list of peers sent by Wifi framework, when already connect is in process
            Log.d(TAG, "SIGMA:Desired Peer was already discovered, hence ignoring this peer list");
            return;
        }

        boolean matchfound = false;
        for (NetDeviceIF peer : peers) {
            Log.d(TAG, "NetDeviceIF: peer mac discovered:  " + peer.getAddress());
            if(0 == peer.getAddress().compareTo(sigmaPeerMacAddress)) {
                Log.d(TAG, "Our peer has been discovered from the peer list !!");
                sigmaPeerDevice = peer;
                matchfound = true;
                sigmaDiscoverySuccess = true;
                notifyCompletion();
            }
        }
        if(!matchfound) {
            Log.d(TAG, "Requested peer not found in the discovered list");
            sigmaDiscoverySuccess = false;
        }
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult() called with "+ resultCode +"becasue of activity"
                 + "started with " + requestCode);
        mSurfaceStared = false;
        if(requestCode == START_SURFACE && resultCode == RESULT_CANCELED) {
            Log.e(TAG, "Surface Activity has been destroyed , clear up UI");
            Message messageClearUI = mEventHandler.obtainMessage(CLEAR_UI);
            mEventHandler.sendMessage(messageClearUI);
        }
    }

    public void showHideWfdSurface(final IWfdSession wfdsession) {
        if (!mSurfaceStared) {
            Intent i = new Intent(this, SurfaceActivity.class);
            i.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            //The below method is listed as deprecated, but its usable for now
            i.putExtra(WFD_SESSION_BINDER_KEY, wfdsession.asBinder());
            startActivityForResult(i, START_SURFACE);
            mSurfaceStared = true;
        } else {
            //SurfaceActivity is already running, so we need another session
            Whisperer.getInstance().getHandler(SurfaceActivity.class.getName()).obtainMessage(42,wfdsession).sendToTarget();
        }
    }

    public void bringWfdSurfaceToForeground() {
        Intent i = new Intent(this, SurfaceActivity.class);
        i.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
        startActivity(i);
    }

    public int enableWfd(String ipAddress, String macAddress, String peerAddress, String smControlPort, String deviceType, String decoderLatency)
    {
        Log.d(TAG, "Succeeded in parsing command and calling :: enableWfd");
        Log.d(TAG, "ipAddress="+ipAddress+"macAddress="+macAddress+"peerAddress="+peerAddress+
             "SMControlPort="+smControlPort+"deviceType="+deviceType+"decoderLatency="+decoderLatency);
        return 0;
    }

    public int startWfdSession(String macAddress, String deviceType, String goIntentValue, String initWfdVal, String extSupport) {
        Log.d(TAG, "Succeeded in parsing command and calling :: startWfdSession");
        Log.d(TAG, "macAddress=" +macAddress+ "deviceType=" +deviceType+ "goIntentValue=" +goIntentValue+ "initWfdVal=" +initWfdVal+ "MiracastVersion="+extSupport);

        try {

            if(!ServiceUtil.getmServiceAlreadyBound()){
               /* Previous teardown leads to WFDService being destroyed.
                * We'll need to re-bind before launching a new session
                */
                Log.e(TAG, "Re-binding to WFD service");
                try {
                    ServiceUtil.bindService(getApplicationContext(), mEventHandler);
                } catch (ServiceUtil.ServiceFailedToBindException e) {
                    Log.e(TAG, "ServiceFailedToBindException received");
                }
            }

            configureDeviceAsSourceOrSink(Integer.parseInt(deviceType), extSupport);

            int  initReturn = discoverPeerAndConnect(macAddress.trim(), Integer.parseInt(goIntentValue), Integer.parseInt(initWfdVal));

            if (0 == initReturn) {
                Log.d(TAG, "P2P Connect Success !!");
                initSigmaWfdSession();
                if (1 == Integer.parseInt(deviceType)) {
                    Log.d(TAG, "Device is running as Sink, start SurfaceActivity...");
                    startSigmaSinkSurface();
                }
            }
            else {
                Log.d(TAG, "P2P Connect Failed !!");
                return initReturn;
            }
            initReturn = wfdSession.init(mActionListener, localWfdDev);
            if (!(initReturn == 0 || initReturn == WfdEnums.ErrorType.ALREADY_INITIALIZED.getCode())) {
                Log.e(TAG, "startWfdSession: init failed with error- " + initReturn);
                return initReturn;
            }

            initReturn = wfdSession.startWfdSession(peerWfdDev);
            Log.e(TAG, "startWfdSession - initReturn : " + initReturn);

            Whisperer.getInstance().setInSession(true);
            if (!whisperer.isMultiSinkMode()) {
                netManager.stopPeerDiscovery(ResultListener.NullListener);
            }
            return initReturn;
        }
        catch (RemoteException e) {
            Log.e(TAG, "startWfdSession failed", e);
            return -1;
        }
    }

    public void teardownRTSP(){
        Log.d(TAG, "Succeeded in parsing command and calling :: teardownRTSP");
        if (mSessionStack.empty()) {
            Log.d(TAG, "teardown: no sessions ongoing");
            return;
        }

        try {
            if(ServiceUtil.getmServiceAlreadyBound()) {
                Log.d(TAG, "Calling - teardown()");
                int ret =  wfdSession.teardown();
                if (ret < 0) {
                    Log.e(TAG, "Error when calling teardown(): " + ret);
                } else {
                    Log.d(TAG, "Called teardown() successfully");
                }
            }
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException in teardown");
        }
        Log.d(TAG, "Called teardown() successfully- outside try block");
        localWfdDev = peerWfdDev = null;
    }

    public void pauseRTSP(){
        Log.d(TAG, "Succeeded in parsing command and calling :: pauseRTSP");
        try {
            Log.d(TAG, "Calling - pause()");
            int ret = wfdSession.pause();
            if (ret < 0) {
                Log.e(TAG, "Error when calling pause(): " + ret);
            } else {
                Log.d(TAG, "Called pause() successfully");
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception when calling pause()", e);
        }
        Log.d(TAG, "Called pause() successfully- outside try block");
    }

    public void playRTSP(){
        Log.d(TAG, "Succeeded in parsing command and calling :: playRTSP");
        try {
            Log.d(TAG, "Calling - play()");
            int ret = wfdSession.play();
            if (ret < 0) {
                Log.e(TAG, "Error when calling play(): " + ret);
            } else {
                Log.d(TAG, "Called play() successfully");
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception when calling play()", e);
        }
        Log.d(TAG, "Called play() successfully- outside try block");
    }

    public int standbyRTSP(){
        Log.d(TAG, "Succeeded in parsing command and calling :: standbyRTSP");
        try {
            Log.d(TAG, "Calling - standby()");
            int ret = wfdSession.standby();
            if (ret < 0) {
                Log.e(TAG, "Error when calling standby(): " + ret);
            } else {
                Log.d(TAG, "Called standby() successfully");
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception when calling standby()", e);
        }
        Log.d(TAG, "Called standby() successfully- outside try block");
        return 0;
    }

    public void showSinkSurface(){
        Log.d(TAG, "Succeeded in parsing command and calling :: showSinkSurface");
        bringWfdSurfaceToForeground();  //Bring Sink Surface to FG
    }

    public void hideSinkSurface(){
        Log.d(TAG, "Succeeded in parsing command and calling :: hideSinkSurface");
        moveTaskToBack(true); //Move Sink to BG
    }

    public void startUIBC(){
        Log.d(TAG, "Succeeded in parsing command and calling :: startUIBC");
        try {
            int ret = wfdSession.startUibcSession();
            Log.e(TAG, "startUIBC: startUibcSession failed with error- " + ret);
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception when calling startUibcSession()", e);
        }
    }

    public void stopUIBC(){
        Log.d(TAG, "Succeeded in parsing command and calling :: stopUIBC");
        try {
            int ret = wfdSession.stopUibcSession();
            Log.e(TAG, "stopUIBC: stopUibcSession failed with error- " + ret);
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception when calling stopUibcSession()", e);
        }
    }

    public void selectTCP(String tcp_buffering_delay){
        Log.d(TAG, "Succeeded in parsing command and calling :: selectTCP :: tcp_buffering_delay = " + tcp_buffering_delay);
        try {
            int ret = wfdSession.setRtpTransport(WfdEnums.RtpTransportType.TCP.ordinal(), Integer.parseInt(tcp_buffering_delay), 0);
            if (ret < 0) {
                Log.e(TAG, "Error when calling setRtpTransport(): " + ret);
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception when calling setRtpTransport()", e);
        }
    }

    public void selectUDP(){
        Log.d(TAG, "Succeeded in parsing command and calling :: selectUDP");
        try {
                int ret = wfdSession.setRtpTransport(WfdEnums.RtpTransportType.UDP.ordinal(), 0, 0);
                if (ret < 0) {
                    Log.e(TAG, "Error when calling setRtpTransport(): " + ret);
                }
        } catch (RemoteException e) {
                  Log.e(TAG, "Remote exception when calling setRtpTransport()", e);
        }
    }

    public void setDecoderLatency(String decoderLatency){
        Log.d(TAG, "Succeeded in parsing command and calling :: setDecoderLatency :: decoder latency = " + decoderLatency);
        try {
               int ret = wfdSession.setDecoderLatency(Integer.parseInt(decoderLatency));
               if (ret < 0) {
                      Log.e(TAG, "Error when calling setDecoderLatency(): " + ret);
               }
        } catch (RemoteException e) {
               Log.e(TAG, "Remote exception when calling setDecoderLatency()", e);
        }
    }

    public void flush(){
        Log.d(TAG, "Succeeded in parsing command and calling :: flush");
        try {
               int ret = wfdSession.tcpPlaybackControl(WfdEnums.ControlCmdType.FLUSH.ordinal(), 0);
               if (ret < 0) {
                   Log.e(TAG, "Error when calling flush(): " + ret);
               }
        } catch (RemoteException e) {
               Log.e(TAG, "Remote exception when calling flush()", e);
        }
    }

    public void toggleDirectStreaming(boolean mode){
        Log.d(TAG, "Succeeded in parsing command and calling :: toggleDirectStreaming");
        try {
               if(!ServiceUtil.getmServiceAlreadyBound()) {
                   Log.e(TAG, "FAIL: toggleDirectStreaming - called when not in WFD Session.");
                   return;
               }

               WfdStatus wfdStatus = wfdSession.getStatus();
               if (wfdStatus.state != WfdEnums.SessionState.PLAY.ordinal()) {
                   Log.e(TAG, "FAIL: toggleDirectStreaming Mode not honoured in non-PLAY state.");
                   return;
               }

               int ret = wfdSession.toggleDSMode(mode);
               if (ret < 0) {
                   Log.e(TAG, "Error when calling toggleDSMode(): " + ret);
               }
        } catch (RemoteException e) {
                   Log.e(TAG, "Remote exception when calling toggleDSMode()", e);
        }
    }

    private int discoverPeerAndConnect(String peerMacAddress, int goIntentValue, int initWfd)
    {
        Log.d(TAG, "discoverPeerAndConnect");
        synchronized (SigmaP2pStaticLock) {

        sigmaDiscoverySuccess = false;
        sigmaP2pConnectSuccess = false;
        sigmaPeerMacAddress = peerMacAddress;
        Log.d(TAG, "SIGMA: peerMacAddress ->"+sigmaPeerMacAddress);
        whisperer.setDiscoveryRequested(true);
        netManager.discoveryPeers(new ResultListener() {
                @Override
                public void onSuccess(Object result) {
                    Toast.makeText(WFDSigmaActivity.this,
                            "Discovery Initiated",
                            Toast.LENGTH_SHORT).show();
                }

                @Override
                public void onFailure(Object err) {
                    Toast.makeText(WFDSigmaActivity.this,
                            "Discovery Failed : " + err,
                            Toast.LENGTH_SHORT).show();
                    notifyCompletion();
                }
        });

        /* Follow the SIGMA certification order, the device with initWfd as 0 will wait to be discovered
           and not attempt any retry, while the device with initWfd as 1 will retry discovery if needed */
        try {
            if(initWfd == 1) {
                SigmaP2pStaticLock.wait(DISCOVERY_TIMEOUT) ;  //await peer list discovery and matching
            }
            else {
                SigmaP2pStaticLock.wait(); // in this case don't return any discovery status to ref device
            }
        } catch (Exception e) {
            Log.e(TAG, "SIGMA:Exception" + e);
        }

        if ((sigmaPeerDevice != null) && sigmaDiscoverySuccess)  {
            Log.d(TAG, "SIGMA:Peer has been discovered successfully");
            if (initWfd == 1) {
                Log.d(TAG, "SIGMA: initWfd is enabled, hence initiating P2P connect");
                WifiP2pConfig config = new WifiP2pConfig();
                config.deviceAddress = sigmaPeerDevice.getAddress();
                config.wps.setup = WpsInfo.PBC;
                if (goIntentValue > 15) {
                      config.groupOwnerIntent = 3; //setting default value
                } else {
                      config.groupOwnerIntent = goIntentValue;
                }

                sigmaPeerDevice.setConfig(config);
                connect(sigmaPeerDevice);

                try {
                    SigmaP2pStaticLock.wait();  //wait for P2P Connect result
                } catch (Exception e) {
                    Log.e(TAG, "SIGMA:Exception" + e);
                }
            }
            else {
                Log.d(TAG, "SIGMA: initWfd is disabled");
                if (sigmaP2pConnectSuccess == true) {
                    /* This is a corner case when p2p connect has already happened successfully
                       prior to discovery attempt. This can happen when the sigma peer at the other end,
                       with initWfd as 1, has successfully discovered and initiated p2p connect */
                    Log.d(TAG, "SIGMA: we were already connected, now peer discovery has also succeeded !!");
                } else {
                    //we need to await p2p connect result to become successful, before we start session
                    Log.d(TAG, "SIGMA: awaiting P2P connect from other end");
                    try {
                        SigmaP2pStaticLock.wait();  //wait for P2P Connect result
                    } catch (Exception e) {
                        Log.e(TAG, "SIGMA:Exception" + e);
                    }
                }
            }
        }
        else {
            Log.e(TAG, "SIGMA:Peer discovery failed !!" + sigmaPeerDevice + sigmaDiscoverySuccess);  //show a toast here, asking to retry..
            this.runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(WFDSigmaActivity.this,
                        "Peer Discovery Failed, Try again !! ",
                        Toast.LENGTH_SHORT).show();
                }
            });
            return -5; //if peer discovery failed, proceed no further.return retry error code
        }

        Log.d(TAG, "SIGMA:P2P Connect result : " + sigmaP2pConnectSuccess);
        return ((sigmaP2pConnectSuccess) ? 0 : -6) ;   //return connect failure error code
        }
    }

    private void connect(NetDeviceIF dev) {
        dev.connect(new ResultListener() {
            @Override
            public void onSuccess(Object result) {
                                Log.d(TAG, "P2P Connect Success !!");
                                  Toast.makeText(WFDSigmaActivity.this,
              "Connect Success.", Toast.LENGTH_SHORT)
              .show();
            }

            @Override
            public void onFailure(Object err) {
                Log.d(TAG, "P2P Connect Failure !!");
                Toast.makeText(WFDSigmaActivity.this,
                "Connect failed. Retry.", Toast.LENGTH_SHORT)
                .show();
            }
        });
    }

    public void initSigmaWfdSession() {
        Log.d(TAG, "initSigmaWfdSesssion");

        if(!ServiceUtil.getmServiceAlreadyBound()){
            /* This is not a normal scenario as we always bind service in onOptionFind()
            * Its almost impossible for user to start session without pressing find.
            * Just addressing the corner case here
            */
            Log.e(TAG, "Service is not already bound, do it now");
            try {
                ServiceUtil.bindService(getApplicationContext(), mEventHandler);
            } catch (ServiceUtil.ServiceFailedToBindException e) {
                Log.e(TAG, "ServiceFailedToBindException received");
            }
        }

        if (sigmaPeerDevice == null && whisperer.isMultiSinkMode()) {
            throw new IllegalArgumentException("InDevice is null in multi sink mode");
        }

        Log.d(TAG, "initSigmaWfdSesssion() called for " + (sigmaPeerDevice != null? sigmaPeerDevice.getName() : "unknown"));

        mlocalDevice = sigmaPeerDevice.getNetTypeManager().getLocalDevice();
        if (!mlocalDevice.hasDeviceInfo() ||localDevInfo == null) {
            Log.e(TAG, "Something amiss!! local device or localDevInfo is null");
            return;
        }

        avPlaybackMode = AVPlaybackMode.AUDIO_VIDEO;

        final IWfdSession session = createNewSessionInstance();
        mSessionStack.push(session);
        configureSessionAsSourceOrSink();

        if (mLocalDeviceType != WfdEnums.WFDDeviceType.SOURCE && localDevInfo.isGroupOwner()) {
            /* When sink is acting as GO, we need to await fetching peer IP from ARP Cache
            and peer device details to get populated, (Async Task should complete)
            before going on to call SM-A startWfdSession. In other cases of source and sink
            we are good to proceed and don't need any synchronization mechanism */
            Log.d(TAG, "initSigmaWfdSesssion: Sink as GO");
            synchronized (SigmaP2pStaticLock) {
                this.runOnUiThread(new Runnable() {
                    public void run() {
                        setupWfdSession(session);
                    }
                }); //end of this.runOnUiThread

                try {
                    ipTaskSuccess = false;
                    SigmaP2pStaticLock.wait();  //await AsyncTask to complete
                } catch (Exception e) {
                    Log.e(TAG, "SIGMA:Exception" + e);
                }

                if(!ipTaskSuccess) {
                    Log.e(TAG, "SIGMA:Peer device details fetch failed !!");
                    return;
                }
            }
        } else {
            setupWfdSession(session);
        }
    }
    public void startSigmaSinkSurface() {
        Log.d(TAG, "startSigmaSinkSurface");
        if (mLocalDeviceType != WfdEnums.WFDDeviceType.SOURCE) {
            Log.d(TAG, "device is sink, showHideWfdSurface");
            showHideWfdSurface(wfdSession);
        }
    }
    public void notifyCompletion() {
        Log.d(TAG, "notifyCompletion");
        synchronized (SigmaP2pStaticLock) {
            SigmaP2pStaticLock.notifyAll();
        }
    }

    public void printResolutionFps(int cea_mode, int vesa_mode, int hh_mode) {
        Log.d(TAG, "printResolutionFps");
        int height=0,width=0,frame_rate=0;

        if(cea_mode != 0)  //cea mode
        {
            switch (cea_mode)
            {
                case 1:
                    width = 640;
                    height = 480;
                    frame_rate = 60;
                    break;
                case 2:
                    width = 720;
                    height = 480;
                    frame_rate = 60;
                    break;
                case 4:
                //480i
                    width = 720;
                    height = 480;
                    frame_rate = 60;
                    break;
                case 8:
                    width = 720;
                    height = 576;
                    frame_rate = 50;
                    break;
                case 16:
                //576i
                    width = 720;
                    height = 576;
                    frame_rate = 50;
                    break;
                case 32:
                    width = 1280;
                    height = 720;
                    frame_rate = 30;
                    break;
                case 64:
                    width = 1280;
                    height = 720;
                    frame_rate = 60;
                    break;
                case 128:
                    width = 1920;
                    height = 1080;
                    frame_rate = 30;
                    break;
                case 256:
                    width = 1920;
                    height = 1080;
                    frame_rate = 60;
                    break;
                case 512:
                    //1080i 60
                    width = 1920;
                    height = 1080;
                    frame_rate = 60;
                    break;
                case 1024:
                    width = 1280;
                    height = 720;
                    frame_rate = 25;
                    break;
                case 2048:
                    width = 1280;
                    height = 720;
                    frame_rate = 50;
                    break;
                case 4096:
                    width = 1920;
                    height = 1080;
                    frame_rate = 25;
                    break;
                case 8192:
                    width = 1920;
                    height = 1080;
                    frame_rate = 50;
                    break;
                case 16384:
                    width = 1920;
                    height = 1080;
                    frame_rate = 50;
                    break;
                case 32768:
                    width = 1280;
                    height = 720;
                    frame_rate = 24;
                    break;
                case 65536:
                    width = 1920;
                    height = 1080;
                    frame_rate = 24;
                    break;

                default:
                    Log.d(TAG, "Invalid CEA mode value !!");
                    break;
            }
            Log.d(TAG, "Printing final negotiated resolution and frame rate : width = " +width + " height = " + height + " Frame rate = " + frame_rate + "fps");
        }
        else if(vesa_mode != 0)  //vesa mode
        {
            switch (vesa_mode)
            {
                case 1:
                    width = 800;
                    height = 600;
                    frame_rate = 30;
                    break;
                case 2:
                    width = 800;
                    height = 600;
                    frame_rate = 60;
                    break;
                case 4:
                    width = 1024;
                    height = 768;
                    frame_rate = 30;
                    break;
                case 8:
                    width = 1024;
                    height = 768;
                    frame_rate = 60;
                    break;
                case 16:
                    width = 1152;
                    height = 864;
                    frame_rate = 30;
                    break;
                case 32:
                    width = 1152;
                    height = 864;
                    frame_rate = 60;
                    break;
                case 64:
                    width = 1280;
                    height = 768;
                    frame_rate = 30;
                    break;
                case 128:
                    width = 1280;
                    height = 768;
                    frame_rate = 60;
                    break;
                case 256:
                    width = 1280;
                    height = 800;
                    frame_rate = 30;
                    break;
                case 512:
                    width = 1280;
                    height = 800;
                    width = 1366;
                    height = 768;
                    frame_rate = 30;
                    break;
                case 8192:
                    width = 1366;
                    height = 768;
                    frame_rate = 60;
                    break;
                case 16384:
                    width = 1280;
                    height = 1024;
                    frame_rate = 30;
                    break;
                case 32768:
                    width = 1280;
                    height = 1024;
                    frame_rate = 60;
                    break;
                case 65536:
                    width = 1400;
                    height = 1050;
                    frame_rate = 30;
                    break;
                case 131072:
                    width = 1400;
                    height = 1050;
                    frame_rate = 60;
                    break;
                case 262144:
                    width = 1440;
                    height = 900;
                    frame_rate = 30;
                    break;
                case 524288:
                    width = 1440;
                    height = 900;
                    frame_rate = 60;
                    break;
                case 1048576:
                    width = 1600;
                    height = 900;
                    frame_rate = 30;
                    break;
                case 2097152:
                    width = 1600;
                    height = 900;
                    frame_rate = 60;
                    break;
                case 4194304:
                    width = 1600;
                    height = 1200;
                    frame_rate = 30;
                    break;
                case 8388608:
                    width = 1600;
                    height = 1200;
                    frame_rate = 60;
                    break;
                case 16777216:
                    width = 1680;
                    height = 1024;
                    frame_rate = 30;
                    break;
                case 33554432:
                    width = 1680;
                    height = 1024;
                    frame_rate = 60;
                    break;
                case 67108864:
                    width = 1680;
                    height = 1050;
                    frame_rate = 30;
                    break;
                case 134217728:
                    width = 1680;
                    height = 1050;
                    frame_rate = 60;
                    break;
                case 268435456:
                    width = 1920;
                    height = 1200;
                    frame_rate = 30;
                    break;
                case 536870912:
                    width = 1920;
                    height = 1200;
                    frame_rate = 60;
                    break;
                default:
                    Log.d(TAG, "Invalid VESA mode value !!");
                    break;
            }
            Log.d(TAG, "Printing final negotiated resolution and frame rate : width = " +width + " height = " + height + " Frame rate = " + frame_rate + "fps");
        }
        else if(hh_mode !=0)  //hh mode
        {
            switch (hh_mode)
            {
                case 1:
                    width = 800;
                    height = 480;
                    frame_rate = 30;
                    break;
                case 2:
                    width = 800;
                    height = 480;
                    frame_rate = 60;
                    break;
                case 4:
                    width = 854;
                    height = 480;
                    frame_rate = 30;
                    break;
                case 8:
                    width = 854;
                    height = 480;
                    frame_rate = 60;
                    break;
                case 16:
                    width = 864;
                    height = 480;
                    frame_rate = 30;
                    break;
                case 32:
                    width = 864;
                    height = 480;
                    frame_rate = 60;
                    break;
                case 64:
                    width = 640;
                    height = 360;
                    frame_rate = 30;
                    break;
                case 128:
                    width = 640;
                    height = 360;
                    frame_rate = 60;
                    break;
                case 256:
                    width = 960;
                    height = 540;
                    frame_rate = 30;
                    break;
                case 512:
                    width = 960;
                    height = 540;
                    frame_rate = 60;
                    break;
                case 1024:
                    width = 848;
                    height = 480;
                    frame_rate = 30;
                    break;
                case 2048:
                    width = 848;
                    height = 480;
                    frame_rate = 60;
                    break;
                default:
                    Log.d(TAG, "Invalid HH mode value !!");
                    break;
            }
            Log.d(TAG, "Printing final negotiated resolution and frame rate : width = " +width + " height = " + height + " Frame rate = " + frame_rate + "fps");
        }
    }

    public IWfdSession getCurrentSession() {
        if (mSessionStack.empty()) {
            return null;
        } else {
            return mSessionStack.peek();
        }
    }

    private IWfdSession createNewSessionInstance() {
        Log.v(TAG, "createNewSessionInstance");
        try {
            IWfdSession session = IWfdSession.Stub.asInterface(ServiceUtil.getInstance().getWiFiDisplaySession());
            //set local device type
            int setDeviceTypeRet = session.setDeviceType(whisperer.getLocalDeviceType().getCode());
            if (setDeviceTypeRet == 0) {
                Log.d(TAG, "mWfdService.setDeviceType called.");
            } else {
                Log.d(TAG, "mWfdService.setDeviceType failed with error code: "
                        + setDeviceTypeRet);
            }
            return session;
        } catch (RemoteException e) {
            Log.e(TAG, "WfdService init() failed", e);
            return null;
        }
    }


}
