/* 
 * Copyright (c) 2014, 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
package com.qualcomm.qti.smq.util;

import android.net.Uri;
import android.provider.BaseColumns;

public class OptInOutProductConstants {

    /**
     * The User Setings table name.
     */
    public static final String QTR_SMQ_SETTINGS_TBL = "smq_settings";

    /**
     * The name of the column for storing keys.
     */
    public static final String COL_KEY = "key";

    /**
     * The name of the column for storing values.
     */
    public static final String COL_VALUE = "value";

    // KEY CONSTANTS
    /**
     * The key for storing the data preference.
     */
    public static final String QTR_KEY_DATA_PREFERENCE = "data_preference";

    /**
     * The key for storing the operation mode.
     */
    public static final String KEY_OPERATION_MODE = "operationMode";


    /**
     * The SMQ content provider authority.
     */
    public static final String AUTHORITY = "com.qti.smq.Feedback.provider";
    /**
     * The content URI.
     */
    public static final Uri AUTHORITY_URI = Uri.parse("content://" + AUTHORITY);

    public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, QTR_SMQ_SETTINGS_TBL);
}
