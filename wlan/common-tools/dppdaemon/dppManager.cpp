/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "dppManager.h"


#include <chrono>
#include <getopt.h>
#include <memory>
#include <regex>

constexpr auto LOGGER_PRETTY_TIME_FORMAT = "%Y-%m-%d_%H:%M:%S";

static struct option options[] = {
    {"interface", required_argument, nullptr, 'i'},
    {"client_path", required_argument, nullptr, 'c'},
    {"netrole", required_argument, nullptr, 'n'},
    {"prov_role", required_argument, nullptr, 'p'},
    {"auth_role", required_argument, nullptr, 'a'},
    {"xml", required_argument, nullptr, 'x'},
    {"log_dir", required_argument, nullptr, 'l'},
    {"help", no_argument, nullptr, 'h'},
    {0, 0, 0, 0}
};

static void usage() {
    printf("\n Usage Options-\n"
               " -i, --interface   "
               "interface on which wpa_supplicant is running\n"
               " -c, --client_path "
               "wpa_supplicant client path\n"
               " -n, --netrole     "
               "set the device type, can be either sta or ap\n"
               " -p, --prov_role   "
               "set the device provisioning role,"
               "can be either enrollee / configurator or both\n"
               " -a, --auth_role   "
               "set the device authentication role, can be either "
               "Initiator / Responder\n"
               " -x, --xml         "
               "set the xml file name along with it's path\n"
               " -l, --log_dir     "
               "set the log directory name (optional)\n"
               " -h, --help        display this help and exit\n");
}


DPPManager::~DPPManager() {
    if (dpp_config_p_->log_file_handle) {
        fclose(dpp_config_p_->log_file_handle);
    }
}

void DPPManager::InitialiseLogging() {
    char buffer[128] = {0};
    auto curr_time =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto time_info  = std::localtime(&curr_time);
    if (strftime(buffer, sizeof(buffer),
                 LOGGER_PRETTY_TIME_FORMAT, time_info) == 0) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to fill the buffer with time info",
                         __func__);
        return;
    }

    if (!dpp_config_p_->log_file_dir.empty()) {
        const std::string filepath =
            dpp_config_p_->log_file_dir + "dppdaemon_" + std::string(buffer);
        dpp_config_p_->log_file_handle = fopen(filepath.c_str(), "a");
        if (dpp_config_p_->log_file_handle == NULL) {
            dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                             "[%s] Failed to open log file", __func__);
            return;
        }
        dpp_config_p_->dppdaemon_print_level = DPPDAEMON_MSG_DEBUG;
    }
}

int DPPManager::StartEnrolleeAsResponder() const {
    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_INFO,
                     "[%s] Starting DPP Enrollee sta in listening mode....",
                     __func__);
    return enrollee_p_->startDppListen();
}

void DPPManager::SetInterface(const std::string& ifname) {
    dpp_config_p_->interface = ifname;
}

void DPPManager::SetClientPath(std::string& path) {
    if (!IsValidPath(path)) {
        return;
    }
    if (path[path.length() - 1] != '/') {
        path += '/';
    }
    dpp_config_p_->client_path = path;
}

void DPPManager::SetNetRole(const std::string& netrole) {
    if (netrole == "sta") {
        dpp_config_p_->netrole = NetRole::STA;
    } else if (netrole == "ap") {
        dpp_config_p_->netrole = NetRole::AP;
    } else {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid net role", __func__);
        dpp_config_p_->netrole = NetRole::INVALID_NETROLE;
    }
}

void DPPManager::SetProvRole(const std::string& prov_role) {
    if (prov_role == "enrollee") {
        dpp_config_p_->prov_role = ProvRole::ENROLLEE;
    } else if (prov_role == "configurator") {
        dpp_config_p_->prov_role = ProvRole::CONFIGURATOR;
    } else {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid prov role", __func__);
        dpp_config_p_->prov_role=ProvRole::INVALID_PROVROLE;
    }
}

void DPPManager::SetAuthRole(const std::string& auth_role) {
    if (auth_role == "initiator") {
        dpp_config_p_->auth_role = AuthRole::INITIATOR;
    } else if (auth_role == "responder") {
        dpp_config_p_->auth_role = AuthRole::RESPONDER;
    } else {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid auth role", __func__);
        dpp_config_p_->auth_role=AuthRole::INVALID_AUTHROLE;
    }
}

void DPPManager::SetXmlPath(const std::string file) {
    if (!IsValidPath(file, true)) {
        return;
    }
    dpp_config_p_->xml_file = file;
}

void DPPManager::SetLogDirPath(std::string path) {
    /* Validate Log file directory */
    if (!IsValidPath(path)) {
        return;
    }
    if (path[path.length() - 1] != '/') {
        path += '/';
    }
    dpp_config_p_->log_file_dir = path;
}

int DPPManager::ProcessCommand(const std::string& cmd) {
    int c;
    std::string netrole;
    std::string prov_role;
    std::string auth_role;
    std::string xml_file;
    std::string log_dir;
    std::string interface;
    std::string client_path;
    bool flag = false;
    const std::regex ws_re("\\s+");

    const auto tokens = std::vector<std::string>(
            std::sregex_token_iterator(cmd.begin(), cmd.end(), ws_re, -1),
            std::sregex_token_iterator()
    );
    int argc = tokens.size();
    char* argv[argc];
    for (int idx = 0; idx < argc; idx++) {
        argv[idx] = new char[tokens[idx].size() + 1];
        strbufcpy(argv[idx], tokens[idx].c_str(),
                  sizeof(*argv[idx]) * (tokens[idx].size() + 1));
    }

    while (1) {
        c = getopt_long(argc, argv, "i:c:n:p:a:x:l:h",options,nullptr);

        /* Detect the end of the options. */
        if (c < 0||flag)
            break;
        switch (c) {
            case 'i':
                    interface = optarg;
                    SetInterface(interface);
                    break;
            case 'c':
                    client_path = optarg;
                    SetClientPath(client_path);
                    if (dpp_config_p_->client_path.empty()) {
                        dpp_daemon_print(dpp_config_p_.get(),
                                         DPPDAEMON_MSG_ERROR,
                                         "[%s] Invalid client path %s ",
                                         "either not exist or not have "
                                         "required permissions",
                                         __func__, client_path.c_str());
                        return -1;
                    }
                    break;
            case 'n':
                    netrole = optarg;
                    std::transform(netrole.begin(), netrole.end(),
                                   netrole.begin(), ::tolower);
                    SetNetRole(netrole);
                    break;
            case 'p':
                    prov_role = optarg;
                    std::transform(prov_role.begin(), prov_role.end(),
                                   prov_role.begin(), ::tolower);
                    SetProvRole(prov_role);
                    break;
            case 'a':
                    auth_role = optarg;
                    std::transform(auth_role.begin(), auth_role.end(),
                                   auth_role.begin(), ::tolower);
                    SetAuthRole(const_cast<char*>(auth_role.c_str()));
                    break;
            case 'x':
                    xml_file = optarg;
                    SetXmlPath(const_cast<char*>(xml_file.c_str()));
                    if (dpp_config_p_->xml_file.empty()) {
                        dpp_daemon_print(dpp_config_p_.get(),
                                         DPPDAEMON_MSG_ERROR,
                                         "[%s] Invalid xml file path %s ",
                                         "either not exist or not have "
                                         "required permissions",
                                         __func__, xml_file.c_str());
                        return -1;
                    }
                    break;
            case 'l':
                    log_dir = optarg;
                    SetLogDirPath(const_cast<char*>(log_dir.c_str()));
                    break;
            case 'h':
            default :
                    usage();
                    flag = true;
                    break;
        }
    }

    if (interface.empty() || client_path.empty() || netrole.empty() ||
        prov_role.empty() || auth_role.empty() || xml_file.empty()) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid command !!", __func__);
        usage();
        return -1;
    }

    if (GetNetRole() == NetRole::INVALID_NETROLE ||
        GetProvRole() == ProvRole::INVALID_PROVROLE ||
        GetAuthRole() == AuthRole::INVALID_AUTHROLE) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid role values", __func__);
        return -1;
    }

    if (GetNetRole() == NetRole::STA &&
        GetProvRole() == ProvRole::ENROLLEE &&
        GetAuthRole() == AuthRole::RESPONDER) {
        /* Initialise logging */
        InitialiseLogging();

        dpp_config_p_->dpp_timeout = 20;
        enrollee_p_ = std::make_unique<DPPEnrollee> (dpp_config_p_);
        if (StartEnrolleeAsResponder()) {
            dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                             "[%s] failed", __func__);
        }
    } else {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_INFO,
                         "[%s] This combination of roles "
                         "currently not supported...!!", __func__);
        return -1;
    }

    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_INFO,
                     "[%s] iface = %s cli_path = %s netrole = %s "
                     "prov_role = %s auth_role = %s xml_file = %s "
                     "log_dir = %s",
                     __func__, dpp_config_p_->interface.c_str(),
                     dpp_config_p_->client_path.c_str(),
                     net_role_map.at(dpp_config_p_->netrole).c_str(),
                     prov_role_map.at(dpp_config_p_->prov_role).c_str(),
                     auth_role_map.at(dpp_config_p_->auth_role).c_str(),
                     dpp_config_p_->xml_file.c_str(),
                     dpp_config_p_->log_file_dir.empty()
                         ? "Not specified"
                         : dpp_config_p_->log_file_dir.c_str());

    return 0;
}
