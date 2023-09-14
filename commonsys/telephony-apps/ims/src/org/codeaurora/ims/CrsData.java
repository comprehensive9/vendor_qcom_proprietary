/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

/* CrsData class takes care of all the details like crs type and
 * original call type for MT IMS calls with CRS(Customized Ringing
 * Signal). This class is not relevant for non-IMS MT calls.
 */

public final class CrsData {

    private int mCrsType = QtiCallConstants.CRS_TYPE_INVALID;
    private int mOriginalCallType = CallDetails.CALL_TYPE_UNKNOWN;

    public CrsData() {
        this(QtiCallConstants.CRS_TYPE_INVALID,
                CallDetails.CALL_TYPE_UNKNOWN);
    }

    public CrsData(CrsData crsData) {
        this(crsData.getCrsType(), crsData.getOriginalCallType());
    }

    public CrsData(int crsType, int originalCallType) {
        mCrsType = crsType;
        mOriginalCallType = originalCallType;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == this) {
            return true;
        }
        if (!(obj instanceof CrsData)) {
            return false;
        }
        CrsData in = (CrsData)obj;
        return this.mCrsType == in.getCrsType()
            && this.mOriginalCallType == in.getOriginalCallType();
    }

    public int getCrsType() {
        return mCrsType;
    }

    public int getOriginalCallType() {
        return mOriginalCallType;
    }

    public void setCrsType(int crsType) {
        mCrsType = crsType;
    }

    public void setOriginalCallType(int originalCallType) {
        mOriginalCallType = originalCallType;
    }

    public boolean update(CrsData update) {
        boolean hasChanged = false;
        if (update == null) {
            return false;
        }
        if (mCrsType != update.getCrsType()) {
            mCrsType = update.getCrsType();
            hasChanged = true;
        }

        if (mOriginalCallType != update.getOriginalCallType()) {
            mOriginalCallType = update.getOriginalCallType();
            hasChanged = true;
        }
        return hasChanged;
    }
    public String toString() {
        return "CrsData crsType: " + mCrsType +
               " originalCallType: " + mOriginalCallType;
    }
}
