/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WIFI_DPP_COMMON_H
#define __WIFI_DPP_COMMON_H

#include <cstdarg>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <sys/time.h>

enum dppdaemon_print_level {
    DPPDAEMON_MSG_DEBUG,
    DPPDAEMON_MSG_INFO,
    DPPDAEMON_MSG_ERROR,
};

enum NetRole {
    INVALID_NETROLE = 0,
    STA             = 1,
    AP              = 2,
    MAX_NROLE       = AP
};

enum ProvRole {
    INVALID_PROVROLE = 0,
    ENROLLEE         = 1,
    CONFIGURATOR     = 2,
    BOTH             = 3,
    MAX_PROLE        = BOTH
};

enum AuthRole {
    INVALID_AUTHROLE = 0,
    INITIATOR        = 1,
    RESPONDER        = 2,
    MAX_AROLE        = RESPONDER
};

enum DPP_STATUS {
    DPP_STATUS_FAIL = -1,
    DPP_STATUS_SUCCESS = 0
};

const std::unordered_map<int, std::string> net_role_map = {
    {NetRole::STA, "station"},
    {NetRole::AP,  "AP"},
};

const std::unordered_map<int, std::string> prov_role_map = {
    {ProvRole::ENROLLEE,     "Enrollee"},
    {ProvRole::CONFIGURATOR, "Configurator"},
    {ProvRole::BOTH,         "Both"},
};

const std::unordered_map<int, std::string> auth_role_map = {
    {AuthRole::INITIATOR, "Initiator"},
    {AuthRole::RESPONDER, "Responder"},
};

typedef struct dpp_config {
    NetRole netrole;
    AuthRole auth_role;
    ProvRole prov_role;
    std::string mac_address;
    std::string iface_name;
    std::string bootstrap_uri;
    int channel;
    int op_class;
    int channel_freq;
    std::string dpp_key;
    std::string public_key;
    std::string xml_file;
    std::string uri;
    std::string interface;
    std::string client_path;
    unsigned int dpp_timeout;
/* DppDaemon logging parameters */
    int dppdaemon_print_level = DPPDAEMON_MSG_INFO;
    std::string log_file_dir;
    FILE* log_file_handle;
} DppConfig;

/* Common safe str buffer copy api */
size_t strbufcpy(char *dst, const char *src, size_t bufsize);

/* Validate system paths */
bool IsValidPath(const std::string path, bool is_file = false);

/* DPP print */
void dpp_daemon_print(const DppConfig *dpp_config, int level, const char *fmt, ...);

#endif /* __WIFI_DPP_COMMON__ */
