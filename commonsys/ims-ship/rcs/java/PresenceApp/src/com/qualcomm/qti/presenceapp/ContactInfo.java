/**
 * Copyright (c)2017,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.android.ims.internal.uce.common.CapInfo;

public class ContactInfo extends AppCompatActivity {

    final String LOG_TAG = MainActivity.LOG_TAG + ":ContactInfo";
    final static String CONTACT_NAME = "name";
    final static String CONTACT_PHONE = "phone";
    final static String CONTACT_INDEX = "index";
    final static String CONTACT_TAB_INSTANCE = "instance";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_contact_info);
        Intent startUpIntent = this.getIntent();
        String name = startUpIntent.getStringExtra(CONTACT_NAME);
        String phone = startUpIntent.getStringExtra(CONTACT_PHONE);
        TextView nameView = findViewById(R.id.name);
        nameView.setText(name);
        TextView phoneView = findViewById(R.id.phone);
        phoneView.setText(phone);
        int position = startUpIntent.getIntExtra(CONTACT_INDEX, -1);
        Contact c = MainActivity.contacts.get(phone);
        int instance = startUpIntent.getIntExtra(CONTACT_TAB_INSTANCE, -1);
        ListView lv = (ListView) findViewById(R.id.CapabilityInfo);
        LinearLayout cmdStatusView = (LinearLayout) findViewById(R.id.StatusView);
        ProgressBar bar = (ProgressBar)findViewById(R.id.contactInfoProgressBar);

        Log.d(LOG_TAG, "instance[" + instance +"]" );
        //set some sample data
        CapInfo contactCaps = new CapInfo();
        contactCaps.setIpVideoSupported(true);
        contactCaps.setIpVoiceSupported(true);
        contactCaps.setCdViaPresenceSupported(true);
        c.setCapabilities(new CapabilityInfo(contactCaps));
        String uri = c.getUriString();

        Button AvailabilityFetch = (Button)findViewById(R.id.AvailabilityFetch);
        AvailabilityFetch.setOnClickListener(
                new AvailabilityFetchClickListener(c, getApplicationContext(),
                        instance,lv, cmdStatusView, bar, false));

        Button OptionsFetch = (Button)findViewById((R.id.optionsBtn));
        OptionsFetch.setOnClickListener(new AvailabilityFetchClickListener(c,getApplicationContext(),
                           instance,lv,cmdStatusView,bar, true));

        Button done_btn = (Button)findViewById(R.id.Done);
        done_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
    }
}
