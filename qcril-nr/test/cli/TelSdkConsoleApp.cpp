/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved
 */

/**
 * @file       TelSdkConsoleApp.cpp
 *
 * @brief      This is entry class for console application for Telematics SDK,
 *             It allows one to interactively invoke most of the public APIs in the Telematics SDK.
 */

#include <cerrno>
#include <iostream>
#include <memory>

extern "C" {
#include <cxxabi.h>
#include <signal.h>
}

#include "Call/CallMenu.hpp"
#include "ECall/ECallMenu.hpp"
#include "Phone/PhoneMenu.hpp"
#include "Sms/SmsMenu.hpp"
#include "Data/DataMenu.hpp"
#include "SimCardServices/SimCardServicesMenu.hpp"
#include "Ims/ImsMenu.hpp"

#include "TelSdkConsoleApp.hpp"

TelSdkConsoleApp::TelSdkConsoleApp(const std::string& rilSocketPath, std::string appName,
    std::string cursor)
    : ConsoleApp(appName, cursor), rilSession(rilSocketPath) {
}

TelSdkConsoleApp::~TelSdkConsoleApp() {
}

/**
 * Used for creating a menus of high level features
 */
void TelSdkConsoleApp::init() {
    Status s = rilSession.initialize(
      [] (Status s) {
            std::cout << "Error detected in session. Error: " << (int) s;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << "Failed to initialize API session.";
    }

    rilSession.registerRilServerReadyIndicationHandler(
        [] (int version) {
          std::cout << std::endl
              << "RIL server is ready to accept requests. RIL Server Version: "
              << version << "." << std::endl;
        }
    );

//    std::shared_ptr<ConsoleAppCommand> smsMenuCommand
//        = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
//            "3", "SMS", {}, std::bind(&TelSdkConsoleApp::smsMenu, this, std::placeholders::_1)));
//
//std::shared_ptr<ConsoleAppCommand> eCallMenuCommand
//    = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand("4", "eCall", {},
//        std::bind(&TelSdkConsoleApp::eCallMenu, this, std::placeholders::_1)));
//
//    std::shared_ptr<ConsoleAppCommand> simCardMenuCommand
//        = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand("5", "Card_Services", {},
//            std::bind(&TelSdkConsoleApp::simCardMenu, this, std::placeholders::_1)));
//
//    std::shared_ptr<ConsoleAppCommand> dataMenuCommand
//        = std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
//            "6", "Data", {}, std::bind(&TelSdkConsoleApp::dataMenu, this, std::placeholders::_1)));

    std::vector<std::shared_ptr<ConsoleAppCommand>> mainMenuCommands = {
       CREATE_COMMAND(TelSdkConsoleApp::phoneMenu, "Phone_Status"),
       CREATE_COMMAND(TelSdkConsoleApp::callMenu, "Dialer"),
       CREATE_COMMAND(TelSdkConsoleApp::imsMenu, "IMS_Menu"),
       CREATE_COMMAND(TelSdkConsoleApp::dataMenu, "Data"),
       CREATE_COMMAND(TelSdkConsoleApp::simCardMenu, "Simcard"),
       CREATE_COMMAND(TelSdkConsoleApp::smsMenu, "SMS"),
    };

    addCommands(mainMenuCommands);
    TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::phoneMenu(std::vector<std::string> userInput) {
  PhoneMenu phoneMenu("Phone Menu", "phone> ", 1, rilSession);
  phoneMenu.init();
  phoneMenu.mainLoop();
  TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::callMenu(std::vector<std::string> userInput) {
  CallMenu callMenu("Dialer Menu", "dialer> ", rilSession);
  callMenu.init();
  callMenu.mainLoop();
  TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::imsMenu(std::vector<std::string> userInput) {
  ImsMenu imsMenu("IMS Menu", "IMS> ", rilSession);
  imsMenu.init();
  imsMenu.mainLoop();
  TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::eCallMenu(std::vector<std::string> userInput) {
  ECallMenu eCallMenu("eCall Menu", "eCall> ");
  eCallMenu.init();
  eCallMenu.mainLoop();
  TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::simCardMenu(std::vector<std::string> userInput) {
  SimCardServicesMenu simCardServicesMenu("SIM Card Services Menu", "card_services> ", rilSession);
  simCardServicesMenu.init();
  simCardServicesMenu.mainLoop();
  TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::smsMenu(std::vector<std::string> userInput) {
  SmsMenu smsMenu("SMS Menu", "SMS> ", 1, rilSession);
  smsMenu.init();
  smsMenu.mainLoop();
  TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::dataMenu(std::vector<std::string> userInput) {
    DataMenu dataMenu("Data Menu", "data> ", rilSession);
    dataMenu.init();
    dataMenu.mainLoop();
    TelSdkConsoleApp::displayMenu();
}

void TelSdkConsoleApp::displayMenu() {
  ConsoleApp::displayMenu();

  // Do not perform requestOperatingMode in CV2X machine
  // since operating mode cannot be changed
  //#ifndef FEATURE_CV2X_ONLY
  //    std::shared_ptr<ModemStatus> modemStatus = std::make_shared<ModemStatus>();
  //    modemStatus->printOperatingMode();
  //#endif
}

/*std::string& toString() {

}*/

void signalHandler(int sig) {
  exit(1);
}

void setupSignal() {
  signal(SIGSEGV, signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGBUS, signalHandler);
  signal(SIGILL, signalHandler);
  signal(SIGFPE, signalHandler);
  signal(SIGPIPE, signalHandler);
}

// Main function that displays the console and processes user input
int main(int argc, char** argv) {
  //    auto sdkVersion = telux::common::Version::getSdkVersion();
  //    std::string appName = "Telematics SDK v" + std::to_string(sdkVersion.major) + "."
  //                          + std::to_string(sdkVersion.minor) + "."
  //                          + std::to_string(sdkVersion.patch);

  const char* rilInstanceArg = nullptr;
  for (int i = 1; i < argc; i++) {
    // handles both "-i\S+" and "-i \S+"
    if (argv[i] != nullptr && !strncmp(argv[i], "-i", 2)) {
      // argv[i] is expected to be null-terminated
      if (argv[i][2] != '\0') {    // handle "-i\S+"
        rilInstanceArg = &argv[i][2];
      } else {                     // handle "-i \S+"
        i++;
        if (i < argc) {
          rilInstanceArg = argv[i];
        }
      }
    }
  }

  unsigned long instance = 0;
  if (rilInstanceArg != nullptr) {
    char* end = nullptr;
    instance = std::strtoul(rilInstanceArg, &end, 10);
    int errsv = errno;
    const bool conversionError = (errsv == ERANGE || end == rilInstanceArg);

    if (conversionError
        || instance > static_cast<unsigned long>(RilInstance::MAX)) {
      std::cout << "RIL instance ID provided (" << rilInstanceArg << ") is invalid."
        << " RIL instance ID must be greater than or equal to "
        << static_cast<unsigned long>(RilInstance::MIN) << " and less than or equal to "
        << static_cast<unsigned long>(RilInstance::MAX) << "." << std::endl;
      exit(1);
    }
  }

  std::cout << "Connecting to RIL instance " << instance << "." << std::endl;

  std::string appName = "QCRIL Client Lib v1.0";

  setupSignal();

  TelSdkConsoleApp telsdkConsoleApp(
      std::string("/dev/socket/qcrild/rild") + std::to_string(instance),
      appName, "ril> ");

  telsdkConsoleApp.init();  // initialize commands and display

  return telsdkConsoleApp.mainLoop();  // Main loop to continuously read and execute commands
}
