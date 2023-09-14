/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "SetupMinijail.h"

#include <minijail.h>

using android::SetUpMinijail;

void setupMinijail(const char* seccomp_policy_path){
    SetUpMinijail(std::string(seccomp_policy_path),"");
}

