/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Debug Application for Diag-router

GENERAL DESCRIPTION
  Contains main implementation of Diagnostic Services Debug Application.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <ctype.h>
#ifdef FEATURE_LE_DIAG
#include <fcntl.h>
#endif
#include "errno.h"
#include "event.h"
#include "msg.h"
#include "log.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"
#include "diag_lsm.h"
#include "diag_lsmi.h"
#include "diag_shared_i.h"
#include "ts_linux.h"
#include <limits.h>
#include "diag_lsm_comm.h"

struct diag_cmd_dbg {
        unsigned int first;
        unsigned int last;
        int periph_id;
};

static void usage(void)
{
	fprintf(stderr,
		"User space application for diag interface\n"
		"\n"
		"usage: debug-diag [-hd]\n"
		"\n"
		"options:\n"
		"   -h   show parameter usage\n"
		"   -d   <dynamic debug mask to enable diag-router logs>\n"
		"        < 0  - disable logs>\n"
		"        < 1  - Information logs>\n"
		"        < 2  - USB Rx logs>\n"
		"        < 3  - USB tx logs>\n"
		"        < 4  - ctrl logs>\n"
		"        < 16 - cmd logs>\n"
		"        < 32 - data logs>\n"
		"        < 64 - watch logs>\n"
	       );

	exit(1);
}

int main(int argc, char **argv)
{
	int c, err;
	int diag_fd_dbg = DIAG_INVALID_HANDLE;
	uint32_t dyn_debug_mask = 0;
	boolean bInit_Success = FALSE;
	(void)argc;
	(void)argv;

	diag_fd_dbg = open("/dev/diag", O_RDWR);
	if (diag_fd_dbg > 0) {
		printf("Diag kernel support is present so exiting...\n");
		close(diag_fd_dbg);
		exit(0);
	} else {
		bInit_Success = Diag_LSM_Init(NULL);
		if (!bInit_Success) {
			DIAG_LOGE("\nDiag_LSM_Init() failed. Exiting...\n");
			exit(0);
		}
		diag_fd_dbg = diag_fd;
	}

	for (;;) {
		c = getopt(argc, argv, "hd:");
		if (c < 0)
			break;
		switch (c) {
		case 'd':
			dyn_debug_mask = atoi(optarg);
			err = diag_lsm_comm_ioctl(diag_fd_dbg, DIAG_IOCTL_UPDATE_DYN_DBG_MASK, &dyn_debug_mask, 0);
			if (err) {
				printf("debug: Error in calling DIAG_IOCTL_UPDATE_DYN_DBG_MASK exiting...\n");
				goto exit;
			}
			break;
		default:
		case 'h':
			usage();
			break;
		}
	}

	err = diag_lsm_comm_ioctl(diag_fd_dbg, DIAG_IOCTL_QUERY_REG_TABLE, NULL, 0);
	if (err) {
		printf("debug: Error in calling DIAG_IOCTL_QUERY_REG_TABLE exiting...\n");
		goto exit;
	}

	err = diag_lsm_comm_ioctl(diag_fd_dbg, DIAG_IOCTL_QUERY_DEBUG_ALL, NULL, 0);
	if (err) {
		printf("debug: Error in calling DIAG_IOCTL_QUERY_DEBUG_ALL exiting...\n");
		goto exit;
	}

exit:
	if (diag_fd_dbg != DIAG_INVALID_HANDLE)
		Diag_LSM_DeInit();
	diag_fd_dbg = DIAG_INVALID_HANDLE;

	return 0;
}
