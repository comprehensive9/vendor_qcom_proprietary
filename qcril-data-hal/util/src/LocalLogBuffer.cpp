/*
 * Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*============================================================================
 FILE:         LocalLogBuffer.cpp

 OVERVIEW:     Log rildata information

 DEPENDENCIES: Logging, C++ STL
 ============================================================================*/
#define LOG_TAG "LocalLogBuffer/dump"

/* External Includes */
#include <android/log.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef QMI_RIL_UTF
#include <sys/time.h>
#endif

/* Internal Includes */
#include "LocalLogBuffer.h"

using namespace rildata;

LocalLogBuffer::LocalLogBuffer(string name, int maxLogs) : mName(name),
    mMaxLogs(maxLogs) {
} /* LocalLogBuffer */

void LocalLogBuffer::addLog(string log) {
    while (mLogs.size() > mMaxLogs)
        mLogs.pop_front();
    mLogs.push_back(log);
} /* addLog */

void LocalLogBuffer::addLogWithTimestamp(string log) {
    struct timeval tv = {};
    char timeBuffer[50];
    char timeBufferWithMS[100];
    memset(timeBuffer,0,sizeof(timeBuffer));
    memset(timeBufferWithMS,0,sizeof(timeBufferWithMS));
    std::string finalLog("    ");
#ifndef RIL_FOR_MDM_LE
    //TODO: to enable logging for LE
    gettimeofday(&tv, NULL);
#endif
    strftime(timeBuffer, sizeof(timeBuffer), "%F %X", (std::localtime (&tv.tv_sec)));
    snprintf(timeBufferWithMS, sizeof(timeBufferWithMS), "%s.%03d",timeBuffer, (int)tv.tv_usec/1000);
    finalLog += std::string(std::string(timeBufferWithMS) + " [" + getName() + "]" + log);
    while (mLogs.size() > mMaxLogs)
        mLogs.pop_front();
    mLogs.push_back(finalLog);
} /* addLog */

#ifndef RIL_FOR_MDM_LE
void LocalLogBuffer::toLogcat() {
    for (size_t i = 0; i < mLogs.size(); i++)
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s: %s", mName.c_str(), mLogs[i].c_str());
} /* toLogcat */
#endif

void LocalLogBuffer::toFd(string msg, int fd) {
    write(fd, msg.c_str(), msg.size());
} /* toFd */

void LocalLogBuffer::setName(string name) {
    mName = name;
}

string LocalLogBuffer::getName() {
    return mName;
}

vector<string> LocalLogBuffer::getLogs()
{
    vector<string> retLogs(mLogs.begin(), mLogs.end());
    return retLogs;
}

void LocalLogBuffer::flush() {
    mLogs.clear();
}
