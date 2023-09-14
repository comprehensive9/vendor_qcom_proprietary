/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */

/**
 * Call Menu class provides dialer functionality of the SDK
 * it has menu options for dial, answer, hangup, reject, conference and swap calls
 */

#include <chrono>
#include <iostream>

//#include <telux/tel/PhoneFactory.hpp>

#include "CallMenu.hpp"

CallMenu::CallMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
  std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
  startTime = std::chrono::system_clock::now();
  // Get the PhoneFactory and PhoneManager instances.
  //   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
  //   phoneManager_ = phoneFactory.getPhoneManager();

  // Check if telephony subsystem is ready
  //   bool subSystemStatus = phoneManager_->isSubsystemReady();
  bool subSystemStatus = true;
  // If telephony subsystem is not ready, wait for it to be ready
  //   if(!subSystemStatus) {
  //      std::cout << "\nTelephony subsystem is not ready, Please wait" << std::endl;
  //      std::future<bool> f = phoneManager_->onSubsystemReady();
  //      // Wait unconditionally for telephony subsystem to be ready
  //      subSystemStatus = f.get();
  //   }

  // Exit the application, if SDK is unable to initialize telephony subsystems
  if (subSystemStatus) {
    endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Elapsed Time for Subsystems to ready : " << elapsedTime.count() << "s"
              << std::endl;
  } else {
    std::cout << "ERROR - Unable to initialize subSystem" << std::endl;
    exit(0);
  }

  //   if(subSystemStatus) {
  //      callManager_ = telux::tel::PhoneFactory::getInstance().getCallManager();
  //      myDialCallCmdCb_ = std::make_shared<MyDialCallback>();
  //      myHangupCb_ = std::make_shared<MyCallCommandCallback>("Hang");
  //      myHoldCb_ = std::make_shared<MyCallCommandCallback>("Hold");
  //      myResumeCb_ = std::make_shared<MyCallCommandCallback>("Resume");
  //      myAnswerCb_ = std::make_shared<MyCallCommandCallback>("Answer");
  //      myRejectCb_ = std::make_shared<MyCallCommandCallback>("Reject");
  //      myConferenceCb_ = std::make_shared<MyCallCommandCallback>("Conference");
  //      mySwapCb_ = std::make_shared<MyCallCommandCallback>("Swap");
  //      myPlayTonesCb_ = std::make_shared<MyCallCommandCallback>("Play Tone");
  //      myStartToneCb_ = std::make_shared<MyCallCommandCallback>("Start Tone");
  //      myStopToneCb_ = std::make_shared<MyCallCommandCallback>("Stop Tone");
  //      callListener_ = std::make_shared<MyCallListener>();
  //      // registering listener
  ////      telux::common::Status status = callManager_->registerListener(callListener_);
  ////      if(status != telux::common::Status::SUCCESS) {
  ////         std::cout << "Unable to register Call Manager listener" << std::endl;
  ////      }
  //   }
}

CallMenu::~CallMenu() {
  //   callManager_->removeListener(callListener_);
  //   myDialCallCmdCb_ = nullptr;
  //   myHangupCb_ = nullptr;
  //   myHoldCb_ = nullptr;
  //   myResumeCb_ = nullptr;
  //   myAnswerCb_ = nullptr;
  //   myRejectCb_ = nullptr;
  //   myConferenceCb_ = nullptr;
  //   mySwapCb_ = nullptr;
  //   myPlayTonesCb_ = nullptr;
  //   myStartToneCb_ = nullptr;
  //   myStopToneCb_ = nullptr;
}

void CallMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListCallSubMenu = {
    CREATE_COMMAND(CallMenu::registerForIndications, "RegisterForIndications"),
    CREATE_COMMAND(CallMenu::dial, "Dial", "number"),
    CREATE_COMMAND(CallMenu::acceptCall, "Accept_call"),
    CREATE_COMMAND(CallMenu::rejectCall, "Reject_call"),
    CREATE_COMMAND(CallMenu::hangupWithCallIndex, "Hangup", "call_index"),
    CREATE_COMMAND(CallMenu::holdCall, "Hold"),
    CREATE_COMMAND(CallMenu::resumeCall, "Resume"),
    CREATE_COMMAND(CallMenu::conference, "Conference"),
    CREATE_COMMAND(CallMenu::getCalls, "Get_Current_Calls"),
    CREATE_COMMAND(CallMenu::playDtmfTone, "Play_DTMF_tone", "number * #"),
    CREATE_COMMAND(CallMenu::startDtmfTone, "Start_DTMF_tone", "number * #"),
    CREATE_COMMAND(CallMenu::stopDtmfTone, "Stop_DTMF_tone"),
    CREATE_COMMAND(CallMenu::sendUssd, "SendUSSD", "Ussd"),
    CREATE_COMMAND(CallMenu::cancelUssd, "CancelUSSD"),
    CREATE_COMMAND(CallMenu::lastCallFailCause, "LastCallFailCause"),
    CREATE_COMMAND(CallMenu::setClir, "SetClir"),
    CREATE_COMMAND(CallMenu::getClir, "GetClir"),
    CREATE_COMMAND(CallMenu::hangupWaitingOrBackground, "HangupWaitingOrBackground"),
    CREATE_COMMAND(CallMenu::hangupForegroundResumeBackground, "HangupForegroundResumeBackground"),
    CREATE_COMMAND(CallMenu::separateConnection, "SeparateConnection", "call_index"),
    CREATE_COMMAND(CallMenu::queryCallForwardStatus, "QueryCallForwardStatus"),
    CREATE_COMMAND(CallMenu::setCallForward, "SetCallForward"),
    CREATE_COMMAND(CallMenu::queryCallWaiting, "QueryCallWaiting"),
    CREATE_COMMAND(CallMenu::setCallWaiting, "SetCallWaiting"),
    CREATE_COMMAND(CallMenu::changeBarringPassword, "ChangeBarringPassword"),
    CREATE_COMMAND(CallMenu::queryClip, "QueryClip"),
    CREATE_COMMAND(CallMenu::setSuppSvcNotification, "SetSuppSvcNotification"),
    CREATE_COMMAND(CallMenu::sendCdmaFlash, "SendCdmaFlash"),
    CREATE_COMMAND(CallMenu::sendCdmaBurstDtmf, "SendCdmaBurstDtmf"),
    CREATE_COMMAND(CallMenu::getImsRegState, "GetImsRegState"),
    CREATE_COMMAND(CallMenu::explicitCallTransfer, "ExplicitCallTransfer"),
    CREATE_COMMAND(CallMenu::setMute, "SetMute"),
    CREATE_COMMAND(CallMenu::getMute, "GetMute"),
    CREATE_COMMAND(CallMenu::setTtyMode, "SetTtyMode"),
    CREATE_COMMAND(CallMenu::getTtyMode, "GetTtyMode"),
    CREATE_COMMAND(CallMenu::setCdmaVoicePrefMode, "SetCdmaVoicePrefMode"),
    CREATE_COMMAND(CallMenu::getCdmaVoicePrefMode, "GetCdmaVoicePrefMode"),
  };

  addCommands(commandsListCallSubMenu);
  ConsoleApp::displayMenu();
}

std::ostream& operator<<(std::ostream& os, const RIL_CDMA_OTA_ProvisionStatus& arg) {
  os << "RIL_CDMA_OTA_ProvisionStatus: " << std::endl;
  switch (arg) {
    case CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED:
      os << "CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED";
      break;
    case CDMA_OTA_PROVISION_STATUS_SPC_RETRIES_EXCEEDED:
      os << "CDMA_OTA_PROVISION_STATUS_SPC_RETRIES_EXCEEDED";
      break;
    case CDMA_OTA_PROVISION_STATUS_A_KEY_EXCHANGED:
      os << "CDMA_OTA_PROVISION_STATUS_A_KEY_EXCHANGED";
      break;
    case CDMA_OTA_PROVISION_STATUS_SSD_UPDATED:
      os << "CDMA_OTA_PROVISION_STATUS_SSD_UPDATED";
      break;
    case CDMA_OTA_PROVISION_STATUS_NAM_DOWNLOADED:
      os << "CDMA_OTA_PROVISION_STATUS_NAM_DOWNLOADED";
      break;
    case CDMA_OTA_PROVISION_STATUS_MDN_DOWNLOADED:
      os << "CDMA_OTA_PROVISION_STATUS_MDN_DOWNLOADED";
      break;
    case CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED:
      os << "CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED";
      break;
    case CDMA_OTA_PROVISION_STATUS_PRL_DOWNLOADED:
      os << "CDMA_OTA_PROVISION_STATUS_PRL_DOWNLOADED";
      break;
    case CDMA_OTA_PROVISION_STATUS_COMMITTED:
      os << "CDMA_OTA_PROVISION_STATUS_COMMITTED";
      break;
    case CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED:
      os << "CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED";
      break;
    case CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED:
      os << "CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED";
      break;
    case CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED:
      os << "CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED";
      break;
  }
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_CDMA_RedirectingReason& arg) {
  os << "RIL_CDMA_RedirectingReason: " << std::endl;
  switch (arg) {
    case RIL_REDIRECTING_REASON_UNKNOWN:
      os << "RIL_REDIRECTING_REASON_UNKNOWN";
      break;
    case RIL_REDIRECTING_REASON_CALL_FORWARDING_BUSY:
      os << "RIL_REDIRECTING_REASON_CALL_FORWARDING_BUSY";
      break;
    case RIL_REDIRECTING_REASON_CALL_FORWARDING_NO_REPLY:
      os << "RIL_REDIRECTING_REASON_CALL_FORWARDING_NO_REPLY";
      break;
    case RIL_REDIRECTING_REASON_CALLED_DTE_OUT_OF_ORDER:
      os << "RIL_REDIRECTING_REASON_CALLED_DTE_OUT_OF_ORDER";
      break;
    case RIL_REDIRECTING_REASON_CALL_FORWARDING_BY_THE_CALLED_DTE:
      os << "RIL_REDIRECTING_REASON_CALL_FORWARDING_BY_THE_CALLED_DTE";
      break;
    case RIL_REDIRECTING_REASON_CALL_FORWARDING_UNCONDITIONAL:
      os << "RIL_REDIRECTING_REASON_CALL_FORWARDING_UNCONDITIONAL";
      break;
    case RIL_REDIRECTING_REASON_RESERVED:
      os << "RIL_REDIRECTING_REASON_RESERVED";
      break;
  }
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_SrvccState& arg) {
  os << "RIL_SrvccState: " << std::endl;
  switch (arg) {
    case HANDOVER_STARTED:
      os << "HANDOVER_STARTED";
      break;
    case HANDOVER_COMPLETED:
      os << "HANDOVER_COMPLETED";
      break;
    case HANDOVER_FAILED:
      os << "HANDOVER_FAILED";
      break;
    case HANDOVER_CANCELED:
      os << "HANDOVER_CANCELED";
      break;
  }
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_SsRequestType& arg) {
  os << "RIL_SsRequestType: " << std::endl;
  switch (arg) {
    case SS_ACTIVATION:
      os << "SS_ACTIVATION";
      break;
    case SS_DEACTIVATION:
      os << "SS_DEACTIVATION";
      break;
    case SS_INTERROGATION:
      os << "SS_INTERROGATION";
      break;
    case SS_REGISTRATION:
      os << "SS_REGISTRATION";
      break;
    case SS_ERASURE:
      os << "SS_ERASURE";
      break;
  }
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_SsTeleserviceType& arg) {
  os << "RIL_SsTeleserviceType: " << std::endl;
  switch (arg) {
    case SS_ALL_TELE_AND_BEARER_SERVICES:
      os << "SS_ALL_TELE_AND_BEARER_SERVICES";
      break;
    case SS_ALL_TELESEVICES:
      os << "SS_ALL_TELESEVICES";
      break;
    case SS_TELEPHONY:
      os << "SS_TELEPHONY";
      break;
    case SS_ALL_DATA_TELESERVICES:
      os << "SS_ALL_DATA_TELESERVICES";
      break;
    case SS_SMS_SERVICES:
      os << "SS_SMS_SERVICES";
      break;
    case SS_ALL_TELESERVICES_EXCEPT_SMS:
      os << "SS_ALL_TELESERVICES_EXCEPT_SMS";
      break;
  }
  os << std::endl;
  return os;
}
std::ostream& operator<<(std::ostream& os, const RIL_SsServiceType& arg) {
  os << "RIL_SsServiceType: " << std::endl;
  switch (arg) {
    case SS_CFU:
      os << "SS_CFU";
      break;
    case SS_CF_BUSY:
      os << "SS_CF_BUSY";
      break;
    case SS_CF_NO_REPLY:
      os << "SS_CF_NO_REPLY";
      break;
    case SS_CF_NOT_REACHABLE:
      os << "SS_CF_NOT_REACHABLE";
      break;
    case SS_CF_ALL:
      os << "SS_CF_ALL";
      break;
    case SS_CF_ALL_CONDITIONAL:
      os << "SS_CF_ALL_CONDITIONAL";
      break;
    case SS_CLIP:
      os << "SS_CLIP";
      break;
    case SS_CLIR:
      os << "SS_CLIR";
      break;
    case SS_COLP:
      os << "SS_COLP";
      break;
    case SS_COLR:
      os << "SS_COLR";
      break;
    case SS_WAIT:
      os << "SS_WAIT";
      break;
    case SS_BAOC:
      os << "SS_BAOC";
      break;
    case SS_BAOIC:
      os << "SS_BAOIC";
      break;
    case SS_BAOIC_EXC_HOME:
      os << "SS_BAOIC_EXC_HOME";
      break;
    case SS_BAIC:
      os << "SS_BAIC";
      break;
    case SS_BAIC_ROAMING:
      os << "SS_BAIC_ROAMING";
      break;
    case SS_ALL_BARRING:
      os << "SS_ALL_BARRING";
      break;
    case SS_OUTGOING_BARRING:
      os << "SS_OUTGOING_BARRING";
      break;
    case SS_INCOMING_BARRING:
      os << "SS_INCOMING_BARRING";
      break;
    case SS_INCOMING_BARRING_DN:
      os << "SS_INCOMING_BARRING_DN";
      break;
    case SS_INCOMING_BARRING_ANONYMOUS:
      os << "SS_INCOMING_BARRING_ANONYMOUS";
      break;
  }
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RIL_RadioTechnologyFamily& arg) {
  os << "RIL_RadioTechnologyFamily: " << std::endl;
  switch (arg) {
    case RADIO_TECH_FAMILY_UNKNOWN:
      os << "RADIO_TECH_FAMILY_UNKNOWN";
      break;
    case RADIO_TECH_3GPP:
      os << "RADIO_TECH_3GPP";
      break;
    case RADIO_TECH_3GPP2:
      os << "RADIO_TECH_3GPP2";
      break;
  }
  os << std::endl;
  return os;
}

std::istream& operator >> (std::istream &in, RIL_CallForwardInfo &arg)
{
  std::cout << "0. disable" << std::endl;
  std::cout << "1. enable" << std::endl;
  std::cout << "2. interrogate" << std::endl;
  std::cout << "3. registeration" << std::endl;
  std::cout << "4. erasure" << std::endl;
  std::cout << "Enter status(integer type):" << std::endl;
  in >> arg.status;

  std::cout << "0. unconditional" << std::endl;
  std::cout << "1. mobile busy" << std::endl;
  std::cout << "2. no reply" << std::endl;
  std::cout << "3. not reachable" << std::endl;
  std::cout << "4. all call forwarding" << std::endl;
  std::cout << "5. all conditional call forwarding" << std::endl;
  std::cout << "Enter reason(integer type):" << std::endl;
  in >> arg.reason;

  std::cout << "1. voice (telephony)" << std::endl;
  std::cout << "2. data" << std::endl;
  std::cout << "4. fax" << std::endl;
  std::cout << "8. short message service" << std::endl;
  std::cout << "16. data circuit sync" << std::endl;
  std::cout << "32. data circuit async" << std::endl;
  std::cout << "64. dedicated packet access" << std::endl;
  std::cout << "128. dedicated PAD access" << std::endl;
  std::cout << "Enter serviceClass(sum of integers representing services):" << std::endl;
  in >> arg.serviceClass;

  std::cout << "Enter type of number(145 or 129):" << std::endl;
  in >> arg.toa;

  std::string tempNumber;
  std::cout << "Enter number:" << std::endl;
  in >> tempNumber;
  arg.number = new char[tempNumber.size()+1]{};
  tempNumber.copy(arg.number, tempNumber.size());

  std::cout << "Enter timeSeconds:" << std::endl;
  in >> arg.timeSeconds;
  return in;
}

std::ostream& operator <<(std::ostream& os, const RIL_CallForwardInfo &arg)
{
  os << "RIL_CallForwardInfo : " << std::endl;
  os << "status: " << arg.status << std::endl;
  os << "reason: " << arg.reason << std::endl;
  os << "serviceClass: " << arg.serviceClass << std::endl;
  os << "toa: " << arg.toa << std::endl;
  if (arg.number) {
    os << "number: " << arg.number << std::endl;
  }
  os << "timeSeconds: " << arg.timeSeconds << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_SignalInfoRecord &arg)
{
  os << "RIL_CDMA_SignalInfoRecord : " << std::endl;
  os << "isPresent: " << (int)arg.isPresent << std::endl;
  os << "signalType: " << (int)arg.signalType << std::endl;
  os << "alertPitch: " << (int)arg.alertPitch << std::endl;
  os << "signal: " << (int)arg.signal << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_SuppSvcNotification &arg)
{
  os << "RIL_SuppSvcNotification : " << std::endl;
  os << "notificationType: " << arg.notificationType << std::endl;
  os << "code: " << arg.code << std::endl;
  os << "index: " << arg.index << std::endl;
  os << "type: " << arg.type << std::endl;
  if (arg.number) {
    os << "number: " << arg.number << std::endl;
  }
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_CallWaiting_v6 &arg)
{
  os << "RIL_CDMA_CallWaiting_v6 : " << std::endl;
  if (arg.number) {
    os << "number: " << arg.number << std::endl;
  }
  os << "numberPresentation: " << arg.numberPresentation << std::endl;
  if (arg.name) {
    os << "name: " << arg.name << std::endl;
  }
  os << arg.signalInfoRecord;
  os << "number_type: " << arg.number_type << std::endl;
  os << "number_plan: " << arg.number_plan << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_DisplayInfoRecord &arg)
{
  os << "RIL_CDMA_DisplayInfoRecord : " << std::endl;
  os << "alpha_len: " << arg.alpha_len << std::endl;
  os << "alpha_buf: " << arg.alpha_buf << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_NumberInfoRecord &arg)
{
  os << "RIL_CDMA_NumberInfoRecord : " << std::endl;
  os << "len: " << (int)arg.len << std::endl;
  os << "buf: " << arg.buf << std::endl;
  os << "number_type: " << (int)arg.number_type << std::endl;
  os << "number_plan: " << (int)arg.number_plan << std::endl;
  os << "pi: " << (int)arg.pi << std::endl;
  os << "si: " << (int)arg.si << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_RedirectingNumberInfoRecord &arg)
{
  os << "RIL_CDMA_RedirectingNumberInfoRecord : " << std::endl;
  os << "redirectingNumber: " << arg.redirectingNumber;
  os << "redirectingReason: " << arg.redirectingReason;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_LineControlInfoRecord &arg)
{
  os << "RIL_CDMA_LineControlInfoRecord : " << std::endl;
  os << "lineCtrlPolarityIncluded: " << (int)arg.lineCtrlPolarityIncluded << std::endl;
  os << "lineCtrlToggle: " << (int)arg.lineCtrlToggle << std::endl;
  os << "lineCtrlReverse: " << (int)arg.lineCtrlReverse << std::endl;
  os << "lineCtrlPowerDenial: " << (int)arg.lineCtrlPowerDenial << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_T53_CLIRInfoRecord &arg)
{
  os << "RIL_CDMA_T53_CLIRInfoRecord : " << std::endl;
  os << "cause: " << (int)arg.cause << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_T53_AudioControlInfoRecord &arg)
{
  os << "RIL_CDMA_T53_AudioControlInfoRecord : " << std::endl;
  os << "upLink: " << (int)arg.upLink << std::endl;
  os << "downLink: " << (int)arg.downLink << std::endl;
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_InformationRecord &arg)
{
  os << "RIL_CDMA_InformationRecord : " << std::endl;
  os << "name: " << arg.name << std::endl;
  switch (arg.name) {
    case RIL_CDMA_DISPLAY_INFO_REC:
    case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
      os << arg.rec.display;
      break;
    case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
    case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
    case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
      os << arg.rec.number;
      break;
    case RIL_CDMA_SIGNAL_INFO_REC:
      os << arg.rec.signal;
      break;
    case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
      os << arg.rec.redir;
      break;
    case RIL_CDMA_LINE_CONTROL_INFO_REC:
      os << arg.rec.lineCtrl;
      break;
    case RIL_CDMA_T53_CLIR_INFO_REC:
      os << arg.rec.clir;
      break;
    case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC:
      os << arg.rec.audioCtrl;
      break;
    case RIL_CDMA_T53_RELEASE_INFO_REC:
    default:
      break;
  }
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CDMA_InformationRecords &arg)
{
  os << "RIL_CDMA_InformationRecords : " << std::endl;
  os << "numberOfInfoRecs: " << (int)arg.numberOfInfoRecs << std::endl;
  char numRecords =
      (arg.numberOfInfoRecs > RIL_CDMA_MAX_NUMBER_OF_INFO_RECS ? RIL_CDMA_MAX_NUMBER_OF_INFO_RECS
                                                               : arg.numberOfInfoRecs);
  for (int i = 0; i < numRecords; i++) {
    os << arg.infoRec[i];
  }
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_CfData &arg)
{
  os << "RIL_CfData : " << std::endl;
  os << "numValidIndexes: " << (int)arg.numValidIndexes << std::endl;
  for(size_t i = 0; i < arg.numValidIndexes && i < NUM_SERVICE_CLASSES; i++) {
    os << "cfInfo[" << i << "]:" << std::endl;
    os << arg.cfInfo[i];
  }
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_UssdModeType mode)
{
  os << "UUSD Mode: " << std::endl;
  switch(mode) {
    case RIL_USSD_NOTIFY:
      os << "RIL_USSD_NOTIFY" << std::endl;
      break;
    case RIL_USSD_REQUEST:
      os << "RIL_USSD_REQUEST" << std::endl;
      break;
    case RIL_USSD_NW_RELEASE:
      os << "RIL_USSD_NW_RELEASE" << std::endl;
      break;
    case RIL_USSD_LOCAL_CLIENT:
      os << "RIL_USSD_LOCAL_CLIENT" << std::endl;
      break;
    case RIL_USSD_NOT_SUPPORTED:
      os << "RIL_USSD_NOT_SUPPORTED" << std::endl;
      break;
    case RIL_USSD_NW_TIMEOUT:
      os << "RIL_USSD_NW_TIMEOUT" << std::endl;
      break;
  }
  return os;
}

std::ostream& operator <<(std::ostream& os, const RIL_StkCcUnsolSsResponse &arg)
{
  os << "RIL_StkCcUnsolSsResponse : " << std::endl;
  os << "serviceType: " << arg.serviceType << std::endl;
  os << "requestType: " << arg.requestType << std::endl;
  os << "teleserviceType: " << arg.teleserviceType << std::endl;
  os << "serviceClass: " << arg.serviceClass << std::endl;
  os << "result: " << arg.result << std::endl;
  if ((arg.requestType == SS_INTERROGATION) &&
      (arg.serviceType == SS_CFU ||
       arg.serviceType == SS_CF_BUSY ||
       arg.serviceType == SS_CF_NO_REPLY ||
       arg.serviceType == SS_CF_NOT_REACHABLE ||
       arg.serviceType == SS_CF_ALL ||
       arg.serviceType == SS_CF_ALL_CONDITIONAL)) {
    // CF
    os << "cfData: " << arg.cfData;
  } else {
    for(size_t i = 0; i < SS_INFO_MAX; i++) {
      os << "ssInfo[" << i << "]: " << arg.ssInfo[i] << std::endl;
    }
  }
  return os;
}

void CallMenu::registerForIndications(std::vector<std::string> userInput) {
  rilSession.registerUnsolCallStateChangeIndicationHandler(
      [] () {
          std::cout<<"[CallMenu]: Received Call State Change";
      }
  );

  rilSession.registerCallRingIndicationHandler(
      [](const RIL_CDMA_SignalInfoRecord* record) {
        std::cout << "Received CallRingIndication." << std::endl;
        if (record) {
          std::cout << *record;
        }
      });

  rilSession.registerOnUssdIndicationHandler(
      [](const char& mode, const std::string& message) {
        std::cout << "Received OnUssdIndication." << std::endl;
        std::cout << static_cast<RIL_UssdModeType>(mode);
        std::cout << "message: " << message << std::endl;
      });

  rilSession.registerSuppSvcNotificationIndicationHandler(
      [](const RIL_SuppSvcNotification& suppSvc) {
        std::cout << "Received SuppSvcIndication." << std::endl;
        std::cout << suppSvc;
      });

  rilSession.registerCdmaCallWaitingIndicationHandler(
      [](const RIL_CDMA_CallWaiting_v6& callWaitingRecord) {
        std::cout << "Received CdmaCallWaitingIndication." << std::endl;
        std::cout << callWaitingRecord;
      });

  rilSession.registerCdmaOtaProvisionStatusIndicationHandler(
      [](const RIL_CDMA_OTA_ProvisionStatus& status) {
        std::cout << "Received CdmaOtaProvisionStatusIndication." << std::endl;
        std::cout << status;
      });
  rilSession.registerCdmaInfoRecIndicationHandler(
      [](const RIL_CDMA_InformationRecords& record) {
        std::cout << "Received CdmaInfoRecIndication." << std::endl;
        std::cout << record;
      });
  rilSession.registerSrvccStatusIndicationHandler(
      [](const RIL_SrvccState& state) {
        std::cout << "Received SrvccStatusIndication." << std::endl;
        std::cout << state;
      });
  rilSession.registerOnSupplementaryServiceIndicationHandler(
      [](const RIL_StkCcUnsolSsResponse& state) {
        std::cout << "Received OnSupplementaryServiceIndication." << std::endl;
        std::cout << state;
      });
  rilSession.registerRingBackToneIndicationHandler(
      [](const int& tone) {
        std::cout << "Received RingBackToneIndication." << std::endl;
        std::cout << "play tone: " << tone << std::endl;
      });
  rilSession.registerImsNetworkStateChangeIndicationHandler(
      []() {
        std::cout << "Received ImsNetworkStateChangeIndication." << std::endl;
      });
}

void CallMenu::dial(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_Dial dialParams;
  dialParams.address = const_cast<char*>(userInput[1].c_str());
  dialParams.clir = 0;
  dialParams.uusInfo = nullptr;

  Status s = rilSession.dial(dialParams, [](RIL_Errno err) -> void {
    std::cout << "Got response for dial request: " << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Dialed successfully" : "Failed to Dial") << std::endl;
}

void CallMenu::sendUssd(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2) {
    std::cout << "Incorrect number of parameters. Exiting.." << std::endl;
    return;
  }

  Status s = rilSession.sendUssd(userInput[1], [](RIL_Errno err) -> void {
    std::cout << "Got response for SendUSSD request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "SendUSSD request sent succesfully"
                                       : "Failed to send SendUSSD")
            << std::endl;
}

void CallMenu::cancelUssd(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.cancelUssd([](RIL_Errno err) -> void {
    std::cout << "Got response for CancelUSSD request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "CancelUSSD request sent succesfully"
                                       : "Failed to send CancelUSSD")
            << std::endl;
}

void CallMenu::setClir(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int type;
  std::cout << "Select one of the below options" << std::endl;
  std::cout << "0 - Subscription Default" << std::endl;
  std::cout << "1- CLIR invocation" << std::endl;
  std::cout << "2 - CLIR Suppression" << std::endl;
  std::cout << "Enter Choice:";
  std::cin >> type;
  Status s = rilSession.setClir(&type, [](RIL_Errno err) -> void {
    std::cout << "Got response for SetClir request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "SetClir request sent succesfully"
                                       : "Failed to send SetClir")
            << std::endl;
}

void CallMenu::getClir(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getClir([](RIL_Errno err, int m, int n) -> void {
    std::cout << "Got response for GetClir request." << err << std::endl;
    std::cout << "m: " <<  m << " n: " << n << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "GetClir request sent succesfully"
                                       : "Failed to send GetClir")
            << std::endl;
}

void CallMenu::hangupWaitingOrBackground(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status status = rilSession.hangupWaitingOrBackground([](RIL_Errno err) -> void {
    std::cout << "Got response for hangupWaitingOrBackground request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send hangupWaitingOrBackground" << std::endl;
  }
}

void CallMenu::hangupForegroundResumeBackground(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status status = rilSession.hangupForegroundResumeBackground([](RIL_Errno err) -> void {
    std::cout << "Got response for hangupForegroundResumeBackground request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send hangupForegroundResumeBackground" << std::endl;
  }
}

void CallMenu::separateConnection(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2) {
    std::cout << "Incorrect number of parameters. Exiting.." << std::endl;
    return;
  }

  auto connid = std::stoi(userInput[1]);
  Status status = rilSession.separateConnection(connid, [](RIL_Errno err) -> void {
    std::cout << "Got response for separateConnection request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send separateConnection" << std::endl;
  }
}

void CallMenu::queryCallForwardStatus(std::vector<std::string> userInput) {
  size_t numSsInfos = 1;
  RIL_CallForwardInfo callFwdInfo{};
  std::cin >> callFwdInfo;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.queryCallForwardStatus(
      callFwdInfo,
      [](RIL_Errno err, const size_t numCallFwdInfos,
         const RIL_CallForwardInfo** callFwdInfos) -> void {
        std::cout << "Got response for queryCallForwardStatus request." << err << std::endl;
        std::cout << "numCallFwdInfos: " << numCallFwdInfos << std::endl;
        if (callFwdInfos) {
          for (int i = 0; i < numCallFwdInfos; i++) {
            if (callFwdInfos[i]) {
              std::cout << *(callFwdInfos[i]);
            }
          }
        }
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send queryCallForwardStatus" << std::endl;
  }
}

void CallMenu::setCallForward(std::vector<std::string> userInput) {
  RIL_CallForwardInfo callFwdInfo{};
  std::cin >> callFwdInfo;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.setCallForward(callFwdInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for setCallForward request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send setCallForward" << std::endl;
  }
}

void CallMenu::queryCallWaiting(std::vector<std::string> userInput) {
  int serviceClass = 0;
  std::cout << "1. voice (telephony)" << std::endl;
  std::cout << "2. data" << std::endl;
  std::cout << "4. fax" << std::endl;
  std::cout << "8. short message service" << std::endl;
  std::cout << "16. data circuit sync" << std::endl;
  std::cout << "32. data circuit async" << std::endl;
  std::cout << "64. dedicated packet access" << std::endl;
  std::cout << "128. dedicated PAD access" << std::endl;
  std::cout << "Enter serviceClass(sum of integers representing services):" << std::endl;
  std::cin >> serviceClass;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.queryCallWaiting(
      serviceClass,
      [](RIL_Errno err, const int enabled, const int serviceClass) -> void {
        std::cout << "Got response for queryCallWaiting request." << err << std::endl;
        std::cout << "serviceClass: " << serviceClass << std::endl;
        std::cout << "enabled: " << enabled << std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send queryCallWaiting" << std::endl;
  }
}

void CallMenu::setCallWaiting(std::vector<std::string> userInput) {
  int serviceClass = 0;
  int enabled = 0;
  std::cout << "1. voice (telephony)" << std::endl;
  std::cout << "2. data" << std::endl;
  std::cout << "4. fax" << std::endl;
  std::cout << "8. short message service" << std::endl;
  std::cout << "16. data circuit sync" << std::endl;
  std::cout << "32. data circuit async" << std::endl;
  std::cout << "64. dedicated packet access" << std::endl;
  std::cout << "128. dedicated PAD access" << std::endl;
  std::cout << "Enter serviceClass(sum of integers representing services):" << std::endl;
  std::cin >> serviceClass;
  std::cout << "Enter enabled(1 for TRUE, 0 for FALSE):" << std::endl;
  std::cin >> enabled;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.setCallWaiting(serviceClass, enabled, [](RIL_Errno err) -> void {
    std::cout << "Got response for setCallWaiting request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send setCallWaiting" << std::endl;
  }
}

void CallMenu::changeBarringPassword(std::vector<std::string> userInput) {
  std::string facility;
  std::string oldPassword;
  std::string newPassword;
  std::cout << "AO. BAOC (Barr All Outgoing Calls)" << std::endl;
  std::cout << "OI. BOIC (Barr Outgoing International Calls)" << std::endl;
  std::cout << "OX. BOIC exHC (Barr Outgoing International Calls except to Home Country)"
            << std::endl;
  std::cout << "AI. BAIC (Barr All Incoming Calls)" << std::endl;
  std::cout << "IR. BIC Roam (Barr Incoming Calls when Roaming outside the home country)"
            << std::endl;
  std::cout << "AB. All Barring services" << std::endl;
  std::cout << "AG. All outGoing barring services" << std::endl;
  std::cout << "AC. All inComing barring services" << std::endl;
  std::cout << "Enter Facility: " << std::endl;
  std::cin >> facility;
  std::cout << "Enter OldPassword: " << std::endl;
  std::cin >> oldPassword;
  std::cout << "Enter NewPassword: " << std::endl;
  std::cin >> newPassword;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.changeBarringPassword(
      facility, oldPassword, newPassword, [](RIL_Errno err) -> void {
        std::cout << "Got response for changeBarringPassword request." << err << std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send changeBarringPassword" << std::endl;
  }
}

void CallMenu::queryClip(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.queryClip([](RIL_Errno err, const int status) -> void {
    std::cout << "Got response for queryClip request." << err << std::endl;
    std::cout << "status: " << status << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send queryClip" << std::endl;
  }
}

void CallMenu::setSuppSvcNotification(std::vector<std::string> userInput) {
  int enabled;
  std::cout << "Enter enabled (1 for TRUE, 0 for FALSE): " << std::endl;
  std::cin >> enabled;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.setSuppSvcNotification(enabled, [](RIL_Errno err) -> void {
    std::cout << "Got response for setSuppSvcNotification request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send setSuppSvcNotification" << std::endl;
  }
}

void CallMenu::sendCdmaFlash(std::vector<std::string> userInput) {
  std::string flash;
  std::cout << "Enter flash message: " << std::endl;
  std::cin >> flash;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.sendCdmaFlash(flash, [](RIL_Errno err) -> void {
    std::cout << "Got response for sendCdmaFlash request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send sendCdmaFlash" << std::endl;
  }
}

void CallMenu::sendCdmaBurstDtmf(std::vector<std::string> userInput) {
  std::string dtmfString;
  std::cout << "Enter dtmf string: " << std::endl;
  std::cin >> dtmfString;
  int on = 0;
  std::cout << "Enter dtmf on length (in ms; 0 to use default): " << std::endl;
  std::cin >> on;
  int off = 0;
  std::cout << "Enter dtmf off length (in ms; 0 to use default): " << std::endl;
  std::cin >> off;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.sendCdmaBurstDtmf(dtmfString, on, off, [](RIL_Errno err) -> void {
    std::cout << "Got response for sendCdmaBurstDtmf request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send sendCdmaBurstDtmf" << std::endl;
  }
}

void CallMenu::getImsRegState(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.getImsRegState(
      [](RIL_Errno err, const bool& registered, const RIL_RadioTechnologyFamily& rat) -> void {
        std::cout << "Got response for getImsRegState request." << err << std::endl;
        std::cout << "registered = " << registered << std::endl;
        std::cout << "rat = " << rat << std::endl;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send getImsRegState" << std::endl;
  }
}

void CallMenu::acceptCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.answer([](RIL_Errno err) -> void {
    std::cout << "Got response for Answer request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Answer request sent succesfully" : "Failed to send Answer")
            << std::endl;
}

void CallMenu::rejectCall(std::vector<std::string> userInput) {
  std::mutex callMutex;
  std::condition_variable callCv;
  std::optional<bool> callPresent;
  using namespace std::literals;

  Status s = rilSession.getCurrentCalls(
      [&](RIL_Errno err, const size_t numCalls, const RIL_Call** calls) -> void {
        for (int i = 0; i < numCalls; i++) {
          if (calls[i]->state == RIL_CALL_INCOMING) {
            {
              std::unique_lock<std::mutex> lk(callMutex);
              callPresent = true;
            }
            callCv.notify_all();
            return;
          }
        }
        {
          std::unique_lock<std::mutex> lk(callMutex);
          callPresent = false;
        }
        callCv.notify_all();
      });
  if (s != Status::SUCCESS) {
    std::cout << "Unable to get current calls" << std::endl;
    return;
  }

  std::unique_lock<std::mutex> lk(callMutex);
  callCv.wait_for(lk, 2s, [&callPresent] { return !!callPresent; });

  if (callPresent) {
    if (*callPresent) {
      Status s = rilSession.udub([](RIL_Errno err) -> void {
        std::cout << "Got response for Reject request." << err << std::endl;
      });
      std::cout << ((s == Status::SUCCESS) ? "Reject request sent succesfully"
                                           : "Failed to send Reject")
                << std::endl;
    } else {
      std::cout << "No Incoming call to Reject" << std::endl;
    }
  } else {
    std::cout << "Unable to get current calls resp" << std::endl;
  }
}

void CallMenu::rejectWithSms(std::vector<std::string> userInput) {
  //   std::shared_ptr<telux::tel::ICall> spCall = nullptr;
  //   // Fetch the list of in progress calls from CallManager and reject the incoming call with
  //   // sms.
  //   std::vector<std::shared_ptr<telux::tel::ICall>> inProgressCalls
  //      = callManager_->getInProgressCalls();
  //   for(auto callIterator = std::begin(inProgressCalls); callIterator != std::end(inProgressCalls);
  //       ++callIterator) {
  //      if((*callIterator)->getCallState() == telux::tel::CallState::CALL_INCOMING) {
  //         spCall = *callIterator;
  //         break;
  //      }
  //   }
  //   if(spCall) {
  //      spCall->reject("Testing reject with reason", myRejectCb_);
  //   } else {
  //      std::cout << "No incoming call" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CallMenu::hangupDialingOrAlerting(std::vector<std::string> userInput) {
  //   std::shared_ptr<telux::tel::ICall> spCall = nullptr;
  //   int noOfExistingCalls = 0;
  //   // Iterate through the call list in the application and hangup the first Call that is
  //   // in Dialing or Alerting state
  //   std::vector<std::shared_ptr<telux::tel::ICall>> inProgressCalls
  //      = callManager_->getInProgressCalls();
  //   for(auto callIterator = std::begin(inProgressCalls); callIterator != std::end(inProgressCalls);
  //       ++callIterator) {
  //      if((*callIterator)->getCallState() != telux::tel::CallState::CALL_ENDED) {
  //         noOfExistingCalls++;
  //         spCall = *callIterator;
  //      }
  //   }
  //   if(noOfExistingCalls > 1) {
  //      std::cout << "More than one call: use Hangup cmd with Index " << std::endl;
  //      return;
  //   }
  //   if(spCall) {
  //      spCall->hangup(myHangupCb_);
  //   } else {
  //      std::cout << "No dialing or alerting call found" << std::endl;
  //   }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
}

void CallMenu::hangupWithCallIndex(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2) {
    std::cout << "Incorrect number of parameters. Exiting.." << std::endl;
    return;
  }

  auto connid = std::stoi(userInput[1]);
  Status s = rilSession.hangup(connid, [](RIL_Errno err) -> void {
    std::cout << "Got response for Hangup request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Hangup request sent succesfully" : "Failed to send Hangup")
            << std::endl;
}

void CallMenu::holdCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  std::mutex callMutex;
  std::condition_variable callCv;
  std::optional<bool> callPresent;
  using namespace std::literals;

  Status s = rilSession.getCurrentCalls(
      [&](RIL_Errno err, const size_t numCalls, const RIL_Call** calls) -> void {
        for (int i = 0; i < numCalls; i++) {
          if (calls[i]->state == RIL_CALL_ACTIVE) {
            {
              std::unique_lock<std::mutex> lk(callMutex);
              callPresent = true;
            }
            callCv.notify_all();
            return;
          }
        }
        {
          std::unique_lock<std::mutex> lk(callMutex);
          callPresent = false;
        }
        callCv.notify_all();
      });
  if (s != Status::SUCCESS) {
    std::cout << "Unable to get current calls" << std::endl;
    return;
  }

  std::unique_lock<std::mutex> lk(callMutex);
  callCv.wait_for(lk, 2s, [&callPresent] { return !!callPresent; });

  if (callPresent) {
    if (*callPresent) {
      Status s = rilSession.switchWaitingOrHoldingAndActive([](RIL_Errno err) -> void {
        std::cout << "Got response for Hold request." << err << std::endl;
      });
      std::cout << ((s == Status::SUCCESS) ? "Hold request sent succesfully" : "Failed to send Hold")
                << std::endl;
    } else {
      std::cout << "Active call needed for HOLD" << std::endl;
    }
  } else {
    std::cout << "Unable to get current calls resp" << std::endl;
  }
}

void CallMenu::conference(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.conference([](RIL_Errno err) -> void {
    std::cout << "Got response for Conference request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "Conference request sent succesfully"
                                       : "Failed to send Conference")
            << std::endl;
}

void CallMenu::swap(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  std::mutex callMutex;
  std::condition_variable callCv;
  std::optional<bool> callPresent;
  using namespace std::literals;

  Status s = rilSession.getCurrentCalls(
      [&](RIL_Errno err, const size_t numCalls, const RIL_Call** calls) -> void {
        {
          std::unique_lock<std::mutex> lk(callMutex);
          callPresent = (numCalls > 1) ? true : false;
        }
        callCv.notify_all();
      });
  if (s != Status::SUCCESS) {
    std::cout << "Unable to get current calls" << std::endl;
    return;
  }

  std::unique_lock<std::mutex> lk(callMutex);
  callCv.wait_for(lk, 2s, [&callPresent] { return !!callPresent; });

  if (callPresent) {
    if (*callPresent) {
      Status s = rilSession.switchWaitingOrHoldingAndActive([](RIL_Errno err) -> void {
        std::cout << "Got response for Swap request." << err << std::endl;
      });
      std::cout << ((s == Status::SUCCESS) ? "Swap request sent succesfully" : "Failed to send Swap")
                << std::endl;
    } else {
      std::cout << "2 calls needed for swap" << std::endl;
    }
  } else {
    std::cout << "Unable to get current calls resp" << std::endl;
  }
}

const std::string getAudioQualityString(RIL_AudioQuality audioQuality) {
  std::string ret = "NONE";
  switch (audioQuality) {
    case RIL_AUDIO_QUAL_UNSPECIFIED:
      ret = "RIL_AUDIO_QUAL_UNSPECIFIED";
      break;
    case RIL_AUDIO_QUAL_AMR:
      ret = "RIL_AUDIO_QUAL_AMR";
      break;
    case RIL_AUDIO_QUAL_AMR_WB:
      ret = "RIL_AUDIO_QUAL_AMR_WB";
      break;
    case RIL_AUDIO_QUAL_GSM_EFR:
      ret = "RIL_AUDIO_QUAL_GSM_EFR";
      break;
    case RIL_AUDIO_QUAL_GSM_FR:
      ret = "RIL_AUDIO_QUAL_GSM_FR";
      break;
    case RIL_AUDIO_QUAL_GSM_HR:
      ret = "RIL_AUDIO_QUAL_GSM_HR";
      break;
    case RIL_AUDIO_QUAL_EVRC:
      ret = "RIL_AUDIO_QUAL_EVRC";
      break;
    case RIL_AUDIO_QUAL_EVRC_B:
      ret = "RIL_AUDIO_QUAL_EVRC_B";
      break;
    case RIL_AUDIO_QUAL_EVRC_WB:
      ret = "RIL_AUDIO_QUAL_EVRC_WB";
      break;
    case RIL_AUDIO_QUAL_EVRC_NW:
      ret = "RIL_AUDIO_QUAL_EVRC_NW";
      break;
  }
  return ret;
}

const std::string getCallStateString(RIL_CallState state) {
  std::string ret = "NONE";
  switch (state) {
    case 0:
      ret = "CALL_ACTIVE";
      break;
    case 1:
      ret = "CALL_HOLDING";
      break;
    case 2:
      ret = "CALL_DIALING";
      break;
    case 3:
      ret = "CALL_ALERTING";
      break;
    case 4:
      ret = "CALL_INCOMING";
      break;
    case 5:
      ret = "CALL_WAITING";
      break;
    case 6:
      ret = "CALL_END";
      break;
  }
  return ret;
}

void printCall(const RIL_Call* call) {
  std::cout << "Call Index: " << call->index << std::endl
            << "Call State: " << getCallStateString(call->state) << std::endl
            << "Call Direction: " << ((call->isMT) ? ("Mobile Terminated") : ("Mobile Originated"))
            << std::endl
            << "Phone Number: " << call->number << std::endl
            << "numberPresentation: " << call->numberPresentation << std::endl
            << "toa: " << call->toa << std::endl
            << "isMpty: " << static_cast<uint32_t>(call->isMpty) << std::endl
            << "isMT: " << static_cast<uint32_t>(call->isMT) << std::endl
            << "als: " << static_cast<uint32_t>(call->als) << std::endl
            << "isVoice: " << static_cast<uint32_t>(call->isVoice) << std::endl
            << "isVoicePrivacy: " << static_cast<uint32_t>(call->isVoicePrivacy) << std::endl
            << "toa: " << call->toa << std::endl
            << "name: " << (call->name ? call->name : " ") << std::endl
            << "namePresentation: " << call->namePresentation << std::endl
            << "redirNum: " << (call->redirNum ? call->redirNum : " ") << std::endl
            << "redirNumPresentation: " << call->redirNumPresentation << std::endl
            << "audioQuality: " << getAudioQualityString(call->audioQuality) << std::endl;
}

void CallMenu::getCalls(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getCurrentCalls(
      [](RIL_Errno err, const size_t numCalls, const RIL_Call** calls) -> void {
        std::cout << "Got response for Current Calls request." << err << std::endl;
        for (int i = 0; i < numCalls; i++) {
          std::cout << "Call " << i + 1 << ": " << std::endl;
          printCall(calls[i]);
          std::cout << std::endl;
        }
      });
  std::cout << ((s == Status::SUCCESS) ? "Current calls request sent succesfully"
                                       : "Failed to send Current calls request")
            << std::endl;
}

void CallMenu::resumeCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  std::mutex callMutex;
  std::condition_variable callCv;
  std::optional<bool> callPresent;
  using namespace std::literals;

  Status s = rilSession.getCurrentCalls(
      [&](RIL_Errno err, const size_t numCalls, const RIL_Call** calls) -> void {
        for (int i = 0; i < numCalls; i++) {
          if (calls[i]->state == RIL_CALL_HOLDING) {
            {
              std::unique_lock<std::mutex> lk(callMutex);
              callPresent = true;
            }
            callCv.notify_all();
            return;
          }
        }
        {
          std::unique_lock<std::mutex> lk(callMutex);
          callPresent = false;
        }
        callCv.notify_all();
      });
  if (s != Status::SUCCESS) {
    std::cout << "Unable to get current calls" << std::endl;
    return;
  }

  std::unique_lock<std::mutex> lk(callMutex);
  callCv.wait_for(lk, 2s, [&callPresent] { return !!callPresent; });

  if (callPresent) {
    if (*callPresent) {
      Status s = rilSession.switchWaitingOrHoldingAndActive([](RIL_Errno err) -> void {
        std::cout << "Got response for Resume request." << err << std::endl;
      });
      std::cout << ((s == Status::SUCCESS) ? "Resume request sent succesfully"
                                           : "Failed to send Resume")
                << std::endl;
    } else {
      std::cout << "Held call needed for Resume" << std::endl;
    }
  } else {
    std::cout << "Unable to get current calls resp" << std::endl;
  }
}

void CallMenu::playDtmfTone(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2 || userInput[1].empty()) {
    std::cout << "Incorrect arguments. Exiting.." << std::endl;
    return;
  }

  Status s = rilSession.dtmf(userInput[1][0], [](RIL_Errno err) -> void {
    std::cout << "Got response for DTMF request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "DTMF request sent succesfully" : "Failed to send DTMF")
            << std::endl;
}

void CallMenu::startDtmfTone(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  if (userInput.size() < 2 || userInput[1].empty()) {
    std::cout << "Incorrect arguments. Exiting.." << std::endl;
    return;
  }

  Status s = rilSession.dtmfStart(userInput[1][0], [](RIL_Errno err) -> void {
    std::cout << "Got response for StartDTMF request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "StartDTMF request sent succesfully"
                                       : "Failed to send StartDTMF")
            << std::endl;
}

void CallMenu::stopDtmfTone(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.dtmfStop([](RIL_Errno err) -> void {
    std::cout << "Got response for StopDTMF request." << err << std::endl;
  });
  std::cout << ((s == Status::SUCCESS) ? "StopDTMF request sent succesfully"
                                       : "Failed to send StopDTMF")
            << std::endl;
}

void CallMenu::lastCallFailCause(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.lastCallFailCause(
      [](RIL_Errno err, const RIL_LastCallFailCauseInfo& cause) -> void {
        std::cout << "Got response for Last call fail cause request:" << err << std::endl;
        std::cout << "Cause code: " << cause.cause_code << std::endl;
        if (cause.vendor_cause) {
          std::cout << "Vendor cause: " << cause.vendor_cause << std::endl;
        }
      });
  std::cout << ((s == Status::SUCCESS) ? "Request sent succesfully" : "Failed to send request")
            << std::endl;
}

void CallMenu::explicitCallTransfer(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.explicitCallTransfer([](RIL_Errno err) -> void {
    std::cout << "Got response for ExplicitCallTransfer request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send explicitCallTransfer" << std::endl;
  }
}

void CallMenu::setMute(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  bool mute;
  std::cout << "Enter mute:" <<std::endl;
  std::cin >> mute;
  Status s = rilSession.setMute(mute, [](RIL_Errno err) -> void {
    std::cout << "Got response for SetMute request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send setMute" << std::endl;
  }
}

void CallMenu::getMute(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getMute(
      [](RIL_Errno err, int mute) -> void {
        std::cout << "Got response for GetMute request:" << err << std::endl;
        std::cout << "mute: " << mute << std::endl;
      });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send getMute" << std::endl;
  }
}

void CallMenu::setTtyMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int mode;
  std::cout << "Enter mode:" <<std::endl;
  std::cin >> mode;
  Status s = rilSession.setTtyMode(mode, [](RIL_Errno err) -> void {
    std::cout << "Got response for setTtyMode request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send setTtyMode" << std::endl;
  }
}

void CallMenu::getTtyMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getTtyMode([](RIL_Errno err, int mode) -> void {
    std::cout << "Got response for getTtyMode request." << err << std::endl;
    std::cout << "mode: " << mode << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send getTtyMode" << std::endl;
  }
}

void CallMenu::setCdmaVoicePrefMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  int mode;
  std::cout << "Enter mode:" <<std::endl;
  std::cin >> mode;
  Status s = rilSession.setCdmaVoicePrefMode(mode, [](RIL_Errno err) -> void {
    std::cout << "Got response for setCdmaVoicePrefMode request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send setCdmaVoicePrefMode" << std::endl;
  }
}

void CallMenu::getCdmaVoicePrefMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.getCdmaVoicePrefMode([](RIL_Errno err, int mode) -> void {
    std::cout << "Got response for getCdmaVoicePrefMode request." << err << std::endl;
    std::cout << "mode: " << mode << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send getCdmaVoicePrefMode" << std::endl;
  }
}
