/**
 * Copyright (c)2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.connmgr;

import android.annotation.SuppressLint;
import android.content.Context;

import androidx.annotation.Nullable;
import androidx.annotation.StringRes;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;

import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import java.util.List;

/**
 * A [FragmentPagerAdapter] that returns a fragment corresponding to
 * one of the sections/tabs/pages.
 */
public class SectionsPagerAdapter extends FragmentPagerAdapter {

    /*@StringRes
    private static final int[] TAB_TITLES =
    new int[]{R.string.tab_text_1, R.string.tab_text_2};
    */

    private int MaxTabsSupported = 1;
    public final static int SUB_PRIMARY = 0;
    public final static  int SUB_SECOND = 1;
    public final static  int SUB_INVALID = -1;
    public final static  int MAX_SUB_SUPPORTED = 2;
    public SubscriptionFragment[] mSubsTabs;
    final String LOG_TAG = "SectionsPagerAdapter:";

    private final Context mContext = null;

    public SectionsPagerAdapter(FragmentManager fm, Context context) {
        super(fm);
        SubscriptionManager sm = (SubscriptionManager)
          context.getSystemService(Context.TELEPHONY_SUBSCRIPTION_SERVICE);
        @SuppressLint("MissingPermission") List<SubscriptionInfo> subs =
          sm.getActiveSubscriptionInfoList();
        Log.i(LOG_TAG, "MaxTabsSupported = "+ MaxTabsSupported);
        MaxTabsSupported = subs.size();
        if(MaxTabsSupported > MAX_SUB_SUPPORTED) {
            Log.e(LOG_TAG, "Unsupported number of subscriptions");
        } else if(MaxTabsSupported ==0) {
            Log.e(LOG_TAG, "Executing in Emulator Scenario");
            MaxTabsSupported = 2;
            mSubsTabs = new SubscriptionFragment[MaxTabsSupported];
            mSubsTabs[0] = new SubscriptionFragment(
                SUB_PRIMARY,
                SUB_PRIMARY,
                "iccid1");
            mSubsTabs[1] = new SubscriptionFragment(
                SUB_SECOND,
                SUB_PRIMARY,
                "iccid2");
        } else {
            mSubsTabs = new SubscriptionFragment[MaxTabsSupported];
            for (int i = 0; i < MaxTabsSupported && i <= SUB_SECOND; i++) {
                mSubsTabs[i] = new SubscriptionFragment(
                    i,
                    subs.get(i).getSubscriptionId(),
                    subs.get(i).getIccId());
            }
        }
        //mContext = context;
    }

    @Override
    public Fragment getItem(int position) {
        if(position < 0 || position > MaxTabsSupported) {
            Log.i(LOG_TAG,":getItem returning NULL for= "+ position);
            return  null;
        }
        return mSubsTabs[position];
    }

    @Nullable
    @Override
    public CharSequence getPageTitle(int position) {
        return "Sub-" + position;
    }

    @Override
    public int getCount() {
        return MaxTabsSupported;
    }
}
