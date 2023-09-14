/* Copyright (c) 2020 Wi-Fi Alliance                                                */

/* Permission to use, copy, modify, and/or distribute this software for any         */
/* purpose with or without fee is hereby granted, provided that the above           */
/* copyright notice and this permission notice appear in all copies.                */

/* THE SOFTWARE IS PROVIDED 'AS IS' AND THE AUTHOR DISCLAIMS ALL                    */
/* WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                    */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL                     */
/* THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR                       */
/* CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING                        */
/* FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF                       */
/* CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT                       */
/* OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS                          */
/* SOFTWARE. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "indigo_api.h"
#include "vendor_specific.h"
#include "utils.h"
#include "wpa_ctrl.h"
#include "indigo_api_callback.h"
#ifdef _OPENWRT_QTI_
#include "vendor_specific_openwrt.h"
#endif


static char pac_file_path[S_BUFFER_LEN] = {0};
struct interface_info* band_transmitter[16];
#ifdef _OPENWRT_QTI_
extern char *indigo_radio_ifname[];
extern char *indigo_radio_ifname_ap[];
extern int num_radio;
int run_system(const char *cmd);
int generate_wireless_config(char *output, int output_size,
            struct packet_wrapper *wrapper,
            struct interface_info* wlanp, struct indigo_dut *dut);
#define BUZZ_SIZE 1024
#endif

void register_apis(void) {
    /* Basic */
    register_api(API_GET_IP_ADDR, NULL, get_ip_addr_handler);
    register_api(API_GET_MAC_ADDR, NULL, get_mac_addr_handler);
    register_api(API_GET_CONTROL_APP_VERSION, NULL, get_control_app_handler);
    register_api(API_START_LOOP_BACK_SERVER, NULL, start_loopback_server);
    register_api(API_STOP_LOOP_BACK_SERVER, NULL, stop_loop_back_server_handler);
    register_api(API_CREATE_NEW_INTERFACE_BRIDGE_NETWORK, NULL, create_bridge_network_handler);
    register_api(API_ASSIGN_STATIC_IP, NULL, assign_static_ip_handler);
    register_api(API_DEVICE_RESET, NULL, reset_device_handler);
    /* AP */
    register_api(API_AP_START_UP, NULL, start_ap_handler);
    register_api(API_AP_STOP, NULL, stop_ap_handler);
    register_api(API_AP_CONFIGURE, NULL, configure_ap_handler);
    register_api(API_AP_TRIGGER_CHANSWITCH, NULL, trigger_ap_channel_switch);
    register_api(API_AP_SEND_DISCONNECT, NULL, send_ap_disconnect_handler);
    register_api(API_AP_SET_PARAM , NULL, set_ap_parameter_handler);
    register_api(API_AP_SEND_BTM_REQ, NULL, send_ap_btm_handler);
    /* STA */
    register_api(API_STA_ASSOCIATE, NULL, associate_sta_handler);
    register_api(API_STA_CONFIGURE, NULL, configure_sta_handler);
    register_api(API_STA_DISCONNECT, NULL, stop_sta_handler);
    register_api(API_STA_SEND_DISCONNECT, NULL, send_sta_disconnect_handler);
    register_api(API_STA_REASSOCIATE, NULL, send_sta_reconnect_handler);
    register_api(API_STA_SET_PARAM, NULL, set_sta_parameter_handler);
    register_api(API_STA_SEND_BTM_QUERY, NULL, send_sta_btm_query_handler);
    register_api(API_STA_SEND_ANQP_QUERY, NULL, send_sta_anqp_query_handler);
    /* TODO: Add the handlers */
    register_api(API_STA_SET_CHANNEL_WIDTH, NULL, NULL);
    register_api(API_STA_POWER_SAVE, NULL, NULL);
}

static int get_control_app_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(TLV_VALUE_OK), TLV_VALUE_OK);
    fill_wrapper_tlv_bytes(resp, TLV_CONTROL_APP_VERSION, strlen(TLV_VALUE_APP_VERSION), TLV_VALUE_APP_VERSION);
    return 0;
}

static int reset_device_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_RESET_NOT_OK;
    char buffer[TLV_VALUE_SIZE];
    char role[TLV_VALUE_SIZE], log_level[TLV_VALUE_SIZE], band[TLV_VALUE_SIZE];
    struct tlv_hdr *tlv = NULL;

    /* TLV: ROLE */
    tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
    memset(role, 0, sizeof(role));
    if (tlv) {
        memcpy(role, tlv->value, tlv->len);
    } else {
        goto done;
    }
    /* TLV: DEBUG_LEVEL */
    tlv = find_wrapper_tlv_by_id(req, TLV_DEBUG_LEVEL);
    memset(log_level, 0, sizeof(log_level));
    if (tlv) {
        memcpy(log_level, tlv->value, tlv->len);
    }
    /* TLV: TLV_BAND */
    memset(band, 0, sizeof(band));
    tlv = find_wrapper_tlv_by_id(req, TLV_BAND);
    if (tlv) {
        memcpy(band, tlv->value, tlv->len);
    }

    if (atoi(role) == DUT_TYPE_STAUT) {
        /* stop the wpa_supplicant and release IP address */
        memset(buffer, 0, sizeof(buffer));
#ifdef _OPENWRT_QTI_
        run_system("wifi config > /etc/config/wireless");
        sleep(1);
        memset(&dut,0,sizeof(dut));
#else
        snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_wpas_exec_file());
        indigo_logger(LOG_LEVEL_DEBUG, "SYSTEM_CMD: %s", buffer);
        system(buffer);
        sleep(1);
        reset_interface_ip(get_wireless_interface());
        if (strlen(log_level)) {
            set_wpas_debug_level(get_debug_level(atoi(log_level)));
        }
#endif
    } else if (atoi(role) == DUT_TYPE_APUT) {
        /* stop the hostapd and release IP address */
        memset(buffer, 0, sizeof(buffer));
#ifdef _OPENWRT_QTI_
        run_system("wifi config > /etc/config/wireless");
        sleep(1);
        memset(&dut,0,sizeof(dut));
#else
        snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_hapd_exec_file());
        system(buffer);
        sleep(1);
        reset_interface_ip(get_wireless_interface());
        if (strlen(log_level)) {
            set_hostapd_debug_level(get_debug_level(atoi(log_level)));
        }
        reset_bridge(BRIDGE_WLANS);
#endif
        /* reset interfaces info */
        clear_interfaces_resource();
    }

    if (strcmp(band, TLV_BAND_24GHZ) == 0) {
        set_default_wireless_interface_info(BAND_24GHZ);
    } else if (strcmp(band, TLV_BAND_5GHZ) == 0) {
        set_default_wireless_interface_info(BAND_5GHZ);
    } else if (strcmp(band, TLV_BAND_6GHZ) == 0) {
        set_default_wireless_interface_info(BAND_6GHZ);
    }

    memset(band_transmitter, 0, sizeof(band_transmitter));

    vendor_device_reset();

    sleep(1);

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_RESET_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'AP stop completed : Hostapd service is inactive.'}
static int stop_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len = 0, reset = 0;
    char buffer[S_BUFFER_LEN], reset_type[16];
    char *parameter[] = {"pidof", get_hapd_exec_file(), NULL};
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;

    /* TLV: RESET_TYPE */
    tlv = find_wrapper_tlv_by_id(req, TLV_RESET_TYPE);
    memset(reset_type, 0, sizeof(reset_type));
    if (tlv) {
        memcpy(reset_type, tlv->value, tlv->len);
        reset = atoi(reset_type);
        indigo_logger(LOG_LEVEL_DEBUG, "Reset Type: %d", reset);
    }

    memset(buffer, 0, sizeof(buffer));
#ifdef _OPENWRT_QTI_
    run_system("wifi down");
    run_system("wifi config > /etc/config/wireless");
#else
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_hapd_exec_file());
    system(buffer);
    sleep(2);
#endif

#ifdef _OPENWRT_
#else
    len = system("rfkill unblock wlan");
    if (len) {
        indigo_logger(LOG_LEVEL_DEBUG, "Failed to run rfkill unblock wlan");
    }
    sleep(1);
#endif

    memset(buffer, 0, sizeof(buffer));
    len = pipe_command(buffer, sizeof(buffer), "/bin/pidof", parameter);
    if (len) {
#ifdef _OPENWRT_QTI_
        message = TLV_VALUE_HOSTAPD_STOP_OK;
#else
        message = TLV_VALUE_HOSTAPD_STOP_NOT_OK;
#endif
    } else {
        message = TLV_VALUE_HOSTAPD_STOP_OK;
    }

    /* Test case teardown case */
    if (reset == RESET_TYPE_TEARDOWN) {
    }

    /* reset interfaces info */
    if (clear_interfaces_resource()) {
    }

    if (reset == RESET_TYPE_INIT) {
#ifndef _OPENWRT_QTI_
        system("rm -rf /var/log/hostapd.log >/dev/null 2>/dev/null");
#endif
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

#ifdef _RESERVED_
/* The function is reserved for the defeault hostapd config */
#define HOSTAPD_DEFAULT_CONFIG_SSID                 "QuickTrack"
#define HOSTAPD_DEFAULT_CONFIG_CHANNEL              "36"
#define HOSTAPD_DEFAULT_CONFIG_HW_MODE              "a"
#define HOSTAPD_DEFAULT_CONFIG_WPA_PASSPHRASE       "12345678"
#define HOSTAPD_DEFAULT_CONFIG_IEEE80211N           "1"
#define HOSTAPD_DEFAULT_CONFIG_WPA                  "2"
#define HOSTAPD_DEFAULT_CONFIG_WPA_KEY_MGMT         "WPA-PSK"
#define HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE         "CCMP"

static void append_hostapd_default_config(struct packet_wrapper *wrapper) {
    if (find_wrapper_tlv_by_id(wrapper, TLV_SSID) == NULL) {
        add_wrapper_tlv(wrapper, TLV_SSID, strlen(HOSTAPD_DEFAULT_CONFIG_SSID), HOSTAPD_DEFAULT_CONFIG_SSID);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_CHANNEL) == NULL) {
        add_wrapper_tlv(wrapper, TLV_CHANNEL, strlen(HOSTAPD_DEFAULT_CONFIG_CHANNEL), HOSTAPD_DEFAULT_CONFIG_CHANNEL);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_HW_MODE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_HW_MODE, strlen(HOSTAPD_DEFAULT_CONFIG_HW_MODE), HOSTAPD_DEFAULT_CONFIG_HW_MODE);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_PASSPHRASE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_PASSPHRASE, strlen(HOSTAPD_DEFAULT_CONFIG_WPA_PASSPHRASE), HOSTAPD_DEFAULT_CONFIG_WPA_PASSPHRASE);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_IEEE80211_N) == NULL) {
        add_wrapper_tlv(wrapper, TLV_IEEE80211_N, strlen(HOSTAPD_DEFAULT_CONFIG_IEEE80211N), HOSTAPD_DEFAULT_CONFIG_IEEE80211N);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA, strlen(HOSTAPD_DEFAULT_CONFIG_WPA), HOSTAPD_DEFAULT_CONFIG_WPA);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_KEY_MGMT) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_KEY_MGMT, strlen(HOSTAPD_DEFAULT_CONFIG_WPA_KEY_MGMT), HOSTAPD_DEFAULT_CONFIG_WPA_KEY_MGMT);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_RSN_PAIRWISE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_RSN_PAIRWISE, strlen(HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE), HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE);
    }
}
#endif /* _RESERVED_ */

#ifndef _OPENWRT_QTI_
static int generate_hostapd_config(char *output, int output_size, struct packet_wrapper *wrapper, struct interface_info* wlanp) {
    int has_sae = 0, has_wpa = 0, has_pmf = 0, has_owe = 0, has_transition = 0, has_sae_groups = 0;
    int channel = 0, chwidth = 1, enable_ax = 0, chwidthset = 0, enable_muedca = 0, vht_chwidthset = 0;
    int i, enable_ac = 0, enable_11h = 0;
    char buffer[S_BUFFER_LEN], cfg_item[2*S_BUFFER_LEN];
    char band[64], value[16];
    char country[16];
    struct tlv_to_config_name* cfg = NULL;
    struct tlv_hdr *tlv = NULL;
    int is_6g_only = 0, unsol_pr_resp_interval = 0;

#if HOSTAPD_SUPPORT_MBSSID
    if (wlanp->mbssid_enable && !wlanp->transmitter)
        snprintf(output, output_size, "bss=%s\nctrl_interface=%s\n", wlanp->ifname, get_hapd_ctrl_path());
    else
#if defined(ANDROID) || defined(MDM)
        snprintf(output, output_size, "ctrl_interface=%s\ninterface=%s\n", get_hapd_ctrl_path(), wlanp->ifname);
#else
        snprintf(output, output_size, "ctrl_interface=%s\nctrl_interface_group=0\ninterface=%s\n", get_hapd_ctrl_path(), wlanp->ifname);
#endif
#else /*HOSTAPD_SUPPORT_MBSSID*/
#if defined(ANDROID) || defined(MDM)
    snprintf(output, output_size, "ctrl_interface=%s\ninterface=%s\n", get_hapd_ctrl_path(), wlanp->ifname);
#else
    snprintf(output, output_size, "ctrl_interface=%s\nctrl_interface_group=0\ninterface=%s\n", get_hapd_ctrl_path(), wlanp->ifname);
#endif
#endif /*HOSTAPD_SUPPORT_MBSSID*/

#ifdef _RESERVED_
    /* The function is reserved for the defeault hostapd config */
    append_hostapd_default_config(wrapper);
#endif

    memset(country, 0, sizeof(country));
    memset(band, 0, sizeof(band));

    /* QCA WTS image doesn't apply 11ax, mu_edca, country, 11d, 11h in hostapd */
    for (i = 0; i < wrapper->tlv_num; i++) {
        tlv = wrapper->tlv[i];

        if (tlv->id == TLV_HE_6G_ONLY) {
            is_6g_only = 1;
            continue;
        }

        if (tlv->id == TLV_BSS_IDENTIFIER) {
            if (is_band_enabled(BAND_6GHZ) && !wlanp->mbssid_enable) {
                strlcat(output, "rnr=1\n", output_size);
            }
            continue;
        }

        cfg = find_tlv_config(tlv->id);
        if (!cfg) {
            indigo_logger(LOG_LEVEL_ERROR, "Unknown AP configuration name: TLV ID 0x%04x", tlv->id);
            continue;
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv->value, "SAE") && strstr(tlv->value, "WPA-PSK")) {
            has_transition = 1;
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv->value, "OWE")) {
            has_owe = 1;
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv->value, "SAE")) {
            has_sae = 1;
        }

        if (tlv->id == TLV_WPA && strstr(tlv->value, "2")) {
            has_wpa = 1;
        }

        if (tlv->id == TLV_IEEE80211_W) {
            has_pmf = 1;
        }

        if (tlv->id == TLV_HW_MODE) {
            if (tlv->len > sizeof(band)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memcpy(band, tlv->value, tlv->len);
        }

        if (tlv->id == TLV_CHANNEL) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            channel = atoi(value);
        }

        if (tlv->id == TLV_HE_OPER_CHWIDTH) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            chwidth = atoi(value);
            chwidthset = 1;
#ifdef _WTS_OPENWRT_
            continue;
#endif
        }

        if (tlv->id == TLV_VHT_OPER_CHWIDTH) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            chwidth = atoi(value);
            vht_chwidthset = 1;
        }

        if (tlv->id == TLV_IEEE80211_AC && strstr(tlv->value, "1")) {
            enable_ac = 1;
        }

        if (tlv->id == TLV_IEEE80211_AX && strstr(tlv->value, "1")) {
            enable_ax = 1;
#ifdef _WTS_OPENWRT_
            continue;
#endif
        }

        if (tlv->id == TLV_HE_MU_EDCA) {
#ifdef _WTS_OPENWRT_
            continue;
#endif
            enable_muedca = 1;
        }

        if (tlv->id == TLV_SAE_GROUPS) {
            has_sae_groups = 1;
        }

        if (tlv->id == TLV_COUNTRY_CODE) {
            if (tlv->len > sizeof(country)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memcpy(country, tlv->value, tlv->len);
#ifdef _WTS_OPENWRT_
            continue;
#endif
        }

        if (tlv->id == TLV_IEEE80211_H) {
#ifdef _WTS_OPENWRT_
            continue;
#endif
            enable_11h = 1;
        }

#ifdef _WTS_OPENWRT_
        if (tlv->id == TLV_IEEE80211_D || tlv->id == TLV_HE_OPER_CENTR_FREQ)
            continue;

#endif

        if (tlv->id == TLV_HE_UNSOL_PR_RESP_CADENCE) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            unsol_pr_resp_interval = atoi(value);
        }

        memset(buffer, 0, sizeof(buffer));
        memset(cfg_item, 0, sizeof(cfg_item));
        if (tlv->id == TLV_OWE_TRANSITION_BSS_IDENTIFIER) {
            struct bss_identifier_info bss_info;
            struct interface_info *wlan;
            int bss_identifier;
            char bss_identifier_str[8];
            if (tlv->len >= sizeof(bss_identifier_str)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(&bss_info, 0, sizeof(bss_info));
            memset(bss_identifier_str, 0, sizeof(bss_identifier_str));
            memcpy(bss_identifier_str, tlv->value, tlv->len);
            bss_identifier = atoi(bss_identifier_str);
            parse_bss_identifier(bss_identifier, &bss_info);
            wlan = get_wireless_interface_info(bss_info.band, bss_info.identifier);
            if (NULL == wlan) {
                wlan = assign_wireless_interface_info(&bss_info);
            }
            printf("TLV_OWE_TRANSITION_BSS_IDENTIFIER: TLV_BSS_IDENTIFIER 0x%x identifier %d mapping ifname %s\n",
                    bss_identifier,
                    bss_info.identifier,
                    wlan ? wlan->ifname : "n/a"
                    );
            if (wlan) {
                memcpy(buffer, wlan->ifname, strlen(wlan->ifname));
                snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name, buffer);
                strlcat(output, cfg_item, output_size);
                if (has_owe) {
                    memset(cfg_item, 0, sizeof(cfg_item));
                    snprintf(cfg_item, sizeof(cfg_item), "ignore_broadcast_ssid=1\n");
                    strlcat(output, cfg_item, output_size);
                }
            }
        } else {
            memcpy(buffer, tlv->value, tlv->len);
            snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name, buffer);
            strlcat(output, cfg_item, output_size);
        }
    }

    if (has_pmf == 0) {
        if (has_transition) {
            strlcat(output, "ieee80211w=1\n", output_size);
        } else if (has_sae && has_wpa) {
            strlcat(output, "ieee80211w=2\n", output_size);
        } else if (has_owe) {
            strlcat(output, "ieee80211w=2\n", output_size);
        } else if (has_wpa) {
            strlcat(output, "ieee80211w=1\n", output_size);
        }
    }

    if (has_sae == 1) {
        strlcat(output, "sae_require_mfp=1\n", output_size);
    }

#if HOSTAPD_SUPPORT_MBSSID
    if (wlanp->mbssid_enable && wlanp->transmitter) {
        strlcat(output, "multiple_bssid=1\n", output_size);
    }
#endif

    // Note: if any new DUT configuration is added for sae_groups,
    // then the following unconditional sae_groups addition should be
    // changed to become conditional on there being no other sae_groups
    // configuration
    // e.g.:
    // if RequestTLV.SAE_GROUPS not in tlv_values:
    //     field_name = tlv_hostapd_config_mapper.get(RequestTLV.SAE_GROUPS)
    //     hostapd_config += "\n" + field_name + "=15 16 17 18 19 20 21"
    // Append the default SAE groups for SAE and no SAE groups TLV
    if (has_sae && has_sae_groups == 0) {
        strlcat(output, "sae_groups=15 16 17 18 19 20 21\n", output_size);
    }

    // Channel width configuration
    // Default: 20MHz in 2.4G(No configuration required) 80MHz(40MHz for 11N only) in 5G
    if (enable_ac == 0 && enable_ax == 0)
        chwidth = 0;

    if (is_6g_only) {
        if (chwidthset == 0) {
            snprintf(buffer, sizeof(buffer), "he_oper_chwidth=%d\n", chwidth);
            strlcat(output, buffer, output_size);
        }
        if (chwidth == 1)
            strlcat(output, "op_class=133\n", output_size);
        else if (chwidth == 2)
            strlcat(output, "op_class=134\n", output_size);
        snprintf(buffer, sizeof(buffer), "he_oper_centr_freq_seg0_idx=%d\n", get_6g_center_freq_index(channel, chwidth));
        strlcat(output, buffer, output_size);
        if (unsol_pr_resp_interval) {
            snprintf(buffer, sizeof(buffer), "unsol_bcast_probe_resp_interval=%d\n", unsol_pr_resp_interval);
            strlcat(output, buffer, output_size);
        } else {
            strlcat(output, "fils_discovery_max_interval=20\n", output_size);
        }
        /* Enable bss_color and country IE */
        strlcat(output, "he_bss_color=19\n", output_size);
        strlcat(output, "ieee80211d=1\n", output_size);
        strlcat(output, "country_code=US\n", output_size);
    } else if (strstr(band, "a")) {
        if (is_ht40plus_chan(channel))
            strlcat(output, "ht_capab=[HT40+]\n", output_size);
        else if (is_ht40minus_chan(channel))
            strlcat(output, "ht_capab=[HT40-]\n", output_size);
        else // Ch 165 and avoid hostapd configuration error
            chwidth = 0;
        if (chwidth > 0) {
            int center_freq = get_center_freq_index(channel, chwidth);
#ifndef _WTS_OPENWRT_
            if (chwidth == 2) {
                /* 160M: Need to enable 11h for DFS and enable 11d for 11h */
                strlcat(output, "ieee80211d=1\n", output_size);
                strlcat(output, "country_code=US\n", output_size);
                strlcat(output, "ieee80211h=1\n", output_size);
            }
#endif
            if (enable_ac) {
                if (vht_chwidthset == 0) {
                    snprintf(buffer, sizeof(buffer), "vht_oper_chwidth=%d\n", chwidth);
                    strlcat(output, buffer, output_size);
                }
                snprintf(buffer, sizeof(buffer), "vht_oper_centr_freq_seg0_idx=%d\n", center_freq);
                strlcat(output, buffer, output_size);
#ifndef _WTS_OPENWRT_
                if (chwidth == 2) {
                    strlcat(output, "vht_capab=[VHT160]\n", output_size);
                }
#endif
            }
            if (enable_ax) {
#ifndef _WTS_OPENWRT_
                if (chwidthset == 0) {
                    snprintf(buffer, sizeof(buffer), "he_oper_chwidth=%d\n", chwidth);
                    strlcat(output, buffer, output_size);
                }
                snprintf(buffer, sizeof(buffer), "he_oper_centr_freq_seg0_idx=%d\n", center_freq);
                strlcat(output, buffer, output_size);
#endif
            }
        }
    }

    if (enable_muedca) {
        strlcat(output, "he_mu_edca_qos_info_queue_request=1\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_timer=255\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_aci=1\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_timer=255\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_aci=2\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_timer=255\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_aci=3\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_timer=255\n", output_size);
    }

#if defined(_OPENWRT_) && !defined(_WTS_OPENWRT_)
    /* Make sure AP include power constranit element even in non DFS channel */
    if (enable_11h) {
        strlcat(output, "spectrum_mgmt_required=1\n", output_size);
        strlcat(output, "local_pwr_constraint=3\n", output_size);
    }
#endif

    /* vendor specific config, not via hostapd */
    configure_ap_radio_params(band, country, channel, chwidth);

    return strlen(output);
}
#endif

// RESP: {<ResponseTLV.STATUS: 40961>: '0',<ResponseTLV.MESSAGE: 40960>: 'DUT configured as AP : Configuration file created'}
static int configure_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len = 0;
    char buffer[L_BUFFER_LEN], ifname[S_BUFFER_LEN];
    struct tlv_hdr *tlv;
    char *message = "DUT configured as AP : Configuration file created";
    int bss_identifier = 0, band;
    struct interface_info* wlan = NULL;
    char bss_identifier_str[16], hw_mode_str[8];
    struct bss_identifier_info bss_info;

    memset(buffer, 0, sizeof(buffer));
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_IDENTIFIER);
    memset(ifname, 0, sizeof(ifname));
    memset(&bss_info, 0, sizeof(bss_info));
    if (tlv) {
        /* Multiple wlans configure must carry TLV_BSS_IDENTIFIER */
        memset(bss_identifier_str, 0, sizeof(bss_identifier_str));
        memcpy(bss_identifier_str, tlv->value, tlv->len);
        bss_identifier = atoi(bss_identifier_str);
        parse_bss_identifier(bss_identifier, &bss_info);
        wlan = get_wireless_interface_info(bss_info.band, bss_info.identifier);
        if (NULL == wlan) {
            wlan = assign_wireless_interface_info(&bss_info);
        }
        if (wlan && bss_info.mbssid_enable) {
            configure_ap_enable_mbssid();
            if (bss_info.transmitter) {
                band_transmitter[bss_info.band] = wlan;
            }
        }
        printf("TLV_BSS_IDENTIFIER 0x%x band %d multiple_bssid %d transmitter %d identifier %d\n",
               bss_identifier,
               bss_info.band,
               bss_info.mbssid_enable,
               bss_info.transmitter,
               bss_info.identifier
               );
#ifdef _OPENWRT_QTI_
        if (bss_info.band == 0)
                dut.ap_interface_2g = 1;
        else if (bss_info.band == 1)
                dut.ap_interface_5g = 1;
        else
                dut.ap_interface_6g = 1;

        if (dut.ap_interface_5g && dut.ap_interface_2g)
                dut.ap_is_dual = 1;
        else if (dut.ap_interface_6g && dut.ap_interface_5g)
                dut.ap_is_dual = 1;

        dut.ap_bss_info.identifier= bss_info.identifier;
        dut.ap_bss_info.band= bss_info.band;
#endif
    } else {
        /* Single wlan case */
        tlv = find_wrapper_tlv_by_id(req, TLV_HW_MODE);
        if (tlv)
        {
            memset(hw_mode_str, 0, sizeof(hw_mode_str));
            memcpy(hw_mode_str, tlv->value, tlv->len);
            if (find_wrapper_tlv_by_id(req, TLV_HE_6G_ONLY)) {
                band = BAND_6GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_6g = 1;
#endif
            } else if (!strncmp(hw_mode_str, "a", 1)) {
                band = BAND_5GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_5g = 1;
#endif
            } else {
                band = BAND_24GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_2g = 1;
#endif
            }
            /* Single wlan use ID 1 */
            bss_info.band = band;
            bss_info.identifier = 1;
            wlan = assign_wireless_interface_info(&bss_info);
        }
#ifdef _OPENWRT_QTI_
        /**** PLACEHOLDER CONDITION ****/
        if (dut.ap_interface_5g && dut.ap_interface_2g)
            dut.ap_is_dual = 1;;
#endif
    }
    if (wlan) {
#if _OPENWRT_QTI_
        len = generate_wireless_config(buffer, sizeof(buffer),
                req, wlan, &dut);
#else
        printf("ifname %s hostapd conf file %s\n",
               wlan ? wlan->ifname : "n/a",
               wlan ? wlan->hapd_conf_file: "n/a"
               );
        len = generate_hostapd_config(buffer, sizeof(buffer), req, wlan);
#endif
        if (len)
        {
#if HOSTAPD_SUPPORT_MBSSID
            if (bss_info.mbssid_enable && !bss_info.transmitter) {
                if (band_transmitter[bss_info.band]) {
                    append_file(band_transmitter[bss_info.band]->hapd_conf_file, buffer, len);
                }
                memset(wlan->hapd_conf_file, 0, sizeof(wlan->hapd_conf_file));
            } else
#endif
            {
                indigo_logger(LOG_LEVEL_DEBUG, "%s: hostapd_conf:\n %s", __func__, buffer);
                write_file(wlan->hapd_conf_file, buffer, len);
            }
        }
    }
    show_wireless_interface_info();

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len > 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

#ifdef HOSTAPD_SUPPORT_MBSSID_WAR
extern int use_openwrt_wpad;
#endif
// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'AP is up : Hostapd service is active'}

static int start_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char *message = TLV_VALUE_HOSTAPD_START_OK;
    char buffer[S_BUFFER_LEN];
    int len;
#ifndef _OPENWRT_QTI_
    int status = TLV_VALUE_STATUS_OK;
#endif
    int swap_hostapd = 0;

#ifdef _WTS_OPENWRT_
#ifndef _OPENWRT_QTI_
    openwrt_apply_radio_config();
    // DFS wait again if set wlan params after hostapd starts
    iterate_all_wlan_interfaces(start_ap_set_wlan_params);
#endif
#endif

    memset(buffer, 0, sizeof(buffer));
#if (defined(ANDROID) || defined(MDM))
    snprintf(buffer, sizeof(buffer), "%s -t -g %s %s %s &",
        get_hapd_full_exec_path(),
        get_hapd_ctrl_path(),
        get_hostapd_debug_arguments(),
        get_all_hapd_conf_files(&swap_hostapd));
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    len = system(buffer);
    sleep(5);
#else
#ifndef _OPENWRT_QTI_
    snprintf(buffer, sizeof(buffer), "%s -B -t -P /var/run/hostapd.pid -g %s %s -f /var/log/hostapd.log %s",
        get_hapd_full_exec_path(),
        get_hapd_global_ctrl_path(),
        get_hostapd_debug_arguments(),
        get_all_hapd_conf_files(&swap_hostapd));
    len = system(buffer);
    sleep(1);
#endif
#endif

    /* Bring up VAPs with MBSSID disable using WFA hostapd */
    if (swap_hostapd) {
#ifdef HOSTAPD_SUPPORT_MBSSID_WAR
        indigo_logger(LOG_LEVEL_INFO, "Use WFA hostapd for MBSSID disable VAPs with RNR");
        system("cp /overlay/hostapd /usr/sbin/hostapd");
        use_openwrt_wpad = 0;
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%s -B -t -P /var/run/hostapd_1.pid %s -f /var/log/hostapd_1.log %s",
                get_hapd_full_exec_path(),
                get_hostapd_debug_arguments(),
                get_all_hapd_conf_files(&swap_hostapd));
        len = system(buffer);
        sleep(1);
#endif
    }

#ifdef _OPENWRT_QTI_
    /* Configure Radio & VAP, commit the config */
    len = bring_up_ap_interface();
#else
#ifndef _WTS_OPENWRT_
    iterate_all_wlan_interfaces(start_ap_set_wlan_params);
#endif
#endif

    bridge_init(BRIDGE_WLANS);

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len >= 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

/* deprecated */
static int create_bridge_network_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int err = 0;
    char static_ip[S_BUFFER_LEN];
    struct tlv_hdr *tlv;
    char *message = TLV_VALUE_CREATE_BRIDGE_OK;

    /* TLV: TLV_STATIC_IP */
    memset(static_ip, 0, sizeof(static_ip));
    tlv = find_wrapper_tlv_by_id(req, TLV_STATIC_IP);
    if (tlv) {
        memcpy(static_ip, tlv->value, tlv->len);
    } else {
        message = TLV_VALUE_CREATE_BRIDGE_NOT_OK;
        err = -1;
        goto response;
    }

    /* Create new bridge */
    create_bridge(BRIDGE_WLANS);

    add_all_wireless_interface_to_bridge(BRIDGE_WLANS);

    set_interface_ip(BRIDGE_WLANS, static_ip);

    response:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, err >= 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

// Bytes to DUT : 01 50 06 00 ed ff ff 00 55 0c 31 39 32 2e 31 36 38 2e 31 30 2e 33
// RESP :{<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'Static Ip successfully assigned to wireless interface'}
static int assign_static_ip_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char buffer[64];
    struct tlv_hdr *tlv = NULL;
#ifndef _OPENWRT_QTI_
    char *ifname = NULL;
    int len = 0;
#endif
    char *message = TLV_VALUE_ASSIGN_STATIC_IP_OK;

    memset(buffer, 0, sizeof(buffer));
    tlv = find_wrapper_tlv_by_id(req, TLV_STATIC_IP);
    if (tlv) {
        memcpy(buffer, tlv->value, tlv->len);
    } else {
        message = "Failed.";
        goto response;
    }

#ifndef _OPENWRT_QTI_
    if (is_bridge_created()) {
        ifname = BRIDGE_WLANS;
    } else {
        ifname = get_wireless_interface();
    }
    /* Release IP address from interface */
    reset_interface_ip(ifname);
    /* Bring up interface */
    control_interface(ifname, "up");
    /* Set IP address with network mask */
    strlcat(buffer, "/24", sizeof(buffer));
    len = set_interface_ip(ifname, buffer);
    if (len) {
        message = TLV_VALUE_ASSIGN_STATIC_IP_NOT_OK;
    }
#endif
    response:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

// Bytes to DUT : 01 50 01 00 ee ff ff
// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: '9c:b6:d0:19:40:c7', <ResponseTLV.DUT_MAC_ADDR: 40963>: '9c:b6:d0:19:40:c7'}
static int get_mac_addr_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    struct tlv_hdr *tlv;
    struct wpa_ctrl *w = NULL;

    int status = TLV_VALUE_STATUS_NOT_OK;
    size_t resp_len = 0;
    char *message = TLV_VALUE_NOT_OK;

    char cmd[16];
    char response[L_BUFFER_LEN];

    char band[S_BUFFER_LEN];
    char ssid[S_BUFFER_LEN];
    char role[S_BUFFER_LEN];

    char connected_freq[S_BUFFER_LEN];
    char connected_ssid[S_BUFFER_LEN];
    char mac_addr[S_BUFFER_LEN] = {0};
    int bss_identifier = 0;
    struct interface_info* wlan = NULL;
    char bss_identifier_str[16];
    struct bss_identifier_info bss_info;
    char buff[S_BUFFER_LEN];
#ifdef _OPENWRT_QTI_
    char *ifname;
#endif

    if (req->tlv_num == 0) {
        get_mac_address(mac_addr, sizeof(mac_addr), get_wireless_interface());
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;

        goto done;
    } else {
        /* TLV: TLV_ROLE */
        memset(role, 0, sizeof(role));
        tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
        if (tlv) {
            memcpy(role, tlv->value, tlv->len);
        }

        /* TLV: TLV_BAND */
        memset(band, 0, sizeof(band));
        tlv = find_wrapper_tlv_by_id(req, TLV_BAND);
        if (tlv) {
            memcpy(band, tlv->value, tlv->len);
        }

        /* TLV: TLV_SSID */
        memset(ssid, 0, sizeof(ssid));
        tlv = find_wrapper_tlv_by_id(req, TLV_SSID);
        if (tlv) {
            memcpy(ssid, tlv->value, tlv->len);
        }

        memset(&bss_info, 0, sizeof(bss_info));
        tlv = find_wrapper_tlv_by_id(req, TLV_BSS_IDENTIFIER);
        if (tlv) {
            memset(bss_identifier_str, 0, sizeof(bss_identifier_str));
            memcpy(bss_identifier_str, tlv->value, tlv->len);
            bss_identifier = atoi(bss_identifier_str);
            parse_bss_identifier(bss_identifier, &bss_info);

            printf("TLV_BSS_IDENTIFIER 0x%x identifier %d band %d\n",
                    bss_identifier,
                    bss_info.identifier,
                    bss_info.band
                    );
        } else {
            bss_info.identifier = -1;
        }
    }

    if (atoi(role) == DUT_TYPE_STAUT) {
        w = wpa_ctrl_open(get_wpas_ctrl_path());
    } else {
        wlan = get_wireless_interface_info(bss_info.band, bss_info.identifier);
#ifdef _OPENWRT_QTI_
        ifname = owrt_get_ap_wireless_interface();

        if (dut.ap_is_dual) {
            if (bss_info.band == 0) {
               /* band id is '0'for 2.4G and '1' for 5G*/
               snprintf(buff, sizeof(buff), "/var/run/hostapd-wifi%d/ath%d",
                        bss_info.band + 1, bss_info.band + 1);
            } else if (bss_info.band == 1) {
               snprintf(buff, sizeof(buff), "/var/run/hostapd-wifi%d/ath%d",
                        bss_info.band - 1, bss_info.band - 1);
            } else {
               snprintf(buff, sizeof(buff), "/var/run/hostapd-wifi%d/ath%d",
                        bss_info.band, bss_info.band);
            }
        } else {
            if (bss_info.identifier > 1) {
               snprintf(buff, sizeof(buff), "%s%d", ifname, (bss_info.identifier - 1));
            } else {
               snprintf(buff, sizeof(buff), "%s", ifname);
            }
            get_mac_address(mac_addr, sizeof(mac_addr), buff);
            status = TLV_VALUE_STATUS_OK;
            message = TLV_VALUE_OK;
            goto done;
        }
        w = wpa_ctrl_open(buff);
#else
        w = wpa_ctrl_open(get_hapd_ctrl_path_by_id(wlan));
#endif
    }

    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to %s", atoi(role) == DUT_TYPE_STAUT ? "wpa_supplicant" : "hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_NOT_OK;
        goto done;
    }

    /* Assemble hostapd command */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "STATUS");
    /* Send command to hostapd UDS socket */
    resp_len = sizeof(response) - 1;
    memset(response, 0, sizeof(response));
    wpa_ctrl_request(w, cmd, strlen(cmd), response, &resp_len, NULL);

    /* Check response */
    get_key_value(connected_freq, response, "freq");

    memset(mac_addr, 0, sizeof(mac_addr));
    if (atoi(role) == DUT_TYPE_STAUT) {
        get_key_value(connected_ssid, response, "ssid");
        get_key_value(mac_addr, response, "address");
    } else {
#if HOSTAPD_SUPPORT_MBSSID
        if(wlan && wlan->mbssid_enable) {
            snprintf(buff, sizeof(buff), "ssid[%d]", wlan->hapd_bss_id);
            get_key_value(connected_ssid, response, buff);
            snprintf(buff, sizeof(buff), "bssid[%d]", wlan->hapd_bss_id);
            get_key_value(mac_addr, response, buff);
        } else {
            get_key_value(connected_ssid, response, "ssid[0]");
            get_key_value(mac_addr, response, "bssid[0]");
        }
#else
        get_key_value(connected_ssid, response, "ssid[0]");
        get_key_value(mac_addr, response, "bssid[0]");
#endif
    }

    if (bss_info.identifier >= 0) {
        printf("Get mac_addr %s\n", mac_addr);
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
        goto done;
    }

    /* Check band and connected freq*/
    if (strlen(band)) {
        int band_id = 0;

        if (strcmp(band, "2.4GHz") == 0)
            band_id = BAND_24GHZ;
        else if (strcmp(band, "5GHz") == 0)
            band_id = BAND_5GHZ;
        else if (strcmp(band, "6GHz") == 0)
            band_id = BAND_6GHZ;
        if (verify_band_from_freq(atoi(connected_freq), band_id) == 0) {
            status = TLV_VALUE_STATUS_OK;
            message = TLV_VALUE_OK;
        } else {
            status = TLV_VALUE_STATUS_NOT_OK;
            message = "Unable to get mac address associated with the given band";
            goto done;
        }
    }

    /* Check SSID and connected SSID */
    if (strlen(ssid)) {
        if (strcmp(ssid, connected_ssid) == 0) {
            status = TLV_VALUE_STATUS_OK;
            message = TLV_VALUE_OK;
        } else {
            status = TLV_VALUE_STATUS_NOT_OK;
            message = "Unable to get mac address associated with the given ssid";
            goto done;
        }
    }

    /* TODO: BSSID */

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    if (status == TLV_VALUE_STATUS_OK) {
        fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(mac_addr), mac_addr);
        fill_wrapper_tlv_bytes(resp, TLV_DUT_MAC_ADDR, strlen(mac_addr), mac_addr);
    } else {
        fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    }
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int start_loopback_server(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char local_ip[256];
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_LOOPBACK_SVR_START_NOT_OK;
    char tool_udp_port[16] = {0};

    /* Find network interface. If BRIDGE_WLANS exists, then use it. Otherwise, it uses the initiation value. */
    memset(local_ip, 0, sizeof(local_ip));
    if (find_interface_ip(local_ip, sizeof(local_ip), BRIDGE_WLANS)) {
        indigo_logger(LOG_LEVEL_DEBUG, "use %s", BRIDGE_WLANS);
    } else if (find_interface_ip(local_ip, sizeof(local_ip), get_wireless_interface())) {
        indigo_logger(LOG_LEVEL_DEBUG, "use %s", get_wireless_interface());
// #ifdef __TEST__
    } else if (find_interface_ip(local_ip, sizeof(local_ip), "eth0")) {
        indigo_logger(LOG_LEVEL_DEBUG, "use %s", "eth0");
// #endif /* __TEST__ */
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "No available interface");
        goto done;
    }
    /* Start loopback */
    if (!loopback_server_start(local_ip, tool_udp_port, LOOPBACK_TIMEOUT)) {
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_LOOPBACK_SVR_START_OK;
    }
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    fill_wrapper_tlv_bytes(resp, TLV_LOOP_BACK_SERVER_PORT, strlen(tool_udp_port), tool_udp_port);

    return 0;
}

// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'Loopback server in idle state'}
static int stop_loop_back_server_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    /* Stop loopback */
    if (loopback_server_status()) {
        loopback_server_stop();
    }
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(TLV_VALUE_LOOP_BACK_STOP_OK), TLV_VALUE_LOOP_BACK_STOP_OK);

    return 0;
}

static int send_ap_disconnect_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char buffer[S_BUFFER_LEN];
    char *message = NULL;
#ifndef _OPENWRT_QTI_
    size_t resp_len;
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    char *parameter[] = {"pidof", get_hapd_exec_file(), NULL};
    char address[32];
    char response[S_BUFFER_LEN];
    int len;
#endif

    /* Check hostapd status. TODO: it may use UDS directly */
    memset(buffer, 0, sizeof(buffer));
#ifndef _OPENWRT_QTI_
    len = pipe_command(buffer, sizeof(buffer), "/bin/pidof", parameter);
    if (len == 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to find hostapd PID");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_NOT_OK;
        goto done;
    }
    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }
    /* ControlApp on DUT */
    /* TLV: TLV_ADDRESS */
    memset(address, 0, sizeof(address));
    tlv = find_wrapper_tlv_by_id(req, TLV_ADDRESS);
    if (tlv) {
        memcpy(address, tlv->value, tlv->len);
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV:Address");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        goto done;
    }
    /* Assemble hostapd command */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "DISASSOCIATE %s reason=1", address);
    /* Send command to hostapd UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
#else
    run_system("wifi down");
    sleep(1);
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_HOSTAPD_STOP_OK;
    goto done;
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_HOSTAPD_STOP_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
#ifndef _OPENWRT_QTI_
    if (w) {
        wpa_ctrl_close(w);
    }
#endif
    return 0;
}

static int set_ap_parameter_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
#ifndef _OPENWRT_QTI_
    char response[1024];
    char param_name[32];
    char param_value[256];
    size_t resp_len;
    char buffer[8192];
    struct tlv_hdr *tlv = NULL;
#endif
    struct wpa_ctrl *w = NULL;
#ifdef _OPENWRT_QTI_
    set_ap_params(req);

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
    goto done;
#else

    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }

    /* ControlApp on DUT */
    /* TLV: MBO_ASSOC_DISALLOW or GAS_COMEBACK_DELAY */
    memset(param_value, 0, sizeof(param_value));
    tlv = find_wrapper_tlv_by_id(req, TLV_MBO_ASSOC_DISALLOW);
    if (!tlv) {
        tlv = find_wrapper_tlv_by_id(req, TLV_GAS_COMEBACK_DELAY);
    }
    if (tlv && find_tlv_config_name(tlv->id) != NULL) {
        strlcpy(param_name, find_tlv_config_name(tlv->id), sizeof(param_name));
        memcpy(param_value, tlv->value, tlv->len);
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_MBO_ASSOC_DISALLOW or TLV_GAS_COMEBACK_DELAY");
        goto done;
    }
    /* Assemble hostapd command */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "SET %s %s", param_name, param_value);
    /* Send command to hostapd UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int send_ap_btm_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
#ifndef _OPENWRT_QTI_
    char response[4096];
    char buffer[1024];
    size_t resp_len;
    char request[4096];
#endif
    char bssid[256];
    char disassoc_imminent[256];
    char disassoc_timer[256];
    char candidate_list[256];
    char reassoc_retry_delay[256];
    char bss_term_bit[256];
    char bss_term_tsf[256];
    char bss_term_duration[256];

    memset(bssid, 0, sizeof(bssid));
    memset(disassoc_imminent, 0, sizeof(disassoc_imminent));
    memset(disassoc_timer, 0, sizeof(disassoc_timer));
    memset(candidate_list, 0, sizeof(candidate_list));
    memset(reassoc_retry_delay, 0, sizeof(reassoc_retry_delay));
    memset(bss_term_bit, 0, sizeof(bss_term_bit));
    memset(bss_term_tsf, 0, sizeof(bss_term_tsf));
    memset(bss_term_duration, 0, sizeof(bss_term_duration));

    /* ControlApp on DUT */
    /* TLV: BSSID (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSSID);
    if (tlv) {
        memcpy(bssid, tlv->value, tlv->len);
    }
    /* DISASSOC_IMMINENT            disassoc_imminent=%s */
    tlv = find_wrapper_tlv_by_id(req, TLV_DISASSOC_IMMINENT);
    if (tlv) {
        memcpy(disassoc_imminent, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_disassoc_imnt = atoi(disassoc_imminent);
#endif
    }
    /* DISASSOC_TIMER               disassoc_timer=%s */
    tlv = find_wrapper_tlv_by_id(req, TLV_DISASSOC_TIMER);
    if (tlv) {
        memcpy(disassoc_timer, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_disassoc_timer = atoi(disassoc_timer);
#endif
    }
    /* REASSOCIAITION_RETRY_DELAY   mbo=0:{}:0 */
    tlv = find_wrapper_tlv_by_id(req, TLV_REASSOCIAITION_RETRY_DELAY);
    if (tlv) {
        memcpy(reassoc_retry_delay, tlv->value, tlv->len);
    }
    /* CANDIDATE_LIST              pref=1 */
    tlv = find_wrapper_tlv_by_id(req, TLV_CANDIDATE_LIST);
    if (tlv) {
        memcpy(candidate_list, tlv->value, tlv->len);
    }
    /* BSS_TERMINATION              bss_term_bit */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_TERMINATION);
    if (tlv) {
        memcpy(bss_term_bit, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_term_bit = atoi(bss_term_bit);
#endif
    }
    /* BSS_TERMINATION_TSF          bss_term_tsf */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_TERMINATION_TSF);
    if (tlv) {
        memcpy(bss_term_tsf, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_bss_term_tsf = atoi(bss_term_tsf);
#endif
    }
    /* BSS_TERMINATION_DURATION     bss_term_duration */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_TERMINATION_DURATION);
    if (tlv) {
        memcpy(bss_term_duration, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_bss_term_dur = atoi(bss_term_duration);
#endif
    }

#ifdef _OPENWRT_QTI_
    send_ap_btm_req(bssid);
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
    goto done;
#else
    /* Assemble hostapd command for BSS_TM_REQ */
    memset(request, 0, sizeof(request));
    snprintf(request, sizeof(request), "BSS_TM_REQ %s", bssid);
    /*  disassoc_imminent=%s */
    if (strlen(disassoc_imminent)) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " disassoc_imminent=%s", disassoc_imminent);
        strlcat(request, buffer, sizeof(request));
    }
    /* disassoc_timer=%s */
    if (strlen(disassoc_timer)) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " disassoc_timer=%s", disassoc_timer);
        strlcat(request, buffer, sizeof(request));
    }
    /* reassoc_retry_delay=%s */
    if (strlen(reassoc_retry_delay)) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " mbo=0:%s:0", reassoc_retry_delay);
        strlcat(request, buffer, sizeof(request));
    }
    /* if bss_term_bit && bss_term_tsf && bss_term_duration, then bss_term={bss_term_tsf},{bss_term_duration} */
    if (strlen(bss_term_bit) && strlen(bss_term_tsf) && strlen(bss_term_duration) ) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " bss_term=%s,%s", bss_term_tsf, bss_term_duration);
        strlcat(request, buffer, sizeof(request));
    }
    /* candidate_list */
    if (strlen(candidate_list) && atoi(candidate_list) == 1) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " pref=1");
        strlcat(request, buffer, sizeof(request));
    }
    indigo_logger(LOG_LEVEL_DEBUG, "cmd:%s", request);

    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, request, strlen(request), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
#endif
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int trigger_ap_channel_switch(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    char request[S_BUFFER_LEN];
#ifndef _OPENWRT_QTI_
    char response[S_BUFFER_LEN];
    size_t resp_len;
#else
    char buf[L_BUFFER_LEN];
    int width = 0;
    char *ifname;
#endif
    char channel[64];
    char frequency[64];
    int freq, center_freq, offset;

    memset(channel, 0, sizeof(channel));
    memset(frequency, 0, sizeof(frequency));

    /* ControlApp on DUT */
    /* TLV: TLV_CHANNEL (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_CHANNEL);
    if (tlv) {
        memcpy(channel, tlv->value, tlv->len);
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_CHANNEL");
        goto done;
    }
    /* TLV_FREQUENCY (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_FREQUENCY);
    if (tlv) {
        memcpy(frequency, tlv->value, tlv->len);
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_FREQUENCY");
    }

    center_freq = 5000 + get_center_freq_index(atoi(channel), 1) * 5;
    freq = atoi(frequency);
    if ((center_freq == freq + 30) || (center_freq == freq - 10))
        offset = 1;
    else
        offset = -1;
    /* Assemble hostapd command for channel switch */
    memset(request, 0, sizeof(request));
    snprintf(request, sizeof(request), "CHAN_SWITCH 10 %s center_freq1=%d sec_channel_offset=%d bandwidth=80 vht", frequency, center_freq, offset);
    indigo_logger(LOG_LEVEL_INFO, "%s", request);

#ifdef _OPENWRT_QTI_
    if (dut.ap_chwidth == AP_20)
        width = 20;
    else if (dut.ap_chwidth == AP_40)
        width = 40;
    else if (dut.ap_chwidth == AP_80)
        width = 80;

    ifname = owrt_get_ap_wireless_interface();
    snprintf(buf, sizeof(buf), "cfg80211tool %s doth_ch_chwidth %d 3 %d", ifname, atoi(channel), width);
    run_system(buf);
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
    goto done;
#else
    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, request, strlen(request), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int get_ip_addr_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
    char buffer[64] = {0};

    if (find_interface_ip(buffer, sizeof(buffer), BRIDGE_WLANS)) {
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
    } else if (find_interface_ip(buffer, sizeof(buffer), get_wireless_interface())) {
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_NOT_OK;
    }
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (status == TLV_VALUE_STATUS_OK) {
        fill_wrapper_tlv_bytes(resp, TLV_DUT_WLAN_IP_ADDR, strlen(buffer), buffer);
    }
    return 0;
}

static int stop_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len = 0, reset = 0;
    char buffer[S_BUFFER_LEN], reset_type[16];
    char *parameter[] = {"pidof", get_wpas_exec_file(), NULL};
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;

    /* TLV: RESET_TYPE */
    tlv = find_wrapper_tlv_by_id(req, TLV_RESET_TYPE);
    memset(reset_type, 0, sizeof(reset_type));
    if (tlv) {
        memcpy(reset_type, tlv->value, tlv->len);
        reset = atoi(reset_type);
        indigo_logger(LOG_LEVEL_DEBUG, "Reset Type: %d", reset);
    }

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_wpas_exec_file());
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(2);

    /* Test case teardown case */
    if (reset == RESET_TYPE_TEARDOWN) {
    }

    if (reset == RESET_TYPE_INIT) {
        /* clean the log */
#ifndef ANDROID
        system("rm -rf /var/log/supplicant.log >/dev/null 2>/dev/null");
#endif

        /* remove pac file if needed */
        if (strlen(pac_file_path)) {
            remove_pac_file(pac_file_path);
            memset(pac_file_path, 0, sizeof(pac_file_path));
        }
    }

    len = reset_interface_ip(get_wireless_interface());
    if (len) {
        indigo_logger(LOG_LEVEL_DEBUG, "Failed to free IP address");
    }
    sleep(1);

    len = pipe_command(buffer, sizeof(buffer), "/bin/pidof", parameter);
    if (len) {
        message = TLV_VALUE_WPA_S_STOP_NOT_OK;
    } else {
        message = TLV_VALUE_WPA_S_STOP_OK;
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

#ifdef _RESERVED_
/* The function is reserved for the defeault wpas config */
#define WPAS_DEFAULT_CONFIG_SSID                    "QuickTrack"
#define WPAS_DEFAULT_CONFIG_WPA_KEY_MGMT            "WPA-PSK"
#define WPAS_DEFAULT_CONFIG_PROTO                   "RSN"
#define HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE         "CCMP"
#define WPAS_DEFAULT_CONFIG_WPA_PASSPHRASE          "12345678"

static void append_wpas_network_default_config(struct packet_wrapper *wrapper) {
    if (find_wrapper_tlv_by_id(wrapper, TLV_SSID) == NULL) {
        add_wrapper_tlv(wrapper, TLV_SSID, strlen(WPAS_DEFAULT_CONFIG_SSID), WPAS_DEFAULT_CONFIG_SSID);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_KEY_MGMT) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_KEY_MGMT, strlen(WPAS_DEFAULT_CONFIG_WPA_KEY_MGMT), WPAS_DEFAULT_CONFIG_WPA_KEY_MGMT);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_PROTO) == NULL) {
        add_wrapper_tlv(wrapper, TLV_PROTO, strlen(WPAS_DEFAULT_CONFIG_PROTO), WPAS_DEFAULT_CONFIG_PROTO);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_RSN_PAIRWISE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_RSN_PAIRWISE, strlen(HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE), HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_PASSPHRASE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_PASSPHRASE, strlen(WPAS_DEFAULT_CONFIG_WPA_PASSPHRASE), WPAS_DEFAULT_CONFIG_WPA_PASSPHRASE);
    }
}
#endif /* _RESERVED_ */

static int generate_wpas_config(char *buffer, int buffer_size, struct packet_wrapper *wrapper) {
    int i;
    char value[S_BUFFER_LEN], cfg_item[2*S_BUFFER_LEN], buf[S_BUFFER_LEN];
    int ieee80211w_configured = 0;
    int transition_mode_enabled = 0;
    int owe_configured = 0;
    int sae_only = 0;

    struct tlv_to_config_name* cfg = NULL;

    snprintf(buffer, buffer_size, "ctrl_interface=%s\nap_scan=1\npmf=1\n", get_wpas_ctrl_path());

    for (i = 0; i < wrapper->tlv_num; i++) {
        cfg = find_wpas_global_config_name(wrapper->tlv[i]->id);
        if (cfg) {
            memset(value, 0, sizeof(value));
            memcpy(value, wrapper->tlv[i]->value, wrapper->tlv[i]->len);
            snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name, value);
            strlcat(buffer, cfg_item, buffer_size);
        }
    }
    strlcat(buffer, "network={\n", buffer_size);

#ifdef _RESERVED_
    /* The function is reserved for the defeault wpas config */
    append_wpas_network_default_config(wrapper);
#endif /* _RESERVED_ */

    for (i = 0; i < wrapper->tlv_num; i++) {
        cfg = find_tlv_config(wrapper->tlv[i]->id);
        if (cfg && find_wpas_global_config_name(wrapper->tlv[i]->id) == NULL) {
            memset(value, 0, sizeof(value));
            memcpy(value, wrapper->tlv[i]->value, wrapper->tlv[i]->len);

            if ((wrapper->tlv[i]->id == TLV_IEEE80211_W) || (wrapper->tlv[i]->id == TLV_STA_IEEE80211_W)) {
                ieee80211w_configured = 1;
            } else if (wrapper->tlv[i]->id == TLV_KEY_MGMT) {
                if (strstr(value, "WPA-PSK") && strstr(value, "SAE")) {
                    transition_mode_enabled = 1;
                }
                if (!strstr(value, "WPA-PSK") && strstr(value, "SAE")) {
                    sae_only = 1;
                }

                if (strstr(value, "OWE")) {
                    owe_configured = 1;
                }
            } else if ((wrapper->tlv[i]->id == TLV_CA_CERT) && strcmp("DEFAULT", value) == 0) {
                snprintf(value, sizeof(value), "/etc/ssl/certs/ca-certificates.crt");
            } else if ((wrapper->tlv[i]->id == TLV_PAC_FILE)) {
                memset(pac_file_path, 0, sizeof(pac_file_path));
                snprintf(pac_file_path, sizeof(pac_file_path), "%s", value);
            } else if (wrapper->tlv[i]->id == TLV_SERVER_CERT) {
                memset(buf, 0, sizeof(buf));
                get_server_cert_hash(value, buf, sizeof(buf));
                memcpy(value, buf, sizeof(buf));
            }

            if (cfg->quoted) {
                snprintf(cfg_item, sizeof(cfg_item), "%s=\"%s\"\n", cfg->config_name, value);
                strlcat(buffer, cfg_item, buffer_size);
            } else {
                snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name, value);
                strlcat(buffer, cfg_item, buffer_size);
            }
        }
    }

    if (ieee80211w_configured == 0) {
        if (transition_mode_enabled) {
            strlcat(buffer, "ieee80211w=1\n", buffer_size);
        } else if (sae_only) {
            strlcat(buffer, "ieee80211w=2\n", buffer_size);
        } else if (owe_configured) {
            strlcat(buffer, "ieee80211w=2\n", buffer_size);
        }
    }

    /* TODO: merge another file */
    /* python source code:
        if merge_config_file:
        appended_supplicant_conf_str = ""
        existing_conf = StaCommandHelper.get_existing_supplicant_conf()
        wpa_supplicant_dict = StaCommandHelper.__convert_config_str_to_dict(config = wps_config)
        for each_key in existing_conf:
            if each_key not in wpa_supplicant_dict:
                wpa_supplicant_dict[each_key] = existing_conf[each_key]

        for each_supplicant_conf in wpa_supplicant_dict:
            appended_supplicant_conf_str += each_supplicant_conf + "=" + wpa_supplicant_dict[each_supplicant_conf] + "\n"
        wps_config = appended_supplicant_conf_str.rstrip()
    */

    strlcat(buffer, "}\n", buffer_size);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SUPPLICANT_CONF:\n %s\n", __func__, buffer);
    return strlen(buffer);
}

static int configure_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len;
    char buffer[L_BUFFER_LEN];
    char *message = "DUT configured as STA : Configuration file created";

    memset(buffer, 0, sizeof(buffer));
    len = generate_wpas_config(buffer, sizeof(buffer), req);
    if (len) {
        write_file(get_wpas_conf_file(), buffer, len);
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len > 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

static int associate_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char *message = TLV_VALUE_WPA_S_START_UP_NOT_OK;
    char buffer[256];
    int status = TLV_VALUE_STATUS_NOT_OK;
#if defined(ANDROID) || defined(MDM)
    struct wpa_ctrl *w = NULL;
    size_t resp_len;
    char response[1024];
#endif

#ifdef _OPENWRT_
#else
    system("rfkill unblock wlan");
    sleep(1);
#endif

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_wpas_exec_file());
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(5);

    /* Start WPA supplicant */
    memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s",
        get_wpas_full_exec_path(),
        get_wpas_conf_file(),
        get_wpas_debug_arguments(),
        get_wireless_interface());
#else
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s -f /var/log/supplicant.log",
        get_wpas_full_exec_path(),
        get_wpas_conf_file(),
        get_wpas_debug_arguments(),
        get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(4);

#if defined(ANDROID) || defined(MDM)
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_START_UP_NOT_OK;
        goto done;
    }
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "SCAN");
    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    sleep(5);
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_START_UP_OK;

#if defined(ANDROID) || defined(MDM)
done:
#endif
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }

    return 0;
}

static int send_sta_disconnect_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    struct wpa_ctrl *w = NULL;
    char *message = TLV_VALUE_WPA_S_DISCONNECT_NOT_OK;
    char buffer[256], response[1024];
    int status;
    size_t resp_len;

    /* Open WPA supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_DISCONNECT_NOT_OK;
        goto done;
    }
    /* Send command to hostapd UDS socket */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "DISCONNECT");
    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        status = TLV_VALUE_STATUS_NOT_OK;
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_DISCONNECT_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int send_sta_reconnect_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    struct wpa_ctrl *w = NULL;
    char *message = TLV_VALUE_WPA_S_RECONNECT_NOT_OK;
    char buffer[256], response[1024];
    int status;
    size_t resp_len;

    /* Open WPA supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_RECONNECT_NOT_OK;
        goto done;
    }
    /* Send command to hostapd UDS socket */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "RECONNECT");
    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        status = TLV_VALUE_STATUS_NOT_OK;
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_RECONNECT_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int set_sta_parameter_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    size_t resp_len;
    char *message = NULL;
    char buffer[BUFFER_LEN];
    char response[BUFFER_LEN];
    char param_name[32];
    char param_value[256];
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    /* Example: Use TLV_STA_IEEE80211_W. Change to corresponding TLV from Tool */
    memset(param_value, 0, sizeof(param_value));
    tlv = find_wrapper_tlv_by_id(req, TLV_STA_IEEE80211_W);
    if (tlv && find_tlv_config_name(tlv->id) != NULL) {
        strlcpy(param_name, find_tlv_config_name(tlv->id), sizeof(param_name));
        memcpy(param_value, tlv->value, tlv->len);
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: STA_IEEE80211_W");
        goto done;
    }
    /* Assemble wpa_supplicant command */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "SET %s %s", param_name, param_value);
    /* Send command to wpa_supplicant UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_WPA_SET_PARAMETER_NO_OK;
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int send_sta_btm_query_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    size_t resp_len;
    char *message = TLV_VALUE_WPA_S_BTM_QUERY_NOT_OK;
    char buffer[1024];
    char response[1024];
    char reason_code[256];
    char candidate_list[256] = {0};
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    /* TLV: BTMQUERY_REASON_CODE */
    tlv = find_wrapper_tlv_by_id(req, TLV_BTMQUERY_REASON_CODE);
    if (tlv) {
        memcpy(reason_code, tlv->value, tlv->len);
    } else {
        goto done;
    }

    /* TLV: TLV_CANDIDATE_LIST */
    tlv = find_wrapper_tlv_by_id(req, TLV_CANDIDATE_LIST);
    if (tlv) {
        memcpy(candidate_list, tlv->value, tlv->len);
    }

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "WNM_BSS_QUERY %s", reason_code);
    if (strcmp(candidate_list, "1") == 0) {
        strlcat(buffer, " list", sizeof(buffer));
    }

    /* Send command to wpa_supplicant UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

static int send_sta_anqp_query_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len, status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_WPA_S_BTM_QUERY_NOT_OK;
    char buffer[1024];
    char response[1024];
    char bssid[256];
    char anqp_info_id[256];
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    size_t resp_len;

    /* It may need to check whether to just scan */
    memset(buffer, 0, sizeof(buffer));
    len = snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\nnetwork={\nssid=\"Scanning\"\n}", get_wpas_ctrl_path());
    if (len) {
        write_file(get_wpas_conf_file(), buffer, len);
    }

    memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s",
        get_wpas_full_exec_path(),
        get_wpas_conf_file(),
        get_wpas_debug_arguments(),
        get_wireless_interface());
#else
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s -i %s -f /var/log/supplicant.log",
        get_wpas_full_exec_path(),
        get_wpas_conf_file(),
        get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    len = system(buffer);
    sleep(2);

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    // SCAN
    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "SCAN");
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        goto done;
    }
    sleep(10);

    /* TLV: BSSID */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSSID);
    if (tlv) {
        memset(bssid, 0, sizeof(bssid));
        memcpy(bssid, tlv->value, tlv->len);
    } else {
        goto done;
    }

    memset(anqp_info_id, 0, sizeof(anqp_info_id));
    /* TLV: ANQP_INFO_ID */
    tlv = find_wrapper_tlv_by_id(req, TLV_ANQP_INFO_ID);
    if (tlv) {
        memcpy(anqp_info_id, tlv->value, tlv->len);
    }

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "ANQP_GET %s", bssid);
    if (strcmp(anqp_info_id, "NeighborReportReq") == 0) {
        strlcat(buffer, " 272", sizeof(buffer));
    } else if (strcmp(anqp_info_id, "QueryListWithCellPref") == 0) {
        strlcat(buffer, " mbo:2", sizeof(buffer));
    }

    /* Send command to wpa_supplicant UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);

    printf("%s -> resp: %s\n", buffer, response);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}
