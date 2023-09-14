/*
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/**
@file
@brief SLIM Remote provider interface file.

This file declares SLIM Remote provider object.

@ingroup slim_RemoteProvider
*/
#ifndef SLIM_REMOTE_PROVIDER_H_INCLUDED
#define SLIM_REMOTE_PROVIDER_H

#include <MultiplexingProvider.h>
#include <slim_os_api.h>
#include <LocIpc.h>
#include <loc_cfg.h>
#include <MsgTask.h>
#include <loc_pla.h>
#include <slim_client_types.h>
#include <SlimRemoteMsgs.h>

#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
#include <SlimRemoteMsgsPbHandler.h>
#endif

/* Wait for Response Timeout value */
#define WAIT_FOR_RESPONSE_TIMER_TIMEOUT_MSEC        (10000)

extern "C"
{

using namespace std;
using namespace loc_util;

namespace slim
{
class remoteProvIpcListener;
class remoteProv;

/*! remoteProv state enumeration */
enum remoteProvState
{
  eRP_STATE_INVALID = -1,
  eRP_STATE_INIT = 0,
  eRP_STATE_WAIT_IC_UP,
  eRP_STATE_READY,
  eRP_STATE_ACTIVE,
  eRP_STATE_MAX,
};

/*! remoteProv sytem connection state */
enum SystemState
{
  eSYS_CONNECTED = 0,
  eSYS_NOT_CONNECTED,
  eSYS_SSR_STATE,
};

/*
 * Remote provider state machine methods
 * inherited from Base state method
 * 1. RP Init state method
 * 2. RP Wait state method
 * 3. RP Ready state method
 * 4. RP Active state method
 */

/* RP Base state method */
class remoteProvStateBase
{
public:
  remoteProv *mpRemoteProv;
  inline remoteProvStateBase(remoteProv *remoteProvObj):
    mpRemoteProv(remoteProvObj) {}
  virtual ~remoteProvStateBase() {}
  virtual void stateAction() = 0;
  virtual void stateExitAction() = 0;
};


/* RP Init state method */
class remoteProvStateInit : public remoteProvStateBase
{
public:
  inline remoteProvStateInit(remoteProv *remoteProvObj):
    remoteProvStateBase(remoteProvObj) {}
  virtual ~remoteProvStateInit() {}
  virtual void stateAction();
  virtual void stateExitAction();
};

/* RP Wait state method */
class remoteProvStateWait : public remoteProvStateBase
{
public:
  inline remoteProvStateWait(remoteProv *remoteProvObj):
    remoteProvStateBase(remoteProvObj) {}
  virtual ~remoteProvStateWait(){}
  virtual void stateAction();
  virtual void stateExitAction();
};

/* RP Ready state method */
class remoteProvStateReady : public remoteProvStateBase
{
public:
  inline remoteProvStateReady(remoteProv *remoteProvObj):
    remoteProvStateBase(remoteProvObj) {}
  virtual ~remoteProvStateReady(){}
  virtual void stateAction();
  virtual void stateExitAction();
};

/* RP Active state method */
class remoteProvStateActive
  : public remoteProvStateBase
{
public:
  inline remoteProvStateActive(remoteProv *remoteProvObj):
    remoteProvStateBase(remoteProvObj) {}
  virtual ~remoteProvStateActive(){}
  virtual void stateAction();
  virtual void stateExitAction();
};

/*
 * Remote provider method
 */
class remoteProv :
  public MultiplexingProvider,
  public LocIpc
{
  friend remoteProvIpcListener;
//  friend remoteProvStateBase;

private:

  /* Method related */
  static remoteProv        *spInstance;
  static Mutex             mInstanceMutex;
  MsgTask*                 mMsgTask;
  shared_ptr<LocIpcSender> mIpcSender;

  /* Slim thread related */
  slim_ThreadT             mThread;
  const uint32_t           mThreadId;

  /* Slim Timer related */
  enum {
    eTIMER_WAIT_FOR_RESPONSE_MSG = 0x01
  };
  enum {
    /**< Timer message ID for expiry */
    eIPC_MSG_TIMER = M_IPC_MSG_TIMER,
  };
  slim_TimerT*                      mWaitForResponseTimer;

  /* State machine related */
  remoteProvState          mCurrentRemoteProvState;
  remoteProvStateBase      *mpRemoteProvStateBase[eRP_STATE_MAX];

  /* Slim Service related */
  uint32_t                                 mServiceStatusCount;
  slimServiceStatusEventStructT            mServiceStatus[MAX_SLIM_AVAILABLE_SERVICES];
  static const slimAvailableServiceMaskT   mSupportedServices;
  slimAvailableServiceMaskT                mEnabledServices;

  /* Cached mechanism used for auto connect feature */
  struct ServiceState
  {
  uint8_t isEnabled;
  /**< Cached service state */
  /*!
   * @brief Service state parameters
   *
   * The parameters of the service state.
   */
  union Params
  {
    //! @brief Common field for all enable/disable states
    slimEnableServiceDataStructT        mEnableConf;
    //! @brief Sensor state parameters.
    slimEnableSensorDataRequestStructT  mEnableSensorDataRequest;
    //! @brief Pedometer state parameters.
    slimEnablePedometerRequestStructT   mEnablePedometerRequest;
    //! @brief QMD state parameters.
    slimEnableQmdDataRequestStructT     mEnableQmdDataRequest;
    //! @brief Vehicle data state parameters.
    slimEnableVehicleDataRequestStructT mEnableVehicleDataRequest;
  };
  Params mActive;       /**< Present state */
  };

  /**
  * @brief Class constructor
  * Creates and initializes new instance of the class.
  */
  remoteProv();

  /**
  * @brief Class destructor
  * Destroys current instance
  */
  ~remoteProv();

  /**
  * @brief initialize RP method
  * Create and start RP main thread
  */
  bool initialize();

  /**
  * @brief runEventLoop()
  * Handle run time events
  */
  void runEventLoop();

  /**
  * @brief Method to send Control Data Request Msg to remote device
  * @param[in]   Control Data Request Msg
  * @param[in]   Size of Control Data Request Msg
  * @return void
  */
  void sendMsg
  (
    SlimRemoteCtrlMsg *pData,
    uint32_t msgSize
  );

  /**
  * @brief Method to send sensor data request to remote device
  * @param[in]   Sensor Data Request Msg
  * @param[in]   Size of Sensor Data Request Msg
  * @return void
  */
  void sendSensorDataReq
  (
    SlimRemoteSensorDataReqCtrlMsg *reqData,
    uint32_t msgSize
  );

  /**
  * @brief Method to send vehicle motion data request to remote device
  * @param[in]   vehicle motion data request Msg
  * @param[in]   Size of vehicle motion data request Msg
  *
  * @return void
  */
  void sendVehicleMotionDataReq
  (
    SlimRemoteVehicleDataReqCtrlMsg * reqData,
    uint32_t msgSize
  );

  /*
  * @brief Method for enabling or disabling sensor service.
  * @param[in] uEnable       Flag that indicates if the service shall be enabled or disabled.
  * @param[in] wReportRateHz Requested reporting rate in Hz.
  * @param[in] wSampleCount  Requested sample count in one batch.
  * @param[in] eService      Service to control.
  * @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateSensorStatus
  (
    bool uEnable,
    uint16_t wReportRateHz,
    uint16_t wSampleCount,
    slimServiceEnumT eService
  );

  /**
  * @brief Method for enabling or disabling vehicle services.
  * @param[in] uEnable Flag that indicates if the service shall be enabled or disabled.
  * @param[in] eService      Service to control.
  * @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateVehicleDataStatus
  (
    bool uEnable,
    slimServiceEnumT eService
  );

  /**
  * @brief Method to set remote prov state.
  * @param[in]   new remote prov state.
  * @return      void
  */
  void remoteProvSetState
  (
    remoteProvState inState
  );

public:

  /* IPC method related */
  unique_ptr<LocIpcRecver>            mIpcRecver;
  bool mIsListenerReady;
  bool mIsProtobufEnabled;
#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
  SlimRemoteMsgsPbHandler             mPbHandler;
#endif
  ServiceState                   mServiceStates[eSLIM_SERVICE_LAST + 1];
  SystemState                    mSystemState;

  /*!
  * @brief Returns provider instance.
  *
  * Method provides access to provider instance. If necessary, the instance
  * is created and initialized.
  *
  * @return Provider instance.
  * @retval 0 On error.
  */
  static slim_ServiceProviderInterfaceType *getProvider();

  /*!
  * @brief SLIM Remote provider main thread function
  *
  */
  static void threadMain
  (
    void *pData
  );


  /**
  * @brief Method to establish remote device connection
  * @param[in]   remote prov obj.
  * @return      True if success or else False
  */
  bool establishRemoteConnection
  (
    void *pData
  );


  /**
  * @brief Method to start Wait for available service response timer
  * @return   void
  */
  void startWaitForResponseTimer();

  /**
  * @brief Method to stop Wait for available service response timer
  * @return   void
  */
  void stopWaitForResponseTimer();


  /**
  @brief Method for requesting available services from remote device.
  *
  * Method fetches available service from remote device
  *
  @return void
  */
  void requestAvailableServices();

  /**
  @brief Method for reporting available services to slim core.
  *
  * Method fetches available service from remote device
  * and reports it to core
  *
  @return void
  */
  void reportAvailableServices();

  /**
  @brief Method resets available services info.
  *
  * Method resets available service
  *
  @return void
  */
  void resetAvailableServiceInfo();


  /**
  @brief Method validate AvailableServiceInfo from remote device
  *
  * Method validate available service contents
  *
  @return true  if available service info is non zero value
          false if available service info is zero value
  */
  inline bool validateAvailableServiceInfo
  (
    const char* data
  )
  {
    SlimRemoteAvailSvcsRespCtrlMsg recvdPkt = *(SlimRemoteAvailSvcsRespCtrlMsg *)data;
    /* return true if available service mask is set */
    if (0 != recvdPkt.mAvailableServicesMask)
      return true;
    else
      return false;
  }


  /**
  @brief Method for storing available services from remote device.
  *
  * Parses received packet from remote device
  * Stores it in class variables
  *
  @param[in]   Received packet data from remote device
  *
  @return void
  */
  void storeAvailableServiceInfo
  (
    const char* data
  );

  /**
  @brief Method for storing service request
  *
  * cache a copy of sensor request received from client
  * cache data will be used in SSR case
  *
  @return void
  */
  void storeServiceRequest
  (
    slimServiceEnumT eService,
    const void *zRequest
  );

  /**
  @brief Method for restoring service states during SSR.
  *
  * Restores service states during SSR handling
  * Sends enable/disable service based on cached info.
  *
  @return void
  */
  void restoreServiceStates();

  /**
  @brief Method to set state to SSR
  *
  * Set remote prov state for SSR handling
  *
  @return void
  */
  void setSsrState();

  /**
  @brief Method to route sensor data to client.
  *
  * Parses received packet from remote device
  * Performs gPtp conversion if needed
  * Route data to registered client
  *
  @return void
  */
  void routeSensorDataToClient
  (
    const char* data
  );

  /**
  @brief Method to route vehicle motion data to client.
  *
  * Parses received packet from remote device
  * Performs gPtp conversion if needed
  * Route data to registered client
  *
  @return void
  */
  void routeVehicleMotionDataToClient
  (
    const char* data
  );

  slimServiceEnumT SlimSensorTypeToService
  (
    slimSensorTypeEnumT slimSensorType
  );

  slimServiceEnumT SlimMotionSensorTypeToService
  (
    slimVehicleMotionSensorTypeEnumT slimMotionSensorType
  );

  void initGptp();

#ifdef SLIM_REMOTE_MSG_PROTO_ENABLED
  void routeSensorDataToClientPB(pbSlimRemoteSensorDataMsg &pbSesnorData);
  void routeVehicleMotionDataToClientPB(pbSlimRemoteVehicleMotionDataMsg &pbVecMotionDataMsg);
  void decodeFromPbData(const char* data, uint32_t length);
  void requestAvailableServicesPB();
  void requestSensorDataPB(
    bool        uEnable,
    uint16_t    wReportRateHz,
    uint16_t    wSampleCount,
    slimServiceEnumT    eService
  );
  void requestVnwDataPB(bool uEnable, slimServiceEnumT eService);
  void sendPbMsg(string &os);
  void sendPbStream(uint8_t *msg, uint32_t msgSize);

#endif
};

}
}
#endif /* SLIM_REMOTE_PROVIDER_H   */
