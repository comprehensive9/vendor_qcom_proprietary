/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.ViewGroup;
import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.fragment.app.FragmentTransaction;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.Set;

public class PagerAdapter extends FragmentPagerAdapter {
    private int MaxTabsSupported = 1;
    private Context mContext;
    private long baseId = 0;
    public final static int SUB_PRIMARY = 0;
    public final static  int SUB_SECOND = 1;
    public final static  int SUB_INVALID = -1;
    public final static int MAX_SUB_SUPPORTED = 2;
    TreeMap<Integer, SubsriptionTab> mSubsTabs = new TreeMap<Integer, SubsriptionTab>();
    final String LOG_TAG = MainActivity.LOG_TAG + ":PageAdapter:";

    @RequiresApi(api = Build.VERSION_CODES.Q)
    @SuppressLint("ServiceCast")
    public PagerAdapter(FragmentManager fm, Context ctx) {
        super(fm);
        mContext = ctx;
        SubscriptionManager sm = (SubscriptionManager) ctx.getSystemService(Context.TELEPHONY_SUBSCRIPTION_SERVICE);
        List<SubscriptionInfo> subs = sm.getActiveSubscriptionInfoList();
        MaxTabsSupported = subs.size();
        Log.i(LOG_TAG, "MaxTabsSupported = "+ MaxTabsSupported);

        if(MaxTabsSupported > MAX_SUB_SUPPORTED) {
            Log.e(LOG_TAG, "Unsupported number of subscriptions");
        } else if(MaxTabsSupported ==0) {
            Log.e(MainActivity.LOG_TAG, "Executing in Emulator Scenario");
            //THis is for emulator scenario only
            MaxTabsSupported = 2;
            mSubsTabs.put(SUB_PRIMARY, new SubsriptionTab(SUB_PRIMARY, SUB_PRIMARY, "iccid1"));
            mSubsTabs.put(SUB_SECOND, new SubsriptionTab(SUB_SECOND, SUB_PRIMARY, "iccid2"));
        }
    }
    @NonNull
    @Override
    public Fragment getItem(int position) {
        if(position < 0 || position >= mSubsTabs.size()){
            Log.i(MainActivity.LOG_TAG,"PagerAdapter::getItem returning NULL for= "+ position);
            return  null;
        }
        return mSubsTabs.get(mSubsTabs.keySet().toArray()[position]);
    }

    @Override
    public int getCount() {
        return mSubsTabs.size();
    }

    @Override
    public CharSequence getPageTitle(int position) {
        return "Sub-" + (mSubsTabs.get(mSubsTabs.keySet().toArray()[position])).getTabInstance();
    }

    @Override
    public void destroyItem(ViewGroup container, int position, Object object) {
      if(object != null && !(mSubsTabs.containsValue((SubsriptionTab) object))) {
          FragmentManager manager = ((Fragment) object).getFragmentManager();
          if(manager != null) {
              FragmentTransaction trans = manager.beginTransaction();
              if(trans != null) {
                 trans.remove((Fragment) object);
                 trans.commitAllowingStateLoss();
              }
          }
       }
    }

    @Override public int getItemPosition(Object itemIdentifier) {
      if(!(mSubsTabs.containsValue((SubsriptionTab) itemIdentifier)))
          return POSITION_NONE;

      Set<Map.Entry<Integer, SubsriptionTab>> entries = mSubsTabs.entrySet();
      for( Map.Entry<Integer, SubsriptionTab> entry : entries ){
          if(entry.getValue() == (SubsriptionTab) itemIdentifier)
             return entry.getKey();
      }
     return POSITION_NONE;
    }

   @Override
    public long getItemId(int position) {
        if(position < 0 || position >= mSubsTabs.size())
        return 0;

        // give an ID different from position when position has been changed
        return (mSubsTabs.get(mSubsTabs.keySet().toArray()[position])).getFragmentId();
    }

    public Fragment removeSubscriptionTabForSlot(int slotId)
    {
        if(slotId < 0 || slotId > MaxTabsSupported || !mSubsTabs.containsKey(slotId)){
            Log.i(MainActivity.LOG_TAG,"PagerAdapter::removeSubscriptionTabForSlot returning NULL for= "+ slotId);
            return  null;
        }

        Fragment subTab = mSubsTabs.get(slotId);
        mSubsTabs.remove(slotId);
        MaxTabsSupported = mSubsTabs.size();
        return subTab;
    }

    public Fragment addSubscriptionTabForSlot(int slotId)
    {
        if(mSubsTabs.containsKey(slotId)) {
            Log.i(MainActivity.LOG_TAG,"PagerAdapter::addSubscriptionTabForSlot Subscription already present for "+ slotId);
            return mSubsTabs.get(slotId);
        }
        SubscriptionManager subMgr =  SubscriptionManager.from(mContext);
        SubscriptionInfo subInfo = subMgr.getActiveSubscriptionInfoForSimSlotIndex(slotId);
        mSubsTabs.put(slotId, new SubsriptionTab(slotId, subInfo.getSubscriptionId(),
                                                 subInfo.getIccId()));
        MaxTabsSupported = mSubsTabs.size();
        mSubsTabs.get(slotId).setId(baseId++);
        return mSubsTabs.get(slotId);
    }

    public boolean containsSubscription(int slotId)
    {
         if(mSubsTabs.containsKey(slotId)) return true;

         return false;
    }

    public Fragment getSubscriptionTabForSlot(int slotId)
  {
        if(mSubsTabs.containsKey(slotId))
           return mSubsTabs.get(slotId);

        return null;
  }

}
