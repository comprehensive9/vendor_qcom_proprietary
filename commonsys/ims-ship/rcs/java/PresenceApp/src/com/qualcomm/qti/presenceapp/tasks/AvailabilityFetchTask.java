/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp.tasks;

import android.content.Context;
import android.os.AsyncTask;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.qualcomm.qti.PresenceApp.MainActivity;
import com.qualcomm.qti.PresenceApp.R;
import com.qualcomm.qti.PresenceApp.service.TaskListener;

public class AvailabilityFetchTask  extends AsyncTask<Void, Integer, Integer> {
    ProgressBar mProgressBar;
    int mTabInstance;
    String mUri;
    TaskListener mListener;
    boolean isOptions = false;

    public AvailabilityFetchTask(int instance, String Uri, TaskListener listener, ProgressBar progressBar) {
        mProgressBar = progressBar;
        mTabInstance = instance;
        mUri = Uri;
        mListener = listener;
    }
    public void isOptions(boolean x) {
        isOptions = x;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        mProgressBar.setVisibility(View.VISIBLE);
        mProgressBar.setMin(10);
        mProgressBar.setMax(100);
        mProgressBar.setProgress(100, true);
    }

    @Override
    protected void onPostExecute(Integer integer) {
        super.onPostExecute(integer);
        mProgressBar.setProgress(0);
        mProgressBar.setVisibility(View.INVISIBLE);
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
        super.onProgressUpdate(values);
        //mProgressBar.setProgress(values[0]);
    }

    @Override
    protected Integer doInBackground(Void... voids) {
        if(isOptions) {
            MainActivity.mServiceWrapper.sendOptionsRequest(mTabInstance, mUri, mListener);
        } else {
            MainActivity.mServiceWrapper.availabilityFetch(mTabInstance, mUri, mListener);
        }
        return 100;
    }
}
