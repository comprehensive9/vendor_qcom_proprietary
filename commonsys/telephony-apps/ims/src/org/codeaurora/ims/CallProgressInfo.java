/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import java.util.Objects;

/* This class is responsible to cache the call progress info received as part of call state change
 * indication. The call progress info is received only for MO calls during alerting state.
 * We get the call progress information in below cases
 *    1. MO call will be rejected with protocol Q850 error.
 *    2. There is already an ACTIVE call at remote UE side and this call is a WAITING call from
 *       remote UE perspective.
 *    3. Call forwarding is enabled at remote UE and this call will be forwarded from remote UE
 *       perspective.
 *    4. MT call is alerting from remote UE perspective.
 */

public final class CallProgressInfo {

    // Call Progress Information type.
    private int mType = QtiCallConstants.CALL_PROGRESS_INFO_TYPE_INVALID;

    // Reason code for call rejection based on Q850 defined by ITU.
    // Ref: https://www.itu.int/rec/T-REC-Q.850
    // This field will be valid only for QtiCallConstants#CALL_PROGRESS_INFO_TYPE_CALL_REJ_Q850.
    private int mReasonCode = QtiCallConstants.CALL_REJECTION_CODE_INVALID;

    // Reason text for call rejection, this field will be valid only for
    // QtiCallConstants#CALL_PROGRESS_INFO_TYPE_CALL_REJ_Q850
    private String mReasonText = null;

    public CallProgressInfo() {}

    public CallProgressInfo(CallProgressInfo callProgInfo) {
        this(callProgInfo.getType(), callProgInfo.getReasonCode(), callProgInfo.getReasonText());
    }

    public CallProgressInfo(int type, int reasonCode, String reasonText) {
        mType = type;
        mReasonCode = reasonCode;
        mReasonText = reasonText;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == this) {
            return true;
        }
        if (!(obj instanceof CallProgressInfo)) {
            return false;
        }
        CallProgressInfo in = (CallProgressInfo)obj;
        return this.mType == in.getType()
                && this.mReasonCode == in.getReasonCode()
                && Objects.equals(this.mReasonText, in.getReasonText());
    }

    public int getType() {
        return mType;
    }

    public int getReasonCode() {
        return mReasonCode;
    }

    public String getReasonText() {
        return mReasonText;
    }

    public void setType(int callProgType) {
        mType = callProgType;
    }

    public void setReasonCode(int reasonCode) {
        mReasonCode = reasonCode;
    }

    public void setReasonText(String reasonText) {
        mReasonText = reasonText;
    }

    public String toString() {
        return "CallProgressInfo Type: " + mType + " Reason code: " + mReasonCode +
                " Reason Text: " + mReasonText;
    }
}
