/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

public class SelfCapabilitiesActivity extends AppCompatActivity {
    CapabilityInfo mCapabilities;
    int mTabInstance;
    public final static String INSTANCE = "instance";
    final String LOG_TAG = MainActivity.LOG_TAG + ":SelfCapabilitiesActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_self_capabilities);

        Intent startupIntent = this.getIntent();
        mTabInstance = startupIntent.getIntExtra(INSTANCE, -1);
        mCapabilities = MainActivity.mCapabilities.get(mTabInstance);
        String[] stringArray = getResources().getStringArray(R.array.capabilities);
        CapabilityAdapter<String> adapter = new CapabilityAdapter<String>(getApplicationContext(),
                R.layout.fragment_change_capabilities, R.id.change_capList,
                Arrays.asList(stringArray));
        final ListView lv = (ListView)findViewById(R.id.change_capList);
        Button saveCapBtn = (Button) findViewById(R.id.save_capabilitiesBtn);
        Button selectAllBtn = (Button) findViewById(R.id.self_cap_select_all);
        Button selectBasicBtn = (Button) findViewById(R.id.self_cap_select_basic);
        lv.setAdapter(adapter);

        saveCapBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(MainActivity.mCapabilities.get(mTabInstance) != null)
                  MainActivity.mCapabilities.replace(mTabInstance, mCapabilities);
                finish();
            }
        });

        selectAllBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                int size = lv.getChildCount();
                for(int i=0; i<size;i++) {
                    View vw= lv.getChildAt(i);
                    TextView tv = (TextView)vw.findViewById(R.id.capabilityFeature);
                    Switch sw =vw.findViewById(R.id.capFeatureToglebtn);
                    sw.setChecked(true);
                }
            }
        });

        selectBasicBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                int size = lv.getChildCount();
                for(int i=0; i<size;i++) {
                    View vw= lv.getChildAt(i);
                    TextView tv = (TextView)vw.findViewById(R.id.capabilityFeature);
                    String label = (String) tv.getText();
                    Switch sw = vw.findViewById(R.id.capFeatureToglebtn);
                    if(label.equals("VT") ||
                       label.equals("Presence") ||
                       label.equals("VoLTE")) {
                        sw.setChecked(true);
                    }
                }
            }
        });
    }

    private class SwitchBtnListener implements CompoundButton.OnCheckedChangeListener {
        String mCapName;
        public  SwitchBtnListener(String CapName) {
            mCapName = CapName;
        }
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            mCapabilities.setCap(mCapName,isChecked);
        }
    }
    private class CapabilityAdapter<T> extends ArrayAdapter<T> {
        int mResource;
        Map<String,Boolean> caps = mCapabilities.getCapsInMap();
        List<String> capNames;

        public CapabilityAdapter(@NonNull Context context, int resource, int textViewResourceId,
                                 @NonNull List<T> objects) {
            super(context, resource, textViewResourceId, objects);
            capNames = ( List<String>)objects;
            mResource = resource;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            LayoutInflater inflater = LayoutInflater.from(getContext());
            convertView =inflater.inflate(mResource, parent, false);

            TextView tv = (TextView)convertView.findViewById(R.id.capabilityFeature);
            Switch sw =convertView.findViewById(R.id.capFeatureToglebtn);
            tv.setText(capNames.get(position));
            if((capNames.get(position)).equals("VT") ||
               (capNames.get(position)).equals("VOLTE")){
               Log.d(LOG_TAG,"getView: cap["+capNames.get(position)+"]");
               sw.setClickable(false);
            }
            else {
               sw.setChecked(caps.get(capNames.get(position)));
               sw.setOnCheckedChangeListener(new SwitchBtnListener(capNames.get(position)));
            }

            return convertView;
        }
    }
}
