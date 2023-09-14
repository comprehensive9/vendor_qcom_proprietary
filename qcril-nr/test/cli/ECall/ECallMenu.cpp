/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */
#include <chrono>

#include <iostream>

//#include <telux/tel/PhoneFactory.hpp>

#include "ECallMenu.hpp"
//#include "MyECallListener.hpp"
//#include "Utils.hpp"

// Config file name. Using current directory as default path.
#define MSDSETTINGS_FILE "./msdsettings.txt"
#define UPDATED_MSDSETTINGS_FILE "./updated_msdsettings.txt"

#define PRINT_NOTIFICATION std::cout << "\033[1;35mNOTIFICATION: \033[0m"

const std::string GREEN = "\033[0;32m";
const std::string RED = "\033[0;31m";
const std::string BOLD_RED = "\033[1;31m";
const std::string DONE = "\033[0m";  // No color

//// std::function callback for CallManager::makeECall
// void makeEcallResponse(telux::common::ErrorCode error, std::shared_ptr<telux::tel::ICall> call) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_NOTIFICATION << "makeECall Request failed with errorCode: " << static_cast<int>(error)
//                         << ", description: " << Utils::getErrorCodeAsString(error) << std::endl;
//   }
//}
//
//// std::function callback for CallManager::updateECallMsd
// void updateEcallResponse(telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_NOTIFICATION
//         << "updateECallMsd Request failed with errorCode: " << static_cast<int>(error)
//         << ", description: " << Utils::getErrorCodeAsString(error) << std::endl;
//   }
//}

ECallMenu::ECallMenu(std::string appName, std::string cursor) : ConsoleApp(appName, cursor) {
  //   callListener_ = std::make_shared<MyECallListener>();
  //   callCommandCallback_ = std::make_shared<CallCommandCallback>();
  //   updateMsdCommandCallback_ = std::make_shared<UpdateMsdCommandCallback>();
  //   hangupCommandCallback_ = std::make_shared<HangupCommandCallback>();
  //   answerCommandCallback_ = std::make_shared<AnswerCommandCallback>();
}

ECallMenu::~ECallMenu() {
  //   removeCallListener(callListener_);
  //   callCommandCallback_ = nullptr;
  //   updateMsdCommandCallback_ = nullptr;
  //   hangupCommandCallback_ = nullptr;
  //   answerCommandCallback_ = nullptr;
}

/**
 * Initializing Commands and Display
 */
void ECallMenu::init() {
  // below commands are used to add menu options like below

  std::shared_ptr<ConsoleAppCommand> eCallSosCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "1", "ECall-SOS", {}, std::bind(&ECallMenu::eCallSos, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> eCallCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("2", "ECall",
                        { ECALL_CATEGORY_AUTO + " | " + ECALL_CATEGORY_MANUAL,
                          ECALL_VARIANT_TEST + " | " + ECALL_VARIANT_EMERGENCY },
                        std::bind(&ECallMenu::makeECall, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> voiceECallCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("3", "Voice_ECall",
                        { ECALL_CATEGORY_AUTO + " | " + ECALL_CATEGORY_MANUAL, "phone number" },
                        std::bind(&ECallMenu::makeVoiceECall, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> updateMsdCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("4", "Update_eCall_MSD", {},
                        std::bind(&ECallMenu::updateECallMSD, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> dialCommad =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "5", "Dial", { "number" }, std::bind(&ECallMenu::makeCall, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> hangupCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "6", "Hangup", {}, std::bind(&ECallMenu::hangup, this, std::placeholders::_1)));

  std::shared_ptr<ConsoleAppCommand> getCallsCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("7", "Get_InProgress_calls", {},
                        std::bind(&ECallMenu::getCalls, this, std::placeholders::_1)));

  std::shared_ptr<ConsoleAppCommand> answerCallCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "8", "Answer_call", {}, std::bind(&ECallMenu::answerCall, this, std::placeholders::_1)));

  std::shared_ptr<ConsoleAppCommand> eCallWithPdu = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("9", "eCall_with_MSD_PDU", {},
                        std::bind(&ECallMenu::eCallWithPdu, this, std::placeholders::_1)));

  std::shared_ptr<ConsoleAppCommand> updateEcallMsd = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("10", "Update_eCall_MSD_PDU", {},
                        std::bind(&ECallMenu::updateEcallMsdWithPdu, this, std::placeholders::_1)));

  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsList = {
    eCallSosCommand, eCallCommand,    voiceECallCommand, updateMsdCommand, dialCommad,
    hangupCommand,   getCallsCommand, answerCallCommand, eCallWithPdu,     updateEcallMsd
  };

  addCommands(commandsList);

  if (ECallMenu::initalizeSDK()) {
    ConsoleApp::displayMenu();
  }
}

// void ECallMenu::registerCallListener(std::shared_ptr<telux::tel::ICallListener> listener) {
//   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
//   auto callManager = phoneFactory.getCallManager();
//   callManager->registerListener(listener);
//}
//
// void ECallMenu::removeCallListener(std::shared_ptr<telux::tel::ICallListener> listener) {
//   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
//   auto callManager = phoneFactory.getCallManager();
//   callManager->removeListener(listener);
//}

bool ECallMenu::initalizeSDK() {
  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();
  //   //  Get the PhoneFactory and PhoneManager instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto phoneManager = phoneFactory.getPhoneManager();
  //
  //   //  Check if telephony subsystem is ready
  //   bool subSystemStatus = phoneManager->isSubsystemReady();
  bool subSystemStatus = true;
  //   //  If telephony subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::future<bool> f = phoneManager->onSubsystemReady();
  //      // If we want to wait unconditionally for telephony subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  //  Exit the application, if SDK is unable to initialize telephony subsystems
  if (subSystemStatus) {
    endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    //      registerCallListener(callListener_);
    return true;
  } else {
    std::cout << "Unable to initialize subSystem" << std::endl;
    exit(1);
  }
}

/**
 * Sample dial application
 */
void ECallMenu::makeCall(std::vector<std::string> inputCommand) {
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto callManager = phoneFactory.getCallManager();
  //   auto phoneManager = phoneFactory.getPhoneManager();
  //   auto spDefaultPhone = phoneManager->getPhone();
  //   std::cout << "dialing " << inputCommand[1] << std::endl;  // Phone Number entered by user
  //   std::shared_ptr<telux::tel::ICall> spCall;
  //   const std::string phoneNumber = inputCommand[1];  // Phone Number mandatory
  //   int phoneId = DEFAULT_PHONE_ID;
  //   telux::common::Status status = callManager->makeCall(phoneId, phoneNumber,
  //   callCommandCallback_); std::cout
  //      << (status == telux::common::Status::SUCCESS ? GREEN + "  Dial request is successful" +
  //      DONE
  //                                                   : RED + "  Dial request failed" + DONE)
  //      << '\n';
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

void ECallMenu::answerCall(std::vector<std::string> inputCommand) {
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   try {
  //      std::shared_ptr<telux::tel::ICall> spCall = nullptr;
  //      std::vector<std::shared_ptr<telux::tel::ICall>> inProgressCalls
  //         = phoneFactory.getCallManager()->getInProgressCalls();
  //      // Fetch the list of in prgress calls from CallManager and accept the incoming call.
  //      for(auto callIterator = std::begin(inProgressCalls);
  //          callIterator != std::end(inProgressCalls); ++callIterator) {
  //         if((*callIterator)->getCallState() == telux::tel::CallState::CALL_INCOMING) {
  //            spCall = *callIterator;
  //            break;
  //         }
  //      }
  //      if(spCall) {
  //         std::cout << "Sending request to accept call " << std::endl;
  //         spCall->answer(answerCommandCallback_);
  //      } else {
  //         std::cout << "No incoming call to accept " << std::endl;
  //      }
  //   } catch(const std::exception &e) {
  //      std::cout << "ERROR: Exception caught -" << e.what() << std::endl;
  //   }
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

/**
 * Sample hangup operation
 */
void ECallMenu::hangup(std::vector<std::string> inputCommand) {
  //   try {
  //
  //      std::shared_ptr<telux::tel::ICall> spCall = nullptr;
  //      // Iterate through the call list in the application and hangup the first Call that is
  //      // Active or on Hold
  //      auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //      std::vector<std::shared_ptr<telux::tel::ICall>> callList
  //         = phoneFactory.getCallManager()->getInProgressCalls();
  //      for(auto callIterator = std::begin(callList); callIterator != std::end(callList);
  //          ++callIterator) {
  //         telux::tel::CallState callState = (*callIterator)->getCallState();
  //         if(callState != telux::tel::CallState::CALL_ENDED) {
  //            spCall = *callIterator;
  //            break;
  //         }
  //      }
  //      if(spCall) {
  //         std::cout << "Sending request to hangup call " << std::endl;
  //         spCall->hangup(hangupCommandCallback_);
  //      } else {
  //         std::cout << "No active or on-hold call found" << std::endl;
  //      }
  //   } catch(const std::exception &e) {
  //      std::cout << "ERROR: Exception caught -" << e.what();
  //   }
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

void ECallMenu::eCallSos(std::vector<std::string> inputCommand) {
  //   // Get Phone from PhoneFactory
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto spDefaultPhone = phoneFactory.getPhoneManager()->getPhone();
  //
  //   telux::tel::ECallCategory emergencyCategory
  //      = telux::tel::ECallCategory::VOICE_EMER_CAT_AUTO_ECALL;
  //   telux::tel::ECallVariant eCallVariant = telux::tel::ECallVariant::ECALL_EMERGENCY;
  //
  //   MsdSettings msdSettings;
  //   auto eCallMsdData = msdSettings.readMsdFromFile(MSDSETTINGS_FILE);
  //   auto callManager = phoneFactory.getCallManager();
  //   int phoneId = DEFAULT_PHONE_ID;
  //   auto ret = callManager->makeECall(phoneId, eCallMsdData, (int)emergencyCategory,
  //                                     (int)eCallVariant, callCommandCallback_);
  //   std::cout
  //      << (ret == telux::common::Status::SUCCESS ? GREEN + "  ECall request is successful" + DONE
  //                                                : RED + "  ECall request failed" + DONE)
  //      << '\n';
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

/**
 * Sample eCall operation
 */
void ECallMenu::makeECall(std::vector<std::string> inputCommand) {
  //   // Get Phone from PhoneFactory
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto spDefaultPhone = phoneFactory.getPhoneManager()->getPhone();
  //
  //   // Fetch eCall category and variant
  //   std::string category = toLowerCase(inputCommand[1]);
  //   std::string variant = toLowerCase(inputCommand[2]);
  //
  //   telux::tel::ECallCategory emergencyCategory;
  //   telux::tel::ECallVariant eCallVariant;
  //
  //   if(category == ECALL_CATEGORY_AUTO) {  // Automatically triggered eCall.
  //      emergencyCategory = telux::tel::ECallCategory::VOICE_EMER_CAT_AUTO_ECALL;
  //   } else if(category == ECALL_CATEGORY_MANUAL) {  // Manually triggered eCall.
  //      emergencyCategory = telux::tel::ECallCategory::VOICE_EMER_CAT_MANUAL;
  //   } else {
  //      std::cout << "Invalid Emergency Call Category" << std::endl;
  //      return;
  //   }
  //   if(variant == ECALL_VARIANT_TEST) {  // Will use the PSAP number configured in NV settings
  //      eCallVariant = telux::tel::ECallVariant::ECALL_TEST;
  //   } else if(variant
  //             == ECALL_VARIANT_EMERGENCY) {  // Will use the emergency number configured in FDN
  //                                            // i.e. 112.
  //      eCallVariant = telux::tel::ECallVariant::ECALL_EMERGENCY;
  //   } else {
  //      std::cout << "Invalid Emergency Call Variant" << std::endl;
  //      return;
  //   }
  //
  //   MsdSettings msdSettings;
  //   auto eCallMsdData = msdSettings.readMsdFromFile(MSDSETTINGS_FILE);
  //   auto callManager = phoneFactory.getCallManager();
  //   int phoneId = DEFAULT_PHONE_ID;
  //   auto ret = callManager->makeECall(phoneId, eCallMsdData, (int)emergencyCategory,
  //                                     (int)eCallVariant, callCommandCallback_);
  //   std::cout
  //      << (ret == telux::common::Status::SUCCESS ? GREEN + "  ECall request is successful" + DONE
  //                                                : RED + "  ECall request failed" + DONE)
  //      << '\n';
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

/**
 * Sample voice eCall operation
 */
void ECallMenu::makeVoiceECall(std::vector<std::string> inputCommand) {
  //   // Get Phone from PhoneFactory
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto spDefaultPhone = phoneFactory.getPhoneManager()->getPhone();
  //
  //   // Fetch eCall category and variant
  //   std::string category = toLowerCase(inputCommand[1]);
  //   std::string dialNumber = toLowerCase(inputCommand[2]);
  //
  //   telux::tel::ECallCategory emergencyCategory;
  //
  //   if(category == ECALL_CATEGORY_AUTO) {  // Automatically triggered eCall.
  //      emergencyCategory = telux::tel::ECallCategory::VOICE_EMER_CAT_AUTO_ECALL;
  //   } else if(category == ECALL_CATEGORY_MANUAL) {  // Manually triggered eCall.
  //      emergencyCategory = telux::tel::ECallCategory::VOICE_EMER_CAT_MANUAL;
  //   } else {
  //      std::cout << "Invalid Emergency Call Category" << std::endl;
  //      return;
  //   }
  //
  //   MsdSettings msdSettings;
  //   auto eCallMsdData = msdSettings.readMsdFromFile(MSDSETTINGS_FILE);
  //   auto callManager = phoneFactory.getCallManager();
  //   int phoneId = DEFAULT_PHONE_ID;
  //   auto ret = callManager->makeVoiceECall(phoneId, dialNumber, eCallMsdData, (int)emergencyCategory,
  //                                            callCommandCallback_);
  //   std::cout
  //      << (ret == telux::common::Status::SUCCESS ? GREEN + "  ECall request is successful" + DONE
  //                                                : RED + "  ECall request failed" + DONE)
  //      << '\n';
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

/**
 * Sample Update eCall MSD operation
 */
void ECallMenu::updateECallMSD(std::vector<std::string> inputCommand) {
  //   MsdSettings msdSettings;
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto spDefaultPhone = phoneFactory.getPhoneManager()->getPhone();
  //   auto eCallMsdData = msdSettings.readMsdFromFile(UPDATED_MSDSETTINGS_FILE);
  //   auto callManager = phoneFactory.getCallManager();
  //   int phoneId = DEFAULT_PHONE_ID;
  //   auto ret = callManager->updateECallMsd(phoneId, eCallMsdData, updateMsdCommandCallback_);
  //   std::cout << (ret == telux::common::Status::SUCCESS
  //                    ? GREEN + "  Update MSD request is successful" + DONE
  //                    : RED + "  Update MSD request failed" + DONE)
  //             << '\n';
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

void ECallMenu::eCallWithPdu(std::vector<std::string> inputCommand) {
  //   char delimiter = '\n';
  //   std::string category;
  //   std::cout << "Enter category(1 - auto | 2 - manual): ";
  //   std::getline(std::cin, category, delimiter);
  //   int opt1 = -1;
  //   if(!category.empty()) {
  //      try {
  //         opt1 = std::stoi(category);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << opt1 << std::endl;
  //      }
  //   } else {
  //      std::cout << "Empty input going with default auto category\n";
  //      opt1 = CATEGORY_AUTO;
  //   }
  //   std::string variant;
  //   std::cout << "Enter variant(1 - test | 2 - emergency | 3 - voice): ";
  //   std::getline(std::cin, variant, delimiter);
  //   int opt2 = -1;
  //   if(!variant.empty()) {
  //      try {
  //         opt2 = std::stoi(variant);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << opt2 << std::endl;
  //      }
  //   } else {
  //      std::cout << "Empty input going with default Emergency variant\n";
  //      opt2 = VARIANT_EMERGENCY;
  //   }
  //   // Get Phone from PhoneFactory
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto spDefaultPhone = phoneFactory.getPhoneManager()->getPhone();
  //
  //   telux::tel::ECallCategory emergencyCategory;
  //   telux::tel::ECallVariant eCallVariant;
  //   std::string dialNumber = std::string();
  //
  //   if(opt1 == CATEGORY_AUTO) {  // Automatically triggered eCall.
  //      emergencyCategory = telux::tel::ECallCategory::VOICE_EMER_CAT_AUTO_ECALL;
  //   } else if(opt1 == CATEGORY_MANUAL) {  // Manually triggered eCall.
  //      emergencyCategory = telux::tel::ECallCategory::VOICE_EMER_CAT_MANUAL;
  //   } else {
  //      std::cout << "Invalid Emergency Call Category" << std::endl;
  //      return;
  //   }
  //
  //   if(opt2 == VARIANT_TEST) {  // Will use the PSAP number configured in NV settings
  //      eCallVariant = telux::tel::ECallVariant::ECALL_TEST;
  //   } else if(opt2 == VARIANT_EMERGENCY) {  // Will use the emergency number configured in FDN
  //                                           // i.e. 112.
  //      eCallVariant = telux::tel::ECallVariant::ECALL_EMERGENCY;
  //   } else if(opt2 == VARIANT_VOICE) {  // Will use the emergency number provided by user
  //      eCallVariant = telux::tel::ECallVariant::ECALL_VOICE;
  //      std::cout << "Enter the phone number : ";
  //      std::getline(std::cin, dialNumber, delimiter);
  //   } else {
  //      std::cout << "Invalid Emergency Call Variant" << std::endl;
  //      return;
  //   }
  //
  //   auto callManager = phoneFactory.getCallManager();
  //   int phoneId = DEFAULT_PHONE_ID;
  //
  //   std::string msdData;
  //   std::cout << "Enter MSD PDU: ";
  //   std::getline(std::cin, msdData, delimiter);
  //   std::vector<uint8_t> rawData;
  //
  //   if(!msdData.empty()) {
  //      rawData = convertHexToBytes(msdData);
  //   } else {
  //      rawData = {2,   41,  68, 6,  128, 227, 10, 81,  67, 158, 41,  85,  212, 56,  0,
  //                 128, 4,   52, 10, 140, 65,  89, 164, 56, 119, 207, 131, 54,  210, 63,
  //                 65,  104, 16, 24, 8,   32,  19, 198, 68, 0,   0,   48,  20};
  //   }
  //   telux::common::Status ret;
  //   if(eCallVariant != telux::tel::ECallVariant::ECALL_VOICE) {
  //      ret = callManager->makeECall(phoneId, rawData, (int)emergencyCategory, (int)eCallVariant,
  //                                     &makeEcallResponse);
  //   } else {
  //      ret = callManager->makeVoiceECall(phoneId, dialNumber, rawData, (int)emergencyCategory,
  //                                     &makeEcallResponse);
  //   }
  //   std::cout
  //      << (ret == telux::common::Status::SUCCESS ? GREEN + "  ECall request is successful" + DONE
  //                                                : RED + "  ECall request failed" + DONE)
  //      << '\n';
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

void ECallMenu::updateEcallMsdWithPdu(std::vector<std::string> userInput) {
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   auto spDefaultPhone = phoneFactory.getPhoneManager()->getPhone();
  //   auto callManager = phoneFactory.getCallManager();
  //   int phoneId = DEFAULT_PHONE_ID;
  //   char delimiter = '\n';
  //   std::string msdData;
  //   std::cout << "Enter raw msd: ";
  //   std::getline(std::cin, msdData, delimiter);
  //   std::vector<uint8_t> rawData;
  //
  //   if(!msdData.empty()) {
  //      rawData = convertHexToBytes(msdData);
  //   } else {
  //      rawData = {2,   41,  68, 6,  128, 227, 10, 81,  67, 158, 41,  85,  212, 56,  0,
  //                 128, 4,   52, 10, 140, 65,  89, 164, 56, 119, 207, 131, 54,  210, 63,
  //                 65,  104, 16, 24, 8,   32,  19, 198, 68, 0,   0,   48,  20};
  //   }
  //
  //   auto ret = callManager->updateECallMsd(phoneId, rawData, &updateEcallResponse);
  //   std::cout << (ret == telux::common::Status::SUCCESS
  //                    ? GREEN + "  Update MSD request is successful" + DONE
  //                    : RED + "  Update MSD request failed" + DONE)
  //             << '\n';
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

/**
 * Sample get in progress calls operations
 */
void ECallMenu::getCalls(std::vector<std::string> inputCommand) {
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   std::vector<std::shared_ptr<telux::tel::ICall>> callList
  //      = phoneFactory.getCallManager()->getInProgressCalls();
  //   if(callList.size() == 0) {
  //      std::cout << "No calls detected in the system" << std::endl;
  //   } else {
  //      for(auto call : callList) {
  //         std::cout << getCallDescription(call) << std::endl;
  //      }
  //   }
  for (auto& arg : inputCommand) std::cout << __func__ << ":" << arg << std::endl;
}

/**
 * Get a human readable description of this call - such as phone numbers, call state
 * Useful for display or debugging
 */
// std::string ECallMenu::getCallDescription(std::shared_ptr<telux::tel::ICall> call) {
//   std::string callDesc;
//   callDesc += "Call Index: " + std::to_string(call->getCallIndex()) + ", ";
//   callDesc += "Phone Number: " + call->getRemotePartyNumber() + ", Call State: ";
//   callDesc += std::to_string(static_cast<int>(call->getCallState())) + ", Call Type: "
//               + std::to_string(static_cast<int>(call->getCallDirection()));
//   return callDesc;
//    for(auto &arg:userInput)
//    std::cout<<__func__<<":"<<arg<<std::endl;
//
//}

/**
 * This method is useful to trim the spaces in options and converting them to lower case
 */
std::string ECallMenu::toLowerCase(std::string inputOption) {
  std::string convertedString;
  convertedString =
      inputOption.erase(0, inputOption.find_first_not_of(" \n\r\t"));  // trim std::string
  std::transform(convertedString.begin(), convertedString.end(), convertedString.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return convertedString;
}

// void ECallMenu::CallCommandCallback::makeCallResponse(telux::common::ErrorCode errorCode,
//                                                      std::shared_ptr<telux::tel::ICall> call) {
//   std::string infoStr = "";
//   if(errorCode == telux::common::ErrorCode::SUCCESS) {
//      infoStr.append("Call is successful ");
//   } else {
//      infoStr.append("Call failed with error code: " + std::to_string(static_cast<int>(errorCode))
//                     + ":" + Utils::getErrorCodeAsString(errorCode));
//   }
//
//   PRINT_NOTIFICATION << infoStr << std::endl;
//}
//
// void ECallMenu::UpdateMsdCommandCallback::commandResponse(telux::common::ErrorCode errorCode) {
//   std::string infoStr = "";
//   if(errorCode == telux::common::ErrorCode::SUCCESS) {
//      infoStr.append(" MSD Update is successful");
//   } else {
//      infoStr.append("Update MSD failed with error code: "
//                     + std::to_string(static_cast<int>(errorCode)) + ":"
//                     + Utils::getErrorCodeAsString(errorCode));
//   }
//   PRINT_NOTIFICATION << infoStr << std::endl;
//}
//
// void ECallMenu::HangupCommandCallback::commandResponse(telux::common::ErrorCode errorCode) {
//   std::string infoStr = "";
//   if(errorCode == telux::common::ErrorCode::SUCCESS) {
//      infoStr.append(" Hangup is successful");
//   } else {
//      infoStr.append(" Hangup failed with error code: "
//                     + std::to_string(static_cast<int>(errorCode)) + ":"
//                     + Utils::getErrorCodeAsString(errorCode));
//   }
//   PRINT_NOTIFICATION << infoStr << std::endl;
//}
//
// void ECallMenu::AnswerCommandCallback::commandResponse(telux::common::ErrorCode errorCode) {
//   std::string infoStr = "";
//   if(errorCode == telux::common::ErrorCode::SUCCESS) {
//      infoStr.append(" Answer Call is successful");
//   } else {
//      infoStr.append(" Answer call failed with error code: "
//                     + std::to_string(static_cast<int>(errorCode)) + ":"
//                     + Utils::getErrorCodeAsString(errorCode));
//   }
//   PRINT_NOTIFICATION << infoStr << std::endl;
//}
/** Convert the hexadecimal string to bytes
 *  Eg: i/p: 0229440680E30A51439E
 *      o/p: 2,41,68,6,128,227,10,81,67,158
 */
std::vector<uint8_t> ECallMenu::convertHexToBytes(std::string msdData) {
  std::vector<uint8_t> rawMsd;
  size_t i, len;
  uint8_t rawData1 = 0, rawData2 = 0, rawData = 0;

  len = msdData.length();
  for (i = 0; i < len; i = i + 2) {
    if (msdData[i] >= '0' && msdData[i] <= '9') {
      rawData1 = (msdData[i] - 48) * 16;
    } else if (msdData[i] >= 'A' && msdData[i] <= 'F') {
      rawData1 = (msdData[i] - 55) * 16;
    } else if (msdData[i] >= 'a' && msdData[i] <= 'f') {
      rawData1 = (msdData[i] - 87) * 16;
    }

    if (msdData[i + 1] >= '0' && msdData[i + 1] <= '9') {
      rawData2 = msdData[i + 1] - 48;
    } else if (msdData[i + 1] >= 'A' && msdData[i + 1] <= 'F') {
      rawData2 = msdData[i + 1] - 55;
    } else if (msdData[i + 1] >= 'a' && msdData[i + 1] <= 'f') {
      rawData2 = msdData[i + 1] - 87;
    }

    rawData = rawData1 + rawData2;
    rawMsd.emplace_back(rawData);
  }
  return rawMsd;
}
