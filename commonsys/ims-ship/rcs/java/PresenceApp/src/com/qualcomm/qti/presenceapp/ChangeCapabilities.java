/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.content.Context;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link ChangeCapabilities#newInstance} factory method to
 * create an instance of this fragment.
 */
public class ChangeCapabilities extends Fragment {

    private CapabilityInfo mCapabilities;
    private Context mCtx;
    private int mTabInstance;


    public ChangeCapabilities(CapabilityInfo capabilities, Context ctx, int instance) {
        mCapabilities = capabilities;
        mCtx = ctx;
        mTabInstance = instance;
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_change_capabilities, container, false);
    }
}