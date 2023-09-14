/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"
#include "AndroidAudioModule.h"
#include <cstring>
extern "C" {
#include "qcril_other.h"
}
#include <interfaces/voice/QcRilUnsolAudioStateChangedMessage.h>
#include <src/hidl_impl/base/qcril_qmi_audio_service.h>

using std::shared_ptr;
using namespace ril::modules;
using AmCallState = qcril::interfaces::AudioCallState;

static load_module<AndroidAudioModule> sAndroidAudioModule;

namespace ril {
namespace modules {

AndroidAudioModule* getAndroidAudioModule() {
  return &(sAndroidAudioModule.get_module());
}

AndroidAudioModule::AndroidAudioModule() {
  mName = "AndroidAudioModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, AndroidAudioModule::handleQcrilInit),
    HANDLER(QcRilGetAudioParamSyncMessage, AndroidAudioModule::handleQcrilGetAudioParameters),
    HANDLER(QcRilSetAudioParamSyncMessage, AndroidAudioModule::handleQcrilSetAudioParameters)
  };
}

AndroidAudioModule::~AndroidAudioModule() {
}

void AndroidAudioModule::init() {
  Module::init();
}

void AndroidAudioModule::handleQcrilInit(shared_ptr<QcrilInitMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();

  /* Init Android Audio service */
  initAndroidAudio(msg->get_instance_id());

  QCRIL_LOG_FUNC_RETURN();
}

void AndroidAudioModule::handleQcrilGetAudioParameters(shared_ptr<QcRilGetAudioParamSyncMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  string params{};
  shared_ptr<QcRilGetAudioParams> resp = nullptr;
  Message::Callback::Status status = Message::Callback::Status::FAILURE;
  static constexpr const char* ALL_CALL_STRING = "all_call_states";

  if (msg != nullptr) {
    if (mAudioService != nullptr) {
      params = mAudioService->getParameters(android::String8(ALL_CALL_STRING)).string();
      QCRIL_LOG_DEBUG("[AndroidAudio]: GetParams returned %s", params.c_str());
      auto index = params.find(msg->getVsid());
      if (index != std::string::npos && (index + msg->getVsid().length() + 1) < params.length()) {
        QCRIL_LOG_DEBUG("[AndroidAudio]: Couldn't find the given vsid");
      } else {
        resp = std::make_shared<QcRilGetAudioParams>();
        if (resp != nullptr) {
          status = Message::Callback::Status::SUCCESS;
          resp->call_state =
              convertAudioNumToCallState(params[index + msg->getVsid().length() + 1] - '0');
          QCRIL_LOG_DEBUG("[AndroidAudio]: Call State for vsid:%s=%d", msg->getVsid().c_str(),
                          resp->call_state);
        }
      }
    }
    msg->sendResponse(msg, status, resp);
  }

  QCRIL_LOG_FUNC_RETURN();
}

void AndroidAudioModule::handleQcrilSetAudioParameters(shared_ptr<QcRilSetAudioParamSyncMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY("msg = %s", (msg ? msg->dump().c_str() : "nullptr"));

  int ret{ 0 };
  shared_ptr<RIL_Errno> resp = std::make_shared<RIL_Errno>(RIL_E_SUCCESS);
  Message::Callback::Status status = Message::Callback::Status::FAILURE;

  if (msg != nullptr) {
    if (mAudioService != nullptr) {
      auto aParams = msg->getAudioParams();
      string oemString = "";
      /*Set the params for Non active first*/
      for (auto& param : aParams) {
        if (param.call_state != AmCallState::ACTIVE) {
          auto paramString = convertRilAudioParamtersToAndroid(param);
          oemString += paramString.string();
          oemString += ";";
          ret = mAudioService->setParameters(paramString);
          if (ret) {
            QCRIL_LOG_DEBUG("Set Parameters error returned: %d", ret);
            break;
          }
        }
      }
      if (!ret) {
        /*Set the params for first active*/
        for (auto& param : aParams) {
          if (param.call_state == AmCallState::ACTIVE) {
            auto paramString = convertRilAudioParamtersToAndroid(param);
            oemString += paramString.string();
            oemString += ";";
            ret = mAudioService->setParameters(paramString);
            break;
          }
        }
#ifndef QMI_RIL_UTF
        /* Send OEM Hook Unsol*/
        auto oemmsg = std::make_shared<QcRilUnsolAudioStateChangedMessage>(oemString);
        if (oemmsg != nullptr) {
          oemmsg->broadcast();
        }
#endif
      }
      if (resp != nullptr) {
        status = Message::Callback::Status::SUCCESS;
        if (ret) {
          *resp = RIL_E_GENERIC_FAILURE;
        }
      }
    }
    msg->sendResponse(msg, status, resp);
  }

  QCRIL_LOG_FUNC_RETURN();
}

void AndroidAudioModule::initAndroidAudio(qcril_instance_id_e_type instance_id) {
  QCRIL_LOG_FUNC_ENTRY();

  if (mAudioService == nullptr) {
    using namespace vendor::qti::hardware::radio::am::V1_0::implementation;

    mAudioService = new qcril_audio_impl(instance_id);
    if (mAudioService != nullptr) {
      mAudioService->registerService();
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}

android::String8 AndroidAudioModule::convertRilAudioParamtersToAndroid(QcRilSetAudioParams params) {
  string aParams = "vsid=" + std::to_string(params.vsid) +
                   ";call_state=" + std::to_string(convertCallStateToAudioNum(params.call_state)) +
                   ";call_type=";
  string call_mode_string = "UNKNOWN";
  switch (params.call_mode) {
    case CALL_MODE_CDMA_V02:
      call_mode_string = "CDMA";
      break;
    case CALL_MODE_GSM_V02:
      call_mode_string = "GSM";
      break;
    case CALL_MODE_UMTS_V02:
      call_mode_string = "UMTS";
      break;
    case CALL_MODE_NR5G_V02:
      call_mode_string = "NR5G";
      break;
    case CALL_MODE_LTE_V02:
      call_mode_string = "LTE";
      break;
    case CALL_MODE_TDS_V02:
      call_mode_string = "TDS";
      break;
    case CALL_MODE_WLAN_V02:
      call_mode_string = "WLAN";
      break;
    default:
      call_mode_string = "UNKNOWN";
      break;
  }
  aParams += call_mode_string;
  return android::String8(aParams.c_str());
}

uint32_t AndroidAudioModule::convertCallStateToAudioNum(AmCallState call_state) {
  uint32_t result = 0;
  switch (call_state) {
    case AmCallState::INVALID:
      result = 0;
      break;
    case AmCallState::INACTIVE:
      result = 1;
      break;
    case AmCallState::ACTIVE:
      result = 2;
      break;
    case AmCallState::HOLD:
      result = 3;
      break;
    case AmCallState::LOCAL_HOLD:
      result = 4;
      break;
    default:
      result = 0;
      break;
  }
  return result;
}

AmCallState AndroidAudioModule::convertAudioNumToCallState(uint32_t call_state) {
  AmCallState result = AmCallState::INVALID;
  switch (call_state) {
    case 0:
      result = AmCallState::INVALID;
      break;
    case 1:
      result = AmCallState::INACTIVE;
      break;
    case 2:
      result = AmCallState::ACTIVE;
      break;
    case 3:
      result = AmCallState::HOLD;
      break;
    case 4:
      result = AmCallState::LOCAL_HOLD;
      break;
    default:
      result = AmCallState::INVALID;
      break;
  }
  return result;
}

}  // namespace modules
}  // namespace ril
