/**
 * Copyright (c) 2016, 2020 Qualcomm Technologies, Inc.
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

package com.qualcomm.qti.wigig.p2p;

import android.os.Messenger;
import com.qualcomm.qti.wigig.IActionListener;
import com.qualcomm.qti.wigig.WpsInfo;
import com.qualcomm.qti.wigig.p2p.IConnectionInfoListener;
import com.qualcomm.qti.wigig.p2p.IGroupInfoListener;
import com.qualcomm.qti.wigig.p2p.IHandoverMessageListener;
import com.qualcomm.qti.wigig.p2p.IPeerListListener;
import com.qualcomm.qti.wigig.p2p.IPersistentGroupInfoListener;
import com.qualcomm.qti.wigig.p2p.WifiP2pConfig;
import com.qualcomm.qti.wigig.p2p.WifiP2pWfdInfo;
import com.qualcomm.qti.wigig.p2p.nsd.IServiceResponseListener;
import com.qualcomm.qti.wigig.p2p.nsd.IDnsSdServiceResponseListener;
import com.qualcomm.qti.wigig.p2p.nsd.IDnsSdTxtRecordListener;
import com.qualcomm.qti.wigig.p2p.nsd.IUpnpServiceResponseListener;
import com.qualcomm.qti.wigig.p2p.nsd.WifiP2pServiceInfo;
import com.qualcomm.qti.wigig.p2p.nsd.WifiP2pServiceRequest;

/**
 * Interface that WigigP2pService implements
 *
 * {@hide}
 */
interface IWigigP2pManager
{
    Messenger getMessenger();
    Messenger getP2pStateMachineMessenger();
    void setMiracastMode(int mode);
    oneway void discoverPeers(in IBinder binder, in IActionListener callback);
    oneway void stopPeerDiscovery(in IBinder binder, in IActionListener callback);
    oneway void connect(in WifiP2pConfig config, in IBinder binder, in IActionListener callback);
    oneway void cancelConnect(in IBinder binder, in IActionListener callback);
    oneway void createGroup(in IBinder binder, in IActionListener callback);
    oneway void removeGroup(in IBinder binder, in IActionListener callback);
    oneway void listen(boolean enable, in IBinder binder, in IActionListener callback);
    oneway void setWigigP2pChannels(int lc, int oc, in IBinder binder, in IActionListener callback);
    oneway void startWps(in WpsInfo wps, in IBinder binder, in IActionListener callback);
    oneway void addLocalService(in Messenger client, in WifiP2pServiceInfo servInfo, in IBinder binder, in IActionListener callback);
    oneway void removeLocalService(in Messenger client, in WifiP2pServiceInfo servInfo, in IBinder binder, in IActionListener callback);
    oneway void clearLocalServices(in Messenger client, in IBinder binder, in IActionListener callback);
    oneway void setServiceResponseListener(in Messenger client, in IBinder binder, in IServiceResponseListener callback);
    oneway void setDnsSdServiceResponseListener(in Messenger client, in IBinder binder, in IDnsSdServiceResponseListener callback);
    oneway void setDnsSdTxtRecordListener(in Messenger client, in IBinder binder, in IDnsSdTxtRecordListener callback);
    oneway void setUpnpServiceResponseListener(in Messenger client, in IBinder binder, in IUpnpServiceResponseListener callback);
    oneway void discoverServices(in Messenger client, in IBinder binder, in IActionListener callback);
    oneway void addServiceRequest(in Messenger client, in WifiP2pServiceRequest req, in IBinder binder, in IActionListener callback);
    oneway void removeServiceRequest(in Messenger client, in WifiP2pServiceRequest req, in IBinder binder, in IActionListener callback);
    oneway void clearServiceRequests(in Messenger client, in IBinder binder, in IActionListener callback);
    oneway void requestPeers(in IBinder binder, in IPeerListListener callback);
    oneway void requestConnectionInfo(in IBinder binder, in IConnectionInfoListener callback);
    oneway void requestGroupInfo(in IBinder binder, in IGroupInfoListener callback);
    oneway void setDeviceName(String devName, in IBinder binder, in IActionListener callback);
    oneway void setWFDInfo(in WifiP2pWfdInfo wfdInfo, in IBinder binder, in IActionListener callback);
    oneway void deletePersistentGroup(int netId, in IBinder binder, in IActionListener callback);
    oneway void requestPersistentGroupInfo(in IBinder binder, in IPersistentGroupInfoListener callback);
    oneway void getNfcHandoverRequest(in IBinder binder, in IHandoverMessageListener callback);
    oneway void getNfcHandoverSelect(in IBinder binder, in IHandoverMessageListener callback);
    oneway void initiatorReportNfcHandover(String handoverSelect, in IBinder binder, in IActionListener callback);
    oneway void responderReportNfcHandover(String handoverRequest, in IBinder binder, in IActionListener callback);
}

