/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diag Legacy Service Mapping DLL

GENERAL DESCRIPTION

Implementation of entry point and Initialization functions for Diag_LSM.dll.


EXTERNALIZED FUNCTIONS
DllMain
Diag_LSM_Init
Diag_LSM_DeInit

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2011,2015,2017-2018,2020, 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header:

when       who    what, where, why
--------   ---    ----------------------------------------------------------
10/01/08   SJ     Created

===========================================================================*/

#include "ts_linux.h"
#include "string.h"
#include "errno.h"
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include "diag_lsm.h"

#define TIME_SYNC_PARAM "/sys/module/diagchar/parameters/timestamp_switch"
#define DIAG_MODEM_EPOCH_DIFFERENCE     315964800
#ifdef DIAG_DISABLE_TIME_SERVICES
/* Dividend of factor to multiply for converting sclk to cx32 */
#define TIME_CONV_SCLK_CX32_DIVIDEND   256
/* Divisor of factor to multiply for converting sclk to cx32 */
#define TIME_CONV_SCLK_CX32_DIVISOR    125
/* Conversion from cx32 to 1.25ms */
#define TIME_CONV_CX32_PER_1p25MS      0xC000

static unsigned long long time_get_from_timetick(void)
{
	unsigned long long seconds = 0, ticks_qt = 0;
	unsigned long long cx32 = 0, cx32_xos = 0, xos = 0, timestamp = 0, ticks_1_25ms = 0;

#if __aarch64__
	asm volatile("mrs %0, cntvct_el0" : "=r" (ticks_qt));
#else
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (ticks_qt));
#endif
	xos = ticks_qt;
	cx32_xos = (xos * TIME_CONV_SCLK_CX32_DIVIDEND)/TIME_CONV_SCLK_CX32_DIVISOR;
	/* Add the number of 1.25ms units to the timestamp */
	cx32 += cx32_xos;
	ticks_1_25ms = cx32/TIME_CONV_CX32_PER_1p25MS;
	timestamp += (ticks_1_25ms << 16);
	/* Determine the remaining cx32s that we didn't add to the timestamp's upper part */
	cx32 -= (ticks_1_25ms * TIME_CONV_CX32_PER_1p25MS);
	/* Attach the cx32 back to end of the timestamp */
	timestamp |= cx32&0xffffULL;
	seconds = timestamp;

	return seconds;
}
#else
extern unsigned long long time_get_from_timetick(void);
#endif /* DIAG_NO_TIME_SERVICES */

extern int time_stamp_switch_rcvd;
extern pthread_mutex_t timestamp_switch_mutex;

static unsigned long long get_time_of_day(void)
{
	struct timeval tv;
	unsigned long long seconds, microseconds;

	gettimeofday(&tv, NULL);
	seconds = (unsigned long long)tv.tv_sec;
	/* Offset to sync timestamps between Modem & Apps Proc.
	Number of seconds between Jan 1, 1970 & Jan 6, 1980 */
	if (seconds >= DIAG_MODEM_EPOCH_DIFFERENCE)
               seconds = seconds - DIAG_MODEM_EPOCH_DIFFERENCE;

	seconds = (unsigned long long)(seconds * 1000);
	microseconds = (unsigned long long)tv.tv_usec;
	microseconds = microseconds/1000;
	seconds = seconds + microseconds;
	seconds = seconds*4;
	seconds = seconds/5;
	seconds = seconds << 16;
	return seconds;
}

/*===========================================================================

FUNCTION    ts_get

DESCRIPTION
  This extracts time from the system and feeds the pointer passed in.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/

void ts_get (void *timestamp)
{
		char *temp1;
		char *temp2;
		unsigned long long seconds;
		int i, fd_param = -1, ret, time_stamp_switch = 0;
		int time_sync_param[1];
		time_sync_param[0] = 0;
		if (diag_use_dev_node) {
			fd_param = open(TIME_SYNC_PARAM, O_RDONLY);
			if (fd_param < 0) {
				DIAG_LOGE("ts_get: could not open file: %s\n", strerror(errno));
				goto default_ts;
			}
			ret = read(fd_param, time_sync_param, 1);
                	if (ret < 0) {
                        	DIAG_LOGE("ts_get: Unable to read file: %s\n", strerror(errno));
				close(fd_param);
                        	goto default_ts;
                	}
			close(fd_param);
			time_stamp_switch = time_sync_param[0] - '0';
		} else {
			pthread_mutex_lock(&timestamp_switch_mutex);
			time_stamp_switch = time_stamp_switch_rcvd;
			pthread_mutex_unlock(&timestamp_switch_mutex);
		}

		switch (time_stamp_switch) {
		case 0:
		default:
			seconds = get_time_of_day();
			break;
		case 1:
			seconds = time_get_from_timetick();
			break;
		}
		temp1 = (char *) (timestamp);
		temp2 = (char *) &(seconds);
		/*
		 * This is assuming that you have 8 consecutive
		 * bytes to store the data.
		 */
		for (i=0;i<8;i++)
			*(temp1+i) = *(temp2+i);
		return;
default_ts:
	seconds = get_time_of_day();
	temp1 = (char *) (timestamp);
        temp2 = (char *) &(seconds);
	/*
         * This is assuming that you have 8 consecutive
         * bytes to store the data.
         */
        for (i=0;i<8;i++)
		*(temp1+i) = *(temp2+i);
        return;
}     /* ts_get */

/*===========================================================================
FUNCTION   ts_get_lohi

DESCRIPTION
  Extracts timestamp from system and places into lo and hi parameters

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
void ts_get_lohi(uint32 *ts_lo, uint32 *ts_hi)
{
	char buf[8];

	ts_get(buf);
	*ts_lo = *(uint32 *)&buf[0];
	*ts_hi = *(uint32 *)&buf[4];

	return;
}

