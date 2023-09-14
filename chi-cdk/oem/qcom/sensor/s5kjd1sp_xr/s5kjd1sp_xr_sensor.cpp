////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxsensordriverapi.h"
// NOWHINE ENTIRE FILE
#include <android/log.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define S5KJD1SP_DBG(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "S5KJD1SP", fmt, ##args)
#define MINVALUE(Value1, Value2) (Value1 > Value2 ? Value2 : Value1)
#define MAXVALUE(Value1, Value2) (Value1 < Value2 ? Value2 : Value1)
#define VERTICAL_OFFSET(resolutionIndex) (resolutionIndex == 0 ? 5:10) // Vertical offset for Full Size Mode is 5
                                                                       // Vertical offset for Summation and SHDR modes are 10
#define MIN_LINE_COUNT(resolutionMode) ((SensorResolutionMode::SHDR == resolutionMode)? 9:5)
                                                                   // Min LineCount for Full Size and Summation Mode is 5
                                                                   // Min LineCount for SHDR Mode is 10
#define ROUNDUP6(Value) (Value % 6 == 0)? Value : (Value + (6 - (Value % 6)))

static const UINT   S5KJD1SPMinRegisterGain          = 32;                                    ///< Minimum analog register gain
static const UINT   S5KJD1SPMaxRegisterGain          = 2048;                                   ///< Maximum analog register gain
static const UINT   S5KJD1SPMaxRegisterGainFullSize  = 512;                                   ///< Maximum analog register gain
static const DOUBLE S5KJD1SPMinRealGain              = (S5KJD1SPMinRegisterGain/32);          ///< Minimum analog real gain (1X)
static const DOUBLE S5KJD1SPMaxRealGain              = (S5KJD1SPMaxRegisterGain/32);          ///< Maximum analog real gain (64X)
static const DOUBLE S5KJD1SPMaxRealGainFullSize      = (S5KJD1SPMaxRegisterGainFullSize/32);  ///< Maximum analog real gain (16X)
static const UINT   S5KJD1SPMinDigitalRegisterGain   = 256;                                   ///< Minimum digital register gain (1x)
static const UINT   S5KJD1SPMaxDigitalRegisterGain   = 4096;                                  ///< Maximum digital register gain (16x)
static const UINT   S5KJD1SPDigitalGainDecimator     = 256;                                   /// < Digital gain decimator factor
static const DOUBLE S5KJD1SPMinDigitalRealGain       = (S5KJD1SPMinDigitalRegisterGain / S5KJD1SPDigitalGainDecimator);
                                                                                          ///< Minimum digital real gain (1X)
static const DOUBLE S5KJD1SPMaxDigitalRealGain       = (S5KJD1SPMaxDigitalRegisterGain/ S5KJD1SPDigitalGainDecimator);
                                                                                          ///< Maximum digital real gain (16X)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain)
{
    if(S5KJD1SPMaxRegisterGain < registerGain)
    {
        registerGain = S5KJD1SPMaxRegisterGain;
    }

    return ((float)registerGain/S5KJD1SPMinRegisterGain);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT RealToRegisterGain(
    DOUBLE realGain)
{
    if (S5KJD1SPMinRealGain > realGain)
    {
        realGain = S5KJD1SPMinRealGain;
    }
    else if (S5KJD1SPMaxRealGain < realGain)
    {
        realGain = S5KJD1SPMaxRealGain;
    }

    return static_cast<UINT>(realGain*S5KJD1SPMinRegisterGain);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CalculateDigitalGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT CalculateDigitalGain(
    FLOAT totalRealGain,
    FLOAT sensorRealGain)
{
    float digitalRealGain = S5KJD1SPMinDigitalRealGain;

    if (S5KJD1SPMaxRealGain < totalRealGain)
    {
        digitalRealGain = totalRealGain / sensorRealGain;
    }
    else
    {
        digitalRealGain = S5KJD1SPMinDigitalRealGain;
    }

    if (S5KJD1SPMaxDigitalRealGain < digitalRealGain)
    {
        digitalRealGain = S5KJD1SPMaxDigitalRealGain;
    }

    return static_cast<UINT>(digitalRealGain * S5KJD1SPDigitalGainDecimator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VerifyAnalogGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID VerifyAnalogGain(
    UINT*  analogRegisterGain,
    FLOAT* analogRealGain,
    FLOAT  realGain,
    UINT*  lineCount,
    UINT   aecLineCount,
    UINT32 sensorResolutionIndex)
{
    *lineCount = aecLineCount;

    switch (sensorResolutionIndex)
    {
        /* For fullsize mode S5KJD1SP only accepts real analog gains
        in the range of 3dB to 18dB. Please refer sensor specifications
        for more details
        */
        case 0:
        case 1:
            if (S5KJD1SPMaxRegisterGainFullSize < *analogRegisterGain)
            {
                *analogRegisterGain   = S5KJD1SPMaxRegisterGainFullSize;
                *analogRealGain       = S5KJD1SPMaxRealGainFullSize;
            }
            else if (S5KJD1SPMinRegisterGain > *analogRegisterGain)
            {
                *analogRegisterGain   = S5KJD1SPMinRegisterGain;
                *analogRealGain       = S5KJD1SPMinRealGain;
                *lineCount            = (aecLineCount) * (realGain) / S5KJD1SPMinRealGain;
            }
            else
            {
                *analogRealGain = static_cast<FLOAT>(RegisterToRealGain(*analogRegisterGain));
            }
            break;

        default:
            *analogRealGain = static_cast<FLOAT>(RegisterToRealGain(*analogRegisterGain));
            break;
    }
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
    pExposureInfo->digitalRegisterGain  = CalculateDigitalGain(pCalculateExposureData->realGain, pExposureInfo->analogRealGain);
    pExposureInfo->digitalRealGain      = static_cast<FLOAT>(pExposureInfo->digitalRegisterGain/
        static_cast<FLOAT>(S5KJD1SPDigitalGainDecimator));
    pExposureInfo->ISPDigitalGain       =
        pCalculateExposureData->realGain / (pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
    pExposureInfo->lineCount            = pCalculateExposureData->lineCount;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FillExposureSettings(
    RegSettingsInfo*        pRegSettingsInfo,
    SensorFillExposureData* pExposureData)
{
    BOOL   result   = FALSE;
    UINT32 index    = 0;
    UINT16 regCount = 0;

    if ((NULL != pRegSettingsInfo) && (NULL != pExposureData))
    {
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
            regCount++;
        }

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0x00FF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr+ 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->GlobalDigitalGainAddr;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->GlobalDigitalGainAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF);
        regCount++;

        for (index = 0; (pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index) < regCount; index++)
        {
            pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].regAddrType =
                I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].regDataType =
                I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].delayUs     =
                0;
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
            regCount++;
        }

        pRegSettingsInfo->regSettingCount = regCount;

        result = TRUE;
    }

    if ((TRUE == result) && (MAX_REG_SETTINGS <= regCount))
    {
        result = FALSE;
    }

    return result;
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
