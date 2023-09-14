/*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* @file
 @brief SLIM Remote provider messages file.

 This file defines SLIM Remote provider message interfaces.
 */

#ifndef SLIM_REMOTE_MSGS_H
#define SLIM_REMOTE_MSGS_H

#include <slim_client_types.h>

#define MAX_SLIM_AVAILABLE_SERVICES 32

typedef enum {
    eSLIM_MESSAGE_CLASS_CONTROL = 10,
    eSLIM_MESSAGE_CLASS_DATA,
    eSLIM_MESSAGE_CLASS_MAX = 2147483647 /* Force 32bit */
}slimRemoteMsgClsEnumT;

typedef enum {
    eSLIM_CONTROL_MESSAGE_ID_INVALID = 0,

    eSLIM_CONTROL_MESSAGE_ID_SLIM_INT_CLIENT_UP = 200,
    /* Slim_ClientControlMsg (no payload) */

    eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_REQ,
    /* Slim_ClientControlMsg (no payload) */

    eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_RESP,
    /*Slim_ClientAvailableServicesControlMsg uint32_t (available proviers mask) */

    eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ,
    /* Slim_ClientSensorDataReqControlMsg slimEnableSensorDataRequestStructT
    translated to slim_InternalEnableSensorData call*/

    eSLIM_CONTROL_MESSAGE_ID_MOTION_DATA_REQ,
    /*Slim_ClientMotionDataReqControlMsg (slimEnableMotionDataRequestStructT)
    translated to slim_InternalEnableMotionData call*/

    eSLIM_CONTROL_MESSAGE_ID_PEDOMETER_REQ,
    /*Slim_ClientPedometerDataReqControlMsg (slimEnablePedometerRequestStructT)
    translated to slim_InternalEnablePedometer call*/

    eSLIM_CONTROL_MESSAGE_ID_QMD_DATA_REQ,
    /*Slim_ClientQmdDataReqControlMsg (slimEnableQmdDataRequestStructT)
    translated to slim_InternalEnableQmdData call*/

    eSLIM_CONTROL_MESSAGE_ID_SMD_DATA_REQ,
    /*Slim_ClientSmdDataReqControlMsg (slimEnableSmdDataRequestStructT)
    translated to slim_InternalEnableSmdData call*/

    eSLIM_CONTROL_MESSAGE_ID_DISTANCE_BOUND_DATA_REQ,
    /*Slim_ClientDistanceBoundReqControlMsg (slimEnableDistanceBoundRequestStructT)
    translated to slim_InternalEnableDistanceBound call*/

    eSLIM_CONTROL_MESSAGE_ID_SET_DISTANCE_BOUND_DATA_REQ,
    /*Slim_ClientSetDistanceBounceReqControlMsg (slimSetDistanceBoundRequestStructT)
    translated to slim_InternalSetDistanceBound call*/

    eSLIM_CONTROL_MESSAGE_ID_GET_DISTANCE_BOUND_REPORT_REQ,
    /*Slim_ClientControlMsg (no request payload),
    translated to slim_InternalGetDistanceBoundReport call*/

    eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ,
    /*Slim_ClientVehicleDataReqControlMsg (slimEnableVehicleDataRequestStructT)
    translated to slim_InternalEnableVehicleData call (sensor, odometry, motion)*/

    eSLIM_CONTROL_MESSAGE_ID_PED_ALIGNMENT_REQ,
    /*Slim_ClientPedestrianAlignmentReqControlMsg (slimEnablePedestrianAlignmentRequestStructT)
    translated to slim_InternalEnablePedestrianAlignment call*/

    eSLIM_CONTROL_MESSAGE_ID_MAG_FIELD_DATA_REQ
    /*Slim_ClientMagneticFieldDataReqControlMsg (slimEnableMagneticFieldDataRequestStructT)
    translated to slim_InternalEnableMagneticFieldData call*/
}slimRemoteCtrlMsgEnumT;

/* Message Base */
struct SlimRemoteMsgBase
{
    slimRemoteMsgClsEnumT           mMsgClass;
    uint32_t                        mMsgId;

    inline SlimRemoteMsgBase(uint32_t inMsgId,
                slimRemoteMsgClsEnumT inMsgClass):
                mMsgId(inMsgId),
                mMsgClass(inMsgClass)
                {}
};

/*control msg with no specific payload */
struct SlimRemoteCtrlMsg : public SlimRemoteMsgBase
{
    inline SlimRemoteCtrlMsg(uint32_t inMsgId,
                slimRemoteMsgClsEnumT inMsgClass = eSLIM_MESSAGE_CLASS_CONTROL):
                SlimRemoteMsgBase(inMsgId, inMsgClass)
                {}
};

struct SlimRemoteAvailSvcsRespCtrlMsg : public SlimRemoteCtrlMsg
{
    uint32_t                         mAvailableServicesMask;
    slimServiceStatusEventStructT    mServiceStatus[MAX_SLIM_AVAILABLE_SERVICES];

    inline SlimRemoteAvailSvcsRespCtrlMsg(uint32_t inMsgId):
                SlimRemoteCtrlMsg(inMsgId)
                {}
}; /* eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_RESP */

struct SlimRemoteSensorDataReqCtrlMsg : public SlimRemoteCtrlMsg
{
    slimEnableSensorDataRequestStructT             mSensorDataRequest;

    inline SlimRemoteSensorDataReqCtrlMsg(uint32_t inMsgId):
                SlimRemoteCtrlMsg(inMsgId)
                {}
}; /* eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ */

struct SlimRemoteVehicleDataReqCtrlMsg : public SlimRemoteCtrlMsg
{
    slimEnableVehicleDataRequestStructT            mVehicleDataRequest;

    inline SlimRemoteVehicleDataReqCtrlMsg(uint32_t inMsgId):
                SlimRemoteCtrlMsg(inMsgId)
                {}
}; /* eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ */

/*Data Messages */
struct SlimRemotePTPtime
{
  /* if true then qtoffset cal is done */
  /* Sender device can choose to avoid offset calculation to save compute resource.*/
  /* Receiving end is responsible for check this field and perform offset operation */
    bool        qtOffsetCalculationDone;
    uint64_t    gPTPtime;
};

struct SlimRemoteDataMsg : public SlimRemoteMsgBase
{
    slimMessageHeaderStructT         mHeader;
    SlimRemotePTPtime                mGptp;

    inline SlimRemoteDataMsg(
        uint32_t                        inMsgId,
        const slimMessageHeaderStructT *inHeader,
        slimRemoteMsgClsEnumT           inMsgClass = eSLIM_MESSAGE_CLASS_DATA
        ) : SlimRemoteMsgBase(inMsgId, inMsgClass) {
        memscpy(&mHeader,  sizeof(slimMessageHeaderStructT),
          inHeader, sizeof(slimMessageHeaderStructT));
    }
};

/*Data messages of Indication type */
struct SlimRemoteSensorDataMsg : public SlimRemoteDataMsg
{
    slimSensorDataStructT               mSensorData;

    inline SlimRemoteSensorDataMsg(
        uint32_t                 inMsgId,
        const slimMessageHeaderStructT *inHeader
        ) : SlimRemoteDataMsg(inMsgId, inHeader) {}
}; /* eSLIM_MESSAGE_ID_SENSOR_DATA_IND */

struct SlimRemoteVehicleSensorDataMsg : public SlimRemoteDataMsg
{
    slimVehicleSensorDataStructT        mVehicleSensorData;

    inline SlimRemoteVehicleSensorDataMsg(
        uint32_t                 inMsgId,
        const slimMessageHeaderStructT *inHeader
        ) : SlimRemoteDataMsg(inMsgId, inHeader) {}
}; /* eSLIM_MESSAGE_ID_VEHICLE_SENSOR_IND */

struct SlimRemoteVehicleOdometryDataMsg : public SlimRemoteDataMsg
{
    slimVehicleOdometryDataStructT      mVehicleOdometryData;

    inline SlimRemoteVehicleOdometryDataMsg(
        uint32_t                 inMsgId,
        const slimMessageHeaderStructT *inHeader
        ) : SlimRemoteDataMsg(inMsgId, inHeader) {}
}; /* eSLIM_MESSAGE_ID_VEHICLE_ODOMETRY_IND */

struct SlimRemoteVehicleMotionDataMsg : public SlimRemoteDataMsg
{
    slimVehicleMotionDataStructT        mVehicleMotionData;

    inline SlimRemoteVehicleMotionDataMsg(
        uint32_t                 inMsgId,
        const slimMessageHeaderStructT *inHeader
        ) : SlimRemoteDataMsg(inMsgId, inHeader) {}
}; /* eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND */

#endif /* SLIM_REMOTE_MSGS_H */
