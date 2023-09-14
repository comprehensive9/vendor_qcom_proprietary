/**
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef PHYSICALCONFIGSTRUCTUPDATEMESSAGE
#define PHYSICALCONFIGSTRUCTUPDATEMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "iterator"

namespace rildata {

struct PhysicalConfigStructInfo_t {
  CellConnectionStatus_t status;
  int32_t cellBandwidthDownlink;
  RadioAccessFamily_t rat;
  RadioFrequencyInfo_t rfInfo;
  std::vector<int32_t> contextIds;
  uint32_t physicalCellId;
  void dump(std::string padding, std::ostream& os) const {
    std::stringstream ss;
    ss << "<Status=" << (int)status;
    ss << ", downlink=" << cellBandwidthDownlink;
    ss << ", rat=" << rat;
    ss << ", rfInfo";
    rfInfo.dump("", ss);
    ss << ", cids=[";
    std::copy(contextIds.begin(), contextIds.end(), std::ostream_iterator<int>(ss, ","));
    ss << "], physicalCellId=" << physicalCellId << ">";
    os << padding << ss.str();
  }
};

/********************** Class Definitions *************************/
class PhysicalConfigStructUpdateMessage: public UnSolicitedMessage,
                           public add_message_id<PhysicalConfigStructUpdateMessage> {

private:
  std::vector<PhysicalConfigStructInfo_t> mPhysicalConfigStructInfo;
public:
  static constexpr const char *MESSAGE_NAME = "PhysicalConfigStructUpdateMessage";

  PhysicalConfigStructUpdateMessage() = delete;
  ~PhysicalConfigStructUpdateMessage(){};
  PhysicalConfigStructUpdateMessage(std::vector<PhysicalConfigStructInfo_t> const &physicalConfigInfo);

  std::shared_ptr<UnSolicitedMessage> clone();
  const std::vector<PhysicalConfigStructInfo_t> & getPhysicalConfigStructInfo();
  static std::string convertToString(const PhysicalConfigStructInfo_t& config);
  string dump();
};

inline PhysicalConfigStructUpdateMessage::PhysicalConfigStructUpdateMessage
(
  const std::vector<PhysicalConfigStructInfo_t> &physicalConfigStructInfo
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mPhysicalConfigStructInfo = physicalConfigStructInfo;
}

inline const std::vector<PhysicalConfigStructInfo_t> &
PhysicalConfigStructUpdateMessage::getPhysicalConfigStructInfo()
{
  return mPhysicalConfigStructInfo;
}

inline std::shared_ptr<UnSolicitedMessage> PhysicalConfigStructUpdateMessage::clone()
{
  return std::make_shared<PhysicalConfigStructUpdateMessage>(mPhysicalConfigStructInfo);
}

inline string PhysicalConfigStructUpdateMessage::dump()
{
  std::stringstream ss;
  ss << mName << " {";
  for (auto it = mPhysicalConfigStructInfo.begin(); it != mPhysicalConfigStructInfo.end(); it++) {
    it->dump("", ss);
    ss << ",";
  }
  ss << "}";
  return ss.str();
}

} //namespace

#endif