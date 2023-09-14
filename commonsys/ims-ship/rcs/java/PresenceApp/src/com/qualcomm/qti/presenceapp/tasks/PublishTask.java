/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp.tasks;

import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.view.View;
import android.widget.ProgressBar;

import com.qualcomm.qti.PresenceApp.MainActivity;
import com.qualcomm.qti.PresenceApp.service.UceService;

public class PublishTask extends AsyncTask<Void, Integer, Integer> {
    Context mContext;
    //ProgressBar mProgressBar;
    int mTabInstance;

    public PublishTask(Context ctx, int instance) {
        mContext = ctx;
        //mProgressBar = progressBar;
        mTabInstance = instance;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
//        mProgressBar.setVisibility(View.VISIBLE);
//        mProgressBar.setMin(10);
//        mProgressBar.setMax(100);
//        mProgressBar.setProgress(100, true);

    }

    @Override
    protected void onPostExecute(Integer integer) {
        super.onPostExecute(integer);
        //mProgressBar.setVisibility(View.INVISIBLE);
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
        super.onProgressUpdate(values);
        //mProgressBar.setProgress(values[0]);
    }

    @Override
    protected Integer doInBackground(Void... voids) {

        if(MainActivity.mCapabilities.get(mTabInstance) == null)
            return -1;

        MainActivity.mServiceWrapper.publish(mTabInstance,
                MainActivity.mCapabilities.get(mTabInstance).getCapInfo());
        return -1;
    }
}
