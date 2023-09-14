/* 
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
package com.qualcomm.qti.smq.util;

import android.content.ContentValues;
import android.content.Context;
import android.util.Log;

public class OptInOutHandler {
    /** The MainScreen log tag. */
    private final String LOG_TAG = "QCC-TR-UI";
    public final int SET_OFF = 0;
    public final int SET_ON = 1;
    public final int NOT_INITIAL = 3;

    /**
     * A handle to the Application context.
     */
    private Context mContext;

    public OptInOutHandler(Context mContext) {
        this.mContext = mContext;
    }

    public int getDBFromKey(String key){
        int ret = 0;
        Log.i(LOG_TAG, "getDBFromKey, key : "+key);
        final OptInOutHelper helper = new OptInOutHelper(mContext);
        final boolean isKeyValid = helper.isKeyValid(key);
        if (isKeyValid){
            Log.d(LOG_TAG, "DB Key is valid");
            final int dbValue = helper.getDBValue(key);
            Log.d(LOG_TAG, "DB value is : "+dbValue);
            ret = dbValue;
        }else{
            Log.e(LOG_TAG, "DB Key is not valid, maybe DB is not there");
        }

        Log.i(LOG_TAG, "getDBFromKey, ret = "+ret);
        return ret;
    }

    public int getTypeOptInOut(String key, int mode){
        int ret = NOT_INITIAL;
        Log.i(LOG_TAG, "getTypeOptInOut, key : "+key);
        final OptInOutHelper helper = new OptInOutHelper(mContext);
        final boolean isKeyValid = helper.isKeyValid(key);
        if (isKeyValid){
            Log.d(LOG_TAG, "DB Key is valid");
            final int dbValue = helper.getDBValue(key);
            Log.d(LOG_TAG, "DB value is : "+dbValue);
            if(dbValue>0){
                ret = SET_ON;
            }else{
                ret = SET_OFF;
            }
        }else{
            Log.e(LOG_TAG, "DB Key is not valid, maybe DB is not there");
            ret = NOT_INITIAL;
        }

        Log.i(LOG_TAG, "getTypeOptInOut, ret = "+ret);
        return ret;
    }

    public void changeSettingsAsync(final String key, final Boolean value) {
        //Update Shared Prefs.
        Log.d(LOG_TAG, "changeSettingsAsync, key: "+key+" /value: "+value);
        final String whereClause = OptInOutProductConstants.COL_KEY + "=?";
        final String[] selectionArgs = {
                key };
        final ContentValues values = new ContentValues();
        values.put(OptInOutProductConstants.COL_KEY, key);
        values.put(OptInOutProductConstants.COL_VALUE, value ? 1 : 0);
        OptInOutHelperUpdate helperUpdate = new OptInOutHelperUpdate(values, whereClause, selectionArgs, mContext);
        helperUpdate.updateExcuter();
    }
}
