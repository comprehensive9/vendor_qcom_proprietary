/*
 * Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QCRIL_DATA_LOG_BUFFER_H_
#define _QCRIL_DATA_LOG_BUFFER_H_

/*============================================================================
 FILE:         LocalLogBuffer.h

 OVERVIEW:     Log rildata information

 DEPENDENCIES: Logging, C++ STL
 ============================================================================*/

/* External Includes */
#include <deque>
#include <vector>
#include <string>
#include <mutex>
#include <sys/types.h>

namespace rildata {

/* Namespace pollution avoidance */
using ::std::deque;
using ::std::string;
using ::std::vector;

class LocalLogBuffer {
public:
    LocalLogBuffer(string name, int maxLogs);
    void addLog(string /* log */);
    void addLogWithTimestamp(string /* log */);

#ifndef RIL_FOR_MDM_LE
    void toLogcat();
#endif

    static void toFd(string msg, int fd);
    string getName();
    void setName(string /* name */);
    vector<string> getLogs();
    void flush();
private:
    deque<string> mLogs;
    string mName;
    const size_t mMaxLogs;
}; /* LocalLogBuffer */

}
#endif /* _QCRIL_DATA_LOG_BUFFER_H_ */
