/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_HIDL_ILOCHIDLGNSS_H
#define GNSS_MULTI_CLIENT_HIDL_ILOCHIDLGNSS_H

#include "IGnssAPI.h"
#include <LocationAPI.h>
#include <GnssMultiClientHidlILocHidlGnssCb.h>
#include <vector>
#include "DataItemId.h"
#include "IGardenCase.h"

using android::OK;
using android::sp;
using android::wp;
using android::status_t;
using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_vec;
using android::hardware::hidl_death_recipient;
using android::hidl::base::V1_0::IBase;

using ILocHidlGnss_V4_0 = ::vendor::qti::gnss::V4_0::ILocHidlGnss;
using ::vendor::qti::gnss::V2_1::ILocHidlGnssConfigService;
using ILocHidlGnssConfigService_V4_0 = ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigService;
using ::vendor::qti::gnss::V3_0::ILocHidlGnss;
using ::vendor::qti::gnss::V3_0::ILocHidlIzatConfig;
using ::vendor::qti::gnss::V1_1::ILocHidlIzatSubscription;
namespace garden {

#define GNSS_VENDOR_SERVICE_INSTANCE "gnss_vendor"

/* Data structures for HIDL service interaction */
struct LocHidlGnssDeathRecipient : virtual public hidl_death_recipient {
    virtual void serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) override;
    virtual ~LocHidlGnssDeathRecipient() {};
};

/* GNSS MULTI CLIENT HIDL CLIENT FOR IGNSS INTERFACE */
class GnssMultiClientHidlILocHidlGnss {

    friend class GnssMultiClientHidlILocHidlGnssCb;

private:
    static GnssMultiClientHidlILocHidlGnss* sInstance;
    GnssMultiClientHidlILocHidlGnss() {
        mLocHidlGnssCb = new GnssMultiClientHidlILocHidlGnssCb(this);
    };

public:
    static GnssMultiClientHidlILocHidlGnss& get() {
        if (nullptr == sInstance) {
            sInstance = new GnssMultiClientHidlILocHidlGnss();
        }
        return *sInstance;
    }

public:
    GARDEN_RESULT menuTest();
private:
    /*util*/
    template <class T>
    GARDEN_RESULT checkResultOk(T& Result, const std::string printMsg);
    template <class T>
    Return<bool> ILocHidlExtinit(T& ext);
    template <class T>
    Return<bool> ILocHidlExtinit_4_0(T& ext);
    template <class T>
    void convertGnssTypeMaskToConstellationVec(GnssSvTypesMask enableMask, GnssSvTypesMask
        disableMask, std::vector<T>& disableVec);
    /* Individual test cases triggered from menu test */
    GARDEN_RESULT ILocHidlIzatConfig_1_0_init();
    GARDEN_RESULT ILocHidlIzatConfig_1_0_readConfig();
    GARDEN_RESULT ILocHidlIzatSubscription_1_0_init();
    GARDEN_RESULT ILocHidlIzatSubscription_1_1_boolDataItemUpdate(int result);
    GARDEN_RESULT ILocHidlGnssConfigService_init();
    GARDEN_RESULT ILocHidlGnssConfigService_reset();
    GARDEN_RESULT ILocHidlGnssConfigService_setGnssSvTypeConfig(GnssSvTypesMask enableMask,
        GnssSvTypesMask disableMask);
    GARDEN_RESULT ILocHidlGnssConfigService_getGnssSvTypeConfig();
    void initVendorHal();

public:
    //LocHidl death recepient uses below to instantiate service
    GARDEN_RESULT createHidlClient();
    sp<ILocHidlGnss> mLocHidlGnssIface = nullptr;
    sp<ILocHidlGnss_V4_0> mLocHidlGnssIface_V4_0 = nullptr;
    sp<ILocHidlIzatConfig> mLocHidlGnssExtensionIzatConfig = nullptr;
    sp<LocHidlGnssDeathRecipient> mLocHidlGnssDeathRecipient = nullptr;
    sp<ILocHidlIzatSubscription> mLocHidlIzatSubscription = nullptr;
    sp<ILocHidlGnssConfigService> mLocHidlGnssConfigService_V2_1 = nullptr;
    sp<ILocHidlGnssConfigService_V4_0> mLocHidlGnssConfigService_V4_0 = nullptr;
    sp<GnssMultiClientHidlILocHidlGnssCb> mLocHidlGnssCb = nullptr;
};

} //namespace garden


#endif //GNSS_MULTI_CLIENT_HIDL_ILOCHIDLGNSS_H
