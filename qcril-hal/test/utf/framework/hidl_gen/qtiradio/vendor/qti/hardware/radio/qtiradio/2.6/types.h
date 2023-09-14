/*===========================================================================
 *
 *    Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *===========================================================================*/

#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_6_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_6_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace V2_6 {

// Forward declaration for forward reference support:
enum class RadioTechnology : int32_t;
enum class RadioAccessFamily : int32_t;

enum class RadioTechnology : int32_t {
    UNKNOWN = 0,
    GPRS = 1,
    EDGE = 2,
    UMTS = 3,
    IS95A = 4,
    IS95B = 5,
    ONE_X_RTT = 6,
    EVDO_0 = 7,
    EVDO_A = 8,
    HSDPA = 9,
    HSUPA = 10,
    HSPA = 11,
    EVDO_B = 12,
    EHRPD = 13,
    LTE = 14,
    HSPAP = 15,
    GSM = 16,
    TD_SCDMA = 17,
    IWLAN = 18,
    LTE_CA = 19,
    NR_NSA = 20,
    NR_SA = 21,
};

enum class RadioAccessFamily : int32_t {
    UNKNOWN = 1 /* 1 << RadioTechnology:UNKNOWN */,
    GPRS = 2 /* 1 << RadioTechnology:GPRS */,
    EDGE = 4 /* 1 << RadioTechnology:EDGE */,
    UMTS = 8 /* 1 << RadioTechnology:UMTS */,
    IS95A = 16 /* 1 << RadioTechnology:IS95A */,
    IS95B = 32 /* 1 << RadioTechnology:IS95B */,
    ONE_X_RTT = 64 /* 1 << RadioTechnology:ONE_X_RTT */,
    EVDO_0 = 128 /* 1 << RadioTechnology:EVDO_0 */,
    EVDO_A = 256 /* 1 << RadioTechnology:EVDO_A */,
    HSDPA = 512 /* 1 << RadioTechnology:HSDPA */,
    HSUPA = 1024 /* 1 << RadioTechnology:HSUPA */,
    HSPA = 2048 /* 1 << RadioTechnology:HSPA */,
    EVDO_B = 4096 /* 1 << RadioTechnology:EVDO_B */,
    EHRPD = 8192 /* 1 << RadioTechnology:EHRPD */,
    LTE = 16384 /* 1 << RadioTechnology:LTE */,
    HSPAP = 32768 /* 1 << RadioTechnology:HSPAP */,
    GSM = 65536 /* 1 << RadioTechnology:GSM */,
    TD_SCDMA = 131072 /* 1 << RadioTechnology:TD_SCDMA */,
    LTE_CA = 524288 /* 1 << RadioTechnology:LTE_CA */,
    NR_NSA = 1048576 /* 1 << RadioTechnology:NR_NSA */,
    NR_SA = 2097152 /* 1 << RadioTechnology:NR_SA */,
};

//
// type declarations for package
//

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology o, ::std::ostream* os);
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily o, ::std::ostream* os);
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily e) {
    v &= static_cast<int32_t>(e);
    return v;
}

//
// type header definitions for package
//

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UNKNOWN) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GPRS) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GPRS)) {
        os += (first ? "" : " | ");
        os += "GPRS";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GPRS;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EDGE) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EDGE)) {
        os += (first ? "" : " | ");
        os += "EDGE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EDGE;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UMTS) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UMTS)) {
        os += (first ? "" : " | ");
        os += "UMTS";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UMTS;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95A) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95A)) {
        os += (first ? "" : " | ");
        os += "IS95A";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95A;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95B) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95B)) {
        os += (first ? "" : " | ");
        os += "IS95B";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95B;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::ONE_X_RTT) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::ONE_X_RTT)) {
        os += (first ? "" : " | ");
        os += "ONE_X_RTT";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::ONE_X_RTT;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_0) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_0)) {
        os += (first ? "" : " | ");
        os += "EVDO_0";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_0;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_A) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_A)) {
        os += (first ? "" : " | ");
        os += "EVDO_A";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_A;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSDPA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSDPA)) {
        os += (first ? "" : " | ");
        os += "HSDPA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSDPA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSUPA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSUPA)) {
        os += (first ? "" : " | ");
        os += "HSUPA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSUPA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPA)) {
        os += (first ? "" : " | ");
        os += "HSPA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_B) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_B)) {
        os += (first ? "" : " | ");
        os += "EVDO_B";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_B;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EHRPD) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EHRPD)) {
        os += (first ? "" : " | ");
        os += "EHRPD";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EHRPD;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE)) {
        os += (first ? "" : " | ");
        os += "LTE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPAP) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPAP)) {
        os += (first ? "" : " | ");
        os += "HSPAP";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPAP;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GSM) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GSM)) {
        os += (first ? "" : " | ");
        os += "GSM";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GSM;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::TD_SCDMA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::TD_SCDMA)) {
        os += (first ? "" : " | ");
        os += "TD_SCDMA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::TD_SCDMA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IWLAN) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IWLAN)) {
        os += (first ? "" : " | ");
        os += "IWLAN";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IWLAN;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE_CA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE_CA)) {
        os += (first ? "" : " | ");
        os += "LTE_CA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE_CA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_NSA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_NSA)) {
        os += (first ? "" : " | ");
        os += "NR_NSA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_NSA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_SA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_SA)) {
        os += (first ? "" : " | ");
        os += "NR_SA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_SA;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GPRS) {
        return "GPRS";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EDGE) {
        return "EDGE";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UMTS) {
        return "UMTS";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95A) {
        return "IS95A";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95B) {
        return "IS95B";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::ONE_X_RTT) {
        return "ONE_X_RTT";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_0) {
        return "EVDO_0";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_A) {
        return "EVDO_A";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSDPA) {
        return "HSDPA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSUPA) {
        return "HSUPA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPA) {
        return "HSPA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_B) {
        return "EVDO_B";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EHRPD) {
        return "EHRPD";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE) {
        return "LTE";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPAP) {
        return "HSPAP";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GSM) {
        return "GSM";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::TD_SCDMA) {
        return "TD_SCDMA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IWLAN) {
        return "IWLAN";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE_CA) {
        return "LTE_CA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_NSA) {
        return "NR_NSA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_SA) {
        return "NR_SA";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UNKNOWN) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GPRS) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GPRS)) {
        os += (first ? "" : " | ");
        os += "GPRS";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GPRS;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EDGE) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EDGE)) {
        os += (first ? "" : " | ");
        os += "EDGE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EDGE;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UMTS) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UMTS)) {
        os += (first ? "" : " | ");
        os += "UMTS";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UMTS;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95A) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95A)) {
        os += (first ? "" : " | ");
        os += "IS95A";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95A;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95B) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95B)) {
        os += (first ? "" : " | ");
        os += "IS95B";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95B;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::ONE_X_RTT) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::ONE_X_RTT)) {
        os += (first ? "" : " | ");
        os += "ONE_X_RTT";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::ONE_X_RTT;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_0) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_0)) {
        os += (first ? "" : " | ");
        os += "EVDO_0";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_0;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_A) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_A)) {
        os += (first ? "" : " | ");
        os += "EVDO_A";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_A;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSDPA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSDPA)) {
        os += (first ? "" : " | ");
        os += "HSDPA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSDPA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSUPA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSUPA)) {
        os += (first ? "" : " | ");
        os += "HSUPA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSUPA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPA)) {
        os += (first ? "" : " | ");
        os += "HSPA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_B) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_B)) {
        os += (first ? "" : " | ");
        os += "EVDO_B";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_B;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EHRPD) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EHRPD)) {
        os += (first ? "" : " | ");
        os += "EHRPD";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EHRPD;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE)) {
        os += (first ? "" : " | ");
        os += "LTE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPAP) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPAP)) {
        os += (first ? "" : " | ");
        os += "HSPAP";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPAP;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GSM) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GSM)) {
        os += (first ? "" : " | ");
        os += "GSM";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GSM;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::TD_SCDMA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::TD_SCDMA)) {
        os += (first ? "" : " | ");
        os += "TD_SCDMA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::TD_SCDMA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE_CA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE_CA)) {
        os += (first ? "" : " | ");
        os += "LTE_CA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE_CA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_NSA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_NSA)) {
        os += (first ? "" : " | ");
        os += "NR_NSA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_NSA;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_SA) == static_cast<int32_t>(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_SA)) {
        os += (first ? "" : " | ");
        os += "NR_SA";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_SA;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GPRS) {
        return "GPRS";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EDGE) {
        return "EDGE";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UMTS) {
        return "UMTS";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95A) {
        return "IS95A";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95B) {
        return "IS95B";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::ONE_X_RTT) {
        return "ONE_X_RTT";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_0) {
        return "EVDO_0";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_A) {
        return "EVDO_A";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSDPA) {
        return "HSDPA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSUPA) {
        return "HSUPA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPA) {
        return "HSPA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_B) {
        return "EVDO_B";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EHRPD) {
        return "EHRPD";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE) {
        return "LTE";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPAP) {
        return "HSPAP";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GSM) {
        return "GSM";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::TD_SCDMA) {
        return "TD_SCDMA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE_CA) {
        return "LTE_CA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_NSA) {
        return "NR_NSA";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_SA) {
        return "NR_SA";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily o, ::std::ostream* os) {
    *os << toString(o);
}


}  // namespace V2_6
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology, 22> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UNKNOWN,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GPRS,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EDGE,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::UMTS,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95A,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IS95B,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::ONE_X_RTT,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_0,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_A,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSDPA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSUPA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EVDO_B,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::EHRPD,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::HSPAP,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::GSM,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::TD_SCDMA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::IWLAN,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::LTE_CA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_NSA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioTechnology::NR_SA,
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily, 21> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UNKNOWN,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GPRS,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EDGE,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::UMTS,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95A,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::IS95B,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::ONE_X_RTT,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_0,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_A,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSDPA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSUPA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EVDO_B,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::EHRPD,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::HSPAP,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::GSM,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::TD_SCDMA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::LTE_CA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_NSA,
    ::vendor::qti::hardware::radio::qtiradio::V2_6::RadioAccessFamily::NR_SA,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_6_TYPES_H
