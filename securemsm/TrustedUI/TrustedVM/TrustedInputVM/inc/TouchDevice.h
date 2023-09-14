/*===================================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

#ifndef __PROCESSTOUCHDATA_H__
#define __PROCESSTOUCHDATA_H__

#include <vector>

#include "TouchApiDefs.h"

using namespace std;

/* Read the touchevent into this struct */
typedef struct {
    struct timeval time;
    unsigned short type;
    unsigned short code;
    int value;
} inputEvent;

/* struct to store touch data */
typedef struct {
    int x;
    int y;
    int type;
    int abs_id;
    int slot_id;
} touchEvent;

typedef struct {
    int maxX;
    int maxY;
    int screenX;
    int screenY;
    float ratioX;
    float ratioY;
} touchScreenRatio;

static const vector<string> controlFilelocations = {
        /* Lahaina MTP touch driver location */
        "/sys/devices/platform/soc/990000.i2c/i2c-0/0-0049",
        /* Lahaina QRD touch driver location */
        "/sys/devices/platform/soc/990000.i2c/i2c-0/0-0038",
        /* Shima MTP touch driver location */
        "/sys/devices/platform/soc/988000.i2c/i2c-0/0-0038",
        /* Yupik MTP touch driver location*/
        "/sys/devices/platform/soc/a94000.i2c/i2c-0/0-0062"
};

class TouchDevice
{
   private:
    enum mTouchEvents {
        TOUCH_EVENT_NONE = 0,
        TOUCH_EVENT_DOWN,
        TOUCH_EVENT_MOVE,
        TOUCH_EVENT_UP = 4
    };
    inline static const string findDeviceNode = "/dev/input/event";
    inline static const string deviceNodeCmd = "cat /proc/bus/input/devices";
    inline static const string searchDeviceNode = "event";

    /*  struct to store the finger data */
    tsFingerData mTSFinger;
    /* Store the current finger slot id */
    uint32_t curr_slot_id = 0;
    /* struct to compute the x and y ratio */
    touchScreenRatio mRatio;
    int32_t mTouchDeviceFd = -1;

    /******************************************************************************
    *                  Private Class Function Definitions
    *****************************************************************************/

    /* Description : This API iterates through all the control device nodes
    * and return true if string present else false.
    *
    * In : str : strings from "cat /proc/bus/input/devices".
    *
    * Return : True,
    *          Flase
    */

    bool isControlFilelocation(string str);

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

    int32_t findTouchDeviceNode(string &deviceNode);

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
    int32_t computeTouchToScreenRatio(const uint32_t screenX,
                                      const uint32_t screenY);

    /******************************************************************************
    *                        Public Class Function Definitions
    *****************************************************************************/
   public:
    /* Description : This API reads touch events in Standard Linux defined
    * struct format "input_event" and processes the touch events in the
    * "fingerData" struct format and fills up the buffer along with buffer
    * size and returns it to the VMClientApp.
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
    int32_t readTouchData(vector<int8_t> &buf, size_t &bufSize);

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
    int32_t openTouchDeviceFd(const uint32_t screenX, const uint32_t screenY);

    /* Description : This API return the touch device fd.
    *
    * Return : touch device fd
    */
    int32_t getTouchDeviceFd() const;

    /* Description : This API closes the touch device fd.
    *
    */
    void closeTouchDeviceFd();
};

#endif /*__PROCESSTOUCHDATA_H__ */
