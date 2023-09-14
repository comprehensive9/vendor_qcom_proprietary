/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <string>
#include <vector>

namespace qcril {
namespace interfaces {

enum class AudioCallState { INVALID, INACTIVE, ACTIVE, HOLD, LOCAL_HOLD };

inline std::string toString(const AudioCallState& o) {
  if (o == AudioCallState::INVALID) {
    return "INVALID";
  }
  if (o == AudioCallState::INACTIVE) {
    return "INACTIVE";
  }
  if (o == AudioCallState::ACTIVE) {
    return "ACTIVE";
  }
  if (o == AudioCallState::HOLD) {
    return "HOLD";
  }
  if (o == AudioCallState::LOCAL_HOLD) {
    return "LOCAL_HOLD";
  }
  return "<invalid>";
}

}  // namespace interfaces
}  // namespace qcril
