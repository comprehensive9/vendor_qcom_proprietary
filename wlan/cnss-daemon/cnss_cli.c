/*
 * Copyright (c) 2019,2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cnss_cli.h"
#include <sys/un.h>
#include <errno.h>

#define MAX_CMD_LEN 256
#define MAX_NUM_OF_PARAMS 2
#define MAX_CNSS_CMD_LEN 32

#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))

static char *cnss_cmd[][2] = {
	{"qdss_trace_start", ""},
	{"qdss_trace_stop", "<Hex base number: e.g. 0x3f>"},
	{"qdss_trace_load_config", ""},
	{"quit", ""}
};

void help(void)
{
	int i = 0;

	printf("Supported Command:\n");
	for (i = 0; i < (int)ARRAY_SIZE(cnss_cmd); i++)
		printf("%s %s\n", cnss_cmd[i][0], cnss_cmd[i][1]);
}

static int send_cmd_to_daemon_via_unixsocket(char *cmd_buffer, char *resp_buffer)
{
	int ret = 0;
	int sockfd;
	unsigned int len;
	struct sockaddr_un cl_addr;
	struct sockaddr_un sv_addr;

	if ((strlen(CNSS_USER_CLIENT) > (sizeof(cl_addr.sun_path) - 1)) ||
	    (strlen(CNSS_USER_SERVER) > (sizeof(sv_addr.sun_path) - 1))) {
		fprintf(stderr, "Invalid client/server path %s %s\n",
			CNSS_USER_CLIENT, CNSS_USER_SERVER);
		return -EINVAL;
	}

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Failed to connect to cnss-daemon\n");
		return sockfd;
	}

	memset(&cl_addr, 0, sizeof(cl_addr));
	cl_addr.sun_family = AF_UNIX;
	strlcpy(cl_addr.sun_path, CNSS_USER_CLIENT,
		(sizeof(cl_addr.sun_path) - 1));

	memset(&sv_addr, 0, sizeof(sv_addr));
	sv_addr.sun_family = AF_UNIX;
	strlcpy(sv_addr.sun_path, CNSS_USER_SERVER,
		(sizeof(sv_addr.sun_path) - 1));

	if (bind(sockfd, (struct sockaddr *)&cl_addr, sizeof(cl_addr)) < 0) {
		fprintf(stderr, "Fail to bind client socket %s\n",
			strerror(errno));
		ret = -errno;
		goto out;
	}

	if (sendto(sockfd, cmd_buffer, CNSS_CLI_MAX_PAYLOAD, 0,
		   (struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
		fprintf(stderr, "Failed to send: Error: %s\n",
			strerror(errno));
		ret = -errno;
		goto out;
	}

	if (resp_buffer) {
		/* Send to socket was success, wait for response from daemon */
		if (recvfrom(sockfd, resp_buffer, CNSS_CLI_MAX_PAYLOAD, 0,
			     (struct sockaddr *)&sv_addr, &len) <= 0) {
			fprintf(stderr,
				"Failed to get response Error: %s, retrying\n",
				strerror(errno));
			ret = -errno;
		}
	}

out:
	remove(CNSS_USER_CLIENT);
	close(sockfd);
	return ret;
}

static int check_resp(char *cmd_buffer, char *resp_buffer)
{
	struct cnss_cli_msg_hdr *hdr = NULL;
	struct cnss_cli_msg_hdr *resp_hdr = NULL;

	/* Check if received a valid response from the daemon */
	if (resp_buffer) {
		hdr = (struct cnss_cli_msg_hdr *)cmd_buffer;
		resp_hdr = (struct cnss_cli_msg_hdr *)resp_buffer;
		if (hdr->type != resp_hdr->type)
			fprintf(stderr,
				"Invalid response req type:%d, resp_type:%d\n",
				hdr->type, resp_hdr->type);
		else if (resp_hdr->resp_status)
			fprintf(stderr,
				"Response code %d from daemon\n",
				resp_hdr->resp_status);
		printf("type %d, resp_status: %d\n", resp_hdr->type, resp_hdr->resp_status);
	}
	return 0;
}

static int send_cmd_to_daemon_and_check_resp(int sock_type, char *cmd_buffer, char *resp_buffer)
{
	int ret = 0;

	switch (sock_type) {
	case AF_UNIX:
		ret = send_cmd_to_daemon_via_unixsocket(cmd_buffer, resp_buffer);
		break;
	default:
		printf("%s: Unknown sock type %d\n", __func__, sock_type);
		ret = -EINVAL;
	}

	if (!ret)
		check_resp(cmd_buffer, resp_buffer);
	return ret;
}

int main(void)
{
	char *tmp = NULL;
	char cmd_str[MAX_CMD_LEN];
	char token[MAX_NUM_OF_PARAMS][MAX_CNSS_CMD_LEN];
	int token_num = 0;
	int i = 0;
	int valid_cmd = 0;
	char buffer[CNSS_CLI_MAX_PAYLOAD] = {0};
	char resp_buffer[CNSS_CLI_MAX_PAYLOAD] = {0};
	struct cnss_cli_msg_hdr *hdr = NULL;
	void *cnss_cli_data = NULL;
	enum cnss_cli_cmd_type type = CNSS_CLI_CMD_NONE;

	while (1) {
		printf("cnss_cli_cmd >> ");
		fgets(cmd_str, MAX_CMD_LEN, stdin);
		if (!strcmp(cmd_str, "\n"))
			continue;

		tmp = &cmd_str[0];
		i = 0;
		int len = 0;
		char *tmp1;

		while (*tmp != '\0') {
			if (i >= MAX_NUM_OF_PARAMS) {
				printf("Invalid input, max number of params is %d\n",
				       MAX_NUM_OF_PARAMS);
				break;
			}
			tmp1 = tmp;
			len = 0;

			while (*tmp1 == ' ') {
				tmp1++;
				tmp++;
			}
			while (*tmp1 != ' ' && *tmp1 != '\n') {
				len++;
				tmp1++;
			}
			if (*tmp1 != '\n')
				*tmp1 = '\0';

			strlcpy(token[i], tmp, sizeof(token[i]));
			token[i][len] = '\0';
			tmp = tmp1;
			if (*tmp == '\n')
				break;
			tmp++;
			i++;
		}

		if (i >= MAX_NUM_OF_PARAMS)
			continue;

		token_num = i + 1;
		valid_cmd = 1;

		if (!strcmp(token[0], "qdss_trace_start")) {
			type = QDSS_TRACE_START;
		} else if (!strcmp(token[0], "qdss_trace_stop")) {
			if (token_num != 2) {
				printf("qdss_trace_stop <option>\n");
				continue;
			}
			type = QDSS_TRACE_STOP;
		} else if (!strcmp(token[0], "qdss_trace_load_config")) {
			type = QDSS_TRACE_CONFIG_DOWNLOAD;
		} else if (!strcmp(token[0], "help")) {
			help();
			type = CNSS_CLI_CMD_NONE;
		} else if (!strcmp(token[0], "quit")) {
			goto out;
		} else {
			printf("Invalid command %s\n", token[0]);
			type = CNSS_CLI_CMD_NONE;
		}

		if (type == CNSS_CLI_CMD_NONE)
			continue;

		memset(buffer, 0, sizeof(buffer));

		hdr = (struct cnss_cli_msg_hdr *)buffer;
		hdr->type = type;

		switch (type) {
		case QDSS_TRACE_START:
		case QDSS_TRACE_CONFIG_DOWNLOAD:
			hdr->len = 0;
			break;
		case QDSS_TRACE_STOP:
			{
				struct cnss_cli_qdss_trace_stop_data data;
				char *pend;

				hdr->len = sizeof(data);
				data.option = strtoull(token[1], &pend, 16);
				cnss_cli_data = (char *)hdr +
					sizeof(struct cnss_cli_msg_hdr);
				memcpy(cnss_cli_data, &data, sizeof(data));
			}
			break;
		default:
			goto out;
		}

		send_cmd_to_daemon_and_check_resp(AF_UNIX, buffer, resp_buffer);
	}
out:
	return 0;
}
