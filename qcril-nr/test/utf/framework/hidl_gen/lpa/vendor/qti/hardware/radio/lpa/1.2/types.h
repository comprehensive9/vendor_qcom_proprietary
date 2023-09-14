#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_LPA_V1_2_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_LPA_V1_2_TYPES_H

#include <vendor/qti/hardware/radio/lpa/1.0/types.h>
#include <vendor/qti/hardware/radio/lpa/1.1/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_2 {

// Forward declaration for forward reference support:
enum class UimLpaUserEventId : uint32_t;
struct UimLpaUserRespV1_2;

enum class UimLpaUserEventId : uint32_t {
    UIM_LPA_UNKNOWN_EVENT_ID = 0u,
    UIM_LPA_ADD_PROFILE = 1u,
    UIM_LPA_ENABLE_PROFILE = 2u,
    UIM_LPA_DISABLE_PROFILE = 3u,
    UIM_LPA_DELETE_PROFILE = 4u,
    UIM_LPA_EUICC_MEMORY_RESET = 5u,
    UIM_LPA_GET_PROFILE = 6u,
    UIM_LPA_UPDATE_NICKNAME = 7u,
    UIM_LPA_GET_EID = 8u,
    UIM_LPA_USER_CONSENT = 9u,
    UIM_LPA_SRV_ADDR_OPERATION = 10u,
    UIM_LPA_CONFIRM_CODE = 11u,
    UIM_LPA_EUICC_INFO2 = 12u,
};

struct UimLpaUserRespV1_2 final {
    ::vendor::qti::hardware::radio::lpa::V1_0::UimLpaUserResp base __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<uint8_t> euicc_info2 __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2, base) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2, euicc_info2) == 72, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2) == 88, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2) == 8, "wrong alignment");

//
// type declarations for package
//

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId o);
static inline void PrintTo(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& rhs);

//
// type header definitions for package
//

template<>
inline std::string toString<::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UNKNOWN_EVENT_ID) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UNKNOWN_EVENT_ID)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_UNKNOWN_EVENT_ID";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UNKNOWN_EVENT_ID;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ADD_PROFILE) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ADD_PROFILE)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_ADD_PROFILE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ADD_PROFILE;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ENABLE_PROFILE) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ENABLE_PROFILE)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_ENABLE_PROFILE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ENABLE_PROFILE;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DISABLE_PROFILE) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DISABLE_PROFILE)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_DISABLE_PROFILE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DISABLE_PROFILE;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DELETE_PROFILE) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DELETE_PROFILE)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_DELETE_PROFILE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DELETE_PROFILE;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_MEMORY_RESET) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_MEMORY_RESET)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_EUICC_MEMORY_RESET";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_MEMORY_RESET;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_PROFILE) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_PROFILE)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_GET_PROFILE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_PROFILE;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UPDATE_NICKNAME) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UPDATE_NICKNAME)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_UPDATE_NICKNAME";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UPDATE_NICKNAME;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_EID) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_EID)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_GET_EID";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_EID;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_USER_CONSENT) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_USER_CONSENT)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_USER_CONSENT";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_USER_CONSENT;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_SRV_ADDR_OPERATION) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_SRV_ADDR_OPERATION)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_SRV_ADDR_OPERATION";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_SRV_ADDR_OPERATION;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_CONFIRM_CODE) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_CONFIRM_CODE)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_CONFIRM_CODE";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_CONFIRM_CODE;
    }
    if ((o & ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_INFO2) == static_cast<uint32_t>(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_INFO2)) {
        os += (first ? "" : " | ");
        os += "UIM_LPA_EUICC_INFO2";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_INFO2;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UNKNOWN_EVENT_ID) {
        return "UIM_LPA_UNKNOWN_EVENT_ID";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ADD_PROFILE) {
        return "UIM_LPA_ADD_PROFILE";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ENABLE_PROFILE) {
        return "UIM_LPA_ENABLE_PROFILE";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DISABLE_PROFILE) {
        return "UIM_LPA_DISABLE_PROFILE";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DELETE_PROFILE) {
        return "UIM_LPA_DELETE_PROFILE";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_MEMORY_RESET) {
        return "UIM_LPA_EUICC_MEMORY_RESET";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_PROFILE) {
        return "UIM_LPA_GET_PROFILE";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UPDATE_NICKNAME) {
        return "UIM_LPA_UPDATE_NICKNAME";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_EID) {
        return "UIM_LPA_GET_EID";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_USER_CONSENT) {
        return "UIM_LPA_USER_CONSENT";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_SRV_ADDR_OPERATION) {
        return "UIM_LPA_SRV_ADDR_OPERATION";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_CONFIRM_CODE) {
        return "UIM_LPA_CONFIRM_CODE";
    }
    if (o == ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_INFO2) {
        return "UIM_LPA_EUICC_INFO2";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".base = ";
    os += ::vendor::qti::hardware::radio::lpa::V1_0::toString(o.base);
    os += ", .euicc_info2 = ";
    os += ::android::hardware::toString(o.euicc_info2);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& rhs) {
    if (lhs.base != rhs.base) {
        return false;
    }
    if (lhs.euicc_info2 != rhs.euicc_info2) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& lhs, const ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserRespV1_2& rhs){
    return !(lhs == rhs);
}


}  // namespace V1_2
}  // namespace lpa
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId, 13> hidl_enum_values<::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId> = {
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UNKNOWN_EVENT_ID,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ADD_PROFILE,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_ENABLE_PROFILE,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DISABLE_PROFILE,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_DELETE_PROFILE,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_MEMORY_RESET,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_PROFILE,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_UPDATE_NICKNAME,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_GET_EID,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_USER_CONSENT,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_SRV_ADDR_OPERATION,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_CONFIRM_CODE,
    ::vendor::qti::hardware::radio::lpa::V1_2::UimLpaUserEventId::UIM_LPA_EUICC_INFO2,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_LPA_V1_2_TYPES_H
