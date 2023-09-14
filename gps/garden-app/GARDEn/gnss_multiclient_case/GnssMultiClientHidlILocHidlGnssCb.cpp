/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_ILocHidlGnssCb"

#include "GnssMultiClientHidlILocHidlGnssCb.h"
#include "GnssMultiClientCaseUtils.h"

namespace garden {

GnssMultiClientHidlILocHidlGnssCb::GnssMultiClientHidlILocHidlGnssCb(
        GnssMultiClientHidlILocHidlGnss* hidlGnss)
{
    mLocHidlGnss = hidlGnss;
}

GnssMultiClientHidlILocHidlGnssCb::~GnssMultiClientHidlILocHidlGnssCb() {}

Return<void> GnssMultiClientHidlILocHidlGnssCb::izatConfigCallback(
        const ::android::hardware::hidl_string& izatConfigContent)
{
    PRINTLN("izatConfigCallback()");
    PRINTLN("izat.conf content:\n%s", izatConfigContent.c_str());
    return Void();
}

//ILocHidlGnssConfigServiceCb
Return<void> GnssMultiClientHidlILocHidlGnssCb::getGnssSvTypeConfigCb(
        const hidl_vec<GnssConstellationType_V1_0>& disabledSvTypeList) {
    PRINTLN("GnssSvTypeConfigCb >>> ");
    std::string enableMsg, disableMsg;
    for (GnssConstellationType_V1_0 type : disabledSvTypeList) {
        int constellation = convertConstellationToint(type);
        if (0 != constellation) {
            disableMsg.append(std::to_string(constellation) + " ");
        } else {
            enableMsg.append(std::to_string(((uint8_t)(~(uint32_t)type))) + " ");
        }
    }
    PRINTLN("GPS: 1, SBAS: 2,GLONASS: 3, QZSS: 4, BEIDUO:5, GALILEO:6");
    PRINTLN("Constellation enable: %s \nConstellation disable: %s", enableMsg.c_str(),
        disableMsg.c_str());
    return Void();
}

Return<void> GnssMultiClientHidlILocHidlGnssCb::getGnssSvTypeConfigCb_4_0(
        const hidl_vec<LocHidlGnssConstellationType>& disabledSvTypeList) {
    PRINTLN("GnssSvTypeConfigCb_4_0 >>> ");
    std::string enableMsg, disableMsg;
    for (LocHidlGnssConstellationType type : disabledSvTypeList) {
        if (type == LocHidlGnssConstellationType::IRNSS) {
            disableMsg.append(std::to_string(NAVIC) + " ");
            continue;
        }
        if (NAVIC == ((uint8_t)((~(uint32_t)type)))) {
            enableMsg.append(std::to_string(NAVIC) + " ");
            continue;
        }
        int constellation = convertConstellationToint(type);
        if (0 != constellation) {
            disableMsg.append(std::to_string(constellation) + " ");
        } else {
            enableMsg.append(std::to_string((uint8_t)(~(uint32_t)type)) + " ");
        }
    }
    PRINTLN("GPS: 1, SBAS: 2,GLONASS: 3, QZSS: 4, BEIDUO:5, GALILEO:6, NAVIC:7");
    PRINTLN("Constellation enable: %s \nConstellation disable: %s", enableMsg.c_str(),
        disableMsg.c_str());
    return Void();
}
} // namespace garden
