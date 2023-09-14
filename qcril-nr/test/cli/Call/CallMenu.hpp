/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

/**
 * Call Menu class provides dialer functionality of the SDK
 * it has menu options for dial, answer, hangup, reject, conference and swap calls
 */

#ifndef CALLMENU_HPP
#define CALLMENU_HPP

#include <RilApiSession.hpp>
#include "console_app_framework/ConsoleApp.hpp"
//#include "MyCallListener.hpp"

class CallMenu : public ConsoleApp {
 public:
  CallMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
  ~CallMenu();
  void init();

 private:
  RilApiSession& rilSession;

 private:
  void registerForIndications(std::vector<std::string> userInput);
  void dial(std::vector<std::string> userInput);
  void acceptCall(std::vector<std::string> userInput);
  void rejectCall(std::vector<std::string> userInput);
  void rejectWithSms(std::vector<std::string> userInput);
  void hangupDialingOrAlerting(std::vector<std::string> userInput);
  void hangupWithCallIndex(std::vector<std::string> userInput);
  void holdCall(std::vector<std::string> userInput);
  void conference(std::vector<std::string> userInput);
  void swap(std::vector<std::string> userInput);
  void getCalls(std::vector<std::string> userInput);
  void resumeCall(std::vector<std::string> userInput);
  void playDtmfTone(std::vector<std::string> userInput);
  void startDtmfTone(std::vector<std::string> userInput);
  void stopDtmfTone(std::vector<std::string> userInput);
  void sendUssd(std::vector<std::string> userInput);
  void cancelUssd(std::vector<std::string> userInput);
  void lastCallFailCause(std::vector<std::string> userInput);
  void setClir(std::vector<std::string> userInput);
  void getClir(std::vector<std::string> userInput);
  void hangupWaitingOrBackground(std::vector<std::string> userInput);
  void hangupForegroundResumeBackground(std::vector<std::string> userInput);
  void separateConnection(std::vector<std::string> userInput);
  void queryCallForwardStatus(std::vector<std::string> userInput);
  void setCallForward(std::vector<std::string> userInput);
  void queryCallWaiting(std::vector<std::string> userInput);
  void setCallWaiting(std::vector<std::string> userInput);
  void changeBarringPassword(std::vector<std::string> userInput);
  void queryClip(std::vector<std::string> userInput);
  void setSuppSvcNotification(std::vector<std::string> userInput);
  void sendCdmaFlash(std::vector<std::string> userInput);
  void sendCdmaBurstDtmf(std::vector<std::string> userInput);
  void getImsRegState(std::vector<std::string> userInput);
  void explicitCallTransfer(std::vector<std::string> userInput);
  void setMute(std::vector<std::string> userInput);
  void getMute(std::vector<std::string> userInput);
  void setTtyMode(std::vector<std::string> userInput);
  void getTtyMode(std::vector<std::string> userInput);
  void setCdmaVoicePrefMode(std::vector<std::string> userInput);
  void getCdmaVoicePrefMode(std::vector<std::string> userInput);

  int phoneId_;
  //   std::shared_ptr<telux::tel::IPhoneManager> phoneManager_;
  //   std::shared_ptr<telux::tel::ICallListener> callListener_;
  //   std::shared_ptr<telux::tel::ICallManager> callManager_;
  //   std::shared_ptr<MyDialCallback> myDialCallCmdCb_;
  //   std::shared_ptr<MyCallCommandCallback> myHangupCb_;
  //   std::shared_ptr<MyCallCommandCallback> myHoldCb_;
  //   std::shared_ptr<MyCallCommandCallback> myResumeCb_;
  //   std::shared_ptr<MyCallCommandCallback> myAnswerCb_;
  //   std::shared_ptr<MyCallCommandCallback> myRejectCb_;
  //   std::shared_ptr<MyCallCommandCallback> myConferenceCb_;
  //   std::shared_ptr<MyCallCommandCallback> mySwapCb_;
  //   std::shared_ptr<MyCallCommandCallback> myPlayTonesCb_;
  //   std::shared_ptr<MyCallCommandCallback> myStartToneCb_;
  //   std::shared_ptr<MyCallCommandCallback> myStopToneCb_;
};

#endif  // CALLMENU_HPP
