/**
 * Copyright (c)2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.qualcomm.qti.PresenceApp.tasks.CapabilityPollingTask;
import com.qualcomm.qti.PresenceApp.tasks.PublishTask;

import java.util.ArrayList;
import java.util.Map;

public class SubsStatusAdapter extends RecyclerView.Adapter<SubsStatusAdapter.SubsViewHolder> {

    final private int MaxItemCount = 1;
    private SubsStatus subscriptionStatus = new SubsStatus();
    private ArrayAdapter<String> subsDetails;
    private int mTabInstance;
    private Context mCtx;
    private String mNumber = "";
    private ListView SubsStatusListView;
    final private static String LOG_TAG = MainActivity.LOG_TAG + ":SubsStatusAdapter: ";


    public SubsStatusAdapter(int instance) {
        mTabInstance = instance;
    }

    public void setContext(Context cxt) {
        mCtx = cxt;
    }

    public void setContactUri(String number) {
        mNumber = number;
    }

    @NonNull
    @Override
    public SubsViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View vw = LayoutInflater.from(parent.getContext()).inflate(R.layout.substatus_fragment,
                parent, false);
        return new SubsViewHolder(vw);
    }

    @Override
    public void onBindViewHolder(@NonNull SubsViewHolder holder, int position) {
        SubsStatusListView.setAdapter(subsDetails);
        boolean isExpanded = subscriptionStatus.isExpanded();
        holder.expandableLayout.setVisibility(isExpanded ? View.VISIBLE : View.GONE);
    }

    @Override
    public int getItemCount() {
        return MaxItemCount;
    }

    public void updateSubsData(SubsStatus subStatus) {
        subscriptionStatus = subStatus;
        if(mCtx == null) return;
        subsDetails = new ArrayAdapter<String>(mCtx,
                android.R.layout.simple_list_item_1, subscriptionStatus.getData());
        SubsStatusListView.setAdapter(subsDetails);
    }
    public SubsStatus getSubsStatus() {
        return subscriptionStatus;
    }

    class SubsViewHolder extends RecyclerView.ViewHolder {

        TextView SubsStatusBtn;
        //expandable_layout
        RelativeLayout expandableLayout;
        Button manualPublishBtn;
        Button capabilityPollingBtn;
        Button changeCapsBtn;

        public SubsViewHolder(@NonNull View itemView) {
            super(itemView);
            subsDetails = new ArrayAdapter<String>(itemView.getContext(),
                    android.R.layout.simple_list_item_1, subscriptionStatus.getData());
            SubsStatusBtn = (TextView) itemView.findViewById(R.id.SubsStatusBtn);
            SubsStatusListView = (ListView) itemView.findViewById(R.id.SubsStatusList);
            SubsStatusListView.setAdapter(subsDetails);

            expandableLayout = itemView.findViewById(R.id.expandable_layout);
            SubsStatusBtn.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    subscriptionStatus.setExpanded(!subscriptionStatus.isExpanded());
                    notifyItemChanged(getAdapterPosition());
                }
            });

            manualPublishBtn = itemView.findViewById(R.id.manual_publish);
            manualPublishBtn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    Log.i(LOG_TAG, "Manual Publish Clicked");
                    PublishTask task = new PublishTask(mCtx,mTabInstance);
                    task.execute();
                }
            });
            capabilityPollingBtn = itemView.findViewById(R.id.capabilityPolling);
            capabilityPollingBtn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    Log.i(LOG_TAG, "Capability Polling Clicked");

                    ArrayList<String> uri2 = new ArrayList<String>();

                    if(mNumber != null && !(mNumber.equals(""))) {
                        uri2.add(mNumber);
                    }

                    for(Map.Entry<String, Contact> e: MainActivity.phoneBookContacts.entrySet()) {
                        uri2.add(e.getValue().getUriString());
                    }
                    CapabilityPollingTask task = new CapabilityPollingTask(mCtx,
                            uri2.toArray(new String[uri2.size()]),
                            mTabInstance);
                    task.execute();
                }
            });

            changeCapsBtn = (Button) itemView.findViewById(R.id.changeCapabilitiesBtn);
            changeCapsBtn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent i = new Intent();
                    i.setClassName("com.qualcomm.qti.PresenceApp",
                            "com.qualcomm.qti.PresenceApp.SelfCapabilitiesActivity");
                    i.putExtra(SelfCapabilitiesActivity.INSTANCE, mTabInstance);
                    mCtx.startActivity(i);
                }
            });
        }
    }
}
