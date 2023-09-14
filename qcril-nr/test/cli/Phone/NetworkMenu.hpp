/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#ifndef NETWORKMENU_HPP
#define NETWORKMENU_HPP

#include <memory>
#include <string>
#include <vector>

//#include "telux/tel/PhoneManager.hpp"
//#include <telux/tel/NetworkSelectionManager.hpp>

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

#define PRINT_NOTIFICATION std::cout << std::endl << "\033[1;35mNOTIFICATION: \033[0m" << std::endl

class NetworkMenu : public ConsoleApp {
 public:
  /**
   * Initialize commands and SDK
   */
  void init();

  NetworkMenu(std::string appName, std::string cursor, RilApiSession& rilSession);

  ~NetworkMenu();

  void getNetworkSelectionMode(std::vector<std::string> userInput);
  void setNetworkSelectionMode(std::vector<std::string> userInput);
  void getPreferredNetworks(std::vector<std::string> userInput);
  void setPreferredNetworks(std::vector<std::string> userInput);
  void performNetworkScan(std::vector<std::string> userInput);
  void startNetworkScan(std::vector<std::string> userInput);
  void stopNetworkScan(std::vector<std::string> userInput);
  void setSystemSelectionChannels(std::vector<std::string> userInput);
  void setBandMode(std::vector<std::string> userInput);
  void queryAvailableBandMode(std::vector<std::string> userInput);
  void registerForUnsols(std::vector<std::string> userInput);
  void queryAvailableNetworks(std::vector<std::string> userInput);

 private:
  RilApiSession& rilSession;
  // Member variable to keep the Listener object alive till application ends.
  //   std::shared_ptr<telux::tel::INetworkSelectionListener> networkListener_;
  //   std::shared_ptr<telux::tel::INetworkSelectionManager> networkManager_ = nullptr;
  int convertToRatType(int input);
  //   telux::tel::PreferredNetworkInfo getNetworkInfoFromUser();
};

#endif  // NETWORKMENU_HPP
