/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

#ifndef SAPCARDSERVICESMENU_HPP
#define SAPCARDSERVICESMENU_HPP

#include <iostream>
#include <string>
#include <memory>

//#include <telux/tel/SapCardManager.hpp>

#include "console_app_framework/ConsoleApp.hpp"
//#include "MySapCardListener.hpp"

class SapCardServicesMenu : public ConsoleApp {
 public:
  SapCardServicesMenu(std::string appName, std::string cursor);
  ~SapCardServicesMenu();
  void init();

 private:
  void openSapConnection(std::vector<std::string> userInput);
  void getSapAtr(std::vector<std::string> userInput);
  void requestSapState(std::vector<std::string> userInput);
  void getState(std::vector<std::string> userInput);
  void transmitSapApdu(std::vector<std::string> userInput);
  void sapSimPowerOff(std::vector<std::string> userInput);
  void sapSimPowerOn(std::vector<std::string> userInput);
  void sapSimReset(std::vector<std::string> userInput);
  void sapCardReaderStatus(std::vector<std::string> userInput);
  void closeSapConnection(std::vector<std::string> userInput);
  //   void logSapState(telux::tel::SapState sapState);
  //
  //   std::shared_ptr<telux::tel::ISapCardManager> sapCardMgr_ = nullptr;
  //   std::shared_ptr<MySapCommandResponseCallback> mySapCmdResponseCb_;
  //   std::shared_ptr<MyCardReaderCallback> mySapCardReaderCb_;
  //   std::shared_ptr<MySapTransmitApduResponseCallback> myTransmitApduResponseCb_;
  //   std::shared_ptr<MyAtrResponseCallback> myAtrCb_;
};

#endif  // SAPCARDSERVICESMENU_HPP
