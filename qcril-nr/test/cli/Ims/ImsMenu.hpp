/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

/**
 * IMS Menu class provides IMS functionality of the SDK
 */

#ifndef IMSMENU_HPP
#define IMSMENU_HPP

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"

class ImsMenu : public ConsoleApp {
 public:
  ImsMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~ImsMenu();
  void init();

 private:
  RilApiSession& rilSession;

 private:
  void registrationStatus(std::vector<std::string> userInput);
  void imsDial(std::vector<std::string> userInput);
  void acceptCall(std::vector<std::string> userInput);
  void hangupCall(std::vector<std::string> userInput);
  void imsRegistrationChange(std::vector<std::string> userInput);
  void imsQueryServiceStatus(std::vector<std::string> userInput);
  void imsSetServiceStatus(std::vector<std::string> userInput);
  void imsGetImsSubConfig(std::vector<std::string> userInput);
  void imsDtmf(std::vector<std::string> userInput);
  void imsDtmfStart(std::vector<std::string> userInput);
  void imsDtmfStop(std::vector<std::string> userInput);
  void imsModifyCallInitiate(std::vector<std::string> userInput);
  void imsModifyCallConfirm(std::vector<std::string> userInput);
  void imsCancelModifyCall(std::vector<std::string> userInput);
  void imsRequestConference(std::vector<std::string> userInput);
  void imsRequestHoldCall(std::vector<std::string> userInput);
  void imsRequestResumeCall(std::vector<std::string> userInput);
  void imsRequestExitEmergencyCallbackMode(std::vector<std::string> userInput);
  void imsSendRttMessage(std::vector<std::string> userInput);
  void imsSendGeolocationInfo(std::vector<std::string> userInput);
  void imsSetColr(std::vector<std::string> userInput);
  void imsGetColr(std::vector<std::string> userInput);
  void imsRegisterMultiIdentityLines(std::vector<std::string> userInput);
  void imsQueryVirtualLineInfo(std::vector<std::string> userInput);
  void imsSendSms(std::vector<std::string> userInput);
  void imsAckSms(std::vector<std::string> userInput);
  void imsGetSmsFormat(std::vector<std::string> userInput);
  void imsSetCallWaiting(std::vector<std::string> userInput);
  void imsQueryCallWaiting(std::vector<std::string> userInput);
  void registerForImsIndications(std::vector<std::string> userInput);
  void imsGetClir(std::vector<std::string> userInput);
  void imsSetClir(std::vector<std::string> userInput);
  void imsExplicitCallTransfer(std::vector<std::string> userInput);
  void imsSendUiTtyMode(std::vector<std::string> userInput);
  void imsDeflectCall(std::vector<std::string> userInput);
  void imsQueryClip(std::vector<std::string> userInput);
  void imsSetSuppSvcNotification(std::vector<std::string> userInput);

  int phoneId_;
};

#endif  // IMSMENU_HPP
