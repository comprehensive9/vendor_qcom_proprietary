/*===================================================================================
  Copyright (c) 2020 - 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

#ifndef __TOUCHINPUTVM_H__
#define __TOUCHINPUTVM_H__

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/

#include <vendor/qti/hardware/trustedui/1.1/ITrustedInput.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {
namespace V1_2 {
namespace implementation {

using ::vendor::qti::hardware::trustedui::V1_1::ITrustedInput;
using ::vendor::qti::hardware::trustedui::V1_0::ITrustedInputCallback;
using ::vendor::qti::hardware::trustedui::V1_0::Response;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::hidl_string;
using namespace std;

struct TouchInput : public ITrustedInput {
private:
  static std::mutex sLock;
  static std::shared_ptr<TouchInput> sInstance;
  bool stSession = false;
  int mControlFd = -1;
  int32_t mTouchControllerId = -1;
  string mControlFile = "";
  string mTouchTypeFile = "";
  string mDisplayType = "";
  inline static const vector<string> controlFilelocations = {
        /* Lahaina MTP touch driver location */
        "/sys/devices/platform/soc/990000.i2c/i2c-0/0-0049",
        /* Lahaina QRD touch driver location */
        "/sys/devices/platform/soc/990000.i2c/i2c-0/0-0038",
        /* Shima MTP touch driver location */
        "/sys/devices/platform/soc/988000.i2c/i2c-1/1-0038",
        /* Yupik MTP touch driver location */
        "/sys/devices/platform/soc/a94000.i2c/i2c-2/2-0062"};
  inline static const vector<string> controlFileName = {"trusted_touch_type", "trusted_touch_enable"};
  inline static const string defaultDisplayType = "primary";

 /******************************************************************************
 *                        Private Class Function Definitions
 *****************************************************************************/

  /* Description :  This API returns the controller id.
   *
   * In : str : Control FileName.
   *
   * Out : mTouchControllerId : controller id.
   *
   */
  void getControllerId(char *str, int32_t &mTouchControllerId);

    /* Description :  This API searches for the control filename.
     *
     * Return :  -ENOENT,
     *           -EINVAL
     *
     */

    int32_t touchSearchDevice();

  /* Description :  This API assigns touch control to LE VM
   *
   * Return :  -errno,
   *           -1
   *
   */

  int32_t assignTouchControl();

  /* Description :  This API validates display type.
   *
   * Return :  0,
   *          -1
   *
   */
  int32_t validateDisplayType();

 /******************************************************************************
 *                        Public Class Function Definitions
 *****************************************************************************/
public:
  /* Methods from ::vendor::qti::hardware::trustedui::V1_0::ITrustedInput
   * follow. */
  /* Description :  This API starts the TouchInput session in Android
   *
   * hidl Return :  Response::TUI_SUCCESS along with mTouchControllerId
   *                Response::TUI_FAILURE along with mTouchControllerId
   *
   */
  Return<void> init(const sp<ITrustedInputCallback> &cb,
                    init_cb _hidl_cb) override;

  /* Description :  This API terminates the TouchInput session in Android
  *
  * Return :  Response::TUI_SUCCESS,
  *           Response::TUI_FAILURE
  *
  */
  Return<Response> terminate() override;
  /* Description :  This API always returns "Response::TUI_FAILURE",
   * as this is mainly executed by LEVM TouchInput.
   *
   * Return :   Response::TUI_FAILURE
   *
   *
   */
  Return<Response> getInput(int32_t timeout) override;

  /* Methods from ::vendor::qti::hardware::trustedui::V1_1::ITrustedInput
   * follow. */

  /* Description :  This API starts the TouchInput session in Android
   *                and verifies the display type.
   *
   * hidl Return :  Response::TUI_SUCCESS along with mTouchControllerId
   *                Response::TUI_FAILURE along with mTouchControllerId
   *
   */
  Return<void> init2(const sp<ITrustedInputCallback> &cb,
                     const hidl_string &displayType,
                     init2_cb _hidl_cb) override;

  /* Methods from ::android::hidl::base::V1_0::IBase follow. */
  static std::shared_ptr<TouchInput> getInstance();
};

} /* namespace implementation */
} /* namespace V1_2 */
} /* namespace trustedui */
} /* namespace hardware */
} /* namespace qti */
} /* namespace vendor */

#endif /* __TOUCHINPUTVM_H__ */
