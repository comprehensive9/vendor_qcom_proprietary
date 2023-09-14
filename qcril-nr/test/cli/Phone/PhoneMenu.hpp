/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

#ifndef PHONEMENU_HPP
#define PHONEMENU_HPP

//#include "telux/tel/PhoneListener.hpp"
//#include "telux/tel/PhoneManager.hpp"
//#include "telux/tel/SubscriptionManager.hpp"
//#include <telux/tel/Phone.hpp>

//#include "MyPhoneListener.hpp"
//#include "MySignalStrengthHandler.hpp"
//#include "MySubscriptionListener.hpp"

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class PhoneMenu : public ConsoleApp {
 public:
  PhoneMenu(std::string appName, std::string cursor, int phoneId, RilApiSession& rilSession);
  ~PhoneMenu();
  void init();

 private:
  RilApiSession& rilSession;

 private:
  void requestSignalStrength(std::vector<std::string> userInput);
  void getRadioState(std::vector<std::string> userInput);
  void requestRadioTechnology(std::vector<std::string> userInput);
  void setRadioPower(std::vector<std::string> userInput);
  void allowData(std::vector<std::string> userInput);
  void enableSim(std::vector<std::string> userInput);
  void getEnableSimStatus(std::vector<std::string> userInput);
  void getActivityInfo(std::vector<std::string> userInput);
  void setIndicationFilter(std::vector<std::string> userInput);
  void shutDown(std::vector<std::string> userInput);
  void exitEmergencyCbMode(std::vector<std::string> userInput);
  void setLocationUpdates(std::vector<std::string> userInput);
  void getSubscription(std::vector<std::string> userInput);
  void getVoiceRegistration(std::vector<std::string> userInput);
  void getDataRegistration(std::vector<std::string> userInput);
  void requestVoiceServiceState(std::vector<std::string> userInput);
  void requestCellularCapabilities(std::vector<std::string> userInput);
  void getOperatingMode(std::vector<std::string> userInput);
  void setOperatingMode(std::vector<std::string> userInput);
  void requestCellInfoList(std::vector<std::string> userInput);
  void setCellInfoListRate(std::vector<std::string> userInput);
  void getCdmaSubscription(std::vector<std::string> userInput);
  void servingSystemMenu(std::vector<std::string> userInput);
  void networkMenu(std::vector<std::string> userInput);
  void setECallOperatingMode(std::vector<std::string> userInput);
  void requestECallOperatingMode(std::vector<std::string> userInput);
  void registerForIndications(std::vector<std::string> userInput);
  void requestVoiceRadioTech(std::vector<std::string> userInput);
  void requestDeviceIdentity(std::vector<std::string> userInput);
  void requestOperatorName(std::vector<std::string> userInput);
  void requestBasebandVersion(std::vector<std::string> userInput);
  void requestSetUnsolCellInfoListRate(std::vector<std::string> userInput);
  void registerForUnsols(std::vector<std::string> userInput);

  void setCdmaSubscriptionSource(std::vector<std::string> userInput);
  void getCdmaSubscriptionSource(std::vector<std::string> userInput);
  void setCdmaRoamingPreference(std::vector<std::string> userInput);
  void getCdmaRoamingPreference(std::vector<std::string> userInput);
  void setNetworkSelectionAutomatic(std::vector<std::string> userInput);
  void setNetworkSelectionManual(std::vector<std::string> userInput);
  void getCellInfo(std::vector<std::string> userInput);
  //   std::string getRadioStateAsString(telux::tel::RadioState radioState);
  //   std::string getServiceStateAsString(telux::tel::ServiceState serviceState);
  // Member variable to keep the Listener object alive till application ends.
  //   std::shared_ptr<telux::tel::IPhoneListener> phoneListener_;
  //   std::shared_ptr<telux::tel::IPhoneManager> phoneManager_;
  //   std::shared_ptr<telux::tel::ISubscriptionManager> subscriptionMgr_;
  //   std::shared_ptr<MySubscriptionListener> subscriptionListener_;
  //   std::shared_ptr<MySignalStrengthCallback> mySignalStrengthCb_;
  //   std::shared_ptr<MyVoiceRadioTechnologyCallback> myVoiceRadioTechCb_;
  //   std::shared_ptr<MyRadioPowerCallback> myRadioPowerCb_;
  //   std::shared_ptr<MyVoiceServiceStateCallback> myVoiceSrvStateCb_;
  //   std::shared_ptr<MyCellularCapabilityCallback> myCellularCapabilityCb_;
  //   std::shared_ptr<MyGetOperatingModeCallback> myGetOperatingModeCb_;
  //   std::shared_ptr<MySetOperatingModeCallback> mySetOperatingModeCb_;
  //   std::shared_ptr<telux::tel::IPhone> phone_;
};

const std::string getRatFromValue(RIL_RadioTechnology rat);

#endif  // PHONEMENU_HPP
