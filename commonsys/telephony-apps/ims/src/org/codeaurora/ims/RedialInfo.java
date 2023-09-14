/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

public class RedialInfo {

    private int retryCallFailCause;

    private int retryCallFailRadioTech;

    public RedialInfo(int retryCallFailCause, int retryCallFailRadioTech) {
        this.retryCallFailCause = retryCallFailCause;
        this.retryCallFailRadioTech = retryCallFailRadioTech;
    }

    public int getRetryCallFailCause() {
        return retryCallFailCause;
    }

    public int getRetryCallFailRadioTech() {
        return retryCallFailRadioTech;
    }

    @Override
    public String toString() {
        return "RedialInfo: retryCallFailCause = " + retryCallFailCause +
                " retryCallFailRadioTech = " + retryCallFailRadioTech;
    }
}
