/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "OperationStatus.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

enum class wil_fw_state;
enum class wil_nl_60g_pmc_cmd;
enum class IfindexState;
struct driver_event_report;
class IDriverCmdHandler;

class NlService final
{
public:
    /* Initialize the netlink interface and test if driver supports driver events
    * interfaceName is the interface name without transport prefix
    * ppState is initialized with a handler to the allocated state
    */
    NlService(const char* interfaceName, bool localSocket);
    ~NlService();

    /* Get bitmask of the driver capabilities */
    OperationStatus NlGetCapabilities(uint32_t& driverCapabilities);

    /* Request to Enable/Disable events publishing to the user space */
    OperationStatus NlEnableDriverEventsTransport(bool enable);

    /* FW Reset through generic driver command */
    OperationStatus NlFwReset();

    /* Retrieve FW health state */
    OperationStatus NlGetFwState(wil_fw_state& fw_state);

    /* Retrieve info for all STA's */
    OperationStatus NlGetStaInfo(std::vector<struct nl_60g_sta_info_entry>& outBuf);

    /* Retrieve FW capabilities */
    OperationStatus NlGetFwCapa(std::vector<uint32_t>& fwCapa);

    // PMC commands
    OperationStatus NlPmcAlloc(uint32_t numDesc, uint32_t payloadLenBytes);
    OperationStatus NlPmcFree();
    OperationStatus NlPmcGetData(uint32_t& bufLenBytesInOut, uint8_t* pBuffer, uint32_t& minDataLengthBytes, bool& hasMoreData);
    OperationStatus NlPmcGetDescData(uint32_t& bufLenBytesInOut, uint8_t* pBuffer, uint32_t& minDataLengthBytes, bool& hasMoreData);

    // IO commands
    OperationStatus NlMemRead(uint32_t address, uint32_t& value, bool& retriableError);
    OperationStatus NlMemReadBlock(uint32_t address, uint32_t size, char *arrBlock);
    OperationStatus NlMemWrite(uint32_t addr, uint32_t val);
    OperationStatus NlMemWriteBlock(uint32_t addr, uint32_t size, const char *valuesToWrite);

    /*
     * Send command to the Driver
     * Notes:
     * Id represents driver command type which is a contract between the Driver and the command initiator.
     * Response is updated only for DRIVER_CMD_GENERIC_COMMAND.
     */
    OperationStatus NlSendDriverCommand(uint32_t id, uint32_t bufLen, const void* pBuffer, uint32_t* pResponse);

    /* Blocking call to get the next driver event */
    OperationStatus NlGetDriverEvent(int cancelationFd, struct driver_event_report* pMessageBuf, IfindexState& ifiState);

    OperationStatus GetLastStatus() const { return OperationStatus(m_lastStatus.IsSuccess(), m_lastStatus.GetStatusMessage()); }

private:
    /* Release the structures for NL communication */
    void NlRelease();
    static int NoSeqCheck(struct nl_msg *msg, void *arg);
    static int ErrorHandler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg);
    static int FinishHandler(struct nl_msg *msg, void *arg);
    static int AckHandler(struct nl_msg *msg, void *arg);
    OperationStatus ConvertErrnoToStatus(int err, bool localSocket);
    static int FamilyHandler(struct nl_msg *msg, void *arg);
    static int WiphyInfoHandler(struct nl_msg *msg, void* arg);
    static int CommandInfoHandler(struct nl_msg *msg, void* arg);
    OperationStatus NlCmdSendAndRecv(struct nl_sock *sk, struct nl_msg *msg, int(*validHandler)(struct nl_msg *, void *), void *validData);
    OperationStatus NlGetMulticastId(const char *family, const char *group, int& groupId);
    static int NlGetLocalSockFd(struct nl_sock *sk);
    void NlSetLocalSockFd(struct nl_sock *sk, int fd);
    static int NlRecvHandler(struct nl_sock *sk, struct sockaddr_nl *nla, unsigned char **buf, struct ucred **creds);
    static int NlSendHandler(struct nl_sock *sk, struct nl_msg *msg);
    int NlConnectLocalSocket(struct nl_sock *sk, const char* interfaceName);
    static int NlEventHandler(struct nl_msg *msg, void *arg);
    OperationStatus NlInitNlGlobals(const char *interfaceName);
    struct nl_msg *NlAllocateNlCmdMsg(int flags, uint8_t cmd);
    OperationStatus NlCmdSendAndRecvNonblock(struct nl_sock *sk, struct nl_msg *msg);
    OperationStatus NlGetPublishEventCapability();
    OperationStatus NlSendDriverCommandInternal(struct nl_sock *sk, uint32_t id, uint32_t bufLen, const void* pBuffer, IDriverCmdHandler* pCmdHandler = nullptr);
    OperationStatus NlPmcGetDataInternal(wil_nl_60g_pmc_cmd pmcCmdId, uint32_t& bufLenBytesInOut, uint8_t* pBuffer,
                                             uint32_t& minDataLengthBytes, bool& hasMoreData);

    /* callbacks handle for synchronous NL commands */
    struct nl_cb *m_cb;
    /* nl socket handle for synchronous NL commands */
    struct nl_sock *m_nl;
    /* nl socket handler for events */
    struct nl_sock *m_nlEvent;
    /* nl socket handle for synchronous PMC commands */
    struct nl_sock *m_nlPmc;
    /* family id for nl80211 events */
    int m_nl80211Id;
    /* interface index of wigig driver */
    int m_ifindex;

    struct nlHandlerHelper* m_nlHandlerHelper;
    /* pointer to preallocated command handler */
    std::unique_ptr<IDriverCmdHandler> m_upCmdHandler;
    /* pointer to preallocated PMC command handler */
    std::unique_ptr<IDriverCmdHandler> m_upPmcCmdHandler;
    /* true if driver has ability to publish WMI events and receive wmi CMD */
    bool m_hasWmiPub;

    bool m_retriableError;
    bool m_isLocalSocket;

    bool m_ifindexAttrPrintWarning;

    OperationStatus m_lastStatus;

    std::mutex m_mutex;
    std::mutex m_pmcMutex;
};