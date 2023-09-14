/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.internal.telephony.dataconnection;

import com.android.internal.telephony.vendor.dataconnection.VendorDataResetEventTracker;
import com.android.internal.telephony.Phone;

public class QtiDataResetEventTracker extends VendorDataResetEventTracker {
    public QtiDataResetEventTracker(int transportType, Phone phone,
            ResetEventListener listener) {
        super(transportType, phone, listener);
    }
}
