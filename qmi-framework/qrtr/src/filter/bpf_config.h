/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a contribution.
 *
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _BPF_CONFIG_H
#define _BPF_CONFIG_H

#include <stdio.h>
#include <stdlib.h>

#define QMI_ACCESS_CTRL_VERSION1		1

#define QMI_MSG_ACCESS_CTRL_MASK		0x0001
#define QMI_SRV_BROADCAST_CTRL_MASK		0x0002

#define QMI_SRV_BROADCAST_ENABLE		0x0
#define QMI_SRV_BROADCAST_DISABLE		0x1

#define QMI_ANY_INSTANCE				0xFFFFFFFF

/* Below are the group config for the platform picked from
 * the path system/core/include/private/android_filesystem_config.h
 */

#define AID_ROOT             0  /* traditional unix root user */

#define AID_SYSTEM        1000  /* system server */
#define AID_RADIO         1001  /* telephony subsystem, RIL */
#define AID_BLUETOOTH     1002  /* bluetooth subsystem */
#define AID_GRAPHICS      1003  /* graphics devices */
#define AID_INPUT         1004  /* input devices */
#define AID_AUDIO         1005  /* audio devices */
#define AID_CAMERA        1006  /* camera devices */
#define AID_LOG           1007  /* log devices */
#define AID_COMPASS       1008  /* compass device */
#define AID_MOUNT         1009  /* mountd socket */
#define AID_WIFI          1010  /* wifi subsystem */
#define AID_ADB           1011  /* android debug bridge (adbd) */
#define AID_INSTALL       1012  /* group for installing packages */
#define AID_MEDIA         1013  /* mediaserver process */
#define AID_DHCP          1014  /* dhcp client */
#define AID_SDCARD_RW     1015  /* external storage write access */
#define AID_VPN           1016  /* vpn system */
#define AID_KEYSTORE      1017  /* keystore subsystem */
#define AID_USB           1018  /* USB devices */
#define AID_DRM           1019  /* DRM server */
#define AID_MDNSR         1020  /* MulticastDNSResponder (service discovery) */
#define AID_GPS           1021  /* GPS daemon */
#define AID_UNUSED1       1022  /* deprecated, DO NOT USE */
#define AID_MEDIA_RW      1023  /* internal media storage write access */
#define AID_MTP           1024  /* MTP USB driver access */
#define AID_UNUSED2       1025  /* deprecated, DO NOT USE */
#define AID_DRMRPC        1026  /* group for drm rpc */
#define AID_NFC           1027  /* nfc subsystem */
#define AID_SDCARD_R      1028  /* external storage read access */
#define AID_CLAT          1029  /* clat part of nat464 */
#define AID_LOOP_RADIO    1030  /* loop radio devices */
#define AID_MEDIA_DRM     1031  /* MediaDrm plugins */
#define AID_PACKAGE_INFO  1032  /* access to installed package details */
#define AID_SDCARD_PICS   1033  /* external storage photos access */
#define AID_SDCARD_AV     1034  /* external storage audio/video access */
#define AID_SDCARD_ALL    1035  /* access all users external storage */
#define AID_LOGD          1036  /* log daemon */
#define AID_SHARED_RELRO  1037  /* creator of shared GNU RELRO files */
#define AID_DBUS          1038  /* dbus-daemon IPC broker process */
#define AID_TLSDATE       1039  /* tlsdate unprivileged user */
#define AID_MEDIA_EX      1040  /* mediaextractor process */
#define AID_AUDIOSERVER   1041  /* audioserver process */
#define AID_METRICS_COLL  1042  /* metrics_collector process */
#define AID_METRICSD      1043  /* metricsd process */
#define AID_WEBSERV       1044  /* webservd process */
#define AID_DEBUGGERD     1045  /* debuggerd unprivileged user */
#define AID_MEDIA_CODEC   1046  /* mediacodec process */
#define AID_CAMERASERVER  1047  /* cameraserver process */

#define AID_SHELL         2000  /* adb and debug shell user */
#define AID_CACHE         2001  /* cache access */
#define AID_DIAG          2002  /* access to diagnostic resources */

/* The range 2900-2999 is reserved for OEM, and must never be
 * used here */
#define AID_OEM_RESERVED_START 2900
#define AID_OEM_RESERVED_END   2999

/* The 3000 series are intended for use as supplemental group id's only.
 * They indicate special Android capabilities that the kernel is aware of. */
#define AID_NET_BT_ADMIN  3001  /* bluetooth: create any socket */
#define AID_NET_BT        3002  /* bluetooth: create sco, rfcomm or l2cap sockets */
#define AID_INET          3003  /* can create AF_INET and AF_INET6 sockets */
#define AID_NET_RAW       3004  /* can create raw INET sockets */
#define AID_NET_ADMIN     3005  /* can configure interfaces and routing tables. */
#define AID_NET_BW_STATS  3006  /* read bandwidth statistics */
#define AID_NET_BW_ACCT   3007  /* change bandwidth statistics accounting */
#define AID_NET_BT_STACK  3008  /* bluetooth: access config files */
#define AID_QCOM_DIAG     3009  /* can read/write /dev/diag */

#define AID_SENSORS       3011 /* access to /dev/socket/sensor_ctl_socket & QCCI/QCSI */

#define AID_RFS           3012  /* Remote Filesystem for peripheral processors */
#define AID_RFS_SHARED    3013  /* Shared files for Remote Filesystem for peripheral processors  */

#define AID_EVERYBODY     9997  /* shared between all apps in the same profile */
#define AID_MISC          9998  /* access to misc storage */

/* ------------ Define  QMI services information here -------------- */

/* QMI TEST/PING Service */
#define QMI_TEST_SERVICE_ID	15
#define QMI_TEST_INSTANCE_ID	0

/* ----- Different param size in eBPF skbuff data ----- */

#define SERVICE_ID_LEN	4
#define INSTANCE_ID_LEN	4
#define NODE_ID_LEN	4
#define PKT_TYPE_FIELD_LEN	2
#define PKT_DIR_FIELD_LEN	2
#define QMI_CNTL_FLAG_LEN	1
#define QMI_TXN_ID_LEN		2
#define QMI_MSG_ID_LEN		2
#define QMI_MSG_SIZE_LEN	2
#define GID_LEN				4

#define TO_BITS				8

/* ------------ GID offset in GID list -------------- */

#define SERVICE_ID_OFFSET		0
#define INSTANCE_ID_OFFSET		(SERVICE_ID_OFFSET+SERVICE_ID_LEN)
#define NODE_ID_OFFSET			(INSTANCE_ID_OFFSET+INSTANCE_ID_LEN)
#define PKT_TYPE_OFFSET			(NODE_ID_OFFSET+NODE_ID_LEN)
#define PKT_DIR_OFFSET			(PKT_TYPE_OFFSET+PKT_TYPE_FIELD_LEN)
#define QMI_CNTL_FLAG_OFFSET		(PKT_DIR_OFFSET+PKT_DIR_FIELD_LEN)
#define QMI_TXN_ID_OFFSET		(QMI_CNTL_FLAG_OFFSET+QMI_CNTL_FLAG_LEN)
#define QMI_MSG_ID_OFFSET		(QMI_TXN_ID_OFFSET+QMI_TXN_ID_LEN)
#define QMI_MSG_LEN_OFFSET		(QMI_MSG_ID_OFFSET+QMI_MSG_ID_LEN)
#define GID_LEN_OFFSET			(QMI_MSG_LEN_OFFSET+QMI_MSG_SIZE_LEN)
#define EFF_GID_OFFSET			(GID_LEN_OFFSET+GID_LEN)
#define _1st_GID_OFFSET			(EFF_GID_OFFSET+GID_LEN)
#define _2nd_GID_OFFSET			(_1st_GID_OFFSET+GID_LEN)
#define _3rd_GID_OFFSET			(_2nd_GID_OFFSET+GID_LEN)
#define _4th_GID_OFFSET			(_3rd_GID_OFFSET+GID_LEN)
#define _5th_GID_OFFSET			(_4th_GID_OFFSET+GID_LEN)
#define _6th_GID_OFFSET			(_5th_GID_OFFSET+GID_LEN)
#define _7th_GID_OFFSET			(_6th_GID_OFFSET+GID_LEN)
#define _8th_GID_OFFSET			(_7th_GID_OFFSET+GID_LEN)
#define _9th_GID_OFFSET			(_8th_GID_OFFSET+GID_LEN)
#define _10th_GID_OFFSET		(_9th_GID_OFFSET+GID_LEN)
#define _11th_GID_OFFSET		(_10th_GID_OFFSET+GID_LEN)
#define _12th_GID_OFFSET		(_11th_GID_OFFSET+GID_LEN)
#define _13th_GID_OFFSET		(_12th_GID_OFFSET+GID_LEN)
#define _14th_GID_OFFSET		(_13th_GID_OFFSET+GID_LEN)
#define _15th_GID_OFFSET		(_14th_GID_OFFSET+GID_LEN)
#define BROADCAST_NODE_OFFSET		(_15th_GID_OFFSET+GID_LEN)

/* Node id definitions */
#define MSM_APP_NODE_ID	1
#define MDM_A7_NODE_ID	2
#define MDM_Q6_NODE_ID  3

typedef struct
{
	unsigned int	gid;
}gid_list_type;

typedef gid_list_type access_rule1_type;

typedef struct
{
	unsigned short	msg_id_low;
	unsigned short	msg_id_high;
	unsigned short	msg_rule_mask;
	unsigned int	size;
	const gid_list_type *gid_list;
}access_rule2_type;

typedef struct
{
	unsigned int	node_id;
}node_list;

typedef struct
{
	unsigned int	version;
	unsigned int	service_id;
	unsigned int	instance_id;
	unsigned int	rule1_size;
	const access_rule1_type *rule1;
	unsigned int	rule2_size;
	const access_rule2_type *rule2;
	unsigned int	svc_broadcast_dis;
	unsigned int	node_size;
	node_list	*node;
}svc_access_ctrl_rule_type;

extern const svc_access_ctrl_rule_type qmi_access_cntl_rule[];
extern const unsigned int access_rule_size;

#endif /* _BPF_CONFIG_H */
