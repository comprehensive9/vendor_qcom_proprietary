/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <telephony/ril.h>
#include <marshal/GetSimStatusResponse.h>

template <>
Marshal::Result Marshal::write<RIL_SimAuthentication>(const RIL_SimAuthentication &arg) {
    WRITE_AND_CHECK(arg.authContext);
    WRITE_AND_CHECK(arg.authData);
    WRITE_AND_CHECK(arg.aid);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SimAuthentication>(RIL_SimAuthentication &out) const {
    READ_AND_CHECK(out.authContext);
    READ_AND_CHECK(out.authData);
    READ_AND_CHECK(out.aid);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SimAuthentication>(RIL_SimAuthentication &arg) {
    release(arg.authData);
    release(arg.aid);
    return Marshal::Result::SUCCESS;
}
template <>
Marshal::Result Marshal::write<RIL_AppType>(const RIL_AppType &arg) {
    WRITE_AND_CHECK(static_cast<typename std::underlying_type<RIL_AppType>::type>(arg));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_AppType>(RIL_AppType &out) const {
    typename std::underlying_type<RIL_AppType>::type tmp = 0;
    READ_AND_CHECK(tmp);
    RIL_AppType tmpe = static_cast<RIL_AppType>(tmp);
    Marshal::Result ret = Marshal::Result::FAILURE;
    switch(tmpe) {
        case RIL_APPTYPE_UNKNOWN:
        case RIL_APPTYPE_SIM:
        case RIL_APPTYPE_USIM:
        case RIL_APPTYPE_RUIM:
        case RIL_APPTYPE_CSIM:
        case RIL_APPTYPE_ISIM:
            ret = Marshal::Result::SUCCESS;
            out = tmpe;
            break;
        default:
            break;
    }
    return ret;
    
}

template <>
Marshal::Result Marshal::write<RIL_AppState>(const RIL_AppState &arg) {
    WRITE_AND_CHECK(static_cast<typename std::underlying_type<RIL_AppState>::type>(arg));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_AppState>(RIL_AppState &out) const {
    typename std::underlying_type<RIL_AppState>::type tmp = 0;
    READ_AND_CHECK(tmp);
    RIL_AppState tmpe = static_cast<RIL_AppState>(tmp);
    Marshal::Result ret = Marshal::Result::FAILURE;
    switch(tmpe) {
        case RIL_APPSTATE_UNKNOWN:
        case RIL_APPSTATE_DETECTED:
        case RIL_APPSTATE_PIN:
        case RIL_APPSTATE_PUK:
        case RIL_APPSTATE_SUBSCRIPTION_PERSO:
        case RIL_APPSTATE_READY:
            ret = Marshal::Result::SUCCESS;
            out = tmpe;
            break;
        default:
            break;
    }
    return ret;
}

template <>
Marshal::Result Marshal::write<RIL_PersoSubstate>(const RIL_PersoSubstate &arg) {
    WRITE_AND_CHECK(static_cast<typename std::underlying_type<RIL_PersoSubstate>::type>(arg));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_PersoSubstate>(RIL_PersoSubstate &out) const {
    typename std::underlying_type<RIL_PersoSubstate>::type tmp = 0;
    READ_AND_CHECK(tmp);
    RIL_PersoSubstate tmpe = static_cast<RIL_PersoSubstate>(tmp);
    Marshal::Result ret = Marshal::Result::FAILURE;
    switch(tmpe) {
        case RIL_PERSOSUBSTATE_UNKNOWN:
        case RIL_PERSOSUBSTATE_IN_PROGRESS:
        case RIL_PERSOSUBSTATE_READY:
        case RIL_PERSOSUBSTATE_SIM_NETWORK:
        case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET:
        case RIL_PERSOSUBSTATE_SIM_CORPORATE:
        case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER:
        case RIL_PERSOSUBSTATE_SIM_SIM:
        case RIL_PERSOSUBSTATE_SIM_NETWORK_PUK:
        case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK:
        case RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK:
        case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK:
        case RIL_PERSOSUBSTATE_SIM_SIM_PUK:
        case RIL_PERSOSUBSTATE_RUIM_NETWORK1:
        case RIL_PERSOSUBSTATE_RUIM_NETWORK2:
        case RIL_PERSOSUBSTATE_RUIM_HRPD:
        case RIL_PERSOSUBSTATE_RUIM_CORPORATE:
        case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER:
        case RIL_PERSOSUBSTATE_RUIM_RUIM:
        case RIL_PERSOSUBSTATE_RUIM_NETWORK1_PUK:
        case RIL_PERSOSUBSTATE_RUIM_NETWORK2_PUK:
        case RIL_PERSOSUBSTATE_RUIM_HRPD_PUK:
        case RIL_PERSOSUBSTATE_RUIM_CORPORATE_PUK:
        case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK:
        case RIL_PERSOSUBSTATE_RUIM_RUIM_PUK:
            ret = Marshal::Result::SUCCESS;
            out = tmpe;
            break;
        default:
            break;
    }
    return ret;
}

template <>
Marshal::Result Marshal::write<RIL_PinState>(const RIL_PinState &arg) {
    WRITE_AND_CHECK(static_cast<typename std::underlying_type<RIL_PinState>::type>(arg));
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_PinState>(RIL_PinState &out) const {
    typename std::underlying_type<RIL_PinState>::type tmp = 0;
    READ_AND_CHECK(tmp);
    RIL_PinState tmpe = static_cast<RIL_PinState>(tmp);
    Marshal::Result ret = Marshal::Result::FAILURE;
    switch(tmpe) {
        case RIL_PINSTATE_UNKNOWN:
        case RIL_PINSTATE_ENABLED_NOT_VERIFIED:
        case RIL_PINSTATE_ENABLED_VERIFIED:
        case RIL_PINSTATE_DISABLED:
        case RIL_PINSTATE_ENABLED_BLOCKED:
        case RIL_PINSTATE_ENABLED_PERM_BLOCKED:
            ret = Marshal::Result::SUCCESS;
            out = tmpe;
            break;
        default:
            break;
    }
    return ret;
}

template <>
Marshal::Result Marshal::write<RIL_AppStatus>(const RIL_AppStatus &arg) {
    WRITE_AND_CHECK(arg.app_type);
    WRITE_AND_CHECK(arg.app_state);
    WRITE_AND_CHECK(arg.perso_substate);
    WRITE_AND_CHECK(arg.aid_ptr);
    WRITE_AND_CHECK(arg.app_label_ptr);
    WRITE_AND_CHECK(arg.pin1_replaced);
    WRITE_AND_CHECK(arg.pin1);
    WRITE_AND_CHECK(arg.pin2);
    return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_AppStatus>(RIL_AppStatus &out) const {
    READ_AND_CHECK(out.app_type);
    READ_AND_CHECK(out.app_state);
    READ_AND_CHECK(out.perso_substate);
    READ_AND_CHECK(out.aid_ptr);
    READ_AND_CHECK(out.app_label_ptr);
    READ_AND_CHECK(out.pin1_replaced);
    READ_AND_CHECK(out.pin1);
    READ_AND_CHECK(out.pin2);
    return Marshal::Result::SUCCESS;
}
template <>
Marshal::Result Marshal::write<RIL_CardStatus_v6>(const RIL_CardStatus_v6& arg) {

  WRITE_AND_CHECK(static_cast<uint32_t>(arg.card_state));
  WRITE_AND_CHECK(arg.physical_slot_id);
  WRITE_AND_CHECK(arg.atr);
  WRITE_AND_CHECK(arg.iccid);
  WRITE_AND_CHECK(arg.universal_pin_state);
  WRITE_AND_CHECK(arg.gsm_umts_subscription_app_index);
  WRITE_AND_CHECK(arg.cdma_subscription_app_index);
  WRITE_AND_CHECK(arg.ims_subscription_app_index);
  WRITE_AND_CHECK(arg.num_applications);
  WRITE_AND_CHECK(arg.applications,arg.num_applications);
  WRITE_AND_CHECK(arg.eid);
  
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CardStatus_v6>(RIL_CardStatus_v6& arg) const {

  READ_AND_CHECK(*reinterpret_cast<uint32_t*>(&arg.card_state));
  READ_AND_CHECK(arg.physical_slot_id);
  READ_AND_CHECK(arg.atr);
  READ_AND_CHECK(arg.iccid);
  READ_AND_CHECK(arg.universal_pin_state);
  READ_AND_CHECK(arg.gsm_umts_subscription_app_index);
  READ_AND_CHECK(arg.cdma_subscription_app_index);
  READ_AND_CHECK(arg.ims_subscription_app_index);
  READ_AND_CHECK(arg.num_applications);
  READ_AND_CHECK(arg.applications);
  READ_AND_CHECK(arg.eid);
  
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_AppStatus>(RIL_AppStatus& arg) {
  release(arg.aid_ptr);
  release(arg.app_label_ptr);
  return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_CardStatus_v6>(RIL_CardStatus_v6& arg) {
   release(arg.atr);
   release(arg.iccid);
   release(arg.applications,arg.num_applications);
   release(arg.eid);
   return Result::SUCCESS;
}
