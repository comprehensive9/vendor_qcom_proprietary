/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef LOC_QMI_WDS_H
#define LOC_QMI_WDS_H

#include "qmi_client.h"
#include "gps_extended.h"
#include "wireless_data_service_v01.h"

//Timeout for the service to respond to sync msg
#define DS_CLIENT_SYNC_MSG_TIMEOUT (5000)

class LocQmiWds {
private:
    LocQmiWds() {}
    static LocQmiWds* sInstance;
    qmi_client_type mWdsQmiClientHandle;
    void locQmiWdsInitClient(qmi_client_type& qmiSvcClient);
    void locQmiWdsReleaseClient(qmi_client_type& qmiSvcClient);
    void locQmiBindSubscription(qmi_client_type& qmiSvcClient, int qcmapSubId);
    qmi_client_error_type locQmiWdsGetProfileList(qmi_client_type& qmiSvcClient,
            wds_get_profile_list_req_msg_v01& profListReq,
            wds_get_profile_list_resp_msg_v01& profListResp,
            wds_profile_type_enum_v01 profileType);
    qmi_client_error_type locQmiWdsGetProfileSettings(qmi_client_type& qmiSvcClient,
            wds_get_profile_settings_req_msg_v01&   profSettingReq,
            wds_get_profile_settings_resp_msg_v01&  profSettingResp,
            wds_profile_identifier_type_v01 *profileIdentifier);
public:
    static LocQmiWds* getInstance() {
        if (nullptr == sInstance) {
            sInstance = new LocQmiWds();
        }
        return sInstance;
    }
    void queryPdnType(const std::string& apnName, std::function<void(int)> pdnCb);
    std::string queryEsProfileIndex(int qcmapSubId, uint8_t& esProfileIndex);
};

void getPdnType(const std::string& apnName, std::function<void(int)> pdnCb);
#endif //LOC_QMI_WDS_H
