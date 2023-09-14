/*===================================================================================
  Copyright (c) 2020 - 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <fstream>

#include <utils/Log.h>
#include "TUIUtils.h"
#include "TouchInputVM.h"
#include <unistd.h>

using namespace std;

/******************************************************************************
*       Constant Definitions And Local Variables
*****************************************************************************/
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "HLOSTouchInputVM"

#define MAX_RETRY_ATTEMPTS 5

namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {
namespace V1_2 {
namespace implementation {

std::mutex TouchInput::sLock;
std::shared_ptr<TouchInput> TouchInput::sInstance = nullptr;

// singleton
std::shared_ptr<TouchInput> TouchInput::getInstance()
{
    if (sInstance != nullptr) {
        return sInstance;
    }

    std::lock_guard<std::mutex> l(sLock);
    sInstance = std::shared_ptr<TouchInput>(new TouchInput);

    return sInstance;
}

/******************************************************************************
 *                        Private Class Function Definitions
*****************************************************************************/

/* Description :  This API searches for the control filename.
 *
 * Return :  -ENOENT,
 *           -EINVAL
 *
 */

int32_t TouchInput::touchSearchDevice()
{
    int32_t ret = -ENOENT;
    bool bFound = false;

    for (int32_t i = 0; i < controlFilelocations.size() && !bFound; ++i) {
        DIR *dirp = NULL;
        struct dirent *dirInfo = NULL;
        string devicePath;
        string path = controlFilelocations[i];

        TUI_CHECK_ERR(!path.empty(), -EINVAL);
        ALOGD("%s, %d : Searching %s", __func__, __LINE__, path.c_str());
        dirp = opendir(&path[0]);
        if (dirp == nullptr)
           continue;
        devicePath = path + '/';
        while ((dirInfo = readdir(dirp)) != NULL) {
            if ((!strncmp(dirInfo->d_name, ".", 10)) ||
                (!strncmp(dirInfo->d_name, "..", 10)))
                continue;
            ALOGD("%s::%d - d_name - %s", __func__, __LINE__, dirInfo->d_name);
            if ((dirInfo->d_type == DT_REG) &&
                (strcmp(dirInfo->d_name, controlFileName[1].c_str()) == 0)) {
                mTouchTypeFile = devicePath + controlFileName[0];
                mControlFile = devicePath + controlFileName[1];
                ret = validateDisplayType();
                if (ret) {
                  mTouchTypeFile.clear();
                  mControlFile.clear();
                  break;
                }
                ALOGD("%s::%d : Trusted touch enable file found %s", __func__,
                      __LINE__, mControlFile.c_str());
                ret = 0;
                bFound = true;
                break;
            }
        }
        closedir(dirp);
    }
    TUI_CHECK_ERR(bFound == true, ret);
errorExit:
    return ret;
}

/* Description :  This API returns the controller id.
 *
 * In : str : Control FileName.
 *
 * Out : mTouchControllerId : controller id.
 *
 */

void TouchInput::getControllerId(char *str, int32_t &mTouchControllerId)
{
    char *token, *stringPtr;

    token = strtok_r(str, "-", &stringPtr);
    if (token != NULL) token = strtok_r(NULL, "-", &stringPtr);
    if (token != NULL) token = strtok_r(NULL, "/", &stringPtr);
    if (token != NULL) mTouchControllerId = strtol(token, NULL, 16);
}

/* Description :  This API assigns touch control to LE VM
 *
 * Return :  -errno,
 *           -1
 *
 */
int32_t TouchInput::assignTouchControl()
{
    int32_t ret = 0, retry = MAX_RETRY_ATTEMPTS;
    ssize_t writtenBytes = 0;
    ssize_t readBytes = 0;
    char c;
    string str;

    /* Get the controller ID, so that the info is shared with hidl.  */
    str = mControlFile;
    getControllerId(&str[0], mTouchControllerId);
    ALOGD("%s, %d : Opening control file: %s, mTouchControllerId:0x%x",
          __func__, __LINE__, mControlFile.c_str(), mTouchControllerId);

    mControlFd = open(mControlFile.c_str(), O_RDWR);
    if (mControlFd < 0) {
        ALOGE("%s, %d : Failed to get controlFd", __func__, __LINE__);
        ret = -errno;
        TUI_CHECK_ERR(ret == 0, ret);
    }
    /* Donate touch device to VM */
    writtenBytes = pwrite(mControlFd, "1", 1, 0);
    if (writtenBytes <= 0) {
        ALOGE("%s, %d : Failed to write to control FD", __func__, __LINE__);
        ret = -errno;
        TUI_CHECK_ERR(ret == 0, ret);
    }
    /* Read back to confirm if touch is now donated to VM, retry is needed
     * in case we read back too early */
    do {
        ret = 0;
        readBytes = pread(mControlFd, &c, 1, 0);
        TUI_CHECK_ERR(readBytes > 0, -errno);
        if (c - '0' != 1) {
            ALOGE("%s,%d: Touch is not yet assigned to VM", __func__, __LINE__);
            ret = -1;
        }
        retry--;
        usleep(50);
    } while(retry && (ret != 0));

errorExit:
    return ret;
}

/* Description :  This API validates display type.
 *
 * Return :  0,
 *          -1
 *
 */
int32_t TouchInput::validateDisplayType()
{
    int32_t ret = 0;
    string displayTypeOut = "";
    ifstream touchFile;

    ALOGD("%s::%d Opening trusted touch type file,  mTouchTypeFile - %s",
       __func__, __LINE__, mTouchTypeFile.c_str());

    touchFile.open(mTouchTypeFile.c_str(), ifstream::in);
    if (touchFile.fail()) {
        ALOGE("%s::%d Failed to open trusted touch type file - %s",
            __func__, __LINE__, mTouchTypeFile.c_str());
        displayTypeOut = defaultDisplayType;
    } else {
        getline(touchFile, displayTypeOut, '\0');
    }
    ALOGD("%s::%d displayType - %s, displayTypeOut - %s", __func__, __LINE__,
           mDisplayType.c_str(), displayTypeOut.c_str());
    TUI_CHECK_ERR(displayTypeOut == mDisplayType , -1);

errorExit:
    touchFile.close();
    return ret;
}

/* Methods from ::vendor::qti::hardware::trustedui::V1_0::ITrustedInput follow.
 */

/******************************************************************************
 *                        Public Class Function Definitions
 *****************************************************************************/

/* Methods from ::vendor::qti::hardware::trustedui::V1_0::ITrustedInput
* follow. */

/* Description :  This API starts the TouchInput session in Android
 *
 * hidl Return :  Response::TUI_SUCCESS along with mTouchControllerId
 *                Response::TUI_FAILURE along with mTouchControllerId
 *
 */
Return<void> TouchInput::init(const sp<ITrustedInputCallback> &cb,
                              init_cb _hidl_cb)
{
    ALOGE("%s::%d ITrustedInput Version 1.0 init function is deprecated in TUI VM",
        __func__, __LINE__);
    _hidl_cb(Response::TUI_FAILURE, mTouchControllerId);
    return Void();
}

/* Description :  This API terminates the TouchInput session in Android
 *
 * Return :  Response::TUI_SUCCESS,
 *           Response::TUI_FAILURE
 *
 */

Return<Response> TouchInput::terminate()
{
    int32_t ret = 0;
    ssize_t readBytes = 0;
    ssize_t writtenBytes = 0;
    char c;
    int32_t retry = MAX_RETRY_ATTEMPTS;
    /* Check if already the session is inactive */
    TUI_CHECK_ERR(stSession == true, -EBUSY);
    /* Write 0 to trusted_touch_enable to reclaim TrustedVM to access the touchdata fd */
    writtenBytes = pwrite(mControlFd, "0", 1, 0);
    TUI_CHECK_ERR(writtenBytes > 0, -errno);
    ALOGD("%s, %d : write(fd=%d) writtenBytes %zd", __func__, __LINE__,
            mControlFd, writtenBytes);

    do {
        ret = 0;
        /* Read the trusted_touch_enable node to verify access is reclaimed to Android */
        readBytes = pread(mControlFd, &c, 1, 0);
        TUI_CHECK_ERR(readBytes > 0, -errno);
        if (c - '0' != 0) {
            ALOGE("%s,%d: Touch is not yet reclaimed to Android",
                    __func__, __LINE__);
            ret = -1;
        }
        retry--;
        usleep(50);
    } while(retry && (ret !=0));

    if (ret == 0) {
        ALOGE("%s,%d: Touch is reclaimed to Android", __func__, __LINE__);
    }

errorExit:
    close(mControlFd);
    mControlFd = -1;
    mControlFile.clear();
    mTouchTypeFile.clear();
    stSession = false;
    return (ret == 0) ? Response::TUI_SUCCESS : Response::TUI_FAILURE;
}

/* Description :  This API always returns "Response::TUI_FAILURE",
 * as this is mainly executed by LEVM TouchInput.
 *
 * Return :   Response::TUI_FAILURE
 *
 *
 */
Return<Response> TouchInput::getInput(int32_t timeout)
{
    /*getInput will not perform any functionality in Android, it will be taken
     * care by LEVM TouchInput */
    ALOGE("%s, %d : getInput is not implemented in Android", __func__,
          __LINE__);
    return Response::TUI_FAILURE;
}

  /* Methods from ::vendor::qti::hardware::trustedui::V1_1::ITrustedInput
   * follow. */

/* Description :  This API starts the TouchInput session in Android
 *                and verifies the display type.
 *
 * hidl Return :  Response::TUI_SUCCESS along with mTouchControllerId
 *                Response::TUI_FAILURE along with mTouchControllerId
 *
 */
Return<void> TouchInput::init2(const sp<ITrustedInputCallback> &cb,
                              const hidl_string &displayType,
                              init2_cb _hidl_cb)
{
    int32_t ret = 0;

    /* Check if already a TouchInput session is active */
    TUI_CHECK_ERR(stSession == false, -EBUSY);

    mDisplayType = displayType;
    /* Search for touch type and touch enable filename  */
    ret = touchSearchDevice();
    TUI_CHECK_ERR(ret == 0 && !mControlFile.empty() && !mTouchTypeFile.empty(), ret);


    ret = assignTouchControl();
    TUI_CHECK_ERR(ret == 0, ret);

    if (ret == 0) {
        ALOGE("%s,%d: Touch is now assigned to VM", __func__, __LINE__);
        stSession = true;
    }

errorExit:
    _hidl_cb((ret == 0) ? Response::TUI_SUCCESS : Response::TUI_FAILURE,
             mTouchControllerId);
    return Void();
}

/* Methods from ::android::hidl::base::V1_0::IBase follow. */

} /* namespace implementation */
} /* namespace V1_2 */
} /* namespace trustedui */
} /* namespace hardware */
} /* namespace qti */
} /* namespace vendor */
