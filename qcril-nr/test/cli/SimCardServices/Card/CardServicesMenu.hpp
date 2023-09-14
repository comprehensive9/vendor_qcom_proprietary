/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

#ifndef CARDSERVICESMENU_HPP
#define CARDSERVICESMENU_HPP

//#include "telux/tel/CardManager.hpp"
//#include "telux/tel/CardDefines.hpp"
#include "console_app_framework/ConsoleApp.hpp"
//#include "MyCardListener.hpp"
//#include "telux/tel/CardApp.hpp"
#include <RilApiSession.hpp>

class CardServicesMenu : public ConsoleApp {
 public:
  CardServicesMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~CardServicesMenu();
  void init();

 private:
  RilApiSession& rilSession;
  void getCardState(std::vector<std::string> userInput);
  void getSupportedApps(std::vector<std::string> userInput);
  void openLogicalChannel(std::vector<std::string> userInput);
  void closeLogicalChannel(std::vector<std::string> userInput);
  void transmitApdu(std::vector<std::string> userInput);
  void basicTransmitApdu(std::vector<std::string> userInput);
  void changeCardPin(std::vector<std::string> userInput);
  void changeCardPin2(std::vector<std::string> userInput);
  void unlockCardByPuk(std::vector<std::string> userInput);
  void unlockCardByPuk2(std::vector<std::string> userInput);
  void unlockCardByPin(std::vector<std::string> userInput);
  void unlockCardByPin2(std::vector<std::string> userInput);
  void queryPin1LockState(std::vector<std::string> userInput);
  void queryFdnLockState(std::vector<std::string> userInput);
  void setCardLock(std::vector<std::string> userInput);
  void getImsirequest(std::vector<std::string> userInput);
  void setSimIoReq(std::vector<std::string> userInput);
  void CallSetup_command(std::vector<std::string> userInput);
  void Terminalresp_command(std::vector<std::string> userInput) ;
  void envelopecommand(std::vector<std::string> userInput) ;
  void enveloperequest(std::vector<std::string> userInput) ;
  void ServiceRunning_command(std::vector<std::string> userInput);
  void IsimAuthenticate_command(std::vector<std::string> userInput);
  void simAuthenticate_command(std::vector<std::string> userInput);
  void NwDeperso_command(std::vector<std::string> userInput);
  void registerForIndications(std::vector<std::string> userInput);
  void GetAtr(std::vector<std::string> userInput);
  //   std::string appTypeToString(telux::tel::AppType appType);
  //   std::string appStateToString(telux::tel::AppState appState);
  //   std::string cardStateToString(telux::tel::CardState state);

  //   std::shared_ptr<telux::tel::ICardListener> cardListener_;
  //   std::shared_ptr<telux::tel::ICard> card_ = nullptr;
  //   std::shared_ptr<MyOpenLogicalChannelCallback> myOpenLogicalChannelCb_;
  //   std::shared_ptr<MyTransmitApduResponseCallback> myTransmitApduCb_;
  //   std::shared_ptr<MyCardCommandResponseCallback> myCloseLogicalChannelCb_;
  //   std::shared_ptr<telux::tel::ICardManager> cardManager_;
};

#endif  // CARDSERVICESMENU_HPP
