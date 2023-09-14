/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <sstream>

#include "ConsoleApp.hpp"
#include "ConsoleAppCommand.hpp"

const std::string MENU_DIVIDER = "------------------------------------------------";

ConsoleApp::ConsoleApp(std::string appName, std::string cursor)
    : appName_(appName), cursor_(cursor) {
}

/**
 * Displaying Menu of Applications Requested
 */
void ConsoleApp::displayMenu() {
  displayBanner();
  // Iterate through the supportedCommands_ list and display all the commands
  for (auto index = 0; index < supportedCommands_.size(); index++) {
    std::cout << "   " << index + 1 << " - ";
    supportedCommands_[index]->displayCommand();
  }
  std::cout << std::endl;
  std::cout << "   ? / h - help" << std::endl;
  std::cout << "   q / 0 - exit" << std::endl << std::endl;
  std::cout << MENU_DIVIDER << std::endl << std::endl;
}

/**
 * Display Cursor to Read User Input
 */
void ConsoleApp::displayCursor() {
  std::cout << cursor_;
}

/**
 * Display the title banner
 */
void ConsoleApp::displayBanner() {
  std::cout << MENU_DIVIDER << std::endl;
  int paddingLength = MENU_DIVIDER.length() / 2 + appName_.length() / 2;
  std::cout << std::setw(paddingLength) << appName_ << std::endl;
  std::cout << MENU_DIVIDER << std::endl << std::endl;
}

/**
 * Read user request from command line
 */
std::vector<std::string> ConsoleApp::readCommand() {
  ConsoleApp::displayCursor();
  // input string
  std::string command;
  std::getline(std::cin, command);

  // separate input string based on whitespace
  std::istringstream iss(command);

  // iterate on a stream and store collection of substring into vector of strings
  std::vector<std::string> userInput(std::istream_iterator<std::string>{ iss },
                                     std::istream_iterator<std::string>());
  return userInput;
}

inline bool operator==(const std::shared_ptr<ConsoleAppCommand>& command,
                       const std::vector<std::string>& inputCommand) {
  std::string cmdStr = command->getName();
  std::string inpStr = inputCommand[0];
  // Function to convert to lowercase
  std::transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), ::tolower);
  std::transform(inpStr.begin(), inpStr.end(), inpStr.begin(), ::tolower);

  if (cmdStr == inpStr) {
    if (command->getArguments().size() == (inputCommand.size() - 1)) {
      return true;
    }
  }
  return false;
}
/**
 * Get console app command from user input
 */
std::shared_ptr<ConsoleAppCommand> ConsoleApp::getAppCommandFromUserInput(
    std::vector<std::string> inputCommand) {
  if (inputCommand.size() > 0) {
    for (auto command : supportedCommands_) {
      if (command == inputCommand) {
        return command;
      }
    }

    auto index = std::strtol(inputCommand[0].c_str(), nullptr, 10);
    if (index > 0 && index <= supportedCommands_.size() &&
        supportedCommands_[index - 1]->getArguments().size() == (inputCommand.size() - 1)) {
      return supportedCommands_[index - 1];
    }
  }
  return nullptr;
}

/**
 * Add  commands into supportCommands_ list
 */
void ConsoleApp::addCommands(std::vector<std::shared_ptr<ConsoleAppCommand>> supportedCommandsList) {
  for (auto command : supportedCommandsList) {
    supportedCommands_.emplace_back(command);
  }
}

int ConsoleApp::mainLoop() {
  while (true) {
    std::vector<std::string> userInput = readCommand();
    if (userInput.size() == 0) continue;
    if (userInput[0] == "0" || userInput[0] == "exit" || userInput[0] == "q" ||
        userInput[0] == "quit" || userInput[0] == "back") {
      break;
    } else if (userInput[0] == "?" || userInput[0] == "help" || userInput[0] == "h") {
      displayMenu();
      continue;
    }

    std::shared_ptr<ConsoleAppCommand> conAppCmd = getAppCommandFromUserInput(userInput);
    if (conAppCmd) {
      conAppCmd->executeCommand(userInput);
    } else {
      std::cout << "Invalid command: " << userInput[0] << " entered." << std::endl;
      std::cout << "Please enter valid command and arguments." << std::endl;
    }
  }
  return 0;
}
