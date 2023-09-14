/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_RIL_IMS_H
#define ANDROID_RIL_IMS_H 1

#include <telephony/ril.h>

enum RIL_IMS_RegistrationState {
  RIL_IMS_REG_STATE_UNKNOWN = 0,
  RIL_IMS_REG_STATE_REGISTERED = 1,
  RIL_IMS_REG_STATE_NOT_REGISTERED = 2,
  RIL_IMS_REG_STATE_REGISTERING = 3,
};

struct RIL_IMS_Registration {
  RIL_IMS_RegistrationState state;
  uint32_t errorCode; /* Returned when the state is NOT_REGISTERED
                                      Values:
                                        - 3xx -- Redirection
                                        - 4xx -- Client failure
                                        - 5xx -- Server failure
                                        - 6xx -- Global failure */
  char* errorMessage; /* String associated with the error code */
  RIL_RadioTechnology radioTech;
  char* pAssociatedUris; /* List of semi-colon-separated URI(s) for self-identity. */
};

enum RIL_IMS_CallType {
  RIL_IMS_CALL_TYPE_UNKNOWN = 0,      /* Unknown */
  RIL_IMS_CALL_TYPE_VOICE = 1,        /* Voice only call */
  RIL_IMS_CALL_TYPE_VT_TX = 2,        /* PS Video telephony call: one way TX video,
                                       * two way audio */
  RIL_IMS_CALL_TYPE_VT_RX = 3,        /* Video telephony call: one way RX video,
                                       * two way audio */
  RIL_IMS_CALL_TYPE_VT = 4,           /* Video telephony call: two way video,
                                       * two way audio */
  RIL_IMS_CALL_TYPE_VT_NODIR = 5,     /* Video telephony call: no direction
                                       * two way audio
                                       * intermediate state in a video call till
                                       * video link is setup */
  RIL_IMS_CALL_TYPE_SMS = 6,          /* SMS */
  RIL_IMS_CALL_TYPE_UT = 7,           /* UT  - Supplementary services */
  RIL_IMS_CALL_TYPE_USSD = 8,         /* USSD */
  RIL_IMS_CALL_TYPE_CALLCOMPOSER = 9, /* CALLCOMPOSER */
};

enum RIL_IMS_StatusType {
  RIL_IMS_STATUS_DISABLED = 0,
  RIL_IMS_STATUS_PARTIALLY_ENABLED = 1,
  RIL_IMS_STATUS_ENABLED = 2,
  RIL_IMS_STATUS_NOT_SUPPORTED = 3, /* Example no modem support for IMS */
};

enum RIL_IMS_RttMode {
  RIL_IMS_RTT_DISABLED = 0,
  RIL_IMS_RTT_FULL = 1,
};

struct RIL_IMS_AccessTechnologyStatus {
  RIL_RadioTechnology networkMode;
  RIL_IMS_StatusType status; /* Status takes precedence over registered i.e if due to a
                              * error scenario status is disabled but registered says IMS
                              * registered then the feature is treated disabled */
  uint32_t restrictCause;
  RIL_IMS_Registration registration; /* Registered is applicable only to Query request and
                                      * and not Set. Field is valid only for status values of
                                      * Enabled or Partially Enabled - n/a for other
                                      * values of status */
};

struct RIL_IMS_ServiceStatusInfo {
  RIL_IMS_CallType callType;                    /* Type of Call the update is for */
  RIL_IMS_AccessTechnologyStatus accTechStatus; /* Status of Service per access tech */
  RIL_IMS_RttMode rttMode;                      /* Rtt Mode */
};

struct RIL_IMS_SubConfigInfo {
  uint32_t simultStackCount;   /* Simultaneous IMS stack static support */
  size_t   imsStackEnabledLen; /* Length of the imsStackEnabled array */
  uint8_t *imsStackEnabled;    /* Array indicating IMS Capability of the Device;
                                 ex: [true, false] - Ims enabled on slot 1; disabled on slot 2 */
};

enum RIL_IMS_ToneOperation {
    RIL_IMS_TONE_OP_INVALID,
    RIL_IMS_TONE_OP_STOP,
    RIL_IMS_TONE_OP_START,
};

enum RIL_IMS_CallState {
    RIL_IMS_CALLSTATE_UNKNOWN,
    RIL_IMS_CALLSTATE_ACTIVE,
    RIL_IMS_CALLSTATE_HOLDING,
    RIL_IMS_CALLSTATE_DIALING,    /* MO call only */
    RIL_IMS_CALLSTATE_ALERTING,   /* MO call only */
    RIL_IMS_CALLSTATE_INCOMING,   /* MT call only */
    RIL_IMS_CALLSTATE_WAITING,    /* MT call only */
    RIL_IMS_CALLSTATE_END,
};

enum RIL_IMS_CallDomain {
    RIL_IMS_CALLDOMAIN_UNKNOWN,
    RIL_IMS_CALLDOMAIN_CS,        /* Circuit Switched Domain */
    RIL_IMS_CALLDOMAIN_PS,        /* Packet Switched Domain */
    RIL_IMS_CALLDOMAIN_AUTOMATIC, /* Automatic domain - domain for a new call should be selected by modem */
    RIL_IMS_CALLDOMAIN_NOT_SET,
    RIL_IMS_CALLDOMAIN_INVALID,
};

enum RIL_IMS_CallModifiedCause {
  RIL_IMS_CMODCAUSE_NONE = 0, /* No call modify cause information \n  */
  RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_LOCAL_REQ = 1, /* Upgrade due to a local request \n  */
  RIL_IMS_CMODCAUSE_UPGRADE_DUE_TO_REMOTE_REQ = 2, /* Upgrade due to a remote request \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ = 3, /* Downgrade due to a local request \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ = 4, /* Downgrade due to a remote request \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT = 5, /* Downgrade due to an RTP timeout \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_QOS = 6, /* Downgrade due to QOS \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS = 7, /* Downgrade due to a packet loss \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT = 8, /* Downgrade due to low throughput \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION = 9, /* Downgrade due to thermal mitigation \n  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_LIPSYNC = 10, /* Downgrade due to lipsync  */
  RIL_IMS_CMODCAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR = 11, /* Downgrade due to a general error \n  */
};

enum RIL_IMS_CallFailCause {
  RIL_IMS_FAILCAUSE_UNKNOWN = 0,
  RIL_IMS_FAILCAUSE_UNOBTAINABLE_NUMBER = 1,
  RIL_IMS_FAILCAUSE_NO_ROUTE_TO_DESTINATION = 3,
  RIL_IMS_FAILCAUSE_CHANNEL_UNACCEPTABLE = 6,
  RIL_IMS_FAILCAUSE_OPERATOR_DETERMINED_BARRING = 8,
  RIL_IMS_FAILCAUSE_NORMAL = 16,
  RIL_IMS_FAILCAUSE_BUSY = 17,
  RIL_IMS_FAILCAUSE_NO_USER_RESPONDING = 18,
  RIL_IMS_FAILCAUSE_NO_ANSWER_FROM_USER = 19,
  RIL_IMS_FAILCAUSE_SUBSCRIBER_ABSENT = 20,
  RIL_IMS_FAILCAUSE_CALL_REJECTED = 21,
  RIL_IMS_FAILCAUSE_NUMBER_CHANGED = 22,
  RIL_IMS_FAILCAUSE_PREEMPTION = 25,
  RIL_IMS_FAILCAUSE_DESTINATION_OUT_OF_ORDER = 27,
  RIL_IMS_FAILCAUSE_INVALID_NUMBER_FORMAT = 28,
  RIL_IMS_FAILCAUSE_FACILITY_REJECTED = 29,
  RIL_IMS_FAILCAUSE_RESP_TO_STATUS_ENQUIRY = 30,
  RIL_IMS_FAILCAUSE_NORMAL_UNSPECIFIED = 31,
  RIL_IMS_FAILCAUSE_CONGESTION = 34,
  RIL_IMS_FAILCAUSE_NETWORK_OUT_OF_ORDER = 38,
  RIL_IMS_FAILCAUSE_TEMPORARY_FAILURE = 41,
  RIL_IMS_FAILCAUSE_SWITCHING_EQUIPMENT_CONGESTION = 42,
  RIL_IMS_FAILCAUSE_ACCESS_INFORMATION_DISCARDED = 43,
  RIL_IMS_FAILCAUSE_REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE = 44,
  RIL_IMS_FAILCAUSE_RESOURCES_UNAVAILABLE_OR_UNSPECIFIED = 47,
  RIL_IMS_FAILCAUSE_QOS_UNAVAILABLE = 49,
  RIL_IMS_FAILCAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED = 50,
  RIL_IMS_FAILCAUSE_INCOMING_CALLS_BARRED_WITHIN_CUG = 55,
  RIL_IMS_FAILCAUSE_BEARER_CAPABILITY_NOT_AUTHORIZED = 57,
  RIL_IMS_FAILCAUSE_BEARER_CAPABILITY_UNAVAILABLE = 58,
  RIL_IMS_FAILCAUSE_SERVICE_OPTION_NOT_AVAILABLE = 63,
  RIL_IMS_FAILCAUSE_BEARER_SERVICE_NOT_IMPLEMENTED = 65,
  RIL_IMS_FAILCAUSE_ACM_LIMIT_EXCEEDED = 68,
  RIL_IMS_FAILCAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED = 69,
  RIL_IMS_FAILCAUSE_ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE = 70,
  RIL_IMS_FAILCAUSE_SERVICE_OR_OPTION_NOT_IMPLEMENTED = 79,
  RIL_IMS_FAILCAUSE_INVALID_TRANSACTION_IDENTIFIER = 81,
  RIL_IMS_FAILCAUSE_USER_NOT_MEMBER_OF_CUG = 87,
  RIL_IMS_FAILCAUSE_INCOMPATIBLE_DESTINATION = 88,
  RIL_IMS_FAILCAUSE_INVALID_TRANSIT_NW_SELECTION = 91,
  RIL_IMS_FAILCAUSE_SEMANTICALLY_INCORRECT_MESSAGE = 95,
  RIL_IMS_FAILCAUSE_INVALID_MANDATORY_INFORMATION = 96,
  RIL_IMS_FAILCAUSE_MESSAGE_TYPE_NON_IMPLEMENTED = 97,
  RIL_IMS_FAILCAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 98,
  RIL_IMS_FAILCAUSE_INFORMATION_ELEMENT_NON_EXISTENT = 99,
  RIL_IMS_FAILCAUSE_CONDITIONAL_IE_ERROR = 100,
  RIL_IMS_FAILCAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 101,
  RIL_IMS_FAILCAUSE_RECOVERY_ON_TIMER_EXPIRED = 102,
  RIL_IMS_FAILCAUSE_PROTOCOL_ERROR_UNSPECIFIED = 111,
  RIL_IMS_FAILCAUSE_INTERWORKING_UNSPECIFIED = 127,
  RIL_IMS_FAILCAUSE_CALL_BARRED = 240,
  RIL_IMS_FAILCAUSE_FDN_BLOCKED = 241,
  RIL_IMS_FAILCAUSE_IMSI_UNKNOWN_IN_VLR = 242,
  RIL_IMS_FAILCAUSE_IMEI_NOT_ACCEPTED = 243,
  RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_USSD = 244, /* STK Call Control */
  RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_SS = 245,   /* STK Call Control */
  RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_DIAL = 246, /* STK Call Control */
  RIL_IMS_FAILCAUSE_RADIO_OFF = 247, /* Radio is OFF */
  RIL_IMS_FAILCAUSE_OUT_OF_SERVICE = 248, /* No cellular coverage */
  RIL_IMS_FAILCAUSE_NO_VALID_SIM = 249, /* No valid SIM is present */
  RIL_IMS_FAILCAUSE_RADIO_INTERNAL_ERROR = 250, /* Internal error at Modem */
  RIL_IMS_FAILCAUSE_NETWORK_RESP_TIMEOUT = 251, /* No response from network */
  RIL_IMS_FAILCAUSE_NETWORK_REJECT = 252, /* Explicit network reject */
  RIL_IMS_FAILCAUSE_RADIO_ACCESS_FAILURE = 253, /* RRC connection failure. Eg.RACH */
  RIL_IMS_FAILCAUSE_RADIO_LINK_FAILURE = 254, /* Radio Link Failure */
  RIL_IMS_FAILCAUSE_RADIO_LINK_LOST = 255, /* Radio link lost due to poor coverage */
  RIL_IMS_FAILCAUSE_RADIO_UPLINK_FAILURE = 256, /* Radio uplink failure */
  RIL_IMS_FAILCAUSE_RADIO_SETUP_FAILURE = 257, /* RRC connection setup failure */
  RIL_IMS_FAILCAUSE_RADIO_RELEASE_NORMAL = 258, /* RRC connection release, normal */
  RIL_IMS_FAILCAUSE_RADIO_RELEASE_ABNORMAL = 259, /* RRC connection release, abnormal */
  RIL_IMS_FAILCAUSE_ACCESS_CLASS_BLOCKED = 260, /* Access class barring */
  RIL_IMS_FAILCAUSE_NETWORK_DETACH = 261, /* Explicit network detach */
  RIL_IMS_FAILCAUSE_EMERGENCY_TEMP_FAILURE = 325, /* CALL_END_CAUSE_TEMP_REDIAL_ALLOWED = 0x145 */
  RIL_IMS_FAILCAUSE_EMERGENCY_PERM_FAILURE = 326, /* CALL_END_CAUSE_PERM_REDIAL_NOT_NEEDED = 0x146 */
  RIL_IMS_FAILCAUSE_CDMA_LOCKED_UNTIL_POWER_CYCLE = 1000,
  RIL_IMS_FAILCAUSE_CDMA_DROP = 1001,
  RIL_IMS_FAILCAUSE_CDMA_INTERCEPT = 1002,
  RIL_IMS_FAILCAUSE_CDMA_REORDER = 1003,
  RIL_IMS_FAILCAUSE_CDMA_SO_REJECT = 1004,
  RIL_IMS_FAILCAUSE_CDMA_RETRY_ORDER = 1005,
  RIL_IMS_FAILCAUSE_CDMA_ACCESS_FAILURE = 1006,
  RIL_IMS_FAILCAUSE_CDMA_PREEMPTED = 1007,
  RIL_IMS_FAILCAUSE_CDMA_NOT_EMERGENCY = 1008, /* For non-emergency number dialed during emergency callback mode */
  RIL_IMS_FAILCAUSE_CDMA_ACCESS_BLOCKED = 1009, /* CDMA network access probes blocked */
  RIL_IMS_FAILCAUSE_INCOMPATIBILITY_DESTINATION,
  RIL_IMS_FAILCAUSE_HO_NOT_FEASIBLE,     /* Call was ended due to LTE to 3G/2G handover not feasible*/
  RIL_IMS_FAILCAUSE_USER_BUSY,           /* User ends a call.*/
  RIL_IMS_FAILCAUSE_USER_REJECT,         /* User rejects incoming call.*/
  RIL_IMS_FAILCAUSE_LOW_BATTERY,         /* Call end/reject due to low battery.*/
  RIL_IMS_FAILCAUSE_BLACKLISTED_CALL_ID, /* Call end/reject due to blacklisted caller.*/
  RIL_IMS_FAILCAUSE_CS_RETRY_REQUIRED,   /* Call end due to CS Fallback request from lower layers.*/
  RIL_IMS_FAILCAUSE_NETWORK_UNAVAILABLE,
  RIL_IMS_FAILCAUSE_FEATURE_UNAVAILABLE,
  RIL_IMS_FAILCAUSE_SIP_ERROR,
  RIL_IMS_FAILCAUSE_MISC,
  RIL_IMS_FAILCAUSE_ANSWERED_ELSEWHERE,
  RIL_IMS_FAILCAUSE_PULL_OUT_OF_SYNC,
  RIL_IMS_FAILCAUSE_CAUSE_CALL_PULLED,
  RIL_IMS_FAILCAUSE_SIP_REDIRECTED,
  RIL_IMS_FAILCAUSE_SIP_BAD_REQUEST,
  RIL_IMS_FAILCAUSE_SIP_FORBIDDEN,
  RIL_IMS_FAILCAUSE_SIP_NOT_FOUND,
  RIL_IMS_FAILCAUSE_SIP_NOT_SUPPORTED,
  RIL_IMS_FAILCAUSE_SIP_REQUEST_TIMEOUT,
  RIL_IMS_FAILCAUSE_SIP_TEMPORARILY_UNAVAILABLE,
  RIL_IMS_FAILCAUSE_SIP_BAD_ADDRESS,
  RIL_IMS_FAILCAUSE_SIP_BUSY,
  RIL_IMS_FAILCAUSE_SIP_REQUEST_CANCELLED,
  RIL_IMS_FAILCAUSE_SIP_NOT_ACCEPTABLE,
  RIL_IMS_FAILCAUSE_SIP_NOT_REACHABLE,
  RIL_IMS_FAILCAUSE_SIP_SERVER_INTERNAL_ERROR,
  RIL_IMS_FAILCAUSE_SIP_SERVER_NOT_IMPLEMENTED,
  RIL_IMS_FAILCAUSE_SIP_SERVER_BAD_GATEWAY,
  RIL_IMS_FAILCAUSE_SIP_SERVICE_UNAVAILABLE,
  RIL_IMS_FAILCAUSE_SIP_SERVER_TIMEOUT,
  RIL_IMS_FAILCAUSE_SIP_SERVER_VERSION_UNSUPPORTED,
  RIL_IMS_FAILCAUSE_SIP_SERVER_MESSAGE_TOOLARGE,
  RIL_IMS_FAILCAUSE_SIP_SERVER_PRECONDITION_FAILURE,
  RIL_IMS_FAILCAUSE_SIP_USER_REJECTED,
  RIL_IMS_FAILCAUSE_SIP_GLOBAL_ERROR,
  RIL_IMS_FAILCAUSE_MEDIA_INIT_FAILED,
  RIL_IMS_FAILCAUSE_MEDIA_NO_DATA,
  RIL_IMS_FAILCAUSE_MEDIA_NOT_ACCEPTABLE,
  RIL_IMS_FAILCAUSE_MEDIA_UNSPECIFIED_ERROR,
  RIL_IMS_FAILCAUSE_HOLD_RESUME_FAILED,
  RIL_IMS_FAILCAUSE_HOLD_RESUME_CANCELED,
  RIL_IMS_FAILCAUSE_HOLD_REINVITE_COLLISION,
  RIL_IMS_FAILCAUSE_DIAL_MODIFIED_TO_DIAL_VIDEO,       /* STK Call Control */
  RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_DIAL,       /* STK Call Control */
  RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_DIAL_VIDEO, /* STK Call Control */
  RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_SS,         /* STK Call Control */
  RIL_IMS_FAILCAUSE_DIAL_VIDEO_MODIFIED_TO_USSD,       /* STK Call Control */
  RIL_IMS_FAILCAUSE_SIP_ALTERNATE_EMERGENCY_CALL,
  RIL_IMS_FAILCAUSE_NO_CSFB_IN_CS_ROAM,
  RIL_IMS_FAILCAUSE_SRV_NOT_REGISTERED,
  RIL_IMS_FAILCAUSE_CALL_TYPE_NOT_ALLOWED,
  RIL_IMS_FAILCAUSE_EMRG_CALL_ONGOING,
  RIL_IMS_FAILCAUSE_CALL_SETUP_ONGOING,
  RIL_IMS_FAILCAUSE_MAX_CALL_LIMIT_REACHED,
  RIL_IMS_FAILCAUSE_UNSUPPORTED_SIP_HDRS,
  RIL_IMS_FAILCAUSE_CALL_TRANSFER_ONGOING,
  RIL_IMS_FAILCAUSE_PRACK_TIMEOUT,
  RIL_IMS_FAILCAUSE_QOS_FAILURE,
  RIL_IMS_FAILCAUSE_ONGOING_HANDOVER,
  RIL_IMS_FAILCAUSE_VT_WITH_TTY_NOT_ALLOWED,
  RIL_IMS_FAILCAUSE_CALL_UPGRADE_ONGOING,
  RIL_IMS_FAILCAUSE_CONFERENCE_WITH_TTY_NOT_ALLOWED,
  RIL_IMS_FAILCAUSE_CALL_CONFERENCE_ONGOING,
  RIL_IMS_FAILCAUSE_VT_WITH_AVPF_NOT_ALLOWED,
  RIL_IMS_FAILCAUSE_ENCRYPTION_CALL_ONGOING,
  RIL_IMS_FAILCAUSE_CALL_ONGOING_CW_DISABLED,
  RIL_IMS_FAILCAUSE_CALL_ON_OTHER_SUB,
  RIL_IMS_FAILCAUSE_ONE_X_COLLISION,
  RIL_IMS_FAILCAUSE_UI_NOT_READY,
  RIL_IMS_FAILCAUSE_CS_CALL_ONGOING,
  RIL_IMS_FAILCAUSE_SIP_USER_MARKED_UNWANTED,
  RIL_IMS_FAILCAUSE_REJECTED_ELSEWHERE,
  RIL_IMS_FAILCAUSE_USER_REJECTED_SESSION_MODIFICATION,
  RIL_IMS_FAILCAUSE_USER_CANCELLED_SESSION_MODIFICATION,
  RIL_IMS_FAILCAUSE_SESSION_MODIFICATION_FAILED,
  RIL_IMS_FAILCAUSE_SIP_METHOD_NOT_ALLOWED,
  RIL_IMS_FAILCAUSE_SIP_PROXY_AUTHENTICATION_REQUIRED,
  RIL_IMS_FAILCAUSE_SIP_REQUEST_ENTITY_TOO_LARGE,
  RIL_IMS_FAILCAUSE_SIP_REQUEST_URI_TOO_LARGE,
  RIL_IMS_FAILCAUSE_SIP_EXTENSION_REQUIRED,
  RIL_IMS_FAILCAUSE_SIP_INTERVAL_TOO_BRIEF,
  RIL_IMS_FAILCAUSE_SIP_CALL_OR_TRANS_DOES_NOT_EXIST,
  RIL_IMS_FAILCAUSE_SIP_LOOP_DETECTED,
  RIL_IMS_FAILCAUSE_SIP_TOO_MANY_HOPS,
  RIL_IMS_FAILCAUSE_SIP_AMBIGUOUS,
  RIL_IMS_FAILCAUSE_SIP_REQUEST_PENDING,
  RIL_IMS_FAILCAUSE_SIP_UNDECIPHERABLE,
  RIL_IMS_FAILCAUSE_RETRY_ON_IMS_WITHOUT_RTT,

  /* OEM specific error codes. Used to distinguish error from
   * ERROR_UNSPECIFIED and help assist debugging */
  RIL_IMS_FAILCAUSE_OEM_CAUSE_1 = 0xf001,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_2 = 0xf002,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_3 = 0xf003,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_4 = 0xf004,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_5 = 0xf005,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_6 = 0xf006,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_7 = 0xf007,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_8 = 0xf008,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_9 = 0xf009,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_10 = 0xf00a,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_11 = 0xf00b,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_12 = 0xf00c,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_13 = 0xf00d,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_14 = 0xf00e,
  RIL_IMS_FAILCAUSE_OEM_CAUSE_15 = 0xf00f,
  RIL_IMS_FAILCAUSE_ERROR_UNSPECIFIED = 0xffff /* This error will be deprecated soon,
                                vendor code should make sure to map error
                                code to specific error */
};

struct RIL_IMS_SipErrorInfo {
    uint32_t errorCode;
    char     *errorString;
};
enum RIL_IMS_VerificationStatus {
  RIL_IMS_VERSTAT_UNKNOWN,
  RIL_IMS_VERSTAT_NONE, /* Telephone number is not validated. */
  RIL_IMS_VERSTAT_PASS, /* Telephone number validation passed. */
  RIL_IMS_VERSTAT_FAIL, /* Telephone number validation failed. */
};
struct RIL_IMS_CallFailCauseResponse {
    RIL_IMS_CallFailCause   failCause;
    uint32_t                extendedFailCause;
    size_t                  errorInfoLen;
    uint8_t*                errorInfo;
    char                    *networkErrorString;
    RIL_IMS_SipErrorInfo    *errorDetails;

};


struct RIL_IMS_VerstatInfo {
    uint8_t canMarkUnwantedCall;
    RIL_IMS_VerificationStatus verificationStatus;
};

/**
 * Notification Type of Call Progress Info
 */
enum RIL_IMS_CallProgressInfoType {
  RIL_IMS_CALL_PROGRESS_INFO_UNKNOWN,       /** Invalid if CallProgressInfo is not present. */
  RIL_IMS_CALL_PROGRESS_INFO_CALL_REJ_Q850, /** MO call will be rejected with protocol Q850 error */
  RIL_IMS_CALL_PROGRESS_INFO_CALL_WAITING, /** There is already an ACTIVE call at remote UE side and
                                              this call is a WAITING call from remote UE perspective. */
  RIL_IMS_CALL_PROGRESS_INFO_CALL_FORWARDING, /** Call forwarding is enabled at remote UE and this
                                                 call will be forwarded from remote UE perspective. */
  RIL_IMS_CALL_PROGRESS_INFO_REMOTE_AVAILABLE, /**  MT call is alerting from remote UE perspective. */
};

struct RIL_IMS_CallProgressInfo {
  RIL_IMS_CallProgressInfoType type;
  uint16_t reasonCode;
  char* reasonText;
};

enum RIL_IMS_TirMode {
  RIL_IMS_TIRMODE_UNKNOWN,
  RIL_IMS_TIRMODE_TEMPORARY,
  RIL_IMS_TIRMODE_PERMANENT,
};

struct RIL_IMS_CallInfo {
    RIL_IMS_CallState               callState;
    uint32_t                        index;
    uint32_t                        toa;
    uint8_t                         isMpty;
    uint8_t                         isMt;
    uint8_t                         als;
    uint8_t                         isVoice;
    uint8_t                         isVoicePrivacy;
    char                            *number;
    uint32_t                        numberPresentation;
    char                            *name;
    uint32_t                        namePresentation;
    char                            *redirNum;
    uint32_t                        redirNumPresentation;
    RIL_IMS_CallType                callType;
    RIL_IMS_CallDomain              callDomain;
    uint32_t                        callSubState;
    uint8_t                         isEncrypted;
    uint8_t                         isCalledPartyRinging;
    uint8_t                         isVideoConfSupported;
    char                            *historyInfo;
    uint32_t                        mediaId;
    RIL_IMS_CallModifiedCause       causeCode;
    RIL_IMS_RttMode                 rttMode;
    char                            *sipAlternateUri;
    size_t                          localAbilityLen;
    RIL_IMS_ServiceStatusInfo       *localAbility;
    size_t                          peerAbilityLen;
    RIL_IMS_ServiceStatusInfo       *peerAbility;
    RIL_IMS_CallFailCauseResponse   *callFailCauseResponse;
    char *                          terminatingNumber;
    uint8_t                         isSecondary;
    RIL_IMS_VerstatInfo             *verstatInfo;
    RIL_UUS_Info                    *uusInfo;
    RIL_AudioQuality                *audioQuality;
    uint32_t                        Codec;
    char *                          displayText;
    char *                          additionalCallInfo;
    char *                          childNumber;
    uint32_t                        emergencyServiceCategory;
    RIL_IMS_TirMode                 tirMode;
    RIL_IMS_CallProgressInfo        *callProgInfo;
};

enum RIL_IMS_IpPresentation {
    RIL_IMS_IP_PRESENTATION_NUM_ALLOWED = 0,
    RIL_IMS_IP_PRESENTATION_NUM_RESTRICTED = 1,
    RIL_IMS_IP_PRESENTATION_NUM_DEFAULT = 2,
    RIL_IMS_IP_PRESENTATION_INVALID = 3,
};

struct RIL_IMS_CallDetails {
    RIL_IMS_CallType callType;
    RIL_IMS_CallDomain callDomain;
    uint32_t extrasLength;
    char** extras;
    uint32_t localAbilityLength;
    RIL_IMS_ServiceStatusInfo* localAbility;
    uint32_t peerAbilityLength;
    RIL_IMS_ServiceStatusInfo* peerAbility;
    uint32_t callSubstate;
    uint32_t mediaId;
    uint32_t causeCode;
    RIL_IMS_RttMode rttMode;
    char* sipAlternateUri;
};

enum RIL_IMS_MultiIdentityRegistrationStatus {
    RIL_IMS_STATUS_UNKNOWN = 0,
    RIL_IMS_STATUS_DISABLE = 1,
    RIL_IMS_STATUS_ENABLE = 2,
};

enum RIL_IMS_MultiIdentityLineType {
    RIL_IMS_LINE_TYPE_UNKNOWN = 0,
    RIL_IMS_LINE_TYPE_PRIMARY = 1,
    RIL_IMS_LINE_TYPE_SECONDARY = 2,
};

struct RIL_IMS_MultiIdentityLineInfo {
    char* msisdn;
    RIL_IMS_MultiIdentityRegistrationStatus registrationStatus;
    RIL_IMS_MultiIdentityLineType lineType;
};

struct RIL_IMS_Dial {
    char* address;
    uint32_t clirMode;
    RIL_IMS_IpPresentation presentation;
    bool hasCallDetails;
    RIL_IMS_CallDetails callDetails;
    bool hasIsConferenceUri;
    bool isConferenceUri;
    bool hasIsCallPull;
    bool isCallPull;
    bool hasIsEncrypted;
    bool isEncrypted;
    RIL_IMS_MultiIdentityLineInfo multiLineInfo;
};

struct RIL_IMS_Answer {
    RIL_IMS_CallType callType;
    RIL_IMS_IpPresentation presentation;
    RIL_IMS_RttMode rttMode;
};

struct RIL_IMS_Hangup {
    int32_t connIndex;
    bool hasMultiParty;
    bool multiParty;
    char* connUri;
    uint32_t conf_id;
    bool hasFailCauseResponse;
    RIL_IMS_CallFailCauseResponse failCauseResponse;
};

enum RIL_IMS_ConfigItem {
    RIL_IMS_CONFIG_ITEM_NONE,

    /* Value for this item is: (String)
     * String consisting of 0-7 seperated
     * by comma "," e.g., "1,5,7"
     * Reference for 0-7 values:
     * Values:
     *   - 0  -- 4.75 kbps
     *   - 1  -- 5.15 kbps
     *   - 2  -- 5.9 kbps
     *   - 3  -- 6.17 kbps
     *   - 4 -- 7.4 kbps
     *   - 5 -- 7.95 kbps
     *   - 6 -- 10.2 kbps
     *   - 7 -- 12.2 kbps
     * Spec reference: RFC 4867
     */

    RIL_IMS_CONFIG_ITEM_VOCODER_AMRMODESET,
    /* Value for this Item is: (String)
     * String consisting of 0-8 seperated
     * by comma "," e.g., "2,6,8"
     * Reference for 0-8 values:
     * Values:
     *   - 0  -- 6.60 kbps
     *   - 1  -- 8.85 kbps
     *   - 2  -- 12.65 kbps
     *   - 3  -- 14.25 kbps
     *   - 4 -- 15.85 kbps
     *   - 5 -- 18.25 kbps
     *   - 6 -- 19.85 kbps
     *   - 7 -- 23.05 kbps
     *   - 8 -- 23.85 kbps
     * Spec reference: RFC 4867
     */
    RIL_IMS_CONFIG_ITEM_VOCODER_AMRWBMODESET,

    /* Value for this item is: (Int)
     * SIP Session duration, in seconds
     */
    RIL_IMS_CONFIG_ITEM_SIP_SESSION_TIMER,

    /* Value for this item is: (Int)
     * Minimum allowed value for session timer, in seconds
     */
    RIL_IMS_CONFIG_ITEM_MIN_SESSION_EXPIRY,

    /* Value for this item is: (Int)
     * SIP timer operator mode A, in seconds
     * Valid Range: 0 - 30
     * By default 6 seconds is used.
     */
    RIL_IMS_CONFIG_ITEM_CANCELLATION_TIMER,

    /* Value for this item is: (Int)
     * Ims Registration wait time when iRAT transitions from eHRPD to LTE,
     * in seconds.
     */
    RIL_IMS_CONFIG_ITEM_T_DELAY,

    /* Value for this item is: (Int)
     * Flag that allows a device to silently redial over 1xRTT.
     * If this is not included in the request, a value of TRUE
     * (i.e., enabled) is used.
     * Value:
     *   - 0 -- Disable
     *   - 1 -- Enable
     */
    RIL_IMS_CONFIG_ITEM_SILENT_REDIAL_ENABLE,

    /* Value for this item is: (Int)
     * RTT estimate, in milliseconds.
     */
    RIL_IMS_CONFIG_ITEM_SIP_T1_TIMER,

    /* Value for this item is: (Int)
     * Maximum retransmit interval, in milliseconds,
     * for non-invite requests
     * and invite responses.
     */
    RIL_IMS_CONFIG_ITEM_SIP_T2_TIMER,

    /* Value for this item is: (Int)
     * Non-invite transaction timeout timer, in milliseconds.
     */
    RIL_IMS_CONFIG_ITEM_SIP_TF_TIMER,

    /* Value for this item is: (Int)
     * Enable VoLTE Support through Client Provisioning
     * Values:
     *   - 0 -- Disable
     *   - 1 -- Enable
     */
    RIL_IMS_CONFIG_ITEM_VLT_SETTING_ENABLED,

    /* Value for this item is: (Int)
     * Enable VT Support through Client Provisioning
     * Values:
     *   - 0 -- Disable
     *   - 1 -- Enable
     */
    RIL_IMS_CONFIG_ITEM_LVC_SETTING_ENABLED,

    /* Value for this item is: (String)
     * Ims Domain Name
     */
    RIL_IMS_CONFIG_ITEM_DOMAIN_NAME,

    /* Value for this item is: (Int)
     * Sms Format.
     * Values:
     *   - 0 -- 3GPP2
     *   - 1 -- 3GPP
     */
    RIL_IMS_CONFIG_ITEM_SMS_FORMAT,

    /* Value for this item is: (Int)
     * SMS over IP network indication flag
     * Values:
     *    - 0 -- Turn off MO SMS
     *    - 1 -- Turn on MO SMS
     */
    RIL_IMS_CONFIG_ITEM_SMS_OVER_IP,

    /* Value for this item is: (Int)
     * Publish timer, in seconds, when publish is sent on
     * an IMS network using 4G Radio access technology
     */
    RIL_IMS_CONFIG_ITEM_PUBLISH_TIMER,

    /* Value for this item is: (Int)
     * Publish extended timer, in seconds,
     * when publish is sent on an ims
     * network in non-4G RAT or
     * when in Airplane powerdown mode in a 4G RAT
     */
    RIL_IMS_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED,

    /* Value for this item is: (Int)
     * Duration of time, in seconds,
     * for which the retrieved capability
     * is considered valid
     */
    RIL_IMS_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION,

    /* Value for this item is: (Int)
     * Duration of time, in seconds,
     * for which the retrieved availability
     * is considered valid
     */
    RIL_IMS_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION,

    /* Value for this item is: (Int)
     * Duration of time, in seconds,
     * between successive capability polling
     */
    RIL_IMS_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL,

    /* Value for this item is: (Int)
     * Duration, in seconds,
     * between successive publish requests
     */
    RIL_IMS_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH,

    /* Value for this item is: (Int)
     * Maximum number of entries that
     * can be kept in the list subscription
     */
    RIL_IMS_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL,

    /* Value for this item is: (Int)
     * Expiry timer value, in seconds,
     * for the list subscription req
     */
    RIL_IMS_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP,

    /* Value for this item is: (Int)
     * Gzip compression enable flag
     * Values:
     *   - 0 -- Disable
     *   - 1 -- Enable
     */
    RIL_IMS_CONFIG_ITEM_GZIP_FLAG,

    /* Value for this item is: (Int)
     * Enable presence support through client provisioning
     * Values:
     *   - 0 -- Disable
     *   - 1 -- Enable
     */
    RIL_IMS_CONFIG_ITEM_EAB_SETTING_ENABLED,

    /* Value for this item is: (Int)
     * Mobile Data Status
     * Values:
     *   - 0 -- Disable
     *   - 1 -- Enable
     */
    RIL_IMS_CONFIG_ITEM_MOBILE_DATA_ENABLED,

    /* Value for this item is: (int)
     * Wi-Fi Calling Status
     * Values:
     *   - 0 -- NOT_SUPPORTED
     *   - 1 -- WIFI_CALLING_ON
     *   - 2 -- WIFI_CALLING_OFF
     */
    RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED,

    /* Value for this item is: (Int)
     * Wi-Fi Call Roaming status.
     * Values:
     *   - 1 -- ROAMING_ENABLED
     *   - 0 -- ROAMING_DISABLED
     *
     * Note: Generic error will be returned
     * is the value is ROAMING_NOT_SUPPORTED
     */
    RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING,

    /* Value for this item is: (Int)
     * Wi-Fi Call Mode Preference
     * Values:
     *   - 0 -- WIFI_ONLY/WLAN_ONLY
     *   - 1 -- CELLULAR_PREFERRED
     *   - 2 -- WIFI_PREFERRED/WLAN_PREFERRED
     *   - 10 -- IMS_PREFERRED
     *
     * Note: Generic error will be returned
     * if the value is not any of the above.
     */
    RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_MODE,

    /* Value for this item is: (Int)
     * Flag indicating whether Discovery
     * Capability is enabled
     * Values:
     *   - 0 -- Disable
     *          Presence doesnot publish/subscribe
     *          and ignores any notification received
     *
     *   - 1 -- Enable
     *          Presence publishes/subscribes and
     *          processes any notification received
     *
     * NOTE: Not supported currently
     */
    RIL_IMS_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED,

    /* Value for this item is: (Int)
     * Emergenciy call timer
     *
     * NOTE: Not supported currently
     */
    RIL_IMS_CONFIG_ITEM_EMERGENCY_CALL_TIMER,

    /* Value for this item is: (Int)
     * When UE moves from non-Voice area to Voice
     * area, the device shall start a hysteresis timer
     * and domain selection will not notify IMS about
     * voice preference until the expiry of hysteresis
     * timer
     *
     * NOTE: Not supported currently
     */
    RIL_IMS_CONFIG_ITEM_SSAC_HYSTERESIS_TIMER,

    /* VoLTE user opted in status.
     * Value is in Integer format.
     * Opted-in (1) Opted-out (0).
     */
    RIL_IMS_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS,

    /* Proxy for Call Session Control Function(P-CSCF)
     * address for Local-BreakOut(LBO).
     * Value is in String format.
     */
    RIL_IMS_CONFIG_ITEM_LBO_PCSCF_ADDRESS,

    /* Keep Alive Enabled for SIP.
     * Value is in Integer format. On(1), OFF(0).
     */
    RIL_IMS_CONFIG_ITEM_KEEP_ALIVE_ENABLED,

    /* Registration retry Base Time value
     * in seconds.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC,

    /* Registration retry Max Time value
     * in seconds.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC,

    /* Smallest RTP port for speech codec.
     * Value is in integer format.
     */
    RIL_IMS_CONFIG_ITEM_SPEECH_START_PORT,

    /* Largest RTP port for speech code.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SPEECH_END_PORT,

    /* SIP Timer A's value in msec.
     * Timer A is the INVITE request
     * retransmit interval, UDP only.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC,

    /* SIP Timer B's value in msec.
     * Timer B is the wait time for
     * INVITE message to be acknowledged.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC,

    /* SIP Timer D's value in msec.
     * Timer D is the wait time for
     * response retransmits of the
     * invite client transactions.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC,

    /* SIP Timer E's value in msec.
     * Timer E is the value
     * Non-INVITE request
     * retransmit interval, for
     * UDP only.
     * Value in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC,

    /* SIP Timer F's value in msec
     * Timer F is the value of
     * Non-INVITE transaction
     * timeout timer.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC,

    /* SIP Timer G's value in msec.
     * Timer G is the value of INVITE
     * response retransmit interval.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC,

    /* SIP Timer H's value in msec.
     * Timer H is value of wait time
     * for ACK receipt.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC,

    /* SIP Timer I's value in msec.
     * Timer I is value of wait time for
     * ACK retransmits.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC,

    /* SIP Timer J's value in
     * msec. Timer J is the
     * value of wait time for
     * non-invite request
     * retransmission.
     * Value is in Integer
     * format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC,

    /* SIP Timer K's value in
     * msec. Timer K is the
     * value of wait time for
     * non-invite response
     * retransmits.
     * Value is in Integer
     * format.
     */
    RIL_IMS_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC,

    /* AMR WB octet aligned dynamic payload
     * type.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT,

    /* AMR WB bandwidth efficient
     * payload type.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT,

    /* AMR octet aligned dynamic payload type.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT,

    /* AMR bandwidth efficient payload
     * type.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT,


    /* DTMF WB payload type.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_DTMF_WB_PT,

    /* DTMF NB payload type.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_DTMF_NB_PT,

    /* AMR Default encoding mode.
     * Value is in Integer format.
     */
    RIL_IMS_CONFIG_ITEM_AMR_DEFAULT_MODE,

    /* SMS Public Service Identity.
     * Value is in String format.
     */
    RIL_IMS_CONFIG_ITEM_SMS_PSI,

    /* Value for this item is: (Int)
     * Video Quality values are
     *  - 0 -- Low Quality
     *  - 1 -- Medium Quality
     *  - 2 -- High Quality
     */
    RIL_IMS_CONFIG_ITEM_VIDEO_QUALITY,

    /* Value for this item is: (Int)
     * Threshold for LTE to WiFi Handover
     */
    RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE1,

    /* Value for this item is: (Int)
     * Threshold for WiFi to LTE Handover
     */
    RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE2,

    /* Value for this item is: (Int)
     * Threshold for LTE to WiFi Handover
     * and WiFi to LTE Handover
     */
    RIL_IMS_CONFIG_ITEM_THRESHOLD_LTE3,

    /* Value for this item is: (Int)
     * 1x Threshold value for comparing
     * averaged signal strength.
     */
    RIL_IMS_CONFIG_ITEM_THRESHOLD_1x,

    /* Value for this item is: (Int)
     * Threshold for LTE to WiFi Handover
     */
    RIL_IMS_CONFIG_ITEM_THRESHOLD_WIFI_A,

    /* Value for this item is: (Int)
     * Threshold for WiFi to LTE Handover
     */
    RIL_IMS_CONFIG_ITEM_THRESHOLD_WIFI_B,

    /* Value for this item is: (Int)
     * Ims handover hysteresis timer for
     * WLAN to WWAN in seconds.
     */
    RIL_IMS_CONFIG_ITEM_T_EPDG_LTE,

    /* Value for this item is: (Int)
     * Ims handover hysteresis timer for
     * WWAN to WLAN in seconds.
     */
    RIL_IMS_CONFIG_ITEM_T_EPDG_WIFI,

    /* Value for this item is: (Int)
     * Ims handover hysteresis timer for
     * WLAN to 1x in seconds.
     */
    RIL_IMS_CONFIG_ITEM_T_EPDG_1x,


    /* Value for this item is: (Int)
     * Enable VoWiFi through client
     * provisioning.
     * Values
     *   - 0 -- DISABLED
     *   - 1 -- ENABLED
     */
    RIL_IMS_CONFIG_ITEM_VWF_SETTING_ENABLED,

    /* Value for this item is: (Int)
     * Enable VICE through client
     * provisioning.
     * Values
     *   - 0 -- DISABLED
     *   - 1 -- ENABLED
     */
    RIL_IMS_CONFIG_ITEM_VCE_SETTING_ENABLED,
    /* Value for this item is: (Int)
     * Enable RTT through client
     * provisioning.
     * Values
     *   - 0 -- DISABLED
     *   - 1 -- ENABLED
     */
    RIL_IMS_CONFIG_ITEM_RTT_SETTING_ENABLED,

    /* Value for this item is: (Int)
     * Report sms app selection through
     * client provisioning.
     * Values
     *   - 0 -- SMS app selection not allowed.
     *   - 1 -- RCS app is default sms app.
     *   - 2 -- RCS app is not default sms app.
     */
    RIL_IMS_CONFIG_ITEM_SMS_APP,

    /* Value for this item is: (Int)
     * Report vvm app selection through
     * client provisioning.
     * Values
     *   - 0 -- RCS app is default vvm app.
     *   - 1 -- RCS app is not default vvm app.
     */
    RIL_IMS_CONFIG_ITEM_VVM_APP,

    /* Value for this item is: (Int)
     * Wi-Fi Call Mode Roaming Preference
     * Values:
     *   - 0 -- WIFI_ONLY/WLAN_ONLY
     *   - 1 -- CELLULAR_PREFERRED
     *   - 2 -- WIFI_PREFERRED/WLAN_PREFERRED
     *   - 10 -- IMS_PREFERRED
     *
     * Note: Generic error will be returned
     * if the value is not any of the above.
     */
    RIL_IMS_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING_MODE,

     /* Value for this item is: (Int)
      * Auto Reject Call Preference
      * Values:
      *   - 0 -- DISABLE AUTO REJECT
      *   - 1 -- AUTO REJECT
      *   - 2 -- ALLOW ALERTING
      *
      * Note: Generic error will be returned
      * if the value is not any of the above.
      */
    RIL_IMS_CONFIG_ITEM_AUTO_REJECT_CALL_ENABLED,

     /* Value for this item is: (Int)
      * Call Composer Call Preference
      * Values:
      *   - 0 -- DISABLED
      *   - 1 -- ENABLED
      *
      * Note: Generic error will be returned
      * if the value is not any of the above.
      */
    RIL_IMS_CONFIG_ITEM_CALL_COMPOSER_SETTING,

    RIL_IMS_CONFIG_ITEM_INVALID
};

enum RIL_IMS_ConfigFailureCause {
    RIL_IMS_CONFIG_FAILURE_CAUSE_NO_ERR,
    RIL_IMS_CONFIG_FAILURE_CAUSE_IMS_NOT_READY,
    RIL_IMS_CONFIG_FAILURE_CAUSE_FILE_NOT_AVAILABLE,
    RIL_IMS_CONFIG_FAILURE_CAUSE_READ_FAILED,
    RIL_IMS_CONFIG_FAILURE_CAUSE_WRITE_FAILED,
    RIL_IMS_CONFIG_FAILURE_CAUSE_OTHER_INTERNAL_ERR,
    RIL_IMS_CONFIG_FAILURE_CAUSE_INVALID
};

struct RIL_IMS_ConfigInfo {
    RIL_IMS_ConfigItem item;
    uint8_t hasBoolValue;
    uint8_t boolValue;
    uint8_t hasIntValue;
    uint32_t intValue;
    char *stringValue;
    uint8_t hasErrorCause;
    RIL_IMS_ConfigFailureCause errorCause;
};

enum RIL_IMS_CallModifyFailCause {
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_UNKNOWN,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_SUCCESS,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_RADIO_NOT_AVAILABLE,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_GENERIC_FAILURE,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REQUEST_NOT_SUPPORTED,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_CANCELLED, /* Cancelled */
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_UNUSED,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_INVALID_PARAMETER,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REJECTED_BY_REMOTE,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_IMS_DEREGISTERED,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_NETWORK_NOT_SUPPORTED,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_FAILED,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_HOLD_RESUME_CANCELED,
  RIL_IMS_CALL_MODIFY_FAIL_CAUSE_REINVITE_COLLISION,
};

struct RIL_IMS_CallModifyInfo {
  uint32_t callId;
  RIL_IMS_CallType callType;
  RIL_IMS_CallDomain callDomain;
  uint8_t hasRttMode;
  RIL_IMS_RttMode rttMode;
  uint8_t hasCallModifyFailCause;
  RIL_IMS_CallModifyFailCause callModifyFailCause;
};

enum RIL_IMS_MessageType {
  RIL_IMS_MESSAGE_TYPE_UNKNOWN,
  RIL_IMS_MESSAGE_TYPE_NONE,
  RIL_IMS_MESSAGE_TYPE_VOICE,
  RIL_IMS_MESSAGE_TYPE_VIDEO,
  RIL_IMS_MESSAGE_TYPE_FAX,
  RIL_IMS_MESSAGE_TYPE_PAGER,
  RIL_IMS_MESSAGE_TYPE_MULTIMEDIA,
  RIL_IMS_MESSAGE_TYPE_TEXT,
};

enum RIL_IMS_MessagePriority {
  RIL_IMS_MESSAGE_PRIORITY_UNKNOWN,
  RIL_IMS_MESSAGE_PRIORITY_LOW,
  RIL_IMS_MESSAGE_PRIORITY_NORMAL,
  RIL_IMS_MESSAGE_PRIORITY_URGENT,
};

struct RIL_IMS_MessageSummary {
  RIL_IMS_MessageType messageType;
  uint32_t newMessageCount;
  uint32_t oldMessageCount;
  uint32_t newUrgentMessageCount;
  uint32_t oldUrgentMessageCount;
};

struct RIL_IMS_MessageDetails {
  char *toAddress;
  char *fromAddress;
  char *subject;
  char *date;
  RIL_IMS_MessagePriority priority;
  char *messageId;
  RIL_IMS_MessageType messageType;
};

struct RIL_IMS_MessageWaitingInfo {
  size_t messageSummaryLen;
  RIL_IMS_MessageSummary *messageSummary;
  char *ueAddress;
  size_t messageDetailsLen;
  RIL_IMS_MessageDetails *messageDetails;
};

struct RIL_IMS_CallFwdTimerInfo {
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
  uint32_t timezone;
};

struct RIL_IMS_CallForwardInfo {
  uint32_t status;
  uint32_t reason;
  uint32_t serviceClass;
  uint32_t toa;
  char *number;
  uint32_t timeSeconds;
  uint8_t hasCallFwdTimerStart;
  RIL_IMS_CallFwdTimerInfo callFwdTimerStart;
  uint8_t hasCallFwdTimerEnd;
  RIL_IMS_CallFwdTimerInfo callFwdTimerEnd;
};

struct RIL_IMS_QueryCallForwardStatusInfo {
  size_t                   callForwardInfoLen;
  RIL_IMS_CallForwardInfo *callForwardInfo;
  RIL_IMS_SipErrorInfo    *errorDetails;
};

struct RIL_IMS_SetCallForwardStatus {
  uint32_t reason;
  uint8_t status;
  RIL_IMS_SipErrorInfo *errorDetails;
};

struct RIL_IMS_SetCallForwardStatusInfo {
  size_t                        setCallForwardStatusLen;
  RIL_IMS_SetCallForwardStatus *setCallForwardStatus;
  RIL_IMS_SipErrorInfo    *errorDetails;
};

struct RIL_IMS_CfData {
  int numValidIndexes; /* This gives the number of valid values in cfInfo.
                       For example if voice is forwarded to one number and data
                       is forwarded to a different one then numValidIndexes will be
                       2 indicating total number of valid values in cfInfo.
                       Similarly if all the services are forwarded to the same
                       number then the value of numValidIndexes will be 1. */

  RIL_IMS_CallForwardInfo cfInfo[NUM_SERVICE_CLASSES]; /* This is the response data
                                                      for SS request to query call
                                                      forward status. see
                                                      RIL_REQUEST_QUERY_CALL_FORWARD_STATUS */
};

struct RIL_IMS_StkCcUnsolSsResponse {
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

    RIL_IMS_CfData cfData;
  };
};

struct RIL_IMS_SuppSvcNotification {
    int     notificationType;   /*
                                 * 0 = MO intermediate result code
                                 * 1 = MT unsolicited result code
                                 */
    int     code;               /* "code1" for MO
                                   "code2" for MT. */
    int     index;              /* CUG index. */
    int     type;               /* "type" (MT only). */
    char *  number;             /* "number"
                                   (MT only, may be NULL). */
    uint32_t connId;
    char   *historyInfo;
    uint8_t hasHoldTone;
    uint8_t holdTone;
};

struct RIL_IMS_ClirInfo {
    uint32_t action;
    uint32_t presentation;
};

struct RIL_IMS_SetClirInfo {
    uint32_t action;
};

struct RIL_IMS_DeflectCallInfo {
    uint32_t connIndex;
    char *number;
};

enum RIL_IMS_TtyModeType {
    RIL_IMS_TTY_MODE_OFF,
    RIL_IMS_TTY_MODE_FULL,
    RIL_IMS_TTY_MODE_HCO,
    RIL_IMS_TTY_MODE_VCO,
    RIL_IMS_TTY_MODE_INVALID,
};

struct RIL_IMS_TtyNotifyInfo {
    RIL_IMS_TtyModeType mode;
    uint8_t userData;
};

enum RIL_IMS_ClipStatus {
    RIL_IMS_CLIP_STATUS_UNKNOWN,
    RIL_IMS_CLIP_STATUS_NOT_PROVISIONED, /* Not privisioned */
    RIL_IMS_CLIP_STATUS_PROVISIONED,     /* Provisioned */
    RIL_IMS_CLIP_STATUS_STATUS_UNKNOWN,  /* Unknown */
};

struct RIL_IMS_ClipInfo {
    RIL_IMS_ClipStatus clipStatus;
    RIL_IMS_SipErrorInfo *errorDetails;
};

enum RIL_IMS_EctType {
    RIL_IMS_ECT_TYPE_UNKNOWN,
    RIL_IMS_ECT_TYPE_BLIND_TRANSFER,
    RIL_IMS_ECT_TYPE_ASSURED_TRANSFER,
    RIL_IMS_ECT_TYPE_CONSULTATIVE_TRANSFER,
};

struct RIL_IMS_ExplicitCallTransfer {
    uint32_t callId;
    RIL_IMS_EctType ectType;
    char *targetAddress;
    uint32_t targetCallId;
};

enum RIL_IMS_ConfParticipantOperation {
  RIL_IMS_CONF_PARTICIPANT_OP_UNKNOWN,
  RIL_IMS_CONF_PARTICIPANT_OP_ADD,    /* Add */
  RIL_IMS_CONF_PARTICIPANT_OP_REMOVE, /* Remove */
};

struct RIL_IMS_ParticipantStatusInfo {
  uint32_t callId;
  RIL_IMS_ConfParticipantOperation operation;
  uint32_t sipStatus;
  char *participantUri;
  uint8_t hasIsEct;
  uint8_t isEct;
};

enum RIL_IMS_RegFailureReasonType {
  RIL_IMS_REG_FAILURE_REASON_TYPE_UNKNOWN,
  RIL_IMS_REG_FAILURE_REASON_TYPE_UNSPECIFIED,          /* Unspecified */
  RIL_IMS_REG_FAILURE_REASON_TYPE_MOBILE_IP,            /*Mobile IP */
  RIL_IMS_REG_FAILURE_REASON_TYPE_INTERNAL,             /* Internal */
  RIL_IMS_REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED, /* Call manager defined  */
  RIL_IMS_REG_FAILURE_REASON_TYPE_SPEC_DEFINED,         /* specification defined */
  RIL_IMS_REG_FAILURE_REASON_TYPE_PPP,                  /* PPP */
  RIL_IMS_REG_FAILURE_REASON_TYPE_EHRPD,                /* EHRPD */
  RIL_IMS_REG_FAILURE_REASON_TYPE_IPV6,                 /* IPv6 */
  RIL_IMS_REG_FAILURE_REASON_TYPE_IWLAN,                /* IWLAN */
  RIL_IMS_REG_FAILURE_REASON_TYPE_HANDOFF               /* Handoff */
};
enum RIL_IMS_BlockReasonType {
  RIL_IMS_BLOCK_REASON_TYPE_UNKNOWN,
  RIL_IMS_BLOCK_REASON_TYPE_PDP_FAILURE,          /* Pdp failure */
  RIL_IMS_BLOCK_REASON_TYPE_REGISTRATION_FAILURE, /* Registration failure */
  RIL_IMS_BLOCK_REASON_TYPE_HANDOVER_FAILURE,     /* Handover failure */
  RIL_IMS_BLOCK_REASON_TYPE_OTHER_FAILURE         /* Other failures */
};

struct RIL_IMS_BlockReasonDetails {
  RIL_IMS_RegFailureReasonType regFailureReasonType;
  uint8_t hasRegFailureReason;
  uint32_t regFailureReason;
};

struct RIL_IMS_BlockStatus {
  RIL_IMS_BlockReasonType blockReason;
  uint8_t hasBlockReasonDetails;
  RIL_IMS_BlockReasonDetails blockReasonDetails;
};

struct RIL_IMS_RegistrationBlockStatus {
  uint8_t hasBlockStatusOnWwan;
  RIL_IMS_BlockStatus blockStatusOnWwan;
  uint8_t hasBlockStatusOnWlan;
  RIL_IMS_BlockStatus blockStatusOnWlan;
};

struct RIL_IMS_AutoCallRejectionInfo {
  RIL_IMS_CallType callType;
  RIL_IMS_CallFailCause cause;
  uint16_t sipErrorCode;
  char* number;
};

struct RIL_IMS_UssdInfo {
  RIL_UssdModeType mode;
  RIL_IMS_SipErrorInfo* errorDetails;
};

enum RIL_IMS_VoiceInfoType {
  RIL_IMS_VOICE_INFO_SILENT, /* Voice Info silent is sent when remote party is silent on a RTT call */
  RIL_IMS_VOICE_INFO_SPEECH, /* Voice Info speech is sent when remote party starts speaking on a RTT call  */
};

enum RIL_IMS_ServiceClassStatus {
  RIL_IMS_SERVICE_CLASS_STATUS_DISABLED,
  RIL_IMS_SERVICE_CLASS_STATUS_ENABLED,
};

enum RIL_IMS_ServiceClassProvisionStatus {
  RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_NOT_PROVISIONED,
  RIL_IMS_SERVICE_CLASS_PROVISION_STATUS_PROVISIONED,
};

enum RIL_IMS_SuppSvcOperationType {
  RIL_IMS_SUPP_OPERATION_ACTIVATE,
  RIL_IMS_SUPP_OPERATION_DEACTIVATE,
  RIL_IMS_SUPP_OPERATION_QUERY,
  RIL_IMS_SUPP_OPERATION_REGISTER,
  RIL_IMS_SUPP_OPERATION_ERASURE,
};

enum RIL_IMS_FacilityType {
  RIL_IMS_FACILITY_CLIP,    /* Calling Line Identification Presentation */
  RIL_IMS_FACILITY_COLP,    /* Connected Line Identification Presentation */
  RIL_IMS_FACILITY_BAOC,    /* Bar All Outgoing Calls */
  RIL_IMS_FACILITY_BAOIC,   /* Bar All Outgoing International Calls */
  RIL_IMS_FACILITY_BAOICxH, /* Bar all Outgoing International Calls except those
                             * directed to home PLMN country
                             */
  RIL_IMS_FACILITY_BAIC,    /* Bar All Incoming Calls */
  RIL_IMS_FACILITY_BAICr,   /* Bar All Incoming Calls when Roaming outside
                             * the home PLMN country
                             */
  RIL_IMS_FACILITY_BA_ALL,  /* Bar All incoming & outgoing Calls */
  RIL_IMS_FACILITY_BA_MO,   /* Bar All Outgoing Calls */
  RIL_IMS_FACILITY_BA_MT,   /* Bar All Incming Calls */
  RIL_IMS_FACILITY_BS_MT,   /* Bar Incoming Calls from specific DN */
  RIL_IMS_FACILITY_BAICa,   /* Bar All Incoming Calls from Anonymous numbers*/
};

struct RIL_IMS_CallBarringNumbersInfo {
  RIL_IMS_ServiceClassStatus status;
  char *number;
};

struct RIL_IMS_CallBarringNumbersListInfo {
  uint32_t serviceClass;
  size_t callBarringNumbersInfoLen;
  RIL_IMS_CallBarringNumbersInfo *callBarringNumbersInfo;
};

struct RIL_IMS_SuppSvcRequest {
  RIL_IMS_SuppSvcOperationType operationType;
  RIL_IMS_FacilityType facilityType;
  RIL_IMS_CallBarringNumbersListInfo* callBarringNumbersListInfo;
  char* password;
};

struct RIL_IMS_SuppSvcResponse {
  RIL_IMS_ServiceClassStatus status;
  RIL_IMS_ServiceClassProvisionStatus provisionStatus;
  RIL_IMS_FacilityType facilityType;
  size_t callBarringNumbersListInfoLen;
  RIL_IMS_CallBarringNumbersListInfo* callBarringNumbersListInfo;
  RIL_IMS_SipErrorInfo* errorDetails;
};


struct RIL_IMS_ColrInfo {
    RIL_IMS_ServiceClassStatus status;
    RIL_IMS_ServiceClassProvisionStatus provisionStatus;
    RIL_IMS_IpPresentation presentation;
    RIL_IMS_SipErrorInfo* errorDetails;
};

struct RIL_IMS_QueryVirtualLineInfoResponse {
    char* msisdn;
    size_t numLines;
    char** virtualLines;
};

struct RIL_IMS_GeolocationInfo {
    double latitude;
    double longitude;
    char* countryCode;
    char* country;
    char* state;
    char* city;
    char* postalCode;
    char* street;
    char* houseNumber;
};

enum RIL_IMS_VowifiCallQuality {
    RIL_IMS_CALL_QUALITY_UNKNOWN,
    RIL_IMS_CALL_QUALITY_NONE,
    RIL_IMS_CALL_QUALITY_EXCELLENT,
    RIL_IMS_CALL_QUALITY_FAIR,
    RIL_IMS_CALL_QUALITY_BAD,
};

enum RIL_IMS_SmsFormat {
    RIL_IMS_SMS_FORMAT_UNKNOWN = 0,
    RIL_IMS_SMS_FORMAT_3GPP = 1,
    RIL_IMS_SMS_FORMAT_3GPP2 = 2,
};

struct RIL_IMS_SmsMessage {
    uint32_t messageRef;
    RIL_IMS_SmsFormat format;
    char* smsc;
    uint32_t smscLength;
    bool shallRetry;
    uint8_t* pdu;
    uint32_t pduLength;
};

enum RIL_IMS_SmsSendStatus {
    /** Message was sent successfully. */
    RIL_IMS_SMS_SEND_STATUS_OK,
    /** IMS provider failed to send the message and platform
        should not retry falling back to sending the message
        using the radio. */
    RIL_IMS_SMS_SEND_STATUS_ERROR,
    /** IMS provider failed to send the message and platform
        should retry again after setting TP-RD */
    RIL_IMS_SMS_SEND_STATUS_ERROR_RETRY,
    /** IMS provider failed to send the message and platform
        should retry falling back to sending the message
        using the radio. */
    RIL_IMS_SMS_SEND_STATUS_ERROR_FALLBACK,
};

struct RIL_IMS_SendSmsResponse {
    RIL_IMS_SmsSendStatus status;
    uint32_t messageRef;
};

enum RIL_IMS_SmsDeliveryStatus {
    /** Message was delivered successfully. */
    RIL_IMS_SMS_DELIVERY_STATUS_OK,
    /** Message was not delivered. */
    RIL_IMS_SMS_DELIVERY_STATUS_ERROR,
    /** Message was not delivered due to lack of memory. */
    RIL_IMS_SMS_DELIVERY_STATUS_ERROR_NO_MEMORY,
    /** Message was not delivered as the request is not supported. */
    RIL_IMS_SMS_DELIVERY_STATUS_ERROR_REQUEST_NOT_SUPPORTED,
};

struct RIL_IMS_SmsAck {
    uint32_t messageRef;
    RIL_IMS_SmsDeliveryStatus deliveryStatus;
};

struct RIL_IMS_IncomingSms {
    RIL_IMS_SmsFormat format;
    uint8_t* pdu;
    uint32_t pduLength;
    RIL_IMS_VerificationStatus verificationStatus;
};

struct RIL_IMS_SmsStatusReport {
    uint32_t messageRef;
    RIL_IMS_SmsFormat format;
    uint8_t* pdu;
    uint32_t pduLength;
};

struct RIL_IMS_CallWaitingSettings {
    bool enabled;
    uint32_t serviceClass;
};

struct RIL_IMS_QueryCallWaitingResponse {
    RIL_IMS_CallWaitingSettings callWaitingSettings;
    RIL_IMS_SipErrorInfo sipError;
};

enum RIL_IMS_HandoverType {
    RIL_IMS_HO_START,
    RIL_IMS_HO_COMPLETE_SUCCESS,
    RIL_IMS_HO_COMPLETE_FAIL,
    RIL_IMS_HO_CANCEL,
    RIL_IMS_HO_NOT_TRIGGERED,
    RIL_IMS_HO_NOT_TRIGGERED_MOBILE_DATA_OFF,
    RIL_IMS_HO_INVALID,
    RIL_IMS_HO_UNKNOWN,
};

enum RIL_IMS_HoExtraType {
    RIL_IMS_HO_XT_TYPE_LTE_TO_IWLAN_HO_FAIL,
    RIL_IMS_HO_XT_TYPE_INVALID,
};

struct RIL_IMS_HoExtra {
    RIL_IMS_HoExtraType type;
    size_t extraInfoLen;
    uint8_t *extraInfo;
};
struct RIL_IMS_HandoverInfo {
    RIL_IMS_HandoverType type;
    RIL_RadioTechnology srcTech;
    RIL_RadioTechnology targetTech;
    RIL_IMS_HoExtra *hoExtra;
    char *errorCode;
    char *errorMessage;
};

enum RIL_IMS_ConferenceCallState {
    RIL_IMS_CONF_CALL_STATE_UNKNOWN,
    RIL_IMS_CONF_CALL_STATE_FOREGROUND, /* Conference call is in forground */
    RIL_IMS_CONF_CALL_STATE_BACKGROUND, /* Conference call is in backgroudn */
    RIL_IMS_CONF_CALL_STATE_RINGING,    /* Conference call is in ringing state */
};

struct RIL_IMS_RefreshConferenceInfo {
    RIL_IMS_ConferenceCallState conferenceCallState;
    size_t confInfoUriLength;
    uint8_t *confInfoUri;
};

enum RIL_IMS_GeoLocationDataStatus {
  RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN,
  RIL_IMS_GEO_LOC_DATA_STATUS_TIMEOUT, /* Received when time out to get the Longitude and Latitude
                                          from GPS engine. */
  RIL_IMS_GEO_LOC_DATA_STATUS_NO_CIVIC_ADDRESS, /* Received when Modem does not have the relevant
                                                   information (ex: country code) to register */
  RIL_IMS_GEO_LOC_DATA_STATUS_ENGINE_LOCK,      /* Received if GPS is disabled by user */
  RIL_IMS_GEO_LOC_DATA_STATUS_RESOLVED,         /* Received when there are no GPS errors */
};

/* IMS Requests */
#define RIL_IMS_REQUEST_BASE 10000

/**
 * RIL_IMS_REQUEST_GET_REGISTRATION_STATE
 *
 * Request current IMS registration state
 *
 * "data" is NULL
 *
 * "response" is RIL_IMS_Registration
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_GET_REGISTRATION_STATE 10001

/**
 * RIL_IMS_REQUEST_DIAL
 *
 * Initiate voice call
 *
 * "data" is RIL_IMS_Dial
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_DIAL 10002

/**
 * RIL_IMS_REQUEST_ANSWER
 *
 * Answer incoming call
 *
 * "data" is RIL_IMS_Answer
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_ANSWER 10003

/**
 * RIL_IMS_REQUEST_HANGUP
 *
 * Hang up a specific line
 *
 * "data" is RIL_IMS_Hangup
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_HANGUP 10004
#define RIL_IMS_REQUEST_CONFERENCE 10005
#define RIL_IMS_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE 10006

/**
 * RIL_IMS_REQUEST_EXPLICIT_CALL_TRANSFER
 *
 * For transfering the call to a third party.
 * The call can be transferred to third party (Transfer target) by
 * passing the transfer target address (blind or assured transfer)
 * or by passing the call id of the already established call with
 * transfer target (consultative transfer).
 * In success case, the call/calls will be disconnected.
 *
 * "data" is RIL_IMS_ExplicitCallTransfer
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_EXPLICIT_CALL_TRANSFER 10007

/**
 * RIL_IMS_REQUEST_DTMF
 *
 * Send a DTMF tone
 *
 * If the implementation is currently playing a tone requested via
 * RIL_IMS_REQUEST_DTMF_START, that tone should be cancelled and the new tone
 * should be played instead
 *
 * "data" is a char containing a single character with one of 12 values: 0-9,*,#
 * "response" is NULL
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
 * See also: RIL_IMS_REQUEST_DTMF_STOP, RIL_IMS_REQUEST_DTMF_START
 *
 */
#define RIL_IMS_REQUEST_DTMF 10008

/**
 * RIL_IMS_REQUEST_DTMF_START
 *
 * Start playing a DTMF tone. Continue playing DTMF tone until
 * RIL_IMS_REQUEST_DTMF_STOP is received
 *
 * If a RIL_IMS_REQUEST_DTMF_START is received while a tone is currently playing,
 * it should cancel the previous tone and play the new one.
 *
 * "data" is a char
 * "data" is a single character with one of 12 values: 0-9,*,#
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
 * See also: RIL_IMS_REQUEST_DTMF, RIL_IMS_REQUEST_DTMF_STOP
 */
#define RIL_IMS_REQUEST_DTMF_START 10009

/**
 * RIL_IMS_REQUEST_DTMF_STOP
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
 * See also: RIL_IMS_REQUEST_DTMF, RIL_IMS_REQUEST_DTMF_START
 */
#define RIL_IMS_REQUEST_DTMF_STOP 10010

/**
 * RIL_IMS_REQUEST_MODIFY_CALL_INITIATE
 *
 * Request to upgrade/downgrade existing IMS calls
 *
 * "data" is RIL_IMS_CallModifyInfo
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_MODIFY_CALL_INITIATE 10011

/**
 * RIL_IMS_REQUEST_MODIFY_CALL_CONFIRM
 *
 * Request to accept the call upgrade of existing IMS calls
 *
 * "data" is RIL_IMS_CallModifyInfo
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_MODIFY_CALL_CONFIRM 10012

/**
 * RIL_IMS_REQUEST_QUERY_CLIP
 *
 * Queries the status of the CLIP supplementary service
 * (for MMI code "*#30#")
 * "data" is NULL
 * "response" is RIL_IMS_ClipInfo
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *  FDN_CHECK_FAILURE
 */
#define RIL_IMS_REQUEST_QUERY_CLIP 10013

/**
 * RIL_IMS_REQUEST_GET_CLIR
 *
 * Gets current CLIR status
 *
 * "data" is NULL
 * "response" is RIL_IMS_ClirInfo
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *  FDN_CHECK_FAILURE
 */
#define RIL_IMS_REQUEST_GET_CLIR 10014

/**
 * RIL_IMS_REQUEST_SET_CLIR
 *
 * Sets the Clir parameter
 *
 * "data" is RIL_IMS_SETClirInfo
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *  FDN_CHECK_FAILURE
 */
#define RIL_IMS_REQUEST_SET_CLIR 10015

/**
 * RIL_IMS_REQUEST_QUERY_CALL_FORWARD_STATUS
 *
 * Requests the call forward settings
 *
 * "data" is RIL_IMS_CallForwardInfo
 *
 * "response" is RIL_IMS_QueryCallForwardStatusInfo
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_QUERY_CALL_FORWARD_STATUS 10016

/**
 * RIL_IMS_REQUEST_SET_CALL_FORWARD_STATUS
 *
 * Configure call forward rule
 *
 * "data" is RIL_IMS_CallForwardInfo
 *
 * "response" is RIL_IMS_SetCallForwardStatusInfo
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_SET_CALL_FORWARD_STATUS 10017
#define RIL_IMS_REQUEST_QUERY_CALL_WAITING 10018
#define RIL_IMS_REQUEST_SET_CALL_WAITING 10019

/**
 * RIL_IMS_REQUEST_REGISTRATION_CHANGE
 *
 * Request current IMS Registration/Deregistration
 *
 * "data" is RIL_IMS_RegistrationState
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_REGISTRATION_CHANGE 10020

/**
 * RIL_IMS_REQUEST_SET_SUPP_SVC_NOTIFICATION
 *
 * Enables/disables supplementary service related notifications
 * from the network.
 *
 * Notifications are reported via RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION.
 *
 * "data" is int *
 * ((int *)data)[0] is == 1 for notifications enabled
 * ((int *)data)[0] is == 0 for notifications disabled
 *
 * "response" is NULL
 *
 * Valid errors:
 *  E_SUCCESS
 *  E_RADIO_NOT_AVAILABLE
 *  E_GENERIC_FAILURE
 *  E_FDN_CHECK_FAILURE
 *
 * See also: RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION.
*/
#define RIL_IMS_REQUEST_SET_SUPP_SVC_NOTIFICATION 10021

/**
 * RIL_IMS_REQUEST_ADD_PARTICIPANT
 *
 * Request to add a participant to a call (to the current active call or to the held call if there
 * is no active calls).
 *
 * "data" is "char *" containing address
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_ADD_PARTICIPANT 10023

/**
 * RIL_IMS_REQUEST_QUERY_SERVICE_STATUS
 *
 * Request to get the status of IMS Services
 *
 * "data" is NULL
 *
 * "response" is "const RIL_IMS_ServiceStatusInfo **"; all services must be specified in the response
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_QUERY_SERVICE_STATUS 10024

/**
 * RIL_IMS_REQUEST_SET_SERVICE_STATUS
 *
 * Request to set the status of an IMS Service
 *
 * "data" is "const RIL_IMS_ServiceStatusInfo **"
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_SET_SERVICE_STATUS 10025

/**
 * RIL_IMS_REQUEST_SUPP_SVC_STATUS
 *
 * Request to activate/deactivate/query the supplementary service of an IMS Service
 * This is currently being used for Call Barring, CLIP, COLP
 *
 * "data" is RIL_IMS_SuppSvcRequest
 * "response" is RIL_IMS_SuppSvcResponse
 *
 * Valid errors:
 *  SUCCESS
 *  RADIO_NOT_AVAILABLE
 *  GENERIC_FAILURE
 *  FDN_CHECK_FAILURE
 */
#define RIL_IMS_REQUEST_SUPP_SVC_STATUS 10026

/**
 * RIL_IMS_REQUEST_DEFLECT_CALL
 *
 * deflect the call to specific number mentioned by user
 *
 * "data" is "RIL_IMS_DeflectCallInfo"
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_DEFLECT_CALL 10027
#define RIL_IMS_REQUEST_GET_COLR 10028
#define RIL_IMS_REQUEST_SET_COLR 10029
#define RIL_IMS_REQUEST_HOLD 10030
#define RIL_IMS_REQUEST_RESUME 10031

/**
 * RIL_IMS_REQUEST_SEND_UI_TTY_MODE
 *
 * deflect the call to specific number mentioned by user
 *
 * "data" is "RIL_IMS_TtyNotifyInfo"
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_SEND_UI_TTY_MODE 10032

/**
 * RIL_IMS_REQUEST_SET_IMS_CONFIG
 *
 * Sets value for Ims Config item
 *
 * "data" is "RIL_IMS_ConfigInfo"
 *
 * "response" is "RIL_IMS_ConfigInfo"
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_SET_IMS_CONFIG 10033

/**
 * RIL_IMS_REQUEST_GET_IMS_CONFIG
 *
 * Sets value for Ims Config item
 *
 * "data" is "RIL_IMS_ConfigInfo"
 *
 * "response" is "RIL_IMS_ConfigInfo"
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_GET_IMS_CONFIG 10034

/**
 * RIL_IMS_REQUEST_SEND_GEOLOCATION_INFO
 * MsgType: REQUEST
 *
 * Send GeoLocation Address info.
 *
 * "data" is RIL_IMS_GeolocationInfo
 *
 * "response" is null
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_SEND_GEOLOCATION_INFO 10035

/**
 * RIL_IMS_REQUEST_GET_IMS_SUB_CONFIG
 *
 * Get IMS Subscription specific configuration
 *
 * "data" is NULL
 *
 * "response" is RIL_IMS_SubConfigInfo
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_GET_IMS_SUB_CONFIG 10036

/**
 * RIL_IMS_REQUEST_SEND_RTT_MSG
 * MsgType: REQUEST
 *
 * Request to send the RTT message
 *
 * "data" is message char*
 *
 * "response" is null
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_SEND_RTT_MSG 10037

/**
 * RIL_IMS_REQUEST_CANCEL_MODIFY_CALL
 *
 * Cancel call modify request
 *
 * "data" is uint32_t
 *
 * "response" is NULL
 *
 * Valid errors:
 *  SUCCESS
 *  GENERIC_FAILURE
 */
#define RIL_IMS_REQUEST_CANCEL_MODIFY_CALL 10038
#define RIL_IMS_REQUEST_SEND_SMS 10039
#define RIL_IMS_REQUEST_ACK_SMS 10040
#define RIL_IMS_REQUEST_ACK_SMS_REPORT 10041
#define RIL_IMS_REQUEST_GET_SMS_FORMAT 10042
#define RIL_IMS_REQUEST_REGISTER_MULTI_IDENTITY_LINES 10043
#define RIL_IMS_REQUEST_QUERY_VIRTUAL_LINE_INFO 10044

/* IMS Unsolicited Indications */

#define RIL_IMS_UNSOL_BASE 15000

/**
 * RIL_IMS_UNSOL_CALL_STATE_CHANGED
 *
 * Indicate when call state has changed
 *
 * "data" is RIL_IMS_CallInfo*
 */
#define RIL_IMS_UNSOL_CALL_STATE_CHANGED 15001

/**
 * RIL_IMS_UNSOL_CALL_RING
 *
 * Ring indication for an incoming call
 *
 * "data" is NULL
 */
#define RIL_IMS_UNSOL_CALL_RING 15002

/**
 * RIL_IMS_UNSOL_RINGBACK_TONE
 *
 * Indicates that nework doesn't have in-band information, need to play out-band tone.
 *
 * "data" is RIL_IMS_ToneOperation
 */
#define RIL_IMS_UNSOL_RINGBACK_TONE 15003

/**
 * RIL_IMS_UNSOL_REGISTRATION_STATE_CHANGED
 *
 * Notification for change in ims registration state
 * Registration failures reported through this API are temporary.
 *
 * "data" is RIL_IMS_Registration
 */
#define RIL_IMS_UNSOL_REGISTRATION_STATE_CHANGED 15004

/**
 * RIL_IMS_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE
 *
 * Indicates that the radio system selection module has
 * autonomously entered emergency callback mode.
 *
 * "data" is NULL
 */
#define RIL_IMS_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE 15005

/**
 * RIL_IMS_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE
 *
 * Called when Emergency Callback Mode Ends.
 * Indicates that the radio system selection module has
 * proactively exited emergency callback mode.
 *
 * "data" is NULL
 */
#define RIL_IMS_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE 15006

/**
 * RIL_IMS_UNSOL_MODIFY_CALL
 *
 * Unsol message to notify that an upgrade of a call was triggered from the remote party
 *
 * "data" is RIL_IMS_CallModifyInfo
 */
#define RIL_IMS_UNSOL_MODIFY_CALL 15007

/**
 * RIL_IMS_UNSOL_HANDOVER
 *
 * Unsol message to notify that a call has been handed over between technologies
 *
 * "data" is RIL_IMS_HandoverInfo
 */
#define RIL_IMS_UNSOL_HANDOVER 15008

/**
 * RIL_IMS_UNSOL_REFRESH_CONF_INFO
 *
 * Unsol message to notify conference status updates
 *
 * "data" is RIL_IMS_RefreshConferenceInfo
 */
#define RIL_IMS_UNSOL_REFRESH_CONF_INFO 15009

/**
 * RIL_IMS_UNSOL_SERVICE_STATUS_CHANGED
 *
 * Status of services supported on IMS network.
 *
 * "data" is "const RIL_IMS_ServiceStatusInfo **"
 */
#define RIL_IMS_UNSOL_SERVICE_STATUS_CHANGED 15010

/**
 * RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION
 *
 * Reports supplementary service related notification from the network.
 *
 * "data" is RIL_IMS_SuppSvcNotification
 */
#define RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION 15011
#define RIL_IMS_UNSOL_TTY_NOTIFICATION 15012
#define RIL_IMS_UNSOL_RADIO_STATE_CHANGED 15013

/**
 * RIL_IMS_UNSOL_MESSAGE_WAITING
 *
 * Unsol message to notify clients about message waiting indication
 *
 * "data" is RIL_IMS_MessageWaitingInfo
 */
#define RIL_IMS_UNSOL_MESSAGE_WAITING 15014
#define RIL_IMS_UNSOL_REFRESH_VICE_INFO 15015

/**
* RIL_IMS_UNSOL_REQUEST_GEOLOCATION
*
* Request from lower layers for GeoLocation address
* information.
*
* 'request' is latitude (double), longitude (double)
*/
#define RIL_IMS_UNSOL_REQUEST_GEOLOCATION 15016

/**
* UNSOL_VOWIFI_CALL_QUALITY
*
* Notification for VoWiFi call quality information
*
* "response" is message RIL_IMS_VowifiCallQuality
*/
#define RIL_IMS_UNSOL_VOWIFI_CALL_QUALITY 15017

/**
 * RIL_IMS_UNSOL_VOPS_CHANGED
 *
 * Unsol message to notify clients about vops change indication
 *
 * "data" is uint8_t
 */
#define RIL_IMS_UNSOL_VOPS_CHANGED 15018

/*
 * RIL_IMS_UNSOL_PARTICIPANT_STATUS_INFO
 *
 * Notification for participant status information
 *
 * "data" is RIL_IMS_ParticipantStatusInfo
 */
#define RIL_IMS_UNSOL_PARTICIPANT_STATUS_INFO 15019

/**
 * RIL_IMS_UNSOL_IMS_SUB_CONFIG_CHANGED
 *
 * Notification for IMS Subscription configuration changes
 *
 * "data" is RIL_IMS_SubConfigInfo
 */
#define RIL_IMS_UNSOL_IMS_SUB_CONFIG_CHANGED 15020

/*
 * RIL_IMS_UNSOL_REGISTRATION_BLOCK_STATUS
 *
 * Notification for permanent ims registration failure.
 * Permanent registration failure happens when modem has exhausted all its
 * retries. At this point modem will stop attempting to register until user
 * toggles the registration state or network reports favorable conditions for
 * registration.
 *
 * "data" is RIL_IMS_RegistrationBlockStatus
 */
#define RIL_IMS_UNSOL_REGISTRATION_BLOCK_STATUS 15021

/**
* RIL_IMS_UNSOL_RTT_MSG_RECEIVED
*
* Notification for received RTT message
*
* 'response' is message RttMessage (string)
*/
#define RIL_IMS_UNSOL_RTT_MSG_RECEIVED 15022

/**
 * RIL_IMS_UNSOL_ON_SS
 *
 * Called when SS response is received when DIAL/USSD/SS is changed to SS by
 * call control.
 *
 * "data" is RIL_IMS_StkCcUnsolSsResponse
 */
#define RIL_IMS_UNSOL_ON_SS 15023
#define RIL_IMS_UNSOL_SMS_STATUS_REPORT 15023

/**
 * RIL_IMS_UNSOL_AUTO_CALL_REJECTION_IND
 *
 * Notifies clients about a MT call being automatically rejected by the modem.
 *
 * "data" is RIL_IMS_AutoCallRejectionInfo
 */
#define RIL_IMS_UNSOL_AUTO_CALL_REJECTION_IND 15024

/**
 * RIL_IMS_UNSOL_VOICE_INFO
 *
 * Sends updates for the RTT voice info which indicates whether there is speech or silence
 * from remote user
 *
 * "data" is enum RIL_IMS_VoiceInfoType
 */
#define RIL_IMS_UNSOL_VOICE_INFO 15025

/**
 * RIL_IMS_UNSOL_MULTI_IDENTITY_REGISTRATION_STATUS
 *
 * Sends the registration status of Multi-line identity
 *
 * "data" is RIL_IMS_MultiIdentityLineInfo *
 */
#define RIL_IMS_UNSOL_MULTI_IDENTITY_REGISTRATION_STATUS 15026
#define RIL_IMS_UNSOL_MULTI_IDENTITY_PENDING_INFO 15027

/**
 * RIL_IMS_UNSOL_WFC_ROAMING_MODE_CONFIG_SUPPORT
 *
 * Indicates if modem supports WFC roaming mode configuration
 *
 * "data" is uint8_t
 */
#define RIL_IMS_UNSOL_WFC_ROAMING_MODE_CONFIG_SUPPORT 15028
#define RIL_IMS_UNSOL_INCOMING_IMS_SMS 15029

/**
 * RIL_IMS_UNSOL_USSD_FAILED
 *
 * Indicates if ussd message failed over IMS
 *
 * "data" is RIL_IMS_UssdInfo
 */
#define RIL_IMS_UNSOL_USSD_FAILED 15030

/**
 * RIL_IMS_UNSOL_RETRIEVING_GEOLOCATION_DATA_STATUS
 *
 * Indicates geo location retrival status in modem.
 * Modem needs geo location to register IMS over standalone WIFI.
 *
 * "data" is RIL_IMS_GeoLocationDataStatus
 */
#define RIL_IMS_UNSOL_RETRIEVING_GEOLOCATION_DATA_STATUS 15031

#endif /*ANDROID_RIL_IMS_H*/
