/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "BTConfigStore.h"
#include <cutils/properties.h>
#include <string.h>
#include "data_handler.h"
#include "btcommon_interface_defs.h"

#define LOG_TAG "vendor.qti.hardware.btconfigstore@2.0-BTConfigStore"

using ::android::hardware::bluetooth::V1_0::implementation::DataHandler;

namespace vendor {
namespace qti {
namespace hardware {
namespace btconfigstore {
namespace V2_0 {
namespace implementation {

// Methods from ::vendor::qti::hardware::btconfigstore::V2_0::IBTConfigStore follow.
Return<void> BTConfigStore::getVendorProperties(const uint32_t vPropType,
                                                    getVendorProperties_cb _hidl_cb) {
  int ret = 0;
  char soc_name[PROPERTY_VALUE_MAX] = {'\0'};
  char splita2dp[PROPERTY_VALUE_MAX];
  char a2dp_offload_cap[PROPERTY_VALUE_MAX];
  char wipower_support[PROPERTY_VALUE_MAX];
  char aac_frame_ctl[PROPERTY_VALUE_MAX];
  char swb_enabled[PROPERTY_VALUE_MAX];
  char swbpm_enabled[PROPERTY_VALUE_MAX];
  char a2dp_multicast[PROPERTY_VALUE_MAX];
  char twsp_state_support[PROPERTY_VALUE_MAX];
  char max_pow_support[PROPERTY_VALUE_MAX];
  Result result = Result::SUCCESS;
  hidl_vec<VendorProperty> vProp;

  switch (vPropType) {
    case BT_PROP_ALL :
      ret = property_get("persist.vendor.qcom.bluetooth.soc", soc_name, "pronto");
      ALOGI("%s:: Bluetooth soc type set to: %s, ret: %d",__func__, soc_name, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.enable.splita2dp",
          splita2dp, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.enable.splita2dp: %s, ret: %d",
          __func__, splita2dp, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.a2dp_offload_cap",
          a2dp_offload_cap, "");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.a2dp_offload_cap: %s, ret: %d",
          __func__, a2dp_offload_cap, ret);

      if (ret == 0) {
        ALOGI("%s: a2dp_offload_cap string is empty", __func__);
      }

      ret = property_get("ro.vendor.bluetooth.wipower", wipower_support,
          "false");
      ALOGI("%s:: ro.vendor.bluetooth.wipower: %s, ret: %d", __func__,
          wipower_support, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled",
          aac_frame_ctl, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled: %s, ret: %d",
          __func__, aac_frame_ctl, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.enable.swb",
          swb_enabled, "true");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.enable.swb: %s, ret: %d",
          __func__, swb_enabled, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.enable.swbpm",
          swbpm_enabled, "true");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.enable.swbpm: %s, ret: %d",
          __func__, swbpm_enabled, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled",
          a2dp_multicast, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled: %s, ret: %d",
          __func__, a2dp_multicast, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.twsp_state.enabled",
          twsp_state_support, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.twsp_state.enabled: %s, ret: %d",
          __func__, twsp_state_support, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      ret = property_get("persist.vendor.qcom.bluetooth.max_power_support",
          max_pow_support, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.max_power_support: %s, ret: %d",
          __func__, max_pow_support, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_SOC_TYPE, soc_name},
        {BT_PROP_SPILT_A2DP, splita2dp},
        {BT_PROP_A2DP_OFFLOAD_CAP, a2dp_offload_cap},
        {BT_PROP_WIPOWER, wipower_support},
        {BT_PROP_AAC_FRAME_CTL, aac_frame_ctl},
        {BT_PROP_SWB_ENABLE, swb_enabled},
        {BT_PROP_SWBPM_ENABLE, swbpm_enabled},
        {BT_PROP_A2DP_MCAST_TEST, a2dp_multicast},
        {BT_PROP_TWSP_STATE, twsp_state_support},
        {BT_PROP_MAX_POWER, max_pow_support},
      };

      break;

    case BT_PROP_SWB_ENABLE :
      ret = property_get("persist.vendor.qcom.bluetooth.enable.swb",
          swb_enabled, "true");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.enable.swb: %s, ret: %d",
          __func__, swb_enabled, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_SWB_ENABLE, swb_enabled},
      };

      break;

    case BT_PROP_SWBPM_ENABLE :
      ret = property_get("persist.vendor.qcom.bluetooth.enable.swbpm",
          swbpm_enabled, "true");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.enable.swbpm: %s, ret: %d",
          __func__, swbpm_enabled, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_SWBPM_ENABLE, swbpm_enabled},
      };

      break;

    case BT_PROP_SOC_TYPE :
      if (soc_name[0]== '\0') {
        ret = property_get("persist.vendor.qcom.bluetooth.soc", soc_name, "pronto");

        if (ret == 0) {
          result = Result::FAILED;
        }
      }
      ALOGI("%s:: soc_name: %s",__func__, soc_name);
      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_SOC_TYPE, soc_name},
      };

      break;

    case BT_PROP_SPILT_A2DP :
      ret = property_get("persist.vendor.qcom.bluetooth.enable.splita2dp",
          splita2dp, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.enable.splita2dp: %s, ret: %d",
          __func__, splita2dp, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_SPILT_A2DP, splita2dp},
      };

      break;

    case BT_PROP_A2DP_OFFLOAD_CAP :
      ret = property_get("persist.vendor.qcom.bluetooth.a2dp_offload_cap",
          a2dp_offload_cap, "");
      ALOGI("%s:persist.vendor.qcom.bluetooth.a2dp_offload_cap: %s, ret: %d",
          __func__, a2dp_offload_cap, ret);

      if (ret == 0) {
        ALOGI("%s: a2dp_offload_cap string is empty", __func__);
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_A2DP_OFFLOAD_CAP, a2dp_offload_cap},
      };

      break;

    case BT_PROP_WIPOWER:
      ret = property_get("ro.vendor.bluetooth.wipower", wipower_support,
          "false");
      ALOGI("%s:: ro.vendor.bluetooth.wipower: %s, ret: %d",
          __func__, wipower_support, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_WIPOWER, wipower_support},
      };

      break;

    case BT_PROP_AAC_FRAME_CTL:
      ret = property_get("persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled",
          aac_frame_ctl, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.aac_frm_ctl.enabled: %s, ret: %d",
          __func__, aac_frame_ctl, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_AAC_FRAME_CTL, aac_frame_ctl},
      };
      break;

    case BT_PROP_A2DP_MCAST_TEST:
      ret = property_get("persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled",
          a2dp_multicast, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.a2dp_mcast_test.enabled: %s, ret: %d",
          __func__, a2dp_multicast, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_A2DP_MCAST_TEST, a2dp_multicast},
      };

      break;

    case BT_PROP_TWSP_STATE:
      ret = property_get("persist.vendor.qcom.bluetooth.twsp_state.enabled",
          twsp_state_support, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.twsp_state.enabled: %s, ret: %d",
          __func__, twsp_state_support, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_TWSP_STATE, twsp_state_support},
      };

      break;

    case FM_PROP_NOTCH_VALUE:
      char notch_value[PROPERTY_VALUE_MAX];
      ret = property_get("vendor.notch.value", notch_value, "");
      ALOGI("%s:: vendor.notch.value: %s, ret: %d",
          __func__, notch_value, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {FM_PROP_NOTCH_VALUE, notch_value},
      };

      break;

    case FM_PROP_HW_INIT:
      char fm_hw_init[PROPERTY_VALUE_MAX];
      ret = property_get("vendor.hw.fm.init", fm_hw_init, "0");
      ALOGI("%s:: vendor.hw.fm.init: %s, ret: %d",
          __func__, fm_hw_init, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {FM_PROP_HW_INIT, fm_hw_init},
      };

      break;

    case FM_STATS_PROP:
      char fm_stats[PROPERTY_VALUE_MAX];
      ret = property_get("persist.vendor.fm.stats", fm_stats, "false");
      ALOGI("%s:: persist.vendor.fm.stats: %s, ret: %d",
          __func__, fm_stats, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {FM_STATS_PROP, fm_stats},
      };

      break;

    case FM_PROP_WAN_RATCONF:
      char fm_wan_ratconf[PROPERTY_VALUE_MAX];
      ret = property_get("persist.vendor.fm_wan.ratconf", fm_wan_ratconf, "0");
      ALOGI("%s:: persist.vendor.fm_wan.ratconf: %s, ret: %d",
          __func__, fm_wan_ratconf, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {FM_PROP_WAN_RATCONF, fm_wan_ratconf},
      };

      break;

    case FM_PROP_BTWLAN_LPFENABLER:
      char fm_btwlan_lpfenabler[PROPERTY_VALUE_MAX];
      ret = property_get("persist.vendor.btwlan.lpfenabler", fm_btwlan_lpfenabler, "0");
      ALOGI("%s:: persist.vendor.btwlan.lpfenabler: %s, ret: %d",
          __func__, fm_btwlan_lpfenabler, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {FM_PROP_BTWLAN_LPFENABLER, fm_btwlan_lpfenabler},
      };

      break;

    case BT_PROP_MAX_POWER:
      ret = property_get("persist.vendor.qcom.bluetooth.max_power_support",
            max_pow_support, "false");
      ALOGI("%s:: persist.vendor.qcom.bluetooth.max_power_support: %s, ret: %d",
          __func__, max_pow_support, ret);

      if (ret == 0) {
        result = Result::FAILED;
      }

      vProp =  hidl_vec<VendorProperty>{
        {BT_PROP_MAX_POWER, max_pow_support},
      };

      break;

    default:
      ALOGI("%s:: request not handled vPropType: %d", __func__, vPropType);
      result = Result::FAILED;
      break;
  }

  _hidl_cb(result, vProp);
  return Void();
}

Return<Result> BTConfigStore::setVendorProperty(const VendorProperty& vendorProp) {

    const char *vPropValue = vendorProp.value.c_str();

    ALOGI("%s:: type: 0x%04x, value: %s", __func__, vendorProp.type, vPropValue);
    switch (vendorProp.type) {
      case FM_PROP_CTL_START:
        property_set("ctl.start", vPropValue);
        break;

      case FM_PROP_CTL_STOP:
         property_set("ctl.stop", vPropValue);
        break;

      case FM_PROP_HW_INIT:
         property_set("vendor.hw.fm.init", vPropValue);
        break;

      case FM_PROP_NOTCH_VALUE:
        property_set("vendor.notch.value", vPropValue);
        break;

      case FM_PROP_HW_MODE:
        property_set("vendor.hw.fm.mode", vPropValue);
        break;

      case BT_PROP_STACK_TIMEOUT: {
        if (!strcmp(vPropValue, "true")) {
          DataHandler *data_handler = DataHandler::Get();
          if (data_handler != nullptr) {
            ALOGW("%s: timeout in stack informing HIDL", __func__);
            data_handler->StackTimeoutTriggered();
          }
        }
        break;
      }

      default:
        break;

    }
    return Result::SUCCESS;
}

Return<void> BTConfigStore::getHostAddOnFeatures(getHostAddOnFeatures_cb _hidl_cb) {
  DataHandler *data_handler = DataHandler::Get();
  HostAddOnFeatures_t* host_features = NULL;
  HostAddOnFeatures add_on_features;
  Result result = Result::SUCCESS;

  if (data_handler != nullptr) {
    host_features = data_handler->GetHostAddOnFeatures();
    ALOGI("%s: %p", __func__, host_features);

    if (host_features != NULL && host_features->feat_mask_len > 0) {
      add_on_features.feat_mask_len = host_features->feat_mask_len;
      add_on_features.features.setToExternal((uint8_t *)&host_features->features[0],
         host_features->feat_mask_len);

    } else {
      result = Result::FAILED;
    }
  } else {
    result = Result::FAILED;
  }
  _hidl_cb(result, add_on_features);
  return Void();

}

Return<void> BTConfigStore::getControllerAddOnFeatures(
                        getControllerAddOnFeatures_cb _hidl_cb) {
  DataHandler *data_handler = DataHandler::Get();
  SocAddOnFeatures_t* soc_features = NULL;
  ControllerAddOnFeatures add_on_features;
  Result result = Result::SUCCESS;

  if (data_handler != nullptr) {
    soc_features = data_handler->GetSoCAddOnFeatures();
    ALOGI("%s: %p", __func__, soc_features);

    if (soc_features != NULL) {
      add_on_features.product_id = soc_features->product_id;
      add_on_features.rsp_version = soc_features->rsp_version;
      add_on_features.feat_mask_len = soc_features->feat_mask_len;
      add_on_features.features.setToExternal((uint8_t *)&soc_features->features[0],
         soc_features->feat_mask_len);
    } else {
      result = Result::FAILED;
    }
  } else {
    result = Result::FAILED;
  }
  _hidl_cb(result, add_on_features);
  return Void();

}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IBTConfigStore* HIDL_FETCH_IBTConfigStore(const char* /*name*/) {
    return new BTConfigStore();
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace btconfigstore
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
