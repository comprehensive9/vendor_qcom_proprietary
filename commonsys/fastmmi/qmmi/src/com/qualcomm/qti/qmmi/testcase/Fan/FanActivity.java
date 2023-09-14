/*
 * Copyright (c) 2021, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Fan;

import android.content.Intent;
import android.os.Bundle;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.framework.BaseActivity;

public class FanActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.case_act;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    }
}
