/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.connmgr;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.AsyncTask;
import android.os.IHwBinder;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.ListView;
import android.widget.Toast;
import android.widget.Button;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProviders;
import com.qualcomm.qti.connmgr.R;

import java.util.ArrayList;
import java.util.List;
import java.util.NoSuchElementException;

import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionInfo;

import com.qualcomm.qti.imscmservice.V2_2.IImsCmService;
import com.qualcomm.qti.imscmservice.V2_2.IImsCmServiceListener;
import com.qualcomm.qti.imscmservice.V2_0.IImsCMConnectionListener;
import com.qualcomm.qti.imscmservice.V2_0.IImsCMConnection;
import com.qualcomm.qti.imscmservice.V2_0.outgoingMessage;
import com.qualcomm.qti.imscmservice.V2_0.outgoingMessageKeys;
import com.qualcomm.qti.imscmservice.V2_0.messageType;
import com.qualcomm.qti.imscmservice.V2_0.sipProtocolType;
import com.qualcomm.qti.imscmservice.V2_0.userConfig;
import com.qualcomm.qti.imscmservice.V2_0.userConfigKeys;
import com.qualcomm.qti.imscmservice.V2_0.deviceConfig;
import com.qualcomm.qti.imscmservice.V2_2.deviceConfigKeys;
import com.qualcomm.qti.imscmservice.V2_0.configData;
import com.qualcomm.qti.imscmservice.V2_0.autoConfig;
import com.qualcomm.qti.imscmservice.V2_0.keyValuePairBufferType;
import com.qualcomm.qti.imscmservice.V2_0.keyValuePairStringType;
import com.qualcomm.qti.imscmservice.V2_0.methodResponseData;
import com.qualcomm.qti.imscmservice.V2_0.methodResponseDataKeys;
import com.qualcomm.qti.imscmservice.V2_0.ConfigType;


public class SubscriptionFragment extends Fragment {
    final String LOG_TAG = "CMTestApp:SubscriptionFragment";
    int mSub;
    String mIccid;
    int mTabInstance;
    private int status = 0;
    private static final String ARG_SECTION_NUMBER = "section_number";
    private PageViewModel pageViewModel;
    private ActivitiesAdapter activities;
    private Spinner spinner;
    private Spinner ftSpinner;
    private String itemSelected;
    private String ftSelected;
    private String regFtSelected;
    private AdapterView cmServiceAdapterView;
    static ArrayAdapter<String> statusAdapter;
    private ListView statusListView;
    int mUserData = 0;
    public static ConnectionManagerService  cmServiceObj= null;
    int mHalVersion = -1;
    private ConnectionManagerParser mCmParser = null;
    private long mDeathBinderCookie = 1011010;


    private IHwBinder.DeathRecipient mDeathRecipient = new IHwBinder.DeathRecipient() {
        @Override
        public void serviceDied(long cookie) {
            Log.d(LOG_TAG + mIccid, "ImsCmServiceImpl connMgr"+
                   "HIDL Service DIED");
            String status = "Connection Manager Status: SERVICE_DIED";
            MainActivity.globaldata[mTabInstance].connMgrStatusList.add(status);
            MainActivity.mSectionsPagerAdapter.mSubsTabs[mTabInstance].
                         statusAdapter.notifyDataSetChanged();
            if (cookie == mDeathBinderCookie) {
                MainActivity.globaldata[mTabInstance].connMgr.unlinkToDeath(mDeathRecipient);
                MainActivity.globaldata[mTabInstance].connMgr = null;
            }
        }
    };

    public SubscriptionFragment(int index, int Sub, String iccid) {
        Log.d(LOG_TAG, "Sub[" +
         mSub+"], iccid[" +
         iccid +"], tabInstance[" +
         index +"]" );
        mSub = Sub;
        mIccid = iccid;
        mTabInstance = index;
        if(index == 0) {
            mUserData = 6789;
        } else {
            mUserData = 9560;
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        pageViewModel = ViewModelProviders.of(this).get(PageViewModel.class);
        int index = 1;
        if (getArguments() != null) {
            index = getArguments().getInt(ARG_SECTION_NUMBER);
        }
        pageViewModel.setIndex(index);
        cmServiceObj = new ConnectionManagerService();
    }

    @Override
    public View onCreateView(
            @NonNull LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {

        final View root = inflater.inflate(
            R.layout.fragment_subscription, container, false);
        MainActivity.globaldata[mTabInstance].primaryScreenView = root;
        spinner = root.findViewById(R.id.spinner);
        spinner.setOnItemSelectedListener(
            new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(
                AdapterView<?> adapterView,
                View view, int i, long l) {
                cmServiceAdapterView = adapterView;
                itemSelected = adapterView.getSelectedItem().toString();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        Button cmSvcButton = root.findViewById(R.id.button);
        final ConstraintLayout layout = root.findViewById(
            R.id.constraintLayout);
        statusListView = root.findViewById(R.id.statusListView);
        statusAdapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_list_item_1,
                MainActivity.globaldata[mTabInstance].connMgrStatusList);
        statusListView.setAdapter(statusAdapter);
        cmSvcButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                switch(itemSelected) {
                    case "Create CM Service":
                        handleCreateConnectionManagerSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    case "Display List of FTs":
                        handleCreateConnectionFTListSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    case "Trigger Registration":
                        handleTriggerRegistrationSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    case "Get Configuration":
                        handleGetConfigurationSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    case "Trigger Deregistration":
                        handleTriggerDeRegistrationSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    case "Trigger ACS Request":
                        handleTriggerAcsSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    case "Method Response":
                        handleMethodResponseSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    case "Close CM Service":
                        handleCloseConnectionManagerSelection(
                            root,cmServiceAdapterView.getContext());
                        break;
                    default:
                        Toast.makeText(
                            cmServiceAdapterView.getContext(),
                            "Unsupported operation",
                            Toast.LENGTH_SHORT).show();
                        break;
                }
            }
        });
        createFTSpinnerElement(root);
        createRegisteredFTSpinnerElement(root);

        return root;
    }

    public void handleCreateConnectionManagerSelection(View v, Context ctx) {
        if(MainActivity.globaldata[mTabInstance].connMgr == null) {
        MainActivity.globaldata[mTabInstance].connMgr =
          cmServiceObj.getConnectionManagerService();
        if (MainActivity.globaldata[mTabInstance].connMgr != null) {
            MainActivity.globaldata[mTabInstance].connMgr.linkToDeath(mDeathRecipient,mDeathBinderCookie++);
            mHalVersion = MainActivity.globaldata[mTabInstance].
              connMgr.getHalVersion();
            ConnectionManagerListenerImpl connMgrListener =
              new ConnectionManagerListenerImpl(mTabInstance, v);
            MainActivity.globaldata[mTabInstance].mConnMgrListener = connMgrListener;
            MainActivity.globaldata[mTabInstance].userData = mUserData;
            boolean status =  MainActivity.globaldata[mTabInstance].
              connMgr.InitializeService(mIccid,
                                        connMgrListener,
                                        mUserData);
            if(status) {
                Log.d(LOG_TAG + mIccid, "ImsCmServiceImpl connMgr"+
                   "initialized: "+status);
            }
        }
        else {
            Log.d(LOG_TAG, "ERROR: ImsCmServiceImpl.InitializeService called"+
              " | V2_2, V2_1, V2_0 are null, return");
              Toast.makeText(
                  cmServiceAdapterView.getContext(),
                  "Create Connection Manager:\n V2_2, V2_1, V2_0 are null",
                  Toast.LENGTH_SHORT).show();
            return;
        }
        } else {
            Log.d(LOG_TAG, "ERROR: ImsCmServiceImpl.InitializeService"+
              "Already cm service exists");
            Toast.makeText(cmServiceAdapterView.getContext(),
                             "Already Connection Manager Service exists",
                             Toast.LENGTH_SHORT).show();
            return;
        }
    }

    public void handleCreateConnectionFTListSelection(View v, Context ctx) {
        ArrayList<String> ftSpinnerList = new ArrayList<String>();
        mCmParser = ConnectionManagerParser.getInstance(ctx);
        mCmParser.startParseFeatureTag(mTabInstance);
        int numofFts = mCmParser.ftList.size();
        Log.d(LOG_TAG,"ftlist size is" + numofFts);
        if(numofFts > 0){
          if(MainActivity.globaldata[mTabInstance].
           connectionSpinnerList.size() == 0){
           MainActivity.globaldata[mTabInstance].
            connectionSpinnerList.addAll(mCmParser.ftList);
           Log.d(LOG_TAG, "handleCreateConnectionFTListSelection:"+
             "Spinner 2 size:[" +
              MainActivity.globaldata[mTabInstance].
              connectionSpinnerList.size() +
              "]");
           Spinner ftSpinner = v.findViewById(R.id.spinner2);
           ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_spinner_item,
                MainActivity.globaldata[mTabInstance].connectionSpinnerList);
           ftSpinner.setAdapter(adapter);
           adapter.notifyDataSetChanged();
           LinearLayout ftLinearLayout = v.findViewById(R.id.FeatureTagLayout);
           ftLinearLayout.setVisibility(View.VISIBLE);
           } else{
            Toast.makeText(cmServiceAdapterView.getContext(),
                           "Already FT List is populated below",
                            Toast.LENGTH_SHORT).show();
           }
        } else{
           Toast.makeText(cmServiceAdapterView.getContext(),
                           "Please add FT in the config file to parse",
                            Toast.LENGTH_SHORT).show();
        }
    }

    private void handleCreateConnection(View v, String featureTag) {
        //Check whether Connection fot FT already exists
        Log.d(LOG_TAG, "createConnection : ftSelected "+ featureTag);
        boolean isFTValid = MainActivity.globaldata[mTabInstance].
                                connectionSpinnerList.contains(featureTag);
        Log.d(LOG_TAG, "Before create Connection connectionMapSize :"+
          MainActivity.globaldata[mTabInstance].connectionMap.size());
        Log.d(LOG_TAG,"LIST: "+MainActivity.globaldata[mTabInstance].
        connectionSpinnerList.toString());
        Log.d(LOG_TAG, "FT validity status: "+isFTValid);
        if (isFTValid && (MainActivity.globaldata[mTabInstance].
            connectionMap.size() < 5)) {
            String ftString = MainActivity.globaldata[mTabInstance].
              parsedFeatureTagMap.get(featureTag);
            Log.d(LOG_TAG, "createConnection FT: " + ftString);
            MainActivity.globaldata[mTabInstance].featureTagStatusMap.
              put(featureTag, false);
                CMConnection conn =  MainActivity.globaldata[mTabInstance].
                  connMgr.createConnection(
                    MainActivity.globaldata[mTabInstance].serviceHandle,
                    mTabInstance,
                    featureTag,
                    ftString,
                    v);
                if (conn != null) {
                    MainActivity.globaldata[mTabInstance].
                      connectionMap.put(featureTag, conn);
                Log.d(LOG_TAG, "After create Connection connectionMapSize :"+
                MainActivity.globaldata[mTabInstance].connectionMap.size());
                }else{
                    Log.d(LOG_TAG, "ERROR: ImsCmServiceImpl.createConnection"+
                    " null return");
            }
        }else{
            Toast.makeText(
                cmServiceAdapterView.getContext(),
                "Only Maximum of 5 Active Connections are allowed",
                Toast.LENGTH_SHORT).show();
            }
        }

    public void handleTriggerRegistrationSelection(View v, Context ctx) {
        if(MainActivity.globaldata[mTabInstance].connMgrInitialised == 1) {
            Log.d(LOG_TAG, "TriggerRegistration: iccid:" + mIccid);
            if (MainActivity.globaldata[mTabInstance].connMgr != null) {
                boolean status = MainActivity.globaldata[mTabInstance].
                  connMgr.triggerRegistration(
                        MainActivity.globaldata[mTabInstance].serviceHandle,
                        mUserData);

                Log.d(LOG_TAG + mIccid, "Trigger Registration status: "+
                      status);
            } else {
                Log.d(LOG_TAG, "ERROR: connMgr | V2_2, V2_1, V2_0"+
                " are null, return");
                return;
            }
            }
        else {
            Log.d(LOG_TAG, "ERROR: connMgr not initialized successfully");
        }
    }

    public void handleGetConfigurationSelection(View v, Context ctx) {
        if (MainActivity.globaldata[mTabInstance].connMgrInitialised == 1) {
          Log.d(LOG_TAG, "Get Configuration: iccid:" + mIccid);
          if (MainActivity.globaldata[mTabInstance].connMgr != null) {
              ConfigurationTask task = new
                   ConfigurationTask(MainActivity.mCtx, mTabInstance);
              task.execute();
          }
          else {
            Log.d(LOG_TAG, "ERROR: connMgr | V2_2, V2_1, V2_0"+
                " are null, return");
            return;
          }
        }
        else {
          Log.d(LOG_TAG, "ERROR: connMgr not initialized successfully");
        }
    }

    public void handleTriggerDeRegistrationSelection(View v, Context ctx) {
        if (MainActivity.globaldata[mTabInstance].connMgrInitialised == 1) {
            Log.d(LOG_TAG, "TriggerDeRegistration: iccid:" + mIccid);
            if (MainActivity.globaldata[mTabInstance].connMgr != null) {
                if(MainActivity.globaldata[mTabInstance].
                  registeredConnectionSpinnerList.size() > 0){
                boolean status = MainActivity.globaldata[mTabInstance].
                  connMgr.triggerDeRegistration(
                        MainActivity.globaldata[mTabInstance].serviceHandle,
                        mUserData);
                    Log.d(LOG_TAG + mIccid, "ImsCmServiceImpl Trigger "+
                      "DeRegistration status: " +
                          status);
                } else {
                    Toast.makeText(cmServiceAdapterView.getContext(),
                             "No registered FTs present",
                             Toast.LENGTH_SHORT).show();
                }

            } else {
                Log.d(LOG_TAG, "ERROR: connMgr | V2_2, V2_1, V2_0"+
                " are null, return");
                return;
            }
        } else {
            Log.d(LOG_TAG, "ERROR: connMgr not initialized successfully");
            }
    }

    public void handleTriggerAcsSelection(View v, Context ctx) {
        Log.d(LOG_TAG,"TriggerAcsSelection");
        if (MainActivity.globaldata[mTabInstance].connMgr!= null)
                {
            if (MainActivity.globaldata[mTabInstance].
                connMgrInitialised == 1) {
                Log.d(LOG_TAG, "Triggering Acs Request : iccid: " + mIccid);
                boolean status = MainActivity.globaldata[mTabInstance].
                   connMgr.triggerACSRequest(
                        MainActivity.globaldata[mTabInstance].serviceHandle,
                        MainActivity.globaldata[mTabInstance].acsTriggerReason,
                        mUserData);
                Log.d(LOG_TAG, "TriggerAcs :status: " + status);
                }
                else
                {
                Log.d(LOG_TAG, "connection manager is not initilised");
                }
            }
        else{
            Log.d(LOG_TAG, "TriggerAcs: connMgr is NULL");
        }
    }

    public void handleMethodResponseSelection(View v, Context ctx) {
        Log.d(LOG_TAG,"MethodResponseSelection");
        short rspCode = 404;
        if (MainActivity.globaldata[mTabInstance].connMgr!= null)
        {
            if (MainActivity.globaldata[mTabInstance].
                connMgrInitialised == 1) {
                methodResponseData data = new methodResponseData();
                keyValuePairStringType methodType = new
                  keyValuePairStringType();
                methodType.key = methodResponseDataKeys.method;
                methodType.value = "INVITE";
                data.data.add(methodType);

                keyValuePairStringType responseCodeType = new
                  keyValuePairStringType();
                responseCodeType.key = methodResponseDataKeys.responseCode;
                responseCodeType.value = Integer.toString(rspCode);
                data.data.add(responseCodeType);

                boolean status = MainActivity.globaldata[mTabInstance].
                  connMgr.methodResponse(
                        MainActivity.globaldata[mTabInstance].serviceHandle,
                        data,
                        mUserData);
                Log.d(LOG_TAG, "Methodresponse: Status: " + status);
            }
            else
            {
                Log.d(LOG_TAG, "Methodresponse: connection manager"+
                      " is not initilised");
            }
        }
        else{
            Log.d(LOG_TAG, "Methodresponse: connMgr is NULL");
            }
        }

    public void handleCloseConnectionManagerSelection(View v, Context ctx) {
        Log.d(LOG_TAG, "CloseConnectionManager: iccid:"+ mIccid);
        if (MainActivity.globaldata[mTabInstance].connMgr!= null){
            //Clear conection map
            MainActivity.globaldata[mTabInstance].connectionMap.clear();
            MainActivity.globaldata[mTabInstance].
              registeredConnectionSpinnerList.clear();
            LinearLayout regFtLinearLayout = v.findViewById(
                R.id.RegisteredTagLayout);
            regFtLinearLayout.setVisibility(View.GONE);
            MainActivity.globaldata[mTabInstance].
              connectionSpinnerList.clear();
            LinearLayout ftLinearLayout = v.findViewById(
                R.id.FeatureTagLayout);
            ftLinearLayout.setVisibility(View.GONE);
            ConnectionManagerListenerImpl cmListener =
                MainActivity.globaldata[mTabInstance].
                mConnMgrListener;
            if (cmListener != null ) {
              Log.d(LOG_TAG, "ImsCmServiceImpl removeListener"+
                    " called: iccid"+
                    mIccid);
              Log.d(LOG_TAG, "ImsCmServiceImpl.removeListener"+
                          cmListener);
              boolean status = MainActivity.globaldata[mTabInstance].
                   connMgr.removeListener(
                       MainActivity.globaldata[mTabInstance].serviceHandle,
                       cmListener.getListenerId());
              Log.d(LOG_TAG, "RemoveListener :status: " + status);
            }
            Log.d(LOG_TAG, "CloseConnectionManager called: iccid :"+
                  mIccid);
            boolean cm_status = MainActivity.globaldata[mTabInstance].
                connMgr.closeService(
                    MainActivity.globaldata[mTabInstance].serviceHandle);
            Log.d(LOG_TAG, "CloseConnectionManager :status: " +
              cm_status);
            MainActivity.globaldata[mTabInstance].connMgr = null;
        }
        else
        {
            Log.d(LOG_TAG, "CloseService connection Mgr is NULL: iccid :"+
                  mIccid);
        }
        Log.d(LOG_TAG, "CloseConnectionManagerButtonClick end: iccid"+
              mIccid);
        MainActivity.globaldata[mTabInstance].connMgrStatusList.clear();
        statusAdapter.notifyDataSetChanged();
    }

    private void createRegisteredFTSpinnerElement(View v){
        Log.d(LOG_TAG,"createRegisteredFTSpinnerElement");
        Log.d(LOG_TAG, "createRegisteredFtSpinnerElement: Spinner 3 size:[" +
              MainActivity.globaldata[mTabInstance].
              registeredConnectionSpinnerList.size() +
              "]");
        if(MainActivity.globaldata[mTabInstance].
           registeredConnectionSpinnerList.size() == 0 ){
            Log.d(LOG_TAG, "createRegisteredFtSpinnerElement: "+
            "Spinner list size is 0; gone");
            LinearLayout regFtLinearLayout = v.findViewById(
                R.id.RegisteredTagLayout);
            regFtLinearLayout.setVisibility(View.GONE);
        }
        Spinner ftSpinner = v.findViewById(R.id.spinner3);
        ArrayAdapter<String> regFtSpinnerAdapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_spinner_item,
                MainActivity.globaldata[mTabInstance].
                  registeredConnectionSpinnerList
        );
        ftSpinner.setAdapter(regFtSpinnerAdapter);

        ftSpinner.setOnItemSelectedListener(
            new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(
                AdapterView<?> adapterView,
                View view, int i, long l) {
                regFtSelected = adapterView.getSelectedItem().toString();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        Button ftButton = v.findViewById(R.id.button3);
        ftButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                handleLaunchFeatureOperationActivity(regFtSelected);
            }
        });
    }

    public void updateRegisteredFtSpinnerElement(View v){
        Log.d(LOG_TAG, "updateRegisteredFtSpinnerElement");
        Log.d(LOG_TAG, "updateRegisteredFtSpinnerElement: Spinner 3 size:[" +
              MainActivity.globaldata[mTabInstance].
              registeredConnectionSpinnerList.size() +
              "]");
        if(v != null){
            Spinner regFtSpinner = v.findViewById(R.id.spinner3);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_spinner_item,
                MainActivity.globaldata[mTabInstance].
                  registeredConnectionSpinnerList);
        regFtSpinner.setAdapter(adapter);
        adapter.notifyDataSetChanged();
            if(MainActivity.globaldata[mTabInstance].
              registeredConnectionSpinnerList.size()>0){
        LinearLayout ftLinearLayout = v.findViewById(R.id.RegisteredTagLayout);
        ftLinearLayout.setVisibility(View.VISIBLE);
            }else{
                Log.d(LOG_TAG, "updateRegisteredFtSpinnerElement: "+
                "Spinner size 0; gone");
                LinearLayout ftLinearLayout = v.findViewById(
                    R.id.RegisteredTagLayout);
                ftLinearLayout.setVisibility(View.GONE);
            }
        }else{
            Log.d(LOG_TAG, "updateRegisteredFtSpinnerElement:"+
              "Null view element passed");
        }
    }

    private void createFTSpinnerElement(View v){
        ftSpinner = v.findViewById(R.id.spinner2);
        ArrayAdapter<String> ftSpinnerAdapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_spinner_item,
                MainActivity.globaldata[mTabInstance].connectionSpinnerList);
        ftSpinner.setAdapter(ftSpinnerAdapter);
        ftSpinner.setOnItemSelectedListener(
            new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(
                AdapterView<?> adapterView,
                View view, int i, long l) {
                ftSelected = adapterView.getSelectedItem().toString();
                Log.d(LOG_TAG, "createFTSpinnerElement: ftSelected: "+
                ftSelected);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        Button ftButton = v.findViewById(R.id.button2);
        ftButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                handleCreateConnection(v, ftSelected);
            }
        });
    }

    public void updateFTSpinnerElement(View v){
        Log.d(LOG_TAG, "updateFTSpinnerElement");
        Spinner ftSpinner = v.findViewById(R.id.spinner2);
        Log.d(LOG_TAG, "updateFTSpinnerElement: Spinner 2 size:[" +
              MainActivity.globaldata[mTabInstance].
              connectionSpinnerList.size() +
              "]");
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_spinner_item,
                MainActivity.globaldata[mTabInstance].connectionSpinnerList
        );
        ftSpinner.setAdapter(adapter);
        adapter.notifyDataSetChanged();
    }

    private void handleLaunchFeatureOperationActivity(String ftTag){
        Intent i = new Intent();
        i.setClassName(
            "com.qualcomm.qti.connmgr",
            "com.qualcomm.qti.connmgr.FeatureOperationActivity");
        i.putExtra(FeatureOperationActivity.FEATURE_TAG_NAME, ftTag);
        i.putExtra(FeatureOperationActivity.TAB_INSTANCE, mTabInstance);
        startActivity(i);
    }

    private void setCmServiceStatus(int status){
        this.status = status;
    }

    private int getCmServiceStatus(){
        return this.status;
    }

    public void clearConnection(String tagName) {
        Log.d(LOG_TAG, "clearConnection for FT: " + tagName);
        MainActivity.globaldata[mTabInstance].connectionMap.remove(tagName);
    }

    private class ConfigurationTask extends AsyncTask<Void, Integer, Integer> {
        Context mContext;
        int mTabInstance;

        public ConfigurationTask(Context ctx, int instance) {
            mContext = ctx;
            mTabInstance = instance;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected void onPostExecute(Integer integer) {
            super.onPostExecute(integer);
        }

        @Override
        protected void onProgressUpdate(Integer... values) {
            super.onProgressUpdate(values);
        }

        @Override
        protected Integer doInBackground(Void... voids) {
            boolean status = MainActivity.globaldata[mTabInstance].
                    connMgr.getConfiguration(
                          MainActivity.globaldata[mTabInstance].serviceHandle,
                          ConfigType.USER_CONFIG,
                          mUserData);

            status = MainActivity.globaldata[mTabInstance].
                    connMgr.getConfiguration(
                          MainActivity.globaldata[mTabInstance].serviceHandle,
                          ConfigType.DEVICE_CONFIG,
                          mUserData);

            status = MainActivity.globaldata[mTabInstance].
                    connMgr.getConfiguration(
                          MainActivity.globaldata[mTabInstance].serviceHandle,
                          ConfigType.AUTO_CONFIG,
                          mUserData);

            return -1;
        }
    }
}
