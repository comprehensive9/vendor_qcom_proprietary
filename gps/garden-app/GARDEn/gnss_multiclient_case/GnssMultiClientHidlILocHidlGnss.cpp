/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_ILocHidlGnss"

#include "GnssMultiClientHidlILocHidlGnss.h"
#include "GnssMultiClientCaseUtils.h"

#include <android/hardware/gnss/1.0/types.h>

using ::vendor::qti::gnss::V3_0::ILocHidlGnss;

namespace garden {

#define getUserInputEnterToContinue GmccUtils::get().getUserInputEnterToContinue
#define getUserInputInt GmccUtils::get().getUserInputInt
#define getUserInputDouble GmccUtils::get().getUserInputDouble
#define getUserInputMask64Bit GmccUtils::get().getUserInputMask64Bit
#define getUserInputString GmccUtils::get().getUserInputString
#define getUserInputYesNo GmccUtils::get().getUserInputYesNo
#define getUserInputSessionMode GmccUtils::get().getUserInputSessionMode
#define getUserInputTrackingOptions GmccUtils::get().getUserInputTrackingOptions
#define getUserInputLocClientIndex GmccUtils::get().getUserInputLocClientIndex
#define getUserInputGnssConfig GmccUtils::get().getUserInputGnssConfig
#define getUserInputGnssConfigFlagsMask GmccUtils::get().getUserInputGnssConfigFlagsMask
#define getUserInputGnssConfigBlacklistSvId GmccUtils::get().getUserInputGnssConfigBlacklistSvId
#define getUserInputGnssSvIdSource GmccUtils::get().getUserInputGnssSvIdSource
#define getUserInputGnssSvType GmccUtils::get().getUserInputGnssSvType
#define getUserInputGnssSvTypesMask GmccUtils::get().getUserInputGnssSvTypesMask
#define printGnssConfigBlacklistSvId GmccUtils::get().printGnssConfigBlacklistSvId
#define convertGnssSvIdMaskToList GmccUtils::get().convertGnssSvIdMaskToList
#define strUtilTokenize GmccUtils::get().strUtilTokenize


/* GnssMultiClientHidlILocHidlGnss static elements */
GnssMultiClientHidlILocHidlGnss* GnssMultiClientHidlILocHidlGnss::sInstance = nullptr;

/* GnssMultiClientHidlILocHidlGnss Public APIs */
GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::menuTest()
{
    bool exit_loop = false;
    GARDEN_RESULT gardenResult = GARDEN_RESULT_INVALID;

    while(!exit_loop) {
        PRINTLN("\n\n"
                "1: Create HIDL client for ILocHidlGnss 4.0 \n"
                "1001: ILocHidlIzatConfig 1.0 -> init \n"
                "1002: ILocHidlIzatConfig 1.0 -> readConfig \n"
                "2001: ILocHidlIzatSubscription 1.0 -> init \n"
                "2002: ILocHidlIzatSubscription -> boolDataItemUpdate (Opt-in)\n"
                "3001: ILocHidlIzatConfig_init\n"
                "3002: ILocHidlIzatConfig_reset\n"
                "3003: ILocHidlGnssConfigService_setGnssSvTypeConfig\n"
                "3004: ILocHidlGnssConfigService_getGnssSvTypeConfig\n"
                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            gardenResult = createHidlClient();
            break;
        case 1001:
            gardenResult = ILocHidlIzatConfig_1_0_init();
            break;
        case 1002:
            gardenResult = ILocHidlIzatConfig_1_0_readConfig();
            break;
        case 2001:
            gardenResult = ILocHidlIzatSubscription_1_0_init();
            break;
        case 2002:
            {
                int result = getUserInputInt("Enable: 1, disable: 0");
                gardenResult = ILocHidlIzatSubscription_1_1_boolDataItemUpdate(result);
                break;
            }
        case 3001:
                gardenResult = ILocHidlGnssConfigService_init();
                break;
        case 3002:
                gardenResult = ILocHidlGnssConfigService_reset();
                break;
        case 3003:
            {
                PRINTLN("Constellation allowed to disable: glo / gal / bds / qzss/ navic");
                GnssSvTypesMask enableMask = getUserInputGnssSvTypesMask
                    ("*** Enable constellation Input ***");
                GnssSvTypesMask disableMask = getUserInputGnssSvTypesMask
                    ("*** disable constellation Input ***");
                if (!(enableMask & disableMask)) {
                    gardenResult = ILocHidlGnssConfigService_setGnssSvTypeConfig(enableMask,
                        disableMask);
                } else {
                    PRINTERROR("Same Constallation cannot be in enable and disable mask!!!");
                }
                break;
            }
        case 3004:
                gardenResult = ILocHidlGnssConfigService_getGnssSvTypeConfig();
                break;
        case 99:
            continue;
        case 0:
            gardenResult = GARDEN_RESULT_ABORT;
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }

        if (0 != choice) {
            PRINTLN("\nExecution Result: %d", gardenResult);
            getUserInputEnterToContinue();
        }
    }

    return gardenResult;
}

/* Callbacks registered with HIDL framework */
void LocHidlGnssDeathRecipient::serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) {
    PRINTERROR("ILocHidlGnss service died");
    GnssMultiClientHidlILocHidlGnss::get().mLocHidlGnssIface = nullptr;
    GnssMultiClientHidlILocHidlGnss::get().mLocHidlGnssIface_V4_0 = nullptr;
    //get LocHidl service
    GnssMultiClientHidlILocHidlGnss::get().createHidlClient();
}

template <class T>
GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::checkResultOk(T& Result,
    const std::string printMsg) {
    if (Result.isOk()) {
        PRINTLN("%s success.", printMsg.c_str());
        return GARDEN_RESULT_PASSED;
    } else {
        PRINTERROR("%s failed.", printMsg.c_str());
        return GARDEN_RESULT_FAILED;
    }
}

void GnssMultiClientHidlILocHidlGnss::initVendorHal() {
    if (mLocHidlGnssIface == nullptr) {
        LOC_LOGd("Getting gnssHal 4.0");
        mLocHidlGnssIface_V4_0 = ILocHidlGnss_V4_0::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (mLocHidlGnssIface_V4_0 != nullptr) {
            mLocHidlGnssIface = mLocHidlGnssIface_V4_0;
            return;
        }
        LOC_LOGd("gnssHal 4.0 was null, trying 3.0");
        mLocHidlGnssIface = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
    }
}

/* GnssMultiClientHidlILocHidlGnss TEST CASES */
GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::createHidlClient()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocHidlGnssIface) {
        PRINTLN("ILocHidlGnss instance already initalized!");
        return GARDEN_RESULT_PASSED;
    }
    // Get IGNSS service
    initVendorHal();
    if (mLocHidlGnssIface != nullptr) {
        PRINTLN("ILocHidlGnss::getService() success.");

        // Link to IGNSS service Death
        mLocHidlGnssDeathRecipient = new LocHidlGnssDeathRecipient();
        android::hardware::Return<bool> linked =
            mLocHidlGnssIface->linkToDeath(mLocHidlGnssDeathRecipient, 0);
        if (linked.isOk() && linked) {
            PRINTLN("mLocHidlGnssIface->linkToDeath() success.");
            // Get Extension : IGnssConfiguration 1.1
            auto izatConfig = mLocHidlGnssIface->getExtensionLocHidlIzatConfig();
            gardenResult = checkResultOk(izatConfig,
                               "mLocHidlGnssIface->getExtensionLocHidlIzatConfig");
            mLocHidlGnssExtensionIzatConfig = izatConfig;

            // Get Extension : IzatSubscribtion 1.1
            auto izatSubscribe = mLocHidlGnssIface->getExtensionLocHidlIzatSubscription_1_1();
            gardenResult = checkResultOk(izatSubscribe,
                               "mLocHidlGnssIface->getExtensionLocHidlIzatSubscription");
            mLocHidlIzatSubscription = izatSubscribe;

            //Get Extension : ILocHidlGnssConfigService
            if (mLocHidlGnssIface_V4_0 != nullptr) {
                auto gnssConfigService = mLocHidlGnssIface_V4_0->
                    getExtensionLocHidlGnssConfigService_4_0();
                gardenResult = checkResultOk(gnssConfigService,
                    "mLocHidlGnssIface->getExtensionLocHidlGnssConfigService_4_0()");
                mLocHidlGnssConfigService_V4_0 = gnssConfigService;
                mLocHidlGnssConfigService_V2_1 = mLocHidlGnssConfigService_V4_0;
            }  else {
                auto gnssConfigService = mLocHidlGnssIface->
                    getExtensionLocHidlGnssConfigService_2_1();
                gardenResult = checkResultOk(gnssConfigService,
                    "mLocHidlGnssIface->getExtensionLocHidlGnssConfigService_2_1()");
                mLocHidlGnssConfigService_V2_1 = gnssConfigService;
            }
        } else {
            PRINTERROR("mLocHidlGnssIface->linkToDeath() failed, error: %s",
                    linked.description().c_str());
        }
    } else {
        PRINTERROR("ILocHidlGnss::getService() call failed.");
    }

    return gardenResult;
}

//init 1.0
template <class T>
Return<bool> GnssMultiClientHidlILocHidlGnss::ILocHidlExtinit(T& ext) {
    return ext->init(mLocHidlGnssCb);
}

//init 4.0
template <class T>
Return<bool> GnssMultiClientHidlILocHidlGnss::ILocHidlExtinit_4_0(T& ext) {
    return ext->init_4_0(mLocHidlGnssCb);
}

GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlIzatConfig_1_0_init()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mLocHidlGnssIface != nullptr) {
        if (mLocHidlGnssExtensionIzatConfig != nullptr) {
            auto result = ILocHidlExtinit(mLocHidlGnssExtensionIzatConfig);
            gardenResult = checkResultOk(result, "mLocHidlGnssExtensionIzatConfig->init");
        } else {
            PRINTERROR("mLocHidlGnssExtensionIzatConfig null");
        }
    } else {
        PRINTERROR("LOC HIDL client not created.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlIzatConfig_1_0_readConfig()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mLocHidlGnssIface != nullptr) {
        if (mLocHidlGnssExtensionIzatConfig != nullptr) {
            auto result = mLocHidlGnssExtensionIzatConfig->readConfig();
            gardenResult = checkResultOk(result, "mLocHidlGnssExtensionIzatConfig->readConfig");
        } else {
            PRINTERROR("mLocHidlGnssExtensionIzatConfig null");
        }
    } else {
        PRINTERROR("LOC HIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlIzatSubscription_1_0_init() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocHidlGnssIface != nullptr) {
        if (mLocHidlIzatSubscription != nullptr) {
            auto result = ILocHidlExtinit(mLocHidlIzatSubscription);
            gardenResult = checkResultOk(result, "mLocHidlIzatSubscription->init");
        } else {
            PRINTERROR("mLocHidlIzatSubscription null");
        }
    } else {
        PRINTERROR("LOC HIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlIzatSubscription_1_1_boolDataItemUpdate
    (int result) {
    std::vector<ILocHidlIzatSubscription::BoolDataItem> dataItemArray;
    ILocHidlIzatSubscription::BoolDataItem dataItem;
    dataItem.id = ENH_DATA_ITEM_ID;
    dataItem.enabled = (result > 0) ? true: false;
    dataItemArray.push_back(dataItem);
    auto r = mLocHidlIzatSubscription->boolDataItemUpdate(dataItemArray);
    return checkResultOk(r, "mLocHidlIzatSubscription->boolDataItemUpdate");
}

GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlGnssConfigService_init() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocHidlGnssIface_V4_0 != nullptr) {
        if (mLocHidlGnssConfigService_V4_0 != nullptr) {
            auto result = ILocHidlExtinit_4_0(mLocHidlGnssConfigService_V4_0);
            gardenResult = checkResultOk(result, "mLocHidlGnssConfigService_V4_0->init_4_0");
        } else {
            PRINTERROR("mLocHidlGnssConfigService_V4_0 null");
        }
    }
    else if (mLocHidlGnssIface != nullptr) {
        if (mLocHidlGnssConfigService_V2_1 != nullptr) {
            auto result = ILocHidlExtinit(mLocHidlGnssConfigService_V2_1);
            gardenResult = checkResultOk(result, "mLocHidlGnssConfigService_V2_1->init");
        } else {
            PRINTERROR("mLocHidlGnssConfigService_V2_1 null");
        }
    } else {
        PRINTERROR("LOC HIDL client not created.");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlGnssConfigService_reset() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocHidlGnssConfigService_V2_1 != nullptr) {
       auto r = mLocHidlGnssConfigService_V2_1->resetGnssSvTypeConfig();
       gardenResult = checkResultOk(r, "mLocHidlGnssConfigService_V2_1->resetGnssSvTypeConfig()");
    }
    return gardenResult;
}

template <class T>
void GnssMultiClientHidlILocHidlGnss::convertGnssTypeMaskToConstellationVec(GnssSvTypesMask
    enableMask, GnssSvTypesMask disableMask,
    std::vector<T>& disableVec) {
    //push disable constellation
    if (GNSS_SV_TYPES_MASK_GLO_BIT & disableMask) {
        disableVec.push_back(T::GLONASS);
    }
    if (GNSS_SV_TYPES_MASK_BDS_BIT & disableMask) {
        disableVec.push_back(T::BEIDOU);
    }
    if (GNSS_SV_TYPES_MASK_QZSS_BIT & disableMask) {
        disableVec.push_back(T::QZSS);
    }
    if (GNSS_SV_TYPES_MASK_GAL_BIT & disableMask) {
        disableVec.push_back(T::GALILEO);
    }

   //enable constellation as inverted
    if (GNSS_SV_TYPES_MASK_GLO_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientHidlILocHidlGnssCb::
            convertConstellationToint(T::GLONASS)));
    }
    if (GNSS_SV_TYPES_MASK_BDS_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientHidlILocHidlGnssCb::
            convertConstellationToint(T::BEIDOU)));
    }
    if (GNSS_SV_TYPES_MASK_QZSS_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientHidlILocHidlGnssCb::
            convertConstellationToint(T::QZSS)));
    }
    if (GNSS_SV_TYPES_MASK_GAL_BIT & enableMask) {
        disableVec.push_back((T)(~GnssMultiClientHidlILocHidlGnssCb::
            convertConstellationToint(T::GALILEO)));
    }
}


GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlGnssConfigService_setGnssSvTypeConfig(
    GnssSvTypesMask enableMask, GnssSvTypesMask disableMask) {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    PRINTLN("Enabled mask: %0x, Disable Mask: %0x", enableMask, disableMask);
    if (mLocHidlGnssIface_V4_0 != nullptr && mLocHidlGnssConfigService_V4_0 != nullptr) {
        std::vector<LocHidlGnssConstellationType> disableVec4_0 = {};
        if (disableMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
            disableVec4_0.push_back(LocHidlGnssConstellationType::IRNSS);
        }
        convertGnssTypeMaskToConstellationVec(enableMask, disableMask, disableVec4_0);
        if (enableMask & GNSS_SV_TYPES_MASK_NAVIC_BIT) {
            disableVec4_0.push_back((LocHidlGnssConstellationType)(~(NAVIC)));
        }
        auto r = mLocHidlGnssConfigService_V4_0->setGnssSvTypeConfig_4_0(disableVec4_0);
        gardenResult = checkResultOk(r, "mLocHidlGnssConfigService_V4_0->setGnssSvTypeConfig_4_0");
    } else if (mLocHidlGnssIface != nullptr && mLocHidlGnssConfigService_V2_1 != nullptr) {
        std::vector<GnssConstellationType_V1_0> disableVec2_1 = {};
        convertGnssTypeMaskToConstellationVec(enableMask, disableMask, disableVec2_1);
        auto r = mLocHidlGnssConfigService_V2_1->setGnssSvTypeConfig(disableVec2_1);
        gardenResult = checkResultOk(r, "mLocHidlGnssConfigService_V2_1->setGnssSvTypeConfig");
    } else {
        PRINTERROR("Unable to execute ILocHidlGnssConfigService_setGnssSvTypeConfig");
    }
    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlILocHidlGnss::ILocHidlGnssConfigService_getGnssSvTypeConfig() {
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;
    if (mLocHidlGnssIface != nullptr && mLocHidlGnssConfigService_V2_1 != nullptr) {
        auto r = mLocHidlGnssConfigService_V2_1->getGnssSvTypeConfig();
        gardenResult = checkResultOk(r, "mLocHidlGnssConfigService_V2_1->getGnssSvTypeConfig");
    }
    return gardenResult;
}
} // namespace garden
