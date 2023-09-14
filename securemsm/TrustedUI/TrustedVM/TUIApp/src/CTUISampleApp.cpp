/*========================================================================
  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#include <inttypes.h>
#include <stdint.h>
#include <mutex>
#include <string>
#include <thread>
#include <fcntl.h>
#include <unistd.h>

#include "object.h"
#include "proxy_base.hpp"
#include "impl_base.hpp"
#include "CTrustedUIApp.hpp"
#include "IHLOSListenerCBO.hpp"
#include "ITUICoreService.hpp"
#include "ITrustedUIApp_invoke.hpp"
#include "ITuiComposer.h"
#include "TUIErrorCodes.h"
#include "TUILog.h"
#include "TUIUtils.h"
#include "TrustedUILib.h"
#include "fdwrapper.h"
#include "memscpy.h"
#include "secure_memset.h"
#define LOG_TAG "VM_CTrustedUIApp"

#define ENABLE_LM

#define MAX_SESSIONS 1
#define NUM_TUI_SD_BUFFERS 3

#define TRUSTEDUIVM_CMD_OFFSET 0x03000000
#define TUI_CMD_SEND_PING      ( 1 | TRUSTEDUIVM_CMD_OFFSET )
#define TUI_CMD_RECV_PING 17
#define TUI_CMD_SEND_MSG 18

#define TUI_LOCK_STR "tuiapp"

#define TUI_DISPLAY_BUFFERS 2
#define DEFAULT_WIDTH 1440
#define DEFAULT_HEIGHT 2880
#define BYTES_PER_PIXEL 4

//#define ENABLE_LM
#define TIME_STOP \
    do {          \
    } while (0)
#define TIME_START \
    do {           \
    } while (0)

#define ENTER   \
    TIME_START; \
    TUILOGE("%s:++", __func__);
#define EXIT   \
    TIME_STOP; \
    TUILOGE("%s:--", __func__);

Object appObj = Object_NULL;

class CTrustedUIApp : public TrustedUIAppImplBase
{
   public:
    enum State : uint32_t {
        UNINITIALIZED,        // TA not loaded
        INITIALIZED,          // TA loaded, Events/resources registered
        HANDLING_INPUT,       // Input handling (processSecureInput) ongoing
        HANDLING_INPUT_DONE,  // Input handling done
        ABORTING              // Session is aborting
    };
    CTrustedUIApp();
    virtual ~CTrustedUIApp()
    {
        TUILOGD("%s: Destructor: destryoing app Object %p ", __func__, this);
    }

    virtual int32_t createSession(uint32_t displayId_val,
                                  uint32_t inputMethod_val, Object HLOSCbo_val);
    virtual int32_t startSession(const TUIConfig *TUIconf_ptr);
    virtual int32_t stopSession();
    virtual int32_t deleteSession();
    virtual int32_t handleCmd(const void *cmd_ptr, size_t cmd_len,
                              void *response_ptr, size_t response_len,
                              size_t *response_lenout);
    static tuiComposerStatus_t tuiComposerCBAllocHelperBuf(
        void *pTUIAppContext, tuiComposerBufferHandle_t *handle);
    static tuiComposerStatus_t tuiComposerCBFreeHelperBuf(
        void *pTUIAppContext, tuiComposerBufferHandle_t *handle);
    static int32_t tuiComposerCBHandleCmd(
        const void* cmd_ptr, size_t cmd_len,
        void* rsp_ptr, size_t rsp_len, size_t* rsp_lenout);

    //Listener Object from TUI HAL
    static HLOSListenerCBO *mHLOSCBORef;

   private:
    int refs;
    uint32_t mSessionId = UINT32_MAX;
    State mState = UNINITIALIZED;
    // Size of Display Buffers
    uint64_t mSDBufSize;
    TUIDisplayConfig mDispCfg;

    std::mutex mLock;
    std::shared_ptr<std::thread> mTouchThread = nullptr;
    void _touchThreadHandler();
    void _stopTouchThreadIfRunning();
    int32_t _acquireWakeLock();
    int32_t _releaseWakeLock();

    //Composer defines
    std::string mLayoutName;
    tuiComposerInput_t mInput;
    ITuiComposer_t *mComposer = nullptr;
    ITuiComposerCallback_t *mComposerCB = nullptr;
    int32_t _validateDisplayBuf(tuiComposerDisplayBuffer_t *pDispBuf,
                                TUIDisplayBuffer *pBuf) const;
    int32_t _cleanUpSession();
    void _cleanUpComposer();

    const char *ToString(State state);
};

HLOSListenerCBO* CTrustedUIApp::mHLOSCBORef = nullptr;

/*=============================================================*/

#ifdef __cplusplus
extern "C" {
#endif

tuiComposerStatus_t CTrustedUIApp::tuiComposerCBAllocHelperBuf(
    void *pTUIAppContext, tuiComposerBufferHandle_t *pHandle)
{
    tuiComposerStatus_t ret = TUI_COMPOSER_STATUS_OK;
    TUIResult res = TUI_OK;
    CTrustedUIApp *pSelf = (CTrustedUIApp *)pTUIAppContext;

    TUI_CHECK_COND(
        (res = TUIAllocateAppBuffer(pSelf->mSessionId,
                                    (TUIBufferHandle *)pHandle)) == TUI_OK ||
            pHandle->vAddr != nullptr,
        TUI_COMPOSER_STATUS_NO_MEM);

    TUILOGD("%s: allocated helper buffer: 0x%x for size: 0x%x\n", __func__,
            pHandle->vAddr, pHandle->size);
end:
    return ret;
}

tuiComposerStatus_t CTrustedUIApp::tuiComposerCBFreeHelperBuf(
    void *pTUIAppContext, tuiComposerBufferHandle_t *pHandle)
{
    tuiComposerStatus_t ret = TUI_COMPOSER_STATUS_OK;
    TUIResult res = TUI_OK;
    CTrustedUIApp *pSelf = (CTrustedUIApp *)pTUIAppContext;

    TUI_CHECK_COND((pHandle != nullptr), TUI_COMPOSER_STATUS_BAD_PARAM);
    TUI_CHECK_COND((pHandle->vAddr != nullptr) || (pHandle->size > 0),
                   TUI_COMPOSER_STATUS_BAD_PARAM);
    TUI_CHECK_COND((pHandle->vAddr + pHandle->size >= pHandle->vAddr),
                   TUI_COMPOSER_STATUS_BAD_PARAM);

    TUI_CHECK_COND(
        (res = TUIFreeAppBuffer(pSelf->mSessionId,
                                (TUIBufferHandle *)pHandle)) == TUI_OK,
        TUI_COMPOSER_STATUS_FAILED);

    pHandle->size = 0;
    pHandle->vAddr = nullptr;

    TUILOGD("%s: allocated helper buffer: 0x%x for size: 0x%x\n", __func__,
            pHandle->vAddr, pHandle->size);
end:
    return ret;
}


int32_t CTrustedUIApp::tuiComposerCBHandleCmd(
    const void* cmd_ptr, size_t cmd_len,
    void* rsp_ptr, size_t rsp_len, size_t* rsp_lenout)
{
    int32_t ret = 0;

    if (!cmd_ptr || cmd_len == 0
         || !rsp_ptr || rsp_len == 0
         || !rsp_lenout) {
        TUILOGE("%s--: Invalid parameter.", __func__);
        return -1;
    }

    if (mHLOSCBORef != nullptr) {
        ret = mHLOSCBORef->handleCmd(cmd_ptr, cmd_len, rsp_ptr, rsp_len, rsp_lenout);
    } else {
        TUILOGE("%s--: can't call HLOS via HLOSCBO, as mHLOSCBORef is NULL.", __func__);
        ret = -1;
    }

    return ret;
}

#ifdef __cplusplus
}
#endif

/*=============================================================*/
CTrustedUIApp::CTrustedUIApp() : refs(1)
{
    TUILOGD("Creating app Object %p ", this);
}

/*=============================================================*/
int32_t _calculateBufSize(uint32_t height, uint32_t width, uint32_t *bufSize)
{
    int32_t ret = 0;
    uint32_t tmpSize = 0;
    *bufSize = 0;
    tmpSize = width * height;
    if (tmpSize / width != height) {
        TUILOGE("%s: overflow detected sxh (%d, %d)", __func__,
                width, height);
        ret = -1;
        goto end;
    }
    *bufSize = tmpSize;
    tmpSize *= BYTES_PER_PIXEL; //TODO : Need to compute PixelWidth
    if (tmpSize / 4 != *bufSize) {
        TUILOGE("%s: overflow detected rxd (%d, %d)", __func__,
                *bufSize, BYTES_PER_PIXEL);
        ret = -1;
        goto end;
    }
    *bufSize = tmpSize;
end:
    return ret;

}

/*=============================================================*/
int32_t CTrustedUIApp::_validateDisplayBuf(tuiComposerDisplayBuffer_t *pDispBuf,
                                           TUIDisplayBuffer *pBuf) const
{
    int32_t ret = 0;
    /*
     Display buffer size = stride * height * pixelwidth
     Note: stride is nothing but (width + padding)
     Pixelwidth is the depth of the pixel i.e. number of bits per pixel
    */
    TUI_CHECK_COND((_calculateBufSize(pDispBuf->config.height, pDispBuf->config.stride,
            &pDispBuf->handle.size) == 0), -1);

    if (pDispBuf->handle.size != pBuf->buffHandle.size) {
        TUILOGE("%s: size mismatch (%d, %d)", __func__, pDispBuf->handle.size,
                pBuf->buffHandle.size);
        ret = -1;
        goto end;
    }
    TUILOGD("%s: size  (%d, %d)", __func__, pDispBuf->handle.size,
            pBuf->buffHandle.size);

end:
    return ret;
}

/* Clean up Display and TouchInput after a session completes. */
int32_t CTrustedUIApp::_cleanUpSession()
{
    int32_t ret = Object_OK;
    TUIResult res = TUI_OK;
    TUI_CHECK_COND((res = TUIStopSession(mSessionId)) == 0, (int32_t)res);
end:
    return ret;
}

/* Clean up Composer only during stop session. */
void CTrustedUIApp::_cleanUpComposer()
{
    if (mComposer) {
        TUILOGE("%s::%d Free Composer", __func__, __LINE__);
        /* There is no need to call Composer deInit because it is anyways
         * called within destroyTUIComposer. */
        destroyTUIComposer(mComposer);
        mComposer = nullptr;
    }
    if (mComposerCB) {
        TUILOGE("%s::%d Free Composer Callback", __func__, __LINE__);
        delete mComposerCB;
        mComposerCB = nullptr;
    }
}

/*=============================================================*/
int32_t CTrustedUIApp::createSession(uint32_t displayId_val,
                                     uint32_t inputMethod_val,
                                     Object HLOSCbo_val)
{
    ENTER;
    int32_t ret = Object_OK;
    uint32_t bufSize = 0;
    int32_t numSDBuffers = TUI_DISPLAY_BUFFERS;
    int32_t useCaseHeapSize = 0;
    TUIResult res = TUI_OK;
    TUISessionConfig cfg = {displayId_val, numSDBuffers,
                            1 /*colorFormat*/, inputMethod_val, 0, HLOSCbo_val};
    std::lock_guard<std::mutex> l(mLock);
    TUI_CHECK_COND(mState == UNINITIALIZED, TUI_ERR_INVALID_STATE);

    TUI_CHECK_COND(Object_isNull(HLOSCbo_val) == 0, TUI_ERR_INVALID_PARAM);
    Object_retain(HLOSCbo_val);

    mHLOSCBORef = new HLOSListenerCBO(HLOSCbo_val);
    TUI_CHECK(mHLOSCBORef != nullptr);

    TUI_CHECK_COND((ret = _acquireWakeLock()) == 0, -1);

    TUI_CHECK_COND(_calculateBufSize(DEFAULT_HEIGHT, DEFAULT_WIDTH, &bufSize) == 0,
            TUI_ERR_GENERIC_FAILURE);
    cfg.useCaseHeapSize = bufSize * (numSDBuffers + 2);
    TUI_CHECK_COND(cfg.useCaseHeapSize > 0, TUI_ERR_GENERIC_FAILURE);

    // Create a session with TUI Core Framework
    TUI_CHECK_COND((res = TUICreateSession(&cfg, &mSessionId)) == TUI_OK,
                   TUI_ERR_CORE_SVC_NOT_READY);

    mState = INITIALIZED;
    TUILOGE("%s::%d State changed to %s", __func__, __LINE__, ToString(mState));
end:
    // clean-up
    if (ret) {
        TUILOGE("%s: Error in creating session", __func__);
        if (mHLOSCBORef) {
            TUILOGE("%s: Delete my reference to listener", __func__);
            delete mHLOSCBORef;
        }
    }
    EXIT;
    return ret;
}

int32_t CTrustedUIApp::_acquireWakeLock() {
    int32_t ret = 0;
    const char * lockFile = "/sys/power/wake_lock";
    int lockFd = -1;

    errno = 0;
    lockFd = open(lockFile, O_WRONLY | O_APPEND);
    if (lockFd < 0) {
        TUILOGE("%s: Unable to open wake lock file: %s", __func__, strerror(errno));
        ret = -1;
        goto end;
    }

    ret = write(lockFd, TUI_LOCK_STR, strlen(TUI_LOCK_STR));
    if (ret < 0) {
        TUILOGE("%s: Unable to write to wake lock file: %s", __func__, strerror(errno));
        ret =  -1;
        goto end;
    }

    ret = 0;
    TUILOGD("%s wake-lock acquired successfully", __func__);

end:
    if (lockFd > 0) {
        close(lockFd);
    }
    return ret;
}

int32_t CTrustedUIApp::_releaseWakeLock() {
    int32_t ret = 0;
    const char * unlockFile = "/sys/power/wake_unlock";
    int unlockFd = -1;

    errno = 0;
    unlockFd = open(unlockFile, O_WRONLY | O_APPEND);
    if (unlockFd < 0) {
        TUILOGE("%s: Unable to open wake unlock file: %s", __func__, strerror(errno));
        ret = -1;
        goto end;
    }

    ret = write(unlockFd, TUI_LOCK_STR, strlen(TUI_LOCK_STR));
    if (ret < 0) {
        TUILOGE("%s: Unable to write to wake unlock file: %s", __func__, strerror(errno));
        ret = -1;
        goto end;
    }

    TUILOGD("%s wake-lock released successfully", __func__);

end:
    if (unlockFd > 0) {
        close(unlockFd);
    }
    return ret;
}

void CTrustedUIApp::_touchThreadHandler()
{
    int32_t ret = Object_OK;
    int32_t retVal = Object_OK;
    TUIResult res = TUI_OK;
    TUIInputData inputData;
    TUIDisplayBuffer buffer;
    TUIDisplayConfig *bufferCfg = &(buffer.config);
    tuiComposerDisplayBuffer_t dispBuf;

    // Continue to process touch events
    while ((mInput.status == TUI_COMPOSER_INPUT_STATUS_CONTINUE) &&
           (ret == 0)) {
        TUI_CHECK_COND((res = TUIGetInputData(mSessionId, mInput.timeOut,
                                              &inputData)) == TUI_OK,
                       (int32_t)res);

        std::lock_guard<std::mutex> l(mLock);
        //Check if the GetInput call exited due to ABORT scenario
        if (mState == ABORTING)
            break;

        buffer.buffHandle.size = mSDBufSize;
        TUILOGD("Requesting  buffer of size : %d", mSDBufSize);
        TUI_CHECK_COND(
            (res = TUIDequeueDisplayBuffer(mSessionId, &buffer)) == TUI_OK,
            (int32_t)res);

        TUILOGD("%s: bufAddr:%p, bufSize:%d", __func__, buffer.buffHandle.vAddr,
                mSDBufSize);

        dispBuf.config.width = mDispCfg.width;
        dispBuf.config.stride = mDispCfg.width;
        dispBuf.config.height = mDispCfg.height;
        dispBuf.config.format = TUI_COMPOSER_FORMAT_RGBA;
        dispBuf.handle.vAddr = buffer.buffHandle.vAddr;

        bufferCfg->width = mDispCfg.width;
        bufferCfg->height = mDispCfg.height;
        bufferCfg->stride = mDispCfg.width;
        bufferCfg->colorFormat = 1;
        bufferCfg->offHeight = 0;
        bufferCfg->offWidth = 0;

        ret = _validateDisplayBuf(&dispBuf, &buffer);
        TUI_CHECK_COND(ret == 0, TUI_ERR_GENERIC_FAILURE);

        mInput.cmd = TUI_COMPOSER_INPUT_CMD_DATA;
        TUILOGD("%s : %d - sizeof(inputData.data.touch) -  %d", __func__,
                __LINE__, sizeof(inputData.data.touch));
        secure_memset(&mInput.data.touch, 0, sizeof(TUITouchInput));
        memscpy((void *)&mInput.data.touch, sizeof(TUITouchInput),
                (void *)&inputData.data.touch, sizeof(inputData.data.touch));

        ret = mComposer->compose(mComposer->ctx, &mInput, &dispBuf);
        TUI_CHECK_COND(ret == TUI_COMPOSER_STATUS_OK, ret);

        TUILOGD("%s: mInput.status :%d", __func__, mInput.status);
        if (TUI_COMPOSER_INPUT_STATUS_COMPLETE == mInput.status )
            TUILOGD("%s: mInput.status TUI_COMPOSER_INPUT_STATUS_COMPLETE ", __func__);
        else if ( TUI_COMPOSER_INPUT_STATUS_CANCEL == mInput.status)
            TUILOGD("%s: mInput.status TUI_COMPOSER_INPUT_STATUS_CANCEL ", __func__);
        TUI_CHECK_COND(
            (res = TUIEnqueueDisplayBuffer(mSessionId, &buffer)) == TUI_OK,
            (int32_t)res);
    }

end :
    {
        if (mState != ABORTING) {
            TUILOGD("Updating thread state..");
            retVal = _cleanUpSession();
            if (retVal) {
            TUILOGE("%s::%d Failed to clean up TUIApp session, retVal - %d",
                    __func__, __LINE__, retVal);
            }
            std::lock_guard<std::mutex> l(mLock);
            mState = HANDLING_INPUT_DONE;
        } else {
            TUILOGD("TUI Session is ABORTING");
        }
        TUILOGD("%s: state:%s ret:%d mInput.status:%d", __func__, ToString(mState),
                  ret, mInput.status);
        ret = ((ret == 0) && (mInput.status == TUI_COMPOSER_INPUT_STATUS_COMPLETE));
    }
    TUILOGD("%s: ret:%d Notify HLOS via HLOSCBO ", __func__, ret);
    (ret) ? mHLOSCBORef->onComplete() : mHLOSCBORef->onError(ret);

    TUILOGD("%s--: ret:%d ", __func__, ret);
    return;
}

int32_t CTrustedUIApp::startSession(const TUIConfig *TUIconf_ptr)
{
    ENTER;
    int32_t ret = Object_OK;
    int32_t retVal = Object_OK;
    TUIResult res = TUI_OK;
    uint32_t pixelWidth = 4;
    TUIDisplayBuffer buffer;
    TUIDisplayConfig *bufferCfg = &(buffer.config);
    tuiComposerParam_t composerParams;
    tuiComposerDisplayBuffer_t dispBuf;

    std::lock_guard<std::mutex> l(mLock);
    TUI_CHECK_COND(mState == INITIALIZED, TUI_ERR_INVALID_STATE);

    mComposerCB = new ITuiComposerCallback_t();
    TUI_CHECK_COND(mComposerCB != nullptr, TUI_ERR_OUT_OF_MEMORY);

    mComposerCB->cbData = this;
    mComposerCB->allocHelperBuff = CTrustedUIApp::tuiComposerCBAllocHelperBuf;
    mComposerCB->freeHelperBuff = CTrustedUIApp::tuiComposerCBFreeHelperBuf;
    mComposerCB->handleCmd = CTrustedUIApp::tuiComposerCBHandleCmd;

    TUI_CHECK_COND(
        (res = TUIGetDisplayProperties(mSessionId, &mDispCfg)) == TUI_OK,
        (int32_t)res);

    mSDBufSize =
        static_cast<uint64_t>(mDispCfg.height * mDispCfg.width * pixelWidth);
    buffer.buffHandle.size = mSDBufSize;

    TUI_CHECK_COND(
        (res = TUIDequeueDisplayBuffer(mSessionId, &buffer)) == TUI_OK,
        (int32_t)res);

    mLayoutName = reinterpret_cast<char *>(
        const_cast<uint8_t *>(TUIconf_ptr->layoutName));

    composerParams.layoutId = &mLayoutName[0];
    composerParams.width = mDispCfg.width;
    composerParams.height = mDispCfg.height;
    composerParams.format = TUI_COMPOSER_FORMAT_RGBA;
    composerParams.useSecureIndicator = TUIconf_ptr->useSecureIndicator;

    if (mLayoutName.compare("pin") == 0 ||
        mLayoutName.compare("msg_pin") == 0 ||
        mLayoutName.compare("login") == 0 ||
        mLayoutName.compare("msg_login") == 0) {
        TUILOGD("%s::%d - Selecting Dialog Composer", __func__, __LINE__);
        mComposer = createTUIComposer(TUI_COMPOSER_TYPE_DIALOG);
    } else if (mLayoutName.compare("draw_dot") == 0){
        TUILOGD("%s::%d Selecting Custom Composer", __func__, __LINE__);
        mComposer = createTUIComposer(TUI_COMPOSER_TYPE_CUSTOM);
    }
    TUI_CHECK_COND(mComposer != nullptr, TUI_ERR_OUT_OF_MEMORY);

    ret = mComposer->init(mComposer->ctx, &composerParams, mComposerCB);
    TUI_CHECK_COND(ret == TUI_COMPOSER_STATUS_OK, ret);

    dispBuf.config.width = mDispCfg.width;
    dispBuf.config.stride = mDispCfg.width;
    dispBuf.config.height = mDispCfg.height;
    dispBuf.config.format = TUI_COMPOSER_FORMAT_RGBA;
    dispBuf.handle.vAddr = buffer.buffHandle.vAddr;

    /* TUIStartSession is imperative to start a secure session in the VM. TUICore
     * Service will make sure to switch the display and touch in secure mode */
    bufferCfg->width = mDispCfg.width;
    bufferCfg->height = mDispCfg.height;
    bufferCfg->stride = mDispCfg.width;
    bufferCfg->colorFormat = 1;
    bufferCfg->offHeight = 0;
    bufferCfg->offWidth = 0;

    ret = _validateDisplayBuf(&dispBuf, &buffer);
    TUI_CHECK_COND(ret == 0, TUI_ERR_GENERIC_FAILURE);

    mInput.type = TUI_COMPOSER_INPUT_TOUCH;
    mInput.cmd = TUI_COMPOSER_INPUT_CMD_START;

    ret = mComposer->compose(mComposer->ctx, &mInput, &dispBuf);
    TUI_CHECK_COND(ret == TUI_COMPOSER_STATUS_OK, ret);

    TUI_CHECK_COND((res = TUIStartSession(mSessionId, &buffer)) == TUI_OK,
                   (int32_t)res);

    // Start async input handling during session
    mTouchThread = std::make_shared<std::thread>(
        [](CTrustedUIApp *app) { app->_touchThreadHandler(); }, this);

    if (mTouchThread == nullptr) {
        TUILOGE("Failed to start touch thread, abort");
        ret = TUI_ERR_GENERIC_FAILURE;
        goto end;
    }
    mState = HANDLING_INPUT;
    TUILOGE("%s::%d State changed to %s", __func__, __LINE__, ToString(mState));
end:
    if (ret) {
        TUILOGE("%s::%d Start session failed with ret - %d",
                    __func__, __LINE__, ret);
        _cleanUpComposer();
        retVal = _cleanUpSession();
        if (retVal) {
            TUILOGE("%s::%d Failed to clean up TUIApp session, ret - %d",
                    __func__, __LINE__, retVal);
        }
    }
    return ret;
}

void CTrustedUIApp::_stopTouchThreadIfRunning()
{
    if (mTouchThread != nullptr && mTouchThread->joinable()) {
        TUILOGV("%s: waiting for touch thread to join", __func__);
        mTouchThread->join();
        TUILOGV("%s: thread joined ", __func__);
        mTouchThread = nullptr;
        TUILOGV("%s: touch thread is stopped", __func__);
    }
}

int32_t CTrustedUIApp::stopSession()
{
    ENTER;
    int32_t ret = Object_OK;
    int32_t retVal = Object_OK;
    TUIResult res = TUI_OK;
    TUILOGE("%s++: Stop the session: Current State: %s ", __func__, ToString(mState));
    {
      std::lock_guard<std::mutex> l(mLock);
        TUI_CHECK_COND(mState == HANDLING_INPUT ||
                       mState == HANDLING_INPUT_DONE,
                       TUI_ERR_INVALID_STATE);

        if (mState == HANDLING_INPUT) {
            TUILOGE("%s: HLOS - Request sent to abort this session", __func__);
            mState = ABORTING;
        }
        _cleanUpComposer();
        if (mState != HANDLING_INPUT_DONE) {
            TUILOGD("%s++: cleanup session ", __func__);
            retVal = _cleanUpSession();
            if (retVal) {
                TUILOGE("%s::%d Failed to clean up TUIApp session, retVal - %d",
                    __func__, __LINE__, retVal);
            }
        }
    }
    // Notify TUI Framework to stop/abort this session
    // Release lock for touch thread handler to terminate & update state
    TUILOGD("%s: Wait for touch thread to terminate", __func__);
    _stopTouchThreadIfRunning();
    TUILOGD("%s::%d Touch thread terminated", __func__, __LINE__);
    {
      TUILOGD("%s::%d Session Stopped curr state:%s", __func__, __LINE__, ToString(mState));
      std::lock_guard<std::mutex> l(mLock);
      mState = INITIALIZED;
    }
    TUILOGE("%s::%d State changed to %s", __func__, __LINE__, ToString(mState));

end:
    TUILOGD("%s-- ", __func__);
    return ret;
}

int32_t CTrustedUIApp::deleteSession()
{
    int32_t ret = Object_OK;
    TUIResult res = TUI_OK;
    ENTER;

    std::lock_guard<std::mutex> l(mLock);
    TUI_CHECK_COND(mState == INITIALIZED, TUI_ERR_INVALID_STATE);

    TUILOGD("%s:++ ", __func__);
    // Notify TUI Framework to delete this session
    TUI_CHECK_COND((res = TUIDeleteSession(mSessionId)) == TUI_OK, -1);

    // App's clean-up routine
    if (mHLOSCBORef) {
        //We need to release the ref held by proxy object
        TUILOGE("delete my reference to listener ");
        delete mHLOSCBORef;
    }

    _releaseWakeLock();

    mState = UNINITIALIZED;
    TUILOGE("%s: State changed to %s", __func__, ToString(mState));

end:
    EXIT;
    return ret;
}

int32_t CTrustedUIApp::handleCmd(const void *cmd_ptr, size_t cmd_len,
                                 void *response_ptr, size_t response_len,
                                 size_t *response_lenout)
{
    int32_t ret = Object_OK;
    uint32_t cmdId = 0;
    char * cmd = nullptr;
    char * rsp = nullptr;
    char * msg = nullptr;
    ENTER;
    std::lock_guard<std::mutex> l(mLock);

    TUI_CHECK_COND(mState != UNINITIALIZED, TUI_ERR_INVALID_STATE);
    TUI_CHECK_COND(cmd_ptr != nullptr &&
                   response_ptr != nullptr &&
                   response_lenout != nullptr &&
                   response_len > 0 &&
                   cmd_len >= sizeof(uint32_t), TUI_ERR_INVALID_INPUT);

    cmd = (char *)cmd_ptr;
    rsp = (char *)response_ptr;

//extract command id from byte buffer
    for (int i = 0; i < 4 ; i++) {
        cmdId = cmdId << 8;
        cmdId |= cmd[i];
    }

    TUILOGD("cmdId: %d", cmdId);
// App's custom cmd handling
    if (cmdId == TUI_CMD_SEND_PING) {
        TUILOGD("ping received, sending response");
        rsp[0] = TUI_CMD_RECV_PING;
        *response_lenout = 1;
    }
    else if (cmdId == TUI_CMD_SEND_MSG) {
        if (cmd_len > 4) {
            msg = cmd + 4;

            TUILOGE("%s", msg);
            for (int i = 4; i < response_len; i++) {
                rsp[i] = i % 256;
            }
            rsp[0] = TUI_CMD_RECV_PING;
            *response_lenout = response_len;
        }
        else {
            TUILOGE("%s: no msg", __func__);
            return -1;
        }
    }

end:
    return ret;
}

int32_t CTrustedUIApp_open(Object *objOut)
{
    TUILOGD("%s ++", __func__);
    CTrustedUIApp *me = new CTrustedUIApp();
    if (!me) {
        TUILOGE("Memory allocation for CTrustedUIApp failed!");
        return Object_ERROR_KMEM;
    }

    *objOut = (Object){ImplBase::invoke, me};
    TUILOGD("%s --", __func__);
    return Object_OK;
}

extern "C" int32_t CApp_init(Object *objOut)
{
    int32_t ret = CTrustedUIApp_open(objOut);
	appObj = (*objOut);
    return ret;
}

extern "C" void CApp_deinit()
{
    if (!Object_isNull(appObj)) {
        TrustedUIApp* app = new TrustedUIApp(appObj);
        if (app) {
            TUILOGE("%s:%u Abort current session", __func__, __LINE__);
            app->stopSession();
            app->deleteSession();
        }
    }
}

const char *CTrustedUIApp::ToString(State state)
{
    switch (state) {
        case UNINITIALIZED:
            return "UNINITIALIZED";
        case INITIALIZED:
            return "INITIALIZED";
        case HANDLING_INPUT:
            return "HANDLING_INPUT";
        case HANDLING_INPUT_DONE:
            return "HANDLING_INPUT_DONE";
        case ABORTING:
            return "ABORTING";
        default:
            return "INVALID";
    }
}
