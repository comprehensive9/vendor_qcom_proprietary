/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#include "Card/CardServicesMenu.hpp"
#include "SapCard/SapCardServicesMenu.hpp"

#include "SimCardServicesMenu.hpp"

SimCardServicesMenu::SimCardServicesMenu(std::string appName, std::string cursor,
                                         RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
}

SimCardServicesMenu::~SimCardServicesMenu() {
}

void SimCardServicesMenu::init() {
  std::shared_ptr<ConsoleAppCommand> cardServicesMenuCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "1", "Card_Services", {},
          std::bind(&SimCardServicesMenu::cardServicesMenu, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> sapCardMenuCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("2", "Sap_Card_Services", {},
                        std::bind(&SimCardServicesMenu::sapCardMenu, this, std::placeholders::_1)));

  std::vector<std::shared_ptr<ConsoleAppCommand>> mainMenuCommands = { cardServicesMenuCommand,
                                                                       sapCardMenuCommand };

  addCommands(mainMenuCommands);
  ConsoleApp::displayMenu();
}

void SimCardServicesMenu::cardServicesMenu(std::vector<std::string> userInput) {
  CardServicesMenu cardServicesMenu("Card Services Menu", "card> ", rilSession);
  cardServicesMenu.init();
  cardServicesMenu.mainLoop();  // Main loop to continuously read and execute commands
}

void SimCardServicesMenu::sapCardMenu(std::vector<std::string> userInput) {
  SapCardServicesMenu sapCardServicesMenu("SapCard Services Menu", "sap> ");
  sapCardServicesMenu.init();
  sapCardServicesMenu.mainLoop();  // Main loop to continuously read and execute commands
}
