/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2017, The Linux Foundation. All rights reserved
 */

#ifndef CONSOLEAPPCOMMAND_HPP
#define CONSOLEAPPCOMMAND_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

/**
 * This class provides a generic way of creating a menu command for a given Telematics SDK API.
 * It is intended to be used in console applications derived from Console App Framework.
 */
class ConsoleAppCommand {
 public:
  ConsoleAppCommand(std::string id, std::string name, std::vector<std::string> arguments,
                    std::function<void(std::vector<std::string>&)> command);

  /**
   * Get command id
   */
  std::string getId();

  /**
   * Get command name
   */
  std::string getName();

  /**
   * Get command arguments
   */
  std::vector<std::string> getArguments();

  /**
   * Display the command
   */
  void displayCommand();

  /**
   * Executes the command
   */
  void executeCommand(std::vector<std::string> userInput);

 private:
  std::string id_;                      // Command id
  std::string name_;                    // Command  name
  std::vector<std::string> arguments_;  // Input arguments sequence
  std::function<void(std::vector<std::string>&)> command_;
};

#define CREATE_COMMAND(HANDLER, KEY, ...)                \
  std::make_shared<ConsoleAppCommand>(ConsoleAppCommand( \
      "", KEY, { __VA_ARGS__ }, std::bind(&HANDLER, this, std::placeholders::_1)))

#endif  // CONSOLEAPPCOMMAND_HPP
