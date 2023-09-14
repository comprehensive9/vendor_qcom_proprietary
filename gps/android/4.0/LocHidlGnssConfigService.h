/*
 * Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V4_0_LOCHIDLGNSSCONFIGSERVICE_H
#define VENDOR_QTI_GNSS_V4_0_LOCHIDLGNSSCONFIGSERVICE_H

#include "LocHidlUtils.h"

#define LOC_HIDL_VERSION V4_0

#include <vendor/qti/gnss/4.0/ILocHidlGnssConfigService.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <location_interface.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace LOC_HIDL_VERSION {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

using ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigService;
using ::android::hardware::gnss::V1_0::GnssConstellationType;
using ILocHidlGnssConfigServiceCallback_V2_1 =
        ::vendor::qti::gnss::V2_1::ILocHidlGnssConfigServiceCallback;
using ILocHidlGnssConfigServiceCallback_V4_0 =
        ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigServiceCallback;
using LocHidlGnssConstellationType =
        ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigServiceCallback::GnssConstellationType;

using RobustLocationInfo =
        ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigServiceCallback::RobustLocationInfo;
struct LocHidlGnssConfigService : public ILocHidlGnssConfigService {
    LocHidlGnssConfigService();
    virtual ~LocHidlGnssConfigService();
    // Methods from ::vendor::qti::gnss::V2_1::ILocHidlGnssConfigService follow.
    Return<bool> init(const sp<ILocHidlGnssConfigServiceCallback_V2_1>& callback) override;
    Return<void> getGnssSvTypeConfig() override;
    Return<void> setGnssSvTypeConfig(const hidl_vec<GnssConstellationType>& disabledSvTypeVec)
            override;
    Return<void> resetGnssSvTypeConfig() override;
    // Methods from ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigService follow.
    Return<bool> init_4_0(const sp<ILocHidlGnssConfigServiceCallback_V4_0>& callback) override;
    Return<void> setGnssSvTypeConfig_4_0(
            const hidl_vec<LocHidlGnssConstellationType>& disabledSvTypeList) override;
    Return<void> getRobustLocationConfig() override;
    Return<void> setRobustLocationConfig(bool enable, bool enableForE911) override;
    // Precise location
    Return<void> updateNTRIPGGAConsent(bool consentAccepted) override;
    Return<void> enablePPENtripStream(const NtripConnectionParams& params, bool enableRTKEngine) override;
    Return<void> disablePPENtripStream() override;

private:
    const GnssInterface* getGnssInterface();
    static void getGnssSvTypeConfigCallback(const GnssSvTypeConfig& config);
    template <typename T>
    static bool svTypeToConfigCommon(T svType, GnssSvTypeConfig& config);
    static void svTypeVecToConfig(const std::vector<GnssConstellationType>& disabledSvTypeVec,
                                      GnssSvTypeConfig& config);
    static void svTypeVecToConfig(const std::vector<LocHidlGnssConstellationType>& disabledSvTypeVec,
                                      GnssSvTypeConfig& config);
    static void svTypeConfigToVec(const GnssSvTypeConfig& config,
                                      std::vector<GnssConstellationType>& disabledSvTypeVec);
    static void svTypeConfigToVec(const GnssSvTypeConfig& config,
                                      std::vector<LocHidlGnssConstellationType>& disabledSvTypeVec);
    template <typename T>
    static void svTypeConfigToVecCommon(const GnssSvTypeConfig& config, std::vector<T>& svTypeVec);
    template <typename T>
    static bool svTypeToMaskUpdate(T svType, GnssSvTypesMask& mask);
    static LocHidlGnssConstellationType intToGnssConstellation(int32_t svTypeInt);
    static int32_t gnssConstellationToInt(LocHidlGnssConstellationType svType);
    template <typename T>
    static int32_t gnssConstellationToIntCommon(T svType);
private:
    const GnssInterface* mGnssInterface = nullptr;
    bool mGetGnssInterfaceFailed = false;
    LocationControlCallbacks mControlCallbacks;
    static sp<LocHidlDeathRecipient> mDeathRecipient;
    static sp<ILocHidlGnssConfigServiceCallback_V2_1> sCallbackIface;
    static sp<ILocHidlGnssConfigServiceCallback_V4_0> sCallbackIface_4_0;
};

}  // namespace implementation
}  // namespace LOC_HIDL_VERSION
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#undef LOC_HIDL_VERSION

#endif  // VENDOR_QTI_GNSS_V4_0_LOCHIDLGNSSCONFIGSERVICE_H
