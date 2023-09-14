/* Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

// This is an auto-generated file from input/cmdRSTcalHandler.s
#ifndef _CMDRSTCALHANDLER_H_
#define _CMDRSTCALHANDLER_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct rst_parms {
    A_UINT32	calTxGain;
    A_UINT32	forcedRXIdx;
    A_INT32	dacGain;
    A_UINT8	rstDir;
    A_UINT8	phyId;
    A_UINT16	freq;
} __ATTRIB_PACK CMD_RST_PARMS;

typedef struct rstrsp_parms {
    A_INT32	rssi;
} __ATTRIB_PACK CMD_RSTRSP_PARMS;

typedef void (*RST_OP_FUNC)(void *pParms);
typedef void (*RSTRSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initRSTOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL RSTOp(void *pParms);

void* initRSTRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL RSTRSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDRSTCALHANDLER_H_
