/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

/**
 * Telematics SDK - Sample Application for sap card services
 */

#include <iostream>

#include "SapCardServicesMenu.hpp"
//#include "Utils.hpp"

SapCardServicesMenu::SapCardServicesMenu(std::string appName, std::string cursor)
    : ConsoleApp(appName, cursor) {
  //   // Get default SAP Card Manager instance
  //   sapCardMgr_ = telux::tel::PhoneFactory::getInstance().getSapCardManager();
  //   if(sapCardMgr_) {
  //      mySapCmdResponseCb_ = std::make_shared<MySapCommandResponseCallback>();
  //      myTransmitApduResponseCb_ = std::make_shared<MySapTransmitApduResponseCallback>();
  //      mySapCardReaderCb_ = std::make_shared<MyCardReaderCallback>();
  //      myAtrCb_ = std::make_shared<MyAtrResponseCallback>();
  //   }
}

SapCardServicesMenu::~SapCardServicesMenu() {
  //   mySapCmdResponseCb_ = nullptr;
  //   myTransmitApduResponseCb_ = nullptr;
  //   mySapCardReaderCb_ = nullptr;
  //   myAtrCb_ = nullptr;
}

void SapCardServicesMenu::init() {
  std::shared_ptr<ConsoleAppCommand> openSapConnectionCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "1", "Open_sap_connection", {},
          std::bind(&SapCardServicesMenu::openSapConnection, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> getSapAtrCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("2", "Get_sap_ATR", {},
                        std::bind(&SapCardServicesMenu::getSapAtr, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> requestSapStateCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "3", "Request_sap_state", {},
          std::bind(&SapCardServicesMenu::requestSapState, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> transmitSapApduCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "4", "Transmit_sap_APDU", {},
          std::bind(&SapCardServicesMenu::transmitSapApdu, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> sapSimPowerOffCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "5", "Sap_sim_power_off", {},
          std::bind(&SapCardServicesMenu::sapSimPowerOff, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> sapSimPowerOnCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "6", "Sap_sim_power_on", {},
          std::bind(&SapCardServicesMenu::sapSimPowerOn, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> sapSimResetCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("7", "Sap_sim_reset", {},
                        std::bind(&SapCardServicesMenu::sapSimReset, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> sapCardReaderStatusCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "8", "Sap_card_reader_status", {},
          std::bind(&SapCardServicesMenu::sapCardReaderStatus, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> closeSapConnectionCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "9", "Close_sap_connection", {},
          std::bind(&SapCardServicesMenu::closeSapConnection, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> getStateCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("10", "Get_sap_state", {},
                        std::bind(&SapCardServicesMenu::getState, this, std::placeholders::_1)));
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListSapManagerSubMenu = {
    openSapConnectionCommand, getSapAtrCommand,           requestSapStateCommand,
    transmitSapApduCommand,   sapSimPowerOffCommand,      sapSimPowerOnCommand,
    sapSimResetCommand,       sapCardReaderStatusCommand, closeSapConnectionCommand,
    getStateCommand
  };
  addCommands(commandsListSapManagerSubMenu);
  ConsoleApp::displayMenu();
}

// void SapCardServicesMenu::logSapState(telux::tel::SapState sapState) {
//   if(sapState == telux::tel::SapState::SAP_STATE_NOT_ENABLED) {
//      std::cout << "Sap state not enabled \n";
//   } else if(sapState == telux::tel::SapState::SAP_STATE_CONNECTING) {
//      std::cout << "Sap state connecting \n";
//   } else if(sapState == telux::tel::SapState::SAP_STATE_CONNECTED_SUCCESSFULLY) {
//      std::cout << "Sap state connected successfully \n";
//   } else if(sapState == telux::tel::SapState::SAP_STATE_CONNECTION_ERROR) {
//      std::cout << "Sap state connection error \n";
//   } else if(sapState == telux::tel::SapState::SAP_STATE_DISCONNECTING) {
//      std::cout << "Sap state disconnecting \n";
//   } else if(sapState == telux::tel::SapState::SAP_STATE_DISCONNECTED_SUCCESSFULLY) {
//      std::cout << "Sap state disconnected successfully \n";
//   }
//}

void SapCardServicesMenu::openSapConnection(std::vector<std::string> userInput) {
  //   sapCardMgr_->openConnection(telux::tel::SapCondition::SAP_CONDITION_BLOCK_VOICE_OR_DATA,
  //                               mySapCmdResponseCb_);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void SapCardServicesMenu::getSapAtr(std::vector<std::string> userInput) {
  //   sapCardMgr_->requestAtr(myAtrCb_);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::transmitSapApdu(std::vector<std::string> userInput) {
  //   int cla, instruction, p1, p2, lc, tmpInp;
  //   std::vector<uint8_t> data;
  //
  //   cla = 0;
  //   instruction = 0;
  //   p1 = 0;
  //   p2 = 0;
  //   lc = 0;
  //
  //   std::string user_input;
  //   std::cout << std::endl;
  //   std::cout << "Enter CLA : ";
  //   std::cin >> cla;
  //   Utils::validateInput(cla);
  //   std::cout << "Enter INS : ";
  //   std::cin >> instruction;
  //   Utils::validateInput(instruction);
  //   std::cout << "Enter P1 : ";
  //   std::cin >> p1;
  //   Utils::validateInput(p1);
  //   std::cout << "Enter P2 : ";
  //   std::cin >> p2;
  //   Utils::validateInput(p2);
  //   std::cout << "Enter Lc : ";
  //   std::cin >> lc;
  //   Utils::validateInput(lc);
  //   for(int i = 0; i < lc; i++) {
  //      std::cout << "Enter DATA (" << i + 1 << ") :";
  //      std::cin >> tmpInp;
  //      Utils::validateInput(tmpInp);
  //      data.emplace_back((uint8_t)tmpInp);
  //   }
  //   auto ret
  //      = sapCardMgr_->transmitApdu((uint8_t)cla, (uint8_t)instruction, (uint8_t)p1, (uint8_t)p2,
  //                                  (uint8_t)lc, data, 0, myTransmitApduResponseCb_);
  //   if(ret == telux::common::Status::SUCCESS) {
  //      std::cout << "Sap transmit APDU is successful \n";
  //   } else {
  //      std::cout << "Sap transmit APDU failed \n";
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::sapSimPowerOff(std::vector<std::string> userInput) {
  //   sapCardMgr_->requestSimPowerOff(mySapCmdResponseCb_);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::sapSimPowerOn(std::vector<std::string> userInput) {
  //   sapCardMgr_->requestSimPowerOn(mySapCmdResponseCb_);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::sapSimReset(std::vector<std::string> userInput) {
  //   sapCardMgr_->requestSimReset(mySapCmdResponseCb_);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::sapCardReaderStatus(std::vector<std::string> userInput) {
  //   sapCardMgr_->requestCardReaderStatus(mySapCardReaderCb_);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::closeSapConnection(std::vector<std::string> userInput) {
  //   sapCardMgr_->closeConnection(mySapCmdResponseCb_);
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::requestSapState(std::vector<std::string> userInput) {
  //   telux::tel::SapState sapstate;
  //   if(sapCardMgr_->requestSapState(MySapStateCallback::sapStateResponse)
  //      == telux::common::Status::SUCCESS) {
  //      std::cout << "Request sap state success \n";
  //   } else {
  //      std::cout << "Request sap state failed \n";
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void SapCardServicesMenu::getState(std::vector<std::string> userInput) {
  //   telux::tel::SapState sapstate;
  //   if(sapCardMgr_->getState(sapstate) == telux::common::Status::SUCCESS) {
  //      logSapState(sapstate);
  //      std::cout << "Get sap state success \n";
  //   } else {
  //      std::cout << "Get sap state failed \n";
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
