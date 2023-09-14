/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.connmgr;

import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.tabs.TabLayout;

import androidx.viewpager.widget.ViewPager;
import androidx.appcompat.app.AppCompatActivity;

import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.Toast;
import android.util.Log;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;

import com.qualcomm.qti.connmgr.R;

public class MainActivity extends AppCompatActivity {

    public static int MAX_SUBS_SUPPORTED = 1;
    public static Context mCtx = null;
    public static HandlerThread mThread = new HandlerThread("CbThread");
    final public static String LOG_TAG = "CMTestApp_UI";
    public static SectionsPagerAdapter mSectionsPagerAdapter;
    public static ConnectionGlobalData[] globaldata;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mCtx = getApplicationContext();
        mThread.start();
        mSectionsPagerAdapter = new SectionsPagerAdapter(
                                 getSupportFragmentManager(),
                                 getApplicationContext());
        MAX_SUBS_SUPPORTED = mSectionsPagerAdapter.getCount();
        globaldata = new ConnectionGlobalData[MAX_SUBS_SUPPORTED];
        for (int i=0; i< MAX_SUBS_SUPPORTED; i++) {
            globaldata[i] = new ConnectionGlobalData(); // per sub
        }
        ViewPager viewPager = findViewById(R.id.view_pager);
        viewPager.setAdapter(mSectionsPagerAdapter);
        TabLayout tablayout = findViewById(R.id.tabs);
        tablayout.setupWithViewPager(viewPager);
        tablayout.addOnTabSelectedListener(
                    new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                viewPager.setCurrentItem(tab.getPosition());
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {

            }

            @Override
            public void onTabReselected(TabLayout.Tab tab) {

            }
        });
    }
}
