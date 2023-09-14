/*
   Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
   */
/**
  @file
  @brief Sensor PlayBack provider interface file.

  This file declares Sensor PlayBack provider object.

  @ingroup slim_SensorPlayBackProvider
  */

#include <inttypes.h>
#include <slim_os_log_api.h>
#include <slim_os_api.h>
#include "SlimSensorPlayBackProvider.h"
#include <loc_cfg.h>


#include <linux/input.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <algorithm>
#include "diag_lsm.h"
//#include "diag_lsm_dci.h"
#include <dlfcn.h>
#include <fstream>

#include <loc_pla.h>


#include <utils/SystemClock.h>
#include <utils/Timers.h>

using namespace slim;
using namespace std;

SensorPlayBackProvider::HalLibInfo SensorPlayBackProvider::m_zLibInfo = {0};

Mutex SensorPlayBackProvider::s_zInstanceMutex;

SensorPlayBackProvider *SensorPlayBackProvider::s_pzInstance;


const uint32_t SensorPlayBackProvider::s_qThreadId = THREAD_ID_SENSOR_PLAYBACK;

const slimAvailableServiceMaskT SensorPlayBackProvider::s_qSupportedServices =
(uint32_t(1) << eSLIM_SERVICE_SENSOR_ACCEL)|
(uint32_t(1) << eSLIM_SERVICE_SENSOR_GYRO)|
(uint32_t(1) << eSLIM_SERVICE_SENSOR_MAG_UNCALIB);
/**
  @brief Method for enabling or disabling sensor service.

  Generic function to start/stop a sensor based on provided sampling rate,
  batching rate, mounted state, and sensor information using Sensor1 Buffering
  API.

  @todo The sampling frequency is a mere suggestion to the sensor1 daemon.
  Sensor 1 will stream at the requested minimum sampling frequency requested
  by all AP clients combined. So we do see cases where buffering API does not
  help and we get single sensor data for every indication. In that case should
  SLIM AP do the batching?

  @param[in] uEnable       Flag that indicates if the service sPlayBackl be enabled
  or disabled.
  @param[in] wReportRateHz Requested reporting rate in Hz.
  @param[in] wSampleCount  Requested sample count in one batch.
  @param[in] eService      Service to control.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  slimErrorEnumT SensorPlayBackProvider::doUpdateSensorStatus
(
 bool uEnable,
 uint16_t wReportRateHz,
 uint16_t wSampleCount,
 slimServiceEnumT eService
 )
{
  SLIM_LOGD("sensorUpdate service %d enable %d rate %d sampleCount %d",
      eService, uEnable, wReportRateHz, wSampleCount);

  MutexLock _l(m_zLock);

  if (true == uEnable)
  {
    if (eSLIM_SERVICE_SENSOR_ACCEL == eService)
    {
      m_bACCELService = TRUE;
    }
    else if (eSLIM_SERVICE_SENSOR_GYRO == eService)
    {
      m_bGYROService = TRUE;
    }
    else if (eSLIM_SERVICE_SENSOR_MAG_CALIB == eService)
    {
      m_bMAGService = TRUE;
    }

    if (NULL != SessionStartedPtr && NULL != ValidGpsTimeRxdPtr && 0 == *SessionStartedPtr)
    {
      /* Reset the States */
      m_bNotFoundInFile = FALSE;
      *ValidGpsTimeRxdPtr = FALSE;

      m_bSearchLowerLimit = TRUE;
      if (NULL == DCIInitPtr || (this->*DCIInitPtr)(&mClient_id) != eSLIM_SUCCESS )
      {
        SLIM_LOGE("Diag_DCI_Init return error");
      }

      SLIM_LOGD("Diag_DCI_Init Success. client ID = %d", mClient_id);

      SLIM_LOGD("Enable Sensor service Request. Signal Sensor Play back to continue");
      pthread_mutex_lock (&m_zPlayBackStartMutex);
      pthread_cond_broadcast (&m_zPlayBackStartCond);
      pthread_mutex_unlock (&m_zPlayBackStartMutex);
      pthread_mutex_unlock (&m_zPlayBackStartMutex2);
      /* Give some time (2 Msec) to start the regular operations for the threads */
      usleep(2000);

      SLIM_LOGD("wReportRateHz = %d  wSampleCount = %d Timer_Interval = %d",
          wReportRateHz, wSampleCount, (1000/wReportRateHz));
      // SPB_TBD
      SLIM_LOGD("create_timer = %d ", 100);
      if (NULL != create_timerPtr && NULL != SessionStartedPtr)
      {
        (*create_timerPtr)(100);
        *SessionStartedPtr = 1;
      }
    }
  }
  else
  {
    SLIM_LOGD("Disable Sensor service Request.");
    if (eSLIM_SERVICE_SENSOR_ACCEL == eService)
    {
      m_bACCELService = FALSE;
      SLIM_LOGD("m_bACCELService = FALSE");
    }
    else if (eSLIM_SERVICE_SENSOR_GYRO == eService)
    {
      m_bGYROService = FALSE;
      SLIM_LOGD("m_bGYROService = FALSE");
    }
    else if (eSLIM_SERVICE_SENSOR_MAG_CALIB == eService)
    {
      m_bMAGService = FALSE;
      SLIM_LOGD("m_bMAGService = FALSE");
    }

    if (NULL != SessionStartedPtr && NULL != TimerIdPtr && FALSE == m_bACCELService &&
        FALSE == m_bGYROService && FALSE == m_bMAGService)
    {
      usleep(5000);

      SLIM_LOGD("All Service disabled.");
      timer_delete(*TimerIdPtr);
      SLIM_LOGD("Deleted the Timer");
      *SessionStartedPtr = 0;

      SLIM_LOGD("Signal the FileRead to Close the File ");
    }
  }
  return eSLIM_SUCCESS;
}

/*!
 * @brief Creates the Main Thread which will be sending the sensor data.
 *
 * Method controlls the Sensor PlackBack Data File read and Look Up Buffer for each sensor.
 * Also periodically sends the data to the clients requested.
 *
 */
void SensorPlayBackProvider::threadMain(void *pData)
{
  SensorPlayBackProvider *pzSensorPlayBackProvider =
    reinterpret_cast<SensorPlayBackProvider*>(pData);

  slim_IpcStart(s_qThreadId);
  pzSensorPlayBackProvider->m_qServiceMask = s_qSupportedServices;
  pzSensorPlayBackProvider->routeConfigurationChange(s_qSupportedServices);
  /* Main Thread Function which periodically sends the data to the clients requested */
  pzSensorPlayBackProvider->runEventLoop();
}

/*!
 * @brief Returns provider instance.
 *
 * Method provides access to provider instance. If necessary, the instance
 * is created and initialized.
 *
 * @return Provider instance.
 * @retval 0 On error.
 */
slim_ServiceProviderInterfaceType *SensorPlayBackProvider::getProvider()
{
  SLIM_LOGD("Sensor Playback provider initialization");
  SensorPlayBackProvider ready;
  MutexLock _l(s_zInstanceMutex);
  if (0 == s_pzInstance)
  {
    s_pzInstance = new SensorPlayBackProvider;
    if (!s_pzInstance->initialize())
    {
      SLIM_LOGE("Sensor Playback provider initialization failed;");
      delete s_pzInstance;
      s_pzInstance = 0;
      return 0;
    }
  }
  return s_pzInstance->getSlimInterface();
}

/*!
 * @brief Class constructor
 * Creates and initializes new instance of the class.
 */
  SensorPlayBackProvider::SensorPlayBackProvider()
  : MultiplexingProvider(SLIM_PROVIDER_SENSOR_PLAYBACK, false, 0),
  m_zThread(), m_zFileReadThread(), m_zLock(), DCIInitPtr(), create_timerPtr(),
  SessionStartedPtr(), ValidGpsTimeRxdPtr(), DCIDeInitPtr(), TimerOffsetMutexPtr(), ReadDataPtr(),
  RetrieveData(), TimerIdPtr(), PlaybackTimeMutexPtr(), PlayBackTimerCondPtr(), fPTR(NULL)
{
  void *handle = NULL;
  loc_param_s_type z_sapParameterTable[] =
  {
    { "SENSOR_TYPE", &m_zLibInfo.sensorType, NULL, 'n'},
    { "SENSOR_HAL_LIB_PATH", m_zLibInfo.sensorHalLibPath, &m_zLibInfo.sensorHalLibPathStrSet, 's'}
  };
  /* Read logging configuration and sensor provider */
  UTIL_READ_CONF(LOC_PATH_SAP_CONF, z_sapParameterTable);
  SLIM_LOGD("SPB:%d,%s,%d", m_zLibInfo.sensorType,
      m_zLibInfo.sensorHalLibPath, m_zLibInfo.sensorHalLibPathStrSet );

  if (255 != m_zLibInfo.sensorType)
  {
    return;
  }

  handle = dlopen(m_zLibInfo.sensorHalLibPath, RTLD_NOW);
  if (!handle) {
    SLIM_LOGE("Unable to load senor HAL lib %s: %s\n", m_zLibInfo.sensorHalLibPath, dlerror());
    return;
  }

  getsymptr getptr;
  *(void**)(&getptr) = dlsym(handle, "WrappGetDCIInitPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found GetDCIInitPtr %s\n", dlerror());
    return;
  }
  *(void**)(&DCIInitPtr) = getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetCreateTimerPtr");
  if (!getptr)
    {
    SLIM_LOGE("Function not found create_timerPtr %s\n", dlerror());
    return;
  }
  *(void**)(&create_timerPtr) = getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetSessionStartedPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found SessionStartedPtr %s\n", dlerror());
    return;
  }
  SessionStartedPtr = (VPtr1)getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetGpsTimeRxdPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found ValidGpsTimeRxdPtr %s\n", dlerror());
    return;
  }
  ValidGpsTimeRxdPtr = (VPtr2)getptr();


  *(void**)(&getptr) = dlsym(handle, "WrappGetDCIDeInitPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found GetDCIDeInitPtr %s\n", dlerror());
    return;
  }
  *(void**)(&DCIDeInitPtr) = getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetTimerOffsetMutexPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found TimerOffsetMutexPtr %s\n", dlerror());
    return;
  }
  TimerOffsetMutexPtr = (VPtr3)getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetReadPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found ReadDataPtr %s\n", dlerror());
    return;
  }
  *(void**)(&ReadDataPtr) = getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetRetrievePtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found RetrieveData %s\n", dlerror());
    return;
  }
  *(void**)(&RetrieveData) = getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetTimerIdPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found TimerIdPtr %s\n", dlerror());
    return;
  }
  TimerIdPtr = (VPtr4)getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetPlayBackTimerMutexPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found PlaybackTimeMutexPtr %s\n", dlerror());
    return;
  }
  PlaybackTimeMutexPtr = (VPtr3)getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetPlayBackTimerCondPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found PlayBackTimerCondPtr %s\n", dlerror());
    return;
  }
  PlayBackTimerCondPtr = (VPtr5)getptr();

  *(void**)(&getptr) = dlsym(handle, "WrappGetTimerElapsedPtr");
  if (!getptr)
  {
    SLIM_LOGE("Function not found TimerElapsedPtr %s\n", dlerror());
    return;
  }
  TimerElapsedPtr = (VPtr2)getptr();

  SLIM_LOGD("SPB:%p,%p,%p,%p,%p,%p,%p,%p,%p,%p,%p,%p", DCIInitPtr, SessionStartedPtr,
      ValidGpsTimeRxdPtr, DCIDeInitPtr, TimerOffsetMutexPtr, RetrieveData, TimerIdPtr,
      PlaybackTimeMutexPtr, PlayBackTimerCondPtr, ReadDataPtr, create_timerPtr, TimerElapsedPtr);
  memset(&m_zThread, 0, sizeof(m_zThread));
  memset(&m_zFileReadThread, 0, sizeof(m_zFileReadThread));
  pthread_mutex_init (&m_zPlayBackStartMutex, NULL);
  pthread_cond_init (&m_zPlayBackStartCond, NULL);

  pthread_mutex_init (PlaybackTimeMutexPtr, NULL);
  pthread_cond_init (PlayBackTimerCondPtr, NULL);
  pthread_mutex_init (TimerOffsetMutexPtr, NULL);
  pthread_mutex_lock (&m_zPlayBackStartMutex2);
}

/*!
 * @brief Class destructor
 * Destroys current instance.
 */
SensorPlayBackProvider::~SensorPlayBackProvider()
{
  pthread_mutex_destroy (&m_zPlayBackStartMutex);
  pthread_cond_destroy (&m_zPlayBackStartCond);

  pthread_mutex_destroy (PlaybackTimeMutexPtr);
  pthread_cond_destroy (PlayBackTimerCondPtr);
  pthread_mutex_destroy (TimerOffsetMutexPtr);

  /* Release Diag DCI. It’s better to keep this function unchanged. */
  if (NULL != DCIDeInitPtr)
  {
    (this->*DCIDeInitPtr)(mClient_id);
  }
  bool bResult  = Diag_LSM_DeInit();
  if ( false == bResult )
  {
    SLIM_LOGE("nativeDiagTerminate() failed on Diag_LSM_DeInit()");
  }
}

/*!
 * @brief Class destructor
 * Destroys current instance.
 */
bool SensorPlayBackProvider::initialize()
{
  int err;
  loc_param_s_type z_sapParameterTable[] =
  {
    { "SENSOR_PLAYBACK_FILE", &mqSensorPlayBackFile, NULL, 's' },
    { "TARGET_LATENCY", &m_qTargetLatency, NULL, 'n' },
    { "INJECT_LATENCY", &m_qInjectLatency, NULL, 'n' }
  };
  /* Read logging configuration and sensor provider */
  UTIL_READ_CONF(LOC_PATH_SAP_CONF, z_sapParameterTable);


  if (!slim_ThreadCreate(&m_zThread, threadMain, this, "SensorPlayback"))
  {
    return false;
  }
  slim_TaskReadyWait(THREAD_ID_SENSOR_PLAYBACK);

  if (!slim_ThreadCreate(&m_zFileReadThread, ReadDataPtr, this, "PlaybackRecordFileRead"))
  {
    return eSLIM_ERROR_INTERNAL;
  }
  slim_TaskReadyWait(THREAD_ID_PLAYBACK_FILEREAD);

  SLIM_LOGD("Both the Worker threads are created successfully..\n");

  return true;
}

/*!
 * @brief Main Thread function
 * Responsible to send the Sensor Packets
 */
void SensorPlayBackProvider::runEventLoop()
{
  slim_TaskReadyAck();

  SLIM_LOGD("m_zPlayBackStartMutex handle in runevent - %p, %p, %p, %p",
      &this->m_zPlayBackStartMutex, &m_zPlayBackStartMutex, this, TimerElapsedPtr);
  pthread_mutex_lock (&m_zPlayBackStartMutex);
  pthread_cond_wait (&m_zPlayBackStartCond, &m_zPlayBackStartMutex);
  pthread_mutex_unlock (&m_zPlayBackStartMutex);

  SLIM_LOGD("Enable Sensor Data triggered Going to start File Read");
  static int cnt;
  while (1)
  {
    if (cnt==0)
    {
      SLIM_LOGD("entered timer elapsed");
      cnt++;
    }
    if ( NULL != TimerElapsedPtr && true == (*TimerElapsedPtr) )
    {
      pthread_mutex_lock(PlaybackTimeMutexPtr);
      SLIM_LOGD("locked timer mutex before set to false");
      *TimerElapsedPtr=false;
      pthread_mutex_unlock (PlaybackTimeMutexPtr);

      SLIM_LOGD("Triggered s_zPlayBackTimerCond");

      if (NULL != ValidGpsTimeRxdPtr && NULL != RetrieveData && *ValidGpsTimeRxdPtr)
      {
        SLIM_LOGD("Going to call RetrieveSensorData");
        (this->*RetrieveData)();
      }
    }
  }
}

/**
    @brief Initiates time offset request.

    Function for making the time request. Successful response enable SLIM to
    calculate the offset between modem time and sensor time.
    @param[in] lTxnId Service transaction id.
    @return eSLIM_SUCCESS if time request is made successfully.
*/

slimErrorEnumT SensorPlayBackProvider::getTimeUpdate(int32_t lTxnId)
{
  /*  return MultiplexingProvider::getTimeUpdate(lTxnId); */
  slimErrorEnumT retVal = eSLIM_ERROR_INTERNAL;
  uint64 t_Time = slim_TimeTickGetMilliseconds();
  dispatchTimeResponse(lTxnId, t_Time, t_Time);

  // routeTimeResponse(lTxnId, eSLIM_SUCCESS, R_Time, t_Time);
  SLIM_LOGD("Time update reported:R_Time = %d, t_Time = %d ", t_Time, t_Time);
  retVal = eSLIM_SUCCESS;
  SLIM_LOGD("retVal= %d", retVal);
  return retVal;
}
