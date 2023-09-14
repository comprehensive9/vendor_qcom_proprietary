/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#ifndef SERVINGSYSTEMMENU_HPP
#define SERVINGSYSTEMMENU_HPP

#include <memory>
#include <string>
#include <vector>

//#include "telux/tel/PhoneManager.hpp"
//#include <telux/tel/ServingSystemManager.hpp>

#include "console_app_framework/ConsoleApp.hpp"

#define PRINT_NOTIFICATION std::cout << std::endl << "\033[1;35mNOTIFICATION: \033[0m" << std::endl

class ServingSystemMenu : public ConsoleApp {
 public:
  /**
   * Initialize commands and SDK
   */
  void init();

  ServingSystemMenu(std::string appName, std::string cursor);

  ~ServingSystemMenu();

  void getRatModePreference(std::vector<std::string> userInput);
  void setRatModePreference(std::vector<std::string> userInput);
  void getServiceDomainPreference(std::vector<std::string> userInput);
  void setServiceDomainPreference(std::vector<std::string> userInput);

 private:
  //   // Member variable to keep the Listener object alive till application ends.
  //   std::shared_ptr<telux::tel::IServingSystemListener> servingSystemListener_;
  //   std::shared_ptr<telux::tel::IServingSystemManager> servingSystemManager_ = nullptr;
};

#endif  // SERVINGSYSTEMMENU_HPP
