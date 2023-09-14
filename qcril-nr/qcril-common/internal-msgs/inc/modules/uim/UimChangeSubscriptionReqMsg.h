/******************************************************************************
#  Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"

/*There are only two provisioning session types(GW, 1X)*/
#define RIL_UIM_MAX_PROVISIONING_INFO_LEN 2

/* These are only used when qcril_provisioning_response_info has response_info_len as 2*/
#define RESPONSE_INDEX_GW 0
#define RESPONSE_INDEX_1X 1

typedef enum {
  UIM_UICC_SUBSCRIPTION_DEACTIVATE = 0,
  UIM_UICC_SUBSCRIPTION_ACTIVATE   = 1
} RIL_UIM_UiccSubActStatus;

typedef enum {
  UIM_SUBSCRIPTION_1 = 0,
  UIM_SUBSCRIPTION_2 = 1,
  UIM_SUBSCRIPTION_3 = 2
} RIL_UIM_SubscriptionType;

typedef enum {
  QCRIL_SUBS_MODE_GW    = 1,
  QCRIL_SUBS_MODE_1X    = 2,

  /* Indicates both GW and 1X */
  QCRIL_SUBS_MODE_GW_1X = 3,
  QCRIL_SUBS_MODE_UNKNOWN = 0xFF
} qcril_uim_subs_mode_pref;

typedef enum
{
  UIM_DEACTIVATE_PROVISION_STATUS,
  UIM_ACTIVATE_PROVISION_STATUS,
} uim_provision_status_type;

typedef enum
{
  UIM_SESSION_TYPE_PRI_GW_PROV          = 0,
  UIM_SESSION_TYPE_PRI_1X_PROV          = 1,
  UIM_SESSION_TYPE_SEC_GW_PROV          = 2,
  UIM_SESSION_TYPE_SEC_1X_PROV          = 3,
  UIM_SESSION_TYPE_NON_PROV_SLOT_1      = 4,
  UIM_SESSION_TYPE_NON_PROV_SLOT_2      = 5,
  UIM_SESSION_TYPE_CARD_SLOT_1          = 6,
  UIM_SESSION_TYPE_CARD_SLOT_2          = 7,
  UIM_SESSION_TYPE_TER_GW_PROV          = 10,
  UIM_SESSION_TYPE_TER_1X_PROV          = 11,
  UIM_SESSION_TYPE_NON_PROV_SLOT_3      = 16,
  UIM_SESSION_TYPE_CARD_SLOT_3          = 19
} uim_session_type;

/* Provision status */
typedef enum
{
  QCRIL_PROVISION_STATUS_SUCCESS     = 0, /*!< Provision success */
  QCRIL_PROVISION_STATUS_FAILURE     = 1, /*!< Provision failure */
} qcril_provision_status_e_type;

/*Structures used for requests */
typedef struct {
  uint8_t  slot;                         /* 0, 1, ... etc. */
  RIL_UIM_SubscriptionType  sub_type;    /* Indicates subscription 0 or subscription 1 */
  RIL_UIM_UiccSubActStatus  act_status;
} RIL_UIM_SelectUiccSub;

typedef struct {
  RIL_UIM_SelectUiccSub    uicc_subs_info;
  qcril_uim_subs_mode_pref subs_mode_pref;
} qcril_uim_uicc_subs_info_type;

/* Structures used for responses */
typedef struct
{
  qcril_provision_status_e_type  status;       /* Status of the provision */
  uim_session_type               session_type; /* UIM session type on which this subscription is
                                                  currently provisioned */
  uint32_t                       err_code;     /* RIL_Errno */
} qcril_provisioning_response_info_type;

typedef struct
{
  uint8_t response_info_len;
  qcril_provisioning_response_info_type response_info[RIL_UIM_MAX_PROVISIONING_INFO_LEN];
} qcril_provisioning_response_info;

/*===========================================================================

  CLASS:  UimChangeSubscriptionReqMsg

===========================================================================*/
/*!
    @brief
    Unsol UIM Message request for activate and deactivate subscription.

    0 - Deactivate
    1 - Activate

    @return
    None.
*/
/*=========================================================================*/
class UimChangeSubscriptionReqMsg : public SolicitedMessage<qcril_provisioning_response_info>,
                                    public add_message_id<UimChangeSubscriptionReqMsg>
{
  private:
    qcril_uim_uicc_subs_info_type    mSubInfo;
    qcril_provisioning_response_info mProvisioningResponseInfo = {};

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.change_provision_request";
    UimChangeSubscriptionReqMsg() = delete;
    ~UimChangeSubscriptionReqMsg() = default;

    inline UimChangeSubscriptionReqMsg(const qcril_uim_uicc_subs_info_type &sub_info):
                                  SolicitedMessage<qcril_provisioning_response_info>(get_class_message_id())
    {
      mSubInfo = sub_info;
      mName = MESSAGE_NAME;
    }

    inline string dump()
    {
      string log = "Mode Pref: " + std::to_string(mSubInfo.subs_mode_pref);

      log = log + "Slot: " + std::to_string(mSubInfo.uicc_subs_info.slot);
      log = log + "subscription: " + std::to_string(mSubInfo.uicc_subs_info.sub_type);
      log = log + "Activation req: " + std::to_string(mSubInfo.uicc_subs_info.act_status);

      return mName + "{}" + log;
    }

    inline const qcril_uim_uicc_subs_info_type *get_sub_info(void)
    {
      return &mSubInfo;
    }

    inline void set_provisioning_response_info(qcril_provisioning_response_info_type response_info)
    {
      if(mSubInfo.subs_mode_pref == QCRIL_SUBS_MODE_GW_1X)
      {
        if(response_info.session_type == UIM_SESSION_TYPE_PRI_GW_PROV ||
           response_info.session_type == UIM_SESSION_TYPE_SEC_GW_PROV ||
           response_info.session_type == UIM_SESSION_TYPE_TER_GW_PROV)
        {
          mProvisioningResponseInfo.response_info[RESPONSE_INDEX_GW] = response_info;
          mProvisioningResponseInfo.response_info_len++;
        }
        else if(response_info.session_type == UIM_SESSION_TYPE_PRI_1X_PROV ||
                response_info.session_type == UIM_SESSION_TYPE_SEC_1X_PROV ||
                response_info.session_type == UIM_SESSION_TYPE_TER_1X_PROV)
        {
          mProvisioningResponseInfo.response_info[RESPONSE_INDEX_1X] = response_info;
          mProvisioningResponseInfo.response_info_len++;
        }
      }
      else
      {
        mProvisioningResponseInfo.response_info[0] = response_info;
        mProvisioningResponseInfo.response_info_len++;
      }
    }

    inline qcril_provisioning_response_info get_provisioning_response_info(void)
    {
      return mProvisioningResponseInfo;
    }

};
