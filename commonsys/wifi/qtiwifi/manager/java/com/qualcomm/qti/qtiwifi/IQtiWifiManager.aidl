/* ==============================================================================
 * IQtiWifiManager.aidl
 *
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.qti.qtiwifi;
import com.qualcomm.qti.qtiwifi.ICsiCallback;

interface IQtiWifiManager
{
    void startCsi();
    void stopCsi();
    void registerCsiCallback(in IBinder binder, in ICsiCallback callback, int callbackIdentifier);
    void unregisterCsiCallback(int callbackIdentifier);
    void doDriverCmd(String command);
}
