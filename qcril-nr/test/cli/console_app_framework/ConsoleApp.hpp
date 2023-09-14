/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

#ifndef CONSOLEAPP_HPP
#define CONSOLEAPP_HPP

#include <string>
#include <vector>

#include "ConsoleAppCommand.hpp"

/**
 * ConsoleApp provides skeleton implementation to display a list of commands,
 * read user input and trigger commands.
 */
class ConsoleApp {
 public:
  /**
   * Displaying menu of supported commands
   */
  void displayMenu();

  /**
   * Add commands into supported commands list
   */
  void addCommands(std::vector<std::shared_ptr<ConsoleAppCommand>> supportedCommandsList);

  /**
   * Initialize commands and display
   */
  void init(){};

  /**
   * Main loop to display commands, read user input and execute the commands
   */
  int mainLoop();

  ConsoleApp(std::string appName, std::string cursor);

 private:
  /**
   * A list of commands supported by the ConsoleApp
   */
  std::vector<std::shared_ptr<ConsoleAppCommand>> supportedCommands_;
  std::string appName_, cursor_;

  /**
   * Display cursor to read user input
   */
  void displayCursor();

  /**
   * Display the title banner
   */
  void displayBanner();

  /**
   * Read user input from command line
   */
  std::vector<std::string> readCommand();

  /**
   * Get console app command from user input
   */
  std::shared_ptr<ConsoleAppCommand> getAppCommandFromUserInput(
      std::vector<std::string> inputCommand);
};

#endif  // CONSOLEAPP_HPP
