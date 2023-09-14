/**
 * Copyright (c) 2014-2016,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef SLIM_PROCESSOR_H
#define SLIM_PROCESSOR_H
/*!
@file
@brief SLIM processor-specific types and declarations.

  The content of this file is dependent on the host processor. For proper
  operation the following items shall be defined:
  - #SLIM_MSG_MED0
  - #SLIM_MSG_MED1
  - #SLIM_MSG_MED2
  - #SLIM_MSG_MED
  - #SLIM_MSG_HIGH0
  - #SLIM_MSG_HIGH1
  - #SLIM_MSG_HIGH2
  - #SLIM_MSG_HIGH
  - #SLIM_MSG_ERROR0
  - #SLIM_MSG_ERROR1
  - #SLIM_MSG_ERROR2
  - #SLIM_MSG_ERROR
  - #SLIM_CLIENT_MAX_COUNT
  - #SLIM_TASK_THREAD_ID
  - #SLIM_HANDLE_L
  - #slim_MutexStructType
  - #slim_ClientEnumType
  - #slim_ProviderEnumType

  QMI-SLIM requires the following definitions:
  - #SLIM_MSG0
  - #SLIM_MSG1
  - #SLIM_MSG2
  - #SLIM_MSG

@dependencies
- MSG_SSID_SLIM -- When defined, SLIM SSID is used for diagnostics logging.
- FEATURE_CGPS_LBS_TASK -- If MSG_SSID_SLIM is undefined, MSG_SSID_GPSSM is used
                           for logging if this variable is defined; otherwise
                           MSG_SSID_GNSS_LOCMW is used.
                           Sets task identifier to THREAD_ID_LBS
- FEATURE_CGPS_LOC_MIDDLEWARE -- SLIM task id is THREAD_ID_LOC_MIDDLEWARE

@ingroup slim_MPSS
*/
/* $Header: //components/rel/gnss8.mpss/10.1/gnss/slim/mpss/inc/slim_processor.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <loc_pla.h>
#include <msg.h>
#include <slim_os_api.h>
#include <slim_os_log_api.h>

#ifdef __cplusplus
extern "C" {
#endif

//! @əddtogroup slim_APSS
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
@def SLIM_MSG0
@brief Logs message
@param[in] LEVEL Message logging level
@param[in] str   Message to log

@def SLIM_MSG1
@brief Logs message with single argument
@param[in] LEVEL Message logging level
@param[in] str   Formatting string
@param[in] a     Argument.

@def SLIM_MSG2
@brief Logs message with two arguments
@param[in] LEVEL Message logging level
@param[in] str   Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG
@brief Logs message with three arguments
@param[in] LEVEL Message logging level
@param[in] str   Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_MSG_MED0
@brief Logs medium priority message
@param[in] str Message to log.

@def SLIM_MSG_MED1
@brief Logs medium priority message with single argument.
@param[in] str Formatting string
@param[in] a     Argument.

@def SLIM_MSG_MED2
@brief Logs medium priority message with two arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG_MED
@brief Logs medium priority message with three arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_MSG_HIGH0
@brief Logs high priority message
@param[in] str Message to log.

@def SLIM_MSG_HIGH1
@brief Logs high priority message with single argument.
@param[in] str Formatting string
@param[in] a     Argument.

@def SLIM_MSG_HIGH2
@brief Logs high priority message with two arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG_HIGH
@brief Logs high priority message with three arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_MSG_ERROR0
@brief Logs error priority message
@param[in] str Message to log.

@def SLIM_MSG_ERROR1
@brief Logs error priority message with single argument.
@param[in] str Formatting string
@param[in] a     Argument.

@def SLIM_MSG_ERROR2
@brief Logs error priority message with two arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG_ERROR
@brief Logs error priority message with three arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_TASK_THREAD_ID
@brief SLIM task identifier for IPC message routing.

Depending on the target build, SLIM can be run as a part of LOC_MW or LBS task.
@sa THREAD_ID_LOC_MIDDLEWARE THREAD_ID_LBS

*/

#define SLIM_MSG(LEVEL, str, a, b, c) \
  SLIM_LOG3(SLIM_LOG_MOD_SLIM_NK,LEVEL,str,a,b,c)
#define SLIM_MSG0(LEVEL, str) \
  SLIM_LOG0(SLIM_LOG_MOD_SLIM_NK,LEVEL,str)
#define SLIM_MSG1(LEVEL, str, a) \
  SLIM_LOG3(SLIM_LOG_MOD_SLIM_NK,LEVEL,str,a,0,0)
#define SLIM_MSG2(LEVEL, str, a, b) \
  SLIM_LOG3(SLIM_LOG_MOD_SLIM_NK,LEVEL,str,a,b,0)

#define SLIM_MSG_LOW0(str)              SLIM_MSG0(SLIM_LOG_VERBOSE, str)
#define SLIM_MSG_LOW1(str, a)           SLIM_MSG1(SLIM_LOG_VERBOSE, str, a)
#define SLIM_MSG_LOW2(str, a, b)        SLIM_MSG2(SLIM_LOG_VERBOSE, str, a, b)
#define SLIM_MSG_LOW(str, a, b, c)      SLIM_MSG(SLIM_LOG_VERBOSE, str, a, b, c)
#define SLIM_MSG_MED0(str)              SLIM_MSG0(SLIM_LOG_DEBUG, str)
#define SLIM_MSG_MED1(str, a)           SLIM_MSG1(SLIM_LOG_DEBUG, str, a)
#define SLIM_MSG_MED2(str, a, b)        SLIM_MSG2(SLIM_LOG_DEBUG, str, a, b)
#define SLIM_MSG_MED(str, a, b, c)      SLIM_MSG(SLIM_LOG_DEBUG, str, a, b, c)
#define SLIM_MSG_HIGH0(str)             SLIM_MSG0(SLIM_LOG_INFO, str)
#define SLIM_MSG_HIGH1(str, a)          SLIM_MSG1(SLIM_LOG_INFO, str, a)
#define SLIM_MSG_HIGH2(str, a, b)       SLIM_MSG2(SLIM_LOG_INFO, str, a, b)
#define SLIM_MSG_HIGH(str, a, b, c)     SLIM_MSG(SLIM_LOG_INFO, str, a, b, c)
#define SLIM_MSG_ERROR0(str)            SLIM_MSG0(SLIM_LOG_ERROR, str)
#define SLIM_MSG_ERROR1(str, a)         SLIM_MSG1(SLIM_LOG_ERROR, str, a)
#define SLIM_MSG_ERROR2(str, a, b)      SLIM_MSG2(SLIM_LOG_ERROR, str, a, b)
#define SLIM_MSG_ERROR(str, a, b, c)    SLIM_MSG(SLIM_LOG_ERROR, str, a, b, c)

/*!
@brief Maximum SLIM client count

SLIM has a hard limit of number of supported clients. The total number shall
be maximum of simultaneously opened client connections including:
- local connections (SAML, SAP)
- remote connections through QMI-SLIM (SLIM-AP)
*/
#define SLIM_CLIENT_MAX_COUNT (SLIM_CLIENT_LAST + 10)
/*! @brief Double precision floating point */
#define DBL double
/*! @brief Single precision floating point */
#define FLT float

#define SLIM_TASK_THREAD_ID THREAD_ID_SLIM
/**
@brief Convert pointer to DIAG argument

This macro handles proper handle conversion from pointer type to uint32. It is
used for logging.

@param[in] pHandle Client handle.
*/
#define SLIM_HANDLE_L(pHandle)  ((uint32)(uintptr_t)(pHandle))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
@brief SLIM mutex in APSS

SLIM requires locking in case when multiple tasks or threads are used. The lock
type must support recursion.
*/
typedef os_MutexBlockType slim_MutexStructType;

/**
@brief Enum for SLIM clients in AP.

The enumeration has no mandatory item, however #SLIM_CLIENT_LAST is used for
#SLIM_PROVIDER_COUNT definition.

@sa SLIM_PROVIDER_COUNT
*/
typedef enum
{
  SLIM_CLIENT_MPSS  = 1,    /**< Client id for Modem clients (QMI-LOC) */
  SLIM_CLIENT_QMISLIM_MP,   /**< Client id for Modem clients (QMI-SLIM) */
  SLIM_CLIENT_QMISLIM_AP,   /**< Client id for AP clients (QMI-SLIM) */
  SLIM_CLIENT_QMISLIM_TEST, /**< Client id for test clients (QMI-SLIM) */
  SLIM_CLIENT_INTERNAL,     /**< Client id for internal clients */
  SLIM_CLIENT_LAST = SLIM_CLIENT_INTERNAL, /**< Last client handle enum
                                                    value in use */

  //! @cond
  SLIM_CLIENT_MAX  = 2147483647 /* To force a 32 bit signed enum.
                                   Do not change or use */
  //! @endcond
} slim_ClientEnumType;

/**
@brief Enum for SLIM Service Providers

Values should correspond to array indexes in providers array in provider
registry.

The first provider must have a value of 0, and the following elements are
mandatory:
- SLIM_PROVIDER_NONE -- Shall be equal to -1
- SLIM_PROVIDER_COUNT -- Shall be equal to total number of providers (last + 1)
*/
typedef enum
{
  SLIM_PROVIDER_NONE = -1,     /**< Invalid */
  SLIM_PROVIDER_SENSOR1 = 0,   /**< Sensor1 API */
  SLIM_PROVIDER_NDK,           /**< Android NDK */
  SLIM_PROVIDER_QMISLIM_MPSS,  /**< Modem (QMI-SLIM) provider */
  SLIM_PROVIDER_QMISLIM_TEST,  /**< Test (QMI-SLIM) provider */
  SLIM_PROVIDER_VNW,           /**< Vehicle Network provider */
  SLIM_PROVIDER_SENSOR_HAL,     /**< Sensor HAL provider */
  SLIM_PROVIDER_VNS,           /**< Vehicle Network Services (N car) provider */
  SLIM_PROVIDER_RP,            /**< Slim Remote provider */
  SLIM_PROVIDER_SENSOR_PLAYBACK, /**< Sensor PLAYBACK provider */
  SLIM_PROVIDER_COUNT,         /**< Required value for SLIM */
  //! @cond
  SLIM_PROVIDER_MAX     = 2147483647 /* To force a 32 bit signed enum.
                                        Do not change or use */
  //! @endcond
} slim_ProviderEnumType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#if defined(__ANDROID__) || defined(USE_GLIB)
inline uint64_t slim_GetQTimerTickCount()
{
    uint64_t qTimerCount = 0;

    #if __aarch64__
        asm volatile("mrs %0, cntvct_el0" : "=r" (qTimerCount));
    #elif defined (__i386__) || defined (__x86_64__)
        /* Qtimer not supported in x86 architecture */
        qTimerCount = 0;
    #else
        asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (qTimerCount));
    #endif

    return qTimerCount;
}

inline uint64_t slim_GetQTimerFrequency()
{
    uint64_t qTimerFreq = 0;

    #if __aarch64__
        asm volatile("mrs %0, cntfrq_el0" : "=r"(qTimerFreq));
    #else
        qTimerFreq =  19200000LL; //19.2 MHz TBD: find right asm instruction
    #endif

    return qTimerFreq;
}

#endif

#ifdef __cplusplus
}
#endif

//! @}

#endif /* #ifndef SLIM_PROCESSOR_H */
