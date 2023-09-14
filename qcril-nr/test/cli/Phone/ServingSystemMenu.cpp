/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#include <chrono>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

//#include <telux/tel/PhoneFactory.hpp>

//#include "MyServingSystemHandler.hpp"
#include "ServingSystemMenu.hpp"

ServingSystemMenu::ServingSystemMenu(std::string appName, std::string cursor)
    : ConsoleApp(appName, cursor) {
  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();
  //  Get the PhoneFactory and ServingSystemManager instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   servingSystemManager_
  //      = telux::tel::PhoneFactory::getInstance().getServingSystemManager(DEFAULT_SLOT_ID);
  //
  //   //  Check if serving subsystem is ready
  //   bool subSystemStatus = servingSystemManager_->isSubsystemReady();
  bool subSystemStatus = true;

  //   //  If serving subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::cout << "\n\nServing subsystem is not ready, Please wait!!!..." << std::endl;
  //      std::future<bool> f = servingSystemManager_->onSubsystemReady();
  //      // If we want to wait unconditionally for serving subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  //  Exit the application, if SDK is unable to initialize serving subsystems
  if (subSystemStatus) {
    endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Elapsed Time for Subsystems to ready : " << elapsedTime.count() << "s\n"
              << std::endl;
  } else {
    std::cout << " *** ERROR - Unable to initialize serving subsystem" << std::endl;
    exit(0);
  }
  //   servingSystemListener_ = std::make_shared<MyServingSystemListener>();
  //   telux::common::Status status = servingSystemManager_->registerListener(servingSystemListener_);
  //   if(status != telux::common::Status::SUCCESS) {
  //      std::cout << "Failed to registerListener for Serving system Manager" << std::endl;
  //   }
}

ServingSystemMenu::~ServingSystemMenu() {
  //   servingSystemManager_->deregisterListener(servingSystemListener_);
  //   servingSystemManager_ = nullptr;
}

void ServingSystemMenu::init() {
  std::shared_ptr<ConsoleAppCommand> getRatModePreferenceCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "1", "Get_RAT_mode_preference", {},
          std::bind(&ServingSystemMenu::getRatModePreference, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> setRatModePreferenceCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "2", "Set_RAT_mode_preference", {},
          std::bind(&ServingSystemMenu::setRatModePreference, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> getServiceDomainPreferenceCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "3", "Get_service_domain_preference", {},
          std::bind(&ServingSystemMenu::getServiceDomainPreference, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> setServiceDomainPreferenceCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "4", "Set_service_domain_preference", {},
          std::bind(&ServingSystemMenu::setServiceDomainPreference, this, std::placeholders::_1)));
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListNetworkSubMenu = {
    getRatModePreferenceCommand, setRatModePreferenceCommand, getServiceDomainPreferenceCommand,
    setServiceDomainPreferenceCommand
  };
  addCommands(commandsListNetworkSubMenu);
  ConsoleApp::displayMenu();
}

void ServingSystemMenu::getRatModePreference(std::vector<std::string> userInput) {
  //   if(servingSystemManager_) {
  //      auto ret = servingSystemManager_->requestRatPreference(
  //         MyRatPreferenceResponseCallback::ratPreferenceResponse);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "\nGet RAT mode preference request sent successfully\n";
  //      } else {
  //         std::cout << "\nGet RAT mode preference request failed \n";
  //      }
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void ServingSystemMenu::setRatModePreference(std::vector<std::string> userInput) {
  //   if(servingSystemManager_) {
  //      char delimiter = '\n';
  //      std::string preference;
  //      telux::tel::RatPreference pref;
  //      std::vector<int> options;
  //      std::cout
  //         << "Available RAT mode preferences: \n"
  //            "(0 - CDMA_1X\n 1 - CDMA_EVDO\n 2 - GSM\n 3 - WCDMA\n 4 - LTE\n 5 - TDSCDMA) \n\n";
  //      std::cout
  //         << "Enter RAT mode preferences\n(For example: enter 2,4 to prefer GSM & LTE mode): ";
  //      std::getline(std::cin, preference, delimiter);
  //
  //      std::stringstream ss(preference);
  //      int i;
  //      while(ss >> i) {
  //         options.push_back(i);
  //         if(ss.peek() == ',' || ss.peek() == ' ')
  //            ss.ignore();
  //      }
  //
  //      for(auto &opt : options) {
  //         if(opt >= 0 && opt <= 5) {
  //            try {
  //               pref.set(opt);
  //            } catch(const std::exception &e) {
  //               std::cout << "ERROR: invalid input, please enter numerical values " << opt
  //                         << std::endl;
  //            }
  //         } else {
  //            std::cout << "Preference should not be out of range" << std::endl;
  //         }
  //      }
  //      auto ret = servingSystemManager_->setRatPreference(
  //         pref, MyServingSystemResponsecallback::servingSystemResponse);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "\nSet RAT mode preference request sent successfully\n";
  //      } else {
  //         std::cout << "\nSet RAT mode preference request failed \n";
  //      }
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void ServingSystemMenu::getServiceDomainPreference(std::vector<std::string> userInput) {
  //   if(servingSystemManager_) {
  //      auto ret = servingSystemManager_->requestServiceDomainPreference(
  //         MyServiceDomainResponseCallback::serviceDomainResponse);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "\nGet service domain preference request sent successfully\n";
  //      } else {
  //         std::cout << "\nGet service domain preference request failed \n";
  //      }
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void ServingSystemMenu::setServiceDomainPreference(std::vector<std::string> userInput) {
  //   if(servingSystemManager_) {
  //      std::string serviceDomain;
  //      int opt = -1;
  //      std::cout << "Enter service domain preference: (0 - CS, 1 - PS, 2 - CS/PS): ";
  //      std::cin >> serviceDomain;
  //      if(!serviceDomain.empty()) {
  //         try {
  //            opt = std::stoi(serviceDomain);
  //         } catch(const std::exception &e) {
  //            std::cout << "ERROR: invalid input " << opt << std::endl;
  //         }
  //      } else {
  //         std::cout << "Service domain should not be empty";
  //      }
  //      telux::tel::ServiceDomainPreference domainPref
  //         = static_cast<telux::tel::ServiceDomainPreference>(opt);
  //      auto ret = servingSystemManager_->setServiceDomainPreference(
  //         domainPref, MyServingSystemResponsecallback::servingSystemResponse);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "\nSet service domain preference request sent successfully\n";
  //      } else {
  //         std::cout << "\nSet service domain preference request failed \n";
  //      }
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
