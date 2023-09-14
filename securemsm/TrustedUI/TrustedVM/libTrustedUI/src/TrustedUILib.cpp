/*========================================================================
Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
#include "object.h" /* Mink object */
#include <atomic>
#include <cstdint>
#include <unordered_map>
#include <unistd.h>
#include <sys/time.h>

#include "MINKHelper.h"    /* For error codes */
#include "TUIErrorCodes.h" /* For error codes */
#include "TUIInputCallback.h"
#include "TUIInputTypes.h" /* For TUIInputData */
#include "TUIUtils.h"      /* For TUIInputData */
#include "TrustedUILib.h"  /* For error codes */
#include "cp_ion.h"
#include "secure_memset.h"
#include "ITUISessionCallBack.hpp"
#include "ITUISessionCallBack_invoke.hpp"

#define LOG_TAG "VMApp:libTrustedUI"

#define PROFILE_ON

#ifdef PROFILE_ON
#define PROFILE \
      unsigned long long stop;         \
      unsigned long long start;        \
      struct timeval start_time;       \
      unsigned long long delay = 0; \
      struct timeval stop_time;

#define TIME_START                   \
      gettimeofday(&start_time, NULL); \
      start = start_time.tv_usec;

#define TIME_STOP                   \
    gettimeofday(&stop_time, NULL); \
    stop = stop_time.tv_usec;        \
    delay = ((stop_time.tv_sec - start_time.tv_sec) * 1000000) + (stop_time.tv_usec - start_time.tv_usec); \
    TUILOGD("%s::%d- (stop time: %llu micro seconds)", __func__, __LINE__, stop); \
    TUILOGE("%s::%d (time taken = %llu micro seconds)", __func__, __LINE__, delay);
#else
#define PROFILE \
    do {           \
    } while (0)

#define TIME_START \
    do {           \
    } while (0)
#define TIME_STOP \
    do {          \
    } while (0)

#endif

/*
* @brief This class wraps the IPC object along with the 'raw pointer' to the
*        input CBO.The object reference (and hence access to raw ptr)
*         remains valid throughout the lifetime of this wrapper.
*/

class InputCBOHolder {
public:
    InputCBOHolder(Object obj, InputCBHelper* inputCB) {
            mInputCBOobj = obj;
            mInputCBO = inputCB;
            Object_retain(mInputCBOobj);
        }
    ~InputCBOHolder() {
        TUILOGV("~InputCBOHolder() Destructor ++");
        Object_ASSIGN_NULL(mInputCBOobj);
        TUILOGV("~InputCBOHolder() Destructor --");
    }
    InputCBHelper* mInputCBO = nullptr;
private:
    Object mInputCBOobj = Object_NULL;

};


class TUISession : public TUISessionCallBackImplBase {
public:
  TUISession() = default;
  ~TUISession();

  TUIResult init(const TUISessionConfig *config);

  uint32_t id() const { return mId; }

  TUIResult start(TUIDisplayBuffer *initScreenBuffer);

  TUIResult stop();

  TUIResult getInputData(int32_t inputTimeout, TUIInputData *inputData);

  TUIResult getDisplayProperties(TUIDisplayConfig *cfgInfo);

  TUIResult enqueueDisplayBuffer(TUIDisplayBuffer *buffer);

  TUIResult dequeueDisplayBuffer(TUIDisplayBuffer *buffer);

  TUIResult allocateAppBuffer(TUIBufferHandle *buffer);

  TUIResult freeAppBuffer(TUIBufferHandle *buffer);

  int32_t OnError();

private:
  uint32_t mId = 0;
 /* mTUISessionCBO holds TUISession this pointer as the object context,
  * hence we don't need to release the object, it is deleted when the
  * TUISession destructor is called by the TUISessionHolder. */
  Object mTUISessionCBO = Object_NULL;
  //MINK Objects for TUICoreService , SystemSessionObject
  TUICoreService *mTUICoreSvc = nullptr;

  std::shared_ptr<TUISystemSession> mSystemSession = nullptr;
  /* mSystemSessionInvokeObj is needed only for passing to core svc during deleteSession */
  Object mSystemSessionInvokeObj = Object_NULL;


  std::unique_ptr<InputCBOHolder> mInputHelper = nullptr;
  //InputCBHelper* mInputHelper = nullptr;

  struct bufhandle {
    bufhandle() : ionFd(-1), vAddr(nullptr), bufSize(0) {}
    bufhandle(int32_t fd, uint8_t *addr, size_t _size) : ionFd(fd), vAddr(addr),
                                                         bufSize(_size) {}
    int32_t ionFd;
    uint8_t *vAddr;
    size_t bufSize;
  };
  using BufList = std::map<int32_t /*slot*/, bufhandle /*buffer info*/>;
  BufList mDispBufList;
  BufList mAppBufList;
  static uint32_t GetUniqueId() {
    static std::atomic<uint32_t> sId;
    return sId.fetch_add(1);
  }
  void freeAllBuffers(BufList bufList);
};

/* TUICoreService calls OnError through it's callback interface in case
 * of any errors such as h/w errors in display. */
int32_t TUISession::OnError() {
  TUILOGD("%s::%d Aborting TUI Session", __func__, __LINE__);
  mInputHelper->mInputCBO->abort();
  return 0;
}

void TUISession::freeAllBuffers(BufList bufList) {
  int32_t ionFdIdx = -1;
  int32_t ionFd = -1;
  uint8_t *buf = nullptr;

  TUILOGD("%s:%d Release all buffers num:%d", __func__, __LINE__,
      bufList.size());
  for (auto element : bufList) {
    buf = (element.second).vAddr;
    ionFdIdx = element.first;
    if (nullptr != buf && ionFdIdx != -1) {
      ionFd = (element.second).ionFd;
      TUILOGD("%s: unmap on ionFd: %d bufAddr: %p size:%zu", __func__, ionFd,
          buf, (element.second).bufSize);
      cp_unmap(buf, (element.second).bufSize, ionFd);
      TUILOGD("Close the ionFd for this buffer : ionFd: %d", ionFd);
      close(ionFd);
    }
  }
  bufList.clear();
}

TUIResult TUISession::init(const TUISessionConfig *config) {
  TUIResult ret = TUI_OK;

  Object objInput = Object_NULL;
  InputCBHelper *inputCB = nullptr;
  TUICreateConfig coreSvcCfg = {0, 0, 0, 0, 0};

  // Connect to TUI Core Service
  mTUICoreSvc = getTUIService();
  TUI_CHECK_COND(mTUICoreSvc != NULL, TUI_ERR_CORE_SVC_NOT_READY);


  inputCB = new InputCBHelper(-1 /*timeout*/);
  TUI_CHECK_COND(inputCB != nullptr, TUI_ERR_OUT_OF_MEMORY);

  objInput = (Object){ImplBase::invoke, inputCB};

  mInputHelper = std::make_unique<InputCBOHolder>(objInput, inputCB);
  TUI_CHECK_COND(mInputHelper != nullptr, TUI_ERR_OUT_OF_MEMORY);

  /*Create/register a new session with TUI Core Service running in the VM
    If session is created successfully, which means that the display and input
    resoures as needed  by this session, are not in use by any other TUI
    session,
    a Session Object is returned which can be used to run the session later.
    */
  coreSvcCfg.displayId = config->displayId;
  coreSvcCfg.numDisplayBuffers = config->numDisplayBuffers;
  coreSvcCfg.colorFormat = config->colorFormat;
  coreSvcCfg.inputUID = config->inputUID;
  coreSvcCfg.useCaseHeapSize = config->useCaseHeapSize;
  TUILOGD("%s: Setting use case heap size to : [%d]", __func__, coreSvcCfg.useCaseHeapSize);

  /* Create TUISession callback object and pass it to CoreService,
   * which can be used by it to make callbacks to App. */
  mTUISessionCBO = (Object){ImplBase::invoke, this};

  TUI_CHECK((ret = (TUIResult)mTUICoreSvc->createSession(
                 &coreSvcCfg, objInput, mTUISessionCBO, mSystemSessionInvokeObj)) == 0);

  TUI_CHECK_COND(!Object_isNull(mSystemSessionInvokeObj), TUI_ERR_GENERIC_FAILURE);

  mSystemSession =
      std::shared_ptr<TUISystemSession>(new TUISystemSession(mSystemSessionInvokeObj));
  TUI_CHECK_COND(mSystemSession != nullptr, TUI_ERR_OUT_OF_MEMORY);

  // register the session-object with input helper to recieve inputs
  TUI_CHECK_COND(mInputHelper->mInputCBO != nullptr, TUI_ERR_GENERIC_FAILURE);
  mInputHelper->mInputCBO->setSessionObj(mSystemSession);

  TUILOGD("%s: Release my reference to Input CBO", __func__);
  Object_release(objInput);

  mId = GetUniqueId();

end:
  return ret;
}

TUISession::~TUISession() {
  TUILOGD("%s: Release Session obj reference in app", __func__);
  mInputHelper->mInputCBO->releaseSessionObj();
  TUILOGD("%s: Free all buffers with the app ", __func__);
  freeAllBuffers(mDispBufList);
  freeAllBuffers(mAppBufList);

  if (mSystemSession) {
    TUILOGD("%s: Call delete Session on Core Service ", __func__);
    mTUICoreSvc->deleteSession(mSystemSessionInvokeObj);

    TUILOGD("%s: Release reference to SystemSession Object", __func__);
    mSystemSession = nullptr;
  }

  TUILOGD("%s: Release Input helper ", __func__);
  mInputHelper = nullptr;
  if (mTUICoreSvc) {
    releaseTUIService(mTUICoreSvc);
  }
}

TUIResult TUISession::start(TUIDisplayBuffer *initScreenBuffer) {
  TUIResult ret = TUI_OK;
  int32_t err = 0;
  int32_t ionFdIdx = -1;
  int32_t ionFd = -1;
  uint8_t *buf = nullptr;
  displayConfig dispCfg = {0, 0, 0, 0, 0, 0, 0};
  TUIDisplayBuffer *buffer = initScreenBuffer;
  TUIDisplayConfig *cfgInfo = &buffer->config;

  TUI_CHECK_COND(mSystemSession != nullptr, TUI_ERR_CORE_SVC_NOT_READY);

  if (buffer == nullptr) {
    // If tha application passed no init screen , then commit a null
    // frame to the display
    TUI_CHECK_COND((ret = dequeueDisplayBuffer(buffer)) == 0,
                   TUI_ERR_CORE_SVC_NOT_READY);

    secure_memset(buffer->buffHandle.vAddr, 0, buffer->buffHandle.size);
  }

  if (!mDispBufList.empty()) {
    TUILOGD("look up in the queue for buf:%p", buffer->buffHandle.vAddr);
    for (auto element : mDispBufList) {
      buf = (element.second).vAddr;
      if (buf == buffer->buffHandle.vAddr) {
        ionFdIdx = element.first;
        ionFd = (element.second).ionFd;
        break;
      }
    }
  }

  TUI_CHECK_COND((ionFdIdx != -1), TUI_ERR_INVALID_PARAM);
  TUILOGD("%s: unmap on ionFd: %d bufAddr: %p", __func__, ionFd, buf);
  TUI_CHECK_COND((err = cp_unmap(buf, buffer->buffHandle.size, ionFd)) == 0,
                 TUI_ERR_GENERIC_FAILURE);

  TUILOGD("Close the ionFd for this buffer after enqueue is done : ionFd: %d", ionFd);
  close(ionFd);
  mDispBufList.erase(ionFdIdx);

  dispCfg.u32Height = cfgInfo->height;
  dispCfg.u32Width = cfgInfo->width;
  dispCfg.u32Stride = cfgInfo->stride;
  dispCfg.u32ColorFormat = cfgInfo->colorFormat;
  dispCfg.u32OffHeight = cfgInfo->offHeight;
  dispCfg.u32OffWidth = cfgInfo->offWidth;
  dispCfg.u32pixelWidth = 4; // TBD: derive from color format

  // Look up ionFdIdx based on addr
  TUILOGD("%s: Enqueue buffer ionFdIdx: %d", __func__, ionFdIdx);
  // TO-DO: pass init screen to startSession
  TUI_CHECK_COND((err = mSystemSession->startSession(ionFdIdx, &dispCfg)) ==
                     0,
                 TUI_ERR_CORE_SVC_NOT_READY);

end:
  return ret;
}

TUIResult TUISession::stop() {
  TUIResult ret = TUI_OK;
  int32_t err = 0;
  TUI_CHECK_COND(mSystemSession != nullptr, TUI_ERR_CORE_SVC_NOT_READY);
  TUI_CHECK_COND(mInputHelper != nullptr, TUI_ERR_OUT_OF_MEMORY);
  TUI_CHECK_COND(mInputHelper->mInputCBO != nullptr, TUI_ERR_GENERIC_FAILURE);

  TUILOGD("%s: Abort Input Wait", __func__);
  mInputHelper->mInputCBO->abort();

  TUILOGD("%s: Stop System Session", __func__);
  TUI_CHECK_COND((err = mSystemSession->stopSession()) == 0,
                 TUI_ERR_CORE_SVC_NOT_READY);
  TUILOGD("%s--", __func__);
end:
  return ret;
}

TUIResult TUISession::getInputData(int32_t inputTimeout,
                                   TUIInputData *inputData) {
  TUIResult ret = TUI_OK;
  TUITouchInput *fingers = &inputData->data.touch;
  int32_t inputStatus = InputCBHelper::NOTIFY_ERROR;

  TUI_CHECK_COND(inputData != nullptr, TUI_ERR_INVALID_PARAM);
  TUI_CHECK_COND(mInputHelper != nullptr, TUI_ERR_OUT_OF_MEMORY);
  TUI_CHECK_COND(mInputHelper->mInputCBO != nullptr, TUI_ERR_GENERIC_FAILURE);

  inputStatus = mInputHelper->mInputCBO->getInput_b((tsFingerData *)fingers);
  if (inputStatus != InputCBHelper::NOTIFY_OK) {
    TUILOGE("%s: TouchHandler recieved %s .. termination", __func__,
          inputStatus == InputCBHelper::NOTIFY_TIMEOUT
              ? "TIMEOUT"
              : inputStatus == InputCBHelper::NOTIFY_ABORT ? "ABORT" : "ERROR");
    ret = TUI_ERR_INVALID_INPUT;
    goto end;
  }

end:
  TUILOGD("%s--", __func__);
  return ret;
}

TUIResult TUISession::getDisplayProperties(TUIDisplayConfig *cfgInfo) {
  TUIResult ret = TUI_OK;
  int32_t err = 0;
  displayConfig dispCfg = {0, 0, 0, 0, 0, 0, 0};
  TUI_CHECK_COND(cfgInfo != nullptr, TUI_ERR_INVALID_PARAM);

  TUI_CHECK_COND((err = mSystemSession->getDisplayProperties(&dispCfg)) == 0,
                 TUI_ERR_CORE_SVC_NOT_READY);

  cfgInfo->height = dispCfg.u32Height;
  cfgInfo->width = dispCfg.u32Width;
  cfgInfo->stride = dispCfg.u32Stride;

end:
  TUILOGD("%s--", __func__);
  return ret;
}

TUIResult TUISession::enqueueDisplayBuffer(TUIDisplayBuffer *buffer) {
  TUIResult ret = TUI_OK;
  int32_t err = 0;
  int32_t ionFdIdx = -1;
  int32_t ionFd = -1;
  uint8_t *buf = nullptr;
  displayConfig dispCfg = {0, 0, 0, 0, 0, 0, 0};
  TUIDisplayConfig *cfgInfo = &buffer->config;

  TUI_CHECK_COND(buffer != nullptr, TUI_ERR_INVALID_PARAM);

  if (!mDispBufList.empty()) {
    TUILOGD("look up in the queue for buf:%p", buffer->buffHandle.vAddr);
    for (auto element : mDispBufList) {
      buf = (element.second).vAddr;
      if (buf == buffer->buffHandle.vAddr) {
        ionFdIdx = element.first;
        ionFd = (element.second).ionFd;
        break;
      }
    }
  }

  TUI_CHECK_COND((ionFdIdx != -1), TUI_ERR_INVALID_PARAM);

  TUILOGD("%s: unmap on ionFd: %d bufAddr: %p", __func__, ionFd, buf);
  TUI_CHECK_COND((err = cp_unmap(buf, buffer->buffHandle.size, ionFd)) == 0,
          TUI_ERR_GENERIC_FAILURE);
  TUILOGD("Close the ionFd for this buffer : ionFd: %d", ionFd);
  close(ionFd);
  mDispBufList.erase(ionFdIdx);


  dispCfg.u32Height = cfgInfo->height;
  dispCfg.u32Width = cfgInfo->width;
  dispCfg.u32Stride = cfgInfo->stride;
  dispCfg.u32ColorFormat = cfgInfo->colorFormat;
  dispCfg.u32OffHeight = cfgInfo->offHeight;
  dispCfg.u32OffWidth = cfgInfo->offWidth;
  dispCfg.u32pixelWidth = 4; // TBD: derive from color format

  // Look up ionFdIdx based on addr
  TUILOGD("%s: Enqueue buffer ionFdIdx: %d", __func__, ionFdIdx);
  TUI_CHECK_COND((err = mSystemSession->enqueueBuffer(ionFdIdx, &dispCfg)) ==
                     0,
                 TUI_ERR_CORE_SVC_NOT_READY);

end:
  TUILOGD("%s--", __func__);
  return ret;
}

TUIResult TUISession::dequeueDisplayBuffer(TUIDisplayBuffer *buffer) {
  TUIResult ret = TUI_OK;
  Object ionFdObj;
  int32_t ionFdIdx = 0, ionFd = -1, err = 0;
  uint32_t size = 0;
  uint8_t *bufAddr;
  struct bufhandle p;

  TUI_CHECK_COND(buffer != nullptr, TUI_ERR_INVALID_PARAM);
  size = buffer->buffHandle.size;

  TUILOGD("%s: Dequeue Buffer of size:%d", __func__, size);
  TUI_CHECK_COND(
      (err = mSystemSession->dequeueBuffer(ionFdObj, &ionFdIdx, size)) == 0,
      TUI_ERR_NOT_ENOUGH_BUFFERS);

  err = Object_unwrapFd(ionFdObj, &ionFd);
  TUI_CHECK_COND(err == 0, TUI_ERR_GENERIC_FAILURE);

  TUILOGD("%s: map on ionFd: %d ionFdIdx : %d", __func__, ionFd, ionFdIdx);
  TUI_CHECK_COND((err = cp_mmap(&bufAddr, size, ionFd)) == 0,
                 TUI_ERR_GENERIC_FAILURE);

  TUILOGD("%s: bufAddr: %p", __func__, bufAddr);
  (buffer->buffHandle).vAddr = bufAddr;

  // Add/update buf addr in the list
  p = {ionFd, bufAddr, size};
  mDispBufList[ionFdIdx] = p;

end:
  TUILOGD("%s--", __func__);
  return ret;
}

TUIResult TUISession::allocateAppBuffer(TUIBufferHandle *buffer) {
  TUIResult ret = TUI_OK;
  Object ionFdObj;
  int32_t ionFdIdx = 0, ionFd = -1, err = 0;
  uint32_t size = 0;
  uint8_t *bufAddr;
  struct bufhandle p;

  TUI_CHECK_COND(buffer != nullptr, TUI_ERR_INVALID_PARAM);
  size = buffer->size;

  TUILOGD("%s: allocate APP-Buffer of size:%d", __func__, size);
  TUI_CHECK_COND((err = mSystemSession->allocateAppBuffer(size, ionFdObj,
                                                             &ionFdIdx)) == 0,
                 TUI_ERR_NOT_ENOUGH_BUFFERS);

  err = Object_unwrapFd(ionFdObj, &ionFd);
  TUI_CHECK_COND(err == 0, TUI_ERR_GENERIC_FAILURE);

  TUILOGD("%s: map on ionFd: %d ionFdIdx : %d", __func__, ionFd, ionFdIdx);
  TUI_CHECK_COND((err = cp_mmap(&bufAddr, size, ionFd)) == 0,
                 TUI_ERR_GENERIC_FAILURE);

  TUILOGD("%s: bufAddr: 0x%x", __func__, bufAddr);
  buffer->vAddr = bufAddr;

  // Add/update buf addr in the list
  p = {ionFd, bufAddr, size};
  mAppBufList[ionFdIdx] = p;
end:
  TUILOGD("%s--", __func__);
  return ret;
}

TUIResult TUISession::freeAppBuffer(TUIBufferHandle *buffer) {
  TUIResult ret = TUI_OK;
  uint8_t *buf = nullptr;
  int32_t ionFdIdx = -1, ionFd = -1, err = 0;

  TUI_CHECK_COND(buffer != nullptr, TUI_ERR_INVALID_PARAM);

  if (!mAppBufList.empty()) {
    TUILOGD("look up in the queue for buf:%p", buffer->vAddr);
    for (auto element : mAppBufList) {
      buf = (element.second).vAddr;
      if (buf == buffer->vAddr) {
        ionFdIdx = element.first;
        ionFd = (element.second).ionFd;
        break;
      }
    }
  }

  TUI_CHECK_COND((ionFdIdx != -1), TUI_ERR_INVALID_PARAM);

  TUILOGD("%s: unmap on ionFd: %d bufAddr: %p", __func__, ionFd, buf);
  TUI_CHECK_COND((err = cp_unmap(buf, buffer->size, ionFd)) == 0,
          TUI_ERR_GENERIC_FAILURE);
  TUILOGD("Close the ionFd for this buffer : ionFd: %d", ionFd);
  close(ionFd);
  mAppBufList.erase(ionFdIdx);

  TUILOGD("%s: free app-buffer ", __func__);
  TUI_CHECK_COND((err = mSystemSession->freeAppBuffer(ionFdIdx)) == 0,
                 TUI_ERR_GENERIC_FAILURE);

end:
  TUILOGD("%s--", __func__);
  return ret;
}

/**
 * Singleton holding session objects
 */
class TUISessionHolder {
public:
  static TUIResult AddSession(uint32_t sessionId,
                              std::shared_ptr<TUISession> session) {
    if (session == nullptr)
      return TUI_ERR_GENERIC_FAILURE;
    std::lock_guard<std::mutex> lock(sLock);
    if (sSessions.count(sessionId)) {
      return TUI_ERR_GENERIC_FAILURE;
    }
    TUILOGD("%s: Add session: %p, with Id: %d", __func__, session.get(),
          sessionId);
    sSessions[sessionId] = session;
    return TUI_OK;
  }

  static TUIResult RemoveSession(uint32_t sessionId) {
    std::lock_guard<std::mutex> lock(sLock);
    if (sSessions.count(sessionId) == 0) {
      return TUI_ERR_GENERIC_FAILURE;
    }
    sSessions.erase(sessionId);
    TUILOGD("%s: Removed session with Id: %d", __func__, sessionId);
    return TUI_OK;
  }

  static std::shared_ptr<TUISession> RetrieveSession(uint32_t sessionId) {
    std::lock_guard<std::mutex> lock(sLock);
    if (sSessions.count(sessionId) == 0) {
      TUILOGE("%s: No session with sessionId: %d", __func__, sessionId);
      return nullptr;
    }
    return sSessions[sessionId];
  }

private:
  static std::mutex sLock;
  static std::unordered_map<uint32_t, std::shared_ptr<TUISession>> sSessions;
};

std::mutex TUISessionHolder::sLock;
std::unordered_map<uint32_t, std::shared_ptr<TUISession>>
    TUISessionHolder::sSessions;

/**----------------------------------------------------------------------------
 * @title TrustedUILib.cpp
 * @brief Implementation of TrustedUILib APIs used by TUI Applications
 * -----------------------------------------------------------------------------*/

/**----------------------------------------------------------------------------
  @title  TUICreateSession
  @brief  Establishes connection with TUI framework, and allows application
          to acquire and use Display and Input peripherals. On successful
session
          creation, a non-zero sessionId is returned. All other session APIs
          must pass this sessionId as the input.
  @param  in TUISessionConfig config Display and Input configuration
  @param  out uint32_t sessionId session identifier
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUICreateSession(const TUISessionConfig *config,
                           uint32_t *sessionId) {
  TUIResult ret = TUI_OK;
  PROFILE;

  if ((config == nullptr) || (sessionId == nullptr)) {
    return TUI_ERR_INVALID_PARAM;
  }

  TIME_START;
  auto session = std::make_shared<TUISession>();
  TUI_CHECK_COND(session != nullptr, TUI_ERR_OUT_OF_MEMORY);

  ret = session->init(config);
  TUI_CHECK_COND(ret == TUI_OK, ret);

  ret = TUISessionHolder::AddSession(session->id(), session);
  TUI_CHECK_COND(ret == TUI_OK, ret);

  *sessionId = session->id();
end:
  TIME_STOP;
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIDeleteSession
  @brief  Tears down connection with TUI framework
  @param  in uint32_t sessionId
  @return TUIResult
  -----------------------------------------------------------------------------*/
TUIResult TUIDeleteSession(uint32_t sessionId) {
  TUIResult ret = TUI_OK;
  PROFILE;
  TIME_START;
  ret = TUISessionHolder::RemoveSession(sessionId);
  TIME_STOP;
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIStartSession
  @brief  Initiates a Trusted UI session by claiming ownership of Display and
          Input peripherals. On successful return of this method, the
          initScreenBuffer will be displayed on the screen.
          After TUIStartSession() completes successfully, Apps can dequeue
          secure buffers, get user input using TUIGetInputData mehtod
          and render it using TUIEnqueueDisplayBuffer
  @param  in TUIDisplayBuffer* initScreenBuffer
  @param  in uint32_t sessionId
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIStartSession(uint32_t sessionId,
                          TUIDisplayBuffer *initScreenBuffer) {
  TUIResult ret = TUI_OK;
  PROFILE;
  TIME_START;
  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  ret = session->start(initScreenBuffer);
end:
  TIME_STOP;
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIStopSession
  @brief  Terminates a Trusted UI session by releasing ownership of Display and
          Input peripherals
  @param  in uint32_t sessionId
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIStopSession(uint32_t sessionId) {
  TUIResult ret = TUI_OK;
  PROFILE;
  TIME_START;
  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  ret = session->stop();
end:
  TIME_STOP;
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIGetInputData
  @brief  Applications use this method to get the next available input data
  @param  in uint32_t sessionId
  @param  in int32_t inputTimeout time in milliseconds after which input wait
                     will stop
  @param  out TUIInputData inputData
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIGetInputData(uint32_t sessionId, int32_t inputTimeout,
                          TUIInputData *inputData) {
  TUIResult ret = TUI_OK;
  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  return session->getInputData(inputTimeout, inputData);
end:
  return ret;
}

/**----------------------------------------------------------------------------
  @title  TUIGetDisplayProperties
  @brief  Allows applications to get Display configuration for a specific
display
          device
          Note: This method should be called after creating a TUI Session
  @param  out TUIDisplayConfig
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIGetDisplayProperties(uint32_t sessionId,
                                  TUIDisplayConfig *cfgInfo) {
  TUIResult ret = TUI_OK;

  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  return session->getDisplayProperties(cfgInfo);
end:
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIEnqueueDisplayBuffer
  @brief  This method takes a buffer address as input and renders it on the
          Display. It also validates that the buffer is a secure buffer and
          belongs to this session. On successful validation, it is enqueued to
          display.
  @param  in TUIDisplayBuffer buffer to be rendered
  @param  in uint32_t sessionId
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIEnqueueDisplayBuffer(uint32_t sessionId,
                                  TUIDisplayBuffer *buffer) {
  TUIResult ret = TUI_OK;
  PROFILE;
  TIME_START;

  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  ret = session->enqueueDisplayBuffer(buffer);
end:
  TIME_STOP;
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIDequeueDisplayBuffer
  @brief  This method provides a display buffer for applications to compose into
          Default mapping is cached
  @param  in uint32_t sessionId
  @param  out TUIDisplayBuffer buffer dequeued
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIDequeueDisplayBuffer(uint32_t sessionId,
                                  TUIDisplayBuffer *buffer) {
  TUIResult ret = TUI_OK;
  PROFILE;
  TIME_START;
  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  ret = session->dequeueDisplayBuffer(buffer);
end:
  TIME_STOP;
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIAllocateAppBuffer
  @brief  Allocates a secure buffer for the application, Apps can use these
          as scratch buffers during image composition.
          These buffers do not qualify to be used as Display buffers as a input
          to TUIEnqueueDisplayBuffer.
  @param  in TUIBufferHandle* Requested buffer size
  @param  out TUIBufferHandle* Size and address after buffer allocation
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIAllocateAppBuffer(uint32_t sessionId, TUIBufferHandle *buffer) {
  TUIResult ret = TUI_OK;
  PROFILE;
  TIME_START;

  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  ret = session->allocateAppBuffer(buffer);
end:
  TIME_STOP;
  return ret;
};

/**----------------------------------------------------------------------------
  @title  TUIFreeAppBuffer
  @brief  This method frees a App buffer
  @param  in TUIBufferHandle buffer to be freed
  @return TUIResult
-----------------------------------------------------------------------------*/
TUIResult TUIFreeAppBuffer(uint32_t sessionId, TUIBufferHandle *buffer) {
  TUIResult ret = TUI_OK;
  PROFILE;
  TIME_START;

  auto session = TUISessionHolder::RetrieveSession(sessionId);
  TUI_CHECK_COND(session != nullptr, TUI_ERR_INVALID_PARAM);

  ret = session->freeAppBuffer(buffer);
end:
  TIME_STOP;
  return ret;
};
