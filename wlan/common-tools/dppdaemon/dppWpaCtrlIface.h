/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WIFI_DPP_CTRL_IFACE_H
#define __WIFI_DPP_CTRL_IFACE_H

#include "dppCommon.h"

#include <memory>
#include <vector>
#include <sys/un.h>

struct wpa_ctrl {
    int s;
    struct sockaddr_un local;
    struct sockaddr_un dest;
};

class DPPWpaCtrlIface
{
    public:
        DPPWpaCtrlIface(std::shared_ptr<DppConfig> config_p);

        int DppListen();

        int DppStopListen();

        int WpaCommand(const std::string& cmd, std::string& resp);

    private:
        int ListenToWpaEvents(struct wpa_ctrl *mon,
                              const std::vector<std::string>& events,
                              std::vector<char>& buf, size_t buf_size,
                              const bool keep_listen = false);

        int WaitForWpaEvent(struct wpa_ctrl *mon,
                            const std::string& event);

        int DppWaitTxStatus(struct wpa_ctrl *ctrl, int frame_type);

        std::shared_ptr<wpa_ctrl> open_wpa_mon();

        void close_wpa_mon();

        /* data members */
        std::shared_ptr<wpa_ctrl> wpa_ctrl_mon_p_;

        std::shared_ptr<DppConfig> dpp_config_p_;
};
#endif /* __WIFI_DPP_CTRL_IFACE_H */
