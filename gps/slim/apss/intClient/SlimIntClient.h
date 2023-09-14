/*===========================================================================

Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/**
@file
@brief SLIM Internal Client interface file.

This file declares SLIM Internal Client object.

@ingroup SlimIntClient
*/

#ifndef SLIM_INT_CLIENT_H
#define SLIM_INT_CLIENT_H

#include <stdio.h>
#include <LocIpc.h>
#include <MsgTask.h>
#include <loc_pla.h>
#include <log_util.h>
#include <slim_client_types.h>
#include <slim_internal_client_types.h>
#include <slim_os_api.h>
#include <loc_cfg.h>
#include <slim_os_log_api.h>
#include <slim_utils.h>
#include <SlimRemoteMsgs.h>

#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
#include <SlimRemoteMsgsPbHandler.h>
#endif
#define SLIM_INT_CLIENT_UP_RETRY_TIMER_TIMEOUT_MSEC        (10000)
#define SLIM_QTIMER_BOOTTIME_OFFSET_CAL_TIMER_MSEC         (60000)
#define SLIM_INT_CLIENT_SERVICE_TXN_ID                     (0x10)
#define SLIM_QTIMER_BOOTTIME_OFFSET_CAL_SAMPLE_COUNT       (20)

/* threshold to check premption */
#define APSS_QTIMER_GAP_THRESHOLD_NS   (5000)
#define APSS_QTIMER_GAP_MAX_ITERATION  (20)

#ifdef __cplusplus
extern "C"
{
#endif

using namespace std;
using namespace loc_util;

namespace slim
{
    class SlimIntClientIpcListener;

    class SlimIntClient;

    class SlimIntClientStateBase
    {
        public:
            SlimIntClient        *mSlimIntClient;
            inline SlimIntClientStateBase(SlimIntClient *slimIntClientObj):
                    mSlimIntClient(slimIntClientObj) {}
            virtual ~SlimIntClientStateBase(){}
            virtual void stateAction() = 0;
            virtual void stateExitAction() = 0;
    };

    class SlimIntClientStateInit : public SlimIntClientStateBase
    {
        public:
            inline SlimIntClientStateInit(SlimIntClient *slimIntClientObj):
                SlimIntClientStateBase(slimIntClientObj) {}
            virtual ~SlimIntClientStateInit(){}
            virtual void stateAction();
            virtual void stateExitAction();
    };

    class SlimIntClientStateWaitForRP : public SlimIntClientStateBase
    {
        public:
            inline SlimIntClientStateWaitForRP(SlimIntClient *slimIntClientObj):
                SlimIntClientStateBase(slimIntClientObj) {}
            virtual ~SlimIntClientStateWaitForRP(){}
            virtual void stateAction();
            virtual void stateExitAction();
    };

    class SlimIntClientStateReady : public SlimIntClientStateBase
    {
        public:
            inline SlimIntClientStateReady(SlimIntClient *slimIntClientObj):
                SlimIntClientStateBase(slimIntClientObj) {}
            virtual ~SlimIntClientStateReady(){}
            virtual void stateAction();
            virtual void stateExitAction();
    };

    class SlimIntClientStateActive : public SlimIntClientStateBase
    {
        public:
            SlimIntClientStateActive(SlimIntClient *slimIntClientObj):
                SlimIntClientStateBase(slimIntClientObj) {}
            virtual ~SlimIntClientStateActive(){}
            virtual void stateAction();
            virtual void stateExitAction();
    };

    class SlimIntClient : public LocIpc
    {
        friend     SlimIntClientIpcListener;

        enum ClientChannelState {
            CLOSED, //!< Connection is closed
            OPENED, //!< Connection is opened
        };

        enum SlimIntClientState {
            eSTATE_INVALID = -1,
            eSTATE_INIT = 0, //!< Connection is closed
            eSTATE_WAIT_FOR_RP, //!< Connection is opened
            eSTATE_READY,
            eSTATE_ACTIVE,
            eSTATE_MAX,
        };

        enum {
            eTIMER_INT_CLIENT_UP_RETRY_TRIMER = 10, /**< Default timer */
            eTIMER_QTIMER_OFFSET_CAL_TIMER,
        };

        enum {
            eIPC_MSG_TIMER = M_IPC_MSG_TIMER,
            /**< Timer message ID */
        };

        private:
            static SlimIntClient*               spInstance;
            const uint32_t                      mThreadId;
            MsgTask*                            mMsgTask;
            shared_ptr<LocIpcSender>            mIpcSender;
            bool                                mIsListenerReady;
            ClientChannelState                  mChannelState;
            slimClientContextT                  mContext;
            slim_ThreadT                        mThread;
            uint32_t                            mAvailableServicesMask;
            slimServiceStatusEventStructT
                        mServiceStatusData[MAX_SLIM_AVAILABLE_SERVICES];
            SlimIntClientState                  mCurrentSlimIntClientState;
            SlimIntClientStateBase*             mpSlimIntClientState[eSTATE_MAX];
            slim_TimerT*                        mRetryTimer;
            slim_TimerT*                        mQtimerOffsetTimer;
            int64_t
                        mQtimerOffset[SLIM_QTIMER_BOOTTIME_OFFSET_CAL_SAMPLE_COUNT];
            int64_t                             mQtimerOffsetFromBootTime;
            uint32_t                            mEnabledServices;
            bool                                mIsGptpInitialized;
            bool                                mIsGptpCalEnabled;
            bool                                mIsProtobufEnabled;
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
            SlimRemoteMsgsPbHandler                mPbHandler;
#endif
        public:
            unique_ptr<LocIpcRecver>            mIpcrecver;
            bool                                mStopDisableResponse;
            SlimIntClient();
            ~SlimIntClient();

            void slimIntClientHandleCb
            (
                const slimMessageHeaderStructT *pzHeader,
                void *pMessage
            );
            void slimIntClientProcessOpenResponse
            (
                const slimMessageHeaderStructT *pzHeader
            );
            void slimIntClientSetState
            (
                SlimIntClientState inClientState
            );
            void slimIntClientSendCtrlMsg
            (
                SlimRemoteCtrlMsg *inMsg,
                uint32_t msgSize
            );
            void slimIntClientAvailSvcsRespSendMsg
            (
                SlimRemoteAvailSvcsRespCtrlMsg *inMsg,
                uint32_t msgSize
            );
            void slimIntClientSensorDataSendMsg
            (
                SlimRemoteSensorDataMsg *inMsg,
                uint32_t msgSize
            );
            void slimIntClientDataSendMsg
            (
                SlimRemoteDataMsg *inMsg,
                uint32_t msgSize
            );
            void slimIntClientVehicleSensorDataSendMsg
            (
                SlimRemoteVehicleSensorDataMsg *inMsg,
                uint32_t msgSize
            );
            void slimIntClientVehicleOdometryDataSendMsg
            (
                SlimRemoteVehicleOdometryDataMsg *inMsg,
                uint32_t msgSize
            );
            void slimIntClientVehicleMotionDataSendMsg
            (
                SlimRemoteVehicleMotionDataMsg *inMsg,
                uint32_t msgSize
            );
            void slimIntClientsetSensorDataGptpTime
            (
            slimSensorDataStructT *pSensor
            );
            void slimIntClientsetVnwDataGptpTime
            (
            slimVehicleMotionDataStructT *pVnw
            );
            void runEventLoop();
            void slimIntClientInitialize();
            void slimIntClientCalculateQtimerOffset();
            void slimIntClientSendUpMsg();
            bool slimIntClientStartUpMsgRetryTimer();
            bool slimIntClientStopUpMsgRetryTimer();
            bool slimIntClientStartQtimerOffsetCalTimer();
            bool slimIntClientStopQtimerOffsetCalTimer();
            void slimIntClientSendAvailableServices();
            void slimIntClientCalQtimerOffset();
            void slimIntClientDisableAllServices();
            static SlimIntClient* getInstance();
            static void threadMain
            (
                void *pData
            );
            static void slimIntClientCallback
            (
                uint64 tCbData,
                const slimMessageHeaderStructT *pzHeader,
                void *pMessage
            );
            uint32_t ticksToMsecs
            (
                uint32_t t_Ticks
            );
            void handleSensorRequest(slimEnableSensorDataRequestStructT &zRequest);
            void handleVehicleMotionRequest(slimEnableVehicleDataRequestStructT &zRequest);
            uint64_t slimIntGetQtimerTimeNs();
            uint64_t slimIntGetClockBootTimeNs();
            slimServiceEnumT SlimSensorTypeToService(slimSensorTypeEnumT slimSensorType);
            slimServiceEnumT SlimMotionSensorTypeToService
            (
                slimVehicleMotionSensorTypeEnumT slimMotionSensorType
            );
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
            void sendPbStream(uint8_t *msg, uint32_t msgSize);
            void sendPbMsg(string &os);
            void slimIntClientSendUpMsgPb();
            void slimIntClientSendAvailableServicesPb();
            void decodeFromPbData(const char* data, uint32_t length);
#endif
    };
}
#ifdef __cplusplus
}
#endif /* extern c */

#endif /* SLIM_INT_CLIENT_H */
