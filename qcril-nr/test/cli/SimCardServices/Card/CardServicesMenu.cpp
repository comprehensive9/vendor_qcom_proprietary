/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2019, The Linux Foundation. All rights reserved
 */

/**
 * CardServicesMenu provides menu options to invoke Card Services such as Transmit APDU.
 */

#include <chrono>
#include <iostream>
//#include "Utils.hpp"
//#include <telux/tel/PhoneFactory.hpp>

#include "CardServicesMenu.hpp"

#define PRINT_CB std::cout << "\033[1;35mCallback: \033[0m"

// void ChangeCardPinResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Change Card Pin Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Change Card Pin Request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void unlockCardByPinResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Unlock Card By Pin Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Unlock Card By Pin Request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void unlockCardByPukResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Unlock Card By Puk Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Unlock Card By Puk request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void setCardLockResponseCb(int retryCount, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Set Card Lock Request failed with errorCode: " << static_cast<int>(error) << ":"
//               << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Set Card Lock request successful retryCount:" << retryCount << std::endl;
//   }
//}
//
// void queryFdnLockResponseCb(bool isAvailable, bool isEnabled, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Query FDN lock state request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Query FDN lock state is successful" << std::endl;
//      if(isAvailable) {
//         PRINT_CB << "FDN lock is available and ";
//         if(isEnabled) {
//            std::cout << "enabled" << std::endl;
//         } else {
//            std::cout << "disabled" << std::endl;
//         }
//      } else {
//         PRINT_CB << "FDN lock not available" << std::endl;
//      }
//   }
//}
//
// void queryPin1LockResponseCb(bool state, telux::common::ErrorCode error) {
//   if(error != telux::common::ErrorCode::SUCCESS) {
//      PRINT_CB << "Query Pin1 Lock Request failed with errorCode: " << static_cast<int>(error)
//               << ":" << Utils::getErrorCodeAsString(error) << std::endl;
//   } else {
//      PRINT_CB << "Query Pin1 Lock Request successful state:" << state << std::endl;
//   }
//}

CardServicesMenu::CardServicesMenu(std::string appName, std::string cursor,
                                   RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();
  //  Get the PhoneFactory and PhoneManager instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   cardManager_ = phoneFactory.getCardManager();
  //
  //   //  Check if telephony subsystem is ready
  //   bool subSystemStatus = cardManager_->isSubsystemReady();
  bool subSystemStatus = true;
  //   //  If telephony subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::cout << "Telephony subsystem is not ready, Please wait" << std::endl;
  //      std::future<bool> f = cardManager_->onSubsystemReady();
  //      // If we want to wait unconditionally for telephony subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  //   //  Exit the application, if SDK is unable to initialize telephony subsystems
  //   if(subSystemStatus) {
  //      endTime = std::chrono::system_clock::now();
  //      std::chrono::duration<double> elapsedTime = endTime - startTime;
  //      std::cout << "Elapsed Time for Subsystems to ready : " << elapsedTime.count() << "s\n"
  //                << std::endl;
  //   } else {
  //      std::cout << "ERROR - Unable to initialize subSystem" << std::endl;
  //      exit(0);
  //   }
  //
  //   if(subSystemStatus) {
  //      std::vector<int> slotIds;
  //      cardManager_->getSlotIds(slotIds);
  //
  //      // get the default card object
  //      card_ = cardManager_->getCard();
  //
  //      // listener
  //      cardListener_ = std::make_shared<MyCardListener>();
  //
  //      // callbacks
  //      myOpenLogicalChannelCb_ = std::make_shared<MyOpenLogicalChannelCallback>();
  //      myTransmitApduCb_ = std::make_shared<MyTransmitApduResponseCallback>();
  //      myCloseLogicalChannelCb_ = std::make_shared<MyCardCommandResponseCallback>();
  //
  //      // registering Listener
  //      telux::common::Status status = cardManager_->registerListener(cardListener_);
  //      if(status != telux::common::Status::SUCCESS) {
  //         std::cout << "Unable to registerListener" << std::endl;
  //      }
  //   }
}

CardServicesMenu::~CardServicesMenu() {
  //   if(cardListener_) {
  //      cardManager_->removeListener(cardListener_);
  //      cardListener_ = nullptr;
  //   }
  //   myOpenLogicalChannelCb_ = nullptr;
  //   myTransmitApduCb_ = nullptr;
  //   myCloseLogicalChannelCb_ = nullptr;
}

void CardServicesMenu::init() {
  std::shared_ptr<ConsoleAppCommand> getCardStateCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("1", "Get_card_state", {},
                        std::bind(&CardServicesMenu::getCardState, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> getSupportedAppsCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "2", "Get_supported_apps", {},
          std::bind(&CardServicesMenu::getSupportedApps, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> openLogicalChannelCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "3", "Open_logical_channel", { "aid" ,"p2"},
          std::bind(&CardServicesMenu::openLogicalChannel, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> closeLogicalChannelCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "4", "Close_logical_channel", { "channel" },
          std::bind(&CardServicesMenu::closeLogicalChannel, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> transmitApduCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("5", "Transmit_APDU", {"session-id","cla","ins(Hexvalue)","p1","p2","p3","data"},
                        std::bind(&CardServicesMenu::transmitApdu, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> basicTransmitApduCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "6", "Basic_transmit_APDU", {"session-id","cla","ins(Hexvalue)","p1","p2","p3","data"},
          std::bind(&CardServicesMenu::basicTransmitApdu, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> changeCardPinCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("7", "Change_card_pin", {"newpin","oldpin","aid"},
                        std::bind(&CardServicesMenu::changeCardPin, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> changeCardPin2Command = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("8", "Change_card_pin2", {"newpin","oldpin","aid"},
                        std::bind(&CardServicesMenu::changeCardPin2, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> unlockCardByPinCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("9", "Unlock_card_by_pin", {"pin","aid"},
                        std::bind(&CardServicesMenu::unlockCardByPin, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> unlockCardByPin2Command = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("10", "Unlock_card_by_pin2", {"pin2","aid"},
                        std::bind(&CardServicesMenu::unlockCardByPin2, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> unlockCardByPukCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("11", "Unlock_card_by_puk", {"puk code","pin","aid"},
                        std::bind(&CardServicesMenu::unlockCardByPuk, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> unlockCardByPuk2Command = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("12", "Unlock_card_by_puk2", {"pukcode","pin2","aid"},
                        std::bind(&CardServicesMenu::unlockCardByPuk2, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> queryPin1LockStateCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "13", "Query_pin1_lockState", {},
          std::bind(&CardServicesMenu::queryPin1LockState, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> queryFdnLockStateCommand =
      std::make_shared<ConsoleAppCommand>(ConsoleAppCommand(
          "14", "Query_fdn_lockState", {"facility","password","service-class","aid"},
          std::bind(&CardServicesMenu::queryFdnLockState, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> setCardLockCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("15", "Set_card_lock", {"facility","lock_status","password","service-class","aid"},
                        std::bind(&CardServicesMenu::setCardLock, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> GetIMSICommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("16", "Get_imsi_command", {"aid"},
                        std::bind(&CardServicesMenu::getImsirequest, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> SimIOReq = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("17", "Sim_Io_req", {"command(Hexvalue)","fileid(Hexvalue)","path","p1","p2","p3","data","pin2","aidPtr"},
                        std::bind(&CardServicesMenu::setSimIoReq, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> EnvelopeReq = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("18", "Envelope_Req", {"command"},
                        std::bind(&CardServicesMenu::enveloperequest, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand> EnvelopeCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("19", "Envelope_command", {"command"},
                        std::bind(&CardServicesMenu::envelopecommand, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand>TerminalRespCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("20", "TerminalResp_command", {"command"},
                        std::bind(&CardServicesMenu::Terminalresp_command, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand>CallSetupRequest = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("21", "CallSetupRequest_command", {"command(hexvalue)"},
                        std::bind(&CardServicesMenu::CallSetup_command, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand>ServiceIsRunning = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("22", "ServiceIsRunning_command", {"command"},
                        std::bind(&CardServicesMenu::ServiceRunning_command, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand>IsimAuthenticate = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("23", "IsimAuth_command", {"command"},
                        std::bind(&CardServicesMenu::IsimAuthenticate_command, this, std::placeholders::_1)));
  std::shared_ptr<ConsoleAppCommand>simAuthenticate = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("24", "simAuth_command", {"authContext(Hexvalue)","authData","aid"},
                        std::bind(&CardServicesMenu::simAuthenticate_command, this, std::placeholders::_1)));
 std::shared_ptr<ConsoleAppCommand>Networkdeperso = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("25", "NetworkDeperso_command", {"command"},
                        std::bind(&CardServicesMenu::NwDeperso_command, this, std::placeholders::_1)));
 std::shared_ptr<ConsoleAppCommand>Unsol_Registration = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("26", "Unsol_Registration", {},
                        std::bind(&CardServicesMenu::registerForIndications, this, std::placeholders::_1)));
 std::shared_ptr<ConsoleAppCommand>Get_atr = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("27", "Get_atr", {"slot-id"},
                        std::bind(&CardServicesMenu::GetAtr, this, std::placeholders::_1)));
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListCardServicesSubMenu = {
    getCardStateCommand,        getSupportedAppsCommand,  openLogicalChannelCommand,
    closeLogicalChannelCommand, transmitApduCommand,      basicTransmitApduCommand,
    changeCardPinCommand,       changeCardPin2Command,    unlockCardByPinCommand,
    unlockCardByPin2Command,    unlockCardByPukCommand,   unlockCardByPuk2Command,
    queryPin1LockStateCommand,  queryFdnLockStateCommand, setCardLockCommand,
    GetIMSICommand,    SimIOReq , EnvelopeReq ,EnvelopeCommand,
    TerminalRespCommand ,CallSetupRequest ,ServiceIsRunning,
    IsimAuthenticate,simAuthenticate,Networkdeperso,Unsol_Registration,Get_atr
  };
  addCommands(commandsListCardServicesSubMenu);
  ConsoleApp::displayMenu();
}

// std::string CardServicesMenu::cardStateToString(telux::tel::CardState state) {
//   std::string cardState;
//   switch(state) {
//      case telux::tel::CardState::CARDSTATE_ABSENT:
//         cardState = "Absent";
//         break;
//      case telux::tel::CardState::CARDSTATE_PRESENT:
//         cardState = "Present";
//         break;
//      case telux::tel::CardState::CARDSTATE_ERROR:
//         cardState = "Either error or absent";
//         break;
//      case telux::tel::CardState::CARDSTATE_RESTRICTED:
//         cardState = "Restricted";
//         break;
//      default:
//         cardState = "Unknown card state";
//         break;
//   }
//   return cardState;
//}

void CardServicesMenu::getCardState(std::vector<std::string> userInput) {
  //   if(card_) {
  //      telux::tel::CardState cardState;
  //      card_->getState(cardState);
  //      std::cout << "CardState : " << cardStateToString(cardState) << std::endl;
  //   }
  Status s = rilSession.simGetSimStatusReq([](RIL_Errno err,const RIL_CardStatus_v6 *cardStatusInfo) -> void {
    std::cout << "Got response for get card state " << err << std::endl;
    if(cardStatusInfo != nullptr)
    {
     std::cout << "cardStatusInfo.card_state "<<cardStatusInfo->card_state<<std::endl;
     std::cout << "cardStatusInfo.physical_slot_id "<<cardStatusInfo->physical_slot_id<<std::endl;
     if(cardStatusInfo->atr && cardStatusInfo->iccid) {
         std::cout << "cardStatusInfo.atr "<<cardStatusInfo->atr <<std::endl;
         std::cout << "cardStatusInfo.iccid "<<cardStatusInfo->iccid <<std::endl;
     }
     std::cout << "cardStatusInfo.universal_pin_state "<<cardStatusInfo->universal_pin_state <<std::endl;
     std::cout << "cardStatusInfo.gsm_umts_subscription_app_index "<<cardStatusInfo->gsm_umts_subscription_app_index <<std::endl;
     std::cout << "cardStatusInfo.cdma_subscription_app_index "<<cardStatusInfo->cdma_subscription_app_index <<std::endl;
     std::cout << "cardStatusInfo.ims_subscription_app_index "<<cardStatusInfo->ims_subscription_app_index <<std::endl;
     std::cout << "cardStatusInfo.num_applications "<<cardStatusInfo->num_applications <<std::endl;
     for(uint32_t cnt=0 ;cnt< cardStatusInfo->num_applications;cnt++)
     {
         std::cout << "cardStatusInfo.applications[cnt].app_type "<<cardStatusInfo->applications[cnt].app_type <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].app_state "<<cardStatusInfo->applications[cnt].app_state <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].perso_substate "<<cardStatusInfo->applications[cnt].perso_substate <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].aid_ptr "<<cardStatusInfo->applications[cnt].aid_ptr <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].app_label_ptr "<<cardStatusInfo->applications[cnt].app_label_ptr <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].pin1_replaced "<<cardStatusInfo->applications[cnt].pin1_replaced <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].pin1 "<<cardStatusInfo->applications[cnt].pin1 <<std::endl;
         std::cout << "cardStatusInfo.applications[cnt].pin2 "<<cardStatusInfo->applications[cnt].pin2<<std::endl;

     }
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sim status response succesfully"
                                       : "Sim status response received with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

// std::string CardServicesMenu::appTypeToString(telux::tel::AppType appType) {
//   std::string applicationType;
//   switch(appType) {
//      case telux::tel::AppType::APPTYPE_SIM:
//         applicationType = "SIM";
//         break;
//      case telux::tel::AppType::APPTYPE_USIM:
//         applicationType = "USIM";
//         break;
//      case telux::tel::AppType::APPTYPE_RUIM:
//         applicationType = "RUIM";
//         break;
//      case telux::tel::AppType::APPTYPE_CSIM:
//         applicationType = "CSIM";
//         break;
//      case telux::tel::AppType::APPTYPE_ISIM:
//         applicationType = "ISIM";
//         break;
//      default:
//         applicationType = "Unknown";
//         break;
//   }
//   return applicationType;
//}
//
// std::string CardServicesMenu::appStateToString(telux::tel::AppState appState) {
//   std::string applicationState;
//   switch(appState) {
//      case telux::tel::AppState::APPSTATE_DETECTED:
//         applicationState = "Detected";
//         break;
//      case telux::tel::AppState::APPSTATE_PIN:
//         applicationState = "PIN";
//         break;
//      case telux::tel::AppState::APPSTATE_PUK:
//         applicationState = "PUK";
//         break;
//      case telux::tel::AppState::APPSTATE_SUBSCRIPTION_PERSO:
//         applicationState = "Subscription Perso";
//         break;
//      case telux::tel::AppState::APPSTATE_READY:
//         applicationState = "Ready";
//         break;
//      default:
//         applicationState = "Unknown";
//         break;
//   }
//   return applicationState;
//}

void CardServicesMenu::getSupportedApps(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //      applications = card_->getApplications();
  //      if(applications.size() != 0)  {
  //         for(auto cardApp : applications) {
  //            std::cout << "App type: " << appTypeToString(cardApp->getAppType()) << std::endl;
  //            std::cout << "App state: " << appStateToString(cardApp->getAppState()) << std::endl;
  //            std::cout << "AppId : " << cardApp->getAppId() << std::endl;
  //         }
  //      } else {
  //         std::cout <<"No supported applications"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
//  Status s = rilSession.simGetSimStatusReq([](RIL_Errno err) -> void {
//    std::cout << "Got response for get card state " << err << std::endl;
//  });
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::openLogicalChannel(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::string aid = userInput[1];
  //      std::cout << "Open logical channel with aid:" << aid << std::endl;
  //      auto ret = card_->openLogicalChannel(aid, myOpenLogicalChannelCb_);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "Open logical channel request sent successfully \n";
  //      } else {
  //         std::cout << "Open logical channel request failed \n";
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }

  RIL_OpenChannelParams open_chnl;

  open_chnl.aidPtr =  const_cast<char *>(userInput[1].c_str());
  open_chnl.p2      = std::stoi(userInput[2]);
  Status s = rilSession.simOpenChannelReq(open_chnl, [](RIL_Errno err ,const int *aid ,size_t len) -> void {
    std::cout << "Got response for open channel: " << err << std::endl;
    if(aid != nullptr)
    {
       std::cout << "Response Aid " << std::to_string(*aid) << std::endl;
       std::cout << "Response len " << len << std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "open channel response  success"
                                       : "open channel response failed  with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::transmitApdu(std::vector<std::string> userInput) {
  //   if(card_) {
  //      int channel;
  //      int cla, instruction, p1, p2, p3;
  //      std::vector<uint8_t> data;
  //
  //      cla = 0;
  //      instruction = 0;
  //      p1 = 0;
  //      p2 = 0;
  //      p3 = 0;
  //
  //      std::cout << std::endl;
  //      std::cout << "Enter the channel : ";
  //      std::cin >> channel;
  //      Utils::validateInput(channel);
  //      std::cout << "Enter CLA : ";
  //      std::cin >> cla;
  //      Utils::validateInput(cla);
  //      std::cout << "Enter INS : ";
  //      std::cin >> instruction;
  //      Utils::validateInput(instruction);
  //      std::cout << "Enter P1 : ";
  //      std::cin >> p1;
  //      Utils::validateInput(p1);
  //      std::cout << "Enter P2 : ";
  //      std::cin >> p2;
  //      Utils::validateInput(p2);
  //      std::cout << "Enter P3 : ";
  //      std::cin >> p3;
  //      Utils::validateInput(p3);
  //      int dataInput;
  //      for(int i = 0; i < p3; i++) {
  //         std::cout << "Enter DATA (" << i + 1 << ") :";
  //         std::cin >> dataInput;
  //         Utils::validateInput(dataInput);
  //         data.emplace_back((uint8_t)dataInput);
  //      }
  //
  //      auto ret = card_->transmitApduLogicalChannel(channel, (uint8_t)cla, (uint8_t)instruction,
  //                                                   (uint8_t)p1, (uint8_t)p2, (uint8_t)p3, data,
  //                                                   myTransmitApduCb_);
  //      std::cout << (ret == telux::common::Status::SUCCESS ?"Transmit APDU request sent successfully"
  //                                                          : "Transmit APDU request failed")
  //                << '\n';
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
  RIL_SIM_APDU sim_apdu;
  sim_apdu.sessionid = std::stoi(userInput[1]);
  sim_apdu.cla       = std::stoi(userInput[2]);
  sim_apdu.instruction = std::stoi(userInput[3],NULL,16);
  sim_apdu.p1          = std::stoi(userInput[4]);
  sim_apdu.p2        = std::stoi(userInput[5]);
  sim_apdu.p3        = std::stoi(userInput[6]);
  sim_apdu.data      = const_cast<char *>(userInput[7].c_str());
  Status s = rilSession.simTransmitApduChannelReq(sim_apdu, [](RIL_Errno err, const RIL_SIM_IO_Response *res) -> void {
    std::cout << "Got response for Transmit apdu channel: " << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1"<<res->sw1;
          std::cout<<"Statusword2"<<res->sw2;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse"<<std::string(res->simResponse)<<std::endl;
          }
      }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Transmit apdu response success"
                                       : "Received Transmit apdu response failed")
            << std::endl;

  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::basicTransmitApdu(std::vector<std::string> userInput) {
  //   if(card_) {
  //      int cla, instruction, p1, p2, p3;
  //      std::vector<uint8_t> data;
  //
  //      cla = 0;
  //      instruction = 0;
  //      p1 = 0;
  //      p2 = 0;
  //      p3 = 0;
  //
  //      std::string user_input;
  //      std::cout << std::endl;
  //      std::cout << "Enter CLA : ";
  //      std::cin >> cla;
  //      Utils::validateInput(cla);
  //      std::cout << "Enter INS : ";
  //      std::cin >> instruction;
  //      Utils::validateInput(instruction);
  //      std::cout << "Enter P1 : ";
  //      std::cin >> p1;
  //      Utils::validateInput(p1);
  //      std::cout << "Enter P2 : ";
  //      std::cin >> p2;
  //      Utils::validateInput(p2);
  //      std::cout << "Enter P3 : ";
  //      std::cin >> p3;
  //      Utils::validateInput(p3);
  //      int tmpInp;
  //      for(int i = 0; i < p3; i++) {
  //         std::cout << "Enter DATA (" << i + 1 << ") :";
  //         std::cin >> tmpInp;
  //         Utils::validateInput(tmpInp);
  //         data.emplace_back((uint8_t)tmpInp);
  //      }
  //      auto ret = card_->transmitApduBasicChannel((uint8_t)cla, (uint8_t)instruction, (uint8_t)p1,
  //                                                 (uint8_t)p2, (uint8_t)p3, data, myTransmitApduCb_);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "Basic transmit APDU request sent successfully\n";
  //      } else {
  //         std::cout << "Basic transmit APDU request failed\n";
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }

  RIL_SIM_APDU sim_apdu;
  sim_apdu.sessionid = std::stoi(userInput[1]);
  sim_apdu.cla       = std::stoi(userInput[2]);
  sim_apdu.instruction = std::stoi(userInput[3],NULL,16);
  sim_apdu.p1          = std::stoi(userInput[4]);
  sim_apdu.p2        = std::stoi(userInput[5]);
  sim_apdu.p3        = std::stoi(userInput[6]);
  sim_apdu.data      = const_cast<char *>(userInput[7].c_str());
  Status s = rilSession.simTransmitApduBasicReq(sim_apdu, [](RIL_Errno err,const RIL_SIM_IO_Response *res) -> void {
    std::cout << "Got response for basic transmit apdu: "  << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1"<<res->sw1;
          std::cout<<"Statusword2"<<res->sw2;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse"<<std::string(res->simResponse)<<std::endl;
          }
      }
  });

  std::cout << ((s == Status::SUCCESS) ? "Received basic transmit apdu response success"
                                       : "Received Failure response for transmit apdu")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::closeLogicalChannel(std::vector<std::string> userInput) {
  //   if(card_) {
  //      int channel = std::stoi(userInput[1]);
  //      std::cout << "Close logical channel with channel:" << channel << std::endl;
  //      auto ret = card_->closeLogicalChannel(channel, myCloseLogicalChannelCb_);
  //      if(ret == telux::common::Status::SUCCESS) {
  //         std::cout << "Close logical channel request sent successfully \n";
  //      } else {
  //         std::cout << "Close logical channel request failed \n";
  //      }
  //   }  else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
  int session_id;
  session_id = std::stoi(userInput[1]);
  Status s = rilSession.simCloseChannelReq(session_id, [](RIL_Errno err) -> void {
    std::cout << "Got response for close channel: " << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Success response for close channel"
                                       : "Received Failure response for close channel")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::changeCardPin(std::vector<std::string> userInput) {
  //   std::string oldPin, newPin, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter 1-PIN1/2-PIN2 : ";
  //   std::getline(std::cin, lockType, delimiter);
  //   // Validating user input
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN2;
  //   } else {
  //      std::cout << "wrong input, going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //   std::cout << "Enter old PIN: ";
  //   std::getline(std::cin, oldPin, delimiter);
  //
  //   std::cout << "Enter new PIN: ";
  //   std::getline(std::cin, newPin, delimiter);
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if((cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM)
  //            && (cardApp->getAppState() == telux::tel::AppState::APPSTATE_READY)) {
  //            auto ret
  //               = cardApp->changeCardPassword(cardLockType, oldPin, newPin,
  //                  &ChangeCardPinResponseCb);
  //            if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Change card PIN request sent successfully\n";
  //            } else {
  //               std::cout << "Change card PIN request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Change card PIN request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }

  const char *new_pinp[3] = {};
  new_pinp[0] =const_cast<char*>(userInput[1].c_str());
  new_pinp[1] =const_cast<char*>(userInput[2].c_str());
  new_pinp[2] =const_cast<char*>(userInput[3].c_str());
  Status s = rilSession.simChangePinReq(new_pinp, [](RIL_Errno err, int *res) -> void {
    std::cout << "Got response for change pin : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Change Pin "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received change pin response succesfully"
                                       : "change pin response received with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::changeCardPin2(std::vector<std::string> userInput) {

  const char *new_pinp[3] = {};
  new_pinp[0] =const_cast<char*>(userInput[1].c_str());
  new_pinp[1] =const_cast<char*>(userInput[2].c_str());
  new_pinp[2] =const_cast<char*>(userInput[3].c_str());
  Status s = rilSession.simChangePin2Req(new_pinp, [](RIL_Errno err, int *res) -> void {
    std::cout << "Got response for change pin2 : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Change Pin2 "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received change pin2 response succesfully"
                                       : "change pin2 response received with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::unlockCardByPuk(std::vector<std::string> userInput) {
  //   std::string puk, newPin, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter 1-PUK1/2-PUK2 : ";
  //   std::getline(std::cin, lockType, delimiter);
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PUK1\n";
  //      cardLockType = telux::tel::CardLockType::PUK1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PUK1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::PUK2;
  //   } else {
  //      std::cout << "As the user entered wrong input, setting default lock type value 'PUK1'\n";
  //      cardLockType = telux::tel::CardLockType::PUK1;
  //   }
  //   std::cout << "Enter PUK: ";
  //   std::getline(std::cin, puk, delimiter);
  //
  //   std::cout << "Enter New PIN: ";
  //   std::getline(std::cin, newPin, delimiter);
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if((cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM)
  //            && (cardApp->getAppState() == telux::tel::AppState::APPSTATE_PUK)) {
  //            auto ret = cardApp->unlockCardByPuk(cardLockType, puk, newPin,
  //               &unlockCardByPukResponseCb);
  //            if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Unlock card by PUK request sent successfully\n";
  //            } else {
  //               std::cout << "Unlock card by PUK request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Unlock card by PUK request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }
  const char *new_puk[3] = {};
  new_puk[0] = const_cast<char*>(userInput[1].c_str());
  new_puk[1] = const_cast<char*>(userInput[2].c_str());
  new_puk[2] = const_cast<char*>(userInput[3].c_str());
  Status s = rilSession.simEnterPukReq(new_puk, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for enter puk req : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Enter Puk "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received unblock card by puk response succesfully"
                                       : "Unblock card by puk response received with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::unlockCardByPuk2(std::vector<std::string> userInput) {
  const char *new_puk[3] = {};
  new_puk[0] = const_cast<char*>(userInput[1].c_str());
  new_puk[1] = const_cast<char*>(userInput[2].c_str());
  new_puk[2] = const_cast<char*>(userInput[3].c_str());
  Status s = rilSession.simEnterPuk2Req(new_puk, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for enter puk2 : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Enter Puk2 "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received unblock card by puk2 response succesfully"
                                       : "Unblock card by puk2 response received with error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::unlockCardByPin(std::vector<std::string> userInput) {
  //   std::string newPin, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter the 1-PIN1/2-PIN2: ";
  //   std::getline(std::cin, lockType, delimiter);
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN2;
  //   } else {
  //      std::cout << "As the user entered wrong option, setting default lock type value 'PIN1'\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   std::cout << "Enter PIN: ";
  //   std::getline(std::cin, newPin, delimiter);
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //            auto ret = cardApp->unlockCardByPin(cardLockType, newPin,
  //            &unlockCardByPinResponseCb); if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Unlock card by pin request sent successfully\n";
  //            } else {
  //               std::cout << "Unlock card by pin request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Unlock card by PIN request failed\n"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }
  const char *unblock_pin[2] = {};
  unblock_pin[0] = const_cast<char*>(userInput[1].c_str());
  unblock_pin[1] = const_cast<char*>(userInput[2].c_str());
  Status s = rilSession.simEnterPinReq(unblock_pin, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for unblock pin : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Unblock Pin "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for unblock_pin"
                                       : "Received Failure response for unblock_pin")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::unlockCardByPin2(std::vector<std::string> userInput) {
  const char *unblock_pin2[2] = {};
  unblock_pin2[0] = const_cast<char*>(userInput[1].c_str());
  unblock_pin2[1] = const_cast<char*>(userInput[2].c_str());
  Status s = rilSession.simEnterPin2Req(unblock_pin2, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for unblock pin2 : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for Unblock Pin2 "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for unblock_pin2"
                                       : "Received Failure response for unblock_pin2")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::queryPin1LockState(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //      applications = card_->getApplications();
  //      if(applications.size() != 0)  {
  //         for(auto cardApp : applications) {
  //            if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //               auto ret = cardApp->queryPin1LockState(queryPin1LockResponseCb);
  //               if(ret == telux::common::Status::SUCCESS) {
  //                  std::cout << "Query pin1 lock state request sent successfully\n";
  //               } else {
  //                  std::cout << "Query pin1 lock state request failed\n";
  //               }
  //            }
  //         }
  //      } else {
  //         std::cout <<"Query pin1 lock state request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //      }
  //   } else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::queryFdnLockState(std::vector<std::string> userInput) {
  //   if(card_) {
  //      std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //      applications = card_->getApplications();
  //      if(applications.size() != 0)  {
  //         for(auto cardApp : applications) {
  //            if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //               auto ret = cardApp->queryFdnLockState(&queryFdnLockResponseCb);
  //               if(ret == telux::common::Status::SUCCESS) {
  //                  std::cout << "Query FDN lock state request sent successfully\n";
  //               } else {
  //                  std::cout << "Query FDN lock state request failed\n";
  //               }
  //            }
  //         }
  //      } else {
  //         std::cout <<"Query FDN lock state request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //      }
  //   } else {
  //      std::cout << "ERROR: Unable to get card instance";
  //   }

  const char *quert_fci_lock[4] ={};
  quert_fci_lock[0] = const_cast<char*>(userInput[1].c_str());
  quert_fci_lock[1] = const_cast<char*>(userInput[2].c_str());
  quert_fci_lock[2] = const_cast<char*>(userInput[3].c_str());
  quert_fci_lock[3] = const_cast<char*>(userInput[4].c_str());
  Status s = rilSession.simQueryFacilityLockReq(quert_fci_lock, [](RIL_Errno err, int32_t *res) -> void {
    std::cout << "Got response for Query Fdn lock req : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for queryFdn lock "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for query facility lock"
                                       : "Received Failure response for query facility lockn")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::setCardLock(std::vector<std::string> userInput) {
  //   std::string pwd, isEnable, lockType;
  //   telux::tel::CardLockType cardLockType;
  //   char delimiter = '\n';
  //
  //   if(!card_) {
  //      std::cout << "ERROR: Unable to get card instance";
  //      return;
  //   }
  //   std::cout << "Enter the 1-PIN1/2-FDN : ";
  //   std::getline(std::cin, lockType, delimiter);
  //   if(!lockType.empty()) {
  //      try {
  //         std::stoi(lockType);
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << lockType
  //                   << std::endl;
  //      }
  //   } else {
  //      std::cout << "empty input going with default PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   if(lockType.compare("1") == 0) {
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   } else if(lockType.compare("2") == 0) {
  //      cardLockType = telux::tel::CardLockType::FDN;
  //   } else {
  //      std::cout << "As the user entered wrong option, going with default card lock as PIN1\n";
  //      cardLockType = telux::tel::CardLockType::PIN1;
  //   }
  //
  //   std::cout << "Enter PIN: ";
  //   std::getline(std::cin, pwd, delimiter);
  //
  //   std::cout << "Enter 1-Enable/0-Disable: ";
  //   std::getline(std::cin, isEnable, delimiter);
  //   bool opt = false;
  //   if(!isEnable.empty()) {
  //      try {
  //         int value = std::stoi(isEnable);
  //         if(value != 0 && value != 1) {
  //            std::cout << "As user entered wrong input, taking default input as 'enable'\n";
  //            opt = true;
  //         } else {
  //            opt = value;
  //         }
  //      } catch(const std::exception &e) {
  //         std::cout << "ERROR: invalid input, please enter numerical values " << opt << std::endl;
  //      }
  //   } else {
  //      std::cout << "As user entered empty input, taking default input as 'enable'\n";
  //      opt = true;
  //   }
  //
  //   std::vector<std::shared_ptr<telux::tel::ICardApp>> applications;
  //   applications = card_->getApplications();
  //   if(applications.size() != 0)  {
  //      for(auto cardApp : applications) {
  //         if(cardApp->getAppType() == telux::tel::AppType::APPTYPE_USIM) {
  //            auto ret = cardApp->setCardLock(cardLockType, pwd, opt, &setCardLockResponseCb);
  //            if(ret == telux::common::Status::SUCCESS) {
  //               std::cout << "Set card lock request sent successfully\n";
  //            } else {
  //               std::cout << "Set card lock request failed\n";
  //            }
  //         }
  //      }
  //   } else {
  //         std::cout <<"Set card lock request failed"<< std::endl;
  //         telux::tel::CardState cardState;
  //         card_->getState(cardState);
  //         std::cout << "Card State : " << cardStateToString(cardState) << std::endl;
  //   }
  const char *set_fci_lock[5] ={};
  set_fci_lock[0] = const_cast<char *>(userInput[1].c_str());
  set_fci_lock[1] = const_cast<char *>(userInput[2].c_str());
  set_fci_lock[2] = const_cast<char *>(userInput[3].c_str());
  set_fci_lock[3] = const_cast<char *>(userInput[4].c_str());
  set_fci_lock[4] = const_cast<char *>(userInput[5].c_str());
  Status s = rilSession.simSetFacilityLockReq(set_fci_lock, [](RIL_Errno err ,int *res) -> void {
    std::cout << "Got response for set facility lock : " << err << std::endl;
    if(res!=NULL)
    {
       std::cout << "Response for set facility req "<< std::to_string(*res)<<std::endl;
    }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for set facility lock"
                                       : "Received Failure response for set facility lockn")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CardServicesMenu::getImsirequest(std::vector<std::string> userInput) {
    const char *aid;
    aid = const_cast<char *>(userInput[1].c_str());
    Status s = rilSession.simGetImsiReq(aid, [](RIL_Errno err ,const char *res) -> void {
      std::cout << "Got response for get imsi request : " << err << std::endl;
      if(res!=NULL)
      {
         std::cout << "IMSI value "<< std::string(res)<<std::endl;
      }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for imsi request"
                                         : "Received Failure response for imsi request")
              << std::endl;
}

void CardServicesMenu::setSimIoReq(std::vector<std::string> userInput) {

  RIL_SIM_IO_v6 Io_Req_params;

  Io_Req_params.command = std::stoi(userInput[1],NULL,16);
  Io_Req_params.fileid  = std::stoi(userInput[2],NULL,16);
  Io_Req_params.path    = const_cast<char *>(userInput[3].c_str());
  Io_Req_params.p1      = std::stoi(userInput[4]);
  Io_Req_params.p2      = std::stoi(userInput[5]);
  Io_Req_params.p3      = std::stoi(userInput[6]);
  Io_Req_params.data    = const_cast<char *>(userInput[7].c_str());
  Io_Req_params.pin2 = const_cast<char*>(userInput[8].c_str());
  Io_Req_params.aidPtr = const_cast<char*>(userInput[9].c_str());
  Status s = rilSession.simIOReq(Io_Req_params, [](RIL_Errno err ,const RIL_SIM_IO_Response *res) -> void {
    std::cout << "Got response for get simIO request : " <<std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1"<<res->sw1;
          std::cout<<"Statusword2"<<res->sw2;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse"<<std::string(res->simResponse)<<std::endl;
          }
      }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for SimIo request"
                                       : "Received Failure response for SimIo request")
            << std::endl;
}
void CardServicesMenu::enveloperequest(std::vector<std::string> userInput) {
    const char *aid;
    aid = const_cast<char *>(userInput[1].c_str());
    Status s = rilSession.simStkSendEnvelopeWithStatusReq(aid, [](RIL_Errno err ,const RIL_SIM_IO_Response *res) -> void {
      std::cout << "Got response for envelope status request : " << err << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1"<<res->sw1;
          std::cout<<"Statusword2"<<res->sw2;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse"<<std::string(res->simResponse)<<std::endl;
          }
      }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for envelope status request"
                                         : "Received Failure response for envelope status request")
              << std::endl;
}
void CardServicesMenu::envelopecommand(std::vector<std::string> userInput) {
    const char *command;
    size_t len;
    command = const_cast<char *>(userInput[1].c_str());
    len = strlen(command) + 1;
    Status s = rilSession.StkSendEnvelopeCommand(command,len, [](RIL_Errno err ,const char *res,size_t size) -> void {
      std::cout << "Got response for envelope command request : " << err << std::endl;
      if((res != NULL)&&(size > 0))
      {
          std::cout<<"Response data :"<<std::string(res)<<std::endl;
          std::cout<<"Length of the response"<<size<<std::endl;
      }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for envelope command request"
                                         : "Received Failure response for envelope command request")
              << std::endl;
}
void CardServicesMenu::Terminalresp_command(std::vector<std::string> userInput) {
    const char *command;
    size_t len;
    command = const_cast<char *>(userInput[1].c_str());
    len = strlen(command) + 1;
    Status s = rilSession.StkSendTerminalResponse(command,len, [](RIL_Errno err ) -> void {
      std::cout << "Got response for Terminal response command request : " << err << std::endl;
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for Terminal Response command request"
                                         : "Received Failure response for Terminal command request")
              << std::endl;
}
void CardServicesMenu::CallSetup_command(std::vector<std::string> userInput) {
    int command;
    command = std::stoi(userInput[1],NULL,16);
    Status s = rilSession.StkHandleCallSetupRequestedFromSim(command, [](RIL_Errno err ) -> void {
      std::cout << "Got response for callsetuprequestedrequest : " << err << std::endl;
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Sucess response for CallSetupRequested from sim request"
                                         : "Received Failure response for Callsetuprequestedfromsim request")
              << std::endl;
}
void CardServicesMenu::ServiceRunning_command(std::vector<std::string> userInput) {
  Status s = rilSession.ReportStkServiceIsRunning([](RIL_Errno err) -> void {
     std::cout << "Got response for Stk service running req " << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Received Stk service running  response succesfully"
                                       : "Received Stk service running with  error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::IsimAuthenticate_command(std::vector<std::string> userInput) {
  const char *command;
  command = const_cast<char *>(userInput[1].c_str());
  Status s = rilSession.simIsimAuthenticationReq(command,[](RIL_Errno err,const char *res) -> void {
     std::cout << "Got response for IsimAuthenticate req " << err << std::endl;
     if(res!=NULL)
     {
        std::cout << "Sim response "<< std::string(res)<<std::endl;
     }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received  Isimauthenticate  response succesfully"
                                       : "Received Isimauthticate with  error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::simAuthenticate_command(std::vector<std::string> userInput) {
  RIL_SimAuthentication params;
  params.authContext = std::stoi(userInput[1],NULL,16);
  params.authData = const_cast<char *>(userInput[2].c_str());
  params.aid = const_cast<char *>(userInput[3].c_str());
  Status s = rilSession.simAuthenticationReq(params,[](RIL_Errno err, const RIL_SIM_IO_Response *res) -> void {
     std::cout << "Got response for simAuthenticate req " << std::endl;
      if(res != nullptr) {
          std::cout<<"StatusWord1"<<res->sw1;
          std::cout<<"Statusword2"<<res->sw2;
          if(res->simResponse != nullptr)
          {
             std::cout<<"simresponse"<<std::string(res->simResponse)<<std::endl;
          }
      }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received  simauthenticate  response succesfully"
                                       : "Received simauthticate with  error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::NwDeperso_command(std::vector<std::string> userInput) {
  const char *command[1] = {};
  command[0] = const_cast<char *>(userInput[1].c_str());
  Status s = rilSession.EnterNetworkDepersonalization(command,[](RIL_Errno err,int32_t *res) -> void {
     std::cout << "Got response for Network Deperso req " << err << std::endl;
     if(res != nullptr)
     {
         std::cout << "Response for Network Personalization " << std::to_string(*res)<< std::endl;
     }
  });
  std::cout << ((s == Status::SUCCESS) ? "Received  Networkdeperso  response succesfully"
                                       : "Received Networkdeperso with  error")
            << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
void CardServicesMenu::registerForIndications(std::vector<std::string> userInput) {
   rilSession.registerStkUnsolSessionEnd(
      [] () {
          std::cout<<"[CardServicesMenu]: Received Stk SessionEnd Indications";
      }
  );
  rilSession.registerStkUnsolProactiveCommand(
      [](const char *res) {
        std::cout << "Received Proactive Commanfd Notification." << std::endl;
        if (res) {
          std::cout << std::string(res);
        }
      });

  rilSession.registerStkEventNotify(
      [] (const char *res ) {
          std::cout<<"[CardServicesMenu]: Received Stk Event Notifiy Indications";
          if(res)
          {
            std::cout<<std::string(res);
          }
      }
  );
  rilSession.registerStkCallSetup(
      [] (const int &res ) {
          std::cout<<"[CardServicesMenu]: Received Stk Call Set Up Indications"<< "message"<<res<<std::endl;
      }
  );
  rilSession.registerStkCcAlphaNotify(
      [] (const char *res ) {
          std::cout<<"[CardServicesMenu]: Received Stk  Event AlphaNotify Indications";
          if(res)
          {
            std::cout<<std::string(res);
          }
      }
  );
  rilSession.registerSimRefresh(
      [] (const RIL_SimRefreshResponse_v7 *res ) {
          std::cout<<"[CardServicesMenu]: Received Sim Refresh Indications";
          if(res)
          {
            std::cout<<"result"<<res->result;
            std::cout<<"ef_id"<<std::to_string(res->ef_id);
            std::cout<<"aid" << std::string(res->aid);
          }
      }
  );
  rilSession.registerSimStatusChanged(
      [] () {
          std::cout<<"[CardServicesMenu]: Received Sim Status Changed Indication";
      }
  );

}
void CardServicesMenu::GetAtr(std::vector<std::string> userInput) {
    int slot_id;

    slot_id = std::stoi(userInput[1]);

    std::cout << "slot_id" << slot_id << std::endl;

    Status s = rilSession.GetAtr(slot_id,[](RIL_Errno err, const char *atr ,int len) -> void {
       std::cout << "Got response for getAtr req " << std::endl;
        if(len > 0 && atr != NULL) {
            std::cout << "Atr : " << atr << std::endl;
            std::cout << "Atrlength : " << len << std::endl;
        }
    });
    std::cout << ((s == Status::SUCCESS) ? "Received Atr values  succesfully"
                                         : "Error in retrieving Atr values")
              << std::endl;
    for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}
