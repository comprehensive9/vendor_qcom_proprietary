/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once
#include <mutex>
#include <vector>
#include <map>
#include <condition_variable>
#include <memory>

#include "OperationStatus.h"

class WmiEventTransmissionContext
{
public:
    WmiEventTransmissionContext(uint32_t commandId, uint32_t moduleId, uint32_t subtypeId);

    // Waits untill condition variable notifies or timeout expired
    void WaitForReply(unsigned timeoutSec);
    // Notifies when event arrived and copies event's binary data (with padding to DWORD alignment)
    void NotifyReplyArrived(unsigned eventBufferLength, const uint8_t* eventBinaryData);
    void NotifyCommandNotSupported();
    const std::string& GetErrorMsg() const { return m_errorMsg; }
    const std::vector<uint8_t>& GetBinaryData() const { return m_binaryData; }
    bool IsEventArrived() const { return m_responseReceived && m_errorMsg.empty(); } // 2nd condition covers unsupported command case
    uint32_t GetCommandId() const { return m_commandId;  }
    uint32_t GetModuleId() const { return m_moduleId; }
    uint32_t GetSubtypeId() const { return m_subtypeId; }

private:
    const uint32_t m_commandId;
    const uint32_t m_moduleId;
    const uint32_t m_subtypeId;

    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;
    bool m_responseReceived; // used to prevent spurious wakeup
    // Event's binary payload, should be copied from driver event as source is already dead when we use it
    std::vector<uint8_t> m_binaryData;
    std::string m_errorMsg;
};

class WmiEventTransmissionContextContent
{
public:
    // Called in main loop of polling for incoming driver events
    void OnEventArrived(unsigned eventId, unsigned eventBufferLength, const uint8_t* eventBinaryData);

    OperationStatus SubscribeToWmiEvent(uint32_t eventId, std::shared_ptr<WmiEventTransmissionContext>& eventContext,
                                        uint32_t commandId, uint32_t moduleId, uint32_t subtypeId);
    void UnsubscribeFromWmiEvent(uint32_t eventId);
private:
    std::map<uint32_t, std::shared_ptr<WmiEventTransmissionContext>> m_eventContexts;
    std::mutex m_mutex; // for map m_eventContexts thread-safety

    void ProcessNotSupportedCmdEvent(const uint8_t* eventBinaryData);
};