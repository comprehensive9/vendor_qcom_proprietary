/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "interfaces/common.h"
#include <string>
#include <vector>

namespace qcril {
namespace interfaces {

enum class NasSettingResultCode {
    SUCCESS = RIL_E_SUCCESS,
    FAILURE = RIL_E_INTERNAL_ERR,
};

struct RilGetVoiceRegResult_t : public qcril::interfaces::BasePayload {
  RIL_VoiceRegistrationStateResponse respData;
};

struct RilGetDataRegResult_t : public qcril::interfaces::BasePayload {
  RIL_DataRegistrationStateResponse respData;
};

struct RilGetOperatorResult_t: public qcril::interfaces::BasePayload {
  std::string longName;
  std::string shortName;
  std::string numeric;
  template<typename T1, typename T2, typename T3>
  inline RilGetOperatorResult_t(T1&& _longName, T2&& _shortName, T3&& _numeric):
        longName(std::forward<T1>(_longName)), shortName(std::forward<T2>(_shortName)),
        numeric(std::forward<T3>(_numeric)) {}
};

struct RilGetSelectModeResult_t: public qcril::interfaces::BasePayload {
  bool bManual;
  explicit inline RilGetSelectModeResult_t(bool manual) : bManual(manual) {}
};

enum class NetworkStatus {UNKNOWN, AVAILABLE, CURRENT, FORBIDDEN};
struct NetworkInfo {
  std::string alphaLong;
  std::string alphaShort;
  std::string operatorNumeric;
  NetworkStatus status;
};
struct RilGetAvailNetworkResult_t: public qcril::interfaces::BasePayload {
  std::vector<NetworkInfo> info;
  template<typename T>
  inline RilGetAvailNetworkResult_t(T _info):
        info(std::forward<T>(_info)) {}
};

struct RilGetModePrefResult_t: public qcril::interfaces::BasePayload {
  int pref;
  inline RilGetModePrefResult_t(int value): pref(value) {}
};

struct RilGetSignalStrengthResult_t: public qcril::interfaces::BasePayload {
  RIL_SignalStrength respData;
};

struct RilGetVoiceTechResult_t: public qcril::interfaces::BasePayload {
  int rat;
  inline RilGetVoiceTechResult_t(int value): rat(value) {}
};

struct RilGetRadioAccessFamilyResult_t: public qcril::interfaces::BasePayload {
  int32_t radioAccessFamily;
  inline RilGetRadioAccessFamilyResult_t(int32_t raf): radioAccessFamily(raf) {}
};

struct RilNeighboringCell_t {
  std::string cid;
  int rssi;
};

struct RilGetNeighborCellIdResult_t: public qcril::interfaces::BasePayload {
  std::vector<RilNeighboringCell_t> cellList;
  template<typename T>
  inline RilGetNeighborCellIdResult_t(T list):
        cellList(std::forward<T>(list)) {}
};

struct RilGetCdmaSubscriptionResult_t: public qcril::interfaces::BasePayload {
  std::string mdn;
  std::string hSid;
  std::string hNid;
  std::string min;
  std::string prl;

  template<typename T1, typename T2, typename T3, typename T4, typename T5>
  inline RilGetCdmaSubscriptionResult_t(T1 _mdn, T2 _hSid, T3 _hNid, T4 _min, T5 _prl):
    mdn(std::forward<T1>(_mdn)), hSid(std::forward<T2>(_hSid)),
    hNid(std::forward<T3>(_hNid)), min(std::forward<T4>(_min)),
    prl(std::forward<T5>(_prl)) {}
};

struct RilQueryAvailBandsResult_t: public qcril::interfaces::BasePayload {
  std::vector<int> bandList;
  template<typename T>
  inline RilQueryAvailBandsResult_t(T list):
    bandList(std::forward<T>(list)) {}
};

struct RilRadioCapResult_t: public qcril::interfaces::BasePayload {
  RIL_RadioCapability respData;
};

struct RilGetModemStackResult_t: public qcril::interfaces::BasePayload {
    bool state;
    inline RilGetModemStackResult_t(bool stackstate): state(stackstate) {}
};

struct RilGetUiccAppStatusResult_t: public qcril::interfaces::BasePayload {
    bool state;
    inline RilGetUiccAppStatusResult_t(bool enable): state(enable) {}
};

enum class RIL_EccNumberSourceMask {
    DEFAULT = 1 << 0,
    NETWORK_SIGNALING = 1 << 1,
    SIM = 1 << 2,
    MODEM_CONFIG = 1 << 3
};

struct RIL_EmergencyNumber_t: public qcril::interfaces::BasePayload {
    std::string number;
    std::string mcc;
    std::string mnc;
    uint64_t categories;
    vector<std::string> urns;
    RIL_EccNumberSourceMask sources;
};

typedef std::vector<std::shared_ptr<qcril::interfaces::RIL_EmergencyNumber_t>> RIL_EccList;
typedef std::unordered_map<std::string, std::shared_ptr<RIL_EmergencyNumber_t>> RIL_EccMapType;

typedef struct {
    uint8_t modemId;
} RIL_ModemInfo;

typedef struct {
    uint8_t maxActiveData;
    uint8_t maxActiveInternetData;
    bool isInternetLingeringSupported;
    std::vector<RIL_ModemInfo> logicalModemList;
} RIL_PhoneCapability;

struct RilPhoneCapabilityResult_t: public qcril::interfaces::BasePayload {
    RIL_PhoneCapability phoneCap;
    inline RilPhoneCapabilityResult_t(RIL_PhoneCapability pc): phoneCap(pc) {}
};

struct RilQuery5GStatusResult_t: public qcril::interfaces::BasePayload {
  five_g_status status;
  inline RilQuery5GStatusResult_t(five_g_status _status): status(_status) {}
};

struct RilQueryEndcStatusResult_t: public qcril::interfaces::BasePayload {
  endc_status status;
  inline RilQueryEndcStatusResult_t(endc_status _status): status(_status) {}
};

struct RilQueryNrDcParamResult_t: public qcril::interfaces::BasePayload {
  five_g_endc_dcnr dc;
  inline RilQueryNrDcParamResult_t(five_g_endc_dcnr _dc): dc(_dc) {}
};

struct RilQueryNrBearAllocResult_t: public qcril::interfaces::BasePayload {
  five_g_bearer_status status;
  inline RilQueryNrBearAllocResult_t(five_g_bearer_status _status):
        status(_status) {}
};

struct RilQueryNrSignalStrengthResult_t: public qcril::interfaces::BasePayload {
  five_g_signal_strength signal;
  inline RilQueryNrSignalStrengthResult_t(five_g_signal_strength _signal):
        signal(_signal) {}
};

struct RilQueryUpperLayerIndInfoResult_t: public qcril::interfaces::BasePayload {
  five_g_upper_layer_ind_info upli_info;
  inline RilQueryUpperLayerIndInfoResult_t(five_g_upper_layer_ind_info _upli_info):
        upli_info(_upli_info) {}
};

struct RilQuery5gConfigInfoResult_t: public qcril::interfaces::BasePayload {
  five_g_config_type config_info;
  inline RilQuery5gConfigInfoResult_t(five_g_config_type _config_info):
        config_info(_config_info) {}
};

enum class ServiceDomain {
    UNSPECIFIED = 0,
    CS_DOMAIN = 1 /* (1 << 0) */,
    PS_DOMAIN = 2 /* (1 << 1) */,
};

struct RilNwRegistrationRejectInfo_t: public qcril::interfaces::BasePayload {
  bool causeCode_valid;
  int32_t causeCode;
  bool additionalCauseCode_valid;
  uint64_t domain;
  int32_t additionalCauseCode;
  char choosenPlmn[MAX_MCC_MNC_LEN+1];
  bool rilCellIdentity_valid;
  RIL_CellIdentity_v16 rilCellIdentity;
};

struct RilQueryCdmaRoamingPrefResult_t : public qcril::interfaces::BasePayload {
  int mPrefType;
  inline RilQueryCdmaRoamingPrefResult_t(int value): mPrefType(value) {}
};

struct RilGetCdmaSubscriptionSourceResult_t : public qcril::interfaces::BasePayload {
  RIL_CdmaSubscriptionSource mSource;
  inline RilGetCdmaSubscriptionSourceResult_t(RIL_CdmaSubscriptionSource value): mSource(value) {}
};

enum class RilBarringServiceType {
    /** Applicable to UTRAN */
    /** Barring for all CS services, including registration */
    CS_SERVICE,
    /** Barring for all PS services, including registration */
    PS_SERVICE,
    /** Barring for mobile-originated circuit-switched voice calls */
    CS_VOICE,

    /** Applicable to EUTRAN, NGRAN */
    /** Barring for mobile-originated signalling for any purpose */
    MO_SIGNALLING,
    /** Barring for mobile-originated internet or other interactive data */
    MO_DATA,
    /** Barring for circuit-switched fallback calling */
    CS_FALLBACK,
    /** Barring for IMS voice calling */
    MMTEL_VOICE,
    /** Barring for IMS video calling */
    MMTEL_VIDEO,

    /** Applicable to UTRAN, EUTRAN, NGRAN */
    /** Barring for emergency services, either CS or emergency MMTEL */
    EMERGENCY,
    /** Barring for short message services */
    SMS,

    /** Operator-specific barring codes; applicable to NGRAN */
    OPERATOR_1 = 1001,
    OPERATOR_2 = 1002,
    OPERATOR_3 = 1003,
    OPERATOR_4 = 1004,
    OPERATOR_5 = 1005,
    OPERATOR_6 = 1006,
    OPERATOR_7 = 1007,
    OPERATOR_8 = 1008,
    OPERATOR_9 = 1009,
    OPERATOR_10 = 1010,
    OPERATOR_11 = 1011,
    OPERATOR_12 = 1012,
    OPERATOR_13 = 1013,
    OPERATOR_14 = 1014,
    OPERATOR_15 = 1015,
    OPERATOR_16 = 1016,
    OPERATOR_17 = 1017,
    OPERATOR_18 = 1018,
    OPERATOR_19 = 1019,
    OPERATOR_20 = 1020,
    OPERATOR_21 = 1021,
    OPERATOR_22 = 1022,
    OPERATOR_23 = 1023,
    OPERATOR_24 = 1024,
    OPERATOR_25 = 1025,
    OPERATOR_26 = 1026,
    OPERATOR_27 = 1027,
    OPERATOR_28 = 1028,
    OPERATOR_29 = 1029,
    OPERATOR_30 = 1030,
    OPERATOR_31 = 1031,
    OPERATOR_32 = 1032,
};

enum class RilBarringType {
    /** Device is not barred for the given service */
    NONE,
    /** Device may be barred based on time and probability factors */
    CONDITIONAL,
    /* Device is unconditionally barred */
    UNCONDITIONAL,
};

struct RilBarringTypeSpecificInfo {
    /** The barring factor as a percentage 0-100 */
    int32_t barringFactor;

    /** The number of seconds between re-evaluations of barring */
    int32_t barringTimeSeconds;

    /**
     * Indicates whether barring is currently being applied.
     *
     * <p>True if the UE applies barring to a conditionally barred
     * service based on the conditional barring parameters.
     *
     * <p>False if the service is conditionally barred but barring
     * is not currently applied, which could be due to either the
     * barring criteria not having been evaluated (if the UE has not
     * attempted to use the service) or due to the criteria being
     * evaluated and the UE being permitted to use the service
     * despite conditional barring.
     */
    bool isBarred;
};

struct RILBarringInfo {
    RilBarringServiceType barringService;
    RilBarringType barringType;
    RilBarringTypeSpecificInfo typeSpecificInfo;
};

struct RilCellBarringInfo_t: public qcril::interfaces::BasePayload {
  std::vector<RILBarringInfo> barring_info;
  bool rilCellIdentity_valid;
  RIL_CellIdentity_v16 rilCellIdentity;
};

struct RilUnsolBarringFilterResult_t: public qcril::interfaces::BasePayload {
  int32_t mIndicationFilter;
  inline RilUnsolBarringFilterResult_t(int32_t value): mIndicationFilter(value) {}
};

struct RilQueryNrConfigResult_t: public qcril::interfaces::BasePayload {
  RIL_NR_DISABLE_MODE mode;
  inline RilQueryNrConfigResult_t(RIL_NR_DISABLE_MODE _mode): mode(_mode) {}
};
}  // namespace interfaces
}  // namespace qcril
