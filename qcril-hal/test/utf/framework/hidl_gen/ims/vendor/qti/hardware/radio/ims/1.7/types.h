/*===========================================================================
   Copyright (c) 2021 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_IMS_V1_7_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_IMS_V1_7_TYPES_H

#include <vendor/qti/hardware/radio/ims/1.0/types.h>
#include <vendor/qti/hardware/radio/ims/1.3/types.h>
#include <vendor/qti/hardware/radio/ims/1.4/types.h>
#include <vendor/qti/hardware/radio/ims/1.5/types.h>
#include <vendor/qti/hardware/radio/ims/1.6/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace V1_7 {

// Forward declaration for forward reference support:
enum class CrsType : int32_t;
struct CrsData;
enum class CallProgressInfoType : int32_t;
struct CallProgressInfo;
struct CallInfo;

// Order of inner types was changed for forward reference support.

enum class CrsType : int32_t {
    INVALID = 0,
    AUDIO = 1 /* 1 << 0 */,
    VIDEO = 2 /* 1 << 1 */,
};

struct CrsData final {
    /*
     * INVALID if CrsData is invalid,
     * AUDIO if only audio will be played,
     * VIDEO if only video will be played,
     * AUDIO | VIDEO if both video and audio will be played.
     */
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::ims::V1_7::CrsType> type __attribute__ ((aligned(4)));
    /*
     * Valid only if CrsData.type is not INVALID
     * Call type of the actual call received from network.
     */
    ::vendor::qti::hardware::radio::ims::V1_0::CallType originalCallType __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CrsData, type) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CrsData, originalCallType) == 4, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::ims::V1_7::CrsData) == 8, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::ims::V1_7::CrsData) == 4, "wrong alignment");

enum class CallProgressInfoType : int32_t {
    INVALID = 0,
    /*
     * Invalid if CallProgressInfo is not present.
     */
    CALL_REJ_Q850 = 1 /* ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType.INVALID implicitly + 1 */,
    /*
     * MO call will be rejected with protocol Q850 error
     */
    CALL_WAITING = 2 /* ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType.CALL_REJ_Q850 implicitly + 1 */,
    /*
     * There is already an ACTIVE call at remote UE side and this call is a
     * WAITING call from remote UE perspective.
     */
    CALL_FORWARDING = 3 /* ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType.CALL_WAITING implicitly + 1 */,
    /*
     * Call forwarding is enabled at remote UE and this call will be forwarded
     * from remote UE perspective.
     */
    REMOTE_AVAILABLE = 4 /* ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType.CALL_FORWARDING implicitly + 1 */,
};

struct CallProgressInfo final {
    ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType type __attribute__ ((aligned(4)));
    /*
     * Type of call progress info
     */
    uint16_t reasonCode __attribute__ ((aligned(2)));
    /*
     * Reason code for call rejection, this will be valid only for
     * CallProgressInfoType#CALL_REJ_Q850.
     */
    ::android::hardware::hidl_string reasonText __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo, type) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo, reasonCode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo, reasonText) == 8, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo) == 24, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo) == 8, "wrong alignment");

struct CallInfo final {
    ::vendor::qti::hardware::radio::ims::V1_0::CallState state __attribute__ ((aligned(4)));
    uint32_t index __attribute__ ((aligned(4)));
    uint32_t toa __attribute__ ((aligned(4)));
    bool hasIsMpty __attribute__ ((aligned(1)));
    bool isMpty __attribute__ ((aligned(1)));
    bool hasIsMT __attribute__ ((aligned(1)));
    bool isMT __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::radio::ims::V1_4::MultiIdentityLineInfoHal mtMultiLineInfo __attribute__ ((aligned(8)));
    uint32_t als __attribute__ ((aligned(4)));
    bool hasIsVoice __attribute__ ((aligned(1)));
    bool isVoice __attribute__ ((aligned(1)));
    bool hasIsVoicePrivacy __attribute__ ((aligned(1)));
    bool isVoicePrivacy __attribute__ ((aligned(1)));
    ::android::hardware::hidl_string number __attribute__ ((aligned(8)));
    uint32_t numberPresentation __attribute__ ((aligned(4)));
    ::android::hardware::hidl_string name __attribute__ ((aligned(8)));
    uint32_t namePresentation __attribute__ ((aligned(4)));
    bool hasCallDetails __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::radio::ims::V1_6::CallDetails callDetails __attribute__ ((aligned(8)));
    bool hasFailCause __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::radio::ims::V1_5::CallFailCauseResponse failCause __attribute__ ((aligned(8)));
    bool hasIsEncrypted __attribute__ ((aligned(1)));
    bool isEncrypted __attribute__ ((aligned(1)));
    bool hasIsCalledPartyRinging __attribute__ ((aligned(1)));
    bool isCalledPartyRinging __attribute__ ((aligned(1)));
    ::android::hardware::hidl_string historyInfo __attribute__ ((aligned(8)));
    bool hasIsVideoConfSupported __attribute__ ((aligned(1)));
    bool isVideoConfSupported __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::radio::ims::V1_3::VerstatInfo verstatInfo __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::radio::ims::V1_5::TirMode tirMode __attribute__ ((aligned(4)));
    /*
     * True if the message is intended for preliminary resource allocation
     * only and shall not be visible to the end user.
     * Default: False
     */
    bool isPreparatory __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::radio::ims::V1_7::CrsData crsData __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo callProgInfo __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, state) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, index) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, toa) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasIsMpty) == 12, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isMpty) == 13, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasIsMT) == 14, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isMT) == 15, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, mtMultiLineInfo) == 16, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, als) == 40, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasIsVoice) == 44, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isVoice) == 45, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasIsVoicePrivacy) == 46, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isVoicePrivacy) == 47, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, number) == 48, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, numberPresentation) == 64, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, name) == 72, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, namePresentation) == 88, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasCallDetails) == 92, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, callDetails) == 96, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasFailCause) == 192, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, failCause) == 200, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasIsEncrypted) == 272, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isEncrypted) == 273, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasIsCalledPartyRinging) == 274, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isCalledPartyRinging) == 275, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, historyInfo) == 280, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, hasIsVideoConfSupported) == 296, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isVideoConfSupported) == 297, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, verstatInfo) == 300, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, tirMode) == 308, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, isPreparatory) == 312, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, crsData) == 316, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo, callProgInfo) == 328, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo) == 352, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::ims::V1_7::CallInfo) == 8, "wrong alignment");

//
// type declarations for package
//

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::ims::V1_7::CrsType o);
static inline void PrintTo(::vendor::qti::hardware::radio::ims::V1_7::CrsType o, ::std::ostream* os);
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::ims::V1_7::CrsType lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::ims::V1_7::CrsType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::ims::V1_7::CrsType lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::ims::V1_7::CrsType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::qti::hardware::radio::ims::V1_7::CrsType e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::qti::hardware::radio::ims::V1_7::CrsType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& rhs);

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType o);
static inline void PrintTo(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType o, ::std::ostream* os);
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& rhs);

//
// type header definitions for package
//

template<>
inline std::string toString<::vendor::qti::hardware::radio::ims::V1_7::CrsType>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::ims::V1_7::CrsType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CrsType::INVALID) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CrsType::INVALID)) {
        os += (first ? "" : " | ");
        os += "INVALID";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CrsType::INVALID;
    }
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CrsType::AUDIO) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CrsType::AUDIO)) {
        os += (first ? "" : " | ");
        os += "AUDIO";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CrsType::AUDIO;
    }
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CrsType::VIDEO) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CrsType::VIDEO)) {
        os += (first ? "" : " | ");
        os += "VIDEO";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CrsType::VIDEO;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::ims::V1_7::CrsType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CrsType::INVALID) {
        return "INVALID";
    }
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CrsType::AUDIO) {
        return "AUDIO";
    }
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CrsType::VIDEO) {
        return "VIDEO";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::ims::V1_7::CrsType o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".type = ";
    os += ::vendor::qti::hardware::radio::ims::V1_7::toString<::vendor::qti::hardware::radio::ims::V1_7::CrsType>(o.type);
    os += ", .originalCallType = ";
    os += ::vendor::qti::hardware::radio::ims::V1_0::toString(o.originalCallType);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& rhs) {
    if (lhs.type != rhs.type) {
        return false;
    }
    if (lhs.originalCallType != rhs.originalCallType) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CrsData& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::INVALID) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::INVALID)) {
        os += (first ? "" : " | ");
        os += "INVALID";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::INVALID;
    }
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_REJ_Q850) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_REJ_Q850)) {
        os += (first ? "" : " | ");
        os += "CALL_REJ_Q850";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_REJ_Q850;
    }
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_WAITING) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_WAITING)) {
        os += (first ? "" : " | ");
        os += "CALL_WAITING";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_WAITING;
    }
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_FORWARDING) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_FORWARDING)) {
        os += (first ? "" : " | ");
        os += "CALL_FORWARDING";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_FORWARDING;
    }
    if ((o & ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::REMOTE_AVAILABLE) == static_cast<int32_t>(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::REMOTE_AVAILABLE)) {
        os += (first ? "" : " | ");
        os += "REMOTE_AVAILABLE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::REMOTE_AVAILABLE;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::INVALID) {
        return "INVALID";
    }
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_REJ_Q850) {
        return "CALL_REJ_Q850";
    }
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_WAITING) {
        return "CALL_WAITING";
    }
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_FORWARDING) {
        return "CALL_FORWARDING";
    }
    if (o == ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::REMOTE_AVAILABLE) {
        return "REMOTE_AVAILABLE";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".type = ";
    os += ::vendor::qti::hardware::radio::ims::V1_7::toString(o.type);
    os += ", .reasonCode = ";
    os += ::android::hardware::toString(o.reasonCode);
    os += ", .reasonText = ";
    os += ::android::hardware::toString(o.reasonText);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& rhs) {
    if (lhs.type != rhs.type) {
        return false;
    }
    if (lhs.reasonCode != rhs.reasonCode) {
        return false;
    }
    if (lhs.reasonText != rhs.reasonText) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfo& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".state = ";
    os += ::vendor::qti::hardware::radio::ims::V1_0::toString(o.state);
    os += ", .index = ";
    os += ::android::hardware::toString(o.index);
    os += ", .toa = ";
    os += ::android::hardware::toString(o.toa);
    os += ", .hasIsMpty = ";
    os += ::android::hardware::toString(o.hasIsMpty);
    os += ", .isMpty = ";
    os += ::android::hardware::toString(o.isMpty);
    os += ", .hasIsMT = ";
    os += ::android::hardware::toString(o.hasIsMT);
    os += ", .isMT = ";
    os += ::android::hardware::toString(o.isMT);
    os += ", .mtMultiLineInfo = ";
    os += ::vendor::qti::hardware::radio::ims::V1_4::toString(o.mtMultiLineInfo);
    os += ", .als = ";
    os += ::android::hardware::toString(o.als);
    os += ", .hasIsVoice = ";
    os += ::android::hardware::toString(o.hasIsVoice);
    os += ", .isVoice = ";
    os += ::android::hardware::toString(o.isVoice);
    os += ", .hasIsVoicePrivacy = ";
    os += ::android::hardware::toString(o.hasIsVoicePrivacy);
    os += ", .isVoicePrivacy = ";
    os += ::android::hardware::toString(o.isVoicePrivacy);
    os += ", .number = ";
    os += ::android::hardware::toString(o.number);
    os += ", .numberPresentation = ";
    os += ::android::hardware::toString(o.numberPresentation);
    os += ", .name = ";
    os += ::android::hardware::toString(o.name);
    os += ", .namePresentation = ";
    os += ::android::hardware::toString(o.namePresentation);
    os += ", .hasCallDetails = ";
    os += ::android::hardware::toString(o.hasCallDetails);
    os += ", .callDetails = ";
    os += ::vendor::qti::hardware::radio::ims::V1_6::toString(o.callDetails);
    os += ", .hasFailCause = ";
    os += ::android::hardware::toString(o.hasFailCause);
    os += ", .failCause = ";
    os += ::vendor::qti::hardware::radio::ims::V1_5::toString(o.failCause);
    os += ", .hasIsEncrypted = ";
    os += ::android::hardware::toString(o.hasIsEncrypted);
    os += ", .isEncrypted = ";
    os += ::android::hardware::toString(o.isEncrypted);
    os += ", .hasIsCalledPartyRinging = ";
    os += ::android::hardware::toString(o.hasIsCalledPartyRinging);
    os += ", .isCalledPartyRinging = ";
    os += ::android::hardware::toString(o.isCalledPartyRinging);
    os += ", .historyInfo = ";
    os += ::android::hardware::toString(o.historyInfo);
    os += ", .hasIsVideoConfSupported = ";
    os += ::android::hardware::toString(o.hasIsVideoConfSupported);
    os += ", .isVideoConfSupported = ";
    os += ::android::hardware::toString(o.isVideoConfSupported);
    os += ", .verstatInfo = ";
    os += ::vendor::qti::hardware::radio::ims::V1_3::toString(o.verstatInfo);
    os += ", .tirMode = ";
    os += ::vendor::qti::hardware::radio::ims::V1_5::toString(o.tirMode);
    os += ", .isPreparatory = ";
    os += ::android::hardware::toString(o.isPreparatory);
    os += ", .crsData = ";
    os += ::vendor::qti::hardware::radio::ims::V1_7::toString(o.crsData);
    os += ", .callProgInfo = ";
    os += ::vendor::qti::hardware::radio::ims::V1_7::toString(o.callProgInfo);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& rhs) {
    if (lhs.state != rhs.state) {
        return false;
    }
    if (lhs.index != rhs.index) {
        return false;
    }
    if (lhs.toa != rhs.toa) {
        return false;
    }
    if (lhs.hasIsMpty != rhs.hasIsMpty) {
        return false;
    }
    if (lhs.isMpty != rhs.isMpty) {
        return false;
    }
    if (lhs.hasIsMT != rhs.hasIsMT) {
        return false;
    }
    if (lhs.isMT != rhs.isMT) {
        return false;
    }
    if (lhs.mtMultiLineInfo != rhs.mtMultiLineInfo) {
        return false;
    }
    if (lhs.als != rhs.als) {
        return false;
    }
    if (lhs.hasIsVoice != rhs.hasIsVoice) {
        return false;
    }
    if (lhs.isVoice != rhs.isVoice) {
        return false;
    }
    if (lhs.hasIsVoicePrivacy != rhs.hasIsVoicePrivacy) {
        return false;
    }
    if (lhs.isVoicePrivacy != rhs.isVoicePrivacy) {
        return false;
    }
    if (lhs.number != rhs.number) {
        return false;
    }
    if (lhs.numberPresentation != rhs.numberPresentation) {
        return false;
    }
    if (lhs.name != rhs.name) {
        return false;
    }
    if (lhs.namePresentation != rhs.namePresentation) {
        return false;
    }
    if (lhs.hasCallDetails != rhs.hasCallDetails) {
        return false;
    }
    if (lhs.callDetails != rhs.callDetails) {
        return false;
    }
    if (lhs.hasFailCause != rhs.hasFailCause) {
        return false;
    }
    if (lhs.failCause != rhs.failCause) {
        return false;
    }
    if (lhs.hasIsEncrypted != rhs.hasIsEncrypted) {
        return false;
    }
    if (lhs.isEncrypted != rhs.isEncrypted) {
        return false;
    }
    if (lhs.hasIsCalledPartyRinging != rhs.hasIsCalledPartyRinging) {
        return false;
    }
    if (lhs.isCalledPartyRinging != rhs.isCalledPartyRinging) {
        return false;
    }
    if (lhs.historyInfo != rhs.historyInfo) {
        return false;
    }
    if (lhs.hasIsVideoConfSupported != rhs.hasIsVideoConfSupported) {
        return false;
    }
    if (lhs.isVideoConfSupported != rhs.isVideoConfSupported) {
        return false;
    }
    if (lhs.verstatInfo != rhs.verstatInfo) {
        return false;
    }
    if (lhs.tirMode != rhs.tirMode) {
        return false;
    }
    if (lhs.isPreparatory != rhs.isPreparatory) {
        return false;
    }
    if (lhs.crsData != rhs.crsData) {
        return false;
    }
    if (lhs.callProgInfo != rhs.callProgInfo) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& lhs, const ::vendor::qti::hardware::radio::ims::V1_7::CallInfo& rhs){
    return !(lhs == rhs);
}


}  // namespace V1_7
}  // namespace ims
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::ims::V1_7::CrsType, 3> hidl_enum_values<::vendor::qti::hardware::radio::ims::V1_7::CrsType> = {
    ::vendor::qti::hardware::radio::ims::V1_7::CrsType::INVALID,
    ::vendor::qti::hardware::radio::ims::V1_7::CrsType::AUDIO,
    ::vendor::qti::hardware::radio::ims::V1_7::CrsType::VIDEO,
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType, 5> hidl_enum_values<::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType> = {
    ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::INVALID,
    ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_REJ_Q850,
    ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_WAITING,
    ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::CALL_FORWARDING,
    ::vendor::qti::hardware::radio::ims::V1_7::CallProgressInfoType::REMOTE_AVAILABLE,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_IMS_V1_7_TYPES_H
