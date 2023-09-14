/******************************************************************************
#  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ_SSRepCritMod"
#include <functional>
#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include <framework/Dispatcher.h>
#include <modules/qmi/QmiIndMessage.h>
#include <modules/qmi/QmiAsyncResponseMessage.h>
#include <modules/nas/NasSetSignalStrengthCriteria.h>
#include <modules/nas/qcril_qmi_nas.h>
#include <modules/qmi/NasModemEndPoint.h>
#include <modules/qmi/ModemEndPointFactory.h>
#include <modules/qmi/EndpointStatusIndMessage.h>
#include <network_access_service_v01.h>
#include <modules/nas/qcril_nas_legacy.h>
#include <qcril_qmi_client.h>
#include <modules/dms/RadioPowerStatusMessage.h>
#include <modules/android/ClientConnectedMessage.h>

using std::placeholders::_1;

class SignalStrengthReportingCriteriaModule : public Module {
    public:
        SignalStrengthReportingCriteriaModule();
        ~SignalStrengthReportingCriteriaModule() {}
        void init();

    private:
        bool mReady = false;
        nas_config_sig_info2_req_msg_v01 req{};

        static constexpr unsigned int LTE_EXP_COUNT{3};
        unsigned int lte_req_count{0};
        unsigned int lte_disabled_count{0};
        static constexpr unsigned int NR5G_EXP_COUNT{3};
        unsigned int nr5g_req_count{0};
        unsigned int nr5g_disabled_count{0};

        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
        void handleNasQmiIndMessage(std::shared_ptr<QmiIndMessage> msg);
        void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
        void handleNasEndpointStatusIndMessage(std::shared_ptr<EndpointStatusIndMessage> msg);
        void handleNasSetSignalStrengthCriteria(std::shared_ptr<NasSetSignalStrengthCriteria> msg);
        void handleClientConnectedMessage(std::shared_ptr<ClientConnectedMessage> msg);
        void handleRadioPowerStatusMessage(std::shared_ptr<RadioPowerStatusMessage> msg);
        void cleanUp() {
            memset(&req, 0, sizeof(req));
        }
        void resetLteCounts();
        void reset5gnrCounts();
        void updateCriteria(nas_config_sig_info2_req_msg_v01 &out, const std::vector<SignalStrengthCriteriaEntry> &criteria);
};

static load_module<SignalStrengthReportingCriteriaModule> the_module;

SignalStrengthReportingCriteriaModule &getSignalStrengthReportingCriteriaModule() {
    return the_module.get_module();
}

SignalStrengthReportingCriteriaModule::SignalStrengthReportingCriteriaModule() {
    mName = "SignalStrengthReportingCriteriaModule";
    ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint();

    mMessageHandler = {
        HANDLER(QcrilInitMessage, SignalStrengthReportingCriteriaModule::handleQcrilInit),
        HANDLER_MULTI(QmiIndMessage, "NAS_QMI_IND", SignalStrengthReportingCriteriaModule::handleNasQmiIndMessage),
        // End Point Status Indication
        HANDLER_MULTI(EndpointStatusIndMessage, "NAS_ENDPOINT_STATUS_IND", SignalStrengthReportingCriteriaModule::handleNasEndpointStatusIndMessage),
        // Qmi Async Response
        HANDLER(QmiAsyncResponseMessage, SignalStrengthReportingCriteriaModule::handleQmiAsyncRespMessage),
        HANDLER(NasSetSignalStrengthCriteria, SignalStrengthReportingCriteriaModule::handleNasSetSignalStrengthCriteria),
        HANDLER(ClientConnectedMessage, SignalStrengthReportingCriteriaModule::handleClientConnectedMessage),
        HANDLER(RadioPowerStatusMessage, SignalStrengthReportingCriteriaModule::handleRadioPowerStatusMessage),
    };
}

void SignalStrengthReportingCriteriaModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
    QCRIL_LOG_DEBUG("[%s]: Handling msg = %s", mName.c_str(), msg->dump().c_str());
    /* Init QMI services.*/
    GenericCallback<string> gcb([](std::shared_ptr<Message> msg, Message::Callback::Status status, std::shared_ptr<void> payload) -> void {
            (void)msg;
            (void)status;
            (void)payload;
            });
    ModemEndPointFactory<NasModemEndPoint>::getInstance().buildEndPoint()->requestSetup("", &gcb);
}

void SignalStrengthReportingCriteriaModule::init()
{
    QCRIL_LOG_DEBUG("init");
    Module::init();
}

void SignalStrengthReportingCriteriaModule::handleNasQmiIndMessage(std::shared_ptr<QmiIndMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
}

void SignalStrengthReportingCriteriaModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
}

void SignalStrengthReportingCriteriaModule::handleNasEndpointStatusIndMessage(std::shared_ptr<EndpointStatusIndMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
    if (msg->getState() == ModemEndPoint::State::OPERATIONAL) {
        mReady = true;
    } else {
        mReady = false;
        QCRIL_LOG_DEBUG("Modem SSR. Resetting the counts for LTE and nr5g");
        resetLteCounts();
        reset5gnrCounts();
    }
}

void SignalStrengthReportingCriteriaModule::handleClientConnectedMessage(std::shared_ptr<ClientConnectedMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
    QCRIL_LOG_DEBUG("Client connected again. Resetting the counts for LTE and nr5g");
    resetLteCounts();
    reset5gnrCounts();
}

void SignalStrengthReportingCriteriaModule::handleRadioPowerStatusMessage(std::shared_ptr<RadioPowerStatusMessage> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = %s", msg->dump().c_str() );
}

#define MAX_THRESHOLDS 32
void fillGeran(nas_config_sig_info2_req_msg_v01 &config, const SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();

    if (entry.type != MT_RSSI)
        QCRIL_LOG_WARN("Unexpected geran measure type: %d", entry.type);

    if (entry.isEnabled) {
        if(sz > 0) {
            if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
            config.gsm_rssi_threshold_list_valid = TRUE;
            QCRIL_LOG_DEBUG("Setting threshold_list_len to %u", sz);
            config.gsm_rssi_threshold_list_len = sz;
            for (unsigned i = 0; i < sz; i++) {
                config.gsm_rssi_threshold_list[i] = entry.thresholds[i] * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling geran rssi threshold by setting delta to INT MAX");
            config.gsm_rssi_delta_valid = TRUE;
            config.gsm_rssi_delta = 65535;
        }

        if (entry.hysteresisDb >= 0) {
            QCRIL_LOG_DEBUG("Setting gsm rssi hysteresisDb");
            config.gsm_hysteresis_valid = 1;
            config.gsm_hysteresis_len = 1;
            config.gsm_hysteresis[0].signal = NAS_SIG_TYPE_RSSI_V01;
            config.gsm_hysteresis[0].delta = entry.hysteresisDb * 10;
        }
    } else {
        QCRIL_LOG_DEBUG("All types for gsm disabled. Setting gsm rssi delta to implementation defined value");
        config.gsm_rssi_delta_valid = TRUE;
        config.gsm_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
    }
}

void fillUtran(nas_config_sig_info2_req_msg_v01 &config, const SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();

    if (entry.type == MT_RSCP) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.wcdma_rscp_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting wcdma rscp threshold_list_len to %u", sz);
                config.wcdma_rscp_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.wcdma_rscp_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Disabling utran rscp threshold by setting delta to INT MAX");
                config.wcdma_rscp_delta_valid = TRUE;
                config.wcdma_rscp_delta = 65535;
            }
            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting wcdma rscp hysteresisDb");
                config.wcdma_hysteresis_valid = 1;
                config.wcdma_hysteresis_len = 1;
                config.wcdma_hysteresis[0].signal = NAS_SIG_TYPE_RSCP_V01;
                config.wcdma_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("All types for utran disabled. Setting utran rscp delta to implementation defined value");
            config.wcdma_rscp_delta_valid = TRUE;
            config.wcdma_rscp_delta = NasSetSignalStrengthCriteria::RSCP_DELTA;
        }
    } else if (entry.type == MT_RSSI) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.wcdma_rssi_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting wcdma rssi threshold_list_len to %u", sz);
                config.wcdma_rssi_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.wcdma_rssi_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Disabling utran rssi threshold by setting delta to INT MAX");
                config.wcdma_rssi_delta_valid = TRUE;
                config.wcdma_rssi_delta = 65535;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting wcdma rssi hysteresisDb");
                config.wcdma_hysteresis_valid = 1;
                config.wcdma_hysteresis_len = 1;
                config.wcdma_hysteresis[0].signal = NAS_SIG_TYPE_RSSI_V01;
                config.wcdma_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("All types for utran disabled. Setting utran rssi delta to implementation defined value");
            config.wcdma_rssi_delta_valid = TRUE;
            config.wcdma_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
        }
    } else {
        QCRIL_LOG_ERROR("Unexpected utran measure type: %d", entry.type);
    }
}

void setLteDeltaValues(nas_config_sig_info2_req_msg_v01 &config){
    config.lte_rsrp_delta_valid = TRUE;
    config.lte_rsrp_delta = NasSetSignalStrengthCriteria::RSRP_DELTA;
    config.lte_rsrq_delta_valid = TRUE;
    config.lte_rsrq_delta = NasSetSignalStrengthCriteria::RSRQ_DELTA;
    config.lte_snr_delta_valid = TRUE;
    config.lte_snr_delta = NasSetSignalStrengthCriteria::RSSNR_DELTA;
}

void fillEutran(nas_config_sig_info2_req_msg_v01 &config, const SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();
    if (entry.type == MT_RSRP) {
        if (entry.isEnabled) {
            if (sz > 0){
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.lte_rsrp_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting lte rsrp threshold_list_len to %u", sz);
                config.lte_rsrp_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.lte_rsrp_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Setting lte rsrp delta to INT MAX to disable thresholds from reporting");
                config.lte_rsrp_delta_valid = TRUE;
                config.lte_rsrp_delta = 65535;
            }
            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting lte rsrp hysteresisDb");
                config.lte_hysteresis_valid = 1;
                config.lte_hysteresis_len = 1;
                config.lte_hysteresis[0].signal = NAS_SIG_TYPE_RSRP_V01;
                config.lte_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling lte_rsrp by setting lte_rsrp delta to INT MAX");
            config.lte_rsrp_delta_valid = TRUE;
            config.lte_rsrp_delta = 65535;
        }
    } else if (entry.type == MT_RSRQ) {
        if (entry.isEnabled) {
            if (sz > 0){
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.lte_rsrq_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting lte rsrq threshold_list_len to %u", sz);
                config.lte_rsrq_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.lte_rsrq_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Setting lte rsrq delta to INT MAX to disable thresholds from reporting");
                config.lte_rsrq_delta_valid = TRUE;
                config.lte_rsrq_delta = 65535;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting lte rsrq hysteresisDb");
                config.lte_hysteresis_valid = 1;
                config.lte_hysteresis_len = 1;
                config.lte_hysteresis[0].signal = NAS_SIG_TYPE_RSRQ_V01;
                config.lte_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling lte_rsrq by setting lte rsrq delta to INT MAX");
            config.lte_rsrq_delta_valid = TRUE;
            config.lte_rsrq_delta = 65535;
        }
    } else if (entry.type == MT_RSSNR) {
        if (entry.isEnabled) {
            if(sz > 0){
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.lte_snr_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting lte rssnr threshold_list_len to %u", sz);
                config.lte_snr_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.lte_snr_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Setting lte rssnr delta to INT MAX to disable thresholds from reporting");
                config.lte_snr_delta_valid = TRUE;
                config.lte_snr_delta = 65535;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting lte rssnr hysteresisDb");
                config.lte_hysteresis_valid = 1;
                config.lte_hysteresis_len = 1;
                config.lte_hysteresis[0].signal = NAS_SIG_TYPE_SNR_V01;
                config.lte_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling lte snr by setting lte snr delta to INT MAX");
            config.lte_snr_delta_valid = TRUE;
            config.lte_snr_delta = 65535;
        }
    } else {
        QCRIL_LOG_ERROR("Unexpected lte measure type: %d", entry.type);
    }

    config.lte_sig_rpt_config_valid = TRUE;
    config.lte_sig_rpt_config = {
        .rpt_rate = NAS_LTE_SIG_RPT_RATE_3_SEC_V01,
        .avg_period = NAS_LTE_SIG_AVG_PRD_5_SEC_V01
    };
}

void fillCdma(nas_config_sig_info2_req_msg_v01 &config, const SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();

    if (entry.type != MT_RSSI)
        QCRIL_LOG_WARN("Unexpected cdma measure type: %d", entry.type);

    if (entry.isEnabled) {
        if (sz > 0) {
            if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
            config.cdma_rssi_threshold_list_valid = TRUE;
            QCRIL_LOG_DEBUG("Setting cdma rssi threshold_list_len to %u", sz);
            config.cdma_rssi_threshold_list_len = sz;
            for (unsigned i = 0; i < sz; i++) {
                config.cdma_rssi_threshold_list[i] = entry.thresholds[i] * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling cdma rssi threshold by setting delta to INT MAX");
            config.cdma_rssi_delta_valid = TRUE;
            config.cdma_rssi_delta = 65535;
        }

        if (entry.hysteresisDb >= 0) {
            QCRIL_LOG_DEBUG("Setting cdma rssi hysteresisDb");
            config.cdma_hysteresis_valid = 1;;
            config.cdma_hysteresis_len = 1;
            config.cdma_hysteresis[0].signal = NAS_SIG_TYPE_RSSI_V01;
            config.cdma_hysteresis[0].delta = entry.hysteresisDb * 10;
        }
    } else {
        QCRIL_LOG_DEBUG("All types for cdma disabled. Setting cdma rssi delta to implementation defined value");
        config.cdma_rssi_delta_valid = TRUE;
        config.cdma_rssi_delta = NasSetSignalStrengthCriteria::RSSI_DELTA;
    }
}

void setNr5gDeltaValues(nas_config_sig_info2_req_msg_v01 &config){
    config.nr5g_rsrp_delta_valid = TRUE;
    config.nr5g_rsrp_delta = NasSetSignalStrengthCriteria::RSRP_DELTA;
    config.nr5g_rsrq_delta_valid = TRUE;
    config.nr5g_rsrq_delta = NasSetSignalStrengthCriteria::RSRQ_DELTA;
    config.nr5g_snr_delta_valid = TRUE;
    config.nr5g_snr_delta = NasSetSignalStrengthCriteria::RSSNR_DELTA;
}


void fillNgran(nas_config_sig_info2_req_msg_v01 &config, const SignalStrengthCriteriaEntry &entry) {
    unsigned sz = entry.thresholds.size();
    if (entry.type == MT_SSRSRP) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.nr5g_rsrp_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting nr5g rsrp threshold_list_len to %u", sz);
                config.nr5g_rsrp_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.nr5g_rsrp_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Setting nr5g rsrp delta to INT MAX to disable thresholds from reporting");
                config.nr5g_rsrp_delta_valid = TRUE;
                config.nr5g_rsrp_delta = 65535;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting nr5g rsrp hysteresisDb");
                config.nr5g_hysteresis_valid = 1;
                config.nr5g_hysteresis_len = 1;
                config.nr5g_hysteresis[0].signal = NAS_SIG_TYPE_RSRP_V01;
                config.nr5g_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling nr5g_rsrp by setting nr5g rsrp delta to INT MAX");
            config.nr5g_rsrp_delta_valid = TRUE;
            config.nr5g_rsrp_delta = 65535;
        }
    } else if (entry.type == MT_SSRSRQ) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.nr5g_rsrq_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting nr5g rsrq threshold_list_len to %u", sz);
                config.nr5g_rsrq_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.nr5g_rsrq_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Setting nr5g rsrq delta to INT MAX to disable thresholds from reporting");
                config.nr5g_rsrq_delta_valid = TRUE;
                config.nr5g_rsrq_delta = 65535;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting nr5g rsrq hysteresisDb");
                config.nr5g_hysteresis_valid = 1;
                config.nr5g_hysteresis_len = 1;
                config.nr5g_hysteresis[0].signal = NAS_SIG_TYPE_RSRQ_V01;
                config.nr5g_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling nr5g_rsrq by setting nr5g rsrq delta to INT MAX");
            config.nr5g_rsrq_delta_valid = TRUE;
            config.nr5g_rsrq_delta = 65535;
        }
    } else if (entry.type == MT_SSSINR) {
        if (entry.isEnabled) {
            if (sz > 0) {
                if (sz >= MAX_THRESHOLDS) sz = MAX_THRESHOLDS;
                config.nr5g_snr_threshold_list_valid = TRUE;
                QCRIL_LOG_DEBUG("Setting nr5g snr threshold_list_len to %u", sz);
                config.nr5g_snr_threshold_list_len = sz;
                for (unsigned i = 0; i < sz; i++) {
                    config.nr5g_snr_threshold_list[i] = entry.thresholds[i] * 10;
                }
            } else {
                QCRIL_LOG_DEBUG("Setting nr5g snr delta to INT MAX to disable thresholds from reporting");
                config.nr5g_snr_delta_valid = TRUE;
                config.nr5g_snr_delta = 65535;
            }

            if (entry.hysteresisDb >= 0) {
                QCRIL_LOG_DEBUG("Setting nr5g snr hysteresisDb");
                config.nr5g_hysteresis_valid = 1;
                config.nr5g_hysteresis_len = 1;
                config.nr5g_hysteresis[0].signal = NAS_SIG_TYPE_SNR_V01;
                config.nr5g_hysteresis[0].delta = entry.hysteresisDb * 10;
            }
        } else {
            QCRIL_LOG_DEBUG("Disabling nr5g_snr by setting nr5g snr delta to INT MAX");
            config.nr5g_snr_delta_valid = TRUE;
            config.nr5g_snr_delta = 65535;
        }
    } else {
        QCRIL_LOG_ERROR("Unexpected nr5g measure type: %d", entry.type);
    }

    config.nr5g_sig_rpt_config_valid = TRUE;
    config.nr5g_sig_rpt_config = {
        .rpt_rate = NR5G_COMMON_RSSI_REPORTING_RATE_3_SEC_V01,
        .avg_period = NR5G_COMMON_RSSI_AVERAGING_PERIOD_5_SEC_V01
    };
}

void SignalStrengthReportingCriteriaModule::resetLteCounts(){
    lte_req_count = 0;
    lte_disabled_count = 0;
}

void SignalStrengthReportingCriteriaModule::reset5gnrCounts(){
    nr5g_req_count = 0;
    nr5g_disabled_count = 0;
}

void SignalStrengthReportingCriteriaModule::updateCriteria(nas_config_sig_info2_req_msg_v01 &out, const std::vector<SignalStrengthCriteriaEntry> &criteria) {
    QCRIL_LOG_FUNC_ENTRY();

    for (const SignalStrengthCriteriaEntry &entry: criteria) {
        QCRIL_LOG_DEBUG("Found entry criteria");

        if ((entry.isEnabled) && (entry.hysteresisMs >= 0)) {
            QCRIL_LOG_DEBUG("Setting the global hysteresis timer");
            out.hysteresis_timer_valid = TRUE;
            out.hysteresis_timer = entry.hysteresisMs;
        }

        switch(entry.ran) {
            case GERAN:
                QCRIL_LOG_DEBUG("RAN: GERAN");
                fillGeran(out, entry);
                break;
            case UTRAN:
                QCRIL_LOG_DEBUG("RAN: UTRAN");
                fillUtran(out, entry);
                break;
            case EUTRAN:
                QCRIL_LOG_DEBUG("RAN: EUTRAN");
                if ((entry.isEnabled) && ((entry.type == MT_RSRP) || (entry.type == MT_RSRQ) || (entry.type == MT_RSSNR))) {
                    lte_req_count++;
                    QCRIL_LOG_DEBUG("Current req = Enable ; lte req count %u ", lte_req_count);

                } else if ((!entry.isEnabled) && ((entry.type == MT_RSRP) || (entry.type == MT_RSRQ) || (entry.type == MT_RSSNR))){
                    lte_req_count++;
                    QCRIL_LOG_DEBUG("Current req = Disable ; lte req count %u", lte_req_count);
                    lte_disabled_count++;
                    if (lte_disabled_count == LTE_EXP_COUNT){
                        QCRIL_LOG_DEBUG("All Lte types are disabled. Setting delta for all lte types to implementation defined values");
                        setLteDeltaValues(out);
                        resetLteCounts();
                        return;
                    }
                }
                if (lte_req_count == LTE_EXP_COUNT){
                    QCRIL_LOG_DEBUG("All LTE reqs received. Resetting req and disabled count");
                    resetLteCounts();
                }
                fillEutran(out, entry);
                break;
            case CDMA:
                QCRIL_LOG_DEBUG("RAN: CDMA");
                fillCdma(out, entry);
                break;
            case IWLAN:
                QCRIL_LOG_DEBUG("RAN: IWLAN Unsupported");
                break;
            case NGRAN:
                QCRIL_LOG_DEBUG("RAN: NGRAN");
                if ((entry.isEnabled) && ((entry.type == MT_SSRSRP) || (entry.type == MT_SSRSRQ) || (entry.type == MT_SSSINR))) {
                    nr5g_req_count++;
                    QCRIL_LOG_DEBUG("Current req = Enable ; nr5g req count %u", nr5g_req_count);
                } else if ((!entry.isEnabled) && ((entry.type == MT_SSRSRP) || (entry.type == MT_SSRSRQ) || (entry.type == MT_SSSINR))){
                    nr5g_req_count++;
                    QCRIL_LOG_DEBUG("Current req = Disable ; nr5g req count %u", nr5g_req_count);
                    nr5g_disabled_count++;
                    if (nr5g_disabled_count == NR5G_EXP_COUNT){
                        QCRIL_LOG_DEBUG("All nr5g types are disabled. Setting delta for all nr5g types to implementation defined values");
                        setNr5gDeltaValues(out);
                        reset5gnrCounts();
                        return;
                    }
                }
                if (nr5g_req_count == NR5G_EXP_COUNT){
                    QCRIL_LOG_DEBUG("All nr5g reqs received. Resetting req and disabled count");
                    reset5gnrCounts();
                }
                fillNgran(out, entry);
                break;
            default:
                QCRIL_LOG_ERROR("RAN: Unknown value %d", entry.ran);
                break;
        }
    }
    QCRIL_LOG_FUNC_RETURN();
}

void SignalStrengthReportingCriteriaModule::handleNasSetSignalStrengthCriteria(std::shared_ptr<NasSetSignalStrengthCriteria> msg)
{
    QCRIL_LOG_DEBUG("Handling msg = NasSetSignalStrengthCriteria");
    if (msg) {
        QCRIL_LOG_DEBUG("Contents: %s", msg->dump().c_str());
        std::shared_ptr<RIL_Errno> rsp = nullptr;
        Message::Callback::Status status = Message::Callback::Status::FAILURE;

        if (mReady) {
            QCRIL_LOG_DEBUG("Making shared_ptr");
            rsp = std::make_shared<RIL_Errno>(RIL_E_SUCCESS);
            QCRIL_LOG_DEBUG("Calling updateCriteria");
            SignalStrengthReportingCriteriaModule::cleanUp();
            updateCriteria(req, msg->getCriteria());
            QCRIL_LOG_DEBUG("Returned from updateCriteria");
            qmi_client_error_type err = qmi_client_nas_send_async(QMI_NAS_CONFIG_SIG_INFO2_REQ_MSG_V01,
                    &req,
                    sizeof(req),
                    sizeof(nas_config_sig_info2_resp_msg_v01),
                    [msg] (unsigned int, std::shared_ptr<void> qmi_response, unsigned int, void *, qmi_client_error_type qmi_client_error) -> void {
                        QCRIL_LOG_DEBUG("Received response from QMI");
                        RIL_Errno ril_req_res =
                            qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                qmi_client_error,
                                &(std::static_pointer_cast<nas_config_sig_info2_resp_msg_v01>(qmi_response)->resp) );
                        std::shared_ptr<RIL_Errno> rsp = std::make_shared<RIL_Errno>(ril_req_res);
                        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, rsp);
                    },
                    NULL);
            if (err) {
                QCRIL_LOG_DEBUG("Received QMI error: %d", err);
                rsp = std::make_shared<RIL_Errno>(RIL_E_SYSTEM_ERR);
                msg->sendResponse(msg, Message::Callback::Status::SUCCESS, rsp);
            }
        } else {
            QCRIL_LOG_DEBUG("Responding not ready");
            rsp = std::make_shared<RIL_Errno>(RIL_E_RADIO_NOT_AVAILABLE);
            msg->sendResponse(msg, status, rsp);
        }
    }
}
