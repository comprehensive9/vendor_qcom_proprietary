/*
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#define LOG_TAG "vendor.qti.hardware.radio.qtiradio@2.7::types"

#include <log/log.h>
#include <cutils/trace.h>
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <vendor/qti/hardware/radio/qtiradio/2.7/types.h>
#include <vendor/qti/hardware/radio/qtiradio/2.7/hwtypes.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace V2_7 {

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::Qos() {
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 28

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::~Qos() {
    hidl_destructUnion();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::Qos(Qos&& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::eps: {
            new (&hidl_u.eps) ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos(std::move(other.hidl_u.eps));
            break;
        }
        case hidl_discriminator::nr: {
            new (&hidl_u.nr) ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos(std::move(other.hidl_u.nr));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::Qos(const Qos& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::eps: {
            new (&hidl_u.eps) ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos(other.hidl_u.eps);
            break;
        }
        case hidl_discriminator::nr: {
            new (&hidl_u.nr) ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos(other.hidl_u.nr);
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

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::operator=)(Qos&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::eps: {
            eps(std::move(other.hidl_u.eps));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::operator=)(const Qos& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::eps: {
            eps(other.hidl_u.eps);
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::eps: {
            ::android::hardware::details::destructElement(&(hidl_u.eps));
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
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

::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::eps(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& o) {
    if (hidl_d != hidl_discriminator::eps) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eps) ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos(o);
        hidl_d = hidl_discriminator::eps;
    }
    else if (&(hidl_u.eps) != &o) {
        hidl_u.eps = o;
    }
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::eps(::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos&& o) {
    if (hidl_d != hidl_discriminator::eps) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.eps) ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos(std::move(o));
        hidl_d = hidl_discriminator::eps;
    }
    else if (&(hidl_u.eps) != &o) {
        hidl_u.eps = std::move(o);
    }
}

::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::eps)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eps)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eps));
    }

    return hidl_u.eps;
}

const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::eps)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::eps)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::eps));
    }

    return hidl_u.eps;
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::nr(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos(o);
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = o;
    }
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::nr(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos&& o) {
    if (hidl_d != hidl_discriminator::nr) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.nr) ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos(std::move(o));
        hidl_d = hidl_discriminator::nr;
    }
    else if (&(hidl_u.nr) != &o) {
        hidl_u.nr = std::move(o);
    }
}

::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::nr)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::nr)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::nr)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::nr));
    }

    return hidl_u.nr;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_union::hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_union::~hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator (::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::getDiscriminator)() const {
    return hidl_d;
}

::android::status_t readEmbeddedFromParcel(
        const QosParametersResult &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_qosSessions_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession> &>(obj.qosSessions),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult, qosSessions), &_hidl_qosSessions_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.qosSessions.size(); ++_hidl_index_0) {
        _hidl_err = readEmbeddedFromParcel(
                const_cast<::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession &>(obj.qosSessions[_hidl_index_0]),
                parcel,
                _hidl_qosSessions_child,
                _hidl_index_0 * sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    return _hidl_err;
}

::android::status_t writeEmbeddedToParcel(
        const QosParametersResult &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_qosSessions_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.qosSessions,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult, qosSessions), &_hidl_qosSessions_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.qosSessions.size(); ++_hidl_index_0) {
        _hidl_err = writeEmbeddedToParcel(
                obj.qosSessions[_hidl_index_0],
                parcel,
                _hidl_qosSessions_child,
                _hidl_index_0 * sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    return _hidl_err;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::MaybePort() {
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 12

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::~MaybePort() {
    hidl_destructUnion();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::MaybePort(MaybePort&& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::range: {
            new (&hidl_u.range) ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange(std::move(other.hidl_u.range));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::MaybePort(const MaybePort& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::range: {
            new (&hidl_u.range) ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange(other.hidl_u.range);
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

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& (::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::operator=)(MaybePort&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::range: {
            range(std::move(other.hidl_u.range));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& (::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::operator=)(const MaybePort& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::range: {
            range(other.hidl_u.range);
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::range: {
            ::android::hardware::details::destructElement(&(hidl_u.range));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
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

::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::range(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& o) {
    if (hidl_d != hidl_discriminator::range) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.range) ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange(o);
        hidl_d = hidl_discriminator::range;
    }
    else if (&(hidl_u.range) != &o) {
        hidl_u.range = o;
    }
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::range(::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange&& o) {
    if (hidl_d != hidl_discriminator::range) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.range) ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange(std::move(o));
        hidl_d = hidl_discriminator::range;
    }
    else if (&(hidl_u.range) != &o) {
        hidl_u.range = std::move(o);
    }
}

::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& (::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::range)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::range)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::range));
    }

    return hidl_u.range;
}

const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& (::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::range)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::range)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::range));
    }

    return hidl_u.range;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_union::hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_union::~hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_discriminator (::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::getDiscriminator)() const {
    return hidl_d;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::TypeOfService() {
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService, hidl_u) == 1, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    // no padding to zero starting at offset 1
    // no padding to zero starting at offset 2

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::~TypeOfService() {
    hidl_destructUnion();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::TypeOfService(TypeOfService&& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            new (&hidl_u.value) uint8_t(std::move(other.hidl_u.value));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::TypeOfService(const TypeOfService& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            new (&hidl_u.value) uint8_t(other.hidl_u.value);
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::operator=)(TypeOfService&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::operator=)(const TypeOfService& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
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

::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::value(uint8_t o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) uint8_t(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

uint8_t& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

uint8_t (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_union::hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_union::~hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_discriminator (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::getDiscriminator)() const {
    return hidl_d;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::Ipv6FlowLabel() {
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 8

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::~Ipv6FlowLabel() {
    hidl_destructUnion();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::Ipv6FlowLabel(Ipv6FlowLabel&& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            new (&hidl_u.value) uint32_t(std::move(other.hidl_u.value));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::Ipv6FlowLabel(const Ipv6FlowLabel& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            new (&hidl_u.value) uint32_t(other.hidl_u.value);
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::operator=)(Ipv6FlowLabel&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::operator=)(const Ipv6FlowLabel& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
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

::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::value(uint32_t o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) uint32_t(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

uint32_t& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

uint32_t (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_union::hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_union::~hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_discriminator (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::getDiscriminator)() const {
    return hidl_d;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::IpsecSpi() {
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi, hidl_d) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi, hidl_u) == 4, "wrong offset");

    ::std::memset(&hidl_u, 0, sizeof(hidl_u));
    ::std::memset(reinterpret_cast<uint8_t*>(this) + 1, 0, 3);
    // no padding to zero starting at offset 8

    hidl_d = hidl_discriminator::noinit;
    new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::~IpsecSpi() {
    hidl_destructUnion();
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::IpsecSpi(IpsecSpi&& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            new (&hidl_u.value) uint32_t(std::move(other.hidl_u.value));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::IpsecSpi(const IpsecSpi& other) : ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi() {
    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            new (&hidl_u.noinit) ::android::hidl::safe_union::V1_0::Monostate(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            new (&hidl_u.value) uint32_t(other.hidl_u.value);
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::operator=)(IpsecSpi&& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(std::move(other.hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            value(std::move(other.hidl_u.value));
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

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::operator=)(const IpsecSpi& other) {
    if (this == &other) { return *this; }

    switch (other.hidl_d) {
        case hidl_discriminator::noinit: {
            noinit(other.hidl_u.noinit);
            break;
        }
        case hidl_discriminator::value: {
            value(other.hidl_u.value);
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_destructUnion() {
    switch (hidl_d) {
        case hidl_discriminator::noinit: {
            ::android::hardware::details::destructElement(&(hidl_u.noinit));
            break;
        }
        case hidl_discriminator::value: {
            ::android::hardware::details::destructElement(&(hidl_u.value));
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) hidl_d) + ").").c_str());
        }
    }

}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::noinit(const ::android::hidl::safe_union::V1_0::Monostate& o) {
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

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::noinit(::android::hidl::safe_union::V1_0::Monostate&& o) {
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

::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::noinit)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

const ::android::hidl::safe_union::V1_0::Monostate& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::noinit)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::noinit)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::noinit));
    }

    return hidl_u.noinit;
}

void ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::value(uint32_t o) {
    if (hidl_d != hidl_discriminator::value) {
        hidl_destructUnion();
        ::std::memset(&hidl_u, 0, sizeof(hidl_u));

        new (&hidl_u.value) uint32_t(o);
        hidl_d = hidl_discriminator::value;
    }
    else if (&(hidl_u.value) != &o) {
        hidl_u.value = o;
    }
}

uint32_t& (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::value)() {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

uint32_t (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::value)() const {
    if (CC_UNLIKELY(hidl_d != hidl_discriminator::value)) {
        LOG_ALWAYS_FATAL("Bad safe_union access: safe_union has discriminator %" PRIu64 " but discriminator %" PRIu64 " was accessed.",
                static_cast<uint64_t>(hidl_d), static_cast<uint64_t>(hidl_discriminator::value));
    }

    return hidl_u.value;
}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_union::hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_union::~hidl_union() {}

::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_discriminator (::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::getDiscriminator)() const {
    return hidl_d;
}

::android::status_t readEmbeddedFromParcel(
        const QosFilter &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_localAddresses_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::android::hardware::hidl_string> &>(obj.localAddresses),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, localAddresses), &_hidl_localAddresses_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.localAddresses.size(); ++_hidl_index_0) {
        _hidl_err = ::android::hardware::readEmbeddedFromParcel(
                const_cast<::android::hardware::hidl_string &>(obj.localAddresses[_hidl_index_0]),
                parcel,
                _hidl_localAddresses_child,
                _hidl_index_0 * sizeof(::android::hardware::hidl_string));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    size_t _hidl_remoteAddresses_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::android::hardware::hidl_string> &>(obj.remoteAddresses),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, remoteAddresses), &_hidl_remoteAddresses_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.remoteAddresses.size(); ++_hidl_index_0) {
        _hidl_err = ::android::hardware::readEmbeddedFromParcel(
                const_cast<::android::hardware::hidl_string &>(obj.remoteAddresses[_hidl_index_0]),
                parcel,
                _hidl_remoteAddresses_child,
                _hidl_index_0 * sizeof(::android::hardware::hidl_string));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    return _hidl_err;
}

::android::status_t writeEmbeddedToParcel(
        const QosFilter &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_localAddresses_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.localAddresses,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, localAddresses), &_hidl_localAddresses_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.localAddresses.size(); ++_hidl_index_0) {
        _hidl_err = ::android::hardware::writeEmbeddedToParcel(
                obj.localAddresses[_hidl_index_0],
                parcel,
                _hidl_localAddresses_child,
                _hidl_index_0 * sizeof(::android::hardware::hidl_string));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    size_t _hidl_remoteAddresses_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.remoteAddresses,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, remoteAddresses), &_hidl_remoteAddresses_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.remoteAddresses.size(); ++_hidl_index_0) {
        _hidl_err = ::android::hardware::writeEmbeddedToParcel(
                obj.remoteAddresses[_hidl_index_0],
                parcel,
                _hidl_remoteAddresses_child,
                _hidl_index_0 * sizeof(::android::hardware::hidl_string));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    return _hidl_err;
}

::android::status_t readEmbeddedFromParcel(
        const QosSession &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_qosFilters_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter> &>(obj.qosFilters),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession, qosFilters), &_hidl_qosFilters_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.qosFilters.size(); ++_hidl_index_0) {
        _hidl_err = readEmbeddedFromParcel(
                const_cast<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter &>(obj.qosFilters[_hidl_index_0]),
                parcel,
                _hidl_qosFilters_child,
                _hidl_index_0 * sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    return _hidl_err;
}

::android::status_t writeEmbeddedToParcel(
        const QosSession &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_qosFilters_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.qosFilters,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession, qosFilters), &_hidl_qosFilters_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    for (size_t _hidl_index_0 = 0; _hidl_index_0 < obj.qosFilters.size(); ++_hidl_index_0) {
        _hidl_err = writeEmbeddedToParcel(
                obj.qosFilters[_hidl_index_0],
                parcel,
                _hidl_qosFilters_child,
                _hidl_index_0 * sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter));

        if (_hidl_err != ::android::OK) { return _hidl_err; }

    }

    return _hidl_err;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V2_7
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
