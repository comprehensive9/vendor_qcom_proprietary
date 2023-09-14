/* 
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
package com.qualcomm.qti.smq.util;

import android.content.Context;
import android.database.Cursor;

public class OptInOutHelper {
    /**
     * Handle to the application context.
     */
    private Context mContext;

    /**
     * Default constructor.
     *
     * @param context
     *            The app context
     */
    public OptInOutHelper(final Context context) {
        super();
        mContext = context;
    }

    /**
     * Get the DB values given a key.
     *
     * @param key
     *            the key.
     * @return the value
     */
    public int getDBValue(final String key) {
        final String whereClause = OptInOutProductConstants.COL_KEY + "=?";
        final String[] selectionArgs = {
                key };
        final Cursor c = mContext.getContentResolver().query(
                OptInOutProductConstants.CONTENT_URI, null, whereClause,
                selectionArgs, null);
        int bRetValue = 0;
        if (!isCursorEmpty(c)) {
            final int value = c.getInt(c.getColumnIndex(OptInOutProductConstants.COL_VALUE));
            if (value > 0) {
                bRetValue = value;
            }
        }
        checkCursorAndClose(c);
        return bRetValue;

    }

    /**
     * Check if the key exists in the database.
     * @param key the key to check
     * @return true if the key exists else false
     */
    public boolean isKeyValid(final String key) {
        final String whereClause = OptInOutProductConstants.COL_KEY + "=?";
        final String[] selectionArgs = {
                key };
        final Cursor c = mContext.getContentResolver().query(
                OptInOutProductConstants.CONTENT_URI, null, whereClause,
                selectionArgs, null);
        if (!isCursorEmpty(c)) {
            checkCursorAndClose(c);
            return true;
        } else {
            checkCursorAndClose(c);
            return false;
        }
    }

    /**
     * Utility method to find if a cursor is empty or not.
     * This method also moves the cursor to the first element if not empty.
     *
     * @param c
     *            the cursor.
     * @return true is empty false otherwise.
     */
    public static boolean isCursorEmpty(final Cursor c) {

        if (c == null) {
            return true;
        } else {
            if (!c.moveToFirst()) {
                return true;
            } else {
                return false;
            }
        }

    }

    /**
     * Utility method to check if cursor is null and close.
     * @param c The cursor
     */
    public static void checkCursorAndClose(final Cursor c) {
        // Close cursor to avoid any leaks.
        if (c != null) {
            c.close();
        }
    }
}
