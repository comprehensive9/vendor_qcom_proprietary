/*
 * Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

/**
 * Data Connection Manager Sample Application using Telematics SDK
 * This is used to demonstrate data connection manager APIs like start/stop data
 * calls and profile management
 */

#ifndef DATAMENU_HPP
#define DATAMENU_HPP



#include "RilApiSession.hpp"

#include "console_app_framework/ConsoleApp.hpp"


class DataMenu : public ConsoleApp {
    public:

       void init();
       void startDataCall(std::vector<std::string> inputCommand);
       void stopDataCall(std::vector<std::string> inputCommand);
       void SetDataProfile(std::vector<std::string> inputCommand);
       void SetInitialAttach(std::vector<std::string> inputCommand);
       void GetDataCallList(std::vector<std::string> inputCommand);
       void SetLinkCapFilter(std::vector<std::string> inputCommand);
       void SetLinkCapRptCriteria(std::vector<std::string> inputCommand);
       void GetDataNrIconType(std::vector<std::string> inputCommand);
       void StartKeepAlive(std::vector<std::string> inputCommand);
       void StopKeepAlive(std::vector<std::string> inputCommand);
       void SetPreferredDataModem(std::vector<std::string> inputCommand);
       void StartLCE(std::vector<std::string> inputCommand);
       void StopLCE(std::vector<std::string> inputCommand);
       void PullLCE(std::vector<std::string> inputCommand);
       void CarrierInfoImsiEncryption(std::vector<std::string> inputCommand);
       void registerForDataIndications(std::vector<std::string> userInput);
       void CaptureRilDataDump(std::vector<std::string> inputCommand);
       void registerDataUnsolIndication(std::vector<std::string> inputCommand);
       DataMenu(std::string appName, std::string cursor, RilApiSession& rilSession);
       ~DataMenu();

    private:
       RilApiSession& rilSession;
};

#endif
