/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

/**
 * SmsMenu provides menu options to invoke SMS functions such as send SMS,
 * receive SMS etc.
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <vector>

#include <RilApiSession.hpp>
#include <telephony/ril.h>
#include "SmsMenu.hpp"

SmsMenu::SmsMenu(std::string appName, std::string cursor, int phoneId, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();
  //  Get the PhoneFactory and PhoneManager instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   phoneManager_ = phoneFactory.getPhoneManager();
  //
  //   //  Check if telephony subsystem is ready
  //   bool subSystemStatus = phoneManager_->isSubsystemReady();
  bool subSystemStatus = true;
  //   //  If telephony subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::cout << "Telephony subsystem is not ready, Please wait" << std::endl;
  //      std::future<bool> f = phoneManager_->onSubsystemReady();
  //      // If we want to wait unconditionally for telephony subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  //  Exit the application, if SDK is unable to initialize telephony subsystems
  if (subSystemStatus) {
    endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Elapsed Time for Subsystems to ready : " << elapsedTime.count() << "s\n"
              << std::endl;
  } else {
    std::cout << "ERROR - Unable to initialize subSystem" << std::endl;
    exit(0);
  }
  //   if(subSystemStatus) {
  //      mySmsCmdCb_ = std::make_shared<MySmsCommandCallback>();
  //      mySmscAddrCb_ = std::make_shared<MySmscAddressCallback>();
  //      mySmsDeliveryCb_ = std::make_shared<MySmsDeliveryCallback>();
  //      smsListener_ = std::make_shared<MySmsListener>();
  //
  //      smsManager_ = phoneFactory.getSmsManager(phoneId);
  //
  //      // add listeners for incoming SMS notification
  //      telux::common::Status status = smsManager_->registerListener(smsListener_);
  //      if(status != telux::common::Status::SUCCESS) {
  //         std::cout << "Unable to register Listener" << std::endl;
  //      }
  //   }
}

SmsMenu::~SmsMenu() {
}

void SmsMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListSmsSubMenu = {
    CREATE_COMMAND(SmsMenu::registerForUnsols, "register_for_unsols"),
    CREATE_COMMAND(SmsMenu::sendSms, "Send_SMS"),
    CREATE_COMMAND(SmsMenu::sendCdmaSms, "Send_CDMA_SMS"),
    CREATE_COMMAND(SmsMenu::getSmscAddr, "Get_SMSC_address"),
    CREATE_COMMAND(SmsMenu::setSmscAddr, "Set_SMSC_address"),
    CREATE_COMMAND(SmsMenu::ackSms, "Ack_Sms"),
    CREATE_COMMAND(SmsMenu::ackCdmaSms, "Ack_Cdma_Sms"),
    CREATE_COMMAND(SmsMenu::getCdmaSmsBroadcastConfig, "Get_Cdma_sms_broadcast_config"),
    CREATE_COMMAND(SmsMenu::setCdmaSmsBroadcastConfig, "Set_Cdma_sms_broadcast_config"),
    CREATE_COMMAND(SmsMenu::setCdmaSmsBroadcastActivation, "Set_Cdma_sms_broadcast_activation"),
    CREATE_COMMAND(SmsMenu::writeCdmaSmsToRuim, "Write_Cdma_sms_to_ruim"),
    CREATE_COMMAND(SmsMenu::deleteCdmaSmsOnRuim, "Delete_Cdma_sms_to_ruim"),
    CREATE_COMMAND(SmsMenu::calculateMessageAttributes, "Calculate_message_attributes"),
    CREATE_COMMAND(SmsMenu::writeSmsToSim, "Write SMS to SIM"),
    CREATE_COMMAND(SmsMenu::deleteSmsOnSim, "Delete SMS on SIM"),
    CREATE_COMMAND(SmsMenu::reportSmsMemoryStatus, "Report SMS storage space status"),
    CREATE_COMMAND(SmsMenu::gsmGetBroadcastSmsConfig, "Get GSM Broadcast SMS Config"),
    CREATE_COMMAND(SmsMenu::setGsmSetBroadcastSMSConfig, "Set GSM Broadcast SMS Config"),
    CREATE_COMMAND(SmsMenu::setGsmSmsBroadcastActivation, "Set GSM Broadcast SMS Activation"),
  };
  addCommands(commandsListSmsSubMenu);
  ConsoleApp::displayMenu();
  std::cout << "Device is listening for any incoming messages" << std::endl;
}

std::ostream& operator <<(std::ostream& out, const RIL_SMS_Response& resp) {
  out << "== SMS Response ==" << std::endl;
  out << "   Error Code: " << resp.errorCode << std::endl;
  out << "   Message Ref: " << resp.messageRef << std::endl;
  out << "   Ack PDU:" << std::endl;

  if (resp.ackPDU != nullptr) {
    out << std::hex;
    for (char* c = resp.ackPDU; *c; c++) {
      out << *c << " ";
    }
    out << std::dec;
  }

  return out;
}

// SMS Requests
void SmsMenu::sendSms(std::vector<std::string> userInput) {
  // To: +12243238312 (https://www.receivesms.co/receive-sms-from-test/)
  // Message: "Hello World"
  // SMS PDU: "01000B912142238313F200000BC8329BFD065DDF723619"
  std::string destinationAddress;
  do {
    std::cin.clear();
    std::cout << "Enter the number to send SMS to (-1 - Exit): ";
    std::cin >> destinationAddress;
  } while (std::cin.fail());

  if (destinationAddress == "-1") {
    return;
  }

  if (destinationAddress.size() % 2) {
    destinationAddress.append(1, 'F');
  }

  for (std::string::size_type i = 0; i < destinationAddress.size(); i += 2) {
    std::swap(destinationAddress[i], destinationAddress[i + 1]);
  }

  std::string pdu("01000B91");
  //pdu.append("2142238313F2"); // To: 12243238312
  pdu.append(destinationAddress);
  pdu.append("00000BC8329BFD065DDF723619"); // Message: "Hello World"

  Status s = rilSession.sendSms(
    pdu,
    "",
    false,
    [] (RIL_Errno e, const RIL_SMS_Response& resp) {
      std::cout << ((e == RIL_E_SUCCESS) ?
        "Successfully sent SMS." : "Failed to send SMS.")
        << " Error: " << e << "." << std::endl;
      std::cout << resp << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << "Failed to issue request to send SMS." << std::endl;
  }
}

std::istream& operator >> (std::istream &in, RIL_CDMA_SMS_Address& addr) {
    uint32_t input_op;

    std::cout << "Address digit mode: 0 - 4bit, 1 - 8bit: " << std::endl;
    do {
        in >> input_op;
    } while (input_op != 0 || input_op != 1);
    addr.digit_mode = static_cast<RIL_CDMA_SMS_DigitMode>(input_op);

    if (addr.digit_mode == RIL_CDMA_SMS_DIGIT_MODE_8_BIT)
    {
        std::cout << "SMS number mode: 0 - Not data notwork 1 - data network: "
                  << std::endl;
        do {
            in >> input_op;
        } while (input_op != 0 || input_op != 1);
        addr.number_mode = static_cast<RIL_CDMA_SMS_NumberMode>(input_op);

        std::cout << "SMS number type: " << std::endl
                  << " 0 - Unknown" << std::endl
                  << " 1 - international or data ip" << std::endl
                  << " 2 - national or internet mail" << std::endl
                  << " 3 - network" << std::endl
                  << " 4 - subscriber" << std::endl
                  << " 5 - alphanumeric" << std::endl
                  << " 6 - abbreviated" << std::endl
                  << " 7 - reserved" << std::endl;
        do {
            in >> input_op;
        } while (input_op > 6);
        addr.number_type = static_cast<RIL_CDMA_SMS_NumberType>(input_op);

        std::cout << "SMS number plan: " << std::endl
                  << " 0 - Unknown" << std::endl
                  << " 1 - telephony" << std::endl
                  << " 3 - data" << std::endl
                  << " 4 - telex" << std::endl
                  << " 9 - private" << std::endl
                  << " 2, 5~8, 10~15 - reserved" << std::endl; 
        do {
            in >> input_op;
        } while (input_op != 0 || input_op != 1 || input_op != 3 ||
                input_op != 4 || input_op != 9);
        addr.number_plan = static_cast<RIL_CDMA_SMS_NumberPlan>(input_op);
    }

    std::cout << "number of digits: " << std::endl;
    do {
        in >> input_op;
    } while (input_op > RIL_CDMA_SMS_ADDRESS_MAX);
    addr.number_of_digits = static_cast<unsigned char>(input_op);
    std::cout << "input digits: " << std::endl;
    std::copy_n(std::istream_iterator<unsigned char>(in),
            addr.number_of_digits, addr.digits);

    return in;
}

std::istream& operator >> (std::istream &in, RIL_CDMA_SMS_Message& cdmaSms) {
    std::cout << "teleservice id: " << std::endl;
    in >> cdmaSms.uTeleserviceID;
    std::cout << "is service present (0 or 1): " << std::endl;
    in >> cdmaSms.bIsServicePresent;
    std::cout << "service category: " << std::endl;
    in >> cdmaSms.uServicecategory;
    in >> cdmaSms.sAddress;
    uint32_t len;
    do {
        std::cout << "bearer data len: " << std::endl;
        in >> len;
        if (len > RIL_CDMA_SMS_BEARER_DATA_MAX) {
            std::cout << "lengh must not be greater than "
                      << RIL_CDMA_SMS_BEARER_DATA_MAX << std::endl;
        } else {
            cdmaSms.uBearerDataLen = len;
        }
    } while (len <= RIL_CDMA_SMS_BEARER_DATA_MAX);
    std::cout << "input bearer data: " << std::endl;
    std::copy_n(std::istream_iterator<unsigned char>(in),
            cdmaSms.uBearerDataLen, cdmaSms.aBearerData);

    cdmaSms.expectMore= 0;

    return in;
}

void SmsMenu::sendCdmaSms(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_CDMA_SMS_Message cdmaSms{};
  std::cin >> cdmaSms;
  Status s = rilSession.sendCdmaSms(cdmaSms,
        [](RIL_Errno err, const RIL_SMS_Response& resp) -> void {
            std::cout << "Got response for send CDMA SMS, ril err: "
                      << err << std::endl;
            std::cout << "message reference: " << resp.messageRef
                      << "error code: " << resp.errorCode << std::endl;

        });
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send MO CDMA SMS req"
                                       : "Failed to send MO CDMA SMS req")
            << std::endl;
}

void SmsMenu::getSmscAddr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getSmscAddress(
        [](RIL_Errno err, const char* addr) -> void {
            std::cout << "Get response for get SMSC address, err code: "
                      << err << " address: " << (addr ? addr : "null") << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send get SMSC address req"
                                       : "Failed to send get SMSC address req")
            << std::endl;
}

void SmsMenu::setSmscAddr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::cout << "set SMSC Address \n" << std::endl;
  std::string smscAddress;
  std::cin >> smscAddress;
  Status s = rilSession.setSmscAddress(smscAddress,
        [](RIL_Errno err) -> void {
            std::cout << "Got response for set SMSC address: " << err << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send set SMSC address req"
                                       : "Failed to send set SMSC address req")
            << std::endl;
}

std::istream& operator >> (std::istream &in, RIL_GsmSmsAck& ack) {
  std::cout << "SMS acknowledge result: (0 -> failure, others -> success) " << std::endl;
  in >> ack.result;
  if (!ack.result) {
    std::cout << "SMS acknowledge failure cause: " << std::endl;
    in >> ack.cause;
  }
  return in;
}

void SmsMenu::ackSms(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_GsmSmsAck ack{};
  std::cin >> ack;
  Status s = rilSession.ackSms(ack,
        [](RIL_Errno err) -> void {
            std::cout << "Got response for acknowledge sms: " << err << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send acknowledge sms req"
                                       : "Failed to send acknowledge sms req")
            << std::endl;
}

std::istream& operator >> (std::istream &in, RIL_CDMA_SMS_Ack& ack) {
  int input;
  std::cout << "CDMA SMS acknowledge error class: (0 -> no error, 1 -> error) "
            << std::endl;
  do {
    in >> input;
  } while (input != 0 || input != 1);
  ack.uErrorClass = static_cast<RIL_CDMA_SMS_ErrorClass>(input);

  std::cout << "SMS faliure cause (defined in N.S00005, 6.5.2.125): " << std::endl
            << " - 0: no error" << std::endl
            << " - 35: resource shortage" << std::endl
            << " - 39: other terminal problem" << std::endl
            << " - other integers" << std::endl;
  in >> input;
  ack.uSMSCauseCode = input;

  return in;
}

void SmsMenu::ackCdmaSms(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_CDMA_SMS_Ack ack{};
  std::cin >> ack;
  Status s = rilSession.ackCdmaSms(ack,
        [](RIL_Errno err) -> void {
            std::cout << "Got response for acknowledge CDMA sms: " << err << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send acknowledge CDMA sms req"
                                       : "Failed to send acknowledge CDMA sms req")
            << std::endl;
}

void SmsMenu::getCdmaSmsBroadcastConfig(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getCdmaSmsBroadcastConfig(
        [](RIL_Errno err, const RIL_CDMA_BroadcastSmsConfigInfo* configInfo,
                size_t configSize) -> void {
            std::cout << "Got response for get cdma sms broadcast config: "
                      << err << std::endl;
            if (configInfo != nullptr && configSize) {
                for (int i = 0; i < configSize; i++) {
                    std::cout << "config " << i << ":" << std::endl
                              << " - service_category: "
                              << configInfo[i].service_category << std::endl
                              << " - language: "
                              << configInfo[i].language << std::endl
                              << " - selected: "
                              << static_cast<unsigned int>(configInfo[i].selected) << std::endl;
                }
            }
        });
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send get CDMA sms broadcast config req"
                                       : "Failed to send get CDMA sms broadcast config req")
            << std::endl;

}

std::istream& operator >> (std::istream &in, RIL_CDMA_BroadcastSmsConfigInfo& config) {
  std::cout << "service category: " << std::endl;
  in >> config.service_category;
  std::cout << "language: " << std::endl;
  in >> config.language;
  std::cout << "selected: " << std::endl;
  in >> config.selected;

  return in;
}

void SmsMenu::setCdmaSmsBroadcastConfig(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  uint32_t input;
  std::cout << "please input number of configs: (qmi limit: 50): " << std::endl;
  do {
    std::cin >> input;
  } while (input > 50);
  size_t size = input;

  Status s = Status::FAILURE;
  auto cfgParams = new RIL_CDMA_BroadcastSmsConfigInfo[size];
  if (cfgParams != nullptr) {
    for (int i = 0; i < size; i++) {
        std::cout << "config " << i << ":" << std::endl;
        std::cin >> cfgParams[i];
    }
    s = rilSession.setCdmaSmsBroadcastConfig(cfgParams, size,
        [](RIL_Errno err) -> void {
            std::cout << "Got response for set cdma sms broadcast config: "
                      << err << std::endl;
        });
    delete []cfgParams;
  }
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send set CDMA sms broadcast config req"
                                       : "Failed to send set CDMA sms broadcast config req")
            << std::endl;
}

void SmsMenu::setCdmaSmsBroadcastActivation(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  int32_t input;
  std::cout << "CDMA SMS broadcast, 0 - Activate, 1 - Turn off: " << std::endl;
  do {
    std::cin >> input;
  } while (input != 0 && input != 1);

  Status s = rilSession.setCdmaSmsBroadcastActivation(input,
        [](RIL_Errno err) -> void {
            std::cout << "Got response for activating cdma sms broadcast: "
                      << err << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send set CDMA sms broadcast activation req"
                                       : "Failed to send set CDMA sms broadcast activation req")
            << std::endl;
}

std::istream& operator >> (std::istream &in, RIL_CDMA_SMS_WriteArgs &arg) {
  std::cout << "CDMA sms write args - status: " << std::endl
            << " 0 - REC UNREAD" << std::endl
            << " 1 - REC READ" << std::endl
            << " 2 - STO UNSENT" << std::endl
            << " 3 - STO SENT" << std::endl;
  int input;
  do {
    in >> input;
  } while (input < 0 && input > 3);
  arg.status = input;

  std::cout << "CDMA sms write args - sms: " << std::endl;
  in >> arg.message;

  return in;
}

void SmsMenu::writeCdmaSmsToRuim(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_CDMA_SMS_WriteArgs arg{};
  std::cin >> arg;
  Status s = rilSession.writeCdmaSmsToRuim(arg,
        [](RIL_Errno err, int32_t recordNumber) -> void {
            std::cout << "Got response for writing cdma sms to Ruim: "
                      << err << " record number: " << recordNumber << std::endl;
        });

  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send write CDMA sms to Ruim req"
                                       : "Failed to send write CDMA sms to Ruim req")
            << std::endl;
}

void SmsMenu::deleteCdmaSmsOnRuim(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::cout << "Please input the index: " << std::endl;
  int32_t index;
  std::cin >> index;
  Status s = rilSession.deleteCdmaSmsOnRuim(index,
        [](RIL_Errno err) -> void {
            std::cout << "Got response for deleting cdma sms on Ruim: "
                      << err << std::endl;
        });

  std::cout << ((s == Status::SUCCESS) ? "Succeeded to send delete CDMA sms to Ruim req"
                                       : "Failed to send delete CDMA sms to Ruim req")
            << std::endl;

}

void SmsMenu::calculateMessageAttributes(std::vector<std::string> userInput) {
  //   std::string smsMessage;
  //   char delimiter = '\n';
  //
  //   std::cout << "Enter Message: ";
  //   std::getline(std::cin, smsMessage, delimiter);
  //
  //   auto msgAttributes = smsManager_->calculateMessageAttributes(smsMessage);
  //   std::cout
  //      << "Message Attributes \n encoding: " << (int)msgAttributes.encoding
  //      << "\n numberOfSegments: " << msgAttributes.numberOfSegments
  //      << "\n segmentSize: " << msgAttributes.segmentSize
  //      << "\n numberOfCharsLeftInLastSegment: " << msgAttributes.numberOfCharsLeftInLastSegment
  //      << std::endl;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

std::ostream& operator << (std::ostream &os, const RIL_CDMA_SMS_Address &addr) {
  os << "Digit Mode: " << addr.digit_mode << std::endl;
  if (addr.digit_mode == RIL_CDMA_SMS_DIGIT_MODE_8_BIT) {
    os << "Number Mode: " << addr.number_mode << std::endl;
    os << "Number Type: " << addr.number_type << std::endl;
    os << "Number Plan: " << addr.number_plan << std::endl;
  }
  os << "Number of Digits: " << addr.number_of_digits;
  if (addr.number_of_digits > 0) {
    os << std::endl << "Digits: ";
    uint32_t size = (addr.number_of_digits > RIL_CDMA_SMS_ADDRESS_MAX) ?
            RIL_CDMA_SMS_ADDRESS_MAX : addr.number_of_digits;
    for (uint32_t i = 0; i < size; i++) {
        os << addr.digits[i] << " ";
    }
  }
  return os;
}

std::ostream& operator << (std::ostream &os, const RIL_CDMA_SMS_Subaddress &addr) {
  os << "Sub Address Type: " << addr.subaddressType << std::endl;
  os << "odd: " << addr.odd << std::endl;
  os << "Number of Digits: " << addr.number_of_digits;
  if (addr.number_of_digits > 0) {
    os << std::endl << "Digits: ";
    uint32_t size = (addr.number_of_digits > RIL_CDMA_SMS_SUBADDRESS_MAX) ?
            RIL_CDMA_SMS_SUBADDRESS_MAX : addr.number_of_digits;
    for (uint32_t i = 0; i < size; i++) {
        os << addr.digits[i] << " ";
    }
  }
  return os;
}

std::ostream& operator << (std::ostream &os, const RIL_CDMA_SMS_Message &sms) {
  os << "Teleservice ID: " << sms.uTeleserviceID << std::endl;
  os << "Is Service Present: " << sms.bIsServicePresent << std::endl;
  os << "Service Category: " << sms.uServicecategory << std::endl;
  os << "Bear Data Length: " << sms.uBearerDataLen << std::endl;
  os << "Address Info: " << std::endl << sms.sAddress << std::endl;
  os << "Subaddress Info: " << std::endl << sms.sSubAddress << std::endl;
  if (sms.uBearerDataLen > 0) {
    os << "Bear Data: ";
    uint32_t size = (sms.uBearerDataLen > RIL_CDMA_SMS_BEARER_DATA_MAX) ?
            RIL_CDMA_SMS_BEARER_DATA_MAX : sms.uBearerDataLen;
    for (uint32_t i = 0; i < size; i++) {
        os << sms.aBearerData[i] << " ";
    }
    os << std::endl;
  }
  os << "Expect More: " << sms.expectMore;
  return os;
}

void SmsMenu::registerForUnsols(std::vector<std::string> userInput) {
    rilSession.registerSimSmsStorageFullIndicationHandler(
        [] () {
            std::cout<<"[SmsMenu]: Received Sim Sms Storage Full Indication"<< std::endl;
        }
    );

    rilSession.registerNewSmsStatusReportIndicationHandler(
        [] (const char *statusReport) {
            std::cout<<"[SmsMenu]: Received New Sms Status Report Indication"<< std::endl;
            if(statusReport) {
                std::cout<<"[SmsMenu]: statusReport: "<< statusReport << std::endl;;
            }
        }
    );

    rilSession.registerCdmaSmsRuimStorageFullIndicationHandler(
        [] () {
            std::cout<<"[SmsMenu]: Received Cdma Sms Ruim Storage Full Indication"<< std::endl;
        }
    );
    rilSession.registerCdmaNewSmsIndicationHandler(
        [](const RIL_CDMA_SMS_Message& sms) {
            std::cout << "Received CDMA MT SMS." << std::endl;
            std::cout << sms << std::endl;
        }
    );

    rilSession.registerIncoming3GppSmsIndicationHandler(
      [] (const uint8_t pdu[], size_t pduLength) {
        std::cout << std::endl << "== Incoming SMS ==" << std::endl;
        std::cout << std::hex;
        for (size_t i = 0; i < pduLength; i++) {
          std::cout << static_cast<unsigned int>(pdu[i]) << " ";
        }
        std::cout << std::dec;
        std::cout << std::endl;
      }
    );

    rilSession.registerBroadcastSmsIndicationHandler(
      [] (const std::vector<uint8_t>& pdu) {
        std::cout << std::endl << "== Incoming Broadcast SMS ==" << std::endl;
        std::cout << std::hex;
        for (const uint8_t& i : pdu) {
          std::cout << i << " ";
        }
        std::cout << std::dec;
        std::cout << std::endl;
      }
    );

    rilSession.registerNewSmsOnSimIndicationHandler(
      [] (const int& i) {
        std::cout << std::endl << "A new SMS has been saved on the SIM at index "
          << i << "." << std::endl;
      }
    );
}

void SmsMenu::writeSmsToSim(std::vector<std::string> userInput) {
  Status s = rilSession.writeSmsToSim(
    "",
    "",
    0,
    [] (RIL_Errno e, int32_t recordIndex) {
      std::cout << ((e == RIL_E_SUCCESS) ?
        "Successfully wrote SMS to SIM." : "Failed to save SMS to SIM.")
        << " Message Record Index: " << recordIndex << ", Error: " << e
        << "." << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << "Failed to issue request to write SMS to SIM." << std::endl;
  }
}

void SmsMenu::deleteSmsOnSim(std::vector<std::string> userInput) {
  int32_t recordIndex = -1;

  do {
    std::cin.clear();
    std::cout << "Enter the record index of the message to delete (-1 - Exit): ";
    std::cin >> recordIndex;
  } while (std::cin.fail() || recordIndex < -1);

  if (recordIndex == -1) return;

  Status s = rilSession.deleteSmsOnSim(
    recordIndex,
    [] (RIL_Errno e) {
      std::cout << ((e == RIL_E_SUCCESS) ?
        "Successfully deleted SMS from SIM." : "Failed to delete SMS from SIM.")
        << " Error: " << e << "." << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << "Failed to issue request to delete SMS on SIM." << std::endl;
  }
}

void SmsMenu::reportSmsMemoryStatus(std::vector<std::string> userInput) {
  bool storageSpaceAvailable = true;

  do {
    std::cin.clear();
    std::cout << "Is enough storage space available for new SMS (0 - No !0 - Yes)? ";
    std::cin >> storageSpaceAvailable;
  } while (std::cin.fail());

  Status s = rilSession.reportSmsMemoryStatus(
    storageSpaceAvailable,
    [] (RIL_Errno e) {
      std::cout << ((e == RIL_E_SUCCESS) ? "Successfully reported " : "Failed to report ")
        << "SMS storage space availability. Error: " << e << "." << std::endl;
    }
  );

  if (s != Status::SUCCESS) {
    std::cout << "Failed to issue request to report SMS storage space availability."
      << std::endl;
  }
}

void SmsMenu::gsmGetBroadcastSmsConfig(std::vector<std::string> userInput) {
    for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

    Status s = rilSession.GsmGetBroadcastSmsConfig(
          [](RIL_Errno err, const RIL_GSM_BroadcastSmsConfigInfo* configInfo,
                  size_t configSize) -> void {
              std::cout << "Got response for get gsm sms broadcast config: err: "
                        << err << " response len: " << configSize << std::endl;
              if (configInfo != nullptr && configSize) {
                  for (int i = 0; i < configSize; i++) {
                      std::cout << "config " << i << ":" << std::endl
                                << " - fromservice_id: "
                                << configInfo[i].fromServiceId << std::endl
                                << " - toServiceId: "
                                << configInfo[i].toServiceId << std::endl
                                << " - fromCodeScheme: "
                                << configInfo[i].fromCodeScheme << std::endl
                                << " - toCodeScheme: "
                                << configInfo[i].toCodeScheme << std::endl
                                << " - selected: "
                                << static_cast<unsigned>(configInfo[i].selected) << std::endl;
                  }
              }
          });
    std::cout << ((s == Status::SUCCESS) ? "Succeeded to send get GSM sms broadcast config req"
                                         : "Failed to send get GSM sms broadcast config req")
              << std::endl;

}

std::istream& operator >> (std::istream &in, RIL_GSM_BroadcastSmsConfigInfo& config) {
  std::cout << "fromServiceId: " << std::endl;
  in >> config.fromServiceId;
  std::cout << "toServiceId: " << std::endl;
  in >> config.toServiceId;
  std::cout << "fromCodeScheme: " << std::endl;
  in >> config.fromCodeScheme;
  std::cout << "toCodeScheme: " << std::endl;
  in >> config.toCodeScheme;
  std::cout << "selected: " << std::endl;
  in >> config.selected;

  return in;
}


void SmsMenu::setGsmSetBroadcastSMSConfig(std::vector<std::string> userInput) {
    for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

    uint32_t input;
    std::cout << "please input number of configs: (qmi limit: 50): " << std::endl;
    do {
      std::cin >> input;
    } while (input > 50);
    size_t size = input;

    Status s = Status::FAILURE;
    auto cfgParams = new RIL_GSM_BroadcastSmsConfigInfo[size];
    if (cfgParams != nullptr) {
      for (int i = 0; i < size; i++) {
          std::cout << "config " << i << ":" << std::endl;
          std::cin >> cfgParams[i];
      }
      s = rilSession.GsmSetBroadcastSMSConfig(cfgParams, size,
          [](RIL_Errno err) -> void {
              std::cout << "Got response for set cdma sms broadcast config: "
                        << err << std::endl;
          });
      delete []cfgParams;
    }
    std::cout << ((s == Status::SUCCESS) ? "Succeeded to send set CDMA sms broadcast config req"
                                         : "Failed to send set CDMA sms broadcast config req")
              << std::endl;
}

void SmsMenu::setGsmSmsBroadcastActivation(std::vector<std::string> userInput) {
    for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

    int32_t input;
    std::cout << "GSM SMS broadcast, 0 - Activate, 1 - Turn off: " << std::endl;
    do {
      std::cin >> input;
    } while (input != 0 && input != 1);

    Status s = rilSession.GsmSmsBroadcastActivation(input,
          [](RIL_Errno err) -> void {
              std::cout << "Got response for activating gsm sms broadcast: "
                        << err << std::endl;
          });
    std::cout << ((s == Status::SUCCESS) ? "Succeeded to send set gsm sms broadcast activation req"
                                         : "Failed to send set gsm sms broadcast activation req")
              << std::endl;
}
