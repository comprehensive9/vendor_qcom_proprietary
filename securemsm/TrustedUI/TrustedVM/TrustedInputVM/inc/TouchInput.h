/*===================================================================================
  Copyright (c) 2020 - 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

#ifndef __TOUCHINPUT_H__
#define __TOUCHINPUT_H__

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include "ITrustedInput.hpp"
#include "ITrustedInput_invoke.hpp"
#include "TouchDevice.h"
#include "object.h"

using namespace std;

enum ThreadState : uint32_t {
    INVALID = 0,
    WAITING_ON_APP_REQ = 1,
    WAITING_ON_INPUT_EVENT = 2
};

class CTouchInput : public TrustedInputImplBase
{
   private:
    /******************************************************************************
    *                        Private Class Member Variables
    *****************************************************************************/
    /* Number of events to be polled */
    enum mPollFDs { TOUCH_EVENT, ABORT_EVENT, MAX_EVENTS };
    /* Unique pointer to process touch data */
    unique_ptr<TouchDevice> mTouchDevice = make_unique<TouchDevice>();
    /* Thread to wait on for getInput */
    shared_ptr<thread> mThread = nullptr;
    /* VM ClientApp callback to notify touch events */
    TrustedInputCallBack *mTouchInputNotify = nullptr;
    /* Condition variable and Mutex to notify touchHandler */
    condition_variable mCondVar;
    mutex mMutex;
    /* Boolean variable to be true until getInput is called */
    bool mTouchSessionActive = false;
    /* Maintain thread state for synchronizing getInput call */
    ThreadState mThreadState = INVALID;
    string mControlFile = "";
    string mTouchTypeFile = "";
    /* FDs for Control, Event and Touch */
    int32_t mAbortFd = -1;
    int32_t mControlFd = -1;
    /* Timeout period in milliseconds to notify the VMClientApp*/
    int mTimeout = -1;
    inline static const std::vector<std::string> displayTypes = {"primary", "secondary"};
    std::string mDisplayType = "";
    inline static const std::vector<std::string> controlFileName = {"trusted_touch_type",
                                                                    "trusted_touch_enable"};

    /******************************************************************************
    *                        Private Class Member Functions
    *****************************************************************************/

    /* Description : This API searches for the controlfd sysfs path.
    *
    *
    * Return : -EINVAL,
    *          -ENOENT,
    *          ITrustedInput::ERROR_INVALID_DISPLAY_TYPE,
    *          ITrustedInput::SUCCESS
    *
    */
    int32_t searchControlDevice();

    /* Description : This API gets the controlfd and validates if the touch
    * driver fd can now be accessed by the TrsutedInput in LEVM.
    *
    *
    * Return : ITrustedInput::ERROR_ACCESS_CONTROLFD,
    *          ITrustedInput::ERROR_OPEN_CONTROLFD,
    *          ITrustedInput::ERROR_INVALID_DISPLAY_TYPE,
    *          ITrustedInput::SUCCESS
    *
    */
    int32_t validateDeviceAssignToVM();

    /* Description : This API polls on mTouchDataFd and mAbortFd with a timeout
    * as set in mTimeout.
    *
    *
    * Return : -errno,
    *          -ETIMEDOUT,
    *          -ECONNABORTED,
    *          ITrustedInput::SUCCESS
    *
    */

    int32_t waitForEvent();

    /* Description :  This API polls for the eventfd and touch event.
    * If there is a touch event, it notifies either
    * a) If timeout, then  it notifies the VM Client App that it couldn't get
    * the touchdata.
    * b) Notifies the VM Client App with the touch data within the timeout
    * period.
    *
    *
    * Return : -errno,
    *          -ETIMEDOUT,
    *          -ECONNABORTED,
    *          -EBUSY,
    *          ITrustedInput::ERROR_READ_FAILED,
    *          ITrustedInput::ERROR_INVALID_FINGER_NUM,
    *          ITrustedInput::SUCCESS
    *
    */
    void getTouchData();

    /* Description : This API creates a mutex lock during init and waits for the
    * getInput API to set the condition variable to poll for the touch event.
    */
    void touchHandler();

    /* Description :  This API validates display type.
    *
    * Return :  ITrustedInput::SUCCESS,
    *           ITrustedInput::ERROR_INVALID_DISPLAY_TYPE
    *
    */
    int32_t validateDisplayType();

   public:
    /******************************************************************************
     *                        Public Class Function Definitions
     *****************************************************************************/

    /* Description :  This is the mink interface API that is called by
    * CoreService to initiate the session.
    * The following functionalities are performed:
    * Functionality 1 - Here, we check if the touch driver is assigned to
    * TouchInput VM, if so, we obtain the file descriptor for Touch driver
    * and also eventfd.
    * Functionality 2 - Create a thread that conditionally waits for the
    * getInput call from VMCLientApp.
    * Functionality 3 - Retain the call-back object of VMClientApp to call
    * notifyTimeout or to notify touchdata or notify error in the previously
    * created thread.
    *
    *
    * In : TouchInputCBO : Callback object of VMClientAapp.
    *      displayType : displayType value from the Coreservice.
    *      screenX : screen X value from the Coreservice.
    *      screenY : screen Y value from the Coreservice.
    *
    * Return : ITrustedInput::ERROR_ACCESS_CONTROLFD,
    *          ITrustedInput::ERROR_OPEN_CONTROLFD,
    *          ITrustedInput::ERROR_READ_DEVICENODE,
    *          ITrustedInput::ERROR_GET_DEVICENODE,
    *          ITrustedInput::ERROR_IOCTL_TOUCHDATAFD,
    *          ITrustedInput::ERROR_OPEN_TOUCHDATAFD,
    *          ITrustedInput::ERROR_REGISTER_CBO,
    *          ITrustedInput::ERROR_CREATE_THREAD,
    *          ITrustedInput::ERROR_INVALID_TOUCH_DEVICENODE,
    *          ITrustedInput::ERROR_TOUCH_SESSION_ACTIVE,
    *          ITrustedInput::ERROR_INVALID_DISPLAY_TYPE,
    *          ITrustedInput::SUCCESS
    *
    */
    int32_t init(Object TouchInputCBO, uint32_t displayType, uint32_t screenX, uint32_t screenY);

    /* Description :  This is the mink interface API called by the VMClientApp.
    * This API notifies the thread that was created in init() call, which then
    * polls for the touch data using touchdata file descriptor and also polls
    * for eventfd for any external user-application interrupts.
    * When a touch event occurs, it calls "TouchDevice" class APIs to read
    * the touchevent and processes corresponding finger number data.Eventually
    * it
    * either passes the touch data buffer or it notifies the App that it has
    * timed
    * out.
    * Basically, touch data is asynchronously shared with VMClientApp using
    * call-back object.
    *
    *
    * In : timeout : timeout period to notify touchdata to the VMClientApp.
    *
    * Return :  ITrustedInput::SUCCESS,
    *           ITrustedInput::ERROR_POLL_IS_STILL_ACTIVE
    *
    */
    int32_t getInput(int32_t timeout);

    /* Description :  This is the mink interface API called by the CoreService
    * to terminate the session.Here, the following functionalities are
    * performed.
    * Functionality 1 - Control of the touch driver access is returned back to
    * Android.
    * Functionality 2 - VMClientApp call-back object is released.
    * Functionality 3 - Thread is joined.
    * Functionality 4 - Close the touchfd and eventfd.
    *
    *
    * Return :  ITrustedInput::ERROR_WRITE_ABORTFD,
    *           ITrustedInput::ERROR_WRITE_CONTROLFD,
    *           ITrustedInput::SUCCESS
    *
    */
    int32_t terminate();

    CTouchInput() { TUILOGD("CTouchInput: Constructor");}
    ~CTouchInput() { TUILOGD("~CTouchInput: Destructor");}
};

#endif /*__TOUCHINPUT_H__ */
