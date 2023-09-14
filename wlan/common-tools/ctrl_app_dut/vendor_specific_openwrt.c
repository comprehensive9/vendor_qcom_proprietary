/*
 *   Copyright (c) 2022 Qualcomm Technologies, Inc.
 *   All Rights Reserved.
 *   Confidential and Proprietary - Qualcomm Technologies, Inc.


 *   Sigma Control API DUT (station/AP)
 *   Copyright (c) 2010-2011, Atheros Communications, Inc.
 *   Copyright (c) 2011-2017, Qualcomm Atheros, Inc.
 *   Copyright (c) 2018-2021, The Linux Foundation
 *   All Rights Reserved.
 *   Licensed under the Clear BSD license.


 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted (subject to the limitations in the
 *   disclaimer below) provided that the following conditions are met:

 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of Qualcomm Atheros, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
 *   BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 *   BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "indigo_api.h"
#include "vendor_specific.h"
#include "vendor_specific_openwrt.h"
#include "utils.h"
#include "wpa_ctrl.h"
#include <sys/ioctl.h>
#include <stdarg.h>

extern char *indigo_radio_ifname[];
char *indigo_radio_ifname_ap[MAX_RADIO] = {NULL, NULL, NULL};
extern int num_radio;
int run_system(const char *cmd);
struct tlv_to_config_name* find_tlv_config(int tlv_id);
#define BUZZ_SIZE 1024

int run_system(const char *cmd)
{
    int res=0;

    indigo_logger(LOG_LEVEL_DEBUG, "System: %s", cmd);
    res = system(cmd);
    if (res < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute command '%s'",
                cmd);
        return res;
    }
    return res;
}

int run_system_wrapper(const char *cmd, ...)
{
    va_list ap;
    char *buf;
    int bytes_required;
    int res;

    va_start(ap, cmd);
    bytes_required = vsnprintf(NULL, 0, cmd, ap);
    bytes_required += 1;
    va_end(ap);
    buf = malloc(bytes_required);
    if (!buf) {
        indigo_logger(LOG_LEVEL_ERROR, "ERROR!! No memory");
        return -1;
    }
    va_start(ap, cmd);
    vsnprintf(buf, bytes_required, cmd, ap);
    va_end(ap);
    res = run_system(buf);
    free(buf);
    return res;
}

char*
owrt_get_ap_wireless_interface(void)
{
    char *ifname;
    if (indigo_radio_ifname_ap[0] &&
        strcmp(indigo_radio_ifname_ap[0], "wifi2") == 0)
        ifname = "ath2";
    else if (indigo_radio_ifname_ap[0] &&
             strcmp(indigo_radio_ifname_ap[0], "wifi1") == 0)
        ifname = "ath1";
    else
        ifname = "ath0";

    return ifname;
}

void owrt_ap_set_vap(int id, const char *key,const char *val)
{
    char buf[256];

    if (val == NULL) {
        snprintf(buf, sizeof(buf),
                 "uci delete wireless.@wifi-iface[%d].%s", id, key);
        run_system(buf);
        return;
    }

    snprintf(buf, sizeof(buf), "uci set wireless.@wifi-iface[%d].%s=%s",
             id, key, val);
    run_system(buf);
}

void owrt_ap_add_vap(int id, const char *key,
                     const char *val)
{
    char buf[256];
    int res;

    if (val == NULL) {
        res = snprintf(buf, sizeof(buf),
                       "uci delete wireless.@wifi-iface[%d].%s",
                       id, key);
        if (res >= 0 && res < sizeof(buf))
            run_system(buf);
        return;
    }

    run_system("uci add wireless wifi-iface");
    res = snprintf(buf, sizeof(buf),
                   "uci set wireless.@wifi-iface[%d].%s=%s",
                   id, key, val);
    if (res >= 0 && res < sizeof(buf))
        run_system(buf);
    snprintf(buf, sizeof(buf), "uci set wireless.@wifi-iface[%d].%s=%s",
             id, "network", "lan");
    run_system(buf);
    snprintf(buf, sizeof(buf), "uci set wireless.@wifi-iface[%d].%s=%s",
             id, "mode", "ap");
    run_system(buf);
    snprintf(buf, sizeof(buf), "uci set wireless.@wifi-iface[%d].%s=%s",
             id, "encryption", "none");
    run_system(buf);
}

void owrt_ap_set_list_vap(int id,const char *key, const char *val)
{
    char buf[4096];

    if (val == NULL) {
        snprintf(buf, sizeof(buf),
                 "uci del_list wireless.@wifi-iface[%d].%s", id, key);
        run_system(buf);
        return;
    }

    snprintf(buf, sizeof(buf),
             "uci add_list wireless.@wifi-iface[%d].%s=%s",
             id, key, val);
    run_system(buf);
}

void owrt_ap_set_radio( int id,const char *key, const char *val)
{
    char buf[100];

    if (val == NULL) {
        snprintf(buf, sizeof(buf),
                 "uci delete wireless.wifi%d.%s", id, key);
        run_system(buf);
        return;
    }

    snprintf(buf, sizeof(buf), "uci set wireless.wifi%d.%s=%s",
             id, key, val);
    run_system(buf);
}

void owrt_ap_set_iniwifi( const char *key, const char *val)
{
    char buf[100];

    if (val == NULL) {
        snprintf(buf, sizeof(buf),
                 "uci delete wireless.qcawifi.%s", key);
        run_system(buf);
        return;
    }

    snprintf(buf, sizeof(buf), "uci set wireless.qcawifi=qcawifi");
    run_system(buf);
    snprintf(buf, sizeof(buf), "uci set wireless.qcawifi.%s=%s", key, val);
    run_system(buf);
}

#define OPENWRT_MAX_NUM_RADIOS (MAX_RADIO)
int owrt_ap_config_radio(struct indigo_dut *dut)
{
    int radio_id[MAX_RADIO] = { 0, 1, 2 };
    int radio_count, radio_no;
    char buf[64];

    for (radio_count = 0; radio_count < OPENWRT_MAX_NUM_RADIOS;
         radio_count++) {
        snprintf(buf, sizeof(buf), "%s%d", "wifi", radio_count);
        for (radio_no = 0; radio_no < MAX_RADIO; radio_no++) {
            if (!indigo_radio_ifname_ap[radio_no] ||
                strcmp(indigo_radio_ifname_ap[radio_no], buf) != 0)
                continue;
            owrt_ap_set_radio(radio_count, "disabled", "0");
            owrt_ap_set_vap(radio_count, "device", buf);
            radio_id[radio_no] = radio_count;
        }
    }

    /* Hardware mode (11a/b/g/n/ac) & HT mode selection */
    switch (dut->ap_mode) {
        case AP_11g:
            if(dut->enable_11ax == 1) {
                owrt_ap_set_radio(radio_id[0], "hwmode", "11axg");
            } else {
            owrt_ap_set_radio(radio_id[0], "hwmode", "11g");
            }
            break;
        case AP_11b:
            owrt_ap_set_radio(radio_id[0], "hwmode", "11b");
            break;
        case AP_11ng:
            owrt_ap_set_radio(radio_id[0], "hwmode", "11ng");
            owrt_ap_set_radio(radio_id[0], "htmode", "HT20");
            break;
        case AP_11a:
            if(dut->enable_11ax == 1) {
                 owrt_ap_set_radio(radio_id[0], "hwmode", "11axa");
            } else {
            owrt_ap_set_radio(radio_id[0], "hwmode", "11a");
            }
            break;
        case AP_11na:
            owrt_ap_set_radio(radio_id[0], "hwmode", "11na");
            owrt_ap_set_radio(radio_id[0], "htmode", "HT20");
            break;
        case AP_11ac:
            owrt_ap_set_radio(radio_id[0], "hwmode", "11ac");
            owrt_ap_set_radio(radio_id[0], "htmode", "HT80");
            break;
        case AP_11ax:
            if (dut->ap_channel >= 36) {
                owrt_ap_set_radio(radio_id[0], "hwmode", "11axa");
                owrt_ap_set_radio(radio_id[0], "htmode", "HT80");
            } else {
                owrt_ap_set_radio(radio_id[0], "hwmode", "11axg");
                owrt_ap_set_radio(radio_id[0], "htmode", "HT20");
            }
            break;
        case AP_inval:
           printf("MODE NOT SPECIFIED!");
            break;
        default:
            owrt_ap_set_radio(radio_id[0], "hwmode", "11ng");
            owrt_ap_set_radio(radio_id[0], "htmode", "HT20");
            break;
    }

    /* Channel */
    snprintf(buf, sizeof(buf), "%d", dut->ap_channel);
    owrt_ap_set_radio(radio_id[0], "channel", buf);

    switch (dut->ap_chwidth) {
        case AP_20:
            owrt_ap_set_radio(radio_id[0], "htmode", "HT20");
            break;
        case AP_40:
            owrt_ap_set_radio(radio_id[0], "htmode", "HT40");
            break;
        case AP_80:
            owrt_ap_set_radio(radio_id[0], "htmode", "HT80");
            break;
        case AP_160:
            owrt_ap_set_radio(radio_id[0], "htmode", "HT160");
            break;
        case AP_80_80:
            owrt_ap_set_radio(radio_id[0], "htmode", "HT80_80");
            break;
        case AP_AUTO:
        default:
            break;
    }

    if (dut->ap_is_dual) {
        snprintf(buf, sizeof(buf), "%d", dut->ap_channel_1);
        owrt_ap_set_radio(radio_id[1], "channel", buf);
        /* edge_channel_deprioritize WAR*/
        owrt_ap_set_radio(radio_id[1], "edge_channel_deprioritize", "0");
    } else {
        owrt_ap_set_radio(radio_id[0], "edge_channel_deprioritize", "0");
    }

    /* Country Code */
    if (dut->ap_reg_domain == REG_DOMAIN_GLOBAL) {
        const char *country;

        country = dut->ap_countrycode[0] ? dut->ap_countrycode : "US";
        snprintf(buf, sizeof(buf), "%s4", country);
        owrt_ap_set_radio(radio_id[0], "country", buf);
        if (dut->ap_is_dual)
            owrt_ap_set_radio(radio_id[1], "country", buf);
    } else if (dut->ap_countrycode[0]) {
        owrt_ap_set_radio(radio_id[0], "country",
                          dut->ap_countrycode);
    }

    if (dut->ap_bss_info.identifier > 1 && !dut->ap_has_owe_identifier) {
        owrt_ap_set_iniwifi("mbss_ie_enable", "0x00070007");
        owrt_ap_set_iniwifi("ema_ap_max_pp", "10");
        owrt_ap_set_iniwifi("ema_ap_max_groups", "1");
    }

    if(dut->enable_11ax == 1) {
        owrt_ap_set_radio(radio_id[0], "he_bsscolor", "'1 1'");
        owrt_ap_set_iniwifi("ap_bss_color_collision_detection", "0");
    }

    if (dut->ap_interface_6g) {
        owrt_ap_set_radio(radio_id[0], "band", "3");
        if (dut->ap_is_dual) {
            owrt_ap_set_radio(radio_id[1], "band", "1");
        }
    }

    return 0;
}

static int ap_ft_enabled(struct indigo_dut *dut)
{
    return dut->ap_ft_oa == 1 ||
        dut->ap_ft_ds == VALUE_ENABLED ||
        dut->ap_key_mgmt == AP_WPA2_FT_EAP ||
        dut->ap_key_mgmt == AP_WPA2_FT_PSK ||
        dut->ap_key_mgmt == AP_WPA2_ENT_FT_EAP ||
        (dut->ap_akm_values &
         ((1 << AKM_FT_EAP) |
          (1 << AKM_FT_PSK) |
          (1 << AKM_FT_SAE) |
          (1 << AKM_FT_SUITE_B) |
          (1 << AKM_FT_FILS_SHA256) |
          (1 << AKM_FT_FILS_SHA384)));
}

static const char *
hostapd_group_mgmt_cipher_name(enum ap_group_mgmt_cipher cipher)
{
    switch (cipher) {
        case AP_BIP_GMAC_256:
            return "BIP-GMAC-256";
        case AP_BIP_CMAC_256:
            return "BIP-CMAC-256";
        case AP_BIP_GMAC_128:
            return "BIP-GMAC-128";
        case AP_BIP_CMAC_128:
            return "AES-128-CMAC";
        default:
            return "UNKNOWN";
    }
}

static const char * hostapd_cipher_name(enum ap_cipher cipher)
{
    switch (cipher) {
        case AP_CCMP:
            return "CCMP";
        case AP_TKIP:
            return "TKIP";
        case AP_CCMP_TKIP:
            return "CCMP TKIP";
        case AP_GCMP_256:
            return "GCMP-256";
        case AP_GCMP_128:
            return "GCMP";
        case AP_CCMP_256:
            return "CCMP-256";
        case AP_CCMP_128_GCMP_256:
            return "CCMP GCMP-256";
        default:
            return "UNKNOWN";
    }
}

static int sae_pw_id_used(struct indigo_dut *dut)
{
        return dut->ap_sae_passwords &&
                    strchr(dut->ap_sae_passwords, ':');
}

int owrt_ap_config_vap(struct indigo_dut *dut)
{
    char buf[512];
    int vap_id = 0, vap_count, j, res;
    int dual_vap_id = 0; /*Correct vap id for dual vap configuration*/
    const char *ifname;
    char ifname2[50];
    int radius_das_port = 3799;
    int radius_das_count = 1;

    ifname = owrt_get_ap_wireless_interface();

    for (vap_count = 0; vap_count < OPENWRT_MAX_NUM_RADIOS; vap_count++) {
        if (!dut->ap_is_dual)
            snprintf(buf, sizeof(buf), "wifi%d", vap_count);
        else {
            if (vap_count == 2)
                continue;
            strlcpy(buf, indigo_radio_ifname_ap[vap_count], sizeof(buf));
        }

        for (vap_id = 0; vap_id < MAX_RADIO; vap_id++) {
            if (indigo_radio_ifname_ap[vap_id] &&
                strcmp(indigo_radio_ifname_ap[vap_id], buf) == 0)
                break;
        }
        if (vap_id == MAX_RADIO)
            continue;

        /* Single VAP configuration */
        if (!dut->ap_is_dual)
            vap_id = vap_count;

        /*Dual Vap configuration */
        if (dut->ap_is_dual && vap_count == 0) {
            if(strcmp(buf,"wifi1") == 0)
                vap_id=vap_count+1;
            if(strcmp(buf,"wifi2") == 0)
                vap_id=vap_count+2;
        }
        if (dut->ap_is_dual && vap_count == 1) {
            if(strcmp(buf,"wifi0") == 0)
                vap_id=vap_count-1;
            if(strcmp(buf,"wifi2") == 0)
                vap_id=vap_count+1;
        }

        for (j = 0; j < MAX_WLAN_TAGS - 1; j++) {
            /*
             * We keep a separate array of ap_tag_ssid and
             * ap_tag_key_mgmt for tags starting from WLAN_TAG=2.
             * So j=0 => WLAN_TAG = 2
             */
            int wlan_tag = 0;
            int id;

            if (dut->ap_is_dual) {
                if(strcmp(indigo_radio_ifname_ap[0],"wifi2") == 0 ||
                   strcmp(indigo_radio_ifname_ap[1],"wifi2") == 0){
                    id = dual_vap_id + 3;
                } else {
                    id = dual_vap_id + 2;
                }
            } else {
                wlan_tag = j + 2;
                id = vap_count + (wlan_tag - 1);
            }

            if (wlan_tag == 2 && dut->program == PROGRAM_WPA3 &&
                (dut->ap_interface_5g || dut->ap_interface_2g)) {
                res = snprintf(
                               dut->ap_tag_ssid[wlan_tag - 2],
                               sizeof(dut->ap_tag_ssid[wlan_tag - 2]),
                               "%s-owe", dut->ap_ssid);
                if (res < 0 ||
                    res >= sizeof(dut->ap_tag_ssid[wlan_tag -
                                  2]))
                    dut->ap_tag_ssid[wlan_tag - 2][0] =
                        '\0';
            }

            if(!dut->ap_is_dual){
                if (!dut->ap_interface_5g && !dut->ap_interface_2g &&
                    dut->ap_tag_ssid[j][0] == '\0')
                    continue;
                if (dut->ap_interface_5g && dut->ap_tag_ssid[j][0] == '\0')
                    continue;
                if (dut->ap_interface_2g && !(!(dut->ap_tag_ssid[j][0] == '\0')
                                              || !(dut->ap_tag_ssid_1[j][0] == '\0')))
                    continue;
            } else {
                if (vap_count == 0) {
                    if(dut->ap_tag_ssid[j][0] == '\0')
                        continue;
                } else {
                    if(dut->ap_tag_ssid_1[j][0] == '\0')
                        continue;
                }
                dual_vap_id = dual_vap_id + 1;
            }
            if(dut->ap_is_dual) {
                strlcpy(buf, indigo_radio_ifname_ap[vap_count], sizeof(buf));
                owrt_ap_add_vap(id,
                                "device", buf);
            } else {
                snprintf(buf, sizeof(buf), "%s%d", "wifi", vap_count);
                owrt_ap_add_vap(id,
                                "device", buf);
            }

            /* SSID */
            if (dut->ap_is_dual) {
                if (vap_count == 0) {
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_tag_ssid[j]);
                    owrt_ap_set_vap(id,
                                    "ssid", buf);
                } else {
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_tag_ssid_1[j]);
                    owrt_ap_set_vap(id,
                                    "ssid", buf);
                }
            } else if (dut->ap_interface_2g){
                if (dut->ap_tag_ssid[j][0] == '\0'){
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_tag_ssid_1[j]);
                } else {
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_tag_ssid[j]);
                }
                owrt_ap_set_vap(id,
                                "ssid", buf);
            } else {
                snprintf(buf, sizeof(buf), "\"%s\"",
                         dut->ap_tag_ssid[j]);
                owrt_ap_set_vap(id,
                                "ssid", buf);
            }

            if (dut->ap_key_mgmt == AP_WPA2_OWE &&
                dut->ap_tag_ssid[0][0] &&
                dut->ap_tag_key_mgmt[0] == AP2_OPEN) {
                /* OWE transition mode */
                snprintf(buf, sizeof(buf), "%s", ifname);
                owrt_ap_set_vap(id,
                                "owe_transition_ifname", buf);
            }

            if (dut->ap_key_mgmt == AP_OPEN &&
                dut->ap_tag_key_mgmt[0] == AP2_WPA2_OWE) {
                /* OWE transition mode */
                snprintf(buf, sizeof(buf), "%s", ifname);
                owrt_ap_set_vap(id,
                                "owe_transition_ifname", buf);
                owrt_ap_set_vap(id,
                                "hidden", "1");
            }

            if (ap_ft_enabled(dut)) {
                char self_mac[ETH_ALEN];
                char mac_str[20];

                owrt_ap_set_vap(id,
                                "mobility_domain",
                                dut->ap_mobility_domain);
                owrt_ap_set_vap(id,
                                "ft_over_ds",
                                dut->ap_ft_ds == VALUE_ENABLED ?
                                "1" : "0");
                owrt_ap_set_vap(id,
                                "ieee80211r", "1");
                owrt_ap_set_vap(id,
                                "nasid", "nas1.example.com");
                if (get_mac_address(self_mac, sizeof(self_mac),
                            indigo_radio_ifname_ap[0]) < 0)
                    return -1;
                snprintf(mac_str, sizeof(mac_str),
                         "%02x:%02x:%02x:%02x:%02x:%02x",
                         self_mac[0], self_mac[1], self_mac[2],
                         self_mac[3], self_mac[4], self_mac[5]);
                owrt_ap_set_vap(id,
                                "ap_macaddr", mac_str);
                snprintf(mac_str, sizeof(mac_str),
                         "%02x%02x%02x%02x%02x%02x",
                         self_mac[0], self_mac[1], self_mac[2],
                         self_mac[3], self_mac[4], self_mac[5]);
                owrt_ap_set_vap(id,
                                "r1_key_holder", mac_str);
                owrt_ap_set_vap(id,
                                "ft_psk_generate_local", "1");
                owrt_ap_set_vap(id,
                                "pmk_r1_push", "0");
                owrt_ap_set_vap(id,
                                "kh_key_hex",
                                "000102030405060708090a0b0c0d0e0f");
                snprintf(mac_str, sizeof(mac_str),
                         "%02x:%02x:%02x:%02x:%02x:%02x",
                         dut->ft_bss_mac_list[0][0],
                         dut->ft_bss_mac_list[0][1],
                         dut->ft_bss_mac_list[0][2],
                         dut->ft_bss_mac_list[0][3],
                         dut->ft_bss_mac_list[0][4],
                         dut->ft_bss_mac_list[0][5]);
                owrt_ap_set_vap(id,
                                "ap2_macaddr", mac_str);
                owrt_ap_set_vap(id,
                                "ap2_r1_key_holder", mac_str);
                owrt_ap_set_vap(id,
                                "nasid2", "nas2.example.com");
            }

            if (dut->ap_tag_key_mgmt[j] == AP2_OSEN &&
                wlan_tag == 2) {
                /* Only supported for WLAN_TAG=2 */
                owrt_ap_set_vap(vap_count + 1, "osen",
                                "1");
                snprintf(buf, sizeof(buf), "wpa2");
                owrt_ap_set_vap(vap_count + 1,
                                "encryption", buf);
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap2_radius_ipaddr);
                owrt_ap_set_vap(vap_count + 1,
                                "auth_server", buf);
                snprintf(buf, sizeof(buf), "%d",
                         dut->ap2_radius_port);
                owrt_ap_set_vap(vap_count + 1,
                                "auth_port", buf);
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap2_radius_password);
                owrt_ap_set_vap(vap_count + 1,
                                "auth_secret", buf);
            } else if (dut->ap_tag_key_mgmt[j] == AP2_WPA2_PSK) {
                owrt_ap_set_vap(id,
                                "encryption", "psk2+ccmp");
                if (dut->ap_mbssid == VALUE_ENABLED) {
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_tag_passphrase[j]);
                } else {
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_passphrase);
                }
                owrt_ap_set_vap(id,
                                "key", buf);
                snprintf(buf, sizeof(buf), "%d", dut->ap_pmf);
                owrt_ap_set_vap(id,
                                "ieee80211w", buf);
            } else if (dut->ap_tag_key_mgmt[j] == AP2_WPA2_EAP) {
                owrt_ap_set_vap(id,
                                "encryption", "wpa2+ccmp");
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_ipaddr);
                owrt_ap_set_vap(id,
                                "auth_server", buf);
                snprintf(buf, sizeof(buf), "%d", dut->ap_radius_port);
                owrt_ap_set_vap(id,
                                "auth_port", buf);
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap_radius_password);
                owrt_ap_set_vap(id,
                                "auth_secret", buf);

                if (dut->program == PROGRAM_HS2_R3 && dut->ap_tnc_file_name) {
                    snprintf(buf, sizeof(buf), "%d", radius_das_port + radius_das_count);
                    radius_das_count++;
                    owrt_ap_set_vap(id,
                                    "radius_das_port", buf);
                    snprintf(buf, sizeof(buf), "'0.0.0.0 %s'",
                             dut->ap_radius_password);
                    owrt_ap_set_vap(id,
                                    "radius_das_client", buf);
                    owrt_ap_set_vap(id,
                                    "radius_das_require_event_timestamp", "1");
                }
            } else if (dut->ap_tag_key_mgmt[0] == AP2_WPA2_OWE) {
                owrt_ap_set_vap(id,
                                "owe", "1");
                snprintf(buf, sizeof(buf), "ccmp");
                owrt_ap_set_vap(id,
                                "encryption", buf);
                owrt_ap_set_vap(id,
                                "ieee80211w", "2");
                if (dut->ap_sae_groups) {
                    snprintf(buf, sizeof(buf), "\'%s\'",
                             dut->ap_sae_groups);
                    owrt_ap_set_list_vap(vap_count +
                                         (wlan_tag - 1),
                                         "owe_groups", buf);
                }
            } else if (dut->ap_tag_key_mgmt[j] == AP2_WPA2_EAP) {
                snprintf(buf, sizeof(buf), "wpa2");

                if (dut->ap_cipher == AP_CCMP_TKIP)
                    strlcat(buf, "+ccmp+tkip", sizeof(buf));
                else if (dut->ap_cipher == AP_TKIP)
                    strlcat(buf, "+tkip", sizeof(buf));
                else
                    strlcat(buf, "+ccmp", sizeof(buf));

                owrt_ap_set_vap(vap_count + (wlan_tag - 1),
                                "encryption", buf);
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_ipaddr);
                owrt_ap_set_vap(vap_count + (wlan_tag - 1),
                                "auth_server", buf);
                snprintf(buf, sizeof(buf), "%d", dut->ap_radius_port);
                owrt_ap_set_vap(vap_count + (wlan_tag - 1),
                                "auth_port", buf);
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_password);
                owrt_ap_set_vap(vap_count + (wlan_tag - 1),
                                "auth_secret", buf);
            } else if (dut->ap_tag_key_mgmt[j] == AP2_WPA2_SAE) {
                snprintf(buf, sizeof(buf), "ccmp");
                owrt_ap_set_vap(id, "encryption", buf);
                snprintf(buf, sizeof(buf), "\"%s\"",
                         dut->ap_passphrase);
                owrt_ap_set_vap(id, "key", buf);
                owrt_ap_set_list_vap(id, "sae_password",
                                     buf);
                owrt_ap_set_vap(id, "sae", "1");
            }
            if ((dut->program == PROGRAM_HS2_R2) ||
                (dut->program == PROGRAM_HS2_R3)) {
                if ((dut->ap_mode != AP_11ac) && (dut->ap_mode_1 != AP_11ac))
                    owrt_ap_set_vap(id,
                                    "vht_11ng", "0");
            }

            if (dut->ap_interface_6g) {
                owrt_ap_set_vap(id, "rrm", "1");
                owrt_ap_set_vap(id, "mbo", "1");
            }

            if (dut->ap_interface_6g && (vap_count == 0 || !dut->ap_is_dual)) {
                if (dut->sae_pwe != SAE_PWE_DEFAULT || dut->sae_h2e_default ||
                    dut->sae_h2e_only) {
                    const char *sae_pwe = NULL;

                    if (dut->sae_pwe == SAE_PWE_LOOP && sae_pw_id_used(dut))
                        sae_pwe = "3";
                    else if (dut->sae_pwe == SAE_PWE_LOOP)
                        sae_pwe = "0";
                    else if (dut->sae_pwe == SAE_PWE_H2E || dut->sae_h2e_only)
                        sae_pwe = "1";
                    else if (dut->sae_h2e_default)
                        sae_pwe = "2";
                    if (sae_pwe)
                        owrt_ap_set_vap(id, "sae_pwe", sae_pwe);
                }
            }
        }

        if (ap_ft_enabled(dut)) {
            char self_mac[ETH_ALEN];
            char mac_str[20];

            owrt_ap_set_vap(vap_count,
                            "mobility_domain",
                            dut->ap_mobility_domain);
            owrt_ap_set_vap(vap_count,
                            "ft_over_ds",
                            dut->ap_ft_ds == VALUE_ENABLED ?
                            "1" : "0");
            owrt_ap_set_vap(vap_count,
                            "ieee80211r", "1");
            owrt_ap_set_vap(vap_count,
                            "nasid", "nas1.example.com");
            get_mac_address(self_mac, sizeof(self_mac),
                    indigo_radio_ifname_ap[0]);
            snprintf(mac_str, sizeof(mac_str),
                     "%02x:%02x:%02x:%02x:%02x:%02x",
                     self_mac[0], self_mac[1], self_mac[2],
                     self_mac[3], self_mac[4], self_mac[5]);
            owrt_ap_set_vap(vap_count,
                            "ap_macaddr", mac_str);
            snprintf(mac_str, sizeof(mac_str),
                     "%02x%02x%02x%02x%02x%02x",
                     self_mac[0], self_mac[1], self_mac[2],
                     self_mac[3], self_mac[4], self_mac[5]);
            owrt_ap_set_vap(vap_count,
                            "r1_key_holder", mac_str);
            owrt_ap_set_vap(vap_count,
                            "ft_psk_generate_local", "1");
            owrt_ap_set_vap(vap_count,
                            "pmk_r1_push", "0");
            owrt_ap_set_vap(vap_count,
                            "kh_key_hex",
                            "000102030405060708090a0b0c0d0e0f");
            snprintf(mac_str, sizeof(mac_str),
                     "%02x:%02x:%02x:%02x:%02x:%02x",
                     dut->ft_bss_mac_list[0][0],
                     dut->ft_bss_mac_list[0][1],
                     dut->ft_bss_mac_list[0][2],
                     dut->ft_bss_mac_list[0][3],
                     dut->ft_bss_mac_list[0][4],
                     dut->ft_bss_mac_list[0][5]);
            owrt_ap_set_vap(vap_count,
                            "ap2_macaddr", mac_str);
            owrt_ap_set_vap(vap_count,
                            "ap2_r1_key_holder", mac_str);
            owrt_ap_set_vap(vap_count,
                            "nasid2", "nas2.example.com");
        }

        if (dut->ap_filsdscv == VALUE_DISABLED)
            owrt_ap_set_vap(vap_id, "fils_fd_period", "0");

        /* NAIRealm */
        if (dut->ap_nairealm_int == 1) {
            snprintf(buf, sizeof(buf), "\"%s\"", dut->ap_nairealm);
            owrt_ap_set_vap(vap_id, "fils_realm", buf);
            owrt_ap_set_vap(vap_id, "erp_domain", buf);
        }

        if (dut->ap_bss_info.identifier > 1 && dut->ap_has_owe_identifier) {
            snprintf(buf, sizeof(buf), "%s%d", "wifi", vap_id);
            vap_id = vap_id + (dut->ap_bss_info.identifier - 1);
            owrt_ap_add_vap(vap_id, "device", buf);
        }

        /* Device for each vap*/
        if (dut->ap_is_dual) {
            strlcpy(buf, indigo_radio_ifname_ap[vap_count], sizeof(buf));
            owrt_ap_add_vap(vap_id,
                            "device", buf);
        }
        /* SSID */
        snprintf(buf, sizeof(buf), "\"%s\"", dut->ap_ssid);
        owrt_ap_set_vap(vap_id, "ssid", buf);

        /* SSID - AP is functioning in Dual Band mode*/
        if (dut->ap_is_dual && vap_count == 1 && dut->ap_ssid_1[0] != '\0') {
            snprintf(buf, sizeof(buf), "\"%s\"", dut->ap_ssid_1);
            owrt_ap_set_vap(vap_id, "ssid", buf);
        }

        if (dut->ap_akm_values) {
            char keymgmt[256] = {0};
            char temp[256];
            struct {
                int akm;
                const char *str;
            } akms[] = {
                { AKM_WPA_EAP, "WPA-EAP" },
                { AKM_WPA_PSK, "WPA-PSK" },
                { AKM_FT_EAP, "FT-EAP" },
                { AKM_FT_PSK, "FT-PSK" },
                { AKM_EAP_SHA256, "WPA-EAP-SHA256" },
                { AKM_PSK_SHA256, "WPA-PSK-SHA256" },
                { AKM_SAE, "SAE" },
                { AKM_FT_SAE, "FT-SAE" },
                { AKM_SUITE_B, "WPA-EAP-SUITE-B-192" },
                { AKM_FT_SUITE_B, "FT-EAP-SHA384" },
                { AKM_FILS_SHA256, "FILS-SHA256" },
                { AKM_FILS_SHA384, "FILS-SHA384" },
                { AKM_FT_FILS_SHA256, "FT-FILS-SHA256" },
                { AKM_FT_FILS_SHA384, "FT-FILS-SHA384" },
            };
            int first = 1;
            unsigned int i;

            for (i = 0; i < ARRAY_SIZE(akms); i++) {
                if (dut->ap_akm_values & (1 << akms[i].akm)) {
                    snprintf(temp, sizeof(temp), "%s%s",first ? "" : " ",
                             akms[i].str);

                    if (first == 1)
                        strlcpy(keymgmt, temp, sizeof(keymgmt));
                    else
                        strlcat(keymgmt, temp, sizeof(keymgmt));

                    first = 0;
                }
            }
            snprintf(buf, sizeof(buf), "\'%s\'", keymgmt);
            owrt_ap_set_vap(vap_id, "wpa_key_mgmt", buf);

            if (dut->ap_akm_values & ((1 << AKM_WPA_PSK) |
                                      (1 << AKM_FT_PSK) |
                                      (1 << AKM_PSK_SHA256))) {
                snprintf(buf, sizeof(buf), "psk2");
            } else if (dut->ap_akm_values & ((1 << AKM_SAE) |
                                             (1 << AKM_FT_SAE))) {
                snprintf(buf, sizeof(buf), "ccmp");
            } else if (dut->ap_akm_values & ((1 << AKM_WPA_EAP) |
                                             (1 << AKM_FT_EAP) |
                                             (1 << AKM_EAP_SHA256) |
                                             (1 << AKM_FILS_SHA256) |
                                             (1 << AKM_FILS_SHA384) |
                                             (1 << AKM_FT_FILS_SHA256) |
                                             (1 << AKM_FT_FILS_SHA384))) {
                snprintf(buf, sizeof(buf), "wpa2");
            } else {
                snprintf(buf, sizeof(buf), "psk");
            }

            if (!(dut->ap_akm_values & ((1 << AKM_SAE) |
                                        (1 << AKM_FT_SAE)))) {
                if (dut->ap_cipher == AP_CCMP_TKIP)
                    strlcat(buf, "+ccmp+tkip", sizeof(buf));
                else if (dut->ap_cipher == AP_TKIP) {
                    if (dut->program == PROGRAM_HE && dut->device_type == AP_testbed)
                        strlcat(buf, "+tkip-pure", sizeof(buf));
                    else
                        strlcat(buf, "+tkip", sizeof(buf));
                } else
                    strlcat(buf, "+ccmp", sizeof(buf));
            }

            if ((dut->ap_akm_values &
                 ((1 << AKM_SAE) | (1 << AKM_FT_SAE))) &&
                (dut->ap_akm_values &
                 ((1 << AKM_WPA_PSK) | (1 << AKM_FT_PSK))))
                strlcat(buf, "+ccmp", sizeof(buf));

            owrt_ap_set_vap(vap_id, "encryption", buf);

            if ((dut->ap_akm_values &
                 ((1 << AKM_SAE) | (1 << AKM_FT_SAE))) &&
                !(dut->ap_akm_values &
                  ((1 << AKM_WPA_PSK) | (1 << AKM_FT_PSK))) &&
                dut->ap_passphrase[0]) {
                if (dut->ap_sae_pk && dut->ap_sae_pk_keypair_sig) {
                    snprintf(buf, sizeof(buf), "'%s|pk=%s:%s:%s'",
                             dut->ap_passphrase,
                             dut->ap_sae_pk_modifier,
                             dut->ap_sae_pk_keypair,
                             dut->ap_sae_pk_keypair_sig);
                    owrt_ap_set_list_vap(vap_count, "sae_password",
                                         buf);
                } else if (dut->ap_sae_pk) {
                    snprintf(buf, sizeof(buf), "'%s|pk=%s:%s'",
                             dut->ap_passphrase,
                             dut->ap_sae_pk_modifier,
                             dut->ap_sae_pk_keypair);
                    owrt_ap_set_list_vap(vap_id, "sae_password",
                                         buf);
                } else {
                    snprintf(buf, sizeof(buf), "%s",
                             dut->ap_passphrase);
                    owrt_ap_set_list_vap(vap_id, "sae_password",
                                         buf);
                }
            } else if ((dut->ap_akm_values &
                        ((1 << AKM_SAE) | (1 << AKM_FT_SAE))) &&
                       (dut->ap_akm_values &
                        ((1 << AKM_WPA_PSK) | (1 << AKM_FT_PSK))) &&
                       dut->ap_passphrase[0]) {
                if (dut->ap_sae_pk && dut->ap_sae_pk_keypair_sig) {
                    snprintf(buf, sizeof(buf), "'%s|pk=%s:%s:%s'",
                             dut->ap_passphrase,
                             dut->ap_sae_pk_modifier,
                             dut->ap_sae_pk_keypair,
                             dut->ap_sae_pk_keypair_sig);
                    owrt_ap_set_list_vap(vap_count, "sae_password",
                                         buf);
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_passphrase);
                    owrt_ap_set_vap(vap_id, "key", buf);
                } else if (dut->ap_sae_pk) {
                    snprintf(buf, sizeof(buf), "'%s|pk=%s:%s'",
                             dut->ap_passphrase,
                             dut->ap_sae_pk_modifier,
                             dut->ap_sae_pk_keypair);
                    owrt_ap_set_list_vap(vap_id, "sae_password",
                                         buf);
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_passphrase);
                    owrt_ap_set_vap(vap_id, "key", buf);
                } else {
                    snprintf(buf, sizeof(buf), "%s",
                             dut->ap_passphrase);
                    owrt_ap_set_list_vap(vap_id, "sae_password",
                                         buf);
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_passphrase);
                    owrt_ap_set_vap(vap_id, "key", buf);
                }
            } else if (!dut->ap_passphrase[0] && dut->ap_psk[0]) {
                snprintf(buf, sizeof(buf), "\"%s\"",
                         dut->ap_psk);
                owrt_ap_set_vap(vap_id, "key", buf);
            } else if (dut->ap_passphrase[0]) {
                snprintf(buf, sizeof(buf), "\"%s\"",
                         dut->ap_passphrase);
                owrt_ap_set_vap(vap_id, "key", buf);
            }

            if (dut->ap_akm_values & ((1 << AKM_SAE) |
                                      (1 << AKM_FT_SAE)))
                owrt_ap_set_vap(vap_id, "sae", "1");
            else
                owrt_ap_set_vap(vap_id, "sae", "0");

            if ((dut->ap_akm_values & ((1 << AKM_SAE) |
                                       (1 << AKM_FT_SAE))) &&
                (dut->ap_akm_values & ((1 << AKM_WPA_PSK) |
                                       (1 << AKM_FT_PSK)))) {
                dut->ap_add_sha256 = 1;
            }

            if (dut->ap_akm_values & ((1 << AKM_WPA_EAP) |
                                      (1 << AKM_FT_EAP) |
                                      (1 << AKM_EAP_SHA256) |
                                      (1 << AKM_FILS_SHA256) |
                                      (1 << AKM_FILS_SHA384) |
                                      (1 << AKM_FT_FILS_SHA256) |
                                      (1 << AKM_FT_FILS_SHA384))) {
                owrt_ap_set_vap(vap_id, "ieee8021x", "1");
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_ipaddr);
                owrt_ap_set_vap(vap_id, "auth_server", buf);
                snprintf(buf, sizeof(buf), "%d", dut->ap_radius_port);
                owrt_ap_set_vap(vap_id, "auth_port", buf);
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap_radius_password);
                owrt_ap_set_vap(vap_id, "auth_secret", buf);
            }

            if (dut->ap_akm_values & ((1 << AKM_SUITE_B) |
                                      (1 << AKM_FT_SUITE_B))) {
                owrt_ap_set_vap(vap_id, "suite_b", "192");
                snprintf(buf, sizeof(buf), "gcmp");
                owrt_ap_set_vap(vap_id, "encryption", buf);
                owrt_ap_set_vap(vap_id, "ieee8021x", "1");
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_ipaddr);
                owrt_ap_set_vap(vap_id, "auth_server", buf);
                snprintf(buf, sizeof(buf), "%d", dut->ap_radius_port);
                owrt_ap_set_vap(vap_id, "auth_port", buf);
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap_radius_password);
                owrt_ap_set_vap(vap_id, "auth_secret", buf);
                snprintf(buf, sizeof(buf), "%s",hostapd_group_mgmt_cipher_name(
                                                                               dut->ap_group_mgmt_cipher));
                owrt_ap_set_vap(vap_id, "group_mgmt_cipher",
                                buf);
            }
            if (dut->ap_akm_values & ((1 << AKM_EAP_SHA256) |
                                      (1 << AKM_PSK_SHA256) |
                                      (1 << AKM_FILS_SHA256) |
                                      (1 << AKM_FT_FILS_SHA256))) {
                dut->ap_add_sha256 = 1;
            }

            if (dut->ap_akm_values & ((1 << AKM_FILS_SHA384) |
                                      (1 << AKM_FT_FILS_SHA384))) {
                dut->ap_add_sha384 = 1;
            }
            goto skip_key_mgmt_owrt;
        }
        /* Encryption */
        switch (dut->ap_key_mgmt) {
            case AP_OPEN:
                if (dut->ap_cipher == AP_WEP) {
                    owrt_ap_set_vap(vap_id, "encryption",
                                    "wep-mixed");
                    owrt_ap_set_vap(vap_id, "key",
                                    dut->ap_wepkey);
                } else {
                    owrt_ap_set_vap(vap_id, "encryption",
                                    "none");
                }
                if (dut->ap_key_mgmt == AP_OPEN &&
                    dut->ap_has_owe_identifier) {
                    /* OWE transition mode */
                    if (dut->ap_has_owe_identifier > 1)
                        snprintf(ifname2, sizeof(ifname2), "%s%d",
                                ifname, (dut->ap_has_owe_identifier -1));
                    else
                        snprintf(ifname2, sizeof(ifname2), "%s", ifname);
                    owrt_ap_set_vap(vap_id,
                                    "owe_transition_ifname",
                                    ifname2);
                }
                break;
            case AP_WPA2_PSK:
            case AP_WPA2_PSK_MIXED:
            case AP_WPA_PSK:
            case AP_WPA2_SAE:
            case AP_WPA2_PSK_SAE:
            case AP_WPA2_PSK_SHA256:
            case AP_WPA2_FT_PSK:
                if (dut->ap_key_mgmt == AP_WPA2_PSK ||
                    dut->ap_key_mgmt == AP_WPA2_PSK_SAE ||
                    dut->ap_key_mgmt == AP_WPA2_PSK_SHA256 ||
                    dut->ap_key_mgmt == AP_WPA2_FT_PSK) {
                    snprintf(buf, sizeof(buf), "psk2");
                } else if (dut->ap_key_mgmt == AP_WPA2_PSK_MIXED) {
                    snprintf(buf, sizeof(buf), "psk-mixed");
                } else if (dut->ap_key_mgmt == AP_WPA2_SAE) {
                    snprintf(buf, sizeof(buf), "ccmp");
                } else {
                    snprintf(buf, sizeof(buf), "psk");
                }

                if (dut->ap_key_mgmt != AP_WPA2_SAE) {
                    if (dut->ap_cipher == AP_CCMP_TKIP)
                        strlcat(buf, "+ccmp+tkip", sizeof(buf));
                    else if (dut->ap_cipher == AP_TKIP) {
                        if (dut->program == PROGRAM_HE &&
                            dut->device_type == AP_testbed)
                            strlcat(buf, "+tkip-pure", sizeof(buf));
                        else
                            strlcat(buf, "+tkip", sizeof(buf));
                    } else if (dut->ap_cipher == AP_GCMP_128)
                        strlcat(buf, "+gcmp", sizeof(buf));
                    else
                        strlcat(buf, "+ccmp", sizeof(buf));
                }

                if (dut->ap_key_mgmt == AP_WPA_PSK) {
                    if (dut->ap_wpa_pairwise == AP_PAIRWISE_CCMP_TKIP)
                         strlcat(buf, "+tkip", sizeof(buf));
                    owrt_ap_set_vap(vap_id, "enable_tkip", "1");
                }

                owrt_ap_set_vap(vap_id, "encryption", buf);

                if (!dut->ap_passphrase[0] && dut->ap_psk[0]) {
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_psk);
                    owrt_ap_set_vap(vap_id, "key", buf);
                } else {
                    snprintf(buf, sizeof(buf), "\"%s\"",
                             dut->ap_passphrase);
                    owrt_ap_set_vap(vap_id, "key", buf);
                }

                if (dut->ap_key_mgmt == AP_WPA2_SAE ||
                    dut->ap_key_mgmt == AP_WPA2_PSK_SAE ||
                    (dut->ap_akm_values & (AKM_SAE | AKM_WPA_PSK)) ==
                    (AKM_SAE | AKM_WPA_PSK))
                    owrt_ap_set_vap(vap_id, "sae", "1");
                else
                    owrt_ap_set_vap(vap_id, "sae", "0");

                if (!dut->ap_akm_values) {
                    if (dut->ap_key_mgmt == AP_WPA2_SAE && dut->ap_sae_pk &&
                        dut->ap_sae_pk_keypair_sig) {
                        snprintf(buf, sizeof(buf), "'%s|pk=%s:%s:%s'",
                                 dut->ap_passphrase,
                                 dut->ap_sae_pk_modifier,
                                 dut->ap_sae_pk_keypair,
                                 dut->ap_sae_pk_keypair_sig);
                        owrt_ap_set_vap(vap_count, "sae_password",
                                        buf);
                    } else if (dut->ap_key_mgmt == AP_WPA2_SAE &&
                               dut->ap_sae_pk) {
                        snprintf(buf, sizeof(buf), "'%s|pk=%s:%s'",
                                 dut->ap_passphrase,
                                 dut->ap_sae_pk_modifier,
                                 dut->ap_sae_pk_keypair);
                        owrt_ap_set_list_vap(vap_id, "sae_password",
                                             buf);
                    } else if (dut->ap_key_mgmt == AP_WPA2_SAE) {
                        snprintf(buf, sizeof(buf), "%s",
                                 dut->ap_passphrase);
                        owrt_ap_set_list_vap(vap_id, "sae_password",
                                             buf);
                    } else {
                        snprintf(buf, sizeof(buf), "%s",
                                 dut->ap_passphrase);
                        owrt_ap_set_vap(vap_id,
                                        "wpa_passphrase", buf);
                    }
                }
                break;
            case AP_WPA2_EAP:
            case AP_WPA2_EAP_MIXED:
            case AP_WPA_EAP:
            case AP_WPA2_EAP_OSEN:
            case AP_WPA2_FT_EAP:
            case AP_WPA2_EAP_SHA256:
            case AP_WPA2_ENT_FT_EAP:
                if (dut->ap_key_mgmt == AP_WPA2_EAP || dut->ap_key_mgmt == AP_WPA2_EAP_OSEN ||
                    dut->ap_key_mgmt == AP_WPA2_FT_EAP || dut->ap_key_mgmt == AP_WPA2_EAP_SHA256
                    || dut->ap_key_mgmt == AP_WPA2_ENT_FT_EAP) {
                    snprintf(buf, sizeof(buf), "wpa2");
                } else if (dut->ap_key_mgmt == AP_WPA2_EAP_MIXED) {
                    snprintf(buf, sizeof(buf), "wpa-mixed");
                } else {
                    snprintf(buf, sizeof(buf), "wpa");
                }

                if (dut->ap_cipher == AP_CCMP_TKIP)
                    strlcat(buf, "+ccmp+tkip", sizeof(buf));
                else if (dut->ap_cipher == AP_TKIP)
                    strlcat(buf, "+tkip", sizeof(buf));
                else
                    strlcat(buf, "+ccmp", sizeof(buf));

                if (dut->ap_key_mgmt == AP_WPA2_EAP_OSEN)
                    strlcat(buf, "+osen", sizeof(buf));

                if (dut->ap_key_mgmt == AP_WPA_EAP) {
                    if (dut->ap_wpa_pairwise == AP_PAIRWISE_CCMP_TKIP)
                        strlcat(buf, "+tkip", sizeof(buf));
                }

                owrt_ap_set_vap(vap_id, "encryption", buf);
                owrt_ap_set_vap(vap_id, "ieee80211x", "1");
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_ipaddr);
                owrt_ap_set_vap(vap_id, "auth_server", buf);
                snprintf(buf, sizeof(buf), "%d", dut->ap_radius_port);
                owrt_ap_set_vap(vap_id, "auth_port", buf);
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap_radius_password);
                owrt_ap_set_vap(vap_id, "auth_secret", buf);
                if (dut->program == PROGRAM_HS2_R3 && dut->ap_tnc_file_name) {
                    snprintf(buf, sizeof(buf), "%d", radius_das_port);
                    owrt_ap_set_vap(vap_id, "radius_das_port", buf);
                    snprintf(buf, sizeof(buf), "'0.0.0.0 %s'",
                             dut->ap_radius_password);
                    owrt_ap_set_vap(vap_id, "radius_das_client", buf);
                    owrt_ap_set_vap(vap_id, "radius_das_require_event_timestamp", "1");
                }
                break;
            case AP_SUITEB:
                owrt_ap_set_vap(vap_id, "suite_b", "192");
                snprintf(buf, sizeof(buf), "gcmp");
                owrt_ap_set_vap(vap_id, "encryption", buf);
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_ipaddr);
                owrt_ap_set_vap(vap_id, "auth_server", buf);
                snprintf(buf, sizeof(buf), "%d", dut->ap_radius_port);
                owrt_ap_set_vap(vap_id, "auth_port", buf);
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap_radius_password);
                owrt_ap_set_vap(vap_id, "auth_secret", buf);
                snprintf(buf, sizeof(buf), "%s",
                         hostapd_group_mgmt_cipher_name(dut->ap_group_mgmt_cipher));
                owrt_ap_set_vap(vap_id, "group_mgmt_cipher",
                                buf);
                break;
            case AP_WPA2_OWE:
                owrt_ap_set_vap(vap_id, "owe", "1");
                snprintf(buf, sizeof(buf), "ccmp");
                owrt_ap_set_vap(vap_id, "encryption", buf);
                if (dut->ap_sae_groups) {
                    snprintf(buf, sizeof(buf), "\'%s\'",
                             dut->ap_sae_groups);
                    owrt_ap_set_list_vap(vap_id,
                                         "owe_groups", buf);
                }

                if (dut->ap_key_mgmt == AP_WPA2_OWE &&
                    dut->ap_has_owe &&
                    dut->ap_has_owe_identifier) {
                    /* OWE transition mode */
                    if (dut->ap_has_owe_identifier > 1)
                        snprintf(ifname2, sizeof(ifname2), "%s%d",
                                ifname, (dut->ap_has_owe_identifier - 1));
                    else
                        snprintf(ifname2, sizeof(ifname2), "%s",
                                ifname);
                    owrt_ap_set_vap(vap_id,
                                    "owe_transition_ifname",
                                    ifname2);
                    owrt_ap_set_vap(vap_id, "ignore_broadcast_ssid", "1");
                }
                break;
            case AP_OSEN:
                owrt_ap_set_vap(vap_id, "osen", "1");
                owrt_ap_set_vap(vap_id, "disable_dgaf", "1");
                owrt_ap_set_vap(vap_id, "encryption", "osen+ccmp");
                snprintf(buf, sizeof(buf), "%s", dut->ap_radius_ipaddr);
                owrt_ap_set_vap(vap_id, "auth_server", buf);
                if (dut->ap_radius_port) {
                    snprintf(buf, sizeof(buf), "%d", dut->ap_radius_port);
                    owrt_ap_set_vap(vap_id, "auth_port", buf);
                }
                snprintf(buf, sizeof(buf), "%s",
                         dut->ap_radius_password);
                owrt_ap_set_vap(vap_id, "auth_secret", buf);

                if (dut->ap_group_cipher != AP_NO_GROUP_CIPHER_SET) {
                    snprintf(buf, sizeof(buf), "%s", hostapd_cipher_name(dut->ap_group_cipher));
                    owrt_ap_set_vap(vap_id, "group_cipher", buf);
                }
                break;
        }

skip_key_mgmt_owrt:
        if (dut->program == PROGRAM_HE && dut->ap_is_dual && vap_count == 0) {
            if (dut->sae_pwe != SAE_PWE_DEFAULT || dut->sae_h2e_default || dut->sae_h2e_only) {
                const char *sae_pwe = NULL;

                if (dut->sae_pwe == SAE_PWE_LOOP && sae_pw_id_used(dut))
                    sae_pwe = "3";
                else if (dut->sae_pwe == SAE_PWE_LOOP)
                    sae_pwe = "0";
                else if (dut->sae_pwe == SAE_PWE_H2E || dut->sae_h2e_only)
                    sae_pwe = "1";
                else if (dut->sae_h2e_default)
                    sae_pwe = "2";
                if (sae_pwe)
                    owrt_ap_set_vap(vap_id, "sae_pwe", sae_pwe);
            }
        }

        if (dut->ap_interface_6g) {
            owrt_ap_set_vap(vap_id, "rrm", "1");
            owrt_ap_set_vap(vap_id, "mbo", "1");
        }

        /* PMF */
        snprintf(buf, sizeof(buf), "%d", dut->ap_pmf);
        owrt_ap_set_vap(vap_id, "ieee80211w", buf);

        /* SAE_REQUIRE_MFP */
        if (dut->ap_sae_require_mfp) {
            owrt_ap_set_list_vap(vap_id, "sae_groups", "'15 16 17 18 19 20 21'");
            snprintf(buf, sizeof(buf), "%d", dut->ap_sae_require_mfp);
            owrt_ap_set_vap(vap_id, "sae_require_mfp", buf);
        }

        /* Add SHA256 */
        snprintf(buf, sizeof(buf), "%d", dut->ap_add_sha256);
        owrt_ap_set_vap(vap_id, "add_sha256", buf);

        /* Add SHA384 for akmsuitetype 15 */
        if (dut->ap_akm == 1) {
            snprintf(buf, sizeof(buf), "%d", dut->ap_add_sha384);
            owrt_ap_set_vap(vap_id, "add_sha384", buf);
        }

        if (dut->ap_sae_passwords) {
            char *tmp, *pos, *end, *id;

            tmp = strdup(dut->ap_sae_passwords);
            if (!tmp)
                return -1;

            pos = tmp;
            while (*pos) {
                end = strchr(pos, ';');
                if (end)
                    *end = '\0';
                id = strchr(pos, ':');
                if (id)
                    *id++ = '\0';

                snprintf(buf, sizeof(buf), "\'%s%s%s\'", pos, id ? "|id=" : "", id ? id : "");
                owrt_ap_set_list_vap(vap_id, "sae_password", buf);

                if (!end)
                    break;

                pos = end + 1;

            }

            free(tmp);
        }

        /* Enable RSN preauthentication, if asked to */
        snprintf(buf, sizeof(buf), "%d", dut->ap_rsn_preauth);
        owrt_ap_set_vap(vap_id, "rsn_preauth", buf);

        if ((dut->program == PROGRAM_HS2_R2) ||
            (dut->program == PROGRAM_HS2_R3)) {
            if ((dut->ap_mode != AP_11ac) && (dut->ap_mode_1 != AP_11ac))
                owrt_ap_set_vap(vap_id, "vht_11ng", "0");
        }

        /* Interworking */
        if (dut->ap_interworking) {
            snprintf(buf, sizeof(buf), "%d", dut->ap_access_net_type);
            owrt_ap_set_vap(vap_id, "access_network_type", buf);
            snprintf(buf, sizeof(buf), "%d", dut->ap_internet);
            owrt_ap_set_vap(vap_id, "internet", buf);
            snprintf(buf, sizeof(buf), "%d", dut->ap_venue_group);
            owrt_ap_set_vap(vap_id, "venue_group", buf);
            snprintf(buf, sizeof(buf), "%d", dut->ap_venue_type);
            owrt_ap_set_vap(vap_id, "venue_type", buf);
            snprintf(buf, sizeof(buf), "%s", dut->ap_hessid);
            owrt_ap_set_vap(vap_id, "hessid", buf);

            if (dut->ap_gas_cb_delay > 0) {
                snprintf(buf, sizeof(buf), "%d", dut->ap_gas_cb_delay);
                owrt_ap_set_vap(vap_id, "gas_comeback_delay", buf);
            }

            if (dut->ap_roaming_cons[0]) {
                char *rcons, *temp_ptr;

                rcons = strdup(dut->ap_roaming_cons);
                if (rcons == NULL)
                    return -1;

                temp_ptr = strchr(rcons, ';');

                if (temp_ptr)
                    *temp_ptr++ = '\0';

                owrt_ap_set_list_vap(vap_id, "roaming_consortium",
                                     rcons);

                if (temp_ptr)
                    owrt_ap_set_list_vap(vap_id,
                                         "roaming_consortium",
                                         temp_ptr);

                free(rcons);
            }
        }

        /* Proxy-ARP */
        snprintf(buf, sizeof(buf), "%d", dut->ap_proxy_arp);
        owrt_ap_set_vap(vap_id, "proxyarp", buf);

        /* DGAF */
        snprintf(buf, sizeof(buf), "%d", dut->ap_dgaf_disable);
        /* parse to hostapd */
        owrt_ap_set_vap(vap_id, "disable_dgaf", buf);
        /* parse to wifi driver */
        owrt_ap_set_vap(vap_id, "dgaf_disable", buf);

        /* HCBSSLoad */
        if (dut->ap_bss_load) {
            unsigned int bssload = 0;

            if (dut->ap_bss_load == 1) {
                /* STA count: 1, CU: 50, AAC: 65535 */
                bssload = 0x0132ffff;
            } else if (dut->ap_bss_load == 2) {
                /* STA count: 1, CU: 200, AAC: 65535 */
                bssload = 0x01c8ffff;
            } else if (dut->ap_bss_load == 3) {
                /* STA count: 1, CU: 75, AAC: 65535 */
                bssload = 0x014bffff;
            }

            snprintf(buf, sizeof(buf), "%d", bssload);
            owrt_ap_set_vap(vap_id, "hcbssload", buf);
            if (dut->program != PROGRAM_HE)
                owrt_ap_set_vap(vap_id, "bss_load_update_period", "10");
        }

        /* L2TIF */
        if  (dut->ap_l2tif) {
            owrt_ap_set_vap(vap_id, "l2tif", "1");
            run_system_wrapper("uci set firewall.qcanssecm=exclude");
        }

        if (!dut->ap_is_dual)
            break;
    }

    if (dut->ap_is_dual)
        return 1;

    if (dut->ap_disable_protection == 1)
        owrt_ap_set_vap(vap_id, "enablertscts", "0");

    if (dut->ap_txBF) {
        owrt_ap_set_vap(vap_id, "vhtsubfee", "1");
        owrt_ap_set_vap(vap_id, "vhtsubfer", "1");
        if (dut->program == PROGRAM_HE) {
            owrt_ap_set_vap(vap_id, "he_subfer", "1");
            owrt_ap_set_vap(vap_id, "cwmenable", "0");
        }
    } else {
        owrt_ap_set_vap(vap_id, "vhtsubfee", "0");
        owrt_ap_set_vap(vap_id, "vhtsubfer", "0");
        if (dut->program == PROGRAM_HE)
            owrt_ap_set_vap(vap_id, "he_subfer", "0");
    }

    if (dut->ap_mu_txBF) {
        owrt_ap_set_vap(vap_id, "vhtmubfer", "1");
        if (dut->program == PROGRAM_HE) {
            owrt_ap_set_vap(vap_id, "he_mubfer", "1");
            owrt_ap_set_vap(vap_id, "he_mubfee", "1");
        }
    } else {
        owrt_ap_set_vap(vap_id, "vhtmubfer", "0");
        if (dut->program == PROGRAM_HE) {
            owrt_ap_set_vap(vap_id, "he_mubfer", "0");
            owrt_ap_set_vap(vap_id, "he_mubfee", "0");
        }
    }

    if (dut->ap_tx_stbc) {
        /* STBC and beamforming are mutually exclusive features */
        owrt_ap_set_vap(vap_id, "implicitbf", "0");
    }

    /* enable dfsmode */
    if (dut->program == PROGRAM_MBO) {
        owrt_ap_set_vap(vap_id, "doth", "1");
    } else {
        snprintf(buf, sizeof(buf), "%d", dut->ap_dfs_mode);
        owrt_ap_set_vap(vap_id, "doth", buf);
    }

    if ((dut->program == PROGRAM_LOC || dut->program == PROGRAM_WPA3) && dut->ap_interworking) {
        char anqpval[1024];

        owrt_ap_set_vap(vap_id, "interworking", "1");

        if (dut->ap_lci == 1 && strlen(dut->ap_tag_ssid[0]) == 0) {
            snprintf(anqpval, sizeof(anqpval),
                     "'265:0010%s%s060101'",
                     dut->ap_val_lci, dut->ap_infoz);
            owrt_ap_set_list_vap(vap_id, "anqp_elem", anqpval);
        }

        if (dut->ap_lcr == 1) {
            snprintf(anqpval, sizeof(anqpval),
                     "'266:0000b2555302ae%s'",
                     dut->ap_val_lcr);
            owrt_ap_set_list_vap(vap_id, "anqp_elem", anqpval);
        }

        if (dut->ap_fqdn_held == 1 && dut->ap_fqdn_supl == 1)
            owrt_ap_set_list_vap(vap_id, "anqp_elem",
                                 "'267:00110168656c642e6578616d706c652e636f6d0011027375706c2e6578616d706c652e636f6d'");
    }

    if (dut->program == PROGRAM_MBO || dut->program == PROGRAM_DE) {
        owrt_ap_set_vap(vap_id, "interworking", "1");
        owrt_ap_set_vap(vap_id, "mbo", "1");
        owrt_ap_set_vap(vap_id, "rrm", "1");
        owrt_ap_set_vap(vap_id, "mbo_cell_data_conn_pref", "1");

        owrt_ap_set_list_vap(vap_id, "anqp_elem",
                             "'272:34108cfdf0020df1f7000000733000030101'");
        if (dut->ap_gas_cb_delay) {
            snprintf(buf, sizeof(buf), "%d", dut->ap_gas_cb_delay);
            owrt_ap_set_vap(vap_id, "gas_comeback_delay", buf);
        }
    }

    if (ap_ft_enabled(dut)) {
        char *mac[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
        char *mac_ss;
        int i = 0;
        char *macss_val;
        char *saveptr;
        char mac_str[50];
        char buff[BUZZ_SIZE];

        owrt_ap_set_vap(vap_id, "ft_over_ds",
                        dut->ap_ft_ds == VALUE_ENABLED ? "1" : "0");
        owrt_ap_set_vap(vap_id, "ieee80211r", "1");
        snprintf(buf, sizeof(buf),
                 "uci show wireless.%s.macaddr | grep : | cut -b 25-41 > /tmp/macaddr",
                 indigo_radio_ifname_ap[0]);
        if (system(buf) != 0)
            printf("Retrieve HWaddr failed");
        snprintf(buf, sizeof(buf), "/tmp/macaddr");
        FILE *fp = fopen(buf, "r");
        if (!fp) {
            printf("Failed to open %s", buf);
            return -1;
        }
        fgets(buff, BUZZ_SIZE, fp);
        fclose(fp);

        owrt_ap_set_vap(vap_id, "ap_macaddr", buff);
        mac_ss = strdup(buff);
        if (!mac_ss)
            return -1;

        macss_val = strtok_r(mac_ss, ":", &saveptr);
        while (macss_val != NULL)
        {
            mac[i++] = macss_val;
            macss_val = strtok_r(NULL, ":", &saveptr);
        }
        snprintf(mac_str, sizeof(mac_str),
                 "%s%s%s%s%s%s",
                 mac[0], mac[1], mac[2],
                 mac[3], mac[4], mac[5]);
        owrt_ap_set_vap(vap_id, "r1_key_holder", mac_str);
        owrt_ap_set_vap(vap_id, "ft_psk_generate_local", "1");
        owrt_ap_set_vap(vap_id, "pmk_r1_push", "0");
        owrt_ap_set_vap(vap_id, "kh_key_hex",
                        "000102030405060708090a0b0c0d0e0f");
        snprintf(mac_str, sizeof(mac_str),
                 "%02x:%02x:%02x:%02x:%02x:%02x",
                 dut->ft_bss_mac_list[0][0],
                 dut->ft_bss_mac_list[0][1],
                 dut->ft_bss_mac_list[0][2],
                 dut->ft_bss_mac_list[0][3],
                 dut->ft_bss_mac_list[0][4],
                 dut->ft_bss_mac_list[0][5]);
        owrt_ap_set_vap(vap_id, "ap2_macaddr", mac_str);
        owrt_ap_set_vap(vap_id, "mobility_domain",
                        dut->ap_mobility_domain);
        owrt_ap_set_vap(vap_id, "ap2_r1_key_holder", mac_str);
        owrt_ap_set_vap(vap_id, "nasid2", "nas2.example.com");
        owrt_ap_set_vap(vap_id, "nasid", "nas1.example.com");
    }

    if ((ap_ft_enabled(dut) && dut->ap_name == 0) ||
        (ap_ft_enabled(dut) && dut->ap_name == 2)) {
        owrt_ap_set_vap(vap_id, "nasid2", "nas2.example.com");
        owrt_ap_set_vap(vap_id, "nasid", "nas1.example.com");
    }

    if (ap_ft_enabled(dut) && dut->ap_name == 1) {
        owrt_ap_set_vap(vap_id, "nasid2", "nas1.example.com");
        owrt_ap_set_vap(vap_id, "nasid", "nas2.example.com");
    }

    if (dut->ap_broadcast_ssid == VALUE_DISABLED)
        owrt_ap_set_vap(vap_id, "hidden", "1");

    /* Enable/disable PMKSA caching, if asked to */
    if (dut->ap_pmksa == 1) {
        snprintf(buf, sizeof(buf), "%d", dut->ap_pmksa_caching);
        owrt_ap_set_vap(vap_id, "disable_pmksa_caching", buf);
    }

    if (dut->rsne_override) {
        snprintf(buf, sizeof(buf), "%s", dut->rsne_override);
        owrt_ap_set_vap(vap_count, "own_ie_override", buf);
    }

    if (dut->rsnxe_override_eapol)
        owrt_ap_set_vap(vap_count, "rsnxe_override_eapol",
                        dut->rsnxe_override_eapol);

    if (dut->sae_commit_override) {
        snprintf(buf, sizeof(buf), "%s", dut->sae_commit_override);
        owrt_ap_set_vap(vap_count, "sae_commit_override", buf);
    }

    if (dut->sae_pwe != SAE_PWE_DEFAULT || dut->sae_h2e_default || dut->sae_h2e_only) {
        const char *sae_pwe = NULL;

        if (dut->sae_pwe == SAE_PWE_LOOP && sae_pw_id_used(dut))
            sae_pwe = "3";
        else if (dut->sae_pwe == SAE_PWE_LOOP)
            sae_pwe = "0";
        else if (dut->sae_pwe == SAE_PWE_H2E || dut->sae_h2e_only)
            sae_pwe = "1";
        else if (dut->sae_h2e_default)
            sae_pwe = "2";
        if (sae_pwe)
            owrt_ap_set_vap(vap_count, "sae_pwe", sae_pwe);
    }

    if (dut->sae_anti_clogging_threshold > 0) {
        snprintf(buf, sizeof(buf), "%d",
                 dut->sae_anti_clogging_threshold);
        owrt_ap_set_vap(vap_count, "sae_anti_clogging_threshold",
                        buf);
    }

    if (dut->ap_sae_commit_status > 0) {
        snprintf(buf, sizeof(buf), "%d", dut->ap_sae_commit_status);
        owrt_ap_set_vap(vap_count, "sae_commit_status", buf);
    }

    if (dut->ap_sae_pk_omit)
        owrt_ap_set_vap(vap_count, "sae_pk_omit", "1");

    if (dut->sae_reflection)
        owrt_ap_set_vap(vap_count, "sae_reflection_attack", "1");

    if (dut->sae_confirm_immediate)
        owrt_ap_set_vap(vap_count, "sae_confirm_immediate", "2");

    if (dut->ap_he_dlofdma == VALUE_ENABLED && dut->ap_he_ppdu == PPDU_MU) {
        dut->ap_txBF = 0;
        dut->ap_mu_txBF = 0;
        owrt_ap_set_vap(vap_id, "vhtsubfer", "0");
        owrt_ap_set_vap(vap_id, "vhtsubfee", "0");
        owrt_ap_set_vap(vap_id, "he_subfer", "0");
    }

    if (dut->program == PROGRAM_HE &&
        (dut->ap_txBF || dut->ap_he_ulofdma == VALUE_ENABLED ||
         dut->ap_he_mimo == MIMO_DL)) {
        switch (dut->ap_chwidth) {
            case AP_20:
                owrt_ap_set_vap(vap_id, "chwidth", "0");
                break;
            case AP_40:
                owrt_ap_set_vap(vap_id, "chwidth", "1");
                break;
            case AP_80:
                owrt_ap_set_vap(vap_id, "chwidth", "2");
                break;
            case AP_160:
                owrt_ap_set_vap(vap_id, "chwidth", "3");
                break;
            case AP_80_80:
                owrt_ap_set_vap(vap_id, "chwidth", "3");
                break;
            case AP_AUTO:
            default:
                break;
        }
    }

    if (dut->program == PROGRAM_HS2_R2)
        owrt_ap_set_vap(vap_count, "hs20_release", "2");

    if (dut->program == PROGRAM_WPA3) {
        owrt_ap_set_vap(vap_id, "wpa_strict_rekey", "1");
        if ((dut->ap_mode != AP_11ac) && (dut->ap_mode_1 != AP_11ac))
            owrt_ap_set_vap(vap_id, "vht_11ng", "0");
        if (dut->device_type == AP_testbed)
            owrt_ap_set_vap(vap_id, "force_invalid_group", "1");
    }

    if (dut->ap_6g_legacy_security == VALUE_ENABLED)
        owrt_ap_set_vap(vap_id, "en_6g_sec_comp", "0");

    if (dut->program == PROGRAM_HE && dut->ap_reg_domain == REG_DOMAIN_GLOBAL) {
        owrt_ap_set_vap(vap_id, "rrm", "1");
        owrt_ap_set_vap(vap_id, "mbo", "1");
    }

    if (dut->ap_beaconprotection)
        owrt_ap_set_vap(vap_count, "beacon_prot", "1");

    if (dut->ap_transition_disable) {
        snprintf(buf, sizeof(buf), "0x%02x",
                 dut->ap_transition_disable);
        owrt_ap_set_vap(vap_id, "transition_disable", buf);
    }

    if (dut->ap_ocvc == 1)
        owrt_ap_set_vap(vap_count, "ocv", "1");
    else if (dut->ap_ocvc == 0)
        owrt_ap_set_vap(vap_count, "ocv", "0");

    if (dut->ap_keyrotationint) {
        snprintf(buf, sizeof(buf), "%d", dut->ap_keyrotationint);
        owrt_ap_set_vap(vap_count, "wpa_group_rekey", buf);
    }

    if (dut->ap_sae_pk_password_check_skip == 1 && dut->ap_sae_pk &&
        dut->program == PROGRAM_WPA3) {
        owrt_ap_set_vap(vap_count, "sae_pk_password_check_skip", "1");
    }

    if (dut->ap_mscs == VALUE_ENABLED)
        owrt_ap_set_vap(vap_count, "mscs", "1");
    else if (dut->ap_mscs == VALUE_DISABLED)
        owrt_ap_set_vap(vap_count, "mscs", "0");

    if (dut->ap_dpp_conf_addr && dut->ap_dpp_conf_pkhash) {
        snprintf(buf, sizeof(buf), "'ipaddr=%s pkhash=%s'",
                 dut->ap_dpp_conf_addr, dut->ap_dpp_conf_pkhash);
        owrt_ap_set_vap(vap_count, "dpp_controller", buf);
    }

    return 1;
}

/* void (*callback_fn)(void *), callback of active interface iterator
 *
 * Called by start_ap_handler() after invoking hostapd
 */
void start_ap_set_ath_params(void *if_info) {
    char buffer[S_BUFFER_LEN];
    const char *ifname = NULL;

    memset(buffer, 0, sizeof(buffer));
    ifname = owrt_get_ap_wireless_interface();

    snprintf(buffer, sizeof(buffer), "cfg80211tool %s he_ul_ofdma 0", ifname);
    run_system(buffer);
    /* Avoid target assert during channel switch */
    snprintf(buffer, sizeof(buffer), "cfg80211tool %s he_ul_mimo 0", ifname);
    run_system(buffer);
    snprintf(buffer, sizeof(buffer), "cfg80211tool %s twt_responder 0", ifname);
    run_system(buffer);

    if (dut.ap_enable_11h) {
        snprintf(buffer, sizeof(buffer), "cfg80211tool %s vap_doth 1", ifname);
        run_system(buffer);
    }
}

int bring_up_ap_interface(void)
{
    int len = 0;

    len = owrt_ap_config_radio(&dut);
    if (len < 0)
        return -1;
    len = owrt_ap_config_vap(&dut);
    if (len < 0)
        return -1;
    run_system("uci commit");
    len = run_system("wifi load");
    iterate_all_wlan_interfaces(start_ap_set_ath_params);
    return len;
}

int generate_wireless_config(char *output, int output_size, struct packet_wrapper *wrapper,
        struct interface_info* wlanp, struct indigo_dut *dut)
{
    int has_sae = 0, has_wpa = 0, has_pmf = 0, has_owe = 0, has_transition = 0,
    has_transition_disable = 0;
    int chan = 0, chwidth = 1;
    int i, enable_11h = 0;
    char buffer[S_BUFFER_LEN], cfg_item[2*S_BUFFER_LEN];
    char band[64], value[16];
    char country[16];
    struct tlv_to_config_name* cfg = NULL;
    struct tlv_hdr *tlv = NULL;
    char *tlv_value = NULL;
    char ssid[TLV_VALUE_SIZE];
    struct bss_identifier_info bss_info;
    struct interface_info *wlan;

    memset(country, 0, sizeof(country));

    for (i = 0; i < wrapper->tlv_num; i++) {
        tlv = wrapper->tlv[i];

        if (tlv->id == TLV_HE_6G_ONLY) {
            continue;
        }

        if (tlv->id == TLV_INTERFACE_NAME) {
            /* Do Nothing */
        }

        if (tlv->id == TLV_BSS_IDENTIFIER) {
            if (is_band_enabled(BAND_6GHZ) && !wlanp->mbssid_enable) {
                strlcat(output, "rnr=1\n", sizeof(output));
            }
            continue;
        }
        cfg = find_tlv_config(tlv->id);
        if (!cfg) {
            indigo_logger(LOG_LEVEL_ERROR, "Unknown AP configuration name: TLV ID 0x%04x", tlv->id);
            continue;
        }

        tlv_value = (char *)tlv->value;
        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv_value, "SAE") && strstr(tlv_value, "WPA-PSK")) {
            has_transition = 1;
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv_value, "OWE")) {
            has_owe = 1;
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv_value, "SAE")) {
            if (!has_transition)
                has_sae = 1;
        }

        if (tlv->id == TLV_WPA && strstr(tlv_value, "2")) {
            has_wpa = 1;
        }

        if (tlv->id == TLV_TRANSITION_DISABLE && strstr(tlv_value, "1")) {
            has_transition_disable = 1;
        }

        if (tlv->id == TLV_SAE_PWE) {
            if (strstr(tlv_value, "1")) {
                dut->sae_h2e_only = 1;
            } else if (strstr(tlv_value, "2")) {
                dut->sae_h2e_default = 1;
            }
        }
        if (tlv->id == TLV_IEEE80211_W) {
            has_pmf = 1;
        }
        if (tlv->id == TLV_WMM_ENABLED) {
            if (strstr(tlv_value, "1")) {
                dut->ap_wme = 1;
            }
        }

        if (tlv->id == TLV_WEP_KEY0) {
            dut->ap_key_mgmt = AP_OPEN;
            dut->ap_cipher = AP_WEP;
            memcpy(dut->ap_wepkey, tlv_value, tlv->len);
        }

        if (tlv->id == TLV_WPA_PAIRWISE) {
            if (strstr(tlv_value, "CCMP TKIP")) {
                dut->ap_wpa_pairwise = AP_PAIRWISE_CCMP_TKIP;
            }
        }

        if (tlv->id == TLV_RSN_PAIRWISE) {
            if (strstr(tlv_value, "CCMP")) {
                dut->ap_cipher = AP_CCMP;
            } else if (strstr(tlv_value, "TKIP")) {
                dut->ap_cipher = AP_TKIP;
            } else if (strstr(tlv_value, "WEP")) {
                dut->ap_cipher = AP_WEP;
            } else if (strstr(tlv_value, "PLAIN")) {
                dut->ap_cipher = AP_PLAIN;
            } else if (strstr(tlv_value, "CCMP TKIP")) {
                dut->ap_cipher = AP_CCMP_TKIP;
            } else if (strstr(tlv_value, "GCMP_256")) {
                dut->ap_cipher = AP_GCMP_256;
            } else if (strstr(tlv_value, "GCMP_128")) {
                dut->ap_cipher = AP_GCMP_128;
            } else if (strstr(tlv_value, "CCMP_256")) {
                dut->ap_cipher = AP_CCMP_256;
            } else if (strstr(tlv_value, "CCMP_128_GCMP_256")) {
                dut->ap_cipher = AP_CCMP_128_GCMP_256;
            }
        }

        if (tlv->id == TLV_WPA_KEY_MGMT) {
            if (tlv_value) {
                if (dut->ap_bss_info.identifier > 1 && !dut->ap_has_owe_identifier) {
                    if (strncasecmp(tlv_value, "NONE", tlv->len) == 0) {
                        dut->ap_tag_key_mgmt[dut->ap_bss_info.identifier - 2] = AP2_OPEN;
                    } else if (strncasecmp(tlv_value, "OSEN", tlv->len) == 0 &&
                        dut->ap_bss_info.identifier == 2) {
                        /*
                        * OSEN only supported on WLAN_TAG = 2 for now
                        */
                        dut->ap_tag_key_mgmt[dut->ap_bss_info.identifier - 2] = AP2_OSEN;
                    } else if (strncasecmp(tlv_value, "WPA2-PSK", tlv->len) == 0) {
                        dut->ap_tag_key_mgmt[dut->ap_bss_info.identifier - 2] = AP2_WPA2_PSK;
                    } else if (strncasecmp(tlv_value, "WPA-PSK", tlv->len) == 0) {
                        dut->ap_tag_key_mgmt[dut->ap_bss_info.identifier - 2] = AP_WPA_PSK;
                        dut->ap_cipher = AP_TKIP;
                    } else if (strncasecmp(tlv_value, "WPA2-Ent", tlv->len) == 0 &&
                               dut->ap_bss_info.identifier == 2) {
                        dut->ap_tag_key_mgmt[dut->ap_bss_info.identifier - 2] = AP2_WPA2_EAP;
                    } else if (strncasecmp(tlv_value, "OWE", tlv->len) == 0 &&
                               dut->ap_bss_info.identifier == 2) {
                        dut->ap_tag_key_mgmt[dut->ap_bss_info.identifier - 2] = AP2_WPA2_OWE;
                    } else if (strncasecmp(tlv_value, "SAE", tlv->len) == 0) {
                        dut->ap_tag_key_mgmt[dut->ap_bss_info.identifier - 2] = AP2_WPA2_SAE;
                    }
                } else {
                if (strncasecmp(tlv_value, "WPA2-PSK", tlv->len) == 0 ||
                    strncasecmp(tlv_value, "WPA2", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_PSK;
                    dut->ap_cipher = AP_CCMP;
                } else if (strncasecmp(tlv_value, "WPA2-EAP", tlv->len) == 0 ||
                           strncasecmp(tlv_value, "WPA2-Ent", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_EAP;
                    dut->ap_cipher = AP_CCMP;
                } else if (strncasecmp(tlv_value, "SuiteB", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_SUITEB;
                    dut->ap_cipher = AP_GCMP_256;
                    dut->ap_pmf = AP_PMF_REQUIRED;
                } else if (strncasecmp(tlv_value, "WPA-PSK", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA_PSK;
                    dut->ap_cipher = AP_TKIP;
                } else if (strncasecmp(tlv_value, "WPA-EAP", tlv->len) == 0 ||
                           strncasecmp(tlv_value, "WPA-Ent", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA_EAP;
                    dut->ap_cipher = AP_TKIP;
                } else if (strncasecmp(tlv_value, "WPA2-Mixed", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_EAP_MIXED;
                    dut->ap_cipher = AP_CCMP_TKIP;
                } else if (strncasecmp(tlv_value, "WPA2-PSK-Mixed", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_PSK_MIXED;
                    dut->ap_cipher = AP_CCMP_TKIP;
                } else if (strncasecmp(tlv_value, "WPA2-SAE", tlv->len) == 0 ||
                           strncasecmp(tlv_value, "SAE", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_SAE;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_REQUIRED;
                } else if (strncasecmp(tlv_value, "WPA2-PSK-SAE", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_PSK_SAE;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "OWE", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_OWE;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_REQUIRED;
                } else if (strncasecmp(tlv_value, "WPA2-ENT-OSEN", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_EAP_OSEN;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "OSEN", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_OSEN;
                    dut->ap_cipher = AP_CCMP;
                } else if (strncasecmp(tlv_value, "FT-EAP", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_FT_EAP;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "FT-PSK", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_FT_PSK;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "WPA2-ENT-256", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_EAP_SHA256;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "WPA-PSK WPA-PSK-SHA256", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_PSK_SHA256;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_add_sha256 = 1;
                } else if (strncasecmp(tlv_value, "SAE WPA-PSK", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_PSK_SAE;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "WPA2-PSK-256", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_PSK_SHA256;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "WPA2-ENT-FT-EAP", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_WPA2_ENT_FT_EAP;
                    dut->ap_cipher = AP_CCMP;
                    dut->ap_pmf = AP_PMF_OPTIONAL;
                } else if (strncasecmp(tlv_value, "NONE", tlv->len) == 0) {
                    dut->ap_key_mgmt = AP_OPEN;
                    dut->ap_cipher = AP_PLAIN;
                }
            }
          }
        }

        if (tlv->id == TLV_WPA_PASSPHRASE) {
            memset(dut->ap_passphrase, 0 , sizeof(dut->ap_passphrase));
            if (tlv->len < 63)
                memcpy(dut->ap_passphrase, tlv->value, tlv->len);
        }

        if (tlv->id == TLV_AUTH_SERVER_ADDR) {
            memset(dut->ap_radius_ipaddr, 0 , sizeof(dut->ap_radius_ipaddr));
            memcpy(dut->ap_radius_ipaddr, tlv->value, tlv->len);
        }

        if (tlv->id == TLV_AUTH_SERVER_PORT) {
            dut->ap_radius_port = atoi(tlv_value);
        }

        if (tlv->id == TLV_AUTH_SERVER_SHARED_SECRET) {
           memset(dut->ap_radius_password, 0, sizeof(dut->ap_radius_password));
           memcpy(dut->ap_radius_password, tlv->value, tlv->len);
        }

        if (tlv->id == TLV_OWE_GROUPS) {
            memset(dut->ap_sae_groups, 0, sizeof(dut->ap_sae_groups));
            memcpy(dut->ap_sae_groups, tlv->value, tlv->len);
        }

        if (tlv->id == TLV_IEEE80211_W && strstr(tlv_value, "1")) {
            has_pmf = 1;
        }

        if (tlv->id == TLV_INTERWORKING) {
            dut->ap_interworking = atoi(tlv_value);
        }

        if (tlv->id == TLV_GAS_COMEBACK_DELAY) {
            dut->ap_gas_cb_delay = atoi(tlv_value);
        }

        if (tlv->id == TLV_MBO) {
            dut->program = PROGRAM_MBO;
        }

        if (tlv->id == TLV_HW_MODE) {
            memset(band, 0, sizeof(band));
            memcpy(band, tlv->value, tlv->len);
            if (!strncmp(band, "a", 1)) {
                dut->ap_mode = AP_11a;
            } else if (!strncmp(band, "b", 1)) {
                dut->ap_mode = AP_11b;
            } else if (!strncmp(band, "g", 1)) {
                dut->ap_mode = AP_11g;
            } else if (!strncmp(band, "na", 2)) {
                dut->ap_mode = AP_11na;
            } else if (!strncmp(band, "ac", 2)) {
                dut->ap_mode = AP_11ac;
            } else {
                dut->ap_mode = AP_11ng;
            }
        }

        if (tlv->id == TLV_SSID) {
            memset(ssid,0, sizeof(ssid));
            if(dut->ap_is_dual) {
               if (dut->ap_bss_info.band == 0) {
                   memcpy(dut->ap_ssid_1, tlv->value, tlv->len);
               } else if (dut->ap_bss_info.band == 1)  {
                   memcpy(dut->ap_ssid_1,dut->ap_ssid,sizeof(dut->ap_ssid_1));
                   memcpy(dut->ap_ssid,tlv->value, tlv->len);
               } else {
                   memcpy(dut->ap_ssid_1,dut->ap_ssid,sizeof(dut->ap_ssid_1));
                   memcpy(dut->ap_ssid,tlv->value, tlv->len);
               }
            } else {
               if (dut->ap_bss_info.identifier > 1 && !dut->ap_has_owe_identifier) {
                   memcpy(dut->ap_tag_ssid[dut->ap_bss_info.identifier - 2], tlv->value, tlv->len);
               } else {
                   memcpy(ssid, tlv->value, tlv->len);
                   strlcpy(dut->ap_ssid, ssid, sizeof(dut->ap_ssid));
               }
            }
        }

        if (tlv->id == TLV_CHANNEL) {
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            chan = atoi(value);
            if (dut->ap_is_dual == 1) {
                int temp;
                if (num_radio == 3) {
                    if (!dut->ap_interface_6g) {
                        if (chan <= 64 && chan > 11) {
                            indigo_radio_ifname_ap[0] = indigo_radio_ifname[1];
                        }
                        if (chan > 64){
                            indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                        }
                        indigo_radio_ifname_ap[1] = indigo_radio_ifname[2];
                    } else {
                        /* WAR: for now it will work only for 53 and 37. Make a list of 6Ghz channels for proper fix */
                        if (chan == 53 || chan == 37)
                            indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                        if (chan <= 64 && chan > 11 && chan != 53 && chan != 37)
                            indigo_radio_ifname_ap[1] = indigo_radio_ifname[1];
                        else
                            indigo_radio_ifname_ap[1] = indigo_radio_ifname[2];
                    }
                } else {
                    indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                    indigo_radio_ifname_ap[1] = indigo_radio_ifname[1];
                }
                if (dut->ap_channel <= 11) {
                    temp = dut->ap_channel;
                    dut->ap_channel_1 = temp;
                    dut->ap_channel = atoi(value);
                }
                temp = atoi(value);
                if (temp <= 11) {
                    dut->ap_channel_1 = atoi(value);
                }
                if (dut->ap_interface_6g && (temp <= 64 && temp > 11 && temp != 53 && temp != 37))
                    dut->ap_channel_1 = atoi(value);
            } else {
                dut->ap_channel = chan;
                if (dut->ap_channel <= 11) {
                    if (num_radio == 3)
                        indigo_radio_ifname_ap[0] = indigo_radio_ifname[2];
                    else
                        indigo_radio_ifname_ap[0] = indigo_radio_ifname[1];
                    indigo_radio_ifname_ap[1]= NULL;
                    indigo_radio_ifname_ap[2]= NULL;
                } else if(dut->ap_channel <= 64) {
                    if (num_radio == 3) {
                        if (dut->ap_channel == 53 || dut->ap_channel == 37)
                            indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                        else if (dut->ap_channel != 53 && dut->ap_channel != 37)
                            indigo_radio_ifname_ap[0] = indigo_radio_ifname[1];
                    } else {
                        indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                    }
                    indigo_radio_ifname_ap[1]= NULL;
                    indigo_radio_ifname_ap[2]= NULL;
                } else if(dut->ap_channel > 64) {
                    if (num_radio == 3 && ap_interface_5gh) {
                        int band;
                        band = BAND_5GHZH;
                        bss_info.band = band;
                        bss_info.identifier = 1;
                        wlan = assign_wireless_interface_info(&bss_info);
                        indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                    } else if (num_radio == 3) {
                        indigo_radio_ifname_ap[0] = indigo_radio_ifname[1];
                    } else {
                        indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                    }
                    indigo_radio_ifname_ap[1]= NULL;
                    indigo_radio_ifname_ap[2]= NULL;
                } else {
                    indigo_radio_ifname_ap[0] = indigo_radio_ifname[0];
                    indigo_radio_ifname_ap[1]= NULL;
                }
            }
        }

        if (tlv->id == TLV_HE_OPER_CHWIDTH) {
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            chwidth = atoi(value);
        }

        if (tlv->id == TLV_VHT_OPER_CHWIDTH) {
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            chwidth = atoi(value);
        }

        if (!strncmp(band, "a", 1)) {
            if (chan == 165) { // Force 20M for CH 165
               dut->ap_chwidth = AP_20;
            } else if (chwidth == 2) { // 160M test cases only
                dut->ap_chwidth = AP_160;
            } else if (chwidth == 0) { // 11N only
                dut->ap_chwidth = AP_40;
            } else { // 11AC or 11AX
                dut->ap_chwidth = AP_20;
            }
        }

        if (tlv->id == TLV_IEEE80211_AC && strstr(tlv_value, "1")) {
            dut->ap_mode = AP_11ac;
        }

        if (tlv->id == TLV_IEEE80211_AX && strstr(tlv_value, "1")) {
           dut->ap_mode = AP_11ax;
           dut->enable_11ax = 1;
        }

        if (tlv->id == TLV_COUNTRY_CODE)
            memcpy(dut->ap_countrycode, tlv->value, tlv->len);

        if (tlv->id == TLV_IEEE80211_H)
            enable_11h = 1;

        if (tlv->id == TLV_IEEE80211_D || tlv->id == TLV_HE_OPER_CENTR_FREQ)
            continue;

   }

        memset(buffer, 0, sizeof(buffer));
        memset(cfg_item, 0, sizeof(cfg_item));
        if (tlv->id == TLV_OWE_TRANSITION_BSS_IDENTIFIER) {
            int bss_identifier;
            char bss_identifier_str[8];
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
                if (has_owe)
                    dut->ap_has_owe = 1;
                dut->ap_has_owe_identifier = bss_info.identifier;
                owrt_ap_config_vap(dut);
                run_system("uci commit wireless");
            }
        }

    if (has_pmf == 0) {
        if (has_transition) {
            dut->ap_pmf = AP_PMF_OPTIONAL;
        } else if (has_sae && has_wpa) {
            dut->ap_pmf = AP_PMF_REQUIRED;
        } else if (has_owe) {
            dut->ap_pmf = AP_PMF_REQUIRED;
        } else if (has_wpa) {
            strlcat(output, "ieee80211w=1\n", sizeof(output));
	    dut->ap_pmf = AP_PMF_OPTIONAL;
        }
    }

    if (has_sae == 1) {
        dut->ap_sae_require_mfp = 1;
    }

    if (has_transition_disable == 1) {
        dut->ap_transition_disable = 1;
    }

    /* Make sure AP include power constranit element even in non DFS channel */
    if (enable_11h) {
        dut->ap_dfs_mode = AP_DFS_MODE_ENABLED;
        dut->ap_enable_11h = 1;
    }

    /* vendor specific config, not via hostapd */
  //  configure_ap_radio_params(band, country, channel, chwidth);

    return strlen(output);
}

void set_ap_params(struct packet_wrapper *req)
{
    struct tlv_hdr *tlv = NULL;
    int vap_id;
    char *ifname;
    char buffer[S_BUFFER_LEN];
    char *tlv_value;

    ifname = owrt_get_ap_wireless_interface();
    if (strcmp(ifname, "ath2") == 0) {
        vap_id = 2;
    } else if (strcmp(ifname, "ath1") == 0) {
        vap_id = 0;
    } else {
        vap_id = 0;
    }

    if (find_wrapper_tlv_by_id(req, TLV_MBO_ASSOC_DISALLOW)) {
        snprintf(buffer, sizeof(buffer), "cfg80211tool %s mbo_asoc_dis 1", ifname);
        run_system(buffer);

        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "cfg80211tool %s mbocap 1", ifname);
        run_system(buffer);
    } else if ((tlv = find_wrapper_tlv_by_id(req, TLV_GAS_COMEBACK_DELAY))) {
        tlv_value = (char *)tlv->value;
        dut.ap_gas_cb_delay = atoi(tlv_value);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d", dut.ap_gas_cb_delay);
        owrt_ap_set_vap(vap_id, "gas_comeback_delay", buffer);
        run_system("uci commit wireless");
        run_system("wifi");
        iterate_all_wlan_interfaces(start_ap_set_ath_params);
    }
}

static void inform_and_sleep(int seconds)
{
    indigo_logger(LOG_LEVEL_DEBUG, "sleeping for %d seconds", seconds);
    sleep(seconds);
    indigo_logger(LOG_LEVEL_DEBUG, "woke up after %d seconds", seconds);
}

void send_ap_btm_req(char *bssid)
{
    char request[4096];
    char *ifname;
    int cand_list = 0;

    ifname = owrt_get_ap_wireless_interface();
    memset(request, 0, sizeof(request));
    snprintf(request, sizeof(request), "wifitool %s sendbstmreq "
            "%s %d %d 15 %d %d %d %d",
            ifname, bssid,
            cand_list,
            dut.ap_disassoc_timer,
            dut.ap_btmreq_disassoc_imnt,
            dut.ap_btmreq_term_bit,
            dut.ap_btmreq_bss_term_tsf,
            dut.ap_btmreq_bss_term_dur);
    run_system(request);

    if (dut.ap_btmreq_term_bit) {
        if (dut.ap_btmreq_bss_term_tsf >= 3)
            inform_and_sleep(dut.ap_btmreq_bss_term_tsf - 3);
        memset(request, 0, sizeof(request));
        snprintf(request, sizeof(request), "cfg80211tool %s kickmac %s",
                ifname,  bssid);
        run_system(request);
        inform_and_sleep(1);
        memset(request, 0, sizeof(request));
        snprintf(request, sizeof(request), "ifconfig %s down", ifname);
        run_system(request);
        inform_and_sleep(2);
    } else if (dut.ap_btmreq_disassoc_imnt) {
        inform_and_sleep((dut.ap_disassoc_timer / 1000) + 1);
        memset(request, 0, sizeof(request));
        snprintf(request, sizeof(request), "cfg80211tool %s kickmac %s",
                ifname,  bssid);
        run_system(request);
    }
}
