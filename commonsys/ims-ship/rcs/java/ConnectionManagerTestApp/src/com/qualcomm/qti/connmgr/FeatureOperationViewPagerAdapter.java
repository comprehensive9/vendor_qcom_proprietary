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
import android.widget.TextView;

import androidx.viewpager.widget.PagerAdapter;
import com.qualcomm.qti.connmgr.R;
import java.util.List;

public class FeatureOperationViewPagerAdapter  extends PagerAdapter {
    private Context mContext;
    private List<String> mListData;

    public FeatureOperationViewPagerAdapter(Context context,
                                    List<String> ftOperationsList) {
        mContext = context;
        mListData = ftOperationsList;
    }

    @Override
    public void destroyItem(ViewGroup container, int position, Object object) {
        container.removeView((View) object);
    }

    @Override
    public int getCount() {
        return mListData.size();
    }

    @Override
    public boolean isViewFromObject(View view, Object object) {
        return view == object;
    }

    @Override
    public Object instantiateItem(ViewGroup container, final int position) {
        LayoutInflater inflater = LayoutInflater.from(mContext);
        ViewGroup view = (ViewGroup) inflater.inflate(
                              R.layout.activity_feature_operation,
                              container,
                              false);
        container.addView(view);
        return view;
    }
}
