/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "SetupMinijail.h"

#ifndef QMI_RIL_UTF
#include <minijail.h>

using android::SetUpMinijail;
#endif

void setupMinijail(const char* seccomp_policy_path){
    #ifndef QMI_RIL_UTF
    SetUpMinijail(std::string(seccomp_policy_path),"");
    #endif
}

