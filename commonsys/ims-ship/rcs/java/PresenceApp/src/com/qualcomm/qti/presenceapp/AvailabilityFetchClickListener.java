/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.content.Context;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.qualcomm.qti.PresenceApp.service.TaskListener;
import com.qualcomm.qti.PresenceApp.tasks.AvailabilityFetchTask;

import java.util.List;

public class AvailabilityFetchClickListener implements View.OnClickListener {

    private final Contact c;
    private final Context ctx;
    private final int mTabInstance;
    private final ListView mCapInfo;
    private final LinearLayout mCmdStatusView;
    private final ProgressBar progressBar;
    boolean isOptions = false;

    private class localTaskListener extends TaskListener {

        @Override
        public void onContactCapabilitesUpdated(String phoneNumber) {
            if(!c.phone.equals(phoneNumber)) {
                //LOGD("Phone Number does not Match");
                return;
            }
            progressBar.setProgress(100);
            Contact tempContact = MainActivity.contacts.get(phoneNumber);
            List<String> capabilities = tempContact.getCapabilities().getCapsInString();
            ArrayAdapter<String> adapter = new ArrayAdapter<String>(ctx,
                    android.R.layout.simple_list_item_1, capabilities);
            mCmdStatusView.setVisibility(View.VISIBLE);
            TextView notifyStatus = (TextView)mCmdStatusView.findViewById(R.id.notifyStatus);
            String data = (String) notifyStatus.getText();
            data = data + " success";
            notifyStatus.setText(data);

            mCapInfo.setAdapter(adapter);
            SubsriptionTab tab = (SubsriptionTab)MainActivity.mPageAdapter.getSubscriptionTabForSlot(mTabInstance);
            tab.getAdapter().notifyDataSetChanged();
        }

        @Override
        public void onSipResponse(int Reasoncode, String reason) {
            progressBar.setProgress(75);
            TextView subscribeStatus = (TextView)mCmdStatusView.findViewById(R.id.SubscribeStatus);
            String data = (String) subscribeStatus.getText();
            data = data + " ReasonCode[" + Reasoncode +"], reason["+reason+"]";
            subscribeStatus.setText(data);
        }

        @Override
        public void onNotifyListReceived() {
            SubsriptionTab tab = (SubsriptionTab)MainActivity.mPageAdapter.getSubscriptionTabForSlot(mTabInstance);
            tab.getAdapter().notifyDataSetChanged();
        }

        @Override
        public void onCommandStatus(String Status, int Id) {
            mCmdStatusView.setVisibility(View.VISIBLE);
            TextView cmdStatusData = (TextView)mCmdStatusView.findViewById(R.id.CmdStatusView);
            String data = (String) cmdStatusData.getText();
            data = data + " StatusCode[" + Status +"], ReqId["+Id+"]";
            cmdStatusData.setText(data);
            progressBar.setProgress(25);
        }
    }
    TaskListener mListener = new localTaskListener();

    public AvailabilityFetchClickListener(Contact c, Context ctx, int instance,
                                          ListView capInfo, LinearLayout cmdStatusView,
                                          ProgressBar progressBar, boolean isOptions) {
        this.c = c;
        this.ctx = ctx;
        mTabInstance = instance;
        mCapInfo = capInfo;
        mCmdStatusView = cmdStatusView;
        this.progressBar = progressBar;
        this.isOptions = isOptions;
    }
    @Override
    public void onClick(View v) {
        AvailabilityFetchTask availability = new AvailabilityFetchTask(mTabInstance,
                c.getUriString(),mListener,progressBar);
        availability.isOptions(isOptions);
        availability.execute();
    }
}
