/*
* Copyright (c) 2020,2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
*/
/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "RILC"
#define TAG "RILQ_ril_1_5"

#include <memory>
#include <ril_service_1_5.h>
#include "RilServiceModule_1_5.h"

#ifndef QMI_RIL_UTF
#include <hidl/HidlTransportSupport.h>
#endif

#include <log/log.h>
#include <utils/SystemClock.h>
#include <inttypes.h>
#include <framework/Log.h>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <framework/UnSolicitedMessage.h>

#include "interfaces/dms/RilRequestRadioPowerMessage.h"
#include <interfaces/voice/QcRilRequestGetCurrentCallsMessage.h>
#include <interfaces/nas/RilRequestEnableUiccAppMessage.h>
#include <interfaces/nas/RilRequestGetUiccAppStatusMessage.h>
#include "interfaces/sms/RilRequestCdmaSendSmsMessage.h"
#include <request/GetRadioDataCallListRequestMessage.h>
#include <request/SetDataProfileRequestMessage.h>
#include <request/SetInitialAttachApnRequestMessage.h>
#include <request/SetupDataCallRequestMessage.h>

#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"
#include "UnSolMessages/GetDataCallListResponseIndMessage.h"
#include "interfaces/uim/UimEnterDePersoRequestMsg.h"
#include "interfaces/nas/RilUnsolCellBarringMessage.h"


using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using namespace android::hardware::radio::deprecated::V1_0;
#ifndef QMI_RIL_UTF
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;
#endif

#ifdef QMI_RIL_UTF
extern string qcril_get_property_value(string name, string defaultValue);
#endif

using RadioContext = RadioImpl_1_5::RadioContext;

sp<RadioImpl_1_5> radioImpl_1_5;

void RadioImpl_1_5::clearCallbacks() {
    RLOGD("RadioImpl_1_5::clearCallbacks");
    mRadioResponseV1_5 = NULL;
    mRadioIndicationV1_5 = NULL;
    RadioImpl_1_4::clearCallbacks();
}

Return<void> RadioImpl_1_5::setResponseFunctions_nolock(
        const ::android::sp<IRadioResponse>& radioResponseParam,
        const ::android::sp<IRadioIndication>& radioIndicationParam) {
    RLOGD("RadioImpl_1_5::setResponseFunctions_nolock");

    mRadioResponseV1_5 = V1_5::IRadioResponse::castFrom(radioResponseParam).withDefault(nullptr);
    mRadioIndicationV1_5 = V1_5::IRadioIndication::castFrom(radioIndicationParam).withDefault(nullptr);


    if (mRadioResponseV1_5 == nullptr || mRadioIndicationV1_5 == nullptr) {
        mRadioResponseV1_5 = nullptr;
        mRadioIndicationV1_5 = nullptr;
    }

    QCRIL_LOG_DEBUG("radioResponseParamV1_5: %s. radioIndicationV1_5 %s",
            mRadioResponseV1_5 ? "<not null>" : "<null>",
            mRadioIndicationV1_5 ? "<not null>" : "<null>");

    RadioImpl_1_4::setResponseFunctions_nolock(
            radioResponseParam,
            radioIndicationParam);

    return Void();
}

Return<void> RadioImpl_1_5::setResponseFunctions(
        const ::android::sp<IRadioResponse>& radioResponseParam,
        const ::android::sp<IRadioIndication>& radioIndicationParam) {
    QCRIL_LOG_DEBUG("setResponseFunctions_1_5. radioResponseParam: %s. radioIndicationParam: %s",
            radioResponseParam ? "<not null>" : "<null>",
            radioIndicationParam ? "<not null>" : "<null>");
    RLOGD("setResponseFunctions_1_5");

    radioImpl_1_5 = this;

    qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
    radioServiceRwlockPtr->lock();

    RadioImpl_1_5::setResponseFunctions_nolock(radioResponseParam, radioIndicationParam);

    radioServiceRwlockPtr->unlock();

    RilServiceModule_1_5* rilModule = static_cast<RilServiceModule_1_5*>(RadioImpl_1_5::getRilServiceModule());
    QCRIL_LOG_DEBUG("New client connected. Resetting the counts for LTE and nr5g");
    rilModule->resetLteCounts();
    rilModule->reset5gnrCounts();

#ifndef QMI_RIL_UTF
    // client is connected. Send initial indications.
    android::onNewCommandConnect((RIL_SOCKET_ID) mSlotId);
#endif

    return Void();
}

void RadioImpl_1_5::sendRadioClientCapabilities() {
    dcListChangedCallback = nullptr;
    if (mRadioResponseV1_5 != nullptr && mRadioIndicationV1_5 != nullptr) {
#ifdef QMI_RIL_UTF
        string rilVersion = qcril_get_property_value("vendor.radio.utf.version", "1.5");
        QCRIL_LOG_DEBUG("RadioImpl_1_5 read property version %s", rilVersion.c_str());
        if (rilVersion == "1.5") {
#endif
        dcListChangedCallback = [](const vector<rildata::DataCallResult_t>& dcList) -> void {
            auto indMsg = std::make_shared<rildata::RadioDataCallListChangeIndMessage_1_5>(dcList);
            if (indMsg != nullptr) {
                indMsg->broadcast();
            } else {
                QCRIL_LOG_DEBUG("dcListChangedCallback_1_5 failed to allocate message");
            }
        };
#ifdef QMI_RIL_UTF
        }
#endif
    }
    RadioImpl_1_4::sendRadioClientCapabilities();
}

RadioError RadioImpl_1_5::sanityCheckSignalStrengthCriteriaParams(
        const V1_5::SignalThresholdInfo& threshold,
        V1_5::AccessNetwork ran) {
    (void)ran;
    QCRIL_LOG_DEBUG("performing sanity check");
    if (checkThresholdAndHysteresis(threshold.hysteresisMs, threshold.hysteresisDb, threshold.thresholds)) {
        return RadioError::NONE;
    } else {
        return RadioError::INVALID_ARGUMENTS;
    }
}

RIL_SignalMeasureType RadioImpl_1_5::convertHidlMeasureTypetoRilMeasureType(
        V1_5::SignalMeasurementType type) {
    RIL_SignalMeasureType ret = MT_UNKNOWN;
    switch (type) {
    case V1_5::SignalMeasurementType::RSSI:
        ret = MT_RSSI;
        break;
    case V1_5::SignalMeasurementType::RSCP:
        ret = MT_RSCP;
        break;
    case V1_5::SignalMeasurementType::RSRP:
        ret = MT_RSRP;
        break;
    case V1_5::SignalMeasurementType::RSRQ:
        ret = MT_RSRQ;
        break;
    case V1_5::SignalMeasurementType::RSSNR:
        ret = MT_RSSNR;
        break;
    case V1_5::SignalMeasurementType::SSRSRP:
        ret = MT_SSRSRP;
        break;
    case V1_5::SignalMeasurementType::SSRSRQ:
        ret = MT_SSRSRQ;
        break;
    case V1_5::SignalMeasurementType::SSSINR:
        ret = MT_SSSINR;
        break;
    default:
        ret = MT_UNKNOWN;
    }
    return ret;
}

RIL_RadioAccessNetworks RadioImpl_1_5::convertHidlAccessNetworkToRilAccessNetwork(
    V1_5::AccessNetwork ran) {
    if (ran == V1_5::AccessNetwork::NGRAN) {
        return RIL_RadioAccessNetworks::NGRAN;
    } else {
        return RadioImpl_1_2::convertHidlAccessNetworkToRilAccessNetwork(
            (V1_2::AccessNetwork)ran);
    }
}

void RadioImpl_1_5::fillInSignalStrengthCriteria(std::vector<SignalStrengthCriteriaEntry> &out,
    const V1_5::SignalThresholdInfo& signalThresholdInfo, V1_5::AccessNetwork ran, bool &disableAllTypes)
{
    RilServiceModule_1_5* rilModule = static_cast<RilServiceModule_1_5*>(RadioImpl_1_5::getRilServiceModule());
    RIL_RadioAccessNetworks ril_ran = convertHidlAccessNetworkToRilAccessNetwork(ran);
    auto type = convertHidlMeasureTypetoRilMeasureType(signalThresholdInfo.signalMeasurement);

    switch (ril_ran) {
        case EUTRAN:
            if ((signalThresholdInfo.isEnabled) && ((type == MT_RSRP) || (type == MT_RSRQ) || (type == MT_RSSNR))) {
                rilModule->incLteReqCnt();
            } else if ((!signalThresholdInfo.isEnabled) && ((type == MT_RSRP) || (type == MT_RSRQ) || (type == MT_RSSNR))){
                rilModule->incLteReqCnt();
                rilModule->incLteDisCnt();
                disableAllTypes = rilModule->checkLteDisCnt();
            } else {
                QCRIL_LOG_DEBUG(" Unexpected signal type received for EUTRAN");
            }
            rilModule->checkLteReqCnt();
            break;
        case NGRAN:
            if ((signalThresholdInfo.isEnabled) && ((type == MT_SSRSRP) || (type == MT_SSRSRQ) || (type == MT_SSSINR))) {
                rilModule->incNr5gReqCnt();
            } else if ((!signalThresholdInfo.isEnabled) && ((type == MT_SSRSRP) || (type == MT_SSRSRQ) || (type == MT_SSSINR))){
                rilModule->incNr5gReqCnt();
                rilModule->incNr5gDisCnt();
                disableAllTypes = rilModule->checkNr5gDisCnt();
            } else {
                QCRIL_LOG_DEBUG(" Unexpected signal type received for NGRAN");
            }
            rilModule->checkNr5gReqCnt();
            break;
        default:
            break;
    }

    out.push_back({ril_ran, type, signalThresholdInfo.hysteresisMs,
        signalThresholdInfo.hysteresisDb, signalThresholdInfo.thresholds, signalThresholdInfo.isEnabled});
}

Return<void> RadioImpl_1_5::setSignalStrengthReportingCriteria_1_5(int32_t serial,
        const V1_5::SignalThresholdInfo& signalThresholdInfo, V1_5::AccessNetwork accessNetwork) {

    QCRIL_LOG_FUNC_ENTRY();
    RadioError ret = RadioError::NONE;
    std::vector<SignalStrengthCriteriaEntry> args;
    bool disableAllTypes = false;
    std::shared_ptr<RadioContext> ctx = getContext(serial);
    QCRIL_LOG_DEBUG("New context with serial %d: %s (%s)", serial, ctx ? "<not null>" : "<null>",
                    ctx ? ctx->toString().c_str() : "<null>");

    string dump;
    dump += " hysteresis Ms: " + std::to_string(signalThresholdInfo.hysteresisMs);
    dump += " hysteresis db: " + std::to_string(signalThresholdInfo.hysteresisDb);
    dump += " thresholds: { ";
    for (int32_t th: signalThresholdInfo.thresholds) {
        dump += std::to_string(th) + ", ";
    }
    dump += "}";

    QCRIL_LOG_DEBUG("Arguments : %s",dump.c_str());

    if (RadioError::NONE ==
        sanityCheckSignalStrengthCriteriaParams(signalThresholdInfo, accessNetwork)) {
        fillInSignalStrengthCriteria(args, signalThresholdInfo, accessNetwork, disableAllTypes);

        std::shared_ptr<NasSetSignalStrengthCriteria> msg =
            std::make_shared<NasSetSignalStrengthCriteria>(ctx, args);

        if (msg) {
            if (disableAllTypes) {msg->setDisableAllTypes();}
            GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> msg,
                                                         Message::Callback::Status status,
                                                         std::shared_ptr<RIL_Errno> rsp) -> void {
                RadioError err = convertMsgToRadioError(status, rsp ? *rsp : RIL_E_INTERNAL_ERR);
                RadioResponseInfo info{RadioResponseType::SOLICITED, serial, err};
                QCRIL_LOG_DEBUG("serial: %d. status: %d rsp: %d. result: %d", serial, (int)status,
                                rsp ? *rsp : 0, (int)err);

                qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
                radioServiceRwlockPtr->lock_shared();

                auto response = mRadioResponseV1_5;
                if (response) {
                    Return<void> retStatus = response->setSignalStrengthReportingCriteriaResponse_1_5(info);
                    checkReturnStatus(retStatus);
                } else {
                    QCRIL_LOG_ERROR("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__,
                                    mSlotId);
                }

                radioServiceRwlockPtr->unlock_shared();
            });
            msg->setCallback(&cb);
            msg->dispatch();
        } else {
            ret = RadioError::NO_MEMORY;
        }
    } else {
        ret = RadioError::INVALID_ARGUMENTS;
    }

    if (ret != RadioError::NONE)
    {
        qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
        radioServiceRwlockPtr->lock_shared();

        auto response = mRadioResponseV1_5;
        if (response) {
            RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, ret};
            Return<void> retStatus = response->setSignalStrengthReportingCriteriaResponse_1_5(rsp);
            checkReturnStatus(retStatus);
        } else {
            QCRIL_LOG_ERROR("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__,
                            mSlotId);
        }

        radioServiceRwlockPtr->unlock_shared();
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

#if 0
int RadioImpl_1_5::convertToHidl(V1_5::Call &out, qcril::interfaces::CallInfo &in) {
    RadioImpl_1_2::convertToHidl(out.base, in);
    if (in.hasRedirNum()) {
        out.forwardedNumber = in.getRedirNum();
    }
    return 0;
}

Return<void> RadioImpl_1_5::getCurrentCalls_1_5(int32_t serial) {
  QCRIL_LOG_FUNC_ENTRY("getCurrentCalls: serial %d", serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestGetCurrentCallsMessage>(getContext(serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /* msg */, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          hidl_vec<V1_5::Call> calls;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto currentCalls =
                std::static_pointer_cast<qcril::interfaces::GetCurrentCallsRespData>(resp->data);
            if (currentCalls) {
              auto callList = currentCalls->getCallInfoList();
              if (!callList.empty()) {
                calls.resize(callList.size());
                for (uint32_t i = 0; i < callList.size(); i++) {
                  convertToHidl(calls[i], callList[i]);
                }
              }
            }
          }
          RadioResponseInfo respInfo = {};
          populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, errorCode);
          QCRIL_LOG_DEBUG("getCurrentCallsResponse_1_5 : calls = %s", toString(calls).c_str());
          RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, getCurrentCallsResponse_1_5, respInfo,
                                   calls);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);

  if (sendFailure) {
    hidl_vec<V1_5::Call> calls;
    RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
    RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, getCurrentCallsResponse_1_5, respInfo, calls);
  }
  QCRIL_LOG_FUNC_RETURN();
  return Void();
}
#endif

template<typename T>
bool getHidlPlmnFromCellIdentity(hidl_string& mcc, hidl_string& mnc, const T& cellIdentity) {
    if (cellIdentity.primary_plmn_mcc[0] != '\0' &&
            cellIdentity.primary_plmn_mnc[0] != '\0') {
        mcc = hidl_string(cellIdentity.primary_plmn_mcc,
                strnlen(cellIdentity.primary_plmn_mcc,
                sizeof(cellIdentity.primary_plmn_mcc) - 1));
        mnc = hidl_string(cellIdentity.primary_plmn_mnc,
                strnlen(cellIdentity.primary_plmn_mnc,
                sizeof(cellIdentity.primary_plmn_mnc) - 1));
        return true;

    } else if (cellIdentity.mcc[0] != '\0' && cellIdentity.mnc[0] != '\0') {
        mcc = hidl_string(cellIdentity.mcc,
                strnlen(cellIdentity.mcc,
                sizeof(cellIdentity.mcc) - 1));
        mnc = hidl_string(cellIdentity.mnc,
                strnlen(cellIdentity.mnc,
                sizeof(cellIdentity.mnc) - 1));
        return true;
    }
    return false;
}

void convertGsmCellIdentityRilToHidl(const RIL_CellIdentityGsm_v12& rilGsmCellIdentity, V1_5::CellIdentityGsm& hidlGsmCellIdentity) {
    (void)getHidlPlmnFromCellIdentity(hidlGsmCellIdentity.base.base.mcc,
            hidlGsmCellIdentity.base.base.mnc, rilGsmCellIdentity);

    hidlGsmCellIdentity.base.base.lac = rilGsmCellIdentity.lac;
    hidlGsmCellIdentity.base.base.cid = rilGsmCellIdentity.cid;
    hidlGsmCellIdentity.base.base.arfcn = rilGsmCellIdentity.arfcn;
    hidlGsmCellIdentity.base.base.bsic = rilGsmCellIdentity.bsic;

    hidlGsmCellIdentity.base.operatorNames.alphaLong = hidl_string(
            rilGsmCellIdentity.operatorNames.alphaLong,
            strnlen(rilGsmCellIdentity.operatorNames.alphaLong,
                    sizeof(rilGsmCellIdentity.operatorNames.alphaLong) - 1));

    hidlGsmCellIdentity.base.operatorNames.alphaShort = hidl_string(
            rilGsmCellIdentity.operatorNames.alphaShort,
            strnlen(rilGsmCellIdentity.operatorNames.alphaShort,
                    sizeof(rilGsmCellIdentity.operatorNames.alphaShort) - 1));
}

void convertWcdmaCellIdentityRilToHidl(const RIL_CellIdentityWcdma_v12& rilWcdmaCellIdentity, V1_5::CellIdentityWcdma& hidlWcdmaCellIdentity) {
    (void)getHidlPlmnFromCellIdentity(hidlWcdmaCellIdentity.base.base.mcc,
            hidlWcdmaCellIdentity.base.base.mnc, rilWcdmaCellIdentity);

    hidlWcdmaCellIdentity.base.base.lac = rilWcdmaCellIdentity.lac;
    hidlWcdmaCellIdentity.base.base.cid = rilWcdmaCellIdentity.cid;
    hidlWcdmaCellIdentity.base.base.psc = rilWcdmaCellIdentity.psc;
    hidlWcdmaCellIdentity.base.base.uarfcn = rilWcdmaCellIdentity.uarfcn;

    hidlWcdmaCellIdentity.base.operatorNames.alphaLong = hidl_string(
            rilWcdmaCellIdentity.operatorNames.alphaLong,
            strnlen(rilWcdmaCellIdentity.operatorNames.alphaLong,
                    sizeof(rilWcdmaCellIdentity.operatorNames.alphaLong) - 1));

    hidlWcdmaCellIdentity.base.operatorNames.alphaShort = hidl_string(
            rilWcdmaCellIdentity.operatorNames.alphaShort,
            strnlen(rilWcdmaCellIdentity.operatorNames.alphaShort,
                    sizeof(rilWcdmaCellIdentity.operatorNames.alphaShort) - 1));

}

void convertTdscdmaCellIdentityRilToHidl(const RIL_CellIdentityTdscdma& rilTdscdmaCellIdentity, V1_5::CellIdentityTdscdma& hidlTdscdmaCellIdentity) {
    (void)getHidlPlmnFromCellIdentity(hidlTdscdmaCellIdentity.base.base.mcc,
            hidlTdscdmaCellIdentity.base.base.mnc, rilTdscdmaCellIdentity);

    hidlTdscdmaCellIdentity.base.base.lac = rilTdscdmaCellIdentity.lac;
    hidlTdscdmaCellIdentity.base.base.cid = rilTdscdmaCellIdentity.cid;
    hidlTdscdmaCellIdentity.base.base.cpid = rilTdscdmaCellIdentity.cpid;

    hidlTdscdmaCellIdentity.base.operatorNames.alphaLong = hidl_string(
            rilTdscdmaCellIdentity.operatorNames.alphaLong,
            strnlen(rilTdscdmaCellIdentity.operatorNames.alphaLong,
                    sizeof(rilTdscdmaCellIdentity.operatorNames.alphaLong) - 1));

    hidlTdscdmaCellIdentity.base.operatorNames.alphaShort = hidl_string(
            rilTdscdmaCellIdentity.operatorNames.alphaShort,
            strnlen(rilTdscdmaCellIdentity.operatorNames.alphaShort,
                    sizeof(rilTdscdmaCellIdentity.operatorNames.alphaShort) - 1));

    hidlTdscdmaCellIdentity.base.uarfcn = 0; //default value
}

void convertCdmaCellIdentityRilToHidl(const RIL_CellIdentityCdma& rilCdmaCellIdentity, V1_2::CellIdentityCdma& hidlCdmaCellIdentity) {
    hidlCdmaCellIdentity.base.networkId = rilCdmaCellIdentity.networkId;
    hidlCdmaCellIdentity.base.systemId = rilCdmaCellIdentity.systemId;
    hidlCdmaCellIdentity.base.baseStationId = rilCdmaCellIdentity.basestationId;
    hidlCdmaCellIdentity.base.longitude = rilCdmaCellIdentity.longitude;
    hidlCdmaCellIdentity.base.latitude = rilCdmaCellIdentity.latitude;

    hidlCdmaCellIdentity.operatorNames.alphaLong = hidl_string(
            rilCdmaCellIdentity.operatorNames.alphaLong,
            strnlen(rilCdmaCellIdentity.operatorNames.alphaLong,
                    sizeof(rilCdmaCellIdentity.operatorNames.alphaLong) - 1));

    hidlCdmaCellIdentity.operatorNames.alphaShort = hidl_string(
            rilCdmaCellIdentity.operatorNames.alphaShort,
            strnlen(rilCdmaCellIdentity.operatorNames.alphaShort,
                    sizeof(rilCdmaCellIdentity.operatorNames.alphaShort) - 1));
}

V1_5::EutranBands calculateBandFromEarfcn(int earfcn) {
  if (earfcn >= 70596) {
      return V1_5::EutranBands::BAND_88;
  } else if (earfcn >= 70546) {
      return V1_5::EutranBands::BAND_87;
  } else if (earfcn >= 70366) {
      return V1_5::EutranBands::BAND_85;
  } else if (earfcn >= 69036) {
      return V1_5::EutranBands::BAND_74;
  } else if (earfcn >= 68986) {
      return V1_5::EutranBands::BAND_73;
  } else if (earfcn >= 68936) {
      return V1_5::EutranBands::BAND_72;
  } else if (earfcn >= 68586) {
      return V1_5::EutranBands::BAND_71;
  } else if (earfcn >= 68336) {
      return V1_5::EutranBands::BAND_70;
  } else if (earfcn >= 67536) {
      return V1_5::EutranBands::BAND_68;
  } else if (earfcn >= 66436) {
      return V1_5::EutranBands::BAND_66;
  } else if (earfcn >= 65536) {
      return V1_5::EutranBands::BAND_65;
  } else if (earfcn >= 60140) {
      return V1_5::EutranBands::BAND_53;
  } else if (earfcn >= 59140) {
      return V1_5::EutranBands::BAND_52;
  } else if (earfcn >= 59090) {
      return V1_5::EutranBands::BAND_51;
  } else if (earfcn >= 58240) {
      return V1_5::EutranBands::BAND_50;
  } else if (earfcn >= 56740) {
      return V1_5::EutranBands::BAND_49;
  } else if (earfcn >= 55240) {
      return V1_5::EutranBands::BAND_48;
  } else if (earfcn >= 54540) {
      return V1_5::EutranBands::BAND_47;
  } else if (earfcn >= 46790) {
      return V1_5::EutranBands::BAND_46;
  } else if (earfcn >= 46590) {
      return V1_5::EutranBands::BAND_45;
  } else if (earfcn >= 45590) {
      return V1_5::EutranBands::BAND_44;
  } else if (earfcn >= 43590) {
      return V1_5::EutranBands::BAND_43;
  } else if (earfcn >= 41590) {
      return V1_5::EutranBands::BAND_42;
  } else if (earfcn >= 39650) {
      return V1_5::EutranBands::BAND_41;
  } else if (earfcn >= 38650) {
      return V1_5::EutranBands::BAND_40;
  } else if (earfcn >= 38250) {
      return V1_5::EutranBands::BAND_39;
  } else if (earfcn >= 37750) {
      return V1_5::EutranBands::BAND_38;
  } else if (earfcn >= 37550) {
      return V1_5::EutranBands::BAND_37;
  } else if (earfcn >= 36950) {
      return V1_5::EutranBands::BAND_36;
  } else if (earfcn >= 36350) {
      return V1_5::EutranBands::BAND_35;
  } else if (earfcn >= 36200) {
      return V1_5::EutranBands::BAND_34;
  } else if (earfcn >= 36000) {
      return V1_5::EutranBands::BAND_33;
  } else if (earfcn >= 9870) {
      return V1_5::EutranBands::BAND_31;
  } else if (earfcn >= 9770) {
      return V1_5::EutranBands::BAND_30;
  } else if (earfcn >= 9210) {
      return V1_5::EutranBands::BAND_28;
  } else if (earfcn >= 9040) {
      return V1_5::EutranBands::BAND_27;
  } else if (earfcn >= 8690) {
      return V1_5::EutranBands::BAND_26;
  } else if (earfcn >= 8040) {
      return V1_5::EutranBands::BAND_25;
  } else if (earfcn >= 7700) {
      return V1_5::EutranBands::BAND_24;
  } else if (earfcn >= 7500) {
      return V1_5::EutranBands::BAND_23;
  } else if (earfcn >= 6600) {
      return V1_5::EutranBands::BAND_22;
  } else if (earfcn >= 6450) {
      return V1_5::EutranBands::BAND_21;
  } else if (earfcn >= 6150) {
      return V1_5::EutranBands::BAND_20;
  } else if (earfcn >= 6000) {
      return V1_5::EutranBands::BAND_19;
  } else if (earfcn >= 5850) {
      return V1_5::EutranBands::BAND_18;
  } else if (earfcn >= 5730) {
      return V1_5::EutranBands::BAND_17;
  } else if (earfcn >= 5280) {
      return V1_5::EutranBands::BAND_14;
  } else if (earfcn >= 5180) {
      return V1_5::EutranBands::BAND_13;
  } else if (earfcn >= 5010) {
      return V1_5::EutranBands::BAND_12;
  } else if (earfcn >= 4750) {
      return V1_5::EutranBands::BAND_11;
  } else if (earfcn >= 4150) {
      return V1_5::EutranBands::BAND_10;
  } else if (earfcn >= 3800) {
      return V1_5::EutranBands::BAND_9;
  } else if (earfcn >= 3450) {
      return V1_5::EutranBands::BAND_8;
  } else if (earfcn >= 2750) {
      return V1_5::EutranBands::BAND_7;
  } else if (earfcn >= 2650) {
      return V1_5::EutranBands::BAND_6;
  } else if (earfcn >= 2400) {
      return V1_5::EutranBands::BAND_5;
  } else if (earfcn >= 1950) {
      return V1_5::EutranBands::BAND_4;
  } else if (earfcn >= 1200) {
      return V1_5::EutranBands::BAND_3;
  } else if (earfcn >= 600) {
      return V1_5::EutranBands::BAND_2;
  } else if (earfcn >= 0) {
      return V1_5::EutranBands::BAND_1;
  }
  return V1_5::EutranBands::BAND_1;
}

void convertLteCellIdentityRilToHidl(const RIL_CellIdentityLte_v12& rilLteCellIdentity, V1_5::CellIdentityLte& hidlLteCellIdentity) {
    (void)getHidlPlmnFromCellIdentity(hidlLteCellIdentity.base.base.mcc,
            hidlLteCellIdentity.base.base.mnc, rilLteCellIdentity);

    hidlLteCellIdentity.base.base.ci = rilLteCellIdentity.ci;
    hidlLteCellIdentity.base.base.pci = rilLteCellIdentity.pci;
    hidlLteCellIdentity.base.base.tac = rilLteCellIdentity.tac;
    hidlLteCellIdentity.base.base.earfcn = rilLteCellIdentity.earfcn;

    hidlLteCellIdentity.base.operatorNames.alphaLong = hidl_string(
            rilLteCellIdentity.operatorNames.alphaLong,
            strnlen(rilLteCellIdentity.operatorNames.alphaLong,
                    sizeof(rilLteCellIdentity.operatorNames.alphaLong) - 1));

    hidlLteCellIdentity.base.operatorNames.alphaShort = hidl_string(
            rilLteCellIdentity.operatorNames.alphaShort,
            strnlen(rilLteCellIdentity.operatorNames.alphaShort,
                    sizeof(rilLteCellIdentity.operatorNames.alphaShort) - 1));

    hidlLteCellIdentity.base.bandwidth = rilLteCellIdentity.bandwidth;

    // Fill band only if its valid
    if(rilLteCellIdentity.band != INT_MAX && rilLteCellIdentity.band != 0) {
        hidlLteCellIdentity.bands.resize(1);
        hidlLteCellIdentity.bands[0] = static_cast<V1_5::EutranBands>(rilLteCellIdentity.band);
        QCRIL_LOG_DEBUG("band - %d", hidlLteCellIdentity.bands[0]);
    }
    if (rilLteCellIdentity.band == INT_MAX) {
      hidlLteCellIdentity.bands.resize(1);
      hidlLteCellIdentity.bands[0] = calculateBandFromEarfcn(hidlLteCellIdentity.base.base.earfcn);
      QCRIL_LOG_DEBUG("band - %d", hidlLteCellIdentity.bands[0]);
    }
}

void convertNrCellIdentityRilToHidl(const RIL_CellIdentityNr &rilNrCellIdentity, V1_5::CellIdentityNr& hidlNrCellIdentity) {
    (void)getHidlPlmnFromCellIdentity(hidlNrCellIdentity.base.mcc,
            hidlNrCellIdentity.base.mnc, rilNrCellIdentity);

    hidlNrCellIdentity.base.nci = rilNrCellIdentity.nci;
    hidlNrCellIdentity.base.pci = rilNrCellIdentity.pci;
    hidlNrCellIdentity.base.tac = rilNrCellIdentity.tac;
    hidlNrCellIdentity.base.nrarfcn = rilNrCellIdentity.nrarfcn;

    hidlNrCellIdentity.base.operatorNames.alphaLong = hidl_string(
            rilNrCellIdentity.operatorNames.alphaLong,
            strnlen(rilNrCellIdentity.operatorNames.alphaLong,
                    sizeof(rilNrCellIdentity.operatorNames.alphaLong) - 1));

    hidlNrCellIdentity.base.operatorNames.alphaShort = hidl_string(
            rilNrCellIdentity.operatorNames.alphaShort,
            strnlen(rilNrCellIdentity.operatorNames.alphaShort,
                    sizeof(rilNrCellIdentity.operatorNames.alphaShort) - 1));

    // Fill band only if its valid
    if(rilNrCellIdentity.band != INT_MAX && rilNrCellIdentity.band != 0) {
        hidlNrCellIdentity.bands.resize(1);
        QCRIL_LOG_DEBUG("band - %d", rilNrCellIdentity.band);
        hidlNrCellIdentity.bands[0] = static_cast<V1_5::NgranBands>(rilNrCellIdentity.band);
    }
}

void convertCellInfoRilToHidl(const RIL_CellInfo_v12& rilCellInfo, V1_5::CellInfo& hidlCellInfo) {
    switch(rilCellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM: {
            V1_5::CellInfoGsm cellInfoGsm {};
            convertGsmCellIdentityRilToHidl(rilCellInfo.CellInfo.gsm.cellIdentityGsm, cellInfoGsm.cellIdentityGsm);
            cellInfoGsm.signalStrengthGsm.signalStrength = rilCellInfo.CellInfo.gsm.signalStrengthGsm.signalStrength;
            cellInfoGsm.signalStrengthGsm.bitErrorRate = rilCellInfo.CellInfo.gsm.signalStrengthGsm.bitErrorRate;
            cellInfoGsm.signalStrengthGsm.timingAdvance = rilCellInfo.CellInfo.gsm.signalStrengthGsm.timingAdvance;
            hidlCellInfo.ratSpecificInfo.gsm(std::move(cellInfoGsm));
            break;
        }

        case RIL_CELL_INFO_TYPE_WCDMA: {
            V1_5::CellInfoWcdma cellInfoWcdma {};
            convertWcdmaCellIdentityRilToHidl(rilCellInfo.CellInfo.wcdma.cellIdentityWcdma, cellInfoWcdma.cellIdentityWcdma);
            cellInfoWcdma.signalStrengthWcdma.base.signalStrength = rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.signalStrength;
            cellInfoWcdma.signalStrengthWcdma.base.bitErrorRate = rilCellInfo.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate;
            hidlCellInfo.ratSpecificInfo.wcdma(std::move(cellInfoWcdma));
            break;
        }

        case RIL_CELL_INFO_TYPE_CDMA: {
            V1_2::CellInfoCdma cellInfoCdma {};
            convertCdmaCellIdentityRilToHidl(rilCellInfo.CellInfo.cdma.cellIdentityCdma, cellInfoCdma.cellIdentityCdma);
            cellInfoCdma.signalStrengthCdma.dbm = rilCellInfo.CellInfo.cdma.signalStrengthCdma.dbm;
            cellInfoCdma.signalStrengthCdma.ecio = rilCellInfo.CellInfo.cdma.signalStrengthCdma.ecio;
            cellInfoCdma.signalStrengthEvdo.dbm = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.dbm;
            cellInfoCdma.signalStrengthEvdo.ecio = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.ecio;
            cellInfoCdma.signalStrengthEvdo.signalNoiseRatio = rilCellInfo.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio;
            hidlCellInfo.ratSpecificInfo.cdma(std::move(cellInfoCdma));
            break;
        }

        case RIL_CELL_INFO_TYPE_LTE: {
            V1_5::CellInfoLte cellInfoLte {};
            convertLteCellIdentityRilToHidl(rilCellInfo.CellInfo.lte.cellIdentityLte, cellInfoLte.cellIdentityLte);
            cellInfoLte.signalStrengthLte.signalStrength = rilCellInfo.CellInfo.lte.signalStrengthLte.signalStrength;
            cellInfoLte.signalStrengthLte.rsrp = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrp;
            cellInfoLte.signalStrengthLte.rsrq = rilCellInfo.CellInfo.lte.signalStrengthLte.rsrq;
            cellInfoLte.signalStrengthLte.rssnr = rilCellInfo.CellInfo.lte.signalStrengthLte.rssnr;
            cellInfoLte.signalStrengthLte.cqi = rilCellInfo.CellInfo.lte.signalStrengthLte.cqi;
            cellInfoLte.signalStrengthLte.timingAdvance = rilCellInfo.CellInfo.lte.signalStrengthLte.timingAdvance;
            hidlCellInfo.ratSpecificInfo.lte(std::move(cellInfoLte));
            break;
        }

        case RIL_CELL_INFO_TYPE_NR: {
            V1_5::CellInfoNr cellInfoNr {};
            convertNrCellIdentityRilToHidl(rilCellInfo.CellInfo.nr.cellIdentityNr, cellInfoNr.cellIdentityNr);
            cellInfoNr.signalStrengthNr.ssRsrp =
                rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrp;
            cellInfoNr.signalStrengthNr.ssRsrq =
                rilCellInfo.CellInfo.nr.signalStrengthNr.ssRsrq;
            cellInfoNr.signalStrengthNr.ssSinr =
                rilCellInfo.CellInfo.nr.signalStrengthNr.ssSinr;
            cellInfoNr.signalStrengthNr.csiRsrp =
                rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrp;
            cellInfoNr.signalStrengthNr.csiRsrq =
                rilCellInfo.CellInfo.nr.signalStrengthNr.csiRsrq;
            cellInfoNr.signalStrengthNr.csiSinr =
                rilCellInfo.CellInfo.nr.signalStrengthNr.csiSinr;
            hidlCellInfo.ratSpecificInfo.nr(std::move(cellInfoNr));
            break;
        }

        case RIL_CELL_INFO_TYPE_TD_SCDMA: {
            V1_5::CellInfoTdscdma cellInfoTdscdma {};
            convertTdscdmaCellIdentityRilToHidl(rilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma, cellInfoTdscdma.cellIdentityTdscdma);
            cellInfoTdscdma.signalStrengthTdscdma.rscp = rilCellInfo.CellInfo.tdscdma.signalStrengthTdscdma.rscp;
            hidlCellInfo.ratSpecificInfo.tdscdma(std::move(cellInfoTdscdma));
            break;
        }

        default: {
            break;
        }
    }
}

void convertRilCellInfoList(const std::vector<RIL_CellInfo_v12>& rilCellInfo, hidl_vec<V1_5::CellInfo>& records) {
    auto num = rilCellInfo.size();
    records.resize(num);

    for (unsigned int i = 0; i < num; i++) {
        records[i].registered = rilCellInfo[i].registered;
        // TODO: Use a conversion function to translate RIL_TimeStampType to HIDL TimeStampType
        records[i].timeStampType = (V1_0::TimeStampType) rilCellInfo[i].timeStampType;
        records[i].timeStamp = rilCellInfo[i].timeStamp;
        records[i].connectionStatus = (V1_2::CellConnectionStatus) rilCellInfo[i].connStatus;
        convertCellInfoRilToHidl(rilCellInfo[i], records[i]);
    }
}

void convertRilCellInfoList(void* response, size_t responseLen, hidl_vec<V1_5::CellInfo>& records) {
    int num = responseLen / sizeof(RIL_CellInfo_v12);
    records.resize(num);

    RIL_CellInfo_v12* rilCellInfo = (RIL_CellInfo_v12*) response;
    if (rilCellInfo) {
        for (int i = 0; i < num; i++) {
            records[i].registered = rilCellInfo[i].registered;
            // TODO: Use a conversion function to translate RIL_TimeStampType to HIDL TimeStampType
            records[i].timeStampType = (V1_0::TimeStampType) rilCellInfo[i].timeStampType;
            records[i].timeStamp = rilCellInfo[i].timeStamp;
            records[i].connectionStatus = (V1_2::CellConnectionStatus) rilCellInfo[i].connStatus;
            convertCellInfoRilToHidl(rilCellInfo[i], records[i]);
        }
    }
}

void convertCellIdentityRilToHidl(const RIL_CellIdentity_v16 &in, V1_5::CellIdentity &out) {
    switch(in.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM: {
            V1_5::CellIdentityGsm cellIdentityGsm {};
            convertGsmCellIdentityRilToHidl(in.cellIdentityGsm, cellIdentityGsm);
            out.gsm(std::move(cellIdentityGsm));
            break;
        }
        case RIL_CELL_INFO_TYPE_WCDMA: {
            V1_5::CellIdentityWcdma cellIdentityWcdma {};
            convertWcdmaCellIdentityRilToHidl(in.cellIdentityWcdma, cellIdentityWcdma);
            out.wcdma(std::move(cellIdentityWcdma));
            break;
        }

        case RIL_CELL_INFO_TYPE_CDMA: {
            V1_2::CellIdentityCdma cellIdentityCdma {};
            convertCdmaCellIdentityRilToHidl(in.cellIdentityCdma, cellIdentityCdma);
            out.cdma(std::move(cellIdentityCdma));
            break;
        }

        case RIL_CELL_INFO_TYPE_LTE: {
            V1_5::CellIdentityLte cellIdentityLte {};
            convertLteCellIdentityRilToHidl(in.cellIdentityLte, cellIdentityLte);
            out.lte(std::move(cellIdentityLte));
            break;
        }

        case RIL_CELL_INFO_TYPE_TD_SCDMA: {
            V1_5::CellIdentityTdscdma cellIdentityTdscdma {};
            convertTdscdmaCellIdentityRilToHidl(in.cellIdentityTdscdma, cellIdentityTdscdma);
            out.tdscdma(std::move(cellIdentityTdscdma));
            break;
        }

        case RIL_CELL_INFO_TYPE_NR: {
            V1_5::CellIdentityNr cellIdentityNr {};
            convertNrCellIdentityRilToHidl(in.cellIdentityNr, cellIdentityNr);
            out.nr(std::move(cellIdentityNr));
            break;
        }

        default: {
            break;
        }
    }
}

Return<void> RadioImpl_1_5::sendCdmaSmsExpectMore(int32_t serial, const CdmaSmsMessage& sms) {
#if VDBG
    RLOGD("sendCdmaSmsExpectMore: serial %d", serial);
#endif
    QCRIL_LOG_FUNC_ENTRY();
    RIL_CDMA_SMS_Message rcsm{};
    constructCdmaSms(rcsm, sms);
    rcsm.expectMore = 1;
    auto msg = std::make_shared<RilRequestCdmaSendSmsMessage>(getContext(serial), rcsm);
    if (msg) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            ([this, serial]([[maybe_unused]] std::shared_ptr<Message> msg,
                            Message::Callback::Status status,
                            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
                SendSmsResult result = {};
                RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
                RadioResponseInfo responseInfo = {};
                if(status == Message::Callback::Status::SUCCESS && resp) {
                  auto sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
                  errorCode = resp->errorCode;
                  if (sendSmsResult) {
                    result = makeSendSmsResult(responseInfo, serial, RESPONSE_SOLICITED,
                                               sendSmsResult, errorCode);
                  } else {
                    populateResponseInfo(responseInfo, serial, RESPONSE_SOLICITED, errorCode);
                  }
                } else {
                  populateResponseInfo(responseInfo, serial, RESPONSE_SOLICITED, errorCode);
                }
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5,
                        sendCdmaSmsExpectMoreResponse, responseInfo, result);
        }));
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        SendSmsResult result = {-1, hidl_string(), -1};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5,
                sendCdmaSmsExpectMoreResponse, rsp, result);
    }
    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

#ifdef HAS_QCRIL_DATA_1_5_RESPONSE_TYPES
rildata::AccessNetwork_t RadioImpl_1_5::convertHidlAccessNetworkToDataAccessNetwork(
    V1_5::AccessNetwork ran) {
    if (ran == V1_5::AccessNetwork::NGRAN) {
        return rildata::AccessNetwork_t::NGRAN;
    } else {
        return RadioImpl_1_4::convertHidlAccessNetworkToDataAccessNetwork(
            (V1_4::AccessNetwork)ran);
    }
}
#endif

rildata::DataProfileInfo_t RadioImpl_1_5::convertHidlDataProfileInfoToRil(const V1_5::DataProfileInfo& profile) {
    rildata::DataProfileInfo_t rilProfile = {};
    rilProfile.profileId = (rildata::DataProfileId_t)profile.profileId;
    rilProfile.apn = profile.apn;
    rilProfile.protocol = convertPdpProtocolTypeToString(profile.protocol);
    rilProfile.roamingProtocol = convertPdpProtocolTypeToString(profile.roamingProtocol);
    rilProfile.authType = (rildata::ApnAuthType_t)profile.authType;
    rilProfile.username = profile.user;
    rilProfile.password = profile.password;
    rilProfile.dataProfileInfoType = (rildata::DataProfileInfoType_t)profile.type,
    rilProfile.maxConnsTime = profile.maxConnsTime;
    rilProfile.maxConns = profile.maxConns;
    rilProfile.waitTime = profile.waitTime;
    rilProfile.enableProfile = profile.enabled;
    rilProfile.supportedApnTypesBitmap = (rildata::ApnTypes_t)profile.supportedApnTypesBitmap;
    rilProfile.bearerBitmap = (rildata::RadioAccessFamily_t)profile.bearerBitmap;
    rilProfile.mtu = profile.mtuV4;
    rilProfile.preferred = profile.preferred;
    rilProfile.persistent = profile.persistent;
    return rilProfile;
}

vector<V1_5::LinkAddress> RadioImpl_1_5::convertLinkAddressToHidlVector(vector<rildata::LinkAddress_t> addresses) {
    vector<V1_5::LinkAddress> hidlAddressesVector;
    for (rildata::LinkAddress_t addr : addresses) {
        V1_5::LinkAddress hidlAddr = {
            .address = addr.address,
            .properties = (int)addr.properties,
            .deprecationTime = addr.deprecationTime,
            .expirationTime = addr.expirationTime,
        };
        hidlAddressesVector.push_back(hidlAddr);
    }
    return hidlAddressesVector;
}

V1_5::SetupDataCallResult RadioImpl_1_5::convertDcResultToHidlDcResult_1_5(const rildata::DataCallResult_t& result) {
    V1_5::SetupDataCallResult dcResult = {};
    dcResult.cause = convertDcFailCauseToHidlDcFailCause(result.cause);
    dcResult.suggestedRetryTime = result.suggestedRetryTime;
    dcResult.cid = result.cid;
    dcResult.active = (V1_4::DataConnActiveStatus)result.active;
    dcResult.type = convertStringToPdpProtocolType(result.type);
    dcResult.ifname = result.ifname;
    dcResult.addresses = convertLinkAddressToHidlVector(result.linkAddresses);
    dcResult.dnses = convertAddrStringToHidlStringVector(result.dnses);
    dcResult.gateways = convertAddrStringToHidlStringVector(result.gateways);
    dcResult.pcscf = convertAddrStringToHidlStringVector(result.pcscf);
    dcResult.mtuV4 = result.mtuV4;
    dcResult.mtuV6 = result.mtuV6;
    return dcResult;
}

Return<void> RadioImpl_1_5::setDataProfile_1_5(int32_t serial, const hidl_vec<V1_5::DataProfileInfo>& profiles) {
    vector<rildata::DataProfileInfo_t> p;

    for (size_t i = 0; i < profiles.size(); i++) {
        /* copy all the profiles */
        rildata::DataProfileInfo_t t = convertHidlDataProfileInfoToRil(profiles[i]);
        p.push_back(t);
    }

    auto msg = std::make_shared<rildata::SetDataProfileRequestMessage>(serial, p);
    if(msg) {
        GenericCallback<RIL_Errno> cb(
            ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
                RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
                if (solicitedMsg && responseDataPtr) {
                    Log::getInstance().d(
                        "[SetDataProfile_1_5 cb invoked: [msg = " +
                        solicitedMsg->dump() + "] executed. client data = " +
                        "set-ril-service-cb-token status = " + std::to_string((int)status));
                    if(status == Message::Callback::Status::SUCCESS) {
                        RadioError e = convertMsgToRadioError(status, *responseDataPtr);
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
                    }
                    else {
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::GENERIC_FAILURE};
                    }
                }
                setDataProfileResponse_1_5(responseInfo);
            }));
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
}

Return<void> RadioImpl_1_5::setupDataCall_1_5(int32_t serial, V1_5::AccessNetwork accessNetwork, const V1_5::DataProfileInfo& dataProfileInfo,
            bool roamingAllowed, V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses, const hidl_vec<hidl_string>& dnses) {
#if VDBG
    RLOGD("RadioImpl_1_5::setupDataCall_1_5: serial %d", serial);
#endif
    using namespace rildata;

    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;

    for (size_t i = 0 ; i < addresses.size(); i++ ) {
        radioAddresses.push_back(addresses[i].address);
    }
    for (size_t i = 0 ; i < dnses.size(); i++ ) {
        radioDnses.push_back(dnses[i]);
    }

#ifdef HAS_QCRIL_DATA_1_5_RESPONSE_TYPES
    AccessNetwork_t accnet = convertHidlAccessNetworkToDataAccessNetwork(accessNetwork);
#else
    RIL_RadioAccessNetworks accnet = convertHidlAccessNetworkToRilAccessNetwork(accessNetwork);
#endif

    auto msg =
      std::make_shared<SetupDataCallRequestMessage>(
        serial,
        #ifdef REQUEST_SOURCE_INCLUDED
        RequestSource_t::RADIO,
        #endif
        accnet,
        (DataProfileId_t)dataProfileInfo.profileId,
        dataProfileInfo.apn,
        convertPdpProtocolTypeToString(dataProfileInfo.protocol),
        convertPdpProtocolTypeToString(dataProfileInfo.roamingProtocol),
        (ApnAuthType_t)dataProfileInfo.authType,
        dataProfileInfo.user,
        dataProfileInfo.password,
        (DataProfileInfoType_t)dataProfileInfo.type,
        dataProfileInfo.maxConnsTime,
        dataProfileInfo.maxConns,
        dataProfileInfo.waitTime,
        dataProfileInfo.enabled,
        (ApnTypes_t)dataProfileInfo.supportedApnTypesBitmap,
        (RadioAccessFamily_t)dataProfileInfo.bearerBitmap,
        dataProfileInfo.mtuV4,
        dataProfileInfo.preferred,
        dataProfileInfo.persistent,
        roamingAllowed,
        (DataRequestReason_t)reason,
        radioAddresses,
        radioDnses,
        nullptr);

    if (msg) {
        if( mRadioResponseV1_5 == nullptr)
        {
            GenericCallback<SetupDataCallResponse_t> cb([serial](std::shared_ptr<Message>,
                                                                Message::Callback::Status status,
                                                                std::shared_ptr<SetupDataCallResponse_t> rsp) -> void {
                if (rsp != nullptr) {
                    auto indMsg = std::make_shared<SetupDataCallRadioResponseIndMessage_1_4>(*rsp, serial, status);
                    if (indMsg != nullptr) {
                        indMsg->broadcast();
                    } else {
                        QCRIL_LOG_DEBUG("setup data call cb failed to allocate message status %d respErr %d", status, rsp->respErr);
                    }
                } else {
                    QCRIL_LOG_ERROR("setupDataCall_1_4 resp is nullptr");
                }
            });
            msg->setCallback(&cb);
            msg->dispatch();
        } else {
            GenericCallback<SetupDataCallResponse_t> cb([serial](std::shared_ptr<Message>,
                                                                Message::Callback::Status status,
                                                                std::shared_ptr<SetupDataCallResponse_t> rsp) -> void {
                if (rsp != nullptr) {
                    auto indMsg = std::make_shared<SetupDataCallRadioResponseIndMessage_1_5>(*rsp, serial, status);
                    if (indMsg != nullptr) {
                        indMsg->broadcast();
                    } else {
                        QCRIL_LOG_DEBUG("setup data call cb failed to allocate message status %d respErr %d", status, rsp->respErr);
                    }
                } else {
                    QCRIL_LOG_ERROR("setupDataCall_1_5 resp is nullptr");
                }
            });
            msg->setCallback(&cb);
            msg->dispatch();
        }
    }
    return Void();
}

Return<void> RadioImpl_1_5::setInitialAttachApn_1_5(int32_t serial, const V1_5::DataProfileInfo& dataProfileInfo) {
    rildata::DataProfileInfo_t profile = convertHidlDataProfileInfoToRil(dataProfileInfo);
    auto msg = std::make_shared<rildata::SetInitialAttachApnRequestMessage>(serial, profile, nullptr);
    if(msg) {
        GenericCallback<RIL_Errno> cb(
            ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
                RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
                if (solicitedMsg && responseDataPtr) {
                    Log::getInstance().d(
                        "[setInitialAttachApn_1_5 cb invoked: [msg = " +
                        solicitedMsg->dump() + "] executed. client data = " +
                        "set-ril-service-cb-token status = " + std::to_string((int)status));
                    if(status == Message::Callback::Status::SUCCESS) {
                        RadioError e = convertMsgToRadioError(status, *responseDataPtr);
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
                    }
                    else {
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::GENERIC_FAILURE};
                    }
                }
                setInitialAttachApnResponse_1_5(responseInfo);
            }));
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
}

Return<void> RadioImpl_1_5::setLinkCapacityReportingCriteria_1_5(
    int32_t serial,
    int32_t hysteresisMs,
    int32_t hysteresisDlKbps,
    int32_t hysteresisUlKbps,
    const hidl_vec<int32_t> &thresholdsDownlinkKbps,
    const hidl_vec<int32_t> &thresholdsUplinkKbps,
    V1_5::AccessNetwork ran)
{
    QCRIL_LOG_DEBUG("serial: %d, hysteresisMs: %d, hysteresisDlKbps: %d, hysteresisUlKbps: %d, ran: %d",
                    serial, hysteresisMs, hysteresisDlKbps, hysteresisUlKbps, ran);
    QCRIL_LOG_DEBUG("thresholdsDownlinkKbps: {");
    for (int thrD : thresholdsDownlinkKbps) {
        QCRIL_LOG_DEBUG("| %d", thrD);
    }
    QCRIL_LOG_DEBUG("}");
#if VDBG
    RLOGD("%s(): %d", __FUNCTION__, serial);
#endif

#ifndef QMI_RIL_UTF
    rildata::LinkCapCriteria_t c = {
        .hysteresisMs = hysteresisMs,
        .hysteresisUlKbps = hysteresisUlKbps,
        .hysteresisDlKbps = hysteresisDlKbps,
        .thresholdsUplinkKbps = thresholdsUplinkKbps,
        .thresholdsDownlinkKbps = thresholdsDownlinkKbps,
    };

#ifdef HAS_QCRIL_DATA_1_5_RESPONSE_TYPES
    c.ran = convertHidlAccessNetworkToDataAccessNetwork(ran);
#else
    c.ran = convertHidlAccessNetworkToRilAccessNetwork(ran);
#endif

    auto msg = std::make_shared<rildata::SetLinkCapRptCriteriaMessage>(c);

    GenericCallback<rildata::LinkCapCriteriaResult_t> cb(
        ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<rildata::LinkCapCriteriaResult_t> responseDataPtr) -> void {
            if (solicitedMsg && responseDataPtr) {
                RIL_Errno re = convertLcResultToRilError(*responseDataPtr);
                RadioError e = convertMsgToRadioError(status, re);
                RadioResponseInfo responseInfo = {
                    .serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
                Log::getInstance().d(
                    "setLinkCapacityReportingCriteria_1_5 resp invoked Callback[msg = " +
                    solicitedMsg->dump() + "] executed. client data = " +
                    "set-ril-service-cb-token status = " + std::to_string((int)status));

                qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
                radioServiceRwlockPtr->lock_shared();
                setLinkCapacityReportingCriteriaResponse_1_5(responseInfo);
                radioServiceRwlockPtr->unlock_shared();
            }
        }));
    msg->setCallback(&cb);
    if (msg)
        msg->dispatch();
#endif
    return Void();
}

Return<void> RadioImpl_1_5::getDataCallList(int32_t serial) {
    QCRIL_LOG_DEBUG("RadioImpl_1_5::getDataCallList, serial = %d", serial);
    using namespace rildata;

#ifdef QMI_RIL_UTF
    string rilVersion = qcril_get_property_value("vendor.radio.utf.version", "1.5");
    QCRIL_LOG_DEBUG("RadioImpl_1_5 read property version %s", rilVersion.c_str());
    if (rilVersion != "1.5") {
      return RadioImpl_1_4::getDataCallList(serial);
    }
#endif

    if( mRadioResponseV1_5 == nullptr)
    {
      QCRIL_LOG_DEBUG("mRadioResponseV1_5 is null");
      return RadioImpl_1_4::getDataCallList(serial);
    }

    auto msg = std::make_shared<GetRadioDataCallListRequestMessage>();
    GenericCallback<DataCallListResult_t> cb([serial](std::shared_ptr<Message> solicitedMsg,
                                                             Message::Callback::Status status,
                                                             std::shared_ptr<DataCallListResult_t> responseDataPtr) -> void {
        if (solicitedMsg && responseDataPtr) {
            auto mg = std::make_shared<rildata::GetDataCallListResponseIndMessage>(responseDataPtr, serial, status);
            mg->broadcast();
        } else {
            QCRIL_LOG_DEBUG("responseDataPtr or solicitedMsg is nullptr");
        }
    });

    msg->setCallback(&cb);
    msg->dispatch();

    return Void();
}

void RadioImpl_1_5::setInitialAttachApnResponse_1_5(RadioResponseInfo responseInfo) {
    QCRIL_LOG_FUNC_ENTRY();

    auto rsp = mRadioResponseV1_5;
    if (rsp) {
        QCRIL_LOG_INFO("setInitialAttachApnResponse_1_5");
        Return<void> retStatus = rsp->setInitialAttachApnResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("setInitialAttachApnResponse_1_5: radioService[%d]->mRadioResponse == NULL", mSlotId);
        setInitialAttachApnResponse_1_4(responseInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RadioImpl_1_5::getDataCallListResponse_1_5(std::shared_ptr<rildata::DataCallListResult_t> responseDataPtr, int32_t serial, Message::Callback::Status status) {
    QCRIL_LOG_FUNC_ENTRY();

    auto rsp = mRadioResponseV1_5;
#ifdef QMI_RIL_UTF
      string rilVersion = qcril_get_property_value("vendor.radio.utf.version", "1.5");
      QCRIL_LOG_DEBUG("RadioImpl_1_5 read property version %s", rilVersion.c_str());
      if (rilVersion == "1.5") {
#else
    if (rsp) {
#endif
        QCRIL_LOG_INFO("getDataCallListResponse_1_5");
        qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);

        RadioError e = RadioError::NONE;
        RadioResponseInfo responseInfo {.type = RadioResponseType::SOLICITED, .serial = serial, .error = RadioError::NO_MEMORY};
        ::android::hardware::hidl_vec<V1_5::SetupDataCallResult> dcResultList;

        if ((status == Message::Callback::Status::SUCCESS) &&
            (responseDataPtr->respErr == rildata::ResponseError_t::NO_ERROR)) {
            QCRIL_LOG_DEBUG("getDataCallList cb invoked status %d respErr %d and Call list size = %d",
                             status, responseDataPtr->respErr, responseDataPtr->call.size());

            dcResultList.resize(responseDataPtr->call.size());
            int i=0;
            for (rildata::DataCallResult_t entry: responseDataPtr->call) {
                QCRIL_LOG_DEBUG("cid %d ifname %s addresses %s", entry.cid, entry.ifname.c_str(), entry.addresses.c_str());
                dcResultList[i] = convertDcResultToHidlDcResult_1_5(entry);
                i++;
            }
        } else {
            switch (responseDataPtr->respErr) {
                case rildata::ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
                case rildata::ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                default: e = RadioError::GENERIC_FAILURE; break;
            }
        }
        responseInfo.error = e;
        radioServiceRwlockPtr->lock_shared();
        Return<void> retStatus = rsp->getDataCallListResponse_1_5(responseInfo, dcResultList);
        checkReturnStatus(retStatus);
        radioServiceRwlockPtr->unlock_shared();
    } else {
        RLOGE("getDataCallListResponse_1_5: radioService[%d]->mRadioResponse == NULL", mSlotId);
        RadioImpl_1_4::getDataCallListResponse_1_4(responseDataPtr, serial, status);
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RadioImpl_1_5::setDataProfileResponse_1_5(RadioResponseInfo responseInfo) {
    QCRIL_LOG_FUNC_ENTRY();

    auto rsp = mRadioResponseV1_5;
    if (rsp) {
        QCRIL_LOG_INFO("setDataProfileResponse_1_5");
        Return<void> retStatus = rsp->setDataProfileResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("setDataProfileResponse_1_5: radioService[%d]->mRadioResponse == NULL", mSlotId);
        setDataProfileResponse_1_4(responseInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RadioImpl_1_5::setupDataCallResponse_1_5(RadioResponseInfo responseInfo, const V1_5::SetupDataCallResult& dcResult) {
    QCRIL_LOG_FUNC_ENTRY();

    auto rsp = mRadioResponseV1_5;
    if (rsp) {
        QCRIL_LOG_INFO("setupDataCallResponse_1_5");
        Return<void> retStatus = rsp->setupDataCallResponse_1_5(responseInfo, dcResult);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("setupDataCallResponse_1_5: radioService[%d]->mRadioResponse == NULL", mSlotId);
        // SetupDataCall and DataCallListChanged are handled in the RilServiceModule
        // thread, so the fallback would happen in the Message callback
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RadioImpl_1_5::dataCallListChanged_1_5(RadioIndicationType indType, const hidl_vec<V1_5::SetupDataCallResult>& dcResultList) {
    QCRIL_LOG_FUNC_ENTRY();

    auto ind = mRadioIndicationV1_5;
    if (ind) {
        QCRIL_LOG_INFO("dataCallListChanged_1_5");
        Return<void> retStatus = ind->dataCallListChanged_1_5(indType, dcResultList);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("dataCallListChanged_1_5: radioService[%d]->mRadioResponse == NULL", mSlotId);
        // SetupDataCall and DataCallListChanged are handled in the RilServiceModule
        // thread, so the fallback would happen in the Message callback
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RadioImpl_1_5::setLinkCapacityReportingCriteriaResponse_1_5(RadioResponseInfo responseInfo) {
    QCRIL_LOG_FUNC_ENTRY();

    auto responseV1_5 = mRadioResponseV1_5;
    if (responseV1_5 != NULL) {
        QCRIL_LOG_INFO("setLinkCapacityReportingCriteriaResponse_1_5");
        Return<void> retStatus =
            responseV1_5->setLinkCapacityReportingCriteriaResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__,
                mSlotId);
        setLinkCapacityReportingCriteriaResponse_1_2(responseInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
}

std::shared_ptr<RadioContext> RadioImpl_1_5::getContext(int32_t serial) {
    std::shared_ptr<RadioContext> ctx;
    ctx = std::make_shared<RadioContext>(sp<RadioImpl_1_5>(this), serial);
    return ctx;
}

::android::status_t RadioImpl_1_5::registerAsService(const std::string &serviceName) {
    QCRIL_LOG_INFO("RadioImpl_1_5::registerAsService(%s)", serviceName.c_str());
    return V1_5::IRadio::registerAsService(serviceName);
}

static void __attribute__((constructor)) registerRadioImpl_1_5();
void registerRadioImpl_1_5()
{
    QCRIL_LOG_INFO("Calling registerRadioImpl_1_5");
    getQcrildServiceFactory().registerRadioImpl<RadioImpl_1_5>(nullptr);
}

Module* RadioImpl_1_5::getRilServiceModule() {
    return mModule;
}

void RadioImpl_1_5::createRilServiceModule() {
    mModule = new RilServiceModule_1_5(getInstanceId(), *this);
    if (mModule != nullptr) {
        mModule->init();
    }
}

const QcrildServiceVersion &RadioImpl_1_5::getVersion() {
    static QcrildServiceVersion version(1, 5);
    return version;
}

RadioImpl_1_5::RadioImpl_1_5(qcril_instance_id_e_type instance): RadioImpl_1_4(instance) {
    QCRIL_LOG_INFO("RadioImpl_1_5()");
}

Return<void> RadioImpl_1_5::enableUiccApplications(int32_t serial, bool enable) {
    #if VDBG
    RLOGD("enableUiccApplications: serial %d enable %d", serial, enable);
    #endif
    std::shared_ptr<RadioContext> ctx = getContext(serial);
    auto msg = std::make_shared<RilRequestEnableUiccAppMessage>(ctx, enable);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RadioResponseInfo respInfo = {};
            if (resp != nullptr) {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
            } else {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
            }
            RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, enableUiccApplicationsResponse, respInfo);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, enableUiccApplicationsResponse, respInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

Return<void> RadioImpl_1_5::areUiccApplicationsEnabled(int32_t serial) {
    #if VDBG
    RLOGD("areUiccApplicationsEnabled: serial %d", serial);
    #endif
    std::shared_ptr<RadioContext> ctx = getContext(serial);
    auto msg = std::make_shared<RilRequestGetUiccAppStatusMessage>(ctx);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            bool state = false;
            RadioResponseInfo respInfo = {};
            if (resp != nullptr) {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
                auto result =
                  std::static_pointer_cast<qcril::interfaces::RilGetUiccAppStatusResult_t>(resp->data);
                if(result != nullptr) {
                    state = (bool)result->state;
                }
            } else {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
            }
            RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, areUiccApplicationsEnabledResponse,
                respInfo, state);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        bool state = false;
        RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, areUiccApplicationsEnabledResponse,
            respInfo, state);
    }
    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

int RadioImpl_1_5::sendUiccAppsStatusChanged(std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg)
{
    auto ind = mRadioIndicationV1_5;
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true" );
    if (ind) {
        bool state = msg->getStatus();
        Return<void> ret = ind->uiccApplicationsEnablementChanged(
                RadioIndicationType::UNSOLICITED_ACK_EXP, state);
        checkReturnStatus(ret);
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
}

int RadioImpl_1_5::sendNwRegistrationReject(std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg)
{
    auto ind = mRadioIndicationV1_5;
    V1_5::CellIdentity hidl_cellIdentity;
    auto rejInfo = msg->getNwRegistrationRejectInfo();
     QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true" );
     if (ind) {
        convertCellIdentityRilToHidl(rejInfo.rilCellIdentity, hidl_cellIdentity);
         Return<void> ret = ind->registrationFailed(
                RadioIndicationType::UNSOLICITED_ACK_EXP,
                hidl_cellIdentity,
                hidl_string(rejInfo.choosenPlmn),
                (hidl_bitfield<V1_5::Domain>)rejInfo.domain,
                rejInfo.causeCode,
                rejInfo.additionalCauseCode);
         checkReturnStatus(ret);
     }
     QCRIL_LOG_FUNC_RETURN();
     return 0;
}

void convertRilBarringServiceType(const qcril::interfaces::RilBarringServiceType &inBarringService, V1_5::BarringInfo::ServiceType &outBarringService ) {
  if(qcril::interfaces::RilBarringServiceType::CS_SERVICE == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::CS_SERVICE;
  }
  else if(qcril::interfaces::RilBarringServiceType::PS_SERVICE == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::PS_SERVICE;
  }
  else if(qcril::interfaces::RilBarringServiceType::CS_VOICE == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::CS_VOICE;
  }
  else if(qcril::interfaces::RilBarringServiceType::MO_SIGNALLING == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::MO_SIGNALLING;
  }
  else if(qcril::interfaces::RilBarringServiceType::MO_DATA == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::MO_DATA;
  }
  else if(qcril::interfaces::RilBarringServiceType::CS_FALLBACK == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::CS_FALLBACK;
  }
  else if(qcril::interfaces::RilBarringServiceType::MMTEL_VOICE == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::MMTEL_VOICE;
  }
  else if(qcril::interfaces::RilBarringServiceType::MMTEL_VIDEO == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::MMTEL_VIDEO;
  }
  else if(qcril::interfaces::RilBarringServiceType::EMERGENCY == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::EMERGENCY;
  }
  else if(qcril::interfaces::RilBarringServiceType::SMS == inBarringService)
  {
    outBarringService = V1_5::BarringInfo::ServiceType::SMS;
  }
}

void convertRilBarringType(const qcril::interfaces::RilBarringType &inBarringType, V1_5::BarringInfo::BarringType &outBarringType ) {
  if(qcril::interfaces::RilBarringType::NONE == inBarringType)
  {
    outBarringType = V1_5::BarringInfo::BarringType::NONE;
  }
  else if(qcril::interfaces::RilBarringType::CONDITIONAL == inBarringType)
  {
    outBarringType = V1_5::BarringInfo::BarringType::CONDITIONAL;
  }
  else if(qcril::interfaces::RilBarringType::UNCONDITIONAL == inBarringType)
  {
    outBarringType = V1_5::BarringInfo::BarringType::UNCONDITIONAL;
  }
}

void convertRilBarringInfoList(const std::vector<RILBarringInfo> &rilBarInfo, hidl_vec<V1_5::BarringInfo> &records ) {
    auto num = rilBarInfo.size();
    records.resize(num);
    V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional cond{};

    QCRIL_LOG_INFO("num: %d", num );
    for (unsigned int i = 0; i < num; i++) {
        QCRIL_LOG_INFO("iteration i: %d", i );
        QCRIL_LOG_INFO("serviceType: %d", rilBarInfo[i].barringService );
        QCRIL_LOG_INFO("barringType: %d", rilBarInfo[i].barringType );
        QCRIL_LOG_INFO("barringFactor: %d", rilBarInfo[i].typeSpecificInfo.barringFactor );
        QCRIL_LOG_INFO("barringTimeSeconds: %d", rilBarInfo[i].typeSpecificInfo.barringTimeSeconds );
        QCRIL_LOG_INFO("isBarred: %d", rilBarInfo[i].typeSpecificInfo.isBarred );
        convertRilBarringServiceType(rilBarInfo[i].barringService, records[i].serviceType);
        convertRilBarringType(rilBarInfo[i].barringType, records[i].barringType);
        cond.factor = rilBarInfo[i].typeSpecificInfo.barringFactor;
        cond.timeSeconds = rilBarInfo[i].typeSpecificInfo.barringTimeSeconds;
        cond.isBarred = rilBarInfo[i].typeSpecificInfo.isBarred;
        records[i].barringTypeSpecificInfo.conditional(cond);
    }
}

int RadioImpl_1_5::sendCellBarring(std::shared_ptr<RilUnsolCellBarringMessage> msg)
{
    auto ind = mRadioIndicationV1_5;
    V1_5::CellIdentity hidl_cellIdentity{};
    hidl_vec<V1_5::BarringInfo> hidl_barringInfo {};
    auto barringInfo = msg->getCellBarringInfo();
     QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true" );
     if (ind) {
        convertCellIdentityRilToHidl(barringInfo.rilCellIdentity, hidl_cellIdentity);
        convertRilBarringInfoList(barringInfo.barring_info, hidl_barringInfo);
        Return<void> ret = ind->barringInfoChanged(
                RadioIndicationType::UNSOLICITED,
                hidl_cellIdentity,
                hidl_barringInfo);
         checkReturnStatus(ret);
     }
     QCRIL_LOG_FUNC_RETURN();
     return 0;
}

Return<void> RadioImpl_1_5::setIndicationFilter_1_5(int32_t serial,
    hidl_bitfield<V1_5::IndicationFilter> indicationFilter)
{
    QCRIL_LOG_FUNC_ENTRY("serial=%d, indicationFilter=%d", serial, indicationFilter);
    bool is_barring = false;

#ifndef QMI_RIL_UTF
    std::shared_ptr<rildata::RegistrationFailureReportingStatusMessage> regFailureReportingStatusMsg =
        std::make_shared<rildata::RegistrationFailureReportingStatusMessage>(
            (int32_t) indicationFilter & V1_5::IndicationFilter::REGISTRATION_FAILURE);
    if (regFailureReportingStatusMsg) {
        regFailureReportingStatusMsg->broadcast();
    } else {
        QCRIL_LOG_ERROR("Failed to create message NasPhysChanConfigReportingStatus.");
    }
#endif

    if(indicationFilter & V1_5::IndicationFilter::BARRING_INFO)
    {
        is_barring = true;
    }

    std::shared_ptr<RadioContext> ctx = getContext(serial);
    auto msg = std::make_shared<RilRequestSetUnsolBarringFilterMessage>(ctx, is_barring, indicationFilter);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RadioResponseInfo respInfo = {};
            if ((resp != nullptr) &&(resp->errorCode == RIL_E_SUCCESS)) {
                auto int32Filter = std::static_pointer_cast<qcril::interfaces::RilUnsolBarringFilterResult_t>(resp->data);
                if (int32Filter != nullptr) {
                    setIndicationFilter_1_2(serial, int32Filter->mIndicationFilter);
                } else {
                    populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
                    RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponse, setIndicationFilterResponse, respInfo);
                }
            } else if ((resp != nullptr) &&(resp->errorCode != RIL_E_SUCCESS)){
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponse, setIndicationFilterResponse, respInfo);
            }else{
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponse, setIndicationFilterResponse, respInfo);
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponse, setIndicationFilterResponse, respInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

RIL_RadioAccessNetworks
    RadioImpl_1_5::convertHidlRadioAccessNetworkToRilRadioAccessNetwork(
        V1_5::RadioAccessNetworks ran)
{
    switch(ran)
    {
        case V1_5::RadioAccessNetworks::GERAN:
            return RIL_RadioAccessNetworks::GERAN;
        case V1_5::RadioAccessNetworks::UTRAN:
            return RIL_RadioAccessNetworks::UTRAN;
        case V1_5::RadioAccessNetworks::EUTRAN:
            return RIL_RadioAccessNetworks::EUTRAN;
        case V1_5::RadioAccessNetworks::NGRAN:
            return RIL_RadioAccessNetworks::NGRAN;
        case V1_5::RadioAccessNetworks::CDMA2000:
            return RIL_RadioAccessNetworks::CDMA;
        default:
            return RIL_RadioAccessNetworks::UNKNOWN;
    }
}

RadioError RadioImpl_1_5::convertHidlRasToRilRas(RIL_RadioAccessSpecifier& ras_to,
    const V1_5::RadioAccessSpecifier& ras_from) {

    ras_to.radio_access_network =
        convertHidlRadioAccessNetworkToRilRadioAccessNetwork(
            ras_from.radioAccessNetwork);
    ras_to.channels_length = ras_from.channels.size();

    std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);
    switch (ras_from.radioAccessNetwork) {
        case V1_5::RadioAccessNetworks::GERAN:
            if (ras_from.bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::geranBands) {
                ras_to.bands_length = ras_from.bands.geranBands().size();
                for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
                    ras_to.bands.geran_bands[idx] =
                        static_cast<RIL_GeranBands>(ras_from.bands.geranBands()[idx]);
                }
            }
            break;
        case V1_5::RadioAccessNetworks::UTRAN:
            if (ras_from.bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::utranBands) {
                ras_to.bands_length = ras_from.bands.utranBands().size();
                for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
                    ras_to.bands.utran_bands[idx] =
                        static_cast<RIL_UtranBands>(ras_from.bands.utranBands()[idx]);
                }
            }
            break;
        case V1_5::RadioAccessNetworks::EUTRAN:
            if (ras_from.bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::eutranBands) {
                ras_to.bands_length = ras_from.bands.eutranBands().size();
                for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
                    ras_to.bands.eutran_bands[idx] =
                        static_cast<RIL_EutranBands>(ras_from.bands.eutranBands()[idx]);
                }
            }
            break;
        case V1_5::RadioAccessNetworks::NGRAN:
            if (ras_from.bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::ngranBands) {
                ras_to.bands_length = ras_from.bands.ngranBands().size();
                for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
                    ras_to.bands.ngran_bands[idx] =
                        static_cast<RIL_NgranBands>(ras_from.bands.ngranBands()[idx]);
                }

            }
            break;
        default:
            return RadioError::INVALID_ARGUMENTS;
    }

    return RadioError::NONE;
}

RadioError RadioImpl_1_5::sanityCheck(const hidl_vec<V1_5::RadioAccessSpecifier>& ras) {
    // sanity check
    if (ras.size() > MAX_RADIO_ACCESS_NETWORKS) {
        return RadioError::INVALID_ARGUMENTS;
    }

    for (size_t i = 0; i < ras.size(); ++i) {
        if (ras[i].channels.size() > MAX_CHANNELS) {
            return RadioError::INVALID_ARGUMENTS;
        }
        // bands is safe union, make sure to check descriminator before accessing
        // any safe union fields
        if ((ras[i].bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::geranBands) &&
            (ras[i].bands.geranBands().size() > MAX_BANDS )) {
            return RadioError::INVALID_ARGUMENTS;
        } else if ((ras[i].bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::utranBands) &&
            (ras[i].bands.utranBands().size() > MAX_BANDS )) {
            return RadioError::INVALID_ARGUMENTS;
        } else if ((ras[i].bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::eutranBands) &&
            (ras[i].bands.eutranBands().size() > MAX_BANDS )) {
            return RadioError::INVALID_ARGUMENTS;
        } else if ((ras[i].bands.getDiscriminator() ==
                V1_5::RadioAccessSpecifier::Bands::hidl_discriminator::ngranBands) &&
            (ras[i].bands.ngranBands().size() > MAX_BANDS )) {
            return RadioError::INVALID_ARGUMENTS;
        }
    }

    return RadioError::NONE;
}

RadioError RadioImpl_1_5::fillNetworkScanRequest_1_5(const V1_5::NetworkScanRequest& request,
        RIL_NetworkScanRequest &scanRequest) {
    RadioError error = RadioError::NONE;

    error = sanityCheck(request.specifiers);
    if(error != RadioError::NONE) {
        RLOGE("sanity check failed - %d", error);
        return error;
    }

    if (request.type == V1_1::ScanType::ONE_SHOT) {
      scanRequest.type = RIL_ONE_SHOT;
    } else if (request.type == V1_1::ScanType::PERIODIC) {
      scanRequest.type = RIL_PERIODIC;
    }
    scanRequest.interval = request.interval;
    scanRequest.maxSearchTime = request.maxSearchTime;
    scanRequest.incrementalResults = request.incrementalResults;
    scanRequest.incrementalResultsPeriodicity = request.incrementalResultsPeriodicity;
    scanRequest.mccMncLength = request.mccMncs.size();
    scanRequest.specifiers_length = request.specifiers.size();

    for (size_t i = 0; i < request.specifiers.size(); ++i) {
        error = convertHidlRasToRilRas(scanRequest.specifiers[i], request.specifiers[i]);
        if (error != RadioError::NONE) {
            RLOGE("failed to convert hidl fields to ril fields - %d", error);
            return error;
        }
    }

    for (size_t i = 0; i < request.mccMncs.size(); ++i) {
        strlcpy(scanRequest.mccMncs[i], request.mccMncs[i].c_str(), MAX_MCC_MNC_LEN+1);
    }

    return RadioError::NONE;
}

Return<void> RadioImpl_1_5::startNetworkScan_1_5(int32_t serial, const V1_5::NetworkScanRequest& request) {
    QCRIL_LOG_FUNC_ENTRY();
    RLOGD("startNetworkScan_1_5: serial %d", serial);

    std::shared_ptr<RilRequestStartNetworkScanMessage> msg = nullptr;
    RIL_NetworkScanRequest scanRequest = {};
    auto res = fillNetworkScanRequest_1_5(request, scanRequest);
    if (res == RadioError::NONE) {
        std::shared_ptr<RadioContext> ctx = getContext(serial);
        if (ctx == nullptr ||
            (msg = std::make_shared<RilRequestStartNetworkScanMessage>(ctx, scanRequest)) == nullptr) {
            res = RadioError::NO_MEMORY;
        }
    }

    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RadioResponseInfo respInfo = {};
            if (resp != nullptr) {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
            } else {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_INTERNAL_ERR);
            }
            RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, startNetworkScanResponse_1_5, respInfo);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo {RadioResponseType::SOLICITED, serial, res};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, startNetworkScanResponse_1_5, respInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

RadioError RadioImpl_1_5::fillSetSystemSelectionChannelRequest_1_5(
        const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers,
        RIL_SysSelChannels &request) {
    RadioError error = RadioError::NONE;

    error = sanityCheck(specifiers);
    if(error != RadioError::NONE) {
        RLOGE("sanity check failed - %d", error);
        return error;
    }

    request.specifiers_length = specifiers.size();
    for (size_t i = 0; i < specifiers.size(); ++i) {
        error = convertHidlRasToRilRas(request.specifiers[i], specifiers[i]);
        if (error != RadioError::NONE) {
            RLOGE("failed to convert hidl fields to ril fields - %d", error);
            return error;
        }
    }
    return RadioError::NONE;
}

Return<void> RadioImpl_1_5::setSystemSelectionChannels_1_5(int32_t serial, bool specifyChannels,
        const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers) {
    RLOGD("setSystemSelectionChannels: serial %d, channel info valid - %d",
        serial, specifyChannels);
    std::shared_ptr<RilRequestSetSysSelChannelsMessage> msg;
    RIL_SysSelChannels ril_info = {};

    auto res = RadioError::NONE;

    // If specifyChannels is true scan specific bands otherwise scan all bands
    // If specifier length is zero, RIL should scan all bands.
    if (specifyChannels == true) {
        res = fillSetSystemSelectionChannelRequest_1_5(specifiers, ril_info);
    }

    if (res == RadioError::NONE) {
        std::shared_ptr<RadioContext> ctx = getContext(serial);
        if ((msg = std::make_shared<RilRequestSetSysSelChannelsMessage>(ctx, ril_info)) == nullptr) {
            res = RadioError::NO_MEMORY;
        }
    }

    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RadioResponseInfo respInfo = {};
            if (resp != nullptr) {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
            } else {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
            }
            RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, setSystemSelectionChannelsResponse_1_5, respInfo);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, res};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, setSystemSelectionChannelsResponse_1_5, respInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

RIL_RadioTechnology getRilRadioTechnologyFromRan(V1_5::RadioAccessNetworks ran) {
    switch(ran) {
        case V1_5::RadioAccessNetworks::GERAN:
            return RADIO_TECH_GSM;
        case V1_5::RadioAccessNetworks::UTRAN:
            return RADIO_TECH_UMTS;
        case V1_5::RadioAccessNetworks::EUTRAN:
            return RADIO_TECH_LTE;
        case V1_5::RadioAccessNetworks::NGRAN:
            return RADIO_TECH_5G;
        case V1_5::RadioAccessNetworks::CDMA2000:
            return RADIO_TECH_IS95A;
        default: {
            break;
        }
    }

    return RADIO_TECH_UNKNOWN;
}

Return<void> RadioImpl_1_5::setNetworkSelectionModeManual_1_5(int32_t serial,
    const hidl_string& operatorNumeric, V1_5::RadioAccessNetworks ran) {
    RLOGD("setNetworkSelectionModeManual_1_5: serial %d", serial);
    std::shared_ptr<RadioContext> ctx = getContext(serial);
    RIL_RadioTechnology rat = getRilRadioTechnologyFromRan(ran);
    auto msg = std::make_shared<RilRequestSetNetworkSelectionManualMessage>(ctx, operatorNumeric, rat);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RadioResponseInfo respInfo = {};
            if (resp != nullptr) {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
            } else {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
            }
            RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, setNetworkSelectionModeManualResponse_1_5, respInfo);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, setNetworkSelectionModeManualResponse_1_5, respInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

int RadioImpl_1_5::sendNetworkScanResult(std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();

    auto ind = mRadioIndicationV1_5;
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true" );

    if (ind) {
        V1_5::NetworkScanResult resultV1_5;
        resultV1_5.status = (V1_1::ScanStatus) msg->getStatus();
        resultV1_5.error = (V1_0::RadioError) msg->getError();

        convertRilCellInfoList(msg->getNetworkInfo(), resultV1_5.networkInfos);
        Return<void> retStatus = ind->networkScanResult_1_5(
                RadioIndicationType::UNSOLICITED_ACK_EXP, resultV1_5);
        checkReturnStatus(retStatus);
    } else {
        RadioImpl_1_4::sendNetworkScanResult(msg);
    }

    QCRIL_LOG_FUNC_RETURN();
    return 0;
}

void RadioImpl_1_5::fillVoiceRegistrationStateResponse(V1_5::RegStateResult &out, const RIL_VoiceRegistrationStateResponse &in) {
    out.regState = (V1_0::RegState) in.regState;
    out.rat = (V1_4::RadioTechnology) in.rat;

    out.reasonForDenial = (V1_5::RegistrationFailCause) in.reasonForDenial;

    convertCellIdentityRilToHidl(in.cellIdentity, out.cellIdentity);

    if (in.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_CDMA) {
        V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo cdmaInfo;
        cdmaInfo.cssSupported = static_cast<bool>(in.cssSupported);
        cdmaInfo.roamingIndicator = in.roamingIndicator;
        cdmaInfo.systemIsInPrl = (V1_5::PrlIndicator) in.systemIsInPrl;
        cdmaInfo.defaultRoamingIndicator = in.defaultRoamingIndicator;
        out.accessTechnologySpecificInfo.cdmaInfo(std::move(cdmaInfo));
    }
    else
    {
        out.registeredPlmn = hidl_string(string(in.cellIdentity.cellIdentityGsm.mcc) +
            string(in.cellIdentity.cellIdentityGsm.mnc));
    }
}

Return<void> RadioImpl_1_5::getVoiceRegistrationState_1_5(int32_t serial) {
    RLOGD("RadioImpl_1_5::getVoiceRegistrationState_1_5: serial %d", serial);

    qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
    radioServiceRwlockPtr->lock_shared();
    if (mRadioResponseV1_5 == nullptr)
    {
      radioServiceRwlockPtr->unlock_shared();
      QCRIL_LOG_INFO("mRadioResponseV1_5 == nullptr , fallback to v1_4 request");
      return RadioImpl_1_4::getVoiceRegistrationState(serial);
    }
    radioServiceRwlockPtr->unlock_shared();

    std::shared_ptr<RadioContext> ctx = getContext(serial);
    auto msg = std::make_shared<RilRequestGetVoiceRegistrationMessage>(ctx);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            if (resp != nullptr) {
                RadioResponseInfo respInfo = {};
                V1_5::RegStateResult regResponse = {};
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
                auto rilRegResult = std::static_pointer_cast<qcril::interfaces::RilGetVoiceRegResult_t>(resp->data);
                if (rilRegResult != nullptr) {
                    fillVoiceRegistrationStateResponse(regResponse, rilRegResult->respData);
                }
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5,
                        getVoiceRegistrationStateResponse_1_5, respInfo, regResponse);
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo {RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        V1_5::RegStateResult regResponse = {};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5,
                getVoiceRegistrationStateResponse_1_5, respInfo, regResponse);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

void RadioImpl_1_5::fillDataRegistrationStateResponse(V1_5::RegStateResult &out, const RIL_DataRegistrationStateResponse &in) {
    out.regState = (V1_0::RegState) in.regState;
    out.rat = (V1_4::RadioTechnology) in.rat;

    if (in.cellIdentity.cellInfoType != RIL_CELL_INFO_TYPE_CDMA) {
        out.registeredPlmn = hidl_string(string(in.cellIdentity.cellIdentityGsm.mcc) +
            string(in.cellIdentity.cellIdentityGsm.mnc));
    }

    // TODO: Add mapping
    out.reasonForDenial = (V1_5::RegistrationFailCause) in.reasonDataDenied;

    convertCellIdentityRilToHidl(in.cellIdentity, out.cellIdentity);

    if (in.cellIdentity.cellInfoType == RIL_CELL_INFO_TYPE_LTE) {
        V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo eutranInfo;

        if (in.lteVopsInfoValid) {
            eutranInfo.lteVopsInfo.isVopsSupported = static_cast<bool>(in.lteVopsInfo.isVopsSupported);
            eutranInfo.lteVopsInfo.isEmcBearerSupported = static_cast<bool>(in.lteVopsInfo.isEmcBearerSupported);
        }

        if (in.nrIndicatorsValid) {
            eutranInfo.nrIndicators.isEndcAvailable = static_cast<bool>(in.nrIndicators.isEndcAvailable);
            eutranInfo.nrIndicators.isDcNrRestricted = static_cast<bool>(in.nrIndicators.isDcNrRestricted);
            eutranInfo.nrIndicators.isNrAvailable = static_cast<bool>(in.nrIndicators.plmnInfoListR15Available);
        }

        if (in.lteVopsInfoValid || in.nrIndicatorsValid) {
            out.accessTechnologySpecificInfo.eutranInfo(std::move(eutranInfo));
        }
    }
}

Return<void> RadioImpl_1_5::getDataRegistrationState_1_5(int32_t serial) {
    RLOGD("RadioImpl_1_5::getDataRegistrationState_1_5: serial %d", serial);

    qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
    radioServiceRwlockPtr->lock_shared();
    if (mRadioResponseV1_5 == nullptr)
    {
      radioServiceRwlockPtr->unlock_shared();
      QCRIL_LOG_INFO("mRadioResponseV1_5 == nullptr , fallback to v1_4 request");
      return RadioImpl_1_4::getDataRegistrationState(serial);
    }
    radioServiceRwlockPtr->unlock_shared();

    std::shared_ptr<RadioContext> ctx = getContext(serial);
    auto msg = std::make_shared<RilRequestGetDataRegistrationMessage>(ctx);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            if (resp != nullptr) {
                RadioResponseInfo respInfo = {};
                V1_5::RegStateResult regResponse = {};
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
                auto rilRegResult = std::static_pointer_cast<qcril::interfaces::RilGetDataRegResult_t>(resp->data);
                if (rilRegResult != nullptr) {
                    fillDataRegistrationStateResponse(regResponse, rilRegResult->respData);
                }
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5,
                        getDataRegistrationStateResponse_1_5, respInfo, regResponse);
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo {RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        V1_5::RegStateResult regResponse = {};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5,
                getDataRegistrationStateResponse_1_5, respInfo, regResponse);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

Return<void> RadioImpl_1_5::setRadioPower_1_5(int32_t serial,
    bool on, bool forEmergencyCall, bool preferredForEmergencyCall) {
    RLOGD("setRadioPower: serial %d on %d", serial, on);

    std::shared_ptr<RadioContext> ctx = getContext(serial);
    auto msg = std::make_shared<RilRequestRadioPowerMessage>(ctx, on,
        forEmergencyCall, preferredForEmergencyCall);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RadioResponseInfo respInfo = {};
            if (resp != nullptr) {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
            } else {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
            }
            RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, setRadioPowerResponse_1_5, respInfo);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, setRadioPowerResponse_1_5, respInfo);
    }
    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

int RadioImpl_1_5::sendGetCellInfoListResponse(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
    QCRIL_LOG_FUNC_ENTRY();

    auto rsp = mRadioResponseV1_5;
    if (rsp) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_5::CellInfo> records;
        if ((response == NULL && responseLen != 0)
                || responseLen % sizeof(RIL_CellInfo_v12) != 0) {
            RLOGE("getCellInfoListResponse: Invalid response");
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            convertRilCellInfoList(response, responseLen, records);
        }

        Return<void> retStatus;
        retStatus = rsp->getCellInfoListResponse_1_5(responseInfo, records);
        checkReturnStatus(retStatus);
    } else {
        RadioImpl_1_4::sendGetCellInfoListResponse(slotId, responseType, serial,
                e, response, responseLen);
    }

    QCRIL_LOG_FUNC_RETURN();
    return 0;
}

int RadioImpl_1_5::sendCellInfoListInd(int slotId, int indicationType, int token,
        RIL_Errno e, void *response, size_t responseLen)
{
    QCRIL_LOG_FUNC_ENTRY();

    if (mRadioIndicationV1_5) {
        hidl_vec<V1_5::CellInfo> records;
        convertRilCellInfoList(response, responseLen, records);
        Return<void> retStatus = mRadioIndicationV1_5->cellInfoList_1_5(
                convertIntToRadioIndicationType(indicationType), records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("cellInfoListInd: radioService[%d]->mRadioIndication == NULL", slotId);
        RadioImpl_1_3::sendCellInfoListInd(slotId, indicationType, token, e,
                response, responseLen);
    }

    QCRIL_LOG_FUNC_RETURN();
    return 0;
}

RIL_UIM_PersoSubstate RadioImpl_1_5::convertHidlToRilPersoType(V1_5::PersoSubstate persoType){
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("convertHidlToRilPersoType: persoType %d ", persoType);
  switch(persoType) {
    case V1_5::PersoSubstate::SIM_SPN:
      return RIL_UIM_PERSOSUBSTATE_SIM_SPN;
    case V1_5::PersoSubstate::SIM_SPN_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_SPN_PUK;
    case V1_5::PersoSubstate::SIM_SP_EHPLMN:
      return RIL_UIM_PERSOSUBSTATE_SIM_SP_EHPLMN;
    case V1_5::PersoSubstate::SIM_SP_EHPLMN_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_SP_EHPLMN_PUK;
    case V1_5::PersoSubstate::SIM_ICCID:
      return RIL_UIM_PERSOSUBSTATE_SIM_ICCID;
    case V1_5::PersoSubstate::SIM_ICCID_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_ICCID_PUK;
    case V1_5::PersoSubstate::SIM_IMPI:
      return RIL_UIM_PERSOSUBSTATE_SIM_IMPI;
    case V1_5::PersoSubstate::SIM_IMPI_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_IMPI_PUK;
    case V1_5::PersoSubstate::SIM_NS_SP:
      return RIL_UIM_PERSOSUBSTATE_SIM_NS_SP;
    case V1_5::PersoSubstate::SIM_NS_SP_PUK:
      return RIL_UIM_PERSOSUBSTATE_SIM_NS_SP_PUK;
    default:
      return (RIL_UIM_PersoSubstate)persoType;
      /* Only above values require conversion, others are 1:1 mapped */
  }
}

Return<void> RadioImpl_1_5::supplySimDepersonalization(int32_t serial,
           V1_5::PersoSubstate persoType, const hidl_string& controlKey) {
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("supplySimDepersonalization: serial %d  persoType %d ", serial, persoType);
    RIL_UIM_PersoSubstate   rilPersoType = convertHidlToRilPersoType(persoType);
    QCRIL_LOG_INFO("rilPersoType %d ", rilPersoType);
    auto msg = std::make_shared<UimEnterDePersoRequestMsg>(controlKey, rilPersoType);
    if (msg) {
        GenericCallback<RIL_UIM_PersoResponse> cb(
            ([this, serial, persoType](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_UIM_PersoResponse> responseDataPtr) -> void {
                RadioResponseInfo responseInfo{RadioResponseType::SOLICITED, serial, RadioError::INTERNAL_ERR};
                int retries = -1;

                if (solicitedMsg && responseDataPtr &&
                    status == Message::Callback::Status::SUCCESS) {
                    responseInfo.error = (RadioError)responseDataPtr->err;
                    retries = responseDataPtr->no_of_retries;
                }
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, supplySimDepersonalizationResponse,
                        responseInfo, persoType, retries);
        }));
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo responseInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, supplySimDepersonalizationResponse,
                                 responseInfo, persoType, -1);
    }
    QCRIL_LOG_FUNC_RETURN();
    return Void();
}
Return<void> RadioImpl_1_5::getBarringInfo(int32_t serial) {
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("getBarringInfo: serial %d", serial);
    std::shared_ptr<RadioContext> ctx = getContext(serial);
    auto msg = std::make_shared<RilRequestGetBarringInfoMessage>(ctx);
    if (msg != nullptr) {
        GenericCallback<QcRilRequestMessageCallbackPayload> cb(
            [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RadioResponseInfo respInfo = {};
            V1_5::CellIdentity cellIdentity {};
            hidl_vec<V1_5::BarringInfo> barringInfo {};
            if ((resp != nullptr) && (resp->errorCode == RIL_E_SUCCESS)) {
                auto rilCellBarring = std::static_pointer_cast<qcril::interfaces::RilCellBarringInfo_t>(resp->data);
                if (rilCellBarring != nullptr) {
                    convertCellIdentityRilToHidl(rilCellBarring->rilCellIdentity, cellIdentity);
                    convertRilBarringInfoList(rilCellBarring->barring_info, barringInfo);
                    populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
                }
                else
                {
                    populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
                }
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, getBarringInfoResponse, respInfo, cellIdentity, barringInfo);
            } else if ((resp != nullptr) &&(resp->errorCode != RIL_E_SUCCESS)){
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, resp->errorCode);
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponse, setIndicationFilterResponse, respInfo);
            } else {
                populateResponseInfo(respInfo, serial, RESPONSE_SOLICITED, RIL_E_NO_MEMORY);
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, getBarringInfoResponse, respInfo, cellIdentity, barringInfo);
            }

        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo respInfo{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        const V1_5::CellIdentity cellIdentity {};
        const hidl_vec<V1_5::BarringInfo> barringInfo {};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, getBarringInfoResponse, respInfo, cellIdentity, barringInfo);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}

void RadioImpl_1_5::convertGetIccCardStatusResponse(RadioResponseInfo &responseInfo,
        V1_5::CardStatus &cardStatus, int responseType,
        int serial, std::shared_ptr<RIL_UIM_CardStatus> p_cur) {
    if (p_cur == NULL)
    {
      populateResponseInfo(responseInfo, serial, responseType, RIL_E_INVALID_RESPONSE);
      return;
    }
    else
    {
      populateResponseInfo(responseInfo, serial, responseType, (RIL_Errno)p_cur->err);
    }

    V1_0::CardStatus &cardStatus_1_0 = cardStatus.base.base.base;
    if (p_cur->gsm_umts_subscription_app_index >= p_cur->num_applications
            || p_cur->cdma_subscription_app_index >= p_cur->num_applications
            || p_cur->ims_subscription_app_index >= p_cur->num_applications) {
        QCRIL_LOG_INFO("getIccCardStatusResponse: Invalid response");
        if (p_cur->err == RIL_UIM_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        return;
    } else {
        cardStatus_1_0.cardState = (CardState) p_cur->card_state;
        cardStatus_1_0.universalPinState = (PinState) p_cur->universal_pin_state;
        cardStatus_1_0.gsmUmtsSubscriptionAppIndex = p_cur->gsm_umts_subscription_app_index;
        cardStatus_1_0.cdmaSubscriptionAppIndex = p_cur->cdma_subscription_app_index;
        cardStatus_1_0.imsSubscriptionAppIndex = p_cur->ims_subscription_app_index;

        // Fill 1.1 fields
        cardStatus.base.base.physicalSlotId = p_cur->physical_slot_id;
        cardStatus.base.base.atr            = p_cur->atr;
        cardStatus.base.base.iccid          = p_cur->iccid;

        // Fill 1.4 fields
        if(!p_cur->eid.empty())
        {
            cardStatus.base.eid = p_cur->eid;
        }

        // Fill 1.5 applications fields instead of 1.0 fields
        RIL_UIM_AppStatus *rilAppStatus = p_cur->applications;
        cardStatus_1_0.applications.resize(p_cur->num_applications);
        cardStatus.applications.resize(p_cur->num_applications);
        V1_0::AppStatus *appStatus_1_0 = cardStatus_1_0.applications.data();
        V1_5::AppStatus *appStatus = cardStatus.applications.data();
        QCRIL_LOG_DEBUG("getIccCardStatusResponse: num_applications %d", p_cur->num_applications);
        for (int i = 0; i < p_cur->num_applications; i++) {
            /* Fill both V1_0 & V1_5 AppStatus values */
            appStatus_1_0[i].appType = (AppType) rilAppStatus[i].app_type;
            appStatus_1_0[i].appState = (AppState) rilAppStatus[i].app_state;
            appStatus_1_0[i].persoSubstate = (V1_0::PersoSubstate) rilAppStatus[i].perso_substate;
            appStatus_1_0[i].aidPtr = rilAppStatus[i].aid_ptr;
            appStatus_1_0[i].appLabelPtr = rilAppStatus[i].app_label_ptr;
            appStatus_1_0[i].pin1Replaced = rilAppStatus[i].pin1_replaced;
            appStatus_1_0[i].pin1 = (PinState) rilAppStatus[i].pin1;
            appStatus_1_0[i].pin2 = (PinState) rilAppStatus[i].pin2;

            appStatus[i].base.appType = (AppType) rilAppStatus[i].app_type;
            appStatus[i].base.appState = (AppState) rilAppStatus[i].app_state;
            appStatus[i].base.persoSubstate = (V1_0::PersoSubstate) rilAppStatus[i].perso_substate;
            appStatus[i].base.aidPtr = rilAppStatus[i].aid_ptr;
            appStatus[i].base.appLabelPtr = rilAppStatus[i].app_label_ptr;
            appStatus[i].base.pin1Replaced = rilAppStatus[i].pin1_replaced;
            appStatus[i].base.pin1 = (PinState) rilAppStatus[i].pin1;
            appStatus[i].base.pin2 = (PinState) rilAppStatus[i].pin2;

            // Fill 1.5 added field
            appStatus[i].persoSubstate = (V1_5::PersoSubstate)rilAppStatus[i].perso_substate;
        }

    }
    return;
}

Return<void> RadioImpl_1_5::getIccCardStatus(int32_t serial) {
#if VDBG
        RLOGD("getIccCardStatus: serial %d", serial);
#endif

  qtimutex::QtiSharedMutex *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
  radioServiceRwlockPtr->lock_shared();

  if (mRadioResponseV1_5 == nullptr)
  {
    radioServiceRwlockPtr->unlock_shared();
    return RadioImpl_1_4::getIccCardStatus(serial);
  }

  radioServiceRwlockPtr->unlock_shared();

  auto msg = std::make_shared<UimGetCardStatusRequestMsg>(qmi_ril_get_process_instance_id());
    if (msg) {
        GenericCallback<RIL_UIM_CardStatus> cb(
            ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_UIM_CardStatus> responseDataPtr) -> void {
                RadioResponseInfo responseInfo{RadioResponseType::SOLICITED, serial, RadioError::INTERNAL_ERR};
                V1_5::CardStatus cardStatus = {};

                if (solicitedMsg && responseDataPtr &&
                    status == Message::Callback::Status::SUCCESS) {
                    convertGetIccCardStatusResponse(responseInfo, cardStatus,
                            RESPONSE_SOLICITED, serial, responseDataPtr);

                }
                RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, getIccCardStatusResponse_1_5,
                        responseInfo, cardStatus);
        }));
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        V1_5::CardStatus cardStatus = {};
        RADIO_HIDL_SEND_RESPONSE(mSlotId, mRadioResponseV1_5, getIccCardStatusResponse_1_5, rsp, cardStatus);
    }

    QCRIL_LOG_FUNC_RETURN();
    return Void();
}
