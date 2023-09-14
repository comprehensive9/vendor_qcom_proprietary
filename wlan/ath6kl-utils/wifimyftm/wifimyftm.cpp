/*==============================================================================
*  Copyright (c) 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include <hwbinder/IPCThreadState.h>
#include "wifimyftm.h"
#include <stdio.h>
#include <fcntl.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "myftmHal"
#define UNUSED(x) ((void)x)
#define MAX_STRING_FORMATTING_SIZE 1000

using namespace std;
using ::android::hidl::memory::V1_0::IMemory;
using vendor::qti::hardware::wifi::wifimyftm::V1_0::MyFtmStatus;
namespace vendor {
namespace qti {
namespace hardware {
namespace wifi {
namespace wifimyftm {
namespace V1_0 {
namespace implementation {

Return<void> WifiMyFtm::myftmCmd(const hidl_string& arg, myftmCmd_cb _hidl_cb) {
	MyFtmCmdStatus status = MyFtmCmdStatus::SUCCESS;
	std::string debuglogs="";
	char* line = NULL;
	size_t len = 0;
	std::string str = arg;
	FILE *fp;
	char * cstr = new char [str.length()+1];
	strlcpy(cstr, str.c_str(), str.length()+1);
	ALOGE("The myftm command recieved is : %s\n", cstr);

	if(std::strncmp(cstr,"myftm ", 6) != 0) {
		debuglogs = "Invalid Arguments";
		status = MyFtmCmdStatus::FAILURE_ARGS_INVALID;
		goto call_back;
	}
	/* Execute the myftm command */
	fp = popen(cstr, "r");
	if (fp == NULL) {
		debuglogs = "Invalid Arguments";
		status = MyFtmCmdStatus::FAILURE_UNKNOWN;
		ALOGE("Failed to run command\n" );
		goto call_back;
	}

	while ((getline(&line, &len, fp)) != -1) {
		debuglogs.append(line);
	}

	if(debuglogs.find("usage: myftm [options]") != std::string::npos ) {
		debuglogs = "Invalid Arguments";
		status = MyFtmCmdStatus::FAILURE_ARGS_INVALID;
		goto call_back;
	}
	/* close */
	pclose(fp);
call_back:
	_hidl_cb({status,debuglogs});
	/* clear the string for next commands */
	return Void();
}

IWifiMyFtm *WifiMyFtm::getInstance(void){
    return new WifiMyFtm();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace wifimyftm
}  // namespace wifi
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
