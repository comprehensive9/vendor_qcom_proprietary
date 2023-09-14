/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

/**
 * @file       TelSdkConsoleApp.hpp
 *
 * @brief      This is entry class for console application for Telematics SDK,
 *             It allows one to interactively invoke most of the public APIs in the Telematics SDK.
 */

#ifndef TELSDKCONSOLEAPP_HPP
#define TELSDKCONSOLEAPP_HPP

#include <string>
#include <vector>

//#include "ModemStatus.hpp"
#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class TelSdkConsoleApp : public ConsoleApp {
 public:
  TelSdkConsoleApp(const std::string& rilSocketPath, std::string appName,
    std::string cursor);
  ~TelSdkConsoleApp();

  /**
   * Used for creating a menus of high level features
   */
  void init();

  // Displays main menu
  void displayMenu();

 private:
  RilApiSession rilSession;

 private:
  void phoneMenu(std::vector<std::string> userInput);
  void callMenu(std::vector<std::string> userInput);
  void imsMenu(std::vector<std::string> userInput);
  void eCallMenu(std::vector<std::string> userInput);
  void smsMenu(std::vector<std::string> userInput);
  void simCardMenu(std::vector<std::string> userInput);
  void dataMenu(std::vector<std::string> userInput);
};

#endif  // TELSDKCONSOLEAPP_HPP
