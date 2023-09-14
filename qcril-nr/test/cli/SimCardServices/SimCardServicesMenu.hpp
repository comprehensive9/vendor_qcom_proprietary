/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#ifndef SIMCARDSERVICESMENU_HPP
#define SIMCARDSERVICESMENU_HPP

#include "console_app_framework/ConsoleApp.hpp"

class SimCardServicesMenu : public ConsoleApp {
 public:
  SimCardServicesMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~SimCardServicesMenu();
  void init();
  void cardServicesMenu(std::vector<std::string> userInput);
  void sapCardMenu(std::vector<std::string> userInput);

 private:
  RilApiSession& rilSession;
};

#endif  // SIMCARDSERVICESMENU_HPP
