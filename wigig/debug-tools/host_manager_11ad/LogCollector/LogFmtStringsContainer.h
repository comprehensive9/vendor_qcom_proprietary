/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once
#include "OperationStatus.h"
#include <vector>

class LogFmtStringsContainer
{
public:
    LogFmtStringsContainer() : m_valid(false) {}

    OperationStatus Reset(const std::string& stringConversionFileName);
    bool IsValid() const { return m_valid; } // true if succeed to read string conversion file

    bool IsOffsetValid(uint32_t offset) const { return offset < m_fmtStringOffsetBitMap.size() && m_fmtStringOffsetBitMap[offset]; }
    const char* GetFmtString(uint32_t offset) const { return m_stringConversionBuffer.data() + offset; } // no offset validation, check with IsOffsetValid() first
    const std::string& GetModuleNameByIndex(unsigned int index) const;

    friend std::ostream& operator<<(std::ostream &output, const LogFmtStringsContainer& fmtStringsContainer);

private:
    static const unsigned MaxModules = 16;
    std::string m_moduleNames[MaxModules + 1];

    std::vector<char> m_stringConversionBuffer;
    // represents bit map of format strings offsets (bit is true if it is start of format string)
    // note: vector<bool> uses a packed representation where each element occupies a single bit instead of sizeof(bool) bytes
    std::vector<bool> m_fmtStringOffsetBitMap;
    bool m_valid;

    OperationStatus ReadStringConversionFile(const std::string& fileName);
    void ReadModuleNames();
    const char *GetNextModule(const char *mod) const;
};
