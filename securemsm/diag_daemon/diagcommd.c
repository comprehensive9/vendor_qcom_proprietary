/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        HLOS DiagComm Daemon

GENERAL DESCRIPTION
        DiagComm Daemon setup a call back function with diag module that is used
for interacting with Qseecom and secure world.

EXTERNALIZED FUNCTIONS
        None

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <msg.h>
#include <diag.h>
#include <diagcmd.h>
#include <diagcomm.h>


/* Callback declaration for FUSE packet processing */
void* fuse_diag_dispatch(void* req_pkt, uint16_t len);

/* Diag pkt table for FUSE */
static const diagpkt_user_table_entry_type fuse_diag_func_table[] =
{
  {DIAG_CMD_C,DIAG_CMD_C, fuse_diag_dispatch}
};


/*===========================================================================
FUNCTION   fuse_diag_dispatch

DESCRIPTION
  Processes the request packet and sends it to the QSEECOM for further
  processing

DEPENDENCIES
  NIL

RETURN VALUE
  pointer to DIAG FUSE Response packet

SIDE EFFECTS
  None

===========================================================================*/
void* fuse_diag_dispatch ( void* req_pkt, uint16_t len)
{
	void* rsp = NULL;
	LOGD("Receiving one command from DIAG\n");

	rsp = (void *)fuse_dispatch(req_pkt, len);

	LOGD("command is executed \n");

	return rsp;
}


/*===========================================================================
FUNCTION   main

DESCRIPTION
  Initialises the Diag daemon and registers the PKT table for FUSE REQUEST
  and daemonises

DEPENDENCIES
  NIL

RETURN VALUE
  NIL, Error in the event buffer will mean a NULL App version and Zero HW
  version

SIDE EFFECTS
  None

===========================================================================*/



int main() {
	boolean bInit_Success = FALSE;
	LOGD("Diag Daemon services process entry PPID = %d", getppid());

	signal(SIGPIPE, SIG_IGN);


	LOGD("Calling LSM init \n");

	bInit_Success = Diag_LSM_Init(NULL);

	if(!bInit_Success)
	{
		LOGE("DiagDaemon: Diag_LSM_Init() failed.\n");
	        return -1;
	}

	LOGD("Diag_LSM_Init succeeded.\n");
	DIAGPKT_DISPATCH_TABLE_REGISTER ( DIAG_SUBSYS_SEC ,
                        fuse_diag_func_table );


	LOGD("Diag DAEMON RUNNING ");
	pause();
	LOGD("Diag Daemon services process exiting!!! ");
	// Clean up before exiting
	Diag_LSM_DeInit();
	return -1;
}
