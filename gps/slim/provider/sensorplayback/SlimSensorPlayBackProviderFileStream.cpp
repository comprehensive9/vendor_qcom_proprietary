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
#include <fstream>
#include <dlfcn.h>
#include <loc_pla.h>
#include <loc_cfg.h>
#include "diag_lsm.h"
#include "diag_lsm_dci.h"
//#include <platform_lib_macros.h>

#include <utils/SystemClock.h>
#include <utils/Timers.h>
#include <signal.h>

//! @addtogroup slim_SensorPlayBackProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Logging module for VNW provider.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_PLAYBACK
#define CLK_FREQ_KHZ 19200
#define MAX_CLKRPT_INFO_LINE_STR_LENGTH 512

using namespace slim;
using namespace std;

const uint32_t SensorPlayBackProvider::s_qFileReadThreadId = THREAD_ID_PLAYBACK_FILEREAD;

pthread_mutex_t SensorPlayBackProvider::s_zGPS2APSSTimerOffsetMutex = PTHREAD_MUTEX_INITIALIZER;

uint8 SensorPlayBackProvider::s_uSession_Started = 0;
long long SensorPlayBackProvider::s_qGPS2APSSTimerOffset = 0;
boolean SensorPlayBackProvider::s_bValidGpsTimeRxd = FALSE;

vector<unsigned int> SensorPlayBackProvider::m_zGpsToQTimerVector;
long SensorPlayBackProvider::mTimer_Interval = 0;
unsigned long SensorPlayBackProvider::Buffer_Size = 300;
unsigned int SensorPlayBackProvider::m_ProcessClkCntr = 0;

pthread_mutex_t SensorPlayBackProvider::s_zPlayBackTimerMutex = PTHREAD_MUTEX_INITIALIZER;
timer_t SensorPlayBackProvider::s_ztimer_id;
pthread_cond_t SensorPlayBackProvider::s_zPlayBackTimerCond = PTHREAD_COND_INITIALIZER;

boolean SensorPlayBackProvider::timerElapsed = FALSE;


extern "C"
{
  extern int diag_has_remote_device(uint16 *remote_mask);
}

/**
  @brief Computes time difference in clock ticks.

  Utility function to return the difference in two timestamps in clock ticks.
  Accounts for any timestamp rollovers due to the limited range of milliseconds
  in uint32_t. (Rolls over ~ every 50 days)

  @param[in] tTs1Ms First timestamp in milliseconds. (Comes first in monotonic time)
  @param[in] tTs2Ms Second timestamp in milliseconds. (Comes second in monotonic time)

  @return Time difference in clock ticks
  */
uint32_t SensorPlayBackProvider::calcMsTimeDiffTick(uint64_t tTs1Ms, uint64_t tTs2Ms)
{
  uint64_t tDiffMs = 0;
  if (tTs1Ms <= tTs2Ms)
  {
    tDiffMs = tTs2Ms - tTs1Ms;
  }
  else
  {
    tDiffMs = std::numeric_limits<uint64_t>::max() - tTs1Ms + tTs2Ms + 1;
  }
  return slim_TimeToClockTicks(tDiffMs);
}

/*!
 * @brief Diag stream register function
 *    Diag_Stream_Config: Set log & event masks
 *
 *    @param[in] client_id      client ID
 *    @return eSLIM_SUCCESS is operation succeeded.
 *
 */
int SensorPlayBackProvider::Diag_Stream_Config(int mClient_id)
{
  uint16_t logIds[8]={0};
  int err;
  int numLogs = 1;
  logIds[0] = 0x1c5e;
  SLIM_LOGD("Setting Log Masks: ");
  for (int index = 0; index < numLogs; index++)
  {
    SLIM_LOGD("0x%X", logIds[index]);
  }
  err = diag_log_stream_config(mClient_id, 1, logIds, numLogs);
  if (err != DIAG_DCI_NO_ERROR)
    SLIM_LOGE(" Error sending SET_LOG_MASK to peripheral, err: %d, errno: %d\n", err, errno);

  return err;
}

/*
 * @brief Time Converter
 *  Convert Time from CDMA Time format to Ms
 *    Start from 1970/1/1 0:0:0
 *    @param[in] High and Low DWORDS
 *    @return Time in Ms.
 *
 */
double SensorPlayBackProvider::CDMATimeToMs(uint32 ts_hi, uint32 ts_low)
{
  double divisor = 65536.0;
  double secs, tmp;

  secs = ts_hi * divisor;
  tmp  = (double) ((uint32)(ts_low >> 16) & (uint32)0x0000FFFF);
  secs += tmp;
  secs *= 1.25E-03;

  tmp  = (double) ((ts_low) & (uint32)0x0000FFFF);
  secs += tmp / (32.0 * 1228800.0) ;

  return ( secs * 1000 );
}

/*
 * @brief Time Converter
 *  Convert Time from CDMA Time format to UTC Ms
 *    Start from 1970/1/1 0:0:0
 *    @param[in] High and Low DWORDS
 *    @return Time in Ms.
 *
 */
double SensorPlayBackProvider::CDMATimeToUTCMs(uint32 ts_hi, uint32 ts_low)
{
  double ms = CDMATimeToMs(ts_hi, ts_low);

  // move to UTC time based 1970/1/1
  ms += 315964800000.0;
  // remove fractional part (ms accuracy is enough )
  ms = (double)((long long)ms);

  return ms;
}

/*!
 * @brief Creates the File Read Thread which will be Reading the sending the sensor data.
 *
 * Method Reads the Sensor PlackBack Data File keep in a Buffer.
 *
 */
void SensorPlayBackProvider::FileReadthreadMain(void *pData)
{
  SensorPlayBackProvider *pzSensorPlayBackProvider =
    reinterpret_cast<SensorPlayBackProvider*>(pData);

  slim_IpcStart(s_qFileReadThreadId);
  SLIM_LOGD(" Going to call runFileReadLoop");
  /* Main File Thread Function which Reads the Sensor PlackBack Data File keep in a Buffer */
  pzSensorPlayBackProvider->runFileReadLoop();
}

/*
 * @brief function to compute GPS Msec to APSS time offset.
 *    @param[in] pRaw      Raw Byte array
 *
 */
void SensorPlayBackProvider::processGPS2APSSClkLog(unsigned char *pRaw)
{

  if (NULL == pRaw || 0 == s_uSession_Started || TRUE == s_bValidGpsTimeRxd)
  {
    return;
  }

  uint32 nGPSMs;
  uint32 tvSec;
  uint32 tvNsec;
  uint32 nSysRtcTime;
  uint32 apTimeStampUncertaintyMs;

  log_hdr_type *pHdr = (log_hdr_type *) pRaw;
  double diagTS = CDMATimeToUTCMs(pHdr->ts_hi, pHdr->ts_lo);
  epDiagGnssImplMeasurementReport *pLog = (epDiagGnssImplMeasurementReport*)(pRaw);
  apTimeStampUncertaintyMs =
    (uint32)pLog->msrReport.msrInfo.lastPpsLocaltimeStamp.apTimeStampUncertaintyMs;
  if ( 1 != pLog->msrReport.msrInfo.gpsSystemTime.validityMask.isSystemWeekMsecValid ||
      1 != apTimeStampUncertaintyMs )
  {
    SLIM_LOGD("Received invalid validityMask");
    pthread_mutex_lock(&s_zGPS2APSSTimerOffsetMutex);
    s_bValidGpsTimeRxd = FALSE;
    pthread_mutex_unlock(&s_zGPS2APSSTimerOffsetMutex);
  }
  else
  {
    tvSec = pLog->msrReport.msrInfo.lastPpsLocaltimeStamp.apTimeStamp.tv_sec;
    tvNsec = pLog->msrReport.msrInfo.lastPpsLocaltimeStamp.apTimeStamp.tv_nsec;
    nGPSMs = pLog->msrReport.msrInfo.gpsSystemTime.systemMsec;
    nGPSMs = (((double)nGPSMs / 1000) + 1);
    nGPSMs *= 1000;
    nSysRtcTime = ((double)tvSec * 1000000000 + tvNsec) / 1000000;
    uint32 qGPS2APSSTimerOffset = nGPSMs - nSysRtcTime;
    // uint32 qGPS2APSSTimerOffset =  236179993 - nSysRtcTime;

    SLIM_LOGD("nGPSMs = %d nSysRtcTime = %d", nGPSMs, nSysRtcTime);
    SLIM_LOGD("qGPS2APSSTimerOffset = %d ", qGPS2APSSTimerOffset);
    pthread_mutex_lock(&s_zGPS2APSSTimerOffsetMutex);
    s_qGPS2APSSTimerOffset =  qGPS2APSSTimerOffset;
    pthread_mutex_unlock(&s_zGPS2APSSTimerOffsetMutex);
    s_bValidGpsTimeRxd = TRUE;

    SLIM_LOGD("Coming to the APSS part");


    int64_t uptimeMs = uptimeMillis();
  }
}

/*
 * @brief Diag call back function for set of Logs
 *  Call back function to Process DCI log stream
 *    @param[in] pRaw      Raw Byte array
 *    @param[in] len      Raw Byte array length
 *
 */
void SensorPlayBackProvider::process_dci_log_stream(unsigned char *ptr, int len)
{
  SLIM_LOGD("SensorPlayBackProvider::process_dci_log_stream");
  if (0 == s_uSession_Started || NULL == ptr)
  {
    SLIM_LOGE("process_dci_log_stream : 0 == s_uSession_Started or ptr NULL - %p", (void*)ptr);
    return;
  }

  log_hdr_type *pHdr = (log_hdr_type *)ptr;
  if (len < (int)sizeof(log_hdr_type))
  {
    SLIM_LOGE("len < (int)sizeof(log_hdr_type So Returning.", len);
    return;
  }
  SLIM_LOGD("process_dci_log_stream :pHdr->code = %u ", pHdr->code);
  if (0x1C5E == pHdr->code)
  {
    epDiagGnssImplMeasurementReport *pLog = (epDiagGnssImplMeasurementReport *)(ptr);
    if (pLog->msrReport.header.u_Version < 3)
    {
      SLIM_LOGE("log version = %u. Expected version = 3", pLog->msrReport.header.u_Version);
      return;
    }

    SLIM_LOGD("process_dci_log_stream: 0x1C5E");

    processGPS2APSSClkLog(ptr);
  }
}

/*
 * @brief Diag call back function for set of events
 *  Call back function to Process DCI events stream
 *    @param[in] pRaw      Raw Byte array
 *    @param[in] len      Raw Byte array length
 *
 */
void SensorPlayBackProvider::process_dci_event_stream(unsigned char *ptr, int len)
{
  if (0 == s_uSession_Started)
  {
    SLIM_LOGD("diag session not started");
    return;
  }

  log_hdr_type *pHdr = (log_hdr_type *)ptr;
  if (len < (int)sizeof(log_hdr_type))
  {
    SLIM_LOGD("len less than header size error - %u", len);
    return;
  }
  return;
}

/*
 * @brief Notify Function Handler
 *  Gets called when there is a change in the channel connection
 *    @param[in] signal ID
 *    @param[in] signal Info
 *
 */
void SensorPlayBackProvider::notify_handler(int signal, siginfo_t *info, void* /*unused*/)
{
  if (info)
  {
    SLIM_LOGD("diag: In %s, signal %d received from kernel", __func__, signal);
  }
  else
  {
    SLIM_LOGD("diag: In %s, signal %d received from kernel, but no info value",
        __func__, signal);
  }
}

extern "C"
{
  void* WrappGetDCIInitPtr()
  {
    return GetDCIInitPtr();
  }
  void* WrappGetCreateTimerPtr()
  {
    return GetCreateTimerPtr();
  }
  void* WrappGetSessionStartedPtr()
  {
    return GetSessionStartedPtr();
  }
  void* WrappGetGpsTimeRxdPtr()
  {
    return GetGpsTimeRxdPtr();
  }
  void* WrappGetTimerElapsedPtr()
  {
    return GetTimerElapsedPtr();
  }
  void* WrappGetDCIDeInitPtr()
  {
    return GetDCIDeInitPtr();
  }
  void* WrappGetTimerOffsetMutexPtr()
  {
    return GetTimerOffsetMutexPtr();
  }
  void* WrappGetReadPtr()
  {
    return GetReadPtr();
  }
  void* WrappGetRetrievePtr()
  {
    return GetRetrievePtr();
  }
  void* WrappGetTimerIdPtr()
  {
    return GetTimerIdPtr();
  }

  void* WrappGetPlayBackTimerMutexPtr()
  {
    return GetPlayBackTimerMutexPtr();
  }

  void* WrappGetPlayBackTimerCondPtr()
  {
    return GetPlayBackTimerCondPtr();
  }

}

void* GetDCIInitPtr()
{
  return (void*)(&SensorPlayBackProvider::Diag_DCI_Init);
}
void* GetCreateTimerPtr()
{
  return (void*)(&SensorPlayBackProvider::create_timer);
}
void* GetSessionStartedPtr()
{
  return (void*)(&SensorPlayBackProvider::s_uSession_Started);
}
void* GetGpsTimeRxdPtr()
{
  return (void*)(&SensorPlayBackProvider::s_bValidGpsTimeRxd);
}
void* GetTimerElapsedPtr()
{
  return (void*)(&SensorPlayBackProvider::timerElapsed);
}
void* GetDCIDeInitPtr()
{
  return (void*)(&SensorPlayBackProvider::Diag_DCI_DeInit);
}
void* GetTimerOffsetMutexPtr()
{
  return (void*)(&SensorPlayBackProvider::s_zGPS2APSSTimerOffsetMutex);
}
void* GetReadPtr()
{
  return (void*)(&SensorPlayBackProvider::FileReadthreadMain);
}
void* GetRetrievePtr()
{
  return (void*)(&SensorPlayBackProvider::RetrieveSensorData);
}

void* GetTimerIdPtr()
{
  return (void*)(&SensorPlayBackProvider::s_ztimer_id);
}

void* GetPlayBackTimerMutexPtr()
{
  return (void*)(&SensorPlayBackProvider::s_zPlayBackTimerMutex);
}

void* GetPlayBackTimerCondPtr()
{
  return (void*)(&SensorPlayBackProvider::s_zPlayBackTimerCond);
}
/*
 * @brief Diag Initializer
 *  Diag Initializing function
 *    @param[out] client_id      DCI client ID
 *    @return eSLIM_SUCCESS is operation succeeded.
 *
 */
slimErrorEnumT SensorPlayBackProvider::Diag_DCI_Init(int *pmClient_id)
{
  if (!Diag_LSM_Init(0))
  {
    SLIM_LOGE("Failed to initialize DIAG.");
    return eSLIM_ERROR_UNSUPPORTED;
  }
  else
  {
    SLIM_LOGE("SUCCESS initialize DIAG.");
  }

  if (NULL == pmClient_id)
  {
    return eSLIM_ERROR_UNSUPPORTED;
  }

  int err;
  int signal_type = SIGCONT;
  diag_dci_peripherals list = DIAG_CON_MPSS | DIAG_CON_APSS | DIAG_CON_LPASS | 7262;
  struct sigaction notify_action;
  sigemptyset(&notify_action.sa_mask);
  notify_action.sa_sigaction = notify_handler;
  /* Use SA_SIGINFO to denote we are expecting data with the signal */
  notify_action.sa_flags = SA_SIGINFO;
  sigaction(signal_type, &notify_action, NULL);

#ifndef _DIAG_NOT_SUPPORT_REMOTE_
  mDci_proc = 0; // DIAG_PROC_MSM
  SLIM_LOGD("Not A Family. Diag check remote device. pmClient_id= %d pmClient_id = %d\
      mDci_proc = %d", (*pmClient_id), mDci_proc);
#else
  mDci_proc = 0;
  SLIM_LOGD("Not Family. Diag no remote device.");
#endif

  SLIM_LOGD("Going for diag_register_dci_client");

  if (!pmClient_id)
  {
    SLIM_LOGD("pmClient_id is NULL ");
  }

  /* Register DCI Client */
  err = diag_register_dci_client(pmClient_id, (diag_dci_peripherals*)&list, mDci_proc,
      (void *)&signal_type);
  if (err != DIAG_DCI_NO_ERROR)
  {
    SLIM_LOGE(" Couldn't register DCI Client, %d mClient_id = %d", err, *pmClient_id);
    return eSLIM_ERROR_UNSUPPORTED;
  }

  SLIM_LOGD("diag_register_dci_client Success Going for diag_register_dci_stream_proc");

  /* Register Log and Event call backs */
  err = diag_register_dci_stream_proc(*pmClient_id, process_dci_log_stream,
      process_dci_event_stream);
  if (err != DIAG_DCI_NO_ERROR)
  {
    SLIM_LOGE("Unable to initialize stream, err: %d", err);
    return eSLIM_ERROR_UNSUPPORTED;
  }

  SLIM_LOGD("diag_register_dci_stream_proc Success Going for Diag_Stream_Config");

  /* config log & event stream */
  err = Diag_Stream_Config(*pmClient_id);
  if ( DIAG_DCI_NO_ERROR != err )
  {
    SLIM_LOGE("Error sending SET_LOG_MASK to peripheral, err: %d", err);
    return eSLIM_ERROR_UNSUPPORTED;
  }

  SLIM_LOGD("Diag_DCI_Init Success.");
  return eSLIM_SUCCESS;
}

/*
 * @brief Diag De-Initializer
 *  Diag De-Initializing function
 *    @param[out] client_id      DCI client ID
 *    @return eSLIM_SUCCESS is operation succeeded.
 *
 */
slimErrorEnumT SensorPlayBackProvider::Diag_DCI_DeInit(int mClient_id)
{
  int err;
  err = diag_disable_all_logs(mClient_id);
  if (err != DIAG_DCI_NO_ERROR)
  {
    SLIM_LOGE("Error clearing all Log Codes, err: %d", err);
    return eSLIM_ERROR_UNSUPPORTED;
  }
  err = diag_disable_all_events(mClient_id);
  if (err != DIAG_DCI_NO_ERROR)
  {
    SLIM_LOGE("Error clearing all Event IDs, err: %d", err);
    return eSLIM_ERROR_UNSUPPORTED;
  }
  err = diag_release_dci_client(&mClient_id);
  if (err != DIAG_DCI_NO_ERROR)
  {
    SLIM_LOGE("Error releasing DCI connection, %d", err);
    return eSLIM_ERROR_UNSUPPORTED;
  }

  return eSLIM_SUCCESS;
}

/*!
 * @brief File Thread function
 * Responsible to fill the Look ahead buffer
 */
void SensorPlayBackProvider::runFileReadLoop()
{
  slim_TaskReadyAck();
  /* Protecting File-Reopen cases */
  if (NULL != fPTR)
    return;

  SLIM_LOGD("Waiting for m_zPlayBackStartCond");
  pthread_mutex_lock (&m_zPlayBackStartMutex2);
  while (1)
  {
    SLIM_LOGD("fptr - %p, %s, %s", fPTR, mqSensorPlayBackFile, strerror(errno));
    fPTR = fopen(mqSensorPlayBackFile, "r");
    m_zSensorDataVector.clear();
    char ReadLine[255];
    int result = -1;

    SLIM_LOGD("fptr - %p, %s, %s", fPTR, mqSensorPlayBackFile, strerror(errno));
    if ( NULL != fPTR )
    {
      SLIM_LOGD("Opened the Data from the file Sensor_Playback_Data.txt");
      do
      {
        fgets ( ReadLine, 255, fPTR );
        SLIM_LOGD("Data from the file:\n%s", ReadLine);
        result = strncmp(ReadLine, "% Column #1   : GpsMsec", 17);
      }while (result !=0 && !feof(fPTR));

      for (int skiplines = 0; skiplines < 7; skiplines++)
      {
        fgets ( ReadLine, 255, fPTR );
      }

      while (1)
      {
        SLIM_LOGD("Inside the While (1) \n");
        unsigned long Count =0;
        unsigned int GpsMsec = 0;
        unsigned int RTCMs = 0;
        unsigned int SensorType = 0;
        double SensorXaxis = 0.0;
        double SensorYaxis = 0.0;
        double SensorZaxis = 0.0;

        while (!feof(fPTR) && fscanf(fPTR, "%d %d %d %lf %lf %lf", &GpsMsec, &RTCMs,
              &SensorType, &SensorXaxis, &SensorYaxis, &SensorZaxis) != NULL)
        {
          SensorData SensorDataObj (GpsMsec);
          SensorDataObj.RTCMs = RTCMs;
          SensorDataObj.SensorType = SensorType;
          SensorDataObj.SensorXaxis = SensorXaxis;
          SensorDataObj.SensorYaxis = SensorYaxis;
          SensorDataObj.SensorZaxis = SensorZaxis;
          m_zSensorDataVector.push_back(SensorDataObj);

          SLIM_LOGD("GpsMsec: %d %d %d %lf %lf %lf", GpsMsec, RTCMs, SensorType,
              SensorXaxis, SensorYaxis, SensorZaxis);

          if (m_bSearchFile && (GpsMsec > m_qKeyGpsMsec))
          {
            SLIM_LOGD("m_bSearchFile && GpsMsec > m_qKeyGpsMsec GpsMsec = %d m_qKeyGpsMsec = %d",
                GpsMsec, m_qKeyGpsMsec);
            SLIM_LOGD(" Last Element in m_zSensorDataVector = %d \n",
                m_zSensorDataVector.back().GpsMsec);
            SLIM_LOGD(" Size of m_zSensorDataVector = %d \n", m_zSensorDataVector.size());

            break;
          }
          else if (!m_bSearchFile && (Buffer_Size == Count))
          {
            SLIM_LOGD("Reached %d Records. Now GpsMsec = %d m_qKeyGpsMsec = %d \n",
                Buffer_Size, GpsMsec, m_qKeyGpsMsec);
            if (!m_zSensorDataVector.empty())
            {
              SLIM_LOGD(" Last Element in m_zSensorDataVector = %d Size of m_zSensorDataVector \
                  = %d", m_zSensorDataVector.back().GpsMsec, m_zSensorDataVector.size());
            }
            break;
          }
          Count++;
        }
        SLIM_LOGD("File read status = %s,%d, %d", strerror(errno), errno, Count);

        if (feof(fPTR))
        {
          SLIM_LOGD("File status = eof");
          if (m_bSearchFile)
          {
            SLIM_LOGD("m_bSearchFile. But GpsMsec is not found in the Entire file\
                m_qKeyGpsMsec = %d", m_qKeyGpsMsec);
            m_bNotFoundInFile = TRUE;
          }
          break;
          SLIM_LOGD("eof");
        }
        else
        {
          // Active Wait on the Buffer Size
          while ( !m_zSensorDataVector.empty()
              && ( m_zSensorDataVector.back().GpsMsec >  m_qKeyGpsMsec )
              && ( m_zSensorDataVector.size() >= Buffer_Size )
              && FALSE == m_bSearchFile )
          {
            usleep( 5000 );
          }

          if (0 == s_uSession_Started)
          {
            SLIM_LOGD(" Triggred m_zPlayBackFileReadCond when s_uSession_Started");
            break;
          }
        }
      }

      fclose ( fPTR );
      SLIM_LOGD(" Closed the File");
    }
    else
    {
      SLIM_LOGE ( "Sensor Playback Record File %s does not exists or not able read",
          mqSensorPlayBackFile);
    }
  }
}

/*!
 * @brief Timer Creation function
 * Responsible to create the Timer for Sensor sampling.
 */
void SensorPlayBackProvider::create_timer(const long& Timer_Interval)
{
  int status;
  struct itimerspec ts;
  struct sigevent se;
  struct itimerspec curr_value;

  /*
   * Set the sigevent structure to cause the signal to be
   * delivered by creating a new thread.
   */
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &s_ztimer_id;
  se.sigev_notify_function = timer_thread;
  se.sigev_notify_attributes = NULL;

  ts.it_value.tv_sec = 1;
  ts.it_value.tv_nsec = 0;
  ts.it_interval.tv_sec = Timer_Interval/1000;
  ts.it_interval.tv_nsec = ((Timer_Interval%1000) * 1000000);
  SLIM_LOGD(" ts.it_interval.tv_sec = %d, ts.it_interval.tv_nsec = %d",
      ts.it_interval.tv_sec, ts.it_interval.tv_nsec);

  status = timer_create(CLOCK_REALTIME, &se, &s_ztimer_id);
  if (status == -1)
    SLIM_LOGE("Create timer Failed");

  status = timer_settime(s_ztimer_id, 0, &ts, 0);
  if (status == -1)
  {
    SLIM_LOGE("Set timer Failed");
  }
  else
  {
    mTimer_Interval = Timer_Interval;
    Buffer_Size = (Timer_Interval * 6);
  }

  // timer_gettime(s_ztimer_id, &curr_value);
}

/*!
 * @brief Timer Creation function
 * Responsible to create the Timer for Sensor sampling.
 */
void SensorPlayBackProvider::set_timer(const long& Timer_Interval)
{
  int status;
  struct itimerspec ts;
  struct itimerspec curr_value;

  ts.it_value.tv_sec = 0;
  ts.it_value.tv_nsec = 1;
  ts.it_interval.tv_sec = Timer_Interval/1000;
  ts.it_interval.tv_nsec = ((Timer_Interval%1000) * 1000000);
  SLIM_LOGD(" ts.it_interval.tv_sec = %d, ts.it_interval.tv_nsec = %d",
      ts.it_interval.tv_sec, ts.it_interval.tv_nsec);

  status = timer_settime(s_ztimer_id, 0, &ts, 0);
  if ( -1 == status )
  {
    SLIM_LOGE("Set timer Failed");
  }
  else
  {
    mTimer_Interval = Timer_Interval;
    Buffer_Size = (Timer_Interval * 6);
  }

  timer_gettime(s_ztimer_id, &curr_value);
  SLIM_LOGD(" After timer_gettime curr_value.it_interval.tv_sec = %d, curr_value.it_interv\
      al.tv_nsec = %d", curr_value.it_interval.tv_sec, curr_value.it_interval.tv_nsec);
}


/*!
 * @brief Timer Thread main function
 * Responsible to signal Main thread to prepare and send sensor data
 */
void SensorPlayBackProvider::timer_thread(union sigval arg)
{
  int status;
  static int counter = 0;
  struct itimerspec curr_value;

  uint64 apss_kerneltime_qt = slim_TimeTickGetMilliseconds();
  SLIM_LOGD("Timer Expired. APSSKT = %u", apss_kerneltime_qt);

  pthread_mutex_lock(&s_zPlayBackTimerMutex);
  SLIM_LOGD("timer = true %p", &timerElapsed);
  timerElapsed=true;
  pthread_mutex_unlock (&s_zPlayBackTimerMutex);
}


/*!
 * @brief Main Thread subroutine function
 * Responsible to fill the SensorData and Invoke the routeIndication Function
 */
void SensorPlayBackProvider::SendSensorData(slimSensorDataStructT& SensorData, uint8 sensorType)
{

  uint64_t tCurrentTimestamp = SensorData.timeBase;
  /* SLIM_LOGD("tCurrentTimestamp = %" PRIu64 " ms. sensor=%d, time=%lld, value=<%f,%f,%f>\n",
     tCurrentTimestamp,
     sensorType,
     tCurrentTimestamp,
     SensorData.samples[0].sample[0],
     SensorData.samples[0].sample[1],
     SensorData.samples[0].sample[2]);
     SensorData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
     SensorData.timeSource = eSLIM_TIME_SOURCE_COMMON;
     SensorData.flags = 0;
     SensorData.sensorType = (slimSensorTypeEnumT)sensorType; */

  switch (sensorType)
  {
    case 0 :
      routeIndication(eSLIM_SERVICE_SENSOR_ACCEL, SensorData);
      break;
    case 1 :
      routeIndication(eSLIM_SERVICE_SENSOR_GYRO, SensorData);
      break;
    case 4 :
      routeIndication(eSLIM_SERVICE_SENSOR_MAG_CALIB, SensorData);
      break;
  }
  memset(&SensorData, 0, sizeof(SensorData));
}

/*!
 * @brief Main Thread subroutine function
 * Responsible to get sensor data from the Look ahead buffer
 */
void SensorPlayBackProvider::RetrieveSensorData()
{
  uint64 apss_kerneltime_qt = slim_TimeTickGetMilliseconds();

  memset(&m_zAccSensorData, 0, sizeof(m_zAccSensorData));
  memset(&m_zGyroSensorData, 0, sizeof(m_zGyroSensorData));
  memset(&m_zMagSensorData, 0, sizeof(m_zMagSensorData));

  uint64 t_Time = slim_TimeTickGetMilliseconds();

  //SLIM_LOGD("APSS t_Time = %d", t_Time);

  pthread_mutex_lock(&s_zGPS2APSSTimerOffsetMutex);
  long long GPS2APSSTimerOffset = s_qGPS2APSSTimerOffset;
  pthread_mutex_unlock(&s_zGPS2APSSTimerOffsetMutex);


  // SPB_TBD
  // unsigned int CurrentGpsMsec = GPS2QTimerOffset + curQTimerMs + m_qInjectLatency
  // - m_qTargetLatency;
  // unsigned long long CurrentGpsMsec = GPS2QTimerOffset + curQTimerMs;
  unsigned long long CurrentGpsMsec = GPS2APSSTimerOffset + t_Time;
  // unsigned long long APSS2QTimerOffset = t_Time - curQTimerMs;

  int64_t uptimeMs = uptimeMillis();
  SLIM_LOGD("GPS2APSSTimerOffset= %" PRIu64 ", CurrentGpsMsec = %" PRIu64 " uptimeMs = %"
      PRId64 " t_Time = %" PRId64 " ", GPS2APSSTimerOffset, CurrentGpsMsec, uptimeMs, t_Time);
  m_qKeyGpsMsec = CurrentGpsMsec;

  if ( (FALSE == m_bNotFoundInFile) && (m_zSensorDataVector.empty() ||
        (!m_zSensorDataVector.empty() && (m_zSensorDataVector.back().GpsMsec < CurrentGpsMsec))))
  {
    m_bSearchFile = TRUE;
    SLIM_LOGE("m_bSearchFile Or Re-Read \n CurrentGpsMsec = %" PRIu64 " m_qKeyGpsMsec = %"
        PRIu64 "", CurrentGpsMsec, m_qKeyGpsMsec);

    while ( m_zSensorDataVector.back( ).GpsMsec < CurrentGpsMsec )
    {
      usleep( 5 );
    }
    m_bSearchFile = FALSE;
  }

  bool FoundLast = false;
  std::vector<SensorData>::const_iterator start;

  if (!m_zSensorDataVector.empty())
  {
    SLIM_LOGD("Last element in the Vector = %" PRIu64 "", m_zSensorDataVector.back().GpsMsec);
  }

  if (m_bSearchLowerLimit)
  {
    start = std::lower_bound( m_zSensorDataVector.begin(), m_zSensorDataVector.end(),
        CurrentGpsMsec - mTimer_Interval);
    SLIM_LOGD("Found the FIRST Iterator at (CurrentGpsMsec - %d) = %" PRIu64 " ",
        mTimer_Interval, CurrentGpsMsec - mTimer_Interval);
  }
  else
  {
    start = m_zSensorDataVector.begin();
  }


  uint64 apss_kerneltime_qtEnd = slim_TimeTickGetMilliseconds();
  // SLIM_LOGD("Time to find Fisrt = %d", (apss_kerneltime_qtEnd - apss_kerneltime_qt));


  std::vector<SensorData>::const_iterator last = std::lower_bound( m_zSensorDataVector.begin(),
      m_zSensorDataVector.end(), CurrentGpsMsec);
  if (last == m_zSensorDataVector.end())
  {
    SLIM_LOGE("Not Found the LAST Iterator at CurrentGpsMsec = %" PRIu64 " ", CurrentGpsMsec);
  }
  else
  {
    FoundLast = true;
  }

  apss_kerneltime_qtEnd = slim_TimeTickGetMilliseconds();
  SLIM_LOGE("Time to find Last = %d", (apss_kerneltime_qtEnd - apss_kerneltime_qt));

  if (FoundLast && (start != last))
  {
    auto it = start;
    for ( ; ; ++it)
    {
      if ((*it).GpsMsec > CurrentGpsMsec || it > last)
      {
        if (TRUE == m_bACCELService && m_zAccSensorData.samples_len > 0 &&
            m_zAccSensorData.samples_len < SLIM_SENSOR_MAX_SAMPLE_SETS)
        {
          // m_zAccSensorData.timeBase += APSS2QTimerOffset;
          SLIM_LOGD("Final m_zAccSensorData.timeBase = %d \n", m_zAccSensorData.timeBase);
          SLIM_LOGD("Last ACCEL Sample %lf %lf %lf \n",
              m_zAccSensorData.samples[m_zAccSensorData.samples_len-1].sample[0],
              m_zAccSensorData.samples[m_zAccSensorData.samples_len-1].sample[1],
              m_zAccSensorData.samples[m_zAccSensorData.samples_len-1].sample[2]);
          apss_kerneltime_qtEnd = slim_TimeTickGetMilliseconds();

          SendSensorData(m_zAccSensorData, 0);
        }

        if (TRUE == m_bGYROService && m_zGyroSensorData.samples_len > 0 &&
            m_zGyroSensorData.samples_len < SLIM_SENSOR_MAX_SAMPLE_SETS)
        {
          // m_zGyroSensorData.timeBase += APSS2QTimerOffset;
          SLIM_LOGD("Final m_zGyroSensorData.timeBase = %d \n", m_zGyroSensorData.timeBase);
          SLIM_LOGD("Last GYRO Sample %lf %lf %lf \n",
              m_zGyroSensorData.samples[m_zGyroSensorData.samples_len-1].sample[0],
              m_zGyroSensorData.samples[m_zGyroSensorData.samples_len-1].sample[1],
              m_zGyroSensorData.samples[m_zGyroSensorData.samples_len-1].sample[2]);

          SendSensorData(m_zGyroSensorData, 1);
        }

        if (TRUE == m_bMAGService && m_zMagSensorData.samples_len > 0 &&
            m_zMagSensorData.samples_len < SLIM_SENSOR_MAX_SAMPLE_SETS)
        {
          SLIM_LOGD("Last MAGNETO Sample %d %lf %lf %lf \n", m_zMagSensorData.timeBase +
              m_zMagSensorData.samples[m_zMagSensorData.samples_len-1].sampleTimeOffsetMs,
              m_zMagSensorData.samples[m_zMagSensorData.samples_len-1].sample[0],
              m_zMagSensorData.samples[m_zMagSensorData.samples_len-1].sample[1],
              m_zMagSensorData.samples[m_zMagSensorData.samples_len-1].sample[2]);
          SendSensorData(m_zMagSensorData, 4);
        }
        break;
      }

      SensorData SensorDataObj = *it;

      if (TRUE == m_bACCELService && 0 == SensorDataObj.SensorType)
      {
        m_zAccSensorData.samples[m_zAccSensorData.samples_len].sample[0] =
          SensorDataObj.SensorXaxis;
        m_zAccSensorData.samples[m_zAccSensorData.samples_len].sample[1] =
          SensorDataObj.SensorYaxis;
        m_zAccSensorData.samples[m_zAccSensorData.samples_len].sample[2] =
          SensorDataObj.SensorZaxis;

        if (0 == m_zAccSensorData.samples_len)
        {
          m_zAccSensorData.timeBase = t_Time - ( CurrentGpsMsec - SensorDataObj.GpsMsec);
          SLIM_LOGD("First ACCEL Sample GpsMsec = %d timeBase = %d %d %d %lf %lf %lf \n",
              SensorDataObj.GpsMsec, m_zAccSensorData.timeBase, SensorDataObj.RTCMs,
              SensorDataObj.SensorType, SensorDataObj.SensorXaxis, SensorDataObj.SensorYaxis,
              SensorDataObj.SensorZaxis);
        }

        m_zAccSensorData.samples[m_zAccSensorData.samples_len].sampleTimeOffsetMs =
          (SensorDataObj.GpsMsec - GPS2APSSTimerOffset - m_zAccSensorData.timeBase);
        m_zAccSensorData.samples_len++;

        if (m_zAccSensorData.samples_len == 50)
        {
          // m_zAccSensorData.timeBase += APSS2QTimerOffset;
          SendSensorData(m_zAccSensorData, 0);
        }
      }
      else if (TRUE == m_bGYROService && 1 == SensorDataObj.SensorType)
      {
        m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sample[0] =
          SensorDataObj.SensorXaxis;
        m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sample[1] =
          SensorDataObj.SensorYaxis;
        m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sample[2] =
          SensorDataObj.SensorZaxis;
        if (0 == m_zGyroSensorData.samples_len)
        {
          m_zGyroSensorData.timeBase = t_Time - ( CurrentGpsMsec - SensorDataObj.GpsMsec);
        }

        m_zGyroSensorData.samples[m_zGyroSensorData.samples_len].sampleTimeOffsetMs =
          (SensorDataObj.GpsMsec - GPS2APSSTimerOffset - m_zGyroSensorData.timeBase);
        m_zGyroSensorData.samples_len++;

        if (m_zGyroSensorData.samples_len == 50)
        {
          //m_zGyroSensorData.timeBase += APSS2QTimerOffset;
          SendSensorData(m_zGyroSensorData, 1);
        }
      }
      else if (TRUE == m_bMAGService && 4 == SensorDataObj.SensorType)
      {
        m_zMagSensorData.samples[m_zMagSensorData.samples_len].sample[0] =
          SensorDataObj.SensorXaxis;
        m_zMagSensorData.samples[m_zMagSensorData.samples_len].sample[1] =
          SensorDataObj.SensorYaxis;
        m_zMagSensorData.samples[m_zMagSensorData.samples_len].sample[2] =
          SensorDataObj.SensorZaxis;
        if (0 == m_zMagSensorData.samples_len)
        {
          m_zMagSensorData.timeBase = t_Time - ( CurrentGpsMsec - SensorDataObj.GpsMsec);
        }
        m_zMagSensorData.samples[m_zMagSensorData.samples_len].sampleTimeOffsetMs =
          (SensorDataObj.GpsMsec - GPS2APSSTimerOffset - m_zMagSensorData.timeBase);
        m_zMagSensorData.samples_len++;

        if (m_zMagSensorData.samples_len == 50)
        {
          SendSensorData(m_zMagSensorData, 4);
        }
      }
    }

    //  SLIM_LOGD("Before Erase m_zSensorDataVector Length = %d", m_zSensorDataVector.size());
    m_zSensorDataVector.erase(m_zSensorDataVector.begin(), it);
    m_bSearchLowerLimit = FALSE;

    apss_kerneltime_qtEnd = slim_TimeTickGetMilliseconds();
    SLIM_LOGD("Time Taken For RetrieveSensorData = %d",
        (apss_kerneltime_qtEnd - apss_kerneltime_qt));

  }
  else
  {
    SLIM_LOGE("CurrentGpsMsec = %d is NOT Found in the Sensor Record or It is at the Lower limit",
        CurrentGpsMsec);
  }
}
