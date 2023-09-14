/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "config_data.h"

ConfigData& ConfigData::getInstance() {
    static ConfigData sInstance;
    return sInstance;
}

const std::string ConfigData::getPropertyKey(const property_id_type id) {
    auto idx = static_cast<uint32_t>(id);
    if (idx < static_cast<uint32_t>(property_id_type::PROPERTY_ID_MAX)) {
        return mData[idx].key;
    } else {
        return "";
    }
}

void ConfigData::setConfigCallback(const property_id_type id, qcril_config_callback_t cb) {
    std::lock_guard<qtimutex::QtiSharedMutex> lock(mMutex);
    auto index = static_cast<DataContainer::size_type>(id);
    if (index < length)
        mData[index].callback = cb;
}

qcril_config_callback_t ConfigData::getConfigCallback(const property_id_type id) {
    std::lock_guard<qtimutex::QtiSharedMutex> lock(mMutex);
    auto index = static_cast<DataContainer::size_type>(id);
    if (index < length)
        return mData[index].callback;
    return nullptr;
}
