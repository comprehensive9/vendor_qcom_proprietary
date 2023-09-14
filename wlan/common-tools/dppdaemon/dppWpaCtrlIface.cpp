/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "dppWpaCtrlIface.h"
#include <memory>


const std::vector<std::string> auth_events = {
    "DPP-AUTH-SUCCESS",
    "DPP-NOT-COMPATIBLE",
    "DPP-RESPONSE-PENDING",
    "DPP-SCAN-PEER-QR-CODE",
    "DPP-AUTH-DIRECTION",
};

const std::vector<std::string> conf_events = {
    "DPP-CONF-RECEIVED",
    "DPP-CONF-SENT",
    "DPP-CONF-FAILED",
};

const std::vector<std::string> conn_events = {
    "PMKSA-CACHE-ADDED",
    "DPP-CONNECTOR",
    "CTRL-EVENT-CONNECTED",
};

DPPWpaCtrlIface::DPPWpaCtrlIface(std::shared_ptr<DppConfig> config_p)
      : dpp_config_p_(config_p) {
    wpa_ctrl_mon_p_ = open_wpa_mon();
}

int DPPWpaCtrlIface::DppListen() {
    if (wpa_ctrl_mon_p_ == nullptr) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] monitor interface is null, "
                         "Can't listen !!!!!", __func__);
        return -1;
    }

    std::string cmd_resp;
    /* This command enables auto associate to the AP once receiving
     * Configuration object
     **/
    const std::string dpp_config_proc_cmd = "SET dpp_config_processing 2";
    if (WpaCommand(dpp_config_proc_cmd, cmd_resp) < 0) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] %s failed !!!", __func__,
                         dpp_config_proc_cmd.c_str());
        return -1;
    }

    const std::string listen_cmd = "DPP_LISTEN " +
                                   std::to_string(dpp_config_p_->channel_freq);

    if (WpaCommand(listen_cmd, cmd_resp) < 0) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] %s failed !!!", __func__, listen_cmd.c_str());
        return -1;
    }

    std::vector<char> buf(2000);
    if (ListenToWpaEvents(wpa_ctrl_mon_p_.get(),
                          auth_events, buf, buf.size(), true) == -1) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to listen to auth wpa events",
                         __func__);
        close_wpa_mon();
        return -1;
    }

    if (ListenToWpaEvents(wpa_ctrl_mon_p_.get(),
                          conf_events, buf, buf.size(), true) == -1) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to listen to conf wpa events",
                         __func__);
        close_wpa_mon();
        return -1;
    }

    if (ListenToWpaEvents(wpa_ctrl_mon_p_.get(),
                          conn_events, buf, buf.size(), true) == -1) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to listen to connection wpa events",
                         __func__);
        close_wpa_mon();
        return -1;
    }

    return 0;
}

int DPPWpaCtrlIface::DppStopListen() {
    const std::string stop_listen_cmd = "DPP_STOP_LISTEN";
    std::string cmd_resp;
    if (WpaCommand(stop_listen_cmd, cmd_resp) < 0) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] %s failed !!!",
                         __func__, stop_listen_cmd.c_str());
        return -1;
    }
    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_INFO,
                     "[%s] dpp_listen stopped", __func__);
    close_wpa_mon();
    return 0;
}
