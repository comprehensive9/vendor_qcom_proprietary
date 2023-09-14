/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "__config_key.h"
#include "QtiMutex.h"
#include <array>
#include <string>

using qcril_config_callback_t = std::function<void(const std::string, const std::string)>;
struct qcril_config_entry_t {
    const std::string key;
    qcril_config_callback_t callback;
};

class ConfigData {
public:
  static ConfigData& getInstance();
  const std::string getPropertyKey(property_id_type id);
  void setConfigCallback(const property_id_type id, qcril_config_callback_t cb);
  qcril_config_callback_t getConfigCallback(const property_id_type id);
private:
  ConfigData() = default;
  static constexpr uint32_t length = static_cast<uint32_t>(PROPERTY_ID_MAX);
  using DataContainer = std::array<qcril_config_entry_t, length>;
  DataContainer mData = CONFIG_DATA_AGGRE_INIT;
  qtimutex::QtiSharedMutex mMutex;
};
