/*
* Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
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
/*
 * Driver interaction with Linux nl80211/cfg80211
 * Copyright (c) 2002-2015, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2003-2004, Instant802 Networks, Inc.
 * Copyright (c) 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2007, Johannes Berg <johannes@sipsolutions.net>
 * Copyright (c) 2009-2010, Atheros Communications
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "nl_services.h"
#include "nl_contracts.h"
#include "nl80211_copy.h"
#include "DebugLogger.h"
#include "DriverCommandHandlers.h"
#include "DriverContracts.h"
#include "Utils.h"

#include <fstream>
#include <sstream>
#include <memory>
#include <array>

#include <stdarg.h>
#include <inttypes.h>
#include <stdbool.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netlink/netlink.h>
#include <sys/un.h> // for socket address
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <limits>

struct nlErrorCode
{
    int err;
    bool retriableError;
};

struct driverEventReportWithIfindex
{
    int ifindex;
    IfindexState* ifiState;
    struct driver_event_report *evt;

} __attribute__((packed));

// custom macro definition, cannot use nla_for_each_nested having implicit conversion from void*
#define for_each_nested_attribute(attribute_iter, nested_attribute_ptr, bytes_remaining) \
        for (attribute_iter = (struct nlattr *)nla_data(nested_attribute_ptr), bytes_remaining = nla_len(nested_attribute_ptr); \
             nla_ok(attribute_iter, bytes_remaining); \
             attribute_iter = nla_next(attribute_iter, &(bytes_remaining)))

/**
 * nl callback handler for disabling sequence number checking
 */
int NlService::NoSeqCheck(struct nl_msg *msg, void *arg)
{
    (void)msg;
    (void)arg;
    return NL_OK;
}

/**
 * nl callback handler called on error
 */
int NlService::ErrorHandler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
    (void)nla;

    if ( !(err && arg) ) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl error handler" << std::endl;
        return NL_SKIP;
    }

    struct nlErrorCode *ret = reinterpret_cast<struct nlErrorCode *>(arg);
    ret->err = err->error;

    if (ret->err == -NLE_BUSY)
    {
        ret->retriableError = true;
    }

    // expected to have "No such device" (-ENODEV) error upon rmmod
    LOG_VERBOSE << "nl error handler with error: " << ret->err << " errno: " << errno << std::endl;

    return NL_SKIP;
}

/**
 * nl callback handler called after all messages in
 * a multi-message reply are delivered. It is used
 * to detect end of synchronous command/reply
 */
int NlService::FinishHandler(struct nl_msg *msg, void *arg)
{
    (void)msg;

    if (!arg) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl finish handler" << std::endl;
        return NL_SKIP;
    }

    int *ret = static_cast<int *>(arg);
    *ret = 0;
    return NL_SKIP;
}

/**
 * nl callback handler called when ACK is received
 * for a command. It is also used to detect end of
 * synchronous command/reply
 */
int NlService::AckHandler(struct nl_msg *msg, void *arg)
{
    (void)msg;

    if (!arg) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl ack handler" << std::endl;
        return NL_STOP;
    }

    int *err = static_cast<int *>(arg);
    *err = 0;
    return NL_STOP;
}

// helper for translation of known error codes sent by the host driver to operation status
OperationStatus NlService::ConvertErrnoToStatus(int err, bool localSocket)
{
    if (localSocket)
    {
        std::ostringstream oss;
        oss << "failed to send command, error: " << nl_geterror(err) << " (" << err << ")";
        return OperationStatus(false, oss.str());
    }
    else
    {
        switch (err)
        {
        case (-EINVAL):
            return OperationStatus(false, "command not supported by the host driver");
        case (-EOPNOTSUPP):
            return OperationStatus(false, "command not supported for the DUT FW");
        case (-EAGAIN):
            return OperationStatus(false, "command blocked, system may be in Sys Assert");
        case (-EBUSY):
            return OperationStatus(false, "driver busy, SW reset in progress");
        default:
            std::ostringstream oss;
            oss << "failed to send command, error: " << strerror(-err) << " (" << err << ")";
            return OperationStatus(false, oss.str());
        }
    }
}

/**
 * handler for resolving multicast group (family) id
 * used in nl_get_multicast_id below
 */
struct family_data {
    const char *group;
    int id;
};

int NlService::FamilyHandler(struct nl_msg *msg, void *arg)
{
    if ( !(msg && arg) ) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl family handler" << std::endl;
        return NL_SKIP;
    }

    struct family_data *res = static_cast<struct family_data *>(arg);
    struct nlattr *tb[CTRL_ATTR_MAX + 1];
    struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *mcgrp = nullptr;
    int bytes_remaining = 0;

    nla_parse(tb, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
          genlmsg_attrlen(gnlh, 0), nullptr);
    if (!tb[CTRL_ATTR_MCAST_GROUPS])
        return NL_SKIP;

    for_each_nested_attribute(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], bytes_remaining) {
        struct nlattr *tb2[CTRL_ATTR_MCAST_GRP_MAX + 1];

        nla_parse(tb2, CTRL_ATTR_MCAST_GRP_MAX, (struct nlattr *)nla_data(mcgrp),
              nla_len(mcgrp), nullptr);
        if (!tb2[CTRL_ATTR_MCAST_GRP_NAME] ||
            !tb2[CTRL_ATTR_MCAST_GRP_ID] ||
            strncmp((const char*)nla_data(tb2[CTRL_ATTR_MCAST_GRP_NAME]),
                res->group,
                nla_len(tb2[CTRL_ATTR_MCAST_GRP_NAME])) != 0)
            continue;
        res->id = nla_get_u32(tb2[CTRL_ATTR_MCAST_GRP_ID]);
        break;
    };

    return NL_SKIP;
}

/**
 * handler for NL80211_CMD_GET_WIPHY results
 */
int NlService::WiphyInfoHandler(struct nl_msg *msg, void* arg)
{
    if ( !(msg && arg) ) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl wiphy_info handler" << std::endl;
        return NL_SKIP;
    }

    bool *hasWmiPub = static_cast<bool *>(arg);
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct nlattr *attr = nullptr;
    struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
    struct nl80211_vendor_cmd_info *cmd = nullptr;
    int bytes_remaining = 0;

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
          genlmsg_attrlen(gnlh,0) , nullptr);

    if (tb[NL80211_ATTR_VENDOR_DATA]) {
        for_each_nested_attribute(attr, tb[NL80211_ATTR_VENDOR_DATA], bytes_remaining) {
            if (nla_len(attr) != sizeof(*cmd)) {
                /* "unexpected vendor cmd info\n" */
                continue;
            }
            cmd = (struct nl80211_vendor_cmd_info *)nla_data(attr);
            if (cmd->vendor_id == OUI_QCA &&
                cmd->subcmd ==
                QCA_NL80211_VENDOR_SUBCMD_UNSPEC) {
                *hasWmiPub = true;
                break;
            }
        }
    }
    return NL_SKIP;
}

/**
* handler for getting command result value
* assumes that the response handler is provided through the argument
*/
int NlService::CommandInfoHandler(struct nl_msg *msg, void* arg)
{
    if (!(msg && arg)) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl command_info handler" << std::endl;
        return NL_SKIP;
    }

    IDriverCmdHandler *cmdHandler = static_cast<IDriverCmdHandler *>(arg);
    if (!cmdHandler)
    {
        LOG_ERROR << "command_info_handler: no command handler provided when response expected" << std::endl;
        return NL_SKIP;
    }

    struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));

    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), nullptr);

    if (!tb[NL80211_ATTR_VENDOR_DATA])
    {
        LOG_ERROR << "command_info_handler: VENDOR_DATA attribute is missing" << std::endl;
        return NL_SKIP;
    }

    // Parse nested attributes:
    // Note: Cannot use 'nla_parse_nested' becasue of backward compatibility with libnl 3.2.21
    // used on Ubuntu 14. This package does not allow to use type '0' for a nested attribute,
    // meaning first attribute set by the driver will be ignored.
    std::array<struct nlattr*, QCA_WLAN_VENDOR_ATTR_MAX + 1> tb_nested;
    tb_nested.fill(nullptr); // nla_parse will not be called, initialize the array manually

    struct nlattr *attr = nullptr;
    int bytes_remaining = 0;
    for_each_nested_attribute(attr, tb[NL80211_ATTR_VENDOR_DATA], bytes_remaining)
    {
        uint16_t type = nla_type(attr);
        if (type < tb_nested.size())
        {
            tb_nested[type] = attr;
            LOG_VERBOSE << "command_info handler: extracted attribute with type " << type << std::endl;
        }
        else
        {
            LOG_WARNING << "command_info handler: ignored illegal attribute with type " << type
                << ", maximal value expected is " << tb_nested.size() - 1 << std::endl;
        }
    }

    const auto res = cmdHandler->HandleResponse(tb_nested.data());
    return res;
}

/**
 * send NL command and receive reply synchronously
 */
OperationStatus NlService::NlCmdSendAndRecv(
    struct nl_sock *sk,
    struct nl_msg *msg,
    int(*validHandler)(struct nl_msg *, void *),
    void *validData)
{
    if (!(msg && validData)) // shouldn't happen
    {
        return OperationStatus(false, "invalid arguments, arguments cannot be null");
    }

    // smart pointer with custom deleter calling nl_cb_put on every return path
    std::unique_ptr<struct nl_cb, decltype(&nl_cb_put)> cb_clone(nl_cb_clone(m_cb), nl_cb_put);
    if (!cb_clone)
    {
        return OperationStatus(false, "failed to clone callback handle");
    }

    int err = nl_send_auto_complete(sk, msg); // returns number of bytes sent or a negative (libnl) error code
    if (err < 0)
    {
        std::ostringstream oss;
        oss << "failed to send message, error: " << nl_geterror(err) << " (" << err << ")";
        return OperationStatus(false, oss.str());
    }

    struct nlErrorCode errcode {};
    errcode.err = 1;
    nl_cb_err(cb_clone.get(), NL_CB_CUSTOM, ErrorHandler, &errcode);
    nl_cb_set(cb_clone.get(), NL_CB_FINISH, NL_CB_CUSTOM, FinishHandler, &errcode.err);
    nl_cb_set(cb_clone.get(), NL_CB_ACK, NL_CB_CUSTOM, AckHandler, &errcode.err);
    if (validHandler)
    {
        nl_cb_set(cb_clone.get(), NL_CB_VALID, NL_CB_CUSTOM, validHandler, validData);
    }

    while (errcode.err > 0)
    {
        // nl_recvmsgs return 0 on success, no matter how many messages been read,
        // in case closed of socket the return code also 0.
        // In order to distinguish between the cases, update to nl_recvmsgs_report()
        int res = nl_recvmsgs_report(sk, cb_clone.get());
        if (res < 0)
        {
            LOG_DEBUG << "failed to send command, nl_recvmsgs_report failed, error: " << nl_geterror(res) << " (" << res << ")" << std::endl;
            /* do not exit the loop since similar code in supplicant does not */
        }
        if (!res)
        {
            m_retriableError = errcode.retriableError;
            return OperationStatus(false, "failed to send command: probably socket been closed");
        }
    }

    if (errcode.err < 0) // updated by above callbacks
    {
        // convert known error codes sent by the host driver to operation status
        return ConvertErrnoToStatus(errcode.err, m_isLocalSocket);
    }

    return OperationStatus(true);
}

/**
 * get a multicast group id for registering
 * (such as for vendor events)
 */
OperationStatus NlService::NlGetMulticastId(const char *family, const char *group, int& groupId)
{
    if ( !(family && group) ) // shouldn't happen
    {
        return OperationStatus(false, "invalid arguments, arguments cannot be null");
    }

    // smart pointer with custom deleter calling nlmsg_free on every return path
    std::unique_ptr<struct nl_msg, decltype(&nlmsg_free)> msg(nlmsg_alloc(), nlmsg_free);
    if (!msg)
    {
        return OperationStatus(false, "failed to allocate nl message");
    }

    if (!genlmsg_put(msg.get(), 0, 0, genl_ctrl_resolve(m_nl, "nlctrl"),
             0, 0, CTRL_CMD_GETFAMILY, 0) ||
        nla_put_string(msg.get(), CTRL_ATTR_FAMILY_NAME, family))
    {
        return OperationStatus(false, "failed to add generic nl header to nl message");
    }

    struct family_data res = { group, -ENOENT };
    OperationStatus os = NlCmdSendAndRecv(m_nl, msg.get(), FamilyHandler, &res);
    if (os)
    {
        groupId = res.id;
    }

    return os;
}

/*
 * for local sockets only
 * get local socket fd from nl_sock structure
 * the nl_sock->s_fd is not accessible, so we use
 * the port field of the local address to store the fd
 */
int NlService::NlGetLocalSockFd(struct nl_sock *sk)
{
    uint32_t port = nl_socket_get_local_port(sk);
    return (port == (std::numeric_limits<uint32_t>::max)()) ? -1 : (int)(port - 1);
}

/*
 * for local sockets only
 * store local socket fd inside nl_sock structure
 * use local address port since nl_sock->s_fd is
 * not accessible
 */
void NlService::NlSetLocalSockFd(struct nl_sock *sk, int fd)
{
    /*
     * 0 cannot be stored, otherwise nl_socket_get_local_port
     * will return a random port
     */
    uint32_t port = (fd < 0) ? (std::numeric_limits<uint32_t>::max)() : fd + 1;
    nl_socket_set_local_port(sk, port);
}

/*
 * receive handler for libnl. This is used to override the normal libnl
 * receive to use a local socket
 */
int NlService::NlRecvHandler(struct nl_sock *sk, struct sockaddr_nl *nla, unsigned char **buf, struct ucred **creds)
{
    int s_fd = NlGetLocalSockFd(sk);

    if (!buf || !nla)
    {
        return -NLE_INVAL;
    }

    /* nla is ignored since we use local connected socket */
    memset(nla, 0, sizeof(struct sockaddr_nl));

    struct msghdr msg {};
    msg.msg_name = nullptr;
    msg.msg_namelen = 0;

    /* creds is also ignored since we use connected socket */
    msg.msg_control = nullptr;
    msg.msg_controllen = 0;
    if (creds)
    {
        *creds = nullptr;
    }

    /* get the amount of bytes available so we can allocate
     * large enough buffer for receiving
     */
    int avail = 0;
    int rc = ioctl(s_fd, FIONREAD, &avail);
    if (rc < 0)
    {
        LOG_ERROR << "failed F_IONREAD: " << strerror(errno) << std::endl;
        return -NLE_FAILURE;
    }
    if (avail == 0)
    {
        /* this should never happen in non-blocking mode
         * when using poll. In blocking mode it means we
         * are going to do a blocking receive and we can't
         * tell exactly how many bytes will be received.
         * just allocate really large buffer to support
         * everything (like memory blocks)
         */
        avail = 2 * 1024 * 1024; /* 2MB */
    }

    struct iovec iov{};
    iov.iov_len = avail;
    iov.iov_base = malloc(avail); /* buffer freed by libnl */
    if (!iov.iov_base)
    {
        return -NLE_NOMEM;
    }
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    rc = 0;
    bool cont = true;
    int received = 0;
    while (cont)
    {
        cont = false;
        received = recvmsg(s_fd, &msg, 0);
        if (!received)
        {
            rc = 0;
            free(iov.iov_base);
            return rc;
        }

        if (received < 0)
        {
            if (errno == EINTR)
            {
                cont = true;
                continue;
            }
            rc = -nl_syserr2nlerr(errno);
            free(iov.iov_base);
            return rc;
        }
    }

    if (msg.msg_flags & MSG_TRUNC)
    {
        /* this should never happen */
        LOG_ERROR << "truncated message received!!!" << std::endl;
        rc = -NLE_FAILURE;
        free(iov.iov_base);
        return rc;
    }

    rc = received;
    if (rc > 0)
    {
        *buf = (unsigned char *)iov.iov_base;
    }
    else
    {
        free(iov.iov_base);
    }

    return rc;
}

/*
 * send handler for libnl. This is used to override the normal libnl
 * send to use a local socket
 */
int NlService::NlSendHandler(struct nl_sock *sk, struct nl_msg *msg)
{
    int s_fd = NlGetLocalSockFd(sk);
    if (s_fd < 0)
        return -NLE_BAD_SOCK;

    struct nlmsghdr *nlh = nlmsg_hdr(msg);
    struct iovec iov {};
    iov.iov_base = nlh;
    iov.iov_len = nlh->nlmsg_len;

    /* no need to fill in peer, connected socket */
    struct msghdr mhdr {};
    mhdr.msg_name = nullptr;
    mhdr.msg_namelen = 0;
    mhdr.msg_iov = &iov;
    mhdr.msg_iovlen = 1;
    mhdr.msg_control = nullptr;
    mhdr.msg_controllen = 0;

    int rc = sendmsg(s_fd, &mhdr, 0);
    if (rc < 0)
    {
        return -nl_syserr2nlerr(errno);
    }
    return rc;
}

/*
 * connect to local socket of wil6210 DPDK driver
 */
int NlService::NlConnectLocalSocket(struct nl_sock *sk, const char* interfaceName)
{
    int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd < 0)
    {
        LOG_ERROR << "fail to create socket: " << strerror(errno) << std::endl;
        return -1;
    }

    struct sockaddr_un addr {};
    addr.sun_family = AF_LOCAL;

    static const char * const path = "/dev/wil6210";
    std::ostringstream fname;
    fname << path << "/" << interfaceName;

    SafeStringCopy(addr.sun_path, fname.str().c_str(), sizeof(addr.sun_path));

    if (connect(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        LOG_ERROR << "fail to connect socket: " << strerror(errno) << std::endl;
        close(fd);
        return -1;
    }

    NlSetLocalSockFd(sk, fd);

    return 0;
}

/**
 * handle for vendor events
 */
int NlService::NlEventHandler(struct nl_msg *msg, void *arg)
{
    if ( !(msg && arg) ) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl event handler" << std::endl;
        return NL_SKIP;
    }

    struct driverEventReportWithIfindex *driver_event_report = static_cast<struct driverEventReportWithIfindex *>(arg);
    if (!driver_event_report->evt) // shouldn't happen
    {
        LOG_ERROR << "invalid arguments in nl event handler, driver event report buffer cannot be null" << std::endl;
        return NL_SKIP;
    }

    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
          genlmsg_attrlen(gnlh, 0) , nullptr);

    if (!tb[NL80211_ATTR_VENDOR_ID] ||
        !tb[NL80211_ATTR_VENDOR_SUBCMD] ||
        !tb[NL80211_ATTR_VENDOR_DATA])
    {
        return NL_SKIP;
    }

    if (nla_get_u32(tb[NL80211_ATTR_VENDOR_ID]) != OUI_QCA)
    {
        return NL_SKIP;
    }

    struct nlattr *tb2[QCA_WLAN_VENDOR_ATTR_MAX + 1];
    if (nla_parse_nested(tb2, QCA_WLAN_VENDOR_ATTR_MAX,
                tb[NL80211_ATTR_VENDOR_DATA], nullptr))
    {
        /* "failed to parse vendor command\n" */
        return NL_SKIP;
    }

    uint32_t cmd = nla_get_u32(tb[NL80211_ATTR_VENDOR_SUBCMD]);
    switch (cmd)
    {
    case QCA_NL80211_VENDOR_SUBCMD_UNSPEC:
        if (tb2[QCA_WLAN_VENDOR_ATTR_BUF])
        {
            if (tb[NL80211_ATTR_IFINDEX])
            {
                uint32_t driver_ifindex = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);
                if (driver_ifindex != static_cast<uint32_t>(driver_event_report->ifindex))
                {
                    *driver_event_report->ifiState = IfindexState::IFINDEX_STATE_MISMATCH;
                }
                else
                {
                    *driver_event_report->ifiState = IfindexState::IFINDEX_STATE_MATCH;
                }
            }
            else
            {
                *driver_event_report->ifiState = IfindexState::IFINDEX_STATE_MISSING;
            }

            const uint32_t len = nla_len(tb2[QCA_WLAN_VENDOR_ATTR_BUF]);
            if (len > sizeof(struct driver_event_report))
            {
                /* "event respond length is bigger than allocated %d [bytes]\n", sizeof(struct driver_event_report) */
                return NL_SKIP;
            }

            /* evt validity already tested */
            memcpy(driver_event_report->evt, nla_data(tb2[QCA_WLAN_VENDOR_ATTR_BUF]), len);
        }
        break;
    default:
        /* "\nunknown event %d\n", cmd */
        break;
    }

    return NL_SKIP;
}

/**
 * initialize structures for NL communication
 * in case of failure it is the caller responsibility to call destroy_nl_globals
 */
OperationStatus NlService::NlInitNlGlobals(const char *interfaceName)
{
    /* specify NL_CB_DEBUG instead of NL_CB_DEFAULT to get detailed traces of NL messages */
    m_cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (m_cb == nullptr)
    {
        return OperationStatus(false, "failed to allocate nl callback");
    }

    if (nl_cb_set(m_cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, NoSeqCheck, nullptr) < 0)
    {
        return OperationStatus(false, "failed to set nl callback handler (no_seq_check)");
    }

    if (m_isLocalSocket)
    {
        /* set send and receive callbacks for local socket */
        nl_cb_overwrite_recv(m_cb, NlRecvHandler);
        nl_cb_overwrite_send(m_cb, NlSendHandler);
    }

    m_nl = nl_socket_alloc_cb(m_cb);
    if (m_nl == nullptr)
    {
        return OperationStatus(false, "failed to allocate nl socket");
    }

    int rc = m_isLocalSocket ? NlConnectLocalSocket(m_nl, interfaceName) : genl_connect(m_nl);
    if (rc < 0)
    {
        // NlConnectLocalSocket() returns -1, which converts to NLE_FAILURE
        std::ostringstream oss;
        oss << "failed to connect nl command socket, error: " << nl_geterror(rc) << " (" << rc << ")";
        return OperationStatus(false, oss.str());
    }

    if (m_isLocalSocket)
    {
        /* no need to get nl80211_id for local sockets,
         * since we use direct connection by file
         * put dummy family id, it must be >= NLMSG_MIN_TYPE so libnl
         * will not identify it as some system message.
         */
        m_nl80211Id = NLMSG_MIN_TYPE;
    }
    else
    {
        m_nl80211Id = genl_ctrl_resolve(m_nl, "nl80211");
        if (m_nl80211Id < 0)
        {
            std::ostringstream oss;
            oss << "failed to resolve nl80211 family id, error " << m_nl80211Id;
            return OperationStatus(false, oss.str());
        }
    }

    m_nlEvent = nl_socket_alloc_cb(m_cb);
    if (m_nlEvent == nullptr)
    {
        return OperationStatus(false, "failed to allocate nl socket for events");
    }

    rc = m_isLocalSocket ? NlConnectLocalSocket(m_nlEvent, interfaceName) : genl_connect(m_nlEvent);
    if (rc < 0)
    {
        // NlConnectLocalSocket() returns -1, which converts to NLE_FAILURE
        std::ostringstream oss;
        oss << "failed to connect nl event socket, error: " << nl_geterror(rc) << " (" << rc << ")";
        return OperationStatus(false, oss.str());
    }

    m_nlPmc = nl_socket_alloc_cb(m_cb);
    if (m_nlPmc == nullptr)
    {
        return OperationStatus(false, "failed to allocate nl socket for pmc");
    }

    rc = m_isLocalSocket ? NlConnectLocalSocket(m_nlPmc, interfaceName) : genl_connect(m_nlPmc);
    if (rc < 0)
    {
        // NlConnectLocalSocket() returns -1, which converts to NLE_FAILURE
        std::ostringstream oss;
        oss << "failed to connect nl pmc socket, error: " << nl_geterror(rc) << " (" << rc << ")";
        return OperationStatus(false, oss.str());
    }

    if (!m_isLocalSocket)
    {
        /* register for receiving vendor events */
        int group_id = -1;
        OperationStatus os = NlGetMulticastId("nl80211", "vendor", group_id);
        if (!os)
        {
            os.AddPrefix("could not get vendor multicast group id for nl80211 family");
            return os;
        }

        if (nl_socket_add_membership(m_nlEvent, group_id) < 0)
        {
            return OperationStatus(false, "could not register for vendor events");
        }

        if (nl_socket_set_nonblocking(m_nlEvent) < 0)
        {
            return OperationStatus(false, "failed to set events socket to non-blocking state");
        }
    }

    return OperationStatus(true);
}

/**
 * allocate an nl_msg for sending a command
 */

struct nl_msg *NlService::NlAllocateNlCmdMsg(int flags, uint8_t cmd)
{
    // nlmsg_alloc allocates 4KB buffer by default
    // this may not be enough for a WMI command with lengthy payload, replaced with nlmsg_alloc_size
    struct nl_msg *msg = nlmsg_alloc_size(NL_MSG_SIZE_MAX);

    if (!msg)
    {
        /* "failed to allocate nl msg\n" */
        return nullptr;
    }

    int pid = m_isLocalSocket ? 111 : 0;

    if (!genlmsg_put(msg,
              pid, // pid (automatic)
              0, // sequence number (automatic)
              m_nl80211Id, // family
              0, // user specific header length
              flags, // flags
              cmd, // command
              0) // protocol version
        )
    {
        /* "failed to init msg\n" */
        nlmsg_free(msg);
        return nullptr;
    }

    if (nla_put_u32(msg, NL80211_ATTR_IFINDEX, (uint32_t)m_ifindex) < 0)
    {
        /* "failed to set interface index\n" */
        nlmsg_free(msg);
        return nullptr;
    }

    return msg;
}

/**
 * send NL command and receive reply synchronously, for
 * non-blocking sockets
 */
OperationStatus NlService::NlCmdSendAndRecvNonblock(struct nl_sock *sk, struct nl_msg *msg)
{
    static const int polling_timeout_msec = 1000; /* timeout is in msec. */

    if (!msg) // shouldn't happen
    {
        return OperationStatus(false, "invalid arguments, arguments cannot be null");
    }

    // smart pointer with custom deleter calling nl_cb_put on every return path
    std::unique_ptr<struct nl_cb, decltype(&nl_cb_put)> cb_clone(nl_cb_clone(m_cb), nl_cb_put);
    if (!cb_clone)
    {
        return OperationStatus(false, "failed to clone callback handle");
    }

    struct nl_sock *nl = sk;
    int err = nl_send_auto_complete(nl, msg); // returns number of bytes sent or a negative (libnl) error code
    if (err < 0)
    {
        std::ostringstream oss;
        oss << "failed to send message, error: " << nl_geterror(err) << " (" << err << ")";
        return OperationStatus(false, oss.str());
    }

    struct nlErrorCode errcode {};
    errcode.err = 1;
    nl_cb_err(cb_clone.get(), NL_CB_CUSTOM, ErrorHandler, &errcode);
    nl_cb_set(cb_clone.get(), NL_CB_FINISH, NL_CB_CUSTOM, FinishHandler, &errcode.err);
    nl_cb_set(cb_clone.get(), NL_CB_ACK, NL_CB_CUSTOM, AckHandler, &errcode.err);

    struct pollfd fds {};
    memset(&fds, 0, sizeof(fds));
    if (m_isLocalSocket)
    {
        fds.fd = NlGetLocalSockFd(nl);
    }
    else
    {
        fds.fd = nl_socket_get_fd(nl);
    }
    fds.events = POLLIN;
    while (errcode.err > 0)
    {
        int res = poll(&fds, 1, polling_timeout_msec);

        if (res == 0) // timeout
        {
            std::ostringstream oss;
            oss << "failed to send message, poll timeout of " << polling_timeout_msec << " [msec.] reached";
            return OperationStatus(false, oss.str());
        }
        else if (res < 0) // poll failure
        {
            std::ostringstream oss;
            oss << "failed to send message, poll failure: " << strerror(errno);
            return OperationStatus(false, oss.str());
        }

        if (fds.revents & POLLIN)
        {
            res = nl_recvmsgs(nl, cb_clone.get());
            if (res < 0) // shouldn't happen
            {
                std::ostringstream oss;
                oss << "failed to send command, nl_recvmsgs failed, error: " << nl_geterror(res) << " (" << res << ")" << std::endl;
                m_retriableError = errcode.retriableError;
                return OperationStatus(false, oss.str());
            }
        }
    }

    if (errcode.err < 0) // updated by above callbacks
    {
        m_retriableError = errcode.retriableError;
        // convert known error codes sent by the host driver to operation status
        return ConvertErrnoToStatus(errcode.err, m_isLocalSocket);
    }

    return OperationStatus(true);
}

/**
* get publish_event capability for driver using the
* NL80211_CMD_GET_WIPHY command
*/
OperationStatus NlService::NlGetPublishEventCapability()
{
    //in case of local socket this is not needed
    if (m_isLocalSocket)
    {
        return OperationStatus(true);
    }

    // smart pointer with custom deleter calling nlmsg_free on every return path
    std::unique_ptr<struct nl_msg, decltype(&nlmsg_free)> msg(
        NlAllocateNlCmdMsg(NLM_F_DUMP, NL80211_CMD_GET_WIPHY),
        nlmsg_free);

    if (!msg)
    {
        return OperationStatus(false, "failed to allocate nl message for GET_WIPHY command");
    }

    if (nla_put_flag(msg.get(), NL80211_ATTR_SPLIT_WIPHY_DUMP) < 0)
    {
        return OperationStatus(false, "failed to set params for GET_WIPHY command");
    }

    OperationStatus os = NlCmdSendAndRecv(m_nl, msg.get(), WiphyInfoHandler, &m_hasWmiPub);
    if (!os)
    {
        os.AddPrefix("failed to send GET_WIPHY command");
        return os;
    }

    if(!m_hasWmiPub)
    {
        return OperationStatus(false, "nl events are not supported, new driver version required...");
    }

    return OperationStatus(true);
}

/*
 * Send command to the Driver
 * Notes:
 * Id represents driver command type (wil_nl_60g_cmd_type enumeration) which is a contract between the Driver and the command initiator.
 * When response is required, relevant command handler should be set in the state prior to the call
 */
OperationStatus NlService::NlSendDriverCommandInternal(struct nl_sock *sk, uint32_t id, uint32_t bufLen, const void* pBuffer, IDriverCmdHandler* pCmdHandler)
{
    if (!pBuffer) // shouldn't happen
    {
        return OperationStatus(false, "invalid arguments, state and payload buffer cannot be null");
    }

    // smart pointer with custom deleter calling nlmsg_free on every return path
    std::unique_ptr<struct nl_msg, decltype(&nlmsg_free)> msg(
        NlAllocateNlCmdMsg(0, NL80211_CMD_VENDOR),
        nlmsg_free);

    if (!msg)
    {
        return OperationStatus(false, "failed to allocate nl message for GET_WIPHY");
    }

    if (nla_put_u32(msg.get(), NL80211_ATTR_VENDOR_ID, OUI_QCA) < 0 ||
        nla_put_u32(msg.get(), NL80211_ATTR_VENDOR_SUBCMD, QCA_NL80211_VENDOR_SUBCMD_UNSPEC) < 0)
    {
        return OperationStatus(false, "unable to set parameters for QCA_NL80211_VENDOR_SUBCMD_UNSPEC");
    }

    struct nlattr *vendor_data = nla_nest_start(msg.get(), NL80211_ATTR_VENDOR_DATA);
    if (!vendor_data)
    {
        return OperationStatus(false, "failed to start a new level of nested attributes for NL80211_ATTR_VENDOR_DATA");
    }

    if (nla_put_u32(msg.get(), QCA_WLAN_VENDOR_ATTR_TYPE, id))
    {
        return OperationStatus(false, "failed to set QCA_WLAN_VENDOR_ATTR_TYPE attribute (command Id)");
    }

    if (nla_put(msg.get(), QCA_WLAN_VENDOR_ATTR_BUF, (int)bufLen, pBuffer) < 0)
    {
        return OperationStatus(false, " failed to set QCA_WLAN_VENDOR_ATTR_BUF attribute (command payload buffer)");
    }

    nla_nest_end(msg.get(), vendor_data); /* always returns zero */

    if (pCmdHandler) // response required, blocking send-receive
    {
        return NlCmdSendAndRecv(sk, msg.get(), CommandInfoHandler, pCmdHandler);
    }

    // otherwise, no response expected, non blocking send-receive
    return NlCmdSendAndRecvNonblock(sk, msg.get());
}

/*
 * Send command to the Driver - legacy call assuming that response (if any) is always 32 bits long
 * Notes:
 * Id represents driver command type (wil_nl_60g_cmd_type enumeration) which is a contract between the Driver and the command initiator.
 */
OperationStatus NlService::NlSendDriverCommand(uint32_t id, uint32_t bufLen, const void* pBuffer, uint32_t* pResponse)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_upCmdHandler.reset(pResponse ? new LegacyCmdHandler(*pResponse) : nullptr); // set handler only when response required
    return NlSendDriverCommandInternal(m_nl, id, bufLen, pBuffer, m_upCmdHandler.get());
}

OperationStatus NlService::NlGetDriverEvent(int cancelationFd, struct driver_event_report* pMessageBuf, IfindexState& ifiState)
{
    //mutex not needed because the nl socket is only used here.
    if (!pMessageBuf) // shouldn't happen
    {
        return OperationStatus(false, "invalid arguments, arguments cannot be null");
    }

    /* 'cancelationFd' is a file descriptor for one of the sockets from the cancellation sockets pair */
    /* sockets pair serves as a pipe - a value written to one of its sockets, is also written to the second one */
    struct pollfd fds[2];
    memset(fds, 0, sizeof(fds));
    if (m_isLocalSocket)
    {
        fds[0].fd = NlGetLocalSockFd(m_nlEvent);
    }
    else
    {
        fds[0].fd = nl_socket_get_fd(m_nlEvent);
    }
    fds[0].events |= POLLIN;
    fds[1].fd = cancelationFd;
    fds[1].events |= POLLIN;

    int res = poll(fds, 2, -1); // infinite timeout
    if (res < 0)
    {
        std::ostringstream oss;
        oss << "poll failure: " << strerror(errno);
        return OperationStatus(false, oss.str());
    }

    if (fds[0].revents & POLLIN)
    {
        /* event answer buffer to be filled */
        struct driverEventReportWithIfindex driverEventReportBuf;
        driverEventReportBuf.ifindex = m_ifindex;
        driverEventReportBuf.ifiState = &ifiState;
        driverEventReportBuf.evt = pMessageBuf; /* store report pointer to be used inside the callback*/

        if (nl_cb_set(m_cb, NL_CB_VALID, NL_CB_CUSTOM, NlEventHandler, &driverEventReportBuf) < 0)
        {
            return OperationStatus(false, "failed to set nl callback handler (nl_event_handler)");
        }

        res = nl_recvmsgs(m_nlEvent, m_cb);
        if (res < 0)
        {
            std::ostringstream oss;
            oss << "nl_recvmsgs failed, error: " << nl_geterror(res) << " (" << res << ")" << std::endl;
            return OperationStatus(false, oss.str());
        }
    }
    else
    {
        return OperationStatus(false, "failed to get driver event, no event received");
    }

    if (m_isLocalSocket)
    {
        ifiState = IfindexState::IFINDEX_STATE_DPDK;
    }

    if (m_ifindexAttrPrintWarning && ifiState == IfindexState::IFINDEX_STATE_MISSING)
    {
        LOG_WARNING << "Please upgrade to the driver that supports Ifindex attribute" << std::endl;
        m_ifindexAttrPrintWarning = false;
    }

    return OperationStatus(true);
}

void NlService::NlRelease()
{
    if (m_nl)
    {
        if (m_isLocalSocket)
        {
            int fd = NlGetLocalSockFd(m_nl);
            if (fd >= 0)
            {
                close(fd);
            }
        }
        nl_socket_free(m_nl);
        m_nl = nullptr;
    }
    if (m_nlEvent)
    {
        if (m_isLocalSocket)
        {
            int fd = NlGetLocalSockFd(m_nlEvent);
            if (fd >= 0)
            {
                close(fd);
            }
        }
        nl_socket_free(m_nlEvent);
        m_nlEvent = nullptr;
    }
    if (m_nlPmc)
    {
        if (m_isLocalSocket)
        {
            int fd = NlGetLocalSockFd(m_nlPmc);
            if (fd >= 0)
            {
                close(fd);
            }
        }
        nl_socket_free(m_nlPmc);
        m_nlPmc = nullptr;
    }
    if (m_cb)
    {
        nl_cb_put(m_cb);
        m_cb = nullptr;
    }
    m_nl80211Id = 0;
}

/* Get bitmask of the driver capabilities */
OperationStatus NlService::NlGetCapabilities(uint32_t& driverCapabilities)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // response required, allocate handler (released by the common command handler)
    m_upCmdHandler.reset(new DriverCapaCmdHandler(driverCapabilities));
    // payload contains only the generic command id
    uint32_t buf = static_cast<uint32_t>(wil_nl_60g_generic_cmd::NL_60G_GEN_GET_DRIVER_CAPA);

    return NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_GENERIC_COMMAND), sizeof(buf), &buf, m_upCmdHandler.get());
}

/* Request to Enable/Disable events publishing to the user space */
OperationStatus NlService::NlEnableDriverEventsTransport(bool enable)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // send buffer of 4 bytes with 1 to enable and zero to disable
    uint32_t buf = enable ? 1U : 0U;
    m_upCmdHandler.reset(); // no response expected
    struct nl_sock *sk;
    if (m_isLocalSocket)
    {
        sk = m_nlEvent;
    }
    else
    {
        sk = m_nl;
    }
    return NlSendDriverCommandInternal(sk, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_REGISTER), sizeof(buf), &buf, m_upCmdHandler.get());
}

/* FW Reset through generic driver command */
OperationStatus NlService::NlFwReset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    uint32_t buf = static_cast<uint32_t>(wil_nl_60g_generic_cmd::NL_60G_GEN_FW_RESET); // payload contains only the generic command id
    m_upCmdHandler.reset(); // no response expected
    return NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_GENERIC_COMMAND), sizeof(buf), &buf, m_upCmdHandler.get());
}

/* Get FW state through generic driver command */
OperationStatus NlService::NlGetFwState(wil_fw_state& fw_state)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // response required, allocate handler (released by the common command handler)
    m_upCmdHandler.reset(new LegacyCmdHandler(reinterpret_cast<uint32_t&>(fw_state))); // set handler only when response required

    // payload contains only the generic command id
    uint32_t buf = static_cast<uint32_t>(wil_nl_60g_generic_cmd::NL_60G_GEN_GET_FW_STATE);

    return NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_GENERIC_COMMAND), sizeof(buf), &buf, m_upCmdHandler.get());
}

/* Get FW capabilities through generic driver command */
OperationStatus NlService::NlGetFwCapa(std::vector<uint32_t>& fwCapa)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // response required, allocate handler (released by the common command handler)
    m_upCmdHandler.reset(new DriverFwCapaCmdHandler(fwCapa)); // set handler only when response required

    // payload contains only the generic command id
    uint32_t buf = static_cast<uint32_t>(wil_nl_60g_generic_cmd::NL_60G_GEN_GET_FW_CAPA);

    return NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_GENERIC_COMMAND), sizeof(buf), &buf, m_upCmdHandler.get());
}

// PMC commands - QCA_NL80211_VENDOR_SUBCMD_UNSPEC/NL_60G_CMD_PMC
// Remark: Not currently in use, for unification with dpdk_service
OperationStatus NlService::NlPmcAlloc(uint32_t numDesc, uint32_t payloadLenBytes)
{
    std::lock_guard<std::mutex> lock(m_pmcMutex);
    struct nl60g_pmc cmdPayload = {};
    cmdPayload.hdr.cmd_id = static_cast<uint32_t>(wil_nl_60g_pmc_cmd::NL_60G_GEN_PMC_ALLOC);
    cmdPayload.cmd.alloc.num_desc = numDesc;
    cmdPayload.cmd.alloc.payload_size = payloadLenBytes;

    m_upPmcCmdHandler.reset(); // no response expected
    return NlSendDriverCommandInternal(m_nlPmc, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_PMC), sizeof(cmdPayload), &cmdPayload, m_upPmcCmdHandler.get());
}

// Remark: Not currently in use, for unification with dpdk_service
OperationStatus NlService::NlPmcFree()
{
    std::lock_guard<std::mutex> lock(m_pmcMutex);
    uint32_t buf = static_cast<uint32_t>(wil_nl_60g_pmc_cmd::NL_60G_GEN_PMC_FREE);
    m_upPmcCmdHandler.reset(); // no response expected
    return NlSendDriverCommandInternal(m_nlPmc, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_PMC), sizeof(buf), &buf, m_upPmcCmdHandler.get());
}

OperationStatus NlService::NlPmcGetDataInternal(wil_nl_60g_pmc_cmd pmcCmdId,
    uint32_t& bufLenBytesInOut, uint8_t* pBuffer, uint32_t& minDataLengthBytes, bool& hasMoreData)
{
    // response required, allocate handler (released by the common command handler)
    const uint32_t bufLenBytesIn = bufLenBytesInOut; // store the 'in' value
    bufLenBytesInOut = 0U;
    m_upPmcCmdHandler.reset(new PmcGetDataCmdHandler(bufLenBytesIn, bufLenBytesInOut, pBuffer, minDataLengthBytes, hasMoreData));

    struct nl60g_pmc cmdPayload = {};
    cmdPayload.hdr.cmd_id = static_cast<uint32_t>(pmcCmdId);
    cmdPayload.cmd.get_data.bytes_num = bufLenBytesIn;

    return NlSendDriverCommandInternal(m_nlPmc, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_PMC), sizeof(cmdPayload), &cmdPayload, m_upPmcCmdHandler.get());
}

OperationStatus NlService::NlPmcGetData(uint32_t& bufLenBytesInOut, uint8_t* pBuffer, uint32_t& minDataLengthBytes, bool& hasMoreData)
{
    std::lock_guard<std::mutex> lock(m_pmcMutex);
    return NlPmcGetDataInternal(wil_nl_60g_pmc_cmd::NL_60G_GEN_PMC_GET_DATA,
        bufLenBytesInOut, pBuffer, minDataLengthBytes, hasMoreData);
}

// Remark: Not currently in use, for unification with dpdk_service
OperationStatus NlService::NlPmcGetDescData(uint32_t& bufLenBytesInOut, uint8_t* pBuffer, uint32_t& minDataLengthBytes, bool& hasMoreData)
{
    return NlPmcGetDataInternal(wil_nl_60g_pmc_cmd::NL_60G_GEN_PMC_GET_DESC_DATA,
        bufLenBytesInOut, pBuffer, minDataLengthBytes, hasMoreData);
}

/**
 * read a DWORD register from device memory
 * QCA_NL80211_VENDOR_SUBCMD_UNSPEC/DRIVER_CMD_MEMIO command
 */
OperationStatus NlService::NlMemRead(uint32_t address, uint32_t& value, bool& retriableError)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    struct nl_memio memio {};

    m_retriableError = false;

    // response required, allocate handler (released by the common command handler)
    m_upCmdHandler.reset(new DriverMemioCmdHandler(value));
    memio.op = wil_mmio_read | wil_mmio_addr_ahb;
    memio.addr = address;
    memio.val = 0;
    OperationStatus os = NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_MEMIO), sizeof(struct nl_memio), &memio, m_upCmdHandler.get());
    retriableError = m_retriableError;
    if (!os)
    {
        return os;
    }

    LOG_VERBOSE << "in UnixDPDKDriver::Read, " << AddressValue(address, value) << std::endl;

    return os;
}

/**
 * read a block from device memory, optionally save contents to file
 * QCA_NL80211_VENDOR_SUBCMD_UNSPEC/DRIVER_CMD_MEMIO_BLOCK command
 */
OperationStatus NlService::NlMemReadBlock(uint32_t address, uint32_t size, char *arrBlock)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    struct nl_memioBlock memio {};

    m_upCmdHandler.reset(new DriverMemioBlkCmdHandler(size, arrBlock));
    memio.op = wil_mmio_read | wil_mmio_addr_ahb;
    memio.addr = address;
    memio.size = size;
    return NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_MEMIO_BLOCK), sizeof(struct nl_memio), &memio, m_upCmdHandler.get());
}

/**
 * write a DWORD register in device memory
 * QCA_NL80211_VENDOR_SUBCMD_UNSPEC/DRIVER_CMD_MEMIO command
 */
OperationStatus NlService::NlMemWrite(uint32_t addr, uint32_t val)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_upCmdHandler.reset(); // no response expected

    struct nl_memio memio {};
    memio.op = wil_mmio_write | wil_mmio_addr_ahb;
    memio.addr = addr;
    memio.val = val;

    return NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_MEMIO), sizeof(struct nl_memio), &memio, m_upCmdHandler.get());
}

/**
 * write a block to device memory. Contents to write are read from file
 * QCA_NL80211_VENDOR_SUBCMD_UNSPEC/DRIVER_CMD_MEMIO_BLOCK command
 */
OperationStatus NlService::NlMemWriteBlock(uint32_t addr, uint32_t size, const char *valuesToWrite)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_upCmdHandler.reset(); // no response expected

    struct nl_memioBlock *memio {};
    memio = (nl_memioBlock*)malloc(sizeof(struct nl_memioBlock) + size); // vector u8
    if (!memio)
    {
        return OperationStatus(false, "failed to allocate memory for reading block");
    }

    memio->op = wil_mmio_write | wil_mmio_addr_ahb;
    memio->addr = addr;
    memio->size = size;

    memcpy(memio->buf, valuesToWrite, size);

    OperationStatus os = NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_MEMIO_BLOCK), sizeof(struct nl_memioBlock) + size, memio, m_upCmdHandler.get());
    free(memio);
    return os;
}

OperationStatus NlService::NlGetStaInfo(std::vector<struct nl_60g_sta_info_entry>& staInfo)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_upCmdHandler.reset(new DriverGetStaInfoCmdHandler(staInfo));
    uint32_t buf = static_cast<uint32_t>(wil_nl_60g_generic_cmd::NL_60G_GEN_GET_STA_INFO);

    return NlSendDriverCommandInternal(m_nl, static_cast<uint32_t>(wil_nl_60g_cmd_type::DRIVER_CMD_GENERIC_COMMAND), sizeof(buf), &buf, m_upCmdHandler.get());
}

NlService::NlService(const char* interfaceName, bool localSocket) : m_ifindexAttrPrintWarning(true)
{
    if (!(interfaceName))
    {
        m_lastStatus = OperationStatus(false, "invalid arguments, interface name and nl_state pointer cannot be null");
        return;
    }

    m_isLocalSocket = localSocket;

    if (!m_isLocalSocket)
    {
        int ifindex = if_nametoindex(interfaceName);
        if (ifindex == 0)
        {
            std::ostringstream errorBuilder;
            errorBuilder << "unknown WIGIG interface " << interfaceName;
            m_lastStatus = OperationStatus(false, errorBuilder.str());
            return;
        }
        m_ifindex = ifindex;
    }
    else
    {
        m_ifindex = -1;
    }

    /* initialize structures for NL communication */
    m_lastStatus = NlInitNlGlobals(interfaceName);
    if (!m_lastStatus)
    {
        NlRelease(); /*it is the caller responsibility */
        return;
    }

    m_lastStatus = NlGetPublishEventCapability();
    if (!m_lastStatus)
    {
        NlRelease();
        return;
    }

    m_lastStatus = OperationStatus(true);
}

NlService::~NlService()
{
    NlRelease();
}
