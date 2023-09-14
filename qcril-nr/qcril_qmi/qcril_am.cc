/******************************************************************************
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_am.c
  @brief   qcril qmi - Audio Management

  DESCRIPTION
    Implements Audio Management APIs.

******************************************************************************/

#define LOG_TAG "RILQ"
#define TAG LOG_TAG

#include <cutils/properties.h>
#include <framework/Log.h>
#include <framework/legacy.h>
#include <WakeLock.h>

#include "modules/ims/ImsServiceStatusInd.h"
#include "modules/audio/IpcAudioCallStateMessage.h"
#include "qcril_am.h"
#include "qcril_qmi_oem_events.h"
#include "ip_multimedia_subsystem_application_v01.h"

#define __STDC_FORMAT_MACROS 1

#include "modules/nas/qcril_qmi_nas.h"
#include "modules/voice/qcril_qmi_voice.h"
#include "interfaces/audio/QcRilGetAudioParamSyncMessage.h"
#include "interfaces/audio/QcRilSetAudioParamSyncMessage.h"
#include "qcril_config.h"

extern "C" {
#include "qcrili.h"
}

using namespace qcril::interfaces;
#define QCRIL_IGNORE_IMS_WLAN "persist.vendor.radio.ignore_ims_wlan"

static qtimutex::QtiRecursiveMutex am_state_mutex;
static boolean is_prim_rild;
static boolean is_dsda;
static uint8_t num_of_rilds;
static boolean ignore_ims_wlan;

typedef struct {
    boolean is_valid;
    uint32_t vsid;
} voice_vsid_type;

typedef struct {
    boolean is_valid;
    qcril_am_audio_api_param_type audio_api_param;
} pending_call_state_change_req;

typedef struct {
    // apply to all rild instances
    voice_vsid_type voice_vsid;
    voice_vsid_type lte_vsid;
    voice_vsid_type wlan_vsid;
    boolean in_lch;

    // apply to primary rild only
    boolean ims_on_wlan;
    call_mode_enum_v02 emergency_rat;

    // only apply to DSDA primary rild
    AudioCallState cur_state[QCRIL_MAX_INSTANCE_ID];
    pending_call_state_change_req pending_ril_req[QCRIL_MAX_INSTANCE_ID];
} qcril_am_state_type;

qcril_am_state_type am_state;

// IMSA REFACTOR: INTG
/* Add these subscriptions to the Audio Manager Message registration
  mMessageHandler = {
      HANDLER(QcrilInitMessage, AudioManagerModule::handleQcrilInitMessage),
      HANDLER(ImsServiceStatusInd, AudioManagerModule::handleImsServiceStatusInd), // this should
call qcril_am_set_ims_on_wlan_from_srv_chng_ind()
  };
*/


/***************************************************************************************************
    @function
    qcril_am_lock - this lock could be recursively calling within a thread
***************************************************************************************************/
static void qcril_am_lock()
{
  QCRIL_LOG_INFO("BEFORE LOCK am_state_mutex");
  am_state_mutex.lock();
  QCRIL_LOG_INFO("AFTER LOCK am_state_mutex");
}

/***************************************************************************************************
    @function
    qcril_am_unlock
***************************************************************************************************/
static void qcril_am_unlock()
{
  QCRIL_LOG_INFO("BEFORE UNLOCK am_state_mutex");
  am_state_mutex.unlock();
  QCRIL_LOG_INFO("AFTER UNLOCK am_state_mutex");
}

/* IMSA REFACTOR: INTG
void AudioManagerModule::handleImsServiceStatusInd(std::shared_ptr<ImsaServiceStateInd> ssmsg)
{
    qcril_am_set_ims_on_wlan_from_srv_chng_ind(ssmsg);
} */


/***************************************************************************************************
    @function
    qcril_am_get_audio_vsid

    @implementation detail
    Gets vsid for the voice subsystem as queried.

    @param[in]
        vs_type
            voice subsystem type which is queried

    @param[out]
        vsid
            vsid of the voice subsystem type queried

    @retval
    QCRIL_AM_ERR_NONE if function is successful, QCRIL_AM_ERR_VSID_NOT_AVAILABLE if the vsid
    is not available.
***************************************************************************************************/
static RIL_Errno qcril_am_get_audio_vsid(qcril_am_vs_type vs_type, uint32_t *vsid)
{
    uint32_t voice_modem_vsid[QCRIL_MAX_NUM_VOICE_MODEM] = {0x10C01000, 0x10DC1000};
    RIL_Errno err = RIL_E_SUCCESS;
    int modem_index = 0;

    // TODO: did not find IMS_VSID and VOICE_VSID value in header file
    switch (vs_type)
    {
    case QCRIL_AM_VS_IMS:
        qcril_am_lock();
        if (am_state.lte_vsid.is_valid)
        {
            *vsid = am_state.lte_vsid.vsid;
        }
        else
        {
            *vsid = 0x10C02000; // IMS_VSID
        }
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_IMS_WLAN:
        qcril_am_lock();
        if (am_state.wlan_vsid.is_valid)
        {
            *vsid = am_state.wlan_vsid.vsid;
        }
        else
        {
            *vsid = 0x10002000; // IMS_WLAN_VSID
        }
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_VOICE:
        if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_SGLTE))
        {
            int int_config;
            if (qcril_config_get(PERSIST_VENDOR_RADIO_VOICE_MODEM_INDEX, int_config) == E_SUCCESS)
            {
                if ((int_config >= QMI_RIL_ZERO) && (int_config < QCRIL_MAX_NUM_VOICE_MODEM))
                {
                    *vsid = voice_modem_vsid[modem_index];
                }
                else
                {
                    err = RIL_E_GENERIC_FAILURE;
                    QCRIL_LOG_INFO("Invalid modem index!");
                }
            }
            else
            {
                err = RIL_E_GENERIC_FAILURE;
                QCRIL_LOG_ERROR("modem index is not set");
            }
        }
        else
        {
            qcril_am_lock();
            if (am_state.voice_vsid.is_valid)
            {
                *vsid = am_state.voice_vsid.vsid;
            }
            else
            {
                *vsid = 0x10C01000; // VOICE_VSID
            }
            qcril_am_unlock();
        }
        break;
    default:
        err = RIL_E_GENERIC_FAILURE;
    }
    return err;
}

/***************************************************************************************************
    @function
    qcril_am_get_current_ims_vs_type

    @brief
    Retrieves current IMS vs type (ie., LTE or WLAN)
***************************************************************************************************/
qcril_am_vs_type qcril_am_get_current_ims_vs_type()
{
  qcril_am_vs_type vs_type = QCRIL_AM_VS_IMS;

  if (am_state.emergency_rat == CALL_MODE_WLAN_V02)
  {
    vs_type = QCRIL_AM_VS_IMS_WLAN;
  }
  else if (am_state.emergency_rat == CALL_MODE_LTE_V02 ||
          am_state.emergency_rat == CALL_MODE_NR5G_V02)
  {
    vs_type = QCRIL_AM_VS_IMS;
  }
  else
  {
    if (am_state.ims_on_wlan)
    {
      vs_type = QCRIL_AM_VS_IMS_WLAN;
    }
    else
    {
      vs_type = QCRIL_AM_VS_IMS;
    }
  }

  QCRIL_LOG_INFO("am_state.ims_on_wlan: %d, am_state.emergency_rat = %d, ims_vs_type = %d",
      am_state.ims_on_wlan, am_state.emergency_rat, vs_type);

  return vs_type;
}

/***************************************************************************************************
    @function
    qcril_am_set_emergency_rat

    @brief
    Set emergency_rat based on mode in origination state for emergency calls.
***************************************************************************************************/
void qcril_am_set_emergency_rat(call_mode_enum_v02 rat)
{
  qcril_am_lock();
  am_state.emergency_rat = rat;
  qcril_am_unlock();
}

/***************************************************************************************************
    @function
    qcril_am_reset_emergency_rat

    @brief
   Reset ims_on_lte after the emergency call ends .
***************************************************************************************************/
void qcril_am_reset_emergency_rat()
{
  qcril_am_lock();
  am_state.emergency_rat = CALL_MODE_UNKNOWN_V02;
  qcril_am_unlock();
}

/***************************************************************************************************
    @function
    qcril_am_get_audio_call_state

    @implementation detail
    Maps qcril_am call_state to call_state as defined in AudioSystem.h
***************************************************************************************************/
AudioCallState qcril_am_get_audio_call_state(AudioCallState call_state)
{
    AudioCallState ret = qcril::interfaces::AudioCallState::INACTIVE;
    if (call_state != qcril::interfaces::AudioCallState::INVALID)
    {
        ret = call_state;
        if (qcril::interfaces::AudioCallState::ACTIVE == ret)
        {
            qcril_am_lock();
            if (am_state.in_lch)
            {
                ret = qcril::interfaces::AudioCallState::LOCAL_HOLD;
            }
            qcril_am_unlock();
        }
    }
    else
    {
        QCRIL_LOG_DEBUG("invalid call_state value: %d", call_state);
    }
    return ret;
}


/***************************************************************************************************
    @function
    qcril_am_is_active_call

    @implementation detail
    Checks if a call will be considered as an active call from AM perspective. It includes both
    current active call and the call expected to be changed to active by modem.
***************************************************************************************************/
static boolean qcril_am_is_active_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    boolean ret = FALSE;
    if (call_info_entry)
    {
        if ( CALL_STATE_CONVERSATION_V02 == call_info_entry->voice_scv_info.call_state ||
             CALL_STATE_ORIGINATING_V02 == call_info_entry->voice_scv_info.call_state ||
             CALL_STATE_ALERTING_V02 == call_info_entry->voice_scv_info.call_state ||
             CALL_STATE_DISCONNECTING_V02 == call_info_entry->voice_scv_info.call_state ||
             call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_ANSWERING_CALL ||
             call_info_entry->elaboration & QCRIL_QMI_VOICE_VOIP_CALLINFO_ELA_SWITCHING_CALL_TO_ACTIVE
           )
        {
            ret = TRUE;
        }
    }
    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_is_hold_call

    @implementation detail
    Checks if a call is a hold call.
***************************************************************************************************/
static boolean qcril_am_is_hold_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    boolean ret = FALSE;
    if (call_info_entry)
    {
        if ( CALL_STATE_HOLD_V02 == call_info_entry->voice_scv_info.call_state )
        {
            ret = TRUE;
        }
    }
    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_is_active_ims_call

    @implementation detail
    Checks if a call will be considered as an active ims call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_active_ims_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_ims_audio(call_info_entry) &&
           (qcril_am_is_active_call(call_info_entry) ||
           qcril_qmi_voice_has_crs_type_call(call_info_entry));
}

/***************************************************************************************************
    @function
    qcril_am_is_hold_ims_call

    @implementation detail
    Checks if a call is a hold ims call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_hold_ims_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_ims_audio(call_info_entry) &&
           qcril_am_is_hold_call(call_info_entry);
}

/***************************************************************************************************
    @function
    qcril_am_is_active_voice_call

    @implementation detail
    Checks if a call will be considered as an active voice call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_active_voice_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_voice_audio(call_info_entry) &&
           qcril_am_is_active_call(call_info_entry);
}

/***************************************************************************************************
    @function
    qcril_am_is_hold_voice_call

    @implementation detail
    Checks if a call is a hold ims call from AM perspective.
***************************************************************************************************/
static boolean qcril_am_is_hold_voice_call(const qcril_qmi_voice_voip_call_info_entry_type* call_info_entry)
{
    return qcril_qmi_voice_is_call_has_voice_audio(call_info_entry) &&
           qcril_am_is_hold_call(call_info_entry);
}

/***************************************************************************************************
    @function
    qcril_am_set_voice_call_audio_driver_by_call_state
***************************************************************************************************/
static RIL_Errno qcril_am_set_voice_call_audio_driver_by_call_state()
{
    RIL_Errno ret = RIL_E_SUCCESS;
    call_mode_enum_v02 call_mode = CALL_MODE_UNKNOWN_V02;

    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_voice_call, &call_mode))
    {
        QCRIL_LOG_INFO("has active voice call");
        ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                        qcril::interfaces::AudioCallState::ACTIVE, call_mode);
    }
    else if (qcril_qmi_voice_has_specific_call(qcril_am_is_hold_voice_call, &call_mode))
    {
        QCRIL_LOG_INFO("has hold voice call");
        ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                          qcril::interfaces::AudioCallState::HOLD, call_mode);
    }
    else
    {
        QCRIL_LOG_INFO("no active and hold voice call");
        ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                      qcril::interfaces::AudioCallState::INACTIVE, call_mode);
    }

    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_set_ims_call_audio_driver_by_call_state
***************************************************************************************************/
static RIL_Errno qcril_am_set_ims_call_audio_driver_by_call_state(boolean ignore_no_calls_case = FALSE)
{
    RIL_Errno ret = RIL_E_SUCCESS;
    call_mode_enum_v02 call_mode = CALL_MODE_UNKNOWN_V02;
    qcril_am_vs_type current_vs_type = qcril_am_get_current_ims_vs_type();

    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, &call_mode))
    {
        if (current_vs_type == QCRIL_AM_VS_IMS_WLAN)
        {
          QCRIL_LOG_INFO("has active ims call on WLAN");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::ACTIVE,
                                               call_mode);
        }
        else
        {
          QCRIL_LOG_INFO("has active ims call");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::ACTIVE,
                                               call_mode);
        }
    }
    else if (qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, &call_mode))
    {
        if(current_vs_type == QCRIL_AM_VS_IMS_WLAN)
        {
          QCRIL_LOG_INFO("has hold ims call on WLAN");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::HOLD,
                                               call_mode);
        }
        else
        {
          QCRIL_LOG_INFO("has hold ims call");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::HOLD,
                                               call_mode);
        }
    }
    else if (!ignore_no_calls_case)
    {
          QCRIL_LOG_INFO("no active and hold ims call on WLAN");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);

          QCRIL_LOG_INFO("no active and hold ims call");
          ret = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
    }

    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_is_any_non_inactive_calls_with_vsid
***************************************************************************************************/
static boolean qcril_am_is_any_non_inactive_calls_with_vsid(uint32_t vsid)
{
    boolean ret = FALSE;
    uint vs_type;
    uint32_t audio_vsid;

    for (vs_type = QCRIL_AM_VS_MIN+1; (vs_type < QCRIL_AM_VS_MAX && ret == FALSE); vs_type++)
    {
        if (qcril_am_get_audio_vsid((qcril_am_vs_type)vs_type, &audio_vsid) != RIL_E_SUCCESS)
        {
            break;
        }
        QCRIL_LOG_DEBUG("(vs_type = %d, audio_vsid = %d), queried vsid = %d",
                        vs_type, audio_vsid, vsid);
        if (vsid == audio_vsid)
        {
            switch (vs_type)
            {
                case QCRIL_AM_VS_IMS:
                    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, NULL) ||
                            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, NULL))
                    {
                        if(qcril_am_get_current_ims_vs_type() == QCRIL_AM_VS_IMS)
                        {
                            QCRIL_LOG_DEBUG("Has active/hold IMS calls");
                            ret = TRUE;
                        }
                    }
                    break;

                case QCRIL_AM_VS_IMS_WLAN:
                    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, NULL) ||
                            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, NULL))
                    {
                        if(qcril_am_get_current_ims_vs_type() == QCRIL_AM_VS_IMS_WLAN)
                        {
                            QCRIL_LOG_DEBUG("Has active/hold WLAN calls");
                            ret = TRUE;
                        }
                    }
                    break;

                case QCRIL_AM_VS_VOICE:
                    if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_voice_call, NULL) ||
                            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_voice_call, NULL))
                    {
                        QCRIL_LOG_DEBUG("Has active/hold VOICE calls");
                        ret = TRUE;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    QCRIL_LOG_DEBUG("ret = %d\n", ret);
    return ret;
}

/* this should be called from the AM Module */
void qcril_am_set_ims_on_wlan_from_srv_chng_ind(std::shared_ptr<ImsServiceStatusInd> imsa_ssi)
{
  boolean voip_on_wlan = FALSE;
  boolean vt_on_wlan = FALSE;

  if (ignore_ims_wlan)
  {
    QCRIL_LOG_INFO("ignore ims wlan vsid handling");
    return;
  }

  qcril_am_lock();
  if (imsa_ssi->getServiceStatusInfo().voip_service_rat != ImsServiceStatusInd::ServiceRat::INVALID)
  {
    voip_on_wlan = (imsa_ssi->getServiceStatusInfo().voip_service_rat == ImsServiceStatusInd::ServiceRat::WLAN) ? 1 : 0;
  }

  if ((imsa_ssi->getServiceStatusInfo().vt_service_rat != ImsServiceStatusInd::ServiceRat::INVALID))
  {
    vt_on_wlan = (imsa_ssi->getServiceStatusInfo().vt_service_rat == ImsServiceStatusInd::ServiceRat::WLAN) ? 1 : 0;
  }
  am_state.ims_on_wlan = voip_on_wlan | vt_on_wlan;
  QCRIL_LOG_INFO("am_state.ims_on_wlan: %d", am_state.ims_on_wlan);
  qcril_am_unlock();

  if((num_of_rilds == 1) || qcril_qmi_nas_is_ims_available())
  {
    qcril_am_set_ims_call_audio_driver_by_call_state();
  }
}

/***************************************************************************************************
    @function
    qcril_am_set_ims_on_wlan_from_handover

    @brief
    Sets IMS service is on wlan or not.
***************************************************************************************************/
static void qcril_am_set_ims_on_wlan_from_handover(imsa_service_rat_enum_v01 target_rat)
{
    if (ignore_ims_wlan)
    {
        QCRIL_LOG_INFO("ignore ims wlan vsid handling");
    }
    else
    {
        qcril_am_lock();
        am_state.ims_on_wlan = (target_rat == IMSA_WLAN_V01) ? TRUE: FALSE;
        QCRIL_LOG_INFO("am_state.ims_on_wlan: %d", am_state.ims_on_wlan);
        qcril_am_unlock();
    }
}

static void qcril_am_handle_audio_rat_change(voice_audio_rat_change_info_ind_msg_v02* ind)
{
    if (ignore_ims_wlan)
    {
        QCRIL_LOG_INFO("ignore ims wlan vsid handling");
    }
    else
    {
        if ( !ind || !ind->rat_info_valid )
            return;
        bool is_rat_wlan  = (ind->rat_info == CALL_MODE_WLAN_V02);

        // If audio session info is reported
        // ==> SESSION_START: the current rat is just ind->rat_info
        // ==> SESSION_STOP:
        // 1) The current rat is CALL_MODE_WLAN_V02 if modem reports CALL_MODE_LTE_V02
        // 2) The current rat is CALL_MODE_LTE_V02 if modem reports CALL_MODE_WLAN_V02
        // 3) What if modem reports other values? And is it possible?
        if ( ind->audio_session_info_valid &&
            ind->audio_session_info == VOICE_AUDIO_PASSIVE_SESSION_STOP_V02 )
        {
            is_rat_wlan = !is_rat_wlan;
        }

        qcril_am_lock();
        am_state.ims_on_wlan = is_rat_wlan;
        QCRIL_LOG_INFO("am_state.ims_on_wlan: %d", am_state.ims_on_wlan);
        qcril_am_unlock();

        // since the call could be in active or hold state, still need to check call state
        qcril_am_set_ims_call_audio_driver_by_call_state();
    }
}

/***************************************************************************************************
    @function
    qcril_am_is_same_audio_call_state

    @brief
    Checks if the call state we are trying to set is the same as current audio state.

    AudioSystem::getParameters will return a string in a format of:
        all_call_states=281022464:1,282857472:1,281026560:1,276836352:1
***************************************************************************************************/
boolean qcril_am_is_same_audio_call_state
(
    uint32_t vsid,
    qcril::interfaces::AudioCallState new_call_state
)
{
    boolean ret = FALSE;
    qcril::interfaces::AudioCallState call_state = qcril::interfaces::AudioCallState::INVALID;
    std::shared_ptr<QcRilGetAudioParams> respPtr {};
    auto getParamMsg = std::make_shared<QcRilGetAudioParamSyncMessage>(vsid);

    QCRIL_LOG_INFO("vsid: %d, call_state: %d", vsid, new_call_state);

    if (getParamMsg != nullptr){
        auto retVal = getParamMsg->dispatchSync(respPtr);
        if((retVal == Message::Callback::Status::SUCCESS) && (respPtr != nullptr)) {
          QCRIL_LOG_INFO("AudioSystem::getParameters returned : %d", respPtr->call_state);
          call_state = respPtr->call_state;
          if ( call_state != qcril::interfaces::AudioCallState::INVALID &&
               new_call_state != qcril::interfaces::AudioCallState::INVALID )
          {
              QCRIL_LOG_ESSENTIAL("QCRIL_ERROR:AUDIO: cur or new call state not in valid range");
          }
          if (new_call_state == call_state)
          {
              ret = TRUE;
          }
        }
        else {
            QCRIL_LOG_INFO("getParameters returned %d", retVal);
        }
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)ret);
    return ret;
}

/***************************************************************************************************
    @function
    qcril_am_handle_pending_req

    @brief
    Applies to DSDA only.
    Checks pending requests and calls Audio SetParameter API if applicable. Avoids to have two
    voices both on Active state.
***************************************************************************************************/
static void qcril_am_handle_pending_req()
{
    uint8_t num_of_active_state = 0;
    uint8_t num_of_pending_req = 0;
    uint8_t i;

    qcril_am_lock();
    for (i=0; i<num_of_rilds; i++)
    {
        if (am_state.pending_ril_req[i].is_valid)
        {
            num_of_pending_req++;
            if (qcril::interfaces::AudioCallState::ACTIVE ==
                                        am_state.pending_ril_req[i].audio_api_param.call_state)
            {
                num_of_active_state++;
            }
        }
        else
        {
            if (qcril::interfaces::AudioCallState::ACTIVE == am_state.cur_state[i])
            {
                num_of_active_state++;
            }
        }
    }

    QCRIL_LOG_INFO( "num_of_pending_req: %d, num_of_active_state: %d",
                    num_of_pending_req, num_of_active_state );
    if (num_of_pending_req && num_of_active_state <= 1)
    {
      std::vector<QcRilSetAudioParams> audio_params;
      for (i=0; i<num_of_rilds; i++)
      {
          if ( am_state.pending_ril_req[i].is_valid)
          {
              QcRilSetAudioParams param(am_state.pending_ril_req[i].audio_api_param.voice_vsid,
                                        am_state.pending_ril_req[i].audio_api_param.call_state,
                                        am_state.pending_ril_req[i].audio_api_param.call_mode);
              am_state.cur_state[i] = param.call_state;
              am_state.pending_ril_req[i].is_valid = FALSE;
              audio_params.push_back(param);
          }
      }
      std::shared_ptr<RIL_Errno> respPtr {nullptr};
      auto setParamMsg = std::make_shared<QcRilSetAudioParamSyncMessage>(audio_params);
      if (setParamMsg != nullptr){
          auto ret = setParamMsg->dispatchSync(respPtr);
          if((ret == Message::Callback::Status::SUCCESS) && (respPtr != nullptr)) {
              QCRIL_LOG_INFO("AudioSystem::setParameters return status: %d", *respPtr);
          }
          else {
              QCRIL_LOG_INFO("setParameters returned %d", ret);
          }
      }
    }

    qcril_am_unlock();
}

/***************************************************************************************************
    @function
    qcril_am_inform_am_lch_complete

    @brief
    Send QCRIL_EVT_QMI_VOICE_AM_LCH_COMPLETE
***************************************************************************************************/
static void qcril_am_inform_am_lch_complete()
{
    QCRIL_LOG_INFO( "Inform AM_LCH_COMPLETE");
    // Inform setParameters for LCH is complete
    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_DATA_ON_STACK,
            QCRIL_EVT_QMI_VOICE_AM_LCH_COMPLETE,
            NULL,
            0,
            (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
}

/***************************************************************************************************
    @function
    qcril_am_process_event_inter_ril_call_state

    @brief
    Process the QCRIL_AM_EVENT_INTER_RIL_CALL_STATE
***************************************************************************************************/
static void qcril_am_process_event_inter_ril_call_state(const qcril_am_inter_rild_msg_type *msg)
{
    uint8_t resp_to_ril_id;
    qcril_am_inter_rild_msg_type resp_msg;

    if (msg)
    {
        QCRIL_LOG_INFO( "received rild_id: %d, call_state: %d, voice_vsid: %d",
                        msg->rild_id, msg->param.call_state, msg->param.voice_vsid );
        if (msg->type == QCRIL_AM_INTER_RILD_EVENT_REQUEST)
        {
            if (msg->rild_id < num_of_rilds)
            {
                qcril_am_lock();
                am_state.pending_ril_req[msg->rild_id].is_valid = TRUE;
                am_state.pending_ril_req[msg->rild_id].audio_api_param = msg->param;
                qcril_am_unlock();
                qcril_am_handle_pending_req();
            }
            else
            {
                QCRIL_LOG_ERROR("num_of_rilds(%d) <= msg->rild_id", num_of_rilds);
            }

            // Response required only in case of LOCAL_HOLD
            if (msg->param.call_state == qcril::interfaces::AudioCallState::LOCAL_HOLD)
            {
                resp_to_ril_id = msg->rild_id;
                resp_msg.type = QCRIL_AM_INTER_RILD_EVENT_RESPONSE;
                resp_msg.rild_id = qmi_ril_get_process_instance_id();
                resp_msg.param = msg->param;
                QCRIL_LOG_INFO( "sending response to rild_id: %d, call_state: %d",
                                resp_to_ril_id, resp_msg.param.call_state );
                auto ipcMsg = std::make_shared<IpcAudioCallStateMessage>(resp_msg);
                if (ipcMsg) {
                  ipcMsg->broadcast();
                }
            }
        }
        else if (msg->type == QCRIL_AM_INTER_RILD_EVENT_RESPONSE)
        {
            if (msg->param.call_state == qcril::interfaces::AudioCallState::LOCAL_HOLD)
            {
                qcril_am_inform_am_lch_complete();
            }
        }
    }
    else
    {
        QCRIL_LOG_ERROR("Unexpected NULL data");
    }
}

/***************************************************************************************************
    @function
    qcril_am_audio_system_error_callback_helper
***************************************************************************************************/
static void qcril_am_audio_system_error_callback_helper(void * param)
{
    QCRIL_NOTUSED(param);
    QCRIL_LOG_INFO("Media Server Restarted: reset the VSID and call state");

    qcril_am_handle_event(QCRIL_AM_EVENT_MEDIA_SERVER_INITIALIZED, NULL);
} // qcril_am_audio_system_error_callback_helper

/***************************************************************************************************
    @function
    qcril_am_audio_system_error_callback
***************************************************************************************************/
void qcril_am_audio_system_error_callback()
{
    QCRIL_LOG_ESSENTIAL("QCRIL_ERROR:AUDIO: qcril_am_audio_system_error_callback");
    qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            qcril_am_audio_system_error_callback_helper,
            NULL,  // immediate
            NULL );
}

/***************************************************************************************************
    @function
    qcril_am_pre_init
***************************************************************************************************/
void qcril_am_pre_init()
{
    char prop_str[ PROPERTY_VALUE_MAX ];
    *prop_str = 0;

    property_get( QMI_RIL_SYS_PROP_NAME_MULTI_SIM, prop_str, "" );
    if ( strncmp(prop_str, "dsda", QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM ) == 0)
    {
        is_dsda = TRUE;
    }

    if (QCRIL_DEFAULT_INSTANCE_ID == qmi_ril_get_process_instance_id())
    {
        is_prim_rild = TRUE;
    }
    // initialize am_state.emergency_rat to UNKNOWN
    am_state.emergency_rat = CALL_MODE_UNKNOWN_V02;

    num_of_rilds = qmi_ril_retrieve_number_of_rilds();

    bool bool_config;
    if (qcril_config_get(PERSIST_VENDOR_RADIO_IGNORE_IMS_WLAN, bool_config) == E_SUCCESS)
    {
        ignore_ims_wlan = bool_config;
    }

    //ProcessState::self()->startThreadPool();
}

/***************************************************************************************************
    @function
    qcril_am_set_call_audio_driver

    @implementation detail
    Maps vs_type and call_state to the value defined in AudioSystem.h,
    and calls setParameters API of AudioSystem
***************************************************************************************************/
RIL_Errno qcril_am_set_call_audio_driver
(
    qcril_am_vs_type vs_type,
    AudioCallState call_state,
    call_mode_enum_v02 call_mode
)
{
    QCRIL_LOG_FUNC_ENTRY();
    RIL_Errno err = RIL_E_SUCCESS;
    uint32_t audio_vsid;

    do
    {
        QCRIL_LOG_INFO("vs_type: %d, call_state: %d, call_mode: %d",
                                        vs_type, call_state, call_mode);
        if ( vs_type <= QCRIL_AM_VS_MIN || vs_type >= QCRIL_AM_VS_MAX ||
             call_state == qcril::interfaces::AudioCallState::INVALID )
        {
            err = RIL_E_REQUEST_NOT_SUPPORTED;
            break;
        }

        err = qcril_am_get_audio_vsid(vs_type, &audio_vsid);
        if (RIL_E_SUCCESS != err)
        {
            break;
        }

        // In case of single VSID session (same VSID used for Voice/IMS/WLAN),
        // RIL should set call as INACTIVE to audio only if there is no calls
        // active with the VSID.
        if ((call_state == qcril::interfaces::AudioCallState::INACTIVE) &&
                qcril_am_is_any_non_inactive_calls_with_vsid(audio_vsid))
        {
          QCRIL_LOG_INFO("Active calls with VSID %d in progress, do not set INACTIVE",
                         audio_vsid);
          err = RIL_E_CANCELLED;
          break;
        }

        call_state  = qcril_am_get_audio_call_state(call_state);
    } while (FALSE);

    if (RIL_E_SUCCESS == err )
    {
        if (!is_prim_rild && is_dsda && QCRIL_AM_VS_VOICE == vs_type)
        {
            qcril_am_inter_rild_msg_type msg;
            msg.type = QCRIL_AM_INTER_RILD_EVENT_REQUEST;
            msg.rild_id = qmi_ril_get_process_instance_id();
            msg.param.call_state = call_state;
            msg.param.voice_vsid = audio_vsid;
            msg.param.call_mode = call_mode;
            QCRIL_LOG_INFO( "sending rild_id: %d, call_state: %d, voice_vsid: %d, call_mode: %d",
                            msg.rild_id, msg.param.call_state, msg.param.voice_vsid, msg.param.call_mode );
            auto ipcMsg = std::make_shared<IpcAudioCallStateMessage>(msg);
            if (ipcMsg) {
              ipcMsg->broadcast();
            }
        }
        else if (is_prim_rild && is_dsda && QCRIL_AM_VS_VOICE == vs_type)
        {
            qcril_am_lock();
            am_state.pending_ril_req[0].is_valid = TRUE;
            am_state.pending_ril_req[0].audio_api_param.voice_vsid = audio_vsid;
            am_state.pending_ril_req[0].audio_api_param.call_state = call_state;
            am_state.pending_ril_req[0].audio_api_param.call_mode = call_mode;
            qcril_am_handle_pending_req();
            qcril_am_unlock();
        }
        else
        {
            std::vector<QcRilSetAudioParams>
                    audio_params{QcRilSetAudioParams(audio_vsid, call_state, call_mode)};
            std::shared_ptr<RIL_Errno> respPtr {nullptr};
            auto setParamMsg = std::make_shared<QcRilSetAudioParamSyncMessage>(audio_params);
            if (setParamMsg != nullptr){
                auto ret = setParamMsg->dispatchSync(respPtr);
                if((ret == Message::Callback::Status::SUCCESS) && (respPtr != nullptr)) {
                    QCRIL_LOG_INFO("AudioSystem::setParameters return status: %d", *respPtr);
                }
                else {
                    QCRIL_LOG_INFO("setParameters returned %d", ret);
                }
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)err);
    return err;
}

/***************************************************************************************************
    @function
    qcril_am_state_reset
***************************************************************************************************/
void qcril_am_state_reset()
{
    qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                   qcril::interfaces::AudioCallState::INACTIVE,
                                   CALL_MODE_UNKNOWN_V02);
    qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                   qcril::interfaces::AudioCallState::INACTIVE,
                                   CALL_MODE_UNKNOWN_V02);
    qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                   qcril::interfaces::AudioCallState::INACTIVE,
                                   CALL_MODE_UNKNOWN_V02);
    qcril_am_lock();
    am_state.ims_on_wlan = FALSE;
    am_state.emergency_rat = CALL_MODE_UNKNOWN_V02;
    qcril_am_unlock();
}

/***************************************************************************************************
    @function
    qcril_am_handle_event
***************************************************************************************************/
RIL_Errno qcril_am_handle_event(qcril_am_event_type event, const void* data)
{
    RIL_Errno err = RIL_E_SUCCESS;

    const char *EVT_LOG_STR[] = { "EVENT_MIN",
                                  "IMS_ANSWER",
                                  "IMS_ANSWER_FAIL",
                                  "VOICE_ANSWER",
                                  "VOICE_ANSWER_FAIL",
                                  "SWITCH_CALL",
                                  "SWITCH_CALL_FAIL",
                                  "CALL_STATE_CHANGED",
                                  "SRVCC_START",
                                  "SRVCC_COMPLETE",
                                  "SRVCC_FAIL",
                                  "SRVCC_CANCEL",
                                  "DRVCC_START",
                                  "DRVCC_COMPLETE",
                                  "DRVCC_FAIL",
                                  "DRVCC_CANCEL",
                                  "IMS_SRV_CHANGED",
                                  "IMS_HANDOVER",
                                  "AUDIO_RAT_CHANGED",
                                  "LCH",
                                  "UNLCH",
                                  "INTER_RIL_CALL_STATE",
                                  "MEDIA_SERVER_DIED",
                                  "EVENT_MAX" };

    if ( QCRIL_AM_EVENT_MIN <= event && event < sizeof(EVT_LOG_STR)/sizeof(EVT_LOG_STR[0]) )
    {
        QCRIL_LOG_INFO("processing event: %s", EVT_LOG_STR[event]);
    }
    else
    {
        QCRIL_LOG_INFO("processing UNKNOWN event: %d", event);
    }

    switch (event)
    {
    case QCRIL_AM_EVENT_IMS_ANSWER:
        if(am_state.ims_on_wlan)
        {
          QCRIL_LOG_INFO( "Answer IMS call on WLAN");
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                    qcril::interfaces::AudioCallState::ACTIVE,
                                    qcril_qmi_voice_get_answer_call_mode());
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
        }
        else
        {
          QCRIL_LOG_INFO( "Answer IMS call");
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                    qcril::interfaces::AudioCallState::ACTIVE,
                                    qcril_qmi_voice_get_answer_call_mode());
          err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                               qcril::interfaces::AudioCallState::INACTIVE,
                                               CALL_MODE_UNKNOWN_V02);
        }
        break;

    case QCRIL_AM_EVENT_VOICE_ANSWER:
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_VOICE,
                                   qcril::interfaces::AudioCallState::ACTIVE,
                                   qcril_qmi_voice_get_answer_call_mode());
        break;

    case QCRIL_AM_EVENT_IMS_ANSWER_FAIL:
        err = qcril_am_set_ims_call_audio_driver_by_call_state();
        break;

    case QCRIL_AM_EVENT_VOICE_ANSWER_FAIL:
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        break;

    case QCRIL_AM_EVENT_SWITCH_CALL:
    case QCRIL_AM_EVENT_SWITCH_CALL_FAIL:
    case QCRIL_AM_EVENT_CALL_STATE_CHANGED:
    case QCRIL_AM_EVENT_MEDIA_SERVER_INITIALIZED:
        // Need to set the active VSID first to ensure the existing voice
        // session to continue with the updated VSID if there is any change
        // in VSID(domain) of the call.
        if (qcril_qmi_voice_has_specific_call(qcril_am_is_active_ims_call, NULL) ||
            qcril_qmi_voice_has_specific_call(qcril_am_is_hold_ims_call, NULL))
        {
          err = qcril_am_set_ims_call_audio_driver_by_call_state();
          err = qcril_am_set_voice_call_audio_driver_by_call_state();
        }
        else
        {
          err = qcril_am_set_voice_call_audio_driver_by_call_state();
          err = qcril_am_set_ims_call_audio_driver_by_call_state();
        }
        break;

    case QCRIL_AM_EVENT_SRVCC_START:
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        break;

    case QCRIL_AM_EVENT_SRVCC_COMPLETE:
        //no action
        break;

    case QCRIL_AM_EVENT_SRVCC_FAIL:
    case QCRIL_AM_EVENT_SRVCC_CANCEL:
        err = qcril_am_set_ims_call_audio_driver_by_call_state();
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        break;

    case QCRIL_AM_EVENT_DRVCC_START:
    case QCRIL_AM_EVENT_DRVCC_FAIL:
    case QCRIL_AM_EVENT_DRVCC_CANCEL:
        //no action
        break;

    case QCRIL_AM_EVENT_DRVCC_COMPLETE:
        err = qcril_am_set_voice_call_audio_driver_by_call_state();
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        err = qcril_am_set_call_audio_driver(QCRIL_AM_VS_IMS_WLAN,
                                             qcril::interfaces::AudioCallState::INACTIVE,
                                             CALL_MODE_UNKNOWN_V02);
        //no action
        break;

    case QCRIL_AM_EVENT_IMS_HANDOVER:
        qcril_am_set_ims_on_wlan_from_handover(*(imsa_service_rat_enum_v01*)data);
        // On MultiSim targets, IMS will be running on the SUB which is L capable and
        // set as DDS. Update the call states to Audio only if this SUB is IMS capable.
        if((num_of_rilds == 1) || qcril_qmi_nas_is_ims_available())
        {
          qcril_am_set_ims_call_audio_driver_by_call_state(TRUE);
        }
        break;

    case QCRIL_AM_EVENT_AUDIO_RAT_CHANGED:
        qcril_am_handle_audio_rat_change(
                (voice_audio_rat_change_info_ind_msg_v02*)data);
        break;

    case QCRIL_AM_EVENT_LCH:
        qcril_am_lock();
        am_state.in_lch = TRUE;
        qcril_am_unlock();
        qcril_am_set_voice_call_audio_driver_by_call_state();

        // Inform AM_LCH_COMPLETE immediately in case of single
        // sim (prim_rild) or multi sim cases other than DSDA.
        // In case of DSDA, wait for the INTER_RIL_CALL_STATE resp.
        if (is_prim_rild || !is_dsda ||
                !qcril_qmi_voice_has_specific_call(qcril_am_is_active_voice_call, NULL))
        {
            qcril_am_inform_am_lch_complete();
        }
        break;

    case QCRIL_AM_EVENT_UNLCH:
        qcril_am_lock();
        am_state.in_lch = FALSE;
        qcril_am_unlock();
        qcril_am_set_voice_call_audio_driver_by_call_state();
        break;

    case QCRIL_AM_EVENT_INTER_RIL_CALL_STATE:
        qcril_am_process_event_inter_ril_call_state((const qcril_am_inter_rild_msg_type*)data);
        break;

    default:
        QCRIL_LOG_DEBUG("ignore unexpected event");
        err = RIL_E_REQUEST_NOT_SUPPORTED;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)err);
    return err;
}

/***************************************************************************************************
    @function
    qcril_am_set_vsid
***************************************************************************************************/
RIL_Errno qcril_am_set_vsid(qcril_am_vs_type vs_type, uint32_t vsid)
{
    QCRIL_LOG_INFO("set vs_type: %d to vsid: %u", vs_type, vsid);
    RIL_Errno err = RIL_E_SUCCESS;

    switch (vs_type)
    {
    case QCRIL_AM_VS_VOICE:
        qcril_am_lock();
        am_state.voice_vsid.is_valid = TRUE;
        am_state.voice_vsid.vsid = vsid;
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_IMS:
        qcril_am_lock();
        am_state.lte_vsid.is_valid = TRUE;
        am_state.lte_vsid.vsid = vsid;
        qcril_am_unlock();
        break;
    case QCRIL_AM_VS_IMS_WLAN:
        qcril_am_lock();
        am_state.wlan_vsid.is_valid = TRUE;
        am_state.wlan_vsid.vsid = vsid;
        qcril_am_unlock();
        break;
    default:
        err = RIL_E_REQUEST_NOT_SUPPORTED;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET((int)err);
    return err;
}


/* API for testing only */
boolean qcril_am_get_ims_on_wlan()
{
  qcril_am_lock();
  boolean ims_on_wlan = am_state.ims_on_wlan;
  qcril_am_unlock();
  return ims_on_wlan;
}

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_qcril_am_cleanup()
{
  memset(&am_state, 0, sizeof(am_state));
}
#endif
