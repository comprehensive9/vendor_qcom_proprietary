/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "DebugLogger.h"

class PmcDeviceConfigurations
{
    public:
        PmcDeviceConfigurations()
            : m_collectIdleSmEvents(true)
            , m_collectRxPpduEvents(true)
            , m_collectTxPpduEvents(true)
            , m_collectUcodeEvents(true)
        {
        }

        bool IsCollectIdleSmEvents() const
        {
            return m_collectIdleSmEvents;
        }

        void SetCollectIdleSmEvents(bool collectIdleSmEvents)
        {
            m_collectIdleSmEvents = collectIdleSmEvents;
        }

        bool IsCollectRxPpduEvents() const
        {
            return m_collectRxPpduEvents;
        }

        void SetCollectRxPpduEvents(bool collectRxPpduEvents)
        {
            m_collectRxPpduEvents = collectRxPpduEvents;
        }

        bool IsCollectTxPpduEvents() const
        {
            return m_collectTxPpduEvents;
        }

        void SetCollectTxPpduEvents(bool collectTxPpduEvents)
        {
            m_collectTxPpduEvents = collectTxPpduEvents;
        }

        bool IsCollectUcodeEvents() const
        {
            return m_collectUcodeEvents;
        }

        void SetCollectUcodeEvents(bool collectUcodeEvents)
        {
            m_collectUcodeEvents = collectUcodeEvents;
        }

    private:

        bool m_collectIdleSmEvents; // Enable IDLE_SM state machine events
        bool m_collectRxPpduEvents; // Enable RX PPDU events
        bool m_collectTxPpduEvents; // Enable TX PPDU events
        bool m_collectUcodeEvents;  // Enable selected MSXD commands written by uCode
};

inline std::ostream& operator<<(std::ostream& os, const PmcDeviceConfigurations& config)
{
    return os << "PMC Configuration:"
        << " Collect IDLE_SM: " << BoolStr(config.IsCollectIdleSmEvents())
        << " Collect RX_PPDU: " << BoolStr(config.IsCollectRxPpduEvents())
        << " Collect TX_PPDU: " << BoolStr(config.IsCollectTxPpduEvents())
        << " Collect CORE_WRITE: " << BoolStr(config.IsCollectUcodeEvents());
}