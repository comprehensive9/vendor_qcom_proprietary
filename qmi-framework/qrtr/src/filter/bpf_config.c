/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "bpf_config.h"

/**
 * This is the configuration file where we can update the QMI access
 * control rules for each services. QMI service/client owner is
 * responsible to maintain access control rule to their service.
 * qmi_access_cntl_rule[] structure need to be updated with proper
 * rules. By default no rules will be applicable to any of the QMI
 * client<->service communication. Rules can be added to structure
 * qmi_access_cntl_rule[] by defining minimum below fields
 * - version
 * - service_id
 * - instance_id
 * - rule1_size
 * - rule1 (to control service level access)
 * or
 * - version
 * - service_id
 * - instance_id
 * - svc_broadcast_dis (to control service broadcast)
 * - node_size
 * - node
 *
 * If message level access control is required then message level
 * access rule also need to be updated.
 *
 * Refere QMI_TEST_SERVICE_ID related rule to understand how
 * to add access rule for service & message level.
 */

/* Service level group id rule for test service */
static const access_rule1_type test_svc_lvl_gid_list[] =
{
	AID_NET_BW_STATS,
	AID_AUDIO,
	AID_COMPASS,
};

/* Message level group id rule for test service */
static const access_rule1_type test_svc_msg_lvl_gid_list[] =
{
	AID_SYSTEM,
	AID_NET_BW_STATS,
};

/* Message level access rule for test service */
static const access_rule2_type qmi_test_msg_rest[] =
{
	{ 0x001E, 0x0021, QMI_MSG_ACCESS_CTRL_MASK,
	  sizeof(test_svc_msg_lvl_gid_list)/sizeof(test_svc_msg_lvl_gid_list[0]),
	  test_svc_msg_lvl_gid_list },
	{ 0x0022, 0x0028, QMI_MSG_ACCESS_CTRL_MASK,
	  sizeof(test_svc_msg_lvl_gid_list)/sizeof(test_svc_msg_lvl_gid_list[0]),
	  test_svc_msg_lvl_gid_list }
};

/* Define access control rule for all the services here */
const svc_access_ctrl_rule_type qmi_access_cntl_rule[] =
{
	{
		.version = QMI_ACCESS_CTRL_VERSION1,
		.service_id = QMI_TEST_SERVICE_ID,
		.instance_id = QMI_TEST_INSTANCE_ID,
		.rule1_size = sizeof(test_svc_lvl_gid_list)/sizeof(test_svc_lvl_gid_list[0]),
		.rule1 = test_svc_lvl_gid_list,
		.rule2_size = sizeof(qmi_test_msg_rest)/sizeof(qmi_test_msg_rest[0]),
		.rule2 = qmi_test_msg_rest,
		.svc_broadcast_dis = QMI_SRV_BROADCAST_ENABLE
	}
};

const unsigned int access_rule_size = sizeof(qmi_access_cntl_rule)/sizeof(qmi_access_cntl_rule[0]);
