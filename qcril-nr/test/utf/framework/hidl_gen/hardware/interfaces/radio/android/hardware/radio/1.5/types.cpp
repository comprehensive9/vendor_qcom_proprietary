#define LOG_TAG "android.hardware.radio@1.5::types"

#include <android/hardware/radio/1.5/types.h>
#include <log/log.h>
#include <cutils/trace.h>
#ifndef QMI_RIL_UTF
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <android/hardware/radio/1.5/hwtypes.h>
#endif

namespace android {
namespace hardware {
namespace radio {
namespace V1_5 {

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::Bands() {
    static_assert(offsetof(::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 24

    hidl_d = hidl_discriminator::geranBands;
    new (&hidl_u.geranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>();
}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::~Bands() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::Bands(Bands&& other) : ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands() {
    switch (other.hidl_d) {
        case hidl_discriminator::geranBands: {
            new (&hidl_u.geranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>(std::move(other.hidl_u.geranBands));
            break;
        }
        case hidl_discriminator::utranBands: {
            new (&hidl_u.utranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>(std::move(other.hidl_u.utranBands));
            break;
        }
        case hidl_discriminator::eutranBands: {
            new (&hidl_u.eutranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>(std::move(other.hidl_u.eutranBands));
            break;
        }
        case hidl_discriminator::ngranBands: {
            new (&hidl_u.ngranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>(std::move(other.hidl_u.ngranBands));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::Bands(const Bands& other) : ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands() {
    switch (other.hidl_d) {
        case hidl_discriminator::geranBands: {
            new (&hidl_u.geranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>(other.hidl_u.geranBands);
            break;
        }
        case hidl_discriminator::utranBands: {
            new (&hidl_u.utranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>(other.hidl_u.utranBands);
            break;
        }
        case hidl_discriminator::eutranBands: {
            new (&hidl_u.eutranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>(other.hidl_u.eutranBands);
            break;
        }
        case hidl_discriminator::ngranBands: {
            new (&hidl_u.ngranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>(other.hidl_u.ngranBands);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::operator=)(Bands&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::geranBands: {
            geranBands(std::move(other.hidl_u.geranBands));
            break;
        }
        case hidl_discriminator::utranBands: {
            utranBands(std::move(other.hidl_u.utranBands));
            break;
        }
        case hidl_discriminator::eutranBands: {
            eutranBands(std::move(other.hidl_u.eutranBands));
            break;
        }
        case hidl_discriminator::ngranBands: {
            ngranBands(std::move(other.hidl_u.ngranBands));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::operator=)(const Bands& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::geranBands: {
            geranBands(other.hidl_u.geranBands);
            break;
        }
        case hidl_discriminator::utranBands: {
            utranBands(other.hidl_u.utranBands);
            break;
        }
        case hidl_discriminator::eutranBands: {
            eutranBands(other.hidl_u.eutranBands);
            break;
        }
        case hidl_discriminator::ngranBands: {
            ngranBands(other.hidl_u.ngranBands);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::geranBands: {
            ::android::hardware::details::destructElement(&(hidl_u.geranBands));
            break;
        }
        case hidl_discriminator::utranBands: {
            ::android::hardware::details::destructElement(&(hidl_u.utranBands));
            break;
        }
        case hidl_discriminator::eutranBands: {
            ::android::hardware::details::destructElement(&(hidl_u.eutranBands));
            break;
        }
        case hidl_discriminator::ngranBands: {
            ::android::hardware::details::destructElement(&(hidl_u.ngranBands));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::geranBands(const ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>& o) {
    if (hidl_d != hidl_discriminator::geranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.geranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>(o);
        hidl_d = hidl_discriminator::geranBands;
    }
    else if (&(hidl_u.geranBands) != &o) {
        hidl_u.geranBands = o;
    }
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::geranBands(::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>&& o) {
    if (hidl_d != hidl_discriminator::geranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.geranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>(std::move(o));
        hidl_d = hidl_discriminator::geranBands;
    }
    else if (&(hidl_u.geranBands) != &o) {
        hidl_u.geranBands = std::move(o);
    }
}

::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::geranBands)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::geranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::geranBands));
    }

    return hidl_u.geranBands;
}

const ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::GeranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::geranBands)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::geranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::geranBands));
    }

    return hidl_u.geranBands;
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::utranBands(const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>& o) {
    if (hidl_d != hidl_discriminator::utranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.utranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>(o);
        hidl_d = hidl_discriminator::utranBands;
    }
    else if (&(hidl_u.utranBands) != &o) {
        hidl_u.utranBands = o;
    }
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::utranBands(::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>&& o) {
    if (hidl_d != hidl_discriminator::utranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.utranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>(std::move(o));
        hidl_d = hidl_discriminator::utranBands;
    }
    else if (&(hidl_u.utranBands) != &o) {
        hidl_u.utranBands = std::move(o);
    }
}

::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::utranBands)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::utranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::utranBands));
    }

    return hidl_u.utranBands;
}

const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::UtranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::utranBands)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::utranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::utranBands));
    }

    return hidl_u.utranBands;
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::eutranBands(const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>& o) {
    if (hidl_d != hidl_discriminator::eutranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eutranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>(o);
        hidl_d = hidl_discriminator::eutranBands;
    }
    else if (&(hidl_u.eutranBands) != &o) {
        hidl_u.eutranBands = o;
    }
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::eutranBands(::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>&& o) {
    if (hidl_d != hidl_discriminator::eutranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eutranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>(std::move(o));
        hidl_d = hidl_discriminator::eutranBands;
    }
    else if (&(hidl_u.eutranBands) != &o) {
        hidl_u.eutranBands = std::move(o);
    }
}

::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::eutranBands)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranBands));
    }

    return hidl_u.eutranBands;
}

const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::EutranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::eutranBands)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranBands));
    }

    return hidl_u.eutranBands;
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::ngranBands(const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>& o) {
    if (hidl_d != hidl_discriminator::ngranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.ngranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>(o);
        hidl_d = hidl_discriminator::ngranBands;
    }
    else if (&(hidl_u.ngranBands) != &o) {
        hidl_u.ngranBands = o;
    }
}

void ::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::ngranBands(::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>&& o) {
    if (hidl_d != hidl_discriminator::ngranBands) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.ngranBands) ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>(std::move(o));
        hidl_d = hidl_discriminator::ngranBands;
    }
    else if (&(hidl_u.ngranBands) != &o) {
        hidl_u.ngranBands = std::move(o);
    }
}

::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::ngranBands)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::ngranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::ngranBands));
    }

    return hidl_u.ngranBands;
}

const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::NgranBands>& (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::ngranBands)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::ngranBands)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::ngranBands));
    }

    return hidl_u.ngranBands;
}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::hidl_union::hidl_union() {}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::hidl_discriminator (::android::hardware::radio::V1_5::RadioAccessSpecifier::Bands::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_5::OptionalCsgInfo::OptionalCsgInfo() {
    static_assert(offsetof(::android::hardware::radio::V1_5::OptionalCsgInfo, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_5::OptionalCsgInfo, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 40

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_5::OptionalCsgInfo::~OptionalCsgInfo() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_5::OptionalCsgInfo::OptionalCsgInfo(OptionalCsgInfo&& other) : ::android::hardware::radio::V1_5::OptionalCsgInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::csgInfo: {
            new (&hidl_u.csgInfo) ::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo(std::move(other.hidl_u.csgInfo));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::OptionalCsgInfo::OptionalCsgInfo(const OptionalCsgInfo& other) : ::android::hardware::radio::V1_5::OptionalCsgInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::csgInfo: {
            new (&hidl_u.csgInfo) ::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo(other.hidl_u.csgInfo);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::OptionalCsgInfo& (::android::hardware::radio::V1_5::OptionalCsgInfo::operator=)(OptionalCsgInfo&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::csgInfo: {
            csgInfo(std::move(other.hidl_u.csgInfo));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_5::OptionalCsgInfo& (::android::hardware::radio::V1_5::OptionalCsgInfo::operator=)(const OptionalCsgInfo& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::csgInfo: {
            csgInfo(other.hidl_u.csgInfo);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_5::OptionalCsgInfo::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::csgInfo: {
            ::android::hardware::details::destructElement(&(hidl_u.csgInfo));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_5::OptionalCsgInfo::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_5::OptionalCsgInfo::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::OptionalCsgInfo::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::OptionalCsgInfo::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_5::OptionalCsgInfo::csgInfo(const ::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo& o) {
    if (hidl_d != hidl_discriminator::csgInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.csgInfo) ::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo(o);
        hidl_d = hidl_discriminator::csgInfo;
    }
    else if (&(hidl_u.csgInfo) != &o) {
        hidl_u.csgInfo = o;
    }
}

void ::android::hardware::radio::V1_5::OptionalCsgInfo::csgInfo(::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo&& o) {
    if (hidl_d != hidl_discriminator::csgInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.csgInfo) ::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo(std::move(o));
        hidl_d = hidl_discriminator::csgInfo;
    }
    else if (&(hidl_u.csgInfo) != &o) {
        hidl_u.csgInfo = std::move(o);
    }
}

::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo& (::android::hardware::radio::V1_5::OptionalCsgInfo::csgInfo)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::csgInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::csgInfo));
    }

    return hidl_u.csgInfo;
}

const ::android::hardware::radio::V1_5::ClosedSubscriberGroupInfo& (::android::hardware::radio::V1_5::OptionalCsgInfo::csgInfo)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::csgInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::csgInfo));
    }

    return hidl_u.csgInfo;
}

::android::hardware::radio::V1_5::OptionalCsgInfo::hidl_union::hidl_union() {}

::android::hardware::radio::V1_5::OptionalCsgInfo::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_5::OptionalCsgInfo::hidl_discriminator (::android::hardware::radio::V1_5::OptionalCsgInfo::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::CellInfoRatSpecificInfo() {
    static_assert(offsetof(::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 192

    hidl_d = hidl_discriminator::gsm;
    new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm();
}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::~CellInfoRatSpecificInfo() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::CellInfoRatSpecificInfo(CellInfoRatSpecificInfo&& other) : ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::gsm: {
            new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm(std::move(other.hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellInfoWcdma(std::move(other.hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellInfoTdscdma(std::move(other.hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::lte: {
            new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellInfoLte(std::move(other.hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellInfoNr(std::move(other.hidl_u.nr));
            break;
        }
        case hidl_discriminator::cdma: {
            new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellInfoCdma(std::move(other.hidl_u.cdma));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::CellInfoRatSpecificInfo(const CellInfoRatSpecificInfo& other) : ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::gsm: {
            new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm(other.hidl_u.gsm);
            break;
        }
        case hidl_discriminator::wcdma: {
            new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellInfoWcdma(other.hidl_u.wcdma);
            break;
        }
        case hidl_discriminator::tdscdma: {
            new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellInfoTdscdma(other.hidl_u.tdscdma);
            break;
        }
        case hidl_discriminator::lte: {
            new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellInfoLte(other.hidl_u.lte);
            break;
        }
        case hidl_discriminator::nr: {
            new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellInfoNr(other.hidl_u.nr);
            break;
        }
        case hidl_discriminator::cdma: {
            new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellInfoCdma(other.hidl_u.cdma);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::operator=)(CellInfoRatSpecificInfo&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::gsm: {
            gsm(std::move(other.hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            wcdma(std::move(other.hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            tdscdma(std::move(other.hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::lte: {
            lte(std::move(other.hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            nr(std::move(other.hidl_u.nr));
            break;
        }
        case hidl_discriminator::cdma: {
            cdma(std::move(other.hidl_u.cdma));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::operator=)(const CellInfoRatSpecificInfo& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::gsm: {
            gsm(other.hidl_u.gsm);
            break;
        }
        case hidl_discriminator::wcdma: {
            wcdma(other.hidl_u.wcdma);
            break;
        }
        case hidl_discriminator::tdscdma: {
            tdscdma(other.hidl_u.tdscdma);
            break;
        }
        case hidl_discriminator::lte: {
            lte(other.hidl_u.lte);
            break;
        }
        case hidl_discriminator::nr: {
            nr(other.hidl_u.nr);
            break;
        }
        case hidl_discriminator::cdma: {
            cdma(other.hidl_u.cdma);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::gsm: {
            ::android::hardware::details::destructElement(&(hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            ::android::hardware::details::destructElement(&(hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            ::android::hardware::details::destructElement(&(hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::lte: {
            ::android::hardware::details::destructElement(&(hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            ::android::hardware::details::destructElement(&(hidl_u.nr));
            break;
        }
        case hidl_discriminator::cdma: {
            ::android::hardware::details::destructElement(&(hidl_u.cdma));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::gsm(const ::android::hardware::radio::V1_5::CellInfoGsm& o) {
    if (hidl_d != hidl_discriminator::gsm) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm(o);
        hidl_d = hidl_discriminator::gsm;
    }
    else if (&(hidl_u.gsm) != &o) {
        hidl_u.gsm = o;
    }
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::gsm(::android::hardware::radio::V1_5::CellInfoGsm&& o) {
    if (hidl_d != hidl_discriminator::gsm) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellInfoGsm(std::move(o));
        hidl_d = hidl_discriminator::gsm;
    }
    else if (&(hidl_u.gsm) != &o) {
        hidl_u.gsm = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoGsm& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::gsm)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::gsm)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::gsm));
    }

    return hidl_u.gsm;
}

const ::android::hardware::radio::V1_5::CellInfoGsm& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::gsm)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::gsm)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::gsm));
    }

    return hidl_u.gsm;
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::wcdma(const ::android::hardware::radio::V1_5::CellInfoWcdma& o) {
    if (hidl_d != hidl_discriminator::wcdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellInfoWcdma(o);
        hidl_d = hidl_discriminator::wcdma;
    }
    else if (&(hidl_u.wcdma) != &o) {
        hidl_u.wcdma = o;
    }
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::wcdma(::android::hardware::radio::V1_5::CellInfoWcdma&& o) {
    if (hidl_d != hidl_discriminator::wcdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellInfoWcdma(std::move(o));
        hidl_d = hidl_discriminator::wcdma;
    }
    else if (&(hidl_u.wcdma) != &o) {
        hidl_u.wcdma = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoWcdma& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::wcdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::wcdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::wcdma));
    }

    return hidl_u.wcdma;
}

const ::android::hardware::radio::V1_5::CellInfoWcdma& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::wcdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::wcdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::wcdma));
    }

    return hidl_u.wcdma;
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::tdscdma(const ::android::hardware::radio::V1_5::CellInfoTdscdma& o) {
    if (hidl_d != hidl_discriminator::tdscdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellInfoTdscdma(o);
        hidl_d = hidl_discriminator::tdscdma;
    }
    else if (&(hidl_u.tdscdma) != &o) {
        hidl_u.tdscdma = o;
    }
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::tdscdma(::android::hardware::radio::V1_5::CellInfoTdscdma&& o) {
    if (hidl_d != hidl_discriminator::tdscdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellInfoTdscdma(std::move(o));
        hidl_d = hidl_discriminator::tdscdma;
    }
    else if (&(hidl_u.tdscdma) != &o) {
        hidl_u.tdscdma = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoTdscdma& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::tdscdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::tdscdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::tdscdma));
    }

    return hidl_u.tdscdma;
}

const ::android::hardware::radio::V1_5::CellInfoTdscdma& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::tdscdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::tdscdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::tdscdma));
    }

    return hidl_u.tdscdma;
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::lte(const ::android::hardware::radio::V1_5::CellInfoLte& o) {
    if (hidl_d != hidl_discriminator::lte) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellInfoLte(o);
        hidl_d = hidl_discriminator::lte;
    }
    else if (&(hidl_u.lte) != &o) {
        hidl_u.lte = o;
    }
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::lte(::android::hardware::radio::V1_5::CellInfoLte&& o) {
    if (hidl_d != hidl_discriminator::lte) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellInfoLte(std::move(o));
        hidl_d = hidl_discriminator::lte;
    }
    else if (&(hidl_u.lte) != &o) {
        hidl_u.lte = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoLte& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::lte)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::lte)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::lte));
    }

    return hidl_u.lte;
}

const ::android::hardware::radio::V1_5::CellInfoLte& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::lte)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::lte)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::lte));
    }

    return hidl_u.lte;
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::nr(const ::android::hardware::radio::V1_5::CellInfoNr& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellInfoNr(o);
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = o;
    }
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::nr(::android::hardware::radio::V1_5::CellInfoNr&& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellInfoNr(std::move(o));
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellInfoNr& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::nr)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

const ::android::hardware::radio::V1_5::CellInfoNr& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::nr)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::cdma(const ::android::hardware::radio::V1_2::CellInfoCdma& o) {
    if (hidl_d != hidl_discriminator::cdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellInfoCdma(o);
        hidl_d = hidl_discriminator::cdma;
    }
    else if (&(hidl_u.cdma) != &o) {
        hidl_u.cdma = o;
    }
}

void ::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::cdma(::android::hardware::radio::V1_2::CellInfoCdma&& o) {
    if (hidl_d != hidl_discriminator::cdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellInfoCdma(std::move(o));
        hidl_d = hidl_discriminator::cdma;
    }
    else if (&(hidl_u.cdma) != &o) {
        hidl_u.cdma = std::move(o);
    }
}

::android::hardware::radio::V1_2::CellInfoCdma& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::cdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdma));
    }

    return hidl_u.cdma;
}

const ::android::hardware::radio::V1_2::CellInfoCdma& (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::cdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdma));
    }

    return hidl_u.cdma;
}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_union::hidl_union() {}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator (::android::hardware::radio::V1_5::CellInfo::CellInfoRatSpecificInfo::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_5::CellIdentity::CellIdentity() {
    static_assert(offsetof(::android::hardware::radio::V1_5::CellIdentity, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_5::CellIdentity, hidl_u) == 8, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 7);
    // no padding to zero starting at offset 168

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_5::CellIdentity::~CellIdentity() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_5::CellIdentity::CellIdentity(CellIdentity&& other) : ::android::hardware::radio::V1_5::CellIdentity() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::gsm: {
            new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellIdentityGsm(std::move(other.hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellIdentityWcdma(std::move(other.hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellIdentityTdscdma(std::move(other.hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::cdma: {
            new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellIdentityCdma(std::move(other.hidl_u.cdma));
            break;
        }
        case hidl_discriminator::lte: {
            new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellIdentityLte(std::move(other.hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellIdentityNr(std::move(other.hidl_u.nr));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::CellIdentity::CellIdentity(const CellIdentity& other) : ::android::hardware::radio::V1_5::CellIdentity() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::gsm: {
            new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellIdentityGsm(other.hidl_u.gsm);
            break;
        }
        case hidl_discriminator::wcdma: {
            new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellIdentityWcdma(other.hidl_u.wcdma);
            break;
        }
        case hidl_discriminator::tdscdma: {
            new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellIdentityTdscdma(other.hidl_u.tdscdma);
            break;
        }
        case hidl_discriminator::cdma: {
            new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellIdentityCdma(other.hidl_u.cdma);
            break;
        }
        case hidl_discriminator::lte: {
            new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellIdentityLte(other.hidl_u.lte);
            break;
        }
        case hidl_discriminator::nr: {
            new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellIdentityNr(other.hidl_u.nr);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::CellIdentity& (::android::hardware::radio::V1_5::CellIdentity::operator=)(CellIdentity&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::gsm: {
            gsm(std::move(other.hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            wcdma(std::move(other.hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            tdscdma(std::move(other.hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::cdma: {
            cdma(std::move(other.hidl_u.cdma));
            break;
        }
        case hidl_discriminator::lte: {
            lte(std::move(other.hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            nr(std::move(other.hidl_u.nr));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_5::CellIdentity& (::android::hardware::radio::V1_5::CellIdentity::operator=)(const CellIdentity& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::gsm: {
            gsm(other.hidl_u.gsm);
            break;
        }
        case hidl_discriminator::wcdma: {
            wcdma(other.hidl_u.wcdma);
            break;
        }
        case hidl_discriminator::tdscdma: {
            tdscdma(other.hidl_u.tdscdma);
            break;
        }
        case hidl_discriminator::cdma: {
            cdma(other.hidl_u.cdma);
            break;
        }
        case hidl_discriminator::lte: {
            lte(other.hidl_u.lte);
            break;
        }
        case hidl_discriminator::nr: {
            nr(other.hidl_u.nr);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_5::CellIdentity::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::gsm: {
            ::android::hardware::details::destructElement(&(hidl_u.gsm));
            break;
        }
        case hidl_discriminator::wcdma: {
            ::android::hardware::details::destructElement(&(hidl_u.wcdma));
            break;
        }
        case hidl_discriminator::tdscdma: {
            ::android::hardware::details::destructElement(&(hidl_u.tdscdma));
            break;
        }
        case hidl_discriminator::cdma: {
            ::android::hardware::details::destructElement(&(hidl_u.cdma));
            break;
        }
        case hidl_discriminator::lte: {
            ::android::hardware::details::destructElement(&(hidl_u.lte));
            break;
        }
        case hidl_discriminator::nr: {
            ::android::hardware::details::destructElement(&(hidl_u.nr));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_5::CellIdentity::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_5::CellIdentity::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::CellIdentity::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::CellIdentity::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_5::CellIdentity::gsm(const ::android::hardware::radio::V1_5::CellIdentityGsm& o) {
    if (hidl_d != hidl_discriminator::gsm) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellIdentityGsm(o);
        hidl_d = hidl_discriminator::gsm;
    }
    else if (&(hidl_u.gsm) != &o) {
        hidl_u.gsm = o;
    }
}

void ::android::hardware::radio::V1_5::CellIdentity::gsm(::android::hardware::radio::V1_5::CellIdentityGsm&& o) {
    if (hidl_d != hidl_discriminator::gsm) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.gsm) ::android::hardware::radio::V1_5::CellIdentityGsm(std::move(o));
        hidl_d = hidl_discriminator::gsm;
    }
    else if (&(hidl_u.gsm) != &o) {
        hidl_u.gsm = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellIdentityGsm& (::android::hardware::radio::V1_5::CellIdentity::gsm)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::gsm)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::gsm));
    }

    return hidl_u.gsm;
}

const ::android::hardware::radio::V1_5::CellIdentityGsm& (::android::hardware::radio::V1_5::CellIdentity::gsm)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::gsm)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::gsm));
    }

    return hidl_u.gsm;
}

void ::android::hardware::radio::V1_5::CellIdentity::wcdma(const ::android::hardware::radio::V1_5::CellIdentityWcdma& o) {
    if (hidl_d != hidl_discriminator::wcdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellIdentityWcdma(o);
        hidl_d = hidl_discriminator::wcdma;
    }
    else if (&(hidl_u.wcdma) != &o) {
        hidl_u.wcdma = o;
    }
}

void ::android::hardware::radio::V1_5::CellIdentity::wcdma(::android::hardware::radio::V1_5::CellIdentityWcdma&& o) {
    if (hidl_d != hidl_discriminator::wcdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.wcdma) ::android::hardware::radio::V1_5::CellIdentityWcdma(std::move(o));
        hidl_d = hidl_discriminator::wcdma;
    }
    else if (&(hidl_u.wcdma) != &o) {
        hidl_u.wcdma = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellIdentityWcdma& (::android::hardware::radio::V1_5::CellIdentity::wcdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::wcdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::wcdma));
    }

    return hidl_u.wcdma;
}

const ::android::hardware::radio::V1_5::CellIdentityWcdma& (::android::hardware::radio::V1_5::CellIdentity::wcdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::wcdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::wcdma));
    }

    return hidl_u.wcdma;
}

void ::android::hardware::radio::V1_5::CellIdentity::tdscdma(const ::android::hardware::radio::V1_5::CellIdentityTdscdma& o) {
    if (hidl_d != hidl_discriminator::tdscdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellIdentityTdscdma(o);
        hidl_d = hidl_discriminator::tdscdma;
    }
    else if (&(hidl_u.tdscdma) != &o) {
        hidl_u.tdscdma = o;
    }
}

void ::android::hardware::radio::V1_5::CellIdentity::tdscdma(::android::hardware::radio::V1_5::CellIdentityTdscdma&& o) {
    if (hidl_d != hidl_discriminator::tdscdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.tdscdma) ::android::hardware::radio::V1_5::CellIdentityTdscdma(std::move(o));
        hidl_d = hidl_discriminator::tdscdma;
    }
    else if (&(hidl_u.tdscdma) != &o) {
        hidl_u.tdscdma = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellIdentityTdscdma& (::android::hardware::radio::V1_5::CellIdentity::tdscdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::tdscdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::tdscdma));
    }

    return hidl_u.tdscdma;
}

const ::android::hardware::radio::V1_5::CellIdentityTdscdma& (::android::hardware::radio::V1_5::CellIdentity::tdscdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::tdscdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::tdscdma));
    }

    return hidl_u.tdscdma;
}

void ::android::hardware::radio::V1_5::CellIdentity::cdma(const ::android::hardware::radio::V1_2::CellIdentityCdma& o) {
    if (hidl_d != hidl_discriminator::cdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellIdentityCdma(o);
        hidl_d = hidl_discriminator::cdma;
    }
    else if (&(hidl_u.cdma) != &o) {
        hidl_u.cdma = o;
    }
}

void ::android::hardware::radio::V1_5::CellIdentity::cdma(::android::hardware::radio::V1_2::CellIdentityCdma&& o) {
    if (hidl_d != hidl_discriminator::cdma) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdma) ::android::hardware::radio::V1_2::CellIdentityCdma(std::move(o));
        hidl_d = hidl_discriminator::cdma;
    }
    else if (&(hidl_u.cdma) != &o) {
        hidl_u.cdma = std::move(o);
    }
}

::android::hardware::radio::V1_2::CellIdentityCdma& (::android::hardware::radio::V1_5::CellIdentity::cdma)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdma));
    }

    return hidl_u.cdma;
}

const ::android::hardware::radio::V1_2::CellIdentityCdma& (::android::hardware::radio::V1_5::CellIdentity::cdma)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdma)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdma));
    }

    return hidl_u.cdma;
}

void ::android::hardware::radio::V1_5::CellIdentity::lte(const ::android::hardware::radio::V1_5::CellIdentityLte& o) {
    if (hidl_d != hidl_discriminator::lte) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellIdentityLte(o);
        hidl_d = hidl_discriminator::lte;
    }
    else if (&(hidl_u.lte) != &o) {
        hidl_u.lte = o;
    }
}

void ::android::hardware::radio::V1_5::CellIdentity::lte(::android::hardware::radio::V1_5::CellIdentityLte&& o) {
    if (hidl_d != hidl_discriminator::lte) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.lte) ::android::hardware::radio::V1_5::CellIdentityLte(std::move(o));
        hidl_d = hidl_discriminator::lte;
    }
    else if (&(hidl_u.lte) != &o) {
        hidl_u.lte = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellIdentityLte& (::android::hardware::radio::V1_5::CellIdentity::lte)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::lte)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::lte));
    }

    return hidl_u.lte;
}

const ::android::hardware::radio::V1_5::CellIdentityLte& (::android::hardware::radio::V1_5::CellIdentity::lte)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::lte)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::lte));
    }

    return hidl_u.lte;
}

void ::android::hardware::radio::V1_5::CellIdentity::nr(const ::android::hardware::radio::V1_5::CellIdentityNr& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellIdentityNr(o);
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = o;
    }
}

void ::android::hardware::radio::V1_5::CellIdentity::nr(::android::hardware::radio::V1_5::CellIdentityNr&& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::android::hardware::radio::V1_5::CellIdentityNr(std::move(o));
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = std::move(o);
    }
}

::android::hardware::radio::V1_5::CellIdentityNr& (::android::hardware::radio::V1_5::CellIdentity::nr)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

const ::android::hardware::radio::V1_5::CellIdentityNr& (::android::hardware::radio::V1_5::CellIdentity::nr)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

::android::hardware::radio::V1_5::CellIdentity::hidl_union::hidl_union() {}

::android::hardware::radio::V1_5::CellIdentity::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_5::CellIdentity::hidl_discriminator (::android::hardware::radio::V1_5::CellIdentity::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::BarringTypeSpecificInfo() {
    static_assert(offsetof(::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 16

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::~BarringTypeSpecificInfo() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::BarringTypeSpecificInfo(BarringTypeSpecificInfo&& other) : ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::conditional: {
            new (&hidl_u.conditional) ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional(std::move(other.hidl_u.conditional));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::BarringTypeSpecificInfo(const BarringTypeSpecificInfo& other) : ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::conditional: {
            new (&hidl_u.conditional) ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional(other.hidl_u.conditional);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo& (::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::operator=)(BarringTypeSpecificInfo&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::conditional: {
            conditional(std::move(other.hidl_u.conditional));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo& (::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::operator=)(const BarringTypeSpecificInfo& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::conditional: {
            conditional(other.hidl_u.conditional);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::conditional: {
            ::android::hardware::details::destructElement(&(hidl_u.conditional));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::conditional(const ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional& o) {
    if (hidl_d != hidl_discriminator::conditional) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.conditional) ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional(o);
        hidl_d = hidl_discriminator::conditional;
    }
    else if (&(hidl_u.conditional) != &o) {
        hidl_u.conditional = o;
    }
}

void ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::conditional(::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional&& o) {
    if (hidl_d != hidl_discriminator::conditional) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.conditional) ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional(std::move(o));
        hidl_d = hidl_discriminator::conditional;
    }
    else if (&(hidl_u.conditional) != &o) {
        hidl_u.conditional = std::move(o);
    }
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional& (::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::conditional)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::conditional)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::conditional));
    }

    return hidl_u.conditional;
}

const ::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional& (::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::conditional)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::conditional)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::conditional));
    }

    return hidl_u.conditional;
}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::hidl_union::hidl_union() {}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::hidl_discriminator (::android::hardware::radio::V1_5::BarringInfo::BarringTypeSpecificInfo::getDiscriminator)() const {
    return hidl_d;
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::AccessTechnologySpecificInfo() {
    static_assert(offsetof(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 20

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::~AccessTechnologySpecificInfo() {
    hidl_destructUnion();
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::AccessTechnologySpecificInfo(AccessTechnologySpecificInfo&& other) : ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            new (&hidl_u.cdmaInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo(std::move(other.hidl_u.cdmaInfo));
            break;
        }
        case hidl_discriminator::eutranInfo: {
            new (&hidl_u.eutranInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo(std::move(other.hidl_u.eutranInfo));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::AccessTechnologySpecificInfo(const AccessTechnologySpecificInfo& other) : ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            new (&hidl_u.cdmaInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo(other.hidl_u.cdmaInfo);
            break;
        }
        case hidl_discriminator::eutranInfo: {
            new (&hidl_u.eutranInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo(other.hidl_u.eutranInfo);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }

    hidl_d = other.hidl_d;
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::operator=)(AccessTechnologySpecificInfo&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            cdmaInfo(std::move(other.hidl_u.cdmaInfo));
            break;
        }
        case hidl_discriminator::eutranInfo: {
            eutranInfo(std::move(other.hidl_u.eutranInfo));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::operator=)(const AccessTechnologySpecificInfo& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            cdmaInfo(other.hidl_u.cdmaInfo);
            break;
        }
        case hidl_discriminator::eutranInfo: {
            eutranInfo(other.hidl_u.eutranInfo);
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) other.hidl_d) + ").").c_str());
        }
    }
    return *this;
}

void ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::cdmaInfo: {
            ::android::hardware::details::destructElement(&(hidl_u.cdmaInfo));
            break;
        }
        case hidl_discriminator::eutranInfo: {
            ::android::hardware::details::destructElement(&(hidl_u.eutranInfo));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(o);
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = o;
    }
}

void ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
    if (hidl_d != hidl_discriminator::noinit) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(o));
        hidl_d = hidl_discriminator::noinit;
    }
    else if (&(hidl_u.noinit) != &o) {
        hidl_u.noinit = std::move(o);
    }
}

::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo(const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& o) {
    if (hidl_d != hidl_discriminator::cdmaInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdmaInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo(o);
        hidl_d = hidl_discriminator::cdmaInfo;
    }
    else if (&(hidl_u.cdmaInfo) != &o) {
        hidl_u.cdmaInfo = o;
    }
}

void ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo&& o) {
    if (hidl_d != hidl_discriminator::cdmaInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.cdmaInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo(std::move(o));
        hidl_d = hidl_discriminator::cdmaInfo;
    }
    else if (&(hidl_u.cdmaInfo) != &o) {
        hidl_u.cdmaInfo = std::move(o);
    }
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdmaInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdmaInfo));
    }

    return hidl_u.cdmaInfo;
}

const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::cdmaInfo)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::cdmaInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::cdmaInfo));
    }

    return hidl_u.cdmaInfo;
}

void ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::eutranInfo(const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& o) {
    if (hidl_d != hidl_discriminator::eutranInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eutranInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo(o);
        hidl_d = hidl_discriminator::eutranInfo;
    }
    else if (&(hidl_u.eutranInfo) != &o) {
        hidl_u.eutranInfo = o;
    }
}

void ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::eutranInfo(::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo&& o) {
    if (hidl_d != hidl_discriminator::eutranInfo) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eutranInfo) ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo(std::move(o));
        hidl_d = hidl_discriminator::eutranInfo;
    }
    else if (&(hidl_u.eutranInfo) != &o) {
        hidl_u.eutranInfo = std::move(o);
    }
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::eutranInfo)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranInfo));
    }

    return hidl_u.eutranInfo;
}

const ::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo& (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::eutranInfo)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eutranInfo)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eutranInfo));
    }

    return hidl_u.eutranInfo;
}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::hidl_union::hidl_union() {}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::hidl_union::~hidl_union() {}

::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::hidl_discriminator (::android::hardware::radio::V1_5::RegStateResult::AccessTechnologySpecificInfo::getDiscriminator)() const {
    return hidl_d;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V1_5
}  // namespace radio
}  // namespace hardware
}  // namespace android
