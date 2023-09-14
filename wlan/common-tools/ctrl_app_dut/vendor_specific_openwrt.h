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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof((x)) / (sizeof(((x)[0]))))
#endif

#define IPV6_ADDR_LEN 16

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define MAX_PARAMS 100
#define MAX_RADIO 3

/* Set default operating channel width 80 MHz */
#define VHT_DEFAULT_OPER_CHWIDTH AP_80_VHT_OPER_CHWIDTH

typedef unsigned int u32;
typedef uint16_t u16;
typedef unsigned char u8;

struct indigo_cmd {
	char *params[MAX_PARAMS];
	char *values[MAX_PARAMS];
	int count;
};

enum indigo_ese_type {
	ESE_CBAP,
	ESE_SP,
};

struct indigo_ese_alloc {
	unsigned int percent_bi;
	enum indigo_ese_type type;
	unsigned int src_aid, dst_aid;
};

#define ESE_BCAST_AID  255
#define MAX_ESE_ALLOCS 4
#define NUM_AP_AC 4
#define AP_AC_BE 0
#define AP_AC_BK 1
#define AP_AC_VI 2
#define AP_AC_VO 3

#define MAX_WLAN_TAGS 4
#define MBO_MAX_PREF_BSSIDS 10
#define MAX_FT_BSS_LIST 10

#define TRANSPORT_PROTO_TYPE_TCP 0x06
#define TRANSPORT_PROTO_TYPE_UDP 0x11
#define NAN_TRANSPORT_PORT_DEFAULT 7000
#define NAN_TRANSPORT_PROTOCOL_DEFAULT TRANSPORT_PROTO_TYPE_TCP
#define P2P_GRP_ID_LEN 128
#define IP_ADDR_STR_LEN 16

enum value_not_set_enabled_disabled {
	VALUE_NOT_SET,
	VALUE_ENABLED,
	VALUE_DISABLED
};

enum sec_ch_offset {
	SEC_CH_NO,
	SEC_CH_40ABOVE,
	SEC_CH_40BELOW
};

struct mbo_pref_ap {
	int ap_ne_class;
	int ap_ne_op_ch;
	int ap_ne_pref;
	unsigned char mac_addr[ETH_ALEN];
};

enum akm_suite_values {
	AKM_WPA_EAP = 1,
	AKM_WPA_PSK = 2,
	AKM_FT_EAP = 3,
	AKM_FT_PSK = 4,
	AKM_EAP_SHA256 = 5,
	AKM_PSK_SHA256 = 6,
	AKM_SAE = 8,
	AKM_FT_SAE = 9,
	AKM_SUITE_B = 12,
	AKM_FT_SUITE_B = 13,
	AKM_FILS_SHA256 = 14,
	AKM_FILS_SHA384 = 15,
	AKM_FT_FILS_SHA256 = 16,
	AKM_FT_FILS_SHA384 = 17,

};

struct indigo_dut {
	const char *main_ifname;
	char *main_ifname_2g;
	char *main_ifname_5g;
	const char *station_ifname;
	char *station_ifname_2g;
	char *station_ifname_5g;
	char *p2p_ifname_buf;
	int use_5g;
	int sta_2g_started;
	int sta_5g_started;

	int s; /* server TCP socket */
	int debug_level;
	int stdout_debug;
	struct indigo_cmd_handler *cmds;
	int response_sent;

	/* Default timeout value (seconds) for commands */
	unsigned int default_timeout;

	struct bss_info {
		 int identifier;
		 int mbssid_enable;
                 int band;
	} ap_bss_info;
	int next_streamid;

	const char *bridge; /* bridge interface to use in AP mode */

	enum indigo_mode {
		SIGMA_MODE_UNKNOWN,
		SIGMA_MODE_STATION,
		SIGMA_MODE_AP,
		SIGMA_MODE_SNIFFER
	} mode;

	/*
	 * Local cached values to handle API that does not provide all the
	 * needed information with commands that actually trigger some
	 * operations.
	 */
	int listen_chn;
	int persistent;
	int intra_bss;
	int noa_duration;
	int noa_interval;
	int noa_count;
	enum wfa_cs_wps_method {
		WFA_CS_WPS_NOT_READY,
		WFA_CS_WPS_PBC,
		WFA_CS_WPS_PIN_DISPLAY,
		WFA_CS_WPS_PIN_LABEL,
		WFA_CS_WPS_PIN_KEYPAD
	} wps_method;
	char wps_pin[9];

	struct wfa_cs_p2p_group *groups;

	char infra_ssid[33];
	int infra_network_id;

	enum p2p_mode {
		P2P_IDLE, P2P_DISCOVER, P2P_LISTEN, P2P_DISABLE
	} p2p_mode;

	int go;
	int p2p_client;
	const char *p2p_ifname;

	int client_uapsd;

	char arp_ipaddr[IP_ADDR_STR_LEN];
	char arp_ifname[IFNAMSIZ + 1];

	enum sta_pmf {
		STA_PMF_DISABLED,
		STA_PMF_OPTIONAL,
		STA_PMF_REQUIRED
	} sta_pmf;

	int sta_ft_ds;

	int no_tpk_expiration;

	int er_oper_performed;
	char er_oper_bssid[20];
	int amsdu_size;
	int back_rcv_buf;

	int testbed_flag_txsp;
	int testbed_flag_rxsp;
	int chwidth;
	int enable_amsdu;  /* Flag to support single AMSDU */
	int disable_ampdu;

	unsigned int akm_values;

	/* AP configuration */
	char ap_ssid[33];
	char ap_ssid_1[33];
	/*
	 * WLAN-TAG of 1 will use 'ap_' variables;
	 * tag higher than 1 will use 'ap_tag_' variables.
	 */
	char ap_tag_ssid[MAX_WLAN_TAGS - 1][33];
	char ap_tag_ssid_1[MAX_WLAN_TAGS - 1][33];
    int enable_11ax;
	enum ap_mode {
		AP_11a,
		AP_11g,
		AP_11b,
		AP_11na,
		AP_11ng,
		AP_11ac,
		AP_11ad,
		AP_11ax,
		AP_inval
	} ap_mode;
	int ap_channel;
	int ap_rts;
	int ap_frgmnt;
	int ap_bcnint;
	struct qos_params {
		int ac;
		int cwmin;
		int cwmax;
		int aifs;
		int txop;
		int acm;
	} ap_qos[NUM_AP_AC], ap_sta_qos[NUM_AP_AC];
	enum value_not_set_enabled_disabled ap_noack;
	enum value_not_set_enabled_disabled ap_ampdu;
	enum value_not_set_enabled_disabled ap_amsdu;
	enum value_not_set_enabled_disabled ap_rx_amsdu;
	int ap_ampdu_exp;
	enum value_not_set_enabled_disabled ap_addba_reject;
	int ap_fixed_rate;
	int ap_mcs;
	int ap_rx_streams;
	int ap_tx_streams;
	unsigned int ap_vhtmcs_map;
	enum value_not_set_enabled_disabled ap_ldpc;
	enum value_not_set_enabled_disabled ap_sig_rts;
	enum ap_chwidth {
		AP_20,
		AP_40,
		AP_80,
		AP_160,
		AP_80_80,
		AP_AUTO
	} ap_chwidth;
	enum ap_chwidth_1 {
		AP2_20,
		AP2_40,
		AP2_80,
		AP2_160,
		AP2_80_80,
		AP2_AUTO
	} ap_chwidth_1;
	enum ap_chwidth default_11na_ap_chwidth;
	enum ap_chwidth default_11ng_ap_chwidth;
	int ap_tx_stbc;
	enum value_not_set_enabled_disabled ap_dyn_bw_sig;
	int ap_sgi80;
	int ap_p2p_mgmt;
	int ap_has_owe;
	int ap_has_owe_identifier;
	enum ap_key_mgmt {
		AP_OPEN,
		AP_WPA2_PSK,
		AP_WPA_PSK,
		AP_WPA2_EAP,
		AP_WPA_EAP,
		AP_WPA2_EAP_MIXED,
		AP_WPA2_PSK_MIXED,
		AP_WPA2_SAE,
		AP_WPA2_PSK_SAE,
		AP_SUITEB,
		AP_WPA2_OWE,
		AP_WPA2_EAP_OSEN,
		AP_WPA2_FT_EAP,
		AP_WPA2_FT_PSK,
		AP_WPA2_EAP_SHA256,
		AP_WPA2_PSK_SHA256,
		AP_WPA2_ENT_FT_EAP,
		AP_OSEN,
	} ap_key_mgmt;
	enum ap_tag_key_mgmt {
		AP2_OPEN,
		AP2_OSEN,
		AP2_WPA2_PSK,
		AP2_WPA2_OWE,
		AP2_WPA2_EAP,
		AP2_WPA2_SAE,
	} ap_tag_key_mgmt[MAX_WLAN_TAGS - 1];
	int ap_add_sha256;
	int ap_add_sha384;
	int ap_rsn_preauth;
	enum ap_pmf {
		AP_PMF_DISABLED,
		AP_PMF_OPTIONAL,
		AP_PMF_REQUIRED
	} ap_pmf;
	enum ap_cipher {
		AP_NO_GROUP_CIPHER_SET,
		AP_CCMP,
		AP_TKIP,
		AP_WEP,
		AP_PLAIN,
		AP_CCMP_TKIP,
		AP_GCMP_256,
		AP_GCMP_128,
		AP_CCMP_256,
		AP_CCMP_128_GCMP_256,
	} ap_cipher, ap_group_cipher;
	enum ap_wpa_pairwise {
		AP_PAIRWISE_NO_GROUP_SET,
		AP_PAIRWISE_CCMP,
		AP_PAIRWISE_CCMP_TKIP,
	} ap_wpa_pairwise;
	enum ap_group_mgmt_cipher {
		AP_NO_GROUP_MGMT_CIPHER_SET,
		AP_BIP_GMAC_256,
		AP_BIP_CMAC_256,
		AP_BIP_GMAC_128,
		AP_BIP_CMAC_128,
	} ap_group_mgmt_cipher;
	char ap_sae_groups[27];
	int sae_anti_clogging_threshold;
	int sae_reflection;
	int ap_sae_commit_status;
	int ap_sae_pk_omit;
	int ap_sae_pk_password_check_skip;
	int sae_confirm_immediate;
	int ap_sae_require_mfp;
	char ap_passphrase[101];
	char ap_tag_passphrase[MAX_WLAN_TAGS - 1][101];
	char ap_psk[65];
	char *ap_sae_passwords;
	char *ap_sae_pk_modifier;
	char *ap_sae_pk_keypair;
	char *ap_sae_pk_keypair_sig;
	int ap_sae_pk;
	char ap_wepkey[27];
	char ap_radius_ipaddr[20];
	int ap_radius_port;
	char ap_radius_password[200];
	char ap2_radius_ipaddr[20];
	int ap2_radius_port;
	char ap2_radius_password[200];
	int ap_tdls_prohibit;
	int ap_tdls_prohibit_chswitch;
	int ap_hs2;
	int ap_dgaf_disable;
	int ap_p2p_cross_connect;
	int ap_oper_name;
	int ap_wan_metrics;
	int ap_conn_capab;
	int ap_oper_class;

	int ap_interworking;
	int ap_access_net_type;
	int ap_internet;
	int ap_venue_group;
	int ap_venue_type;
	char ap_hessid[20];
	char ap_roaming_cons[100];
	int ap_venue_name;
	int ap_net_auth_type;
	int ap_nai_realm_list;
	char ap_domain_name_list[1000];
	int ap_ip_addr_type_avail;
	char ap_plmn_mcc[10][4];
	char ap_plmn_mnc[10][4];
	int ap_gas_cb_delay;
	int ap_proxy_arp;
	int ap2_proxy_arp;
	int ap2_osu;
	int ap_l2tif;
	int ap_anqpserver;
	int ap_anqpserver_on;
	int ap_osu_provider_list;
	int ap_osu_provider_nai_list;
	int ap_qos_map_set;
	int ap_bss_load;
	char ap_osu_server_uri[10][256];
	char ap_osu_ssid[33];
	int ap_osu_method[10];
	int ap_osu_icon_tag;
	int ap_venue_url;
	int ap_advice_of_charge;
	int ap_oper_icon_metadata;
	int ap_tnc_file_name;
	unsigned int ap_tnc_time_stamp;

	int ap_fake_pkhash;
	int ap_disable_protection;
	int ap_allow_vht_wep;
	int ap_allow_vht_tkip;

	enum ap_vht_chwidth {
		AP_20_40_VHT_OPER_CHWIDTH,
		AP_80_VHT_OPER_CHWIDTH,
		AP_160_VHT_OPER_CHWIDTH
	} ap_vht_chwidth;
	int ap_txBF;
	int ap_mu_txBF;
	enum ap_regulatory_mode {
		AP_80211D_MODE_DISABLED,
		AP_80211D_MODE_ENABLED,
	} ap_regulatory_mode;
	enum ap_dfs_mode {
		AP_DFS_MODE_DISABLED,
		AP_DFS_MODE_ENABLED,
	} ap_dfs_mode;
	int ap_ndpa_frame;
	int ap_enable_11h;
	int ap_lci;
	char ap_val_lci[33];
	char ap_infoz[17];
	int ap_lcr;
	char ap_val_lcr[400];
	int ap_rrm;
	int ap_rtt;
	int ap_neighap; /* number of configured neighbor APs */
	unsigned char ap_val_neighap[3][6];
	int ap_opchannel; /* number of oper channels */
	int ap_val_opchannel[3];
	int ap_scan;
	int ap_fqdn_held;
	int ap_fqdn_supl;
	int ap_msnt_type;

	int ap_mbo;
	int ap_ne_class;
	int ap_ne_op_ch;
	int ap_set_bssidpref;
	int ap_btmreq_disassoc_imnt;
	int ap_btmreq_term_bit;
	int ap_disassoc_timer;
	int ap_btmreq_bss_term_dur;
	enum reg_domain {
		REG_DOMAIN_NOT_SET,
		REG_DOMAIN_LOCAL,
		REG_DOMAIN_GLOBAL
	} ap_reg_domain;
	char ap_mobility_domain[10];
	unsigned char ap_cell_cap_pref;
	int ap_ft_oa;
	enum value_not_set_enabled_disabled ap_ft_ds;
	int ap_name;
	int ap_interface_5g;
	int ap_interface_2g;
	int ap_interface_6g;
	int ap_assoc_delay;
	int ap_btmreq_bss_term_tsf;
	int ap_fils_dscv_int;
	int ap_nairealm_int;
	char ap_nairealm[33];
	int ap_blechanutil;
	int ap_ble_admit_cap;
	int ap_datappdudura;
	int ap_airtimefract;
	char ap_dhcpserv_ipaddr[20];
	int ap_dhcp_stop;
	int ap_bawinsize;
	int ap_blestacnt;
	int ap_ul_availcap;
	int ap_dl_availcap;
	int ap_akm;
	unsigned int ap_akm_values;
	int ap_pmksa;
	int ap_pmksa_caching;
	int ap_80plus80;
	int ap_preset_chn;
	int cnt_2g; /* count for 2G vaps*/
	int cnt_5g; /* count for 5G vaps*/
        int cnt_6g; /* count for 6G vaps*/
	int cnt_call; /* count for ap_set_wireless call */
	int mac_2g; /* count for 2G ap_get_mac*/
	int mac_5g; /* count for 5G ap_get_mac*/
	int mac_6g; /* count for 6G ap_get_mac*/

	struct mbo_pref_ap mbo_pref_aps[MBO_MAX_PREF_BSSIDS];
	struct mbo_pref_ap mbo_self_ap_tuple;
	int mbo_pref_ap_cnt;
	unsigned char ft_bss_mac_list[MAX_FT_BSS_LIST][ETH_ALEN];
	int ft_bss_mac_cnt;
	int expbcn_len;

	char *ar_ltf;

	int ap_numsounddim;
	unsigned int he_mcsnssmap;
	int he_ul_mcs;
	int he_mmss;
	int he_srctrl_allow;
	int ap_he_extnss;
	int ap_radio_on;
	int nss_mcs_set;
	int ap_cad_unsolicited_proberesp;
	int numnontxbss;
	u8 ap_transition_disable;
	int ap_ocvc;
	int csa_ie;
	int mmic_ie_bipn;
	int mmic_ie_invalid;
	int ap_keyrotationint;
	int ap_beaconprotection;
	int ap_he_ersudisable;
	int ap_start_disabled;
	int ap_txpwr;
	int ap_val_txpwr;
	int ap_ipsubnetid;
	char ap_val_ipsubnetid[15];

	enum value_not_set_enabled_disabled ap_he_smps;
	enum value_not_set_enabled_disabled ap_oce;
	enum value_not_set_enabled_disabled ap_filsdscv;
	enum value_not_set_enabled_disabled ap_filshlp;
	enum value_not_set_enabled_disabled ap_broadcast_ssid;
	enum value_not_set_enabled_disabled ap_rnr;
	enum value_not_set_enabled_disabled ap_esp;

	enum value_not_set_enabled_disabled ap_he_ulofdma;
	enum value_not_set_enabled_disabled ap_he_dlofdma;
	enum value_not_set_enabled_disabled ap_bcc;
	enum value_not_set_enabled_disabled ap_he_frag;
	enum value_not_set_enabled_disabled ap_mu_edca;
	enum value_not_set_enabled_disabled ap_he_rtsthrshld;
	enum value_not_set_enabled_disabled ap_mbssid;
	enum value_not_set_enabled_disabled ap_twtresp;
	enum value_not_set_enabled_disabled he_sounding;
	enum value_not_set_enabled_disabled he_set_sta_1x1;
	enum value_not_set_enabled_disabled ap_unsolicited_proberesp;
	enum value_not_set_enabled_disabled ap_6g_legacy_security;
	enum value_not_set_enabled_disabled ap_activeind_proberesp;
	enum value_not_set_enabled_disabled ap_fullbw_ulmumimo;
	enum value_not_set_enabled_disabled ap_twtinfoframerx;
	enum value_not_set_enabled_disabled ap_ulmudata_disablerx;
	enum value_not_set_enabled_disabled ap_40intolerant;
	enum value_not_set_enabled_disabled ap_mscs;
	enum value_not_set_enabled_disabled ap_ess_rpt;

	enum ppdu {
		PPDU_NOT_SET,
		PPDU_MU,
		PPDU_SU,
		PPDU_ER,
		PPDU_TB,
		PPDU_HESU,
	} ap_he_ppdu;

	enum bufsize {
		BA_BUFSIZE_NOT_SET,
		BA_BUFSIZE_64,
		BA_BUFSIZE_256,
	} ap_ba_bufsize;

	enum mimo {
		MIMO_NOT_SET,
		MIMO_DL,
		MIMO_UL,
	} ap_he_mimo;

	struct indigo_ese_alloc ap_ese_allocs[MAX_ESE_ALLOCS];
	int ap_num_ese_allocs;

	enum transpower {
		TRANSPOWER_NOT_SET,
		TRANSPOWER_MIN,
		TRANSPOWER_MAX,
	} ap_transpower;

	const char *hostapd_debug_log;
	const char *wpa_supplicant_debug_log;

#ifdef CONFIG_TRAFFIC_AGENT
	/* Traffic Agent */
	struct indigo_stream streams[MAX_SIGMA_STREAMS];
	int stream_id;
	int num_streams;
	pthread_t thr;
#endif /* CONFIG_TRAFFIC_AGENT */

	unsigned int throughput_pktsize; /* If non-zero, override pktsize for
					  * throughput tests */
	int no_timestamps;

	const char *sniffer_ifname;
	const char *set_macaddr;
	int tmp_mac_addr;
	int ap_is_dual;
	enum ap_mode ap_mode_1;
	enum ap_chwidth ap_chwidth_ac;
	int ap_channel_1;
	char ap_countrycode[3];
        int map_supported_freq_band;
#define MAP_FREQ_BAND_802_11_2_4GHZ 0x00
#define MAP_FREQ_BAND_802_11_5GHZ   0x01
        int map_mid;
        char map_msgtypeval[15];
        char map_tlvvalue[4096];
/* Define DPP bootstrapping data */
#define MAP_DPP_LOCAL_BOOTSTRAP_DATA 0x01
#define MAP_DPP_PEER_BOOTSTRAP_DATA  0x02
#define MAP_DPP_START_BUFFER         0x03
#define MAP_DPP_STOP_BUFFER          0x04
#define MAP_DPP_GET_FRAME_INFO       0x05
	char map_local_bootstrapping_data[2048];
	char map_local_bootstrapping_key[2048];
	bool map_repacd_enableDPP;
        int map_tlvcnt;
        bool map_multi_tlv;
        bool map_hwaddr_set;
        char *map_pw;
	char map_ifname[16];
        unsigned char map_bssid[6];
        unsigned char map_sta_addr[6];
        unsigned int map_tlvtype;
        char map_alid[13];
        bool map_bh24g;
        bool map_bh5g;
        bool map_bh5gh;
        bool map_ath0_onboard;
        bool map_ath1_onboard;
        bool map_ath2_onboard;
        bool map_all_onboard;
        enum map_device_role {
                MAP_DEVROLE_UNKNOWN,
                MAP_DEVROLE_CONTROLLER,
                MAP_DEVROLE_AGENT,
        } map_device_role;

	int ap_wpsnfc;

	enum ap_wme {
		AP_WME_OFF,
		AP_WME_ON,
	} ap_wme;

	enum ap_wmmps {
		AP_WMMPS_OFF,
		AP_WMMPS_ON,
	} ap_wmmps;

	enum sec_ch_offset ap_chwidth_offset;

	char *ap_dpp_conf_addr;
	char *ap_dpp_conf_pkhash;


	int last_set_ip_config_ipv6;

	int tid_to_handle[8]; /* Mapping of TID to handle */
	int dialog_token; /* Used for generating unique handle for an addTs */

	enum indigo_program {
		PROGRAM_UNKNOWN = 0,
		PROGRAM_TDLS,
		PROGRAM_HS2,
		PROGRAM_HS2_R2,
		PROGRAM_WFD,
		PROGRAM_DISPLAYR2,
		PROGRAM_PMF,
		PROGRAM_WPS,
		PROGRAM_60GHZ,
		PROGRAM_HT,
		PROGRAM_VHT,
		PROGRAM_MAP,
		PROGRAM_MAPR2,
		PROGRAM_MAPR3,
		PROGRAM_NAN,
		PROGRAM_LOC,
		PROGRAM_MBO,
		PROGRAM_IOTLP,
		PROGRAM_DPP,
		PROGRAM_OCE,
		PROGRAM_WPA3,
		PROGRAM_HE,
		PROGRAM_HS2_R3,
		PROGRAM_DE,
		PROGRAM_QM,
	} program;

	enum device_type {
		device_type_unknown,
		AP_unknown,
		AP_testbed,
		AP_dut,
		STA_unknown,
		STA_testbed,
		STA_dut
	} device_type;

	enum {
		DEVROLE_UNKNOWN = 0,
		DEVROLE_STA,
		DEVROLE_PCP,
		DEVROLE_STA_CFON,
		DEVROLE_AP,
	} dev_role;

	enum wps_band {
		WPS_BAND_NON_60G = 0,
		WPS_BAND_60G,
	} band;

	int wps_disable; /* Used for 60G to disable PCP from sending WPS IE */
	int wsc_fragment; /* simulate WSC IE fragmentation */
	int eap_fragment; /* simulate EAP fragmentation */
	int wps_forced_version; /* Used to force reported WPS version */
	enum {
		/* no change */
		FORCE_RSN_IE_NONE = 0,
		/* if exists, remove and clear privacy bit */
		FORCE_RSN_IE_REMOVE,
		/* if not exists, add and set privacy bit */
		FORCE_RSN_IE_ADD,
	} force_rsn_ie; /* override RSN IE in association request */

	const char *version;
	int no_ip_addr_set;
	int sta_channel;

	const char *summary_log;
	const char *hostapd_entropy_log;

	int iface_down_on_reset;
	int write_stats; /* traffic stream e2e*.txt files */
	int sim_no_username; /* do not set SIM username to use real SIM */

	const char *vendor_name; /* device_get_info vendor override */
	const char *model_name; /* device_get_info model override */
	const char *version_name; /* device_get_info version override */
	const char *log_file_dir; /* Directory to generate log file */
	FILE *log_file_fd; /* Pointer to log file */

	/* target option override */
	enum target_opt {
		AP1_OPT,
		AP2_OPT
	} target_opt;

	char wpa3r3_opt[16];

	int ndp_enable; /* Flag which is set once the NDP is setup */

	int ndpe; /* Flag indicating NDPE is supported */
	u16 trans_port; /* transport port number for TCP/UDP connection */
	u8 trans_proto; /* transport protocol, 0x06: TCP, 0x11: UDP */
	u8 nan_ipv6_addr[IPV6_ADDR_LEN]; /* NAN IPv6 address */
	u8 nan_ipv6_len; /* NAN IPv6 address length */

	/* Length of nan_pmk in octets */
	u8 nan_pmk_len;

	/*
	 * PMK: Info is optional in Discovery phase. PMK info can
	 *  be passed during the NDP session.
	 */
	u8 nan_pmk[32];

	enum value_not_set_enabled_disabled wnm_bss_max_feature;
	int wnm_bss_max_idle_time;
	enum value_not_set_enabled_disabled wnm_bss_max_protection;

	char *non_pref_ch_list; /* MBO: non-preferred channel report */
	char *btm_query_cand_list; /* Candidate list for BTM Query */

	char *sae_commit_override;
	char *rsne_override;
	char *rsnxe_override_eapol;
	int sta_associate_wait_connect;
	char server_cert_hash[65];
	int server_cert_tod;
	int sta_tod_policy;
	const char *hostapd_bin;
	int use_hostapd_pid_file;
	const char *hostapd_ifname;
	const char *hostapd_ifname_argsave;
	int hostapd_running;

	char *dpp_peer_uri;
	int dpp_local_bootstrap;
	int dpp_conf_id;
	int dpp_network_id;

	u8 fils_hlp;
	pthread_t hlp_thread;

#ifdef NL80211_SUPPORT
	struct nl80211_ctx *nl_ctx;
	int config_rsnie;
#endif /* NL80211_SUPPORT */

	int sta_nss;

	const char *priv_cmd; /* iwpriv / cfg80211tool command name */

	unsigned int wpa_log_size;
	char dev_start_test_runtime_id[100];

	int sae_h2e_default;
	int sae_h2e_only;
	enum {
		SAE_PWE_DEFAULT,
		SAE_PWE_LOOP,
		SAE_PWE_H2E
	} sae_pwe;
}dut;


enum indigo_dut_print_level {
	DUT_MSG_DEBUG, DUT_MSG_INFO, DUT_MSG_ERROR
};


enum indigo_status {
	SIGMA_RUNNING, SIGMA_INVALID, SIGMA_ERROR, SIGMA_COMPLETE
};

int run_system_wrapper(const char *cmd, ...);
int run_iwpriv(struct indigo_dut *dut, const char *ifname, const char *cmd, ...);


char* owrt_get_ap_wireless_interface(void);
void owrt_ap_add_vap(int id, const char *key,
                    const char *val);
void start_ap_set_ath_params(void *if_info);
void owrt_ap_set_vap(int id, const char *key,const char *val);
void owrt_ap_set_radio( int id,const char *key, const char *val);
int owrt_ap_config_radio(struct indigo_dut *dut);
int owrt_ap_config_vap(struct indigo_dut *dut);
void owrt_ap_set_list_vap(int id,const char *key, const char *val);
int bring_up_ap_interface(void);
void set_ap_params(struct packet_wrapper *req);
void send_ap_btm_req(char *bssid);

int get_value(const char *str);


#ifdef NL80211_SUPPORT
struct nl80211_ctx * nl80211_init(struct indigo_dut *dut);
void nl80211_deinit(struct indigo_dut *dut, struct nl80211_ctx *ctx);
struct nl_msg * nl80211_drv_msg(struct indigo_dut *dut, struct nl80211_ctx *ctx,
				int ifindex, int flags,
				uint8_t cmd);
int send_and_recv_msgs(struct indigo_dut *dut, struct nl80211_ctx *ctx,
		       struct nl_msg *nlmsg,
		       int (*valid_handler)(struct nl_msg *, void *),
		       void *valid_data);
#endif /* NL80211_SUPPORT */

void traffic_register_cmds(void);
void traffic_agent_register_cmds(void);
void powerswitch_register_cmds(void);
void atheros_register_cmds(void);
void dev_register_cmds(void);
void sniffer_register_cmds(void);
void server_register_cmds(void);
void miracast_register_cmds(void);

