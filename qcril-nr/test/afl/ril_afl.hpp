/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void ril_afl_setup(const char* rilSocketPath);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus

#include <string>
#include <RilApiSession.hpp>

namespace qti {
namespace ril {
namespace afl {

Status initRilApiSession(RilApiSession& rilSession);
void readStdin(std::string& input);
void issueRequest(RilApiSession& rilSession, std::string& input);
void processAflInput(const std::string& rilSocketPath);
void setup(const std::string& rilSocketPath);

} // namespace afl
} // namespace ril
} // namespace qti

#endif // __cplusplus