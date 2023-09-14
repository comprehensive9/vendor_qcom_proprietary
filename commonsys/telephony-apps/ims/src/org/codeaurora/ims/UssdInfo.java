/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *  Not a Contribution.
 *
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.codeaurora.ims;

public final class UssdInfo {

    // enum UssdModeType
    public static final int INVALID = -1;

    private int mType;
    private String mMessage;
    private int mErrorCode;
    private String mErrorMessage;

    public UssdInfo(int type, String msg,
                    int errorCode, String errMsg) {
        mType = type;
        mMessage = msg;
        mErrorCode = (errorCode == Integer.MAX_VALUE) ?
                     INVALID : errorCode;
        mErrorMessage = errMsg;
    }

    // returns Ussd mode type
    public int getType() {
        return mType;
    }

    // returns USSD message received in case of success
    public String getMessage() {
        return mMessage;
    }

    // returns Error Code in case of failure
    public int getErrorCode() {
        return mErrorCode;
    }

    // returns Error Message in case of failure
    public String getErrorMessage() {
        return mErrorMessage;
    }

    public String toString() {
        return "UssdInfo type: " + mType +
               " message: " + mMessage +
               " errorCode: " + mErrorCode +
               " errorMessage: " + mErrorMessage;
    }
}
