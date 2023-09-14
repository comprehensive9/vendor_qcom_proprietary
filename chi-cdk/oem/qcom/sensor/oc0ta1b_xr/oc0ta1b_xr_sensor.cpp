////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxsensordriverapi.h"
#include "android/log.h"
// NOWHINE ENTIRE FILE

//#undef OC0TA1B_XR_DBG
//#define OC0TA1B_XR_DBG(fmt, ...) __android_log_print(ANDROID_LOG_ERROR,"OC0TA1B_XR",fmt, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain)
{
    DOUBLE real_gain;
    real_gain = (DOUBLE) (((DOUBLE)(registerGain))/16.0);
    return real_gain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT RealToRegisterGain(
    DOUBLE realGain)
{
    UINT reg_gain = 0;
    realGain = realGain*16.0;
    reg_gain = (UINT)realGain;
    return reg_gain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CalculateDigitalGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT CalculateDigitalGain(
    FLOAT totalRealGain,
    FLOAT sensorRealGain)
{
    CDK_UNUSED_PARAM(totalRealGain);
    CDK_UNUSED_PARAM(sensorRealGain);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CalculateExposure(
    SensorExposureInfo*          pExposureInfo,
    SensorCalculateExposureData* pCalculateExposureData)
{
    if (NULL == pExposureInfo || NULL == pCalculateExposureData)
    {
        return FALSE;
    }

    pExposureInfo->analogRegisterGain   = RealToRegisterGain(pCalculateExposureData->realGain);
    pExposureInfo->analogRealGain       = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
    pExposureInfo->digitalRegisterGain  =
        CalculateDigitalGain(pCalculateExposureData->realGain, pExposureInfo->analogRealGain);
    pExposureInfo->digitalRealGain      = 1.0;
    pExposureInfo->ISPDigitalGain       =
        pCalculateExposureData->realGain /(pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
    pExposureInfo->lineCount            = pCalculateExposureData->lineCount;
    pExposureInfo->shortRegisterGain    = RealToRegisterGain(pCalculateExposureData->shortRealGain);
    pExposureInfo->shortLinecount       = pCalculateExposureData->shortLinecount;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FillExposureSettings(
    RegSettingsInfo*        pRegSettingsInfo,
    SensorFillExposureData* pExposureData)
{
    UINT32  index     = 0;
    INT32   offset    = 0;
    UINT16  regCount  = 0;
    UINT32  strobeStartLines = 0;
    UINT32  strobeWidthLines = 0;
    UINT32  strobeStartOffsetLines2 = 0;

    if ((NULL == pRegSettingsInfo) || (NULL == pExposureData))
    {
        return FALSE;
    }

    // toggle LED strobe on/off based on per-frame controls
    if((NULL != pExposureData->pStrobeInfo) && (FALSE == pExposureData->pStrobeInfo->strobeOff))
    {
        // measured constants to ensure flash at full intensity for entire exposure
        const UINT16 lineFactor = 129; // 1000us = 129 lines
        const INT32 strobeStartOffsetLines = (((pExposureData->pStrobeInfo->strobeRampTimeNs / 1000) * lineFactor) / 1000);
        strobeStartLines = (INT)pExposureData->frameLengthLines -
                           (INT)pExposureData->lineCount - 7 - strobeStartOffsetLines;
        strobeWidthLines = strobeStartOffsetLines + (((pExposureData->pStrobeInfo->strobeWidthNs / 1000) * lineFactor) / 1000);
        strobeStartOffsetLines2 = strobeStartOffsetLines;
    }

    for (index = 0; index < pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount; index++)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].registerAddr;
        pRegSettingsInfo->regSetting[regCount].registerData =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].registerData;
        pRegSettingsInfo->regSetting[regCount].regAddrType  =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].regAddrType;
        pRegSettingsInfo->regSetting[regCount].regDataType  =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].regDataType;
        pRegSettingsInfo->regSetting[regCount].delayUs      =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].delayUs;
        pRegSettingsInfo->regSetting[regCount].operation=IOOperationTypeWrite;
        regCount++;
    }
    offset = pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((pExposureData->frameLengthLines >> 8) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr - 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((pExposureData->lineCount >> 16) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((pExposureData->lineCount >> 8) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((pExposureData->lineCount) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = pExposureData->analogRegisterGain >> 4;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0x0F);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->strobeWidthAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((strobeWidthLines >> 16) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->strobeWidthAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((strobeWidthLines >> 8) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->strobeWidthAddr + 2;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((strobeWidthLines) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->strobeStartAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((strobeStartLines >> 8) & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->strobeStartAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = ((strobeStartLines) & 0xFF);
    regCount++;

    for (index = offset; index < regCount; index++)
    {
        pRegSettingsInfo->regSetting[index].regAddrType  = I2CRegAddressDataTypeWord;
        pRegSettingsInfo->regSetting[index].regDataType  = I2CRegAddressDataTypeByte;
        pRegSettingsInfo->regSetting[index].delayUs      = 0;
        pRegSettingsInfo->regSetting[regCount].operation=IOOperationTypeWrite;
    }
    for (index = 0; index < pExposureData->pRegInfo->groupHoldOffSettings.regSettingCount; index++)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerAddr;
        pRegSettingsInfo->regSetting[regCount].registerData  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerData;
        pRegSettingsInfo->regSetting[regCount].regAddrType  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regAddrType;
        pRegSettingsInfo->regSetting[regCount].regDataType=
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regDataType;
        pRegSettingsInfo->regSetting[regCount].delayUs      =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].delayUs;
        pRegSettingsInfo->regSetting[regCount].operation=IOOperationTypeWrite;
        regCount++;
    }

    pRegSettingsInfo->regSettingCount = regCount;

    //OC0TA1B_XR_DBG("%s | lineCount = %d | strobeStartOffsetLines2 = %d | strobeStartLines = %d | strobeWidthLines = %d | offset = %d | analogRegisterGain = %d", __func__, pExposureData->lineCount, strobeStartOffsetLines2, strobeStartLines, strobeWidthLines, offset, pExposureData->analogRegisterGain);

    if (MAX_REG_SETTINGS <= regCount)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetSensorLibraryAPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetSensorLibraryAPIs(
    SensorLibraryAPI* pSensorLibraryAPI)
{
    pSensorLibraryAPI->majorVersion          = 1;
    pSensorLibraryAPI->minorVersion          = 0;
    pSensorLibraryAPI->pCalculateExposure    = CalculateExposure;
    pSensorLibraryAPI->pFillExposureSettings = FillExposureSettings;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
