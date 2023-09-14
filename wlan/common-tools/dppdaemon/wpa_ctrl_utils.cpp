/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * wpa_supplicant/hostapd control interface library
 * Copyright (c) 2004-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed, used, and modified under the terms of
 * BSD license:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name(s) of the above-listed copyright holder(s) nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "dppWpaCtrlIface.h"

#ifdef ANDROID
#include <dirent.h>
#include <cutils/sockets.h>
#include <grp.h>
#include <pwd.h>
#endif /* ANDROID */

#include <cstring>
#include <errno.h>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#ifndef CONFIG_CTRL_IFACE_CLIENT_DIR
#ifdef ANDROID
#define CONFIG_CTRL_IFACE_CLIENT_DIR "/data/vendor/wifi/wpa/sockets"
#else
#define CONFIG_CTRL_IFACE_CLIENT_DIR "/tmp"
#endif
#endif /* CONFIG_CTRL_IFACE_CLIENT_DIR */
#ifndef CONFIG_CTRL_IFACE_CLIENT_PREFIX
#define CONFIG_CTRL_IFACE_CLIENT_PREFIX "wpa_ctrl_"
#endif /* CONFIG_CTRL_IFACE_CLIENT_PREFIX */


inline static std::string GetEventString(const std::string& resp,
                                         const int start_pos) {
    auto pos2 = resp.find(' ');
    if (pos2 == std::string::npos) {
        pos2 = resp.find('\n');
    }

    return resp.substr(start_pos, pos2 - start_pos);
}

static std::shared_ptr<wpa_ctrl> wpa_ctrl_open2(const DppConfig* dpp_config_p,
                                                const std::string& ctrl_path,
                                                const std::string& cli_path) {
    std::shared_ptr<wpa_ctrl> ctrl;
    static int counter = 0;
    int ret;
    size_t copy_sz;
    int tries = 0;
#ifdef ANDROID
    struct passwd *pw;
    struct group *gr;
#endif /* ANDROID */

    if (ctrl_path.empty()) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] ctrl_path is empty !!", __func__);
        return nullptr;
    }

    ctrl = std::make_shared<wpa_ctrl> ();
    if (ctrl == nullptr)
        return nullptr;

    ctrl->s = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (ctrl->s < 0) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] %s!!", __func__, strerror(errno));
        return nullptr;
    }

    ctrl->local.sun_family = AF_UNIX;
    counter++;

try_again:
    if (!cli_path.empty() && cli_path[0] == '/') {
        ret = snprintf(ctrl->local.sun_path,
                  sizeof(ctrl->local.sun_path),
                  "%s/" CONFIG_CTRL_IFACE_CLIENT_PREFIX "%d-%d",
                  cli_path.c_str(), (int) getpid(), counter);
    } else {
        ret = snprintf(ctrl->local.sun_path,
                  sizeof(ctrl->local.sun_path),
                  CONFIG_CTRL_IFACE_CLIENT_DIR "/"
                  CONFIG_CTRL_IFACE_CLIENT_PREFIX "%d-%d",
                  (int) getpid(), counter);
    }

    if (ret < 0 || (size_t) ret >= sizeof(ctrl->local.sun_path)) {
        close(ctrl->s);
        return nullptr;
    }
    tries++;
    if (bind(ctrl->s, (struct sockaddr *) &ctrl->local,
            sizeof(ctrl->local)) < 0) {
        if (errno == EADDRINUSE && tries < 2) {
            /*
             * getpid() returns unique identifier for this instance
             * of wpa_ctrl, so the existing socket file must have
             * been left by unclean termination of an earlier run.
             * Remove the file and try again.
             */
            unlink(ctrl->local.sun_path);
            goto try_again;
        }
        close(ctrl->s);
        return nullptr;
    }

#ifdef ANDROID
    chmod(ctrl->local.sun_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    pw = getpwnam("system");
    gr = getgrnam("wifi");
    if (pw && gr)
        chown(ctrl->local.sun_path, pw->pw_uid, gr->gr_gid);

    /*
     * If the ctrl_path isn't an absolute pathname, assume that
     * it's the name of a socket in the Android reserved namespace.
     * Otherwise, it's a normal UNIX domain socket appearing in the
     * filesystem.
     */
    if (ctrl_path[0] != '/') {
        std::vector<char> buf(21);
        snprintf(&buf[0], buf.size(), "wpa_%s", ctrl_path.c_str());
        if (socket_local_client_connect(
                ctrl->s, &buf[0],
                ANDROID_SOCKET_NAMESPACE_RESERVED,
                SOCK_DGRAM) < 0) {
            close(ctrl->s);
            unlink(ctrl->local.sun_path);
            return nullptr;
        }
        return ctrl;
    }
#endif /* ANDROID */

    ctrl->dest.sun_family = AF_UNIX;
    copy_sz = strbufcpy(ctrl->dest.sun_path, ctrl_path.c_str(),
                        sizeof(ctrl->dest.sun_path));
    if (copy_sz < ctrl_path.size()) {
        /* check if the whole string got copied or not */
        close(ctrl->s);
        return nullptr;
    }
    if (connect(ctrl->s, (struct sockaddr *) &ctrl->dest,
            sizeof(ctrl->dest)) < 0) {
        close(ctrl->s);
        unlink(ctrl->local.sun_path);
        return nullptr;
    }

    return ctrl;
}

static int wpa_ctrl_request(std::shared_ptr<wpa_ctrl> ctrl, const char *cmd,
                            size_t cmd_len, char *reply, size_t *reply_len,
                            void (*msg_cb)(char *msg, size_t len)) {
    struct timeval tv;
    int res;
    fd_set rfds;
    const char *_cmd = cmd;
    size_t _cmd_len = cmd_len;

    if (send(ctrl->s, _cmd, _cmd_len, 0) < 0) {
        return -1;
    }


    for (;;) {
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_SET(ctrl->s, &rfds);
        res = select(ctrl->s + 1, &rfds, NULL, NULL, &tv);
        if (res < 0)
            return res;
        if (FD_ISSET(ctrl->s, &rfds)) {
            res = recv(ctrl->s, reply, *reply_len, 0);
            if (res < 0)
                return res;
            if (res > 0 && reply[0] == '<') {
                /* This is an unsolicited message from
                 * wpa_supplicant, not the reply to the
                 * request. Use msg_cb to report this to the
                 * caller. */
                if (msg_cb) {
                    /* Make sure the message is nul
                     * terminated. */
                    if ((size_t) res == *reply_len)
                        res = (*reply_len) - 1;
                    reply[res] = '\0';
                    msg_cb(reply, res);
                }
                continue;
            }
            *reply_len = res;
            break;
        } else {
            return -2;
        }
    }
    return 0;
}

static void wpa_ctrl_close(std::shared_ptr<wpa_ctrl> ctrl) {
    if (ctrl == nullptr)
        return;
    unlink(ctrl->local.sun_path);
    if (ctrl->s >= 0)
        close(ctrl->s);
}


static int wpa_ctrl_command(const DppConfig* dpp_config_p,
                            const std::string& path, const std::string& ifname,
                            const std::string& cmd, std::string& resp) {
    std::vector<char> buf(2000);
    size_t len;

    std::string ctrl_path = path + ifname;
    auto ctrl = wpa_ctrl_open2(dpp_config_p, ctrl_path, path);
    if (ctrl == nullptr) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] wpa_ctrl_open2(%s) failed",
                         __func__, ctrl_path.c_str());
        return -1;
    }
    len = buf.size();
    if (wpa_ctrl_request(ctrl, cmd.c_str(), cmd.length(),
                         &buf[0], &len, nullptr) < 0) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] wpa_ctrl_request failed",
                         __func__);
        wpa_ctrl_close(ctrl);
        return -1;
    } else {
        buf[len] = '\0';
        resp = std::string(&buf[0]);
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_INFO,
                         "[%s] wpa_ctrl_request response: %s",
                         __func__, resp.c_str());
    }
    wpa_ctrl_close(ctrl);
    if (resp.find("FAIL", 4) != std::string::npos) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] Command failed (FAIL received)",
                         __func__);
        return -1;
    }
    return 0;
}

static int wpa_ctrl_recv(struct wpa_ctrl *ctrl, char *reply,
                         size_t *reply_len) {
    int res;
    res = recv(ctrl->s, reply, *reply_len, 0);
    if (res < 0)
        return res;
    *reply_len = res;
    return 0;
}


int DPPWpaCtrlIface::ListenToWpaEvents(struct wpa_ctrl *mon,
                                       const std::vector<std::string>& events,
                                       std::vector<char>& buf, size_t buf_size,
                                       const bool keep_listen) {
    int fd, ret;
    fd_set rfd;
    struct timeval tv;
    time_t start, now;

    fd = mon->s;
    if (fd < 0)
        return -1;

    time(&start);
    for (;;) {
        size_t len;

        FD_ZERO(&rfd);
        FD_SET(fd, &rfd);

        time(&now);
        if ((unsigned int) (now - start) >= dpp_config_p_->dpp_timeout)
            tv.tv_sec = 1;
        else
            tv.tv_sec = dpp_config_p_->dpp_timeout -
                (unsigned int) (now - start) + 1;
        tv.tv_usec = 0;
        ret = select(fd + 1, &rfd, NULL, NULL, &tv);
        if (ret == 0 && !keep_listen) {
            dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                             "[%s] Timeout on waiting for events", __func__);
            return -1;
        }
        if (ret < 0) {
            dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                             "[%s] select: %s", __func__, strerror(errno));
            return -1;
        }
        len = buf_size;
        if (wpa_ctrl_recv(mon, &buf[0], &len) < 0) {
            dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                             "[%s] Failure while waiting for events",
                             __func__);
            return -1;
        }
        if (len == buf_size)
            len--;
        buf[len] = '\0';

        std::string event_rcvd(&buf[0]);
        auto pos = event_rcvd.find('>');
        if (pos != std::string::npos) {
            auto event = GetEventString(event_rcvd, pos + 1);
            for (const auto& expected_event : events) {
                if (event == expected_event) {
                    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_INFO,
                                     "[%s] Event Found %s",
                                     __func__, event.c_str());
                    return 0; /* Event found */
                }
           }
        }

        time(&now);
        if ((unsigned int) (now - start) > 300) {
            dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                             "[%s:%d] Timeout on waiting for event",
                             __func__, __LINE__);
            return -1;
        }
    }
}

int DPPWpaCtrlIface::WaitForWpaEvent(struct wpa_ctrl *mon,
                                     const std::string& event) {
    std::vector<char> buf(4096);
    for (;;) {
          if (ListenToWpaEvents(mon, {event}, buf, buf.size()) < 0) {
            return -1;
          }
          break;
    }
    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_INFO,
                     "[%s] : %s", __func__, &buf[0]);
    return 0;
}

int DPPWpaCtrlIface::DppWaitTxStatus(struct wpa_ctrl *ctrl, int frame_type) {
    std::vector<char> buf(200);
    std::vector<char> tmp(20);
    int res;

    snprintf(&tmp[0], tmp.size(), "type=%d", frame_type);
    for (;;) {
        res = ListenToWpaEvents(ctrl, {"DPP-TX"}, buf, buf.size());
        if (res < 0)
            return -1;
        if (strstr(&buf[0], &tmp[0]) != NULL)
            break;
    }

    res = ListenToWpaEvents(ctrl, {"DPP-TX-STATUS"}, buf, buf.size());

    if (res < 0 || strstr(&buf[0], "result=FAILED") != NULL)
        return -1;

    return 0;
}


int DPPWpaCtrlIface::WpaCommand(const std::string& cmd,
                                std::string& resp) {
    const auto ifname = dpp_config_p_->interface;
    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_INFO,
                     "[%s] ifname='%s', cmd='%s'",
                     __func__, ifname.c_str(), cmd.c_str());
    return wpa_ctrl_command(dpp_config_p_.get(), dpp_config_p_->client_path, ifname, cmd, resp);
}

static int wpa_ctrl_attach_helper(std::shared_ptr<wpa_ctrl> ctrl, int attach) {
    std::vector<char> buf(10);
    int ret;
    size_t len = 10;

    ret = wpa_ctrl_request(ctrl, attach ? "ATTACH" : "DETACH", 6,
                   &buf[0], &len, NULL);
    if (ret < 0) {
        return ret;
    }
    if (len == 3 && std::memcmp(&buf[0], "OK\n", 3) == 0) {
        return 0;
    }
    return -1;
}

static int wpa_ctrl_attach(std::shared_ptr<wpa_ctrl> ctrl) {
    return wpa_ctrl_attach_helper(ctrl, 1);
}

static int wpa_ctrl_detach(std::shared_ptr<wpa_ctrl> ctrl) {
    return wpa_ctrl_attach_helper(ctrl, 0);
}


static std::shared_ptr<wpa_ctrl> open_wpa_ctrl_mon(const DppConfig* dpp_config_p,
                                                   const std::string& ctrl_path,
                                                   const std::string& ifname) {
    std::string path = ctrl_path + ifname;
    auto ctrl = wpa_ctrl_open2(dpp_config_p, path, ctrl_path);
    if (ctrl == nullptr) {
        return nullptr;
    }
    if (wpa_ctrl_attach(ctrl) < 0) {
        wpa_ctrl_close(ctrl);
        return nullptr;
    }
    return ctrl;
}

std::shared_ptr<wpa_ctrl> DPPWpaCtrlIface::open_wpa_mon() {
    return open_wpa_ctrl_mon(dpp_config_p_.get(), dpp_config_p_->client_path,
                             dpp_config_p_->interface);
}

void DPPWpaCtrlIface::close_wpa_mon() {
    if (wpa_ctrl_mon_p_ == nullptr)
        return;

    if (wpa_ctrl_detach(wpa_ctrl_mon_p_) < 0) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to detach", __func__);
    }

    wpa_ctrl_close(wpa_ctrl_mon_p_);
}
