/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.connmgr;

import android.view.View;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

public class ActivityOnClickBtnListener implements android.view.View.OnClickListener{
    ConstraintLayout activityResponse;
    TextView responseData;
    public ActivityOnClickBtnListener(ConstraintLayout c, TextView data) {
        activityResponse = c;
        responseData = data;
    }
    @Override
    public void onClick(View v) {
        int visible = (activityResponse.getVisibility() == View.VISIBLE) ?
                      View.GONE : View.VISIBLE;
        activityResponse.setVisibility(visible);
        if(responseData != null) {
            responseData.setText("This is a Response\n Sample Messge \n\n" +
                    "\n\n\n\n\n\n\n\n\n\n\nSUCCESS");
        }
    }
}
