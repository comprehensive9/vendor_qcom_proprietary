/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "dppManager.h"

#include <memory>

int main(int argc, char* argv[]) {
    std::shared_ptr<DppConfig>  dpp_config_p;
    std::unique_ptr<DPPManager> dm_p;
    int ret;
    std::string cmd;

    dpp_config_p = std::make_shared<DppConfig> ();
    if (dpp_config_p == nullptr) {
        dpp_daemon_print(nullptr, DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to allocate DppConfig", __func__);
        goto fail;
    }

    dm_p = std::make_unique<DPPManager>(dpp_config_p);
    if (dm_p == nullptr) {
        dpp_daemon_print(nullptr, DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to allocate DPPManager", __func__);
        goto fail;
    }

    cmd = argv[0];
    for (int idx = 1; idx < argc; idx++) {
        cmd += " " + std::string(argv[idx]);
    }

    ret = dm_p->ProcessCommand(cmd);
    if (ret < 0) {
        dpp_daemon_print(dpp_config_p.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to start DppDaemon with config:"
                         " %s", __func__, cmd.c_str());
        goto fail;
    }
    return DPP_STATUS_SUCCESS;
fail:
    return DPP_STATUS_FAIL;
}


