/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

#ifndef ECALLMENU_HPP
#define ECALLMENU_HPP

#include <cctype>
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

extern "C" {
#include <sys/time.h>
}
//
//#include <telux/tel/Call.hpp>
//#include <telux/common/CommonDefines.hpp>
//#include <telux/tel/Phone.hpp>
//#include <telux/tel/CallListener.hpp>
//#include <telux/tel/CallManager.hpp>

#include "console_app_framework/ConsoleApp.hpp"
#include "MsdSettings.hpp"

#define CATEGORY_AUTO 1
#define CATEGORY_MANUAL 2
#define VARIANT_TEST 1
#define VARIANT_EMERGENCY 2
#define VARIANT_VOICE 3

const std::string ECALL_CATEGORY_AUTO = "auto";
const std::string ECALL_CATEGORY_MANUAL = "manual";
const std::string ECALL_VARIANT_TEST = "test";
const std::string ECALL_VARIANT_EMERGENCY = "emergency";

class ECallMenu : public ConsoleApp {
 public:
  /**
   * Initialize commands and SDK
   */
  void init();

  ECallMenu(std::string appName, std::string cursor);
  ~ECallMenu();

 private:
  bool initalizeSDK();

  /**
   * Sample dial operation
   */
  void makeCall(std::vector<std::string> inputCommand);
  /**
   * Sample hangup operation
   */
  void hangup(std::vector<std::string> inputCommand);

  /**
   * Sample eCall operation
   */
  void makeECall(std::vector<std::string> inputCommand);

  /**
   * Sample voice eCall operation
   */
  void makeVoiceECall(std::vector<std::string> inputCommand);

  /**
   * Sample eCall operation with raw PDU
   */
  void eCallWithPdu(std::vector<std::string> inputCommand);

  /**
   * Sample eCall SOS operation
   */
  void eCallSos(std::vector<std::string> inputCommand);

  // answer incoming call
  void answerCall(std::vector<std::string> inputCommand);

  /**
   * Sample Update eCall MSD operation
   */
  void updateECallMSD(std::vector<std::string> inputCommand);

  /**
   * Sample Update eCall MSD operation with raw pdu
   */
  void updateEcallMsdWithPdu(std::vector<std::string> inputCommand);

  /**
   * Sample get in progress calls operations
   */
  void getCalls(std::vector<std::string> inputCommand);

  //   std::string getCallDescription(std::shared_ptr<telux::tel::ICall> call);

  /**
   * Register a listener
   */
  //   void registerCallListener(std::shared_ptr<telux::tel::ICallListener> listener);

  /**
   * Remove a registered listener
   */
  //   void removeCallListener(std::shared_ptr<telux::tel::ICallListener> listener);

  /**
   * Convert the hexadecimal string to bytes
   */
  std::vector<uint8_t> convertHexToBytes(std::string msdData);

  /**
   * This method is useful to trim the spaces in options and converting them into LOWERCASE
   */
  std::string toLowerCase(std::string inputOption);

  // Member variable to keep the Listener object alive till application ends.
  //   std::shared_ptr<telux::tel::ICallListener> callListener_;

  //   class CallCommandCallback : public telux::tel::IMakeCallCallback {
  //   public:
  //      void makeCallResponse(telux::common::ErrorCode error,
  //                            std::shared_ptr<telux::tel::ICall>) override;
  //   };
  //   std::shared_ptr<CallCommandCallback> callCommandCallback_;
  //
  //   class UpdateMsdCommandCallback : public telux::common::ICommandResponseCallback {
  //   public:
  //      void commandResponse(telux::common::ErrorCode error) override;
  //   };
  //   std::shared_ptr<UpdateMsdCommandCallback> updateMsdCommandCallback_;
  //
  //   class HangupCommandCallback : public telux::common::ICommandResponseCallback {
  //   public:
  //      void commandResponse(telux::common::ErrorCode error) override;
  //   };
  //   std::shared_ptr<HangupCommandCallback> hangupCommandCallback_;
  //
  //   class AnswerCommandCallback : public telux::common::ICommandResponseCallback {
  //   public:
  //      void commandResponse(telux::common::ErrorCode error) override;
  //   };
  //   std::shared_ptr<AnswerCommandCallback> answerCommandCallback_;
};

#endif  // ECallMenu_HPP
