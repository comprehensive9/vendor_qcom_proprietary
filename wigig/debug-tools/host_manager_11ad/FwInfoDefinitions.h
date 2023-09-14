/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <cstdint>
#include <tuple>
#include <limits>
#include <ostream>

/*
 * Define FW info structure which would be sent as a Json encoded string
 * NOTE that the __exactly__ same struct is defined in the side that gets these structures
 */

struct FwVersion
{
    uint32_t m_major;
    uint32_t m_minor;
    uint32_t m_subMinor;
    uint32_t m_build;

    FwVersion() :
        m_major(0), m_minor(0), m_subMinor(0), m_build(0)
    {}

    bool IsWmiOnly() const
    {
        // test if the first bit is set, same as FieldDescription::MaskValue(m_subMinor, 0, 0) == 1
        return (m_subMinor & 0x1) == 1;
    }

    bool operator==(const FwVersion& rhs) const
    {
        return std::tie(m_major, m_minor, m_subMinor, m_build) == std::tie(rhs.m_major, rhs.m_minor, rhs.m_subMinor, rhs.m_build);
    }
};

inline std::ostream& operator<<(std::ostream& os, const FwVersion& fwVersion)
{
    return os << "[" << fwVersion.m_major << "." << fwVersion.m_minor << "." << fwVersion.m_subMinor << "." << fwVersion.m_build << "]";
}

struct FwTimestamp
{
    FwTimestamp(uint32_t hour = 0, uint32_t min = 0, uint32_t sec = 0, uint32_t day = 0, uint32_t month = 0, uint32_t year = 0) :
        m_hour(hour), m_min(min), m_sec(sec), m_day(day), m_month(month), m_year(year)
    {}

    static FwTimestamp CreateInvalidTimestamp()
    {
        return { sc_invalidValue, sc_invalidValue, sc_invalidValue, sc_invalidValue, sc_invalidValue, sc_invalidValue };
    }

    uint32_t m_hour;
    uint32_t m_min;
    uint32_t m_sec;
    uint32_t m_day;
    uint32_t m_month;
    uint32_t m_year;

    bool operator==(const FwTimestamp& rhs) const
    {
        return std::tie(m_hour, m_min, m_sec, m_day, m_month, m_year) == std::tie(rhs.m_hour, rhs.m_min, rhs.m_sec, rhs.m_day, rhs.m_month, rhs.m_year);
    }

    bool IsValid() const
    {
        return m_year != sc_invalidValue;
    }

private:
    static const auto sc_invalidValue = (std::numeric_limits<uint32_t>::max)();
};

inline std::ostream& operator<<(std::ostream& os, const FwTimestamp& timestamp)
{
    if (!timestamp.IsValid())
    {
        return os << "[NA]";
    }

    return os << "[20" << timestamp.m_year << "-" << timestamp.m_month << "-" << timestamp.m_day << " "
        << timestamp.m_hour << ":" << timestamp.m_min << ":" << timestamp.m_sec << "]";
}

struct FwIdentifier
{
    FwVersion m_fwVersion;
    FwTimestamp m_fwTimestamp;
    FwTimestamp m_uCodeTimestamp;

    bool operator==(const FwIdentifier& rhs) const
    {
        return std::tie(m_fwVersion, m_fwTimestamp, m_uCodeTimestamp) == std::tie(rhs.m_fwVersion, rhs.m_fwTimestamp, rhs.m_uCodeTimestamp);
    }

    bool operator!=(const FwIdentifier& rhs) const
    {
        return !(*this == rhs);
    }
};

inline std::ostream& operator<<(std::ostream& os, const FwIdentifier& fwIdentifier)
{
    return os << "FW ver." << fwIdentifier.m_fwVersion << ", timestamp " << fwIdentifier.m_fwTimestamp
                << " (uCode timestamp " << fwIdentifier.m_uCodeTimestamp << ")";
}