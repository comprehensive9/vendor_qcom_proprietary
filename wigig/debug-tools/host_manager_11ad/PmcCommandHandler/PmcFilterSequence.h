/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "OperationStatus.h"

class Device;

class PmcFilterSequence
{
public:

    explicit PmcFilterSequence(const Device& device);

    OperationStatus ConfigureUCodeEvents(bool ucodeCoreWriteEnabled) const;
    OperationStatus ConfigRxMpdu() const;
    OperationStatus ConfigTxMpdu() const;
    OperationStatus ConfigCollectIdleSm(bool configurationValue) const;
    OperationStatus ConfigCollectRxPpdu(bool configurationValue) const;
    OperationStatus ConfigCollectTxPpdu(bool configurationValue) const;
    OperationStatus ConfigCollectUcode(bool configurationValue) const;

private:
    const Device& m_device;
};