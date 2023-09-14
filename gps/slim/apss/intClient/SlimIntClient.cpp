/*===========================================================================

Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/* @file
 @brief SLIM Internal Client implementation file.

 This file defines SLIM Internal Client object implementation.
 */

#include <unistd.h>
#include <SlimIntClient.h>
#include <slim_internal_api.h>
#include <slim_os_api.h>
#include <slim_internal.h>
#include "slim_processor.h"
#include "slim_apss_diag.h"
#include "slim_apss_log.h"

#ifdef PTP_SUPPORTED
#include <gptp_helper.h>
#endif

#define IC_IP_ADDR_ANY "0.0.0.0"

namespace slim {

SlimIntClient *SlimIntClient::spInstance = nullptr;

static char remoteProviderIpAddr[LOC_MAX_PARAM_STRING];
static char remoteProviderPort[LOC_MAX_PARAM_STRING];
static char intClientIpAddr[LOC_MAX_PARAM_STRING];
static char intClientPort[LOC_MAX_PARAM_STRING];


class SlimIntClientIpcListener : public ILocIpcListener {
    SlimIntClient& mSlimClient;
public:
    inline SlimIntClientIpcListener(SlimIntClient& sc) : mSlimClient(sc) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data,
                    uint32_t length, const LocIpcRecver* recver) override;
};

uint32_t SlimIntClient::ticksToMsecs
(
    uint32_t t_Ticks
)
{
    /* Logic for the conversion (32768 Hz clock rate):
    msecs = ticks * 1000 / 32768
    = ticks * 125 / 4096

    nearest msec = (ticks * 125 + 4096/2) / 4096
    = (ticks * 125 + 2048) >> 12
    */
    return (uint32_t)(((uint64_t)t_Ticks * 125 + 2048) >> 12);
}

SlimIntClient* SlimIntClient::getInstance()
{
    int  enableInternalClient = 0;

    if (nullptr == spInstance) {
        loc_param_s_type slimParameterTable[] = {
            {"LOCTECH_SLIM_ENABLE_INT_CLIENT", &enableInternalClient, NULL, 'n'}
        };

        UTIL_READ_CONF(LOC_PATH_SLIM_CONF, slimParameterTable);

        if (enableInternalClient) {
            spInstance = new SlimIntClient();
            if (nullptr == spInstance)
                SLIM_LOGE("SlimIntClient() memory allocation failed \n");
        }
    }
    return spInstance;
}

SlimIntClient::SlimIntClient():
                        mContext(),
                        mMsgTask(new MsgTask("SlimIntClientMsgTask")),
                        mChannelState(CLOSED),
                        mCurrentSlimIntClientState(eSTATE_INVALID),
                        mThreadId(THREAD_ID_IC),
                        mAvailableServicesMask(0),
                        mIsGptpInitialized (false),
                        mQtimerOffsetFromBootTime(0),
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
                        mPbHandler(),
#endif
                        mStopDisableResponse(false),
                        mEnabledServices(0)
{
    int  enableIntClientGptpCal = 1; /*default enable */
    int  enablePbMsgFormat = 0;
    loc_param_s_type slimParameterTable[] = {
        {"REMOTE_PROVIDER_IP_ADDRESS", &remoteProviderIpAddr, nullptr, 's'},
        {"REMOTE_PROVIDER_PORT",       &remoteProviderPort, nullptr, 's'},
        {"SLIM_INTERNAL_CLIENT_IP_ADDRESS", &intClientIpAddr, nullptr, 's'},
        {"SLIM_INTERNAL_CLIENT_PORT",       &intClientPort, nullptr, 's'},
        {"ENABLE_INT_CLIENT_GPTP_CAL", &enableIntClientGptpCal, NULL, 'n'},
        {"ENABLE_SLIM_REMOTE_MSG_PROTO_FORMAT", &enablePbMsgFormat, NULL, 'n'}
    };

    UTIL_READ_CONF(LOC_PATH_SLIM_CONF, slimParameterTable);

    mIsGptpCalEnabled = enableIntClientGptpCal;

    if (enablePbMsgFormat) {
        mIsProtobufEnabled = true;
#ifndef SLIM_REMOTE_MSG_PROTO_ENABLED
        SLIM_LOGE("Protobuf code disabled and runtime flag enabled. \n");
        mIsProtobufEnabled = false;
#endif
    }

    if (!slim_ThreadCreate(&mThread, threadMain, this, "SlimIntClientThread")) {
        SLIM_LOGE("SlimIntClient:thread creation failed \n");
    } else {
        slim_TaskReadyWait(THREAD_ID_IC);
    }
}

SlimIntClient::~SlimIntClient()
{
    if (mpSlimIntClientState[eSTATE_INIT])
        delete mpSlimIntClientState[eSTATE_INIT];
    if (mpSlimIntClientState[eSTATE_WAIT_FOR_RP])
        delete mpSlimIntClientState[eSTATE_WAIT_FOR_RP];
    if (mpSlimIntClientState[eSTATE_READY])
        delete mpSlimIntClientState[eSTATE_READY];
    if (mpSlimIntClientState[eSTATE_ACTIVE])
        delete mpSlimIntClientState[eSTATE_ACTIVE];

    delete spInstance;
}

void SlimIntClient::threadMain
(
    void *pData
)
{
    SlimIntClient        *pzIntClient = reinterpret_cast<SlimIntClient*>(pData);

    pzIntClient->mpSlimIntClientState[eSTATE_INIT] = new SlimIntClientStateInit(pzIntClient);
    pzIntClient->mpSlimIntClientState[eSTATE_WAIT_FOR_RP] =
                                                new SlimIntClientStateWaitForRP(pzIntClient);
    pzIntClient->mpSlimIntClientState[eSTATE_READY] = new SlimIntClientStateReady(pzIntClient);
    pzIntClient->mpSlimIntClientState[eSTATE_ACTIVE] = new SlimIntClientStateActive(pzIntClient);

    if ( (nullptr == pzIntClient->mpSlimIntClientState[eSTATE_INIT]) ||
        (nullptr == pzIntClient->mpSlimIntClientState[eSTATE_WAIT_FOR_RP]) ||
        (nullptr == pzIntClient->mpSlimIntClientState[eSTATE_READY]) ||
        (nullptr == pzIntClient->mpSlimIntClientState[eSTATE_ACTIVE]) )
    {
        SLIM_LOGE("SlimIntClient: memory allocation of state-machine objects failed");
        return;
    }
    slim_IpcStart(pzIntClient->mThreadId);
    slim_TaskReadyAck();
    pzIntClient->slimIntClientSetState(eSTATE_INIT);
    pzIntClient->runEventLoop();
    return;
}

void SlimIntClient::runEventLoop()
{
    /* start qtimer offset calculation timer */
    slimIntClientStartQtimerOffsetCalTimer();

    /* change state */
    slimIntClientSetState(eSTATE_WAIT_FOR_RP);

    while (1)
    {
        slim_IpcMsgT* pz_Msg = NULL;
        while (NULL == (pz_Msg = slim_IpcReceive()))
            ;

        SLIM_LOGD("IntClient: IPC message received. q_MsgId:%" PRIu32 ", q_SrcThreadId:%" PRIu32,
            (uint32_t)pz_Msg->q_MsgId,
            (uint32_t)pz_Msg->q_SrcThreadId);

        switch (pz_Msg->q_MsgId)
        {
            case M_IPC_MSG_TIMER:
            {
                os_TimerExpiryType z_TimerMsg;
                slim_Memscpy(&z_TimerMsg, sizeof(z_TimerMsg), pz_Msg->p_Data,
                  pz_Msg->q_Size);
                  SLIM_LOGD("SlimIntClient: q_TimerId:%d\n", z_TimerMsg.q_TimerId);
                switch (z_TimerMsg.q_TimerId)
                {
                    case eTIMER_INT_CLIENT_UP_RETRY_TRIMER:
                        bool status;
                        slimIntClientSendUpMsg();
                        status = slimIntClientStartUpMsgRetryTimer(); /* restart timer */
                    break;
                    case eTIMER_QTIMER_OFFSET_CAL_TIMER:
                        slimIntClientCalQtimerOffset();
                        slimIntClientStartQtimerOffsetCalTimer();
                    break;
                    default:
                        SLIM_LOGW("Unknown timer event: timer=%" PRIu32 " param=%" PRIu32,
                            (uint32_t)z_TimerMsg.q_TimerId,
                            (uint32_t)z_TimerMsg.q_Param);
                    break;
                }
            }
            break;
        }
        slim_IpcDelete(pz_Msg);
    }
    return;
}

void SlimIntClient::slimIntClientInitialize()
{
    slimOpenFlagsT             qOpenFlags = 0;
    slim_ClientEnumType        qClientId;
    slimErrorEnumT             e_Status = eSLIM_ERROR_INTERNAL;
    SlimIntClient             *pzIntClient;

    qClientId = SLIM_CLIENT_INTERNAL;
    pzIntClient = this;

    /* Client Channel Open */
    e_Status = slim_InternalOpen(qClientId, SlimIntClient::slimIntClientCallback,
                    qOpenFlags, (uint64_t)reinterpret_cast<uintptr_t>(pzIntClient), &mContext);
    if (eSLIM_SUCCESS != e_Status)
    {
        SLIM_LOGE("slim_InternalOpen failed.\n");
    }

    mIpcrecver = getLocIpcInetUdpRecver(make_shared<SlimIntClientIpcListener>(*this),
           IC_IP_ADDR_ANY, atoi(intClientPort));

    mIpcSender = getLocIpcInetUdpSender(remoteProviderIpAddr, atoi(remoteProviderPort));
    /* gptp Initialization */
#ifdef PTP_SUPPORTED
    if (false == mIsGptpInitialized) {
        if (gptpInit()) {
            mIsGptpInitialized = true;
            SLIM_LOGV("GPTP Config mIsGptpInitialized:%s, mIsGptpCalEnabled:%s",
                mIsGptpInitialized?"TRUE":"FALSE", mIsGptpCalEnabled?"TRUE":"FALSE");
        } else {
            SLIM_LOGE("GPTP failed to initialize");
        }
    }
#endif
    /* Initialize timers */
    mRetryTimer = slim_TimerCreate(eTIMER_INT_CLIENT_UP_RETRY_TRIMER, mThreadId);
    mQtimerOffsetTimer = slim_TimerCreate(eTIMER_QTIMER_OFFSET_CAL_TIMER, mThreadId);

    /* Calculate Qtimer Offset */
    slimIntClientCalQtimerOffset();
    return;
}

void SlimIntClient::slimIntClientSetState(SlimIntClientState nextState)
{
    SlimIntClientState currentState;

    if (eSTATE_INVALID == nextState) {
        SLIM_LOGE("Invalid State being set");
        return;
    }
    currentState = mCurrentSlimIntClientState;

    SLIM_LOGV("SlimIntClient currentState:%d, nextState:%d.\n", currentState, nextState);

    if (eSTATE_INVALID != currentState)
        mpSlimIntClientState[currentState]->stateExitAction();
    mCurrentSlimIntClientState = nextState;
    mpSlimIntClientState[nextState]->stateAction();
    return;
}

bool SlimIntClient::slimIntClientStartUpMsgRetryTimer()
{
    SLIM_LOGV("SlimIntClient:     slimIntClientStartUpMsgRetryTimer \n");
    return slim_TimerStart(mRetryTimer,
        SLIM_INT_CLIENT_UP_RETRY_TIMER_TIMEOUT_MSEC,
        eTIMER_INT_CLIENT_UP_RETRY_TRIMER
        );
}

bool SlimIntClient::slimIntClientStopUpMsgRetryTimer()
{
    SLIM_LOGV("SlimIntClient:     slimIntClientStopUpMsgRetryTimer \n");
    return slim_TimerStop(mRetryTimer);
}

bool SlimIntClient::slimIntClientStartQtimerOffsetCalTimer()
{
    return slim_TimerStart(mQtimerOffsetTimer,
        SLIM_QTIMER_BOOTTIME_OFFSET_CAL_TIMER_MSEC,
        eTIMER_QTIMER_OFFSET_CAL_TIMER
        );
}

bool SlimIntClient::slimIntClientStopQtimerOffsetCalTimer()
{
    return slim_TimerStop(mQtimerOffsetTimer);
}

void SlimIntClient::slimIntClientSendUpMsg()
{
    if (!mIsProtobufEnabled) {
        SlimRemoteCtrlMsg *msg;

        msg = new SlimRemoteCtrlMsg(eSLIM_CONTROL_MESSAGE_ID_SLIM_INT_CLIENT_UP);
        if (nullptr == msg)
            return;

        slimIntClientSendCtrlMsg(msg, sizeof(SlimRemoteCtrlMsg));
    } else {

#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
        slimIntClientSendUpMsgPb();
#endif
    }
    return;
}

void SlimIntClient::slimIntClientSendAvailableServices()
{
    if (!mIsProtobufEnabled) {
        SlimRemoteAvailSvcsRespCtrlMsg *msg;

        msg = new SlimRemoteAvailSvcsRespCtrlMsg(
                eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_RESP);
        if (nullptr == msg)
            return;

        msg->mAvailableServicesMask = mAvailableServicesMask;
        slim_Memscpy(&msg->mServiceStatus, sizeof(msg->mServiceStatus),
                &mServiceStatusData, sizeof(mServiceStatusData));

        SLIM_LOGV("mAvailableServicesMask:%d\n", mAvailableServicesMask);

        slimIntClientAvailSvcsRespSendMsg(msg, sizeof(SlimRemoteAvailSvcsRespCtrlMsg));
    } else {

#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
        slimIntClientSendAvailableServicesPb();
#endif
    }
    return;
}

/* Send functions */
void SlimIntClient::slimIntClientSendCtrlMsg(SlimRemoteCtrlMsg *inMsg, uint32_t msgSize)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct SendCtrlMsg: public LocMsg
    {
        const SlimRemoteCtrlMsg *pMsg;
        SlimIntClient                *pObj;
        uint32_t                     size;
        SendCtrlMsg
        (
            const SlimRemoteCtrlMsg *msg,
            uint32_t                    inSize,
            SlimIntClient              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new SendCtrlMsg(inMsg, msgSize, this));
}

void SlimIntClient::slimIntClientAvailSvcsRespSendMsg(
                    SlimRemoteAvailSvcsRespCtrlMsg *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct SendAvailSvcsResp: public LocMsg
    {
        const SlimRemoteAvailSvcsRespCtrlMsg *pMsg;
        SlimIntClient                *pObj;
        uint32_t                     size;
        SendAvailSvcsResp
        (
            const SlimRemoteAvailSvcsRespCtrlMsg *msg,
            uint32_t                    inSize,
            SlimIntClient              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new SendAvailSvcsResp(inMsg, msgSize, this));
}

void SlimIntClient::slimIntClientDataSendMsg(SlimRemoteDataMsg *inMsg, uint32_t msgSize)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct SendData: public LocMsg
    {
        const SlimRemoteDataMsg *pMsg;
        SlimIntClient                *pObj;
        uint32_t                     size;
        SendData
        (
            const SlimRemoteDataMsg *msg,
            uint32_t                    inSize,
            SlimIntClient              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new SendData(inMsg, msgSize, this));
}

void SlimIntClient::slimIntClientSensorDataSendMsg(
                            SlimRemoteSensorDataMsg *inMsg,
                            uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct SendSensorData: public LocMsg
    {
        const SlimRemoteSensorDataMsg *pMsg;
        SlimIntClient                *pObj;
        uint32_t                     size;
        SendSensorData
        (
            const SlimRemoteSensorDataMsg *msg,
            uint32_t                    inSize,
            SlimIntClient              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new SendSensorData(inMsg, msgSize, this));
}

void SlimIntClient::slimIntClientVehicleSensorDataSendMsg
(
    SlimRemoteVehicleSensorDataMsg *inMsg,
    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct SendVehicleSensorData: public LocMsg
    {
        const SlimRemoteVehicleSensorDataMsg *pMsg;
        SlimIntClient                *pObj;
        uint32_t                     size;
        SendVehicleSensorData
        (
            const SlimRemoteVehicleSensorDataMsg *msg,
            uint32_t                    inSize,
            SlimIntClient              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new SendVehicleSensorData(inMsg, msgSize, this));
}

void SlimIntClient::slimIntClientVehicleOdometryDataSendMsg
(
    SlimRemoteVehicleOdometryDataMsg *inMsg,
    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct SendVehicleOdometryData: public LocMsg
    {
        const SlimRemoteVehicleOdometryDataMsg *pMsg;
        SlimIntClient                *pObj;
        uint32_t                     size;
        SendVehicleOdometryData
        (
            const SlimRemoteVehicleOdometryDataMsg *msg,
            uint32_t                    inSize,
            SlimIntClient              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new SendVehicleOdometryData(inMsg, msgSize, this));
}

void SlimIntClient::slimIntClientVehicleMotionDataSendMsg
(
    SlimRemoteVehicleMotionDataMsg *inMsg,
    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct SendVehicleMotionData: public LocMsg
    {
        const SlimRemoteVehicleMotionDataMsg *pMsg;
        SlimIntClient                *pObj;
        uint32_t                     size;
        SendVehicleMotionData
        (
            const SlimRemoteVehicleMotionDataMsg *msg,
            uint32_t                    inSize,
            SlimIntClient              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new SendVehicleMotionData(inMsg, msgSize, this));
}

void SlimIntClient::slimIntClientCallback
(
    uint64 tCbData,
    const slimMessageHeaderStructT *pzHeader,
    void *pMessage
)
{
    SlimIntClient *pObject = reinterpret_cast<SlimIntClient*>((uintptr_t)tCbData);

    if (NULL == pzHeader || NULL == pObject)
        return;

    SLIM_LOGV("SlimIntClient:slimIntClientCallback msgId:%d\n", pzHeader->msgId);

    switch (pzHeader->msgType)
    {
    case eSLIM_MESSAGE_TYPE_RESPONSE:
        switch (pzHeader->msgId)
        {
            case eSLIM_MESSAGE_ID_OPEN_RESP:
                pObject->slimIntClientProcessOpenResponse(pzHeader);
            break;
            case eSLIM_MESSAGE_ID_SENSOR_DATA_ENABLE_RESP:
            case eSLIM_MESSAGE_ID_MOTION_DATA_ENABLE_RESP:
            case eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP:
            case eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP:
            case eSLIM_MESSAGE_ID_VEHICLE_ENABLE_RESP:
            case eSLIM_MESSAGE_ID_SENSOR_DATA_DISABLE_RESP:
            case eSLIM_MESSAGE_ID_VEHICLE_DISABLE_RESP:
                pObject->slimIntClientHandleCb(pzHeader, nullptr);
            break;
            default:
            break;
        }
    break;
    case eSLIM_MESSAGE_TYPE_INDICATION:
        if (eSLIM_SUCCESS == pzHeader->msgError && nullptr != pMessage)
        {
            pObject->slimIntClientHandleCb(pzHeader, pMessage);
        }
        else
        {
            pObject->slimIntClientHandleCb(pzHeader, nullptr);
        }
    break;
    default:
    break;
    }
    return;
}

/* Convert slim sensor type to slim service notation */
slimServiceEnumT SlimIntClient::SlimSensorTypeToService
(
  slimSensorTypeEnumT slimSensorType
)
{
    switch (slimSensorType)
    {
        case eSLIM_SENSOR_TYPE_ACCEL:
            return eSLIM_SERVICE_SENSOR_ACCEL;
        case eSLIM_SENSOR_TYPE_GYRO:
            return eSLIM_SERVICE_SENSOR_GYRO;
        default:
            return eSLIM_SERVICE_MAX;
    }
}


/* Convert slim motion sensor type to slim service notation */
slimServiceEnumT SlimIntClient::SlimMotionSensorTypeToService
(
  slimVehicleMotionSensorTypeEnumT slimMotionSensorType
)
{
    switch (slimMotionSensorType)
    {
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_SPEED:
            return eSLIM_SERVICE_VEHICLE_SPEED;
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS:
            return eSLIM_SERVICE_VEHICLE_DWS;
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_GEAR:
            return eSLIM_SERVICE_VEHICLE_GEAR;
        case eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL:
            return eSLIM_SERVICE_VEHICLE_STEERING_WHEEL;
        default:
            return eSLIM_SERVICE_MAX;
    }
}

void SlimIntClient::slimIntClientHandleCb
(
    const slimMessageHeaderStructT *pzHeader,
    void *pMessage
)
{
    if (nullptr == pzHeader) {
        SLIM_LOGE("slimIntClientHandleCb: pzHeader NULL");
        return;
    }

    if (nullptr == pMessage && !mStopDisableResponse)
    {
        if (!mIsProtobufEnabled) {
            SlimRemoteDataMsg    *msg;
            msg = new SlimRemoteDataMsg(pzHeader->msgId,
                                        pzHeader);
            if (nullptr == msg)
                return;

            slimIntClientDataSendMsg(msg, sizeof(SlimRemoteDataMsg));
        } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
            string os;
            bool ret;
            ret = mPbHandler.getPbDataMsg(pzHeader, os);

            if (ret)
                sendPbMsg(os);
            else
                SLIM_LOGE("getPbDataMsg return FALSE!");
#endif
        }
    }
    else if (nullptr != pMessage)
    {
        switch (pzHeader->msgId)
        {
            case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
            {
                slimServiceStatusEventStructT *pRecvMsg =
                    (slimServiceStatusEventStructT *)pMessage;
                uint32_t temp = mAvailableServicesMask;
                mAvailableServicesMask = pRecvMsg->availableServicesMask;
                slim_Memscpy(&mServiceStatusData[pRecvMsg->service],
                                sizeof(slimServiceStatusEventStructT),
                                pRecvMsg,
                                sizeof(slimServiceStatusEventStructT)
                            );
                if ((mCurrentSlimIntClientState == eSTATE_ACTIVE) &&
                    (temp != mAvailableServicesMask)) { /*send only if any change in mask*/
                    slimIntClientSendAvailableServices();
                }
            }
            break;
            case eSLIM_MESSAGE_ID_SENSOR_DATA_IND:
            {
                SlimRemotePTPtime ptp;
                if (mIsGptpInitialized && mIsGptpCalEnabled) {
                    slimIntClientsetSensorDataGptpTime((slimSensorDataStructT*)pMessage);
                    ptp.qtOffsetCalculationDone = true;
                    ptp.gPTPtime = 0;
                } else {
                    ptp.qtOffsetCalculationDone = false;
                    ptp.gPTPtime = mQtimerOffsetFromBootTime;
                }
#ifdef PTP_SUPPORTED
                uint64_t gptpCurrentns = 0;
                slimSensorDataStructT zData;
                zData = *((slimSensorDataStructT*)pMessage);
                slimServiceEnumT eService = SlimSensorTypeToService(zData.sensorType);
                gptpGetCurPtpTime(&gptpCurrentns);
                gptpCurrentns = gptpCurrentns/1000;
                slimLogDiagPkt(
                    eSLIM_LOG_SOURCE_INTERNAL_CLIENT,
                    eSLIM_TIME_SOURCE_GPTP_TIME,
                    eSLIM_MESSAGE_ID_SENSOR_DATA_IND,
                    gptpCurrentns,
                    pMessage,
                    sizeof(slimSensorDataStructT),
                    eService);
#endif
                if (!mIsProtobufEnabled) {
                    SlimRemoteSensorDataMsg *msg;
                    msg = new SlimRemoteSensorDataMsg(eSLIM_MESSAGE_ID_SENSOR_DATA_IND,
                                            pzHeader);
                    if (nullptr == msg)
                        return;
                    if (sizeof(slimSensorDataStructT) == pzHeader->size)
                        slim_Memscpy(&((SlimRemoteSensorDataMsg*)msg)->mSensorData,
                                sizeof(slimSensorDataStructT),
                                (slimSensorDataStructT*)pMessage,
                                sizeof(slimSensorDataStructT));
                        msg->mGptp.qtOffsetCalculationDone = ptp.qtOffsetCalculationDone;
                        msg->mGptp.gPTPtime = ptp.gPTPtime;
                        slimIntClientSensorDataSendMsg(msg, sizeof(SlimRemoteSensorDataMsg));
                } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
                    string os;
                    bool ret;
                    ret = mPbHandler.getPbSensorData(pzHeader,
                                (slimSensorDataStructT*)pMessage, &ptp, os);
                    if (ret)
                        sendPbMsg(os);
                    else
                        SLIM_LOGE("getPbSensorData return FALSE!");
#endif
                }
            }
            break;
            case eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND:
            {
                SlimRemotePTPtime ptp;
                if (mIsGptpInitialized && mIsGptpCalEnabled) {
                    slimIntClientsetVnwDataGptpTime((slimVehicleMotionDataStructT*)pMessage);
                    ptp.qtOffsetCalculationDone = true;
                    ptp.gPTPtime = 0;
                } else {
                    ptp.qtOffsetCalculationDone = false;
                    ptp.gPTPtime = mQtimerOffsetFromBootTime;
                }
#ifdef PTP_SUPPORTED
                uint64_t gptpCurrentns = 0;
                slimVehicleMotionDataStructT zData;
                zData = *((slimVehicleMotionDataStructT*)pMessage);
                slimServiceEnumT eService = SlimMotionSensorTypeToService(zData.sensorType);
                gptpGetCurPtpTime(&gptpCurrentns);
                gptpCurrentns = gptpCurrentns/1000;
                slimLogDiagPkt(
                    eSLIM_LOG_SOURCE_INTERNAL_CLIENT,
                    eSLIM_TIME_SOURCE_GPTP_TIME,
                    eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND,
                    gptpCurrentns,
                    pMessage,
                    sizeof(slimVehicleMotionDataStructT),
                    eService);
#endif
                if (!mIsProtobufEnabled) {
                    SlimRemoteVehicleMotionDataMsg *msg;
                    msg = new SlimRemoteVehicleMotionDataMsg(
                                    eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND, pzHeader);
                    if (nullptr == msg)
                        return;

                    if (sizeof(slimVehicleMotionDataStructT) == pzHeader->size)
                        slim_Memscpy(&((SlimRemoteVehicleMotionDataMsg*)msg)->mVehicleMotionData,
                                sizeof(slimVehicleMotionDataStructT),
                                (slimVehicleMotionDataStructT*)pMessage,
                                sizeof(slimVehicleMotionDataStructT));
                        msg->mGptp.qtOffsetCalculationDone = ptp.qtOffsetCalculationDone;
                        msg->mGptp.gPTPtime = ptp.gPTPtime;
                        slimIntClientVehicleMotionDataSendMsg(msg,
                                        sizeof(SlimRemoteVehicleMotionDataMsg));
                } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
                    string os;
                    bool ret;
                    ret = mPbHandler.getPbVehicleMotionData(pzHeader,
                                    (slimVehicleMotionDataStructT*)pMessage, &ptp, os);
                    if (ret)
                        sendPbMsg(os);
                    else
                        SLIM_LOGE("getPbVehicleMotionData return FALSE!");
#endif
                }
            }
            break;
            default:
            break;
        }
    }
    return;
}

void SlimIntClient::slimIntClientProcessOpenResponse
(
    const slimMessageHeaderStructT *pzHeader
)
{
    if ((nullptr != pzHeader) && (eSLIM_SUCCESS == pzHeader->msgError))
        mChannelState = OPENED;
    else
        mChannelState = CLOSED;
    return;
}

uint64_t SlimIntClient::slimIntGetQtimerTimeNs()
{
    uint64_t qTimerCnt = 0, qTimerFreq = 0, qTimerSec = 0, qTimerNanoSec = 0;
    uint64_t outTime = 0;
    qTimerCnt = slim_GetQTimerTickCount();
    qTimerFreq = slim_GetQTimerFrequency();
    qTimerSec = (qTimerCnt / qTimerFreq);
    qTimerNanoSec = (qTimerCnt % qTimerFreq);
    qTimerNanoSec *= 1000000000LL;
    qTimerNanoSec /= qTimerFreq;
    outTime = (qTimerSec * 1000000000LL) + qTimerNanoSec;
    return outTime;
}

uint64_t SlimIntClient::slimIntGetClockBootTimeNs()
{
    uint64_t qTimerCnt = 0, qTimerFreq = 0, qTimerSec = 0, qTimerNanoSec = 0;
    uint64_t outTime = 0;
    struct timespec  bootTime;
    clock_gettime(CLOCK_BOOTTIME, &bootTime);
    outTime = (bootTime.tv_sec * 1000000000LL) + bootTime.tv_nsec;
    return outTime;
}

void SlimIntClient::slimIntClientCalQtimerOffset()
{
    uint64_t  bootTime, bootTime_new, qtimerTime;
    int64_t offset;
    int ctr = 0;

    // Find delta between qtimer and system time
    do {
        bootTime = slimIntGetClockBootTimeNs();
        qtimerTime = slimIntGetQtimerTimeNs();
        bootTime_new = slimIntGetClockBootTimeNs();

        offset = (int64_t) (qtimerTime - bootTime);
        ctr++;
    } while (((bootTime_new - bootTime) > APSS_QTIMER_GAP_THRESHOLD_NS) &&
                (ctr < APSS_QTIMER_GAP_MAX_ITERATION));

    if (ctr < APSS_QTIMER_GAP_MAX_ITERATION) {
        mQtimerOffsetFromBootTime = offset;
        SLIM_LOGV("Updated mQtimerOffsetFromBootTime = % " PRId64 "", mQtimerOffsetFromBootTime);
    }
    return;
}

void SlimIntClient::slimIntClientsetSensorDataGptpTime(slimSensorDataStructT *pSensor)
{
    uint64_t qtimerNs;
    uint64_t gPtpNs;
    uint64_t offset = mQtimerOffsetFromBootTime;
    uint64_t timeBase = pSensor->timeBase;
    uint64_t gPTPtimeBase;

    pSensor->timeBase = 0;

#ifdef PTP_SUPPORTED
    if (mIsGptpInitialized) {

        for (int i = 0; i < pSensor->samples_len; ++i)
        {
            qtimerNs = ((((timeBase + pSensor->samples[i].sampleTimeOffsetMs) * 1000000LL) +
                            pSensor->samples[i].sampleTimeOffsetSubMilliNs)) + offset;
            if (gptpGetPtpTimeFromQTimeNs(&gPtpNs, qtimerNs)) {
                gPTPtimeBase = gPtpNs / 1000000LL; /*in ms*/
                if (pSensor->timeBase == 0) {
                    pSensor->timeBase = gPTPtimeBase;
                    pSensor->samples[i].sampleTimeOffsetMs = 0;
                    pSensor->samples[i].sampleTimeOffsetSubMilliNs = 0;
                }
                else {
                    pSensor->samples[i].sampleTimeOffsetMs = gPTPtimeBase - pSensor->timeBase;
                }
                pSensor->samples[i].sampleTimeOffsetSubMilliNs =
                 (gPtpNs -
                    ((pSensor->timeBase + pSensor->samples[i].sampleTimeOffsetMs) * 1000000LL));
            } else {
                SLIM_LOGE("gptpGetPtpTimeFromQTimeNs Failed!");
                pSensor->timeBase = 0;
                break;
            }
        }
    }
#endif
    return;
}

void
SlimIntClient::slimIntClientsetVnwDataGptpTime(slimVehicleMotionDataStructT *pVnw)
{
    uint64_t qtimerNs;
    uint64_t gPtpNs;
    uint64_t offset = mQtimerOffsetFromBootTime;
    uint64_t timeBase = pVnw->timeBase;
    uint64_t gPTPtimeBase, gPTPtimeus;

    pVnw->timeBase = 0;

#ifdef PTP_SUPPORTED
    if (mIsGptpInitialized) {
        for (int i = 0; i < pVnw->samples_len; ++i)
        {
            if ((eSLIM_VEHICLE_MOTION_SENSOR_TYPE_DWS == pVnw->sensorType)) {
                qtimerNs =
                (((timeBase * 1000LL) + pVnw->samples[i].dws.sampleTimeOffset) * 1000LL) + offset;

                if (gptpGetPtpTimeFromQTimeNs(&gPtpNs, qtimerNs)) {
                    gPTPtimeus = gPtpNs / 1000LL;
                    gPTPtimeBase = gPTPtimeus / 1000LL;
                    if (pVnw->timeBase == 0) {
                        pVnw->timeBase = gPTPtimeBase;
                        pVnw->samples[i].dws.sampleTimeOffset = (gPTPtimeus % 1000LL);
                    }
                    else {
                        pVnw->samples[i].dws.sampleTimeOffset =
                            gPTPtimeus - (pVnw->timeBase * 1000LL);
                    }
                } else {
                    pVnw->timeBase = 0;
                    break;
                }
            }
            if ((eSLIM_VEHICLE_MOTION_SENSOR_TYPE_STEERING_WHEEL == pVnw->sensorType)) {
                qtimerNs =
                (((timeBase * 1000LL) + pVnw->samples[i].steering_wheel.sampleTimeOffset) * 1000LL)
                     + offset;

                if (gptpGetPtpTimeFromQTimeNs(&gPtpNs, qtimerNs)) {
                    gPTPtimeus = gPtpNs / 1000LL;
                    gPTPtimeBase = gPTPtimeus / 1000LL;
                    if (pVnw->timeBase == 0) {
                        pVnw->timeBase = gPTPtimeBase;
                        pVnw->samples[i].steering_wheel.sampleTimeOffset = (gPTPtimeus % 1000LL);
                    }
                    else {
                        pVnw->samples[i].steering_wheel.sampleTimeOffset =
                                     gPTPtimeus - (pVnw->timeBase * 1000LL);
                    }
                } else {
                    pVnw->timeBase = 0;
                    break;
                }
            }
        }
    }
#endif
    return;
}

void SlimIntClient::slimIntClientDisableAllServices()
{
    uint8_t u_TxnId;
    slimErrorEnumT              e_Status = eSLIM_ERROR_INTERNAL;
    SLIM_LOGV("slimIntClientDisableAllServices: mEnabledServices:%d -->", mEnabledServices);
    for (int i = 0; i <= eSLIM_SERVICE_LAST  && mEnabledServices; ++i)
    {
        switch (i)
        {
            case eSLIM_SERVICE_SENSOR_ACCEL:
            case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
            case eSLIM_SERVICE_SENSOR_GYRO:
            case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
            {
                if (SLIM_MASK_IS_SET(mEnabledServices, i))
                {
                    slimEnableSensorDataRequestStructT zRequest;
                    zRequest.enableConf.enable = false;
                    zRequest.sensor = (slimServiceEnumT)i;
                    u_TxnId = SLIM_INT_CLIENT_SERVICE_TXN_ID;
                    e_Status = slim_InternalEnableSensorData(&mContext, &zRequest, u_TxnId);
                    if (eSLIM_SUCCESS != e_Status)
                    {
                        SLIM_LOGE("Failed sending sensor request: error=%d", e_Status);
                    }

                SLIM_LOGV("slimIntClientDisableAllServices: stop mEnabledServices:%d -->", i);
                }
            }
            break;
            case eSLIM_SERVICE_VEHICLE_ACCEL:
            case eSLIM_SERVICE_VEHICLE_GYRO:
            case eSLIM_SERVICE_VEHICLE_ODOMETRY:
            case eSLIM_SERVICE_VEHICLE_SPEED:
            case eSLIM_SERVICE_VEHICLE_DWS:
            case eSLIM_SERVICE_VEHICLE_GEAR:
            case eSLIM_SERVICE_VEHICLE_STEERING_WHEEL:
            {
                if (SLIM_MASK_IS_SET(mEnabledServices, i))
                {
                    slimEnableVehicleDataRequestStructT zRequest;
                    zRequest.enableConf.enable = false;
                    zRequest.service = (slimServiceEnumT)i;
                    u_TxnId = SLIM_INT_CLIENT_SERVICE_TXN_ID;
                    e_Status = slim_InternalEnableVehicleData(&mContext, &zRequest, u_TxnId);
                    if (eSLIM_SUCCESS != e_Status)
                    {
                        SLIM_LOGE("Failed sending sensor request: error=%d", e_Status);
                    }
                }
            }
            break;
            default:
                SLIM_LOGE("Not supported Service=%d", i);
            break;
        }
        SLIM_MASK_CLEAR(mEnabledServices, i);
    }
    SLIM_LOGV("slimIntClientDisableAllServices: mEnabledServices:%d -->", mEnabledServices);
    return;
}

void SlimIntClient::handleSensorRequest(slimEnableSensorDataRequestStructT &zRequest)
{
    uint8_t                     u_TxnId;
    slimErrorEnumT              e_Status = eSLIM_ERROR_INTERNAL;
    if (zRequest.enableConf.enable) {
        slimIntClientCalQtimerOffset();
        SLIM_MASK_SET(mEnabledServices, zRequest.sensor);
    }
    else {
        SLIM_MASK_CLEAR(mEnabledServices, zRequest.sensor);
    }

    u_TxnId = SLIM_INT_CLIENT_SERVICE_TXN_ID;
    e_Status = slim_InternalEnableSensorData(&mContext, &zRequest, u_TxnId);
    if (eSLIM_SUCCESS != e_Status)
    {
        SLIM_LOGE("Failed sending sensor request: error=%d", e_Status);
    }

    /* This is not an error message, to check the communication status between
       Internal client and Remote provider few messages are printed using SLIM_LOGE.
    */
    SLIM_LOGE("eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ -->"
                    "sensor-type:%d enableConf:%d\n", zRequest.sensor, zRequest.enableConf.enable);
}

void SlimIntClient::handleVehicleMotionRequest(slimEnableVehicleDataRequestStructT &zRequest)
{
    uint8_t                     u_TxnId;
    slimErrorEnumT              e_Status = eSLIM_ERROR_INTERNAL;
    if (zRequest.enableConf.enable) {
        SLIM_MASK_SET(mEnabledServices, zRequest.service);
    }
    else {
        SLIM_MASK_CLEAR(mEnabledServices, zRequest.service);
    }

    u_TxnId = SLIM_INT_CLIENT_SERVICE_TXN_ID;
    e_Status = slim_InternalEnableVehicleData(&mContext, &zRequest, u_TxnId);
    if (eSLIM_SUCCESS != e_Status)
    {
        SLIM_LOGE("Failed sending sensor request: error=%d", e_Status);
    }
    SLIM_LOGE("eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ -->"
                    "vnw-type:%d enableConf:%d\n", zRequest.service, zRequest.enableConf.enable);
}

void SlimIntClientIpcListener::onListenerReady()
{
    SLIM_LOGV("SlimIntClientIpcListener: onListenerReady -->");
    mSlimClient.mIsListenerReady = true;
    return;
}

void SlimIntClientIpcListener::onReceive(const char* data, uint32_t length,
                                const LocIpcRecver* recver)
{
    if (nullptr == data || 0 == length)
        return;

    if (!mSlimClient.mIsProtobufEnabled) {
        SlimRemoteMsgBase           *m_msg = (SlimRemoteMsgBase *)data;

        SLIM_LOGV("SlimIntClientIpcListener:onReceive m_msg.m_msgId:%d,   length:%d --> \n",
                    m_msg->mMsgId, length);
        switch (m_msg->mMsgId)
        {
            case eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_REQ:
            {
                SLIM_LOGE("eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_REQ --> \n");

                mSlimClient.slimIntClientSetState(
                        SlimIntClient::SlimIntClientState::eSTATE_READY
                        );
                mSlimClient.slimIntClientSetState(
                        SlimIntClient::SlimIntClientState::eSTATE_ACTIVE
                        );
            }
            break;
            case eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ:
            {
                SLIM_LOGV("eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ --> \n");
                slimEnableSensorDataRequestStructT zRequest =
                              (*(SlimRemoteSensorDataReqCtrlMsg*)data).mSensorDataRequest;
                mSlimClient.handleSensorRequest(zRequest);

            }
            break;
            case eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ:
            {
                SLIM_LOGV("eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ --> \n");
                slimEnableVehicleDataRequestStructT zRequest =
                              (*(SlimRemoteVehicleDataReqCtrlMsg*)data).mVehicleDataRequest;
                mSlimClient.handleVehicleMotionRequest(zRequest);

            }
            break;
            default:
            break;
        }
    } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
        mSlimClient.decodeFromPbData(data, length);
#endif
    }
    return;
}

void SlimIntClientStateInit::stateAction()
{
    mSlimIntClient->slimIntClientInitialize();
    return;
}

void SlimIntClientStateInit::stateExitAction()
{
    return;
}

void SlimIntClientStateWaitForRP::stateAction()
{
    mSlimIntClient->slimIntClientSendUpMsg();
    mSlimIntClient->startNonBlockingListening(mSlimIntClient->mIpcrecver);
    mSlimIntClient->slimIntClientStartUpMsgRetryTimer();
    return;
}

void SlimIntClientStateWaitForRP::stateExitAction()
{
    mSlimIntClient->slimIntClientStopUpMsgRetryTimer();

}

void SlimIntClientStateReady::stateAction()
{
    mSlimIntClient->slimIntClientSendAvailableServices();
    return;
}

void SlimIntClientStateReady::stateExitAction()
{
    return;
}

void SlimIntClientStateActive::stateAction()
{
    mSlimIntClient->mStopDisableResponse = false;
    return;
}

void SlimIntClientStateActive::stateExitAction()
{
    mSlimIntClient->mStopDisableResponse = true;
    mSlimIntClient->slimIntClientDisableAllServices();
    return;
}

#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
void SlimIntClient::sendPbStream
(
  uint8_t *msg,
  uint32_t msgSize
)
{
    struct sendPBData: public LocMsg
    {
        uint8_t *m_Data;
        SlimIntClient *mIc;
        uint32_t size;
        sendPBData
        (
            uint8_t *data,
            uint32_t inSize,
            SlimIntClient *ic
        ) :
        LocMsg(),
        m_Data(data),
        size(inSize),
        mIc(ic)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(mIc->mIpcSender), (uint8_t*)(m_Data), size);
            delete m_Data;
        }
    };
    mMsgTask->sendMsg(new sendPBData(msg, msgSize, this));
}

void SlimIntClient::sendPbMsg(string &os)
{
    uint8_t         *outMsg;
    size_t             outMsgSize;
    outMsgSize = os.size();
    outMsg = new uint8_t[outMsgSize];
    SLIM_LOGV("sendPbMsg outMsgSize:%d --> \n", outMsgSize);
    if (outMsg) {
        slim_Memscpy(outMsg, outMsgSize, (uint8_t *) os.c_str(), outMsgSize/*os.size();*/);
    } else {
        SLIM_LOGE("malloc failure for outMsg");
        return;
    }
    sendPbStream(outMsg, outMsgSize);
}

void SlimIntClient::slimIntClientSendUpMsgPb()
{
    string os;
    bool   ret;
    ret = mPbHandler.getPbIntClientUpMsg(os);
    if (ret) {
        sendPbMsg(os);
    } else {
        SLIM_LOGE("getPbSensorData return FALSE!");
    }
    return;
}

void SlimIntClient::slimIntClientSendAvailableServicesPb()
{
    string os;
    bool   ret;
    SLIM_LOGV("slimIntClientSendAvailableServicesPb --> mAvailableServicesMask:%u\n",
    mAvailableServicesMask);

    ret = mPbHandler.getPbAvailableServiceResp(mServiceStatusData, mAvailableServicesMask, os);
    if (ret) {
        sendPbMsg(os);
    } else {
        SLIM_LOGE("getPbSensorData return FALSE!");
    }
    return;
}

void SlimIntClient::decodeFromPbData(const char* data, uint32_t length)
{
    if (NULL == data){
        LOC_LOGe(" data is null!");
        return;
    }
    string s(data, length);
    pbSlimRemoteMsg rMsg;

    if (0 == rMsg.ParseFromString(s)) {
        LOC_LOGe("Failed to parse from input stream!! length: %u", length);
        return;
    }
    uint32_t msgID = rMsg.msgid();
    LOC_LOGi("msgid : %d", msgID);

    switch (msgID) {
        case PB_eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_REQ:
            {
                SLIM_LOGE("eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_REQ --> \n");

                slimIntClientSetState(SlimIntClient::SlimIntClientState::eSTATE_READY);
                slimIntClientSetState(SlimIntClient::SlimIntClientState::eSTATE_ACTIVE);
            }
        break;
        case PB_eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ:
            {
                SLIM_LOGV("PB_eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ --> \n");
                pbSlimRemoteSensorDataReqCtrlMsg sensorReqmsg;
                slimEnableSensorDataRequestStructT mSensorDataRequest;

                if (sensorReqmsg.ParseFromString(rMsg.payload()))
                {
                    bool ret;
                    ret = mPbHandler.getSlimSensorDataReq(sensorReqmsg, mSensorDataRequest);
                    if (ret) {
                        handleSensorRequest(mSensorDataRequest);
                    } else {
                        SLIM_LOGE("getSlimSensorDataReq FAILED! --> \n");
                    }
                }
            }
        break;
        case PB_eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ:
            {
                SLIM_LOGV("PB_eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ --> \n");
                pbSlimRemoteVehicleDataReqCtrlMsg vehicleReqmsg;
                slimEnableVehicleDataRequestStructT mVehicleataRequest;

                if (vehicleReqmsg.ParseFromString(rMsg.payload()))
                {
                    bool ret;
                    ret = mPbHandler.getSlimVehicleMotionDataReq(
                                            vehicleReqmsg, mVehicleataRequest);
                    if (ret) {
                        handleVehicleMotionRequest(mVehicleataRequest);
                    } else {
                        SLIM_LOGE("getSlimVehicleMotionDataReq FAILED! --> \n");
                    }
                }
            }
        break;
        default:
        break;
    }
}
#endif

} //nmaepsace slim
