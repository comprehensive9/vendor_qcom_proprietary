/*
* Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
*/
/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file */

#ifndef ANDROID_RIL_H
#define ANDROID_RIL_H 1

#ifndef _WIN32
#include <stdlib.h>
#endif // _WIN32
#include <stdint.h>
#include <telephony/ril_cdma_sms.h>
#include <telephony/ril_nv_items.h>
#include <telephony/ril_msim.h>

#ifndef FEATURE_UNIT_TEST
#ifndef _WIN32
#include <sys/time.h>
#endif // _WIN32
#endif /* !FEATURE_UNIT_TEST */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIM_COUNT
#if defined(ANDROID_SIM_COUNT_2)
#define SIM_COUNT 2
#elif defined(ANDROID_SIM_COUNT_3)
#define SIM_COUNT 3
#elif defined(ANDROID_SIM_COUNT_4)
#define SIM_COUNT 4
#else
#define SIM_COUNT 1
#endif

#ifndef ANDROID_MULTI_SIM
#define SIM_COUNT 1
#endif
#endif

/*
 * RIL version.
 * Value of RIL_VERSION should not be changed in future. Here onwards,
 * when a new change is supposed to be introduced  which could involve new
 * schemes added like Wakelocks, data structures added/updated, etc, we would
 * just document RIL version associated with that change below. When OEM updates its
 * RIL with those changes, they would return that new RIL version during RIL_REGISTER.
 * We should make use of the returned version by vendor to identify appropriate scheme
 * or data structure version to use.
 *
 * Documentation of RIL version and associated changes
 * RIL_VERSION = 12 : This version corresponds to updated data structures namely
 *                    RIL_Data_Call_Response_v11, RIL_SIM_IO_v6, RIL_CardStatus_v6,
 *                    RIL_SimRefreshResponse_v7, RIL_CDMA_CallWaiting_v6,
 *                    RIL_LTE_SignalStrength_v8, RIL_SignalStrength_v10, RIL_CellIdentityGsm_v12
 *                    RIL_CellIdentityWcdma_v12, RIL_CellIdentityLte_v12,RIL_CellInfoGsm_v12,
 *                    RIL_CellInfoWcdma_v12, RIL_CellInfoLte_v12, RIL_CellInfo_v12.
 *
 * RIL_VERSION = 13 : This version includes new wakelock semantics and as the first
 *                    strongly versioned version it enforces structure use.
 *
 * RIL_VERSION = 14 : New data structures are added, namely RIL_CarrierMatchType,
 *                    RIL_Carrier, RIL_CarrierRestrictions and RIL_PCO_Data.
 *                    New commands added: RIL_REQUEST_SET_CARRIER_RESTRICTIONS,
 *                    RIL_REQUEST_SET_CARRIER_RESTRICTIONS and RIL_UNSOL_PCO_DATA.
 *
 * RIL_VERSION = 15 : New commands added:
 *                    RIL_UNSOL_MODEM_RESTART,
 *                    RIL_REQUEST_SEND_DEVICE_STATE,
 *                    RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER,
 *                    RIL_REQUEST_SET_SIM_CARD_POWER,
 *                    RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION,
 *                    RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION
 *                    The new parameters for RIL_REQUEST_SETUP_DATA_CALL,
 *                    Updated data structures: RIL_DataProfileInfo_v15, RIL_InitialAttachApn_v15
 *                    New data structure RIL_DataRegistrationStateResponse,
 *                    RIL_VoiceRegistrationStateResponse same is
 *                    used in RIL_REQUEST_DATA_REGISTRATION_STATE and
 *                    RIL_REQUEST_VOICE_REGISTRATION_STATE respectively.
 *                    New data structure RIL_OpenChannelParams.
 *                    RIL_REQUEST_START_NETWORK_SCAN
 *                    RIL_REQUEST_STOP_NETWORK_SCAN
 *                    RIL_UNSOL_NETWORK_SCAN_RESULT
 */
#define RIL_VERSION 12
#define LAST_IMPRECISE_RIL_VERSION 12 // Better self-documented name
#define RIL_VERSION_MIN 6 /* Minimum RIL_VERSION supported */

#define CDMA_ALPHA_INFO_BUFFER_LENGTH 64
#define CDMA_NUMBER_INFO_BUFFER_LENGTH 81

#define MAX_RILDS 3
#define MAX_SERVICE_NAME_LENGTH 6
#define MAX_CLIENT_ID_LENGTH 2
#define MAX_DEBUG_SOCKET_NAME_LENGTH 12
#define MAX_QEMU_PIPE_NAME_LENGTH  11
#define MAX_UUID_LENGTH 64
#define MAX_BANDS 8
#define MAX_CHANNELS 32
#define MAX_RADIO_ACCESS_NETWORKS 8
#define MAX_MCC_MNC_LIST_SIZE 20
#define MAX_MCC_MNC_LEN 6
#define MAX_OPERATOR_NAME_LEN 512
#define MAX_RIL_MCC_MNC_SIZE 4
#define MAX_URN_LENGTH 50
#define MAX_NUMBER_OF_URNS 10
#define MAX_EMERGENCY_NUMBERS 24
#define MAX_EMERGENCY_NUMBER_LENGTH 10

/*
 * Indicate compliant IRadio HAL version
 *
 * This define is used in RIL messages translator to use
 * compatible data structure
 *
 * Version 14 is compatible up to IRadio 1.4, IRadioConfig 1.2
 *
 * Added structure:
 *     RIL_DataProfileInfo_IRadio_1_4
 *     - Used in RIL_REQUEST_SETUP_DATA_CALL
 *     - Used in RIL_REQUEST_SET_INITIAL_ATTACH_APN
 *     - Used in RIL_REQUEST_SET_DATA_PROFILE
 */
#define RIL_COMPATIBLE_IRADIO_VERSION 14

typedef void * RIL_Token;

typedef enum {
    RIL_SOCKET_1,
#if (SIM_COUNT >= 2)
    RIL_SOCKET_2,
#if (SIM_COUNT >= 3)
    RIL_SOCKET_3,
#endif
#if (SIM_COUNT >= 4)
    RIL_SOCKET_4,
#endif
#endif
    RIL_SOCKET_NUM
} RIL_SOCKET_ID;

/** @public
 * RIL_ResponseError
 */
typedef enum {
    /** NO ERROR */
    RIL_NO_ERROR = 0,
    /** NOT SUPPORTED */
    RIL_NOT_SUPPORTED,
    /** NOT AVAILABLE */
    RIL_NOT_AVAILABLE,
    /** CALL NOT AVAILABLE */
    RIL_CALL_NOT_AVAILABLE,
    /** INTERNAL ERROR */
    RIL_INTERNAL_ERROR,
    /** INVALID ARGUMENTS */
    RIL_INVALID_ARGUMENT,
}RIL_ResponseError;

/** @public
 *RIL_Errno
 */
typedef enum {
    /** SUCCESS */
    RIL_E_SUCCESS = 0,
    /** If radio did not start or is resetting */
    RIL_E_RADIO_NOT_AVAILABLE = 1,
    /** GENERIC_FAILURE */
    RIL_E_GENERIC_FAILURE = 2,
    /** PASSWORD_INCORRECT for PIN/PIN2 methods only */
    RIL_E_PASSWORD_INCORRECT = 3,
    /** Operation requires SIM PIN2 to be entered */
    RIL_E_SIM_PIN2 = 4,
    /** Operation requires SIM PIN2 to be entered */
    RIL_E_SIM_PUK2 = 5,
    /** REQUEST_NOT_SUPPORTED **/
    RIL_E_REQUEST_NOT_SUPPORTED = 6,
    /** CANCELLED */
    RIL_E_CANCELLED = 7,
    /** Data Ops are not allowed during voice call on a Class C GPRS service */
    RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL = 8,
    /** Data ops are not allowed before device registers on network */
    RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW = 9,
    /** Fail to send sms and need retry */
    RIL_E_SMS_SEND_FAIL_RETRY = 10,
    /** Fail to set the location where CDMA subscription shall be retrieved because of SIM or RUIM card absent */
    RIL_E_SIM_ABSENT = 11,
    /** Fail to find CDMA subscription from specified location */
    RIL_E_SUBSCRIPTION_NOT_AVAILABLE = 12,
    /** HW does not support preferred network type */
    RIL_E_MODE_NOT_SUPPORTED = 13,
    /** Command failed because recipient is not on FDN list  */
    RIL_E_FDN_CHECK_FAILURE = 14,
    /** Network selection failed due to illegal SIM or ME */
    RIL_E_ILLEGAL_SIM_OR_ME = 15,
    /** No logical channel available */
    RIL_E_MISSING_RESOURCE = 16,
    /** Application not found on SIM */
    RIL_E_NO_SUCH_ELEMENT = 17,
    /** DIAL request modified to USSD  */
    RIL_E_DIAL_MODIFIED_TO_USSD = 18,
    /** DIAL request modified to SS */
    RIL_E_DIAL_MODIFIED_TO_SS = 19,
    /** DIAL request modified to DIAL with different data */
    RIL_E_DIAL_MODIFIED_TO_DIAL = 20,
    /** USSD request modified to DIAL */
    RIL_E_USSD_MODIFIED_TO_DIAL = 21,
    /** USSD request modified to SS */
    RIL_E_USSD_MODIFIED_TO_SS = 22,
    /** USSD request modified to different USSD request */
    RIL_E_USSD_MODIFIED_TO_USSD = 23,
    /** SS request modified to DIAL */
    RIL_E_SS_MODIFIED_TO_DIAL = 24,
    /** SS request modified to USSD */
    RIL_E_SS_MODIFIED_TO_USSD = 25,
    /** Subscription not supported by RIL */
    RIL_E_SUBSCRIPTION_NOT_SUPPORTED = 26,
    /** SS request modified to different SS request */
    RIL_E_SS_MODIFIED_TO_SS = 27,
    /** LCE service not supported(36 in RILConstants.java) */
    RIL_E_LCE_NOT_SUPPORTED = 36,
    /** Not sufficient memory to process the request */
    RIL_E_NO_MEMORY = 37,
    /**  Modem hit unexpected error scenario while handling this request  */
    RIL_E_INTERNAL_ERR = 38,
    /** Hit platform or system error */
    RIL_E_SYSTEM_ERR = 39,
    /** Vendor RIL got unexpected or incorrect response from modem for this request */
    RIL_E_MODEM_ERR = 40,
    /** Unexpected request for the current state */
    RIL_E_INVALID_STATE = 41,
    /** Not sufficient resource to process the request */
    RIL_E_NO_RESOURCES = 42,
    /** Received error from SIM card */
    RIL_E_SIM_ERR = 43,
    /** Received invalid arguments in request */
    RIL_E_INVALID_ARGUMENTS = 44,
    /** Can not process the request in current SIM state */
    RIL_E_INVALID_SIM_STATE = 45,
    /** Can not process the request in current Modem state  */
    RIL_E_INVALID_MODEM_STATE = 46,
    /** Received invalid call id in request */
    RIL_E_INVALID_CALL_ID = 47,
    /** ACK received when there is no SMS to ack */
    RIL_E_NO_SMS_TO_ACK = 48,
    /** Received error from network */
    RIL_E_NETWORK_ERR = 49,
    /** Operation denied due to overly-frequent requests */
    RIL_E_REQUEST_RATE_LIMITED = 50,
    /** SIM is busy */
    RIL_E_SIM_BUSY = 51,
    /** The target EF is full */
    RIL_E_SIM_FULL = 52,
    /** Request is rejected by network */
    RIL_E_NETWORK_REJECT = 53,
    /** Not allowed the request now */
    RIL_E_OPERATION_NOT_ALLOWED = 54,
    /** The request record is empty */
    RIL_E_EMPTY_RECORD = 55,
    /** Invalid sms format */
    RIL_E_INVALID_SMS_FORMAT = 56,
    /** Message not encoded properly */
    RIL_E_ENCODING_ERR = 57,
    /** SMSC address specified is invalid */
    RIL_E_INVALID_SMSC_ADDRESS = 58,
    /** No such entry present to perform the request */
    RIL_E_NO_SUCH_ENTRY = 59,
    /** Network is not ready to perform the request */
    RIL_E_NETWORK_NOT_READY = 60,
    /** Device doesnot have this value provisioned */
    RIL_E_NOT_PROVISIONED = 61,
    /** Device doesnot have subscription */
    RIL_E_NO_SUBSCRIPTION = 62,
    /** Network cannot be found */
    RIL_E_NO_NETWORK_FOUND = 63,
    /** Operation cannot be performed because the device is currently in use */
    RIL_E_DEVICE_IN_USE = 64,
    /** Operation aborted */
    RIL_E_ABORTED = 65,
    /** Invalid response sent by vendor code */
    RIL_E_INVALID_RESPONSE = 66,
    // OEM specific error codes. To be used by OEM when they don't want to reveal
    // specific error codes which would be replaced by Generic failure.
    /** OEM_ERROR_1 */
    RIL_E_OEM_ERROR_1 = 501,
    /** OEM_ERROR_2 */
    RIL_E_OEM_ERROR_2 = 502,
    /** OEM_ERROR_3 */
    RIL_E_OEM_ERROR_3 = 503,
    /** OEM_ERROR_4 */
    RIL_E_OEM_ERROR_4 = 504,
    /** OEM_ERROR_5 */
    RIL_E_OEM_ERROR_5 = 505,
    /** OEM_ERROR_6 */
    RIL_E_OEM_ERROR_6 = 506,
    /** OEM_ERROR_7 */
    RIL_E_OEM_ERROR_7 = 507,
    /** OEM_ERROR_8 */
    RIL_E_OEM_ERROR_8 = 508,
    /** OEM_ERROR_9 */
    RIL_E_OEM_ERROR_9 = 509,
    /** OEM_ERROR_10 */
    RIL_E_OEM_ERROR_10 = 510,
    /** OEM_ERROR_11 */
    RIL_E_OEM_ERROR_11 = 511,
    /** OEM_ERROR_12 */
    RIL_E_OEM_ERROR_12 = 512,
    /** OEM_ERROR_13 */
    RIL_E_OEM_ERROR_13 = 513,
    /** OEM_ERROR_14 */
    RIL_E_OEM_ERROR_14 = 514,
    /** OEM_ERROR_15 */
    RIL_E_OEM_ERROR_15 = 515,
    /** OEM_ERROR_16 */
    RIL_E_OEM_ERROR_16 = 516,
    /** OEM_ERROR_17 */
    RIL_E_OEM_ERROR_17 = 517,
    /** OEM_ERROR_18 */
    RIL_E_OEM_ERROR_18 = 518,
    /** OEM_ERROR_19 */
    RIL_E_OEM_ERROR_19 = 519,
    /** OEM_ERROR_20 */
    RIL_E_OEM_ERROR_20 = 520,
    /** OEM_ERROR_21 */
    RIL_E_OEM_ERROR_21 = 521,
    /** OEM_ERROR_22 */
    RIL_E_OEM_ERROR_22 = 522,
    /** OEM_ERROR_23 */
    RIL_E_OEM_ERROR_23 = 523,
    /** OEM_ERROR_24 */
    RIL_E_OEM_ERROR_24 = 524,
    /** OEM_ERROR_25 */
    RIL_E_OEM_ERROR_25 = 525,
    /** Rejected by remote */
    RIL_E_REJECTED_BY_REMOTE = 1001,
    /** Network not supported */
    RIL_E_NETWORK_NOT_SUPPORTED = 1002,
    /** HOLD/RESUME reqeust failed */
    RIL_E_HOLD_RESUME_FAILED = 1003,
    /** HOLD/RESUME reqeust cancelled */
    RIL_E_HOLD_RESUME_CANCELED = 1004,
    /** Reinvite collision */
    RIL_E_REINVITE_COLLISION = 1005,
    /** DIAL request modified to VIDEO DIAL */
    RIL_E_DIAL_MODIFIED_TO_DIAL_VIDEO = 1006,
    /** SS request modified to VIDEO DIAL  */
    RIL_E_SS_MODIFIED_TO_DIAL_VIDEO = 1007,
    /** USSD request modified to VIDEO DIAL */
    RIL_E_USSD_MODIFIED_TO_DIAL_VIDEO = 1008,
    /** VIDEO DIAL request modified to VIDEO */
    RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL = 1009,
    /** VIDEO DIAL request modified to SS */
    RIL_E_DIAL_VIDEO_MODIFIED_TO_SS = 1010,
    /** VIDEO DIAL request modified to USSD */
    RIL_E_DIAL_VIDEO_MODIFIED_TO_USSD = 1011,
    /** VIDEO DIAL request modified to VIDEO */
    RIL_E_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO = 1012,
    /** IMS USSD request modified to CS */
    RIL_E_USSD_CS_FALLBACK = 1013,
} RIL_Errno;

typedef enum {
    RIL_CALL_ACTIVE = 0,
    RIL_CALL_HOLDING = 1,
    RIL_CALL_DIALING = 2,    /* MO call only */
    RIL_CALL_ALERTING = 3,   /* MO call only */
    RIL_CALL_INCOMING = 4,   /* MT call only */
    RIL_CALL_WAITING = 5,     /* MT call only */
    RIL_CALL_END = 6
} RIL_CallState;

typedef enum {
    RADIO_STATE_OFF = 0,                   /* Radio explictly powered off (eg CFUN=0) */
    RADIO_STATE_UNAVAILABLE = 1,           /* Radio unavailable (eg, resetting or not booted) */
    RADIO_STATE_ON = 10                    /* Radio is on */
} RIL_RadioState;

/** @public
 * RIL_RadioTechnology
 */
typedef enum {
    /** UNKNOWN */
    RADIO_TECH_UNKNOWN = 0,
    /** GPRS */
    RADIO_TECH_GPRS = 1,
    /** EDGE */
    RADIO_TECH_EDGE = 2,
    /** UMTS */
    RADIO_TECH_UMTS = 3,
    /** IS95A */
    RADIO_TECH_IS95A = 4,
    /** IS95B */
    RADIO_TECH_IS95B = 5,
    /** RTT */
    RADIO_TECH_1xRTT =  6,
    /** EVDO_0 */
    RADIO_TECH_EVDO_0 = 7,
    /** EVDO_A */
    RADIO_TECH_EVDO_A = 8,
    /** HSDPA */
    RADIO_TECH_HSDPA = 9,
    /** HSUPA */
    RADIO_TECH_HSUPA = 10,
    /** HSPA */
    RADIO_TECH_HSPA = 11,
    /** EVDO_B */
    RADIO_TECH_EVDO_B = 12,
    /** EHRPD */
    RADIO_TECH_EHRPD = 13,
    /** LTE */
    RADIO_TECH_LTE = 14,
    /** HSPAP */
    RADIO_TECH_HSPAP = 15, // HSPA+
    /** GSM */
    RADIO_TECH_GSM = 16, // Only supports voice
    /** TD_CDMA */
    RADIO_TECH_TD_SCDMA = 17,
    /** IWLAN */
    RADIO_TECH_IWLAN = 18,
    /** LTE_CA */
    RADIO_TECH_LTE_CA = 19,
    /** 5G */
    RADIO_TECH_5G = 20,
    /** WIFI */
    RADIO_TECH_WIFI,
    /** ANY */
    RADIO_TECH_ANY,
    /** AUTO */
    RADIO_TECH_AUTO = 23
} RIL_RadioTechnology;

/** @public
 * RIL_KeepaliveStatus_t
 */
typedef enum {
  /** RUNNING */
  RIL_RUNNING = 0,
  /** NEITHER RUNNING NOR REQUESTED */
  RIL_NONE = 1,
  /** REQUESTED */
  RIL_REQUESTED = 2,
}RIL_KeepaliveStatus_t;

typedef enum {
    RAF_UNKNOWN =  (1 <<  RADIO_TECH_UNKNOWN),
    RAF_GPRS = (1 << RADIO_TECH_GPRS),
    RAF_EDGE = (1 << RADIO_TECH_EDGE),
    RAF_UMTS = (1 << RADIO_TECH_UMTS),
    RAF_IS95A = (1 << RADIO_TECH_IS95A),
    RAF_IS95B = (1 << RADIO_TECH_IS95B),
    RAF_1xRTT = (1 << RADIO_TECH_1xRTT),
    RAF_EVDO_0 = (1 << RADIO_TECH_EVDO_0),
    RAF_EVDO_A = (1 << RADIO_TECH_EVDO_A),
    RAF_HSDPA = (1 << RADIO_TECH_HSDPA),
    RAF_HSUPA = (1 << RADIO_TECH_HSUPA),
    RAF_HSPA = (1 << RADIO_TECH_HSPA),
    RAF_EVDO_B = (1 << RADIO_TECH_EVDO_B),
    RAF_EHRPD = (1 << RADIO_TECH_EHRPD),
    RAF_LTE = (1 << RADIO_TECH_LTE),
    RAF_HSPAP = (1 << RADIO_TECH_HSPAP),
    RAF_GSM = (1 << RADIO_TECH_GSM),
    RAF_TD_SCDMA = (1 << RADIO_TECH_TD_SCDMA),
    RAF_LTE_CA = (1 << RADIO_TECH_LTE_CA),
    RAF_5G = (1 << RADIO_TECH_5G)
} RIL_RadioAccessFamily;

#define QCRIL_RAF_GSM (RAF_GSM | RAF_GPRS | RAF_EDGE)
#define QCRIL_RAF_CDMA (RAF_IS95A | RAF_IS95B | RAF_1xRTT)
#define QCRIL_RAF_EVDO (RAF_EVDO_0 | RAF_EVDO_A | RAF_EVDO_B | RAF_EHRPD)
#define QCRIL_RAF_HS (RAF_HSUPA | RAF_HSDPA | RAF_HSPA | RAF_HSPAP)
#define QCRIL_RAF_WCDMA (QCRIL_RAF_HS | RAF_UMTS)
#define QCRIL_RAF_LTE (RAF_LTE | RAF_LTE_CA)
#define QCRIL_RAF_AUTO (1 << RADIO_TECH_AUTO)

typedef enum {
    BAND_MODE_UNSPECIFIED = 0,      //"unspecified" (selected by baseband automatically)
    BAND_MODE_EURO = 1,             //"EURO band" (GSM-900 / DCS-1800 / WCDMA-IMT-2000)
    BAND_MODE_USA = 2,              //"US band" (GSM-850 / PCS-1900 / WCDMA-850 / WCDMA-PCS-1900)
    BAND_MODE_JPN = 3,              //"JPN band" (WCDMA-800 / WCDMA-IMT-2000)
    BAND_MODE_AUS = 4,              //"AUS band" (GSM-900 / DCS-1800 / WCDMA-850 / WCDMA-IMT-2000)
    BAND_MODE_AUS_2 = 5,            //"AUS band 2" (GSM-900 / DCS-1800 / WCDMA-850)
    BAND_MODE_CELL_800 = 6,         //"Cellular" (800-MHz Band)
    BAND_MODE_PCS = 7,              //"PCS" (1900-MHz Band)
    BAND_MODE_JTACS = 8,            //"Band Class 3" (JTACS Band)
    BAND_MODE_KOREA_PCS = 9,        //"Band Class 4" (Korean PCS Band)
    BAND_MODE_5_450M = 10,          //"Band Class 5" (450-MHz Band)
    BAND_MODE_IMT2000 = 11,         //"Band Class 6" (2-GMHz IMT2000 Band)
    BAND_MODE_7_700M_2 = 12,        //"Band Class 7" (Upper 700-MHz Band)
    BAND_MODE_8_1800M = 13,         //"Band Class 8" (1800-MHz Band)
    BAND_MODE_9_900M = 14,          //"Band Class 9" (900-MHz Band)
    BAND_MODE_10_800M_2 = 15,       //"Band Class 10" (Secondary 800-MHz Band)
    BAND_MODE_EURO_PAMR_400M = 16,  //"Band Class 11" (400-MHz European PAMR Band)
    BAND_MODE_AWS = 17,             //"Band Class 15" (AWS Band)
    BAND_MODE_USA_2500M = 18        //"Band Class 16" (US 2.5-GHz Band)
} RIL_RadioBandMode;

typedef enum {
    RC_PHASE_CONFIGURED = 0,  // LM is configured is initial value and value after FINISH completes
    RC_PHASE_START      = 1,  // START is sent before Apply and indicates that an APPLY will be
                              // forthcoming with these same parameters
    RC_PHASE_APPLY      = 2,  // APPLY is sent after all LM's receive START and returned
                              // RIL_RadioCapability.status = 0, if any START's fail no
                              // APPLY will be sent
    RC_PHASE_UNSOL_RSP  = 3,  // UNSOL_RSP is sent with RIL_UNSOL_RADIO_CAPABILITY
    RC_PHASE_FINISH     = 4   // FINISH is sent after all commands have completed. If an error
                              // occurs in any previous command the RIL_RadioAccessesFamily and
                              // logicalModemUuid fields will be the prior configuration thus
                              // restoring the configuration to the previous value. An error
                              // returned by this command will generally be ignored or may
                              // cause that logical modem to be removed from service.
} RadioCapabilityPhase;

typedef enum {
    RC_STATUS_NONE       = 0, // This parameter has no meaning with RC_PHASE_START,
                              // RC_PHASE_APPLY
    RC_STATUS_SUCCESS    = 1, // Tell modem the action transaction of set radio
                              // capability was success with RC_PHASE_FINISH
    RC_STATUS_FAIL       = 2, // Tell modem the action transaction of set radio
                              // capability is fail with RC_PHASE_FINISH.
} RadioCapabilityStatus;

#define RIL_RADIO_CAPABILITY_VERSION 1
typedef struct {
    int version;            // Version of structure, RIL_RADIO_CAPABILITY_VERSION
    int session;            // Unique session value defined by framework returned in all "responses/unsol"
    int phase;              // CONFIGURED, START, APPLY, FINISH
    int rat;                // RIL_RadioAccessFamily for the radio
    char logicalModemUuid[MAX_UUID_LENGTH]; // A UUID typically "com.xxxx.lmX where X is the logical modem.
    int status;             // Return status and an input parameter for RC_PHASE_FINISH
} RIL_RadioCapability;

// Do we want to split Data from Voice and the use
// RIL_RadioTechnology for get/setPreferredVoice/Data ?
typedef enum {
    PREF_NET_TYPE_GSM_WCDMA                = 0, /* GSM/WCDMA (WCDMA preferred) */
    PREF_NET_TYPE_GSM_ONLY                 = 1, /* GSM only */
    PREF_NET_TYPE_WCDMA                    = 2, /* WCDMA  */
    PREF_NET_TYPE_GSM_WCDMA_AUTO           = 3, /* GSM/WCDMA (auto mode, according to PRL) */
    PREF_NET_TYPE_CDMA_EVDO_AUTO           = 4, /* CDMA and EvDo (auto mode, according to PRL) */
    PREF_NET_TYPE_CDMA_ONLY                = 5, /* CDMA only */
    PREF_NET_TYPE_EVDO_ONLY                = 6, /* EvDo only */
    PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO = 7, /* GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL) */
    PREF_NET_TYPE_LTE_CDMA_EVDO            = 8, /* LTE, CDMA and EvDo */
    PREF_NET_TYPE_LTE_GSM_WCDMA            = 9, /* LTE, GSM/WCDMA */
    PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA  = 10, /* LTE, CDMA, EvDo, GSM/WCDMA */
    PREF_NET_TYPE_LTE_ONLY                 = 11, /* LTE only */
    PREF_NET_TYPE_LTE_WCDMA                = 12,  /* LTE/WCDMA */
    PREF_NET_TYPE_TD_SCDMA_ONLY            = 13, /* TD-SCDMA only */
    PREF_NET_TYPE_TD_SCDMA_WCDMA           = 14, /* TD-SCDMA and WCDMA */
    PREF_NET_TYPE_TD_SCDMA_LTE             = 15, /* TD-SCDMA and LTE */
    PREF_NET_TYPE_TD_SCDMA_GSM             = 16, /* TD-SCDMA and GSM */
    PREF_NET_TYPE_TD_SCDMA_GSM_LTE         = 17, /* TD-SCDMA,GSM and LTE */
    PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA       = 18, /* TD-SCDMA, GSM/WCDMA */
    PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE       = 19, /* TD-SCDMA, WCDMA and LTE */
    PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE   = 20, /* TD-SCDMA, GSM/WCDMA and LTE */
    PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO  = 21, /* TD-SCDMA, GSM/WCDMA, CDMA and EvDo */
    PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA   = 22,  /* TD-SCDMA, LTE, CDMA, EvDo GSM/WCDMA */
    PREF_NET_TYPE_NR5G_ONLY                = 23,  /* NR5G Only */
    PREF_NET_TYPE_NR5G_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA = 24,  /* NR5G, TD-SCDMA, LTE, CDMA, EvDo GSM/WCDMA */
    PREF_NET_TYPE_GSM_WCDMA_LTE_NR5G       = 25,  /* GSM/WCDMA, LTE, NR5G */
    PREF_NET_TYPE_GSM_WCDMA_NR5G           = 26,  /* GSM/WCDMA, NR5G */
    PREF_NET_TYPE_GSM_LTE_NR5G             = 27,  /* GSM, LTE, NR5G */
    PREF_NET_TYPE_WCDMA_LTE_NR5G           = 28,  /* WCDMA, LTE, NR5G */
    PREF_NET_TYPE_GSM_NR5G                 = 29,  /* GSM, NR5G */
    PREF_NET_TYPE_WCDMA_NR5G               = 30,  /* WCDMA,NR5G */
    PREF_NET_TYPE_LTE_NR5G                 = 31  /* LTE, NR5G */
} RIL_PreferredNetworkType;

typedef enum RIL_5G_MODE {
    FIVE_G_MODE_DISABLED = 0,  // 5G is disabled
    FIVE_G_MODE_INCLUSIVE = 1, // 5G is enabled in addition to other RATs
    FIVE_G_MODE_EXCLUSIVE = 2, // 5G is enabled exclusively
} RIL_5G_MODE;

typedef enum RIL_NR_DISABLE_MODE {
    NR_DISABLE_MODE_NONE = 0,  // NR5G auto mode
    NR_DISABLE_MODE_SA = 1, // Disable NR5G SA mode
    NR_DISABLE_MODE_NSA = 2, // Disable NR5G NSA mode
} RIL_NR_DISABLE_MODE;

/* Source for cdma subscription */
typedef enum {
   CDMA_SUBSCRIPTION_SOURCE_INVALID = -1,
   CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM = 0,
   CDMA_SUBSCRIPTION_SOURCE_NV = 1
} RIL_CdmaSubscriptionSource;

typedef enum {
    CDMA_ROAMING_PREFERENCE_INVALID = -1,
    CDMA_ROAMING_PREFERENCE_HOME_NETWORK = 0,
    CDMA_ROAMING_PREFERENCE_AFFILIATED_ROAM = 1,
    CDMA_ROAMING_PREFERENCE_ANY_ROAM = 2
} RIL_CdmaRoamingPreference;

/* User-to-User signaling Info activation types derived from 3GPP 23.087 v8.0 */
typedef enum {
    RIL_UUS_TYPE1_IMPLICIT = 0,
    RIL_UUS_TYPE1_REQUIRED = 1,
    RIL_UUS_TYPE1_NOT_REQUIRED = 2,
    RIL_UUS_TYPE2_REQUIRED = 3,
    RIL_UUS_TYPE2_NOT_REQUIRED = 4,
    RIL_UUS_TYPE3_REQUIRED = 5,
    RIL_UUS_TYPE3_NOT_REQUIRED = 6
} RIL_UUS_Type;

/* User-to-User Signaling Information data coding schemes. Possible values for
 * Octet 3 (Protocol Discriminator field) in the UUIE. The values have been
 * specified in section 10.5.4.25 of 3GPP TS 24.008 */
typedef enum {
    RIL_UUS_DCS_USP = 0,          /* User specified protocol */
    RIL_UUS_DCS_OSIHLP = 1,       /* OSI higher layer protocol */
    RIL_UUS_DCS_X244 = 2,         /* X.244 */
    RIL_UUS_DCS_RMCF = 3,         /* Reserved for system mangement
                                     convergence function */
    RIL_UUS_DCS_IA5c = 4          /* IA5 characters */
} RIL_UUS_DCS;

/* User-to-User Signaling Information defined in 3GPP 23.087 v8.0
 * This data is passed in RIL_ExtensionRecord and rec contains this
 * structure when type is RIL_UUS_INFO_EXT_REC */
typedef struct {
  RIL_UUS_Type    uusType;    /* UUS Type */
  RIL_UUS_DCS     uusDcs;     /* UUS Data Coding Scheme */
  int             uusLength;  /* Length of UUS Data */
  char *          uusData;    /* UUS Data */
} RIL_UUS_Info;

/* CDMA Signal Information Record as defined in C.S0005 section 3.7.5.5 */
typedef struct {
  char isPresent;    /* non-zero if signal information record is present */
  char signalType;   /* as defined 3.7.5.5-1 */
  char alertPitch;   /* as defined 3.7.5.5-2 */
  char signal;       /* as defined 3.7.5.5-3, 3.7.5.5-4 or 3.7.5.5-5 */
} RIL_CDMA_SignalInfoRecord;

enum RIL_AudioQuality {
    /** Unspecified audio codec */
    RIL_AUDIO_QUAL_UNSPECIFIED,
    /** AMR (Narrowband) audio codec */
    RIL_AUDIO_QUAL_AMR,
    /** AMR (Wideband) audio codec */
    RIL_AUDIO_QUAL_AMR_WB,
    /** GSM Enhanced Full-Rate audio codec */
    RIL_AUDIO_QUAL_GSM_EFR,
    /** GSM Full-Rate audio codec */
    RIL_AUDIO_QUAL_GSM_FR,
    /** GSM Half-Rate audio codec */
    RIL_AUDIO_QUAL_GSM_HR,
    /** Enhanced Variable rate codec */
    RIL_AUDIO_QUAL_EVRC,
    /** Enhanced Variable rate codec revision B */
    RIL_AUDIO_QUAL_EVRC_B,
    /** Enhanced Variable rate codec (Wideband) */
    RIL_AUDIO_QUAL_EVRC_WB,
    /** Enhanced Variable rate codec (Narrowband) */
    RIL_AUDIO_QUAL_EVRC_NW,
};

typedef struct {
    RIL_CallState   state;
    int             index;      /* Connection Index for use with, eg, AT+CHLD */
    int             toa;        /* type of address, eg 145 = intl */
    char            isMpty;     /* nonzero if is mpty call */
    char            isMT;       /* nonzero if call is mobile terminated */
    char            als;        /* ALS line indicator if available
                                   (0 = line 1) */
    char            isVoice;    /* nonzero if this is is a voice call */
    char            isVoicePrivacy;     /* nonzero if CDMA voice privacy mode is active */
    char *          number;     /* Remote party number */
    int             numberPresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
    char *          name;       /* Remote party name */
    int             namePresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
    char *          redirNum;   /* redirected remote party number */
    int             redirNumPresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
    RIL_UUS_Info *  uusInfo;    /* NULL or Pointer to User-User Signaling Information */
    enum RIL_AudioQuality audioQuality;  /* Introduced for Radio 1.2 */
} RIL_Call;

/* Deprecated, use RIL_Data_Call_Response_v6 */
typedef struct {
    int             cid;        /* Context ID, uniquely identifies this call */
    int             active;     /* 0=inactive, 1=active/physical link down, 2=active/physical link up */
    char *          type;       /* One of the PDP_type values in TS 27.007 section 10.1.1.
                                   For example, "IP", "IPV6", "IPV4V6", or "PPP". */
    char *          apn;        /* ignored */
    char *          address;    /* An address, e.g., "192.0.1.3" or "2001:db8::1". */
} RIL_Data_Call_Response_v4;

/*
 * Returned by RIL_REQUEST_SETUP_DATA_CALL, RIL_REQUEST_DATA_CALL_LIST
 * and RIL_UNSOL_DATA_CALL_LIST_CHANGED, on error status != 0.
 */
typedef struct {
    int             status;     /* A RIL_DataCallFailCause, 0 which is PDP_FAIL_NONE if no error */
    int             suggestedRetryTime; /* If status != 0, this fields indicates the suggested retry
                                           back-off timer value RIL wants to override the one
                                           pre-configured in FW.
                                           The unit is miliseconds.
                                           The value < 0 means no value is suggested.
                                           The value 0 means retry should be done ASAP.
                                           The value of INT_MAX(0x7fffffff) means no retry. */
    int             cid;        /* Context ID, uniquely identifies this call */
    int             active;     /* 0=inactive, 1=active/physical link down, 2=active/physical link up */
    char *          type;       /* One of the PDP_type values in TS 27.007 section 10.1.1.
                                   For example, "IP", "IPV6", "IPV4V6", or "PPP". If status is
                                   PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED this is the type supported
                                   such as "IP" or "IPV6" */
    char *          ifname;     /* The network interface name */
    char *          addresses;  /* A space-delimited list of addresses with optional "/" prefix length,
                                   e.g., "192.0.1.3" or "192.0.1.11/16 2001:db8::1/64".
                                   May not be empty, typically 1 IPv4 or 1 IPv6 or
                                   one of each. If the prefix length is absent the addresses
                                   are assumed to be point to point with IPv4 having a prefix
                                   length of 32 and IPv6 128. */
    char *          dnses;      /* A space-delimited list of DNS server addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty. */
    char *          gateways;   /* A space-delimited list of default gateway addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty in which case the addresses represent point
                                   to point connections. */
} RIL_Data_Call_Response_v6;

typedef struct {
    int             status;     /* A RIL_DataCallFailCause, 0 which is PDP_FAIL_NONE if no error */
    int             suggestedRetryTime; /* If status != 0, this fields indicates the suggested retry
                                           back-off timer value RIL wants to override the one
                                           pre-configured in FW.
                                           The unit is miliseconds.
                                           The value < 0 means no value is suggested.
                                           The value 0 means retry should be done ASAP.
                                           The value of INT_MAX(0x7fffffff) means no retry. */
    int             cid;        /* Context ID, uniquely identifies this call */
    int             active;     /* 0=inactive, 1=active/physical link down, 2=active/physical link up */
    char *          type;       /* One of the PDP_type values in TS 27.007 section 10.1.1.
                                   For example, "IP", "IPV6", "IPV4V6", or "PPP". If status is
                                   PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED this is the type supported
                                   such as "IP" or "IPV6" */
    char *          ifname;     /* The network interface name */
    char *          addresses;  /* A space-delimited list of addresses with optional "/" prefix length,
                                   e.g., "192.0.1.3" or "192.0.1.11/16 2001:db8::1/64".
                                   May not be empty, typically 1 IPv4 or 1 IPv6 or
                                   one of each. If the prefix length is absent the addresses
                                   are assumed to be point to point with IPv4 having a prefix
                                   length of 32 and IPv6 128. */
    char *          dnses;      /* A space-delimited list of DNS server addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty. */
    char *          gateways;   /* A space-delimited list of default gateway addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty in which case the addresses represent point
                                   to point connections. */
    char *          pcscf;    /* the Proxy Call State Control Function address
                                 via PCO(Protocol Configuration Option) for IMS client. */
} RIL_Data_Call_Response_v9;

typedef struct {
    int             status;     /* A RIL_DataCallFailCause, 0 which is PDP_FAIL_NONE if no error */
    int             suggestedRetryTime; /* If status != 0, this fields indicates the suggested retry
                                           back-off timer value RIL wants to override the one
                                           pre-configured in FW.
                                           The unit is miliseconds.
                                           The value < 0 means no value is suggested.
                                           The value 0 means retry should be done ASAP.
                                           The value of INT_MAX(0x7fffffff) means no retry. */
    int             cid;        /* Context ID, uniquely identifies this call */
    int             active;     /* 0=inactive, 1=active/physical link down, 2=active/physical link up */
    char *          type;       /* One of the PDP_type values in TS 27.007 section 10.1.1.
                                   For example, "IP", "IPV6", "IPV4V6", or "PPP". If status is
                                   PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED this is the type supported
                                   such as "IP" or "IPV6" */
    char *          ifname;     /* The network interface name */
    char *          addresses;  /* A space-delimited list of addresses with optional "/" prefix length,
                                   e.g., "192.0.1.3" or "192.0.1.11/16 2001:db8::1/64".
                                   May not be empty, typically 1 IPv4 or 1 IPv6 or
                                   one of each. If the prefix length is absent the addresses
                                   are assumed to be point to point with IPv4 having a prefix
                                   length of 32 and IPv6 128. */
    char *          dnses;      /* A space-delimited list of DNS server addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty. */
    char *          gateways;   /* A space-delimited list of default gateway addresses,
                                   e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
                                   May be empty in which case the addresses represent point
                                   to point connections. */
    char *          pcscf;    /* the Proxy Call State Control Function address
                                 via PCO(Protocol Configuration Option) for IMS client. */
    int             mtu;        /* MTU received from network
                                   Value <= 0 means network has either not sent a value or
                                   sent an invalid value */
} RIL_Data_Call_Response_v11;

typedef enum {
    RADIO_TECH_FAMILY_UNKNOWN = 0,
    RADIO_TECH_3GPP = 1, /* 3GPP Technologies - GSM, WCDMA */
    RADIO_TECH_3GPP2 = 2 /* 3GPP2 Technologies - CDMA */
} RIL_RadioTechnologyFamily;

typedef struct {
    RIL_RadioTechnologyFamily tech;
    unsigned char             retry;       /* 0 == not retry, nonzero == retry */
    int                       messageRef;  /* Valid field if retry is set to nonzero.
                                              Contains messageRef from RIL_SMS_Response
                                              corresponding to failed MO SMS.
                                            */

    union {
        /* Valid field if tech is RADIO_TECH_3GPP2. See RIL_REQUEST_CDMA_SEND_SMS */
        RIL_CDMA_SMS_Message* cdmaMessage;

        /* Valid field if tech is RADIO_TECH_3GPP. See RIL_REQUEST_SEND_SMS */
        char**                gsmMessage;   /* This is an array of pointers where pointers
                                               are contiguous but elements pointed by those pointers
                                               are not contiguous
                                            */
    } message;
} RIL_IMS_SMS_Message;

typedef struct {
    int messageRef;   /* TP-Message-Reference for GSM,
                         and BearerData MessageId for CDMA
                         (See 3GPP2 C.S0015-B, v2.0, table 4.5-1). */
    char *ackPDU;     /* or NULL if n/a */
    int errorCode;    /* See 3GPP 27.005, 3.2.5 for GSM/UMTS,
                         3GPP2 N.S0005 (IS-41C) Table 171 for CDMA,
                         -1 if unknown or not applicable*/
} RIL_SMS_Response;

/** Used by RIL_REQUEST_WRITE_SMS_TO_SIM */
typedef struct {
    int status;     /* Status of message.  See TS 27.005 3.1, "<stat>": */
                    /*      0 = "REC UNREAD"    */
                    /*      1 = "REC READ"      */
                    /*      2 = "STO UNSENT"    */
                    /*      3 = "STO SENT"      */
    char * pdu;     /* PDU of message to write, as an ASCII hex string less the SMSC address,
                       the TP-layer length is "strlen(pdu)/2". */
    char * smsc;    /* SMSC address in GSM BCD format prefixed by a length byte
                       (as expected by TS 27.005) or NULL for default SMSC */
} RIL_SMS_WriteArgs;

typedef struct {
    int32_t  result;
    int32_t  cause;
} RIL_GsmSmsAck;

typedef struct {
    char * smscPdu;                       // SMSC address in GSM BCD format prefixed by a length
                                          // byte (as expected by TS 27.005) or empty string for
                                          // default SMSC
    char * pdu;                           // SMS in PDU format as an ASCII hex string less the
                                          // SMSC address. TP-Layer-Length is be "strlen(pdu)/2
} RIL_GsmSmsMessage;

typedef enum {
    RIL_MEMORY_CAPACITY_EXCEEDED = 0xD3,
    RIL_UNSPECIFIED_ERROR = 0XFF
} RIL_SMS_AckFailCause;

typedef enum {
    RIL_CLIP_UNPROVISIONED,                   // CLIP not provisioned
    RIL_CLIP_PROVISIONED,                     // CLIP provisioned
    RIL_CLIP_UNKNOWN                          // unknown, e.g. no network etc
} RIL_ClipStatus;

typedef enum {
    RIL_OPERATOR_UNKNOWN,
    RIL_OPERATOR_AVAILABLE,
    RIL_OPERATOR_CURRENT,
    RIL_OPERATOR_FORBIDDEN
} RIL_OperatorStatus;

typedef struct {
    char *  alphaLong;                     // long alpha ONS or EONS
    char *  alphaShort;                    // short alpha ONS or EONS
    char *  operatorNumeric;               // 5 or 6 digit numeric code (MCC + MNC)
    RIL_OperatorStatus status;
} RIL_OperatorInfo;

typedef enum {
    RIL_NO_PAP_NO_CHAP,                       // PAP and CHAP is never performed.
    RIL_PAP_NO_CHAP,                          // PAP may be performed; CHAP is never performed.
    RIL_NO_PAP_CHAP,                          // CHAP may be performed; PAP is never performed.
    RIL_PAP_CHAP                              // PAP / CHAP may be performed - baseband dependent.
} RIL_ApnAuthType;

typedef enum {
    RIL_USSD_NOTIFY,                          // USSD-Notify
    RIL_USSD_REQUEST,                         // USSD-Request
    RIL_USSD_NW_RELEASE,                      // Session terminated by network
    RIL_USSD_LOCAL_CLIENT,                    // other local client (eg, SIM Toolkit) has responded
    RIL_USSD_NOT_SUPPORTED,                   // Operation not supported
    RIL_USSD_NW_TIMEOUT                       // Network timeout
} RIL_UssdModeType;

/** @public
 * RIL_PublicKeyType
 */
typedef enum {
    /** RIL */
    UNKNOWN_RIL = 0,
    /** EPDG RIL */
    EPDG_RIL    = 1,
    /** WLAN RIL */
    WLAN_RIL    = 2,
} RIL_PublicKeyType;

/** @public
* RIL_CardPowerState
*/
typedef enum {
    /** Card Power Down*/
    RIL_CARD_POWER_DOWN,
    /** Card Power Up*/
    RIL_CARD_POWER_UP,
    /** Specific for Non Telecom Cards Used for AUthentication
     *  not for camping
     */
    RIL_CARD_POWER_UP_PASS_THROUGH
} RIL_CardPowerState;

/** Used by RIL_REQUEST_DIAL */
typedef struct {
    char * address;
    int clir;
            /* (same as 'n' paremeter in TS 27.007 7.7 "+CLIR"
             * clir == 0 on "use subscription default value"
             * clir == 1 on "CLIR invocation" (restrict CLI presentation)
             * clir == 2 on "CLIR suppression" (allow CLI presentation)
             */
    RIL_UUS_Info *  uusInfo;    /* NULL or Pointer to User-User Signaling Information */
} RIL_Dial;
/** @public
* RIL_SIM_IO_v5
*/
typedef struct {
    /* one of the commands listed for TS 27.007 +CRSM*/
    int command;    /* one of the commands listed for TS 27.007 +CRSM*/
    int fileid;     /* EF id */
    char *path;     /* "pathid" from TS 27.007 +CRSM command.
                       Path is in hex asciii format eg "7f205f70"
                       Path must always be provided.
                     */
    int p1;
    int p2;
    int p3;
    char *data;     /* May be NULL*/
    char *pin2;     /* May be NULL*/
} RIL_SIM_IO_v5;

/* MacrosUsed to identify the Type of command */
#define RIL_UIM_CMD_READ_BINARY                         176
#define RIL_UIM_CMD_READ_RECORD                         178
#define RIL_UIM_CMD_GET_RESPONSE                        192
#define RIL_UIM_CMD_UPDATE_BINARY                       214
#define RIL_UIM_CMD_RETRIEVE_DATA                       203
#define RIL_UIM_CMD_SET_DATA                            219
#define RIL_UIM_CMD_UPDATE_RECORD                       220
#define RIL_UIM_CMD_STATUS                              242

typedef struct {
    int command;    /* one of the commands listed for TS 27.007 +CRSM*/
    int fileid;     /* EF id */
    char *path;     /* "pathid" from TS 27.007 +CRSM command.
                       Path is in hex asciii format eg "7f205f70"
                       Path must always be provided.
                     */
    int p1;
    int p2;
    int p3;
    char *data;     /* May be NULL*/
    char *pin2;     /* May be NULL*/
    char *aidPtr;   /* AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value. */
} RIL_SIM_IO_v6;

/** @public
 *  RIL_SIM_APDU
 */
typedef struct {
    /** "sessionid" from TS 27.007 +CGLA command. Should be
         ignored for +CSIM command. */
    int sessionid;
    /** Following fields are used to derive the APDU ("command" and "length"
       values in TS 27.007 +CSIM and +CGLA commands). */
    int cla;
    int instruction;
    int p1;
    int p2;
   /** A negative P3 implies a 4 byte APDU. */
    int p3;
   /** May be NULL. In hex string format.*/
   char *data;
} RIL_SIM_APDU;
/** @public
* RIL_SIM_IO_Response
*/
typedef struct {
    /**Status Word 1*/
    int sw1;
    /**Status Word 2*/
    int sw2;
    /** In hex string format ([a-fA-F0-9]*), except for SIM_AUTHENTICATION
        response for which it is in Base64 format, see 3GPP TS 31.102 7.1.2 */
    char *simResponse;
} RIL_SIM_IO_Response;

/* See also com.android.internal.telephony.gsm.CallForwardInfo */

typedef struct {
    int             status;     /*
                                 * For RIL_REQUEST_QUERY_CALL_FORWARD_STATUS
                                 * status 1 = active, 0 = not active
                                 *
                                 * For RIL_REQUEST_SET_CALL_FORWARD:
                                 * status is:
                                 * 0 = disable
                                 * 1 = enable
                                 * 2 = interrogate
                                 * 3 = registeration
                                 * 4 = erasure
                                 */

    int             reason;      /* from TS 27.007 7.11 "reason" */
    int             serviceClass;/* From 27.007 +CCFC/+CLCK "class"
                                    See table for Android mapping from
                                    MMI service code
                                    0 means user doesn't input class */
    int             toa;         /* "type" from TS 27.007 7.11 */
    char *          number;      /* "number" from TS 27.007 7.11. May be NULL */
    int             timeSeconds; /* for CF no reply only */
}RIL_CallForwardInfo;

typedef struct {
   char * cid;         /* Combination of LAC and Cell Id in 32 bits in GSM.
                        * Upper 16 bits is LAC and lower 16 bits
                        * is CID (as described in TS 27.005)
                        * Primary Scrambling Code (as described in TS 25.331)
                        *         in 9 bits in UMTS
                        * Valid values are hexadecimal 0x0000 - 0xffffffff.
                        */
   int    rssi;        /* Received RSSI in GSM,
                        * Level index of CPICH Received Signal Code Power in UMTS
                        */
} RIL_NeighboringCell;

typedef struct {
  char lce_status;                 /* LCE service status:
                                    * -1 = not supported;
                                    * 0 = stopped;
                                    * 1 = active.
                                    */
  unsigned int actual_interval_ms; /* actual LCE reporting interval,
                                    * meaningful only if LCEStatus = 1.
                                    */
} RIL_LceStatusInfo;

typedef struct {
  unsigned int last_hop_capacity_kbps; /* last-hop cellular capacity: kilobits/second. */
  unsigned char confidence_level;      /* capacity estimate confidence: 0-100 */
  unsigned char lce_suspended;         /* LCE report going to be suspended? (e.g., radio
                                        * moves to inactive state or network type change)
                                        * 1 = suspended;
                                        * 0 = not suspended.
                                        */
} RIL_LceDataInfo;

typedef struct {
    /**
     * Estimated downlink capacity in kbps. This bandwidth estimate shall be the estimated
     * maximum sustainable link bandwidth (as would be measured at the Upper PDCP or SNDCP SAP).
     * If the DL Aggregate Maximum Bit Rate is known, this value shall not exceed the DL-AMBR
     * for the Internet PDN connection.
     */
    unsigned int downlinkCapacityKbps;
    /**
     * Estimated uplink capacity in kbps. This bandwidth estimate shall be the estimated
     * maximum sustainable link bandwidth (as would be measured at the Upper PDCP or SNDCP SAP).
     * If the UL Aggregate Maximum Bit Rate is known, this value shall not exceed the UL-AMBR
     * for the Internet PDN connection.
     */
    unsigned int uplinkCapacityKbps;
} RIL_LinkCapacityEstimate;

typedef enum {
    RIL_MATCH_ALL = 0,          /* Apply to all carriers with the same mcc/mnc */
    RIL_MATCH_SPN = 1,          /* Use SPN and mcc/mnc to identify the carrier */
    RIL_MATCH_IMSI_PREFIX = 2,  /* Use IMSI prefix and mcc/mnc to identify the carrier */
    RIL_MATCH_GID1 = 3,         /* Use GID1 and mcc/mnc to identify the carrier */
    RIL_MATCH_GID2 = 4,         /* Use GID2 and mcc/mnc to identify the carrier */
} RIL_CarrierMatchType;

typedef struct {
    const char * mcc;
    const char * mnc;
    RIL_CarrierMatchType match_type;   /* Specify match type for the carrier.
                                        * If it’s RIL_MATCH_ALL, match_data is null;
                                        * otherwise, match_data is the value for the match type.
                                        */
    const char * match_data;
} RIL_Carrier;

typedef struct {
  int32_t len_allowed_carriers;         /* length of array allowed_carriers */
  int32_t len_excluded_carriers;        /* length of array excluded_carriers */
  RIL_Carrier * allowed_carriers;       /* whitelist for allowed carriers */
  RIL_Carrier * excluded_carriers;      /* blacklist for explicitly excluded carriers
                                         * which match allowed_carriers. Eg. allowed_carriers match
                                         * mcc/mnc, excluded_carriers has same mcc/mnc and gid1
                                         * is ABCD. It means except the carrier whose gid1 is ABCD,
                                         * all carriers with the same mcc/mnc are allowed.
                                         */
} RIL_CarrierRestrictions;

typedef enum {
    RIL_NO_MULTISIM_POLICY = 0,
    /**
     * Indicates that any SIM card can be used as far as one valid card is present in the device.
     * For the modem, a SIM card is valid when its content (i.e. MCC, MNC, GID, SPN) matches the
     * carrier restriction configuration.
     */
    RIL_ONE_VALID_SIM_MUST_BE_PRESENT = 1,
} RIL_SimLockMultiSimPolicy;

typedef struct {
  int len_allowed_carriers;         /* length of array allowed_carriers */
  int len_excluded_carriers;        /* length of array excluded_carriers */
  RIL_Carrier * allowed_carriers;       /* whitelist for allowed carriers */
  RIL_Carrier * excluded_carriers;      /* blacklist for explicitly excluded carriers
                                         * which match allowed_carriers. Eg. allowed_carriers match
                                         * mcc/mnc, excluded_carriers has same mcc/mnc and gid1
                                         * is ABCD. It means except the carrier whose gid1 is ABCD,
                                         * all carriers with the same mcc/mnc are allowed.
                                         */
  char allowedCarriersPrioritized;      /* Nonzero means that only carriers included in the allowed list and not in the excluded list
                                         * are permitted. Eg. allowedCarriers match mcc/mnc, excludedCarriers has same mcc/mnc and
                                         * gid1 is ABCD. It means except the carrier whose gid1 is ABCD, all carriers with the
                                         * same mcc/mnc are allowed.
                                         * Zero means that all carriers are allowed except those included in the excluded list
                                         * and not in the allowed list.
                                         */
  RIL_SimLockMultiSimPolicy multiSimPolicy;    /* The configuration applies to each slot independently. */
} RIL_CarrierRestrictionsWithPriority;

typedef struct {
  char * mcc;                         /* MCC of the Carrier. */
  char * mnc ;                        /* MNC of the Carrier. */
  uint8_t * carrierKey;               /* Public Key from the Carrier used to encrypt the
                                       * IMSI/IMPI.
                                       */
  int32_t carrierKeyLength;            /* Length of the Public Key. */
  char * keyIdentifier;               /* The keyIdentifier Attribute value pair that helps
                                       * a server locate the private key to decrypt the
                                       * permanent identity.
                                       */
  int64_t expirationTime;             /* Date-Time (in UTC) when the key will expire. */

} RIL_CarrierInfoForImsiEncryption;

/* See RIL_REQUEST_LAST_CALL_FAIL_CAUSE */
typedef enum {
    CALL_FAIL_UNOBTAINABLE_NUMBER = 1,
    CALL_FAIL_NO_ROUTE_TO_DESTINATION = 3,
    CALL_FAIL_CHANNEL_UNACCEPTABLE = 6,
    CALL_FAIL_OPERATOR_DETERMINED_BARRING = 8,
    CALL_FAIL_NORMAL = 16,
    CALL_FAIL_BUSY = 17,
    CALL_FAIL_NO_USER_RESPONDING = 18,
    CALL_FAIL_NO_ANSWER_FROM_USER = 19,
    CALL_FAIL_SUBSCRIBER_ABSENT = 20,
    CALL_FAIL_CALL_REJECTED = 21,
    CALL_FAIL_NUMBER_CHANGED = 22,
    CALL_FAIL_PREEMPTION = 25,
    CALL_FAIL_DESTINATION_OUT_OF_ORDER = 27,
    CALL_FAIL_INVALID_NUMBER_FORMAT = 28,
    CALL_FAIL_FACILITY_REJECTED = 29,
    CALL_FAIL_RESP_TO_STATUS_ENQUIRY = 30,
    CALL_FAIL_NORMAL_UNSPECIFIED = 31,
    CALL_FAIL_CONGESTION = 34,
    CALL_FAIL_NETWORK_OUT_OF_ORDER = 38,
    CALL_FAIL_TEMPORARY_FAILURE = 41,
    CALL_FAIL_SWITCHING_EQUIPMENT_CONGESTION = 42,
    CALL_FAIL_ACCESS_INFORMATION_DISCARDED = 43,
    CALL_FAIL_REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE = 44,
    CALL_FAIL_RESOURCES_UNAVAILABLE_OR_UNSPECIFIED = 47,
    CALL_FAIL_QOS_UNAVAILABLE = 49,
    CALL_FAIL_REQUESTED_FACILITY_NOT_SUBSCRIBED = 50,
    CALL_FAIL_INCOMING_CALLS_BARRED_WITHIN_CUG = 55,
    CALL_FAIL_BEARER_CAPABILITY_NOT_AUTHORIZED = 57,
    CALL_FAIL_BEARER_CAPABILITY_UNAVAILABLE = 58,
    CALL_FAIL_SERVICE_OPTION_NOT_AVAILABLE = 63,
    CALL_FAIL_BEARER_SERVICE_NOT_IMPLEMENTED = 65,
    CALL_FAIL_ACM_LIMIT_EXCEEDED = 68,
    CALL_FAIL_REQUESTED_FACILITY_NOT_IMPLEMENTED = 69,
    CALL_FAIL_ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE = 70,
    CALL_FAIL_SERVICE_OR_OPTION_NOT_IMPLEMENTED = 79,
    CALL_FAIL_INVALID_TRANSACTION_IDENTIFIER = 81,
    CALL_FAIL_USER_NOT_MEMBER_OF_CUG = 87,
    CALL_FAIL_INCOMPATIBLE_DESTINATION = 88,
    CALL_FAIL_INVALID_TRANSIT_NW_SELECTION = 91,
    CALL_FAIL_SEMANTICALLY_INCORRECT_MESSAGE = 95,
    CALL_FAIL_INVALID_MANDATORY_INFORMATION = 96,
    CALL_FAIL_MESSAGE_TYPE_NON_IMPLEMENTED = 97,
    CALL_FAIL_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 98,
    CALL_FAIL_INFORMATION_ELEMENT_NON_EXISTENT = 99,
    CALL_FAIL_CONDITIONAL_IE_ERROR = 100,
    CALL_FAIL_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 101,
    CALL_FAIL_RECOVERY_ON_TIMER_EXPIRED = 102,
    CALL_FAIL_PROTOCOL_ERROR_UNSPECIFIED = 111,
    CALL_FAIL_INTERWORKING_UNSPECIFIED = 127,
    CALL_FAIL_CALL_BARRED = 240,
    CALL_FAIL_FDN_BLOCKED = 241,
    CALL_FAIL_IMSI_UNKNOWN_IN_VLR = 242,
    CALL_FAIL_IMEI_NOT_ACCEPTED = 243,
    CALL_FAIL_DIAL_MODIFIED_TO_USSD = 244, /* STK Call Control */
    CALL_FAIL_DIAL_MODIFIED_TO_SS = 245,
    CALL_FAIL_DIAL_MODIFIED_TO_DIAL = 246,
    CALL_FAIL_RADIO_OFF = 247, /* Radio is OFF */
    CALL_FAIL_OUT_OF_SERVICE = 248, /* No cellular coverage */
    CALL_FAIL_NO_VALID_SIM = 249, /* No valid SIM is present */
    CALL_FAIL_RADIO_INTERNAL_ERROR = 250, /* Internal error at Modem */
    CALL_FAIL_NETWORK_RESP_TIMEOUT = 251, /* No response from network */
    CALL_FAIL_NETWORK_REJECT = 252, /* Explicit network reject */
    CALL_FAIL_RADIO_ACCESS_FAILURE = 253, /* RRC connection failure. Eg.RACH */
    CALL_FAIL_RADIO_LINK_FAILURE = 254, /* Radio Link Failure */
    CALL_FAIL_RADIO_LINK_LOST = 255, /* Radio link lost due to poor coverage */
    CALL_FAIL_RADIO_UPLINK_FAILURE = 256, /* Radio uplink failure */
    CALL_FAIL_RADIO_SETUP_FAILURE = 257, /* RRC connection setup failure */
    CALL_FAIL_RADIO_RELEASE_NORMAL = 258, /* RRC connection release, normal */
    CALL_FAIL_RADIO_RELEASE_ABNORMAL = 259, /* RRC connection release, abnormal */
    CALL_FAIL_ACCESS_CLASS_BLOCKED = 260, /* Access class barring */
    CALL_FAIL_NETWORK_DETACH = 261, /* Explicit network detach */
    CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE = 1000,
    CALL_FAIL_CDMA_DROP = 1001,
    CALL_FAIL_CDMA_INTERCEPT = 1002,
    CALL_FAIL_CDMA_REORDER = 1003,
    CALL_FAIL_CDMA_SO_REJECT = 1004,
    CALL_FAIL_CDMA_RETRY_ORDER = 1005,
    CALL_FAIL_CDMA_ACCESS_FAILURE = 1006,
    CALL_FAIL_CDMA_PREEMPTED = 1007,
    CALL_FAIL_CDMA_NOT_EMERGENCY = 1008, /* For non-emergency number dialed
                                            during emergency callback mode */
    CALL_FAIL_CDMA_ACCESS_BLOCKED = 1009, /* CDMA network access probes blocked */

    CALL_FAIL_DIAL_MODIFIED_TO_DIAL_VIDEO = 3008,  /* STK Call Control */
    CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL = 3009,
    CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO = 3010,
    CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_SS = 3011,
    CALL_FAIL_DIAL_VIDEO_MODIFIED_TO_USSD = 3012,

    /* OEM specific error codes. Used to distinguish error from
     * CALL_FAIL_ERROR_UNSPECIFIED and help assist debugging */
    CALL_FAIL_OEM_CAUSE_1 = 0xf001,
    CALL_FAIL_OEM_CAUSE_2 = 0xf002,
    CALL_FAIL_OEM_CAUSE_3 = 0xf003,
    CALL_FAIL_OEM_CAUSE_4 = 0xf004,
    CALL_FAIL_OEM_CAUSE_5 = 0xf005,
    CALL_FAIL_OEM_CAUSE_6 = 0xf006,
    CALL_FAIL_OEM_CAUSE_7 = 0xf007,
    CALL_FAIL_OEM_CAUSE_8 = 0xf008,
    CALL_FAIL_OEM_CAUSE_9 = 0xf009,
    CALL_FAIL_OEM_CAUSE_10 = 0xf00a,
    CALL_FAIL_OEM_CAUSE_11 = 0xf00b,
    CALL_FAIL_OEM_CAUSE_12 = 0xf00c,
    CALL_FAIL_OEM_CAUSE_13 = 0xf00d,
    CALL_FAIL_OEM_CAUSE_14 = 0xf00e,
    CALL_FAIL_OEM_CAUSE_15 = 0xf00f,

    CALL_FAIL_ERROR_UNSPECIFIED = 0xffff /* This error will be deprecated soon,
                                            vendor code should make sure to map error
                                            code to specific error */
} RIL_LastCallFailCause;

typedef struct {
  RIL_LastCallFailCause cause_code;
  char *                vendor_cause;
} RIL_LastCallFailCauseInfo;

/* See RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE */
typedef enum {
    PDP_FAIL_NONE = 0, /* No error, connection ok */

    /* an integer cause code defined in TS 24.008
       section 6.1.3.1.3 or TS 24.301 Release 8+ Annex B.
       If the implementation does not have access to the exact cause codes,
       then it should return one of the following values,
       as the UI layer needs to distinguish these
       cases for error notification and potential retries. */
    PDP_FAIL_OPERATOR_BARRED = 0x08,               /* no retry */
    PDP_FAIL_NAS_SIGNALLING = 0x0E,
    PDP_FAIL_LLC_SNDCP = 0x19,
    PDP_FAIL_INSUFFICIENT_RESOURCES = 0x1A,
    PDP_FAIL_MISSING_UKNOWN_APN = 0x1B,            /* no retry */
    PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE = 0x1C,      /* no retry */
    PDP_FAIL_USER_AUTHENTICATION = 0x1D,           /* no retry */
    PDP_FAIL_ACTIVATION_REJECT_GGSN = 0x1E,        /* no retry */
    PDP_FAIL_ACTIVATION_REJECT_UNSPECIFIED = 0x1F,
    PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED = 0x20,  /* no retry */
    PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED = 0x21, /* no retry */
    PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER = 0x22,
    PDP_FAIL_NSAPI_IN_USE = 0x23,                  /* no retry */
    PDP_FAIL_REGULAR_DEACTIVATION = 0x24,          /* possibly restart radio,
                                                      based on framework config */
    PDP_FAIL_QOS_NOT_ACCEPTED = 0x25,
    PDP_FAIL_NETWORK_FAILURE = 0x26,
    PDP_FAIL_UMTS_REACTIVATION_REQ = 0x27,
    PDP_FAIL_FEATURE_NOT_SUPP = 0x28,
    PDP_FAIL_TFT_SEMANTIC_ERROR = 0x29,
    PDP_FAIL_TFT_SYTAX_ERROR = 0x2A,
    PDP_FAIL_UNKNOWN_PDP_CONTEXT = 0x2B,
    PDP_FAIL_FILTER_SEMANTIC_ERROR = 0x2C,
    PDP_FAIL_FILTER_SYTAX_ERROR = 0x2D,
    PDP_FAIL_PDP_WITHOUT_ACTIVE_TFT = 0x2E,
    PDP_FAIL_ACTIVATION_REJECTED_BCM_VIOLATION = 0x30,
    PDP_FAIL_ONLY_IPV4_ALLOWED = 0x32,             /* no retry */
    PDP_FAIL_ONLY_IPV6_ALLOWED = 0x33,             /* no retry */
    PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED = 0x34,
    PDP_FAIL_ESM_INFO_NOT_RECEIVED = 0x35,
    PDP_FAIL_PDN_CONN_DOES_NOT_EXIST = 0x36,
    PDP_FAIL_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED = 0x37,
    PDP_FAIL_COLLISION_WITH_NETWORK_INITIATED_REQUEST = 0x38,
    PDP_FAIL_ONLY_IPV4V6_ALLOWED = 0x39,
    PDP_FAIL_ONLY_NON_IP_ALLOWED = 0x3A,
    PDP_FAIL_UNSUPPORTED_QCI_VALUE = 0x3B,
    PDP_FAIL_BEARER_HANDLING_NOT_SUPPORTED = 0x3C,
    PDP_FAIL_MAX_ACTIVE_PDP_CONTEXT_REACHED = 0x41,
    PDP_FAIL_UNSUPPORTED_APN_IN_CURRENT_PLMN = 0x42,
    PDP_FAIL_INVALID_TRANSACTION_ID = 0x51,
    PDP_FAIL_MESSAGE_INCORRECT_SEMANTIC = 0x5F,
    PDP_FAIL_INVALID_MANDATORY_INFO = 0x60,
    PDP_FAIL_MESSAGE_TYPE_UNSUPPORTED = 0x61,
    PDP_FAIL_MSG_TYPE_NONCOMPATIBLE_STATE = 0x62,
    PDP_FAIL_UNKNOWN_INFO_ELEMENT = 0x63,
    PDP_FAIL_CONDITIONAL_IE_ERROR = 0x64,
    PDP_FAIL_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE = 0x65,
    PDP_FAIL_PROTOCOL_ERRORS = 0x6F,             /* no retry */
    PDP_FAIL_APN_TYPE_CONFLICT = 0x70,
    PDP_FAIL_INVALID_PCSCF_ADDR = 0x71,
    PDP_FAIL_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN = 0x72,
    PDP_FAIL_EMM_ACCESS_BARRED = 0x73,
    PDP_FAIL_EMERGENCY_IFACE_ONLY = 0x74,
    PDP_FAIL_IFACE_MISMATCH = 0x75,
    PDP_FAIL_COMPANION_IFACE_IN_USE = 0x76,
    PDP_FAIL_IP_ADDRESS_MISMATCH = 0x77,
    PDP_FAIL_IFACE_AND_POL_FAMILY_MISMATCH = 0x78,
    PDP_FAIL_EMM_ACCESS_BARRED_INFINITE_RETRY = 0x79,
    PDP_FAIL_AUTH_FAILURE_ON_EMERGENCY_CALL = 0x7A,
    PDP_FAIL_INVALID_DNS_ADDR = 0x7B,
    PDP_FAIL_INVALID_PCSCF_OR_DNS_ADDRESS = 0x7C,
    PDP_FAIL_CALL_PREEMPT_BY_EMERGENCY_APN = 0x7F,
    PDP_FAIL_UE_INITIATED_DETACH_OR_DISCONNECT = 0x80,
    PDP_FAIL_MIP_FA_REASON_UNSPECIFIED = 0x7D0,
    PDP_FAIL_MIP_FA_ADMIN_PROHIBITED = 0x7D1,
    PDP_FAIL_MIP_FA_INSUFFICIENT_RESOURCES = 0x7D2,
    PDP_FAIL_MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE = 0x7D3,
    PDP_FAIL_MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE = 0x7D4,
    PDP_FAIL_MIP_FA_REQUESTED_LIFETIME_TOO_LONG = 0x7D5,
    PDP_FAIL_MIP_FA_MALFORMED_REQUEST = 0x7D6,
    PDP_FAIL_MIP_FA_MALFORMED_REPLY = 0x7D7,
    PDP_FAIL_MIP_FA_ENCAPSULATION_UNAVAILABLE = 0x7D8,
    PDP_FAIL_MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE = 0x7D9,
    PDP_FAIL_MIP_FA_REVERSE_TUNNEL_UNAVAILABLE = 0x7DA,
    PDP_FAIL_MIP_FA_REVERSE_TUNNEL_IS_MANDATORY = 0x7DB,
    PDP_FAIL_MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED = 0x7DC,
    PDP_FAIL_MIP_FA_MISSING_NAI = 0x7DD,
    PDP_FAIL_MIP_FA_MISSING_HOME_AGENT = 0x7DE,
    PDP_FAIL_MIP_FA_MISSING_HOME_ADDRESS = 0x7DF,
    PDP_FAIL_MIP_FA_UNKNOWN_CHALLENGE = 0x7E0,
    PDP_FAIL_MIP_FA_MISSING_CHALLENGE = 0x7E1,
    PDP_FAIL_MIP_FA_STALE_CHALLENGE = 0x7E2,
    PDP_FAIL_MIP_HA_REASON_UNSPECIFIED = 0x7E3,
    PDP_FAIL_MIP_HA_ADMIN_PROHIBITED = 0x7E4,
    PDP_FAIL_MIP_HA_INSUFFICIENT_RESOURCES = 0x7E5,
    PDP_FAIL_MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE = 0x7E6,
    PDP_FAIL_MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE = 0x7E7,
    PDP_FAIL_MIP_HA_REGISTRATION_ID_MISMATCH = 0x7E8,
    PDP_FAIL_MIP_HA_MALFORMED_REQUEST = 0x7E9,
    PDP_FAIL_MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS = 0x7EA,
    PDP_FAIL_MIP_HA_REVERSE_TUNNEL_UNAVAILABLE = 0x7EB,
    PDP_FAIL_MIP_HA_REVERSE_TUNNEL_IS_MANDATORY = 0x7EC,
    PDP_FAIL_MIP_HA_ENCAPSULATION_UNAVAILABLE = 0x7ED,
    PDP_FAIL_CLOSE_IN_PROGRESS = 0x7EE,
    PDP_FAIL_NETWORK_INITIATED_TERMINATION = 0x7EF,
    PDP_FAIL_MODEM_APP_PREEMPTED = 0x7F0,
    PDP_FAIL_PDN_IPV4_CALL_DISALLOWED = 0x7F1,
    PDP_FAIL_PDN_IPV4_CALL_THROTTLED = 0x7F2,
    PDP_FAIL_PDN_IPV6_CALL_DISALLOWED = 0x7F3,
    PDP_FAIL_PDN_IPV6_CALL_THROTTLED = 0x7F4,
    PDP_FAIL_MODEM_RESTART = 0x7F5,
    PDP_FAIL_PDP_PPP_NOT_SUPPORTED = 0x7F6,
    PDP_FAIL_UNPREFERRED_RAT = 0x7F7,
    PDP_FAIL_PHYSICAL_LINK_CLOSE_IN_PROGRESS = 0x7F8,
    PDP_FAIL_APN_PENDING_HANDOVER = 0x7F9,
    PDP_FAIL_PROFILE_BEARER_INCOMPATIBLE = 0x7FA,
    PDP_FAIL_SIM_CARD_CHANGED = 0x7FB,
    PDP_FAIL_LOW_POWER_MODE_OR_POWERING_DOWN = 0x7FC,
    PDP_FAIL_APN_DISABLED = 0x7FD,
    PDP_FAIL_MAX_PPP_INACTIVITY_TIMER_EXPIRED = 0x7FE,
    PDP_FAIL_IPV6_ADDRESS_TRANSFER_FAILED = 0x7FF,
    PDP_FAIL_TRAT_SWAP_FAILED = 0x800,
    PDP_FAIL_EHRPD_TO_HRPD_FALLBACK = 0x801,
    PDP_FAIL_MIP_CONFIG_FAILURE = 0x802,
    PDP_FAIL_PDN_INACTIVITY_TIMER_EXPIRED = 0x803,
    PDP_FAIL_MAX_IPV4_CONNECTIONS = 0x804,
    PDP_FAIL_MAX_IPV6_CONNECTIONS = 0x805,
    PDP_FAIL_APN_MISMATCH = 0x806,
    PDP_FAIL_IP_VERSION_MISMATCH = 0x807,
    PDP_FAIL_DUN_CALL_DISALLOWED = 0x808,
    PDP_FAIL_INTERNAL_EPC_NONEPC_TRANSITION = 0x809,
    PDP_FAIL_INTERFACE_IN_USE = 0x80A,
    PDP_FAIL_APN_DISALLOWED_ON_ROAMING = 0x80B,
    PDP_FAIL_APN_PARAMETERS_CHANGED = 0x80C,
    PDP_FAIL_NULL_APN_DISALLOWED = 0x80D,
    PDP_FAIL_THERMAL_MITIGATION = 0x80E,
    PDP_FAIL_DATA_SETTINGS_DISABLED = 0x80F,
    PDP_FAIL_DATA_ROAMING_SETTINGS_DISABLED = 0x810,
    PDP_FAIL_DDS_SWITCHED = 0x811,
    PDP_FAIL_FORBIDDEN_APN_NAME = 0x812,
    PDP_FAIL_DDS_SWITCH_IN_PROGRESS = 0x813,
    PDP_FAIL_CALL_DISALLOWED_IN_ROAMING = 0x814,
    PDP_FAIL_NON_IP_NOT_SUPPORTED = 0x815,
    PDP_FAIL_PDN_NON_IP_CALL_THROTTLED = 0x816,
    PDP_FAIL_PDN_NON_IP_CALL_DISALLOWED = 0x817,
    PDP_FAIL_CDMA_LOCK = 0x818,
    PDP_FAIL_CDMA_INTERCEPT = 0x819,
    PDP_FAIL_CDMA_REORDER = 0x81A,
    PDP_FAIL_CDMA_RELEASE_DUE_TO_SO_REJECTION = 0x81B,
    PDP_FAIL_CDMA_INCOMING_CALL = 0x81C,
    PDP_FAIL_CDMA_ALERT_STOP = 0x81D,
    PDP_FAIL_CHANNEL_ACQUISITION_FAILURE = 0x81E,
    PDP_FAIL_MAX_ACCESS_PROBE = 0x81F,
    PDP_FAIL_CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION = 0x820,
    PDP_FAIL_NO_RESPONSE_FROM_BASE_STATION = 0x821,
    PDP_FAIL_REJECTED_BY_BASE_STATION = 0x822,
    PDP_FAIL_CONCURRENT_SERVICES_INCOMPATIBLE = 0x823,
    PDP_FAIL_NO_CDMA_SERVICE = 0x824,
    PDP_FAIL_RUIM_NOT_PRESENT = 0x825,
    PDP_FAIL_CDMA_RETRY_ORDER = 0x826,
    PDP_FAIL_ACCESS_BLOCK = 0x827,
    PDP_FAIL_ACCESS_BLOCK_ALL = 0x828,
    PDP_FAIL_IS707B_MAX_ACCESS_PROBES = 0x829,
    PDP_FAIL_THERMAL_EMERGENCY = 0x82A,
    PDP_FAIL_CONCURRENT_SERVICES_NOT_ALLOWED = 0x82B,
    PDP_FAIL_INCOMING_CALL_REJECTED = 0x82C,
    PDP_FAIL_NO_SERVICE_ON_GATEWAY = 0x82D,
    PDP_FAIL_NO_GPRS_CONTEXT = 0x82E,
    PDP_FAIL_ILLEGAL_MS = 0x82F,
    PDP_FAIL_ILLEGAL_ME = 0x830,
    PDP_FAIL_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 0x831,
    PDP_FAIL_GPRS_SERVICES_NOT_ALLOWED = 0x832,
    PDP_FAIL_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK = 0x833,
    PDP_FAIL_IMPLICITLY_DETACHED = 0x834,
    PDP_FAIL_PLMN_NOT_ALLOWED = 0x835,
    PDP_FAIL_LOCATION_AREA_NOT_ALLOWED = 0x836,
    PDP_FAIL_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN = 0x837,
    PDP_FAIL_PDP_DUPLICATE = 0x838,
    PDP_FAIL_UE_RAT_CHANGE = 0x839,
    PDP_FAIL_CONGESTION = 0x83A,
    PDP_FAIL_NO_PDP_CONTEXT_ACTIVATED = 0x83B,
    PDP_FAIL_ACCESS_CLASS_DSAC_REJECTION = 0x83C,
    PDP_FAIL_PDP_ACTIVATE_MAX_RETRY_FAILED = 0x83D,
    PDP_FAIL_RADIO_ACCESS_BEARER_FAILURE = 0x83E,
    PDP_FAIL_ESM_UNKNOWN_EPS_BEARER_CONTEXT = 0x83F,
    PDP_FAIL_DRB_RELEASED_BY_RRC = 0x840,
    PDP_FAIL_CONNECTION_RELEASED = 0x841,
    PDP_FAIL_EMM_DETACHED = 0x842,
    PDP_FAIL_EMM_ATTACH_FAILED = 0x843,
    PDP_FAIL_EMM_ATTACH_STARTED = 0x844,
    PDP_FAIL_LTE_NAS_SERVICE_REQUEST_FAILED = 0x845,
    PDP_FAIL_DUPLICATE_BEARER_ID = 0x846,
    PDP_FAIL_ESM_COLLISION_SCENARIOS = 0x847,
    PDP_FAIL_ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK = 0x848,
    PDP_FAIL_ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER = 0x849,
    PDP_FAIL_ESM_BAD_OTA_MESSAGE = 0x84A,
    PDP_FAIL_ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL = 0x84B,
    PDP_FAIL_ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT = 0x84C,
    PDP_FAIL_DS_EXPLICIT_DEACTIVATION = 0x84D,
    PDP_FAIL_ESM_LOCAL_CAUSE_NONE = 0x84E,
    PDP_FAIL_LTE_THROTTLING_NOT_REQUIRED = 0x84F,
    PDP_FAIL_ACCESS_CONTROL_LIST_CHECK_FAILURE = 0x850,
    PDP_FAIL_SERVICE_NOT_ALLOWED_ON_PLMN = 0x851,
    PDP_FAIL_EMM_T3417_EXPIRED = 0x852,
    PDP_FAIL_EMM_T3417_EXT_EXPIRED = 0x853,
    PDP_FAIL_RRC_UPLINK_DATA_TRANSMISSION_FAILURE = 0x854,
    PDP_FAIL_RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER = 0x855,
    PDP_FAIL_RRC_UPLINK_CONNECTION_RELEASE = 0x856,
    PDP_FAIL_RRC_UPLINK_RADIO_LINK_FAILURE = 0x857,
    PDP_FAIL_RRC_UPLINK_ERROR_REQUEST_FROM_NAS = 0x858,
    PDP_FAIL_RRC_CONNECTION_ACCESS_STRATUM_FAILURE = 0x859,
    PDP_FAIL_RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS = 0x85A,
    PDP_FAIL_RRC_CONNECTION_ACCESS_BARRED = 0x85B,
    PDP_FAIL_RRC_CONNECTION_CELL_RESELECTION = 0x85C,
    PDP_FAIL_RRC_CONNECTION_CONFIG_FAILURE = 0x85D,
    PDP_FAIL_RRC_CONNECTION_TIMER_EXPIRED = 0x85E,
    PDP_FAIL_RRC_CONNECTION_LINK_FAILURE = 0x85F,
    PDP_FAIL_RRC_CONNECTION_CELL_NOT_CAMPED = 0x860,
    PDP_FAIL_RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE = 0x861,
    PDP_FAIL_RRC_CONNECTION_REJECT_BY_NETWORK = 0x862,
    PDP_FAIL_RRC_CONNECTION_NORMAL_RELEASE = 0x863,
    PDP_FAIL_RRC_CONNECTION_RADIO_LINK_FAILURE = 0x864,
    PDP_FAIL_RRC_CONNECTION_REESTABLISHMENT_FAILURE = 0x865,
    PDP_FAIL_RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER = 0x866,
    PDP_FAIL_RRC_CONNECTION_ABORT_REQUEST = 0x867,
    PDP_FAIL_RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR = 0x868,
    PDP_FAIL_NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH = 0x869,
    PDP_FAIL_NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH = 0x86A,
    PDP_FAIL_ESM_PROCEDURE_TIME_OUT = 0x86B,
    PDP_FAIL_INVALID_CONNECTION_ID = 0x86C,
    PDP_FAIL_MAXIMIUM_NSAPIS_EXCEEDED = 0x86D,
    PDP_FAIL_INVALID_PRIMARY_NSAPI = 0x86E,
    PDP_FAIL_CANNOT_ENCODE_OTA_MESSAGE = 0x86F,
    PDP_FAIL_RADIO_ACCESS_BEARER_SETUP_FAILURE = 0x870,
    PDP_FAIL_PDP_ESTABLISH_TIMEOUT_EXPIRED = 0x871,
    PDP_FAIL_PDP_MODIFY_TIMEOUT_EXPIRED = 0x872,
    PDP_FAIL_PDP_INACTIVE_TIMEOUT_EXPIRED = 0x873,
    PDP_FAIL_PDP_LOWERLAYER_ERROR = 0x874,
    PDP_FAIL_PDP_MODIFY_COLLISION = 0x875,
    PDP_FAIL_MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED = 0x876,
    PDP_FAIL_NAS_REQUEST_REJECTED_BY_NETWORK = 0x877,
    PDP_FAIL_RRC_CONNECTION_INVALID_REQUEST = 0x878,
    PDP_FAIL_RRC_CONNECTION_TRACKING_AREA_ID_CHANGED = 0x879,
    PDP_FAIL_RRC_CONNECTION_RF_UNAVAILABLE = 0x87A,
    PDP_FAIL_RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE = 0x87B,
    PDP_FAIL_RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE = 0x87C,
    PDP_FAIL_RRC_CONNECTION_ABORTED_AFTER_HANDOVER = 0x87D,
    PDP_FAIL_RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE = 0x87E,
    PDP_FAIL_RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE = 0x87F,
    PDP_FAIL_IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER = 0x880,
    PDP_FAIL_IMEI_NOT_ACCEPTED = 0x881,
    PDP_FAIL_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED = 0x882,
    PDP_FAIL_EPS_SERVICES_NOT_ALLOWED_IN_PLMN = 0x883,
    PDP_FAIL_MSC_TEMPORARILY_NOT_REACHABLE = 0x884,
    PDP_FAIL_CS_DOMAIN_NOT_AVAILABLE = 0x885,
    PDP_FAIL_ESM_FAILURE = 0x886,
    PDP_FAIL_MAC_FAILURE = 0x887,
    PDP_FAIL_SYNCHRONIZATION_FAILURE = 0x888,
    PDP_FAIL_UE_SECURITY_CAPABILITIES_MISMATCH = 0x889,
    PDP_FAIL_SECURITY_MODE_REJECTED = 0x88A,
    PDP_FAIL_UNACCEPTABLE_NON_EPS_AUTHENTICATION = 0x88B,
    PDP_FAIL_CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED = 0x88C,
    PDP_FAIL_NO_EPS_BEARER_CONTEXT_ACTIVATED = 0x88D,
    PDP_FAIL_INVALID_EMM_STATE = 0x88E,
    PDP_FAIL_NAS_LAYER_FAILURE = 0x88F,
    PDP_FAIL_MULTIPLE_PDP_CALL_NOT_ALLOWED = 0x890,
    PDP_FAIL_EMBMS_NOT_ENABLED = 0x891,
    PDP_FAIL_IRAT_HANDOVER_FAILED = 0x892,
    PDP_FAIL_EMBMS_REGULAR_DEACTIVATION = 0x893,
    PDP_FAIL_TEST_LOOPBACK_REGULAR_DEACTIVATION = 0x894,
    PDP_FAIL_LOWER_LAYER_REGISTRATION_FAILURE = 0x895,
    PDP_FAIL_DATA_PLAN_EXPIRED = 0x896,
    PDP_FAIL_UMTS_HANDOVER_TO_IWLAN = 0x897,
    PDP_FAIL_EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY = 0x898,
    PDP_FAIL_EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE = 0x899,
    PDP_FAIL_EVDO_HDR_CHANGED = 0x89A,
    PDP_FAIL_EVDO_HDR_EXITED = 0x89B,
    PDP_FAIL_EVDO_HDR_NO_SESSION = 0x89C,
    PDP_FAIL_EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL = 0x89D,
    PDP_FAIL_EVDO_HDR_CONNECTION_SETUP_TIMEOUT = 0x89E,
    PDP_FAIL_FAILED_TO_ACQUIRE_COLOCATED_HDR = 0x89F,
    PDP_FAIL_OTASP_COMMIT_IN_PROGRESS = 0x8A0,
    PDP_FAIL_NO_HYBRID_HDR_SERVICE = 0x8A1,
    PDP_FAIL_HDR_NO_LOCK_GRANTED = 0x8A2,
    PDP_FAIL_DBM_OR_SMS_IN_PROGRESS = 0x8A3,
    PDP_FAIL_HDR_FADE = 0x8A4,
    PDP_FAIL_HDR_ACCESS_FAILURE = 0x8A5,
    PDP_FAIL_UNSUPPORTED_1X_PREV = 0x8A6,
    PDP_FAIL_LOCAL_END = 0x8A7,
    PDP_FAIL_NO_SERVICE = 0x8A8,
    PDP_FAIL_FADE = 0x8A9,
    PDP_FAIL_NORMAL_RELEASE = 0x8AA,
    PDP_FAIL_ACCESS_ATTEMPT_ALREADY_IN_PROGRESS = 0x8AB,
    PDP_FAIL_REDIRECTION_OR_HANDOFF_IN_PROGRESS = 0x8AC,
    PDP_FAIL_EMERGENCY_MODE = 0x8AD,
    PDP_FAIL_PHONE_IN_USE = 0x8AE,
    PDP_FAIL_INVALID_MODE = 0x8AF,
    PDP_FAIL_INVALID_SIM_STATE = 0x8B0,
    PDP_FAIL_NO_COLLOCATED_HDR = 0x8B1,
    PDP_FAIL_UE_IS_ENTERING_POWERSAVE_MODE = 0x8B2,
    PDP_FAIL_DUAL_SWITCH = 0x8B3,
    PDP_FAIL_PPP_TIMEOUT = 0x8B4,
    PDP_FAIL_PPP_AUTH_FAILURE = 0x8B5,
    PDP_FAIL_PPP_OPTION_MISMATCH = 0x8B6,
    PDP_FAIL_PPP_PAP_FAILURE = 0x8B7,
    PDP_FAIL_PPP_CHAP_FAILURE = 0x8B8,
    PDP_FAIL_PPP_CLOSE_IN_PROGRESS = 0x8B9,
    PDP_FAIL_LIMITED_TO_IPV4 = 0x8BA,
    PDP_FAIL_LIMITED_TO_IPV6 = 0x8BB,
    PDP_FAIL_VSNCP_TIMEOUT = 0x8BC,
    PDP_FAIL_VSNCP_GEN_ERROR = 0x8BD,
    PDP_FAIL_VSNCP_APN_UNATHORIZED = 0x8BE,
    PDP_FAIL_VSNCP_PDN_LIMIT_EXCEEDED = 0x8BF,
    PDP_FAIL_VSNCP_NO_PDN_GATEWAY_ADDRESS = 0x8C0,
    PDP_FAIL_VSNCP_PDN_GATEWAY_UNREACHABLE = 0x8C1,
    PDP_FAIL_VSNCP_PDN_GATEWAY_REJECT = 0x8C2,
    PDP_FAIL_VSNCP_INSUFFICIENT_PARAMETERS = 0x8C3,
    PDP_FAIL_VSNCP_RESOURCE_UNAVAILABLE = 0x8C4,
    PDP_FAIL_VSNCP_ADMINISTRATIVELY_PROHIBITED = 0x8C5,
    PDP_FAIL_VSNCP_PDN_ID_IN_USE = 0x8C6,
    PDP_FAIL_VSNCP_SUBSCRIBER_LIMITATION = 0x8C7,
    PDP_FAIL_VSNCP_PDN_EXISTS_FOR_THIS_APN = 0x8C8,
    PDP_FAIL_VSNCP_RECONNECT_NOT_ALLOWED = 0x8C9,
    PDP_FAIL_IPV6_PREFIX_UNAVAILABLE = 0x8CA,
    PDP_FAIL_HANDOFF_PREFERENCE_CHANGED = 0x8CB,

    // OEM specific error codes. To be used by OEMs when they don't want to
    // reveal error code which would be replaced by PDP_FAIL_ERROR_UNSPECIFIED
    PDP_FAIL_OEM_DCFAILCAUSE_1 = 0x1001,
    PDP_FAIL_OEM_DCFAILCAUSE_2 = 0x1002,
    PDP_FAIL_OEM_DCFAILCAUSE_3 = 0x1003,
    PDP_FAIL_OEM_DCFAILCAUSE_4 = 0x1004,
    PDP_FAIL_OEM_DCFAILCAUSE_5 = 0x1005,
    PDP_FAIL_OEM_DCFAILCAUSE_6 = 0x1006,
    PDP_FAIL_OEM_DCFAILCAUSE_7 = 0x1007,
    PDP_FAIL_OEM_DCFAILCAUSE_8 = 0x1008,
    PDP_FAIL_OEM_DCFAILCAUSE_9 = 0x1009,
    PDP_FAIL_OEM_DCFAILCAUSE_10 = 0x100A,
    PDP_FAIL_OEM_DCFAILCAUSE_11 = 0x100B,
    PDP_FAIL_OEM_DCFAILCAUSE_12 = 0x100C,
    PDP_FAIL_OEM_DCFAILCAUSE_13 = 0x100D,
    PDP_FAIL_OEM_DCFAILCAUSE_14 = 0x100E,
    PDP_FAIL_OEM_DCFAILCAUSE_15 = 0x100F,

    /* Not mentioned in the specification */
    PDP_FAIL_VOICE_REGISTRATION_FAIL = -1,
    PDP_FAIL_DATA_REGISTRATION_FAIL = -2,

   /* reasons for data call drop - network/modem disconnect */
    PDP_FAIL_SIGNAL_LOST = -3,
    PDP_FAIL_PREF_RADIO_TECH_CHANGED = -4,/* preferred technology has changed, should retry
                                             with parameters appropriate for new technology */
    PDP_FAIL_RADIO_POWER_OFF = -5,        /* data call was disconnected because radio was resetting,
                                             powered off - no retry */
    PDP_FAIL_TETHERED_CALL_ACTIVE = -6,   /* data call was disconnected by modem because tethered
                                             mode was up on same APN/data profile - no retry until
                                             tethered call is off */

    PDP_FAIL_ERROR_UNSPECIFIED = 0xffff,  /* retry silently. Will be deprecated soon as
                                             new error codes are added making this unnecessary */
} RIL_DataCallFailCause;

/* See RIL_REQUEST_SETUP_DATA_CALL */
typedef enum {
    RIL_DATA_PROFILE_DEFAULT    = 0,
    RIL_DATA_PROFILE_TETHERED   = 1,
    RIL_DATA_PROFILE_IMS        = 2,
    RIL_DATA_PROFILE_FOTA       = 3,
    RIL_DATA_PROFILE_CBS        = 4,
    RIL_DATA_PROFILE_OEM_BASE   = 1000,    /* Start of OEM-specific profiles */
    RIL_DATA_PROFILE_INVALID    = 0xFFFFFFFF
} RIL_DataProfile;

/* Used by RIL_UNSOL_SUPP_SVC_NOTIFICATION */
typedef struct {
    int     notificationType;   /*
                                 * 0 = MO intermediate result code
                                 * 1 = MT unsolicited result code
                                 */
    int     code;               /* See 27.007 7.17
                                   "code1" for MO
                                   "code2" for MT. */
    int     index;              /* CUG index. See 27.007 7.17. */
    int     type;               /* "type" from 27.007 7.17 (MT only). */
    char *  number;             /* "number" from 27.007 7.17
                                   (MT only, may be NULL). */
} RIL_SuppSvcNotification;

#define RIL_CARD_MAX_APPS     8
/** @public
* RIL_CardState
*/
typedef enum {
    /** Indicates Card State as Absent*/
    RIL_CARDSTATE_ABSENT     = 0,
    /**Indicates Card State as Present*/
    RIL_CARDSTATE_PRESENT    = 1,
    /**Indicates Card State as Error*/
    RIL_CARDSTATE_ERROR      = 2,
   /** card is present but not usable due to carrier restrictions.*/
    RIL_CARDSTATE_RESTRICTED = 3
} RIL_CardState;
/** @public
* RIL_PersoSubstate
*/
typedef enum {
    /** initial state */
    RIL_PERSOSUBSTATE_UNKNOWN                   = 0,
    /** in between each lock transition */
    RIL_PERSOSUBSTATE_IN_PROGRESS               = 1,
    /** when either SIM or RUIM Perso is finished
        since each app can only have 1 active perso
        involved */
    RIL_PERSOSUBSTATE_READY                     = 2,
    RIL_PERSOSUBSTATE_SIM_NETWORK               = 3,
    RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET        = 4,
    RIL_PERSOSUBSTATE_SIM_CORPORATE             = 5,
    RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER      = 6,
    RIL_PERSOSUBSTATE_SIM_SIM                   = 7,
    /** The corresponding perso lock is blocked */
    RIL_PERSOSUBSTATE_SIM_NETWORK_PUK           = 8,
    RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK    = 9,
    RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK         = 10,
    RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK  = 11,
    RIL_PERSOSUBSTATE_SIM_SIM_PUK               = 12,
    RIL_PERSOSUBSTATE_RUIM_NETWORK1             = 13,
    RIL_PERSOSUBSTATE_RUIM_NETWORK2             = 14,
    RIL_PERSOSUBSTATE_RUIM_HRPD                 = 15,
    RIL_PERSOSUBSTATE_RUIM_CORPORATE            = 16,
    RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER     = 17,
    RIL_PERSOSUBSTATE_RUIM_RUIM                 = 18,
    /** The corresponding perso lock is blocked */
    RIL_PERSOSUBSTATE_RUIM_NETWORK1_PUK         = 19,
    RIL_PERSOSUBSTATE_RUIM_NETWORK2_PUK         = 20,
    RIL_PERSOSUBSTATE_RUIM_HRPD_PUK             = 21,
    RIL_PERSOSUBSTATE_RUIM_CORPORATE_PUK        = 22,
    RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK = 23,
    RIL_PERSOSUBSTATE_RUIM_RUIM_PUK             = 24
} RIL_PersoSubstate;

/** @public
* RIL_AppState
*/
typedef enum {
    /**Initial State Before Card is Powered Up*/
    RIL_APPSTATE_UNKNOWN               = 0,
    /**Indicates Card is detected after Powerup*/
    RIL_APPSTATE_DETECTED              = 1,
    /** If PIN1 or UPin is required */
    RIL_APPSTATE_PIN                   = 2,
    /** If PUK1 or Puk for UPin is required */
    RIL_APPSTATE_PUK                   = 3,
    /** perso_substate should be look at
       when app_state is assigned to this value */
    RIL_APPSTATE_SUBSCRIPTION_PERSO    = 4,
    /** State after Succesfull SUbscription Activation*/
    RIL_APPSTATE_READY                 = 5
} RIL_AppState;
/** @public
* RIL_PinState
*/
typedef enum {
    /** Initial State*/
    RIL_PINSTATE_UNKNOWN              = 0,
    /** Pin was enabled and Not verified*/
    RIL_PINSTATE_ENABLED_NOT_VERIFIED = 1,
    /** Pin Enabled and verified*/
    RIL_PINSTATE_ENABLED_VERIFIED     = 2,
    /** Pin Disabled*/
    RIL_PINSTATE_DISABLED             = 3,
    /** Pin enabled and blocked due to wrong Pin entry*/
    RIL_PINSTATE_ENABLED_BLOCKED      = 4,
    /** Pin Enabled and Permission Blocked*/
    RIL_PINSTATE_ENABLED_PERM_BLOCKED = 5
} RIL_PinState;
/** @public
* RIL_AppType
*/
typedef enum {
  /** Initial State*/
  RIL_APPTYPE_UNKNOWN = 0,
  /** Sim card*/
  RIL_APPTYPE_SIM     = 1,
  /** Usim Application*/
  RIL_APPTYPE_USIM    = 2,
  /**RUIM card*/
  RIL_APPTYPE_RUIM    = 3,
  /**CSIM application*/
  RIL_APPTYPE_CSIM    = 4,
  /** ISIM application*/
  RIL_APPTYPE_ISIM    = 5
} RIL_AppType;

/*
 * Please note that registration state UNKNOWN is
 * treated as "out of service" in the Android telephony.
 * Registration state REG_DENIED must be returned if Location Update
 * Reject (with cause 17 - Network Failure) is received
 * repeatedly from the network, to facilitate
 * "managed roaming"
 */
typedef enum {
    RIL_NOT_REG_AND_NOT_SEARCHING = 0,           // Not registered, MT is not currently searching
                                                 // a new operator to register
    RIL_REG_HOME = 1,                            // Registered, home network
    RIL_NOT_REG_AND_SEARCHING = 2,               // Not registered, but MT is currently searching
                                                 // a new operator to register
    RIL_REG_DENIED = 3,                          // Registration denied
    RIL_UNKNOWN = 4,                             // Unknown
    RIL_REG_ROAMING = 5,                         // Registered, roaming
    RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING = 10,   // Same as
                                                 // RIL_NOT_REG_AND_NOT_SEARCHING but indicates that
                                                 // emergency calls are enabled.
    RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING = 12,  // Same as RIL_NOT_REG_AND_SEARCHING
                                                 // but indicates that
                                                 // emergency calls are enabled.
    RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE = 13, // Same as REG_DENIED but indicates that
                                                 // emergency calls are enabled.
    RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE = 14,    // Same as UNKNOWN but indicates that
                                                 // emergency calls are enabled.
} RIL_RegState;

/** @public
 * RIL_RAN
 */
typedef enum{
   /** GERAN */
   GERAN_RAN,
   /** WCDMA */
   WCDMA_RAN,
   /** LTE */
   LTE_RAN,
   /** CDMA */
   CDMA_RAN,
   /** IWLAN */
   IWLAN_RAN,
}RIL_RAN;

/**@public
*RIL_AppStatus
*/
typedef struct
{
  /** Structure Defines Apptype*/
  RIL_AppType      app_type;
  /** Structure Defines application state*/
  RIL_AppState     app_state;
  /** applicable only if app_state ==
    RIL_APPSTATE_SUBSCRIPTION_PERSO */
  RIL_PersoSubstate perso_substate;
  /** null terminated string, e.g., from 0xA0, 0x00 -> 0x41,
      0x30, 0x30, 0x30 */
  char             *aid_ptr;
  /** null terminated string */
  char             *app_label_ptr;
  /** applicable to USIM, CSIM & ISIM */
  int              pin1_replaced;
  /** Indicates PIN1 state*/
  RIL_PinState     pin1;
  /**Indicates PIN2 State*/
  RIL_PinState     pin2;
} RIL_AppStatus;

/* Deprecated, use RIL_CardStatus_v6 */
typedef struct
{
  RIL_CardState card_state;
  RIL_PinState  universal_pin_state;             /* applicable to USIM and CSIM: RIL_PINSTATE_xxx */
  int           gsm_umts_subscription_app_index; /* value < RIL_CARD_MAX_APPS, -1 if none */
  int           cdma_subscription_app_index;     /* value < RIL_CARD_MAX_APPS, -1 if none */
  int           num_applications;                /* value <= RIL_CARD_MAX_APPS */
  RIL_AppStatus applications[RIL_CARD_MAX_APPS];
} RIL_CardStatus_v5;

/** @public
* RIL_CardStatus_v6
*/
typedef struct
{
  /** Indicates card state */
  RIL_CardState card_state;
  /** Indicates Physical slot-id*/
  int           physical_slot_id;
  /** Stream of bytes from the card after power up indicates
   * characteristics of the card
   */
  char          *atr;
  /**Indicates ICCID*/
  char          *iccid;
  /** applicable to USIM and CSIM: RIL_PINSTATE_xxx */
  RIL_PinState  universal_pin_state;
  /** value < RIL_CARD_MAX_APPS, -1 if none */
  int           gsm_umts_subscription_app_index;
  /** value < RIL_CARD_MAX_APPS, -1 if none */
  int           cdma_subscription_app_index;
  /** value < RIL_CARD_MAX_APPS, -1 if none */
  int           ims_subscription_app_index;
  /* value <= RIL_CARD_MAX_APPS */
  int           num_applications;
  RIL_AppStatus applications[RIL_CARD_MAX_APPS];
  /** The EID is the eUICC identifier. The EID shall be stored within the ECASD and can be
  * retrieved by the Device at any time using the standard GlobalPlatform GET DATA command.
  *
  * This data is mandatory and applicable only when cardState is CardState:PRESENT and SIM card
  * supports eUICC.
  */
  char          *eid;
} RIL_CardStatus_v6;

/** @public
* RIL_SimRefreshResult
*/
typedef enum {
    /** A file on SIM has been updated.  data[1] contains the EFID. */
    SIM_FILE_UPDATE = 0,
    /* SIM initialized.  All files should be re-read. */
    SIM_INIT = 1,
    /* SIM reset.  SIM power required, SIM may be locked and all files should be re-read. */
    SIM_RESET = 2
} RIL_SimRefreshResult;

/** @public
* RIL_SimRefreshResponse_v7
*/
typedef struct {
    /**Indicates the reason for Refresh Indication*/
    RIL_SimRefreshResult result;
    /** is the EFID of the updated file if the result is
    * SIM_FILE_UPDATE or 0 for any other result.
    */
    int                  ef_id;
    /** is AID(application ID) of the card application
    * See ETSI 102.221 8.1 and 101.220 4
    * For SIM_FILE_UPDATE result it can be set to AID of
    * application in which updated EF resides or it can be
    * NULL if EF is outside of an application.
    * For SIM_INIT result this field is set to AID of
    * application that caused REFRESH
    * For SIM_RESET result it is NULL
    */

    char *               aid;
} RIL_SimRefreshResponse_v7;

/* Deprecated, use RIL_CDMA_CallWaiting_v6 */
typedef struct {
    char *          number;             /* Remote party number */
    int             numberPresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown */
    char *          name;               /* Remote party name */
    RIL_CDMA_SignalInfoRecord signalInfoRecord;
} RIL_CDMA_CallWaiting_v5;

typedef struct {
    char *          number;             /* Remote party number */
    int             numberPresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown */
    char *          name;               /* Remote party name */
    RIL_CDMA_SignalInfoRecord signalInfoRecord;
    /* Number type/Number plan required to support International Call Waiting */
    int             number_type;        /* 0=Unknown, 1=International, 2=National,
                                           3=Network specific, 4=subscriber */
    int             number_plan;        /* 0=Unknown, 1=ISDN, 3=Data, 4=Telex, 8=Nat'l, 9=Private */
} RIL_CDMA_CallWaiting_v6;

/**
 * Which types of Cell Broadcast Message (CBM) are to be received by the ME
 *
 * uFromServiceID - uToServiceID defines a range of CBM message identifiers
 * whose value is 0x0000 - 0xFFFF as defined in TS 23.041 9.4.1.2.2 for GMS
 * and 9.4.4.2.2 for UMTS. All other values can be treated as empty
 * CBM message ID.
 *
 * uFromCodeScheme - uToCodeScheme defines a range of CBM data coding schemes
 * whose value is 0x00 - 0xFF as defined in TS 23.041 9.4.1.2.3 for GMS
 * and 9.4.4.2.3 for UMTS.
 * All other values can be treated as empty CBM data coding scheme.
 *
 * selected 0 means message types specified in <fromServiceId, toServiceId>
 * and <fromCodeScheme, toCodeScheme>are not accepted, while 1 means accepted.
 *
 * Used by RIL_REQUEST_GSM_GET_BROADCAST_CONFIG and
 * RIL_REQUEST_GSM_SET_BROADCAST_CONFIG.
 */
typedef struct {
    int fromServiceId;
    int toServiceId;
    int fromCodeScheme;
    int toCodeScheme;
    unsigned char selected;
} RIL_GSM_BroadcastSmsConfigInfo;

/* No restriction at all including voice/SMS/USSD/SS/AV64 and packet data. */
#define RIL_RESTRICTED_STATE_NONE           0x00
/* Block emergency call due to restriction. But allow all normal voice/SMS/USSD/SS/AV64. */
#define RIL_RESTRICTED_STATE_CS_EMERGENCY   0x01
/* Block all normal voice/SMS/USSD/SS/AV64 due to restriction. Only Emergency call allowed. */
#define RIL_RESTRICTED_STATE_CS_NORMAL      0x02
/* Block all voice/SMS/USSD/SS/AV64 including emergency call due to restriction.*/
#define RIL_RESTRICTED_STATE_CS_ALL         0x04
/* Block packet data access due to restriction. */
#define RIL_RESTRICTED_STATE_PS_ALL         0x10

/* The status for an OTASP/OTAPA session */
typedef enum {
    CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED,
    CDMA_OTA_PROVISION_STATUS_SPC_RETRIES_EXCEEDED,
    CDMA_OTA_PROVISION_STATUS_A_KEY_EXCHANGED,
    CDMA_OTA_PROVISION_STATUS_SSD_UPDATED,
    CDMA_OTA_PROVISION_STATUS_NAM_DOWNLOADED,
    CDMA_OTA_PROVISION_STATUS_MDN_DOWNLOADED,
    CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED,
    CDMA_OTA_PROVISION_STATUS_PRL_DOWNLOADED,
    CDMA_OTA_PROVISION_STATUS_COMMITTED,
    CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED,
    CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED,
    CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED
} RIL_CDMA_OTA_ProvisionStatus;

typedef struct {
    char alphaShort[MAX_OPERATOR_NAME_LEN];
    char alphaLong[MAX_OPERATOR_NAME_LEN];
} RIL_CellIdentityOperatorNames;


typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int bitErrorRate;    /* bit error rate (0-7, 99) as defined in TS 27.007 8.5 */
} RIL_GW_SignalStrength;

typedef struct {
    uint8_t valid;       /* Whether this entry is valid.
                            Used to distinguish between GW and WCDMA */
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int bitErrorRate;    /* bit error rate (0-7, 99) as defined in TS 27.007 8.5 */
    uint32_t rscp; // Valid values 0-96, 255
    uint32_t ecio; // Valid values 0-49, 255
} RIL_WCDMA_SignalStrength;

typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int bitErrorRate;    /* bit error rate (0-7, 99) as defined in TS 27.007 8.5 */
    int timingAdvance;   /* Timing Advance in bit periods. 1 bit period = 48/13 us.
                          * INT_MAX denotes invalid value */
} RIL_GSM_SignalStrength_v12;

typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int bitErrorRate;    /* bit error rate (0-7, 99) as defined in TS 27.007 8.5 */
} RIL_SignalStrengthWcdma;

typedef struct {
    int dbm;  /* Valid values are positive integers.  This value is the actual RSSI value
               * multiplied by -1.  Example: If the actual RSSI is -75, then this response
               * value will be 75.
               */
    int ecio; /* Valid values are positive integers.  This value is the actual Ec/Io multiplied
               * by -10.  Example: If the actual Ec/Io is -12.5 dB, then this response value
               * will be 125.
               */
} RIL_CDMA_SignalStrength;


typedef struct {
    int dbm;  /* Valid values are positive integers.  This value is the actual RSSI value
               * multiplied by -1.  Example: If the actual RSSI is -75, then this response
               * value will be 75.
               */
    int ecio; /* Valid values are positive integers.  This value is the actual Ec/Io multiplied
               * by -10.  Example: If the actual Ec/Io is -12.5 dB, then this response value
               * will be 125.
               */
    int signalNoiseRatio; /* Valid values are 0-8.  8 is the highest signal to noise ratio. */
} RIL_EVDO_SignalStrength;

typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int rsrp;            /* The current Reference Signal Receive Power in dBm multipled by -1.
                          * Range: 44 to 140 dBm
                          * INT_MAX: 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.133 9.1.4 */
    int rsrq;            /* The current Reference Signal Receive Quality in dB multiplied by -1.
                          * Range: 20 to 3 dB.
                          * INT_MAX: 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.133 9.1.7 */
    int rssnr;           /* The current reference signal signal-to-noise ratio in 0.1 dB units.
                          * Range: -200 to +300 (-200 = -20.0 dB, +300 = 30dB).
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.101 8.1.1 */
    int cqi;             /* The current Channel Quality Indicator.
                          * Range: 0 to 15.
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.101 9.2, 9.3, A.4 */
} RIL_LTE_SignalStrength;

typedef struct {
    int signalStrength;  /* Valid values are (0-31, 99) as defined in TS 27.007 8.5 */
    int rsrp;            /* The current Reference Signal Receive Power in dBm multipled by -1.
                          * Range: 44 to 140 dBm
                          * INT_MAX: 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.133 9.1.4 */
    int rsrq;            /* The current Reference Signal Receive Quality in dB multiplied by -1.
                          * Range: 20 to 3 dB.
                          * INT_MAX: 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.133 9.1.7 */
    int rssnr;           /* The current reference signal signal-to-noise ratio in 0.1 dB units.
                          * Range: -200 to +300 (-200 = -20.0 dB, +300 = 30dB).
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.101 8.1.1 */
    int cqi;             /* The current Channel Quality Indicator.
                          * Range: 0 to 15.
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP TS 36.101 9.2, 9.3, A.4 */
    int timingAdvance;   /* timing advance in micro seconds for a one way trip from cell to device.
                          * Approximate distance can be calculated using 300m/us * timingAdvance.
                          * Range: 0 to 0x7FFFFFFE
                          * INT_MAX : 0x7FFFFFFF denotes invalid value.
                          * Reference: 3GPP 36.321 section 6.1.3.5
                          * also: http://www.cellular-planningoptimization.com/2010/02/timing-advance-with-calculation.html */
} RIL_LTE_SignalStrength_v8;

typedef struct {
    /**
     * SS reference signal received power multiplied by -1.
     *
     * Reference: 3GPP TS 38.215.
     *
     * Range [-44, -140], INT_MAX means invalid/unreported.
     */
    int32_t ssRsrp;

    /**
     * SS reference signal received quality, multipled by -1..
     *
     * Reference: 3GPP TS 38.215.
     *
     * Range [-3, -20], INT_MAX means invalid/unreported.
     */
    int32_t ssRsrq;

    /**
     * SS signal-to-noise and interference ratio.
     *
     * Reference: 3GPP TS 38.215 section 5.1.*, 3GPP TS 38.133 section 10.1.16.1.
     *
     * Range [-23, 40], INT_MAX means invalid/unreported.
     */
    int32_t ssSinr;

    /**
     * CSI reference signal received power, multipled by -1.
     *
     * Reference: 3GPP TS 38.215.
     *
     * Range [44, 140], INT_MAX means invalid/unreported.
     */
    int32_t csiRsrp;

    /**
     * CSI reference signal received quality, multipled by -1.
     *
     * Reference: 3GPP TS 38.215.
     *
     * Range [3, 20], INT_MAX means invalid/unreported.
     */
    int32_t csiRsrq;

    /**
     * CSI signal-to-noise and interference ratio.
     *
     * Reference: 3GPP TS 138.215 section 5.1.*, 3GPP TS 38.133 section 10.1.16.1.
     *
     * Range [-23, 40], INT_MAX means invalid/unreported.
     */
    int32_t csiSinr;
} RIL_NR_SignalStrength;

typedef struct {
    /**
    * UTRA carrier RSSI as defined in TS 25.225 5.1.4
    * Valid values are (0-31, 99) as defined in TS 27.007 8.5
    * INT_MAX denotes that the value is invalid/unreported.
    */
    int signalStrength;

    /**
    * Transport Channel BER as defined in TS 25.225 5.2.5
    * Valid values are (0-7, 99) as defined in TS 27.007 8.5
    * INT_MAX denotes that the value is invalid/unreported.
    */
    int bitErrorRate;

    /**
    * P-CCPCH RSCP as defined in TS 25.225 5.1.1
    * Valid values are (0-96, 255) as defined in TS 27.007 8.69
    * INT_MAX denotes that the value is invalid/unreported.
    */
    int rscp;
} RIL_TD_SCDMA_SignalStrength;

/* Deprecated, use RIL_SignalStrength_v6 */
typedef struct {
    RIL_GW_SignalStrength   GW_SignalStrength;
    RIL_CDMA_SignalStrength CDMA_SignalStrength;
    RIL_EVDO_SignalStrength EVDO_SignalStrength;
} RIL_SignalStrength_v5;

typedef struct {
    RIL_GW_SignalStrength   GW_SignalStrength;
    RIL_CDMA_SignalStrength CDMA_SignalStrength;
    RIL_EVDO_SignalStrength EVDO_SignalStrength;
    RIL_LTE_SignalStrength  LTE_SignalStrength;
} RIL_SignalStrength_v6;

typedef struct {
    RIL_GW_SignalStrength       GW_SignalStrength;
    RIL_CDMA_SignalStrength     CDMA_SignalStrength;
    RIL_EVDO_SignalStrength     EVDO_SignalStrength;
    RIL_LTE_SignalStrength_v8   LTE_SignalStrength;
} RIL_SignalStrength_v8;

typedef struct {
    RIL_GW_SignalStrength       GW_SignalStrength;
    RIL_WCDMA_SignalStrength    WCDMA_SignalStrength;
    RIL_CDMA_SignalStrength     CDMA_SignalStrength;
    RIL_EVDO_SignalStrength     EVDO_SignalStrength;
    RIL_LTE_SignalStrength_v8   LTE_SignalStrength;
    RIL_TD_SCDMA_SignalStrength TD_SCDMA_SignalStrength;
    RIL_NR_SignalStrength       NR_SignalStrength;
} RIL_SignalStrength_v10;

#undef RIL_SignalStrength
#define RIL_SignalStrength      RIL_SignalStrength_v10

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 16-bit GSM Cell Identity described in TS 27.007, 0..65535, INT_MAX if unknown  */
} RIL_CellIdentityGsm;

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    char reg_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char reg_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 16-bit GSM Cell Identity described in TS 27.007, 0..65535, INT_MAX if unknown  */
    int arfcn;  /* 16-bit GSM Absolute RF channel number; this value must be reported */
    uint8_t bsic; /* 6-bit Base Station Identity Code; 0xFF if unknown */
    RIL_CellIdentityOperatorNames operatorNames;
} RIL_CellIdentityGsm_v12;

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455, INT_MAX if unknown  */
    int psc;    /* 9-bit UMTS Primary Scrambling Code described in TS 25.331, 0..511, INT_MAX if unknown */
} RIL_CellIdentityWcdma;

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    char reg_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char reg_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455, INT_MAX if unknown  */
    int psc;    /* 9-bit UMTS Primary Scrambling Code described in TS 25.331, 0..511; this value must be reported */
    int uarfcn; /* 16-bit UMTS Absolute RF Channel Number; this value must be reported */
    RIL_CellIdentityOperatorNames operatorNames;
} RIL_CellIdentityWcdma_v12;

typedef struct {
    int networkId;      /* Network Id 0..65535, INT_MAX if unknown */
    int systemId;       /* CDMA System Id 0..32767, INT_MAX if unknown  */
    int basestationId;  /* Base Station Id 0..65535, INT_MAX if unknown  */
    int longitude;      /* Longitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
                         * It is represented in units of 0.25 seconds and ranges from -2592000
                         * to 2592000, both values inclusive (corresponding to a range of -180
                         * to +180 degrees). INT_MAX if unknown */

    int latitude;       /* Latitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
                         * It is represented in units of 0.25 seconds and ranges from -1296000
                         * to 1296000, both values inclusive (corresponding to a range of -90
                         * to +90 degrees). INT_MAX if unknown */
    RIL_CellIdentityOperatorNames operatorNames;
} RIL_CellIdentityCdma;

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int ci;     /* 28-bit Cell Identity described in TS ???, INT_MAX if unknown */
    int pci;    /* physical cell id 0..503, INT_MAX if unknown  */
    int tac;    /* 16-bit tracking area code, INT_MAX if unknown  */
} RIL_CellIdentityLte;

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    char reg_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char reg_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    int ci;     /* 28-bit Cell Identity described in TS ???, INT_MAX if unknown */
    int pci;    /* physical cell id 0..503; this value must be reported */
    int tac;    /* 16-bit tracking area code, INT_MAX if unknown  */
    int earfcn; /* 18-bit LTE Absolute RF Channel Number; this value must be reported */
    RIL_CellIdentityOperatorNames operatorNames;
    int32_t bandwidth;
    int32_t band;
} RIL_CellIdentityLte_v12;

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    char reg_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char reg_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455, INT_MAX if unknown  */
    int cpid;    /* 8-bit Cell Parameters ID described in TS 25.331, 0..127, INT_MAX if unknown */
    RIL_CellIdentityOperatorNames operatorNames;
    int uarfcn;  /* 16-bit UMTS Absolute RF Channel Number defined in TS 25.102 5.4.4; this value must be valid */
} RIL_CellIdentityTdscdma;

typedef struct {
    char mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    char mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    char reg_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char reg_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mcc[MAX_RIL_MCC_MNC_SIZE];    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    char primary_plmn_mnc[MAX_RIL_MCC_MNC_SIZE];    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    uint64_t nci;    /* 64-bit NR Cell Identity described in 3GPP TS 38.331, INT_MAX if unknown  */
    uint32_t pci;    /* 32-bit Physical cell id described in 3GPP TS 38.331, INT_MAX if unknown  */
    int32_t tac;    /* 16-bit tracking area code, INT_MAX if unknown */
    int32_t nrarfcn; /*NR Absolute Radio Frequency Channel Number Reference: 3GPP TS 38.101-1 and 3GPP TS 38.101-2 section 5.4.2.1*/
    RIL_CellIdentityOperatorNames operatorNames;
    int32_t bandwidth;
    int32_t band;
} RIL_CellIdentityNr;

typedef struct {
    /**
     * Indicates that if E-UTRA-NR Dual Connectivity (EN-DC) is supported by the LTE cell.
     *
     * Non-zero if the plmn-InfoList-r15 is present in SIB2 and at least one bit in this list is true,
     * otherwise this value should be false.
     *
     * Reference: 3GPP TS 36.331 v15.2.2 6.3.1 System information blocks.
     */
    char isEndcAvailable;
} RIL_CellConfigLte;

typedef struct {
  RIL_CellIdentityGsm   cellIdentityGsm;
  RIL_GW_SignalStrength signalStrengthGsm;
} RIL_CellInfoGsm;

typedef struct {
  RIL_CellIdentityGsm_v12   cellIdentityGsm;
  RIL_GSM_SignalStrength_v12 signalStrengthGsm;
} RIL_CellInfoGsm_v12;

typedef struct {
  RIL_CellIdentityWcdma cellIdentityWcdma;
  RIL_SignalStrengthWcdma signalStrengthWcdma;
} RIL_CellInfoWcdma;

typedef struct {
  RIL_CellIdentityWcdma_v12 cellIdentityWcdma;
  RIL_SignalStrengthWcdma signalStrengthWcdma;
} RIL_CellInfoWcdma_v12;

typedef struct {
  RIL_CellIdentityCdma      cellIdentityCdma;
  RIL_CDMA_SignalStrength   signalStrengthCdma;
  RIL_EVDO_SignalStrength   signalStrengthEvdo;
} RIL_CellInfoCdma;

typedef struct {
  RIL_CellIdentityLte        cellIdentityLte;
  RIL_LTE_SignalStrength_v8  signalStrengthLte;
} RIL_CellInfoLte;

typedef struct {
  RIL_CellIdentityLte_v12    cellIdentityLte;
  RIL_LTE_SignalStrength_v8  signalStrengthLte;
  RIL_CellConfigLte          cellConfig;
} RIL_CellInfoLte_v12;

typedef struct {
  RIL_CellIdentityTdscdma cellIdentityTdscdma;
  RIL_TD_SCDMA_SignalStrength signalStrengthTdscdma;
} RIL_CellInfoTdscdma;

typedef struct {
  RIL_CellIdentityNr cellIdentityNr;
  RIL_NR_SignalStrength signalStrengthNr;
} RIL_CellInfoNr;

// Must be the same as CellInfo.TYPE_XXX
typedef enum {
  RIL_CELL_INFO_TYPE_NONE   = 0, /* indicates no cell information */
  RIL_CELL_INFO_TYPE_GSM    = 1,
  RIL_CELL_INFO_TYPE_CDMA   = 2,
  RIL_CELL_INFO_TYPE_LTE    = 3,
  RIL_CELL_INFO_TYPE_WCDMA  = 4,
  RIL_CELL_INFO_TYPE_TD_SCDMA  = 5,
  RIL_CELL_INFO_TYPE_NR  = 6
} RIL_CellInfoType;

// Must be the same as CellInfo.TIMESTAMP_TYPE_XXX
typedef enum {
    RIL_TIMESTAMP_TYPE_UNKNOWN = 0,
    RIL_TIMESTAMP_TYPE_ANTENNA = 1,
    RIL_TIMESTAMP_TYPE_MODEM = 2,
    RIL_TIMESTAMP_TYPE_OEM_RIL = 3,
    RIL_TIMESTAMP_TYPE_JAVA_RIL = 4,
} RIL_TimeStampType;

/** @public
 * _RIL_CellConnectionStatus
 */
typedef enum _RIL_CellConnectionStatus {
    /** NONE */
    RIL_CELL_CONNECTION_STATUS_NONE = 0,
    /** PRIMARY */
    RIL_CELL_CONNECTION_STATUS_PRIMARY = 1,
    /** SECONDARY */
    RIL_CELL_CONNECTION_STATUS_SECONDARY = 2,
} RIL_CellConnectionStatus;

/** @public
 * RIL_FrequencyRange
 * Mapping the frequency to a rough range.
 */
typedef enum {
    /** Indicates the frequency range is below 1GHz. */
    RIL_FREQUENCY_LOW = 1,

    /** Indicates the frequency range is between 1GHz and 3GHz. */
    RIL_FREQUENCY_MID = 2,

    /** Indicates the frequency range is between 3GHz and 6GHz. */
    RIL_FREQUENCY_HIGH = 3,

    /** Indicates the frequency range is above 6GHz (millimeter wave frequency). */
    RIL_FREQUENCY_MMWAVE = 4,
} RIL_FrequencyRange;

#define MAX_CONTEXT_IDS 255

typedef struct {
    /**
     * Connection status for cell. Valid values are PRIMARY_SERVING and SECONDARY_SERVING.
     */
    RIL_CellConnectionStatus status;
    /**
     * Cell bandwidth, in kHz.
     */
    int cellBandwidthDownlink;

    /* The radio technology for this physical channel. */
    RIL_RadioTechnology rat;

    /* The radio frequency info. */
    struct {
      /* A rough frequency range. */
      RIL_FrequencyRange range;

      /* The Absolute Radio Frequency Channel Number. */
      int channelNumber;
    } rfInfo;

    /**
     * A list of data calls mapped to this physical channel. The context id must match the cid of
     * @1.4::SetupDataCallResult. An empty list means the physical channel has no data call mapped
     * to it.
     */
     int           num_context_ids;
     int contextIds[MAX_CONTEXT_IDS];

    /**
     * The physical cell identifier for this cell.
     *
     * In UTRAN, this value is primary scrambling code. The range is [0, 511].
     * Reference: 3GPP TS 25.213 section 5.2.2.
     *
     * In EUTRAN, this value is physical layer cell identity. The range is [0, 503].
     * Reference: 3GPP TS 36.211 section 6.11.
     *
     * In 5G RAN, this value is physical layer cell identity. The range is [0, 1008].
     * Reference: 3GPP TS 38.211 section 7.4.2.1.
     */
    int physicalCellId;
} RIL_PhysicalChannelConfig;

typedef struct {
  RIL_CellInfoType  cellInfoType;   /* cell type for selecting from union CellInfo */
  int               registered;     /* !0 if this cell is registered 0 if not registered */
  RIL_TimeStampType timeStampType;  /* type of time stamp represented by timeStamp */
  uint64_t          timeStamp;      /* Time in nanos as returned by ril_nano_time */
  union {
    RIL_CellInfoGsm     gsm;
    RIL_CellInfoCdma    cdma;
    RIL_CellInfoLte     lte;
    RIL_CellInfoWcdma   wcdma;
    RIL_CellInfoTdscdma tdscdma;
  } CellInfo;
} RIL_CellInfo;

typedef struct {
  RIL_CellInfoType  cellInfoType;   /* cell type for selecting from union CellInfo */
  int               registered;     /* !0 if this cell is registered 0 if not registered */
  RIL_TimeStampType timeStampType;  /* type of time stamp represented by timeStamp */
  uint64_t          timeStamp;      /* Time in nanos as returned by ril_nano_time */
  RIL_CellConnectionStatus connStatus; /* Connection status.*/
  union {
    RIL_CellInfoGsm_v12     gsm;
    RIL_CellInfoCdma        cdma;
    RIL_CellInfoLte_v12     lte;
    RIL_CellInfoWcdma_v12   wcdma;
    RIL_CellInfoTdscdma     tdscdma;
    RIL_CellInfoNr          nr;
  } CellInfo;
} RIL_CellInfo_v12;

typedef struct {
  RIL_CellInfoType  cellInfoType;   /* cell type for selecting from union CellInfo */
  union {
    RIL_CellIdentityGsm_v12 cellIdentityGsm;
    RIL_CellIdentityWcdma_v12 cellIdentityWcdma;
    RIL_CellIdentityLte_v12 cellIdentityLte;
    RIL_CellIdentityTdscdma cellIdentityTdscdma;
    RIL_CellIdentityCdma cellIdentityCdma;
    RIL_CellIdentityNr   cellIdentityNr;
  };
}RIL_CellIdentity_v16;

typedef struct {
    RIL_RegState regState;                // Valid reg states are RIL_NOT_REG_AND_NOT_SEARCHING,
                                          // REG_HOME, RIL_NOT_REG_AND_SEARCHING, REG_DENIED,
                                          // UNKNOWN, REG_ROAMING defined in RegState
    RIL_RadioTechnology rat;              // indicates the available voice radio technology,
                                          // valid values as defined by RadioTechnology.
    int32_t cssSupported;                 // concurrent services support indicator. if
                                          // registered on a CDMA system.
                                          // 0 - Concurrent services not supported,
                                          // 1 - Concurrent services supported
    int32_t roamingIndicator;             // TSB-58 Roaming Indicator if registered
                                          // on a CDMA or EVDO system or -1 if not.
                                          // Valid values are 0-255.
    int32_t systemIsInPrl;                // indicates whether the current system is in the
                                          // PRL if registered on a CDMA or EVDO system or -1 if
                                          // not. 0=not in the PRL, 1=in the PRL
    int32_t defaultRoamingIndicator;      // default Roaming Indicator from the PRL,
                                          // if registered on a CDMA or EVDO system or -1 if not.
                                          // Valid values are 0-255.
    int32_t reasonForDenial;              // reasonForDenial if registration state is 3
                                          // (Registration denied) this is an enumerated reason why
                                          // registration was denied. See 3GPP TS 24.008,
                                          // 10.5.3.6 and Annex G.
                                          // 0 - General
                                          // 1 - Authentication Failure
                                          // 2 - IMSI unknown in HLR
                                          // 3 - Illegal MS
                                          // 4 - Illegal ME
                                          // 5 - PLMN not allowed
                                          // 6 - Location area not allowed
                                          // 7 - Roaming not allowed
                                          // 8 - No Suitable Cells in this Location Area
                                          // 9 - Network failure
                                          // 10 - Persistent location update reject
                                          // 11 - PLMN not allowed
                                          // 12 - Location area not allowed
                                          // 13 - Roaming not allowed in this Location Area
                                          // 15 - No Suitable Cells in this Location Area
                                          // 17 - Network Failure
                                          // 20 - MAC Failure
                                          // 21 - Sync Failure
                                          // 22 - Congestion
                                          // 23 - GSM Authentication unacceptable
                                          // 25 - Not Authorized for this CSG
                                          // 32 - Service option not supported
                                          // 33 - Requested service option not subscribed
                                          // 34 - Service option temporarily out of order
                                          // 38 - Call cannot be identified
                                          // 48-63 - Retry upon entry into a new cell
                                          // 95 - Semantically incorrect message
                                          // 96 - Invalid mandatory information
                                          // 97 - Message type non-existent or not implemented
                                          // 98 - Message type not compatible with protocol state
                                          // 99 - Information element non-existent or
                                          //      not implemented
                                          // 100 - Conditional IE error
                                          // 101 - Message not compatible with protocol state;
                                          // 111 - Protocol error, unspecified
    RIL_CellIdentity_v16 cellIdentity;    // current cell information
}RIL_VoiceRegistrationStateResponse;

typedef struct {
    uint8_t isVopsSupported;      // Indicates if VoLTE is supported
    uint8_t isEmcBearerSupported; // Indicates if VoLTE emergency bearers are supported
} LteVopsInfo;

typedef struct {
    uint8_t isEndcAvailable;          // Indicates that if E-UTRA-NR Dual Connectivity (EN-DC) is supported by the primary serving cell.
    uint8_t isDcNrRestricted;         // Indicates if use of dual connectivity with NR is restricted.
    uint8_t plmnInfoListR15Available; // Indicates if the bit N in the PLMN-InfoList-r15 is true and the selected PLMN is present in plmn-IdentityList at position N.
} NrIndicators;

typedef struct {
    RIL_RegState regState;                // Valid reg states are RIL_NOT_REG_AND_NOT_SEARCHING,
                                          // REG_HOME, RIL_NOT_REG_AND_SEARCHING, REG_DENIED,
                                          // UNKNOWN, REG_ROAMING defined in RegState
    RIL_RadioTechnology rat;              // indicates the available data radio technology,
                                          // valid values as defined by RadioTechnology.
    int32_t reasonDataDenied;             // if registration state is 3 (Registration
                                          // denied) this is an enumerated reason why
                                          // registration was denied. See 3GPP TS 24.008,
                                          // Annex G.6 "Additional cause codes for GMM".
                                          // 7 == GPRS services not allowed
                                          // 8 == GPRS services and non-GPRS services not allowed
                                          // 9 == MS identity cannot be derived by the network
                                          // 10 == Implicitly detached
                                          // 14 == GPRS services not allowed in this PLMN
                                          // 16 == MSC temporarily not reachable
                                          // 40 == No PDP context activated
    int32_t maxDataCalls;                 // The maximum number of simultaneous Data Calls that
                                          // must be established using setupDataCall().
    RIL_CellIdentity_v16 cellIdentity;    // Current cell information
    uint8_t lteVopsInfoValid;             // Indicates if lteVopsInfo field is valid
    LteVopsInfo lteVopsInfo;              // Information about LTE voice over PS
    uint8_t nrIndicatorsValid;            // Indicates if nrIndicators field is valid
    NrIndicators nrIndicators;            // 5G NSA parameters
} RIL_DataRegistrationStateResponse;

/* Names of the CDMA info records (C.S0005 section 3.7.5) */
typedef enum {
  RIL_CDMA_DISPLAY_INFO_REC,
  RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC,
  RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC,
  RIL_CDMA_CONNECTED_NUMBER_INFO_REC,
  RIL_CDMA_SIGNAL_INFO_REC,
  RIL_CDMA_REDIRECTING_NUMBER_INFO_REC,
  RIL_CDMA_LINE_CONTROL_INFO_REC,
  RIL_CDMA_EXTENDED_DISPLAY_INFO_REC,
  RIL_CDMA_T53_CLIR_INFO_REC,
  RIL_CDMA_T53_RELEASE_INFO_REC,
  RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC
} RIL_CDMA_InfoRecName;

/* Display Info Rec as defined in C.S0005 section 3.7.5.1
   Extended Display Info Rec as defined in C.S0005 section 3.7.5.16
   Note: the Extended Display info rec contains multiple records of the
   form: display_tag, display_len, and display_len occurrences of the
   chari field if the display_tag is not 10000000 or 10000001.
   To save space, the records are stored consecutively in a byte buffer.
   The display_tag, display_len and chari fields are all 1 byte.
*/

typedef struct {
  char alpha_len;
  char alpha_buf[CDMA_ALPHA_INFO_BUFFER_LENGTH];
} RIL_CDMA_DisplayInfoRecord;

/* Called Party Number Info Rec as defined in C.S0005 section 3.7.5.2
   Calling Party Number Info Rec as defined in C.S0005 section 3.7.5.3
   Connected Number Info Rec as defined in C.S0005 section 3.7.5.4
*/

typedef struct {
  char len;
  char buf[CDMA_NUMBER_INFO_BUFFER_LENGTH];
  char number_type;
  char number_plan;
  char pi;
  char si;
} RIL_CDMA_NumberInfoRecord;

/* Redirecting Number Information Record as defined in C.S0005 section 3.7.5.11 */
typedef enum {
  RIL_REDIRECTING_REASON_UNKNOWN = 0,
  RIL_REDIRECTING_REASON_CALL_FORWARDING_BUSY = 1,
  RIL_REDIRECTING_REASON_CALL_FORWARDING_NO_REPLY = 2,
  RIL_REDIRECTING_REASON_CALLED_DTE_OUT_OF_ORDER = 9,
  RIL_REDIRECTING_REASON_CALL_FORWARDING_BY_THE_CALLED_DTE = 10,
  RIL_REDIRECTING_REASON_CALL_FORWARDING_UNCONDITIONAL = 15,
  RIL_REDIRECTING_REASON_RESERVED
} RIL_CDMA_RedirectingReason;

typedef struct {
  RIL_CDMA_NumberInfoRecord redirectingNumber;
  /* redirectingReason is set to RIL_REDIRECTING_REASON_UNKNOWN if not included */
  RIL_CDMA_RedirectingReason redirectingReason;
} RIL_CDMA_RedirectingNumberInfoRecord;

/* Line Control Information Record as defined in C.S0005 section 3.7.5.15 */
typedef struct {
  char lineCtrlPolarityIncluded;
  char lineCtrlToggle;
  char lineCtrlReverse;
  char lineCtrlPowerDenial;
} RIL_CDMA_LineControlInfoRecord;

/* T53 CLIR Information Record */
typedef struct {
  char cause;
} RIL_CDMA_T53_CLIRInfoRecord;

/* T53 Audio Control Information Record */
typedef struct {
  char upLink;
  char downLink;
} RIL_CDMA_T53_AudioControlInfoRecord;

typedef struct {

  RIL_CDMA_InfoRecName name;

  union {
    /* Display and Extended Display Info Rec */
    RIL_CDMA_DisplayInfoRecord           display;

    /* Called Party Number, Calling Party Number, Connected Number Info Rec */
    RIL_CDMA_NumberInfoRecord            number;

    /* Signal Info Rec */
    RIL_CDMA_SignalInfoRecord            signal;

    /* Redirecting Number Info Rec */
    RIL_CDMA_RedirectingNumberInfoRecord redir;

    /* Line Control Info Rec */
    RIL_CDMA_LineControlInfoRecord       lineCtrl;

    /* T53 CLIR Info Rec */
    RIL_CDMA_T53_CLIRInfoRecord          clir;

    /* T53 Audio Control Info Rec */
    RIL_CDMA_T53_AudioControlInfoRecord  audioCtrl;
  } rec;
} RIL_CDMA_InformationRecord;

#define RIL_CDMA_MAX_NUMBER_OF_INFO_RECS 10

typedef struct {
  char numberOfInfoRecs;
  RIL_CDMA_InformationRecord infoRec[RIL_CDMA_MAX_NUMBER_OF_INFO_RECS];
} RIL_CDMA_InformationRecords;

/* See RIL_REQUEST_NV_READ_ITEM */
typedef struct {
  RIL_NV_Item itemID;
} RIL_NV_ReadItem;

/* See RIL_REQUEST_NV_WRITE_ITEM */
typedef struct {
  RIL_NV_Item   itemID;
  char *        value;
} RIL_NV_WriteItem;

typedef enum {
    HANDOVER_STARTED = 0,
    HANDOVER_COMPLETED = 1,
    HANDOVER_FAILED = 2,
    HANDOVER_CANCELED = 3
} RIL_SrvccState;

/* hardware configuration reported to RILJ. */
typedef enum {
   RIL_HARDWARE_CONFIG_MODEM = 0,
   RIL_HARDWARE_CONFIG_SIM = 1,
} RIL_HardwareConfig_Type;

typedef enum {
   RIL_HARDWARE_CONFIG_STATE_ENABLED = 0,
   RIL_HARDWARE_CONFIG_STATE_STANDBY = 1,
   RIL_HARDWARE_CONFIG_STATE_DISABLED = 2,
} RIL_HardwareConfig_State;

typedef struct {
   int rilModel;
   uint32_t rat; /* bitset - ref. RIL_RadioTechnology. */
   int maxVoice;
   int maxData;
   int maxStandby;
} RIL_HardwareConfig_Modem;

typedef struct {
   char modemUuid[MAX_UUID_LENGTH];
} RIL_HardwareConfig_Sim;

typedef struct {
  RIL_HardwareConfig_Type type;
  char uuid[MAX_UUID_LENGTH];
  RIL_HardwareConfig_State state;
  union {
     RIL_HardwareConfig_Modem modem;
     RIL_HardwareConfig_Sim sim;
  } cfg;
} RIL_HardwareConfig;

typedef enum {
  SS_CFU,
  SS_CF_BUSY,
  SS_CF_NO_REPLY,
  SS_CF_NOT_REACHABLE,
  SS_CF_ALL,
  SS_CF_ALL_CONDITIONAL,
  SS_CLIP,
  SS_CLIR,
  SS_COLP,
  SS_COLR,
  SS_WAIT,
  SS_BAOC,
  SS_BAOIC,
  SS_BAOIC_EXC_HOME,
  SS_BAIC,
  SS_BAIC_ROAMING,
  SS_ALL_BARRING,
  SS_OUTGOING_BARRING,
  SS_INCOMING_BARRING,
  SS_INCOMING_BARRING_DN,
  SS_INCOMING_BARRING_ANONYMOUS,
} RIL_SsServiceType;

/** @public
 * RIL_ReportFilter
*/

typedef enum {
  /** DISABLE */
  disabled,
  /** ENABLE */
  enabled
}RIL_ReportFilter;

/** @public
 * RIL_LinkCapCriteriaResult
 */
typedef enum {
  /** SUCCESS */
  success               = RIL_E_SUCCESS,
  /** RADIO_NOT_AVAILABLE */
  radio_not_available   = RIL_E_RADIO_NOT_AVAILABLE,
  /** REQUEST_NOT_SUPPORTED */
  request_not_supported = RIL_E_REQUEST_NOT_SUPPORTED,
  /** INTERNAL_ERR */
  internal_err          = RIL_E_INTERNAL_ERR,
  /** INVALID_ARGUMENTS */
  invalid_arguments     = RIL_E_INVALID_ARGUMENTS,
}RIL_LinkCapCriteriaResult;

typedef enum {
  SS_ACTIVATION,
  SS_DEACTIVATION,
  SS_INTERROGATION,
  SS_REGISTRATION,
  SS_ERASURE
} RIL_SsRequestType;

typedef enum {
  SS_ALL_TELE_AND_BEARER_SERVICES,
  SS_ALL_TELESEVICES,
  SS_TELEPHONY,
  SS_ALL_DATA_TELESERVICES,
  SS_SMS_SERVICES,
  SS_ALL_TELESERVICES_EXCEPT_SMS
} RIL_SsTeleserviceType;

typedef enum {
    SS_NONE = 0,
    SS_VOICE = 1 << 0,
    SS_DATA = 1 << 1,
    SS_FAX = 1 << 2,
    SS_SMS = 1 << 3,
    SS_DATA_SYNC = 1 << 4,
    SS_DATA_ASYNC = 1 << 5,
    SS_PACKET = 1 << 6,
    SS_PAD = 1 << 7,
    SS_MAX = 1 << 7
} RIL_SuppServiceClass;

#define SS_INFO_MAX 4
#define NUM_SERVICE_CLASSES 7

typedef struct {
  int numValidIndexes; /* This gives the number of valid values in cfInfo.
                       For example if voice is forwarded to one number and data
                       is forwarded to a different one then numValidIndexes will be
                       2 indicating total number of valid values in cfInfo.
                       Similarly if all the services are forwarded to the same
                       number then the value of numValidIndexes will be 1. */

  RIL_CallForwardInfo cfInfo[NUM_SERVICE_CLASSES]; /* This is the response data
                                                      for SS request to query call
                                                      forward status. see
                                                      RIL_REQUEST_QUERY_CALL_FORWARD_STATUS */
} RIL_CfData;

typedef struct {
   int      hysteresisMs;
   int      hysteresisDlKbps;
   int      hysteresisUlKbps;
   int      thresholdsDownLength;
   int*     thresholdsDownlinkKbps;
   int      thresholdsUpLength;
   int*     thresholdsUplinkKbps;
   RIL_RAN  ran;
} RIL_LinkCapCriteria;

typedef struct {
  RIL_SsServiceType serviceType;
  RIL_SsRequestType requestType;
  RIL_SsTeleserviceType teleserviceType;
  RIL_SuppServiceClass  serviceClass;
  RIL_Errno result;

  union {
    int ssInfo[SS_INFO_MAX]; /* This is the response data for most of the SS GET/SET
                                RIL requests. E.g. RIL_REQUSET_GET_CLIR returns
                                two ints, so first two values of ssInfo[] will be
                                used for response if serviceType is SS_CLIR and
                                requestType is SS_INTERROGATION */

    RIL_CfData cfData;
  };
} RIL_StkCcUnsolSsResponse;

/**
 * Data connection power state
 */
typedef enum {
    RIL_DC_POWER_STATE_LOW      = 1,        // Low power state
    RIL_DC_POWER_STATE_MEDIUM   = 2,        // Medium power state
    RIL_DC_POWER_STATE_HIGH     = 3,        // High power state
    RIL_DC_POWER_STATE_UNKNOWN  = INT32_MAX // Unknown state
} RIL_DcPowerStates;

/**
 * Data connection real time info
 */
typedef struct {
    uint64_t                    time;       // Time in nanos as returned by ril_nano_time
    RIL_DcPowerStates           powerState; // Current power state
} RIL_DcRtInfo;

/**
 * Data profile to modem
 */
typedef struct {
    /* id of the data profile */
    int profileId;
    /* the APN to connect to */
    char* apn;
    /** one of the PDP_type values in TS 27.007 section 10.1.1.
     * For example, "IP", "IPV6", "IPV4V6", or "PPP".
     */
    char* protocol;
    /** authentication protocol used for this PDP context
     * (None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3)
     */
    int authType;
    /* the username for APN, or NULL */
    char* user;
    /* the password for APN, or NULL */
    char* password;
    /* the profile type, TYPE_COMMON-0, TYPE_3GPP-1, TYPE_3GPP2-2 */
    int type;
    /* the period in seconds to limit the maximum connections */
    int maxConnsTime;
    /* the maximum connections during maxConnsTime */
    int maxConns;
    /** the required wait time in seconds after a successful UE initiated
     * disconnect of a given PDN connection before the device can send
     * a new PDN connection request for that given PDN
     */
    int waitTime;
    /* true to enable the profile, 0 to disable, 1 to enable */
    int enabled;
} RIL_DataProfileInfo;

typedef struct {
    /* id of the data profile */
    int profileId;
    /* the APN to connect to */
    char* apn;
    /** one of the PDP_type values in TS 27.007 section 10.1.1.
     * For example, "IP", "IPV6", "IPV4V6", or "PPP".
     */
    char* protocol;
    /** one of the PDP_type values in TS 27.007 section 10.1.1 used on roaming network.
     * For example, "IP", "IPV6", "IPV4V6", or "PPP".
     */
    char *roamingProtocol;
    /** authentication protocol used for this PDP context
     * (None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3)
     */
    int authType;
    /* the username for APN, or NULL */
    char* user;
    /* the password for APN, or NULL */
    char* password;
    /* the profile type, TYPE_COMMON-0, TYPE_3GPP-1, TYPE_3GPP2-2 */
    int type;
    /* the period in seconds to limit the maximum connections */
    int maxConnsTime;
    /* the maximum connections during maxConnsTime */
    int maxConns;
    /** the required wait time in seconds after a successful UE initiated
     * disconnect of a given PDN connection before the device can send
     * a new PDN connection request for that given PDN
     */
    int waitTime;
    /* true to enable the profile, 0 to disable, 1 to enable */
    int enabled;
    /* supported APN types bitmask. See RIL_ApnTypes for the value of each bit. */
    int supportedTypesBitmask;
    /** the bearer bitmask. See RIL_RadioAccessFamily for the value of each bit. */
    int bearerBitmask;
    /** maximum transmission unit (MTU) size in bytes */
    int mtu;
    /** the MVNO type: possible values are "imsi", "gid", "spn" */
    char *mvnoType;
    /** MVNO match data. Can be anything defined by the carrier. For example,
     *        SPN like: "A MOBILE", "BEN NL", etc...
     *        IMSI like: "302720x94", "2060188", etc...
     *        GID like: "4E", "33", etc...
     */
    char *mvnoMatchData;
} RIL_DataProfileInfo_v15;

typedef struct {
    /* id of the data profile */
    int profileId;
    /* the APN to connect to */
    char* apn;
    /** one of the PDP_type values in TS 27.007 section 10.1.1.
     * For example, "IP", "IPV6", "IPV4V6", or "PPP". */
    char* protocol;
    /** one of the PDP_type values in TS 27.007 section 10.1.1 used on roaming network.
     * For example, "IP", "IPV6", "IPV4V6", or "PPP". */
    char *roamingProtocol;
    /** authentication protocol used for this PDP context
     * (None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3) */
    int authType;
    /* the username for APN, or NULL */
    char* user;
    /* the password for APN, or NULL */
    char* password;
    /* the profile type, TYPE_COMMON-0, TYPE_3GPP-1, TYPE_3GPP2-2 */
    int type;
    /* the period in seconds to limit the maximum connections */
    int maxConnsTime;
    /* the maximum connections during maxConnsTime */
    int maxConns;
    /** the required wait time in seconds after a successful UE initiated
     * disconnect of a given PDN connection before the device can send
     * a new PDN connection request for that given PDN */
    int waitTime;
    /* true to enable the profile, 0 to disable, 1 to enable */
    int enabled;
    /* supported APN types bitmask. See RIL_ApnTypes for the value of each bit. */
    int supportedTypesBitmask;
    /** the bearer bitmask. See RIL_RadioAccessFamily for the value of each bit. */
    int bearerBitmask;
    /** maximum transmission unit (MTU) size in bytes */
    int mtu;
    /** True if this data profile was used to bring up the last default (i.e internet)
     * data connection successfully. */
    int preferred;
    /** If true, modem must persist this data profile and profileId must not be
     * set to INVALID (0xFFFFFFFF). If the same data profile exists, this data profile
     * must overwrite it. */
    int persistent;
} RIL_DataProfileInfo_IRadio_1_4;

/* Tx Power Levels */
#define RIL_NUM_TX_POWER_LEVELS     5

/**
 * Aggregate modem activity information
 */
typedef struct {

  /* total time (in ms) when modem is in a low power or
   * sleep state
   */
  uint32_t sleep_mode_time_ms;

  /* total time (in ms) when modem is awake but neither
   * the transmitter nor receiver are active/awake */
  uint32_t idle_mode_time_ms;

  /* total time (in ms) during which the transmitter is active/awake,
   * subdivided by manufacturer-defined device-specific
   * contiguous increasing ranges of transmit power between
   * 0 and the transmitter's maximum transmit power.
   */
  uint32_t tx_mode_time_ms[RIL_NUM_TX_POWER_LEVELS];

  /* total time (in ms) for which receiver is active/awake and
   * the transmitter is inactive */
  uint32_t rx_mode_time_ms;
} RIL_ActivityStatsInfo;

typedef enum {
    RIL_APN_TYPE_UNKNOWN      = 0x0,          // Unknown
    RIL_APN_TYPE_DEFAULT      = 0x1,          // APN type for default data traffic
    RIL_APN_TYPE_MMS          = 0x2,          // APN type for MMS traffic
    RIL_APN_TYPE_SUPL         = 0x4,          // APN type for SUPL assisted GPS
    RIL_APN_TYPE_DUN          = 0x8,          // APN type for DUN traffic
    RIL_APN_TYPE_HIPRI        = 0x10,         // APN type for HiPri traffic
    RIL_APN_TYPE_FOTA         = 0x20,         // APN type for FOTA
    RIL_APN_TYPE_IMS          = 0x40,         // APN type for IMS
    RIL_APN_TYPE_CBS          = 0x80,         // APN type for CBS
    RIL_APN_TYPE_IA           = 0x100,        // APN type for IA Initial Attach APN
    RIL_APN_TYPE_EMERGENCY    = 0x200,        // APN type for Emergency PDN. This is not an IA apn,
                                              // but is used for access to carrier services in an
                                              // emergency call situation.
    RIL_APN_TYPE_MCX          = 0x400,        // APN type for Mission Critical Service
    RIL_APN_TYPE_XCAP         = 0x800,        // APN type carrier XCAP server
                                              // Reference: 3GPP TS 22.280 V15.3.0
    RIL_APN_TYPE_ALL          = 0xFFFFFFFF    // All APN types
} RIL_ApnTypes;

typedef enum {
    RIL_DST_POWER_SAVE_MODE,        // Device power save mode (provided by PowerManager)
                                    // True indicates the device is in power save mode.
    RIL_DST_CHARGING_STATE,         // Device charging state (provided by BatteryManager)
                                    // True indicates the device is charging.
    RIL_DST_LOW_DATA_EXPECTED       // Low data expected mode. True indicates low data traffic
                                    // is expected, for example, when the device is idle
                                    // (e.g. not doing tethering in the background). Note
                                    // this doesn't mean no data is expected.
} RIL_DeviceStateType;

typedef enum {
    RIL_UR_SIGNAL_STRENGTH            = 0x01, // When this bit is set, modem should always send the
                                              // signal strength update through
                                              // RIL_UNSOL_SIGNAL_STRENGTH, otherwise suppress it.
    RIL_UR_FULL_NETWORK_STATE         = 0x02, // When this bit is set, modem should always send
                                              // RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED
                                              // when any field in
                                              // RIL_REQUEST_VOICE_REGISTRATION_STATE or
                                              // RIL_REQUEST_DATA_REGISTRATION_STATE changes. When
                                              // this bit is not set, modem should suppress
                                              // RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED
                                              // only when insignificant fields change
                                              // (e.g. cell info).
                                              // Modem should continue sending
                                              // RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED
                                              // when significant fields are updated even when this
                                              // bit is not set. The following fields are
                                              // considered significant, registration state and
                                              // radio technology.
    RIL_UR_DATA_CALL_DORMANCY_CHANGED = 0x04, // When this bit is set, modem should send the data
                                              // call list changed unsolicited response
                                              // RIL_UNSOL_DATA_CALL_LIST_CHANGED whenever any
                                              // field in RIL_Data_Call_Response changes.
                                              // Otherwise modem should suppress the unsolicited
                                              // response when the only changed field is 'active'
                                              // (for data dormancy). For all other fields change,
                                              // modem should continue sending
                                              // RIL_UNSOL_DATA_CALL_LIST_CHANGED regardless this
                                              // bit is set or not.
    RIL_UR_LINK_CAPACITY_ESTIMATE     = 0x08, // When this bit is set, modem should send link
                                              // capacity updates based on the specified criteria
    RIL_UR_PHYSICAL_CHANNEL_CONFIG    = 0x0A, // When this bit is set, the modem must send the physical channel configuration update through
                                              // IRadioIndication.currentPhysicalChannelConfigs() when the configuration has changed. It is
                                              // recommended that this be reported whenever link capacity or signal strength is reported.
    RIL_UR_BARRING_INFO     = 0x40,           // When this bit is set, modem should send barring info
} RIL_UnsolicitedResponseFilter;
/** @public
*RIL_OpenChannelParams
*/
typedef struct {
    /** AID value, See ETSI 102.221 and 101.220*/
    char * aidPtr;
    /** P2 parameter (described in ISO 7816-4)
       P2Constants:NO_P2 if to be ignored */
    int p2;
} RIL_OpenChannelParams;

typedef enum {
    RIL_ONE_SHOT = 0x01, // Performs the scan only once
    RIL_PERIODIC = 0x02  // Performs the scan periodically until cancelled
} RIL_ScanType;

/** @public
 * RIL_RadioAccessNetworks
 */
typedef enum {
    /** UNKNOWN */
    UNKNOWN = 0x00,
    /**    GSM EDGE Radio Access Network */
    GERAN = 0x01,
    /** Universal Terrestrial Radio Access Network*/
    UTRAN = 0x02,
    /** Evolved Universal Terrestrial Radio Access Network */
    EUTRAN = 0x03,
    /** Next Generation Radio Access Network */
    NGRAN = 0x04,
    /** Code Division Multiple Access Radio Access Network */
    CDMA  = 0x05,
    /** IWLAN Radio Access Network */
    IWLAN = 0x06
} RIL_RadioAccessNetworks;

typedef enum {
    MT_UNKNOWN = 0x00,

    /**
     * Received Signal Strength Indication.
     * Range: -113 dBm and -51 dBm
     * Used RAN: GERAN, CDMA2000
     * Reference: 3GPP TS 27.007 section 8.5.
     */
    MT_RSSI = 0x01,

    /**
     * Received Signal Code Power.
     * Range: -120 dBm to -25 dBm;
     * Used RAN: UTRAN
     * Reference: 3GPP TS 25.123, section 9.1.1.1
     */
    MT_RSCP = 0x02,

    /**
     * Reference Signal Received Power.
     * Range: -140 dBm to -44 dBm;
     * Used RAN: EUTRAN
     * Reference: 3GPP TS 36.133 9.1.4
     */
    MT_RSRP = 0x03,

    /**
     * Reference Signal Received Quality
     * Range: -34 dB to 3 dB;
     * Used RAN: EUTRAN
     * Reference: 3GPP TS 36.133 v12.6.0 section 9.1.7
     */
    MT_RSRQ = 0x04,

    /**
     * Reference Signal Signal to Noise Ratio
     * Range: -20 dB to 30 dB;
     * Used RAN: EUTRAN
     * Note: this field is optional; support is left to the vendor.
     * Although the response code is not enforced, vendor's implementation
     * must ensure this interface doesn't crash.
     */
    MT_RSSNR = 0x05,

    /**
     * 5G SS reference signal received power.
     * Range: -140 dBm to -44 dBm.
     * Used RAN: NGRAN
     * Reference: 3GPP TS 38.215.
     */
    MT_SSRSRP = 0x06,

    /**
     * 5G SS reference signal received quality.
     * Range: -20 dB to -3 dB.
     * Used RAN: NGRAN
     * Reference: 3GPP TS 38.215.
     */
    MT_SSRSRQ = 0x07,

    /**
     * 5G SS signal-to-noise and interference ratio.
     * Range: -23 dB to 40 dB
     * Used RAN: NGRAN
     * Reference: 3GPP TS 38.215 section 5.1.*, 3GPP TS 38.133 section 10.1.16.1.
     */
    MT_SSSINR = 0x08
} RIL_SignalMeasureType;

typedef enum {
    GERAN_BAND_T380 = 1,
    GERAN_BAND_T410 = 2,
    GERAN_BAND_450 = 3,
    GERAN_BAND_480 = 4,
    GERAN_BAND_710 = 5,
    GERAN_BAND_750 = 6,
    GERAN_BAND_T810 = 7,
    GERAN_BAND_850 = 8,
    GERAN_BAND_P900 = 9,
    GERAN_BAND_E900 = 10,
    GERAN_BAND_R900 = 11,
    GERAN_BAND_DCS1800 = 12,
    GERAN_BAND_PCS1900 = 13,
    GERAN_BAND_ER900 = 14,
} RIL_GeranBands;

typedef enum {
    UTRAN_BAND_1 = 1,
    UTRAN_BAND_2 = 2,
    UTRAN_BAND_3 = 3,
    UTRAN_BAND_4 = 4,
    UTRAN_BAND_5 = 5,
    UTRAN_BAND_6 = 6,
    UTRAN_BAND_7 = 7,
    UTRAN_BAND_8 = 8,
    UTRAN_BAND_9 = 9,
    UTRAN_BAND_10 = 10,
    UTRAN_BAND_11 = 11,
    UTRAN_BAND_12 = 12,
    UTRAN_BAND_13 = 13,
    UTRAN_BAND_14 = 14,
    UTRAN_BAND_19 = 19,
    UTRAN_BAND_20 = 20,
    UTRAN_BAND_21 = 21,
    UTRAN_BAND_22 = 22,
    UTRAN_BAND_25 = 25,
    UTRAN_BAND_26 = 26,
} RIL_UtranBands;

typedef enum {
    EUTRAN_BAND_1 = 1,
    EUTRAN_BAND_2 = 2,
    EUTRAN_BAND_3 = 3,
    EUTRAN_BAND_4 = 4,
    EUTRAN_BAND_5 = 5,
    EUTRAN_BAND_6 = 6,
    EUTRAN_BAND_7 = 7,
    EUTRAN_BAND_8 = 8,
    EUTRAN_BAND_9 = 9,
    EUTRAN_BAND_10 = 10,
    EUTRAN_BAND_11 = 11,
    EUTRAN_BAND_12 = 12,
    EUTRAN_BAND_13 = 13,
    EUTRAN_BAND_14 = 14,
    EUTRAN_BAND_17 = 17,
    EUTRAN_BAND_18 = 18,
    EUTRAN_BAND_19 = 19,
    EUTRAN_BAND_20 = 20,
    EUTRAN_BAND_21 = 21,
    EUTRAN_BAND_22 = 22,
    EUTRAN_BAND_23 = 23,
    EUTRAN_BAND_24 = 24,
    EUTRAN_BAND_25 = 25,
    EUTRAN_BAND_26 = 26,
    EUTRAN_BAND_27 = 27,
    EUTRAN_BAND_28 = 28,
    EUTRAN_BAND_29 = 29,
    EUTRAN_BAND_30 = 30,
    EUTRAN_BAND_31 = 31,
    EUTRAN_BAND_32 = 32,
    EUTRAN_BAND_33 = 33,
    EUTRAN_BAND_34 = 34,
    EUTRAN_BAND_35 = 35,
    EUTRAN_BAND_36 = 36,
    EUTRAN_BAND_37 = 37,
    EUTRAN_BAND_38 = 38,
    EUTRAN_BAND_39 = 39,
    EUTRAN_BAND_40 = 40,
    EUTRAN_BAND_41 = 41,
    EUTRAN_BAND_42 = 42,
    EUTRAN_BAND_43 = 43,
    EUTRAN_BAND_44 = 44,
    EUTRAN_BAND_45 = 45,
    EUTRAN_BAND_46 = 46,
    EUTRAN_BAND_47 = 47,
    EUTRAN_BAND_48 = 48,
    EUTRAN_BAND_49 = 49,
    EUTRAN_BAND_65 = 65,
    EUTRAN_BAND_66 = 66,
    EUTRAN_BAND_67 = 67,
    EUTRAN_BAND_68 = 68,
    EUTRAN_BAND_70 = 70,
    EUTRAN_BAND_71 = 71,
    EUTRAN_BAND_72 = 72,
    EUTRAN_BAND_73 = 73,
    EUTRAN_BAND_85 = 85,
    EUTRAN_BAND_125 = 125,
    EUTRAN_BAND_126 = 126,
    EUTRAN_BAND_127 = 127,
    EUTRAN_BAND_250 = 250,
} RIL_EutranBands;

typedef enum {
    NGRAN_BAND_1 = 1,
    NGRAN_BAND_2 = 2,
    NGRAN_BAND_3 = 3,
    NGRAN_BAND_5 = 5,
    NGRAN_BAND_7 = 7,
    NGRAN_BAND_8 = 8,
    NGRAN_BAND_12 = 12,
    NGRAN_BAND_14 = 14,
    NGRAN_BAND_18 = 18,
    NGRAN_BAND_20 = 20,
    NGRAN_BAND_25 = 25,
    NGRAN_BAND_28 = 28,
    NGRAN_BAND_29 = 29,
    NGRAN_BAND_30 = 30,
    NGRAN_BAND_34 = 34,
    NGRAN_BAND_38 = 38,
    NGRAN_BAND_39 = 39,
    NGRAN_BAND_40 = 40,
    NGRAN_BAND_41 = 41,
    NGRAN_BAND_48 = 48,
    NGRAN_BAND_50 = 50,
    NGRAN_BAND_51 = 51,
    NGRAN_BAND_65 = 65,
    NGRAN_BAND_66 = 66,
    NGRAN_BAND_70 = 70,
    NGRAN_BAND_71 = 71,
    NGRAN_BAND_74 = 74,
    NGRAN_BAND_75 = 75,
    NGRAN_BAND_76 = 76,
    NGRAN_BAND_77 = 77,
    NGRAN_BAND_78 = 78,
    NGRAN_BAND_79 = 79,
    NGRAN_BAND_80 = 80,
    NGRAN_BAND_81 = 81,
    NGRAN_BAND_82 = 82,
    NGRAN_BAND_83 = 83,
    NGRAN_BAND_84 = 84,
    NGRAN_BAND_85 = 85,
    NGRAN_BAND_86 = 86,
    NGRAN_BAND_90 = 90,
    NGRAN_BAND_257 = 257,
    NGRAN_BAND_258 = 258,
    NGRAN_BAND_259 = 259,
    NGRAN_BAND_260 = 260,
    NGRAN_BAND_261 = 261,
} RIL_NgranBands;

typedef struct {
    RIL_RadioAccessNetworks radio_access_network; // The type of network to scan.
    uint32_t bands_length;                        // Length of bands
    union {
        RIL_GeranBands geran_bands[MAX_BANDS];
        RIL_UtranBands utran_bands[MAX_BANDS];
        RIL_EutranBands eutran_bands[MAX_BANDS];
        RIL_NgranBands ngran_bands[MAX_BANDS];
    } bands;
    uint32_t channels_length;                     // Length of channels
    uint32_t channels[MAX_CHANNELS];              // Frequency channels to scan
} RIL_RadioAccessSpecifier;

typedef struct {
    uint32_t specifiers_length;                                     // Length of specifiers
    RIL_RadioAccessSpecifier specifiers[MAX_RADIO_ACCESS_NETWORKS]; // Radio access networks
} RIL_SysSelChannels;

typedef struct {
    RIL_ScanType type;                                              // Type of the scan
    int32_t interval;                                               // Time interval in seconds
                                                                    // between periodic scans, only
                                                                    // valid when type=RIL_PERIODIC
    uint32_t specifiers_length;                                     // Length of specifiers
    RIL_RadioAccessSpecifier specifiers[MAX_RADIO_ACCESS_NETWORKS]; // Radio access networks
                                                                    // with bands/channels.

    int32_t maxSearchTime;                  // Maximum duration of the periodic search (in seconds).
                                            // If the search lasts maxSearchTime then
                                            // it must be terminated.
                                            // Default modem value should used if 0.

    int32_t incrementalResults;             // Indicates whether the modem should report incremental
                                            // results of the network scan to the client.
                                            // 0 – Incremental results are not reported.
                                            // 1 (default) – Incremental
                                            // results are reported

    int32_t incrementalResultsPeriodicity;  // Indicates the periodicity with which the
                                            // modem should report incremental results
                                            // to the client (in seconds).
                                            // Default modem value should be
                                            // used if 0

    uint32_t mccMncLength;                     // Length of mccMncs array
    char mccMncs[MAX_MCC_MNC_LIST_SIZE][MAX_MCC_MNC_LEN+1];// Describes the List of PLMN ids (MCC-MNC)
                                               // If any PLMN of this list is
                                               // found, search should end at that
                                               // point and results with all PLMN
                                               // found till that point should be
                                               // sent as response. If list not
                                               // sent, search to be completed till
                                               // end and all PLMNs found to be
                                               // reported.

} RIL_NetworkScanRequest;

/*nw_scan parameter limits , need to update based on changes in Iradio HAL*/
typedef enum {
    SCAN_MIN_INTERVAL = 5,
    SCAN_MAX_INTERVAL = 300,
    SCAN_MIN_SEARCH_TIME = 60,
    SCAN_MAX_SEARCH_TIME = 3600,
    SCAN_MIN_PERIODICITY = 1,
    SCAN_MAX_PERIODICITY = 10
}RIL_NetworkScanLimits;

typedef enum {
    PARTIAL = 0x01,   // The result contains a part of the scan results
    COMPLETE = 0x02,  // The result contains the last part of the scan results
} RIL_ScanStatus;

typedef struct {
    RIL_ScanStatus status;              // The status of the scan
    size_t network_infos_length;      // Total length of RIL_CellInfo
    RIL_CellInfo_v12* network_infos;    // List of network information
    RIL_Errno error;
} RIL_NetworkScanResult;

typedef enum {
    RIL_EMERGENCY_CALLBACK_MODE_EXIT = 0,
    RIL_EMERGENCY_CALLBACK_MODE_ENTER = 1,
} RIL_EmergencyCbMode;

typedef enum {
    /**
     * General emergency call, all categories
     */
    RIL_EMERGENCY_CATEGORY_UNSPECIFIED = 0,
    RIL_EMERGENCY_CATEGORY_POLICE = 1 /* (1 << 0) */,
    RIL_EMERGENCY_CATEGORY_AMBULANCE = 2 /* (1 << 1) */,
    RIL_EMERGENCY_CATEGORY_FIRE_BRIGADE = 4 /* (1 << 2) */,
    RIL_EMERGENCY_CATEGORY_MARINE_GUARD = 8 /* (1 << 3) */,
    RIL_EMERGENCY_CATEGORY_MOUNTAIN_RESCUE = 16 /* (1 << 4) */,
    /**
     * Manually Initiated eCall (MIeC)
     */
    RIL_EMERGENCY_CATEGORY_MIEC = 32 /* (1 << 5) */,
    /**
     * Automatically Initiated eCall (AIeC)
     */
    RIL_EMERGENCY_CATEGORY_AIEC = 64 /* (1 << 6) */,

} RIL_EmergencyServiceCategory;

typedef enum {
    /**
     * Indicates telephony does not require how to handle the corresponding emergency call; it is
     * decided by implementation.
     */
    RIL_ROUTING_UNKNOWN = 0,
    /**
     * Indicates the implementation must handle the call through emergency routing.
     */
    RIL_ROUTING_EMERGENCY = 1,
    /**
     * Indicates the implementation must handle the call through normal call routing.
     */
    RIL_ROUTING_NORMAL = 2,
} RIL_EmergencyCallRouting;

typedef struct {
    RIL_Dial dialInfo;                /* Dial information*/
    RIL_EmergencyServiceCategory categories;
                                      /* The Emergency Service Category(s) of the call */
    uint32_t urnLength;               /* Length of urns array */
    char urns[MAX_NUMBER_OF_URNS][MAX_URN_LENGTH];
                                      /*Describes the List of emergency Uniform Resource Names (URN)*/
    RIL_EmergencyCallRouting routing; /* The emergency call routing information */
    uint8_t fromEmergencyDialer;      /* Indicating if user's intent for the emergency call is known */
    uint8_t isTesting;                /* indicating if this request is for testing purpose */
} RIL_RequestEmergencyDial;

typedef enum {
    /**
     * Indicates the number is from the network signal.
     */
    RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING = 1 /* (1 << 0) */,
    /**
     * Indicates the number is from the sim card.
     */
    RIL_EMERGENCY_NUMBER_SOURCE_SIM = 2 /* (1 << 1) */,
    /**
     * Indicates the number is from the modem config.
     */
    RIL_EMERGENCY_NUMBER_SOURCE_MODEM_CONFIG = 4 /* (1 << 2) */,
    /**
     * Indicates the number is available as default. Per the reference, 112, 911 must always be
     * available; additionally, 000, 08, 110, 999, 118 and 119 must be available when sim is not
     * present.
     */
    RIL_EMERGENCY_NUMBER_SOURCE_DEFAULT = 8 /* (1 << 3) */,
} RIL_EmergencyNumberSource;

typedef struct {
    char number[MAX_EMERGENCY_NUMBER_LENGTH];
    char mcc[MAX_RIL_MCC_MNC_SIZE];
    char mnc[MAX_RIL_MCC_MNC_SIZE];
    uint32_t category;
    uint32_t sources;
    size_t no_of_urns;
    char urn[MAX_NUMBER_OF_URNS][MAX_URN_LENGTH];
} RIL_EmergencyNumberInfo;

typedef struct {
    uint32_t emergency_list_length;
    RIL_EmergencyNumberInfo emergency_numbers[MAX_EMERGENCY_NUMBERS];
} RIL_EmergencyList;

/** @public
 *  RIL_RadioDataRequestReasons
 */
typedef enum {
    /** NORMAL*/
    NORMAL      = 0x01,
    /** SHUTDOWN */
    SHUTDOWN    = 0x02,
    /** HANDOVER */
    HANDOVER    = 0x03,
} RIL_RadioDataRequestReasons;

typedef struct
{
    int32_t endc_available;
    int32_t restrict_dcnr;
} five_g_endc_dcnr;

typedef struct
{
    int32_t rsrp;
    int32_t snr;
} five_g_signal_strength;

typedef enum {
    FIVE_G_STATUS_DISABLED = 0,
    FIVE_G_STATUS_ENABLED = 1,
} five_g_status;

typedef enum {
    ENDC_STATUS_DISABLED = 0,
    ENDC_STATUS_ENABLED = 1,
} endc_status;

typedef enum {
    FIVE_G_BEARER_STATUS_NOT_ALLOCATED = 0,
    FIVE_G_BEARER_STATUS_ALLOCATED     = 1,
    FIVE_G_BEARER_STATUS_MMW_ALLOCATED = 2,
} five_g_bearer_status;

typedef enum {
    FIVE_G_UPPER_LAYER_IND_UNAVAILABLE = 0,
    FIVE_G_UPPER_LAYER_IND_AVAILABLE   = 1,
} five_g_upper_layer_ind_status;

typedef enum {
    FIVE_G_PLMN_LIST_UNAVAILABLE = 0,
    FIVE_G_PLMN_LIST_AVAILABLE   = 1,
} five_g_plmn_info_list_status;

typedef enum {
    FIVE_G_CONFIG_TYPE_NSA = 0,
    FIVE_G_CONFIG_TYPE_SA  = 1,
} five_g_config_type;

/** @public
 * five_g_icon_type
 */
typedef enum {
    /** INVALID */
    FIVE_G_ICON_TYPE_INVALID  = -1,
    /** NONE */
    FIVE_G_ICON_TYPE_NONE     = 0,
    /** BASIC */
    FIVE_G_ICON_TYPE_BASIC    = 1,
    /** Ultra Wideband */
    FIVE_G_ICON_TYPE_UWB      = 2,
} five_g_icon_type;

typedef struct {
    five_g_plmn_info_list_status  plmn_list_status;
    five_g_upper_layer_ind_status upper_layer_ind_info_status;
} five_g_upper_layer_ind_info;

#define RIL_MAX_NUM_EMAIL_COUNT (2)
#define RIL_MAX_NUM_AD_COUNT (4)
typedef struct {
    size_t record_id;
    char *name;
    char *number;
    size_t email_elements;
    char *email[RIL_MAX_NUM_EMAIL_COUNT];
    size_t anr_elements;
    char *ad_number[RIL_MAX_NUM_AD_COUNT];
} RIL_AdnRecordInfo;

#define RIL_NUM_ADN_RECORDS (10)

typedef struct {
    size_t record_elements;
    RIL_AdnRecordInfo adn_record_info[RIL_NUM_ADN_RECORDS];
} RIL_AdnRecords;

typedef struct {
  int                       max_adn_num;
  int                       valid_adn_num;
  int                       max_email_num;
  int                       valid_email_num;
  int                       max_ad_num;
  int                       valid_ad_num;
  int                       max_name_len;
  int                       max_number_len;
  int                       max_email_len;
  int                       max_anr_len;
} RIL_AdnCountInfo;

typedef struct {
    /** The type of network for which to apply these thresholds. */
    RIL_RadioAccessNetworks accessNetwork;

    /** Signal Measurement Type */
    RIL_SignalMeasureType signalStrengthParameter;

    /**
     * Indicates whether the reporting criteria of the corresponding measurement is enabled
     * (isEnabled == 1) or disabled (isEnabled == 0).
     *
     * If enabled, modem must trigger the report based on the criteria.
     * If disabled, modem must not trigger the report based on the criteria.
     */
    uint8_t isEnabled;

    /** A hysteresis time in milliseconds to prevent flapping. A value of 0 disables hysteresis. */
    int32_t hysteresisMs;

    /**
     * An interval in dB defining the required magnitude change between reports.
     * hysteresisDb must be smaller than the smallest threshold delta.
     * An interval value of 0 disables hysteresis.
     */
    int32_t hysteresisDb;

    /**
     * List of threshold values.
     * The length of the list must be provided in the "thresholdsLength" field.
     * Range and unit must reference specific RIL_SignalMeasureType.
     * The threshold values for which to apply criteria.
     */
    int32_t* thresholds;

    /** Number of threshold values in the list supplied in "thresholds".
     * A value of 0 disables the use of thresholds for reporting.
    */
    uint32_t thresholdsLength;
} RIL_SignalStrengthReportingCriteria;

typedef enum {
    /** UNKNOWN */
    QTI_RADIO_TECH_UNKNOWN = 0,
    /** GPRS */
    QTI_RADIO_TECH_GPRS = 1,
    /** EDGE */
    QTI_RADIO_TECH_EDGE = 2,
    /** UMTS */
    QTI_RADIO_TECH_UMTS = 3,
    /** IS95A */
    QTI_RADIO_TECH_IS95A = 4,
    /** IS95B */
    QTI_RADIO_TECH_IS95B = 5,
    /** RTT */
    QTI_RADIO_TECH_1xRTT =  6,
    /** EVDO_0 */
    QTI_RADIO_TECH_EVDO_0 = 7,
    /** EVDO_A */
    QTI_RADIO_TECH_EVDO_A = 8,
    /** HSDPA */
    QTI_RADIO_TECH_HSDPA = 9,
    /** HSUPA */
    QTI_RADIO_TECH_HSUPA = 10,
    /** HSPA */
    QTI_RADIO_TECH_HSPA = 11,
    /** EVDO_B */
    QTI_RADIO_TECH_EVDO_B = 12,
    /** EHRPD */
    QTI_RADIO_TECH_EHRPD = 13,
    /** LTE */
    QTI_RADIO_TECH_LTE = 14,
    /** HSPAP */
    QTI_RADIO_TECH_HSPAP = 15, // HSPA+
    /** GSM */
    QTI_RADIO_TECH_GSM = 16, // Only supports voice
    /** TD_CDMA */
    QTI_RADIO_TECH_TD_SCDMA = 17,
    /** IWLAN */
    QTI_RADIO_TECH_IWLAN = 18,
    /** LTE_CA */
    QTI_RADIO_TECH_LTE_CA = 19,
    /** 5G NSA*/
    QTI_RADIO_TECH_NR_NSA = 20,
    /** 5G SA*/
    QTI_RADIO_TECH_NR_SA = 21,
} QtiRadioTechnology;

typedef enum {
    QTI_RAF_UNKNOWN =  (1 <<  QTI_RADIO_TECH_UNKNOWN),
    QTI_RAF_GPRS = (1 << QTI_RADIO_TECH_GPRS),
    QTI_RAF_EDGE = (1 << QTI_RADIO_TECH_EDGE),
    QTI_RAF_UMTS = (1 << QTI_RADIO_TECH_UMTS),
    QTI_RAF_IS95A = (1 << QTI_RADIO_TECH_IS95A),
    QTI_RAF_IS95B = (1 << QTI_RADIO_TECH_IS95B),
    QTI_RAF_1xRTT = (1 << QTI_RADIO_TECH_1xRTT),
    QTI_RAF_EVDO_0 = (1 << QTI_RADIO_TECH_EVDO_0),
    QTI_RAF_EVDO_A = (1 << QTI_RADIO_TECH_EVDO_A),
    QTI_RAF_HSDPA = (1 << QTI_RADIO_TECH_HSDPA),
    QTI_RAF_HSUPA = (1 << QTI_RADIO_TECH_HSUPA),
    QTI_RAF_HSPA = (1 << QTI_RADIO_TECH_HSPA),
    QTI_RAF_EVDO_B = (1 << QTI_RADIO_TECH_EVDO_B),
    QTI_RAF_EHRPD = (1 << QTI_RADIO_TECH_EHRPD),
    QTI_RAF_LTE = (1 << QTI_RADIO_TECH_LTE),
    QTI_RAF_HSPAP = (1 << QTI_RADIO_TECH_HSPAP),
    QTI_RAF_GSM = (1 << QTI_RADIO_TECH_GSM),
    QTI_RAF_TD_SCDMA = (1 << QTI_RADIO_TECH_TD_SCDMA),
    QTI_RAF_LTE_CA = (1 << QTI_RADIO_TECH_LTE_CA),
    QTI_RAF_NR_NSA = (1 << QTI_RADIO_TECH_NR_NSA),
    QTI_RAF_NR_SA = (1 << QTI_RADIO_TECH_NR_SA),
} QtiRadioAccessFamily;

/**
 * RIL_REQUEST_GET_SIM_STATUS
 *
 * Requests status of the SIM interface and the SIM card
 *
 * "data" is NULL
 *
 * "response" is const RIL_CardStatus_v6 *
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_GET_SIM_STATUS 1

/**
 * RIL_REQUEST_ENTER_SIM_PIN
 *
 * Supplies SIM PIN. Only called if RIL_CardStatus has RIL_APPSTATE_PIN state
 *
 * "data" is const char **
 * ((const char **)data)[0] is PIN value
 * ((const char **)data)[1] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *
 * SUCCESS
 * RADIO_NOT_AVAILABLE (radio resetting)
 * PASSWORD_INCORRECT
 * INTERNAL_ERR
 * NO_MEMORY
 * NO_RESOURCES
 * CANCELLED
 * INVALID_ARGUMENTS
 * INVALID_SIM_STATE
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_ENTER_SIM_PIN 2

/**
 * RIL_REQUEST_ENTER_SIM_PUK
 *
 * Supplies SIM PUK and new PIN.
 *
 * "data" is const char **
 * ((const char **)data)[0] is PUK value
 * ((const char **)data)[1] is new PIN value
 * ((const char **)data)[2] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  PASSWORD_INCORRECT
 *     (PUK is invalid)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_ARGUMENTS
 *  INVALID_SIM_STATE
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_ENTER_SIM_PUK 3

/**
 * RIL_REQUEST_ENTER_SIM_PIN2
 *
 * Supplies SIM PIN2. Only called following operation where SIM_PIN2 was
 * returned as a a failure from a previous operation.
 *
 * "data" is const char **
 * ((const char **)data)[0] is PIN2 value
 * ((const char **)data)[1] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  PASSWORD_INCORRECT
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_ARGUMENTS
 *  INVALID_SIM_STATE
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_ENTER_SIM_PIN2 4

/**
 * RIL_REQUEST_ENTER_SIM_PUK2
 *
 * Supplies SIM PUK2 and new PIN2.
 *
 * "data" is const char **
 * ((const char **)data)[0] is PUK2 value
 * ((const char **)data)[1] is new PIN2 value
 * ((const char **)data)[2] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  PASSWORD_INCORRECT
 *     (PUK2 is invalid)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_ARGUMENTS
 *  INVALID_SIM_STATE
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_ENTER_SIM_PUK2 5

/**
 * RIL_REQUEST_CHANGE_SIM_PIN
 *
 * Supplies old SIM PIN and new PIN.
 *
 * "data" is const char **
 * ((const char **)data)[0] is old PIN value
 * ((const char **)data)[1] is new PIN value
 * ((const char **)data)[2] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  PASSWORD_INCORRECT
 *     (old PIN is invalid)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_ARGUMENTS
 *  INVALID_SIM_STATE
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_CHANGE_SIM_PIN 6


/**
 * RIL_REQUEST_CHANGE_SIM_PIN2
 *
 * Supplies old SIM PIN2 and new PIN2.
 *
 * "data" is const char **
 * ((const char **)data)[0] is old PIN2 value
 * ((const char **)data)[1] is new PIN2 value
 * ((const char **)data)[2] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  PASSWORD_INCORRECT
 *     (old PIN2 is invalid)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_ARGUMENTS
 *  INVALID_SIM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 */

#define RIL_REQUEST_CHANGE_SIM_PIN2 7

/**
 * RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION
 *
 * Requests that network personlization be deactivated
 *
 * "data" is const char **
 * ((const char **)(data))[0]] is network depersonlization code
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  PASSWORD_INCORRECT
 *  SIM_ABSENT
 *     (code is invalid)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION 8

/**
 * RIL_REQUEST_GET_CURRENT_CALLS
 *
 * Requests current call list
 *
 * "data" is NULL
 *
 * "response" must be a "const RIL_Call **"
 *
 * Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *      (request will be made again in a few hundred msec)
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_GET_CURRENT_CALLS 9


/**
 * RIL_REQUEST_DIAL
 *
 * Initiate voice call
 *
 * "data" is const RIL_Dial *
 * "response" is NULL
 *
 * This method is never used for supplementary service codes
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  DIAL_MODIFIED_TO_USSD
 *  DIAL_MODIFIED_TO_SS
 *  DIAL_MODIFIED_TO_DIAL
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  INVALID_STATE
 *  NO_RESOURCES
 *  INTERNAL_ERR
 *  FDN_CHECK_FAILURE
 *  MODEM_ERR
 *  NO_SUBSCRIPTION
 *  NO_NETWORK_FOUND
 *  INVALID_CALL_ID
 *  DEVICE_IN_USE
 *  OPERATION_NOT_ALLOWED
 *  ABORTED
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_DIAL 10

/**
 * RIL_REQUEST_GET_IMSI
 *
 * Get the SIM IMSI
 *
 * Only valid when radio state is "RADIO_STATE_ON"
 *
 * "data" is const char **
 * ((const char **)data)[0] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 * "response" is a const char * containing the IMSI
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_SIM_STATE
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_GET_IMSI 11

/**
 * RIL_REQUEST_HANGUP
 *
 * Hang up a specific line (like AT+CHLD=1x)
 *
 * After this HANGUP request returns, RIL should show the connection is NOT
 * active anymore in next RIL_REQUEST_GET_CURRENT_CALLS query.
 *
 * "data" is an int *
 * (int *)data)[0] contains Connection index (value of 'x' in CHLD above)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  INVALID_STATE
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_CALL_ID
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_HANGUP 12

/**
 * RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND
 *
 * Hang up waiting or held (like AT+CHLD=0)
 *
 * After this HANGUP request returns, RIL should show the connection is NOT
 * active anymore in next RIL_REQUEST_GET_CURRENT_CALLS query.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_CALL_ID
 *  NO_RESOURCES
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND 13

/**
 * RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND
 *
 * Hang up waiting or held (like AT+CHLD=1)
 *
 * After this HANGUP request returns, RIL should show the connection is NOT
 * active anymore in next RIL_REQUEST_GET_CURRENT_CALLS query.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND 14

/**
 * RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE
 *
 * Switch waiting or holding call and active call (like AT+CHLD=2)
 *
 * State transitions should be is follows:
 *
 * If call 1 is waiting and call 2 is active, then if this re
 *
 *   BEFORE                               AFTER
 * Call 1   Call 2                 Call 1       Call 2
 * ACTIVE   HOLDING                HOLDING     ACTIVE
 * ACTIVE   WAITING                HOLDING     ACTIVE
 * HOLDING  WAITING                HOLDING     ACTIVE
 * ACTIVE   IDLE                   HOLDING     IDLE
 * IDLE     IDLE                   IDLE        IDLE
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  INVALID_CALL_ID
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE 15
#define RIL_REQUEST_SWITCH_HOLDING_AND_ACTIVE 15

/**
 * RIL_REQUEST_CONFERENCE
 *
 * Conference holding and active (like AT+CHLD=3)

 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_STATE
 *  INVALID_CALL_ID
 *  INVALID_ARGUMENTS
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_CONFERENCE 16

/**
 * RIL_REQUEST_UDUB
 *
 * Send UDUB (user determined used busy) to ringing or
 * waiting call answer)(RIL_BasicRequest r);
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_RESOURCES
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_UDUB 17

/**
 * RIL_REQUEST_LAST_CALL_FAIL_CAUSE
 *
 * Requests the failure cause code for the most recently terminated call
 *
 * "data" is NULL
 * "response" is a const RIL_LastCallFailCauseInfo *
 * RIL_LastCallFailCauseInfo contains LastCallFailCause and vendor cause.
 * The vendor cause code must be used for debugging purpose only.
 * The implementation must return one of the values of LastCallFailCause
 * as mentioned below.
 *
 * GSM failure reasons codes for the cause codes defined in TS 24.008 Annex H
 * where possible.
 * CDMA failure reasons codes for the possible call failure scenarios
 * described in the "CDMA IS-2000 Release A (C.S0005-A v6.0)" standard.
 * Any of the following reason codes if the call is failed or dropped due to reason
 * mentioned with in the braces.
 *
 *      CALL_FAIL_RADIO_OFF (Radio is OFF)
 *      CALL_FAIL_OUT_OF_SERVICE (No cell coverage)
 *      CALL_FAIL_NO_VALID_SIM (No valid SIM)
 *      CALL_FAIL_RADIO_INTERNAL_ERROR (Modem hit unexpected error scenario)
 *      CALL_FAIL_NETWORK_RESP_TIMEOUT (No response from network)
 *      CALL_FAIL_NETWORK_REJECT (Explicit network reject)
 *      CALL_FAIL_RADIO_ACCESS_FAILURE (RRC connection failure. Eg.RACH)
 *      CALL_FAIL_RADIO_LINK_FAILURE (Radio Link Failure)
 *      CALL_FAIL_RADIO_LINK_LOST (Radio link lost due to poor coverage)
 *      CALL_FAIL_RADIO_UPLINK_FAILURE (Radio uplink failure)
 *      CALL_FAIL_RADIO_SETUP_FAILURE (RRC connection setup failure)
 *      CALL_FAIL_RADIO_RELEASE_NORMAL (RRC connection release, normal)
 *      CALL_FAIL_RADIO_RELEASE_ABNORMAL (RRC connection release, abnormal)
 *      CALL_FAIL_ACCESS_CLASS_BLOCKED (Access class barring)
 *      CALL_FAIL_NETWORK_DETACH (Explicit network detach)
 *
 * OEM causes (CALL_FAIL_OEM_CAUSE_XX) must be used for debug purpose only
 *
 * If the implementation does not have access to the exact cause codes,
 * then it should return one of the values listed in RIL_LastCallFailCause,
 * as the UI layer needs to distinguish these cases for tone generation or
 * error notification.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE
 */
#define RIL_REQUEST_LAST_CALL_FAIL_CAUSE 18

/**
 * RIL_REQUEST_SIGNAL_STRENGTH
 *
 * Requests current signal strength and associated information
 *
 * Must succeed if radio is on.
 *
 * "data" is NULL
 *
 * "response" is a const RIL_SignalStrength *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SIGNAL_STRENGTH 19

/**
 * RIL_REQUEST_VOICE_REGISTRATION_STATE
 *
 * Request current registration state
 *
 * "data" is NULL
 * "response" is a const RIL_VoiceRegistrationStateResponse *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_VOICE_REGISTRATION_STATE 20

/**
 * RIL_REQUEST_DATA_REGISTRATION_STATE
 *
 * Request current DATA registration state
 *
 * "data" is NULL
 * "response" is a const RIL_DataRegistrationStateResponse *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_DATA_REGISTRATION_STATE 21

/**
 * RIL_REQUEST_OPERATOR
 *
 * Request current operator ONS or EONS
 *
 * "data" is NULL
 * "response" is a "const char **"
 * ((const char **)response)[0] is long alpha ONS or EONS
 *                                  or NULL if unregistered
 *
 * ((const char **)response)[1] is short alpha ONS or EONS
 *                                  or NULL if unregistered
 * ((const char **)response)[2] is 5 or 6 digit numeric code (MCC + MNC)
 *                                  or NULL if unregistered
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_OPERATOR 22

/**
 * RIL_REQUEST_RADIO_POWER
 *
 * Toggle radio on and off (for "airplane" mode)
 * If the radio is is turned off/on the radio modem subsystem
 * is expected return to an initialized state. For instance,
 * any voice and data calls will be terminated and all associated
 * lists emptied.
 *
 * "data" is int *
 * ((int *)data)[0] is > 0 for "Radio On"
 * ((int *)data)[0] is == 0 for "Radio Off"
 *
 * "response" is NULL
 *
 * Turn radio on if "on" > 0
 * Turn radio off if "on" == 0
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_STATE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  DEVICE_IN_USE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_RADIO_POWER 23

/**
 * RIL_REQUEST_DTMF
 *
 * Send a DTMF tone
 *
 * If the implementation is currently playing a tone requested via
 * RIL_REQUEST_DTMF_START, that tone should be cancelled and the new tone
 * should be played instead
 *
 * "data" is a char * containing a single character with one of 12 values: 0-9,*,#
 * "response" is NULL
 *
 * FIXME should this block/mute microphone?
 * How does this interact with local DTMF feedback?
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_DTMF_STOP, RIL_REQUEST_DTMF_START
 *
 */
#define RIL_REQUEST_DTMF 24

/**
 * RIL_REQUEST_SEND_SMS
 *
 * Send an SMS message
 *
 * "data" is a const RIL_GsmSmsMessage *
 *
 * "response" is a const RIL_SMS_Response *
 *
 * Based on the return error, caller decides to resend if sending sms
 * fails. SMS_SEND_FAIL_RETRY means retry (i.e. error cause is 332)
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SMS_SEND_FAIL_RETRY
 *  FDN_CHECK_FAILURE
 *  NETWORK_REJECT
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  INVALID_SMS_FORMAT
 *  SYSTEM_ERR
 *  ENCODING_ERR
 *  INVALID_SMSC_ADDRESS
 *  MODEM_ERR
 *  NETWORK_ERR
 *  OPERATION_NOT_ALLOWED
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  MODE_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 * FIXME how do we specify TP-Message-Reference if we need to resend?
 */
#define RIL_REQUEST_SEND_SMS 25


/**
 * RIL_REQUEST_SEND_SMS_EXPECT_MORE
 *
 * Send an SMS message. Identical to RIL_REQUEST_SEND_SMS,
 * except that more messages are expected to be sent soon. If possible,
 * keep SMS relay protocol link open (eg TS 27.005 AT+CMMS command)
 *
 * "data" is const char **
 * ((const char **)data)[0] is SMSC address in GSM BCD format prefixed
 *      by a length byte (as expected by TS 27.005) or NULL for default SMSC
 * ((const char **)data)[1] is SMS in PDU format as an ASCII hex string
 *      less the SMSC address
 *      TP-Layer-Length is be "strlen(((const char **)data)[1])/2"
 *
 * "response" is a const RIL_SMS_Response *
 *
 * Based on the return error, caller decides to resend if sending sms
 * fails. SMS_SEND_FAIL_RETRY means retry (i.e. error cause is 332)
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SMS_SEND_FAIL_RETRY
 *  NETWORK_REJECT
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  INVALID_SMS_FORMAT
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
 *  FDN_CHECK_FAILURE
 *  MODEM_ERR
 *  NETWORK_ERR
 *  ENCODING_ERR
 *  INVALID_SMSC_ADDRESS
 *  OPERATION_NOT_ALLOWED
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  MODE_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 */
#define RIL_REQUEST_SEND_SMS_EXPECT_MORE 26


/**
 * RIL_REQUEST_SETUP_DATA_CALL
 *
 * Setup a packet data connection. If RIL_Data_Call_Response_v6.status
 * return success it is added to the list of data calls and a
 * RIL_UNSOL_DATA_CALL_LIST_CHANGED is sent. The call remains in the
 * list until RIL_REQUEST_DEACTIVATE_DATA_CALL is issued or the
 * radio is powered off/on. This list is returned by RIL_REQUEST_DATA_CALL_LIST
 * and RIL_UNSOL_DATA_CALL_LIST_CHANGED.
 *
 * The RIL is expected to:
 *  - Create one data call context.
 *  - Create and configure a dedicated interface for the context
 *  - The interface must be point to point.
 *  - The interface is configured with one or more addresses and
 *    is capable of sending and receiving packets. The prefix length
 *    of the addresses must be /32 for IPv4 and /128 for IPv6.
 *  - Must NOT change the linux routing table.
 *  - Support up to RIL_REQUEST_DATA_REGISTRATION_STATE response[5]
 *    number of simultaneous data call contexts.
 *
 * "data" is setupdatacall params, RIL_SetUpDataCallParams
 *
 * "response" is a RIL_Data_Call_Response_v11
 *
 * FIXME may need way to configure QoS settings
 *
 * Valid errors:
 *  SUCCESS should be returned on both success and failure of setup with
 *  the RIL_Data_Call_Response_v6.status containing the actual status.
 *  For all other errors the RIL_Data_Call_Resonse_v6 is ignored.
 *
 *  Other errors could include:
 *    RADIO_NOT_AVAILABLE, OP_NOT_ALLOWED_BEFORE_REG_TO_NW,
 *    OP_NOT_ALLOWED_DURING_VOICE_CALL, REQUEST_NOT_SUPPORTED,
 *    INVALID_ARGUMENTS, INTERNAL_ERR, NO_MEMORY, NO_RESOURCES,
 *    CANCELLED and SIM_ABSENT
 *
 * See also: RIL_REQUEST_DEACTIVATE_DATA_CALL
 */
#define RIL_REQUEST_SETUP_DATA_CALL 27


/**
 * RIL_REQUEST_SIM_IO
 *
 * Request SIM I/O operation.
 * This is similar to the TS 27.007 "restricted SIM" operation
 * where it assumes all of the EF selection will be done by the
 * callee.
 *
 * "data" is a const RIL_SIM_IO_v6 *
 * Please note that RIL_SIM_IO has a "PIN2" field which may be NULL,
 * or may specify a PIN2 for operations that require a PIN2 (eg
 * updating FDN records)
 *
 * "response" is a const RIL_SIM_IO_Response *
 *
 * Arguments and responses that are unused for certain
 * values of "command" should be ignored or set to NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SIM_PIN2
 *  SIM_PUK2
 *  INVALID_SIM_STATE
 *  SIM_ERR
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SIM_IO 28

/**
 * RIL_REQUEST_SEND_USSD
 *
 * Send a USSD message
 *
 * If a USSD session already exists, the message should be sent in the
 * context of that session. Otherwise, a new session should be created.
 *
 * The network reply should be reported via RIL_UNSOL_ON_USSD
 *
 * Only one USSD session may exist at a time, and the session is assumed
 * to exist until:
 *   a) The android system invokes RIL_REQUEST_CANCEL_USSD
 *   b) The implementation sends a RIL_UNSOL_ON_USSD with a type code
 *      of "0" (USSD-Notify/no further action) or "2" (session terminated)
 *
 * "data" is a const char * containing the USSD request in UTF-8 format
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  FDN_CHECK_FAILURE
 *  USSD_MODIFIED_TO_DIAL
 *  USSD_MODIFIED_TO_SS
 *  USSD_MODIFIED_TO_USSD
 *  SIM_BUSY
 *  OPERATION_NOT_ALLOWED
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  ABORTED
 *  SYSTEM_ERR
 *  INVALID_STATE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_CANCEL_USSD, RIL_UNSOL_ON_USSD
 */

#define RIL_REQUEST_SEND_USSD 29

/**
 * RIL_REQUEST_CANCEL_USSD
 *
 * Cancel the current USSD session if one exists
 *
 * "data" is null
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SIM_BUSY
 *  OPERATION_NOT_ALLOWED
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_STATE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_CANCEL_USSD 30

/**
 * RIL_REQUEST_GET_CLIR
 *
 * Gets current CLIR status
 * "data" is NULL
 * "response" is int *
 * ((int *)data)[0] is "n" parameter from TS 27.007 7.7
 * ((int *)data)[1] is "m" parameter from TS 27.007 7.7
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  FDN_CHECK_FAILURE
 *  SYSTEM_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_GET_CLIR 31

/**
 * RIL_REQUEST_SET_CLIR
 *
 * "data" is int *
 * ((int *)data)[0] is "n" parameter from TS 27.007 7.7
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  SYSTEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SET_CLIR 32

/**
 * RIL_REQUEST_QUERY_CALL_FORWARD_STATUS
 *
 * "data" is const RIL_CallForwardInfo *
 *
 * "response" is const RIL_CallForwardInfo **
 * "response" points to an array of RIL_CallForwardInfo *'s, one for
 * each distinct registered phone number.
 *
 * For example, if data is forwarded to +18005551212 and voice is forwarded
 * to +18005559999, then two separate RIL_CallForwardInfo's should be returned
 *
 * If, however, both data and voice are forwarded to +18005551212, then
 * a single RIL_CallForwardInfo can be returned with the service class
 * set to "data + voice = 3")
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_QUERY_CALL_FORWARD_STATUS 33


/**
 * RIL_REQUEST_SET_CALL_FORWARD
 *
 * Configure call forward rule
 *
 * "data" is const RIL_CallForwardInfo *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_STATE
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SET_CALL_FORWARD 34


/**
 * RIL_REQUEST_QUERY_CALL_WAITING
 *
 * Query current call waiting state
 *
 * "data" is const int *
 * ((const int *)data)[0] is the TS 27.007 service class to query.
 * "response" is a const int *
 * ((const int *)response)[0] is 0 for "disabled" and 1 for "enabled"
 *
 * If ((const int *)response)[0] is = 1, then ((const int *)response)[1]
 * must follow, with the TS 27.007 service class bit vector of services
 * for which call waiting is enabled.
 *
 * For example, if ((const int *)response)[0]  is 1 and
 * ((const int *)response)[1] is 3, then call waiting is enabled for data
 * and voice and disabled for everything else
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  FDN_CHECK_FAILURE
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_QUERY_CALL_WAITING 35


/**
 * RIL_REQUEST_SET_CALL_WAITING
 *
 * Configure current call waiting state
 *
 * "data" is const int *
 * ((const int *)data)[0] is 0 for "disabled" and 1 for "enabled"
 * ((const int *)data)[1] is the TS 27.007 service class bit vector of
 *                           services to modify
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_STATE
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SET_CALL_WAITING 36

/**
 * RIL_REQUEST_SMS_ACKNOWLEDGE
 *
 * Acknowledge successful or failed receipt of SMS previously indicated
 * via RIL_UNSOL_RESPONSE_NEW_SMS
 *
 * "data" is a const RIL_GsmSmsAck *
 *
 * "response" is NULL
 *
 * FIXME would like request that specified RP-ACK/RP-ERROR PDU
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SMS_ACKNOWLEDGE  37

/**
 * RIL_REQUEST_GET_IMEI - DEPRECATED
 *
 * Get the device IMEI, including check digit
 *
 * The request is DEPRECATED, use RIL_REQUEST_DEVICE_IDENTITY
 * Valid when RadioState is not RADIO_STATE_UNAVAILABLE
 *
 * "data" is NULL
 * "response" is a const char * containing the IMEI
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */

#define RIL_REQUEST_GET_IMEI 38

/**
 * RIL_REQUEST_GET_IMEISV - DEPRECATED
 *
 * Get the device IMEISV, which should be two decimal digits
 *
 * The request is DEPRECATED, use RIL_REQUEST_DEVICE_IDENTITY
 * Valid when RadioState is not RADIO_STATE_UNAVAILABLE
 *
 * "data" is NULL
 * "response" is a const char * containing the IMEISV
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */

#define RIL_REQUEST_GET_IMEISV 39


/**
 * RIL_REQUEST_ANSWER
 *
 * Answer incoming call
 *
 * Will not be called for WAITING calls.
 * RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE will be used in this case
 * instead
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_ANSWER 40

/**
 * RIL_REQUEST_DEACTIVATE_DATA_CALL
 *
 * Deactivate packet data connection and remove from the
 * data call list if SUCCESS is returned. Any other return
 * values should also try to remove the call from the list,
 * but that may not be possible. In any event a
 * RIL_REQUEST_RADIO_POWER off/on must clear the list. An
 * RIL_UNSOL_DATA_CALL_LIST_CHANGED is not expected to be
 * issued because of an RIL_REQUEST_DEACTIVATE_DATA_CALL.
 *
 * "data" is const char **
 * ((char**)data)[0] indicating CID
 * ((char**)data)[1] indicating Disconnect Reason
 *                   0 => No specific reason specified
 *                   1 => Radio shutdown requested
 * ((char**)data)[3] The request reason. Must be normal,
 *                   handover, or shutdown RIL_RadioDataRequestReasons.
 *                   NORMAL => The reason of the data request is normal
 *                   SHUTDOWN => The reason of the data request is device shutdown
 *                   HANDOVER => The reason of the data request is IWLAN data handover
 *                   to another transport (e.g. from cellular to wifi or vise versa)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_CALL_ID
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  REQUEST_NOT_SUPPORTED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  SIM_ABSENT
 *
 * See also: RIL_REQUEST_SETUP_DATA_CALL
 */
#define RIL_REQUEST_DEACTIVATE_DATA_CALL 41

/**
 * RIL_REQUEST_QUERY_FACILITY_LOCK
 *
 * Query the status of a facility lock state
 *
 * "data" is const char **
 * ((const char **)data)[0] is the facility string code from TS 27.007 7.4
 *                      (eg "AO" for BAOC, "SC" for SIM lock)
 * ((const char **)data)[1] is the password, or "" if not required
 * ((const char **)data)[2] is the TS 27.007 service class bit vector of
 *                           services to query
 * ((const char **)data)[3] is AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *                            This is only applicable in the case of Fixed Dialing Numbers
 *                            (FDN) requests.
 *
 * "response" is an int *
 * ((const int *)response) 0 is the TS 27.007 service class bit vector of
 *                           services for which the specified barring facility
 *                           is active. "0" means "disabled for all"
 *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_QUERY_FACILITY_LOCK 42

/**
 * RIL_REQUEST_SET_FACILITY_LOCK
 *
 * Enable/disable one facility lock
 *
 * "data" is const char **
 *
 * ((const char **)data)[0] = facility string code from TS 27.007 7.4
 * (eg "AO" for BAOC)
 * ((const char **)data)[1] = "0" for "unlock" and "1" for "lock"
 * ((const char **)data)[2] = password
 * ((const char **)data)[3] = string representation of decimal TS 27.007
 *                            service class bit vector. Eg, the string
 *                            "1" means "set this facility for voice services"
 * ((const char **)data)[4] = AID value, See ETSI 102.221 8.1 and 101.220 4, NULL if no value.
 *                            This is only applicable in the case of Fixed Dialing Numbers
 *                            (FDN) requests.
 *
 * "response" is int *
 * ((int *)response)[0] is the number of retries remaining, or -1 if unknown
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  MODEM_ERR
 *  INVALID_STATE
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_SET_FACILITY_LOCK 43

/**
 * RIL_REQUEST_CHANGE_BARRING_PASSWORD
 *
 * Change call barring facility password
 *
 * "data" is const char **
 *
 * ((const char **)data)[0] = facility string code from TS 27.007 7.4
 * (eg "AO" for BAOC)
 * ((const char **)data)[1] = old password
 * ((const char **)data)[2] = new password
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_CHANGE_BARRING_PASSWORD 44

/**
 * RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE
 *
 * Query current network selectin mode
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((const int *)response)[0] is
 *     0 for automatic selection
 *     1 for manual selection
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE 45

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC
 *
 * Specify that the network should be selected automatically
 *
 * "data" is NULL
 * "response" is NULL
 *
 * This request must not respond until the new operator is selected
 * and registered
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  ILLEGAL_SIM_OR_ME
 *  OPERATION_NOT_ALLOWED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 * Note: Returns ILLEGAL_SIM_OR_ME when the failure is permanent and
 *       no retries needed, such as illegal SIM or ME.
 *
 */
#define RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC 46

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL
 *
 * Manually select a specified network.
 *
 * "data" is const char * specifying MCCMNC of network to select (eg "310170")
 * "response" is NULL
 *
 * This request must not respond until the new operator is selected
 * and registered
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  ILLEGAL_SIM_OR_ME
 *  OPERATION_NOT_ALLOWED
 *  INVALID_STATE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 * Note: Returns ILLEGAL_SIM_OR_ME when the failure is permanent and
 *       no retries needed, such as illegal SIM or ME.
 *
 */
#define RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL 47

/**
 * RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
 *
 * Scans for available networks
 *
 * "data" is NULL
 * "reponse" is a const RIL_OperatorInfo *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  ABORTED
 *  DEVICE_IN_USE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  CANCELLED
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_QUERY_AVAILABLE_NETWORKS 48

/**
 * RIL_REQUEST_DTMF_START
 *
 * Start playing a DTMF tone. Continue playing DTMF tone until
 * RIL_REQUEST_DTMF_STOP is received
 *
 * If a RIL_REQUEST_DTMF_START is received while a tone is currently playing,
 * it should cancel the previous tone and play the new one.
 *
 * "data" is a char *
 * ((char *)data)[0] is a single character with one of 12 values: 0-9,*,#
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_DTMF, RIL_REQUEST_DTMF_STOP
 */
#define RIL_REQUEST_DTMF_START 49

/**
 * RIL_REQUEST_DTMF_STOP
 *
 * Stop playing a currently playing DTMF tone.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_DTMF, RIL_REQUEST_DTMF_START
 */
#define RIL_REQUEST_DTMF_STOP 50

/**
 * RIL_REQUEST_BASEBAND_VERSION
 *
 * Return string value indicating baseband version, eg
 * response from AT+CGMR
 *
 * "data" is NULL
 * "response" is const char * containing version string for log reporting
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  EMPTY_RECORD
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_BASEBAND_VERSION 51

/**
 * RIL_REQUEST_SEPARATE_CONNECTION
 *
 * Separate a party from a multiparty call placing the multiparty call
 * (less the specified party) on hold and leaving the specified party
 * as the only other member of the current (active) call
 *
 * Like AT+CHLD=2x
 *
 * See TS 22.084 1.3.8.2 (iii)
 * TS 22.030 6.5.5 "Entering "2X followed by send"
 * TS 27.007 "AT+CHLD=2x"
 *
 * "data" is an int *
 * (int *)data)[0] contains Connection index (value of 'x' in CHLD above) "response" is NULL
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_ARGUMENTS
 *  INVALID_STATE
 *  NO_RESOURCES
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  INVALID_STATE
 *  OPERATION_NOT_ALLOWED
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SEPARATE_CONNECTION 52


/**
 * RIL_REQUEST_SET_MUTE
 *
 * Turn on or off uplink (microphone) mute.
 *
 * Will only be sent while voice call is active.
 * Will always be reset to "disable mute" when a new voice call is initiated
 *
 * "data" is an int *
 * (int *)data)[0] is 1 for "enable mute" and 0 for "disable mute"
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_SET_MUTE 53

/**
 * RIL_REQUEST_GET_MUTE
 *
 * Queries the current state of the uplink mute setting
 *
 * "data" is NULL
 * "response" is an int *
 * (int *)response)[0] is 1 for "mute enabled" and 0 for "mute disabled"
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  SS_MODIFIED_TO_DIAL
 *  SS_MODIFIED_TO_USSD
 *  SS_MODIFIED_TO_SS
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_GET_MUTE 54

/**
 * RIL_REQUEST_QUERY_CLIP
 *
 * Queries the status of the CLIP supplementary service
 *
 * (for MMI code "*#30#")
 *
 * "data" is NULL
 * "response" is an int *
 * (int *)response)[0] is 1 for "CLIP provisioned"
 *                           and 0 for "CLIP not provisioned"
 *                           and 2 for "unknown, e.g. no network etc"
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  FDN_CHECK_FAILURE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_QUERY_CLIP 55

/**
 * RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE - Deprecated use the status
 * field in RIL_Data_Call_Response_v6.
 *
 * Requests the failure cause code for the most recently failed PDP
 * context or CDMA data connection active
 * replaces RIL_REQUEST_LAST_PDP_FAIL_CAUSE
 *
 * "data" is NULL
 *
 * "response" is a "int *"
 * ((int *)response)[0] is an integer cause code defined in TS 24.008
 *   section 6.1.3.1.3 or close approximation
 *
 * If the implementation does not have access to the exact cause codes,
 * then it should return one of the values listed in
 * RIL_DataCallFailCause, as the UI layer needs to distinguish these
 * cases for error notification
 * and potential retries.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_LAST_CALL_FAIL_CAUSE
 *
 * Deprecated use the status field in RIL_Data_Call_Response_v6.
 */

#define RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE 56

/**
 * RIL_REQUEST_DATA_CALL_LIST
 *
 * Returns the data call list. An entry is added when a
 * RIL_REQUEST_SETUP_DATA_CALL is issued and removed on a
 * RIL_REQUEST_DEACTIVATE_DATA_CALL. The list is emptied
 * when RIL_REQUEST_RADIO_POWER off/on is issued.
 *
 * "data" is NULL
 * "response" is an array of RIL_Data_Call_Response_v6
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 * See also: RIL_UNSOL_DATA_CALL_LIST_CHANGED
 */

#define RIL_REQUEST_DATA_CALL_LIST 57

/**
 * RIL_REQUEST_RESET_RADIO - DEPRECATED
 *
 * Request a radio reset. The RIL implementation may postpone
 * the reset until after this request is responded to if the baseband
 * is presently busy.
 *
 * The request is DEPRECATED, use RIL_REQUEST_RADIO_POWER
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  REQUEST_NOT_SUPPORTED
 */

#define RIL_REQUEST_RESET_RADIO 58

/**
 * RIL_REQUEST_OEM_HOOK_RAW
 *
 * This request reserved for OEM-specific uses. It passes raw byte arrays
 * back and forth.
 *
 * It can be invoked on the Java side from
 * com.android.internal.telephony.Phone.invokeOemRilRequestRaw()
 *
 * "data" is a char * of bytes copied from the byte[] data argument in java
 * "response" is a char * of bytes that will returned via the
 * caller's "response" Message here:
 * (byte[])(((AsyncResult)response.obj).result)
 *
 * An error response here will result in
 * (((AsyncResult)response.obj).result) == null and
 * (((AsyncResult)response.obj).exception) being an instance of
 * com.android.internal.telephony.gsm.CommandException
 *
 * Valid errors:
 *  All
 */

#define RIL_REQUEST_OEM_HOOK_RAW 59

/**
 * RIL_REQUEST_OEM_HOOK_STRINGS
 *
 * This request reserved for OEM-specific uses. It passes strings
 * back and forth.
 *
 * It can be invoked on the Java side from
 * com.android.internal.telephony.Phone.invokeOemRilRequestStrings()
 *
 * "data" is a const char **, representing an array of null-terminated UTF-8
 * strings copied from the "String[] strings" argument to
 * invokeOemRilRequestStrings()
 *
 * "response" is a const char **, representing an array of null-terminated UTF-8
 * stings that will be returned via the caller's response message here:
 *
 * (String[])(((AsyncResult)response.obj).result)
 *
 * An error response here will result in
 * (((AsyncResult)response.obj).result) == null and
 * (((AsyncResult)response.obj).exception) being an instance of
 * com.android.internal.telephony.gsm.CommandException
 *
 * Valid errors:
 *  All
 */

#define RIL_REQUEST_OEM_HOOK_STRINGS 60

/**
 * RIL_REQUEST_SCREEN_STATE - DEPRECATED
 *
 * Indicates the current state of the screen.  When the screen is off, the
 * RIL should notify the baseband to suppress certain notifications (eg,
 * signal strength and changes in LAC/CID or BID/SID/NID/latitude/longitude)
 * in an effort to conserve power.  These notifications should resume when the
 * screen is on.
 *
 * Note this request is deprecated. Use RIL_REQUEST_SEND_DEVICE_STATE to report the device state
 * to the modem and use RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER to turn on/off unsolicited
 * response from the modem in different scenarios.
 *
 * "data" is int *
 * ((int *)data)[0] is == 1 for "Screen On"
 * ((int *)data)[0] is == 0 for "Screen Off"
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SCREEN_STATE 61


/**
 * RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION
 *
 * Enables/disables supplementary service related notifications
 * from the network.
 *
 * Notifications are reported via RIL_UNSOL_SUPP_SVC_NOTIFICATION.
 *
 * "data" is int *
 * ((int *)data)[0] is == 1 for notifications enabled
 * ((int *)data)[0] is == 0 for notifications disabled
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SIM_BUSY
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_UNSOL_SUPP_SVC_NOTIFICATION.
 */
#define RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION 62

/**
 * RIL_REQUEST_WRITE_SMS_TO_SIM
 *
 * Stores a SMS message to SIM memory.
 *
 * "data" is RIL_SMS_WriteArgs *
 *
 * "response" is int *
 * ((const int *)response)[0] is the record index where the message is stored.
 *
 * Valid errors:
 *  SUCCESS
 *  SIM_FULL
 *  INVALID_ARGUMENTS
 *  INVALID_SMS_FORMAT
 *  INTERNAL_ERR
 *  MODEM_ERR
 *  ENCODING_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  INVALID_MODEM_STATE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_SMSC_ADDRESS
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 */
#define RIL_REQUEST_WRITE_SMS_TO_SIM 63

/**
 * RIL_REQUEST_DELETE_SMS_ON_SIM
 *
 * Deletes a SMS message from SIM memory.
 *
 * "data" is int  *
 * ((int *)data)[0] is the record index of the message to delete.
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  SIM_FULL
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NO_SUCH_ENTRY
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 */
#define RIL_REQUEST_DELETE_SMS_ON_SIM 64

/**
 * RIL_REQUEST_SET_BAND_MODE
 *
 * Assign a specified band for RF configuration.
 *
 * "data" is int *
 * ((int *)data)[0] is a RIL_RadioBandMode
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 * See also: RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE
 */
#define RIL_REQUEST_SET_BAND_MODE 65

/**
 * RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE
 *
 * Query the list of band mode supported by RF.
 *
 * "data" is NULL
 *
 * "response" is int *
 * "response" points to an array of int's, the int[0] is the size of array;
 * subsequent values are a list of RIL_RadioBandMode listing supported modes.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 * See also: RIL_REQUEST_SET_BAND_MODE
 */
#define RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE 66

/**
 * RIL_REQUEST_STK_GET_PROFILE
 *
 * Requests the profile of SIM tool kit.
 * The profile indicates the SAT/USAT features supported by ME.
 * The SAT/USAT features refer to 3GPP TS 11.14 and 3GPP TS 31.111
 *
 * "data" is NULL
 *
 * "response" is a const char * containing SAT/USAT profile
 * in hexadecimal format string starting with first byte of terminal profile
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE (radio resetting)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_STK_GET_PROFILE 67

/**
 * RIL_REQUEST_STK_SET_PROFILE
 *
 * Download the STK terminal profile as part of SIM initialization
 * procedure
 *
 * "data" is a const char * containing SAT/USAT profile
 * in hexadecimal format string starting with first byte of terminal profile
 *
 * "response" is NULL
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE (radio resetting)
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_STK_SET_PROFILE 68

/**
 * RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND
 *
 * Requests to send a SAT/USAT envelope command to SIM.
 * The SAT/USAT envelope command refers to 3GPP TS 11.14 and 3GPP TS 31.111
 *
 * "data" is a const char * containing SAT/USAT command
 * in hexadecimal format string starting with command tag
 *
 * "response" is a const char * containing SAT/USAT response
 * in hexadecimal format string starting with first byte of response
 * (May be NULL)
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE (radio resetting)
 *  SIM_BUSY
 *  OPERATION_NOT_ALLOWED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND 69

/**
 * RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE
 *
 * Requests to send a terminal response to SIM for a received
 * proactive command
 *
 * "data" is a const char * containing SAT/USAT response
 * in hexadecimal format string starting with first byte of response data
 *
 * "response" is NULL
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE (radio resetting)
 *  RIL_E_OPERATION_NOT_ALLOWED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE 70

/**
 * RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM
 *
 * When STK application gets RIL_UNSOL_STK_CALL_SETUP, the call actually has
 * been initialized by ME already. (We could see the call has been in the 'call
 * list') So, STK application needs to accept/reject the call according as user
 * operations.
 *
 * "data" is int *
 * ((int *)data)[0] is > 0 for "accept" the call setup
 * ((int *)data)[0] is == 0 for "reject" the call setup
 *
 * "response" is NULL
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE (radio resetting)
 *  RIL_E_OPERATION_NOT_ALLOWED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM 71

/**
 * RIL_REQUEST_EXPLICIT_CALL_TRANSFER
 *
 * Connects the two calls and disconnects the subscriber from both calls.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  INVALID_STATE
 *  NO_RESOURCES
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  INVALID_STATE
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_EXPLICIT_CALL_TRANSFER 72

/**
 * RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
 *
 * Requests to set the preferred network type for searching and registering
 * (CS/PS domain, RAT, and operation mode)
 *
 * "data" is int * which is RIL_PreferredNetworkType
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  OPERATION_NOT_ALLOWED
 *  MODE_NOT_SUPPORTED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE 73

/**
 * RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE
 *
 * Query the preferred network type (CS/PS domain, RAT, and operation mode)
 * for searching and registering
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)reponse)[0] is == RIL_PreferredNetworkType
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 * See also: RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
 */
#define RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE 74

/**
 * RIL_REQUEST_NEIGHBORING_CELL_IDS
 *
 * Request neighboring cell id in GSM network
 *
 * "data" is NULL
 * "response" must be a " const RIL_NeighboringCell** "
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NO_NETWORK_FOUND
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_GET_NEIGHBORING_CELL_IDS 75

/**
 * RIL_REQUEST_SET_LOCATION_UPDATES
 *
 * Enables/disables network state change notifications due to changes in
 * LAC and/or CID (for GSM) or BID/SID/NID/latitude/longitude (for CDMA).
 * Basically +CREG=2 vs. +CREG=1 (TS 27.007).
 *
 * Note:  The RIL implementation should default to "updates enabled"
 * when the screen is on and "updates disabled" when the screen is off.
 *
 * "data" is int *
 * ((int *)data)[0] is == 1 for updates enabled (+CREG=2)
 * ((int *)data)[0] is == 0 for updates disabled (+CREG=1)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 * See also: RIL_REQUEST_SCREEN_STATE, RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED
 */
#define RIL_REQUEST_SET_LOCATION_UPDATES 76

/**
 * RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE
 *
 * Request to set the location where the CDMA subscription shall
 * be retrieved
 *
 * "data" is int *
 * ((int *)data)[0] is == RIL_CdmaSubscriptionSource
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SIM_ABSENT
 *  SUBSCRIPTION_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE
 */
#define RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE 77

/**
 * RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE
 *
 * Request to set the roaming preferences in CDMA
 *
 * "data" is int *
 * ((int *)data)[0] is == 0 for Home Networks only, as defined in PRL
 * ((int *)data)[0] is == 1 for Roaming on Affiliated networks, as defined in PRL
 * ((int *)data)[0] is == 2 for Roaming on Any Network, as defined in the PRL
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE 78

/**
 * RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE
 *
 * Request the actual setting of the roaming preferences in CDMA in the modem
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)response)[0] is == 0 for Home Networks only, as defined in PRL
 * ((int *)response)[0] is == 1 for Roaming on Affiliated networks, as defined in PRL
 * ((int *)response)[0] is == 2 for Roaming on Any Network, as defined in the PRL
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE 79

/**
 * RIL_REQUEST_SET_TTY_MODE
 *
 * Request to set the TTY mode
 *
 * "data" is int *
 * ((int *)data)[0] is == 0 for TTY off
 * ((int *)data)[0] is == 1 for TTY Full
 * ((int *)data)[0] is == 2 for TTY HCO (hearing carryover)
 * ((int *)data)[0] is == 3 for TTY VCO (voice carryover)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SET_TTY_MODE 80

/**
 * RIL_REQUEST_QUERY_TTY_MODE
 *
 * Request the setting of TTY mode
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)response)[0] is == 0 for TTY off
 * ((int *)response)[0] is == 1 for TTY Full
 * ((int *)response)[0] is == 2 for TTY HCO (hearing carryover)
 * ((int *)response)[0] is == 3 for TTY VCO (voice carryover)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_QUERY_TTY_MODE 81

/**
 * RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE
 *
 * Request to set the preferred voice privacy mode used in voice
 * scrambling
 *
 * "data" is int *
 * ((int *)data)[0] is == 0 for Standard Privacy Mode (Public Long Code Mask)
 * ((int *)data)[0] is == 1 for Enhanced Privacy Mode (Private Long Code Mask)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_CALL_ID
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE 82

/**
 * RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE
 *
 * Request the setting of preferred voice privacy mode
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)response)[0] is == 0 for Standard Privacy Mode (Public Long Code Mask)
 * ((int *)response)[0] is == 1 for Enhanced Privacy Mode (Private Long Code Mask)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  INVALID_ARGUMENTS
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE 83

/**
 * RIL_REQUEST_CDMA_FLASH
 *
 * Send FLASH
 *
 * "data" is const char *
 * ((const char *)data)[0] is a FLASH string
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  INVALID_STATE
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_CDMA_FLASH 84

/**
 * RIL_REQUEST_CDMA_BURST_DTMF
 *
 * Send DTMF string
 *
 * "data" is const char **
 * ((const char **)data)[0] is a DTMF string
 * ((const char **)data)[1] is the DTMF ON length in milliseconds, or 0 to use
 *                          default
 * ((const char **)data)[2] is the DTMF OFF length in milliseconds, or 0 to use
 *                          default
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  INVALID_CALL_ID
 *  NO_RESOURCES
 *  CANCELLED
 *  OPERATION_NOT_ALLOWED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_CDMA_BURST_DTMF 85

/**
 * RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY - DEPRECATED
 *
 * Takes a 26 digit string (20 digit AKEY + 6 digit checksum).
 * If the checksum is valid the 20 digit AKEY is written to NV,
 * replacing the existing AKEY no matter what it was before.
 *
 * "data" is const char *
 * ((const char *)data)[0] is a 26 digit string (ASCII digits '0'-'9')
 *                         where the last 6 digits are a checksum of the
 *                         first 20, as specified in TR45.AHAG
 *                         "Common Cryptographic Algorithms, Revision D.1
 *                         Section 2.2"
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY 86

/**
 * RIL_REQUEST_CDMA_SEND_SMS
 *
 * Send a CDMA SMS message
 *
 * "data" is const RIL_CDMA_SMS_Message *
 *
 * "response" is a const RIL_SMS_Response *
 *
 * Based on the return error, caller decides to resend if sending sms
 * fails. The CDMA error class is derived as follows,
 * SUCCESS is error class 0 (no error)
 * SMS_SEND_FAIL_RETRY is error class 2 (temporary failure)
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SMS_SEND_FAIL_RETRY
 *  NETWORK_REJECT
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  INVALID_SMS_FORMAT
 *  SYSTEM_ERR
 *  FDN_CHECK_FAILURE
 *  MODEM_ERR
 *  NETWORK_ERR
 *  ENCODING_ERR
 *  INVALID_SMSC_ADDRESS
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  MODE_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 */
#define RIL_REQUEST_CDMA_SEND_SMS 87

/**
 * RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE
 *
 * Acknowledge the success or failure in the receipt of SMS
 * previously indicated via RIL_UNSOL_RESPONSE_CDMA_NEW_SMS
 *
 * "data" is const RIL_CDMA_SMS_Ack *
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_SMS_TO_ACK
 *  INVALID_STATE
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INVALID_STATE
 *  OPERATION_NOT_ALLOWED
 *  NETWORK_NOT_READY
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE 88

/**
 * RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG
 *
 * Request the setting of GSM/WCDMA Cell Broadcast SMS config.
 *
 * "data" is NULL
 *
 * "response" is a const RIL_GSM_BroadcastSmsConfigInfo **
 * "responselen" is count * sizeof (RIL_GSM_BroadcastSmsConfigInfo *)
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_STATE
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  SYSTEM_ERR
 *  NO_RESOURCES
 *  MODEM_ERR
 *  SYSTEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG 89

/**
 * RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG
 *
 * Set GSM/WCDMA Cell Broadcast SMS config
 *
 * "data" is a const RIL_GSM_BroadcastSmsConfigInfo **
 * "datalen" is count * sizeof(RIL_GSM_BroadcastSmsConfigInfo *)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
 *  MODEM_ERR
 *  SYSTEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG 90

/**
 * RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION
 *
* Enable or disable the reception of GSM/WCDMA Cell Broadcast SMS
 *
 * "data" is const int *
 * (const int *)data[0] indicates to activate or turn off the
 * reception of GSM/WCDMA Cell Broadcast SMS, 0-1,
 *                       0 - Activate, 1 - Turn off
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
*   MODEM_ERR
*   INTERNAL_ERR
*   NO_RESOURCES
*   CANCELLED
*   INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION 91

/**
 * RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG
 *
 * Request the setting of CDMA Broadcast SMS config
 *
 * "data" is NULL
 *
 * "response" is a const RIL_CDMA_BroadcastSmsConfigInfo **
 * "responselen" is count * sizeof (RIL_CDMA_BroadcastSmsConfigInfo *)
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_STATE
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  SYSTEM_ERR
 *  NO_RESOURCES
 *  MODEM_ERR
 *  SYSTEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG 92

/**
 * RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG
 *
 * Set CDMA Broadcast SMS config
 *
 * "data" is a const RIL_CDMA_BroadcastSmsConfigInfo **
 * "datalen" is count * sizeof(const RIL_CDMA_BroadcastSmsConfigInfo *)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
 *  MODEM_ERR
 *  SYSTEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG 93

/**
 * RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION
 *
 * Enable or disable the reception of CDMA Broadcast SMS
 *
 * "data" is const int *
 * (const int *)data[0] indicates to activate or turn off the
 * reception of CDMA Broadcast SMS, 0-1,
 *                       0 - Activate, 1 - Turn off
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION 94

/**
 * RIL_REQUEST_CDMA_SUBSCRIPTION
 *
 * Request the device MDN / H_SID / H_NID.
 *
 * The request is only allowed when CDMA subscription is available.  When CDMA
 * subscription is changed, application layer should re-issue the request to
 * update the subscription information.
 *
 * If a NULL value is returned for any of the device id, it means that error
 * accessing the device.
 *
 * "response" is const char **
 * ((const char **)response)[0] is MDN if CDMA subscription is available
 * ((const char **)response)[1] is a comma separated list of H_SID (Home SID) if
 *                              CDMA subscription is available, in decimal format
 * ((const char **)response)[2] is a comma separated list of H_NID (Home NID) if
 *                              CDMA subscription is available, in decimal format
 * ((const char **)response)[3] is MIN (10 digits, MIN2+MIN1) if CDMA subscription is available
 * ((const char **)response)[4] is PRL version if CDMA subscription is available
 *
 * Valid errors:
 *  SUCCESS
 *  RIL_E_SUBSCRIPTION_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *
 */

#define RIL_REQUEST_CDMA_SUBSCRIPTION 95

/**
 * RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM
 *
 * Stores a CDMA SMS message to RUIM memory.
 *
 * "data" is RIL_CDMA_SMS_WriteArgs *
 *
 * "response" is int *
 * ((const int *)response)[0] is the record index where the message is stored.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SIM_FULL
 *  INVALID_ARGUMENTS
 *  INVALID_SMS_FORMAT
 *  INTERNAL_ERR
 *  MODEM_ERR
 *  ENCODING_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  INVALID_MODEM_STATE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_SMSC_ADDRESS
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 */
#define RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM 96

/**
 * RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM
 *
 * Deletes a CDMA SMS message from RUIM memory.
 *
 * "data" is int  *
 * ((int *)data)[0] is the record index of the message to delete.
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NO_SUCH_ENTRY
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 */
#define RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM 97

/**
 * RIL_REQUEST_DEVICE_IDENTITY
 *
 * Request the device ESN / MEID / IMEI / IMEISV.
 *
 * The request is always allowed and contains GSM and CDMA device identity;
 * it substitutes the deprecated requests RIL_REQUEST_GET_IMEI and
 * RIL_REQUEST_GET_IMEISV.
 *
 * If a NULL value is returned for any of the device id, it means that error
 * accessing the device.
 *
 * When CDMA subscription is changed the ESN/MEID may change.  The application
 * layer should re-issue the request to update the device identity in this case.
 *
 * "response" is const char **
 * ((const char **)response)[0] is IMEI if GSM subscription is available
 * ((const char **)response)[1] is IMEISV if GSM subscription is available
 * ((const char **)response)[2] is ESN if CDMA subscription is available
 * ((const char **)response)[3] is MEID if CDMA subscription is available
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_DEVICE_IDENTITY 98

/**
 * RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE
 *
 * Request the radio's system selection module to exit emergency
 * callback mode.  RIL will not respond with SUCCESS until the modem has
 * completely exited from Emergency Callback Mode.
 *
 * "data" is NULL
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE 99

/**
 * RIL_REQUEST_GET_SMSC_ADDRESS
 *
 * Queries the default Short Message Service Center address on the device.
 *
 * "data" is NULL
 *
 * "response" is const char * containing the SMSC address.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  SYSTEM_ERR
 *  INTERNAL_ERR
 *  MODEM_ERR
 *  INVALID_ARGUMENTS
 *  INVALID_MODEM_STATE
 *  NOT_PROVISIONED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 *
 */
#define RIL_REQUEST_GET_SMSC_ADDRESS 100

/**
 * RIL_REQUEST_SET_SMSC_ADDRESS
 *
 * Sets the default Short Message Service Center address on the device.
 *
 * "data" is const char * containing the SMSC address.
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  INVALID_SMS_FORMAT
 *  NO_MEMORY
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
 *  MODEM_ERR
 *  NO_RESOURCES
 *  INTERNAL_ERR
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 */
#define RIL_REQUEST_SET_SMSC_ADDRESS 101

/**
 * RIL_REQUEST_REPORT_SMS_MEMORY_STATUS
 *
 * Indicates whether there is storage available for new SMS messages.
 *
 * "data" is int *
 * ((int *)data)[0] is 1 if memory is available for storing new messages
 *                  is 0 if memory capacity is exceeded
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  INVALID_STATE
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_REPORT_SMS_MEMORY_STATUS 102

/**
 * RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING
 *
 * Indicates that the StkSerivce is running and is
 * ready to receive RIL_UNSOL_STK_XXXXX commands.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING 103

/**
 * RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE
 *
 * Request to query the location where the CDMA subscription shall
 * be retrieved
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)data)[0] is == RIL_CdmaSubscriptionSource
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SUBSCRIPTION_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 * See also: RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE
 */
#define RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE 104

/**
 * RIL_REQUEST_ISIM_AUTHENTICATION
 *
 * Request the ISIM application on the UICC to perform AKA
 * challenge/response algorithm for IMS authentication
 *
 * "data" is a const char * containing the challenge string in Base64 format
 * "response" is a const char * containing the response in Base64 format
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_ISIM_AUTHENTICATION 105

/**
 * RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU
 *
 * Acknowledge successful or failed receipt of SMS previously indicated
 * via RIL_UNSOL_RESPONSE_NEW_SMS, including acknowledgement TPDU to send
 * as the RP-User-Data element of the RP-ACK or RP-ERROR PDU.
 *
 * "data" is const char **
 * ((const char **)data)[0] is "1" on successful receipt (send RP-ACK)
 *                          is "0" on failed receipt (send RP-ERROR)
 * ((const char **)data)[1] is the acknowledgement TPDU in hexadecimal format
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU 106

/**
 * RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS
 *
 * Requests to send a SAT/USAT envelope command to SIM.
 * The SAT/USAT envelope command refers to 3GPP TS 11.14 and 3GPP TS 31.111.
 *
 * This request has one difference from RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
 * the SW1 and SW2 status bytes from the UICC response are returned along with
 * the response data, using the same structure as RIL_REQUEST_SIM_IO.
 *
 * The RIL implementation shall perform the normal processing of a '91XX'
 * response in SW1/SW2 to retrieve the pending proactive command and send it
 * as an unsolicited response, as RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND does.
 *
 * "data" is a const char * containing the SAT/USAT command
 * in hexadecimal format starting with command tag
 *
 * "response" is a const RIL_SIM_IO_Response *
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE (radio resetting)
 *  SIM_BUSY
 *  OPERATION_NOT_ALLOWED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 */
#define RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS 107

/**
 * RIL_REQUEST_VOICE_RADIO_TECH
 *
 * Query the radio technology type (3GPP/3GPP2) used for voice. Query is valid only
 * when radio state is not RADIO_STATE_UNAVAILABLE
 *
 * "data" is NULL
 * "response" is int *
 * ((int *) response)[0] is of type const RIL_RadioTechnology
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_VOICE_RADIO_TECH 108

/**
 * RIL_REQUEST_GET_CELL_INFO_LIST
 *
 * Request all of the current cell information known to the radio. The radio
 * must a list of all current cells, including the neighboring cells. If for a particular
 * cell information isn't known then the appropriate unknown value will be returned.
 * This does not cause or change the rate of RIL_UNSOL_CELL_INFO_LIST.
 *
 * "data" is NULL
 *
 * "response" is an array of  RIL_CellInfo_v12.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  NO_NETWORK_FOUND
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_GET_CELL_INFO_LIST 109

/**
 * RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE
 *
 * Sets the minimum time between when RIL_UNSOL_CELL_INFO_LIST should be invoked.
 * A value of 0, means invoke RIL_UNSOL_CELL_INFO_LIST when any of the reported
 * information changes. Setting the value to INT_MAX(0x7fffffff) means never issue
 * a RIL_UNSOL_CELL_INFO_LIST.
 *
 * "data" is int *
 * ((int *)data)[0] is minimum time in milliseconds
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE 110

/**
 * RIL_REQUEST_SET_INITIAL_ATTACH_APN
 *
 * Set an apn to initial attach network
 *
 * "data" is a const RIL_DataProfileInfo_IRadio_1_4 *
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  SUBSCRIPTION_NOT_AVAILABLE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  NOT_PROVISIONED
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_SET_INITIAL_ATTACH_APN 111

/**
 * RIL_REQUEST_IMS_REGISTRATION_STATE
 *
 * This message is DEPRECATED and shall be removed in a future release (target: 2018);
 * instead, provide IMS registration status via an IMS Service.
 *
 * Request current IMS registration state
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)response)[0] is registration state:
 *              0 - Not registered
 *              1 - Registered
 *
 * If ((int*)response)[0] is = 1, then ((int *) response)[1]
 * must follow with IMS SMS format:
 *
 * ((int *) response)[1] is of type RIL_RadioTechnologyFamily
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_IMS_REGISTRATION_STATE 112

/**
 * RIL_REQUEST_IMS_SEND_SMS
 *
 * Send a SMS message over IMS
 *
 * "data" is const RIL_IMS_SMS_Message *
 *
 * "response" is a const RIL_SMS_Response *
 *
 * Based on the return error, caller decides to resend if sending sms
 * fails. SMS_SEND_FAIL_RETRY means retry, and other errors means no retry.
 * In case of retry, data is encoded based on Voice Technology available.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SMS_SEND_FAIL_RETRY
 *  FDN_CHECK_FAILURE
 *  NETWORK_REJECT
 *  INVALID_ARGUMENTS
 *  INVALID_STATE
 *  NO_MEMORY
 *  INVALID_SMS_FORMAT
 *  SYSTEM_ERR
 *  REQUEST_RATE_LIMITED
 *  MODEM_ERR
 *  NETWORK_ERR
 *  ENCODING_ERR
 *  INVALID_SMSC_ADDRESS
 *  OPERATION_NOT_ALLOWED
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_IMS_SEND_SMS 113

/**
 * RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC
 *
 * Request APDU exchange on the basic channel. This command reflects TS 27.007
 * "generic SIM access" operation (+CSIM). The modem must ensure proper function
 * of GSM/CDMA, and filter commands appropriately. It should filter
 * channel management and SELECT by DF name commands.
 *
 * "data" is a const RIL_SIM_APDU *
 * "sessionid" field should be ignored.
 *
 * "response" is a const RIL_SIM_IO_Response *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC 114

/**
 * RIL_REQUEST_SIM_OPEN_CHANNEL
 *
 * Open a new logical channel and select the given application. This command
 * reflects TS 27.007 "open logical channel" operation (+CCHO). This request
 * also specifies the P2 parameter (described in ISO 7816-4).
 *
 * "data" is a const RIL_OpenChannelParam *
 *
 * "response" is int *
 * ((int *)data)[0] contains the session id of the logical channel.
 * ((int *)data)[1] onwards may optionally contain the select response for the
 *     open channel command with one byte per integer.
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  MISSING_RESOURCE
 *  NO_SUCH_ELEMENT
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  SIM_ERR
 *  INVALID_SIM_STATE
 *  MISSING_RESOURCE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SIM_OPEN_CHANNEL 115

/**
 * RIL_REQUEST_SIM_CLOSE_CHANNEL
 *
 * Close a previously opened logical channel. This command reflects TS 27.007
 * "close logical channel" operation (+CCHC).
 *
 * "data" is int *
 * ((int *)data)[0] is the session id of logical the channel to close.
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SIM_CLOSE_CHANNEL 116

/**
 * RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL
 *
 * Exchange APDUs with a UICC over a previously opened logical channel. This
 * command reflects TS 27.007 "generic logical channel access" operation
 * (+CGLA). The modem should filter channel management and SELECT by DF name
 * commands.
 *
 * "data" is a const RIL_SIM_APDU*
 *
 * "response" is a const RIL_SIM_IO_Response *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL 117

/**
 * RIL_REQUEST_NV_READ_ITEM
 *
 * Read one of the radio NV items defined in RadioNVItems.java / ril_nv_items.h.
 * This is used for device configuration by some CDMA operators.
 *
 * "data" is a const RIL_NV_ReadItem *
 *
 * "response" is const char * containing the contents of the NV item
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_NV_READ_ITEM 118

/**
 * RIL_REQUEST_NV_WRITE_ITEM
 *
 * Write one of the radio NV items defined in RadioNVItems.java / ril_nv_items.h.
 * This is used for device configuration by some CDMA operators.
 *
 * "data" is a const RIL_NV_WriteItem *
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_NV_WRITE_ITEM 119

/**
 * RIL_REQUEST_NV_WRITE_CDMA_PRL
 *
 * Update the CDMA Preferred Roaming List (PRL) in the radio NV storage.
 * This is used for device configuration by some CDMA operators.
 *
 * "data" is a const char * containing the PRL as a byte array
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_NV_WRITE_CDMA_PRL 120

/**
 * RIL_REQUEST_NV_RESET_CONFIG
 *
 * Reset the radio NV configuration to the factory state.
 * This is used for device configuration by some CDMA operators.
 *
 * "data" is int *
 * ((int *)data)[0] is 1 to reload all NV items
 * ((int *)data)[0] is 2 for erase NV reset (SCRTN)
 * ((int *)data)[0] is 3 for factory reset (RTN)
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_NV_RESET_CONFIG 121

 /** RIL_REQUEST_SET_UICC_SUBSCRIPTION
 * FIXME This API needs to have more documentation.
 *
 * Selection/de-selection of a subscription from a SIM card
 * "data" is const  RIL_SelectUiccSub*

 *
 * "response" is NULL
 *
 *  Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  SUBSCRIPTION_NOT_SUPPORTED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_SET_UICC_SUBSCRIPTION  122

/**
 *  RIL_REQUEST_ALLOW_DATA - DEPRECATED
 *
 *  Tells the modem whether data calls are allowed or not
 *  The request is DEPRECATED, use RIL_REQUEST_SET_PREFERRED_DATA_MODEM
 *
 * "data" is int *
 * FIXME slotId and aid will be added.
 * ((int *)data)[0] is == 0 to allow data calls
 * ((int *)data)[0] is == 1 to disallow data calls
 *
 * "response" is NULL
 *
 *  Valid errors:
 *
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  MODEM_ERR
 *  INVALID_ARGUMENTS
 *  DEVICE_IN_USE
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_ALLOW_DATA  123

/**
 * RIL_REQUEST_GET_HARDWARE_CONFIG
 *
 * Request all of the current hardware (modem and sim) associated
 * with the RIL.
 *
 * "data" is NULL
 *
 * "response" is an array of  RIL_HardwareConfig.
 *
 * Valid errors:
 * RADIO_NOT_AVAILABLE
 * REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_GET_HARDWARE_CONFIG 124

/**
 * RIL_REQUEST_SIM_AUTHENTICATION
 *
 * Returns the response of SIM Authentication through RIL to a
 * challenge request.
 *
 * "data" Base64 encoded string containing challenge:
 *      int   authContext;          P2 value of authentication command, see P2 parameter in
 *                                  3GPP TS 31.102 7.1.2
 *      char *authData;             the challenge string in Base64 format, see 3GPP
 *                                  TS 31.102 7.1.2
 *      char *aid;                  AID value, See ETSI 102.221 8.1 and 101.220 4,
 *                                  NULL if no value
 *
 * "response" Base64 encoded strings containing response:
 *      int   sw1;                  Status bytes per 3GPP TS 31.102 section 7.3
 *      int   sw2;
 *      char *simResponse;          Response in Base64 format, see 3GPP TS 31.102 7.1.2
 *
 *  Valid errors:
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  INVALID_MODEM_STATE
 *  INVALID_ARGUMENTS
 *  SIM_ERR
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_SIM_AUTHENTICATION 125

/**
 * RIL_REQUEST_GET_DC_RT_INFO - DEPRECATED
 *
 * The request is DEPRECATED, use RIL_REQUEST_GET_ACTIVITY_INFO
 * Requests the Data Connection Real Time Info
 *
 * "data" is NULL
 *
 * "response" is the most recent RIL_DcRtInfo
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  REQUEST_NOT_SUPPORTED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *
 * See also: RIL_UNSOL_DC_RT_INFO_CHANGED
 */
#define RIL_REQUEST_GET_DC_RT_INFO 126

/**
 * RIL_REQUEST_SET_DC_RT_INFO_RATE - DEPRECATED
 *
 * The request is DEPRECATED
 * This is the minimum number of milliseconds between successive
 * RIL_UNSOL_DC_RT_INFO_CHANGED messages and defines the highest rate
 * at which RIL_UNSOL_DC_RT_INFO_CHANGED's will be sent. A value of
 * 0 means send as fast as possible.
 *
 * "data" The number of milliseconds as an int
 *
 * "response" is null
 *
 * Valid errors:
 *  SUCCESS must not fail
 */
#define RIL_REQUEST_SET_DC_RT_INFO_RATE 127

/**
 * RIL_REQUEST_SET_DATA_PROFILE
 *
 * Set data profile in modem
 * Modem should erase existed profiles from framework, and apply new profiles
 * "data" is a const RIL_DataProfileInfo_IRadio_1_4 **
 * "datalen" is count * sizeof(const RIL_DataProfileInfo_IRadio_1_4 *)
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  SUBSCRIPTION_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 */
#define RIL_REQUEST_SET_DATA_PROFILE 128

/**
 * RIL_REQUEST_SHUTDOWN
 *
 * Device is shutting down. All further commands are ignored
 * and RADIO_NOT_AVAILABLE must be returned.
 *
 * "data" is null
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SHUTDOWN 129

/**
 * RIL_REQUEST_GET_RADIO_CAPABILITY
 *
 * Used to get phone radio capablility.
 *
 * "data" is the RIL_RadioCapability structure
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_STATE
 *  REQUEST_NOT_SUPPORTED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_GET_RADIO_CAPABILITY 130

/**
 * RIL_REQUEST_SET_RADIO_CAPABILITY
 *
 * Used to set the phones radio capability. Be VERY careful
 * using this request as it may cause some vendor modems to reset. Because
 * of the possible modem reset any RIL commands after this one may not be
 * processed.
 *
 * "data" is the RIL_RadioCapability structure
 *
 * "response" is the RIL_RadioCapability structure, used to feedback return status
 *
 * Valid errors:
 *  SUCCESS means a RIL_UNSOL_RADIO_CAPABILITY will be sent within 30 seconds.
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  INVALID_STATE
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SET_RADIO_CAPABILITY 131

/**
 * RIL_REQUEST_START_LCE
 *
 * Start Link Capacity Estimate (LCE) service if supported by the radio.
 *
 * "data" is const int *
 * ((const int*)data)[0] specifies the desired reporting interval (ms).
 * ((const int*)data)[1] specifies the LCE service mode. 1: PULL; 0: PUSH.
 *
 * "response" is the RIL_LceStatusInfo.
 *
 * Valid errors:
 * SUCCESS
 * RADIO_NOT_AVAILABLE
 * LCE_NOT_SUPPORTED
 * INTERNAL_ERR
 * REQUEST_NOT_SUPPORTED
 * NO_MEMORY
 * NO_RESOURCES
 * CANCELLED
 * SIM_ABSENT
 */
#define RIL_REQUEST_START_LCE 132

/**
 * RIL_REQUEST_STOP_LCE
 *
 * Stop Link Capacity Estimate (LCE) service, the STOP operation should be
 * idempotent for the radio modem.
 *
 * "response" is the RIL_LceStatusInfo.
 *
 * Valid errors:
 * SUCCESS
 * RADIO_NOT_AVAILABLE
 * LCE_NOT_SUPPORTED
 * INTERNAL_ERR
 * NO_MEMORY
 * NO_RESOURCES
 * CANCELLED
 * REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 */
#define RIL_REQUEST_STOP_LCE 133

/**
 * RIL_REQUEST_PULL_LCEDATA
 *
 * Pull LCE service for capacity information.
 *
 * "response" is the RIL_LceDataInfo.
 *
 * Valid errors:
 * SUCCESS
 * RADIO_NOT_AVAILABLE
 * LCE_NOT_SUPPORTED
 * INTERNAL_ERR
 * NO_MEMORY
 * NO_RESOURCES
 * CANCELLED
 * REQUEST_NOT_SUPPORTED
 *  SIM_ABSENT
 */
#define RIL_REQUEST_PULL_LCEDATA 134

/**
 * RIL_REQUEST_GET_ACTIVITY_INFO
 *
 * Get modem activity information for power consumption estimation.
 *
 * Request clear-on-read statistics information that is used for
 * estimating the per-millisecond power consumption of the cellular
 * modem.
 *
 * "data" is null
 * "response" is const RIL_ActivityStatsInfo *
 *
 * Valid errors:
 *
 * SUCCESS
 * RADIO_NOT_AVAILABLE (radio resetting)
 * NO_MEMORY
 * INTERNAL_ERR
 * SYSTEM_ERR
 * MODEM_ERR
 * NOT_PROVISIONED
 * REQUEST_NOT_SUPPORTED
 * NO_RESOURCES CANCELLED
 */
#define RIL_REQUEST_GET_ACTIVITY_INFO 135

/**
 * RIL_REQUEST_SET_CARRIER_RESTRICTIONS
 *
 * Set carrier restrictions for this sim slot. Expected modem behavior:
 *  If never receives this command
 *  - Must allow all carriers
 *  Receives this command with data being NULL
 *  - Must allow all carriers. If a previously allowed SIM is present, modem must not reload
 *    the SIM. If a previously disallowed SIM is present, reload the SIM and notify Android.
 *  Receives this command with a list of carriers
 *  - Only allow specified carriers, persist across power cycles and FDR. If a present SIM
 *    is in the allowed list, modem must not reload the SIM. If a present SIM is *not* in
 *    the allowed list, modem must detach from the registered network and only keep emergency
 *    service, and notify Android SIM refresh reset with new SIM state being
 *    RIL_CARDSTATE_RESTRICTED. Emergency service must be enabled.
 *
 * "data" is const RIL_CarrierRestrictionsWithPriority *
 * A list of allowed carriers and possibly a list of excluded carriers.
 * If data is NULL, means to clear previous carrier restrictions and allow all carriers
 *
 * "response" is int *
 * ((int *)data)[0] contains the number of allowed carriers which have been set correctly.
 * On success, it should match the length of list data->allowed_carriers.
 * If data is NULL, the value must be 0.
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_INVALID_ARGUMENTS
 *  RIL_E_RADIO_NOT_AVAILABLE
 *  RIL_E_REQUEST_NOT_SUPPORTED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SET_CARRIER_RESTRICTIONS 136

/**
 * RIL_REQUEST_GET_CARRIER_RESTRICTIONS
 *
 * Get carrier restrictions for this sim slot. Expected modem behavior:
 *  Return list of allowed carriers, or null if all carriers are allowed.
 *
 * "data" is NULL
 *
 * "response" is const RIL_CarrierRestrictionsWithPriority *.
 * If response is NULL, it means all carriers are allowed.
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE
 *  RIL_E_REQUEST_NOT_SUPPORTED
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_GET_CARRIER_RESTRICTIONS 137

/**
 * RIL_REQUEST_SEND_DEVICE_STATE
 *
 * Send the updated device state.
 * Modem can perform power saving based on the provided device state.
 * "data" is const int *
 * ((const int*)data)[0] A RIL_DeviceStateType that specifies the device state type.
 * ((const int*)data)[1] Specifies the state. See RIL_DeviceStateType for the definition of each
 *                       type.
 *
 * "datalen" is count * sizeof(const RIL_DeviceState *)
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SEND_DEVICE_STATE 138

/**
 * RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER
 *
 * Set the unsolicited response filter
 * This is used to prevent unnecessary application processor
 * wake up for power saving purposes by suppressing the
 * unsolicited responses in certain scenarios.
 *
 * "data" is an int *
 *
 * ((int *)data)[0] is a 32-bit bitmask of RIL_UnsolicitedResponseFilter
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  INVALID_ARGUMENTS (e.g. the requested filter doesn't exist)
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER 139

 /**
  * RIL_REQUEST_SET_SIM_CARD_POWER
  *
  * Set SIM card power up or down
  *
  * Request is equivalent to inserting and removing the card, with
  * an additional effect where the ability to detect card removal/insertion
  * is disabled when the SIM card is powered down.
  *
  * This will generate RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED
  * as if the SIM had been inserted or removed.
  *
  * "data" is a const RIL_CardPowerState *
  *
  * "response" is NULL
  *
  * Valid errors:
  *  SUCCESS
  *  RADIO_NOT_AVAILABLE
  *  REQUEST_NOT_SUPPORTED
  *  SIM_ABSENT
  *  INVALID_ARGUMENTS
  *  INTERNAL_ERR
  *  NO_MEMORY
  *  NO_RESOURCES
  *  CANCELLED
  */
#define RIL_REQUEST_SET_SIM_CARD_POWER 140

/**
 * RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION
 *
 * Provide Carrier specific information to the modem that will be used to
 * encrypt the IMSI and IMPI. Sent by the framework during boot, carrier
 * switch and everytime we receive a new certificate.
 *
 * "data" is the RIL_CarrierInfoForImsiEncryption * structure.
 *
 * "response" is NULL
 *
 * Valid errors:
 *  RIL_E_SUCCESS
 *  RIL_E_RADIO_NOT_AVAILABLE
 *  SIM_ABSENT
 *  RIL_E_REQUEST_NOT_SUPPORTED
 *  INVALID_ARGUMENTS
 *  MODEM_INTERNAL_FAILURE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION 141

/**
 * RIL_REQUEST_START_NETWORK_SCAN
 *
 * Starts a new network scan
 *
 * Request to start a network scan with specified radio access networks with frequency bands and/or
 * channels.
 *
 * "data" is a const RIL_NetworkScanRequest *.
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  DEVICE_IN_USE
 *  INTERNAL_ERR
 *  NO_MEMORY
 *  MODEM_ERR
 *  INVALID_ARGUMENTS
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 *
 */
#define RIL_REQUEST_START_NETWORK_SCAN 142

/**
 * RIL_REQUEST_STOP_NETWORK_SCAN
 *
 * Stops an ongoing network scan
 *
 * Request to stop the ongoing network scan. Since the modem can only perform one scan at a time,
 * there is no parameter for this request.
 *
 * "data" is NULL
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  INTERNAL_ERR
 *  MODEM_ERR
 *  NO_MEMORY
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_STOP_NETWORK_SCAN 143

/**
 * RIL_REQUEST_START_KEEPALIVE
 *
 * Start a keepalive session
 *
 * Request that the modem begin sending keepalive packets on a particular
 * data call, with a specified source, destination, and format.
 *
 * "data" is a const struct RIL_KeepaliveRequest
 * "response" is RIL_KeepaliveStatus_t with a valid "handle" and "response"
 *
 * Valid errors:
 *  SUCCESS
 *  NO_RESOURCES
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_START_KEEPALIVE 144

/**
 * RIL_REQUEST_STOP_KEEPALIVE
 *
 * Stops an ongoing keepalive session
 *
 * Requests that a keepalive session with the given handle be stopped.
 * there is no parameter for this request.
 *
 * "data" is an integer handle
 * "response" is RIL_ResponseError
 *
 * Valid errors:
 *  SUCCESS
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_STOP_KEEPALIVE 145

/**
 * RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA
 *
 * Sets the signal strength reporting criteria.
 *
 * The resulting reporting criteria are the AND of all the supplied criteria.
 *
 * Note: Reporting criteria must be individually set for each RAN. If unset, reporting criteria
 * for that RAN are implementation-defined.
 *
 * "data"
 * "response" is NULL
 *
 * Valid errors:
 *   RadioError:NONE
 *   RadioError:INVALID_ARGUMENTS
 *   RadioError:RADIO_NOT_AVAILABLE
 *   RadioError:INTERNAL_ERR
 *
 */
#define RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA 146

/**
 * RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA
 *
 * Sets the link capacity reporting criteria.
 *
 * The resulting reporting criteria are the AND of all the supplied criteria.
 *
 * "data"
 * "response" is NULL
 *
 * Valid errors:
 *   RadioError:NONE
 *   RadioError:INVALID_ARGUMENTS
 *   RadioError:RADIO_NOT_AVAILABLE
 *   RadioError:INTERNAL_ERR
 *
 */
#define RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA 147

/**
 * RIL_REQUEST_ENABLE_5G
 *
 * Enable 5G in addition to other RATs
 *
 * "data" is int * to enum RIL_5G_MODE
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  OPERATION_NOT_ALLOWED
 *  MODE_NOT_SUPPORTED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_ENABLE_5G 148

/**
 * RIL_REQUEST_ENABLE_5G_ONLY
 *
 * Enable 5G only without any other RATs
 *
 * "data" is int * to enum RIL_5G_MODE
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  OPERATION_NOT_ALLOWED
 *  MODE_NOT_SUPPORTED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_ENABLE_5G_ONLY 149

/**
 * RIL_REQUEST_DISABLE_5G
 *
 * Disable 5G
 *
 * "data" is int * to enum RIL_5G_MODE
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  OPERATION_NOT_ALLOWED
 *  MODE_NOT_SUPPORTED
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_DISABLE_5G 150

#define RIL_REQUEST_QUERY_5G_STATUS 151
#define RIL_REQUEST_QUERY_ENDC_DCNR 152
#define RIL_REQUEST_QUERY_NR_BEARER_ALLOCATION 153
#define RIL_REQUEST_QUERY_NR_SIGNAL_STRENGTH 154
#define RIL_REQUEST_QUERY_UPPERLAYER_IND_INFO 155
#define RIL_REQUEST_QUERY_5G_CONFIG_INFO 156

/**
 * RIL_REQUEST_SET_NR_DISABLE_MODE
 *
 * set 5G NA/NSA disable mode
 *
 * "data" is int * to enum RIL_NR_DISABLE_MODE
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  MODEM_ERR
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 */
#define RIL_REQUEST_SET_NR_DISABLE_MODE 157

/**
 * RIL_REQUEST_QUERY_NR_DISABLE_MODE
 *
 * Queries the status of 5G NA/NSA disable mode
 *
 * "data" is NULL
 * "response" is a RIL_NR_DISABLE_MODE *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  NO_MEMORY
 *  MODEM_ERR
 *  INTERNAL_ERR
 *  NO_RESOURCES
 *  REQUEST_NOT_SUPPORTED
 */
#define RIL_REQUEST_QUERY_NR_DISABLE_MODE 158

/**
 * RIL_REQUEST_ENABLE_MODEM
 *
 * Toggle modem stack on and off
 *
 * "data" is int *
 * ((int *)data)[0] is > 0 for "Enable Modem Stack"
 * ((int *)data)[0] is == 0 for "Disable Modem Stack"
 *
 * "response" is NULL
 *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_STATE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  DEVICE_IN_USE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_ENABLE_MODEM 157

/**
 * RIL_REQUEST_GET_MODEM_STACK_STATUS
 *
 * Get modem stack status
 *
 * "data" is NULL
 *
 * "response" is int *
 * ((int *)data)[0] is == 1 for "Modem Stack Enabled"
 * ((int *)data)[0] is == 0 for "Modem Stack Disabled"
 *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_STATE
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  MODEM_ERR
 *  DEVICE_IN_USE
 *  OPERATION_NOT_ALLOWED
 *  INVALID_MODEM_STATE
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_GET_MODEM_STACK_STATUS 158

/**
 * RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE
 *
 * Send a CDMA SMS message
 *
 * "data" is const RIL_CDMA_SMS_Message *
 *
 * "response" is a const RIL_SMS_Response *
 *
 * Based on the return error, caller decides to resend if sending sms
 * fails. The CDMA error class is derived as follows,
 * SUCCESS is error class 0 (no error)
 * SMS_SEND_FAIL_RETRY is error class 2 (temporary failure)
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  SMS_SEND_FAIL_RETRY
 *  NETWORK_REJECT
 *  INVALID_STATE
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  REQUEST_RATE_LIMITED
 *  INVALID_SMS_FORMAT
 *  SYSTEM_ERR
 *  FDN_CHECK_FAILURE
 *  MODEM_ERR
 *  NETWORK_ERR
 *  ENCODING_ERR
 *  INVALID_SMSC_ADDRESS
 *  OPERATION_NOT_ALLOWED
 *  NO_RESOURCES
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *  MODE_NOT_SUPPORTED
 *  SIM_ABSENT
 */
#define RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE 159

/**
 * RIL_REQUEST_EMERGENCY_DIAL
 *
 * Initiate an emergency voice call
 *
 * Request that the modem begin making an emergency call instead of
 * using RIL_REQUEST_DIAL if the 'address' in the 'dialInfo' field
 * is identified as an emergency number
 *
 * "data" is a const RIL_RequestEmergencyDial
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE (radio resetting)
 *  DIAL_MODIFIED_TO_USSD
 *  DIAL_MODIFIED_TO_SS
 *  DIAL_MODIFIED_TO_DIAL
 *  INVALID_ARGUMENTS
 *  NO_MEMORY
 *  INVALID_STATE
 *  NO_RESOURCES
 *  INTERNAL_ERR
 *  FDN_CHECK_FAILURE
 *  MODEM_ERR
 *  NO_SUBSCRIPTION
 *  NO_NETWORK_FOUND
 *  INVALID_CALL_ID
 *  DEVICE_IN_USE
 *  OPERATION_NOT_ALLOWED
 *  ABORTED
 *  CANCELLED
 *  REQUEST_NOT_SUPPORTED
 *
 */
#define RIL_REQUEST_EMERGENCY_DIAL 205

/**
 * RIL_REQUEST_SET_PREFERRED_DATA_MODEM
 *
 * Set preferred data modem Id
 *
 * In a multi-SIM device, notify modem layer which logical modem will be used primarily
 * for data. It helps modem with resource optimization and decisions of what data connections
 * should be satisfied.
 *
 * "data" is int * the logical modem ID
 * "response" is RIL_ResponseError
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_SET_PREFERRED_DATA_MODEM 206

/**
 * RIL_REQUEST_GET_DATA_NR_ICON_TYPE
 *
 * Get data NR icon type
 *
 * "data" is NULL
 * "response" is const five_g_icon_type *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_GET_DATA_NR_ICON_TYPE 207

/**
 * RIL_REQUEST_GET_UICC_ENABLEMENT_STATUS
 *
 * Get SIM card enable/disable status
 *
 * "data" is NULL
 * response int *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_GET_UICC_ENABLEMENT_STATUS 208

/**
 * RIL_REQUEST_SET_LINK_CAP_FILTER
 *
 * payload : enum RIL_ReportFilter
 *
 * Response : int
 */
#define RIL_REQUEST_SET_LINK_CAP_FILTER 209
/**
 * RIL_REQUEST_SET_LINK_CAP_RPT_CRITERIA
 *
 * payload : struct RIL_LinkCapCriteria
 *
 * Response : enum RIL_LinkCapCriteriaResult
 */
#define RIL_REQUEST_SET_LINK_CAP_RPT_CRITERIA 210

/**
 * RIL_REQUEST_SIM_GET_ATR
 * payload: uint8_t with slot id
 * Response: const char * with ATR, size_t with length of ATR
 */
#define RIL_REQUEST_SIM_GET_ATR 211

/**
 * RIL_REQUEST_ENABLE_UICC_APPLICATIONS
 *
 * Enable SIM card
 *
 * "data" is int *
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_ENABLE_UICC_APPLICATIONS 212

/**
 * RIL_REQUEST_UPDATE_ADN_RECORD
 *
 * Update PBM ADN record
 *
 * "data" is RIL_AdnRecordInfo*
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_UPDATE_ADN_RECORD 213

/**
 * RIL_REQUEST_GET_ADN_RECORD
 *
 * Get ADN record
 *
 * "data" is void
 * "response" is RIL_AdnCountInfo *
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_GET_ADN_RECORD 214

/**
 * RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS
 *
 * Set System Selection parameters
 *
 * "data" is RIL_SysSelChannels*
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  INTERNAL_ERR
 *  INVALID_ARGUMENTS
 *
 */
#define RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS 215

/**
 * RIL_REQUEST_DATA_DUMP
 * Defined for collecting datamodule dump for LE from CLI
 * "data" is NULL
 * "response" is NULL
 */
#define RIL_REQUEST_DATA_DUMP 216

/**
 * RIL_REQUEST_DISCONNECT
 *
 * Sent by the client to let the server know that the client wishes
 * to disconnect. The server is expected to initiate the disconnection
 * process.
 *
 * "data" is NULL
 * "response" is NULL
 */
#define RIL_REQUEST_DISCONNECT 217

/**
 * RIL_REQUEST_SET_UNSOLICITED_DATA_RESPONSE_FILTER
 *
 * Sent by the client explicitly to register/deregister for Unsol Indication in Data
 *
 * First Bit need to be set if you want to register for UI Info Indication
 *
 * "data" is int
 * "response" is int
 */
#define RIL_REQUEST_SET_UNSOLICITED_DATA_RESPONSE_FILTER 218

/***********************************************************************/

/**
 * RIL_RESPONSE_ACKNOWLEDGEMENT
 *
 * This is used by Asynchronous solicited messages and Unsolicited messages
 * to acknowledge the receipt of those messages in RIL.java so that the ack
 * can be used to let ril.cpp to release wakelock.
 *
 * Valid errors
 * SUCCESS
 * RADIO_NOT_AVAILABLE
 */

#define RIL_RESPONSE_ACKNOWLEDGEMENT 800

/***********************************************************************/


#define RIL_UNSOL_RESPONSE_BASE 1000

/**
 * RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
 *
 * Indicate when value of RIL_RadioState has changed.
 *
 * Callee will invoke RIL_RadioStateRequest method on main thread
 *
 * "data" is NULL
 */

#define RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED 1000


/**
 * RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED
 *
 * Indicate when call state has changed
 *
 * Callee will invoke RIL_REQUEST_GET_CURRENT_CALLS on main thread
 *
 * "data" is NULL
 *
 * Response should be invoked on, for example,
 * "RING", "BUSY", "NO CARRIER", and also call state
 * transitions (DIALING->ALERTING ALERTING->ACTIVE)
 *
 * Redundent or extraneous invocations are tolerated
 */
#define RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED 1001


/**
 * RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED
 *
 * Called when the voice network state changed
 *
 * Callee will invoke the following requests on main thread:
 *
 * RIL_REQUEST_VOICE_REGISTRATION_STATE
 * RIL_REQUEST_OPERATOR
 *
 * "data" is NULL
 *
 * FIXME should this happen when SIM records are loaded? (eg, for
 * EONS)
 */
#define RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED 1002

/**
 * RIL_UNSOL_RESPONSE_NEW_SMS
 *
 * Called when new SMS is received.
 *
 * "data" is const char *
 * This is a pointer to a string containing the PDU of an SMS-DELIVER
 * as an ascii string of hex digits. The PDU starts with the SMSC address
 * per TS 27.005 (+CMT:)
 *
 * Callee will subsequently confirm the receipt of thei SMS with a
 * RIL_REQUEST_SMS_ACKNOWLEDGE
 *
 * No new RIL_UNSOL_RESPONSE_NEW_SMS
 * or RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT messages should be sent until a
 * RIL_REQUEST_SMS_ACKNOWLEDGE has been received
 */

#define RIL_UNSOL_RESPONSE_NEW_SMS 1003

/**
 * RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT
 *
 * Called when new SMS Status Report is received.
 *
 * "data" is const char *
 * This is a pointer to a string containing the PDU of an SMS-STATUS-REPORT
 * as an ascii string of hex digits. The PDU starts with the SMSC address
 * per TS 27.005 (+CDS:).
 *
 * Callee will subsequently confirm the receipt of the SMS with a
 * RIL_REQUEST_SMS_ACKNOWLEDGE
 *
 * No new RIL_UNSOL_RESPONSE_NEW_SMS
 * or RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT messages should be sent until a
 * RIL_REQUEST_SMS_ACKNOWLEDGE has been received
 */

#define RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT 1004

/**
 * RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM
 *
 * Called when new SMS has been stored on SIM card
 *
 * "data" is const int *
 * ((const int *)data)[0] contains the slot index on the SIM that contains
 * the new message
 */

#define RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM 1005

/**
 * RIL_UNSOL_ON_USSD
 *
 * Called when a new USSD message is received.
 *
 * "data" is a const RIL_UssdModeType *
 *
 * The USSD session is assumed to persist if the type code is "1", otherwise
 * the current session (if any) is assumed to have terminated.
 *
 * ((const char **)data)[1] points to a message string if applicable, which
 * should always be in UTF-8.
 */
#define RIL_UNSOL_ON_USSD 1006
/* Previously #define RIL_UNSOL_ON_USSD_NOTIFY 1006   */

/**
 * RIL_UNSOL_ON_USSD_REQUEST
 *
 * Obsolete. Send via RIL_UNSOL_ON_USSD
 */
#define RIL_UNSOL_ON_USSD_REQUEST 1007

/**
 * RIL_UNSOL_NITZ_TIME_RECEIVED
 *
 * Called when radio has received a NITZ time message
 *
 * "data" is const char * pointing to NITZ time string
 * in the form "yy/mm/dd,hh:mm:ss(+/-)tz,dt"
 */
#define RIL_UNSOL_NITZ_TIME_RECEIVED  1008

/**
 * RIL_UNSOL_SIGNAL_STRENGTH
 *
 * Radio may report signal strength rather han have it polled.
 *
 * "data" is a const RIL_SignalStrength *
 */
#define RIL_UNSOL_SIGNAL_STRENGTH  1009


/**
 * RIL_UNSOL_DATA_CALL_LIST_CHANGED
 *
 * "data" is an array of RIL_Data_Call_Response_v6 identical to that
 * returned by RIL_REQUEST_DATA_CALL_LIST. It is the complete list
 * of current data contexts including new contexts that have been
 * activated. A data call is only removed from this list when the
 * framework sends a RIL_REQUEST_DEACTIVATE_DATA_CALL or the radio
 * is powered off/on.
 *
 * See also: RIL_REQUEST_DATA_CALL_LIST
 */

#define RIL_UNSOL_DATA_CALL_LIST_CHANGED 1010

/**
 * RIL_UNSOL_SUPP_SVC_NOTIFICATION
 *
 * Reports supplementary service related notification from the network.
 *
 * "data" is a const RIL_SuppSvcNotification *
 *
 */

#define RIL_UNSOL_SUPP_SVC_NOTIFICATION 1011

/**
 * RIL_UNSOL_STK_SESSION_END
 *
 * Indicate when STK session is terminated by SIM.
 *
 * "data" is NULL
 */
#define RIL_UNSOL_STK_SESSION_END 1012

/**
 * RIL_UNSOL_STK_PROACTIVE_COMMAND
 *
 * Indicate when SIM issue a STK proactive command to applications
 *
 * "data" is a const char * containing SAT/USAT proactive command
 * in hexadecimal format string starting with command tag
 *
 */
#define RIL_UNSOL_STK_PROACTIVE_COMMAND 1013

/**
 * RIL_UNSOL_STK_EVENT_NOTIFY
 *
 * Indicate when SIM notifies applcations some event happens.
 * Generally, application does not need to have any feedback to
 * SIM but shall be able to indicate appropriate messages to users.
 *
 * "data" is a const char * containing SAT/USAT commands or responses
 * sent by ME to SIM or commands handled by ME, in hexadecimal format string
 * starting with first byte of response data or command tag
 *
 */
#define RIL_UNSOL_STK_EVENT_NOTIFY 1014

/**
 * RIL_UNSOL_STK_CALL_SETUP
 *
 * Indicate when SIM wants application to setup a voice call.
 *
 * "data" is const int *
 * ((const int *)data)[0] contains timeout value (in milliseconds)
 */
#define RIL_UNSOL_STK_CALL_SETUP 1015

/**
 * RIL_UNSOL_SIM_SMS_STORAGE_FULL
 *
 * Indicates that SMS storage on the SIM is full.  Sent when the network
 * attempts to deliver a new SMS message.  Messages cannot be saved on the
 * SIM until space is freed.  In particular, incoming Class 2 messages
 * cannot be stored.
 *
 * "data" is null
 *
 */
#define RIL_UNSOL_SIM_SMS_STORAGE_FULL 1016

/**
 * RIL_UNSOL_SIM_REFRESH
 *
 * Indicates that file(s) on the SIM have been updated, or the SIM
 * has been reinitialized.
 *
 * In the case where RIL is version 6 or older:
 * "data" is an int *
 * ((int *)data)[0] is a RIL_SimRefreshResult.
 * ((int *)data)[1] is the EFID of the updated file if the result is
 * SIM_FILE_UPDATE or NULL for any other result.
 *
 * In the case where RIL is version 7:
 * "data" is a RIL_SimRefreshResponse_v7 *
 *
 * Note: If the SIM state changes as a result of the SIM refresh (eg,
 * SIM_READY -> SIM_LOCKED_OR_ABSENT), RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED
 * should be sent.
 */
#define RIL_UNSOL_SIM_REFRESH 1017

/**
 * RIL_UNSOL_CALL_RING
 *
 * Ring indication for an incoming call (eg, RING or CRING event).
 * There must be at least one RIL_UNSOL_CALL_RING at the beginning
 * of a call and sending multiple is optional. If the system property
 * ro.telephony.call_ring.multiple is false then the upper layers
 * will generate the multiple events internally. Otherwise the vendor
 * ril must generate multiple RIL_UNSOL_CALL_RING if
 * ro.telephony.call_ring.multiple is true or if it is absent.
 *
 * The rate of these events is controlled by ro.telephony.call_ring.delay
 * and has a default value of 3000 (3 seconds) if absent.
 *
 * "data" is null for GSM
 * "data" is const RIL_CDMA_SignalInfoRecord * if CDMA
 */
#define RIL_UNSOL_CALL_RING 1018

/**
 * RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED
 *
 * Indicates that SIM state changes.
 *
 * Callee will invoke RIL_REQUEST_GET_SIM_STATUS on main thread

 * "data" is null
 */
#define RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED 1019

/**
 * RIL_UNSOL_RESPONSE_CDMA_NEW_SMS
 *
 * Called when new CDMA SMS is received
 *
 * "data" is const RIL_CDMA_SMS_Message *
 *
 * Callee will subsequently confirm the receipt of the SMS with
 * a RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE
 *
 * No new RIL_UNSOL_RESPONSE_CDMA_NEW_SMS should be sent until
 * RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE has been received
 *
 */
#define RIL_UNSOL_RESPONSE_CDMA_NEW_SMS 1020

/**
 * RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS
 *
 * Called when new Broadcast SMS is received
 *
 * "data" can be one of the following:
 * If received from GSM network, "data" is const char of 88 bytes
 * which indicates each page of a CBS Message sent to the MS by the
 * BTS as coded in 3GPP 23.041 Section 9.4.1.2.
 * If received from UMTS network, "data" is const char of 90 up to 1252
 * bytes which contain between 1 and 15 CBS Message pages sent as one
 * packet to the MS by the BTS as coded in 3GPP 23.041 Section 9.4.2.2.
 *
 */
#define RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS 1021

/**
 * RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL
 *
 * Indicates that SMS storage on the RUIM is full.  Messages
 * cannot be saved on the RUIM until space is freed.
 *
 * "data" is null
 *
 */
#define RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL 1022

/**
 * RIL_UNSOL_RESTRICTED_STATE_CHANGED
 *
 * Indicates a restricted state change (eg, for Domain Specific Access Control).
 *
 * Radio need send this msg after radio off/on cycle no matter it is changed or not.
 *
 * "data" is an int *
 * ((int *)data)[0] contains a bitmask of RIL_RESTRICTED_STATE_* values.
 */
#define RIL_UNSOL_RESTRICTED_STATE_CHANGED 1023

/**
 * RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE
 *
 * Indicates that the radio system selection module has
 * autonomously entered emergency callback mode.
 *
 * "data" is null
 *
 */
#define RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE 1024

/**
 * RIL_UNSOL_CDMA_CALL_WAITING
 *
 * Called when CDMA radio receives a call waiting indication.
 *
 * "data" is const RIL_CDMA_CallWaiting *
 *
 */
#define RIL_UNSOL_CDMA_CALL_WAITING 1025

/**
 * RIL_UNSOL_CDMA_OTA_PROVISION_STATUS
 *
 * Called when CDMA radio receives an update of the progress of an
 * OTASP/OTAPA call.
 *
 * "data" is const int *
 *  For CDMA this is an integer OTASP/OTAPA status listed in
 *  RIL_CDMA_OTA_ProvisionStatus.
 *
 */
#define RIL_UNSOL_CDMA_OTA_PROVISION_STATUS 1026

/**
 * RIL_UNSOL_CDMA_INFO_REC
 *
 * Called when CDMA radio receives one or more info recs.
 *
 * "data" is const RIL_CDMA_InformationRecords *
 *
 */
#define RIL_UNSOL_CDMA_INFO_REC 1027

/**
 * RIL_UNSOL_OEM_HOOK_RAW
 *
 * This is for OEM specific use.
 *
 * "data" is a byte[]
 */
#define RIL_UNSOL_OEM_HOOK_RAW 1028

/**
 * RIL_UNSOL_RINGBACK_TONE
 *
 * Indicates that nework doesn't have in-band information,  need to
 * play out-band tone.
 *
 * "data" is an int *
 * ((int *)data)[0] == 0 for stop play ringback tone.
 * ((int *)data)[0] == 1 for start play ringback tone.
 */
#define RIL_UNSOL_RINGBACK_TONE 1029

/**
 * RIL_UNSOL_RESEND_INCALL_MUTE
 *
 * Indicates that framework/application need reset the uplink mute state.
 *
 * There may be situations where the mute state becomes out of sync
 * between the application and device in some GSM infrastructures.
 *
 * "data" is null
 */
#define RIL_UNSOL_RESEND_INCALL_MUTE 1030

/**
 * RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED
 *
 * Called when CDMA subscription source changed.
 *
 * "data" is int *
 * ((int *)data)[0] is == RIL_CdmaSubscriptionSource
 */
#define RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED 1031

/**
 * RIL_UNSOL_CDMA_PRL_CHANGED
 *
 * Called when PRL (preferred roaming list) changes.
 *
 * "data" is int *
 * ((int *)data)[0] is PRL_VERSION as would be returned by RIL_REQUEST_CDMA_SUBSCRIPTION
 */
#define RIL_UNSOL_CDMA_PRL_CHANGED 1032

/**
 * RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE
 *
 * Called when Emergency Callback Mode Ends
 *
 * Indicates that the radio system selection module has
 * proactively exited emergency callback mode.
 *
 * "data" is NULL
 *
 */
#define RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE 1033

/**
 * RIL_UNSOL_RIL_CONNECTED
 *
 * Called the ril connects and returns the version
 *
 * "data" is int *
 * ((int *)data)[0] is RIL_VERSION
 */
#define RIL_UNSOL_RIL_CONNECTED 1034

/**
 * RIL_UNSOL_VOICE_RADIO_TECH_CHANGED
 *
 * Indicates that voice technology has changed. Contains new radio technology
 * as a data in the message.
 *
 * "data" is int *
 * ((int *)data)[0] is of type const RIL_RadioTechnology
 *
 */
#define RIL_UNSOL_VOICE_RADIO_TECH_CHANGED 1035

/**
 * RIL_UNSOL_CELL_INFO_LIST
 *
 * Same information as returned by RIL_REQUEST_GET_CELL_INFO_LIST, but returned
 * at the rate no greater than specified by RIL_REQUEST_SET_UNSOL_CELL_INFO_RATE.
 *
 * "data" is NULL
 *
 * "response" is an array of RIL_CellInfo_v12.
 */
#define RIL_UNSOL_CELL_INFO_LIST 1036

/**
 * RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED
 *
 * This message is DEPRECATED and shall be removed in a future release (target: 2018);
 * instead, provide IMS registration status via an IMS Service.
 *
 * Called when IMS registration state has changed
 *
 * To get IMS registration state and IMS SMS format, callee needs to invoke the
 * following request on main thread:
 *
 * RIL_REQUEST_IMS_REGISTRATION_STATE
 *
 * "data" is NULL
 *
 */
#define RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED 1037

/**
 * RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED
 *
 * Indicates when there is a change in subscription status.
 * This event will be sent in the following scenarios
 *  - subscription readiness at modem, which was selected by telephony layer
 *  - when subscription is deactivated by modem due to UICC card removal
 *  - When network invalidates the subscription i.e. attach reject due to authentication reject
 *
 * "data" is const int *
 * ((const int *)data)[0] == 0 for Subscription Deactivated
 * ((const int *)data)[0] == 1 for Subscription Activated
 *
 */
#define RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED 1038

/**
 * RIL_UNSOL_SRVCC_STATE_NOTIFY
 *
 * Called when Single Radio Voice Call Continuity(SRVCC)
 * progress state has changed
 *
 * "data" is int *
 * ((int *)data)[0] is of type const RIL_SrvccState
 *
 */

#define RIL_UNSOL_SRVCC_STATE_NOTIFY 1039

/**
 * RIL_UNSOL_HARDWARE_CONFIG_CHANGED
 *
 * Called when the hardware configuration associated with the RILd changes
 *
 * "data" is an array of RIL_HardwareConfig
 *
 */
#define RIL_UNSOL_HARDWARE_CONFIG_CHANGED 1040

/**
 * RIL_UNSOL_DC_RT_INFO_CHANGED - DEPRECATED
 *
 * The message is DEPRECATED, use RIL_REQUEST_GET_ACTIVITY_INFO
 * Sent when the DC_RT_STATE changes but the time
 * between these messages must not be less than the
 * value set by RIL_REQUEST_SET_DC_RT_RATE.
 *
 * "data" is the most recent RIL_DcRtInfo
 *
 */
#define RIL_UNSOL_DC_RT_INFO_CHANGED 1041

/**
 * RIL_UNSOL_RADIO_CAPABILITY
 *
 * Sent when RIL_REQUEST_SET_RADIO_CAPABILITY completes.
 * Returns the phone radio capability exactly as
 * RIL_REQUEST_GET_RADIO_CAPABILITY and should be the
 * same set as sent by RIL_REQUEST_SET_RADIO_CAPABILITY.
 *
 * "data" is the RIL_RadioCapability structure
 */
#define RIL_UNSOL_RADIO_CAPABILITY 1042

/*
 * RIL_UNSOL_ON_SS
 *
 * Called when SS response is received when DIAL/USSD/SS is changed to SS by
 * call control.
 *
 * "data" is const RIL_StkCcUnsolSsResponse *
 *
 */
#define RIL_UNSOL_ON_SS 1043

/**
 * RIL_UNSOL_STK_CC_ALPHA_NOTIFY
 *
 * Called when there is an ALPHA from UICC during Call Control.
 *
 * "data" is const char * containing ALPHA string from UICC in UTF-8 format.
 *
 */
#define RIL_UNSOL_STK_CC_ALPHA_NOTIFY 1044

/**
 * RIL_UNSOL_LCEDATA_RECV
 *
 * Called when there is an incoming Link Capacity Estimate (LCE) info report.
 *
 * "data" is the RIL_LinkCapacityEstimate structure.
 *
 */
#define RIL_UNSOL_LCEDATA_RECV 1045

 /**
  * RIL_UNSOL_PCO_DATA
  *
  * Called when there is new Carrier PCO data received for a data call.  Ideally
  * only new data will be forwarded, though this is not required.  Multiple
  * boxes of carrier PCO data for a given call should result in a series of
  * RIL_UNSOL_PCO_DATA calls.
  *
  * "data" is the RIL_PCO_Data structure.
  *
  */
#define RIL_UNSOL_PCO_DATA 1046

 /**
  * RIL_UNSOL_MODEM_RESTART
  *
  * Called when there is a modem reset.
  *
  * "reason" is "const char *" containing the reason for the reset. It
  * could be a crash signature if the restart was due to a crash or some
  * string such as "user-initiated restart" or "AT command initiated
  * restart" that explains the cause of the modem restart.
  *
  * When modem restarts, one of the following radio state transitions will happen
  * 1) RADIO_STATE_ON->RADIO_STATE_UNAVAILABLE->RADIO_STATE_ON or
  * 2) RADIO_STATE_OFF->RADIO_STATE_UNAVAILABLE->RADIO_STATE_OFF
  * This message can be sent either just before the RADIO_STATE changes to RADIO_STATE_UNAVAILABLE
  * or just after but should never be sent after the RADIO_STATE changes from UNAVAILABLE to
  * AVAILABLE(RADIO_STATE_ON/RADIO_STATE_OFF) again.
  *
  * It should NOT be sent after the RADIO_STATE changes to AVAILABLE after the
  * modem restart as that could be interpreted as a second modem reset by the
  * framework.
  */
#define RIL_UNSOL_MODEM_RESTART 1047

/**
 * RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION
 *
 * Called when the modem needs Carrier specific information that will
 * be used to encrypt IMSI and IMPI.
 *
 * "data" is NULL
 *
 */
#define RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION 1048

/**
 * RIL_UNSOL_NETWORK_SCAN_RESULT
 *
 * Returns incremental result for the network scan which is started by
 * RIL_REQUEST_START_NETWORK_SCAN, sent to report results, status, or errors.
 *
 * "data" is NULL
 * "response" is a const RIL_NetworkScanResult *
 */
#define RIL_UNSOL_NETWORK_SCAN_RESULT 1049

/**
 * RIL_UNSOL_KEEPALIVE_STATUS
 *
 * "response" is the  enum RIL_KeepaliveStatus_t
                      int32_t sessionHandle.
 *
 */
#define RIL_UNSOL_KEEPALIVE_STATUS 1050

/**
 * RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG
 *
 * "data" is the RIL_PhysicalChannelConfig structure.
 *
 */
#define RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG 1051

/**
 * RIL_UNSOL_EMERGENCY_NUMBERS_LIST
 *
 * "data" is the RIL_EmergencyList structure.
 *
 */
#define RIL_UNSOL_EMERGENCY_NUMBERS_LIST 1052

/**
 * RIL_UNSOL_DATA_NR_ICON_TYPE
 *
 * "data" is the five_g_icon_type structure.
 *
 */
#define RIL_UNSOL_DATA_NR_ICON_TYPE 1053

/**
 * RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED
 *
 * payload : int*
 *
 */
#define RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED 1054

/**
 * RIL_UNSOL_RESPONSE_ADN_INIT_DONE
 *
 */
#define RIL_UNSOL_RESPONSE_ADN_INIT_DONE 1055

/**
 * RIL_UNSOL_RESPONSE_ADN_RECORDS
 *
 */
#define RIL_UNSOL_RESPONSE_ADN_RECORDS 1056

/***********************************************************************/


#if defined(ANDROID_MULTI_SIM)
/**
 * RIL_Request Function pointer
 *
 * @param request is one of RIL_REQUEST_*
 * @param data is pointer to data defined for that RIL_REQUEST_*
 *        data is owned by caller, and should not be modified or freed by callee
 *        structures passed as data may contain pointers to non-contiguous memory
 * @param t should be used in subsequent call to RIL_onResponse
 * @param datalen is the length of "data" which is defined as other argument. It may or may
 *        not be equal to sizeof(data). Refer to the documentation of individual structures
 *        to find if pointers listed in the structure are contiguous and counted in the datalen
 *        length or not.
 *        (Eg: RIL_IMS_SMS_Message where we don't have datalen equal to sizeof(data))
 *
 */
typedef void (*RIL_RequestFunc) (int request, void *data,
                                    size_t datalen, RIL_Token t, RIL_SOCKET_ID socket_id);

/**
 * This function should return the current radio state synchronously
 */
typedef RIL_RadioState (*RIL_RadioStateRequest)(RIL_SOCKET_ID socket_id);

#else
/* Backward compatible */

/**
 * RIL_Request Function pointer
 *
 * @param request is one of RIL_REQUEST_*
 * @param data is pointer to data defined for that RIL_REQUEST_*
 *        data is owned by caller, and should not be modified or freed by callee
 *        structures passed as data may contain pointers to non-contiguous memory
 * @param t should be used in subsequent call to RIL_onResponse
 * @param datalen is the length of "data" which is defined as other argument. It may or may
 *        not be equal to sizeof(data). Refer to the documentation of individual structures
 *        to find if pointers listed in the structure are contiguous and counted in the datalen
 *        length or not.
 *        (Eg: RIL_IMS_SMS_Message where we don't have datalen equal to sizeof(data))
 *
 */
typedef void (*RIL_RequestFunc) (int request, void *data,
                                    size_t datalen, RIL_Token t);

/**
 * This function should return the current radio state synchronously
 */
typedef RIL_RadioState (*RIL_RadioStateRequest)();

#endif


/**
 * This function returns "1" if the specified RIL_REQUEST code is
 * supported and 0 if it is not
 *
 * @param requestCode is one of RIL_REQUEST codes
 */

typedef int (*RIL_Supports)(int requestCode);

/**
 * This function is called from a separate thread--not the
 * thread that calls RIL_RequestFunc--and indicates that a pending
 * request should be cancelled.
 *
 * On cancel, the callee should do its best to abandon the request and
 * call RIL_onRequestComplete with RIL_Errno CANCELLED at some later point.
 *
 * Subsequent calls to  RIL_onRequestComplete for this request with
 * other results will be tolerated but ignored. (That is, it is valid
 * to ignore the cancellation request)
 *
 * RIL_Cancel calls should return immediately, and not wait for cancellation
 *
 * Please see ITU v.250 5.6.1 for how one might implement this on a TS 27.007
 * interface
 *
 * @param t token wants to be canceled
 */

typedef void (*RIL_Cancel)(RIL_Token t);

typedef void (*RIL_TimedCallback) (void *param);

/**
 * Return a version string for your RIL implementation
 */
typedef const char * (*RIL_GetVersion) (void);

typedef struct {
    int version;        /* set to RIL_VERSION */
    RIL_RequestFunc onRequest;
    RIL_RadioStateRequest onStateRequest;
    RIL_Supports supports;
    RIL_Cancel onCancel;
    RIL_GetVersion getVersion;
} RIL_RadioFunctions;

typedef struct {
    char *apn;                  /* the APN to connect to */
    char *protocol;             /* one of the PDP_type values in TS 27.007 section 10.1.1 used on
                                   roaming network. For example, "IP", "IPV6", "IPV4V6", or "PPP".*/
    int authtype;               /* authentication protocol used for this PDP context
                                   (None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3) */
    char *username;             /* the username for APN, or NULL */
    char *password;             /* the password for APN, or NULL */
} RIL_InitialAttachApn;

typedef struct {
    char *apn;                  /* the APN to connect to */
    char *protocol;             /* one of the PDP_type values in TS 27.007 section 10.1.1 used on
                                   home network. For example, "IP", "IPV6", "IPV4V6", or "PPP". */
    char *roamingProtocol;      /* one of the PDP_type values in TS 27.007 section 10.1.1 used on
                                   roaming network. For example, "IP", "IPV6", "IPV4V6", or "PPP".*/
    int authtype;               /* authentication protocol used for this PDP context
                                   (None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3) */
    char *username;             /* the username for APN, or NULL */
    char *password;             /* the password for APN, or NULL */
    int supportedTypesBitmask;  /* supported APN types bitmask. See RIL_ApnTypes for the value of
                                   each bit. */
    int bearerBitmask;          /* the bearer bitmask. See RIL_RadioAccessFamily for the value of
                                   each bit. */
    int modemCognitive;         /* indicating the APN setting was sent to the modem through
                                   setDataProfile earlier. */
    int mtu;                    /* maximum transmission unit (MTU) size in bytes */
    char *mvnoType;             /* the MVNO type: possible values are "imsi", "gid", "spn" */
    char *mvnoMatchData;        /* MVNO match data. Can be anything defined by the carrier.
                                   For example,
                                     SPN like: "A MOBILE", "BEN NL", etc...
                                     IMSI like: "302720x94", "2060188", etc...
                                     GID like: "4E", "33", etc... */
} RIL_InitialAttachApn_v15;
/** @public
* RIL_SimAuthentication
*/
typedef struct {
    /** P2 value of authentication command, see P2 parameter in
       3GPP TS 31.102 7.1.2 */
    int authContext;
    /** the challenge string in Base64 format, see 3GPP
       TS 31.102 7.1.2 */
    char *authData;
    /** AID value, See ETSI 102.221 8.1 and 101.220 4,
       NULL if no value. */
    char *aid;
} RIL_SimAuthentication;

typedef struct {
    int cid;                    /* Context ID, uniquely identifies this call */
    char *bearer_proto;         /* One of the PDP_type values in TS 27.007 section 10.1.1.
                                   For example, "IP", "IPV6", "IPV4V6". */
    int pco_id;                 /* The protocol ID for this box.  Note that only IDs from
                                   FF00H - FFFFH are accepted.  If more than one is included
                                   from the network, multiple calls should be made to send all
                                   of them. */
    int contents_length;        /* The number of octets in the contents. */
    char *contents;             /* Carrier-defined content.  It is binary, opaque and
                                   loosely defined in LTE Layer 3 spec 24.008 */
} RIL_PCO_Data;

/** @public
 * RIL_KeepaliveType
 */
typedef enum {
    /** IPV4 */
    NATT_IPV4 = 0,              /* Keepalive specified by RFC 3948 Sec. 2.3 using IPv4 */
    /** IPV6 */
    NATT_IPV6 = 1               /* Keepalive specified by RFC 3948 Sec. 2.3 using IPv6 */
} RIL_KeepaliveType;

#define MAX_INADDR_LEN 16
typedef struct {
    RIL_KeepaliveType type;                  /* Type of keepalive packet */
    char sourceAddress[MAX_INADDR_LEN];      /* Source address in network-byte order */
    int sourcePort;                          /* Source port if applicable, or 0x7FFFFFFF;
                                                the maximum value is 65535 */
    char destinationAddress[MAX_INADDR_LEN]; /* Destination address in network-byte order */
    int destinationPort;                     /* Destination port if applicable or 0x7FFFFFFF;
                                                the maximum value is 65535 */
    int maxKeepaliveIntervalMillis;          /* Maximum milliseconds between two packets */
    int cid;                                 /* Context ID, uniquely identifies this call */
} RIL_KeepaliveRequest;

/** @public
 * RIL_KeepaliveStatusCode
 */
typedef enum {
    /** ACTIVE */
    KEEPALIVE_ACTIVE,                       /* Keepalive session is active */
    /** INACTIVE */
    KEEPALIVE_INACTIVE,                     /* Keepalive session is inactive */
    /** PENDING */
    KEEPALIVE_PENDING                       /* Keepalive session status not available */
} RIL_KeepaliveStatusCode;

typedef struct {
    uint32_t sessionHandle;
    RIL_KeepaliveStatusCode code;
} RIL_KeepaliveStatus;

#ifdef RIL_SHLIB
struct RIL_Env {
    /**
     * "t" is parameter passed in on previous call to RIL_Notification
     * routine.
     *
     * If "e" != SUCCESS, then response can be null/is ignored
     *
     * "response" is owned by caller, and should not be modified or
     * freed by callee
     *
     * RIL_onRequestComplete will return as soon as possible
     */
    void (*OnRequestComplete)(RIL_Token t, RIL_Errno e,
                           void *response, size_t responselen);

#if defined(ANDROID_MULTI_SIM)
    /**
     * "unsolResponse" is one of RIL_UNSOL_RESPONSE_*
     * "data" is pointer to data defined for that RIL_UNSOL_RESPONSE_*
     *
     * "data" is owned by caller, and should not be modified or freed by callee
     */
    void (*OnUnsolicitedResponse)(int unsolResponse, const void *data, size_t datalen, RIL_SOCKET_ID socket_id);
#else
    /**
     * "unsolResponse" is one of RIL_UNSOL_RESPONSE_*
     * "data" is pointer to data defined for that RIL_UNSOL_RESPONSE_*
     *
     * "data" is owned by caller, and should not be modified or freed by callee
     */
    void (*OnUnsolicitedResponse)(int unsolResponse, const void *data, size_t datalen);
#endif
    /**
     * Call user-specifed "callback" function on on the same thread that
     * RIL_RequestFunc is called. If "relativeTime" is specified, then it specifies
     * a relative time value at which the callback is invoked. If relativeTime is
     * NULL or points to a 0-filled structure, the callback will be invoked as
     * soon as possible
     */

    void (*RequestTimedCallback) (RIL_TimedCallback callback,
                                   void *param, const struct timeval *relativeTime);
   /**
    * "t" is parameter passed in on previous call RIL_Notification routine
    *
    * RIL_onRequestAck will be called by vendor when an Async RIL request was received
    * by them and an ack needs to be sent back to java ril.
    */
    void (*OnRequestAck) (RIL_Token t);
};


/**
 *  RIL implementations must defined RIL_Init
 *  argc and argv will be command line arguments intended for the RIL implementation
 *  Return NULL on error
 *
 * @param env is environment point defined as RIL_Env
 * @param argc number of arguments
 * @param argv list fo arguments
 *
 */
const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv);

#else /* RIL_SHLIB */

/**
 * Call this once at startup to register notification routine
 *
 * @param callbacks user-specifed callback function
 */
void RIL_register (const RIL_RadioFunctions *callbacks);

#ifdef __cplusplus
extern "C"
#endif
void rilc_thread_pool();


/**
 *
 * RIL_onRequestComplete will return as soon as possible
 *
 * @param t is parameter passed in on previous call to RIL_Notification
 *          routine.
 * @param e error code
 *          if "e" != SUCCESS, then response can be null/is ignored
 * @param response is owned by caller, and should not be modified or
 *                 freed by callee
 * @param responselen the length of response in byte
 */
void RIL_onRequestComplete(RIL_Token t, RIL_Errno e,
                           void *response, size_t responselen);

/**
 * RIL_onRequestAck will be called by vendor when an Async RIL request was received by them and
 * an ack needs to be sent back to java ril. This doesn't mark the end of the command or it's
 * results, just that the command was received and will take a while. After sending this Ack
 * its vendor's responsibility to make sure that AP is up whenever needed while command is
 * being processed.
 *
 * @param t is parameter passed in on previous call to RIL_Notification
 *          routine.
 */
void RIL_onRequestAck(RIL_Token t);

#if defined(ANDROID_MULTI_SIM)
/**
 * @param unsolResponse is one of RIL_UNSOL_RESPONSE_*
 * @param data is pointer to data defined for that RIL_UNSOL_RESPONSE_*
 *     "data" is owned by caller, and should not be modified or freed by callee
 * @param datalen the length of data in byte
 */

void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen, RIL_SOCKET_ID socket_id);
#else
/**
 * @param unsolResponse is one of RIL_UNSOL_RESPONSE_*
 * @param data is pointer to data defined for that RIL_UNSOL_RESPONSE_*
 *     "data" is owned by caller, and should not be modified or freed by callee
 * @param datalen the length of data in byte
 */

void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen);
#endif

/**
 * Call user-specifed "callback" function on on the same thread that
 * RIL_RequestFunc is called. If "relativeTime" is specified, then it specifies
 * a relative time value at which the callback is invoked. If relativeTime is
 * NULL or points to a 0-filled structure, the callback will be invoked as
 * soon as possible
 *
 * @param callback user-specifed callback function
 * @param param parameter list
 * @param relativeTime a relative time value at which the callback is invoked
 */

void RIL_requestTimedCallback (RIL_TimedCallback callback,
                               void *param, const struct timeval *relativeTime);

#endif /* RIL_SHLIB */

/*RIL_SetUpDataCallParams
 *                          1) accessNetwork, the access Network to use for values
 *                          RIL_RadioAccessNetworks. The access network to setup the data call.
 *                          If the data connection cannot be established on the specified access
 *                          network, the setup request must be failed.
 *                          2) dataProfileInfo, RIL_DataProfileInfo_IRadio_1_4
 *                          3) roamingAllowed, the boolean string indicating data roaming is
 *                          allowed or not. "1" indicates data roaming is enabled by the user,
 *                          "0" indicates disabled.
 *                          4) reason, the request reason. Must be
 *                          RIL_RadioDataRequestReasons.NORMAL or
 *                          RIL_RadioDataRequestReasons.HANDOVER.
 *                          5) addresses, if the reason is RIL_RadioDataRequestReasons.HANDOVER,
 *                          this indicates the space-delimited list of link addresses of the
 *                          existing data connection. The format is IP address with optional "/"
 *                          prefix length (The format is defined in RFC-4291 section 2.3).
 *                          For example, "192.0.1.3", "192.0.1.11/16", or "2001:db8::1/64".
 *                          Typically one IPv4 or one IPv6 or one of each. If the prefix length is
 *                          absent, then the addresses are assumed to be point to point with IPv4
 *                          with prefix length 32 or IPv6 with prefix length 128. This parameter
 *                          must be ignored unless reason is RIL_RadioDataRequestReasons.HANDOVER.
 *                          6) dnses, if the reason is RIL_RadioDataRequestReasons.HANDOVER,
 *                          this indicates the space-delimited list of DNS addresses of the
 *                          existing data connection. The format is defined in RFC-4291
 *                          section 2.2. For example, "192.0.1.3" or "2001:db8::1". This parameter
 *                          must be ignored unless reason is RIL_RadioDataRequestReasons.HANDOVER.
*/

typedef struct {
    RIL_RadioAccessNetworks accessNetwork;
    RIL_DataProfileInfo_IRadio_1_4 profileInfo;
    uint8_t roamingAllowed;
    RIL_RadioDataRequestReasons reason;
    char* addresses;
    char* dnses;
} RIL_SetUpDataCallParams;

#ifdef __cplusplus
}
#endif

#endif /*ANDROID_RIL_H*/