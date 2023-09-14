/**
 * Copyright (c)2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.content.Context;
import android.content.Intent;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Bundle;

import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Handler;
import android.provider.Settings;
import android.provider.Telephony;
import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionInfo;
import android.telephony.ims.ImsMmTelManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import com.qualcomm.qti.PresenceApp.service.TaskListener;
import com.qualcomm.qti.PresenceApp.tasks.PublishTask;
import java.util.ArrayList;


/**
 * A simple {@link Fragment} subclass.
 */
public class SubsriptionTab extends Fragment {
    int mSub;
    String mIccid;
    int mTabInstance;
    String mNumber = "";
    private long fragmentID;
    private SubsStatusAdapter mSubscriptionStatusAdapter;
    private ContactArrayAdapter<Contact> mAdapter = null;
    final String LOG_TAG = MainActivity.LOG_TAG + ":SubsriptionTab";
    final static String IMS_CALL_COMPOSER = "qti.settings.call_composer";
    static final int MOBILE_DATA =1;
    static final int VOLTE_VT = 2;
    static final int CALL_COMPOSER = 3;
    private boolean mVoLTEenabled = true;
    private boolean mVTenabled = true;

    private class SettingsObserver extends ContentObserver {
        int mType = -1;
        public SettingsObserver(int Type) {
            super(new Handler());
            mType = Type;
        }
        public  void register() {
            if(mType == VOLTE_VT) {
                registerForVoLTEInd();
            } else if(mType == MOBILE_DATA) {
                registerMobileData();
            } else if(mType == CALL_COMPOSER) {
                registerForCallComposerToggle();
            }
        }
        public  void registerMobileData() {
            Uri dataUri= null;
            if(MainActivity.MAX_SUBS_SUPPORTED == 1)
              dataUri = Settings.Global.getUriFor(Settings.Global.MOBILE_DATA);
            else
              dataUri = Settings.Global.getUriFor(Settings.Global.MOBILE_DATA + mSub);

            Log.d(LOG_TAG,"SettingsObserver:registerMobileData["+mType+"]: Uri:" +dataUri.toString());
            MainActivity.mCtx.getContentResolver().
                    registerContentObserver(dataUri, false, this);
        }
        public  void registerForVoLTEInd() {
            Uri dataUri= null;
            dataUri = Uri.withAppendedPath(Telephony.SimInfo.CONTENT_URI,Settings.Global.ENHANCED_4G_MODE_ENABLED + mSub);

            Log.d(LOG_TAG,"SettingsObserver:registerForVoLTEInd["+mType+"]: Uri:" +dataUri.toString());
            MainActivity.mCtx.getContentResolver().
                    registerContentObserver(dataUri, false, this);
        }

        public  void registerForCallComposerToggle() {
            Uri dataUri= null;
            dataUri = Settings.Global.getUriFor(IMS_CALL_COMPOSER + mTabInstance);
            Log.d(LOG_TAG,"SettingsObserver:registerForCallComposerToggle["+mType+"]: Uri:" +dataUri.toString());
            MainActivity.mCtx.getContentResolver().
                    registerContentObserver(dataUri, false, this);
        }
        public void unRegister() {
            MainActivity.mCtx.getContentResolver().unregisterContentObserver(this);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            Log.d(LOG_TAG,"SettingsObserver:onChange["+mType+"]: Uri:[" +uri.toString()
              + "]selfChange:"+ selfChange);
            if(mType == VOLTE_VT) {
                VolteVTStatusChanged();
            } else if(mType == MOBILE_DATA) {
                mobileDataChanged();
            } else if(mType == CALL_COMPOSER) {
                callComposerToggled();
            }
        }

        private void  callComposerToggled() {
            Log.i(LOG_TAG, "callComposerToggled: indication Received");
            try {

                int enabled = Settings.Global.getInt(MainActivity.mCtx.getContentResolver(),
                IMS_CALL_COMPOSER + mTabInstance);

                if(MainActivity.mCapabilities.get(mTabInstance) == null)
                    return;

                MainActivity.mCapabilities.get(mTabInstance).setCallComposerStatus(enabled == 1);
                PublishTask task = new PublishTask(MainActivity.mCtx, mTabInstance);
                task.execute();
            }catch (Exception e) {
                Log.e(LOG_TAG, "callComposerToggled:onChange Exception:");
            }
        }
        private void  mobileDataChanged() {
            try {
                int enabled = 1;
                if(MainActivity.MAX_SUBS_SUPPORTED == 1) {
                  enabled = Settings.Global.getInt(MainActivity.mCtx.getContentResolver(),
                            Settings.Global.MOBILE_DATA);
                } else {
                  enabled = Settings.Global.getInt(MainActivity.mCtx.getContentResolver(),
                            Settings.Global.MOBILE_DATA + mSub);
                }
                if(MainActivity.mCapabilities.get(mTabInstance) == null)
                    return;

                PublishTask task = new PublishTask(MainActivity.mCtx, mTabInstance);
                task.execute();
            }catch (Exception e) {
                Log.e(LOG_TAG, "mobileDataChanged:onChange Exception:");
            }
        }
        private void  VolteVTStatusChanged() {
            boolean sendPublish = false;
            try {
                ImsMmTelManager mmTelManager = ImsMmTelManager.createForSubscriptionId(mSub);
                boolean VolteEnabled = mmTelManager.isAdvancedCallingSettingEnabled();
                Log.e(LOG_TAG, "VolteVTStatusChanged:VoLTE enabled:"+ VolteEnabled);

                if(MainActivity.mCapabilities.get(mTabInstance) == null)
                    return;

                MainActivity.mCapabilities.get(mTabInstance).setVolteStatus(VolteEnabled);
                // Now check for VT
                boolean VTenabled = mmTelManager.isVtSettingEnabled();
                Log.e(LOG_TAG, "VolteVTStatusChanged: VT enabled:"+ VTenabled);
                MainActivity.mCapabilities.get(mTabInstance).setVtStatus(VTenabled);

                // we will send publish on when there is a real change in VT/VoLTE
                if(mVoLTEenabled != VolteEnabled || mVTenabled != VTenabled)  {
                    sendPublish = true;
                    mVoLTEenabled = VolteEnabled;
                    mVTenabled = VTenabled;
                    PublishTask task = new PublishTask(MainActivity.mCtx, mTabInstance);
                    task.execute();
                }
            }catch (Exception e) {
                Log.e(LOG_TAG, "VolteStatusChanged:onChange Exception:");
            }
        }
    }
    SettingsObserver mobileDataObserver = new SettingsObserver(MOBILE_DATA);
    SettingsObserver voLTEVTObserver = new SettingsObserver(VOLTE_VT);
    SettingsObserver callComposerObserver = new SettingsObserver(CALL_COMPOSER);

    public ContactArrayAdapter<Contact> getAdapter() {
        return mAdapter;
    }

    private void getSelfContact() {
        Context context = getContext();
        SubscriptionInfo subInfo = null;

        if(context == null) return;

        SubscriptionManager subMgr =  SubscriptionManager.from(context);
        if(subMgr != null) {
            subInfo = subMgr.getActiveSubscriptionInfoForSimSlotIndex(mTabInstance);
        }

        if(subInfo != null) {
            mNumber = subInfo.getNumber();
        }
    }

    private class PublishListener extends TaskListener {
        @Override
        public void onCommandStatus(String Status, int Id) {
            Log.d(LOG_TAG, "PublishTaskListener: onCommandStatus : " + Status + " for tabInstance : " + mTabInstance);
            SubsStatus subStatus = mSubscriptionStatusAdapter.getSubsStatus();
            subStatus.setData(SubsStatus.publishCmdStatus, Status);
            mSubscriptionStatusAdapter.updateSubsData(subStatus);
        }

        @Override
        public void onSipResponse(int Reasoncode, String reason) {
            Log.d(LOG_TAG, "PublishTaskListener: onSipResponse : " + Reasoncode);
            SubsStatus subStatus = mSubscriptionStatusAdapter.getSubsStatus();
            subStatus.setData(SubsStatus.publishResponse, (Integer.toString(Reasoncode) + " " + reason));
            mSubscriptionStatusAdapter.updateSubsData(subStatus);
        }

        @Override
        public void onPublishTrigger(String type) {
            Log.d(LOG_TAG, "PublishTaskListener: onPublishTrigger : " + type);
            SubsStatus subStatus = mSubscriptionStatusAdapter.getSubsStatus();
            subStatus.setData(SubsStatus.publishTrigger, type);
            mSubscriptionStatusAdapter.updateSubsData(subStatus);
        }

        @Override
        public void onRegistrationChange(String x) {
            Log.d(LOG_TAG, "PublishTaskListener: Registration Status: " + x);
            SubsStatus subStatus = mSubscriptionStatusAdapter.getSubsStatus();
            subStatus.setData(SubsStatus.imsRegistration, x);
            mSubscriptionStatusAdapter.updateSubsData(subStatus);
        }
    };
    private PublishListener mTaskListener = new PublishListener();
    public SubsriptionTab(int index, int Sub, String iccid) {
        // Required empty public constructor
        Log.d(LOG_TAG, "Sub[" + mSub+"], iccid[" + iccid +"], tabInstance[" + index +"]" );
        mSub = Sub;
        mIccid = iccid;
        mTabInstance = index;
        Thread t = new Thread() {
            @Override
            public void run() {
                super.run();
                mobileDataObserver.register();
                voLTEVTObserver.register();
                callComposerObserver.register();
                if(mSubscriptionStatusAdapter == null) {
                  getSelfContact();
                  mSubscriptionStatusAdapter = new SubsStatusAdapter(mTabInstance);
                  mSubscriptionStatusAdapter.setContactUri(mNumber);
                }
            }
        };
        t.start();
    }

    private void getVtVoLTEStatus() {
        ImsMmTelManager mmTelManager = ImsMmTelManager.createForSubscriptionId(mSub);
        mVoLTEenabled = mmTelManager.isAdvancedCallingSettingEnabled();
        Log.e(LOG_TAG, "getVtVoLTEStatus:VoLTE enabled:"+ mVoLTEenabled);

        if(MainActivity.mCapabilities.get(mTabInstance) == null)
            return;

        MainActivity.mCapabilities.get(mTabInstance).enableDisableFeatures(mVoLTEenabled);
        // Now check for VT
        mVTenabled = mmTelManager.isVtSettingEnabled();
        Log.e(LOG_TAG, "getVtVoLTEStatus: VT enabled:"+ mVTenabled);
        MainActivity.mCapabilities.get(mTabInstance).setVtStatus(mVTenabled);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View vw = (View) inflater.inflate(R.layout.fragment_subsription_tab, container, false);
        getSelfContact();
        if(mSubscriptionStatusAdapter == null)
           mSubscriptionStatusAdapter = new SubsStatusAdapter(mTabInstance);
        mSubscriptionStatusAdapter.setContext(getContext());
        mSubscriptionStatusAdapter.setContactUri(mNumber);
        RecyclerView recyclerView = vw.findViewById(R.id.recycleView);
        recyclerView.setAdapter(mSubscriptionStatusAdapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(getActivity()));

        ArrayList<Contact> contactsList = new ArrayList<Contact>(
                                        MainActivity.phoneBookContacts.values());

        if(mNumber != null && !(mNumber.equals("")) ) {
            Contact selfContact =  new Contact("Self", mNumber, 0,
                                                    "<Not Subscribed>","");
            contactsList.add(0,selfContact);

            if(!(MainActivity.contacts.containsKey(mNumber)))
                MainActivity.contacts.put(mNumber, selfContact);
        }

        if(mAdapter == null) {
            mAdapter = new ContactArrayAdapter(vw.getContext(),
                    R.layout.fragment_contact_list, R.id.ContactList,
                    contactsList);
        }
        ListView lv = (ListView)vw.findViewById(R.id.ContactList);
        lv.setAdapter(mAdapter);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
              @Override
              public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                  Log.d(LOG_TAG, "instance[" + mTabInstance+"]" );
                  Intent i = new Intent();
                  i.setClassName("com.qualcomm.qti.PresenceApp",
                          "com.qualcomm.qti.PresenceApp.ContactInfo");
                  TextView name = view.findViewById(R.id.name);
                  i.putExtra(ContactInfo.CONTACT_NAME, name.getText());
                  TextView phone = view.findViewById(R.id.phone);
                  i.putExtra(ContactInfo.CONTACT_PHONE, phone.getText());
                  //i.putExtra("Contact", MainActivity.contacts.get(position));
                  i.putExtra(ContactInfo.CONTACT_INDEX, position);
                  i.putExtra(ContactInfo.CONTACT_TAB_INSTANCE, mTabInstance);
                  startActivity(i);
              }
          });
        return vw;
    }
    public void bindToAidlService() {
        //Called from MainActivity
        getVtVoLTEStatus();
        MainActivity.mServiceWrapper.initialize(MainActivity.mCtx,  mTabInstance, mIccid,mTaskListener);
    }
    public void releaseAidlService() {
        //Called from MainActivity
        MainActivity.mServiceWrapper.release(mTabInstance);
    }
    public int getTabInstance() {
        return mTabInstance;
    }
    public void setId(long id) {fragmentID = id;}
    public long getFragmentId() {return fragmentID;}
}
