/*
 * SlimRemoteMsgsPbHandler.h
 * Common location data types definitions, enums, structs
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SLIM_REMOTE_MSGSPB_HANDLER_H
#define SLIM_REMOTE_MSGSPB_HANDLER_H

#include "SlimRemoteMsgs.h"
#include "SlimRemoteMsgs.pb.h"
#include <iostream>
#include <string>

using namespace std;

class SlimRemoteMsgsPbHandler {
    private:
    slimServiceEnumT         getSlimService(pbSlimServiceEnumT pbService);
    slimServiceStatusEnumT   getSlimServiceStatus(pbSlimServiceStatusEnumT pbServiceStatus);
    slimServiceProviderEnumT getSlimServiceProvider(pbSlimServiceProviderEnumT pbProvider);
    slimTimeSourceEnumT      getSlimTimeSource(pbSlimTimeSourceEnumT pbTimeSource);
    slimSensorTypeEnumT               getSlimSensorType(pbSlimSensorTypeEnumT pbSensorType);
    slimVehicleMotionSensorTypeEnumT  getSlimVnwSensorType(
                                      pbSlimVehicleMotionSensorTypeEnumT pbVnwType);
    pbSlimServiceStatusEnumT        getPBEnumForSlimServiceStatus(
                                        slimServiceStatusEnumT serviceStatus);
    pbSlimMessageTypeEnumT          getPBEnumForSlimMessageType(slimMessageTypeEnumT msgType);
    pbSlimErrorEnumT                getPBEnumForSlimError(slimErrorEnumT msgError);
    pbSlimMessageIdEnumT            getPBEnumForSlimMessageId(slimMessageIdEnumT msgId);
    pbSlimServiceProviderEnumT      getPBEnumSlimServiceProvider(
                                        slimServiceProviderEnumT provider);
    pbSlimTimeSourceEnumT            getPBEnumSlimTimeSource(slimTimeSourceEnumT timeSource);
    pbSlimSensorTypeEnumT              getPBEnumSlimSensorType(slimSensorTypeEnumT sensorType);
    pbSlimVehicleMotionSensorTypeEnumT getPBEnumSlimVehicleMotionSensorType(
                                           slimVehicleMotionSensorTypeEnumT sensorType);
    pbSlimServiceEnumT                getPBEnumForSlimService(slimServiceEnumT service);

    public:
    SlimRemoteMsgsPbHandler() {
        SLIM_LOGI("SlimRemoteMsgsPbHandler version  Major:%d, Minor:%d",
                PB_eSLIM_SERVICE_VER_MAJOR, PB_eSLIM_SERVICE_VER_MINOR);
    }
    virtual ~SlimRemoteMsgsPbHandler() = default;

    bool getPbIntClientUpMsg(string &os);
    bool getPbAvailableServiceResp(
        slimServiceStatusEventStructT *pServiceStatusData,
        uint32_t &mAvailableServicesMask, string &os
    );
    void setPBheader(
        const slimMessageHeaderStructT *pzHeader,
        pbSlimMessageHeaderStructT *pbHeader
    );
    bool getPbSensorData(
        const slimMessageHeaderStructT *pzHeader,
        slimSensorDataStructT *pRecvMsg,
        SlimRemotePTPtime *pPTP,
        string &os
    );
    bool getPbVehicleMotionData(
        const slimMessageHeaderStructT *pzHeader,
        slimVehicleMotionDataStructT *pRecvMsg,
        SlimRemotePTPtime *pPTP,
        string &os
    );
    bool getPbAvailableServicesReq(string &os);
    bool getPbSensorDataReq(
        bool         uEnable,
        uint16_t     wReportRateHz,
        uint16_t     wSampleCount,
        slimServiceEnumT eService,
        string             &os
    );
    bool getPbVnwDataReq(
        bool uEnable,
        slimServiceEnumT eService,
        string             &os
    );

    bool getSlimSensorDataReq(
        pbSlimRemoteSensorDataReqCtrlMsg   &sensorReqmsg,
        slimEnableSensorDataRequestStructT &mSensorDataRequest
    );

    bool getSlimVehicleMotionDataReq(
        pbSlimRemoteVehicleDataReqCtrlMsg       &vehicleReqmsg,
        slimEnableVehicleDataRequestStructT     &mVehicleataRequest
    );

    /* validate if service available is set or not */
    inline bool validateAvailableServiceInfoPB(
            pbSlimRemoteAvailSvcsRespCtrlMsg     &pbSvcsResp
    )
    {
        if (0 != pbSvcsResp.mavailableservicesmask())
            return true;
        else
            return false;
    }


    bool storeAvailableServiceInfoPB(
        pbSlimRemoteAvailSvcsRespCtrlMsg     &pbSvcsResp,
        slimServiceStatusEventStructT        *pServiceStatusData,
        uint32_t                             &mAvailableServicesMask,
        uint32_t                             &m_serviceStatusCount
    );

    bool getPbDataMsg(
        const slimMessageHeaderStructT *pzHeader,
        string &os
    );

    bool getSlimSensorData(
        pbSlimRemoteSensorDataMsg    &pbSesnorData,
        slimSensorDataStructT        &zSensorData,
        SlimRemotePTPtime            &gPTP
    );

    bool getSlimVehicleMotionData(
        pbSlimRemoteVehicleMotionDataMsg    &pbVecMotionDataMsg,
        slimVehicleMotionDataStructT        &zVnwData,
        SlimRemotePTPtime                   &gPTP
    );
};

#endif /*SLIM_REMOTE_MSGSPB_HANDLER_H*/
