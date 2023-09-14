/******************************************************************************
#  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include <framework/GenericCallback.h>
#include <modules/android/RilRequestMessage.h>
#include "request/SetLinkCapRptCriteriaMessage.h"
#include <DataCommon.h>
#include "modules/nas/NasUimHeaders.h"
#include "modules/nas/qcril_qmi_prov.h"

class NasDataGetDdsSubIdCallback : public GenericCallback<DDSSubIdInfo> {
private:
  uint32_t mMsgToken;  // use uint32_t to store uint16 token. Use uint32_t (-1) to indicate an invalid token
  bool from_allow_data;
public:
  static constexpr uint32_t INVALID_MSG_TOKEN = (uint32_t)(-1);
  inline NasDataGetDdsSubIdCallback(string clientToken,
        uint32_t msgToken,  boolean from_allow_data):
        GenericCallback(clientToken), mMsgToken(msgToken), from_allow_data(from_allow_data) {};

  inline ~NasDataGetDdsSubIdCallback() {};

  Message::Callback *clone() override;

  void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
      std::shared_ptr<DDSSubIdInfo> responseDataPtr) override;
};

class NasDataRequestDdsSwitchCallback : public GenericCallback<RIL_Errno> {
private:
    uint32_t mMsgToken;  // use uint32_t to store uint16 token. Use uint32_t (-1) to indicate an invalid token
public:
    inline NasDataRequestDdsSwitchCallback(string clientToken,
        uint32_t msgToken) :
        GenericCallback(clientToken), mMsgToken(msgToken) {};

    inline ~NasDataRequestDdsSwitchCallback() {};

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<RIL_Errno> responseDataPtr) override;
};

// TODO move to a separate file later
class NasDataSetLinkCapRptFilterCallback : public GenericCallback<int> {
private:
        legacy_request_payload params;
  public:
    inline NasDataSetLinkCapRptFilterCallback(string clientToken,
        const qcril_request_params_type &p) :
          GenericCallback(clientToken), params(p) {};

    inline ~NasDataSetLinkCapRptFilterCallback() {};

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<int> responseDataPtr) override;

    qcril_request_params_type &get_params() {
        return params.get_params();
    }
};

class NasProvRequestActivateCallback : public GenericCallback<qcril_provisioning_response_info> {
private:
  uint32_t mMsgToken;  // use uint32_t to store uint16 token. Use uint32_t (-1) to indicate an invalid token
public:
  static constexpr uint32_t INVALID_MSG_TOKEN = (uint32_t)(-1);
  inline NasProvRequestActivateCallback(string clientToken,
        uint32_t msgToken):
        GenericCallback(clientToken), mMsgToken(msgToken) {};

  inline ~NasProvRequestActivateCallback() {};

  Message::Callback *clone() override;

  void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
      std::shared_ptr<qcril_provisioning_response_info> responseDataPtr) override;
};

class NasProvRequestDeactivateCallback : public GenericCallback<qcril_provisioning_response_info> {
private:
  uint32_t mMsgToken;  // use uint32_t to store uint16 token. Use uint32_t (-1) to indicate an invalid token
public:
  static constexpr uint32_t INVALID_MSG_TOKEN = (uint32_t)(-1);
  inline NasProvRequestDeactivateCallback(string clientToken,
        uint32_t msgToken):
        GenericCallback(clientToken), mMsgToken(msgToken) {};

  inline ~NasProvRequestDeactivateCallback() {};

  Message::Callback *clone() override;

  void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
      std::shared_ptr<qcril_provisioning_response_info> responseDataPtr) override;
};
