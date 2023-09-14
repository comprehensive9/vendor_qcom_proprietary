/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_HIDL_ILOCHIDLGNSS_CB_H
#define GNSS_MULTI_CLIENT_HIDL_ILOCHIDLGNSS_CB_H
#define NAVIC 7

#include "GnssMultiClientCase.h"
#include "GnssCbBase.h"
#include <stdint.h>
#include <vendor/qti/gnss/4.0/ILocHidlGnss.h>
#include <vendor/qti/gnss/4.0/ILocHidlGnssConfigServiceCallback.h>
#include <vendor/qti/gnss/3.0/ILocHidlIzatConfig.h>
#include <vendor/qti/gnss/1.1/ILocHidlIzatSubscriptionCallback.h>

using android::hardware::Return;
using android::hardware::Void;
using ::android::hardware::hidl_vec;

using ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigServiceCallback;
using ::vendor::qti::gnss::V3_0::ILocHidlGnss;
using ::vendor::qti::gnss::V3_0::ILocHidlIzatConfigCallback;
using ::vendor::qti::gnss::V1_1::ILocHidlIzatSubscriptionCallback;
using vendor::qti::gnss::V1_0::LocHidlSubscriptionDataItemId;
using ::vendor::qti::gnss::V4_0::ILocHidlGnssConfigServiceCallback;
using GnssConstellationType_V1_0 = ::android::hardware::gnss::V1_0::GnssConstellationType;
using LocHidlGnssConstellationType = ::vendor::qti::gnss::V4_0::
    ILocHidlGnssConfigServiceCallback::GnssConstellationType;

namespace garden {

class GnssMultiClientHidlILocHidlGnss;

class GnssMultiClientHidlILocHidlGnssCb :
        public ILocHidlIzatConfigCallback,
        public ILocHidlIzatSubscriptionCallback,
        public ILocHidlGnssConfigServiceCallback
{
public:
    GnssMultiClientHidlILocHidlGnssCb(GnssMultiClientHidlILocHidlGnss* hidlILocHidlGnss);
    ~GnssMultiClientHidlILocHidlGnssCb();

    //IzatConfigCb
    Return<void> izatConfigCallback(const ::android::hardware::hidl_string& izatConfigContent);

    //Izatsubscriptioncb
    Return<void> requestData(const hidl_vec<LocHidlSubscriptionDataItemId>& list) {return Void();}
    Return<void> updateSubscribe(const hidl_vec<LocHidlSubscriptionDataItemId>& list,
        bool subscribe) { return Void();}
    Return<void> unsubscribeAll() {return Void();}
    Return<void> turnOnModule(LocHidlSubscriptionDataItemId di, int32_t timeout) {return Void();}
    Return<void> turnOffModule(LocHidlSubscriptionDataItemId di) {return Void();}
    //ILocHidlGnssConfigServiceCb
    template <class T>
    static void convertGnssTypeMaskToConstellationVec(GnssSvTypesMask enableMask, GnssSvTypesMask
        disableMask, std::vector<T>& disableVec);
    template <class T>
    static int convertConstellationToint(T in) {
        switch (in) {
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
    Return<void> getGnssSvTypeConfigCb(
        const hidl_vec<GnssConstellationType_V1_0>& disabledSvTypeList);
    Return<void> getGnssSvTypeConfigCb_4_0(
        const hidl_vec<LocHidlGnssConstellationType>& disabledSvTypeList);
    Return<void> getRobustLocationConfigCb(const ILocHidlGnssConfigServiceCallback::
        RobustLocationInfo& info) {return Void();}
private:
    GnssMultiClientHidlILocHidlGnss* mLocHidlGnss = nullptr;
};

} //namespace garden

#endif //GNSS_MULTI_CLIENT_HIDL_ILOCHIDLGNSS_CB_H
