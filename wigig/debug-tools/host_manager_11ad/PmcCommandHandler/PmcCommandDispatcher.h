/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "JsonHandlerSDK.h"

// *************************************************************************************************

// On-target PMC CLI commands
class PmcCommandDispatcher : public JsonOpCodeDispatcher
{
public:
    PmcCommandDispatcher();
};

// DmTools legacy PMC commands
class LegacyPmcCommandDispatcher : public JsonOpCodeDispatcher
{
public:
    LegacyPmcCommandDispatcher();
};
