/*
 * Copyright (C) 2019 The Android Open Source Project
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

package com.qualcomm.qti.wigig.p2p.nsd;

import java.util.Map;
import com.qualcomm.qti.wigig.p2p.WifiP2pDevice;

/**
 * Interface for receiving Bonjour TXT record notifications
 * @hide
 */
oneway interface IDnsSdTxtRecordListener
{
    void onDnsSdTxtRecordAvailable(String fullDomainName, in Map<String, String> txtRecordMap,
        in WifiP2pDevice srcDevice);
}
