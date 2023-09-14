/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.connmgr;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;

public class ActivitiesAdapter extends ArrayAdapter<String> {

    private static final String[] mActivities = new String[] {
            "Create CM Service",
            "Create Connection",
            "Trigger Registration",
            "Trigger Deregistration",
            "Trigger ACS Request",
    } ;
    int layoutRes = -1;
    public ActivitiesAdapter(@NonNull Context context, int resource) {
        super(context, resource, mActivities);
        layoutRes = resource;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LayoutInflater inflater = LayoutInflater.from(getContext());
        return convertView;
    }
}
