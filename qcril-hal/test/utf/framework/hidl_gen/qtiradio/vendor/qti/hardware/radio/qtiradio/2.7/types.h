/*
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_7_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_7_TYPES_H

#include <android/hidl/safe_union/1.0/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace V2_7 {

// Forward declaration for forward reference support:
struct QosBandwidth;
struct EpsQos;
struct NrQos;
struct Qos;
struct QosParametersResult;
enum class QosFlowIdRange : uint8_t;
enum class QosPortRange : uint16_t;
struct PortRange;
struct MaybePort;
enum class QosProtocol : int8_t;
enum class QosFilterDirection : int8_t;
struct QosFilter;
struct QosSession;

// Order of inner types was changed for forward reference support.

struct QosBandwidth final {
    /**
     * Maximum bit rate possible on the bearer
     */
    uint32_t maxBitrateKbps __attribute__ ((aligned(4)));
    /**
     * Minimum bit rate that is guaranteed to be provided by the network
     */
    uint32_t guaranteedBitrateKbps __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth, maxBitrateKbps) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth, guaranteedBitrateKbps) == 4, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth) == 8, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth) == 4, "wrong alignment");

/**
 * LTE/EPS Quality of Service parameters as per 3gpp spec 24.301 sec 9.9.4.3.
 */
struct EpsQos final {
    /**
     * Quality of Service Class Identifier (QCI), see 3GPP TS 23.203 and 29.212.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t qci __attribute__ ((aligned(2)));
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth downlink __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth uplink __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos, qci) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos, downlink) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos, uplink) == 12, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos) == 20, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos) == 4, "wrong alignment");

/**
 * 5G Quality of Service parameters as per 3gpp spec 24.501 sec 9.11.4.12
 */
struct NrQos final {
    /**
     * 5G QOS Identifier (5QI), see 3GPP TS 24.501 and 23.501.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t fiveQi __attribute__ ((aligned(2)));
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth downlink __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth uplink __attribute__ ((aligned(4)));
    /**
     * QOS flow identifier of the QOS flow description in the
     * range of QosFlowIdRange::MIN to QosFlowIdRange::MAX
     */
    uint8_t qfi __attribute__ ((aligned(1)));
    uint16_t averagingWindowMs __attribute__ ((aligned(2)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos, fiveQi) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos, downlink) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos, uplink) == 12, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos, qfi) == 20, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos, averagingWindowMs) == 22, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos) == 24, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos) == 4, "wrong alignment");

/**
 * EPS or NR QOS parameters
 */
struct Qos final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        eps = 1,  // ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos
        nr = 2,  // ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos
    };

    Qos();
    ~Qos();
    Qos(Qos&&);
    Qos(const Qos&);
    Qos& operator=(Qos&&);
    Qos& operator=(const Qos&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void eps(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos&);
    void eps(::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos&&);
    ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& eps();
    const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& eps() const;

    void nr(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos&);
    void nr(::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos&&);
    ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& nr();
    const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& nr() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos eps __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos nr __attribute__ ((aligned(4)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_union) == 24, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_union) == 4, "wrong alignment");
    static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos) == 28, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::Qos) == 4, "wrong alignment");

struct QosParametersResult final {
    /**
     * Default bearer QoS. Applicable to LTE and NR
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos defaultQos __attribute__ ((aligned(4)));
    /**
     * Active QOS sessions of the dedicated bearers. Applicable to
     * PDNs that support dedicated bearers.
     */
    ::android::hardware::hidl_vec<::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession> qosSessions __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult, defaultQos) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult, qosSessions) == 32, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult) == 48, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult) == 8, "wrong alignment");

/**
 * Allowed values for 5G QOS flow identifier
 */
enum class QosFlowIdRange : uint8_t {
    MIN = 1,
    MAX = 63,
};

/**
 * Allowed port numbers
 */
enum class QosPortRange : uint16_t {
    MIN = 20,
    MAX = 65535,
};

/**
 * Defines range of ports. start and end are the first and last port numbers
 * (inclusive) in the range. Both start and end are in QosPortRange.MIN to
 * QosPortRange.MAX range. A single port shall be represented by the same
 * start and end value.
 */
struct PortRange final {
    int32_t start __attribute__ ((aligned(4)));
    int32_t end __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange, start) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange, end) == 4, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange) == 8, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange) == 4, "wrong alignment");

/**
 * Port is optional, contains either single port or range of ports
 */
struct MaybePort final {
    enum class hidl_discriminator : uint8_t {
        noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
        range = 1,  // ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange
    };

    MaybePort();
    ~MaybePort();
    MaybePort(MaybePort&&);
    MaybePort(const MaybePort&);
    MaybePort& operator=(MaybePort&&);
    MaybePort& operator=(const MaybePort&);

    void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
    void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
    ::android::hidl::safe_union::V1_0::Monostate& noinit();
    const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

    void range(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange&);
    void range(::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange&&);
    ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& range();
    const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& range() const;

    // Utility methods
    hidl_discriminator getDiscriminator() const;

    constexpr size_t hidl_getUnionOffset() const {
        return offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort, hidl_u);
    }

private:
    void hidl_destructUnion();

    hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
    union hidl_union final {
        ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
        ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange range __attribute__ ((aligned(4)));

        hidl_union();
        ~hidl_union();
    } hidl_u;

    static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_union) == 8, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_union) == 4, "wrong alignment");
    static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_discriminator) == 1, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_discriminator) == 1, "wrong alignment");
};

static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort) == 12, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort) == 4, "wrong alignment");

/**
 * Next header protocol numbers defined by IANA, RFC 5237
 */
enum class QosProtocol : int8_t {
    /**
     * No protocol specified
     */
    UNSPECIFIED = -1 /* -1 */,
    /**
     * Transmission Control Protocol
     */
    TCP = 6,
    /**
     * User Datagram Protocol
     */
    UDP = 17,
    /**
     * Encapsulating Security Payload Protocol
     */
    ESP = 50,
    /**
     * Authentication Header
     */
    AH = 51,
};

enum class QosFilterDirection : int8_t {
    DOWNLINK = 0,
    UPLINK = 1,
    BIDIRECTIONAL = 2,
};

/**
 * See 3gpp 24.008 10.5.6.12 and 3gpp 24.501 9.11.4.13
 */
struct QosFilter final {
    // Forward declaration for forward reference support:
    struct TypeOfService;
    struct Ipv6FlowLabel;
    struct IpsecSpi;

    struct TypeOfService final {
        enum class hidl_discriminator : uint8_t {
            noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
            value = 1,  // uint8_t
        };

        TypeOfService();
        ~TypeOfService();
        TypeOfService(TypeOfService&&);
        TypeOfService(const TypeOfService&);
        TypeOfService& operator=(TypeOfService&&);
        TypeOfService& operator=(const TypeOfService&);

        void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
        void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
        ::android::hidl::safe_union::V1_0::Monostate& noinit();
        const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

        void value(uint8_t);
        uint8_t& value();
        uint8_t value() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
            uint8_t value __attribute__ ((aligned(1)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_union) == 1, "wrong size");
        static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_union) == 1, "wrong alignment");
        static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService) == 2, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService) == 1, "wrong alignment");

    struct Ipv6FlowLabel final {
        enum class hidl_discriminator : uint8_t {
            noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
            value = 1,  // uint32_t
        };

        Ipv6FlowLabel();
        ~Ipv6FlowLabel();
        Ipv6FlowLabel(Ipv6FlowLabel&&);
        Ipv6FlowLabel(const Ipv6FlowLabel&);
        Ipv6FlowLabel& operator=(Ipv6FlowLabel&&);
        Ipv6FlowLabel& operator=(const Ipv6FlowLabel&);

        void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
        void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
        ::android::hidl::safe_union::V1_0::Monostate& noinit();
        const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

        void value(uint32_t);
        uint32_t& value();
        uint32_t value() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
            uint32_t value __attribute__ ((aligned(4)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_union) == 4, "wrong size");
        static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_union) == 4, "wrong alignment");
        static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel) == 8, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel) == 4, "wrong alignment");

    struct IpsecSpi final {
        enum class hidl_discriminator : uint8_t {
            noinit = 0,  // ::android::hidl::safe_union::V1_0::Monostate
            value = 1,  // uint32_t
        };

        IpsecSpi();
        ~IpsecSpi();
        IpsecSpi(IpsecSpi&&);
        IpsecSpi(const IpsecSpi&);
        IpsecSpi& operator=(IpsecSpi&&);
        IpsecSpi& operator=(const IpsecSpi&);

        void noinit(const ::android::hidl::safe_union::V1_0::Monostate&);
        void noinit(::android::hidl::safe_union::V1_0::Monostate&&);
        ::android::hidl::safe_union::V1_0::Monostate& noinit();
        const ::android::hidl::safe_union::V1_0::Monostate& noinit() const;

        void value(uint32_t);
        uint32_t& value();
        uint32_t value() const;

        // Utility methods
        hidl_discriminator getDiscriminator() const;

        constexpr size_t hidl_getUnionOffset() const {
            return offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi, hidl_u);
        }

    private:
        void hidl_destructUnion();

        hidl_discriminator hidl_d __attribute__ ((aligned(1))) ;
        union hidl_union final {
            ::android::hidl::safe_union::V1_0::Monostate noinit __attribute__ ((aligned(1)));
            uint32_t value __attribute__ ((aligned(4)));

            hidl_union();
            ~hidl_union();
        } hidl_u;

        static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_union) == 4, "wrong size");
        static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_union) == 4, "wrong alignment");
        static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_discriminator) == 1, "wrong size");
        static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_discriminator) == 1, "wrong alignment");
    };

    static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi) == 8, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi) == 4, "wrong alignment");

    /**
     * Local and remote IP addresses, typically one IPv4 or one IPv6
     * or one of each. Addresses could be with optional "/" prefix
     * length, e.g.,"192.0.1.3" or "192.0.1.11/16 2001:db8::1/64".
     * If the prefix length is absent, the addresses are assumed to be
     * point to point with IPv4 having a prefix length of 32 and
     * IPv6 128.
     */
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> localAddresses __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<::android::hardware::hidl_string> remoteAddresses __attribute__ ((aligned(8)));
    /**
     * Local and remote port/ranges
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort localPort __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort remotePort __attribute__ ((aligned(4)));
    /**
     * QoS protocol
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol protocol __attribute__ ((aligned(1)));
    /**
     * Type of service value or mask as defined in RFC 1349
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService tos __attribute__ ((aligned(1)));
    /**
     * IPv6 flow label as defined in RFC 6437
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel flowLabel __attribute__ ((aligned(4)));
    /**
     * IPSec security parameter index
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi spi __attribute__ ((aligned(4)));
    /**
     * Filter direction
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection direction __attribute__ ((aligned(1)));
    /**
     * Specifies the order in which the filter needs to be matched.
     * A lower numerical(positive) value has a higher precedence.
     * Set -1 when unspecified.
     */
    int32_t precedence __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, localAddresses) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, remoteAddresses) == 16, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, localPort) == 32, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, remotePort) == 44, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, protocol) == 56, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, tos) == 57, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, flowLabel) == 60, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, spi) == 68, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, direction) == 76, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter, precedence) == 80, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter) == 88, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter) == 8, "wrong alignment");

/**
 * QOS session associated with a dedicated bearer
 */
struct QosSession final {
    /**
     * Unique ID of the QoS session within the data call
     */
    int32_t qosSessionId __attribute__ ((aligned(4)));
    /**
     * QOS attributes
     */
    ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos qos __attribute__ ((aligned(4)));
    /**
     * List of QOS filters associated with this session
     */
    ::android::hardware::hidl_vec<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter> qosFilters __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession, qosSessionId) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession, qos) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession, qosFilters) == 32, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession) == 48, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession) == 8, "wrong alignment");

//
// type declarations for package
//

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& rhs);

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint16_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange o, ::std::ostream* os);
constexpr uint16_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator|(const uint16_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange rhs) {
    return static_cast<uint16_t>(lhs | static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange lhs, const uint16_t rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) | rhs);
}
constexpr uint16_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator&(const uint16_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange rhs) {
    return static_cast<uint16_t>(lhs & static_cast<uint16_t>(rhs));
}
constexpr uint16_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange lhs, const uint16_t rhs) {
    return static_cast<uint16_t>(static_cast<uint16_t>(lhs) & rhs);
}
constexpr uint16_t &operator|=(uint16_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange e) {
    v |= static_cast<uint16_t>(e);
    return v;
}
constexpr uint16_t &operator&=(uint16_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange e) {
    v &= static_cast<uint16_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& rhs);

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol o, ::std::ostream* os);
constexpr int8_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol e) {
    v &= static_cast<int8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int8_t o);
static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection o);
static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection o, ::std::ostream* os);
constexpr int8_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const int8_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection rhs) {
    return static_cast<int8_t>(lhs | static_cast<int8_t>(rhs));
}
constexpr int8_t operator|(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) | rhs);
}
constexpr int8_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const int8_t lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection rhs) {
    return static_cast<int8_t>(lhs & static_cast<int8_t>(rhs));
}
constexpr int8_t operator&(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection lhs, const int8_t rhs) {
    return static_cast<int8_t>(static_cast<int8_t>(lhs) & rhs);
}
constexpr int8_t &operator|=(int8_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection e) {
    v |= static_cast<int8_t>(e);
    return v;
}
constexpr int8_t &operator&=(int8_t& v, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection e) {
    v &= static_cast<int8_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& rhs);

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& o);
static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& rhs);

//
// type header definitions for package
//

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".maxBitrateKbps = ";
    os += ::android::hardware::toString(o.maxBitrateKbps);
    os += ", .guaranteedBitrateKbps = ";
    os += ::android::hardware::toString(o.guaranteedBitrateKbps);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& rhs) {
    if (lhs.maxBitrateKbps != rhs.maxBitrateKbps) {
        return false;
    }
    if (lhs.guaranteedBitrateKbps != rhs.guaranteedBitrateKbps) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosBandwidth& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".qci = ";
    os += ::android::hardware::toString(o.qci);
    os += ", .downlink = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.downlink);
    os += ", .uplink = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.uplink);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& rhs) {
    if (lhs.qci != rhs.qci) {
        return false;
    }
    if (lhs.downlink != rhs.downlink) {
        return false;
    }
    if (lhs.uplink != rhs.uplink) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::EpsQos& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".fiveQi = ";
    os += ::android::hardware::toString(o.fiveQi);
    os += ", .downlink = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.downlink);
    os += ", .uplink = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.uplink);
    os += ", .qfi = ";
    os += ::android::hardware::toString(o.qfi);
    os += ", .averagingWindowMs = ";
    os += ::android::hardware::toString(o.averagingWindowMs);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& rhs) {
    if (lhs.fiveQi != rhs.fiveQi) {
        return false;
    }
    if (lhs.downlink != rhs.downlink) {
        return false;
    }
    if (lhs.uplink != rhs.uplink) {
        return false;
    }
    if (lhs.qfi != rhs.qfi) {
        return false;
    }
    if (lhs.averagingWindowMs != rhs.averagingWindowMs) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::NrQos& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator::eps: {
            os += ".eps = ";
            os += toString(o.eps());
            break;
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator::nr: {
            os += ".nr = ";
            os += toString(o.nr());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator::eps: {
            return (lhs.eps() == rhs.eps());
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos::hidl_discriminator::nr: {
            return (lhs.nr() == rhs.nr());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::Qos& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".defaultQos = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.defaultQos);
    os += ", .qosSessions = ";
    os += ::android::hardware::toString(o.qosSessions);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& rhs) {
    if (lhs.defaultQos != rhs.defaultQos) {
        return false;
    }
    if (lhs.qosSessions != rhs.qosSessions) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosParametersResult& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MIN) == static_cast<uint8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MIN)) {
        os += (first ? "" : " | ");
        os += "MIN";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MIN;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MAX) == static_cast<uint8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MAX)) {
        os += (first ? "" : " | ");
        os += "MAX";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MAX;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MIN) {
        return "MIN";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MAX) {
        return "MAX";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange>(uint16_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MIN) == static_cast<uint16_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MIN)) {
        os += (first ? "" : " | ");
        os += "MIN";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MIN;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MAX) == static_cast<uint16_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MAX)) {
        os += (first ? "" : " | ");
        os += "MAX";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MAX;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MIN) {
        return "MIN";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MAX) {
        return "MAX";
    }
    std::string os;
    os += toHexString(static_cast<uint16_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".start = ";
    os += ::android::hardware::toString(o.start);
    os += ", .end = ";
    os += ::android::hardware::toString(o.end);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& rhs) {
    if (lhs.start != rhs.start) {
        return false;
    }
    if (lhs.end != rhs.end) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::PortRange& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_discriminator::range: {
            os += ".range = ";
            os += toString(o.range());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort::hidl_discriminator::range: {
            return (lhs.range() == rhs.range());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::MaybePort& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UNSPECIFIED) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UNSPECIFIED)) {
        os += (first ? "" : " | ");
        os += "UNSPECIFIED";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UNSPECIFIED;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::TCP) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::TCP)) {
        os += (first ? "" : " | ");
        os += "TCP";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::TCP;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UDP) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UDP)) {
        os += (first ? "" : " | ");
        os += "UDP";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UDP;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::ESP) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::ESP)) {
        os += (first ? "" : " | ");
        os += "ESP";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::ESP;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::AH) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::AH)) {
        os += (first ? "" : " | ");
        os += "AH";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::AH;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UNSPECIFIED) {
        return "UNSPECIFIED";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::TCP) {
        return "TCP";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UDP) {
        return "UDP";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::ESP) {
        return "ESP";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::AH) {
        return "AH";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection>(int8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::DOWNLINK) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::DOWNLINK)) {
        os += (first ? "" : " | ");
        os += "DOWNLINK";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::DOWNLINK;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::UPLINK) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::UPLINK)) {
        os += (first ? "" : " | ");
        os += "UPLINK";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::UPLINK;
    }
    if ((o & ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::BIDIRECTIONAL) == static_cast<int8_t>(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::BIDIRECTIONAL)) {
        os += (first ? "" : " | ");
        os += "BIDIRECTIONAL";
        first = false;
        flipped |= ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::BIDIRECTIONAL;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::DOWNLINK) {
        return "DOWNLINK";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::UPLINK) {
        return "UPLINK";
    }
    if (o == ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::BIDIRECTIONAL) {
        return "BIDIRECTIONAL";
    }
    std::string os;
    os += toHexString(static_cast<int8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::TypeOfService& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::Ipv6FlowLabel& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";

    switch (o.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_discriminator::noinit: {
            os += ".noinit = ";
            os += toString(o.noinit());
            break;
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_discriminator::value: {
            os += ".value = ";
            os += toString(o.value());
            break;
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) o.getDiscriminator()) + ").").c_str());
        }
    }
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& rhs) {
    if (lhs.getDiscriminator() != rhs.getDiscriminator()) {
        return false;
    }
    switch (lhs.getDiscriminator()) {
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_discriminator::noinit: {
            return (lhs.noinit() == rhs.noinit());
        }
        case ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi::hidl_discriminator::value: {
            return (lhs.value() == rhs.value());
        }
        default: {
            ::android::hardware::details::logAlwaysFatal((
                    "Unknown union discriminator (value: " +
                    std::to_string((uint8_t) lhs.getDiscriminator()) + ").").c_str());
        }
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter::IpsecSpi& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".localAddresses = ";
    os += ::android::hardware::toString(o.localAddresses);
    os += ", .remoteAddresses = ";
    os += ::android::hardware::toString(o.remoteAddresses);
    os += ", .localPort = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.localPort);
    os += ", .remotePort = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.remotePort);
    os += ", .protocol = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.protocol);
    os += ", .tos = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.tos);
    os += ", .flowLabel = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.flowLabel);
    os += ", .spi = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.spi);
    os += ", .direction = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.direction);
    os += ", .precedence = ";
    os += ::android::hardware::toString(o.precedence);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& rhs) {
    if (lhs.localAddresses != rhs.localAddresses) {
        return false;
    }
    if (lhs.remoteAddresses != rhs.remoteAddresses) {
        return false;
    }
    if (lhs.localPort != rhs.localPort) {
        return false;
    }
    if (lhs.remotePort != rhs.remotePort) {
        return false;
    }
    if (lhs.protocol != rhs.protocol) {
        return false;
    }
    if (lhs.tos != rhs.tos) {
        return false;
    }
    if (lhs.flowLabel != rhs.flowLabel) {
        return false;
    }
    if (lhs.spi != rhs.spi) {
        return false;
    }
    if (lhs.direction != rhs.direction) {
        return false;
    }
    if (lhs.precedence != rhs.precedence) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilter& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".qosSessionId = ";
    os += ::android::hardware::toString(o.qosSessionId);
    os += ", .qos = ";
    os += ::vendor::qti::hardware::radio::qtiradio::V2_7::toString(o.qos);
    os += ", .qosFilters = ";
    os += ::android::hardware::toString(o.qosFilters);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& rhs) {
    if (lhs.qosSessionId != rhs.qosSessionId) {
        return false;
    }
    if (lhs.qos != rhs.qos) {
        return false;
    }
    if (lhs.qosFilters != rhs.qosFilters) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& lhs, const ::vendor::qti::hardware::radio::qtiradio::V2_7::QosSession& rhs){
    return !(lhs == rhs);
}


}  // namespace V2_7
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange, 2> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MIN,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFlowIdRange::MAX,
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange, 2> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MIN,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosPortRange::MAX,
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol, 5> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UNSPECIFIED,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::TCP,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::UDP,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::ESP,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosProtocol::AH,
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
template<> inline constexpr std::array<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection, 3> hidl_enum_values<::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection> = {
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::DOWNLINK,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::UPLINK,
    ::vendor::qti::hardware::radio::qtiradio::V2_7::QosFilterDirection::BIDIRECTIONAL,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_RADIO_QTIRADIO_V2_7_TYPES_H
