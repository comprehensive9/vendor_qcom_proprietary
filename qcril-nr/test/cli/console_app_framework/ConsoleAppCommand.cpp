/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2017, The Linux Foundation. All rights reserved
 */

#include <iostream>
#include <iterator>
#include <sstream>

#include "ConsoleAppCommand.hpp"

/**
 * Initialize an Command Request
 */
ConsoleAppCommand::ConsoleAppCommand(std::string id, std::string name,
                                     std::vector<std::string> arguments,
                                     std::function<void(std::vector<std::string>&)> command)
    : id_(id), name_(name), arguments_(arguments), command_(command) {
}

std::string ConsoleAppCommand::getId() {
  return id_;
}

std::string ConsoleAppCommand::getName() {
  return name_;
}

std::vector<std::string> ConsoleAppCommand::getArguments() {
  return arguments_;
}

void ConsoleAppCommand::displayCommand() {
  std::cout << name_;
  for (auto argument : arguments_) {
    std::cout << " <" << argument << "> ";
  }
  std::cout << std::endl;
}

void ConsoleAppCommand::executeCommand(std::vector<std::string> userInput) {
  command_(userInput);
}
