/**
 * Copyright (c) 2016, 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (c) 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.wigig;

import java.util.List;
import com.qualcomm.qti.wigig.IActionListener;
import com.qualcomm.qti.wigig.ISoftApCallback;
import com.qualcomm.qti.wigig.IWpsCallback;
import com.qualcomm.qti.wigig.ScanResult;
import com.qualcomm.qti.wigig.WifiConfiguration;
import com.qualcomm.qti.wigig.WifiInfo;
import com.qualcomm.qti.wigig.WpsInfo;

import android.os.Messenger;

/**
 * Interface that allows controlling and querying Wigig connectivity.
 *
 * {@hide}
 */
interface IWigigManager
{
    List<WifiConfiguration> getConfiguredNetworks();
    void startScan();
    List<ScanResult> getScanResults();
    void disconnect();
    WifiInfo getConnectionInfo();
    boolean setWigigEnabled(boolean enable);
    boolean isWigigEnabled();
    boolean setSensingEnabled(boolean enable);
    boolean isSensingEnabled();
    void setWigigApEnabled(in WifiConfiguration wifiConfig, boolean enable);
    int getWigigApEnabledState();
    WifiConfiguration getWigigApConfiguration();
    void setWigigApConfiguration(in WifiConfiguration wifiConfig);
    Messenger getWigigServiceMessenger();
    String getConfigFile();
    boolean setRateUpgradeEnabled(boolean enable);
    boolean isRateUpgradeEnabled();
    void registerSoftApCallback(in IBinder binder, in ISoftApCallback callback, int callbackIdentifier);
    void unregisterSoftApCallback(int callbackIdentifier);
    oneway void connect(in WifiConfiguration config, int netId, in IBinder binder, in IActionListener callback);
    oneway void save(in WifiConfiguration config, in IBinder binder, in IActionListener callback);
    oneway void forget(int netId, in IBinder binder, in IActionListener callback);
    oneway void startWps(in WpsInfo config, in IBinder binder, in IWpsCallback callback);
    oneway void cancelWps(in IBinder binder, in IWpsCallback callback);
}
