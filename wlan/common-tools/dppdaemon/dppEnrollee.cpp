/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "dppEnrollee.h"

#include <regex>

static std::string GetMacAddrFromStatusStr(const std::string& status_str) {
    std::string mac;
    int req_len = std::strlen("address=");
    const std::regex ws_re("\\n");
    const auto tokens = std::vector<std::string>(
        std::sregex_token_iterator(status_str.begin(), status_str.end(), ws_re,
                                   -1),
        std::sregex_token_iterator());
    for (const auto& token : tokens) {
        if (token.substr(0, req_len) == "address=") {
            mac = token.substr(req_len, token.length() - req_len);
        }
    }
    return mac;
}

static std::string GetPKFromURI(const DppConfig* dpp_config_p) {
    std::string pk;
    const std::string uri = dpp_config_p->uri;
    if (uri.empty()) {
        return "";
    }
    auto pk_start_pos = uri.find("K:");
    if (pk_start_pos == std::string::npos) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid uri, missing public key",
                         __func__);
        return "";
    }
    auto pk_end_pos = uri.find(";", pk_start_pos);
    if (pk_end_pos == std::string::npos) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid uri, missing public key",
                         __func__);
        return "";
    }

    pk_start_pos += 2; /* skips "K:" */
    return uri.substr(pk_start_pos, pk_end_pos - pk_start_pos);
}

static int ValidateDppConfig(const DppConfig* dpp_config_p) {
    if (dpp_config_p->op_class == -1 || dpp_config_p->channel_freq == -1 ||
        dpp_config_p->mac_address.empty()) {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] Failed Validation of dpp params from xml file",
                         __func__);
        return -1;
    }

    return 0;
}


int DPPEnrollee::startDppListen() {
    /* Check XML Validation */
    if (!xml_util_p_->IsXmlValid()) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] XML validation failed, "
                         "Please fix the xml as per it's format guidelines",
                         __func__);
        return -1;
    }
    int ret = 0;
    int uri_idx;
    std::string resp;

    dpp_config_p_->channel      = xml_util_p_->getChannel();
    dpp_config_p_->op_class     = xml_util_p_->getOpClass();
    dpp_config_p_->dpp_key      = xml_util_p_->getDppKey();
    dpp_config_p_->mac_address  = xml_util_p_->getMacAddress();
    dpp_config_p_->channel_freq =
        xml_util_p_->getChannelFreq(dpp_config_p_->channel,
                                    dpp_config_p_->op_class);

    /* Get Mac address from wpa_supplicant status string */
    ret = ctrl_iface_p_->WpaCommand("STATUS", resp);
    if (ret) {
        return ret;
    }

    /* mac address not present in xml yet */
    std::string status_mac = GetMacAddrFromStatusStr(resp);
    if (dpp_config_p_->mac_address != status_mac) {
        if (!dpp_config_p_->mac_address.empty()) { /* wrong mac present in xml */
            xml_util_p_->RemoveTagFromXml("mac-address");
        }
        xml_util_p_->AddTagToXml("mac-address", status_mac);
        dpp_config_p_->mac_address  = xml_util_p_->getMacAddress();
    }

    if (ValidateDppConfig(dpp_config_p_.get()) == -1) {
        /* Validation of dpp config failed */
        return -1;
    }

    /* Generate Bootstrap info */
    uri_idx = generateBootstrap();
    if (uri_idx == -1) {
       return -1;
    } else if (getURIandPk(uri_idx) == -1) {
        return -1;
    }

    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_DEBUG,
                     "[%s] \nch_freq : %d\nmac_addr : %s\ndpp-key : %s\n"
                     "public-key : %s\nuri : %s",
                     __func__, dpp_config_p_->channel_freq,
                     dpp_config_p_->mac_address.c_str(),
                     dpp_config_p_->dpp_key.c_str(),
                     dpp_config_p_->public_key.c_str(),
                     dpp_config_p_->uri.c_str());

    ret = ctrl_iface_p_->DppListen();

    if (!ret) {
        ret = removeBootstrap(uri_idx);
    }

    ret = ctrl_iface_p_->DppStopListen();

    return ret;
}

int DPPEnrollee::generateBootstrap() {
    const int configurator_idx = addConfigurator();
    if (configurator_idx == -1) {
        return -1;
    }
    if (dpp_config_p_->dpp_key.empty()) { /* Key is not present in xml */
        if (getConfiguratorkey(dpp_config_p_->dpp_key,
                               configurator_idx) == -1) {
            return -1;
        }
    }
    const std::string bootstrap_cmd = "DPP_BOOTSTRAP_GEN type=qrcode chan=" +
                                      std::to_string(dpp_config_p_->op_class) +
                                      '/' +
                                      std::to_string(dpp_config_p_->channel) +
                                      " mac=" + dpp_config_p_->mac_address +
                                      " key=" + dpp_config_p_->dpp_key;
    std::string resp;
    if (ctrl_iface_p_->WpaCommand(bootstrap_cmd, resp) == -1) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to generate bootstrap", __func__);
        return -1;
    }

    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_DEBUG,
                     "[%s] bootstrap resp = %s", __func__, resp.c_str());

    return std::atoi(resp.c_str());
}

int DPPEnrollee::addConfigurator() {
    std::string resp;
    int configurator_idx;
    const std::string configurator_add_cmd = "DPP_CONFIGURATOR_ADD";
    if (ctrl_iface_p_->WpaCommand(configurator_add_cmd, resp) == -1) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to add configurator", __func__);
        return -1;
    }
    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_DEBUG,
                     "[%s] add configurator resp = %s",
                     __func__, resp.c_str());
    configurator_idx = std::stoi(resp);

    return configurator_idx;
}

int DPPEnrollee::getConfiguratorkey(std::string& key, const int configurator_idx) {
    std::string resp;
    const std::string get_key_cmd = "DPP_CONFIGURATOR_GET_KEY " +
                                    std::to_string(configurator_idx);
    if (ctrl_iface_p_->WpaCommand(get_key_cmd, resp) == -1) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to get configurator key for index %d",
                         __func__, configurator_idx);
        return -1;
    }
    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_DEBUG,
                     "[%s] get configurator key resp = %s",
                     __func__, resp.c_str());
    key = resp;
    xml_util_p_->AddTagToXml("dpp-key", key); /* Write key to xml file */

    return 0;
}

int DPPEnrollee::getURIandPk(unsigned int index) {
    std::string resp;
    std::string xml_uri       = xml_util_p_->getURI();
    std::string xml_pk        = xml_util_p_->getPK();
    const std::string uri_cmd = "DPP_BOOTSTRAP_GET_URI " +
                                std::to_string(index);
    if (ctrl_iface_p_->WpaCommand(uri_cmd, resp) == -1) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Failed to get uri for index %d",
                         __func__, index);
        return -1;
    }

    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_DEBUG,
                     "[%s] get uri resp = %s", __func__, resp.c_str());
    if (index < dpp_uris_list.size()) {
        /* There is a mismatch between our uris records and
         * supplicant ones */
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] There is a mismatch between "
                         "Dpp Manager URI records and supplicant records",
                         __func__);
    }

    dpp_uris_list.resize(index); /* Avoids any mismatch */
    dpp_uris_list[index - 1] = resp;
    dpp_config_p_->uri = resp;
    dpp_config_p_->public_key = GetPKFromURI(dpp_config_p_.get());

    if (xml_uri.empty() && xml_pk.empty()) {
        xml_util_p_->AddTagToXml("public-key", dpp_config_p_->public_key);
        xml_util_p_->AddTagToXml("uri", dpp_config_p_->uri);
        return 0;
    }

    /* Match already existing URI with new generated one */
    if (xml_uri != dpp_config_p_->uri || xml_pk != dpp_config_p_->public_key) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] URI mismatch, generated uri is "
                         "different from one in xml", __func__);
        return -1;
    }

    return 0;
}


int DPPEnrollee::removeBootstrap(unsigned int index) {
    const std::string bootstrap_remove_cmd = "DPP_BOOTSTRAP_REMOVE " +
                                            std::to_string(index);
    std::string resp;

    if (index > dpp_uris_list.size()) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid uri index", __func__);
        return -1;
    }

    if (ctrl_iface_p_->WpaCommand(bootstrap_remove_cmd, resp) == -1) {
        return -1;
    }

    dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_DEBUG,
                     "[%s] uri remove resp = %s", __func__, resp.c_str());
    dpp_uris_list.erase(dpp_uris_list.begin() + index - 1);

    return 0;
}
