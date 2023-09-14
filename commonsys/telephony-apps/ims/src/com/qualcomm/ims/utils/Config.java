/* Copyright (c) 2016,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.ims.utils;

import android.content.Context;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;

/**
 * Used for retrieving IMS configuration values.
 */
public class Config {
    private static final String LOG_TAG = "Config";

    private Config() {
        // Empty private constructor.
    }

    /*
     * Method to obtain the boolean value of a specified config.
     * @param context The Context.
     * @param config The configuration to be queried.
     *
     * @return The boolean value of the configuration.
     */
    public static boolean isConfigEnabled(Context context, int config) {
        try {
            return context.getResources().getBoolean(config);
        } catch (NullPointerException npe) {
            Log.e(LOG_TAG, "isConfigEnabled :: Null context! config=" + config);
        } catch (Exception exc) {
            Log.e(LOG_TAG, "isConfigEnabled :: Error getting boolean value for config="
                    + config + "\n" + exc);
        }
        return false;
    }

   /*
     * Method to obtain the boolean value of a specified carrier config.
     * @param context The Context.
     * @param subId The subscription Id
     * @param config The configuration to be queried.
     *
     * @return The boolean value of the configuration, If any error returns false
     */
    public static boolean isCarrierConfigEnabled(Context context, int subId, String config) {
        if (TextUtils.isEmpty(config) ||
                subId == SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
            return false;
        }
        CarrierConfigManager configManager = (CarrierConfigManager)
            context.getSystemService(Context.CARRIER_CONFIG_SERVICE);
        if (configManager == null) {
            return false;
        }
        PersistableBundle b = configManager.getConfigForSubId(subId);
        return b != null ? b.getBoolean(config, false): false;
    }
}

