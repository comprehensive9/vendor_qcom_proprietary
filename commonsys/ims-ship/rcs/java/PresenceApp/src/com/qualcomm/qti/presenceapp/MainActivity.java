/**
 * Copyright (c)2017, 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.viewpager.widget.ViewPager;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.KeyEvent;

import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.google.android.material.tabs.TabLayout;
import com.qualcomm.qti.PresenceApp.service.UceService;
import android.provider.ContactsContract;
import android.telephony.TelephonyManager;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MainActivity extends AppCompatActivity {
    public static int MAX_SUBS_SUPPORTED = 1;
    public static Map<String, Contact> contacts = new HashMap<String, Contact>();
    public static Map<String, Contact> phoneBookContacts = new HashMap<String, Contact>();
    public static Map<Integer, CapabilityInfo> mCapabilities = new HashMap<Integer, CapabilityInfo>();
    public static Context mCtx = null;
    public static HandlerThread mThread = new HandlerThread("CallbackThread");
    public static UceService mServiceWrapper = new UceService();
    public static boolean UCE_SERVICE_ENABLED = false;
    final public static String LOG_TAG = "Presence_UI";
    public static PagerAdapter mPageAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mCtx = getApplicationContext();
        mThread.start();
        TabLayout tabLayout = findViewById(R.id.SubsTab);
//        TabItem tabSub0 = findViewById(R.id.Sub0);
//        TabItem tabSub1 = findViewById(R.id.Sub1);

        ConfigParser config = new ConfigParser();
        config.init(getApplicationContext());


        final ViewPager vPage = findViewById(R.id.viewPage);

        TelephonyManager tm = (TelephonyManager) mCtx.getSystemService(Context.TELEPHONY_SERVICE);
        MAX_SUBS_SUPPORTED = tm.getSupportedModemCount();

        mPageAdapter = new PagerAdapter(getSupportFragmentManager(), getApplicationContext());
        getContactsFromPhoneBook();
        //generateSelfContact();

        vPage.setAdapter(mPageAdapter);
        tabLayout.setupWithViewPager(vPage);
        tabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                vPage.setCurrentItem(tab.getPosition());
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {

            }

            @Override
            public void onTabReselected(TabLayout.Tab tab) {

            }
        });

        //register a Broadcast receiver with Android to receive UCE status from UceShimService
        BroadcastReceiver receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.d(LOG_TAG, "Received Intent: " + intent.getAction().toString());
                if ((intent.getAction().equals(ImsUceManager.ACTION_UCE_SERVICE_UP)) ||
                    (intent.getAction().equals(ImsUceManager.ACTION_UCE_SERVICE_DOWN))){

                  Bundle bundle = intent.getExtras();

                  for(int slotId = 0; slotId < 2; slotId++) {      
                      boolean isSlotActive = bundle.getBoolean("SLOT_"+slotId, false);
                      if(isSlotActive) {
                        Log.d(LOG_TAG, " Active slot: "+slotId);

                        if(mPageAdapter.containsSubscription(slotId))
                          continue;

                        SubsriptionTab subTab = (SubsriptionTab) mPageAdapter.addSubscriptionTabForSlot(slotId);
                        mPageAdapter.notifyDataSetChanged();

                        if(!(mCapabilities.containsKey(slotId)))
                        {
                            mCapabilities.put(slotId, new CapabilityInfo());
                            mCapabilities.get(slotId).enableAllFeatures();
                        }

                        if(subTab != null) {
                            subTab.bindToAidlService();
                        }

                      } else {
                        Log.d(LOG_TAG, " Inactive slot: "+slotId);
                        if(!(mPageAdapter.containsSubscription(slotId)))
                          continue;

                        SubsriptionTab subTab = (SubsriptionTab) mPageAdapter.removeSubscriptionTabForSlot(slotId);
                        mPageAdapter.notifyDataSetChanged();

                        if(subTab != null) {
                            subTab.releaseAidlService();
                        }
                        mCapabilities.remove(slotId);
                      }
                  }
               }
            }
        };

        //Register for UCE feature
        IntentFilter filter_uceUp = new IntentFilter();
        filter_uceUp.addAction(ImsUceManager.ACTION_UCE_SERVICE_UP);
        registerReceiver(receiver, filter_uceUp);
        //uce DOwn indication
        IntentFilter filter_uceDown = new IntentFilter();
        filter_uceDown.addAction(ImsUceManager.ACTION_UCE_SERVICE_DOWN);
        registerReceiver(receiver, filter_uceDown);

    }

    private void generateSelfContact() {
        TelephonyManager telMgr = (TelephonyManager)mCtx.getSystemService(Context.TELEPHONY_SERVICE);
        String phoneNumber = telMgr.getLine1Number();
        Log.d(LOG_TAG, "Gnani: phoneNumber:" + phoneNumber);
        contacts.put(phoneNumber, new Contact("Self", phoneNumber,
                0, "<Not Subscribed>",""));
    }
    private void getContactsFromPhoneBook() {
        Uri phoneBookContentUri = ContactsContract.Contacts.CONTENT_URI;
        String recordsWithPhoneNumberOnly = ContactsContract.Contacts.HAS_PHONE_NUMBER
                + "='1'";

        Cursor contactsCursor = getContentResolver().query(phoneBookContentUri, null,
                recordsWithPhoneNumberOnly, null, null);
        if(!contactsCursor.moveToFirst()) {
            //if no contacts
            return;
        }
        int count = contactsCursor.getCount();
        contactsCursor.moveToFirst();
        for(int i = 0; i<count; i++) {
            String Id = contactsCursor.getString(contactsCursor.getColumnIndex(ContactsContract.Contacts._ID));
            Cursor row = getContentResolver().query(ContactsContract.Data.CONTENT_URI,null,
                    ContactsContract.Data.CONTACT_ID + " = " + Id, null,null);

            int nameIdx = row.getColumnIndex(ContactsContract.Data.DISPLAY_NAME);
            int phoneIdx = row.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NORMALIZED_NUMBER);
            int rawPhoneIdx = row.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER);
            if(row.moveToFirst()) {
                String cName = row.getString(nameIdx);
                String cNumber = row.getString(phoneIdx);
                if(cNumber == null) {
                    String rawNum = row.getString(rawPhoneIdx);
                    cNumber = "";
                    for(int j=0; j<rawNum.length();j++) {
                        char c = rawNum.charAt(j);
                        //ensuring we only add number with pattern "+123.." or "1234...."
                        if((j == 0 && c == '+') || (Character.isDigit(c)))
                          cNumber = cNumber + c;
                    }
                }
                if(cNumber != null && cNumber.length() >0) {
                    Contact tempContact = new Contact(cName, cNumber,
                               0, "<Not Subscribed>","");
                    contacts.put(cNumber, tempContact);
                    phoneBookContacts.put(cNumber, tempContact);
                }
            }
            contactsCursor.moveToNext();
        }
//        contactsCursor.move(count)
//        do {
//            String Id = contactsCursor.getString(contactsCursor.getColumnIndex(ContactsContract.Contacts._ID));
//            Cursor row = getContentResolver().query(ContactsContract.Data.CONTENT_URI,null,
//                    ContactsContract.Data.CONTACT_ID + " = " + Id, null,null);
//
//            int nameIdx = row.getColumnIndex(ContactsContract.Data.DISPLAY_NAME);
//            int phoneIdx = row.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NORMALIZED_NUMBER);
//            if(row.moveToFirst()) {
//                String cName = row.getString(nameIdx);
//                String cNumber = row.getString(phoneIdx);
//                if(cNumber != null && cNumber.length() >0) {
//                    contacts.put(cNumber, new Contact(cName, cNumber,
//                               0, "<Not Subscribed>",""));
//                }
//            }
//        } while(contactsCursor.moveToNext());
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            moveTaskToBack(true);
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

}
