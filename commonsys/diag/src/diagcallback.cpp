/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag communication support

GENERAL DESCRIPTION

Implementation of diag hidl communication layer between diag library and diag driver
for asynchronous callbacks from diag hidl server to client.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <stdlib.h>
#include "comdef.h"
#include "stdio.h"
#include "diagcallback.h"
#include "diag_lsm.h"
namespace vendor {
namespace qti {
namespace diaghal {
namespace V1_0 {
namespace implementation {

// Methods from ::vendor::qti::diaghal::V1_0::Idiagcallback follow.
Return<int32_t> diagcallback::send_data(const hidl_memory& buf, uint32_t len) {
    sp<IMemory> memory;
    unsigned char *data;

    memory = mapMemory(buf);
    if (memory == nullptr) {
	return 0;
    }
    data = static_cast<unsigned char*>(static_cast<void*>(memory->getPointer()));
    diag_process_data(data,len);
    return 0;
}

Idiagcallback *diagcallback::getInstance(void){
    return new diagcallback();
}

}}}}
}  // namespace vendor::qti::diaghal::implementation
