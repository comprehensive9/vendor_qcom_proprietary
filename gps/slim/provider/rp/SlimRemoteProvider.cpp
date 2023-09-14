/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
/* @file
 @brief SLIM Remote provider implementation file.

 This file defines SLIM Remote provider object implementation.
 */
#include "SlimRemoteProvider.h"
#include <inttypes.h>
#include <stdint.h>
#include <slim_processor.h>
#include "slim_apss_diag.h"
#include "slim_apss_log.h"

#ifdef PTP_SUPPORTED
#include <gptp_helper.h>
#endif

namespace slim {

/* Method related */
class remoteProv;
remoteProv *remoteProv::spInstance = nullptr;
Mutex remoteProv::mInstanceMutex;


/* GPS conf file related */
static char remoteProviderIpAddr[LOC_MAX_PARAM_STRING];
static char remoteProviderPort[LOC_MAX_PARAM_STRING];
static char slimInternalClientIpAddr[LOC_MAX_PARAM_STRING];
static char slimInternalClientPort[LOC_MAX_PARAM_STRING];

#define RP_IP_ADDR_ANY "0.0.0.0"
static loc_param_s_type gpsConfParamTable[] = {
    {"REMOTE_PROVIDER_IP_ADDRESS", &remoteProviderIpAddr,    nullptr, 's'},
    {"REMOTE_PROVIDER_PORT",       &remoteProviderPort,      nullptr, 's'},
    {"SLIM_INTERNAL_CLIENT_IP_ADDRESS", &slimInternalClientIpAddr,    nullptr, 's'},
    {"SLIM_INTERNAL_CLIENT_PORT",       &slimInternalClientPort,      nullptr, 's'}
};


/* LocIpc listener method corresponding to remote provider */
class remoteProvIpcListener : public ILocIpcListener {
    remoteProv& mRemoteProv;
public:
    inline remoteProvIpcListener(remoteProv& rp) : mRemoteProv(rp) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data, uint32_t length, const LocIpcRecver* recver) override;
};


/* Providers support for both sensor and vehicle data services */
const slimAvailableServiceMaskT remoteProv::mSupportedServices =
    (uint32_t(1) << eSLIM_SERVICE_SENSOR_ACCEL)|
    (uint32_t(1) << eSLIM_SERVICE_SENSOR_GYRO)|
    (uint32_t(1) << eSLIM_SERVICE_SENSOR_MAG_UNCALIB)|
    (uint32_t(1) << eSLIM_SERVICE_SENSOR_ACCEL_TEMP)|
    (uint32_t(1) << eSLIM_SERVICE_SENSOR_GYRO_TEMP)|
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_ACCEL)|
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_GYRO)|
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_ODOMETRY)|
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_SPEED)|
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_DWS)|
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_GEAR);


/*!
 * @brief Returns provider instance.
 *
 * Method provides access to provider instance. If necessary, the instance
 * is created and initialized.
 *
 * @return Provider instance.
 * @retval 0 On error.
 */
slim_ServiceProviderInterfaceType *remoteProv::getProvider()
{
  MutexLock _l(mInstanceMutex);
  if (0 == spInstance)
  {
    spInstance = new remoteProv;
    if (!spInstance->initialize())
    {
      SLIM_LOGE("SLIM Remote provider initialization failed;");
      delete spInstance;
      spInstance = 0;
      return 0;
    }
  }
  return spInstance->getSlimInterface();
}


/* Class constructor */
remoteProv::remoteProv()
  : MultiplexingProvider(SLIM_PROVIDER_RP, true, 0),
  mThread(), mThreadId(THREAD_ID_RP), mEnabledServices(0),
  mMsgTask(new MsgTask("SlimRemoteProvMsgTsk", false)),
  mWaitForResponseTimer(0),
  mCurrentRemoteProvState(eRP_STATE_INVALID),
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
  mPbHandler(),
#endif
  mIsProtobufEnabled(false)
{
  int  enablePbMsgFormat = 0;
  static loc_param_s_type rpConfParamTable[] = {
    {"ENABLE_SLIM_REMOTE_MSG_PROTO_FORMAT", &enablePbMsgFormat, NULL, 'n'}
  };
  UTIL_READ_CONF(LOC_PATH_SLIM_CONF, rpConfParamTable);
  if (enablePbMsgFormat) {
    mIsProtobufEnabled = true;
#ifndef SLIM_REMOTE_MSG_PROTO_ENABLED
    SLIM_LOGE("Protobuf code disabled and runtime flag enabled. \n");
    mIsProtobufEnabled = false;
#endif
  }
  mIsListenerReady = false;
  mSystemState = eSYS_NOT_CONNECTED;
  memset(&mThread, 0, sizeof(mThread));
  memset(mServiceStates, 0, sizeof(mServiceStates));
}


/* Class destructor */
remoteProv::~remoteProv()
{
}


/* initialize RP method */
bool remoteProv::initialize()
{
  if (!slim_ThreadCreate(&mThread, threadMain, this, "RpPoll"))
  {
    return false;
  }
  slim_TaskReadyWait(THREAD_ID_RP);

  return true;
}


/* Remote provider main thread function */
void remoteProv::threadMain(void *pData)
{
  SLIM_LOGD("remoteProv::threadMain(0x%"PRIxPTR")",
      (uintptr_t)pData);
  remoteProv *pzRemoteProv = reinterpret_cast<remoteProv*>(pData);

  slim_IpcStart(pzRemoteProv->mThreadId);

  /* Create timer to wait for response from remote setup */
  pzRemoteProv->mWaitForResponseTimer = slim_TimerCreate(eTIMER_WAIT_FOR_RESPONSE_MSG,
                                                         pzRemoteProv->mThreadId);

  /* Update service mask when remote device sends available service */
  pzRemoteProv->m_qServiceMask = 0;
  pzRemoteProv->mpRemoteProvStateBase[eRP_STATE_INIT] = new remoteProvStateInit(pzRemoteProv);
  pzRemoteProv->mpRemoteProvStateBase[eRP_STATE_WAIT_IC_UP] = new remoteProvStateWait(pzRemoteProv);
  pzRemoteProv->mpRemoteProvStateBase[eRP_STATE_READY] = new remoteProvStateReady(pzRemoteProv);
  pzRemoteProv->mpRemoteProvStateBase[eRP_STATE_ACTIVE] = new remoteProvStateActive(pzRemoteProv);
  pzRemoteProv->remoteProvSetState(eRP_STATE_INIT);
  pzRemoteProv->initGptp();
  pzRemoteProv->runEventLoop();
}


/* Method to establish remote device connection */
bool remoteProv::establishRemoteConnection(void *pData)
{
  remoteProv      *pzRemoteProv = reinterpret_cast<remoteProv*>(pData);

  //read IP from conf, later add logic to detect it
  UTIL_READ_CONF(LOC_PATH_SLIM_CONF, gpsConfParamTable);

  SLIM_LOGD("RemoteProviderIpAddr: %s\n", remoteProviderIpAddr);
  SLIM_LOGD("RemoteProviderPort: %s\n", remoteProviderPort);
  SLIM_LOGD("slimInternalClientIpAddr: %s\n", slimInternalClientIpAddr);
  SLIM_LOGD("slimInternalClientPort: %s\n", slimInternalClientPort);

  //read servername and port from config or detect it
  pzRemoteProv->mIpcRecver = pzRemoteProv->getLocIpcInetUdpRecver(
                                           make_shared<remoteProvIpcListener>(*this),
                                           RP_IP_ADDR_ANY,
                                           atoi(remoteProviderPort));

  pzRemoteProv->mIpcSender = pzRemoteProv->getLocIpcInetUdpSender(
                                           slimInternalClientIpAddr,
                                           atoi(slimInternalClientPort));

  if (pzRemoteProv->mIpcRecver != nullptr) {
    if ((pzRemoteProv->mIpcRecver)->isRecvable()) {
      SLIM_LOGD("remoteProv:ipcRecver->isRecvable success <--: \n");
    } else {
      SLIM_LOGE("remoteProv:ipcRecver->isRecvable fail <--: \n");
      return false;
    }

    /* Create and listen on socket*/
    pzRemoteProv->startNonBlockingListening(pzRemoteProv->mIpcRecver);
    SLIM_LOGD("recver not remoteProviderIpAddr null <--: \n");
  } else {
    return false;
  }

  return true;
}

void remoteProv::startWaitForResponseTimer()
{
  SLIM_LOG_ENTRY();
  slim_TimerStart(mWaitForResponseTimer,
    WAIT_FOR_RESPONSE_TIMER_TIMEOUT_MSEC,
    eTIMER_WAIT_FOR_RESPONSE_MSG);
}

void remoteProv::stopWaitForResponseTimer()
{
  SLIM_LOG_ENTRY();
  slim_TimerStop(mWaitForResponseTimer);
}

/* Handle event during run time */
void remoteProv::runEventLoop()
{
  slim_TaskReadyAck();
  remoteProvSetState(eRP_STATE_WAIT_IC_UP);
  while (1)
  {
    slim_IpcMsgT* pz_Msg = NULL;
    while (NULL == (pz_Msg = slim_IpcReceive()));

    switch (pz_Msg->q_MsgId)
    {
      case M_IPC_MSG_TIMER:
      {
        os_TimerExpiryType z_TimerMsg;
        slim_Memscpy(&z_TimerMsg, sizeof(z_TimerMsg), pz_Msg->p_Data, pz_Msg->q_Size);
        switch (z_TimerMsg.q_TimerId)
        {
          case eTIMER_WAIT_FOR_RESPONSE_MSG:
            if (eRP_STATE_WAIT_IC_UP == mCurrentRemoteProvState) {
              /* if RP is yet to receive WAIT_IC_UP msg
                1. request for available service
                2. start wait for response time
               */
              requestAvailableServices();
              startWaitForResponseTimer();
            } else {
              /* if RP received available service response. stop the timer */
              stopWaitForResponseTimer();
            }
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
}


/* Method to send control message data to remote device */
void remoteProv::sendMsg
(
  SlimRemoteCtrlMsg *pData,
  uint32_t msgSize
)
{
  SLIM_LOGD("sendMsg-> Msg ID:%d ..", pData->mMsgId);
  struct sendRequest: public LocMsg
  {
    SlimRemoteCtrlMsg *pMsg;
    remoteProv *mRp;
    uint32_t size;
    sendRequest
    (
      SlimRemoteCtrlMsg *data,
      uint32_t inSize,
      remoteProv *rp
     ) :
        LocMsg(),
        pMsg(data),
        size(inSize),
        mRp(rp)
      {}
    virtual void proc() const
    {
      LocIpc::send(*(mRp->mIpcSender), (uint8_t*)(pMsg), size);
      delete pMsg;
    }
  };
  mMsgTask->sendMsg(new sendRequest(pData, msgSize, this));
}


/* Method to send sensor data request msg to remote device */
void remoteProv::sendSensorDataReq
(
  SlimRemoteSensorDataReqCtrlMsg *reqData,
  uint32_t msgSize
)
{
  SLIM_LOGD("sendSensorDataReq-> Msg ID:%d ..", reqData->mMsgId);
  struct sendRequest: public LocMsg
  {
    SlimRemoteSensorDataReqCtrlMsg *pMsg;
    remoteProv *mRp;
    uint32_t size;
    sendRequest
    (
      SlimRemoteSensorDataReqCtrlMsg *data,
      uint32_t inSize,
      remoteProv *rp
     ) :
        LocMsg(),
        pMsg(data),
        size(inSize),
        mRp(rp)
      {}
    virtual void proc() const
    {
      LocIpc::send(*(mRp->mIpcSender), (uint8_t*)(pMsg), size);
      delete pMsg;
    }
  };
  mMsgTask->sendMsg(new sendRequest(reqData, msgSize, this));
}


/* Method to send vehicle motion data request msg to remote device */
void remoteProv::sendVehicleMotionDataReq
(
  SlimRemoteVehicleDataReqCtrlMsg *reqData,
  uint32_t msgSize
)
{
  SLIM_LOGD("sendVehicleMotionDataReq-> Msg ID:%d ..", reqData->mMsgId);
  struct sendRequest: public LocMsg
  {
    SlimRemoteVehicleDataReqCtrlMsg *pMsg;
    remoteProv *mRp;
    uint32_t size;
    sendRequest
    (
      SlimRemoteVehicleDataReqCtrlMsg *data,
      uint32_t inSize,
      remoteProv *rp
     ) :
        LocMsg(),
        pMsg(data),
        size(inSize),
        mRp(rp)
      {}
    virtual void proc() const
    {
      LocIpc::send(*(mRp->mIpcSender), (uint8_t*)(pMsg), size);
      delete pMsg;
    }
  };
  mMsgTask->sendMsg(new sendRequest(reqData, msgSize, this));
}


/* Convert slim sensor type to slim service notation */
slimServiceEnumT remoteProv::SlimSensorTypeToService
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
slimServiceEnumT remoteProv::SlimMotionSensorTypeToService
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


/**
 * @brief Method for enabling or disabling sensor service.
 *
 * @param[in] uEnable       Flag that indicates if the service shall be
 *                          enabled or disabled.
 * @param[in] wReportRateHz Requested reporting rate in Hz.
 * @param[in] wSampleCount  Requested sample count in one batch.
 * @param[in] eService      Service to control.
 *
 * @return eSLIM_SUCCESS is operation succeeded.
 */
slimErrorEnumT remoteProv::doUpdateSensorStatus
(
  bool uEnable,
  uint16_t wReportRateHz,
  uint16_t wSampleCount,
  slimServiceEnumT eService
)
{
  if (eRP_STATE_ACTIVE != mCurrentRemoteProvState) {
    SLIM_LOGD("Not in active state.. Service %d not enabled..", eService);
    return eSLIM_ERROR_INTERNAL;
  }

  SLIM_LOGD("Set Service:%d status: %d, Report Rate %d, Sample count %c",
            eService, uEnable, wReportRateHz, wSampleCount);

  slimEnableSensorDataRequestStructT      zRequest;

  /* Packetize and transmit */
  zRequest.enableConf.enable = uEnable;
  zRequest.enableConf.providerFlags = 0;
  /* Select native provider in remote device */
  SLIM_MASK_SET
  (
    zRequest.enableConf.providerFlags,
    eSLIM_SERVICE_PROVIDER_NATIVE
  );
  zRequest.reportRate = wReportRateHz;
  zRequest.sampleCount = wSampleCount;
  zRequest.sensor = eService;
  zRequest.batchAlignReq_valid = true;

  /* store service info for SSR routine */
  if (eSYS_SSR_STATE != mSystemState) {
    storeServiceRequest(eService, &zRequest);
  }

  if (!mIsProtobufEnabled) {
    SlimRemoteSensorDataReqCtrlMsg *msgPkt =
      new SlimRemoteSensorDataReqCtrlMsg(eSLIM_CONTROL_MESSAGE_ID_SENSOR_DATA_REQ);

    /* copy request to msgpkt before sending */
    memscpy(&(msgPkt->mSensorDataRequest),
             sizeof(slimEnableSensorDataRequestStructT),
             &zRequest,
             sizeof(slimEnableSensorDataRequestStructT));

    sendSensorDataReq(msgPkt, sizeof(SlimRemoteSensorDataReqCtrlMsg));
  } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
    requestSensorDataPB(uEnable, wReportRateHz, wSampleCount, eService);
#endif
  }

  return eSLIM_SUCCESS;
}


/**
@brief Method for enabling or disabling vehicle services.

@param[in] uEnable Flag that indicates if the service shall be enabled or
                   disabled.
@param[in] eService      Service to control.

@return eSLIM_SUCCESS is operation succeeded.
*/
slimErrorEnumT remoteProv::doUpdateVehicleDataStatus
(
  bool uEnable,
  slimServiceEnumT eService
)
{
  if (eRP_STATE_ACTIVE != mCurrentRemoteProvState) {
    SLIM_LOGD("Not in active state.. Service %d not enabled..", eService);
    return eSLIM_ERROR_INTERNAL;
  }

  SLIM_LOGD("Set Service:%d status: %d", eService, uEnable);

  slimEnableVehicleDataRequestStructT      zRequest;

  zRequest.enableConf.enable = uEnable;
  zRequest.enableConf.providerFlags = 0;
  /* Select native provider in remote device */
  SLIM_MASK_SET
  (
    zRequest.enableConf.providerFlags,
    eSLIM_SERVICE_PROVIDER_NATIVE
  );
  zRequest.service = eService;

  /* store service info for SSR routine */
  if (eSYS_SSR_STATE != mSystemState) {
    storeServiceRequest(eService, &zRequest);
  }

  if (!mIsProtobufEnabled) {
    SlimRemoteVehicleDataReqCtrlMsg *msgPkt =
      new SlimRemoteVehicleDataReqCtrlMsg(eSLIM_CONTROL_MESSAGE_ID_VEHICLE_DATA_REQ);

    /* copy request to msgpkt before sending */
    memscpy(&(msgPkt->mVehicleDataRequest),
             sizeof(slimEnableVehicleDataRequestStructT),
             &zRequest,
             sizeof(slimEnableVehicleDataRequestStructT));

    sendVehicleMotionDataReq(msgPkt, sizeof(SlimRemoteVehicleDataReqCtrlMsg));
  } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
    requestVnwDataPB(uEnable, eService);
#endif
  }
  return eSLIM_SUCCESS;
}

void remoteProv::initGptp()
{
#ifdef PTP_SUPPORTED
  if (gptpInit()) {
      SLIM_LOGV("gptpInit success");
  } else {
      SLIM_LOGE("gptpInit FAIL");
  }
#endif
}

/* Request available service to remote device */
void remoteProv::requestAvailableServices()
{
  if (!mIsProtobufEnabled) {
    SLIM_LOGD("Requesting Available Services..");
    SlimRemoteCtrlMsg *msgPkt;
    msgPkt = new SlimRemoteCtrlMsg(eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_REQ);
    sendMsg(msgPkt, sizeof(SlimRemoteCtrlMsg));
  } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
    requestAvailableServicesPB();
#endif
  }
  return ;
}

/* Report available service to slim core */
void remoteProv::reportAvailableServices()
{
  SLIM_LOGD("Reporting Available Serv %d...", mEnabledServices);

  if (0 != mServiceStatusCount) {
    /* Update Service mask for remote provider */
    m_qServiceMask = mEnabledServices;
  }

  /* Route the change in config to client */
  routeConfigurationChange(mEnabledServices);
}


/* Method to send sensor data to registered clients */
void remoteProv::routeSensorDataToClient
(
  const char* data
)
{
  if (eRP_STATE_ACTIVE != mCurrentRemoteProvState) {
    SLIM_LOGD("Not in active state.. Sensor data routing failed..");
    return;
  }

  SlimRemoteSensorDataMsg recvdPkt = *(SlimRemoteSensorDataMsg *)data;
  slimServiceEnumT eService = SlimSensorTypeToService(recvdPkt.mSensorData.sensorType);
  slimSensorDataStructT zData = recvdPkt.mSensorData;
  SLIM_LOGD("Routing service %d to client", eService);
  routeIndication(eService, zData);
#ifdef PTP_SUPPORTED
  uint64_t gptpCurrentns = 0;
  gptpGetCurPtpTime(&gptpCurrentns);
  gptpCurrentns = gptpCurrentns/1000;
  slimLogDiagPkt(
      eSLIM_LOG_SOURCE_REMOTE_PROVIDER,
      eSLIM_TIME_SOURCE_GPTP_TIME,
      eSLIM_MESSAGE_ID_SENSOR_DATA_IND,
      gptpCurrentns,
      &zData,
      sizeof(slimSensorDataStructT),
      eService);
#endif
  return;
}


/* Method to send vehicle motion data to registered clients */
void remoteProv::routeVehicleMotionDataToClient
(
  const char* data
)
{
  if (eRP_STATE_ACTIVE != mCurrentRemoteProvState) {
    SLIM_LOGD("Not in active state.. Vehicle data routing failed..");
    return;
  }

  SlimRemoteVehicleMotionDataMsg recvdPkt = *(SlimRemoteVehicleMotionDataMsg *)data;
  slimServiceEnumT eService = SlimMotionSensorTypeToService(recvdPkt.mVehicleMotionData.sensorType);
  slimVehicleMotionDataStructT zData = recvdPkt.mVehicleMotionData;
  SLIM_LOGD("Routing service %d to client", eService);
  routeIndication(eService, zData);
#ifdef PTP_SUPPORTED
  uint64_t gptpCurrentns = 0;
  gptpGetCurPtpTime(&gptpCurrentns);
  gptpCurrentns = gptpCurrentns/1000;
  slimLogDiagPkt(
      eSLIM_LOG_SOURCE_REMOTE_PROVIDER,
      eSLIM_TIME_SOURCE_GPTP_TIME,
      eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND,
      gptpCurrentns,
      &zData,
      sizeof(slimVehicleMotionDataStructT),
      eService);
#endif
  return;
}


/* storeAvailableServiceInfo from remote device */
void remoteProv::storeAvailableServiceInfo
(
  const char* data
)
{
  SlimRemoteAvailSvcsRespCtrlMsg recvdPkt = *(SlimRemoteAvailSvcsRespCtrlMsg *)data;

  mEnabledServices = recvdPkt.mAvailableServicesMask;
  for (int eService = 0; eService <= eSLIM_SERVICE_LAST; eService++)
  {
    if (SLIM_MASK_IS_SET(mEnabledServices, eService)) {
      memscpy((void*)&mServiceStatus[eService], sizeof(slimServiceStatusEventStructT),
               (void*)&recvdPkt.mServiceStatus[eService], sizeof(slimServiceStatusEventStructT));
      mServiceStatusCount++;
    }
  }
}


/* In case of SSR, resetAvailableServiceInfo */
void remoteProv::resetAvailableServiceInfo()
{
  mServiceStatusCount = 0;
  m_qServiceMask = 0;
  mEnabledServices = 0;
}

/* In case of SSR, storeServiceRequest */
void remoteProv::storeServiceRequest
(
  slimServiceEnumT eService,
  const void *zRequest
)
{
  ServiceState &zServiceState = mServiceStates[eService];

  switch (eService)
  {
    case eSLIM_SERVICE_SENSOR_ACCEL:
    case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
    case eSLIM_SERVICE_SENSOR_GYRO:
    case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
    case eSLIM_SERVICE_SENSOR_MAG_CALIB:
    case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
    case eSLIM_SERVICE_SENSOR_BARO:
      memscpy(&zServiceState.mActive.mEnableSensorDataRequest,
         sizeof(slimEnableSensorDataRequestStructT),
         zRequest,
         sizeof(slimEnableSensorDataRequestStructT));
      zServiceState.isEnabled = zServiceState.mActive.mEnableSensorDataRequest.enableConf.enable;
      break;
    case eSLIM_SERVICE_VEHICLE_SPEED:
    case eSLIM_SERVICE_VEHICLE_DWS:
    case eSLIM_SERVICE_VEHICLE_GEAR:
    case eSLIM_SERVICE_VEHICLE_STEERING_WHEEL:
      memscpy(&zServiceState.mActive.mEnableVehicleDataRequest,
           sizeof(slimEnableVehicleDataRequestStructT),
           zRequest,
           sizeof(slimEnableVehicleDataRequestStructT));
      zServiceState.isEnabled = zServiceState.mActive.mEnableVehicleDataRequest.enableConf.enable;
      break;
  }

  return;
}

/* In case of SSR, restoreServiceStates */
void remoteProv::restoreServiceStates()
{
  for (int eService = 0; eService <= eSLIM_SERVICE_LAST; eService++)
  {
    if (SLIM_MASK_IS_SET(mEnabledServices, eService)) {
      ServiceState &zServiceState = mServiceStates[eService];
      switch (eService)
      {
        case eSLIM_SERVICE_SENSOR_ACCEL:
        case eSLIM_SERVICE_SENSOR_ACCEL_TEMP:
        case eSLIM_SERVICE_SENSOR_GYRO:
        case eSLIM_SERVICE_SENSOR_GYRO_TEMP:
        case eSLIM_SERVICE_SENSOR_MAG_CALIB:
        case eSLIM_SERVICE_SENSOR_MAG_UNCALIB:
        case eSLIM_SERVICE_SENSOR_BARO:
          if (zServiceState.isEnabled) {
            doUpdateSensorStatus(
              zServiceState.mActive.mEnableSensorDataRequest.enableConf.enable,
              zServiceState.mActive.mEnableSensorDataRequest.reportRate,
              zServiceState.mActive.mEnableSensorDataRequest.sampleCount,
              zServiceState.mActive.mEnableSensorDataRequest.sensor
              );
          }
        break;
      case eSLIM_SERVICE_VEHICLE_SPEED:
      case eSLIM_SERVICE_VEHICLE_DWS:
      case eSLIM_SERVICE_VEHICLE_GEAR:
      case eSLIM_SERVICE_VEHICLE_STEERING_WHEEL:
        if (zServiceState.isEnabled) {
          doUpdateVehicleDataStatus(
            zServiceState.mActive.mEnableVehicleDataRequest.enableConf.enable,
            zServiceState.mActive.mEnableVehicleDataRequest.service
            );
        }
      break;
      }
    }
  }

  return;
}

/* Method to set SSR state*/
void remoteProv::setSsrState()
{
  mSystemState = eSYS_SSR_STATE;
  return;
}

/* Method to set remote prov state*/
void remoteProv::remoteProvSetState(remoteProvState inState)
{
  remoteProvState currentState = mCurrentRemoteProvState;

  if (eRP_STATE_INVALID != currentState) {
    mpRemoteProvStateBase[currentState]->stateExitAction();
  }

  SLIM_LOGD("RP Switching State from %d to %d", mCurrentRemoteProvState, inState);
  mCurrentRemoteProvState = inState;
  mpRemoteProvStateBase[inState]->stateAction();

  return;
}


/* Method to indicate listener is ready */
void remoteProvIpcListener::onListenerReady()
{
  SLIM_LOGD("RP Listener ready");
  mRemoteProv.mIsListenerReady = true;
  return;
}


/* Callback from LocIpc when new msg is received */
void remoteProvIpcListener::onReceive(const char* data, uint32_t length, const LocIpcRecver* recver)
{
  if (!mRemoteProv.mIsProtobufEnabled) {
    SlimRemoteMsgBase           m_msg = *(SlimRemoteMsgBase *)data;
    slimErrorEnumT              e_Status = eSLIM_ERROR_INTERNAL;
    uint8_t                     u_TxnId;
    SLIM_LOGV("remoteProvIpcListener:onReceive m_msg.m_msgId:%d--> \n", m_msg.mMsgId);
    switch (m_msg.mMsgId)
    {
      case eSLIM_CONTROL_MESSAGE_ID_SLIM_INT_CLIENT_UP:
      /* Reset Available service in case of SSR */
      if (mRemoteProv.mCurrentRemoteProvState == eRP_STATE_ACTIVE) {
        mRemoteProv.resetAvailableServiceInfo();
        mRemoteProv.setSsrState();
      }
      mRemoteProv.remoteProvSetState(eRP_STATE_WAIT_IC_UP);
      break;
      case eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_RESP:
      if (mRemoteProv.validateAvailableServiceInfo(data))
      {
        mRemoteProv.storeAvailableServiceInfo(data);
        mRemoteProv.remoteProvSetState(eRP_STATE_ACTIVE);
      }
      break;
      case eSLIM_MESSAGE_ID_SENSOR_DATA_IND:
        mRemoteProv.routeSensorDataToClient(data);
      break;
      case eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND:
        mRemoteProv.routeVehicleMotionDataToClient(data);
      break;
      default:
      break;
    }
  } else {
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
    mRemoteProv.decodeFromPbData(data, length);
#endif
  }
  return;
}


/* Init state action */
void remoteProvStateInit::stateAction()
{
  SLIM_LOGD("Executing RP Init State Action");
  if (!mpRemoteProv->establishRemoteConnection(mpRemoteProv)) {
    SLIM_LOGE("Remote provider init fail...");
    return;
  }
  mpRemoteProv->startWaitForResponseTimer();
}

/* Init state exit action */
void remoteProvStateInit::stateExitAction()
{
  SLIM_LOGD("Executing RP Init State Exit Action");
  return;
}

/* Wait state action */
void remoteProvStateWait::stateAction()
{
  SLIM_LOGD("Executing RP Wait State Action");
  mpRemoteProv->requestAvailableServices();
}

/* Wait state exit action */
void remoteProvStateWait::stateExitAction()
{
  SLIM_LOGD("Executing RP Wait State Exit Action");
  /* if not in SSR state, stop waitForResponseTimer */
  if (eSYS_SSR_STATE != mpRemoteProv->mSystemState) {
    mpRemoteProv->stopWaitForResponseTimer();
  }
  mpRemoteProv->reportAvailableServices();
  return;
}

/* Ready state action */
void remoteProvStateReady::stateAction()
{
  SLIM_LOGD("Executing RP Ready State Action");
  mpRemoteProv->reportAvailableServices();
}

/* Ready state exit action */
void remoteProvStateReady::stateExitAction()
{
  SLIM_LOGD("Executing RP Ready State Exit Action");
  return;
}

/* Active state action */
void remoteProvStateActive::stateAction()
{
  SLIM_LOGD("Executing RP Active State Action");
  if (eSYS_SSR_STATE == mpRemoteProv->mSystemState) {
    mpRemoteProv->restoreServiceStates();
  }
  mpRemoteProv->mSystemState = eSYS_CONNECTED;
  return;
}

/* Active state exit action */
void remoteProvStateActive::stateExitAction()
{
  SLIM_LOGD("Executing RP Active State Exit Action");
  mpRemoteProv->reportAvailableServices();
  return;
}

#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED

void remoteProv::requestAvailableServicesPB()
{
  string   os;
  bool     ret;

  ret = mPbHandler.getPbAvailableServicesReq(os);
  if (ret) {
    sendPbMsg(os);
  } else {
    SLIM_LOGE("getPbAvailableServicesReq return FALSE!");
  }
}

void remoteProv::requestSensorDataPB(
  bool    uEnable,
  uint16_t  wReportRateHz,
  uint16_t  wSampleCount,
  slimServiceEnumT  eService
)
{
  string     os;
  bool       ret;

  ret = mPbHandler.getPbSensorDataReq(uEnable, wReportRateHz, wSampleCount, eService, os);
  if (ret) {
    sendPbMsg(os);
  } else {
    SLIM_LOGE("getPbSensorDataReq return FALSE!");
  }
}

void remoteProv::requestVnwDataPB
(
  bool         uEnable,
  slimServiceEnumT   eService
)
{
  string     os;
  bool       ret;

  ret = mPbHandler.getPbVnwDataReq(uEnable, eService, os);
  if (ret) {
    sendPbMsg(os);
  } else {
    SLIM_LOGE("getPbVnwDataReq return FALSE!");
  }
}

void remoteProv::sendPbMsg(string &os)
{
  uint8_t     *outMsg;
  size_t       outMsgSize;
  outMsgSize = os.size();
  outMsg = new uint8_t[outMsgSize];
  if (outMsg) {
    memscpy(outMsg, outMsgSize, (uint8_t *) os.c_str(), outMsgSize/*os.size()*/);
  } else {
    SLIM_LOGE("malloc failure for outMsg");
    return;
  }
  sendPbStream(outMsg, outMsgSize);
}

void remoteProv::sendPbStream
(
  uint8_t *msg,
  uint32_t msgSize
)
{
  struct sendPBData: public LocMsg
  {
    uint8_t *m_Data;
    remoteProv *mRp;
    uint32_t size;
    sendPBData
    (
      uint8_t *data,
      uint32_t inSize,
      remoteProv *rp
    ) :
    LocMsg(),
    m_Data(data),
    size(inSize),
    mRp(rp)
    {}
    virtual void proc() const
    {
      LocIpc::send(*(mRp->mIpcSender), (uint8_t*)(m_Data), size);
      delete m_Data;
    }
  };
  mMsgTask->sendMsg(new sendPBData(msg, msgSize, this));
}

void remoteProv::routeSensorDataToClientPB(pbSlimRemoteSensorDataMsg &pbSesnorData)
{
  if (eRP_STATE_ACTIVE != mCurrentRemoteProvState) {
    SLIM_LOGE("Not in active state.. Sensor data routing failed..");
    return;
  }

  slimServiceEnumT          eService;
  slimSensorDataStructT     zSensorData;
  SlimRemotePTPtime         gPTP;
  bool                      ret;
  ret = mPbHandler.getSlimSensorData(pbSesnorData, zSensorData, gPTP);
  if (!ret) {
    SLIM_LOGE("getSlimSensorData return FALSE!");
    return;
  }

  eService = SlimSensorTypeToService(zSensorData.sensorType);
  routeIndication(eService, zSensorData);
#ifdef PTP_SUPPORTED
  uint64_t gptpCurrentns = 0;
  gptpGetCurPtpTime(&gptpCurrentns);
  gptpCurrentns = gptpCurrentns/1000;
  slimLogDiagPkt(
      eSLIM_LOG_SOURCE_REMOTE_PROVIDER,
      eSLIM_TIME_SOURCE_GPTP_TIME,
      eSLIM_MESSAGE_ID_SENSOR_DATA_IND,
      gptpCurrentns,
      &zSensorData,
      sizeof(slimSensorDataStructT),
      eService);
#endif
  return;
}

void remoteProv::routeVehicleMotionDataToClientPB(
                pbSlimRemoteVehicleMotionDataMsg &pbVecMotionDataMsg)
{
  if (eRP_STATE_ACTIVE != mCurrentRemoteProvState) {
    SLIM_LOGE("Not in active state.. Sensor data routing failed..");
    return;
  }

  slimServiceEnumT            eService;
  slimVehicleMotionDataStructT    zVnwData;
  SlimRemotePTPtime        gPTP;
  bool                     ret;

  ret = mPbHandler.getSlimVehicleMotionData(pbVecMotionDataMsg, zVnwData, gPTP);
  if (!ret) {
    SLIM_LOGE("getSlimVehicleMotionData return FALSE!");
    return;
  }

  eService = SlimMotionSensorTypeToService(zVnwData.sensorType);
  routeIndication(eService, zVnwData);
#ifdef PTP_SUPPORTED
  uint64_t gptpCurrentns = 0;
  gptpGetCurPtpTime(&gptpCurrentns);
  gptpCurrentns = gptpCurrentns/1000;
  slimLogDiagPkt(
      eSLIM_LOG_SOURCE_REMOTE_PROVIDER,
      eSLIM_TIME_SOURCE_GPTP_TIME,
      eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND,
      gptpCurrentns,
      &zVnwData,
      sizeof(slimVehicleMotionDataStructT),
      eService);
#endif
  return;
}

void remoteProv::decodeFromPbData(const char* data, uint32_t length)
{
  if (NULL == data) {
    SLIM_LOGE(" data is null!");
    return;
  }
  string s(data, length);
  pbSlimRemoteMsg rMsg;

  if (0 == rMsg.ParseFromString(s)) {
    SLIM_LOGE("Failed to parse from input stream!! length: %u", length);
    return;
  }
  uint32_t msgID = rMsg.msgid();
  SLIM_LOGD("msgid : %d", msgID);

  switch (msgID) {
  case PB_eSLIM_CONTROL_MESSAGE_ID_SLIM_INT_CLIENT_UP:
    {
      SLIM_LOGD("PB_eSLIM_CONTROL_MESSAGE_ID_SLIM_INT_CLIENT_UP\n");
      /* Reset Available service in case of SSR */
      if (eRP_STATE_ACTIVE == mCurrentRemoteProvState) {
        resetAvailableServiceInfo();
        setSsrState();
      }
      remoteProvSetState(eRP_STATE_WAIT_IC_UP);
    }
    break;
  case PB_eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_RESP:
    {
      SLIM_LOGD("PB_eSLIM_CONTROL_MESSAGE_ID_AVAILABLE_SERVICE_RESP\n");
      pbSlimRemoteAvailSvcsRespCtrlMsg pbSvcsResp;
      if (pbSvcsResp.ParseFromString(rMsg.payload())) {
        if (mPbHandler.validateAvailableServiceInfoPB(pbSvcsResp)) {
          bool ret;
          ret = mPbHandler.storeAvailableServiceInfoPB(pbSvcsResp,
                    mServiceStatus, mEnabledServices, mServiceStatusCount);
          if (ret) {
            SLIM_LOGD("AVAILABLE_SERVICE_RESP mEnabledServices:%u mServiceStatusCount:%u\n",
                mEnabledServices, mServiceStatusCount);
          } else {
            SLIM_LOGE("storeAvailableServiceInfoPB FAILED!");
          }
          remoteProvSetState(eRP_STATE_ACTIVE);
        }
      }
    }
    break;
  case PB_eSLIM_MESSAGE_ID_SENSOR_DATA_IND:
    {
      SLIM_LOGD("PB_eSLIM_MESSAGE_ID_SENSOR_DATA_IND\n");
      pbSlimRemoteSensorDataMsg pbSesnorData;

      if (pbSesnorData.ParseFromString(rMsg.payload())) {
        routeSensorDataToClientPB(pbSesnorData);
      }
    }
    break;
  case PB_eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND:
    {
      SLIM_LOGD("PB_eSLIM_MESSAGE_ID_VEHICLE_MOTION_DATA_IND\n");
      pbSlimRemoteVehicleMotionDataMsg pbVecMotionDataMsg;
      if (pbVecMotionDataMsg.ParseFromString(rMsg.payload())) {
        routeVehicleMotionDataToClientPB(pbVecMotionDataMsg);
      }
    }
    break;
  default:
    break;
  }
}
#endif

} //end of slim namespace
