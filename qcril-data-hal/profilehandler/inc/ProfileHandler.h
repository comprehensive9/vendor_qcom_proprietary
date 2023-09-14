/******************************************************************************
#  Copyright (c) 2018,2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef PROFILEHANDLER
#define PROFILEHANDLER

#include "DSDModemEndPoint.h"
#include "WDSModemEndPoint.h"
#include "AttachProfile.h"
#include "modules/android/version.h"
#include "telephony/ril.h"
#include "request/RilRequestSetInitialAttachApnRequestMessage.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "sync/SetInitialAttachCompletedMessage.h"
#include "request/SetDataProfileRequestMessage.h"
#include "NasModemEndPointHelper.h"
#include "MessageCommon.h"
#include "LocalLogBuffer.h"
#include "framework/TimeKeeper.h"

#ifndef QCRIL_RIL_VERSION
#error "undefined QCRIL_RIL_VERSION!"
#endif

using namespace rildata;

class ProfileHandler
{
private:
  LocalLogBuffer& logBuffer;
  bool mApAssistMode;
  uint8_t mIARetry;
  std::shared_ptr<Message> mPendingMessage;
  TimeKeeper::timer_id mIARetryAttachTimer;
  static constexpr TimeKeeper::millisec RETRY_ATTACH_TIMEOUT = 500;
  static std::unique_ptr<NasModemEndPointHelper> nas_helper;
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  std::shared_ptr<std::list<uint16_t>> attach_list;
  AttachProfile::params_t ril_params;
  enum Request
  {
   SET_INITIAL_ATTACH,
   SET_DATA_PROFILE
  };

  /**
   * @brief      Is action supported as part of the attach procedure
   */
  enum AttachResult
  {
    NO_DETACH_NEEDED,
    DETACH_NEEDED,
    GENERIC_FAILURE
  };

  enum AttachResponse
  {
    SUCCESS,
    FAILURE,
    INPROGRESS
  };

  static RilRespParams m_resp_params;

  /**
   * @brief      Common implementation agnostic of the version of RIL
   *
   * @return     { description_of_the_return_value }
   */
  bool setAttachListCommon();
  /**
   * @brief      If current profile is in profiles found from attach list, then no
   *             attach required.
   *
   * @param[in]  attach_list  The attach list
   *
   * @return     True if attach required, False otherwise.
   *
   *  NOTE: This uses uint16_t based on the QMI response type
   */
  AttachResponse isAttachRequired(const std::list<uint16_t>& attach_list, AttachProfile &ril_profile);

  /**
   * @brief      Sets the attach list.
   *             Additionally, sets action if it is supported.
   *
   * @param[in]  attach_list  The attach list
   *
   * @return     SUPPORTED if action is supported by QMI service
   *             UNSUPPORTED if action is not supported by QMI service
   */
  AttachResult setAttachList();

  /**
   * @brief      Sends a request resp.
   *
   * @return     { description_of_the_return_value }
   */
  static void sendReqResp(const RilRespParams& resp, RIL_Errno e);
  void sendSetInitialAttachResponse(std::shared_ptr<Message> m, RIL_Errno e);

#if (QCRIL_RIL_VERSION < 15)
  uint8_t copyParams (const RilRequestSetInitialAttachApnRequestMessage::base_attach_params_t& p,
                      AttachProfile::params_t &params);
  void dumpParams(const RilRequestSetInitialAttachApnRequestMessage::base_attach_params_t& params);
#else
  uint8_t copyParams (const RilRequestSetInitialAttachApnRequestMessage::v15_attach_params_t& p,
                      AttachProfile::params_t &params);
  void dumpParams(const RilRequestSetInitialAttachApnRequestMessage::v15_attach_params_t& p);
#endif
  void initialAttachHandler(std::shared_ptr<Message> m);

public:

#if (QCRIL_RIL_VERSION < 15)
  void handleInitialAttachRequest(
              const RilRequestSetInitialAttachApnRequestMessage::base_attach_params_t& p,
              const RilRespParams& resp_params);
#else
  void handleInitialAttachRequest(
              const RilRequestSetInitialAttachApnRequestMessage::v15_attach_params_t& p,
              const RilRespParams& resp_params);
  void handleSetDataProfileRequestMessage(std::shared_ptr<Message> msg);
  uint8_t copyParams (const DataProfileInfo_t& p, BaseProfile::params_t &params);
  void dumpParams(const DataProfileInfo_t& p);

#endif
  void handleInitialAttachRequest(std::shared_ptr<Message> m);

  void qcril_data_nas_detach_attach_ind_hdlr(uint8_t domainPrefValid,
                         nas_srv_domain_pref_enum_type_v01 domainPref );
  static void handleWdsUnSolInd(const wds_set_lte_attach_pdn_list_action_result_ind_msg_v01 *res);
  static void processNasPsAttachDetachResp(RIL_Errno ret);
  void releaseQdpAttachProfile(void);
  AttachResponse compareModemAttachParams(string ril_apn, string ril_ipfamily);
  boolean matchModemProfileParams
  (
  const wds_get_lte_attach_params_resp_msg_v01 *modem_attach_params,
  const char* ril_apn,
  const char* ril_ipfamily
  );
  void init(bool apAssistMode);
  void dump(string padding, ostream& os);
  ProfileHandler(LocalLogBuffer& logBuffer);
  ~ProfileHandler();
  bool inited = false;
  void handleGetModemAttachParamsRetryMessage();
  void getAttachParamsTimeoutHander(void *);
  void processDetachAttachTimeout(std::shared_ptr<Message> msg);
};

#endif
