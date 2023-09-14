/*==============================================================================
*  Copyright (c) 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
#include <hidl/LegacySupport.h>
#include "wifimyftm.h"
#include <stdio.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.wifi.wifimyftm@1.0-service"
#define UNUSED(x) ((void)x)

using vendor::qti::hardware::wifi::wifimyftm::V1_0::IWifiMyFtm;
using vendor::qti::hardware::wifi::wifimyftm::V1_0::implementation::WifiMyFtm;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

int main() {
    int res;
    printf("MyFtm Hal Service Intiating");
    android::sp<IWifiMyFtm> ser = WifiMyFtm::getInstance();
    configureRpcThreadpool(1, true);

    if (ser != nullptr) {
        res = ser->registerAsService();
        if(res != 0){
            printf("Can't register MyFTM HAL service, nullptr");
            return 0;
        }
    } else {
        printf("Can't create instance of WifiMyFtm, nullptr");
        return 0;
    }
    joinRpcThreadpool();
    printf("MyFtm Hal Service Terminated");

    return 0; // should never get here
}
