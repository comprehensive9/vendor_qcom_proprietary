/******************************************************************************
#  Copyright (c) 2017-2018, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "modules/qmi/NasModemEndPoint.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"

#include "modules/nas/NasUimHeaders.h"
#include "modules/nas/NasRequestDataShutdownMessage.h"

#include <modules/android/RilRequestMessage.h>
#include <modules/ims/ImsServiceStatusInd.h>
#include <modules/ims/ImsWfcSettingsStatusInd.h>
#include <modules/ims/ImsPdpStatusInd.h>
#include "modules/nas/NasSetVoiceDomainPreferenceRequest.h"

// Messages from Data
#include "modules/nas/NasEmbmsEnableDataConMessage.h"
#include "modules/nas/NasEmbmsDisableDataMessage.h"

// NAS exported API
#include "modules/nas/NasSetPsAttachDetachMessage.h"
#include "modules/nas/NasGetSysInfoMessage.h"

//interface header files
#include "interfaces/nas/NasPhysChanConfigMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRegistrationMessage.h"
#include "interfaces/nas/RilRequestGetDataRegistrationMessage.h"
#include "interfaces/nas/RilRequestOperatorMessage.h"
#include "interfaces/nas/RilRequestQueryNetworkSelectModeMessage.h"
#include "interfaces/nas/RilRequestAllowDataMessage.h"
#include "interfaces/nas/RilRequestQueryAvailNetworkMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionAutoMessage.h"
#include "interfaces/nas/RilRequestStopNetworkScanMessage.h"
#include "interfaces/nas/RilRequestStartNetworkScanMessage.h"
#include "interfaces/nas/RilRequestGetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestSetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestGetSignalStrengthMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRadioTechMessage.h"
#include "interfaces/nas/RilRequestSetCdmaSubscriptionSourceMessage.h"
#include "interfaces/nas/RilRequestExitEmergencyCallbackMessage.h"
#include "interfaces/nas/RilRequestSendDeviceStateMessage.h"
#include "interfaces/nas/RilRequestGetNeighborCellIdsMessage.h"
#include "interfaces/nas/RilRequestCdmaSubscriptionMessage.h"
#include "interfaces/nas/RilRequestSetCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestQueryCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestGetCdmaSubscriptionSourceMessage.h"
#include "interfaces/nas/RilRequestSetLocationUpdateMessage.h"
#include "interfaces/nas/RilRequestQueryAvailBandModeMessage.h"
#include "interfaces/nas/RilRequestSetBandModeMessage.h"
#include "interfaces/nas/RilRequestSetUiccSubsMessage.h"
#include "interfaces/nas/RilRequestGetRadioCapMessage.h"
#include "interfaces/nas/RilRequestSetRadioCapMessage.h"
#include "interfaces/nas/RilRequestShutDownMessage.h"
#include "interfaces/nas/RilRequestEnableModemMessage.h"
#include "interfaces/nas/RilRequestGetModemStackStatusMessage.h"
#include "interfaces/nas/RilRequestSetSysSelChannelsMessage.h"
#include "interfaces/nas/RilRequestGetPhoneCapabilityMessage.h"
#include "interfaces/nas/NasEnablePhysChanConfigReporting.h"
#include "interfaces/nas/NasSetSignalStrengthCriteriaSupported.h"
#include "interfaces/nas/RilRequestSetUnsolCellInfoListRateMessage.h"
#include "interfaces/nas/RilRequestEnableUiccAppMessage.h"
#include "interfaces/nas/RilRequestGetUiccAppStatusMessage.h"
#include "interfaces/nas/RilRequestSetUnsolRespFilterMessage.h"
#include "modules/android/ClientConnectedMessage.h"
#include "interfaces/nas/RilRequestGetBarringInfoMessage.h"
#include "interfaces/nas/RilUnsolCellBarringMessage.h"

// 5G related
#include "interfaces/nas/RilRequestSet5GStatusMessage.h"
#include "interfaces/nas/RilRequestQuery5GStatusMessage.h"
#include "interfaces/nas/RilRequestQueryNrDcParamMessage.h"
#include "interfaces/nas/RilRequestQueryNrBearAllocationMessage.h"
#include "interfaces/nas/RilRequestQueryNrSignalStrengthMessage.h"
#include "interfaces/nas/RilRequestQueryUpperLayerIndInfoMessage.h"
#include "interfaces/nas/RilRequestQuery5gConfigInfoMessage.h"
#include "interfaces/nas/RilRequestEnableEndcMessage.h"
#include "interfaces/nas/RilRequestQueryEndcStatusMessage.h"
#include "interfaces/nas/RilRequestSetNrConfigMessage.h"
#include "interfaces/nas/RilRequestQueryNrConfigMessage.h"
#include "interfaces/nas/RilRequestGetEnhancedRadioCapabilityMessage.h"

//Data Unsol Messages
#include "modules/nas/NasDataCache.h"
#include "UnSolMessages/CallStatusMessage.h"
#include "UnSolMessages/DataSysStatusMessage.h"
#include "UnSolMessages/SetPrefDataTechMessage.h"
#include "UnSolMessages/DsdSysStatusMessage.h"
#ifndef QMI_RIL_UTF
#include "UnSolMessages/DataRegistrationStateMessage.h"
#endif
#include "UnSolMessages/NewDDSInfoMessage.h"
#include "UnSolMessages/DDSStatusFollowupMessage.h"

#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include <framework/AddPendingMessageList.h>

#include "request/NasGetRfBandInfoMessage.h"

int32_t toKhz(nas_bandwidth_enum_type_v01 bw);
int32_t toKhz(nas_lte_cphy_ca_bandwidth_enum_v01 bw);

class NasModule : public Module, public AddPendingMessageList {
  public:
    NasModule();
    ~NasModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalNasModuleCleanup();
#endif

  private:
    bool mReady = false;
    qtimutex::QtiRecursiveMutex mMutex;
    bool mSetSignalStrengthCriteriaSupported = false;
    PhysChanInfo chaninfo;

  public:
    bool getSetSignalStrengthCriteriaSupported() {
        return mSetSignalStrengthCriteriaSupported;
    }
    int32_t getLteBw();
    int32_t getNgranBw();
    int32_t getLteBand();
    int32_t getNgranBand();
    void resetPhysChanConfig();
    void reportCurrentPhysChanConfig();
    bool isDmsEndpointReady();
  private:
    void handleQcrilInit(std::shared_ptr<Message> msg);
    void handleNasQmiIndMessage(std::shared_ptr<Message> msg);
    void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
    void handleNasEndpointStatusIndMessage(std::shared_ptr<Message> msg);

    void handleGetPreferredNetworkType(std::shared_ptr<RilRequestGetPrefNetworkTypeMessage> msg);
    void handleSetPreferredNetworkType(std::shared_ptr<RilRequestSetPrefNetworkTypeMessage> msg);
    void handleQuery5gStatus(std::shared_ptr<RilRequestQuery5GStatusMessage> msg);
    void handleQueryEndcDcnr(std::shared_ptr<RilRequestQueryNrDcParamMessage> msg);
    void handleQueryNrBearerAllocation(std::shared_ptr<RilRequestQueryNrBearAllocationMessage> msg);
    void handleQueryNrSignalStrength(std::shared_ptr<RilRequestQueryNrSignalStrengthMessage> shared_msg);
    void handleSetCdmaSubscriptionSource(std::shared_ptr<RilRequestSetCdmaSubscriptionSourceMessage> msg);
    void handleExitEmergencyCallbackMode(std::shared_ptr<RilRequestExitEmergencyCallbackMessage> msg);
    void handleGetVoiceRadioTech(std::shared_ptr<RilRequestGetVoiceRadioTechMessage> msg);
    void handleSetUiccSubscription(std::shared_ptr<RilRequestSetUiccSubsMessage> msg);
    void handleQueryUpperLayerIndInfo(std::shared_ptr<RilRequestQueryUpperLayerIndInfoMessage> msg);
    void handleQuery5gConfigInfo(std::shared_ptr<RilRequestQuery5gConfigInfoMessage> msg);
    void handleEnableEndc(std::shared_ptr<RilRequestEnableEndcMessage> msg);
    void handleQueryEndcStatus(std::shared_ptr<RilRequestQueryEndcStatusMessage> msg);

    void handleShutdown(std::shared_ptr<RilRequestShutDownMessage> msg);
    void handleGetRadioCapability(std::shared_ptr<RilRequestGetRadioCapMessage> msg);
    void handleSetRadioCapability(std::shared_ptr<RilRequestSetRadioCapMessage> msg);
    void handleQueryNetworkSelectionMode(std::shared_ptr<RilRequestQueryNetworkSelectModeMessage> msg);
    void handleSetBandMode(std::shared_ptr<RilRequestSetBandModeMessage> msg);
    void handleGetNeighboringCellIds(std::shared_ptr<RilRequestGetNeighborCellIdsMessage> msg);
    void handleSetNetworkSelectionAutomatic(
        std::shared_ptr<RilRequestSetNetworkSelectionAutoMessage> msg);
    void handleSetNetworkSelectionManual(
        std::shared_ptr<RilRequestSetNetworkSelectionManualMessage> msg);
    void handleVoiceRegistrationState(std::shared_ptr<RilRequestGetVoiceRegistrationMessage> msg);
    void handleDataRegistrationState(std::shared_ptr<RilRequestGetDataRegistrationMessage> msg);
    void handleOperator(std::shared_ptr<RilRequestOperatorMessage> msg);
    void handleAllowData(std::shared_ptr<RilRequestAllowDataMessage> msg);
    void handleCdmaSubscription(std::shared_ptr<RilRequestCdmaSubscriptionMessage> msg);
    void handleCdmaSetRoamingPreference(std::shared_ptr<RilRequestSetCdmaRoamingPrefMessage> msg);
    void handleSetLocationUpdates(std::shared_ptr<RilRequestSetLocationUpdateMessage> msg);
    void handleQueryAvailableNetworks(std::shared_ptr<RilRequestQueryAvailNetworkMessage> msg);
    void handleGetBarringInfo(std::shared_ptr<RilRequestGetBarringInfoMessage> msg);
    void handleQueryAvailableBandMode(std::shared_ptr<RilRequestQueryAvailBandModeMessage> msg);
    void handleSignalStrength(std::shared_ptr<RilRequestGetSignalStrengthMessage> msg);
    void handleRilConnectedUnsolMessage(std::shared_ptr<ClientConnectedMessage> msg);
    void handleSetDeviceState(std::shared_ptr<RilRequestSendDeviceStateMessage> msg);
    void handleSetUnsolRespFilter(std::shared_ptr<RilRequestSetUnsolRespFilterMessage> msg);
    void handleSetUnsolBarringFilter(std::shared_ptr<RilRequestSetUnsolBarringFilterMessage> msg);
    void handleStartNetworkScan(std::shared_ptr<RilRequestStartNetworkScanMessage> msg);
    void handleStopNetworkScan(std::shared_ptr<RilRequestStopNetworkScanMessage> msg);
    void handleGetModemStackStatus(
            std::shared_ptr<RilRequestGetModemStackStatusMessage> msg);
    void handleEnableModem(
            std::shared_ptr<RilRequestEnableModemMessage> msg);
    void handleSetSystemSelectionChannels(
            std::shared_ptr<RilRequestSetSysSelChannelsMessage> msg);
    void handleGetPhoneCapability(
            std::shared_ptr<RilRequestGetPhoneCapabilityMessage> msg);
    void handleSetUnsolCellInfoListRateMessage(
            std::shared_ptr<RilRequestSetUnsolCellInfoListRateMessage> msg);
    void handleEnableUiccApplications(
            std::shared_ptr<RilRequestEnableUiccAppMessage> msg);
    void handleGetUiccApplicationStatus(
            std::shared_ptr<RilRequestGetUiccAppStatusMessage> msg);
    void handleCdmaQueryRoamingPreference(
            std::shared_ptr<RilRequestQueryCdmaRoamingPrefMessage> msg);
    void handleCdmaGetSubscriptionSource(
            std::shared_ptr<RilRequestGetCdmaSubscriptionSourceMessage> msg);
    void handleGetEnhancedRadioCapability(
             std::shared_ptr<RilRequestGetEnhancedRadioCapabilityMessage> msg);

    // 5G related:
    void handleSet5GStatus(std::shared_ptr<RilRequestSet5GStatusMessage> msg);
    void handleSetNrConfig(std::shared_ptr<RilRequestSetNrConfigMessage> msg);
    void handleQueryNrConfig(std::shared_ptr<RilRequestQueryNrConfigMessage> msg);

    // UIM handling
    void handleUimCardStatusIndMsg(std::shared_ptr<UimCardStatusIndMsg> msg);
    void handleUimCardStatusChangeIndMsg(std::shared_ptr<UimCardStatusChangeIndMsg> msg);
    void handleUimCardAppStatusIndMsg(std::shared_ptr<UimCardAppStatusIndMsg> msg);

    // IMS handling
    void handleImsServiceStatusInd(std::shared_ptr<ImsServiceStatusInd> msg);
    void handleImsWfcStatusInd(std::shared_ptr<ImsWfcSettingsStatusInd> msg);
    void handleImsPdpStatusInd(std::shared_ptr<ImsPdpStatusInd> msg);
    void handleSetVoiceDomainPreferenceRequest(std::shared_ptr<NasSetVoiceDomainPreferenceRequest> msg);

    //Handlers for Messages from Data
    void handleEmbmsEnableDataConMessage(std::shared_ptr<NasEmbmsEnableDataConMessage> msg);
    void handleEmbmsDisableDataMessage(std::shared_ptr<NasEmbmsDisableDataMessage> msg);
    void handleGetPhyChanConfigMessage(std::shared_ptr<NasGetPhyChanConfigMessage> msg);

    //Data Indication Handlers
    void handleDataCallStatusMessage(std::shared_ptr<rildata::CallStatusMessage> msg);
    void handleDataSetPrefDataTechMessage(std::shared_ptr<rildata::SetPrefDataTechMessage> msg);
    void handleDataSysStatusMessage(std::shared_ptr<rildata::DataSysStatusMessage> msg);
    void handleDataDsdSysStatusMessage(std::shared_ptr<rildata::DsdSysStatusMessage> msg);
#ifndef QMI_RIL_UTF
    void handleDataRegistrationStateMessage(std::shared_ptr<rildata::DataRegistrationStateMessage> msg);
#endif
    void handleDataNewDDSInfoMessage(std::shared_ptr<rildata::NewDDSInfoMessage> msg);
    void handleDataDDSStatusFollowupMessage(std::shared_ptr<rildata::DDSStatusFollowupMessage> msg);

    // Nas API
    void handlePsAttachDetachMessage(std::shared_ptr<NasSetPsAttachDetachMessage> msg);
    void handleNasGetSysInfoMessage(std::shared_ptr<NasGetSysInfoMessage> msg);

    // Handler for DMS Endpoint Status Indications
    void handleDmsEndpointStatusIndMsg(std::shared_ptr<Message> msg);

    // Settings
    void handleSetSignalStrengthCriteriaSupported(
            std::shared_ptr<NasSetSignalStrengthCriteriaSupported> msg);
    void handleEnablePhysChanConfigReporting(
            std::shared_ptr<NasEnablePhysChanConfigReporting> msg);
    std::shared_ptr<std::vector<NasPhysChanInfo>> getCurrentPhysChanConfig();

    // Physical channel configuration
    inline bool shouldReportScell(const nas_lte_cphy_scell_info_type_v01 &scell_info);
    inline int32_t getBandwidth(nas_radio_if_enum_v01 radio_if);
    inline int32_t getBandwidth();
    inline int32_t getBand(nas_radio_if_enum_v01 radio_if);
    inline int32_t getBand();
    void handleQmiNasRfBandInfoMessage(
            std::shared_ptr<QmiNasRfBandInfoMessage> bandInfoMsg);
    void handleQmiNasLteCphyCaIndMessage(
            std::shared_ptr<QmiNasLteCphyCaIndMessage> lteCphyMsg);
    bool getPhysChanConfigFromModem();
    void handleNasGetRfBandInfoMessage(std::shared_ptr<rildata::NasGetRfBandInfoMessage> msg);

};

NasModule &getNasModule();

