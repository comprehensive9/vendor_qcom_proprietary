/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.connmgr;

import android.content.Context;

import androidx.arch.core.util.Function;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Transformations;
import androidx.lifecycle.ViewModel;

public class PageViewModel extends ViewModel {

    private MutableLiveData<Integer> mIndex = new MutableLiveData<>();
    private LiveData<String> mText =
      Transformations.map(mIndex, new Function<Integer, String>() {
        @Override
        public String apply(Integer input) {
            return "Hello world from section: " + input;
        }
    });
    ActivitiesAdapter activities;


    public void setIndex(int index) {
        mIndex.setValue(index);
    }

    public ActivitiesAdapter getActivities(Context context) {
        activities = new ActivitiesAdapter(
            context,
            android.R.layout.simple_list_item_1);
        return activities;
    }

    public LiveData<String> getText() {
        return mText;
    }
}