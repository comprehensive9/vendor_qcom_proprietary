/*
 * Copyright (c) 2019 - 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android-base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <gtest/gtest.h>
#include <hidl/HidlSupport.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <hidlmemory/mapping.h>
#include <hidl/Status.h>
#include <poll.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <stringl.h>
#include <sys/stat.h>
#include <utils/Log.h>
#include <vendor/qti/hardware/trustedui/1.2/ITrustedUI.h>
#include <vendor/qti/hardware/trustedui/1.1/ITrustedInput.h>
#include <thread>
#include "TrustedUICallback.h"
#include "common_log.h"
#include "display_config.h"
#include <config/client_interface.h>
#include "SecureIndicatorAPI.h"


using vendor::qti::hardware::trustedui::V1_2::ITrustedUI;
using vendor::qti::hardware::trustedui::V1_0::ITrustedUICallback;
using vendor::qti::hardware::trustedui::V1_1::ITrustedInput;
using vendor::qti::hardware::trustedui::V1_0::implementation::TrustedUICallback;
using ::vendor::qti::hardware::trustedui::V1_0::Response;
using ::vendor::qti::hardware::trustedui::V1_0::TUICreateParams;
using ::vendor::qti::hardware::trustedui::V1_1::TUICreateParams2;
using ::vendor::qti::hardware::trustedui::V1_0::TUIConfig;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;

using ::android::hidl::memory::V1_0::IMemory;
using ::android::hardware::hidl_memory;
using ::android::hidl::allocator::V1_0::IAllocator;


GTEST_DEFINE_bool_(enable_frame_auth, false,
                   "Enable periodic (every vsync) frame auth");
GTEST_DEFINE_int32_(logLevel, 1, "Set logging level");
GTEST_DEFINE_string_(tuiTEE, "qtee-tz", "Set tee env for TUI: qtee-tz for TZ, qtee-vm for VM");
using namespace android;
using namespace DisplayConfig;
using android::sp;
using android::hardware::Return;
using android::hardware::Void;

/** adb log */
#undef LOG_TAG
#define LOG_TAG "TUI_TEST_APP:"

#define LOGD_PRINT(...)      \
    do {                     \
        LOGD(__VA_ARGS__);   \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)
//#define LOGD_PRINT(...) do {  } while(0)

#define LOGE_PRINT(...)      \
    do {                     \
        LOGE(__VA_ARGS__);   \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)

#define FRAME_AUTH_START \
    if (GTEST_FLAG(enable_frame_auth)) startAuthenticateSecureDisplay();

#define FRAME_AUTH_STOP \
    if (GTEST_FLAG(enable_frame_auth)) stopAuthenticateSecureDisplay();

#define TUI_DEMO_LOGO_PATH "/data/vendor/tui/logo.png"
#define TUI_DEMO_IND_PATH "/data/vendor/tui/sec_ind.png"
#define TUI_SAMPLE_APP_NAME_32BIT "tuiapp32"
#define TUI_SAMPLE_APP_NAME_64BIT "tuiapp"

static const std::string TUITestTAFile = "/data/vendor/tui/tuiapp.mbn";

#define MAX_NAME_LEN  25

// custom commands are 16bits
#define TUI_CMD_AUTHENTICATE_FRAME 5
#define TUI_CMD_SEND_MSG 18

//increased shared buffer size only for secureindicator
#define TA_SHARE_BUFFER_SIZE 420000

bool mUseSecureIndicator = false;
int  mDisplayId = 0, dispIdx = 0;
bool mInputHandlingError = false;
bool mHALkilled = false;
sem_t mSessionComplete;
bool mLoadTAFromBuffer = false;

// TODO: Implement app_buffer_basic_test
static char legacy_app_name[MAX_NAME_LEN + 1] = TUI_SAMPLE_APP_NAME_64BIT;
static char *config_app_name = legacy_app_name;
static const uint32_t TUIAPP_UID = 1234;
static uint32_t appUID = 0;

#ifdef ENABLE_TRUSTED_UI_VM_3_0
static int GetQDUtilsDisplayType(int32_t displayId) {
  switch(displayId) {
    case 0: /*Primary Display*/
      return qdutils::DISPLAY_PRIMARY;
    case 1: /*Secondary Display*/
      return qdutils::DISPLAY_BUILTIN_2;
    default:
      return -1;
  }
}

static int GetDisplayPortId(int32_t displayId, int32_t *portId) {

    int qdutils_disp_type = GetQDUtilsDisplayType(displayId);
    if (qdutils_disp_type < 0) {
        ALOGE("Display Idx :%d is not supported", displayId);
        return -1;
    }
    int ret = qdutils::GetDisplayPortId(qdutils_disp_type, portId);
    if (ret != 0) {
        ALOGE("GetDisplayPortId failed with ret %d", ret);
        return -1;
    }
    ALOGI("display port id is %d for display %d", *portId, displayId);
    return 0;

}
#endif

class TrustedUIAppTest : public ::testing::Test
{
   public:
    virtual ~TrustedUIAppTest() = default;
    sp<ITrustedUI> mTUIIntf = nullptr;
    sp<ITrustedUICallback> mSessionCallback = nullptr;
    uint32_t mSessionId = -1;

   protected:
    virtual void SetUp();
    virtual void TearDown();
    virtual int32_t checkPrerequisites();
    void *postVSyncLoop();
    int32_t startAuthenticateSecureDisplay();
    int32_t stopAuthenticateSecureDisplay();
    void setLogLevel(uint32_t level);

   private:
    struct TuiHALDeathRecipient : public hidl_death_recipient {
        virtual void serviceDied(uint64_t cookie __unused,
                const android::wp<IBase>& who __unused) {
            ALOGD("%s: TUI_HAL died, aborting tests", __func__);
            mHALkilled = true;
            sem_post(&mSessionComplete);
        }
    };

    int32_t _readAppIntoBuffer(hidl_memory &appBin, const char* appPath);
    static inline const std::string memType = "ashmem";
    bool mSetUpOk = false;
    bool gVSyncThreadDone = false;
    std::string mTrustedEE;
    sp<ITrustedInput> mTrustedInputDevice = nullptr;
    sp<TuiHALDeathRecipient> death_recipient_ = nullptr;
    ClientInterface *mDisplayConfigIntf = nullptr;
    std::shared_ptr<std::thread> mAuthThread = nullptr;
};

// TrustedUICallback methods
Return<void> TrustedUICallback::onComplete()
{
    LOGD_PRINT("%s: TrustedUICallback:: onComplete Notification", __func__);
    sem_post(&mSessionComplete);
    return Void();
}

Return<void> TrustedUICallback::onError()
{
    LOGD_PRINT("%s: onError Notification", __func__);
    mInputHandlingError = true;
    sem_post(&mSessionComplete);
    return Void();
}

// TrustedUIAppTest methods

int32_t TrustedUIAppTest::_readAppIntoBuffer(hidl_memory &appBin, const char* appPath)
{
    int32_t ret = -1;
    int fd = -1;
    struct stat f_info = {0};
    size_t TASize = 0;
    char *pBuf = nullptr;
    uint32_t getsize = 0;
    sp<IMemory> memory;
    sp<IAllocator> ashmemAllocator;

    fd = open(appPath, O_RDONLY);
    if (fd < 0) {
        ALOGE("%s::%d Could not open the Trusted App", __func__, __LINE__);
        goto end;
    }

    if (fstat(fd, &f_info) == -1) {
        ALOGE("%s::%d Could not get the Trusted App file info", __func__, __LINE__);
        goto end;
    }
    TASize = f_info.st_size;

    /* Get the Memory Allocator service from Memory HAL */
    ashmemAllocator = IAllocator::getService(memType);
    if (ashmemAllocator == nullptr) {
        ALOGE("%s::%d Could not get IAllocator service", __func__, __LINE__);
        goto end;
    }

    /* Allocate memory of size upto the TrustedApp size */
    ashmemAllocator->allocate(TASize, [&](bool success, const hidl_memory &mem) {
    if (!success) {
        ALOGE("%s::%d ashmem allocate failed!!", __func__, __LINE__);
        return;
    }
    /* Memory map hidl memory, so that the HAL memory can access it*/
    appBin = mem;
    memory = mapMemory(mem);
    if (memory == nullptr) {
        ALOGE("%s::%d Could not map HIDL memory to IMemory", __func__, __LINE__);
        return;
    }
    memory->update();
    pBuf = static_cast<char*>(static_cast<void*>(memory->getPointer()));
    if (pBuf == nullptr) {
        ALOGE("%s::%d Could not get the memory pointer", __func__, __LINE__);
        return;
    }
    getsize = (uint32_t)memory->getSize();
    LOGD_PRINT("%s:%d memory size is: %u", __func__, __LINE__, getsize);
    if (read(fd, pBuf, f_info.st_size) == -1) {
        ALOGE("%s::%d Error %d failed to read image.\n", __func__, __LINE__, errno);
        return;
    }
    memory->commit();
    ret = 0;
    });

end:
   if (fd >= 0) {
      close(fd);
   }
   return ret;
}

void TrustedUIAppTest::SetUp()
{
    hidl_memory appBin, emptyAppBin;

    LOGD("%s: ITrustedUI::getService ", __func__);
    //Consider a way of passing the getService
    // arg as an arg from cmd line instead of hard-coding
    if (GTEST_FLAG(tuiTEE) == "qtee-vm" ) {
        LOGD_PRINT("====================  TUI :: VM ==========================");
        mTUIIntf = ITrustedUI::getService("qtee-vm");
        LOGD("%s: ITrustedInput::getService ", __func__);
        mTrustedInputDevice = ITrustedInput::getService("qtee-vm");

#ifdef ENABLE_TRUSTED_UI_VM_3_0
        //Add dual display support in TVM-TUI
        int32_t portId = 0, rv = -1;
        rv = GetDisplayPortId(dispIdx, &portId);
        if (rv) {
            LOGE_PRINT("%s: Unable to get display Port information for display id :%d", __func__, dispIdx);
        }
        ASSERT_TRUE(rv == 0);

        //Java clients can call getPhysicalDisplayIds to pass displayId used.
        //Native clients can make use of qdutils to pass appropriate portId instead.
        mDisplayId = portId;
#endif

    } else {
        // Default is TZ
        LOGD_PRINT("====================  TUI :: TZ ==========================");
        LOGD("%s: ITrustedInput::getService ", __func__);
        mTUIIntf = ITrustedUI::getService();
        mTrustedInputDevice = ITrustedInput::getService();
    }
    ASSERT_TRUE(mTUIIntf != nullptr);

    ASSERT_TRUE(mTrustedInputDevice != nullptr);

    LOGD("%s: TrustedUICallback ", __func__);
    mSessionCallback = new TrustedUICallback();
    ASSERT_TRUE(mSessionCallback != nullptr);

    int32_t ret = DisplayConfig::ClientInterface::Create("TrustedUI",
                                                     nullptr, &mDisplayConfigIntf);
    ASSERT_TRUE(mDisplayConfigIntf != nullptr);
    ASSERT_TRUE(ret == 0);

    LOGD_PRINT("%s; DISABLE MDP Idle Power Collapse for Trusted UI Session",
               __func__);
    ret = mDisplayConfigIntf->ControlIdlePowerCollapse(0, false);
    if (ret) {
        LOGE_PRINT(
            "%s: IDisplayConfig::controlIdlePowerCollapse failed to "
            "DISABLE MDP Idle PC=[%d] ",
            __func__, ret);
        LOGE_PRINT("%s: Make sure that the display is ON", __func__);
    }
    ASSERT_TRUE(ret == 0);

    LOGD_PRINT(
        "TrustedUIAppTest:: SetUp SUCCESS in connecting to TrustedUI HAL");
    setLogLevel(GTEST_FLAG(logLevel));
    mTrustedEE = config_app_name;
    Response res;

    if (mLoadTAFromBuffer) {
        LOGD_PRINT("%s:%d start to load TA from buff...", __func__, __LINE__);
        ret = _readAppIntoBuffer(appBin, TUITestTAFile.c_str());
        ASSERT_TRUE(appBin.valid() && ret == 0);
        TUICreateParams2 inParams = {config_app_name, mDisplayId,(uint32_t)(mUseSecureIndicator ? TA_SHARE_BUFFER_SIZE : 0), appBin, 0};
        mTUIIntf->createSession3(inParams, mTrustedInputDevice, mSessionCallback,
                        [&](Response _res, uint32_t _sessionId) {
                            res = _res;
                            if (res == Response::TUI_SUCCESS) {
                                mSessionId = _sessionId;
                            }
                        });
    } else {
        TUICreateParams2 inParams = {config_app_name, mDisplayId,(uint32_t)(mUseSecureIndicator ? TA_SHARE_BUFFER_SIZE : 0), emptyAppBin, appUID};
        mTUIIntf->createSession3(inParams, mTrustedInputDevice, mSessionCallback,
                            [&](Response _res, uint32_t _sessionId) {
                              res = _res;
                              if (res == Response::TUI_SUCCESS) {
                                mSessionId = _sessionId;
                              }
                            });
        LOGD_PRINT("TrustedUIAppTest:: SetUp sharedMemSize: %d ",
               inParams.minSharedMemSize);
    }
    ASSERT_EQ(res, Response::TUI_SUCCESS);
    LOGD_PRINT("TrustedUIAppTest:: SetUp  HAL State is now : LOADED ");
    sem_init(&mSessionComplete, 0, 0);

    death_recipient_ = new TuiHALDeathRecipient();
    if (death_recipient_ == NULL) {
        LOGE("%s: Failed to register death_recipient", __func__);
    }
    else {
        Return<bool> linked = mTUIIntf->linkToDeath(death_recipient_, 0);
        if (!linked.isOk()) {
            LOGE("%s: Transaction error in registering death recipient to TUI_HAL", __func__);
        } else if (!linked) {
            LOGE("%s: unable to link to TUI_HAL death notification", __func__);
        } else {
            LOGD("%s: Successfully linked to TUI_HAL death notification", __func__);
        }
    }

    mSetUpOk = true;
}

void TrustedUIAppTest::TearDown()
{
    if (mSetUpOk == false) {
        LOGE_PRINT("TrustedUIAppTest:: Teardown Nothing to be done");
        return;
    }

    if (!mHALkilled) {
    /* Call TUI Stop in case the test is tearing down due to error
     * and did not get to Stop the TUI Session. If not called, it will
     * result in failure of all subsequent tests */
        LOGD_PRINT("TrustedUIAppTest:: Teardown Issuing STOP to TUI HAL");
        mTUIIntf->stopSession(mSessionId);

        Response ret = mTUIIntf->deleteSession(mSessionId);
        if (ret == Response::TUI_FAILURE) {
            LOGE_PRINT("TrustedUIAppTest::TearDown failed to delete the Session");
        }
        if (death_recipient_ != NULL) {
            mTUIIntf->unlinkToDeath(death_recipient_);
        }
    }

    sem_destroy(&mSessionComplete);

    LOGD_PRINT("TrustedUIAppTest::TearDown complete!");
    mSetUpOk = false;

    LOGD_PRINT("%s: ENABLE MDP Idle Power Collapse", __func__);
    int32_t res = mDisplayConfigIntf->ControlIdlePowerCollapse(1, false);
    if (res) {
        LOGE_PRINT(
            "%s: IDisplayConfig::controlIdlePowerCollapse failed to "
            "ENABLE MDP Idle PC=[%d] ",
            __func__, res);
    }
    ASSERT_EQ(res, 0);
}

int32_t TrustedUIAppTest::checkPrerequisites()
{
    struct stat st;
    if (GTEST_FLAG(tuiTEE) == "qtee-vm") {
        return 0;
    }

    if (stat(TUI_DEMO_LOGO_PATH, &st) != 0) {
        LOGE_PRINT("TUI Sample Client: logo image is missing");
        LOGD_PRINT(
            "TUI Sample Client: logo image should be a PNG located at: %s",
            TUI_DEMO_LOGO_PATH);
        return -1;
    }

    if (stat(TUI_DEMO_IND_PATH, &st) != 0) {
        LOGE_PRINT(
            "TUI Sample Client: indicator image is missing, will try to "
            "display the secure indicator instead");
        LOGD_PRINT(
            "TUI Sample Client: indicator image should be a PNG located at: %s",
            TUI_DEMO_IND_PATH);
    }
    return 0;
}

void TrustedUIAppTest::setLogLevel(uint32_t debugLevel)
{
    char *level = NULL;
    switch(debugLevel) {
        case 0x1:
            level = "INFO";
            break;
        case 0x2:
            level = "DEBUG";
            break;
        case 0x3:
            level = "ALL";
            break;
        default:
            level = "ERROR";
    }
    if (0 == property_set("vendor.tui_debug_level", level)) {
        LOGD_PRINT("%s: set logging level:%s", __func__, level);
    }
}

void *TrustedUIAppTest::postVSyncLoop()
{
    int32_t fds[2] = {-1};
    Response ret;

    if (pipe(fds) == -1) {
        ALOGE("%s: pipe() failed !", __func__);
        return NULL;
    }
    struct pollfd poll_fd = {
        .fd = fds[0], .events = POLLIN,
    };

    while (!gVSyncThreadDone) {
        /* poll timeout time is kept random between 16 and 32 ms , so
         * as to make the "time of CRC check in TZ" random   and a max of 32 ms
         * as we want to check CRC every Vsync */
        int random_poll_timeout = 16 + (rand() % 16);
        int status = poll(&poll_fd, 1, random_poll_timeout /*ms*/);
        if (status == 0) { /*TIMEOUT*/
            ALOGD("%s: Sending cmd TUI_CMD_AUTHENTICATE_FRAME", __func__);
            mTUIIntf->sendCommand(
                mSessionId, TUI_CMD_AUTHENTICATE_FRAME, NULL,
                [&](Response _res, std::vector<uint8_t> _responseData) {
                    ret = _res;
                });
            if (ret != Response::TUI_SUCCESS) {
                ALOGE("%s: send command failed with ret : %d", __func__, ret);
                break;
            }
        } else {
            ALOGE("%s: poll failed !", __func__);
            break;
        }
    }

    close(fds[0]);
    close(fds[1]);
    return NULL;
}

int32_t TrustedUIAppTest::startAuthenticateSecureDisplay()
{
    int32_t ret = 0;

    /* Create the main thread of sending Secure UI authenticate cmd requests in
     * a while loop */
    mAuthThread =
        std::make_shared<std::thread>([&]() { this->postVSyncLoop(); });
    if (!mAuthThread->joinable()) {
        ALOGE(
            "Error: Creating thread for vsync-trigger in secure UI client "
            "failed!");
        ret = -1;
        goto end;
    }
end:
    return ret;
}

int32_t TrustedUIAppTest::stopAuthenticateSecureDisplay()
{
    int32_t ret = 0;

    if (gVSyncThreadDone || !mAuthThread->joinable()) {
        ALOGE(
            "Error: %s called while thread not joinable, gVSyncThreadDone:%d "
            "joinable:%d",
            gVSyncThreadDone, mAuthThread->joinable());
        ret = -1;
        goto end;
    }
    gVSyncThreadDone = true;
    mAuthThread->join();
end:
    return ret;
}

// TODO: Address this in current layout instead of doing it for next layout
#define CHECK_FOR_TEST_COMPLETION(ret)                         \
    if ((ret) > 0) {                                           \
        ret = 0;                                               \
        LOGD_PRINT("\nUser Pressed Cancel. Test Completed\n"); \
        goto end;                                              \
    }

TEST_F(TrustedUIAppTest, TestTUILayouts)
{
    ASSERT_TRUE(mTUIIntf != nullptr);
    Response ret;
    int32_t rv = 0;
    mInputHandlingError = 0;
    ASSERT_EQ(checkPrerequisites(), 0);

    TUIConfig cfg;
    while (1) {
        LOGD_PRINT("TrustedUIAppTest:: Start PIN Screen..");

        /* get pin screen */
        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "pin", false /*enableFrameAuth*/};
        ASSERT_GE(ret = mTUIIntf->startSession(mSessionId, cfg),
                  Response::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);
        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        ASSERT_EQ(mTUIIntf->stopSession(mSessionId), Response::TUI_SUCCESS);

        if (mInputHandlingError) break;

        /* message screen for showing the received pin */
        LOGD_PRINT("TrustedUIAppTest:: Start MSG_PIN Screen..");
        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "msg_pin", false /*enableFrameAuth*/};
        ASSERT_GE(ret = mTUIIntf->startSession(mSessionId, cfg),
                  Response::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);

        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        ASSERT_EQ(mTUIIntf->stopSession(mSessionId), Response::TUI_SUCCESS);

        if (mInputHandlingError) break;

        /* login screen */
        LOGD_PRINT("TrustedUIAppTest:: Start LOGIN Screen..");
        cfg = {mUseSecureIndicator /*useSecureIndicator*/, "login", false /*enableFrameAuth*/};
        ASSERT_GE(ret = mTUIIntf->startSession(mSessionId, cfg),
                  Response::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);

        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        ASSERT_EQ(mTUIIntf->stopSession(mSessionId), Response::TUI_SUCCESS);

        if (mInputHandlingError) break;

        /* message screen for showing the received username & password */
        LOGD_PRINT("TrustedUIAppTest:: Start MSG_LOGIN Screen..");
        cfg = {mUseSecureIndicator, "msg_login", false /*enableFrameAuth*/};
        ASSERT_GE(ret = mTUIIntf->startSession(mSessionId, cfg),
                  Response::TUI_SUCCESS);
        rv = (int)ret;
        CHECK_FOR_TEST_COMPLETION(rv);

        LOGD_PRINT(
            "TrustedUIAppTest:: Launched the layout. Start user interaction "
            "..");
        LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

        FRAME_AUTH_START;
        sem_wait(&mSessionComplete);
        LOGD_PRINT("TrustedUIAppTest:: Session completed.");
        FRAME_AUTH_STOP;

        if (mHALkilled) break;

        LOGD_PRINT("TrustedUIAppTest:: Sending cmd STOP to TUI HAL");
        ASSERT_EQ(mTUIIntf->stopSession(mSessionId), Response::TUI_SUCCESS);
        if (mInputHandlingError) break;
    }
end:
    return;
}

TEST_F(TrustedUIAppTest, Basic_SecureTouch)
{
    LOGD_PRINT("TrustedUIAppTest:: Basic_SecureTouch");
    ASSERT_TRUE(mTUIIntf != nullptr);

    Response ret;
    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "draw_dot",
                     false /*enableFrameAuth*/};

    LOGD_PRINT("TrustedUIAppTest::Sending cmd START to TUI HAL");
    ASSERT_EQ(ret = mTUIIntf->startSession(mSessionId, cfg),
              Response::TUI_SUCCESS);

    LOGD_PRINT(
        "TrustedUIAppTest:: Launched the layout. Start user interaction ..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    FRAME_AUTH_START;
    sem_wait(&mSessionComplete);
    LOGD_PRINT("TrustedUIAppTest:: Session completed.");
    FRAME_AUTH_STOP;

    if (mHALkilled) return;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP to TUI HAL");
    ASSERT_EQ(mTUIIntf->stopSession(mSessionId), Response::TUI_SUCCESS);
}

// NOTE: display-only tests are currently not supported
TEST_F(TrustedUIAppTest, Basic_TUIStartStop)
{
    LOGD_PRINT("TrustedUIAppTest::Basic_TUIStartStop");
    ASSERT_TRUE(mTUIIntf != nullptr);

    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "pin",
                     false /*enableFrameAuth*/};
    Response ret;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
    ASSERT_EQ(ret = mTUIIntf->startSession(mSessionId, cfg),
              Response::TUI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest::Session started successfully ");
    LOGD_PRINT("TrustedUIAppTest::Wait for 2 seconds before exiting..");
    sleep(2);

    LOGD_PRINT("TrustedUIAppTest::Sending cmd STOP to TUI HAL");
    ASSERT_EQ(mTUIIntf->stopSession(mSessionId), Response::TUI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest:: Session completed.");
}

//The purpose of this test is to check proper session clean up
//in case of error in startSession api.
TEST_F(TrustedUIAppTest, NegativeTest_TUIStartError)
{
    LOGD_PRINT("TrustedUIAppTest::NegativeTest_TUIStartError");
    ASSERT_TRUE(mTUIIntf != nullptr);

    //"no_layout" is not supported layout, so startSession should return err
    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "no_layout",
                     false /*enableFrameAuth*/};
    Response ret;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
    ASSERT_LT(ret = mTUIIntf->startSession(mSessionId, cfg),
              Response::TUI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest::Start Session failed as expected ret:%d", ret);
}

TEST_F(TrustedUIAppTest, NegativeTest_DeleteSessionWithoutStopSession)
{
    LOGD_PRINT(
        "TrustedUIAppTest::NegativeTest_DeleteSessionWithoutStopSession");
    ASSERT_TRUE(mTUIIntf != nullptr);

    TUIConfig cfg = {mUseSecureIndicator /*useSecureIndicator*/, "pin",
                     false /*enableFrameAuth*/};
    Response ret;

    LOGD_PRINT("TrustedUIAppTest::Sending cmd startSession to TUI HAL");
    ASSERT_EQ(ret = mTUIIntf->startSession(mSessionId, cfg),
              Response::TUI_SUCCESS);

    LOGD_PRINT(
        "TrustedUIAppTest:: Launched the layout. Start user interaction ..");
    LOGD_PRINT("TrustedUIAppTest:: waiting for session to complete ...zz");

    sem_wait(&mSessionComplete);
    LOGD_PRINT("TrustedUIAppTest:: Session completed.");

    if (mHALkilled) return;

    LOGD_PRINT("TrustedUIAppTest:: deleteSession ..");
    ASSERT_EQ(ret = mTUIIntf->deleteSession(mSessionId), Response::TUI_FAILURE);
    LOGD_PRINT("TrustedUIAppTest::deleteSession failed as expected !");
}

TEST_F(TrustedUIAppTest, Test_SecureIndicatorProvision)
{
        boolean result = 0;
        uint32 maxWidthInPixels, maxHeightInPixels;

        LOGD_PRINT(
        "TrustedUIAppTest::NegativeTest_SecureIndicatorProvision");

        LOGD_PRINT("TrustedUIAppTest::Check if the sec_ind.png is present ...");
        ASSERT_EQ(checkPrerequisites(), 0);

        LOGD_PRINT("TrustedUIAppTest::Init the secure Indicator ...");
        ASSERT_EQ(SecureIndicator_Init(), TUI_SI_SUCCESS);

        LOGD_PRINT("TrustedUIAppTest::Check if a secure indicator has been provisioned ...");
        ASSERT_EQ(SecureIndicator_IsIndicatorProvisioned(&result), TUI_SI_SUCCESS);
        if (result) {
                LOGD_PRINT("TrustedUIAppTest::Found a secure indicator has been provisioned, removed it ...");
                ASSERT_EQ(SecureIndicator_RemoveIndicator(), TUI_SI_SUCCESS);
        }

        LOGD_PRINT("TrustedUIAppTest::get the demensions of secure indicator from secure indicator TA ...");
        ASSERT_EQ(SecureIndicator_GetSecureIndicatorDimensions(&maxWidthInPixels, &maxHeightInPixels), TUI_SI_SUCCESS);
        LOGD_PRINT("TrustedUIAppTest:: demensions of secure indicator are %d(width) and %d(hight)...", maxWidthInPixels, maxHeightInPixels);

        LOGD_PRINT("TrustedUIAppTest::Store a new image to secure indicator ...");
        ASSERT_EQ(SecureIndicator_StoreSecureIndicator(TUI_DEMO_IND_PATH), TUI_SI_SUCCESS);

        LOGD_PRINT("TrustedUIAppTest::Check if the secure indicator has been provisioned successfully...");
        ASSERT_EQ(SecureIndicator_IsIndicatorProvisioned(&result), TUI_SI_SUCCESS);
        ASSERT_EQ(result, 1);

        LOGD_PRINT("TrustedUIAppTest::the secure indicator has been provisioned successfully, remove it...");
        ASSERT_EQ(SecureIndicator_RemoveIndicator(), TUI_SI_SUCCESS);

        LOGD_PRINT("TrustedUIAppTest::Tear down the secure indicator ...");
        ASSERT_EQ(SecureIndicator_Teardown(), TUI_SI_SUCCESS);

}

TEST_F(TrustedUIAppTest, TestSendCmd)
{
    LOGD_PRINT("TrustedUIAppTest:: TestSendCmd");
    ASSERT_TRUE(mTUIIntf != nullptr);

    std::vector<uint8_t> cmdData;
    std::vector<uint8_t> rspData;
    uint16_t cmdId = TUI_CMD_SEND_MSG;

    std::string msg = "this msg is from HLOS";
    for (char i : msg) {
        cmdData.push_back(i);
    }
    cmdData.push_back('\0');
    Response ret;

    mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, [&](Response _res, std::vector<uint8_t> _responseData) {
            ret = _res;
            rspData = _responseData;});

    LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
    ASSERT_EQ(ret, Response::TUI_SUCCESS);
    LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));
}

TEST_F(TrustedUIAppTest, SendCmdTestMaxRespSize)
{
    LOGD_PRINT("TrustedUIAppTest:: SendCmdTestMaxRespSize");
    ASSERT_TRUE(mTUIIntf != nullptr);

    std::vector<uint8_t> cmdData;
    std::vector<uint8_t> rspData;
    uint16_t cmdId = TUI_CMD_SEND_MSG;

    //fills buffer with 10240 bytes
    std::string msg = "this msg is from HLOS";
    for (int i = 0; i < 512; i++) {
        for (char i : msg) {
            cmdData.push_back(i);
        }
    }
    cmdData.push_back('\0');
    Response ret;

    mTUIIntf->sendCommand(mSessionId, cmdId, cmdData, [&](Response _res, std::vector<uint8_t> _responseData) {
            ret = _res;
            rspData = _responseData;});

    LOGD_PRINT("TrustedUIAppTest:: SendCmd returned: %d", ret);
    ASSERT_EQ(ret, Response::TUI_SUCCESS);
    LOGD_PRINT("cmd response: %d", *((uint32_t *)rspData.data()));

    //rsp buffer populated with n%256
    LOGD_PRINT("%x", rspData[1280]);
    ASSERT_EQ(rspData[1280], 1280 % 256);
}


TEST_F(TrustedUIAppTest, SecureIndicator2_StoreIndicator)
{
    LOGD_PRINT("TrustedUIAppTest:: SecureIndicator2_StoreIndicator");

    const std::string imagePath = "/data/vendor/tui/sec_ind.png";
    ASSERT_EQ(SecureIndicator2_StoreSecureIndicator(imagePath), TUI_SI_SUCCESS);

    LOGD_PRINT("TrustedUIAppTest:: SecureIndicator2 StoreIndicator call completed.");
}



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    if (argc == 1) goto run;

    for (int i = 1; i < argc; i++) {
        // Enable frame authentication
        if ((strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--frameauth") == 0)) {
            GTEST_FLAG(enable_frame_auth) = true;
            continue;
        } // Enable Medium level logging
        else if ((strcmp(argv[i], "-d1") == 0) || (strcmp(argv[i], "--debug=medium") == 0)) {
            GTEST_FLAG(logLevel) = 1;
            continue;
        // Enable Low level logging
        } else if ((strcmp(argv[i], "-d2") == 0) || (strcmp(argv[i], "--debug=low") == 0)) {
            GTEST_FLAG(logLevel) = 2;
            continue;
        // Enable all logs
        } else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--debug=all") == 0)) {
            GTEST_FLAG(logLevel) = 3;
            continue;
        } else if ((strcmp(argv[i], "-vm") == 0)) {
            LOGD_PRINT("Selecting VM implementation");
            GTEST_FLAG(tuiTEE) = "qtee-vm";
            continue;
        } else if ((strcmp(argv[i], "-tz") == 0)) {
            LOGD_PRINT("Selecting TZ implementation");
            GTEST_FLAG(tuiTEE) = "qtee-tz";
            continue;
        // Enable secure indicator
        } else if ((strcmp(argv[i], "-si") == 0) || (strcmp(argv[i], "--si") == 0)) {
            LOGD_PRINT("Enable secure indicator");
            mUseSecureIndicator = true;
            continue;
        } else if (strcmp(argv[i], "-loadtafrombuffer") == 0) {
            LOGD_PRINT("Enable Loading TA from buffer.");
            mLoadTAFromBuffer = true;
            continue;
        } else if (((strcmp(argv[i], "-UID") == 0) && (i + 1) < argc)||
                   ((strcmp(argv[i], "--appUID") == 0) && (i + 1) < argc)) {
            appUID = atoi(argv[i+1]);
            appUID = appUID >= 0 ? appUID : TUIAPP_UID;
            LOGD_PRINT("TUI App UID :%d", appUID);
            i++;
            continue;
        } else if (((strcmp(argv[i], "-id") == 0) && (i + 1) < argc)||
                   ((strcmp(argv[i], "--displayid") == 0) && (i + 1) < argc)) {
            mDisplayId = atoi(argv[i+1]);
            mDisplayId = mDisplayId >= 0 ? mDisplayId : 0;
            dispIdx = mDisplayId;
            i += 1;
            LOGD_PRINT("set display ID :%d", mDisplayId);
            continue;
        } else if (strcmp(argv[i], "-appname") == 0 && (i + 1) < argc) {
            config_app_name = argv[i+1];
            i++;
            continue;
        } else if (strncmp(argv[i], "--appname=", sizeof("--appname=") - 1 ) == 0) {
            config_app_name = argv[i] + sizeof("--appname=") - 1;
            continue;
        } else {
            LOGD_PRINT("Use: TrustedUISampleTest --gtest_filter=*[test]* [-opts]");
            LOGD_PRINT("options:");
            LOGD_PRINT("--gtest_filter=*{test_name}* \t\t Run specific tests (TestTUILayouts, SecureTouch).");
            LOGD_PRINT("--appname={TUI TA name} \t\t Optional TUI TA name, default is tuiapp");
            LOGD_PRINT("--si \t\t\t\t Enable secure indicator. it is disabled by default.");
            LOGD_PRINT("--displayid {display id number}  Set display ID, it's 0 by defult.");
            LOGD_PRINT("-f \t\t\t\t Enable frame authentication");
            LOGD_PRINT("-vm \t\t\t\t Use VM implementation of TUI");
            LOGD_PRINT("-d1 \t\t\t\t Enable INFO logging. Error is enabled by default");
            LOGD_PRINT("-d2 \t\t\t\t Enable DEBUG logging. Error is enabled by default");
            LOGD_PRINT("-d \t\t\t\t Enable ALL logging. Error is enabled by default");
            LOGD_PRINT("-appname {TUI TA name} \t\t\t\t Optional TUI TA name, default is tuiapp");
            LOGD_PRINT("-si \t\t\t\t Enable secure indicator. it is disabled by default");
            LOGD_PRINT("-loadtafrombuffer \t Enable loading TA from buffer. TA image:%s", TUITestTAFile.c_str());
            LOGD_PRINT("-UID {TUI App UID} \t\t\t\t Pass App UID from client. Default is TUI App UID");
            LOGD_PRINT("-appUID {TUI App UID} \t\t\t\t Pass App UID from client. Default is TUI App UID");
            LOGD_PRINT("-id {display id number}\t Set display ID, it's 0 by defult.");
            return 0;
        }
    }
run:
    return RUN_ALL_TESTS();
}
