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

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "ImsMenu.hpp"
#include "Phone/PhoneMenu.hpp"

using namespace std;

ostream& operator<<(ostream& os, const RIL_IMS_Registration& imsRegInfo);

ImsMenu::ImsMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
    : ConsoleApp(appName, cursor), rilSession(rilSession) {
}

ImsMenu::~ImsMenu() {
}

void ImsMenu::init() {
  std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListImsSubMenu = {
    CREATE_COMMAND(ImsMenu::registrationStatus, "Registration_Status"),
    CREATE_COMMAND(ImsMenu::imsDial, "Dial", "Number"),
    CREATE_COMMAND(ImsMenu::acceptCall, "Answer"),
    CREATE_COMMAND(ImsMenu::hangupCall, "Hangup"),
    CREATE_COMMAND(ImsMenu::imsRegistrationChange, "RegistrationChange"),
    CREATE_COMMAND(ImsMenu::imsQueryServiceStatus, "QueryServiceStatus"),
    CREATE_COMMAND(ImsMenu::imsSetServiceStatus, "SetServiceStatus"),
    CREATE_COMMAND(ImsMenu::imsGetImsSubConfig, "GetImsSubConfig"),
    CREATE_COMMAND(ImsMenu::imsDtmf, "imsDtmf"),
    CREATE_COMMAND(ImsMenu::imsDtmfStart, "imsDtmfStart"),
    CREATE_COMMAND(ImsMenu::imsDtmfStop, "imsDtmfStop"),
    CREATE_COMMAND(ImsMenu::imsSendSms, "Send SMS Over IMS"),
    CREATE_COMMAND(ImsMenu::imsAckSms, "Ack SMS Received Over IMS"),
    CREATE_COMMAND(ImsMenu::imsGetSmsFormat, "Get Current SMS Format"),
    CREATE_COMMAND(ImsMenu::imsSetCallWaiting, "Set IMS Call Waiting"),
    CREATE_COMMAND(ImsMenu::imsQueryCallWaiting, "Query IMS Call Waiting Status"),
    CREATE_COMMAND(ImsMenu::registerForImsIndications, "Register_Indications"),
    CREATE_COMMAND(ImsMenu::imsModifyCallInitiate, "ImsModifyCallInitiate"),
    CREATE_COMMAND(ImsMenu::imsModifyCallConfirm, "ImsModifyCallConfirm"),
    CREATE_COMMAND(ImsMenu::imsCancelModifyCall, "ImsCancelModifyCall"),
    CREATE_COMMAND(ImsMenu::imsRequestConference, "ImsRequestConference"),
    CREATE_COMMAND(ImsMenu::imsRequestHoldCall, "ImsRequestHoldCall"),
    CREATE_COMMAND(ImsMenu::imsRequestResumeCall, "ImsRequestResumeCall"),
    CREATE_COMMAND(ImsMenu::imsRequestExitEmergencyCallbackMode, "ImsRequestExitEmergencyCallbackMode"),
    CREATE_COMMAND(ImsMenu::imsSendRttMessage, "ImsSendRttMessage"),
    CREATE_COMMAND(ImsMenu::imsSendGeolocationInfo, "ImsSendGeolocationInfo"),
    CREATE_COMMAND(ImsMenu::imsGetClir, "GetClir"),
    CREATE_COMMAND(ImsMenu::imsSetClir, "SetClir"),
    CREATE_COMMAND(ImsMenu::imsExplicitCallTransfer, "ExplicitCallTransfer"),
    CREATE_COMMAND(ImsMenu::imsSendUiTtyMode, "SendUiTtyMode"),
    CREATE_COMMAND(ImsMenu::imsDeflectCall, "DeflectCall"),
    CREATE_COMMAND(ImsMenu::imsQueryClip, "QueryClip"),
    CREATE_COMMAND(ImsMenu::imsSetSuppSvcNotification, "SetSuppSvcNotification"),
    CREATE_COMMAND(ImsMenu::imsSetColr, "SetColr"),
    CREATE_COMMAND(ImsMenu::imsGetColr, "GetColr"),
    CREATE_COMMAND(ImsMenu::imsRegisterMultiIdentityLines, "RegisterMultiIdentityLines"),
    CREATE_COMMAND(ImsMenu::imsQueryVirtualLineInfo, "QueryVirtualLineInfo"),
  };

  addCommands(commandsListImsSubMenu);
  ConsoleApp::displayMenu();
}

istream& operator >> (istream &in, RIL_IMS_CallType &callType)
{
    int var;
    cout << "0. RIL_IMS_CALL_TYPE_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_CALL_TYPE_VOICE" << std::endl;
    cout << "2. RIL_IMS_CALL_TYPE_VT_TX" << std::endl;
    cout << "3. RIL_IMS_CALL_TYPE_VT_RX" << std::endl;
    cout << "4. RIL_IMS_CALL_TYPE_VT" << std::endl;
    cout << "5. RIL_IMS_CALL_TYPE_VT_NODIR" << std::endl;
    cout << "6. RIL_IMS_CALL_TYPE_SMS" << std::endl;
    cout << "7. RIL_IMS_CALL_TYPE_UT" << std::endl;
    do {
        cout << "Enter Call Type:";
        in >> var;
    } while (var < 0 || var > 7);
    callType = static_cast<RIL_IMS_CallType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_CallDomain &arg)
{
    int var;
    cout << "0. RIL_IMS_CALLDOMAIN_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_CALLDOMAIN_CS" << std::endl;
    cout << "2. RIL_IMS_CALLDOMAIN_PS" << std::endl;
    cout << "3. RIL_IMS_CALLDOMAIN_AUTOMATIC" << std::endl;
    cout << "4. RIL_IMS_CALLDOMAIN_NOT_SET" << std::endl;
    cout << "5. RIL_IMS_CALLDOMAIN_INVALID" << std::endl;
    do {
        cout << "Enter Call Domain:";
        in >> var;
    } while (var < 0 || var > 5);
    arg = static_cast<RIL_IMS_CallDomain>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_IpPresentation &ipPresentation)
{
    int var;
    cout << "0. RIL_IMS_IP_PRESENTATION_NUM_ALLOWED" << std::endl;
    cout << "1. RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED" << std::endl;
    cout << "2. RIL_IMS_IP_PRESENTATION_NUM_DEFAULT" << std::endl;
    cout << "3. RIL_IMS_IP_PRESENTATION_INVALID" << std::endl;
    do {
        cout << "Enter IP Presentation Type:";
        in >> var;
    } while (var < 0 || var > 3);
    ipPresentation = static_cast<RIL_IMS_IpPresentation>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_RttMode &rttMode)
{
    int var;
    cout << "0. RIL_IMS_IP_PRESENTATION_NUM_ALLOWED" << std::endl;
    cout << "1. RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED" << std::endl;
    do {
        in >> var;
    } while (var < 0 || var > 1);
    rttMode = static_cast<RIL_IMS_RttMode>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_RegistrationState &state)
{
    int var;
    cout << "0. RIL_IMS_REG_STATE_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_REG_STATE_REGISTERED" << std::endl;
    cout << "2. RIL_IMS_REG_STATE_NOT_REGISTERED" << std::endl;
    cout << "3. RIL_IMS_REG_STATE_REGISTERING" << std::endl;
    do {
        in >> var;
    } while (var < 0 || var > 3);
    state = static_cast<RIL_IMS_RegistrationState>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_StatusType &status)
{
    int var;
    cout << "0. RIL_IMS_STATUS_DISABLED" << std::endl;
    cout << "1. RIL_IMS_STATUS_PARTIALLY_ENABLED" << std::endl;
    cout << "2. RIL_IMS_STATUS_ENABLED" << std::endl;
    cout << "3. RIL_IMS_STATUS_NOT_SUPPORTED" << std::endl;
    do {
        cout << "Enter Choice:" << std::endl;
        in >> var;
    } while (var < 0 || var > 3);
    status = static_cast<RIL_IMS_StatusType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_RadioTechnology &tech)
{
    int var;
    cout << "0. RADIO_TECH_UNKNOWN" << std::endl;
    cout << "1. RADIO_TECH_GPRS" << std::endl;
    cout << "2. RADIO_TECH_EDGE" << std::endl;
    cout << "3. RADIO_TECH_UMTS" << std::endl;
    cout << "4. RADIO_TECH_IS95A" << std::endl;
    cout << "5. RADIO_TECH_IS95B" << std::endl;
    cout << "6. RADIO_TECH_1xRTT" << std::endl;
    cout << "7. RADIO_TECH_EVDO_0" << std::endl;
    cout << "8. RADIO_TECH_EVDO_A" << std::endl;
    cout << "9. RADIO_TECH_HSDPA" << std::endl;
    cout << "10. RADIO_TECH_HSUPA" << std::endl;
    cout << "11. RADIO_TECH_HSPA" << std::endl;
    cout << "12. RADIO_TECH_EVDO_B" << std::endl;
    cout << "13. RADIO_TECH_EHRPD" << std::endl;
    cout << "14. RADIO_TECH_LTE" << std::endl;
    cout << "15. RADIO_TECH_HSPAP" << std::endl;
    cout << "16. RADIO_TECH_GSM" << std::endl;
    cout << "17. RADIO_TECH_TD_SCDMA" << std::endl;
    cout << "18. RADIO_TECH_IWLAN" << std::endl;
    cout << "19. RADIO_TECH_LTE_CA" << std::endl;
    cout << "20. RADIO_TECH_5G" << std::endl;
    cout << "21. RADIO_TECH_WIFI" << std::endl;
    cout << "22. RADIO_TECH_ANY" << std::endl;
    cout << "23. RADIO_TECH_AUTO" << std::endl;
    do {
        cout << "Enter Choice:" << std::endl;
        in >> var;
    } while (var < 0 || var > 23);
    tech = static_cast<RIL_RadioTechnology>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_Registration &reg)
{
    cout << "Enter State:" << std::endl;
    in >> reg.state;
    return in;
}


istream& operator >> (istream &in, RIL_IMS_AccessTechnologyStatus &accTech)
{
    cout << "Enter Network Mode:" << std::endl;
    in >> accTech.networkMode;
    cout << "Enter Status:" << std::endl;
    in >> accTech.status;
    cout << "Enter Restrict Cause:" << std::endl;
    in >> accTech.restrictCause;
    //cout << "Enter Registration:" << std::endl;
    //in >> in.registration;
    return in;
}

istream& operator >> (istream &in, RIL_IMS_ServiceStatusInfo &info)
{
    in >> info.callType;
    in >> info.accTechStatus.networkMode;
    in >> info.accTechStatus.status;
    return in;
}

istream& operator >> (istream &in, RIL_IMS_Answer &answer)
{
    in >> answer.callType;
    in >> answer.presentation;
    in >> answer.rttMode;
    return in;
}

istream& operator >> (istream &in, RIL_IMS_Hangup &hangup)
{
    cout << "Enter Connection Id:" << std::endl;
    in >> hangup.connIndex;
    return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_CallModifyInfo &arg)
{
  std::cout << "Enter callId:" << std::endl;
  in >> arg.callId;

  in >> arg.callType;
  in >> arg.callDomain;
  arg.hasRttMode = 1;
  in >> arg.rttMode;
  // CallModifyFailCause not set for req
  arg.hasCallModifyFailCause = 0;

  return in;
}
ostream& operator<<(ostream& os, const RIL_AudioQuality quality) {
    os << "RIL Audio Quality : ";
    switch(quality) {
        case RIL_AUDIO_QUAL_UNSPECIFIED:
            os << "RIL_AUDIO_QUAL_UNSPECIFIED";
            break;
        case RIL_AUDIO_QUAL_AMR:
            os << "RIL_AUDIO_QUAL_AMR";
            break;
        case RIL_AUDIO_QUAL_AMR_WB:
            os << "RIL_AUDIO_QUAL_AMR_WB";
            break;
        case RIL_AUDIO_QUAL_GSM_EFR:
            os << "RIL_AUDIO_QUAL_GSM_EFR";
            break;
        case RIL_AUDIO_QUAL_GSM_FR:
            os << "RIL_AUDIO_QUAL_GSM_FR";
            break;
        case RIL_AUDIO_QUAL_GSM_HR:
            os << "RIL_AUDIO_QUAL_GSM_HR";
            break;
        case RIL_AUDIO_QUAL_EVRC:
            os << "RIL_AUDIO_QUAL_EVRC";
            break;
        case RIL_AUDIO_QUAL_EVRC_B:
            os << "RIL_AUDIO_QUAL_EVRC_B";
            break;
        case RIL_AUDIO_QUAL_EVRC_WB:
            os << "RIL_AUDIO_QUAL_EVRC_WB";
            break;
        case RIL_AUDIO_QUAL_EVRC_NW:
            os << "RIL_AUDIO_QUAL_EVRC_NW";
            break;
    }
    return os;
}


ostream& operator<<(ostream& os, const RIL_UUS_Type status) {
    os << "RIL UUS Type: ";
    switch(status) {
        case RIL_UUS_TYPE1_IMPLICIT:
            os << "RIL_UUS_TYPE1_IMPLICIT";
            break;
        case RIL_UUS_TYPE1_REQUIRED:
            os << "RIL_UUS_TYPE1_REQUIRED";
            break;
        case RIL_UUS_TYPE1_NOT_REQUIRED:
            os << "RIL_UUS_TYPE1_NOT_REQUIRED";
            break;
        case RIL_UUS_TYPE2_REQUIRED:
            os << "RIL_UUS_TYPE2_REQUIRED";
            break;
        case RIL_UUS_TYPE2_NOT_REQUIRED:
            os << "RIL_UUS_TYPE2_NOT_REQUIRED";
            break;
        case RIL_UUS_TYPE3_REQUIRED:
            os << "RIL_UUS_TYPE3_REQUIRED";
            break;
        case RIL_UUS_TYPE3_NOT_REQUIRED:
            os << "RIL_UUS_TYPE3_NOT_REQUIRED";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_UUS_DCS dcs) {
    os << "RIL UUS DCS: ";
    switch(dcs) {
        case RIL_UUS_DCS_USP:
            os << "RIL_UUS_DCS_USP";
            break;
        case RIL_UUS_DCS_OSIHLP:
            os << "RIL_UUS_DCS_OSIHLP";
            break;
        case RIL_UUS_DCS_X244:
            os << "RIL_UUS_DCS_X244";
            break;
        case RIL_UUS_DCS_RMCF:
            os << "RIL_UUS_DCS_RMCF";
            break;
        case RIL_UUS_DCS_IA5c:
            os << "RIL_UUS_DCS_IA5c";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_VerificationStatus status) {
    os << "IMS Verficaition Status : ";
    switch(status) {
        case RIL_IMS_VERSTAT_UNKNOWN:
            os << "UNKNOWN";
            break;
        case RIL_IMS_VERSTAT_NONE:
            os << "NONE";
            break;
        case RIL_IMS_VERSTAT_PASS:
            os << "PASS";
            break;
        case RIL_IMS_VERSTAT_FAIL:
            os << "FAIL";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_TirMode mode) {
    os << "RIL Tir Mode : ";
    switch(mode) {
        case RIL_IMS_TIRMODE_UNKNOWN:
            os << "RIL_IMS_TIRMODE_UNKNOWN";
            break;
        case RIL_IMS_TIRMODE_TEMPORARY:
            os << "RIL_IMS_TIRMODE_TEMPORARY";
            break;
        case RIL_IMS_TIRMODE_PERMANENT:
            os << "RIL_IMS_TIRMODE_PERMANENT";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallProgressInfoType type) {
    os << "RIL IMS CallPropgressInfoType : ";
    switch(type) {
        case RIL_IMS_CALL_PROGRESS_INFO_CALL_REJ_Q850:
            os << "RIL_IMS_CALL_PROGRESS_INFO_CALL_REJ_Q850";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_CALL_WAITING:
            os << "RIL_IMS_CALL_PROGRESS_INFO_CALL_WAITING";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_CALL_FORWARDING:
            os << "RIL_IMS_CALL_PROGRESS_INFO_CALL_FORWARDING";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_REMOTE_AVAILABLE:
            os << "RIL_IMS_CALL_PROGRESS_INFO_REMOTE_AVAILABLE";
            break;
        case RIL_IMS_CALL_PROGRESS_INFO_UNKNOWN:
            os << "RIL_IMS_CALL_PROGRESS_INFO_UNKNOWN";
            break;
        default:
            os << "<Invalid>";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallState state) {
    os << "CALL STATE : ";
    switch(state) {
        case RIL_IMS_CALLSTATE_UNKNOWN:
            os << "UNKNOWN";
            break;
        case RIL_IMS_CALLSTATE_ACTIVE:
            os << "ACTIVE";
            break;
        case RIL_IMS_CALLSTATE_HOLDING:
            os << "HOLDING";
            break;
        case RIL_IMS_CALLSTATE_DIALING:
            os << "DIALING";
            break;
        case RIL_IMS_CALLSTATE_ALERTING:
            os << "ALERTING";
            break;
        case RIL_IMS_CALLSTATE_INCOMING:
            os << "INCOMING";
            break;
        case RIL_IMS_CALLSTATE_WAITING:
            os << "WAITING";
            break;
        case RIL_IMS_CALLSTATE_END:
            os << "END";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ToneOperation op) {
    os << "TONE OP : ";
    switch(op) {
        case RIL_IMS_TONE_OP_INVALID:
            os << "INVALID";
            break;
        case RIL_IMS_TONE_OP_STOP:
            os << "STOP";
            break;
        case RIL_IMS_TONE_OP_START:
            os << "START";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_RttMode mode) {
    os << "IMS RTT Mode : ";
    switch(mode) {
        case RIL_IMS_RTT_DISABLED:
            os << "DISABLED";
            break;
        case RIL_IMS_RTT_FULL:
            os << "FULL";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallDomain domain) {
    os << "CALL DOMAIN : ";
    switch(domain) {
        case RIL_IMS_CALLDOMAIN_UNKNOWN:
            os << "UNKNOWN";
            break;
        case RIL_IMS_CALLDOMAIN_CS:
            os << "CS";
            break;
        case RIL_IMS_CALLDOMAIN_PS:
            os << "PS";
            break;
        case RIL_IMS_CALLDOMAIN_AUTOMATIC:
            os << "AUTOMATIC";
            break;
        case RIL_IMS_CALLDOMAIN_NOT_SET:
            os << "NOT_SET";
            break;
        case RIL_IMS_CALLDOMAIN_INVALID:
            os << "INVALID";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallModifiedCause cause) {
    os << "CALL MODIFIED CAUSE : ";
    switch(cause) {
        case RIL_IMS_CMODCAUSE_NONE:
            os << "NONE";
            break;
        case RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_LOCAL_REQ:
            os << "UPGRADE_DUE_TO_LOCAL_REQ";
            break;
        case RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_REMOTE_REQ:
            os << "UPGRADE_DUE_TO_REMOTE_REQ";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ:
            os << "DOWNGRADE_DUE_TO_LOCAL_REQ";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ:
            os << "DOWNGRADE_DUE_TO_REMOTE_REQ";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT:
            os << "DOWNGRADE_DUE_TO_RTP_TIMEOUT";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_QOS:
            os << "DOWNGRADE_DUE_TO_QOS";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS:
            os << "DOWNGRADE_DUE_TO_PACKET_LOSS";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT:
            os << "DOWNGRADE_DUE_TO_LOW_THRPUT";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION:
            os << "DOWNGRADE_DUE_TO_THERM_MITIGATION";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LIPSYNC:
            os << "DOWNGRADE_DUE_TO_LIPSYNC";
            break;
        case RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR:
            os << "DOWNGRADE_DUE_TO_GENERIC_ERROR";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallType type) {
    switch(type)
    {
        case RIL_IMS_CALL_TYPE_UNKNOWN:
            os << "RIL_IMS_CALL_TYPE_UNKNOWN";
            break;
        case RIL_IMS_CALL_TYPE_VOICE:
            os << "RIL_IMS_CALL_TYPE_VOICE";
            break;
        case RIL_IMS_CALL_TYPE_VT_TX:
            os << "RIL_IMS_CALL_TYPE_VT_TX";
            break;
        case RIL_IMS_CALL_TYPE_VT_RX:
            os << "RIL_IMS_CALL_TYPE_VT_RX";
            break;
        case RIL_IMS_CALL_TYPE_VT:
            os << "RIL_IMS_CALL_TYPE_VT";
            break;
        case RIL_IMS_CALL_TYPE_VT_NODIR:
            os << "RIL_IMS_CALL_TYPE_VT_NODIR";
            break;
        case RIL_IMS_CALL_TYPE_SMS:
            os << "RIL_IMS_CALL_TYPE_SMS";
            break;
        case RIL_IMS_CALL_TYPE_UT:
            os << "RIL_IMS_CALL_TYPE_UT";
            break;
        case RIL_IMS_CALL_TYPE_USSD:
            os << "RIL_IMS_CALL_TYPE_USSD";
            break;
        case RIL_IMS_CALL_TYPE_CALLCOMPOSER:
            os << "RIL_IMS_CALL_TYPE_CALLCOMPOSER";
            break;
        default:
            os << "RIL_IMS_INVALID_CALL_TYPE";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_StatusType type) {

    os << "IMS STATUS : ";
    switch(type) {
        case RIL_IMS_STATUS_DISABLED:
            os << "DISABLED";
            break;
        case RIL_IMS_STATUS_PARTIALLY_ENABLED:
            os << "PARTIALLY_ENABLED";
            break;
        case RIL_IMS_STATUS_ENABLED:
            os << "ENABLED";
            break;
        case RIL_IMS_STATUS_NOT_SUPPORTED:
            os << "NOT_SUPPORTED";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_AccessTechnologyStatus& status) {
    os << status.networkMode << std::endl;
    os << status.status << std::endl;
    os << "Restrict Cause : " << status.restrictCause << std::endl;
    os << status.registration << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_RegistrationState state) {

    os << "IMS Registration State : ";
    switch(state) {
        case RIL_IMS_REG_STATE_UNKNOWN:
            os << "RIL_IMS_REG_STATE_UNKNOWN";
            break;
        case RIL_IMS_REG_STATE_REGISTERED:
            os << "RIL_IMS_REG_STATE_REGISTERED";
            break;
        case RIL_IMS_REG_STATE_NOT_REGISTERED:
            os << "RIL_IMS_REG_STATE_NOT_REGISTERED";
            break;
        case RIL_IMS_REG_STATE_REGISTERING:
            os << "RIL_IMS_REG_STATE_REGISTERING";
            break;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_GeoLocationDataStatus state) {
  os << "RIL_IMS_GeoLocationDataStatus : ";
  switch (state) {
    case RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_TIMEOUT:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_TIMEOUT";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_NO_CIVIC_ADDRESS:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_NO_CIVIC_ADDRESS";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_ENGINE_LOCK:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_ENGINE_LOCK";
      break;
    case RIL_IMS_GEO_LOC_DATA_STATUS_RESOLVED:
      os << "RIL_IMS_GEO_LOC_DATA_STATUS_RESOLVED";
      break;
  }
  return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_Registration& imsRegInfo)
{
    os << imsRegInfo.state << std::endl;
    if (imsRegInfo.state == RIL_IMS_REG_STATE_NOT_REGISTERED) {
        os << "Error Code : " << imsRegInfo.errorCode << std::endl;
        if (imsRegInfo.errorMessage) {
            os << "Error Message : " << imsRegInfo.errorMessage << std::endl;
        }
    }
    os << "Radio Technology : " << getRatFromValue(imsRegInfo.radioTech) << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ServiceStatusInfo* info)
{
    if (info != nullptr) {
        os << "RIL IMS SERVICE STATUS INFO : " << std::endl;
        os << info->callType << std::endl;
        os << info->accTechStatus << std::endl;
        os << info->rttMode << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallFailCauseResponse* info) {
    if (info) {
        os << "IMS CALL FAIL CAUSE RESPONSE :";
        os << "Fail Cause : " << info->failCause << std::endl;
        os << "Extended Fail Cause : " << info->extendedFailCause << std::endl;
        os << "Error Info : ";
        for(size_t i=0; i < info->errorInfoLen; i++) {
            os << info->errorInfo[i] << " ";
        }
        os << std::endl;
        if (info->networkErrorString) {
            os << "Network Error String : " << info->networkErrorString << std::endl;
        }
        os << info->errorDetails << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_UUS_Info* info) {
    os << "RIL UUS Info : " << std::endl;
    os << info->uusType << std::endl;
    os << info->uusDcs << std::endl;
    os << "uusLength : " << info->uusLength << std::endl;
    if(info->uusData && info->uusLength > 0) {
        os << "uusData : " << std::string(info->uusData, info->uusLength) << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_SipErrorInfo* info) {
    os << "IMS SIP ERROR INFO: " << std::endl;
    if(info != nullptr) {
        os << "Error Code : " << info->errorCode << std::endl;
        if(info->errorString != nullptr) {
            os << "Error String : " << info->errorString << std::endl;
        }
    }
    return os;
}

std::ostream& operator<<(ostream& out, const RIL_IMS_SipErrorInfo& sipErrorInfo) {
    out << "== IMS SIP Error Info ==" << std::endl;
    out << "    Error Code : " << sipErrorInfo.errorCode << std::endl;

    out << "    Error String : ";
    if (sipErrorInfo.errorString != nullptr) {
        out << sipErrorInfo.errorString;
    }

    return out;
}

ostream& operator<<(ostream& os, const RIL_IMS_VerstatInfo* info) {
    os << "IMS Verstat Info : " << std::endl;
    os << "canMarkUnwantedCall : " << info->canMarkUnwantedCall << std::endl;
    os << info->verificationStatus << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallProgressInfo* info) {
    os << "IMS Call Progress Info : " << std::endl;
    os << info->type << std::endl;
    os << "reasonCode : " << info->reasonCode << std::endl;
    os << "reasonText : ";
    if (info->reasonText) {
      os << info->reasonText;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_SubConfigInfo& info) {
    os << "IMS Sub Config Info : " << std::endl;
    os << "simultStackCount: " << info.simultStackCount << std::endl;
    os << "imsStackEnabledLen: " << info.imsStackEnabledLen << std::endl;
    os << "simultStack: [ ";
    for(int i=0; i < info.imsStackEnabledLen; i++) {
        os << static_cast<bool>(info.imsStackEnabled[i]) << " ";
    }
    os << "]" << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MultiIdentityRegistrationStatus& type) {
    os << "IMS MultiIdentity Registration Status:" << std::endl;
    switch(type) {
        case RIL_IMS_STATUS_UNKNOWN:
            os << "UNKNOWN";
        break;
        case RIL_IMS_STATUS_DISABLE:
            os << "DISABLE";
        break;
        case RIL_IMS_STATUS_ENABLE:
            os << "ENABLE";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MultiIdentityLineType& type) {
    os << "IMS MultiIdentity Line Type: " << std::endl;
    switch(type) {
        case RIL_IMS_LINE_TYPE_UNKNOWN:
            os << "UNKNOWN";
        break;
        case RIL_IMS_LINE_TYPE_PRIMARY:
            os << "PRIMARY";
        break;
        case RIL_IMS_LINE_TYPE_SECONDARY:
            os << "SECONDARY";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_MultiIdentityLineInfo& info) {
    os << "IMS MultiIdentityLine Info : " << std::endl;
    os << "MSISDN: " << info.msisdn << std::endl;
    os << info.registrationStatus << std::endl;
    os << info.lineType << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HoExtraType& type) {
    os << "IMS Handover Extra Type: " << std::endl;
    switch(type) {
        case RIL_IMS_HO_XT_TYPE_LTE_TO_IWLAN_HO_FAIL:
            os << "LTE_TO_IWLAN_HO_FAIL";
        break;
        case RIL_IMS_HO_XT_TYPE_INVALID:
            os << "INVALID";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HandoverType& type) {
    os << "IMS Handover Type: " << std::endl;
    switch(type) {
        case RIL_IMS_HO_START:
            os << "START";
        break;
        case RIL_IMS_HO_COMPLETE_SUCCESS:
            os << "COMPLETE_SUCCESS";
        break;
        case RIL_IMS_HO_COMPLETE_FAIL:
            os << "COMPLETE_FAIL";
        break;
        case RIL_IMS_HO_CANCEL:
            os << "CANCEL";
        break;
        case RIL_IMS_HO_NOT_TRIGGERED:
            os << "NOT_TRIGGERED";
        break;
        case RIL_IMS_HO_NOT_TRIGGERED_MOBILE_DATA_OFF:
            os << "NOT_TRIGGERED_MOBILE_DATA_OFF";
        break;
        case RIL_IMS_HO_INVALID:
            os << "INVALID";
        break;
        case RIL_IMS_HO_UNKNOWN:
            os << "UNKNOWN";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HoExtra& extra) {
    os << "IMS HoExtra: " << std::endl;
    os << "Extra Type: " << extra.type << std::endl;
    os << "Extra Info Len:" << extra.extraInfoLen << std::endl;
    for(int i=0; i<extra.extraInfoLen; i++) {
        os << "ExtraInfo[" << i << "]" << extra.extraInfo[i] << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_HandoverInfo& info) {
    os << "IMS Handover Info : " << std::endl;
    os << info.type << std::endl;
    os << info.srcTech << std::endl;
    os << info.targetTech << std::endl;
    os << info.hoExtra << std::endl;
    os << "Error Code:" << info.errorCode << std::endl;
    os << "Error Code Message:" << info.errorMessage << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_ConferenceCallState& state) {
    os << "IMS Conference Call State: " << std::endl;
    switch(state) {
        case RIL_IMS_CONF_CALL_STATE_UNKNOWN:
            os << "UNKNOWN";
        break;
        case RIL_IMS_CONF_CALL_STATE_FOREGROUND:
            os << "FOREGROUND";
        break;
        case RIL_IMS_CONF_CALL_STATE_BACKGROUND:
            os << "BACKGROUND";
        break;
        case RIL_IMS_CONF_CALL_STATE_RINGING:
            os << "RINGING";
        break;
    }
    os << std::endl;
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_RefreshConferenceInfo& info) {
    os << "IMS Refresh Conference Info : " << std::endl;
    os << info.conferenceCallState << std::endl;
    os << "Conference Uri Len:" << info.confInfoUriLength << std::endl;
    for(int i=0; i<info.confInfoUriLength; i++) {
        os << "confInfoUri[" << i << "]" << info.confInfoUri[i] << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const RIL_IMS_CallInfo* info) {
    os << "IMS CALL INFO : ";
    if(info)
    {
        os << info->callState << std::endl;
        os << "Call Index : " << info->index << std::endl;
        os << "toa : " << info->toa << std::endl;
        os << "isMpty : " << static_cast<uint32_t>(info->isMpty) << std::endl;
        os << "isMt : " << static_cast<uint32_t>(info->isMt) << std::endl;
        os << "als : " << static_cast<uint32_t>(info->als) << std::endl;
        os << "isVoice : " << static_cast<uint32_t>(info->isVoice) << std::endl;
        os << "isVoicePrivacy : " << static_cast<uint32_t>(info->isVoicePrivacy) << std::endl;
        if(info->number) {
            os << "number : " << info->number << std::endl;
            os << "numberPresentation : " << info->numberPresentation << std::endl;
        }
        if(info->name) {
            os << "name : " << info->name << std::endl;
            os << "namePresentation : " << info->namePresentation << std::endl;
        }
        if(info->redirNum) {
            os << "redirNum : " << info->redirNum << std::endl;
            os << "redirNumPresentation : " << info->redirNumPresentation << std::endl;
        }
        os << info->callType << std::endl;
        os << info->callDomain << std::endl;
        os << "callSubState : " << info->callSubState << std::endl;
        os << "isEncrypted : " << static_cast<uint32_t>(info->isEncrypted) << std::endl;
        os << "isCalledPartyRinging : " << static_cast<uint32_t>(info->isCalledPartyRinging) << std::endl;
        os << "isVideoConfSupported : " << static_cast<uint32_t>(info->isVideoConfSupported) << std::endl;
        if(info->historyInfo) {
            os << "historyInfo : " << info->historyInfo << std::endl;
        }
        os << "mediaId : " << info->mediaId << std::endl;
        os << info->causeCode << std::endl;
        os << info->rttMode << std::endl;
        if(info->sipAlternateUri) {
            os << "sipAlternateUri : " << info->sipAlternateUri << std::endl;
        }
        os << "localAbilityLen" << info->localAbilityLen << std::endl;
        if(info->localAbility) {
            os << info->localAbility << std::endl;
        }
        os << "peerAbilityLen" << info->peerAbilityLen << std::endl;
        if(info->peerAbility) {
            os << info->peerAbility << std::endl;
        }
        os << info->callFailCauseResponse << std::endl;
        if(info->terminatingNumber) {
            os << "terminatingNumber : " << info->terminatingNumber << std::endl;
        }
        os << "isSecondary : " << static_cast<uint32_t>(info->isSecondary) << std::endl;
        if (info->verstatInfo) {
            os << info->verstatInfo << std::endl;
        }
        if (info->uusInfo) {
            os << info->uusInfo << std::endl;
        }
        if (info->audioQuality) {
            os << *(info->audioQuality) << std::endl;
        }
        os << "Codec : " << info->Codec << std::endl;
        if (info->displayText) {
            os << "displayText : " << info->displayText << std::endl;
        }
        if (info->additionalCallInfo) {
            os << "additionalCallInfo : " << info->additionalCallInfo << std::endl;
        }
        if (info->childNumber) {
            os << "childNumber : " << info->childNumber << std::endl;
        }
        os << "emergencyServiceCategory : " << info->emergencyServiceCategory << std::endl;
        os << info->tirMode << std::endl;
        if (info->callProgInfo) {
            os << info->callProgInfo << std::endl;
        }
    }

    return os;
}

std::ostream& operator <<(std::ostream& out, RIL_IMS_SmsFormat imsSmsFormat) {
    out << "IMS SMS Format: ";
    switch (imsSmsFormat) {
        case RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP:
            out << "3GPP";
            break;
        case RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP2:
            out << "3GPP2";
            break;
        default:
            out << "Unknown";
            break;
    }
    return out;
}

void printSmsPdu(std::ostream& out, const uint8_t* pdu, const uint32_t pduLength) {
    out << std::hex;
    for (uint32_t i = 0; i < pduLength; i++) {
        out << static_cast<unsigned int>(pdu[i]) << " ";
    }
    out << std::dec;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_IncomingSms& incomingImsSms) {
    out << "== Incoming SMS Over IMS ==" << std::endl
        << incomingImsSms.format << std::endl
        << incomingImsSms.verificationStatus << std::endl
        << "    PDU:" << std::endl;
    printSmsPdu(out, incomingImsSms.pdu, incomingImsSms.pduLength);
    return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_SmsStatusReport& imsSmsStatusReport) {
    out << "== SMS Over IMS Status Report ==" << std::endl
        << imsSmsStatusReport.format << std::endl
        << "    SMS Message Reference: " << imsSmsStatusReport.messageRef << std::endl
        << "    PDU:" << std::endl;
    printSmsPdu(out, imsSmsStatusReport.pdu, imsSmsStatusReport.pduLength);
    return out;
}

std::ostream& operator <<(std::ostream& out, RIL_IMS_TtyModeType ttyMode) {
    out << "IMS TTY Mode: ";
    switch (ttyMode) {
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_OFF:
            out << "OFF";
            break;
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_FULL:
            out << "FULL";
            break;
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_HCO:
            out << "HCO";
            break;
        case RIL_IMS_TtyModeType::RIL_IMS_TTY_MODE_VCO:
            out << "VCO";
            break;
        default:
            out << "INVALID";
            break;
    }
    return out;
}

// TODO: Come up with a common templated prompt generation and user input
// TODO: extraction function that accepts a prompt, range of valid values,
// TODO: and an exit value. The function returns the extracted value and
// TODO: indicates if the exit value was input.
std::istream& operator >>(std::istream& in, RIL_IMS_SmsMessage& imsSmsMessage) {
    std::string destinationAddress;
    do {
        in.clear();
        std::cout << "Enter the number to send SMS to (-1 - Exit): ";
        in >> destinationAddress;
    } while (in.fail());

    if (destinationAddress == "-1") {
        return in;
    }

    if (destinationAddress.size() % 2) {
        destinationAddress.append(1, 'F');
    }

    for (std::string::size_type i = 0; i < destinationAddress.size(); i += 2) {
        std::swap(destinationAddress[i], destinationAddress[i + 1]);
    }

    static uint8_t imsSmsTestPduPart1[] = {1, 0, 11, 145};
    // message body "Hello World"
    static uint8_t imsSmsTestPduPart2[] = {0, 0, 11, 145, 50, 155, 253, 6, 93, 223, 114, 54, 25};

    size_t pduLength = sizeof(imsSmsTestPduPart1) / sizeof(uint8_t) + sizeof(imsSmsTestPduPart2) / sizeof(uint8_t) + destinationAddress.size() / 2;
    imsSmsMessage.pdu = new uint8_t[pduLength];
    if (imsSmsMessage.pdu != nullptr) {
        imsSmsMessage.pduLength = pduLength;
        uint8_t* pduCursor = imsSmsMessage.pdu;
        std::copy(imsSmsTestPduPart1, imsSmsTestPduPart1 + sizeof(imsSmsTestPduPart1) / sizeof(uint8_t), pduCursor);
        pduCursor += sizeof(imsSmsTestPduPart1) / sizeof(uint8_t);

        // append destination number
        for (auto iter = destinationAddress.begin(); iter != destinationAddress.end(); iter += 2) {
            std::istringstream(std::string(iter, iter + 2)) >> std::hex >> *pduCursor;
            pduCursor++;
        }

        std::copy(imsSmsTestPduPart2, imsSmsTestPduPart2 + sizeof(imsSmsTestPduPart2) / sizeof(uint8_t), pduCursor);
    } else {
        imsSmsMessage.pduLength = 0;
    }

    imsSmsMessage.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP;
    imsSmsMessage.messageRef = 0;
    imsSmsMessage.shallRetry = false;
    imsSmsMessage.smsc = nullptr;
    imsSmsMessage.smscLength = 0;

    return in;
}

std::ostream& operator <<(std::ostream& out, RIL_IMS_SmsSendStatus imsSmsSendStatus) {
    out << "IMS SMS Send Status: ";
    switch (imsSmsSendStatus) {
        case RIL_IMS_SMS_SEND_STATUS_OK:
            out << "Success.";
            break;
        case RIL_IMS_SMS_SEND_STATUS_ERROR:
            out << "Error. Do not retry sending the messager over CS.";
            break;
        case RIL_IMS_SMS_SEND_STATUS_ERROR_RETRY:
            out << "Error. Retry sending message using the message reference.";
            break;
        case RIL_IMS_SMS_SEND_STATUS_ERROR_FALLBACK:
            out << "Error. Retry sending message over CS.";
            break;
        default:
            out << "Unknown.";
            break;
    }
    return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_SendSmsResponse& imsSendSmsResponse) {
    out << "== MO SMS Over IMS Response ==" << std::endl;
    out << "    " << imsSendSmsResponse.status << std::endl;
    out << "    Message Ref: " << imsSendSmsResponse.messageRef << std::endl;
    return out;
}

std::istream& operator >>(std::istream& in, RIL_IMS_SmsDeliveryStatus& imsSmsDeliveryStatus) {
    int userInput = -1;
    do {
        in.clear();
        std::cout << "Enter the delivery status of the SMS being ACK'd"
            << " (0 - Success, 1 - Error, 2 - Storage Unavailable"
            << ", 3 - SMS Not Supported On Device, -1 - Exit): ";
        in >> userInput;
    } while (in.fail() || userInput < -1 || userInput > 3);

    imsSmsDeliveryStatus = static_cast<RIL_IMS_SmsDeliveryStatus>(userInput);
    return in;
}

std::istream& operator >>(std::istream& in, RIL_IMS_SmsAck& imsSmsAck) {
    int32_t userInput = -1;
    do {
        in.clear();
        std::cout << "Enter the ID/reference of the SMS message to ACK (-1 - Exit): ";
        in >> userInput;
    } while (in.fail() || userInput < -1);

    if (userInput == -1) {
        imsSmsAck.messageRef = std::numeric_limits<uint32_t>::max();
        return in;
    } else {
        imsSmsAck.messageRef = userInput;
    }

    in >> imsSmsAck.deliveryStatus;
    return in;
}

std::istream& operator >>(std::istream& in, RIL_IMS_CallWaitingSettings& imsCallwaitingSettings) {
    int32_t userInput = -1;
    do {
        in.clear();
        std::cout << "Enter the service class to enable/disable IMS call waiting on (-1 - Exit): ";
        in >> userInput;
    } while (in.fail() || userInput < -1);

    if (userInput == -1) {
        imsCallwaitingSettings.serviceClass = std::numeric_limits<uint32_t>::max();
        return in;
    } else {
        imsCallwaitingSettings.serviceClass = userInput;
    }

    do {
        in.clear();
        std::cout << "Do you want to enable or disable call waiting (0 - Disable, !0 - Enable)? ";
        in >> imsCallwaitingSettings.enabled;
    } while (in.fail());

    return in;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_CallWaitingSettings& imsCallwaitingSettings) {
    out << "== IMS Call Waiting Setting ==" << std::endl;
    out << "    Service Class: " << imsCallwaitingSettings.serviceClass << std::endl;
    out << "    State: " << (imsCallwaitingSettings.enabled ? "Enabled" : "Disabled");
    return out;
}

std::ostream& operator <<(std::ostream& out, const RIL_IMS_QueryCallWaitingResponse& imsQueryCallWaitingResp) {
    out << imsQueryCallWaitingResp.sipError << std::endl;
    out << imsQueryCallWaitingResp.callWaitingSettings;
    return out;
}

void ImsMenu::registrationStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.imsGetRegistrationState(
      [](RIL_Errno err, const RIL_IMS_Registration& info) -> void {
        std::cout << "Got response for registrationStatus request: " << err << std::endl;
        std::cout << info << std::endl;
      });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send registrationStatus" << std::endl;
  }
}

void ImsMenu::imsDial(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_IMS_Dial dialParams = {};
  dialParams.address = const_cast<char*>(userInput[1].c_str());

  Status s = rilSession.imsDial(dialParams, [](RIL_Errno err) -> void {
    std::cout << "Got response for Ims dial request: " << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDial" << std::endl;
  }
}

void ImsMenu::acceptCall(std::vector<std::string> userInput) {
  RIL_IMS_Answer answer {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  cin >> answer;
  Status s = rilSession.imsAnswer(answer, [](RIL_Errno err) -> void {
    std::cout << "Got response for Answer request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send acceptCall" << std::endl;
  }
}

void ImsMenu::hangupCall(std::vector<std::string> userInput) {
  RIL_IMS_Hangup hangup {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  cin >> hangup;
  Status s = rilSession.imsHangup(hangup, [](RIL_Errno err) -> void {
    std::cout << "Got response for Hangup request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send hangupCall" << std::endl;
  }
}

void ImsMenu::imsRegistrationChange(std::vector<std::string> userInput) {
  RIL_IMS_RegistrationState state {};
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  cin >> state;
  Status s = rilSession.imsRegistrationChange(state, [](RIL_Errno err) -> void {
    std::cout << "Got response for RegistrationChange request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsRegistrationChange" << std::endl;
  }
}

void ImsMenu::imsQueryServiceStatus(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsQueryServiceStatus([](RIL_Errno err,
    const size_t count, const RIL_IMS_ServiceStatusInfo** statusInfo) -> void {
    std::cout << "Got response for Query ServiceStatus request." << err << std::endl;
    std::cout << "count: " << count << std::endl;
    if (statusInfo) {
        for(int i = 0; i < count; i++) {
            cout << statusInfo[i];
        }
    }
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsQueryServiceStatus" << std::endl;
  }
}

void ImsMenu::imsSetServiceStatus(std::vector<std::string> userInput) {
  size_t numSsInfos = 1;
  RIL_IMS_ServiceStatusInfo info {};
  RIL_IMS_ServiceStatusInfo **ssInfos = nullptr;
  cin >> info;
  ssInfos = new RIL_IMS_ServiceStatusInfo* [1]();
  if(ssInfos) {
    ssInfos[0] = &info;
  }
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSetServiceStatus(
    numSsInfos, (const RIL_IMS_ServiceStatusInfo**)ssInfos, [](RIL_Errno err) -> void {
    std::cout << "Got response for SetServiceStatus request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsSetServiceStatus" << std::endl;
  }
  if (ssInfos) {
    delete[] ssInfos;
  }
}

void ImsMenu::imsGetImsSubConfig(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsGetImsSubConfig([](RIL_Errno err,
    const RIL_IMS_SubConfigInfo& config) -> void {
      std::cout << "Got response for Get Ims Sub Config request." << err << std::endl;
      cout << config << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsGetImsSubConfig" << std::endl;
  }
}

void ImsMenu::imsDtmf(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  char digit;
  std::cout << "Enter digit:";
  std::cin >> digit;
  Status s = rilSession.imsDtmf(digit, [](RIL_Errno err) -> void {
      std::cout << "Got response for imsDtmf request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDtmf" << std::endl;
  }
}

void ImsMenu::imsDtmfStart(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  char digit;
  std::cout << "Enter digit:";
  std::cin >> digit;
  Status s = rilSession.imsDtmfStart(digit, [](RIL_Errno err) -> void {
      std::cout << "Got response for imsDtmfStart request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDtmfStart" << std::endl;
  }
}

void ImsMenu::imsDtmfStop(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsDtmfStop([](RIL_Errno err) -> void {
      std::cout << "Got response for imsDtmfStop request." << err << std::endl;
  });
  if(s != Status::SUCCESS) {
    std::cout << "Failed to send imsDtmfStop" << std::endl;
  }
}

void ImsMenu::imsModifyCallInitiate(std::vector<std::string> userInput) {
  RIL_IMS_CallModifyInfo modifyInfo{};
  std::cin >> modifyInfo;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.imsModifyCallInitiate(modifyInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsModifyCallInitiate request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsModifyCallInitiate" << std::endl;
  }
}

void ImsMenu::imsModifyCallConfirm(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  RIL_IMS_CallModifyInfo modifyInfo{};
  std::cin >> modifyInfo;
  Status status = rilSession.imsModifyCallConfirm(modifyInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsModifyCallConfirm request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsModifyCallConfirm" << std::endl;
  }
}

void ImsMenu::imsCancelModifyCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId:";
  std::cin >> callId;
  Status status = rilSession.imsCancelModifyCall(callId, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsCancelModifyCall request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsCancelModifyCall" << std::endl;
  }
}

void ImsMenu::imsRequestConference(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.imsRequestConference(
      [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
        std::cout << "Got response for imsRequestConference request." << err << std::endl;
        std::cout << "errorDetails: " << &errorDetails;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestConference" << std::endl;
  }
}

void ImsMenu::imsRequestHoldCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId:";
  std::cin >> callId;
  Status status = rilSession.imsRequestHoldCall(
      callId, [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
        std::cout << "Got response for imsRequestHoldCall request." << err << std::endl;
        std::cout << "errorDetails: " << &errorDetails;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestHoldCall" << std::endl;
  }
}

void ImsMenu::imsRequestResumeCall(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  uint32_t callId = 0;
  std::cout << "Enter callId:";
  std::cin >> callId;
  Status status = rilSession.imsRequestResumeCall(
      callId, [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errorDetails) -> void {
        std::cout << "Got response for imsRequestResumeCall request." << err << std::endl;
        std::cout << "errorDetails: " << &errorDetails;
      });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestResumeCall" << std::endl;
  }
}

void ImsMenu::imsRequestExitEmergencyCallbackMode(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status status = rilSession.imsRequestExitEmergencyCallbackMode([](RIL_Errno err) -> void {
    std::cout << "Got response for imsRequestExitEmergencyCallbackMode request." << err << std::endl;
  });
  if (status != Status::SUCCESS) {
    std::cout << "Failed to send imsRequestExitEmergencyCallbackMode" << std::endl;
  }
}

void ImsMenu::imsSendRttMessage(std::vector<std::string> userInput) {
  for (auto& arg : userInput) {
    std::cout << __func__ << ":" << arg << std::endl;
  }

  std::string msg;
  std::cout << "Enter rtt message:" << std::endl;
  std::cin >> msg;

  Status s = rilSession.imsSendRttMessage(msg.c_str(), msg.size(), [](RIL_Errno err) -> void {
    std::cout << "Got response for imsSendRttMessage request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSendRttMessage" << std::endl;
  }
}

void ImsMenu::imsSendGeolocationInfo(std::vector<std::string> userInput) {
  for (auto& arg : userInput) {
    std::cout << __func__ << ":" << arg << std::endl;
  }

  RIL_IMS_GeolocationInfo geolocationInfo{};

  std::cout << "Enter latitude:" << std::endl;
  std::cin >> geolocationInfo.latitude;
  std::cout << "Enter longitude:" << std::endl;
  std::cin >> geolocationInfo.longitude;

  std::string countryCode;
  std::cout << "Enter country code:" << std::endl;
  std::cin >> countryCode;
  geolocationInfo.countryCode = const_cast<char*>(countryCode.c_str());
  std::string country;
  std::cout << "Enter country:" << std::endl;
  std::cin >> country;
  geolocationInfo.country = const_cast<char*>(country.c_str());
  std::string state;
  std::cout << "Enter state:" << std::endl;
  std::cin >> state;
  geolocationInfo.state = const_cast<char*>(state.c_str());
  std::string city;
  std::cout << "Enter city:" << std::endl;
  std::cin >> city;
  geolocationInfo.city = const_cast<char*>(city.c_str());
  std::string postalCode;
  std::cout << "Enter postal code:" << std::endl;
  std::cin >> postalCode;
  geolocationInfo.postalCode = const_cast<char*>(postalCode.c_str());
  std::string street;
  std::cout << "Enter street:" << std::endl;
  std::cin >> street;
  geolocationInfo.street = const_cast<char*>(street.c_str());
  std::string houseNumber;
  std::cout << "Enter house number:" << std::endl;
  std::cin >> houseNumber;
  geolocationInfo.houseNumber = const_cast<char*>(houseNumber.c_str());

  Status s = rilSession.imsSendGeolocationInfo(geolocationInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for imsSendGeolocationInfo request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSendGeolocationInfo" << std::endl;
  }
}

void ImsMenu::imsSendSms(std::vector<std::string> userInput) {
    RIL_IMS_SmsMessage imsSmsMessage = {};
    std::cin >> imsSmsMessage;

    if (imsSmsMessage.pdu == nullptr) {
        return;
    }

    Status s = rilSession.imsSendSms(
        imsSmsMessage,
        [] (RIL_Errno e, const RIL_IMS_SendSmsResponse& resp) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully sent SMS over IMS." : "Failed to send SMS over IMS.")
                << " Error: " << e << "." << std::endl;
            std::cout << resp;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl << "Failed to issue request to send SMS over IMS."
            << std::endl;
    }

    if (imsSmsMessage.pdu != nullptr) {
        delete[] imsSmsMessage.pdu;
        imsSmsMessage.pdu = nullptr;
        imsSmsMessage.pduLength = 0;
    }
}

void ImsMenu::imsAckSms(std::vector<std::string> userInput) {
    RIL_IMS_SmsAck smsAck = {};
    std::cin >> smsAck;

    if (smsAck.messageRef == std::numeric_limits<uint32_t>::max()
            || smsAck.deliveryStatus == -1) {
        return;
    }

    Status s = rilSession.imsAckSms(
        smsAck,
        [] (RIL_Errno e) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully ack'd SMS." : "Failed to ack SMS.")
                << " Error: " << e << "." << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl << "Failed to issue request to ack SMS."
            << std::endl;
    }
}

void ImsMenu::imsGetSmsFormat(std::vector<std::string> userInput) {
    Status s = rilSession.imsGetSmsFormat(
        [] (RIL_Errno e, RIL_IMS_SmsFormat smsFormat) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully got the current SMS format." :
                "Failed to get the current SMS format.")
                << " Error: " << e << ". "
                << smsFormat << "." << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl
            << "Failed to issue request to get the current SMS format."
            << std::endl;
    }
}

void ImsMenu::imsSetCallWaiting(std::vector<std::string> userInput) {
    RIL_IMS_CallWaitingSettings callWaitingSettings {};
    std::cin >> callWaitingSettings;

    if (callWaitingSettings.serviceClass == std::numeric_limits<uint32_t>::max()) {
        return;
    }

    Status s = rilSession.imsSetCallWaiting(
        callWaitingSettings,
        [] (RIL_Errno e, const RIL_IMS_SipErrorInfo sipErrorInfo) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully set IMS call waiting." :
                "Failed to set IMS call waiting.")
                << " Error: " << e << "."
                << std::endl << sipErrorInfo << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl
            << "Failed to issue request to set IMS call waiting."
            << std::endl;
    }
}

void ImsMenu::imsQueryCallWaiting(std::vector<std::string> userInput) {
    int32_t _userInput = -1;
    do {
        std::cin.clear();
        std::cout << "Enter the service class to query IMS call waiting status for (-1 - Exit): ";
        std::cin >> _userInput;
    } while (std::cin.fail() || _userInput < -1);

    if (_userInput == -1) {
        return;
    }

    uint32_t serviceClass = _userInput;
    Status s = rilSession.imsQueryCallWaiting(
        serviceClass,
        [] (RIL_Errno e, const RIL_IMS_QueryCallWaitingResponse& callWaitingResp) {
            std::cout << std::endl << ((e == RIL_E_SUCCESS) ?
                "Successfully queried IMS call waiting status." :
                "Failed to query IMS call waiting status.")
                << " Error: " << e << "."
                << std::endl << callWaitingResp << std::endl;
        }
    );

    if (s != Status::SUCCESS) {
        std::cout << std::endl
            << "Failed to issue request to query IMS call waiting status."
            << std::endl;
    }
}

void ImsMenu::registerForImsIndications(std::vector<std::string> userInput) {

    rilSession.registerImsUnsolCallStateChangedIndicationHandler(
        [](const RIL_IMS_CallInfo *calls, size_t sz) {
        std::cout << "Received vowifi call quality indication." << std::endl;
        if(calls) {
            for(int i=0; i < sz; i++) {
                std::cout << &calls[i];
            }
        }
    });

    rilSession.registerImsUnsolCallRingIndicationHandler([]() {
        std::cout << "Received ImsCallUnsolRing." << std::endl;
    });

    rilSession.registerImsUnsolRingBackToneIndicationHandler(
        [](const RIL_IMS_ToneOperation& imsToneOperation) {
        std::cout << "Received ImsUnsolRingBackTone." << std::endl;
        std::cout << imsToneOperation << std::endl;
    });

    rilSession.registerImsUnsolRegistrationStateIndicationHandler(
        [](const RIL_IMS_Registration& imsRegInfo) {
        std::cout << "Received ImsUnsolRegistrationState." << std::endl;
        std::cout << imsRegInfo << std::endl;
    });

    rilSession.registerImsUnsolServiceStatusIndicationHandler(
        [](const size_t count, const RIL_IMS_ServiceStatusInfo** imsSrvStatusInfo) {
        std::cout << "Received ImsUnsolServiceStatus." << std::endl;
        std::cout << "Received count :" << count << std::endl;
        for(size_t i=0; i < count; i++) {
            std::cout << imsSrvStatusInfo[i] << std::endl;
        }
    });

    rilSession.registerImsUnsolSubConfigChangeIndicationHandler(
        [](const RIL_IMS_SubConfigInfo& imsSubConfigInfo) {
        std::cout << "Received ImsUnsolSubConfigChange." << std::endl;
        std::cout << imsSubConfigInfo << std::endl;
    });

    rilSession.registerImsRttMessageIndicationHandler([](const std::string& rttMessage) {
        std::cout << "[ImsMenu]: UnsolImsRttMessage = " + rttMessage << std::endl;
        std::cout << "Received rttMessage indication." << std::endl;
    });

    rilSession.registerImsGeolocationIndicationHandler([](double latitude, double longitude) {
        std::cout << "[ImsMenu]: latitude = " + std::to_string(latitude) << std::endl;
        std::cout << "[ImsMenu]: latitude = " + std::to_string(longitude) << std::endl;
        std::cout << "Received geolocation info indication." << std::endl;
    });
    rilSession.registerImsUnsolRetrievingGeoLocationDataStatus([](RIL_IMS_GeoLocationDataStatus status) {
        std::cout << "[ImsMenu]: status = " + std::to_string(status) << std::endl;
        std::cout << "Received RetrievingGeoLocationDataStatus." << std::endl;
    });

    rilSession.registerImsUnsolHandoverIndicationHandler([](const RIL_IMS_HandoverInfo& handoverInfo) {
        std::cout << "Received ImsUnsolHandoverIndication" << std::endl;
        std::cout << handoverInfo << std::endl;
    });

    rilSession.registerImsUnsolRefreshConferenceInfo([](const RIL_IMS_RefreshConferenceInfo& confInfo) {
        std::cout << "Received ImsUnsolRefreshConferenceInfo" << std::endl;
        std::cout << confInfo << std::endl;
    });

    rilSession.registerImsUnsolMultiIdentityPendingInfo([]() {
        std::cout << "Received ImsUnsolMultiIdentityPendingInfo" << std::endl;
    });

    rilSession.registerImsUnsolMultiIdentityRegistrationStatus([](const RIL_IMS_MultiIdentityLineInfo* info,
        size_t len) {
        std::cout << "Received ImsUnsolMultiIdentityRegistrationStatus" << std::endl;
        if(info) {
            for(int i=0; i<len; i++) {
                std::cout << info[i] << std::endl;
            }
        }
    });

    rilSession.registerImsVowifiQualityIndicationHandler([](const RIL_IMS_VowifiCallQuality&
            vowifiCallQuality) {
        std::cout << "[ImsMenu]: vowifi call quality = " +
            std::to_string(vowifiCallQuality) << std::endl;
        std::cout << "Received vowifi call quality indication." << std::endl;
    });

    rilSession.registerImsUnsolEnterEcbmIndicationHandler([]() {
        std::cout << "Received ImsUnsolEnterEcbmIndication" << std::endl;
    });

    rilSession.registerImsUnsolExitEcbmIndicationHandler([]() {
        std::cout << "Received ImsUnsolExitEcbmIndication" << std::endl;
    });

    rilSession.registerImsUnsolVopsChangedIndicationHandler(
        [](const bool& vopsStatus) {
        std::cout << "Received ImsUnsolVopsChangedIndication." << std::endl;
        std::cout << vopsStatus << std::endl;
    });

    rilSession.registerImsIncomingSmsHandler(
        [] (const RIL_IMS_IncomingSms& imsSms) {
            std::cout << std::endl << imsSms << std::endl;
        }
    );

    rilSession.registerImsSmsStatusReportHandler(
        [] (const RIL_IMS_SmsStatusReport& imsSmsStatusReport) {
            std::cout << std::endl << imsSmsStatusReport << std::endl;
        }
    );

    rilSession.registerImsViceInfoHandler(
        [] (const std::vector<uint8_t>& viceInfo) {
            std::cout << std::endl << "== IMS VICE Info Notification =="
                << std::endl << std::string(viceInfo.begin(), viceInfo.end())
                << std::endl;
        }
    );

    rilSession.registerImsTtyNotificationHandler(
        [] (RIL_IMS_TtyModeType ttyMode) {
            std::cout << std::endl << "== IMS TTY Mode Notification =="
                << std::endl << ttyMode << std::endl;
        }
    );

    rilSession.registerImsRttMessageIndicationHandler([](const std::string& rttMessage) {
        std::cout << "[TelSdkConsoleApp]: UnsolImsRttMessage = " + rttMessage << std::endl;
        std::cout << "Received rttMessage indication." << std::endl;
    });

    rilSession.registerImsGeolocationIndicationHandler([](double latitude, double longitude) {
        std::cout << "[TelSdkConsoleApp]: latitude = " + std::to_string(latitude) << std::endl;
        std::cout << "[TelSdkConsoleApp]: latitude = " + std::to_string(longitude) << std::endl;
        std::cout << "Received geolocation info indication." << std::endl;
    });

    rilSession.registerImsVowifiQualityIndicationHandler([](const RIL_IMS_VowifiCallQuality&
            vowifiCallQuality) {
        std::cout << "[TelSdkConsoleApp]: vowifi call quality = " +
            std::to_string(vowifiCallQuality) << std::endl;
        std::cout << "Received vowifi call quality indication." << std::endl;
    });
}

void ImsMenu::imsGetClir(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsGetClir([](RIL_Errno err, const RIL_IMS_ClirInfo & clirInfo) -> void {
    std::cout << "Got response for GetClir request." << err << std::endl;
    std::cout << "action: " << clirInfo.action << std::endl;
    std::cout << "presentation: " << clirInfo.presentation << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsGetClir" << std::endl;
  }
}

istream& operator >> (istream &in, RIL_IMS_SetClirInfo &setClir)
{
    in >> setClir.action;
    return in;
}

void ImsMenu::imsSetClir(std::vector<std::string> userInput) {
  RIL_IMS_SetClirInfo info{};
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSetClir(info, [](RIL_Errno err) -> void {
    std::cout << "Got response for SetClir request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSetClir" << std::endl;
  }
}

ostream& operator<<(ostream& os, const RIL_IMS_EctType arg) {
    os << "IMS Ect Type : ";
    switch(arg) {
        case RIL_IMS_ECT_TYPE_UNKNOWN:
            os << "RIL_IMS_ECT_TYPE_UNKNOWN";
            break;
        case RIL_IMS_ECT_TYPE_BLIND_TRANSFER:
            os << "RIL_IMS_ECT_TYPE_BLIND_TRANSFER";
            break;
        case RIL_IMS_ECT_TYPE_ASSURED_TRANSFER:
            os << "RIL_IMS_ECT_TYPE_ASSURED_TRANSFER";
            break;
        case RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER:
            os << "RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER";
            break;
    }
    return os;
}

istream& operator >> (istream &in, RIL_IMS_EctType &arg)
{
    int var;
    cout << "0. RIL_IMS_ECT_TYPE_UNKNOWN" << std::endl;
    cout << "1. RIL_IMS_ECT_TYPE_BLIND_TRANSFER" << std::endl;
    cout << "2. RIL_IMS_ECT_TYPE_ASSURED_TRANSFER" << std::endl;
    cout << "3. RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER" << std::endl;
    do {
        cout << "Enter RIL_IMS_EctType: " << std::endl;
        in >> var;
    } while (var < 0 || var > 3);
    arg = static_cast<RIL_IMS_EctType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_ExplicitCallTransfer &explicitCall)
{
  std::cout << "Enter callId:" << std::endl;
  in >> explicitCall.callId;

  in >> explicitCall.ectType;

  std::string tempString;
  std::cout << "Enter targetAddress:" << std::endl;
  in >> tempString;
  explicitCall.targetAddress = new char[tempString.size()+1]();
  tempString.copy(explicitCall.targetAddress, tempString.size());

  std::cout << "Enter targetCallId:" << std::endl;
  in >> explicitCall.targetCallId;
  return in;
}

void ImsMenu::imsExplicitCallTransfer(std::vector<std::string> userInput) {
  RIL_IMS_ExplicitCallTransfer info {};
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsExplicitCallTransfer(info, [](RIL_Errno err) -> void {
    std::cout << "Got response for ExplicitCallTransfer request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsExplicitCallTransfer" << std::endl;
  }
}

istream& operator >> (istream &in, RIL_IMS_TtyModeType &arg)
{
    int var;
    cout << "0. RIL_IMS_TTY_MODE_OFF" << std::endl;
    cout << "1. RIL_IMS_TTY_MODE_FULL" << std::endl;
    cout << "2. RIL_IMS_TTY_MODE_HCO" << std::endl;
    cout << "3. RIL_IMS_TTY_MODE_VCO" << std::endl;
    cout << "4. RIL_IMS_TTY_MODE_INVALID" << std::endl;
    do {
        cout << "Enter RIL_IMS_TtyModeType: " << std::endl;
        in >> var;
    } while (var < 0 || var > 4);
    arg = static_cast<RIL_IMS_TtyModeType>(var);
    return in;
}

istream& operator >> (istream &in, RIL_IMS_ServiceClassStatus &arg)
{
    int var;
    cout << "0. RIL_IMS_SERVICE_CLASS_STATUS_DISABLED" << std::endl;
    cout << "1. RIL_IMS_SERVICE_CLASS_STATUS_ENABLED" << std::endl;
    do {
        cout << "Enter RIL_IMS_ServiceClassStatus: " << std::endl;
        in >> var;
    } while (var < 0 || var > 1);
    arg = static_cast<RIL_IMS_ServiceClassStatus>(var);
    return in;
}


istream& operator >> (istream &in, RIL_IMS_TtyNotifyInfo &ttyInfo)
{
    in >> ttyInfo.mode;
    in >> ttyInfo.userData;
    return in;
}

void ImsMenu::imsSendUiTtyMode(std::vector<std::string> userInput) {
  RIL_IMS_TtyNotifyInfo ttyInfo {};
  cin >> ttyInfo;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSendUiTtyMode(ttyInfo, [](RIL_Errno err) -> void {
    std::cout << "Got response for SendUiTtyMode request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSendUiTtyMode" << std::endl;
  }
}

istream& operator >> (istream &in, RIL_IMS_DeflectCallInfo &deflectCall)
{
    in >> deflectCall.connIndex;
    in >> deflectCall.number;
    return in;
}

void ImsMenu::imsDeflectCall(std::vector<std::string> userInput) {
  RIL_IMS_DeflectCallInfo info {};
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsDeflectCall(info, [](RIL_Errno err) -> void {
    std::cout << "Got response for DeflectCall request." << err << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsDeflectCall" << std::endl;
  }
}

void ImsMenu::imsQueryClip(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsQueryClip([](RIL_Errno err, const RIL_IMS_ClipInfo & clipInfo) -> void {
    std::cout << "Got response for QueryClip request." << err << std::endl;
    std::cout << "clipStatus: " << clipInfo.clipStatus << std::endl;
    std::cout << "errorDetails: " << clipInfo.errorDetails << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsQueryClip" << std::endl;
  }
}

void ImsMenu::imsSetSuppSvcNotification(std::vector<std::string> userInput) {
  RIL_IMS_ServiceClassStatus info;
  cin >> info;
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;
  Status s = rilSession.imsSetSuppSvcNotification(info, [](RIL_Errno err, const RIL_IMS_ServiceClassStatus & srvStatus) -> void {
    std::cout << "Got response for SetSuppSvcNotification request." << err << std::endl;
    std::cout << "srvStatus: " << srvStatus << std::endl;
  });
  if (s != Status::SUCCESS) {
    std::cout << "Failed to send imsSetSuppSvcNotification" << std::endl;
  }
}

std::istream& operator >> (std::istream &in, RIL_IMS_ServiceClassProvisionStatus &status) {
  std::cout << "provisionStatus: (0 - Not provisioned 1 - Provisioned): " << std::endl;
  uint32_t input;
  do {
    in >> input;
  } while (input != 0 && input != 1);
  status = static_cast<RIL_IMS_ServiceClassProvisionStatus>(input);

  return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_SipErrorInfo &info) {
  std::cout << "Input Error Details: " << std::endl;
  uint32_t input;
  in >> input;
  info.errorCode = input;

  std::cout << "any error strings (1 - Yes, 0 - No): " << std::endl;;
  do {
    in >> input;
  } while (input != 0 && input != 1);
  if (input) {
    std::cout << "Input the error string: " << std::endl;
    std::string errorString;
    in >> errorString;
    auto len = errorString.length();
    if (len > 0) {
        info.errorString = new char[len + 1]();
        if (info.errorString) {
            strlcpy(info.errorString, errorString.c_str(), len + 1);
        } else {
            std::cerr << "failed to allocate the error string" << std::endl;
        }
    }
  }
  return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_ColrInfo &info) {
  std::cout << "Input Colr Info - " << std::endl;
  in >> info.status;
  in >> info.provisionStatus;
  in >> info.presentation;

  std::cout << "any error details (1 - Yes, 0 - No): " << std::endl;
  uint32_t input;
  do {
    in >> input;
  } while (input != 0 && input != 1);
  if (input) {
    info.errorDetails = new RIL_IMS_SipErrorInfo;
    if (info.errorDetails) {
        in >> *info.errorDetails;
    } else {
        std::cerr << "failed to allocate error details" << std::endl;
    }
  }
  return in;
}

void ImsMenu::imsSetColr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  RIL_IMS_ColrInfo colr{};
  std::cin >> colr;

  Status s = rilSession.imsSetColr(colr,
        [](RIL_Errno err, const RIL_IMS_SipErrorInfo& errInfo) {
            std::cout << "Got response for imsSetColr: " << err << std::endl
                      << &errInfo << std::endl;

        });
  std::cout << ((s == Status::SUCCESS) ? "request imsSetColr sent succesfully"
                                       : "Failed to send imsSetColr")
            << std::endl;
  if (colr.errorDetails) {
    if (colr.errorDetails->errorString)
        delete []colr.errorDetails->errorString;
    delete colr.errorDetails;
  }
}

std::ostream& operator << (ostream& os, const RIL_IMS_ServiceClassStatus status) {
  os << "IMS Service Class status: ";
  switch (status) {
  case RIL_IMS_SERVICE_CLASS_STATUS_DISABLED:
    os << "IMS_SERVICE_CLASS_STATUS_DISABLED";
    break;
  case RIL_IMS_SERVICE_CLASS_STATUS_ENABLED:
    os << "IMS_SERVICE_CLASS_STATUS_ENABLED";
    break;
  }
  return os;
}

std::ostream& operator << (ostream& os, const RIL_IMS_ServiceClassProvisionStatus status) {
  os << "IMS Service Class Provision status: ";
  switch (status) {
  case RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED:
    os << "IMS_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED";
    break;
  case RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED:
    os << "IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED";
    break;
  }
  return os;
}

std::ostream& operator << (ostream& os, const RIL_IMS_IpPresentation presentation) {
  os << "IMS Ip Presentation: ";
  switch (presentation) {
  case RIL_IMS_IP_PRESENTATION_NUM_ALLOWED:
    os << "IMS_IP_PRESENTATION_NUM_ALLOWED";
    break;
  case RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED:
    os << "IMS_IP_PRESENTATION_NUM_RESTRICTED";
    break;
  case RIL_IMS_IP_PRESENTATION_NUM_DEFAULT:
    os << "IMS_IP_PRESENTATION_NUM_DEFAULT";
    break;
  case RIL_IMS_IP_PRESENTATION_INVALID:
    os << "IMS_IP_PRESENTATION_INVALID";
    break;
  }
  return os;
}

std::ostream& operator << (ostream& os, const RIL_IMS_ColrInfo& colr) {
  os << colr.status << std::endl;
  os << colr.provisionStatus << std::endl;
  os << colr.presentation << std::endl;
  if (colr.errorDetails)
    os << colr.errorDetails << std::endl;
  return os;
}

void ImsMenu::imsGetColr(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  Status s = rilSession.imsGetColr(
        [](RIL_Errno err, const RIL_IMS_ColrInfo& colr) {
            std::cout << "Got response for imsSetColr: " << err << std::endl
                      << colr << std::endl;
        });
  std::cout << ((s == Status::SUCCESS) ? "request imsGetColr sent succesfully"
                                       : "Failed to send imsGetColr")
            << std::endl;
}

std::istream& operator >> (std::istream &in,
        RIL_IMS_MultiIdentityRegistrationStatus &status) {
  uint32_t input;
  do {
    in >> input;
  } while (input > 2);
  status = static_cast<RIL_IMS_MultiIdentityRegistrationStatus>(input);

  return in;
}

std::istream& operator >> (std::istream &in,
        RIL_IMS_MultiIdentityLineType &lineType) {
  uint32_t input;
  do {
    in >> input;
  } while (input > 2);
  lineType = static_cast<RIL_IMS_MultiIdentityLineType>(input);

  return in;
}

std::istream& operator >> (std::istream &in, RIL_IMS_MultiIdentityLineInfo& info) {
  std::string input;
  do {
    std::cout << "Enter msisdn: ";
    in >> input;
  } while (input.empty());
  auto size = input.length();
  info.msisdn = new char[size + 1]();
  if (info.msisdn != nullptr) {
    strlcpy(info.msisdn, input.c_str(), size + 1);
  }
  in >> info.registrationStatus;
  in >> info.lineType;

  return in;
}

void ImsMenu::imsRegisterMultiIdentityLines(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::cout << "Please input the number of line info: ";
  uint32_t input;
  do {
    std::cin >> input;
  } while (input == 0);
  size_t size = input;

  Status s = Status::FAILURE;
  auto lineInfo = new RIL_IMS_MultiIdentityLineInfo[size]();
  if (lineInfo != nullptr) {
    for (int i = 0; i < size; i++) {
        std::cout << "line info " << i << ": " << std::endl;
        std::cin >> lineInfo[i];
    }
    s = rilSession.imsRegisterMultiIdentityLines(lineInfo, size,
            [](RIL_Errno err) {
                std::cout << "Got response for imsRegisterMultiIdentityLines: "
                          << err << std::endl;
            });
    for (int i = 0; i < size; i++) {
        if (lineInfo[i].msisdn) delete []lineInfo[i].msisdn;
    }
    delete []lineInfo;
  }

  std::cout << ((s == Status::SUCCESS)
                ? "request imsRegisterMultiIdentityLines sent succesfully"
                : "Failed to send imsRegisterMultiIdentityLines")
            << std::endl;
}

void ImsMenu::imsQueryVirtualLineInfo(std::vector<std::string> userInput) {
  for (auto& arg : userInput) std::cout << __func__ << ":" << arg << std::endl;

  std::string input;
  do {
    std::cout << "Enter msisdn: ";
    std::cin >> input;
  } while (input.empty());

  Status s = rilSession.imsQueryVirtualLineInfo(input.c_str(),
        [](RIL_Errno err, const RIL_IMS_QueryVirtualLineInfoResponse& info) {
            std::cout << "Got response for imsQueryVirtualLineInfo: "
                      << err << std::endl
                      << "msisdn: " << (info.msisdn ? info.msisdn : "null") << std::endl
                      << "Number of lines: " << info.numLines << std::endl;
            if (info.virtualLines != nullptr) {
                for (int i = 0; i < info.numLines; i++) {
                    std::cout << "Line " << i << ": " << (info.virtualLines[i] ?
                            info.virtualLines[i] : "null") << std::endl;
                }
            }
        });
  std::cout << ((s == Status::SUCCESS)
                ? "request imsQueryVirtualLineInfo sent succesfully"
                : "Failed to send imsQueryVirtualLineInfo")
            << std::endl;
}
