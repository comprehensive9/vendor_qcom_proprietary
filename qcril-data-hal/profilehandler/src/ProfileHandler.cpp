/******************************************************************************
#  Copyright (c) 2018,2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "ProfileHandler.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include <list>
#include "AttachProfile.h"
#include "legacy/qcril_data.h"
#include "string.h"
#include "qcril_reqlist.h"
#include "MessageCommon.h"
#include "DataModule.h"
#include "qdp_adapter.h"
#include "DataConfig.h"
#include "UnSolMessages/GetModemAttachParamsRetryMessage.h"
#include "UnSolMessages/DetachAttachIndTimeoutMessage.h"

#define QCRIL_SUCCESS 0
#define QCRIL_FAILURE 1
#define MAX_IA_RETRY_ATTEMPT 2

std::unique_ptr<NasModemEndPointHelper> ProfileHandler::nas_helper = nullptr;
extern DDSSubIdInfo currentDDSSUB;
extern pthread_mutex_t ddsSubMutex;
ProfileHandler::ProfileHandler(LocalLogBuffer& logBuffer): logBuffer(logBuffer)
{
  attach_list = NULL;
  mApAssistMode = false;
  mIARetry = 0;
  memset(&m_resp_params, 0,sizeof(RilRespParams));
}

ProfileHandler::~ProfileHandler()
{
  if(mPendingMessage != nullptr)
  {
    TimeKeeper::getInstance().clear_timer(mIARetryAttachTimer);
    sendSetInitialAttachResponse(mPendingMessage, RIL_E_INTERNAL_ERR);
    mPendingMessage = nullptr;
    mIARetry = 0;
  }
}

void ProfileHandler::handleGetModemAttachParamsRetryMessage()
{
  if(mPendingMessage != nullptr)
  {
    mIARetry++;
    initialAttachHandler(mPendingMessage);
  }
}

void ProfileHandler::getAttachParamsTimeoutHander(void *)
{
  Log::getInstance().d("[ProfileHandler] Retry Attach Timer expired");
  auto msg = std::make_shared<GetModemAttachParamsRetryMessage>();
  msg->broadcast();
}

ProfileHandler::AttachResponse ProfileHandler::compareModemAttachParams(string ril_apn, string ril_ipfamily)
{
  wds_get_lte_attach_params_resp_msg_v01 curr_attach_params;
  memset(&curr_attach_params, 0x0, sizeof(curr_attach_params));
  ProfileHandler::AttachResponse ret = FAILURE;
  if (wds_endpoint) {
    int rc = wds_endpoint->getLteAttachParams(&curr_attach_params);
    if ((curr_attach_params.resp.result == QMI_RESULT_FAILURE_V01) && (curr_attach_params.resp.error == QMI_ERR_OP_IN_PROGRESS_V01))
    {
      Log::getInstance().d("[ProfileHandler] : Starting Retry Attach Timer");
      auto timeoutFn = std::bind(&ProfileHandler::getAttachParamsTimeoutHander, this, std::placeholders::_1);
      mIARetryAttachTimer = TimeKeeper::getInstance().set_timer(timeoutFn, nullptr, RETRY_ATTACH_TIMEOUT);
      ret = INPROGRESS;
    }
    else if(QMI_NO_ERR == rc)
    {
        if(matchModemProfileParams(&curr_attach_params,ril_apn.c_str(),ril_ipfamily.c_str()))
        {
            ret = SUCCESS;
        }
    }
  }
  return ret;
}

boolean ProfileHandler::matchModemProfileParams
(
  const wds_get_lte_attach_params_resp_msg_v01 *modem_attach_params,
  const char* ril_apn,
  const char* ril_ipfamily
)
{
  #ifdef RIL_FOR_MDM_LE
  propertySetMap("persist.vendor.data.profile_update", PERSIST_VENDOR_DATA_PROFILE_UPDATE);
  #endif
  string propValue = readProperty("persist.vendor.data.profile_update", "false");
  Log::getInstance().d("[ProfileHandler]: Data Profile Update property is " + propValue);
  std::transform( propValue.begin(), propValue.end(), propValue.begin(),
              [](char c) -> char { return std::tolower(c); });
  if( (modem_attach_params->apn_string_valid) &&
      (modem_attach_params->ip_type_valid) )
  {
    if( strncasecmp(ril_apn,
                   modem_attach_params->apn_string,
                   (strlen(ril_apn) + 1)) == 0 )
    {
      switch (modem_attach_params->ip_type)
      {
        case WDS_IP_SUPPORT_TYPE_IPV4_V01:
          if (strncasecmp(ril_ipfamily, QDP_RIL_IP_4, (strlen(QDP_RIL_IP_4) + 1)) == 0)
          {
            Log::getInstance().d("[ProfileHandler]: Modem and RIL IP both matched : IPV4");
            return true;
          }
          if (get_carrier_config_enabled() == false && propValue != "true" && strncasecmp(ril_ipfamily, QDP_RIL_IP_4_6, (strlen(QDP_RIL_IP_4_6) + 1)) == 0)
          {
            Log::getInstance().d("[ProfileHandler]: Modem IP: IPV4. RIL IP: IPV4V6. Considering Match.");
            return true;
          }
          break;
        case WDS_IP_SUPPORT_TYPE_IPV6_V01:
          if (strncasecmp(ril_ipfamily, QDP_RIL_IP_6, (strlen(QDP_RIL_IP_6) + 1)) == 0)
          {
            Log::getInstance().d("[ProfileHandler]: Modem and RIL IP both matched : IPV6");
            return true;
          }
          if (get_carrier_config_enabled() == false && propValue != "true" && strncasecmp(ril_ipfamily, QDP_RIL_IP_4_6, (strlen(QDP_RIL_IP_4_6) + 1)) == 0)
          {
            Log::getInstance().d("[ProfileHandler]: Modem IP: IPV6. RIL IP: IPV4V6. Considering Match.");
            return true;
          }
          break;
        case WDS_IP_SUPPORT_TYPE_IPV4V6_V01:
          if (strncasecmp(ril_ipfamily, QDP_RIL_IP_4_6, (strlen(QDP_RIL_IP_4_6) + 1)) == 0)
          {
            Log::getInstance().d("[ProfileHandler]: Modem and RIL IP both matched : IPV4V6");
            return true;
          }
          if (get_carrier_config_enabled() == false && propValue != "true" && ((strncasecmp(ril_ipfamily, QDP_RIL_IP_6, (strlen(QDP_RIL_IP_6) + 1)) == 0)
                        || (strncasecmp(ril_ipfamily, QDP_RIL_IP_4, (strlen(QDP_RIL_IP_4) + 1)) == 0)))
          {
            Log::getInstance().d("[ProfileHandler]: Modem IP: IPV4V6. RIL IP: " + string(ril_ipfamily)+". Considering Match.");
            return true;
          }
          break;
        default:
          Log::getInstance().d("[ProfileHandler]: Modem has unsopported IP type");
          break;
      }
    }
    else
    {
      Log::getInstance().d("[ProfileHandler]: Modem Apn and RIL_APN did not match");
      return false;
    }
  }
  return false;
}


void ProfileHandler::init(bool apAssistMode)
{
  if (!inited) {
    mApAssistMode = apAssistMode;
    Log::getInstance().d("[ProfileHandler]: initialize Modem Endpoint modules");
    //QmiSetupRequestCallback callback("data-token");
    wds_endpoint = ModemEndPointFactory<WDSModemEndPoint>::getInstance().buildEndPoint();
    //wds_endpoint->requestSetup("data-token-client-server", &callback);
    dsd_endpoint = ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint();
    //Datamodule is handling reqeustsetup for DSD
    //dsd_endpoint->requestSetup("data-token-client-server", &callback);
    nas_helper =  std::make_unique<NasModemEndPointHelper>();
    inited = true;
    Log::getInstance().d("ProfileHandler - request setup done");
  } else {
    Log::getInstance().d("ProfileHandler - already inited");
  }
}

void ProfileHandler::releaseQdpAttachProfile(void)
{
  Log::getInstance().d("[ProfileHandler]: releaseQdpAttachProfile");
  if (attach_list && !attach_list->empty()) {
    unsigned int attachProfile = attach_list->front();

    if (attachProfile != AttachProfile::INVALID_PROFILE_ID && qdp_profile_release(attachProfile)) {
      Log::getInstance().d("Removing attach profile from list "+std::to_string((int)attachProfile));
      attach_list->pop_front();
    }
  }
}

#if (QCRIL_RIL_VERSION < 15)

void ProfileHandler::handleInitialAttachRequest(const RilRequestSetInitialAttachApnRequestMessage::base_attach_params_t& p,
  const RilRespParams& resp_params)
{
  Log::getInstance().d("ProfileHandler - handleInitialAttachRequest");
  m_resp_params = resp_params;
  // Update cache since we will need these params when we asynchronously send
  // response backto RIL
  RIL_Errno result = RIL_E_SUCCESS;
  memset(&ril_params, 0,sizeof(ril_params));
  uint8_t res = copyParams( params, ril_params);
  if( res != QCRIL_SUCCESS )
  {
    Log::getInstance().d("[ProfileHandler] Improper input params received. EXIT with FAILURE" );
    sendReqResp(m_resp_params, RIL_E_OEM_ERROR_3);
    return;
  }
  initialAttachHandler(NULL);
}

#else

void ProfileHandler::handleInitialAttachRequest
(
  const RilRequestSetInitialAttachApnRequestMessage::v15_attach_params_t& params,
  const RilRespParams& resp_params
)
{
  Log::getInstance().d("ProfileHandler - handleInitialAttachRequest v15");
  dumpParams(params);
  m_resp_params = resp_params;
  // Update cache since we will need these params when we asynchronously send
  // response backto RIL
  memset(&ril_params, 0,sizeof(ril_params));
  uint8_t res = copyParams( params, ril_params);
  if( res != QCRIL_SUCCESS )
  {
    Log::getInstance().d("[ProfileHandler] Improper input params received. EXIT with FAILURE" );
    sendReqResp(m_resp_params, RIL_E_OEM_ERROR_3);
    return;
  }
  initialAttachHandler(NULL);
}

void ProfileHandler::initialAttachHandler( std::shared_ptr<Message> msg )
{
  RIL_Errno result = RIL_E_SUCCESS;
  AttachProfile ril_profile(ril_params);

  Log::getInstance().d("[ProfileHandler] initialAttachHandler ENTRY");

  if(!wds_endpoint)
  {
    Log::getInstance().d("[ProfileHandler] wds_endpoint is NULL. Sending error response");
    sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
    return;
  }
  Message::Callback::Status status = wds_endpoint->getAttachListSync(attach_list);
  if (status != Message::Callback::Status::SUCCESS)
  {
    Log::getInstance().d("[ProfileHandler] failed to get attach list, result="
                         + std::to_string((int) status));
    sendSetInitialAttachResponse(msg, RIL_E_OEM_ERROR_3);
    return;
  }
  std::list<uint16_t>::const_iterator it;
  Log::getInstance().d("[ProfileHandler] Attach List contents are:");
  if(attach_list)
  {
    for(it = attach_list->begin(); it != attach_list->end(); ++it)
    {
      Log::getInstance().d("Profile Index ="+std::to_string(*it));
    }
  } else
  {
    Log::getInstance().d("Modem Attach list is NULL. Returning Failure response");
    sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
    return;
  }
   // Check if ril_profile matches any of the the attached profiles
  ProfileHandler::AttachResponse resp = isAttachRequired(*attach_list, ril_profile);
  if(resp == INPROGRESS)
  {
    if(mIARetry == MAX_IA_RETRY_ATTEMPT)
    {
      RIL_Errno result = RIL_E_INTERNAL_ERR;
      sendSetInitialAttachResponse(msg, result);
      mIARetry = 0;
      mPendingMessage = nullptr;
    }
    else if(mIARetry == 0)
    {
      //Storing the IA message only once.
      Log::getInstance().d("Storing the IA message");
      mPendingMessage = msg;
    }
    return;
  }
  else if ( resp == SUCCESS )
  {
    if(mPendingMessage)
    {
      mPendingMessage = nullptr;
      mIARetry = 0;
    }
    unsigned int matched_profile = ril_profile.lookup();
    if (matched_profile == AttachProfile::INVALID_PROFILE_ID)
    {
      Log::getInstance().d("[ProfileHandler] No matching profile found "
                           "and failed to create new profile");
      Log::getInstance().d("EXIT with FAILURE" );
      result = RIL_E_OEM_ERROR_3;
      sendSetInitialAttachResponse(msg, RIL_E_OEM_ERROR_3);
      return;
    }
    // update attach list
    if( !(attach_list->empty()) )
    {
      attach_list->pop_front();
    }
    ril_params.profileId = matched_profile;
    attach_list->push_front(matched_profile);
    std::list<uint16_t>::const_iterator it;
    Log::getInstance().d("[ProfileHandler] Attach List in cache updated to:");
    for(it = attach_list->begin(); it != attach_list->end(); ++it)
    {
      Log::getInstance().d("Profile Index="+std::to_string(*it));
    }
    if(wds_endpoint && ril_params.preferred && ((ril_params.apnTypeBitmask & RIL_APN_TYPE_DEFAULT) == RIL_APN_TYPE_DEFAULT)) {
      Log::getInstance().d("[ProfileHandler] Setting default profile to profileId " + std::to_string(ril_params.profileId));
      wds_endpoint->setDefaultProfileNum(ril_params.profileId);
    }

    /* Three Cases:

       1st Case:
       Non-LTE RAT: If the current RAT is non-LTE, then we update the attach list
       only.  Detach/Attach request is not sent to modem.

       If the current RAT is LTE & PS Service is available,
       2nd Case:
       LTE( PS Service Available) + DDS SUB: We send Detach request to modem &
       after detach is complete, we update the 'attach list' on modem.
       Later Attach request is sent to modem.

       3rd Case:
       LTE( PS Service Available) + Non-DDS SUB: We update the attach list first &
       then issue Detach request to modem. After receiving Detach response from modem,
       we send this as IA response to telephony without waiting for Detach Indication.
       So IA request ends here. Attach request is NOT sent to modem.
    */

    AttachResult res = setAttachList();
    if ( res == AttachResult::DETACH_NEEDED)
    {
#ifndef RIL_FOR_MDM_LE
      if(nas_helper)
      {
        bool lteStatus = nas_helper->retrieveLTEPSAttachStatus();

        qcril_instance_id_e_type rilInstanceId = qmi_ril_get_process_instance_id();

        LOCK_MUTEX(ddsSubMutex);
        if( currentDDSSUB.dds_sub_id == QCRIL_INVALID_MODEM_STACK_ID )
        {
          UNLOCK_MUTEX(ddsSubMutex);
          Log::getInstance().d("Current DDS is invalid. Querying the current DDS from modem");
          DDSSubIdInfo ddsInfo = qcril_data_get_dds_sub_info();

          LOCK_MUTEX(ddsSubMutex);
          currentDDSSUB.dds_sub_id = ddsInfo.dds_sub_id;
          currentDDSSUB.switch_type = ddsInfo.switch_type;
          UNLOCK_MUTEX(ddsSubMutex);

          Log::getInstance().d("Current DDS is on SUB ="+std::to_string(currentDDSSUB.dds_sub_id)+
                               "with switch type="+std::to_string(currentDDSSUB.switch_type));
          LOCK_MUTEX(ddsSubMutex);
        }

        #ifdef QMI_RIL_UTF
        rilInstanceId = (qcril_instance_id_e_type)global_instance_id;
        #endif
        if(((currentDDSSUB.dds_sub_id != rilInstanceId) &&
          (DSD_DDS_DURATION_PERMANANT_V01 == currentDDSSUB.switch_type) && lteStatus) || !lteStatus)
        {
          UNLOCK_MUTEX(ddsSubMutex);

          Log::getInstance().d("Current DDS SUB is"+std::to_string(currentDDSSUB.dds_sub_id)+
                               "switch type = "+std::to_string(currentDDSSUB.switch_type));
          Message::Callback::Status status = Message::Callback::Status::FAILURE;
          if (!mApAssistMode)
          {
            //Need to send DSD_SET_APN_INFO only in legacy mode
            if(dsd_endpoint)
            {
              status = dsd_endpoint->setApnInfoSync(ril_params.apn, ril_params.apnTypeBitmask);
              if (status != Message::Callback::Status::SUCCESS)
              {
                Log::getInstance().d("[ProfileHandler] SET_APN_INFO requst failed"
                 ", result = "+ std::to_string((int) status));
              }
              else
              {
                 Log::getInstance().d("[ProfileHandler] SET_APN_INFO request"
                 " successful, result = "+ std::to_string((int) status));
              }
            }
            else
            {
              Log::getInstance().d("[ProfileHandler] ERROR dsd_endpoint is NULL.");
            }
          }
          SetAttachListSyncMessage::AttachListAction action = SetAttachListSyncMessage::AttachListAction::NONE;
          if( wds_endpoint && attach_list )
          {
            std::list<uint16_t>::const_iterator it;
            Log::getInstance().d("[ProfileHandler] Attach List elements are:");
            for(it = attach_list->begin(); it != attach_list->end(); ++it)
            {
              Log::getInstance().d("Element="+std::to_string(*it));
            }
            status = wds_endpoint->setAttachListSync(attach_list, action);
            if (status != Message::Callback::Status::SUCCESS)
            {
              Log::getInstance().d("[ProfileHandler] failed to set attach"
                                   "list, result = "+ std::to_string((int) status));
              sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
              return;
            }
          }
        }
        else
        {
          UNLOCK_MUTEX(ddsSubMutex);
        }
        std::shared_ptr<SetInitialAttachApnRequestMessage> m =
            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(msg);

        std::shared_ptr<RilRequestSetInitialAttachApnRequestMessage> mg =
            std::static_pointer_cast<RilRequestSetInitialAttachApnRequestMessage>(msg);

        if( lteStatus && ( m != NULL ) )
        {
          Log::getInstance().d("[ProfileHandler] handling Detach Request ");
          if(nas_helper->request)
          {
            Log::getInstance().d("[ProfileHandler] Received another IA while previous one is in process");
            nas_helper->stopDetachOrAttachIndTimer();
            nas_helper->sendIAResponse(RIL_E_INTERNAL_ERR);
          }
          nas_helper->request = m;
          nas_helper->handleDetachRequest();
          return;
        }
        else if( lteStatus && ( mg != NULL ) )
        {
          Log::getInstance().d("[ProfileHandler] handling Detach Request ");
          if(nas_helper->resp_params.t)
          {
            Log::getInstance().d("[ProfileHandler] Received another IA while previous one is in process");
            nas_helper->qcril_data_stop_detach_or_attach_ind_timer();
            nas_helper->sendIAResponse(RIL_E_INTERNAL_ERR);
          }
          nas_helper->resp_params.t = m_resp_params.t;
          nas_helper->resp_params.event_id = m_resp_params.event_id;
          nas_helper->resp_params.instance_id = m_resp_params.instance_id;
          nas_helper->handleDetachRequest(m_resp_params.t, m_resp_params.event_id);
          return;
        }
        else
        {
          Log::getInstance().d("[ProfileHandler] non-LTE returning ");
          sendSetInitialAttachResponse(msg, RIL_E_SUCCESS);
          return;
        }
      }
      else
      {
        Log::getInstance().d("[ProfileHandler] nas_helper is NULL"
                             " Sending error response");
        sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
        return;
      }
#else
      Log::getInstance().d("[ProfileHandler] :: LE .Legacy method. Obsolete with newer modem versions");
      result = RIL_E_INTERNAL_ERR;
#endif
    }
    else if ( res == AttachResult::GENERIC_FAILURE )
    {
      // in the event that attach list update failed due to a QMI error
      // we need to notify RIL
      Log::getInstance().d("[ProfileHandler] GENERIC_FAILURE"
                         "Sending error response");
      result = RIL_E_INTERNAL_ERR;
    }

    #ifdef QMI_RIL_UTF
    qdp_cleanup_profile_ids();
    #endif
  }
  else
  {
    if (!mApAssistMode) {
      Log::getInstance().d("[ProfileHandler] Issuing SET_APN_INFO request"
             " with apntypes = "+std::to_string(ril_params.apnTypeBitmask));
      if(dsd_endpoint)
      {
        status = dsd_endpoint->setApnInfoSync(ril_params.apn,
                                            ril_params.apnTypeBitmask);
        if (status != Message::Callback::Status::SUCCESS)
        {
          Log::getInstance().d("[ProfileHandler] SET_APN_INFO requst failed,"
                             " result = "+ std::to_string((int) status));
        } else
        {
          Log::getInstance().d("[ProfileHandler] SET_APN_INFO request successful"
                                   ", result = "+ std::to_string((int) status));
        }
        } else
        {
        Log::getInstance().d("[ProfileHandler] ERROR dsd_endpoint is NULL.");
      }
    }
    Log::getInstance().d("[ProfileHandler] Skipping reattach");
    result = RIL_E_SUCCESS;
  }
  sendSetInitialAttachResponse(msg, result);
}

#endif

ProfileHandler::AttachResponse ProfileHandler::isAttachRequired(const std::list<uint16_t>& attach_list, AttachProfile &ril_profile)
{
  ProfileHandler::AttachResponse res = FAILURE;
  Log::getInstance().d("[ProfileHandler] In isAttachRequired");
  bool attachRequired;
  unsigned int matching_profile = ril_profile.findMatch(attach_list, attachRequired);
  if ( matching_profile != BaseProfile::INVALID_PROFILE_ID) {
    Log::getInstance().d("[ProfileHandler] isAttachRequired()"
      "Matched profile = " + std::to_string(matching_profile));
    return res;
  }
  /*attachRequired is true if any other param apart frm APN mismatches*/
  if(!attachRequired)
  {
    res = compareModemAttachParams( ril_params.apn, ril_params.protocol);
    if(res == FAILURE) {
      Log::getInstance().d("[ProfileHandler] compareModemAttachParams returns Failure: Attach is required");
      res = SUCCESS;
    } else if(res == INPROGRESS) {
      Log::getInstance().d("[ProfileHandler] isAttachRequired(): failed to retrieve modem Attach params. Retry timer started");
      res = INPROGRESS;
    } else {
      Log::getInstance().d("[ProfileHandler] compareModemAttachParams returns SUCCESS: Reattach is not required");
      res = FAILURE;
    }
  } else {
      Log::getInstance().d("[ProfileHandler] isAttachRequired(): Attach is required");
      res = SUCCESS;
  }
  return res;
}

ProfileHandler::AttachResult ProfileHandler::setAttachList()
{
  AttachResult result = GENERIC_FAILURE;
  do
  {
    auto cap = std::make_shared<AttachListCap>();
    // Check capability before we set attach list
    Message::Callback::Status status = wds_endpoint->getAttachListCapabilitySync(cap);
    if (status != Message::Callback::Status::SUCCESS) {
      Log::getInstance().d("[ProfileHandler] failed to get capability, result = "
        + std::to_string((int) status));
      result = GENERIC_FAILURE;
      break;
    }

    SetAttachListSyncMessage::AttachListAction action =
      SetAttachListSyncMessage::AttachListAction::NONE;

    // If capability is supported then we need to pass in action along
    // with the attach list
    if(cap)
    {
      if (*cap == ACTION_SUPPORTED) {
        action = SetAttachListSyncMessage::AttachListAction::DISCONNECT_ATTACH_APN_ONLY;
        result = NO_DETACH_NEEDED;
        Log::getInstance().d("[ProfileHandler] Action is Supported, only disconnect attach apn");
      } else if( *cap == ACTION_NOT_SUPPORTED)
      {
        Log::getInstance().d("[ProfileHandler] Action is Not Supported, Detach Needed");
        result = DETACH_NEEDED;
      }
    } else {
      Log::getInstance().d("[ProfileHandler] Capability retrieved as NULL");
      result = DETACH_NEEDED;
    }

    // If DETACH is needed then we trigger it with Qcril Common - NASModule -LEGACY way
    if( result != DETACH_NEEDED )
    {
      if (!mApAssistMode)
      {
        //Need to send DSD_SET_APN_INFO only in legacy mode
        Log::getInstance().d("[ProfileHandler] Issuing SET_APN_INFO request"
          " with apntypes = "+std::to_string(ril_params.apnTypeBitmask));
        if(dsd_endpoint)
        {
          status = dsd_endpoint->setApnInfoSync(ril_params.apn,
                                                ril_params.apnTypeBitmask);
          if (status != Message::Callback::Status::SUCCESS)
          {
            Log::getInstance().d("[ProfileHandler] SET_APN_INFO requst failed,"
                               " result = "+ std::to_string((int) status));
          } else
          {
            Log::getInstance().d("[ProfileHandler] SET_APN_INFO request successful"
                                   ", result = "+ std::to_string((int) status));
          }
        } else
        {
          Log::getInstance().d("[ProfileHandler] ERROR dsd_endpoint is NULL.");
        }
      }
      status = wds_endpoint->setAttachListSync(attach_list, action);
      if (status != Message::Callback::Status::SUCCESS)
      {
        Log::getInstance().d("[ProfileHandler] failed to set attach list,"
                              " result = "+ std::to_string((int) status));
        result = GENERIC_FAILURE;
        break;
      }
    }
  }while(0);
  return result;
}

/*===========================================================================

  FUNCTION:  qcril_data_nas_detach_attach_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handler to handle NasSrvDomainPrefIndMessage message

    @return
    None.
*/
/*=========================================================================*/
void ProfileHandler::qcril_data_nas_detach_attach_ind_hdlr(uint8_t domainPrefValid, nas_srv_domain_pref_enum_type_v01 domainPref )
{
  std::shared_ptr<SetInitialAttachApnRequestMessage> msg =
            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(nas_helper->request);

  if( msg == NULL ) {
    IxErrnoType                     found_qcril_request = E_SUCCESS;
    qcril_reqlist_public_type       qcril_req_info;

    Log::getInstance().d("[ProfileHandler]::qcril_data_nas_detach_attach_ind_hdlr Invoked");
    memset(&qcril_req_info, 0, sizeof(qcril_reqlist_public_type));

    found_qcril_request =
           qcril_reqlist_query_by_event( m_resp_params.instance_id,
                                                     QCRIL_DEFAULT_MODEM_ID,
                                                     QCRIL_EVT_QMI_REQUEST_INIT_ATTACH_APN,
                                                     &qcril_req_info);

    if( found_qcril_request != E_SUCCESS ) {
      Log::getInstance().d("[ProfileHandler]qcril_data_nas_detach_attach_ind_hdlr"
          "There is no QCRIL_EVT_QMI_REQUEST_INIT_ATTACH_APN event to handle");
      return;
    }
  }
  if ( nas_helper ) {
    qcril_request_resp_params_type resp_params;
    memset(&resp_params, 0, sizeof(qcril_request_resp_params_type));
    resp_params.t = m_resp_params.t;
    resp_params.request_id = m_resp_params.event_id;
    resp_params.instance_id = m_resp_params.instance_id;
    if((NasModemEndPointHelper::NasDomainPref::QCRIL_NAS_IA_DETACH == NasModemEndPointHelper::currentState) && (((TRUE == domainPrefValid)  && (QMI_SRV_DOMAIN_PREF_CS_ONLY_V01 == domainPref)) || (FALSE == domainPrefValid)))
    {
        Log::getInstance().d( "[ProfileHandler]:: Detach successful . Clearing timer ");
        if( msg != NULL )
          nas_helper->stopDetachOrAttachIndTimer();
        else
          nas_helper->qcril_data_stop_detach_or_attach_ind_timer();

        Message::Callback::Status status = Message::Callback::Status::FAILURE;
        if (!mApAssistMode)
        {
          //Need to send DSD_SET_APN_INFO only in legacy mode
          if(dsd_endpoint) {
            status = dsd_endpoint->setApnInfoSync(ril_params.apn, ril_params.apnTypeBitmask);
            if (status != Message::Callback::Status::SUCCESS) {
              Log::getInstance().d("[ProfileHandler] SET_APN_INFO requst failed"
                  ", result = "+ std::to_string((int) status));
            } else {
              Log::getInstance().d("[ProfileHandler] SET_APN_INFO request"
                   " successful, result = "+ std::to_string((int) status));
            }
          } else {
            Log::getInstance().d("[ProfileHandler] ERROR dsd_endpoint is NULL.");
          }
        }

        SetAttachListSyncMessage::AttachListAction action = SetAttachListSyncMessage::AttachListAction::NONE;
        if( wds_endpoint )
        {
          if( attach_list )
          {
            std::list<uint16_t>::const_iterator it;
            Log::getInstance().d("[ProfileHandler] Attach List elements are:");
            for(it = attach_list->begin(); it != attach_list->end(); ++it)
            {
              Log::getInstance().d("Element="+std::to_string(*it));
            }
            status = wds_endpoint->setAttachListSync(attach_list, action);
            if (status != Message::Callback::Status::SUCCESS)
            {
              Log::getInstance().d("[ProfileHandler] failed to set attach"
                              "list, result = "+ std::to_string((int) status));
              sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
              return;
            }
          } else {
            Log::getInstance().d("[ProfileHandler] ERROR attach_list is NULL"
                       " Sending error response to telephony");
            sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
            return;
          }
        } else
        {
           Log::getInstance().d("[ProfileHandler] ERROR wds_endpoint ");
           sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
           return;
        }
      }
      if( msg != NULL )
        nas_helper->handleDetachAttachInd(domainPrefValid, domainPref);
      else
        nas_helper->qcril_data_handle_detach_attach_ind(resp_params, domainPrefValid, domainPref);
    } else
    {
      Log::getInstance().d("[ProfileHandler]qcril_data_nas_detach_attach_ind_hdlr "
                "ERROR: NO NAS object to handle the indication received");
      sendSetInitialAttachResponse(msg, RIL_E_GENERIC_FAILURE);
    }
}

void ProfileHandler::handleWdsUnSolInd
(
  const wds_set_lte_attach_pdn_list_action_result_ind_msg_v01 *res
)
{
  Log::getInstance().d("ProfileHandler::handleWdsUnSolInd ENTRY");
  if ( res && res->result == 1 )
  {
    Log::getInstance().d("handleWdsUnSolInd action result success");
  } else if ( (res && res->result == 0 ) || !res )
  {
    Log::getInstance().d("handleWdsUnSolInd action result failure");
  }
}

uint8_t ProfileHandler::copyParams
(
  const RilRequestSetInitialAttachApnRequestMessage::v15_attach_params_t& p,AttachProfile::params_t &params
)
{
  if (p.apn.length() > (QMI_WDS_APN_NAME_MAX_V01))
  {
    Log::getInstance().d("RIL provided invalid APN"+p.apn+
                         "APN length"+std::to_string(p.apn.length())+" exceeds max allowed"+
                         std::to_string(QMI_WDS_APN_NAME_MAX_V01));
    return QCRIL_FAILURE;
  }
  params.apn = p.apn;
  if ( p.username.length() > 0)
  {
    if ( p.username.length() > QMI_WDS_USER_NAME_MAX_V01)
    {
      Log::getInstance().d("RIL provided username exceeds max allowed"+
                           std::to_string(QMI_WDS_USER_NAME_MAX_V01));
    } else
    {
      params.username = p.username;
      Log::getInstance().d("RIL USERNAME =" +params.username );
    }
  }
  if ( p.password.length() > 0)
  {
    if ( p.password.length() > QMI_WDS_PASSWORD_MAX_V01)
    {
      Log::getInstance().d("RIL provided password exceeds max allowed"+
                           std::to_string(QMI_WDS_PASSWORD_MAX_V01));
    } else
    {
      params.password = p.password;
      Log::getInstance().d("RIL PASSWORD ="+params.password );
    }
  }
  switch (p.authType)
  {
  case WDS_PROFILE_AUTH_PROTOCOL_NONE_V01:
    params.authType = QDP_RIL_PAP_CHAP_NOT_ALLOWED;
    break;
  case WDS_PROFILE_AUTH_PROTOCOL_PAP_V01:
    params.authType = QDP_RIL_PAP_ONLY_ALLOWED;
    break;
  case WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01:
    params.authType = QDP_RIL_CHAP_ONLY_ALLOWED;
    break;
  case WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01:
    params.authType = QDP_RIL_PAP_CHAP_BOTH_ALLOWED;
    break;
  default:
    Log::getInstance().d("Unknown auth_type = "+std::to_string(p.authType));
                          params.authType = QDP_RIL_PAP_CHAP_NOT_ALLOWED;
    break;
  }
  if ( p.protocol.length() > 0 )
  {
    if ( p.protocol.length() > QCRIL_IP_FAMILY_STR_MAX )
    {
      Log::getInstance().d("RIL provided invalid ip family"+p.protocol+
                  "ip family length"+std::to_string(p.protocol.length())+
                  "exceeds max allowed"+std::to_string(QCRIL_IP_FAMILY_STR_MAX));
      return QCRIL_FAILURE;
    }
    params.protocol = p.protocol;
  }
  params.mvnoType = p.mvnoType;
  params.mvnoMatchData = p.mvnoMatchData;
  if ( p.roamingProtocol.length() > 0 )
  {
    if ( p.roamingProtocol.length() > QCRIL_IP_FAMILY_STR_MAX )
    {
      Log::getInstance().d("RIL provided invalid ip family"+p.roamingProtocol+
                  "ip family length"+std::to_string(p.roamingProtocol.length())+
                  "exceeds max allowed"+std::to_string(QCRIL_IP_FAMILY_STR_MAX));
      return QCRIL_FAILURE;
    }
    params.roamingProtocol = p.roamingProtocol;
  }
  params.apnTypeBitmask = p.apnTypeBitmask;
  params.bearerBitmask = p.bearerBitmask;
  params.modemCognitive = p.modemCognitive;
  params.mtu = p.mtu;
  return QCRIL_SUCCESS;
}

void ProfileHandler::sendReqResp( const RilRespParams& resp, RIL_Errno e )
{
  Log::getInstance().d("[ProfileHandler] Sending response");
  qcril_request_resp_params_type ril_resp;
  memset(&ril_resp, 0, sizeof(qcril_request_resp_params_type));
  qcril_default_request_resp_params(resp.instance_id,
                                    resp.t,
                                    resp.event_id,
                                    e,
                                    &ril_resp);
  qcril_send_request_response(&ril_resp);
  memset(&m_resp_params, 0,sizeof(RilRespParams));
}

void ProfileHandler::sendSetInitialAttachResponse(std::shared_ptr<Message> msg, RIL_Errno result)
{
  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(msg);
  if ( m != nullptr )
  {
    Log::getInstance().d("[ProfileHandler] Sending response");
    auto attachCompletedMsg = std::make_shared<SetInitialAttachCompletedMessage>();
    attachCompletedMsg->setMsg(m);
    if( result != RIL_E_SUCCESS )
    {
      attachCompletedMsg->setStatus(false);
    }
    else
    {
      attachCompletedMsg->setStatus(true);
    }
    stringstream ss;
    ss << "[ProfileHandler]: " << (int)m->getSerial() << "< setInitialAttachResponse resp=";
    ss << (int)result;
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
    auto r = std::make_shared<int>();
    attachCompletedMsg->dispatchSync(r);
  }
  else
  {
    Log::getInstance().d("[ProfileHandler] :: Msg is NULL,IA completed msg couldn't be dispatched to DataModule");
    sendReqResp(m_resp_params, result);
  }
}

#if (QCRIL_RIL_VERSION < 15)
void ProfileHandler::dumpParams(const RilRequestSetInitialAttachApnRequestMessage::base_attach_params_t& params)
{
  Log::getInstance().d("APN="+params.apn);
  Log::getInstance().d("USERNAME="+params.username);
  Log::getInstance().d("PASSWORD="+params.password);
  Log::getInstance().d("AUTH="+std::to_string(params.authType));
  Log::getInstance().d("HOME IPTYPE="+params.protocol);
}
#else
void ProfileHandler::dumpParams(const RilRequestSetInitialAttachApnRequestMessage::v15_attach_params_t& params)
{
  Log::getInstance().d("APN="+params.apn);
  Log::getInstance().d("USERNAME="+params.username);
  Log::getInstance().d("PASSWORD="+params.password);
  Log::getInstance().d("AUTH="+std::to_string(params.authType));
  Log::getInstance().d("HOME IPTYPE="+params.protocol);
  Log::getInstance().d("ROAMING IPTYPE="+params.roamingProtocol);
  Log::getInstance().d("MvnoType="+params.mvnoType);
  Log::getInstance().d("MvnoMatchData="+params.mvnoMatchData);
  Log::getInstance().d("ApnTypeBitmask="+std::to_string(params.apnTypeBitmask));
  Log::getInstance().d("BearerBitmask="+std::to_string(params.bearerBitmask));
  Log::getInstance().d("ModemCognitive="+std::to_string(params.modemCognitive));
  Log::getInstance().d("Mtu="+std::to_string(params.mtu));
}

void ProfileHandler::dumpParams(const DataProfileInfo_t& params)
{
  Log::getInstance().d("ProfileId="+std::to_string((int)params.profileId));
  Log::getInstance().d("APN="+params.apn);
  Log::getInstance().d("USERNAME="+params.username);
  Log::getInstance().d("PASSWORD="+params.password);
  Log::getInstance().d("AUTH="+std::to_string((int)params.authType));
  Log::getInstance().d("HOME IPTYPE="+params.protocol);
  Log::getInstance().d("ROAMING IPTYPE="+params.roamingProtocol);
  Log::getInstance().d("AUTH="+std::to_string((int)params.dataProfileInfoType));
  Log::getInstance().d("MaxConnsTime="+std::to_string(params.maxConnsTime));
  Log::getInstance().d("MaxConns="+std::to_string(params.maxConns));
  Log::getInstance().d("EnableProfile="+std::to_string(params.enableProfile));
  Log::getInstance().d("SupportedApnTypesBitmap="+std::to_string((int)params.supportedApnTypesBitmap));
  Log::getInstance().d("BearerBitmap="+std::to_string((int)params.bearerBitmap));
  Log::getInstance().d("Mtu="+std::to_string(params.mtu));
  Log::getInstance().d("Persistent="+std::to_string(params.persistent));
  Log::getInstance().d("Preferred="+std::to_string(params.preferred));
}
#endif

void ProfileHandler::processNasPsAttachDetachResp(RIL_Errno ret)
{
  if (nas_helper == nullptr )
  {
    Log::getInstance().d("[ProfileHandler] nas_helper is NULL");
    return;
  }
  Log::getInstance().d("[ProfileHandler]::processNasPsAttachDetachResp Invoked");

  //If the Detach request is triggered for non-DDS SUB,
  //we should not wait for Detach IND from modem.
  //This is due to the fact that upon receiving Detach request from APPS ,
  //NAS modem State machine detaches & attaches immediately to CS+PS.
  //Hence there won't be any Detach IND sent from NAS modem for Non-DDS Sub.

  std::shared_ptr<SetInitialAttachApnRequestMessage> msg =
            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(nas_helper->request);

  if( msg != NULL ) {
    nas_helper->processDetachAttachResp(ret);
  } else {
    qcril_request_resp_params_type resp_params;
    memset(&resp_params, 0, sizeof(qcril_request_resp_params_type));
    resp_params.t = m_resp_params.t;
    resp_params.request_id = m_resp_params.event_id;
    resp_params.instance_id = m_resp_params.instance_id;
    nas_helper->processDetachAttachResp(resp_params, ret);
  }
}

void ProfileHandler::processDetachAttachTimeout(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[ProfileHandler]::processDetachAttachTimeout");

  if(!nas_helper)
  {
    Log::getInstance().d("[ProfileHandler] nas_helper is NULL");
    return;
  }

  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
            std::static_pointer_cast<SetInitialAttachApnRequestMessage>(nas_helper->request);

  std::shared_ptr<DetachAttachIndTimeoutMessage> timeoutMsg =
            std::static_pointer_cast<DetachAttachIndTimeoutMessage>(msg);

  if(timeoutMsg == nullptr)
  {
    Log::getInstance().d("[ProfileHandler] : msg is null");
    return;
  }

  if( m != NULL ) {
    nas_helper->processDetachOrAttachIndTimeoutHdlr();
  } else {
    nas_helper->process_qcril_data_detach_or_attach_ind_timeout_hdlr(timeoutMsg->getSigVal());
  }
}

/**
 * @brief: Copy profile parameters to BaseProfile object
 * @return QCRIL_FAILURE or QCRIL_SUCCESS
 */
uint8_t ProfileHandler::copyParams
(
  const DataProfileInfo_t& p, BaseProfile::params_t& params
)
{
  switch (p.profileId) {
    case DataProfileId_t::DEFAULT:
      params.profileId = 0;
      break;
    case DataProfileId_t::TETHERED:
      params.profileId = 1;
      break;
    case DataProfileId_t::IMS:
      params.profileId = 2;
      break;
    case DataProfileId_t::FOTA:
      params.profileId = 3;
      break;
    case DataProfileId_t::CBS:
      params.profileId = 4;
      break;
    case DataProfileId_t::OEM_BASE:
    case DataProfileId_t::INVALID:
    default:
      Log::getInstance().d("Unhandled profile id = "+std::to_string((int)p.profileId));
      break;
  }

  if (p.apn.length() > QMI_WDS_APN_NAME_MAX_V01) {
    Log::getInstance().d("RIL provided invalid APN"+p.apn+
                         "APN length"+std::to_string(p.apn.length())+" exceeds max allowed"+
                         std::to_string(QMI_WDS_APN_NAME_MAX_V01));
    return QCRIL_FAILURE;
  }
  params.apn = p.apn;
  if ( p.username.length() > 0) {
    if ( p.username.length() > QMI_WDS_USER_NAME_MAX_V01) {
      Log::getInstance().d("RIL provided username exceeds max allowed"+
                           std::to_string(QMI_WDS_USER_NAME_MAX_V01));
      return QCRIL_FAILURE;
    } else {
      params.username = p.username;
      Log::getInstance().d("RIL USERNAME =" +params.username );
    }
  }
  if ( p.password.length() > 0) {
    if ( p.password.length() > QMI_WDS_PASSWORD_MAX_V01) {
      Log::getInstance().d("RIL provided password exceeds max allowed"+
                           std::to_string(QMI_WDS_PASSWORD_MAX_V01));
      return QCRIL_FAILURE;
    } else {
      params.password = p.password;
      Log::getInstance().d("RIL PASSWORD ="+params.password );
    }
  }

  switch (p.authType) {
    case ApnAuthType_t::NO_PAP_NO_CHAP:
      params.authType = QDP_RIL_PAP_CHAP_NOT_ALLOWED;
      break;
    case ApnAuthType_t::PAP_NO_CHAP:
      params.authType = QDP_RIL_PAP_ONLY_ALLOWED;
      break;
    case ApnAuthType_t::NO_PAP_CHAP:
      params.authType = QDP_RIL_CHAP_ONLY_ALLOWED;
      break;
    case ApnAuthType_t::PAP_CHAP:
      params.authType = QDP_RIL_PAP_CHAP_BOTH_ALLOWED;
      break;
    default:
      Log::getInstance().d("Unknown auth_type = "+std::to_string((int)p.authType));
      return QCRIL_FAILURE;
  }
  if ( p.protocol.length() > 0 ) {
    if ( p.protocol.length() > QCRIL_IP_FAMILY_STR_MAX ) {
      Log::getInstance().d("RIL provided invalid ip family"+p.protocol+
                  "ip family length"+std::to_string(p.protocol.length())+
                  "exceeds max allowed"+std::to_string(QCRIL_IP_FAMILY_STR_MAX));
      return QCRIL_FAILURE;
    }
    params.protocol = p.protocol;
  }
  if ( p.roamingProtocol.length() > 0 ) {
    if ( p.roamingProtocol.length() > QCRIL_IP_FAMILY_STR_MAX ) {
      Log::getInstance().d("RIL provided invalid ip family"+p.roamingProtocol+
                  "ip family length"+std::to_string(p.roamingProtocol.length())+
                  "exceeds max allowed"+std::to_string(QCRIL_IP_FAMILY_STR_MAX));
      return QCRIL_FAILURE;
    }
    params.roamingProtocol = p.roamingProtocol;
  }
  params.mtu = p.mtu;
  params.preferred = p.preferred;
  params.persistent = p.persistent;
  params.maxConnsTime = p.maxConnsTime;
  params.maxConns = p.maxConns;
  params.waitTime = p.waitTime;
  params.enableProfile = p.enableProfile;
  params.apnTypeBitmask = static_cast<uint32_t>(p.supportedApnTypesBitmap);

  switch(p.dataProfileInfoType)
  {
    case DataProfileInfoType_t::THREE_GPP2:
      params.radioTech = QDP_RADIOTECH_3GPP2;
      break;
    case DataProfileInfoType_t::THREE_GPP:
      params.radioTech = QDP_RADIOTECH_3GPP;
      break;
    case DataProfileInfoType_t::COMMON:
      params.radioTech = QDP_RADIOTECH_COMMON;
      break;
  }

  return QCRIL_SUCCESS;
} /* ProfileHandler::copyParams() */

/**
 * @brief handleSetDataProfileRequestMessage
 * @details
 * - Process each profile in the incoming message thus:
 * -- Looks up matching profile on modem based on APN class, or creates
 *   one if needed
 * -- If operating in AP assist mode, invokes wds_endpoint to update
 *    profile parameters on modem (see handling for 'persistent' and
 *    'preferred' profiles, introduced in Radio HAL 1.4)
 * - Sends response for this message to the HAL
 * @return void
 **/
void ProfileHandler::handleSetDataProfileRequestMessage(std::shared_ptr<Message> msg) {
  RIL_Errno result = RIL_E_SUCCESS;
  uint8 res = QCRIL_FAILURE;
  Log::getInstance().d("[ProfileHandler]: handleSetDataProfileRequestMessage");
  std::shared_ptr<SetDataProfileRequestMessage> m =
    std::static_pointer_cast<SetDataProfileRequestMessage>(msg);

  if(m != nullptr) {
    /* examine all provided profiles and update modem profiles if needed */
    for(auto it : m->getProfiles())
    {
      dumpParams(it);
      BaseProfile::params_t params;
      res = copyParams(it, params);
      if( QCRIL_SUCCESS != res ) { /* error */
        result = RIL_E_INTERNAL_ERR;
        Log::getInstance().d("[ProfileHandler]: Error handling incoming profile");
        break;
      }
      if(it.persistent) { /* erstwhile modemCognitive handling */
        BaseProfile* bp = new BaseProfile(params);
        if( bp ) {
          if( QDP_SUCCESS != bp->lookupByApnClass(params) ) {
            result = RIL_E_INTERNAL_ERR;
            Log::getInstance().d("[ProfileHandler]: Error handling persistent profile");
            break;
          }
          delete bp;
          bp = nullptr;
        }
      } /* persistent */
    } /* process all profiles in message */

    /* Update APN type bitmask, set default profile, etc. for AP Assist
    * mode, via WDSModemEndPoint
    */
    if(QCRIL_SUCCESS == res && mApAssistMode && wds_endpoint) {
      vector<DataProfileInfo_t> profiles = m->getProfiles();
      Message::Callback::Status status = wds_endpoint->setProfileParamsSync(profiles);
      if (status != Message::Callback::Status::SUCCESS) { /* error */
        result = RIL_E_INTERNAL_ERR;
        Log::getInstance().d("[ProfileHandler]: Error during ApAssist mode handling");
      }
    } /* ap assist mode */

    /* finished processing SetDataProfileRequestMessage. Send response */
    auto resp = std::make_shared<RIL_Errno>(result);
    if( RIL_E_SUCCESS !=  result ) {
      m->sendResponse(msg, Message::Callback::Status::FAILURE, resp);
    } else {
      m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
    }
    stringstream ss;
    ss << "[ProfileHandler]: " << (int)m->getSerial() << "< setDataProfileResponse resp=";
    ss << (int)result;
    Log::getInstance().d(ss.str());
    logBuffer.addLogWithTimestamp(ss.str());
  }
  else {// (m == nullptr)
    Log::getInstance().d("[ProfileHandler]: Improper SetDataProfileRequestMessage received");
  }
} /* ProfileHandler::handleSetDataProfileRequestMessage() */

void ProfileHandler::handleInitialAttachRequest(std::shared_ptr<Message> msg)
{
  std::shared_ptr<SetInitialAttachApnRequestMessage> m =
       std::static_pointer_cast<SetInitialAttachApnRequestMessage>(msg);
  Log::getInstance().d("[ProfileHandler]: Handling: " + msg->dump());
  if( m != NULL ) {
    ril_params = {};
    uint8_t res = copyParams( m->mProfile, ril_params);
    if( res != QCRIL_SUCCESS )
    {
      Log::getInstance().d("[ProfileHandler] Improper input params received. EXIT with FAILURE" );
      sendSetInitialAttachResponse(m, RIL_E_OEM_ERROR_3);
      return;
    }
    if(mPendingMessage != nullptr)
    {
      Log::getInstance().d("[ProfileHandler] Received another IA, Stopping retry timer.");
      TimeKeeper::getInstance().clear_timer(mIARetryAttachTimer);
      sendSetInitialAttachResponse(mPendingMessage, RIL_E_INTERNAL_ERR);
      mPendingMessage = nullptr;
      mIARetry = 0;
    }
    initialAttachHandler(msg);
  }
}

void ProfileHandler::dump(string padding, ostream& os) {
  os << padding << "inited=" << std::boolalpha << inited << endl;
  os << padding << "attach_list=";
  if (attach_list == nullptr) {
    os << "null";
  } else {
    copy(attach_list->begin(), attach_list->end(), ostream_iterator<uint16_t>(cout, ","));
  }
  os << endl;
  os << padding << "RIL Params:" << endl;
  ril_params.dump(padding + "    ", os);
}

