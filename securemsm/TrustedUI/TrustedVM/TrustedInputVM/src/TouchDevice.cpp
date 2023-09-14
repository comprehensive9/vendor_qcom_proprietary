/*===================================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/input.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <string>

#include "ITrustedInput.hpp"
#include "TUILog.h"
#include "TUIUtils.h"
#include "TouchDevice.h"
#include "secure_memset.h"

using namespace std;
/******************************************************************************
 *       Constant Definitions And Local Variables
*****************************************************************************/
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TUIVMInput::TouchDevice"

/******************************************************************************
 *                        Private Class Function Definitions
 *****************************************************************************/
/* Description : This API iterates through all the control device nodes
* and return true if string present else false.
*
* In : str : strings from "cat /proc/bus/input/devices".
*
* Return : True,
*          Flase
*/

bool TouchDevice::isControlFilelocation(string str)
{
    bool bFound = false;
    for (int i = 0; i < controlFilelocations.size() && !bFound; ++i) {
        string path = controlFilelocations[i];
        /* remove "/sys" from controlFilelocations to find string */
        string subStr = path.substr(4);
        if (str.find(subStr) != string::npos) {
            bFound = true;
            break;
        }
    }
    return bFound;
}

/* Description : This API iterates through all the device nodes
* and looks for the node with the control file locations and parses
* the corresponding device node value.
*
* Out : deviceNode : Device Node value.
*
* Return : ITrustedInput::ERROR_READ_DEVICENODE,
*          ITrustedInput::ERROR_GET_DEVICENODE,
*          ITrustedInput::SUCCESS
*/

int32_t TouchDevice::findTouchDeviceNode(string &deviceNode)
{
    int32_t ret = ITrustedInput::SUCCESS;
    size_t idx = string::npos;
    string parseStr = "";
    FILE *pStream = nullptr;
    const int MAXBUF = 1024;
    char buf[MAXBUF] = "";
    bool bFound = false;

    TUI_CHECK_COND((pStream = popen(deviceNodeCmd.c_str(), "r")) != nullptr,
                   ITrustedInput::ERROR_GET_DEVICENODE);

    while (fgets(buf, sizeof(buf), pStream) != nullptr && !bFound) {
        string str = buf;
        TUILOGD("%s::%d - str - %s", __func__, __LINE__, str.c_str());
        if (isControlFilelocation(str)) {
            while (fgets(buf, sizeof(buf), pStream) != nullptr && !bFound) {
                str.clear();
                str = buf;
                TUILOGD("%s::%d - str - %s", __func__, __LINE__, str.c_str());
                idx = str.find(searchDeviceNode);
                if (idx != string::npos) {
                    parseStr = str;
                    TUILOGD("%s::%d - idx - %zu", __func__, __LINE__, idx);
                    bFound = true;
                    break;
                }
            }
        }
    }
    pclose(pStream);

    TUI_CHECK_COND(idx != string::npos, ITrustedInput::ERROR_GET_DEVICENODE);
    deviceNode = parseStr.substr(idx + searchDeviceNode.size(), 1);
    TUILOGD("%s::%d - deviceNode - %s", __func__, __LINE__, deviceNode.c_str());

errorExit:
    return ret;
}

/* Description : This API computes the touch to screen ratio, that will be
* applied to the touch data coordinates read from touch device fd.
*
* In :  screenX : screen X value from the Coreservice.
*       screenY : screen Y value from the Coreservice.
*
* Return : ITrustedInput::ERROR_IOCTL_TOUCHDATAFD,
*          ITrustedInput::ERROR_INVALID_SCREEN_X_Y_INFO,
*          ITrustedInput::ERROR_INVALID_MAX_X_Y,
*          ITrustedInput::SUCCESS
*/
int32_t TouchDevice::computeTouchToScreenRatio(const uint32_t screenX,
                                               const uint32_t screenY)
{
    int32_t ret = ITrustedInput::SUCCESS;
    unsigned long xVal, yVal;
    struct input_absinfo absInfo;
    secure_memset(&absInfo, 0, sizeof(absInfo));
    if (screenX == 0 || screenY == 0) {
        return ITrustedInput::ERROR_INVALID_SCREEN_X_Y_INFO;
    }
    mRatio.screenX = screenX;
    mRatio.screenY = screenY;
    TUILOGD("%s::%d - mRatio.screenX - %d", __func__, __LINE__, mRatio.screenX);
    TUILOGD("%s::%d - mRatio.screenY - %d", __func__, __LINE__, mRatio.screenY);

    ret = ioctl(mTouchDeviceFd, EVIOCGABS(ABS_MT_SLOT), &absInfo);
    TUI_CHECK_COND(ret >= 0, ITrustedInput::ERROR_IOCTL_TOUCHDATAFD);

    if (absInfo.maximum == 0) {
        xVal = EVIOCGABS(ABS_X);
        yVal = EVIOCGABS(ABS_Y);
    } else {
        xVal = EVIOCGABS(ABS_MT_POSITION_X);
        yVal = EVIOCGABS(ABS_MT_POSITION_Y);
    }

    ret = ioctl(mTouchDeviceFd, xVal, &absInfo);
    TUI_CHECK_COND(ret >= 0, ITrustedInput::ERROR_IOCTL_TOUCHDATAFD);
    mRatio.maxX = absInfo.maximum;
    TUILOGD("%s::%d - mRatio.maxX - %d", __func__, __LINE__, mRatio.maxX);

    ret = ioctl(mTouchDeviceFd, yVal, &absInfo);
    TUI_CHECK_COND(ret >= 0, ITrustedInput::ERROR_IOCTL_TOUCHDATAFD);
    mRatio.maxY = absInfo.maximum;
    TUILOGD("%s::%d - mRatio.maxY - %d", __func__, __LINE__, mRatio.maxY);

    /* Calculate the touch to screen ratio */
    if (mRatio.maxX == 0 || mRatio.maxY == 0) {
        return ITrustedInput::ERROR_INVALID_MAX_X_Y;
    }
    mRatio.ratioX = (float)mRatio.screenX / (mRatio.maxX);
    mRatio.ratioY = (float)mRatio.screenY / (mRatio.maxY);
    TUILOGD("%s::%d - mRatio.ratioX - %f", __func__, __LINE__, mRatio.ratioX);
    TUILOGD("%s::%d - mRatio.ratioY - %f", __func__, __LINE__, mRatio.ratioY);

errorExit:
    return ret;
}

/******************************************************************************
 *                        Public Class Function Definitions
 *****************************************************************************/

/* Description : This API reads touch events in Standard Linux defined struct
* format "input_event" and processes the touch events in the "fingerData"
* struct format and fills up the buffer along with buffer size and returns
* it to the VMClientApp.
*
*
* Out: buf :  buffer to save the processed touch data.
*      bufSize : buffer size.
*
* Return : ITrustedInput::SUCCESS,
*          ITrustedInput::ERROR_READ_FAILED,
*          ITrustedInput::ERROR_INVALID_FINGER_NUM
*
*/
int32_t TouchDevice::readTouchData(vector<int8_t> &buf, size_t &bufSize)
{
    int32_t ret = ITrustedInput::SUCCESS;
    char tsData[40] = "";
    inputEvent *ev = nullptr;
    int bytes_read = 0;

    /* We need to read mTouchDeviceFd in a do while loop until we get
     DOWN/UP/MOVE event i.e. EV_SYN event, along with x, y and finger number

     Multi finger touch Event handle for type B device
    */
    do {
        secure_memset(tsData, '\0', sizeof(tsData));
        bytes_read = read(mTouchDeviceFd, tsData, sizeof(inputEvent));
        TUILOGD("%s::%d - bytes_read : %d", __func__, __LINE__, bytes_read);
        /* Read the touch data and note that it's mandatory to read  the
       touchdata with sizeof(struct inputEvent) otherwise it will fail.
       Need to have the whole packet, otherwise, don't decode*/
        TUI_CHECK_COND(bytes_read == sizeof(inputEvent),
                       ITrustedInput::ERROR_READ_FAILED);

        ev = (inputEvent *)tsData;
        TUILOGD("%s::%d - ev->type : %d, ev->value : %d, ev->code : %d",
                __func__, __LINE__, ev->type, ev->value, ev->code);
        // Decode the event
        if (ev->type == 3 && ev->code == 0x2f) {
            curr_slot_id = ev->value;
            TUILOGD("%s::%d - slot_id - %d", __func__, __LINE__, curr_slot_id);
        }
        /* Handle only one finger event i.e only slot_id 0
           Other slot events are read but not reported. */
        if (ev->type == 3 && ev->code == 0x39 && curr_slot_id == 0) {
            mTSFinger.finger[curr_slot_id].timestamp = ev->time.tv_sec;
            mTSFinger.finger[curr_slot_id].dirty = 1;

            if (ev->value >= 0)  // Down event
                mTSFinger.finger[curr_slot_id].event = TOUCH_EVENT_DOWN;
            else if (ev->value == -1)  // Up event
                mTSFinger.finger[curr_slot_id].event = TOUCH_EVENT_UP;
            else if (ev->value == -2)  // Move event
                mTSFinger.finger[curr_slot_id].event = TOUCH_EVENT_MOVE;
            TUILOGD("%s::%d - abs_id - %d", __func__, __LINE__, ev->value);
        }
        if (ev->type == 3 && ev->code == 0x35) {
            mTSFinger.finger[curr_slot_id].x = ev->value * mRatio.ratioX;
            TUILOGD("%s::%d - x - %d", __func__, __LINE__, mTSFinger.finger[curr_slot_id].x);
        }
        if (ev->type == 3 && ev->code == 0x36) {
            mTSFinger.finger[curr_slot_id].y = ev->value * mRatio.ratioY;
            TUILOGD("%s::%d - y - %d", __func__, __LINE__, mTSFinger.finger[curr_slot_id].y);
        }
        if (ev->type == 0 && ev->code == 0 && ev->value == 0) {
            /* EV_SYN event received. */
            int8_t *pBuf = nullptr;
            pBuf = reinterpret_cast<int8_t *>(&mTSFinger);
            buf.assign(pBuf, pBuf + sizeof(mTSFinger));
            bufSize = buf.size();

            TUILOGD("%s::%d  - Finger Data : [%u]: %u, (%u, %u),"
                    "%" PRIu64 ", %u",
                    __func__, __LINE__, curr_slot_id, mTSFinger.finger[curr_slot_id].event,
                    mTSFinger.finger[curr_slot_id].x, mTSFinger.finger[curr_slot_id].y,
                    mTSFinger.finger[curr_slot_id].timestamp,
                    mTSFinger.finger[curr_slot_id].dirty);
            break;
        }
    } while (1);

    TUI_CHECK_COND(ret == ITrustedInput::SUCCESS,
                   ITrustedInput::ERROR_INVALID_FINGER_NUM);

errorExit:
    return ret;
}

/* Description : This API performs following functionalities.
* Functionality 1 - Finds the correct device node to get the touchdata file
* descriptor.
* Functionality 2 - Gets the file descriptor for the touch data.
* Functionality 3 - Computes the screen to touch ratio.
*
* In : screenX : screen X value from the Coreservice.
*      screenY : screen Y value from the Coreservice.
*
* Return : ITrustedInput::ERROR_READ_DEVICENODE,
*          ITrustedInput::ERROR_GET_DEVICENODE,
*          ITrustedInput::ERROR_IOCTL_TOUCHDATAFD,
*          ITrustedInput::ERROR_INVALID_TOUCH_DEVICENODE,
*          ITrustedInput::ERROR_INVALID_SCREEN_X_Y_INFO,
*          ITrustedInput::ERROR_INVALID_MAX_X_Y,
*          ITrustedInput::SUCCESS
*
*/
int32_t TouchDevice::openTouchDeviceFd(const uint32_t screenX,
                                       const uint32_t screenY)
{
    int32_t ret = ITrustedInput::SUCCESS;
    string deviceNode = "";
    string touchDataLoc = "";
    char deviceNodeName[64] = "";
    ret = findTouchDeviceNode(deviceNode);
    TUI_CHECK_COND(ret == ITrustedInput::SUCCESS, ret);
    TUILOGD("%s::%d - deviceNode - %s", __func__, __LINE__, deviceNode.c_str());
    touchDataLoc = findDeviceNode + deviceNode;

    TUILOGD("%s::%d - Getting touchdata fd, Filename : %s sizeof(inputEvent) : %d",
        __func__, __LINE__, touchDataLoc.c_str(), sizeof(inputEvent));

    /* Get the touchdata fd  */
    mTouchDeviceFd = open(touchDataLoc.c_str(), O_RDONLY);
    if (mTouchDeviceFd < 0) {
        TUILOGE("%s::%d - mTouchDeviceFd open failed - %s", __func__, __LINE__,
                strerror(-errno));
        TUI_CHECK_COND(mTouchDeviceFd >= 0,
                       ITrustedInput::ERROR_OPEN_TOUCHDATAFD);
    }
    /* Validate touch fd by doing ioctl for device name */
    ret = ioctl(mTouchDeviceFd, EVIOCGNAME(sizeof(deviceNodeName)),
                deviceNodeName);
    TUI_CHECK_COND(ret >= 0, ITrustedInput::ERROR_IOCTL_TOUCHDATAFD);
    TUILOGD("%s::%d - deviceNodeName - %s", __func__, __LINE__, deviceNodeName);

    /* Compute the screen to touch ratio */
    ret = computeTouchToScreenRatio(screenX, screenY);
    TUI_CHECK_COND(ret == ITrustedInput::SUCCESS,
                   ITrustedInput::ERROR_IOCTL_TOUCHDATAFD);

    secure_memset(&mTSFinger, 0, sizeof(mTSFinger));
errorExit:
    return ret;
}

/* Description : This API return the touch device fd.
*
* Return : touch device fd
*/
int32_t TouchDevice::getTouchDeviceFd() const
{
    return mTouchDeviceFd;
}

/* Description : This API closes the touch device fd.
*
*/
void TouchDevice::closeTouchDeviceFd()
{
    if (mTouchDeviceFd >= 0) {
        close(mTouchDeviceFd);
        mTouchDeviceFd = -1;
    }
}
