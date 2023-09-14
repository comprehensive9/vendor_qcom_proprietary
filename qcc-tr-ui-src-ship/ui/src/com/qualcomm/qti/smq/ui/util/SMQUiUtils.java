/* 
 * Copyright (c) 2014, 2017,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.smq.ui.util;

import android.os.SystemProperties;

import android.content.ComponentName;
import android.content.Context;
import android.content.pm.PackageManager;

import java.lang.reflect.Method;

public class SMQUiUtils {

    /**
     * @param c
     * @return
     */
    /**
     * Utility method to disable a component.
     * 
     * @param c
     *            the class of the component.
     * @param context
     *            the app context.
     * @return true if successful, false otherwise.
     */
    public static boolean disableComponent(final Class<?> c,
            final Context context) {
        final PackageManager pm = context.getPackageManager();
        boolean returnStatus = false;

        try {
            pm.setComponentEnabledSetting(new ComponentName(context, c),
                    PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                    PackageManager.DONT_KILL_APP);
            returnStatus = true;
        } catch (final IllegalArgumentException ie) {
			ie.printStackTrace();
        }

        return returnStatus;

    }

    /**
     * Utility methpod to enable a component.
     * 
     * @param c
     *            the class of the component.
     * @param context
     *            the app context.
     * @return true if successful, false otherwise.
     */
    public static boolean enableComponent(final Class<?> c,
            final Context context) {
        final PackageManager pm = context.getPackageManager();
        boolean returnStatus = false;


        try {
            pm.setComponentEnabledSetting(new ComponentName(context, c),
                    PackageManager.COMPONENT_ENABLED_STATE_ENABLED,
                    PackageManager.DONT_KILL_APP);
            returnStatus = true;

        } catch (final IllegalArgumentException ie) {
			ie.printStackTrace();
        }

        return returnStatus;

    }
    /**
     * Enable all receivers registered in the manifest.
     * @param context the app context.
     */
    public static void enbleReceivers(final Context context) {
        //If shutdown is due to non-acceptance of agreement, then disable the boot receiver.
    }
    
    /**
     * Disable all receivers registered in the manifest.
     * @param context the app context.
     */
    public static void disableReceivers(final Context context) {
        //If shutdown is due to non-acceptance of agreement, then disable the boot receiver.
    }

    public static boolean isVendorEnhancedFramework() {
        try {
            String strProperty = SystemProperties.get("ro.vendor.qti.va_aosp.support", "1");
            if(strProperty != null && !strProperty.equals("1")) {
                return false;
            }
        } catch (Exception e) {
            android.util.Log.e("SMQUiUtils", e.toString(), e);
            return true;
        }
        return true;
    }
}
