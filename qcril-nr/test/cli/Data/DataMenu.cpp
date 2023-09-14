/*
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved
 */
#include <chrono>

#include <iostream>
extern "C" {
#include "unistd.h"
}
using namespace std;


#include "DataMenu.hpp"

#define UI_INDICATION_FLAG 1

DataMenu::DataMenu(std::string appName, std::string cursor, RilApiSession& rilSession)
   : ConsoleApp(appName, cursor), rilSession(rilSession) {
          std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
   startTime = std::chrono::system_clock::now();
   bool subSystemStatus = true;
   if(subSystemStatus) {
      endTime = std::chrono::system_clock::now();
      std::chrono::duration<double> elapsedTime = endTime - startTime;
      std::cout << "Elapsed Time for Subsystems to ready : " << elapsedTime.count() << "s\n"
                << std::endl;
   } else {
      std::cout << "ERROR - Unable to initialize subSystem" << std::endl;
      exit(0);
   }
}

DataMenu::~DataMenu(){
}

void DataMenu::init() {

    std::vector<std::shared_ptr<ConsoleAppCommand>> commandsListDataSubMenu = {
        CREATE_COMMAND(DataMenu::startDataCall, "Start DataCall"),
        CREATE_COMMAND(DataMenu::stopDataCall, "Stop DataCall"),
        CREATE_COMMAND(DataMenu::SetDataProfile, "Set DataProfile"),
        CREATE_COMMAND(DataMenu::SetInitialAttach, "Set InitialAttach"),
        CREATE_COMMAND(DataMenu::GetDataCallList, "Get DataCall List"),
        CREATE_COMMAND(DataMenu::SetLinkCapFilter, "Set Link Capacity Report Filter"),
        CREATE_COMMAND(DataMenu::SetLinkCapRptCriteria, "Set Link Capacity Report Criteria"),
        CREATE_COMMAND(DataMenu::GetDataNrIconType, "Get DataNrIconType"),
        CREATE_COMMAND(DataMenu::SetPreferredDataModem, "Set Preferred DataModem"),
        CREATE_COMMAND(DataMenu::StartKeepAlive, "Start KeepAlive"),
        CREATE_COMMAND(DataMenu::StopKeepAlive, "Stop KeepAlive"),
        CREATE_COMMAND(DataMenu::StartLCE, "Start LCE"),
        CREATE_COMMAND(DataMenu::StopLCE, "Stop LCE"),
        CREATE_COMMAND(DataMenu::PullLCE, "Pull LCE"),
        CREATE_COMMAND(DataMenu::CarrierInfoImsiEncryption, "Set CarrierInfoImsiEncryptionMessage"),
        CREATE_COMMAND(DataMenu::CaptureRilDataDump, "Capture RilDataDump"),
        CREATE_COMMAND(DataMenu::registerDataUnsolIndication, "Register For Data Unsol Indications"),
        CREATE_COMMAND(DataMenu::registerForDataIndications, "Register Client Callbacks For Indications"),
    };

   addCommands(commandsListDataSubMenu);
   ConsoleApp::displayMenu();
}

/**Utility Functions**/
char* convertStringCalloc(string str)
{
    char * c = (char*) calloc(str.length() + 1, sizeof(char));
    if(c != nullptr) {
        for (int i = 0; i < str.length(); i++) {
            c[i] = str[i];
        }
        c[str.length()] = '\0';
    }
    return c;
}

int inputIntValidator()
{
    string s = "";
    int x = -1;
    while(1) {
        std::cin >> s;
        try {
            x = stoi(s);
        }
        catch(...){
            std::cout << "Value Must be an Integer.Retry" << std::endl;
            continue;
        }
        break;
    }
    return x;
}

unsigned int inputUIntValidator()
{
    while(1){
        int n = inputIntValidator();
        if (n >= 0) {
            return n;
        }
        else {
            std::cout << " Expected value is Unsigned Integer.Retry." << std::endl;
        }
    }
}
/**Utility Functions**/

void DataMenu::CaptureRilDataDump(std::vector<std::string> inputCommand) {
    std::cout << "CaptureRilDataDump enter" << std::endl;
    if (rilSession.captureRilDataDump([] (RIL_Errno err) -> void {
        if (err == RIL_E_SUCCESS) {
            std::cout << "Ril Data Dump Collected Successfully" << std::endl;
        } else {
            std::cout << "Ril Data Dump Collection Failed in File Error" << std::endl;
        }
        }) == Status::FAILURE) {
        std::cout << "CaptureRilDataDump Failed Due to Marshalling Failure." << std::endl;
    }
    sleep(5);
    std::cout << "CaptureRilDataDump exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::CarrierInfoImsiEncryption(std::vector<std::string> inputCommand) {
    std::cout << " SetCarrierInfoImsiEncryptionMessage" << std::endl;
    RIL_CarrierInfoForImsiEncryption imsi = {};
    std::cout << "Enter mcc" << std::endl;
    string mcc;
    cin >> mcc;
    cout << "Enter mnc" << endl;
    string mnc;
    cin >> mnc;
    cout << "Enter Carrier Public Key" << endl;
    string key;
    cin >> key;
    cout << "Enter Key Indentifier" << endl;
    string identifier;
    cin >> identifier;
    cout << "Enter Expiration Time" << endl;
    imsi.expirationTime = inputIntValidator();
    cout << "Enter PublicKeyType\nEnter 1 for EPDG\nEnter 2 for WLAN\nEnter anything else for default" << endl;
    int keytype = 0;
    keytype = inputIntValidator();
    RIL_PublicKeyType type = static_cast<RIL_PublicKeyType>(0);
    if (keytype == 1 || keytype == 2) {
        type = static_cast<RIL_PublicKeyType>(keytype);
    }
    imsi.mcc = (char*) calloc(mcc.length()+1, sizeof(char));
    if (imsi.mcc) {
        strlcpy(imsi.mcc, mcc.c_str(), (mcc.length())+1);
    }
    imsi.mnc = (char*) calloc(mnc.length()+1, sizeof(char));
    if (imsi.mnc) {
        strlcpy(imsi.mnc, mnc.c_str(), (mnc.length())+1);
    }
    imsi.carrierKeyLength = key.length();
    imsi.carrierKey = (uint8_t*) calloc(key.length(), sizeof(uint8_t));
    if (imsi.carrierKey) {
        for (int i = 0 ; i < key.length(); i++) {
            imsi.carrierKey[i] = static_cast<uint8_t>(key[i]);
        }
    }
    imsi.keyIdentifier = (char*) calloc(identifier.length()+1, sizeof(char));
    if (imsi.keyIdentifier) {
        strlcpy(imsi.keyIdentifier, identifier.c_str(), (identifier.length())+1);
    }
    if (rilSession.CarrierInfoForImsiEncryption(imsi, type,  [] (const RIL_Errno err, const Status status, const RIL_Errno* resp) -> void {
        std::cout << "Got response for Set CarrierInfoForImsiEncryption request: " + std::to_string(err) << std::endl;
        if (status == Status::FAILURE) {
            std::cout << "Set CarrierInfoForImsiEncryption : Failure in socket data read" << std::endl;
            return;
        }
        if (resp) {
            std::cout << "Set CarrierInfoForImsiEncryption Response :" + std::to_string(*resp) << std::endl;
        }
        else {
            std::cout << "Set CarrierInfoForImsiEncryption Response is NULL" << std::endl;
        }
    }) == Status::FAILURE) {
        std::cout << "Set CarrierInfoImsiEncryption Failed Due to Marshalling Failure." << std::endl;
    }
    if (imsi.mcc) {
        free(imsi.mcc);
        imsi.mcc = NULL;
    }
    if (imsi.mnc) {
        free(imsi.mnc);
        imsi.mnc = NULL;
    }
    if (imsi.keyIdentifier){
        free(imsi.keyIdentifier);
        imsi.keyIdentifier = NULL;
    }
    if (imsi.carrierKey) {
        free(imsi.carrierKey);
        imsi.carrierKey = NULL;
    }
    sleep(20);
    std::cout << " SetCarrierInfoImsiEncryptionMessage exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::StartLCE(std::vector<std::string> inputCommand) {
    std::cout << " StartLCE (Might Not be Supported in New Modems)" << std::endl;
    int32_t interval = -1;
    int32_t mode = -1;
    std::cout << "Enter Interval" << endl;
    std::cin >> interval;
    std::cout << "Enter Mode. Enter 0 for PUSH and 1 for PULL" << std::endl;
    std::cin >> mode;
    rilSession.StartLceData( interval, mode, [] (RIL_Errno err, const RIL_LceStatusInfo* response) -> void {
        std::cout << "Got response for StartLCEData request: " + std::to_string(err) << std::endl;
        if (response) {
            std::cout << " Start LCE : LCE status ="
            + std::to_string(response->lce_status) << std::endl;
            std::cout << " Start LCE : Actual Interval in ms = "
            + std::to_string(response->actual_interval_ms) << std::endl;
        }
    });
    sleep(20);
    std::cout << " StartLCE exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::StopLCE(std::vector<std::string> inputCommand) {
    std::cout << " StopLCE (Might Not be Supported in New Modems)" << std::endl;
    rilSession.StopLceData( [] (RIL_Errno err, const RIL_LceStatusInfo* response) -> void {
        std::cout << "Got response for StopLCEData request: " + std::to_string(err) << std::endl;
        if (response) {
            std::cout << " Stop LCE : LCE status ="
            + std::to_string(response->lce_status) << std::endl;
            std::cout << " Stop LCE : Actual Interval in ms = "
            + std::to_string(response->actual_interval_ms) << std::endl;
        }
    });
    sleep(20);
    std::cout << " StopLCE exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::PullLCE(std::vector<std::string> inputCommand) {
    std::cout << " PullLCE (Might Not be Supported in New Modems)" << std::endl;
    rilSession.PullLceData( [] (RIL_Errno err, const RIL_LceDataInfo* response) -> void {
        std::cout << "Got response for PullLCEData request: " + std::to_string(err) << std::endl;
        if (response) {
            std::cout << " Pull LCE : Last hop capacity in kbps ="
            + std::to_string(response->last_hop_capacity_kbps) << std::endl;
            std::cout << " Pull LCE : Confidence level = "
            + std::to_string(response->confidence_level) << std::endl;
            std::cout << " Pull LCE : LCE suspended status="
            + std::to_string(response->lce_suspended) << std::endl;
        }
    });
    sleep(20);
    std::cout << " PullLCE exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::StopKeepAlive(std::vector<std::string> inputCommand) {
    std::cout << "\nStop KeepAlive" << std::endl;
    int handle = -1;
    std::cout << "StopKeepAlive : Enter handle id" << std::endl;
    handle = inputIntValidator();
    rilSession.StopKeepAlive(handle, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response) -> void {
        std::cout << "Got response for StopKeepAlive request: " + std::to_string(err) << std::endl;
        if (status == Status::FAILURE) {
            std::cout << "StopKeepAlive : Failure in socket data read" << std::endl;
            return;
        }
        if (response) {
            std::cout << "StopKeepAlive : RIL response is ="
            + std::to_string(*response) << std::endl;
        }
    });
    sleep(20);
    std::cout << " Stop KeepAlive exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::registerDataUnsolIndication(std::vector<std::string> inputCommand) {
    std::cout << "\nRegister Data Unsol Indication" << std::endl;
    int Filter = 0;
    int uiFilter = 0;
    std::cout << "\nUI INFO INDICATION" << std::endl;
    std::cout << "Enter 1 for Registration\nEnter 0 for DeRegistration\n" << std::endl;
    uiFilter = inputUIntValidator();
    if (uiFilter) {
        Filter = Filter | UI_INDICATION_FLAG;
    }
    rilSession.registerDataUnsolIndication(Filter, [] (RIL_Errno err, const Status status, const int* response) -> void {
        std::cout << "Got response for RegisterDataUnsolIndication request: " + std::to_string(err) << std::endl;
        if (status == Status::FAILURE) {
            std::cout << "RegisterDataUnsolIndication : Failure in socket data read" << std::endl;
            return;
        }
        if (response) {
            if (*response == 0) {
                std::cout << "RegisterDataUnsolIndication : Request is Successfully Processed" << std::endl;
            }
            else {
                std::cout << "RegisterDataUnsolIndication : Request Failed" << std::endl;
            }
        }
    });
    sleep(20);
    std::cout << " RegisterDataUnsolIndication exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::StartKeepAlive(std::vector<std::string> inputCommand) {
    std::cout << "\nStart KeepAlive" << std::endl;
    RIL_KeepaliveRequest request = {};
    request.type = RIL_KeepaliveType::NATT_IPV4;
    string s = "";
    std::cout << "[StartKeepAlive]:  enter IPV4 source address" << std::endl;
    std::cin >> s;
    strlcpy(request.sourceAddress, s.c_str(), (s.length()+1));
    s = "";
    std::cout << "[StartKeepAlive]:  enter IPV4 source port" << std::endl;
    request.sourcePort = inputIntValidator();
    std::cout << "[StartKeepAlive]:  enter IPV4 destination address" << std::endl;
    std::cin >> s;
    strlcpy(request.destinationAddress, s.c_str(), (s.length()+1));
    std::cout << "[StartKeepAlive]:  enter IPV4 destination port" << std::endl;
    request.destinationPort = inputIntValidator();
    std::cout << "[StartKeepAlive]:  enter interval in miliseconds" << std::endl;
    request.maxKeepaliveIntervalMillis = inputIntValidator();
    std::cout << "[StartKeepAlive]:  enter cid" << std::endl;
    request.cid = inputIntValidator();
    rilSession.StartKeepAlive(request, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response,
        const uint32_t* handle, const RIL_KeepaliveStatus_t* statuscode) -> void {
        std::cout << "Got response for StartKeepAlive request: " + std::to_string(err) << std::endl;
        if (status == Status::FAILURE) {
            std::cout << "StartKeepAlive : Failure in socket data read" << std::endl;
            return;
        }
        if (response) {
            std::cout << "StartKeepAlive : Response Error is ="
            + std::to_string(*response) << std::endl;
        }
        if (handle) {
            std::cout << " StartKeepAlive : Handle is ="
            + std::to_string(*handle) << std::endl;
        }
        if (statuscode) {
            std::cout << "StartKeepAlive : Status is ="
            + std::to_string(*statuscode) << std::endl;
        }
    });
    sleep(20);
    std::cout << " Start KeepAlive exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::GetDataNrIconType(std::vector<std::string> inputCommand) {
    std::cout << "\nGet Data Nr IconType" << std::endl;
    rilSession.GetDataNrIconType([] (RIL_Errno err, const Status status, const five_g_icon_type* response) -> void {
        std::cout << "Got response for GetDataNrIconType request: " + std::to_string(err) << std::endl;
        if (status == Status::FAILURE) {
            std::cout << "GetDataNrIconType : Failure in socket data read" << std::endl;
            return;
        }
        if (response) {
            std::cout << "GetDataNrIconType : Nr Icon Type is ="
            + std::to_string(*response) << std::endl;
        }
    });
    sleep(20);
    std::cout << " Get Data Nr IconType exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::SetPreferredDataModem(std::vector<std::string> inputCommand) {
    std::cout << "\nSet Preferred DataModem" << std::endl;
    std::cout << " Enter Preferred Modem Id " << std::endl;
    int modemId = inputIntValidator();
    rilSession.SetPreferredDataModem(modemId, [] (RIL_Errno err, const Status status, const RIL_ResponseError* response) -> void {
        std::cout << "Got response for SetPreferredDataModem request: " + std::to_string(err) << std::endl;
        if (status == Status::FAILURE) {
            std::cout << "SetPreferredDataModem : Failure in socket data read" << std::endl;
            return;
        }
        if (response) {
            std::cout << "SetPreferredDataModem:Response is =" + std::to_string(*response) << std::endl;
        }
    });
    sleep(20);
    std::cout << " Set Preferred DataModem exit " << std::endl;
    ConsoleApp::displayMenu();
}

void DataMenu::SetLinkCapRptCriteria(std::vector<std::string> inputCommand) {
    std::cout << "\nSet Link Capacity Criteria" << std::endl;
    std::cout << "Do you want to use default Settings?\nPress 1 for Yes\nPress anything else for No" << std::endl;
    RIL_LinkCapCriteria criteria = {};
    int choice;
    std::cin >> choice;
    if (choice == 1) {
        /*Default Value provided by telephony*/
        criteria.hysteresisMs = 3000;
        criteria.hysteresisDlKbps = 50;
        criteria.hysteresisUlKbps = 50;
        criteria.thresholdsDownLength = 11;
        criteria.thresholdsDownlinkKbps = (int*) calloc (criteria.thresholdsDownLength, sizeof(int));
        if (criteria.thresholdsDownlinkKbps) {
            criteria.thresholdsDownlinkKbps[0] = 100;
            criteria.thresholdsDownlinkKbps[1] = 500;
            criteria.thresholdsDownlinkKbps[2] = 1000;
            criteria.thresholdsDownlinkKbps[3] = 5000;
            criteria.thresholdsDownlinkKbps[4] = 10000;
            criteria.thresholdsDownlinkKbps[5] = 20000;
            criteria.thresholdsDownlinkKbps[6] = 50000;
            criteria.thresholdsDownlinkKbps[7] = 100000;
            criteria.thresholdsDownlinkKbps[8] = 200000;
            criteria.thresholdsDownlinkKbps[9] = 500000;
            criteria.thresholdsDownlinkKbps[10] = 1000000;
        }
        criteria.thresholdsUpLength = 9;
        criteria.thresholdsUplinkKbps = (int*) calloc (criteria.thresholdsUpLength, sizeof(int));
        if (criteria.thresholdsUplinkKbps) {
            criteria.thresholdsUplinkKbps[0] = 100;
            criteria.thresholdsUplinkKbps[1] = 500;
            criteria.thresholdsUplinkKbps[2] = 1000;
            criteria.thresholdsUplinkKbps[3] = 5000;
            criteria.thresholdsUplinkKbps[4] = 10000;
            criteria.thresholdsUplinkKbps[5] = 20000;
            criteria.thresholdsUplinkKbps[6] = 50000;
            criteria.thresholdsUplinkKbps[7] = 100000;
            criteria.thresholdsUplinkKbps[8] = 200000;
        }
        criteria.ran = RIL_RAN::LTE_RAN;
    }
    else {
        std::cout << "\nEnter hysteresisMs" << std::endl;
        criteria.hysteresisMs = inputIntValidator();
        std::cout << "\nEnter hysteresisDlKbps" << std::endl;
        criteria.hysteresisDlKbps = inputIntValidator();
        std::cout << "\nEnter hysteresisUlKbps" << std::endl;
        criteria.hysteresisUlKbps = inputIntValidator();
        std::cout << "\nEnter thresholdsDownLink List Length" << std::endl;
        criteria.thresholdsDownLength = inputIntValidator();
        criteria.thresholdsDownlinkKbps = (int*) calloc (criteria.thresholdsDownLength, sizeof(int));
        if (criteria.thresholdsDownlinkKbps) {
            std::cout << "Populate DownlinkThreshold List in Kbps" << std::endl;
            for(int i = 0; i < criteria.thresholdsDownLength; i++) {
                std::cout << "Element : " + std::to_string(i + 1) << std::endl;
                criteria.thresholdsDownlinkKbps[i] = inputIntValidator();
            }
        }
        std::cout << "\nEnter thresholdsUpLink List Length" << std::endl;
        criteria.thresholdsUpLength = inputIntValidator();
        criteria.thresholdsUplinkKbps = (int*) calloc (criteria.thresholdsUpLength, sizeof(int));
        if (criteria.thresholdsUplinkKbps) {
            std::cout << "Populate UplinkThreshold List in Kbps" << std::endl;
            for(int i = 0; i < criteria.thresholdsUpLength; i++) {
                std::cout << "Element : " + std::to_string(i + 1) << std::endl;
                criteria.thresholdsUplinkKbps[i] = inputIntValidator();
            }
        }
        std::cout << "Enter Preferred RAN" << std::endl;
        std::cout << "Press 1 for GERAN\nPress 2 for WCDMA\nPress 3 for CDMA" << std::endl;
        std::cout << "Press Anything else for LTE" << std::endl;
        int ran;
        ran = inputIntValidator();
        switch(ran) {
            case 1:
                criteria.ran = RIL_RAN::GERAN_RAN;
                break;
            case 2:
                criteria.ran = RIL_RAN::WCDMA_RAN;
                break;
            case 3:
                criteria.ran = RIL_RAN::CDMA_RAN;
                break;
            default:
                criteria.ran = RIL_RAN::LTE_RAN;
        }
    }
    rilSession.SetLinkCapRptCriteria(criteria, [] (RIL_Errno err, const RIL_LinkCapCriteriaResult* response) -> void {
        std::cout << "Got response for SetLinkCapRptCriteriaMessage request: " + std::to_string(err) << endl;
        if (response) {
            std::cout << "SetLinkCapRptCriteria : Response is ="
            + std::to_string(static_cast<uint8_t>(*response)) << endl;
        }
        });
    sleep(20);
    if (criteria.thresholdsDownlinkKbps) {
        free(criteria.thresholdsDownlinkKbps);
        criteria.thresholdsDownlinkKbps = NULL;
    }
    if (criteria.thresholdsUplinkKbps) {
        free(criteria.thresholdsUplinkKbps);
        criteria.thresholdsUplinkKbps = NULL;
    }
    cout << " Set Link Capacity Criteria exit "<<endl;
    ConsoleApp::displayMenu();
}

void DataMenu::SetLinkCapFilter (std::vector<std::string> inputCommand) {
    std::cout << "\nSet Link Capacity Filter" << std::endl;
    int enable;
    std::cout << "Do you want to enable Link Capacity Report Filter ?\n Press 1 to enable(default)\n Press 0 to disble" << std::endl;
    enable = inputIntValidator();
    RIL_ReportFilter enable_bit = RIL_ReportFilter::enabled;
    switch(enable) {
        case 0 :
            enable_bit = RIL_ReportFilter::disabled;
            break;
        default :
            enable_bit = RIL_ReportFilter::enabled;
            break;
    }
    rilSession.SetLinkCapFilter(enable_bit, [] (RIL_Errno err, const int* response) -> void {
        cout << "Got response for SetLinkCapFilterMessage request: " + std::to_string(err) << endl;
        if (response) {
            cout << " [SetLinkCapFilter] :Response is =" + std::to_string(*response) << endl;
        }
    });
    sleep(20);
    cout << " Set Link Capacity Filter exit "<<endl;
    ConsoleApp::displayMenu();
}

void DataMenu::startDataCall(std::vector<std::string> inputCommand) {
    std::cout << "\nStart data call" << std::endl;
    std::cout << "Enter Preferred RadioAccessNetworks. Default EUTRAN" << std::endl;
    std::cout << "Press 1 for UNKNOWN\nPress 2 for GERAN\nPress 3 for UTRAN" << std::endl;
    std::cout << "Press 4 for NGRAN\nPress 5 for CDMA\nPress anything else for EUTRAN" << std::endl;
    int choice;
    choice = inputIntValidator();
    RIL_RadioAccessNetworks accessNetwork = (RIL_RadioAccessNetworks)0x03;
    switch(choice) {
        case 1:
            accessNetwork = RIL_RadioAccessNetworks::UNKNOWN;
            break;
        case 2:
            accessNetwork = RIL_RadioAccessNetworks::GERAN;
            break;
        case 3:
            accessNetwork = RIL_RadioAccessNetworks::UTRAN;
            break;
        case 4:
            accessNetwork = RIL_RadioAccessNetworks::NGRAN;
            break;
        case 5:
            accessNetwork = RIL_RadioAccessNetworks::CDMA;
            break;
        default:
            accessNetwork = RIL_RadioAccessNetworks::EUTRAN;
    }
    bool roamingAllowed = true;
    int profileId;
    RIL_RadioDataRequestReasons reason = (RIL_RadioDataRequestReasons)0x01;
    RIL_DataProfileInfo_IRadio_1_4 dataProfile;
    memset(&dataProfile, 0, sizeof(RIL_DataProfileInfo_IRadio_1_4));
    dataProfile.profileId = 1;
    std::string protocoltype = "";
    std::string roamingProtocoltype = "";
    int it = 1;
    while(1) {
        std::cout << "Enter Ip Protocol : " << std::endl;
        std::cout << "Press 1 for IP" << std::endl;
        std::cout << "Press 2 for IPV6" << std::endl;
        std::cout << "Press 3 for IPV4V6" << std::endl;
        it = inputIntValidator();
        if (it > 0 && it < 4) {
            switch(it) {
                case 1 :
                    protocoltype = "IP";
                    break;
                case 2 :
                    protocoltype = "IPV6";
                    break;
                case 3 :
                    protocoltype = "IPV4V6";
            }
            break;
        }
        else {
            std::cout << "Entered protocol is invalid, Only Valid protocol types are IP/IPV6/IPV4V6. " << std::endl;
        }
    }
    dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
    while(1) {
        std::cout << "Enter Roaming Ip Protocol : " << std::endl;
        std::cout << "Press 1 for IP" << std::endl;
        std::cout << "Press 2 for IPV6" << std::endl;
        std::cout << "Press 3 for IPV4V6" << std::endl;
        it = inputIntValidator();
        if (it > 0 && it < 4) {
            switch(it) {
                case 1 :
                    roamingProtocoltype = "IP";
                    break;
                case 2 :
                    roamingProtocoltype = "IPV6";
                    break;
                case 3 :
                    roamingProtocoltype = "IPV4V6";
            }
            break;
        }
        else {
            std::cout << "Entered protocol is invalid, Only Valid protocol types are IP/IPV6/IPV4V6. " << std::endl;
        }
    }
    dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
    std::cout << "Enter Apn : ";
    std::string apn;
    std::cin >> apn;
    dataProfile.apn = const_cast<char*>(apn.c_str());
    dataProfile.authType = 0;
    std::string username("xyz");
    dataProfile.user = const_cast<char*>(username.c_str());
    std::string password("xyz");
    dataProfile.password = const_cast<char*>(password.c_str());
    dataProfile.type = 1;
    dataProfile.maxConnsTime = 0;
    dataProfile.maxConns = 0;
    dataProfile.waitTime = 1;
    dataProfile.enabled = 1;
    dataProfile.supportedTypesBitmask = 0X1;
    std::cout << "\nEnter Preferred RadioAccessFamily. Default LTE" << std::endl;
    std::cout << "Enter 1 for UNKNOWN\nEnter 2 for GPRS\nEnter 3 for EDGE" << std::endl;
    std::cout << "Enter 4 for GSM\nEnter 5 for WCDMA" << std::endl;
    std::cout << "Enter 6 for CDMA\nEnter 7 for EVDO\nEnter 8 for 5G" << std::endl;
    std::cout << "Enter 9 for HS\nEnter 10 for AUTO\nEnter Anything else for LTE" << std::endl;
    dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
    choice = inputIntValidator();
    switch(choice) {
        case 1:
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_UNKNOWN;
            break;
        case 2:
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_GPRS;
            break;
        case 3:
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_EDGE;
            break;
        case 4:
            dataProfile.bearerBitmask = QCRIL_RAF_GSM;
            break;
        case 5:
            dataProfile.bearerBitmask = QCRIL_RAF_WCDMA;
            break;
        case 6:
            dataProfile.bearerBitmask = QCRIL_RAF_CDMA;
            break;
        case 7:
            dataProfile.bearerBitmask = QCRIL_RAF_EVDO;
            break;
        case 8:
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_5G;
            break;
        case 9:
            dataProfile.bearerBitmask = QCRIL_RAF_HS;
            break;
        case 10:
            dataProfile.bearerBitmask = QCRIL_RAF_AUTO;
            break;
        default:
            dataProfile.bearerBitmask = QCRIL_RAF_LTE;
    }
    dataProfile.mtu = 1400;
    dataProfile.preferred =0 ;
    dataProfile.persistent = 1;
    RIL_SetUpDataCallParams callparams = {};
    callparams.accessNetwork = accessNetwork;
    callparams.profileInfo = dataProfile;
    callparams.roamingAllowed = roamingAllowed;
    callparams.reason = reason;
    callparams.addresses = (char*)"";
    callparams.dnses = (char*)"";
    if (rilSession.setupDataCall( callparams, [] (RIL_Errno err, const Status status, const RIL_Data_Call_Response_v11* response) -> void {
        cout<<"Got response for setupDatacall request: " + std::to_string(err)<<endl;
        if(response) {
             cout<<"[StartDataCall]: status :"+std::to_string(response->status)<<endl;
             cout<<"[StartDataCall]: suggestedRetryTime :"+std::to_string(response->suggestedRetryTime)<<endl;
             cout<<"[StartDataCall]: cid :"+std::to_string(response->cid)<<endl;
             cout<<"[StartDataCall]: active :"+std::to_string(response->active)<<endl;
             if(response->type) {
                 std::string str(response->type);
                 cout << "[StartDataCall]:: type :"+str << endl;
             }
             if(response->ifname) {
                std::string str(response->ifname);
                cout<<"[StartDataCall]:: ifname :"+str<<endl;
             }
             if(response->addresses) {
                std::string str(response->addresses);
                cout<<"[StartDataCall]:: addresses :"+str<<endl;
             }
             if(response->dnses) {
                std::string str(response->dnses);
                cout<<"[StartDataCall]:: dnses :"+str<<endl;
             }
             if(response->gateways) {
                std::string str(response->gateways);
                cout<<"[StartDataCall]:: gateways :"+str<<endl;
             }
             if(response->pcscf) {
                std::string str(response->pcscf);
                cout<<"[StartDataCall]:: pcscf :"+str<<endl;
             }
             cout<<"[StartDataCall]:: mtu :"+std::to_string(response->mtu)<<endl;
        } else {
            cout<<"[StartDataCall]:: response is NULL!!!"<<endl;
        }
    }) == Status::FAILURE) {
        cout << "[StartDataCall]: Could not be sent. Failure in Marshalling." << endl;
    }
    sleep(20);
    cout << " SetupDataCall exit "<<endl;
    ConsoleApp::displayMenu();
}

void DataMenu::stopDataCall(std::vector<std::string> inputCommand) {
    cout<<"DeactivateDataCall Enter"<<endl;
    int32_t cid;
    std::cout << "Enter Cid : ";
    cid = inputIntValidator();
    bool reason = true;
    rilSession.deactivateDataCall( cid, reason, [] (RIL_Errno err) -> void {
    cout<<"Got response for deactivateDataCall request: " + std::to_string(err) <<endl; });
    cout<<"waiting for 95 sec for DeactivateDataCall Response"<<endl;
    sleep(95);
    cout<<"DeactivateDataCall Exit"<<endl;
    ConsoleApp::displayMenu();
}

void DataMenu::SetDataProfile(std::vector<std::string> inputCommand) {
    std::cout << "\nSet Data Profile" << std::endl;
    RIL_DataProfileInfo_IRadio_1_4 *dataProfilePtr = nullptr;
    std::cout << "Enter the no of profile you want to Enter" << std::endl;
    unsigned int n = inputUIntValidator();
    dataProfilePtr = new RIL_DataProfileInfo_IRadio_1_4[n];
    for (int i = 0; i < n; i++) {
        std::cout << "Profile Serial no" + std::to_string( i + 1) << endl;
        RIL_DataProfileInfo_IRadio_1_4 dataProfile = {};
        std::cout << "Enter Profile Id: ";
        dataProfile.profileId = inputUIntValidator();
        std::cout << "Enter Apn : ";
        std::string apn;
        std::cin >> apn;
        dataProfile.apn = convertStringCalloc(apn);
        std::string protocoltype = "";
        std::string roamingProtocoltype = "";
        int it = 1;
        while(1) {
            std::cout << "Enter Ip Protocol : " << std::endl;
            std::cout << "Press 1 for IP" << std::endl;
            std::cout << "Press 2 for IPV6" << std::endl;
            std::cout << "Press 3 for IPV4V6" << std::endl;
            it = inputIntValidator();
            if (it > 0 && it < 4) {
                switch(it) {
                    case 1 :
                        protocoltype = "IP";
                        break;
                    case 2 :
                        protocoltype = "IPV6";
                        break;
                    case 3 :
                        protocoltype = "IPV4V6";
                }
                break;
            }
            else {
                std::cout << "Entered protocol is invalid, Only Valid protocol types are IP/IPV6/IPV4V6. " << std::endl;
            }
        }
        dataProfile.protocol = convertStringCalloc(protocoltype);
        while(1) {
            std::cout << "Enter Roaming Ip Protocol : " << std::endl;
            std::cout << "Press 1 for IP" << std::endl;
            std::cout << "Press 2 for IPV6" << std::endl;
            std::cout << "Press 3 for IPV4V6" << std::endl;
            it = inputIntValidator();
            if (it > 0 && it < 4) {
                switch(it) {
                    case 1 :
                        roamingProtocoltype = "IP";
                        break;
                    case 2 :
                        roamingProtocoltype = "IPV6";
                        break;
                    case 3 :
                        roamingProtocoltype = "IPV4V6";
                }
                break;
            }
            else {
                std::cout << "Entered protocol is invalid, Only Valid protocol types are IP/IPV6/IPV4V6. " << std::endl;
            }
        }
        dataProfile.roamingProtocol = convertStringCalloc(roamingProtocoltype);
        std::cout << "Enter AuthType (DEFAULT : NO_PAP_NO_CHAP) :" << std::endl;
        std::cout << "Press 0 for  NO_PAP_NO_CHAP: " << std::endl;
        std::cout << "Press 1 for  PAP_NO_CHAP " << std::endl;
        std::cout << "Press 2 for  NO_PAP_CHAP " << std::endl;
        std::cout << "Press 3 for  PAP_CHAP" << std::endl;
        dataProfile.authType = inputIntValidator();
        if (dataProfile.authType < 0 || dataProfile.authType > 3) {
            std::cout << "Invalid option selected. Defaulting to NO_PAP_NO_CHAP" << std::endl;
            dataProfile.authType = 0;
        }
        std::string user;
        std::string password;
        std::cout << "Enter Username: ";
        std::cin >> user;
        dataProfile.user = convertStringCalloc(user);
        std::cout << "Enter Password: ";
        std::cin >> password;
        dataProfile.password = convertStringCalloc(password);
        std::cout << "Enter Profile Type:(DEFAULT : 3GPP) " << std::endl;
        std::cout << "Press 0 for  COMMON : " << std::endl;
        std::cout << "Press 1 for  3GPP " << std::endl;
        std::cout << "Press 2 for  3GPP2 " << std::endl;
        dataProfile.type = inputIntValidator();
        if (dataProfile.type < 0 || dataProfile.type > 2) {
            std::cout << "Invalid option selected. Defaulting to 3GPP" << std::endl;
            dataProfile.type = 1;
        }
        dataProfile.maxConnsTime = 0;
        dataProfile.maxConns = 0;
        dataProfile.waitTime = 1;
        dataProfile.enabled = 1;
        std::cout << "Enter SupportedTypes BitMask : " << std::endl;
        std::cout << "Values Supported for Bitmask : " << std::endl;
        std::cout << "NONE = 0\nDEFAULT = 1\nMMS = 2\nSUPL = 4\nDUN = 8\nHIPRI = 16\nFOTA = 32\nIMS = 64" << std::endl;
        std::cout << "CBS = 128\nIA = 256\nEMERGENCY = 512\nMCX =1024\nXCAP =2048" << std::endl;
        std::cout << "For Multiple Bitmask type Perform Bitwise Or of the values" << std::endl;
        std::cout << "Example: BitMask : DEFAULT and DUN. Enter Bitwise Or of 1 and 8 = 9" << std::endl;
        dataProfile.supportedTypesBitmask = inputIntValidator();
        std::cout << "Enter Mode : \nPress 1 for LTE(default) \nPress 2 for LTE_CA \nPress 3 for 5G" << std::endl;
        int mode = inputIntValidator();
        switch (mode) {
            case 1 :
                dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
                break;
            case 2 :
                dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE_CA;
                break;
            case 3:
                dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_5G;
                break;
            default:
                dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
                break;
        }
        dataProfile.mtu = 1400;
        std::cout << "Enter Preferred Bit(0/1) : 1 indicates preferred " << std::endl;
        dataProfile.preferred = inputIntValidator();
        if (dataProfile.preferred != 0 && dataProfile.preferred != 1) {
            std::cout << "Invalid value. Defaulting to non-preferred" << std::endl;
            dataProfile.preferred = 0;
        }
        dataProfile.persistent = 1;
        dataProfilePtr[i] = dataProfile;
    }
    if(dataProfilePtr) {
       int len = n;
       cout << " Sending SetDataProfile  " << endl;
       rilSession.setDataProfile( dataProfilePtr, len, [] (RIL_Errno err) ->
       void { cout << "Got response for setDataProfile request: " + std::to_string(err) << endl; });
    }
    sleep(20);
    if (dataProfilePtr) {
        for (int i = 0; i < n; i++) {
            if (dataProfilePtr[i].apn) {
                delete dataProfilePtr[i].apn;
                dataProfilePtr[i].apn = nullptr;
            }
            if (dataProfilePtr[i].roamingProtocol) {
                delete dataProfilePtr[i].roamingProtocol;
                dataProfilePtr[i].roamingProtocol = nullptr;
            }
            if (dataProfilePtr[i].protocol) {
                delete dataProfilePtr[i].protocol;
                dataProfilePtr[i].protocol = nullptr;
            }
            if (dataProfilePtr[i].user) {
                delete dataProfilePtr[i].user;
                dataProfilePtr[i].user = nullptr;
            }
            if (dataProfilePtr[i].password) {
                delete dataProfilePtr[i].password;
                dataProfilePtr[i].password = nullptr;
            }
        }
        delete []dataProfilePtr;
        dataProfilePtr = nullptr;
    }
    cout<<"Set Data Profile Exit"<<endl;
    ConsoleApp::displayMenu();
}

void DataMenu::SetInitialAttach(std::vector<std::string> inputCommand) {
    std::cout << "\nSet Initial Attach" << std::endl;
    RIL_DataProfileInfo_IRadio_1_4 dataProfile = {};
    std::cout << "Enter Profile Id: ";
    dataProfile.profileId = inputUIntValidator();
    std::cout << "Enter Apn : ";
    std::string apn;
    std::cin >> apn;
    dataProfile.apn = const_cast<char*>(apn.c_str());
    std::string protocoltype = "";
    std::string roamingProtocoltype = "";
    int it = 1;
    while(1) {
        std::cout << "Enter Ip Protocol : " << std::endl;
        std::cout << "Press 1 for IP" << std::endl;
        std::cout << "Press 2 for IPV6" << std::endl;
        std::cout << "Press 3 for IPV4V6" << std::endl;
        it = inputIntValidator();
        if (it > 0 && it < 4) {
            switch(it) {
                case 1 :
                    protocoltype = "IP";
                    break;
                case 2 :
                    protocoltype = "IPV6";
                    break;
                case 3 :
                    protocoltype = "IPV4V6";
            }
            break;
        }
        else {
            std::cout << "Entered protocol is invalid, Only Valid protocol types are IP/IPV6/IPV4V6. " << std::endl;
        }
    }
    dataProfile.protocol = const_cast<char*>(protocoltype.c_str());
    while(1) {
        std::cout << "Enter Roaming Ip Protocol : " << std::endl;
        std::cout << "Press 1 for IP" << std::endl;
        std::cout << "Press 2 for IPV6" << std::endl;
        std::cout << "Press 3 for IPV4V6" << std::endl;
        it = inputIntValidator();
        if (it > 0 && it < 4) {
            switch(it) {
                case 1 :
                    roamingProtocoltype = "IP";
                    break;
                case 2 :
                    roamingProtocoltype = "IPV6";
                    break;
                case 3 :
                    roamingProtocoltype = "IPV4V6";
            }
            break;
        }
        else {
            std::cout << "Entered protocol is invalid, Only Valid protocol types are IP/IPV6/IPV4V6. " << std::endl;
        }
    }
    dataProfile.roamingProtocol = const_cast<char*>(roamingProtocoltype.c_str());
    std::cout << "Enter AuthType (DEFAULT : NO_PAP_NO_CHAP) :" << std::endl;
    std::cout << "Press 0 for  NO_PAP_NO_CHAP: " << std::endl;
    std::cout << "Press 1 for  PAP_NO_CHAP " << std::endl;
    std::cout << "Press 2 for  NO_PAP_CHAP " << std::endl;
    std::cout << "Press 3 for  PAP_CHAP" << std::endl;
    dataProfile.authType = inputIntValidator();
    if (dataProfile.authType < 0 || dataProfile.authType > 3) {
        std::cout << "Invalid option selected. Defaulting to NO_PAP_NO_CHAP" << std::endl;
        dataProfile.authType = 0;
    }
    std::string user;
    std::string password;
    std::cout << "Enter Username: ";
    std::cin >> user;
    dataProfile.user = const_cast<char*>(user.c_str());
    std::cout << "Enter Password: ";
    std::cin >> password;
    dataProfile.password = const_cast<char*>(password.c_str());
    std::cout << "Enter Profile Type:(DEFAULT : 3GPP) " << std::endl;
    std::cout << "Press 0 for  COMMON : " << std::endl;
    std::cout << "Press 1 for  3GPP " << std::endl;
    std::cout << "Press 2 for  3GPP2 " << std::endl;
    dataProfile.type = inputIntValidator();
    if (dataProfile.type < 0 || dataProfile.type > 2) {
        std::cout << "Invalid option selected. Defaulting to 3GPP" << std::endl;
        dataProfile.type = 1;
    }
    dataProfile.maxConnsTime = 0;
    dataProfile.maxConns = 0;
    dataProfile.waitTime = 1;
    dataProfile.enabled = 1;
    std::cout << "Enter SupportedTypes BitMask : " << std::endl;
    std::cout << "Values Supported for Bitmask : " << std::endl;
    std::cout << "NONE = 0\nDEFAULT = 1\nMMS = 2\nSUPL = 4\nDUN = 8\nHIPRI = 16\nFOTA = 32\nIMS = 64" << std::endl;
    std::cout << "CBS = 128\nIA = 256\nEMERGENCY = 512\nMCX =1024\nXCAP =2048" << std::endl;
    std::cout << "For Multiple Bitmask type Perform Bitwise Or of the values" << std::endl;
    std::cout << "Example: BitMask : DEFAULT and DUN. Enter Bitwise Or of 1 and 8 = 9" << std::endl;
    dataProfile.supportedTypesBitmask = inputIntValidator();
    std::cout << "Enter Mode : \nPress 1 for LTE(default) \nPress 2 for LTE_CA \nPress 3 for 5G" << std::endl;
    int mode = inputIntValidator();
    switch (mode) {
        case 1 :
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
            break;
        case 2 :
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE_CA;
            break;
        case 3:
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_5G;
            break;
        default:
            dataProfile.bearerBitmask = RIL_RadioAccessFamily::RAF_LTE;
            break;
    }
    dataProfile.mtu = 1400;
    std::cout << "Enter Preferred Bit(0/1) : 1 indicates preferred " << std::endl;
    dataProfile.preferred = inputIntValidator();
    if (dataProfile.preferred != 0 && dataProfile.preferred != 1) {
        std::cout << "Invalid value. Defaulting to non-preferred" << std::endl;
        dataProfile.preferred = 0;
    }
    dataProfile.persistent = 1;
    rilSession.setInitialAttachApn( dataProfile, [] (RIL_Errno err) -> void {
        std::cout << "Got response for setInitialAttachApn request: " + std::to_string(err) << endl; });
    sleep(20);
    cout<<"Set Initial Attach Exit"<<endl;
    ConsoleApp::displayMenu();
}

void DataMenu::GetDataCallList(std::vector<std::string> inputCommand) {
    std::cout << "\nGet DataCall List" << std::endl;
    rilSession.getDataCallList( [] (const RIL_Errno e, const RIL_Data_Call_Response_v11* response, const unsigned long sz) -> void {
        std::cout << "[GetDataCallList]: testGetDataCallList status: " + std::to_string(e)<< endl;
        if(!response) {
            std::cout << "[GetDataCallList]: testGetDataCallList response NULL " << endl;
            return;
        }
        std::cout << "[GetDataCallList]: testGetDataCallList  received Data Call size = "+std::to_string(sz) << endl;
        for (int i = 0; (i < sz); i++) {
            std::cout << "[GetDataCallList]: testGetDataCallList reading entry " << endl;
            std::cout << "[GetDataCallList]: status :"+std::to_string(response[i].status) << endl;
            std::cout << "[GetDataCallList]: suggestedRetryTime :"+std::to_string(response[i].suggestedRetryTime) << endl;
            std::cout << "[GetDataCallList]: cid :"+std::to_string(response[i].cid) << endl;
            std::cout << "[GetDataCallList]: active :"+std::to_string(response[i].active) << endl;
            if(response[i].type) {
                std::string str(response[i].type);
                std::cout << "[GetDataCallList]: type :"+str << endl;
            }
            if(response[i].ifname) {
                std::string str(response[i].ifname);
                std::cout << "[GetDataCallList]: ifname :"+str << endl;
            }
            if(response[i].addresses) {
                std::string str(response[i].addresses);
                std::cout << "[GetDataCallList]: addresses :"+str << endl;
            }
            if(response[i].dnses) {
                std::string str(response[i].dnses);
                std::cout << "[GetDataCallList]: dnses :"+str << endl;
            }
            if(response[i].gateways) {
                std::string str(response[i].gateways);
                std::cout << "[GetDataCallList]: gateways :"+str << endl;
            }
            if(response[i].pcscf) {
                std::string str(response[i].pcscf);
                std::cout << "[GetDataCallList]: pcscf :"+str <<endl;
            }
            std::cout << "[GetDataCallList]: mtu :"+std::to_string(response[i].mtu) << endl;
        }
    });
    sleep(20);
    cout << "Get DataCall List Exit" <<endl;
    ConsoleApp::displayMenu();
}

void DataMenu::registerForDataIndications(std::vector<std::string> userInput) {
    rilSession.registerDataNrIconChangeHandler(
        [] (const Status status, const  five_g_icon_type icon) {
            if (status == Status::FAILURE) {
                std::cout<<"UnsolDataNrIconChange : Failure in socket data read"<<std::endl;
                return;
            }
            std::cout<<"[Qcril-Nr-Console-app]: UnsolDataNrIconChange: NR Icon Type = "
            + std::to_string(static_cast<int>(icon))<<std::endl;
            std::cout<<"Received UnsolDataNrIconChange"<<std::endl;
        }
    );

    rilSession.registerLCEDataChangeHandler(
        [] (const  RIL_LinkCapacityEstimate LCEData) {
            std::cout<<"[Qcril-Nr-Console-app]: UnsolLceDataChanged:Downlink capacity in kbps ="
            + std::to_string(LCEData.downlinkCapacityKbps)<<std::endl;
            std::cout<<"[Qcril-Nr-Console-app]: UnsolLceDataChanged:Uplink capacity in kbps = "
            + std::to_string(LCEData.uplinkCapacityKbps)<<std::endl;
            std::cout<<"Received LCEDataChangeIndication"<<std::endl;
        }
    );

    rilSession.registerDataNrIconChangeHandler(
        [] (const Status status, const  five_g_icon_type icon) {
            if (status == Status::FAILURE) {
                std::cout<<"UnsolDataNrIconChange : Failure in socket data read"<<std::endl;
                return;
            }
            std::cout<<"[Qcril-Nr-Console-app]: UnsolDataNrIconChange: NR Icon Type = "
            + std::to_string(static_cast<int>(icon))<<std::endl;
            std::cout<<"Received UnsolDataNrIconChange"<<std::endl;
        }
    );

    rilSession.registerDataCallListChangedIndicationHandler(
        [] (const RIL_Data_Call_Response_v11 response[], size_t sz) {
            std::cout<<"[Qcril-Nr-Console-app]: RadioDataCallListChangeInd  size = "+std::to_string(sz)<<std::endl;
            for (int i = 0; (i < sz); i++) {
                std::cout<<"[Qcril-Nr-Console-app]: RadioDataCallListChangeInd reading entry "<<std::endl;
                std::cout<<"[Qcril-Nr-Console-app]: status :"+std::to_string(response[i].status)<<std::endl;
                std::cout<<"[Qcril-Nr-Console-app]: suggestedRetryTime :"+std::to_string(response[i].suggestedRetryTime)<<std::endl;
                std::cout<<"[Qcril-Nr-Console-app]: cid :"+std::to_string(response[i].cid)<<std::endl;
                std::cout<<"[Qcril-Nr-Console-app]: active :"+std::to_string(response[i].active)<<std::endl;
                if(response[i].type) {
                    std::string str(response[i].type);
                    std::cout<<"[Qcril-Nr-Console-app]: type :"+str<<std::endl;
                }
                if(response[i].ifname) {
                    std::string str(response[i].ifname);
                    std::cout<<"[Qcril-Nr-Console-app]: ifname :"+str<<std::endl;
                }
                if(response[i].addresses) {
                    std::string str(response[i].addresses);
                    std::cout<<"[Qcril-Nr-Console-app]: addresses :"+str<<std::endl;
                }
                if(response[i].dnses) {
                    std::string str(response[i].dnses);
                    std::cout<<"[Qcril-Nr-Console-app]: dnses :"+str<<std::endl;
                }
                if(response[i].gateways) {
                    std::string str(response[i].gateways);
                    std::cout<<"[Qcril-Nr-Console-app]: gateways :"+str<<std::endl;
                }
                if(response[i].pcscf) {
                    std::string str(response[i].pcscf);
                    std::cout<<"[Qcril-Nr-Console-app]: pcscf :"+str<<std::endl;
                }
                std::cout<<"[Qcril-Nr-Console-app]: mtu :"+std::to_string(response[i].mtu)<<std::endl;
            }
            std::cout<<"Received DataCallListChangedIndication"<<std::endl;
        }
    );

    rilSession.registerPcoDataChangeHandler(
        [] (const  RIL_PCO_Data PcoData) {
            std::cout<<"[Qcril-Nr-Console-app]:: UnsolPcoDataChanged: cid = "
            + std::to_string(PcoData.cid)<<std::endl;
            if (PcoData.bearer_proto) {
                std::string str(PcoData.bearer_proto);
                std::cout<<"[Qcril-Nr-Console-app]:: UnsolPcoDataChanged: BearerProtocol = "
                + str<<std::endl;
            }
            else {
                std::cout<<"[Qcril-Nr-Console-app]:: UnsolPcoDataChanged: bearer_proto is NULL"<<std::endl;
            }
            std::cout<<"[Qcril-Nr-Console-app]:: UnsolPcoDataChanged: PcoId "
            + std::to_string(PcoData.pco_id)<<std::endl;
            if (PcoData.contents) {
                for (int i = 0; i < PcoData.contents_length; i++) {
                    std::cout<<"[Qcril-Nr-Console-app]:: UnsolPcoDataChanged: content"
                    + std::to_string(i) + "=" + std::to_string(PcoData.contents[i])<<std::endl;
                }
            }
            else {
                std::cout<<"[Qcril-Nr-Console-app]:: UnsolPcoDataChanged: contents is NULL"<<std::endl;
            }
            std::cout<<"Received PCODataChangeIndication"<<std::endl;
        }
    );

    rilSession.registerRadioKeepAliveHandler(
        [] (const Status status, const uint32_t handle, const RIL_KeepaliveStatusCode statuscode) {
            if (status == Status::FAILURE) {
                std::cout<<"UnsolRadioKeepAlive : Failure in socket data read"<<std::endl;
                return;
            }
            std::cout<<"[Qcril-Nr-Console-app]: UnsolRadioKeepAlive:Handle ="
            + std::to_string(handle)<<std::endl;
            std::cout<<"[Qcril-Nr-Console-app]: UnsolRadioKeepAlive:Status = "
            + std::to_string(statuscode)<<std::endl;
            std::cout<<"Received RadioKeepAliveIndication"<<std::endl;
        }
    );

    rilSession.registerPhysicalConfigStructHandler(
        [] (const Status status, const  std::vector<RIL_PhysicalChannelConfig> arg) {
            if (status == Status::FAILURE) {
                std::cout << "PhysicalConfigStructUpdate : Failure in socket data read. Exiting testcase" << std::endl;
                return;
            }

            for (int j = 0; j < arg.size(); j++) {
                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: List content = "
                    << std::to_string(j) << std::endl;

                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: status = "
                    << std::to_string(static_cast<int>(arg[j].status)) << std::endl;

                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: BandwidthDownlink = "
                    << std::to_string(arg[j].cellBandwidthDownlink) << std::endl;

                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Network RAT "
                    << std::to_string(static_cast<int>(arg[j].rat)) << std::endl;

                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Range = "
                    << std::to_string(static_cast<int>(arg[j].rfInfo.range)) << std::endl;

                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: ChannelNumber = "
                    << std::to_string(arg[j].rfInfo.channelNumber) << std::endl;

                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Number of Context Ids = "
                    << std::to_string(arg[j].num_context_ids) << std::endl;

                for (int i = 0; i < arg[j].num_context_ids; i++) {
                    std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: Cid "
                        << std::to_string(i) << " = " << std::to_string(arg[j].contextIds[i]) << std::endl;
                }

                std::cout << "[RilApiSessionSampleApp]: PhysicalConfigStructUpdate: physicalCellId = "
                    << std::to_string(arg[j].physicalCellId) << std::endl;
            }

            std::cout << "Received PhysicalConfigStructUpdate" << std::endl;
        }
    );
}
