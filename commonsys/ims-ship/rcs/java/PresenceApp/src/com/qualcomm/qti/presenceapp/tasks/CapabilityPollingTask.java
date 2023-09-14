/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp.tasks;

import android.content.Context;
import android.os.AsyncTask;
import android.view.View;
import android.widget.ProgressBar;

import com.qualcomm.qti.PresenceApp.MainActivity;
import com.qualcomm.qti.PresenceApp.R;
import com.qualcomm.qti.PresenceApp.SubsriptionTab;
import com.qualcomm.qti.PresenceApp.service.TaskListener;

public class CapabilityPollingTask  extends AsyncTask<Void, Integer, Integer> {
    Context mContext;
    String[] mUris;
    int mTabinstance;

    public CapabilityPollingTask(Context ctx, String[] uris, int instance) {
        mContext = ctx;
        mUris = uris;
        mTabinstance = instance;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
    }

    @Override
    protected void onPostExecute(Integer integer) {
        super.onPostExecute(integer);
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
        super.onProgressUpdate(values);
    }

    @Override
    protected Integer doInBackground(Void... voids) {
        MainActivity.mServiceWrapper.capabilityPolling(mTabinstance, mUris, new TaskListener() {
            @Override
            public void onNotifyListReceived() {
                SubsriptionTab tab = (SubsriptionTab)MainActivity.mPageAdapter.getSubscriptionTabForSlot(mTabinstance);
                tab.getAdapter().notifyDataSetChanged();
            }
        });
        return null;
    }
}
