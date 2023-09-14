/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

/**
 * PhoneMenu provides menu options to invoke Phone functions such as
 * requestSignalStrength.
 */

#include <chrono>
#include <iostream>

//#include "MyCellInfoHandler.hpp"
//#include <telux/tel/PhoneFactory.hpp>
//#include <Utils.hpp>

#include "NetworkMenu.hpp"
#include "PhoneMenu.hpp"
#include "ServingSystemMenu.hpp"
#include <telephony/ril.h>

PhoneMenu::PhoneMenu(std::string appName, std::string cursor, int phoneId, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();
  //  Get the PhoneFactory and PhoneManager instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   phoneManager_ = phoneFactory.getPhoneManager();

  //  Check if telephony subsystem is ready
  //   bool subSystemStatus = phoneManager_->isSubsystemReady();
  bool subSystemStatus = true;

  //   //  If telephony subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::cout << "\n\nTelephony subsystem is not ready, Please wait" << std::endl;
  //      std::future<bool> f = phoneManager_->onSubsystemReady();
  //      // If we want to wait unconditionally for telephony subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  //  Exit the application, if SDK is unable to initialize telephony subsystems
  if (subSystemStatus) {
    endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Elapsed Time for Subsystem to ready : " << elapsedTime.count() << "s\n"
              << std::endl;
  } else {
    std::cout << "ERROR - Unable to initialize subsystem" << std::endl;
    exit(0);
  }

  //   if(subSystemStatus) {
  //      phone_ = phoneManager_->getPhone(phoneId);
  //      // Turn on the radio if it's not available
  //      if(phone_ != nullptr) {
  //         if(phone_->getRadioState() != telux::tel::RadioState::RADIO_STATE_ON) {
  //            phone_->setRadioPower(true);
  //         }
  //      }
  //
  //      phoneListener_ = std::make_shared<MyPhoneListener>();
  //
  //      subscriptionMgr_ = telux::tel::PhoneFactory::getInstance().getSubscriptionManager();
  //      subscriptionListener_ = std::make_shared<MySubscriptionListener>();
  //      if(!subscriptionMgr_->isSubsystemReady()) {
  //         subscriptionMgr_->onSubsystemReady().get();
  //      }
  //
  //      telux::common::Status status = subscriptionMgr_->registerListener(subscriptionListener_);
  //      if(status != telux::common::Status::SUCCESS) {
  //         std::cout << "Failed to registerListener" << std::endl;
  //      }
  //
  //      status = phoneManager_->registerListener(phoneListener_);
  //      if(status != telux::common::Status::SUCCESS) {
  //         std::cout << "Failed to registerListener" << std::endl;
  //      }
  //
  //      mySignalStrengthCb_ = std::make_shared<MySignalStrengthCallback>();
  //      myVoiceRadioTechCb_ = std::make_shared<MyVoiceRadioTechnologyCallback>();
  //      myRadioPowerCb_ = std::make_shared<MyRadioPowerCallback>();
  //      myVoiceSrvStateCb_ = std::make_shared<MyVoiceServiceStateCallback>();
  //      myCellularCapabilityCb_ = std::make_shared<MyCellularCapabilityCallback>();
  //      myGetOperatingModeCb_ = std::make_shared<MyGetOperatingModeCallback>();
  //      mySetOperatingModeCb_ = std::make_shared<MySetOperatingModeCallback>();
  //   }
}

PhoneMenu::~PhoneMenu() {
  //   phoneManager_->removeListener(phoneListener_);
  //   subscriptionMgr_->removeListener(subscriptionListener_);
  //
  //   subscriptionMgr_ = nullptr;
  //   phoneManager_ = nullptr;
}

void PhoneMenu::init() {
  //   std::shared_ptr<ConsoleAppCommand> setRadioPowerCommand = std::make_shared<ConsoleAppCommand>(
  //      ConsoleAppCommand("2", "Set_radio_power", {},
  //                        std::bind(&PhoneMenu::setRadioPower, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> requestCellularCapabilitiesCommand
  //      = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
  //         "6", "Request_cellular_capabilities", {},
  //         std::bind(&PhoneMenu::requestCellularCapabilities, this, std::placeholders::_1)));
  //
  //   std::shared_ptr<ConsoleAppCommand> getSubscriptionCommand = std::make_shared<ConsoleAppCommand>(
  //      ConsoleAppCommand("7", "Get_subscription", {},
  //                        std::bind(&PhoneMenu::getSubscription, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> getOperatingModeCommand = std::make_shared<ConsoleAppCommand>(
  //      ConsoleAppCommand("8", "Get_operating_mode", {},
  //                        std::bind(&PhoneMenu::getOperatingMode, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> setOperatingModeCommand = std::make_shared<ConsoleAppCommand>(
  //      ConsoleAppCommand("9", "Set_operating_mode", {},
  //                        std::bind(&PhoneMenu::setOperatingMode, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> requestCellInfoListCommand
  //      = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
  //         "10", "Request_cell_info_list", {},
  //         std::bind(&PhoneMenu::requestCellInfoList, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> setCellInfoListRateCommand
  //      = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
  //         "11", "Set_cell_info_list_rate", {},
  //         std::bind(&PhoneMenu::setCellInfoListRate, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> servingSystemMenuCommand
  //      = std::make_shared<ConsoleAppCommand>(
  //         ConsoleAppCommand("13", "Serving_System", {},
  //                           std::bind(&PhoneMenu::servingSystemMenu, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> setECallOperatingModeCommand
  //      = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
  //         "14", "Set_eCall_operating_mode", {},
  //         std::bind(&PhoneMenu::setECallOperatingMode, this, std::placeholders::_1)));
  //   std::shared_ptr<ConsoleAppCommand> requestECallOperatingModeCommand
  //      = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
  //         "15", "Request_eCall_operating_mode", {},
  //         std::bind(&PhoneMenu::requestECallOperatingMode, this, std::placeholders::_1)));

  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListPhoneSubMenu = {
    CREATE_COMMAND(PhoneMenu::requestSignalStrength, "Get_signal_strength"),
    CREATE_COMMAND(PhoneMenu::setRadioPower, "Set_radio_power"),
    CREATE_COMMAND(PhoneMenu::allowData, "AllowData"),
    CREATE_COMMAND(PhoneMenu::enableSim, "Enable_Sim"),
    CREATE_COMMAND(PhoneMenu::getEnableSimStatus, "Get_Enable_Sim_Status"),
    CREATE_COMMAND(PhoneMenu::getVoiceRegistration, "Get_Voice_Registration_State"),
    CREATE_COMMAND(PhoneMenu::getDataRegistration, "Get_Data_Registration_State"),
    CREATE_COMMAND(PhoneMenu::getActivityInfo, "Get_ActivityInfo"),
    CREATE_COMMAND(PhoneMenu::setIndicationFilter, "setIndicationFilter"),
    CREATE_COMMAND(PhoneMenu::shutDown, "ShutDown"),
    CREATE_COMMAND(PhoneMenu::exitEmergencyCbMode, "ExitEmergencyCallbackMode"),
    CREATE_COMMAND(PhoneMenu::setLocationUpdates, "SetLocationUpdates"),
    CREATE_COMMAND(PhoneMenu::networkMenu, "Network_Selection"),
    CREATE_COMMAND(PhoneMenu::requestVoiceRadioTech, "Get_Voice_Radio_Tech"),
    CREATE_COMMAND(PhoneMenu::requestDeviceIdentity, "Get_Device_Identity"),
    CREATE_COMMAND(PhoneMenu::requestOperatorName, "Get_Operator_Name"),
    CREATE_COMMAND(PhoneMenu::requestBasebandVersion, "Get_Baseband_Version"),
    CREATE_COMMAND(PhoneMenu::requestSetUnsolCellInfoListRate, "Set_Unsol_Cell_Info_List_Rate"),
    CREATE_COMMAND(PhoneMenu::registerForUnsols, "register_for_unsols"),
    CREATE_COMMAND(PhoneMenu::getCdmaSubscription, "Get_Cdma_Subscription"),
    CREATE_COMMAND(PhoneMenu::setCdmaSubscriptionSource, "Set CDMA Subscription Source"),
    CREATE_COMMAND(PhoneMenu::getCdmaSubscriptionSource, "Get CDMA Subscription Source"),
    CREATE_COMMAND(PhoneMenu::setCdmaRoamingPreference, "Set CDMA Roaming Preference"),
    CREATE_COMMAND(PhoneMenu::getCdmaRoamingPreference, "Get CDMA Roaming Preference"),
    CREATE_COMMAND(PhoneMenu::setNetworkSelectionAutomatic, "Automatically Select Network"),
    CREATE_COMMAND(PhoneMenu::setNetworkSelectionManual, "Manually Select Network"),
    CREATE_COMMAND(PhoneMenu::getCellInfo, "Get Information About Serving/Neighboring Cells"),
  };

  addCommands(commandsListPhoneSubMenu);
  ConsoleApp::displayMenu();
}

void printSignalInfo(RIL_GW_SignalStrength info) {
  std::cout << "GW Signal Strength Information: " << std::endl;
  std::cout << "signalStrength = " << info.signalStrength << std::endl;
  std::cout << "bitErrorRate = " << info.bitErrorRate << std::endl;
  std::cout << std::endl;
}
void printSignalInfo(RIL_WCDMA_SignalStrength info) {
  if (!info.valid) return;

  std::cout << "WCDMA Signal Strength Information: " << std::endl;
  std::cout << "signalStrength = " << info.signalStrength << std::endl;
  std::cout << "bitErrorRate = " << info.bitErrorRate << std::endl;
  std::cout << "rscp = " << info.rscp << std::endl;
  std::cout << "ecio = " << info.ecio << std::endl;
  std::cout << std::endl;
}
void printSignalInfo(RIL_CDMA_SignalStrength info) {
  std::cout << "CDMA Signal Strength Information: " << std::endl;
  std::cout << "dbm = " << info.dbm << std::endl;
  std::cout << "ecio = " << info.ecio << std::endl;
  std::cout << std::endl;
}
void printSignalInfo(RIL_EVDO_SignalStrength info) {
  std::cout << "EVDO Signal Strength Information: " << std::endl;
  std::cout << "dbm = " << info.dbm << std::endl;
  std::cout << "ecio = " << info.ecio << std::endl;
  std::cout << std::endl;
}
void printSignalInfo(RIL_LTE_SignalStrength_v8 info) {
  std::cout << "LTE Signal Strength Information: " << std::endl;
  std::cout << "signalStrength = " << info.signalStrength << std::endl;
  std::cout << "rsrp = " << info.rsrp << std::endl;
  std::cout << "rsrq = " << info.rsrq << std::endl;
  std::cout << "rssnr = " << info.rssnr << std::endl;
  std::cout << "cqi = " << info.cqi << std::endl;
  std::cout << "timingAdvance = " << info.timingAdvance << std::endl;
  std::cout << std::endl;
}
void printSignalInfo(RIL_TD_SCDMA_SignalStrength info) {
  std::cout << "EVDO Signal Strength Information: " << std::endl;
  std::cout << "rscp = " << info.rscp << std::endl;
  std::cout << std::endl;
}
void printSignalInfo(RIL_NR_SignalStrength info) {
  std::cout << "NR Signal Strength Information: " << std::endl;
  std::cout << "ssRsrp = " << info.ssRsrp << std::endl;
  std::cout << "ssRsrq = " << info.ssRsrq << std::endl;
  std::cout << "ssSinr = " << info.ssSinr << std::endl;
  std::cout << "csiRsrp = " << info.csiRsrp << std::endl;
  std::cout << "csiRsrq = " << info.csiRsrq << std::endl;
  std::cout << "csiSinr = " << info.csiSinr << std::endl;
  std::cout << std::endl;
}

std::ostream& operator<<(std::ostream& os, const RIL_ActivityStatsInfo info) {
  os << "Modem Activity Info:" << std::endl;
  os << "sleep_mode_time_ms = " << info.sleep_mode_time_ms << std::endl;
  os << "idle_mode_time_ms = " << info.idle_mode_time_ms << std::endl;
  for(int i=0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
    os << "tx_mode_time_ms[" << i << "] = " << info.tx_mode_time_ms[i] << std::endl;
  }
  os << "rx_mode_time_ms = " << info.rx_mode_time_ms<< std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_CdmaSubscriptionSource source) {
    switch(source) {
        case CDMA_SUBSCRIPTION_SOURCE_INVALID:
            os << "INVALID";
        break;
        case CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM:
            os << "RUIM_SIM";
        break;
        case CDMA_SUBSCRIPTION_SOURCE_NV:
            os << "NV";
        break;
        default:
        break;
    }
    return os;
}

void PhoneMenu::requestSignalStrength(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getSignalStrength(
      [](RIL_Errno err, const RIL_SignalStrength_v10* signalInfo) -> void {
        std::cout << "Got response for Signal Strength request: " << err << std::endl;
        if (signalInfo) {
          printSignalInfo(signalInfo->GW_SignalStrength);
          printSignalInfo(signalInfo->WCDMA_SignalStrength);
          printSignalInfo(signalInfo->CDMA_SignalStrength);
          printSignalInfo(signalInfo->EVDO_SignalStrength);
          printSignalInfo(signalInfo->LTE_SignalStrength);
          printSignalInfo(signalInfo->TD_SCDMA_SignalStrength);
          printSignalInfo(signalInfo->NR_SignalStrength);
        } else {
          std::cout << "No Signal Strength recieved" << std::endl;
        }
      });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send requestSignalStrength" << std::endl;
  }
}

void PhoneMenu::getRadioState(std::vector<std::string> userInput) {
  //   if(phone_) {
  //      auto radioState = phone_->getRadioState();
  //      std::cout << "RadioState is " << getRadioStateAsString(radioState) << std::endl;
  //   } else {
  //      std::cout << "No default phone found" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

// std::string PhoneMenu::getRadioStateAsString(telux::tel::RadioState radioState) {
//   std::string radioStateString = "";
//   switch(radioState) {
//      case telux::tel::RadioState::RADIO_STATE_OFF:
//         radioStateString = "Off";
//         break;
//      case telux::tel::RadioState::RADIO_STATE_UNAVAILABLE:
//         radioStateString = "Unavailable";
//         break;
//      case telux::tel::RadioState::RADIO_STATE_ON:
//         radioStateString = "On";
//         break;
//      default:
//         break;
//   }
//   return radioStateString;
//}
//
// std::string PhoneMenu::getServiceStateAsString(telux::tel::ServiceState serviceState) {
//   std::string serviceStateString = "";
//   switch(serviceState) {
//      case telux::tel::ServiceState::EMERGENCY_ONLY:
//         serviceStateString = "Emergency Only";
//         break;
//      case telux::tel::ServiceState::IN_SERVICE:
//         serviceStateString = "In Service";
//         break;
//      case telux::tel::ServiceState::OUT_OF_SERVICE:
//         serviceStateString = "Out Of Service";
//         break;
//      case telux::tel::ServiceState::RADIO_OFF:
//         serviceStateString = "Radio Off";
//         break;
//      default:
//         break;
//   }
//   return serviceStateString;
//}

void PhoneMenu::requestRadioTechnology(std::vector<std::string> userInput) {
  //   if(phone_) {
  //      auto voiceTechResponseCb
  //         = std::bind(&MyVoiceRadioTechnologyCallback::voiceRadioTechnologyResponse,
  //                     myVoiceRadioTechCb_, std::placeholders::_1, std::placeholders::_2);
  //      auto ret = phone_->requestVoiceRadioTechnology(voiceTechResponseCb);
  //      std::cout << (ret == telux::common::Status::SUCCESS
  //                       ? "Request Voice Radio Technology is successful \n"
  //                       : "Request Voice Radio Technology failed")
  //                << '\n';
  //   } else {
  //      std::cout << "No default phone found" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::requestVoiceServiceState(std::vector<std::string> userInput) {
  //   if(phone_) {
  //      auto ret = phone_->requestVoiceServiceState(myVoiceSrvStateCb_);
  //      std::cout
  //         << (ret == telux::common::Status::SUCCESS ? "Request Voice Service state is successful \n"
  //                                                   : "Request Voice Service state failed")
  //         << '\n';
  //   } else {
  //      std::cout << "No default phone found" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::getSubscription(std::vector<std::string> userInput) {
  //   telux::common::Status status;
  //   auto subscription = subscriptionMgr_->getSubscription(DEFAULT_SLOT_ID, &status);
  //   if(subscription) {
  //      std::cout << "CarrierName : " << subscription->getCarrierName()
  //                << "\nPhoneNumber : " << subscription->getPhoneNumber()
  //                << "\nIccId : " << subscription->getIccId() << "\nMcc : " << subscription->getMcc()
  //                << "\nMnc : " << subscription->getMnc()
  //                << "\nSlotId : " << subscription->getSlotId()
  //                << "\nImsi : " << subscription->getImsi() << std::endl;
  //   } else {
  //      std::cout << "Subscription is empty" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

const std::string getRegistrationStateFromValue(RIL_RegState state) {
  std::string ret;
  switch (state) {
    case RIL_NOT_REG_AND_NOT_SEARCHING:
      ret = "RIL_NOT_REG_AND_NOT_SEARCHING";
      break;
    case RIL_REG_HOME:
      ret = "RIL_REG_HOME";
      break;
    case RIL_NOT_REG_AND_SEARCHING:
      ret = "RIL_NOT_REG_AND_SEARCHING";
      break;
    case RIL_REG_DENIED:
      ret = "RIL_REG_DENIED";
      break;
    case RIL_UNKNOWN:
      ret = "RIL_UNKNOWN";
      break;
    case RIL_REG_ROAMING:
      ret = "RIL_REG_ROAMING";
      break;
    case RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING:
      ret = "RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING";
      break;
    case RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING:
      ret = "RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING";
      break;
    case RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE:
      ret = "RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE";
      break;
    case RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE:
      ret = "RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE";
      break;
    default:
      ret = "NONE";
  }
  return ret;
}

const std::string getRatFromValue(RIL_RadioTechnology rat) {
  std::string ret;
  switch (rat) {
    case RADIO_TECH_UNKNOWN:
      ret = "RADIO_TECH_UNKNOWN";
      break;
    case RADIO_TECH_GPRS:
      ret = "RADIO_TECH_GPRS";
      break;
    case RADIO_TECH_EDGE:
      ret = "RADIO_TECH_EDGE";
      break;
    case RADIO_TECH_UMTS:
      ret = "RADIO_TECH_UMTS";
      break;
    case RADIO_TECH_IS95A:
      ret = "RADIO_TECH_IS95A";
      break;
    case RADIO_TECH_IS95B:
      ret = "RADIO_TECH_IS95B";
      break;
    case RADIO_TECH_1xRTT:
      ret = "RADIO_TECH_1xRTT";
      break;
    case RADIO_TECH_EVDO_0:
      ret = "RADIO_TECH_EVDO_0";
      break;
    case RADIO_TECH_EVDO_A:
      ret = "RADIO_TECH_EVDO_A";
      break;
    case RADIO_TECH_HSDPA:
      ret = "RADIO_TECH_HSDPA";
      break;
    case RADIO_TECH_HSUPA:
      ret = "RADIO_TECH_HSUPA";
      break;
    case RADIO_TECH_HSPA:
      ret = "RADIO_TECH_HSPA";
      break;
    case RADIO_TECH_EVDO_B:
      ret = "RADIO_TECH_EVDO_B";
      break;
    case RADIO_TECH_EHRPD:
      ret = "RADIO_TECH_EHRPD";
      break;
    case RADIO_TECH_LTE:
      ret = "RADIO_TECH_LTE";
      break;
    case RADIO_TECH_HSPAP:
      ret = "RADIO_TECH_HSPAP";
      break;
    case RADIO_TECH_GSM:
      ret = "RADIO_TECH_GSM";
      break;
    case RADIO_TECH_TD_SCDMA:
      ret = "RADIO_TECH_TD_SCDMA";
      break;
    case RADIO_TECH_IWLAN:
      ret = "RADIO_TECH_IWLAN";
      break;
    case RADIO_TECH_LTE_CA:
      ret = "RADIO_TECH_LTE_CA";
      break;
    case RADIO_TECH_5G:
      ret = "RADIO_TECH_5G";
      break;
    case RADIO_TECH_WIFI:
      ret = "RADIO_TECH_WIFI";
      break;
    case RADIO_TECH_ANY:
      ret = "RADIO_TECH_ANY";
      break;
    case RADIO_TECH_AUTO:
      ret = "RADIO_TECH_AUTO";
      break;
    default:
      ret = "NONE";
  }
  return ret;
}

const std::string getTypeStringFromVal(RIL_CellInfoType type) {
  std::string ret;
  switch (type) {
    case RIL_CELL_INFO_TYPE_NONE:
      ret = "NONE";
      break;
    case RIL_CELL_INFO_TYPE_GSM:
      ret = "GSM";
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      ret = "CDMA";
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      ret = "LTE";
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      ret = "WCDMA";
      break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
      ret = "TD_SCDMA";
      break;
    default:
      ret = "NONE";
  }
  return ret;
}

void printCellIdentity(RIL_CellIdentityGsm_v12 cell) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityCdma cell) {
  std::cout << "Network Id: " << cell.networkId << std::endl
            << "Operator Id: " << cell.basestationId << std::endl
            << "LAT: " << cell.latitude << std::endl
            << "LONG: " << cell.longitude << std::endl
            << "Base Station Id: " << cell.basestationId << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityLte_v12 cell) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.bandwidth << std::endl
            << "Bandwidth: " << cell.ci << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityWcdma_v12 cell) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellIdentity(RIL_CellIdentityTdscdma cell) {
  std::cout << "MCC: " << cell.mcc << std::endl
            << "MNC: " << cell.mnc << std::endl
            << "LAC: " << cell.lac << std::endl
            << "Operator Short: " << cell.operatorNames.alphaShort << std::endl
            << "Operator Long: " << cell.operatorNames.alphaLong << std::endl;
}

void printCellInfo(RIL_CellIdentity_v16 cell) {
  std::cout << "Cell Identity for " << getTypeStringFromVal(cell.cellInfoType) << std::endl;
  switch (cell.cellInfoType) {
    case RIL_CELL_INFO_TYPE_NONE:;
      break;
    case RIL_CELL_INFO_TYPE_GSM:
      printCellIdentity(cell.cellIdentityGsm);
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      printCellIdentity(cell.cellIdentityCdma);
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      printCellIdentity(cell.cellIdentityLte);
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      printCellIdentity(cell.cellIdentityWcdma);
      break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
      printCellIdentity(cell.cellIdentityTdscdma);
      break;
  }
}

void PhoneMenu::getVoiceRegistration(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getVoiceRegStatus([](RIL_Errno err,
                                             RIL_VoiceRegistrationStateResponse reg) -> void {
    std::cout << "Got response for Voice Reg request: " << err << std::endl;
    std::cout << "VOICE REGISTRATION:" << std::endl;
    std::cout << "Registration State: " << getRegistrationStateFromValue(reg.regState) << std::endl;
    std::cout << "Radio Access Technology: " << getRatFromValue(reg.rat) << std::endl;
    std::cout << "Roaming Indicator: " << reg.roamingIndicator << std::endl;
    printCellInfo(reg.cellIdentity);
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send getVoiceRegistration" << std::endl;
  }
}

void PhoneMenu::getDataRegistration(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getDataRegStatus([](RIL_Errno err,
                                            RIL_DataRegistrationStateResponse reg) -> void {
    std::cout << "Got response for Data Reg request: " << err << std::endl;
    std::cout << "DATA REGISTRATION:" << std::endl;
    std::cout << "Registration State: " << getRegistrationStateFromValue(reg.regState) << std::endl;
    std::cout << "Radio Access Technology: " << getRatFromValue(reg.rat) << std::endl;
    std::cout << "Max Data Calls: " << reg.maxDataCalls << std::endl;
    if (reg.lteVopsInfoValid) {
      std::cout << "LTE VoPS: Supported: " << static_cast<bool>(reg.lteVopsInfo.isVopsSupported)
                << " and EMC Bearer Supported " << static_cast<bool>(reg.lteVopsInfo.isEmcBearerSupported) << std::endl;
    }
    if (reg.nrIndicatorsValid) {
      std::cout << "NR Indicators: " << std::endl
                << "Endc Available: " << static_cast<bool>(reg.nrIndicators.isEndcAvailable) << std::endl
                << "PLMN Info List R15 Available: " << static_cast<bool>(reg.nrIndicators.plmnInfoListR15Available)
                << std::endl;
    }
    printCellInfo(reg.cellIdentity);
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send getDataRegistration" << std::endl;
  }
}

void PhoneMenu::setIndicationFilter(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t filter = 0;
  int flag = 0;
  std::cout << "RIL_UR_SIGNAL_STRENGTH (0 - disable, 1 - enable)" << std::endl;
  std::cin >> flag;
  if(flag) { filter |= RIL_UR_SIGNAL_STRENGTH; flag = 0; }
  std::cout << "RIL_UR_FULL_NETWORK_STATE (0 - disable, 1 - enable)" << std::endl;
  std::cin >> flag;
  if(flag) { filter |= RIL_UR_FULL_NETWORK_STATE; flag = 0; }
  std::cout << "RIL_UR_DATA_CALL_DORMANCY_CHANGED (0 - disable, 1 - enable)" << std::endl;
  std::cin >> flag;
  if(flag) { filter |= RIL_UR_DATA_CALL_DORMANCY_CHANGED; flag = 0; }
  std::cout << "RIL_UR_LINK_CAPACITY_ESTIMATE (0 - disable, 1 - enable)" << std::endl;
  std::cin >> flag;
  if(flag) { filter |= RIL_UR_LINK_CAPACITY_ESTIMATE; flag = 0; }
  std::cout << "RIL_UR_PHYSICAL_CHANNEL_CONFIG (0 - disable, 1 - enable)" << std::endl;
  std::cin >> flag;
  if(flag) { filter |= RIL_UR_PHYSICAL_CHANNEL_CONFIG; flag = 0; }

  Status s = rilSession.setIndicationFilter(filter, [](RIL_Errno err) -> void {
    std::cout << "Got response for setIndicationFilter request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send setIndicationFilter" << std::endl;
  }
}

void PhoneMenu::exitEmergencyCbMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.exitEmergencyCbMode([](RIL_Errno err) -> void {
    std::cout << "Got response for exitEmergencyCbMode request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send exitEmergencyCbMode" << std::endl;
  }
}

void PhoneMenu::setLocationUpdates(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  int on;
  std::cin >> on;
  Status s = rilSession.setLocationUpdates(on ? true : false, [](RIL_Errno err) -> void {
    std::cout << "Got response for setLocationUpdates request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send setLocationUpdates" << std::endl;
  }
}

void PhoneMenu::shutDown(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.shutDown([](RIL_Errno err) -> void {
    std::cout << "Got response for Shut Down request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send shutDown" << std::endl;
  }
}

void PhoneMenu::setRadioPower(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  int radioPower;
  std::cout << "Set Radio Power (1 - On, 0 - Off): ";
  std::cin >> radioPower;
  if (radioPower == 1 || radioPower == 0) {
    Status s = rilSession.radioPower(!!radioPower, [](RIL_Errno err) -> void {
      std::cout << "Got response for Radio Power request: " << err << std::endl;
    });
    if(s != Status::SUCCESS) {
      std::cout << "Failed to send setRadioPower" << std::endl;
    }
  } else {
    std::cout << " Invalid mode " << std::endl;
  }
}

void PhoneMenu::allowData(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  int on;
  std::cout << "Allow Data (1 - true, 0 - false): ";
  std::cin >> on;
  if (on == 1 || on == 0) {
    Status s = rilSession.allowData(!!on, [](RIL_Errno err) -> void {
      std::cout << "Got response for Allow Data request: " << err << std::endl;
    });
    if(s != Status::SUCCESS) {
      std::cout << "Failed to send setSystemSelectionChannels" << std::endl;
    }
  } else {
    std::cout << " Invalid mode " << std::endl;
  }
}

void PhoneMenu::enableSim(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  int state;
  std::cout << "(1 - enable, 0 - Disable): ";
  std::cin >> state;
  if (state == 1 || state == 0) {
    Status s = rilSession.enableSim(!!state, [](RIL_Errno err) -> void {
      std::cout << "Got response for EnableSim request: " << err << std::endl;
    });
    if(s != Status::SUCCESS) {
      std::cout << "Failed to send enableSim" << std::endl;
    }
  } else {
    std::cout << " Invalid mode " << std::endl;
  }
}

void PhoneMenu::getEnableSimStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getEnableSimStatus([](RIL_Errno err, bool state) -> void {
    std::cout << "Got response for getEnableSimStatus request: " << err << std::endl;
    std::cout << "Sim Enable Status : " << static_cast<int>(state) << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send getEnableSimStatus" << std::endl;
  }
}

void PhoneMenu::getActivityInfo(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getActivityInfo([](RIL_Errno err, RIL_ActivityStatsInfo info) -> void {
    std::cout << "Got response for getActivityInfo request: " << err << std::endl;
    std::cout << info << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send getActivityInfo" << std::endl;
  }
}

void PhoneMenu::getCdmaSubscription(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getCdmaSubscription(
        [](RIL_Errno err, const char* info[5]) -> void {
            std::cout << "Got response for getCdmaSubscription request: "
                      << err << std::endl
                      << "mdn: " << (info[0] ? info[0] : "null") << std::endl
                      << "hSid: " << (info[1] ? info[1] : "null") << std::endl
                      << "hNid: " << (info[2] ? info[2] : "null") << std::endl
                      << "min: " << (info[3] ? info[3] : "null") << std::endl
                      << "prl: " << (info[4] ? info[4] : "null") << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "getCdmaSubscription request sent successfully"
                                       : "failed to send getCdmaSubscription reqeust")
            << std::endl;
}

void PhoneMenu::requestCellularCapabilities(std::vector<std::string> userInput) {
  //   if(phoneManager_) {
  //      auto ret = phoneManager_->requestCellularCapabilityInfo(myCellularCapabilityCb_);
  //      std::cout << (ret == telux::common::Status::SUCCESS
  //                       ? "Cellular capabilities request is successful \n"
  //                       : "Cellular capabilities request failed")
  //                << '\n';
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::getOperatingMode(std::vector<std::string> userInput) {
  //   if(phoneManager_) {
  //      auto ret = phoneManager_->requestOperatingMode(myGetOperatingModeCb_);
  //      std::cout
  //         << (ret == telux::common::Status::SUCCESS ? "Get Operating mode request is successful \n"
  //                                                   : "Get Operating mode request failed")
  //         << '\n';
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::setOperatingMode(std::vector<std::string> userInput) {
  //   if(phoneManager_) {
  //      int operatingMode;
  //      std::cout << "Enter Operating Mode (0-Online, 1-Airplane, 2-Factory Test,\n"
  //                << "3-Offline, 4-Resetting, 5-Shutting Down, 6-Persistent Low "
  //                   "Power) : ";
  //      std::cin >> operatingMode;
  //      Utils::validateInput(operatingMode);
  //      if(operatingMode >= 0 && operatingMode <= 6) {
  //
  //         auto responseCb = std::bind(&MySetOperatingModeCallback::setOperatingModeResponse,
  //                                     mySetOperatingModeCb_, std::placeholders::_1);
  //         auto ret = phoneManager_->setOperatingMode(
  //            static_cast<telux::tel::OperatingMode>(operatingMode), responseCb);
  //         std::cout << (ret == telux::common::Status::SUCCESS
  //                          ? "Set Operating mode request is successful \n"
  //                          : "Set Operating mode request failed")
  //                   << '\n';
  //      } else {
  //         std::cout << " Invalid input " << std::endl;
  //      }
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::requestCellInfoList(std::vector<std::string> userInput) {
  //   if(phone_) {
  //      auto ret = phone_->requestCellInfo(MyCellInfoCallback::cellInfoListResponse);
  //      std::cout << (ret == telux::common::Status::SUCCESS ? "CellInfo list request is successful \n"
  //                                                          : "CellInfo list request failed")
  //                << '\n';
  //   } else {
  //      std::cout << "No default phone found" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::setCellInfoListRate(std::vector<std::string> userInput) {
  //   if(phone_) {
  //      char delimiter = '\n';
  //      std::string timeIntervalInput;
  //      std::cout
  //         << "Enter time interval in Milliseconds(0 for default or notify when any changes): ";
  //      std::getline(std::cin, timeIntervalInput, delimiter);
  //      uint32_t opt = -1;
  //      if(!timeIntervalInput.empty()) {
  //         try {
  //            opt = std::stoi(timeIntervalInput);
  //         } catch(const std::exception &e) {
  //            std::cout << "ERROR: Invalid input, Enter numerical value " << opt << std::endl;
  //         }
  //      } else {
  //         opt = 0;
  //      }
  //      auto ret = phone_->setCellInfoListRate(opt, MyCellInfoCallback::cellInfoListRateResponse);
  //      std::cout
  //         << (ret == telux::common::Status::SUCCESS ? "Set cell info rate request is successful \n"
  //                                                   : "Set cell info rate request failed")
  //         << '\n';
  //   } else {
  //      std::cout << "No default phone found" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::servingSystemMenu(std::vector<std::string> userInput) {
  ServingSystemMenu servingSystemMenu("Serving System Menu", "ServingSystem> ");
  servingSystemMenu.init();
  servingSystemMenu.mainLoop();
}

void PhoneMenu::networkMenu(std::vector<std::string> userInput) {
  NetworkMenu networkMenu("Network Menu", "Network> ", rilSession);
  networkMenu.init();
  networkMenu.mainLoop();
}

void PhoneMenu::setECallOperatingMode(std::vector<std::string> userInput) {
  //   if(phone_) {
  //      int eCallMode;
  //      std::cout << std::endl;
  //      std::cout << "Enter eCall Operating Mode(0-NORMAL, 1-ECALL_ONLY): ";
  //      std::cin >> eCallMode;
  //
  //      if(eCallMode == 0 || eCallMode == 1) {
  //         auto ret = phone_->setECallOperatingMode(
  //            static_cast<telux::tel::ECallMode>(eCallMode),
  //            MySetECallOperatingModeCallback::setECallOperatingModeResponse);
  //         if(ret == telux::common::Status::SUCCESS) {
  //            std::cout << "Set eCall operating mode request sent successfully \n";
  //         } else {
  //            std::cout << "Set eCall operating mode request failed \n";
  //         }
  //      } else {
  //         std::cout << "Invalid input \n";
  //      }
  //   } else {
  //      std::cout << "No phone found corresponding to default phoneId" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::requestECallOperatingMode(std::vector<std::string> userInput) {
  //   if(phone_) {
  //      auto ret = phone_->requestECallOperatingMode(
  //         MyGetECallOperatingModeCallback::getECallOperatingModeResponse);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "Get eCall Operating mode request sent successfully\n";
  //      } else {
  //         std::cout << "Get eCall Operating mode request failed \n";
  //      }
  //   } else {
  //      std::cout << "No phone found corresponding to default phoneId" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void PhoneMenu::registerForIndications(std::vector<std::string> userInput) {
    rilSession.registerSignalStrengthIndicationHandler([](const RIL_SignalStrength& signalStrength) {
        (void)signalStrength;
        std::cout << "Received signal strength indication" << std::endl;
    });

    if ( rilSession.registerPhysicalConfigStructHandler(
        [] (const Status status, const  std::vector<RIL_PhysicalChannelConfig> arg) {
        if (status == Status::FAILURE) {
            std::cout << "PhysicalConfigStructUpdate : Failure in socket data read. Exiting testcase" << std::endl;
            return;
        }
        for (int j = 0; j < arg.size(); j++) {
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: List content = "
            + std::to_string(j) << std::endl;;
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: status = "
            + std::to_string(static_cast<int>(arg[j].status)) << std::endl;
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: BandwidthDownlink = "
            + std::to_string(arg[j].cellBandwidthDownlink) << std::endl;
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Network RAT "
            + std::to_string(static_cast<int>(arg[j].rat)) << std::endl;
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Range = "
            + std::to_string(static_cast<int>(arg[j].rfInfo.range)) << std::endl;
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: ChannelNumber = "
            + std::to_string(arg[j].rfInfo.channelNumber) << std::endl;
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Number of Context Ids = "
            + std::to_string(arg[j].num_context_ids) << std::endl;
            for (int i = 0; i < arg[j].num_context_ids; i++) {
                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Cid " + std::to_string(i) + " = "
                + std::to_string(arg[j].contextIds[i]) << std::endl;
            }
            std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: physicalCellId = "
            + std::to_string(arg[j].physicalCellId) << std::endl;
        }
        std::cout << "Received PhysicalConfigStructUpdate" << std::endl;
        }
    ) == Status::FAILURE) {
        std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Failed Due to Marshalling Failure." << std::endl;
    }

    rilSession.registerCarrierInfoForImsiHandler(
        [] () {
            std::cout << "Received UnsolCarrierInfoForImsiEncryptionRefreshMessage" << std::endl;
        }
    );

    rilSession.registerEnableSimStatusIndicationHandler(
        [] (bool state) {
            std::cout << "Received eSimStatusIndicationHandler" << std::endl;
            if(state) {
                std::cout << "SIM Enabled" << std::endl;
            } else {
                std::cout << "SIM Disabled" << std::endl;
            }
        }
    );

    rilSession.registerUiccSubStatusIndHandler(
        [] (int status) {
            std::cout << "Received UiccSubStatusInd, status: ";
            if(status) {
                std::cout << "ENABLED" << std::endl;
            } else {
                std::cout << "DISABLED" << std::endl;
            }
        }
    );

    rilSession.registerCdmaSubscriptionSourceChangedIndicationHandler(
        [] (RIL_CdmaSubscriptionSource source) {
            std::cout << "Received eSimStatusIndicationHandler" << std::endl;
            std::cout << "CDMA Subscription Source Changed to " << source << std::endl;
        }
    );

    rilSession.registerEmergencyCbModeIndHandler(
        [] (RIL_EmergencyCbMode mode) {
            std::cout << "Emergency Callback Mode: " << std::endl;
            if(mode == RIL_EMERGENCY_CALLBACK_MODE_EXIT) {
                std::cout << "EXIT" << std::endl;
            } else if(mode == RIL_EMERGENCY_CALLBACK_MODE_ENTER) {
                std::cout << "ENTER" << std::endl;
            }
        }
    );

    rilSession.registerCdmaPrlChangedIndHandler(
        [] (int version) {
            std::cout << "Received CdmaPrlChangedInd, version: " << version << std::endl;
        }
    );

    rilSession.registerCellInfoIndicationHandler(
      [](const RIL_CellInfo_v12 cellInfoList[], size_t cellInfoListLen) {
        std::cout << "Received cell info indication" << std::endl;

        if (cellInfoList == nullptr || cellInfoListLen == 0) {
          std::cout << "No cell info received." << std::endl;
          return;
        } else {
          std::cout << cellInfoListLen << " elements in the cell info array." << std::endl;
        }

        std::for_each(
            cellInfoList, cellInfoList + cellInfoListLen, [](const RIL_CellInfo_v12& cellInfo) {
              const char* mcc = nullptr;
              const char* mnc = nullptr;

              switch (cellInfo.cellInfoType) {
                case RIL_CELL_INFO_TYPE_GSM:
                  mcc = cellInfo.CellInfo.gsm.cellIdentityGsm.mcc;
                  mnc = cellInfo.CellInfo.gsm.cellIdentityGsm.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_LTE:
                  mcc = cellInfo.CellInfo.lte.cellIdentityLte.mcc;
                  mnc = cellInfo.CellInfo.lte.cellIdentityLte.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_WCDMA:
                  mcc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc;
                  mnc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_TD_SCDMA:
                  mcc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc;
                  mnc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc;
                  break;
                case RIL_CELL_INFO_TYPE_NR:
                  mcc = cellInfo.CellInfo.nr.cellIdentityNr.mcc;
                  mnc = cellInfo.CellInfo.nr.cellIdentityNr.mnc;
                  break;
                default:
                  break;
              }

              std::cout << "cellInfoType: " << cellInfo.cellInfoType
                        << " registered: " << cellInfo.registered << " mcc: " << (mcc ? mcc : "")
                        << " mnc: " << (mnc ? mnc : "") << std::endl;
            });
      });

    rilSession.registerRestrictedStateChangedIndicationHandler(
        [] (const int state) {
            std::cout << "Received RestrictedStateChangedIndication, state: " << state << std::endl;
        }
    );
    rilSession.registerVoiceNetworkStateIndicationHandler(
        [] () {
            std::cout << "Received VoiceNetworkStateIndication" << std::endl;
        }
    );
    rilSession.registerVoiceRadioTechIndicationHandler(
        [] (int rat) {
            std::cout << "Received VoiceRadioTechIndication, rat:" << rat << std::endl;
        }
    );
}

void PhoneMenu::requestVoiceRadioTech(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.VoiceRadioTech(
      [](RIL_Errno err, int tech) -> void {
        std::cout << "Got response for voice radio tech request: " << err << std::endl;
        std::cout << "Voice Radio Tech : " << tech << std::endl;
        }
      );
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send requestVoiceRadioTech" << std::endl;
  }
}

void PhoneMenu::requestDeviceIdentity(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.deviceIdentity(
      [](RIL_Errno err, const char ** deviceInfo) -> void {
        std::cout << "Got response for Device Identity request: " << err << std::endl;
        if (deviceInfo) {
          if (deviceInfo[0]) {
            std::cout << "imei : " << deviceInfo[0] << std::endl;
          }
          if (deviceInfo[1]) {
            std::cout << "imeisv : " << deviceInfo[1] << std::endl;
          }
          if (deviceInfo[2]) {
            std::cout << "esn : " << deviceInfo[2] << std::endl;
          }
          if (deviceInfo[3]) {
            std::cout << "meid : " << deviceInfo[3] << std::endl;
          }
        } else {
          std::cout << "No Device Info recieved" << std::endl;
        }
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send requestDeviceIdentity" << std::endl;
  }
}

void PhoneMenu::requestOperatorName(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.operatorName(
      [](RIL_Errno err, const char ** operName) -> void {
        std::cout << "Got response for Operator Name request: " << err << std::endl;
        if (operName) {
          if (operName[0]) {
            std::cout << "longName : " << operName[0] << std::endl;
          }
          if (operName[1]) {
            std::cout << "shortName : " << operName[1] << std::endl;
          }
          if (operName[2]) {
            std::cout << "numeric : " << operName[2] << std::endl;
          }
        } else {
          std::cout << "No Operator Name recieved" << std::endl;
        }
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send requestOperatorName" << std::endl;
  }
}

void PhoneMenu::requestBasebandVersion(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.basebandVersion(
      [](RIL_Errno err, const char * version) -> void {
        std::cout << "Got response for Baseband Version request: " << err << std::endl;
        if (version) {
          std::cout << "Baseband Version : " << version << std::endl;
        } else {
          std::cout << "No Baseband Version recieved" << std::endl;
        }
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send requestBasebandVersion" << std::endl;
  }
}

void PhoneMenu::requestSetUnsolCellInfoListRate(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int32_t mRate;
  std::cout << "Enter mRate:" <<std::endl;
  std::cin >> mRate;
  Status s = rilSession.setUnsolCellInfoListRate(mRate, [](RIL_Errno err) -> void {
    std::cout << "Got response for setUnsolCellInfoListRate request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send requestSetUnsolCellInfoListRate" << std::endl;
  }
}

void PhoneMenu::registerForUnsols(std::vector<std::string> userInput) {
    rilSession.registerNitzTimeIndicationHandler(
        [] (const char *nitzTime) {
            std::cout<<"[PhoneMenu]: Received Nitz Time";
            std::cout<<nitzTime<<std::endl;
        }
    );

    rilSession.registerRadioStateIndicationHandler(
        [] (RIL_RadioState state) {
            std::cout<<"[PhoneMenu]: Received Radio State Change";
            std::cout<<state<<std::endl;
        }
    );

    rilSession.registerSignalStrengthIndicationHandler(
        [] (const RIL_SignalStrength& signalStrength) {
            (void)signalStrength;
            std::cout << "Received signal strength indication" << std::endl;
        }
    );

    rilSession.registerCarrierInfoForImsiHandler(
        [] () {
            std::cout << "Received UnsolCarrierInfoForImsiEncryptionRefreshMessage" << std::endl;
        }
    );

    rilSession.registerCellInfoIndicationHandler(
        [] (const RIL_CellInfo_v12 cellInfoList[], size_t cellInfoListLen) {
            std::cout << "Received cell info indication" << std::endl;

            if (cellInfoList == nullptr || cellInfoListLen == 0) {
                std::cout << "No cell info received." << std::endl;
            } else {
                std::cout << cellInfoListLen << " elements in the cell info array." << std::endl;
            }

            std::for_each(
                cellInfoList,
                cellInfoList + cellInfoListLen,
                [] (const RIL_CellInfo_v12& cellInfo) {
                    const char* mcc = nullptr;
                    const char* mnc = nullptr;

                    switch (cellInfo.cellInfoType) {
                        case RIL_CELL_INFO_TYPE_GSM:
                            mcc = cellInfo.CellInfo.gsm.cellIdentityGsm.mcc;
                            mnc = cellInfo.CellInfo.gsm.cellIdentityGsm.mnc;
                            break;
                        case RIL_CELL_INFO_TYPE_LTE:
                            mcc = cellInfo.CellInfo.lte.cellIdentityLte.mcc;
                            mnc = cellInfo.CellInfo.lte.cellIdentityLte.mnc;
                            break;
                        case RIL_CELL_INFO_TYPE_WCDMA:
                            mcc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc;
                            mnc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc;
                            break;
                        case RIL_CELL_INFO_TYPE_TD_SCDMA:
                            mcc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc;
                            mnc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc;
                            break;
                        case RIL_CELL_INFO_TYPE_NR:
                            mcc = cellInfo.CellInfo.nr.cellIdentityNr.mcc;
                            mnc = cellInfo.CellInfo.nr.cellIdentityNr.mnc;
                            break;
                        default:
                            break;
                    }

                    std::cout << "cellInfoType: " << cellInfo.cellInfoType
                        << " registered: " << cellInfo.registered << " mcc: " << (mcc ? mcc : "")
                        << " mnc: " << (mnc ? mnc : "") << std::endl;
                }
            );
        }
    );

    rilSession.registerModemRestartIndicationHandler(
      [] (const std::string& reason) {
        std::cout << std::endl << "Modem restarted. Reason: " << reason
          << "." << std::endl;
      }
    );
}

std::ostream& operator <<(std::ostream& out, RIL_CdmaRoamingPreference roamingPref) {
  switch (roamingPref) {
    case CDMA_ROAMING_PREFERENCE_HOME_NETWORK:
      out << "Home Network";
      break;
    case CDMA_ROAMING_PREFERENCE_AFFILIATED_ROAM:
      out << "Affiliated Roam";
      break;
    case CDMA_ROAMING_PREFERENCE_ANY_ROAM:
      out << "Any Roam";
      break;
    default:
      out << "Invalid Roaming Preference";
      break;
  }

  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_TimeStampType timeStampType) {
  out << "Time Stamp Type: ";
  switch (timeStampType) {
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_ANTENNA:
      out << "Antenna";
      break;
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_MODEM:
      out << "Modem";
      break;
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL:
      out << "OEM RIL";
      break;
    case RIL_TimeStampType::RIL_TIMESTAMP_TYPE_JAVA_RIL:
      out << "Java RIL";
      break;
    default:
      out << "Unknown";
      break;
  }
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellConnectionStatus connStatus) {
  out << "Cell Connection Status: ";
  switch (connStatus) {
    case RIL_CellConnectionStatus::RIL_CELL_CONNECTION_STATUS_PRIMARY:
      out << "Primary";
      break;
    case RIL_CellConnectionStatus::RIL_CELL_CONNECTION_STATUS_SECONDARY:
      out << "Secondary";
      break;
    default:
      out << "Unknown";
      break;
  }
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityOperatorNames& operatorNames) {
  out << "Operator Short Name: " << std::string(operatorNames.alphaShort,
    strnlen(operatorNames.alphaShort, sizeof(operatorNames.alphaShort))) << std::endl;
  out << "Operator Long Name: " << std::string(operatorNames.alphaLong,
    strnlen(operatorNames.alphaLong, sizeof(operatorNames.alphaLong)));
  return out;
}

template <typename T>
std::ostream& outputMccMnc(std::ostream& out, const T& cellIdentity) {
  out << "MCC: " << std::string(cellIdentity.mcc,
    strnlen(cellIdentity.mcc, sizeof(cellIdentity.mcc))) << std::endl;

  out << "MNC: " << std::string(cellIdentity.mnc,
    strnlen(cellIdentity.mnc, sizeof(cellIdentity.mnc))) << std::endl;

  out << "Registered PLMN MCC: " << std::string(cellIdentity.reg_plmn_mcc,
    strnlen(cellIdentity.reg_plmn_mcc, sizeof(cellIdentity.reg_plmn_mcc))) << std::endl;

  out << "Registered PLMN MNC: " << std::string(cellIdentity.reg_plmn_mnc,
    strnlen(cellIdentity.reg_plmn_mnc, sizeof(cellIdentity.reg_plmn_mnc))) << std::endl;

  out << "Primary PLMN MCC: " << std::string(cellIdentity.primary_plmn_mcc,
    strnlen(cellIdentity.primary_plmn_mcc, sizeof(cellIdentity.primary_plmn_mcc))) << std::endl;

  out << "Primary PLMN MNC: " << std::string(cellIdentity.primary_plmn_mnc,
    strnlen(cellIdentity.primary_plmn_mnc, sizeof(cellIdentity.primary_plmn_mnc)));

  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityGsm_v12& cellIdentityGsm) {
  out << "== GSM Cell Identity ==" << std::endl;
  outputMccMnc(out, cellIdentityGsm) << std::endl;
  out << "LAC: " << cellIdentityGsm.lac << std::endl;
  out << "CID: " << cellIdentityGsm.cid << std::endl;
  out << "ARFCN: " << cellIdentityGsm.arfcn << std::endl;
  out << "BSIC: " << cellIdentityGsm.bsic << std::endl;
  out << cellIdentityGsm.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_GSM_SignalStrength_v12& signalStrengthGsm) {
  out << "== GSM Signal Strength ==" << std::endl;
  out << "Signal Strength: " << signalStrengthGsm.signalStrength << std::endl;
  out << "Bit Error Rate: " << signalStrengthGsm.bitErrorRate << std::endl;
  out << "Timing Advance: " << signalStrengthGsm.timingAdvance;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoGsm_v12& cellInfoGsm) {
  out << "=== GSM Cell Info ===" << std::endl;
  out << cellInfoGsm.cellIdentityGsm << std::endl;
  out << cellInfoGsm.signalStrengthGsm;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityCdma& cellIdentityCdma) {
  out << "== CDMA Cell Identity ==" << std::endl;
  out << "Network ID: " << cellIdentityCdma.networkId << std::endl;
  out << "System ID: " << cellIdentityCdma.systemId << std::endl;
  out << "Base Station ID: " << cellIdentityCdma.basestationId << std::endl;
  out << "Longitude: " << cellIdentityCdma.longitude << std::endl;
  out << "Latitude: " << cellIdentityCdma.latitude << std::endl;
  out << cellIdentityCdma.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CDMA_SignalStrength& signalStrengthCdma) {
  out << "== CDMA Signal Strength ==" << std::endl;
  out << "RSSI (DBM): " << signalStrengthCdma.dbm << std::endl;
  out << "Ec/Io: " << signalStrengthCdma.ecio;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_EVDO_SignalStrength& signalStrengthEvdo) {
  out << "== EVDO Signal Strength ==" << std::endl;
  out << "RSSI (DBM): " << signalStrengthEvdo.dbm << std::endl;
  out << "Ec/Io: " << signalStrengthEvdo.ecio << std::endl;
  out << "SNR: " << signalStrengthEvdo.signalNoiseRatio;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoCdma& cellInfoCdma) {
  out << "=== CDMA Cell Info ===" << std::endl;
  out << cellInfoCdma.cellIdentityCdma << std::endl;
  out << cellInfoCdma.signalStrengthCdma << std::endl;
  out << cellInfoCdma.signalStrengthEvdo;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityLte_v12& cellIdentityLte) {
  out << "== LTE Cell Identity ==" << std::endl;
  outputMccMnc(out, cellIdentityLte) << std::endl;
  out << "Cell ID: " << cellIdentityLte.ci << std::endl;
  out << "Physical Cell ID: " << cellIdentityLte.pci << std::endl;
  out << "TAC: " << cellIdentityLte.tac << std::endl;
  out << "E-ARFCN: " << cellIdentityLte.earfcn << std::endl;
  out << "Bandwidth: " << cellIdentityLte.bandwidth << std::endl;
  out << "Band: " << cellIdentityLte.band << std::endl;
  out << cellIdentityLte.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_LTE_SignalStrength_v8& signalStrengthLte) {
  out << "== LTE Signal Strength ==" << std::endl;
  out << "Signal Strength: " << signalStrengthLte.signalStrength << std::endl;
  out << "RSRP: " << signalStrengthLte.rsrp << std::endl;
  out << "RSRQ: " << signalStrengthLte.rsrq << std::endl;
  out << "RSSNR: " << signalStrengthLte.rssnr << std::endl;
  out << "CQI: " << signalStrengthLte.cqi << std::endl;
  out << "Timing Advance: " << signalStrengthLte.timingAdvance;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellConfigLte& cellConfigLte) {
  out << "== LTE Cell Config ==" << std::endl;
  out << "ENDC Available: " << static_cast<bool>(cellConfigLte.isEndcAvailable);
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoLte_v12& cellInfoLte) {
  out << "=== LTE Cell Info ===" << std::endl;
  out << cellInfoLte.cellIdentityLte << std::endl;
  out << cellInfoLte.signalStrengthLte << std::endl;
  out << cellInfoLte.cellConfig;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityWcdma_v12& cellIdentityWcdma) {
  out << "== WCDMA Cell Identity ==" << std::endl;
  outputMccMnc(out, cellIdentityWcdma) << std::endl;
  out << "LAC: " << cellIdentityWcdma.lac << std::endl;
  out << "Cell ID: " << cellIdentityWcdma.cid << std::endl;
  out << "PSC: " << cellIdentityWcdma.psc << std::endl;
  out << "U-ARFCN: " << cellIdentityWcdma.uarfcn << std::endl;
  out << cellIdentityWcdma.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_SignalStrengthWcdma& signalStrengthWcdma) {
  out << "== WCDMA Signal Strength ==" << std::endl;
  out << "Signal Strength: " << signalStrengthWcdma.signalStrength << std::endl;
  out << "Bit Error Rate: " << signalStrengthWcdma.bitErrorRate;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoWcdma_v12& cellInfoWcdma) {
  out << "=== WCDMA Cell Info ===" << std::endl;
  out << cellInfoWcdma.cellIdentityWcdma << std::endl;
  out << cellInfoWcdma.signalStrengthWcdma;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityTdscdma& cellIdentityTdscdma) {
  out << "== TDSCDMA Cell Identity ==" << std::endl;
  outputMccMnc(out, cellIdentityTdscdma) << std::endl;
  out << "LAC: " << cellIdentityTdscdma.lac << std::endl;
  out << "Cell ID: " << cellIdentityTdscdma.cid << std::endl;
  out << "CPID: " << cellIdentityTdscdma.cpid << std::endl;
  out << "U-ARFCN: " << cellIdentityTdscdma.uarfcn << std::endl;
  out << cellIdentityTdscdma.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_TD_SCDMA_SignalStrength& signalStrengthTdscdma) {
  out << "== TDSCDMA Signal Strength ==" << std::endl;
  out << "RSSI: " << signalStrengthTdscdma.signalStrength << std::endl;
  out << "Bit Error Rate: " << signalStrengthTdscdma.bitErrorRate << std::endl;
  out << "RSCP: " << signalStrengthTdscdma.rscp;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoTdscdma& cellInfoTdscdma) {
  out << "=== TDSCDMA Cell Info ===" << std::endl;
  out << cellInfoTdscdma.cellIdentityTdscdma << std::endl;
  out << cellInfoTdscdma.signalStrengthTdscdma;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellIdentityNr& cellIdentityNr) {
  out << "== 5G NR Cell Identity ==" << std::endl;
  outputMccMnc(out, cellIdentityNr) << std::endl;
  out << "NR Cell ID: " << cellIdentityNr.nci << std::endl;
  out << "Physical Cell ID: " << cellIdentityNr.pci << std::endl;
  out << "TAC: " << cellIdentityNr.tac << std::endl;
  out << "NR-ARFCN: " << cellIdentityNr.nrarfcn << std::endl;
  out << "Bandwidth: " << cellIdentityNr.bandwidth << std::endl;
  out << "Band: " << cellIdentityNr.band << std::endl;
  out << cellIdentityNr.operatorNames;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_NR_SignalStrength& signalStrengthNr) {
  out << "== 5G NR Signal Strength ==" << std::endl;
  out << "SS-RSRP: " << signalStrengthNr.ssRsrp << std::endl;
  out << "SS-RSRQ: " << signalStrengthNr.ssRsrq << std::endl;
  out << "SS-SINR: " << signalStrengthNr.ssSinr << std::endl;
  out << "CSI-RSRP: " << signalStrengthNr.csiRsrp << std::endl;
  out << "CSI-RSRQ: " << signalStrengthNr.csiRsrq << std::endl;
  out << "CSI-SINR: " << signalStrengthNr.csiSinr;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfoNr& cellInfoNr) {
  out << "=== 5G NR Cell Info ===" << std::endl;
  out << cellInfoNr.cellIdentityNr << std::endl;
  out << cellInfoNr.signalStrengthNr;
  return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_CellInfo_v12& cellInfo) {
  out << "==== Cell Info ====" << std::endl;
  out << "Registered: " << cellInfo.registered << std::endl;
  out << cellInfo.timeStampType << std::endl;
  out << "Time Stamp: " << cellInfo.timeStamp << std::endl;
  out << cellInfo.connStatus << std::endl;

  switch (cellInfo.cellInfoType) {
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM:
      out << cellInfo.CellInfo.gsm << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA:
      out << cellInfo.CellInfo.cdma << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE:
      out << cellInfo.CellInfo.lte << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA:
      out << cellInfo.CellInfo.wcdma << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA:
      out << cellInfo.CellInfo.tdscdma << std::endl;
      break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_NR:
      out << cellInfo.CellInfo.nr << std::endl;
      break;
    default:
      break;
  }

  return out;
}

void PhoneMenu::setCdmaSubscriptionSource(std::vector<std::string> userInput) {
  int subSource = -1;
  do {
    std::cin.clear();
    std::cout << "Enter the CDMA subscription source (0 - RUIM/SIM, 1 - NV, -1 - Exit): ";
    std::cin >> subSource;
  } while (std::cin.fail() || subSource < -1 || subSource > 1);

  if (subSource == -1) return;

  Status s = rilSession.setCdmaSubscriptionSource(
    static_cast<RIL_CdmaSubscriptionSource>(subSource),
    [subSource] (RIL_Errno e) {
      std::cout << std::endl
        << (e == RIL_Errno::RIL_E_SUCCESS ? "Successfully " : "Failed to ")
        << "set the CDMA subscription source to " << subSource << "." << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << std::endl
      << "Failed to issue request to set the CDMA subscription source." << std::endl;
  }
}

void PhoneMenu::getCdmaSubscriptionSource(std::vector<std::string> userInput) {
  Status s = rilSession.getCdmaSubscriptionSource(
    [] (RIL_Errno e, RIL_CdmaSubscriptionSource subSource) {
      if (e == RIL_Errno::RIL_E_SUCCESS) {
        std::cout << std::endl << "Successfully got the CDMA subscription source. "
          << "CDMA Subscription Source: " << subSource << "." << std::endl;
      } else {
        std::cout << std::endl << "Failed to get the CDMA subscription source. Error: "
          << e << "." << std::endl;
      }
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << std::endl
      << "Failed to issue request to get the CDMA subscription source." << std::endl;
  }
}

void PhoneMenu::setCdmaRoamingPreference(std::vector<std::string> userInput) {
  int roamingPref = -1;
  do {
    std::cin.clear();
    std::cout << "Enter the CDMA roaming preference "
      << "(0 - Home Network, 1 - Affiliated Roam, 2 - Any Roam, -1 - Exit): ";
    std::cin >> roamingPref;
  } while (std::cin.fail() || roamingPref < -1 || roamingPref > 2);

  if (roamingPref == -1) return;

  Status s = rilSession.setCdmaRoamingPreference(
    static_cast<RIL_CdmaRoamingPreference>(roamingPref),
    [roamingPref] (RIL_Errno e) {
      std::cout << std::endl
        << (e == RIL_Errno::RIL_E_SUCCESS ? "Successfully " : "Failed to ")
        << "set the CDMA roaming preference to " << roamingPref << "." << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << std::endl
      << "Failed to issue request to set the CDMA roaming preference." << std::endl;
  }
}

void PhoneMenu::getCdmaRoamingPreference(std::vector<std::string> userInput) {
  Status s = rilSession.getCdmaRoamingPreference(
    [] (RIL_Errno e, RIL_CdmaRoamingPreference roamingPref) {
      if (e == RIL_Errno::RIL_E_SUCCESS) {
        std::cout << std::endl << "Successfully got the CDMA roaming preference. "
          << "CDMA Roaming Preference: " << roamingPref << "." << std::endl;
      } else {
        std::cout << std::endl << "Failed to get the CDMA roaming preference. Error: "
          << e << "." << std::endl;
      }
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << std::endl << "Failed to issue request to get the CDMA roaming preference."
      << std::endl;
  }
}

void PhoneMenu::setNetworkSelectionAutomatic(std::vector<std::string> userInput) {
  Status s = rilSession.setNetworkSelectionAutomatic(
    [] (RIL_Errno e) {
      std::cout << std::endl
        << (e == RIL_Errno::RIL_E_SUCCESS ? "Successfully requested" : "Failed to request")
        << " to select network automatically." << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << std::endl
      << "Failed to issue request to select network automatically." << std::endl;
  }
}

void PhoneMenu::setNetworkSelectionManual(std::vector<std::string> userInput) {
  int mcc;
  do {
    std::cin.clear();
    std::cout << "Enter the MCC of the network to select (-1 - Exit): ";
    std::cin >> mcc;
  } while (std::cin.fail() || mcc < -1 || mcc > 999);

  if (mcc == -1) {
    return;
  }

  int mnc;
  do {
    std::cin.clear();
    std::cout << "Enter the MNC of the network to select (-1 - Exit): ";
    std::cin >> mnc;
  } while (std::cin.fail() || mcc < -1 || mnc > 999);

  if (mnc == -1) {
    return;
  }

  Status s = rilSession.setNetworkSelectionManual(
    std::to_string(mcc),
    std::to_string(mnc),
    [mcc, mnc] (RIL_Errno e) {
      std::cout << std::endl
        << (e == RIL_Errno::RIL_E_SUCCESS ? "Successfully requested" : "Failed to request")
        << " to select network with MCC: " << mcc << " and MNC: " << mnc << "." << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << std::endl
      << "Failed to issue request to select network." << std::endl;
  }
}

void PhoneMenu::getCellInfo(std::vector<std::string> userInput) {
  Status s = rilSession.getCellInfo(
    [] (RIL_Errno e, const RIL_CellInfo_v12 cellInfoList[], const size_t cellInfoListLen) {
      std::cout << std::endl
        << (e == RIL_Errno::RIL_E_SUCCESS ? "Successfully received" : "Failed to receive")
        << " information about serving/neighboring cells." << std::endl;

      std::for_each(
        cellInfoList,
        cellInfoList + cellInfoListLen,
        [] (const RIL_CellInfo_v12& cellInfo) {
          std::cout << cellInfo << std::endl;
        }
      );
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << std::endl
      << "Failed to issue request to fetch information about serving/neighboring cells."
      << std::endl;
  }
}
