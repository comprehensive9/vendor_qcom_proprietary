/* Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

// This is an auto-generated file from input/cmdRSTcalHandler.s
#include "tlv2Inc.h"
#include "cmdRSTcalHandler.h"

void* initRSTOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_RST_PARMS  *pRSTParms = (CMD_RST_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pRSTParms->calTxGain = pParmDict[PARM_CALTXGAIN].v.valU32;
    pRSTParms->forcedRXIdx = pParmDict[PARM_FORCEDRXIDX].v.valU32;
    pRSTParms->dacGain = pParmDict[PARM_DACGAIN].v.valS32;
    pRSTParms->rstDir = pParmDict[PARM_RSTDIR].v.valU8;
    pRSTParms->phyId = pParmDict[PARM_PHYID].v.valU8;
    pRSTParms->freq = pParmDict[PARM_FREQ].v.valU16;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_CALTXGAIN, (A_UINT32)(((A_UINT32)&(pRSTParms->calTxGain)) - (A_UINT32)pRSTParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_FORCEDRXIDX, (A_UINT32)(((A_UINT32)&(pRSTParms->forcedRXIdx)) - (A_UINT32)pRSTParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_DACGAIN, (A_UINT32)(((A_UINT32)&(pRSTParms->dacGain)) - (A_UINT32)pRSTParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RSTDIR, (A_UINT32)(((A_UINT32)&(pRSTParms->rstDir)) - (A_UINT32)pRSTParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_PHYID, (A_UINT32)(((A_UINT32)&(pRSTParms->phyId)) - (A_UINT32)pRSTParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_FREQ, (A_UINT32)(((A_UINT32)&(pRSTParms->freq)) - (A_UINT32)pRSTParms), pParmsOffset);
    return((void*) pRSTParms);
}

static RST_OP_FUNC RSTOpFunc = NULL;

TLV2_API void registerRSTHandler(RST_OP_FUNC fp)
{
    RSTOpFunc = fp;
}

A_BOOL RSTOp(void *pParms)
{
    CMD_RST_PARMS *pRSTParms = (CMD_RST_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("RSTOp: calTxGain %u\n", pRSTParms->calTxGain);
    A_PRINTF("RSTOp: forcedRXIdx %u\n", pRSTParms->forcedRXIdx);
    A_PRINTF("RSTOp: dacGain %d\n", pRSTParms->dacGain);
    A_PRINTF("RSTOp: rstDir %d\n", pRSTParms->rstDir);
    A_PRINTF("RSTOp: phyId %u\n", pRSTParms->phyId);
    A_PRINTF("RSTOp: freq %u\n", pRSTParms->freq);
#endif //_DEBUG

    if (NULL != RSTOpFunc) {
        (*RSTOpFunc)(pRSTParms);
    }
    return(TRUE);
}

void* initRSTRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_RSTRSP_PARMS  *pRSTRSPParms = (CMD_RSTRSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pRSTRSPParms->rssi = pParmDict[PARM_RSSI].v.valS32;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_RSSI, (A_UINT32)(((A_UINT32)&(pRSTRSPParms->rssi)) - (A_UINT32)pRSTRSPParms), pParmsOffset);
    return((void*) pRSTRSPParms);
}

static RSTRSP_OP_FUNC RSTRSPOpFunc = NULL;

TLV2_API void registerRSTRSPHandler(RSTRSP_OP_FUNC fp)
{
    RSTRSPOpFunc = fp;
}

A_BOOL RSTRSPOp(void *pParms)
{
    CMD_RSTRSP_PARMS *pRSTRSPParms = (CMD_RSTRSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("RSTRSPOp: rssi %d\n", pRSTRSPParms->rssi);
#endif //_DEBUG

    if (NULL != RSTRSPOpFunc) {
        (*RSTRSPOpFunc)(pRSTRSPParms);
    }
    return(TRUE);
}
