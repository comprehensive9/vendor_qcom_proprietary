/*
 * Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_GnssConfigService"
#define LOG_NDEBUG 0

#include "LocHidlGnssConfigService.h"

#define LOC_HIDL_VERSION V4_0

#include <log_util.h>
#include <gps_extended_c.h>
#include <dlfcn.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace LOC_HIDL_VERSION {
namespace implementation {

typedef const GnssInterface* (getLocationInterface)();

#define GNSS_CONFIG_SERVICE_ERROR 1
#define GNSS_CONFIG_SERVICE_SUCCESS 0

sp<LocHidlDeathRecipient> LocHidlGnssConfigService::mDeathRecipient = nullptr;
sp<ILocHidlGnssConfigServiceCallback_V2_1> LocHidlGnssConfigService::sCallbackIface = nullptr;
sp<ILocHidlGnssConfigServiceCallback_V4_0> LocHidlGnssConfigService::sCallbackIface_4_0 = nullptr;

void
LocHidlGnssConfigService::getGnssSvTypeConfigCallback(
        const GnssSvTypeConfig& config)
{
    ENTRY_LOG();

    if (mDeathRecipient != nullptr && mDeathRecipient->peerDied()) {
        ALOGE("%s] Peer Died.", __func__);
        sCallbackIface = nullptr;
        sCallbackIface_4_0 = nullptr;
        return;
    }

    if (sCallbackIface == nullptr && sCallbackIface_4_0 == nullptr) {
        LOC_LOGE("sCallbackIface NULL");
        return;
    }
    if (sCallbackIface_4_0 != nullptr) {
        // convert config to vector
        std::vector<LocHidlGnssConstellationType> disabledSvTypeVec;
        svTypeConfigToVec(config, disabledSvTypeVec);

        TO_HIDL_CLIENT();
        auto r = sCallbackIface_4_0->getGnssSvTypeConfigCb_4_0(disabledSvTypeVec);
        if (!r.isOk()) {
            LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
        }
    } else {
        // convert config to vector
        std::vector<GnssConstellationType> disabledSvTypeVec;
        svTypeConfigToVec(config, disabledSvTypeVec);

        TO_HIDL_CLIENT();
        auto r = sCallbackIface->getGnssSvTypeConfigCb(disabledSvTypeVec);
        if (!r.isOk()) {
            LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
        }
    }
}

const GnssInterface* LocHidlGnssConfigService::getGnssInterface()
{
    ENTRY_LOG();

    if (nullptr == mGnssInterface && !mGetGnssInterfaceFailed) {
        LOC_LOGD("%s]: loading libgnss.so::getGnssInterface ...", __func__);
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle)  {
            LOC_LOGE("dlopen for libgnss.so failed");
        } else if (NULL != (error = dlerror()))  {
            LOC_LOGE("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_LOGE("dlsym for libgnss.so::getGnssInterface failed, error = %s", error);
                getter = NULL;
            }
        }

        if (NULL == getter) {
            mGetGnssInterfaceFailed = true;
        } else {
            mGnssInterface = (const GnssInterface*)(*getter)();
        }
    }
    return mGnssInterface;
}

LocHidlGnssConfigService::LocHidlGnssConfigService() :
        mGnssInterface(nullptr), mGetGnssInterfaceFailed(false)
{
    ENTRY_LOG();
}

LocHidlGnssConfigService::~LocHidlGnssConfigService()
{
    ENTRY_LOG();
}

// Methods from ::vendor::qti::gnss::V2_1::ILocHidlGnssConfigService follow.
Return<bool> LocHidlGnssConfigService::init(
        const sp<ILocHidlGnssConfigServiceCallback_V2_1>& callback)
{

    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([] {
                    LOC_LOGE("%s]: ILocHidlGnssConfigServiceCallback died.", __func__);
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);
    sCallbackIface = callback;
    sCallbackIface_4_0 = nullptr;
    return true;
}

Return<void> LocHidlGnssConfigService::getGnssSvTypeConfig()
{
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }
    GnssSvTypeConfigCallback callback = [](const GnssSvTypeConfig& config) {
        LocHidlGnssConfigService::getGnssSvTypeConfigCallback(config);
    };
    gnssInterface->gnssGetSvTypeConfig(callback);

    return Void();
}

Return<void> LocHidlGnssConfigService::setGnssSvTypeConfig(
        const hidl_vec<GnssConstellationType>& disabledSvTypeVec)
{
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }

    GnssSvTypeConfig config = {};
    config.size = sizeof(config);
    svTypeVecToConfig(disabledSvTypeVec, config);

    gnssInterface->gnssUpdateSvTypeConfig(config);

    return Void();
}

Return<void> LocHidlGnssConfigService::resetGnssSvTypeConfig()
{
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }
    gnssInterface->gnssResetSvTypeConfig();

    return Void();
}

Return<bool> LocHidlGnssConfigService::init_4_0(
        const sp<ILocHidlGnssConfigServiceCallback_V4_0>& callback) {
    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([] {
                LOC_LOGE("%s]: ILocHidlGnssConfigServiceCallback died.", __func__);
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);
    sCallbackIface_4_0 = callback;
    sCallbackIface = nullptr;

    // Register for LocationControlCallbacks to get the gnss config info
    mControlCallbacks.responseCb = [](LocationError error, uint32_t id) {};
    mControlCallbacks.collectiveResponseCb =
            [](size_t count, LocationError* errors, uint32_t* ids) {};
    mControlCallbacks.gnssConfigCb = [](uint32_t session_id, const GnssConfig& config) {
        if (config.flags & GNSS_CONFIG_FLAGS_ROBUST_LOCATION_BIT) {
            RobustLocationInfo info;
            info.validMask = (uint16_t)config.robustLocationConfig.validMask;
            info.enable = config.robustLocationConfig.enabled;
            info.enableForE911 = config.robustLocationConfig.enabledForE911;
            info.major = config.robustLocationConfig.version.major;
            info.minor = config.robustLocationConfig.version.minor;
            sCallbackIface_4_0->getRobustLocationConfigCb(info);
        }
    };
    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return false;
    }
    gnssInterface->setControlCallbacks(mControlCallbacks);
    return true;
}

Return<void> LocHidlGnssConfigService::setGnssSvTypeConfig_4_0(
        const hidl_vec<LocHidlGnssConstellationType>& disabledSvTypeList) {

    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }

    GnssSvTypeConfig config = {};
    config.size = sizeof(config);
    svTypeVecToConfig(disabledSvTypeList, config);

    gnssInterface->gnssUpdateSvTypeConfig(config);

    return Void();
}

Return<void> LocHidlGnssConfigService::getRobustLocationConfig() {
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }
    GnssConfigFlagsMask mask = GNSS_CONFIG_FLAGS_ROBUST_LOCATION_BIT;
    // need to free the memory used to hold sessionIds
    uint32_t* sessionIds = gnssInterface->gnssGetConfig(mask);
    delete [] sessionIds;

    return Void();
}

Return<void> LocHidlGnssConfigService::setRobustLocationConfig(bool enable, bool enableForE911) {
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }
    gnssInterface->configRobustLocation(enable, enableForE911);

    return Void();
}

Return<void> LocHidlGnssConfigService::updateNTRIPGGAConsent(bool consentAccepted) {
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }
    gnssInterface->updateNTRIPGGAConsent(consentAccepted);

    return Void();
}

Return<void> LocHidlGnssConfigService::enablePPENtripStream(const NtripConnectionParams& params,
        bool enableRTKEngine) {
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }

    GnssNtripConnectionParams config = {};
    config.size = sizeof(config);
    config.requiresNmeaLocation = params.requiresNmeaLocation;
    config.hostNameOrIp = std::move(params.hostNameOrIp);
    config.mountPoint = std::move(params.mountPoint);
    config.username = std::move(params.username);
    config.password = std::move(params.password);
    config.port = params.port;
    config.useSSL = params.useSSL;
    gnssInterface->enablePPENtripStream(config, enableRTKEngine);

    return Void();
}

Return<void> LocHidlGnssConfigService::disablePPENtripStream() {
    FROM_HIDL_CLIENT();

    const GnssInterface* gnssInterface = getGnssInterface();
    if (nullptr == gnssInterface) {
        LOC_LOGE("Failed to get GNSS Interface");
        return Void();
    }
    gnssInterface->disablePPENtripStream();

    return Void();
}

void LocHidlGnssConfigService::svTypeVecToConfig(
        const std::vector<LocHidlGnssConstellationType>& disabledSvTypeVec, GnssSvTypeConfig& config)
{
    for(LocHidlGnssConstellationType svType: disabledSvTypeVec) {

        switch(svType) {
            case LocHidlGnssConstellationType::IRNSS:
                config.blacklistedSvTypesMask |= GNSS_SV_TYPES_MASK_NAVIC_BIT;
                break;
            default:
                // Check if enabled sv type
                if (!svTypeToConfigCommon<LocHidlGnssConstellationType>(svType, config)) {
                    svType = intToGnssConstellation((uint8_t)(~(int32_t)svType));
                    if (LocHidlGnssConstellationType::IRNSS == svType) {
                        config.enabledSvTypesMask |= GNSS_SV_TYPES_MASK_NAVIC_BIT;
                    }
                }
        }
    }
}


template <typename T>
bool LocHidlGnssConfigService::svTypeToMaskUpdate(T svType, GnssSvTypesMask& mask) {
     switch (svType) {
     case T::GLONASS:
         mask |= GNSS_SV_TYPES_MASK_GLO_BIT;
         return true;
     case T::QZSS:
         mask |= GNSS_SV_TYPES_MASK_QZSS_BIT;
         return true;
     case T::BEIDOU:
         mask |= GNSS_SV_TYPES_MASK_BDS_BIT;
         return true;
     case T::GALILEO:
         mask |= GNSS_SV_TYPES_MASK_GAL_BIT;
         return true;
     default:
         return false;
     }
 }

template <typename T>
bool LocHidlGnssConfigService::svTypeToConfigCommon(
        T svType, GnssSvTypeConfig& config) {
    return svTypeToMaskUpdate(svType, config.blacklistedSvTypesMask) ||
            svTypeToMaskUpdate(intToGnssConstellation((uint8_t)(~(int32_t)svType)),
                               config.enabledSvTypesMask);
}

void LocHidlGnssConfigService::svTypeVecToConfig(
        const std::vector<GnssConstellationType>& disabledSvTypeVec, GnssSvTypeConfig& config)
{
    for(GnssConstellationType svType: disabledSvTypeVec) {
        svTypeToConfigCommon<GnssConstellationType>(svType, config);
    }
}

template <typename T>
void LocHidlGnssConfigService::svTypeConfigToVecCommon(const GnssSvTypeConfig& config,
                                                           std::vector<T>& svTypeVec) {
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_GLO_BIT) {
        svTypeVec.push_back(T::GLONASS);
    }
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_QZSS_BIT) {
        svTypeVec.push_back(T::QZSS);
    }
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_BDS_BIT) {
        svTypeVec.push_back(T::BEIDOU);
    }
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_GAL_BIT) {
        svTypeVec.push_back(T::GALILEO);
    }

    // Set enabled values
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_GLO_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::GLONASS)));
    }
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_QZSS_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::QZSS)));
    }
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_BDS_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::BEIDOU)));
    }
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_GAL_BIT) {
        svTypeVec.push_back((T)
                (~gnssConstellationToIntCommon(T::GALILEO)));
    }
}

void LocHidlGnssConfigService::svTypeConfigToVec(
        const GnssSvTypeConfig& config, std::vector<GnssConstellationType>& svTypeVec) {
    svTypeConfigToVecCommon<GnssConstellationType>(config, svTypeVec);
}

void LocHidlGnssConfigService::svTypeConfigToVec(
        const GnssSvTypeConfig& config, std::vector<LocHidlGnssConstellationType>& svTypeVec) {
    if (config.blacklistedSvTypesMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
        svTypeVec.push_back(LocHidlGnssConstellationType::IRNSS);
    }
    svTypeConfigToVecCommon<LocHidlGnssConstellationType>(config, svTypeVec);
    // Set enabled values
    if (config.enabledSvTypesMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
        svTypeVec.push_back((LocHidlGnssConstellationType)
                (~gnssConstellationToInt(LocHidlGnssConstellationType::IRNSS)));
    }
}

// Helper method for constellation 1.0 and 2.0 combined
LocHidlGnssConstellationType LocHidlGnssConfigService::intToGnssConstellation(int32_t svTypeInt) {

    switch(svTypeInt) {
    case 1:
        return LocHidlGnssConstellationType::GPS;
    case 2:
        return LocHidlGnssConstellationType::SBAS;
    case 3:
        return LocHidlGnssConstellationType::GLONASS;
    case 4:
        return LocHidlGnssConstellationType::QZSS;
    case 5:
        return LocHidlGnssConstellationType::BEIDOU;
    case 6:
        return LocHidlGnssConstellationType::GALILEO;
    case 7:
        return LocHidlGnssConstellationType::IRNSS;
    default:
        return LocHidlGnssConstellationType::UNKNOWN;
    }
}

template <typename T>
int32_t LocHidlGnssConfigService::gnssConstellationToIntCommon(T svType) {
    switch(svType) {
        case T::GPS:
            return 1;
        case T::SBAS:
            return 2;
        case T::GLONASS:
            return 3;
        case T::QZSS:
            return 4;
        case T::BEIDOU:
            return 5;
        case T::GALILEO:
            return 6;
        default:
            return 0;
    }
}

int32_t LocHidlGnssConfigService::gnssConstellationToInt(LocHidlGnssConstellationType svType) {
    switch(svType) {
        case LocHidlGnssConstellationType::IRNSS:
            return 7;
        default:
            return gnssConstellationToIntCommon(svType);
    }
}

}  // namespace implementation
}  // namespace LOC_HIDL_VERSION
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#undef LOC_HIDL_VERSION
