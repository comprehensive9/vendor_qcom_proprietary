/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_wds"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <math.h>
#include <dlfcn.h>
#include <algorithm>
#include <thread>

#include <log_util.h>
#include "LocQmiWds.h"
#include "qmi_idl_lib.h"
#ifdef USE_GLIB
#include "qualcomm_mobile_access_point_msgr_v01.h"
#endif

using namespace std;
LocQmiWds* LocQmiWds::sInstance = nullptr;
#ifndef FEATURE_DSDA_ENABLED
typedef enum {
    QCMAP_MSGR_SUBSCRIPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647,
    QCMAP_MSGR_DEFAULT_SUBS_V01 = 0, /**<  Default Subscription \n  */
    QCMAP_MSGR_PRIMARY_SUBS_V01 = 1, /**<  Primary Subscription \n  */
    QCMAP_MSGR_SECONDARY_SUBS_V01 = 2, /**<  Secondary Subscription \n  */
    QCMAP_MSGR_SUBSCRIPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647
} qcmap_msgr_subscription_enum_v01;
#endif // FEATURE_MOBILEAP_INDICATION

wds_profile_pdn_type_enum_v01 convertFromPdpType(wds_pdp_type_enum_v01 pdpType) {
    switch (pdpType) {
        case WDS_PDP_TYPE_PDP_IPV4_V01:
            return WDS_PROFILE_PDN_TYPE_IPV4_V01;
        case WDS_PDP_TYPE_PDP_IPV6_V01:
            return WDS_PROFILE_PDN_TYPE_IPV6_V01;
        case WDS_PDP_TYPE_PDP_IPV4V6_V01:
        default:
            return WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01;
    }
}

  /**
   * This callback function is called by the QCCI infrastructure when
   * infrastructure receives an indication for this client.
   * NOTE: Callback happens in the QCCI thread context and raises signal.
   * @param[in]  user_handle         Opaque handle used by the infrastructure to
   *                                identify different services.
   * @param[in]   msg_id              Message ID of the indication
   * @param[in]  ind_buf              Buffer holding the encoded indication
   * @param[in]  ind_buf_len          Length of the encoded indication
   * @param[in]  ind_cb_data          Cookie value supplied by the client during registration
   */
static void wdsClientIndCb(qmi_client_type user_handle, unsigned int msg_id,
        void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data) {
    LOC_LOGv("msg_id: %u", msg_id);
    if (ind_cb_data != nullptr) {
        uint32_t decoded_payload_len = 0;
        unsigned char* decoded_payload = nullptr;
        qmi_client_error_type qmi_err = QMI_CLIENT_PARAM_ERR;

        // decode QMI payload
        qmi_idl_get_message_c_struct_len(
                wds_get_service_object_v01(),
                QMI_IDL_INDICATION,
                msg_id,
                &decoded_payload_len);
        if (decoded_payload_len) {
            decoded_payload = new unsigned char[decoded_payload_len];
            if (decoded_payload != nullptr) {
                qmi_err = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                        msg_id, ind_buf, ind_buf_len, decoded_payload,
                        (int)decoded_payload_len);
            }
        }

        if (decoded_payload != nullptr) {
            delete[] decoded_payload;
        }
    } else {
        LOC_LOGe("Unexpected: no right CB data for QMI ind");
    }
}

void LocQmiWds::locQmiWdsInitClient(qmi_client_type& qmiSvcClient) {
    LOC_LOGv("enter");
    qmi_client_type clnt, notifier = nullptr;
    bool notifierInitFlag = false;
    qmi_client_error_type rc = QMI_NO_ERR;
    qmi_idl_service_object_type wdsObject = wds_get_service_object_v01();
    // os_params must stay in the same scope as notifier
    // because when notifier is initialized, the pointer
    // of os_params is retained in QMI framework, and it
    // used when notifier is released.
    qmi_client_os_params os_params;
    // instances of this service
    qmi_service_info serviceInfo = {};

    do {
        // register for service notification
        rc = qmi_client_notifier_init(wdsObject, &os_params, &notifier);
        notifierInitFlag = (NULL != notifier);

        if (rc != QMI_NO_ERR) {
            LOC_LOGE("%s:%d]: qmi_client_notifier_init failed %d\n",
                    __func__, __LINE__, rc);
            break;
        }

        while (1) {
            QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

            // lookup service with any instance id
            rc = qmi_client_get_any_service(wdsObject, &serviceInfo);
            // get the service addressing information
            LOC_LOGV("%s:%d]: qmi_client_get_service() rc: %d ", __func__, __LINE__, rc);

            if (rc == QMI_NO_ERR)
                break;

            QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
        }

        // initialize the client
        //sent the address of the first service found
        // if IPC router is present, this will go to the service instance
        // enumerated over IPC router, else it will go over the next transport where
        // the service was enumerated.
        rc = qmi_client_init(&serviceInfo, wdsObject, (qmi_client_ind_cb)wdsClientIndCb, NULL,
                NULL, &clnt);

        if (rc != QMI_NO_ERR) {
            LOC_LOGE("%s:%d]: qmi_client_init error %d\n",
                    __func__, __LINE__, rc);
            // client init failure
            if (clnt) {
                LOC_LOGe("Release WDS QMI handle");
                qmi_client_release(clnt);
                clnt = nullptr;
                return;
            }
        } else {
            LOC_LOGv(" No Error in client init");
            qmiSvcClient = clnt;
        }
    } while (0);

    /* release the notifier handle */
    if (true == notifierInitFlag) {
        qmi_client_release(notifier);
    }
}

void LocQmiWds::locQmiWdsReleaseClient(qmi_client_type& qmiSvcClient) {
  LOC_LOGd("qmiSvcClient:  %p\n", qmiSvcClient);
    // release client
    if (qmiSvcClient) {
      qmi_client_error_type rc = qmi_client_release(qmiSvcClient);
      if (rc != QMI_NO_ERR) {
        LOC_LOGe("Failed to release wds client");
      } else {
        qmiSvcClient = nullptr;
        LOC_LOGv("Released wds client");
      }
    }
}

#ifdef USE_GLIB
void LocQmiWds::locQmiBindSubscription(qmi_client_type& qmiSvcClient, int qcmapSubId) {
    // bind to the subId before getting the list
    wds_bind_subscription_enum_v01 qmiSubId;
    switch (qcmapSubId) {
    case QCMAP_MSGR_PRIMARY_SUBS_V01: qmiSubId = WDS_PRIMARY_SUBS_V01; break;
    case QCMAP_MSGR_SECONDARY_SUBS_V01: qmiSubId = WDS_SECONDARY_SUBS_V01; break;
    case QCMAP_MSGR_DEFAULT_SUBS_V01:
    default: qmiSubId = WDS_DEFAULT_SUBS_V01; break;
    }

    LOC_LOGv("Bind subscription to subId=%d", qmiSubId);
    wds_bind_subscription_req_msg_v01   req;
    wds_bind_subscription_resp_msg_v01  resp;
    qmi_client_error_type qmi_ret = QMI_NO_ERR;

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.subscription = qmiSubId;

    qmi_ret = qmi_client_send_msg_sync(
                    qmiSvcClient,
                    QMI_WDS_BIND_SUBSCRIPTION_REQ_V01,
                    (void *)&req,
                    sizeof(req),
                    (void*)&resp,
                    sizeof(resp),
                    DS_CLIENT_SYNC_MSG_TIMEOUT);

    LOC_LOGv("Exit, qmi_client_send_msg_sync returned: %d", qmi_ret);
}
#endif

qmi_client_error_type LocQmiWds::locQmiWdsGetProfileList(qmi_client_type& qmiSvcClient,
        wds_get_profile_list_req_msg_v01& profListReq,
        wds_get_profile_list_resp_msg_v01& profListResp,
        wds_profile_type_enum_v01 profileType) {
    memset(&profListResp, 0, sizeof(profListResp));
    profListReq.profile_type_valid = 1;
    profListReq.profile_type = profileType;

    // NEXT call goes out to modem. We log the callflow before it
    // actually happens to ensure the this comes before resp callflow
    // back from the modem, to avoid confusing log order. We trust
    // that the QMI framework is robust.
    qmi_client_error_type rc = qmi_client_send_msg_sync(
            qmiSvcClient,
            QMI_WDS_GET_PROFILE_LIST_REQ_V01,
            (void*)&profListReq,
            sizeof(wds_get_profile_list_req_msg_v01),
            (void*)&profListResp,
            sizeof(wds_get_profile_list_resp_msg_v01),
            5000);

    LOC_LOGv("qmi_client_send_msg_sync returned %d", rc);
    return rc;
}

qmi_client_error_type LocQmiWds::locQmiWdsGetProfileSettings(qmi_client_type& qmiSvcClient,
        wds_get_profile_settings_req_msg_v01&   profSettingReq,
        wds_get_profile_settings_resp_msg_v01&  profSettingResp,
        wds_profile_identifier_type_v01 *profileIdentifier) {
    LOC_LOGd("enter");
    memset(&profSettingReq, 0, sizeof(profSettingReq));
    memset(&profSettingResp, 0, sizeof(profSettingResp));
    profSettingReq.profile.profile_type = profileIdentifier->profile_type;
    profSettingReq.profile.profile_index = profileIdentifier->profile_index;

    qmi_client_error_type rc = qmi_client_send_msg_sync(
            mWdsQmiClientHandle, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
            (void *)&profSettingReq, sizeof(profSettingReq),
            (void *)&profSettingResp, sizeof(profSettingResp), 5000);
    return rc;
}

void LocQmiWds::queryPdnType(const std::string& apnName, std::function<void(int)> pdnCb) {
    locQmiWdsInitClient(mWdsQmiClientHandle);
    LOC_LOGd("mWdsQmiClientHandle %p, apnName: %s", mWdsQmiClientHandle, apnName.c_str());

    qmi_client_error_type rc;
    int pdnType = -1;
    //iterate two WDS profile Type, 0: WDS_PROFILE_TYPE_3GPP_V01, 1: WDS_PROFILE_TYPE_3GPP2_V01
    for (int i=0; i<2; ++i) {
        wds_get_profile_list_req_msg_v01 profListReq;
        wds_get_profile_list_resp_msg_v01 profListResp;
        rc = locQmiWdsGetProfileList(mWdsQmiClientHandle, profListReq, profListResp,
                (wds_profile_type_enum_v01)i);
        if (rc != QMI_NO_ERR) {
            LOC_LOGe("send_msg_sync error: %d\n", rc);
            break;
        }

        LOC_LOGv("Get profile list response succeed");
        LOC_LOGv("get_profile_list for profile_tech: %d, returned profile ids %u",
                profListReq.profile_type, profListResp.profile_list_len);
        for (uint32_t i=0; i<profListResp.profile_list_len; i++) {
            wds_get_profile_settings_req_msg_v01   profSettingReq;
            wds_get_profile_settings_resp_msg_v01  profSettingResp;
            wds_profile_identifier_type_v01 profileIdentifier;
            //copy profile type
            profileIdentifier.profile_type =
                profListResp.profile_list[i].profile_type;
            //copy profile index
            profileIdentifier.profile_index =
                profListResp.profile_list[i].profile_index;
            rc = locQmiWdsGetProfileSettings(mWdsQmiClientHandle, profSettingReq,
                    profSettingResp, &profileIdentifier);
            if (QMI_NO_ERR != rc) {
                LOC_LOGe("failed to send get profile settings request");
                break;
            } else {
                if (QMI_RESULT_SUCCESS_V01 != profSettingResp.resp.result) {
                    LOC_LOGe(" get profile settings response returned failed, err = %d",
                            profSettingResp.resp.result);
                    break;
                }
                else { // successful query of profile entry
                    string qmiApnName;
                    if ((profListReq.profile_type == WDS_PROFILE_TYPE_3GPP_V01 ) &&
                            profSettingResp.apn_name_valid) {
                        qmiApnName = profSettingResp.apn_name;
                    }
                    if ((profListReq.profile_type == WDS_PROFILE_TYPE_3GPP2_V01 ) &&
                            profSettingResp.apn_string_valid) {
                        qmiApnName = profSettingResp.apn_string;
                    }
                    LOC_LOGv("successfully queried profile: %d, profType: %d, Apn name: %s",
                            profSettingReq.profile.profile_index, profListReq.profile_type,
                            qmiApnName.c_str());
                    //Do case insensitive matching for apn name
                    if ((qmiApnName.length() == apnName.length()) &&
                            (std::equal(qmiApnName.begin(), qmiApnName.end(), apnName.begin(),
                            [] (char a, char b) {
                                return std::tolower(a)==std::tolower(b);}))) {  //apn name match
                        LOC_LOGv("Match found for apnName: %s", apnName.c_str());
                        if (profListReq.profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
                                profSettingResp.pdp_type_valid) {
                            LOC_LOGv("[pdpType: %d ", profSettingResp.pdp_type);
                            // translate wds_pdp_type_enum_v01 to wds_profile_pdn_type_enum_v01
                            pdnType = convertFromPdpType(profSettingResp.pdp_type);
                            //We only look for the 1st entry in profile db that matches apn name
                            break;
                        } else if (profListReq.profile_type == WDS_PROFILE_TYPE_3GPP2_V01 &&
                                profSettingResp.pdn_type_valid) {
                            LOC_LOGv("[pdnType: %d ", profSettingResp.pdn_type);
                            pdnType = profSettingResp.pdn_type;
                            break;
                        } else {
                            LOC_LOGv("apn type mask not present ");
                        }
                    }
                }
            }
        }
        if (pdnType > -1 ) {
            if (pdnCb) {
                pdnCb(pdnType);
            }
            locQmiWdsReleaseClient(mWdsQmiClientHandle);
            return;
        }
    }
    locQmiWdsReleaseClient(mWdsQmiClientHandle);
    if (pdnCb) {
        pdnCb(pdnType);
    }
}

string LocQmiWds::queryEsProfileIndex(int qcmapSubId, uint8_t& esProfileIndex) {
    locQmiWdsInitClient(mWdsQmiClientHandle);
    LOC_LOGd("mWdsQmiClientHandle %p, qcmapSubId: %d", mWdsQmiClientHandle, qcmapSubId);

#ifdef USE_GLIB
    locQmiBindSubscription(mWdsQmiClientHandle, qcmapSubId);
#endif
    qmi_client_error_type rc;
    wds_profile_identifier_type_v01 profileIdentifier;
    uint32_t i = 0;
    bool esProfileIndexFound = false;

    LOC_LOGv("Getting profile list");
    for (int i=0; i<2; ++i) {
        wds_get_profile_list_req_msg_v01 profListReq;
        wds_get_profile_list_resp_msg_v01 profListResp;
        rc = locQmiWdsGetProfileList(mWdsQmiClientHandle, profListReq, profListResp,
                (wds_profile_type_enum_v01)i);
        if (rc != QMI_NO_ERR) {
            LOC_LOGe("send_msg_sync error: %d\n", rc);
            break;
        }
        LOC_LOGv("Got profile list; length = %d",
                profListResp.profile_list_len);

        //Loop over the list of profiles to find a profile that supports
        //emergency calls
        for (i = 0; i < profListResp.profile_list_len; i++) {
            wds_get_profile_settings_req_msg_v01   profSettingReq;
            wds_get_profile_settings_resp_msg_v01  profSettingResp;
            /*QMI_WDS_GET_PROFILE_SETTINGS_REQ requires an input data
              structure that is of type wds_profile_identifier_type_v01
              We have to fill that structure for each profile from the
              info obtained from the profile list*/
            //copy profile type
            profileIdentifier.profile_type =
                profListResp.profile_list[i].profile_type;
            //copy profile index
            profileIdentifier.profile_index =
                profListResp.profile_list[i].profile_index;

            rc = locQmiWdsGetProfileSettings(mWdsQmiClientHandle, profSettingReq, profSettingResp,
                    &profileIdentifier);
            LOC_LOGd("Got profile setting for profile %d; name: %s", i,
                    profSettingResp.profile_name);

            if (profSettingResp.support_emergency_calls_valid) {
                if (profSettingResp.support_emergency_calls) {
                    LOC_LOGv("Found emergency profile in profile %d", i);
                    esProfileIndexFound = true;
                    esProfileIndex = profileIdentifier.profile_index;
                    if (profSettingResp.apn_name_valid) {
                        LOC_LOGd("apn name: %s",
                                profSettingResp.apn_name);
                        string apnName(profSettingResp.apn_name);
                        return apnName;
                    } else {
                        LOC_LOGd("apn name is not valid");
                        return "";
                    }
                    //Break out of for loop since we found the emergency profile
                    break;
                } else {
                    LOC_LOGe("Emergency profile valid but not supported in profile: %d ", i);
                }
            }
            //Since this struct is loaded with settings for the next profile,
            //it is important to clear out the memory to avoid values/flags
            //from being carried over
            memset(&profSettingResp,
                    0, sizeof(wds_get_profile_settings_resp_msg_v01));
        }

    }
    //Release qmi client handle
    if (QMI_NO_ERR != qmi_client_release(mWdsQmiClientHandle)) {
        LOC_LOGe("Could not release qmi client handle");
    }

    LOC_LOGv("Exit");
    return "";
}

void getPdnType(const std::string& apnName, std::function<void(int)> pdnCb) {
    LOC_LOGv("enter getPdnType");
    std::thread wdsThd([apnName, pdnCb]() {
            LocQmiWds* wdsClient = LocQmiWds::getInstance();
            wdsClient->queryPdnType(apnName, pdnCb);
            });
    wdsThd.detach();
}
