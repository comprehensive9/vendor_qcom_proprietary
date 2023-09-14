/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#ifndef SMSMENU_HPP
#define SMSMENU_HPP

#include <memory>
#include <string>
#include <vector>

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class SmsMenu : public ConsoleApp {
 public:
  SmsMenu(std::string appName, std::string cursor, int phoneId, RilApiSession& rilSession);
  ~SmsMenu();
  void init();

 private:
  RilApiSession& rilSession;

 private:
  void sendSms(std::vector<std::string> userInput);
  void sendCdmaSms(std::vector<std::string> userInput);
  void getSmscAddr(std::vector<std::string> userInput);
  void setSmscAddr(std::vector<std::string> userInput);
  void ackSms(std::vector<std::string> userInput);
  void ackCdmaSms(std::vector<std::string> userInput);
  void getCdmaSmsBroadcastConfig(std::vector<std::string> userInput);
  void setCdmaSmsBroadcastConfig(std::vector<std::string> userInput);
  void setCdmaSmsBroadcastActivation(std::vector<std::string> userInput);
  void writeCdmaSmsToRuim(std::vector<std::string> userInput);
  void deleteCdmaSmsOnRuim(std::vector<std::string> userInput);
  void calculateMessageAttributes(std::vector<std::string> userInput);
  void registerForUnsols(std::vector<std::string> userInput);
  void writeSmsToSim(std::vector<std::string> userInput);
  void deleteSmsOnSim(std::vector<std::string> userInput);
  void reportSmsMemoryStatus(std::vector<std::string> userInput);
  void gsmGetBroadcastSmsConfig(std::vector<std::string> userInput);
  void setGsmSetBroadcastSMSConfig(std::vector<std::string> userInput);
  void setGsmSmsBroadcastActivation(std::vector<std::string> userInput);
};

#endif  // SMSMENU_HPP
