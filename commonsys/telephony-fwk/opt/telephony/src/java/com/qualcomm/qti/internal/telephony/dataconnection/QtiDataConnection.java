/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.dataconnection;

import com.android.internal.telephony.dataconnection.DataConnection;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.internal.telephony.dataconnection.DataServiceManager;
import com.android.internal.telephony.dataconnection.DcController;
import com.android.internal.telephony.dataconnection.DcTesterFailBringUpAll;
import com.android.internal.telephony.Phone;

public final class QtiDataConnection extends DataConnection {

    // ***** Constructor (NOTE: uses dcc.getHandler() as its Handler)
    public QtiDataConnection(Phone phone, String name, int id,
            DcTracker dct, DataServiceManager dataServiceManager,
            DcTesterFailBringUpAll failBringUpAll, DcController dcc) {
        super(phone, name, id, dct, dataServiceManager, failBringUpAll, dcc);
    }

    public static DataConnection makeDataConnection(Phone phone, int id,
            DcTracker dct, DataServiceManager dataServiceManager,
            DcTesterFailBringUpAll failBringUpAll, DcController dcc) {
        DataConnection dc = new QtiDataConnection(phone,
                "QtiDC-" + mInstanceNumber.incrementAndGet(), id, dct,
                dataServiceManager, failBringUpAll, dcc);
        dc.start();
        return dc;
    }
}
