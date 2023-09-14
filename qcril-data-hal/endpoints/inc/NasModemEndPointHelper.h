/******************************************************************************
#  Copyright (c) 2018, 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef NASMODEMENDPOINTHELPER
#define NASMODEMENDPOINTHELPER
#include "module/IDataModule.h"
#include "network_access_service_v01.h"
#include "legacy/qcril_data.h"
#include "framework/TimeKeeper.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "sync/SetInitialAttachCompletedMessage.h"
#include "MessageCommon.h"
#include "modules/nas/NasSetPsAttachDetachMessage.h"

using namespace rildata;

class NasModemEndPointHelper
{
public:
   std::shared_ptr<Message> request;
   static RilRespParams resp_params;
   enum class NasDomainPref
   {
    QCRIL_NAS_IA_NONE = 0,
    QCRIL_NAS_IA_DETACH_IN_PROGRESS,
    QCRIL_NAS_IA_DETACH,
    QCRIL_NAS_IA_ATTACH_IN_PROGRESS,
    QCRIL_NAS_IA_ATTACH,
   };
   static NasDomainPref currentState;
   NasModemEndPointHelper() {
     request = NULL;
   }
   ~NasModemEndPointHelper() {
   }
   static void handleDetachRequest(RIL_Token t, qcril_evt_e_type event_id);
   static void handleAttachRequest(RIL_Token t, qcril_evt_e_type event_id);
   static void qcril_data_stop_detach_or_attach_ind_timer();
   static void qcril_data_detach_or_attach_ind_timeout_hdlr(union sigval sval);
   static void process_qcril_data_detach_or_attach_ind_timeout_hdlr(union sigval sval);
   static void qcril_data_start_detach_or_attach_ind_timer(RIL_Token t, qcril_evt_e_type event_id);
   static void qcril_data_handle_detach_attach_ind(qcril_request_resp_params_type &resp_params, uint8_t domainPrefValid, nas_srv_domain_pref_enum_type_v01 domainPref );
   void processDetachAttachResp(qcril_request_resp_params_type &resp_params, RIL_Errno ret);
   void init();
   static boolean retrieveLTEPSAttachStatus();
   static void sendNasAttachDeatchRequest(NasSetPsAttachDetachMessage::PsAttachAction action);

   void handleDetachRequest();
   void handleAttachRequest();
   void stopDetachOrAttachIndTimer();
   void detachOrAttachIndTimeoutHdlr(void *);
   void processDetachOrAttachIndTimeoutHdlr();
   void handleDetachAttachInd(uint8_t domainPrefValid, nas_srv_domain_pref_enum_type_v01 domainPref);
   void processDetachAttachResp(RIL_Errno ret);
   void sendIAResponse(RIL_Errno result);

private:
   static timer_t set_detach_or_attach_timer_id;
   static const  uint32_t QCRIL_DATA_DETACH_ATTACH_IND_TIMEOUT;
   static const uint8_t QCRIL_DATA_INVALID_TIMER_ID;
   TimeKeeper::timer_id detachOrAttachTimerId{TimeKeeper::no_timer};
};

#endif
