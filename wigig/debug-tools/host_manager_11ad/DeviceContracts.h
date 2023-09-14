/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <ostream>

/*
 * The below are contracts between supported handlers (e.g. StaInfoHandler) and a lower level
 * of a driverAPI class whose APIs are wrapped and exposed by the device APIs.
 * Additional contracts of a lower level are defined by the DriverContracts.h file.
 */

enum class StaConnectionStatus
{
    UNUSED = 0,
    PENDING = 1,
    CONNECTED = 2
};

std::ostream& operator<<(std::ostream& os, const StaConnectionStatus& status);

struct StaInfo
{
    StaInfo(uint16_t cidIndex, uint64_t mac, StaConnectionStatus status, uint16_t mid, uint16_t aid)
        : CidIndex(cidIndex), MAC(mac), Status(status), MID(mid), AID(aid) {}

    const uint16_t CidIndex;
    const uint64_t MAC;
    const StaConnectionStatus Status;
    const uint16_t MID;
    const uint16_t AID;
};

std::ostream& operator<<(std::ostream& os, const StaInfo& staInfo);
