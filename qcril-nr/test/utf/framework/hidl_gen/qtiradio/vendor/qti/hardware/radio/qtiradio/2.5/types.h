/*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_5_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_5_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace V2_5 {

// Forward declaration for forward reference support:
enum class Status : int32_t;
enum class NrConfig : int32_t;


enum class Status : int32_t {
    INVALID = -1,
    FAILURE = 0,
    SUCCESS = 1,
};
/**
 * NR5G config type
 * If not specified, default type is 0.
 */
enum class NrConfig : int32_t {
    NR_CONFIG_INVALID = -1,
    NR_CONFIG_COMBINED_SA_NSA = 0,
    NR_CONFIG_NSA = 1,
    NR_CONFIG_SA = 2,
};

//
// type declarations for package
//
template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_5::Status o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_5::Status o, ::std::ostream* os);
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_5::Status e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig o, ::std::ostream* os);
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig e) {
    v &= static_cast<int32_t>(e);
    return v;
}

//
// type header definitions for package
//

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_5::Status>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_5::Status> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::INVALID) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_5::Status::INVALID)) {
        os += (first ? "" : " | ");
        os += "INVALID";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::INVALID;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::FAILURE) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_5::Status::FAILURE)) {
        os += (first ? "" : " | ");
        os += "FAILURE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::FAILURE;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::SUCCESS) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_5::Status::SUCCESS)) {
        os += (first ? "" : " | ");
        os += "SUCCESS";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::SUCCESS;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_5::Status o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::INVALID) {
        return "INVALID";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::FAILURE) {
        return "FAILURE";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::SUCCESS) {
        return "SUCCESS";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_5::Status o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_INVALID) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_INVALID)) {
        os += (first ? "" : " | ");
        os += "NR_CONFIG_INVALID";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_INVALID;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_COMBINED_SA_NSA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_COMBINED_SA_NSA)) {
        os += (first ? "" : " | ");
        os += "NR_CONFIG_COMBINED_SA_NSA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_COMBINED_SA_NSA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_NSA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_NSA)) {
        os += (first ? "" : " | ");
        os += "NR_CONFIG_NSA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_NSA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_SA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_SA)) {
        os += (first ? "" : " | ");
        os += "NR_CONFIG_SA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_SA;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_INVALID) {
        return "NR_CONFIG_INVALID";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_COMBINED_SA_NSA) {
        return "NR_CONFIG_COMBINED_SA_NSA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_NSA) {
        return "NR_CONFIG_NSA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_SA) {
        return "NR_CONFIG_SA";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig o, ::std::ostream* os) {
    *os << toString(o);
}

}  // namespace V2_5
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

//
// global type declarations for package
//
namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_5::Status, 3> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_5::Status> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::INVALID,
    ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::FAILURE,
    ::vendor::qti::hardware::radio::qtiradio::V2_5::Status::SUCCESS,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig, 4> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_INVALID,
    ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_COMBINED_SA_NSA,
    ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_NSA,
    ::vendor::qti::hardware::radio::qtiradio::V2_5::NrConfig::NR_CONFIG_SA,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_5_TYPES_H
