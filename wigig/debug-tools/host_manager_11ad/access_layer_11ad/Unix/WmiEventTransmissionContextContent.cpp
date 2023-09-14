/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "WmiEventTransmissionContextContent.h"
#include "DebugLogger.h"

struct NotSupportedWmiCmdEvent
{
    uint8_t moduleId; // original field's name is "mid" (device id), but FW team uses it for moduleId
    uint8_t reserved0;
    uint16_t commandId;
    uint16_t subtypeId;
    uint16_t reserved1;
};

WmiEventTransmissionContext::WmiEventTransmissionContext(uint32_t commandId, uint32_t moduleId, uint32_t subtypeId) :
    m_commandId(commandId),
    m_moduleId(moduleId),
    m_subtypeId(subtypeId),
    m_responseReceived(false)
{}

void WmiEventTransmissionContext::WaitForReply(unsigned timeoutSec)
{
    auto sec = std::chrono::seconds(1);

    std::unique_lock<std::mutex> lck(m_mutex);
    if (!m_conditionVariable.wait_for(lck, timeoutSec * sec, [this] { return m_responseReceived; }))
    {
        // timeout expired, event did not arrive
        m_errorMsg = "Timer expired. No response arrived";
    }
}

void WmiEventTransmissionContext::NotifyReplyArrived(unsigned eventBufferLength, const uint8_t* eventBinaryData)
{
    std::lock_guard<std::mutex> lck(m_mutex);

    if (eventBufferLength == 0U)
    {
        m_errorMsg = "Received WMI event with no payload";
    }
    else
    {
        m_binaryData = std::vector<uint8_t>(eventBinaryData, eventBinaryData + eventBufferLength);

        // padding with zero to DWORD alignment if needed
        auto reminder = eventBufferLength % 4;
        if (reminder != 0)
        {
            int paddingLength = 4 - reminder;
            for (int i = 0; i < paddingLength; ++i)
            {
                m_binaryData.push_back(0);
            }
        }
    }

    m_responseReceived = true;
    m_conditionVariable.notify_one();
}

void WmiEventTransmissionContext::NotifyCommandNotSupported()
{
    std::lock_guard<std::mutex> lck(m_mutex);
    m_errorMsg = "Not supported WMI command";
    m_responseReceived = true;
    m_conditionVariable.notify_one();
}

void WmiEventTransmissionContextContent::OnEventArrived(unsigned eventId, unsigned eventBufferLength, const uint8_t* eventBinaryData)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    static const unsigned NotSupportedCommandEventId = 0xFFFF;
    if (eventId == NotSupportedCommandEventId) // event for unsupported WMI commands arrived
    {
        ProcessNotSupportedCmdEvent(eventBinaryData);
        return;
    }

    auto it = m_eventContexts.find(eventId);
    if (it == m_eventContexts.end()) // no ontarget request is waiting
    {
        return;
    }

    LOG_DEBUG << "Arrived event is onTarget request, eventId=" << Hex<>(eventId) << std::endl;
    (it->second)->NotifyReplyArrived(eventBufferLength, eventBinaryData);
}

void WmiEventTransmissionContextContent::ProcessNotSupportedCmdEvent(const uint8_t* eventBinaryData)
{
    static const uint32_t wmiUTCommandId = 0x900;
    const auto eventStructReceived = reinterpret_cast<const NotSupportedWmiCmdEvent*>(eventBinaryData);

    for (auto &it : m_eventContexts)
    {
        // search for registered unsupported command
        const auto eventContext = it.second;

        if (eventStructReceived->commandId == eventContext->GetCommandId())
        {
            if (eventStructReceived->commandId == wmiUTCommandId // in case WMI UT, we should also check moduleId and subtypeId
                && !(eventContext->GetModuleId() == eventStructReceived->moduleId
                    && eventContext->GetSubtypeId() == eventStructReceived->subtypeId))
            {
                continue;
            }
            eventContext->NotifyCommandNotSupported();
            return;
        }
    }
}

OperationStatus WmiEventTransmissionContextContent::WmiEventTransmissionContextContent::SubscribeToWmiEvent(uint32_t eventId,
        std::shared_ptr<WmiEventTransmissionContext>& eventContext, uint32_t commandId, uint32_t moduleId, uint32_t subtypeId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto result = m_eventContexts.insert(std::make_pair(eventId, std::make_shared<WmiEventTransmissionContext>(commandId, moduleId, subtypeId)));
    if (!result.second)
    {
        return OperationStatus(false, "Already subscribed for event");
    }
    eventContext = (result.first)->second;
    return OperationStatus();
}

void WmiEventTransmissionContextContent::UnsubscribeFromWmiEvent(uint32_t eventId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_eventContexts.erase(eventId);
}

