/* Copyright (c) 2014, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

public interface ICallListListener {
    void onSessionAdded(ImsCallSessionImpl callSession);

    void onSessionRemoved(ImsCallSessionImpl callSession);

    void onSessionAdded(ImsUssdSessionImpl ussdSession);

    void onSessionRemoved(ImsUssdSessionImpl ussdSession);
}
