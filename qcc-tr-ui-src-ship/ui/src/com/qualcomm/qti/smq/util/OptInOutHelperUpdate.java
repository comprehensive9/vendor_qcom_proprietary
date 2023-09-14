/* 
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
package com.qualcomm.qti.smq.util;

import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.util.Log;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.FutureTask;

public class OptInOutHelperUpdate {
    private final String LOG_TAG = "QCC-TR-UI";
    /**
     * The content values to update.
     */
    private ContentValues mCv;
    /**
     * The where clause.
     */
    private String mWhereClause;

    /**
     * The selection args.
     */
    private String[] mSelectionArgs;

    /**
     * The application context.
     */
    private Context mContext;

    /**
     * Handle to the DB cursor.
     */
    private Cursor mCursor;

    private ExecutorService mExecutor;

    public OptInOutHelperUpdate(ContentValues mCv, String mWhereClause, String[] mSelectionArgs, Context mContext) {
        super();
        this.mCv = mCv;
        this.mWhereClause = mWhereClause;
        this.mSelectionArgs = mSelectionArgs;
        this.mContext = mContext;
        mExecutor = Executors.newSingleThreadExecutor();
    }

    public Boolean updateExcuter() {
        Log.i(LOG_TAG, "updateExcuter()");
        boolean result = false;
        try{
            final FutureTask<Boolean> updateTask = new FutureTask<>(new Callable<Boolean>() {
                @Override
                public Boolean call() {
                    return updater();
                }
            });
            mExecutor.submit(updateTask);
            result = updateTask.get();
            Log.i(LOG_TAG, "updateExcuter() ret : "+result);
        }catch (Exception e){
            Log.e("updateExcuter", e.getMessage());
        }
        mExecutor.shutdown();
        return result;
    }

    private Boolean updater() throws IllegalArgumentException{
        final Cursor c = mContext.getContentResolver().query(
                OptInOutProductConstants.CONTENT_URI, null, mWhereClause,
                mSelectionArgs, null);
        try{
            if (OptInOutHelper.isCursorEmpty(c)) {
                // Empty , insert
                mContext.getContentResolver().insert(
                        OptInOutProductConstants.CONTENT_URI, mCv);

            } else {

                // Not empty , update
                mContext.getContentResolver().update(
                        OptInOutProductConstants.CONTENT_URI, mCv, mWhereClause,
                        mSelectionArgs);

            }
        } catch (IllegalArgumentException e){
            Log.e("OptInOutHelperUpdate", e.getMessage());
            throw e;
        }
        OptInOutHelper.checkCursorAndClose(c);

        return true;
    }
}
