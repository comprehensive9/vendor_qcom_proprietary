/**
 * Copyright (c) 2018, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "SecureElement.h"
#include "QPayAdapter-apdu.h"
#include "QPayAdapter.h"

namespace android {
namespace hardware {
namespace secure_element {
namespace V1_2 {
namespace implementation {

static std::mutex m;
static sp<ISecureElementHalCallback> clientCB = NULL;
static sp<ISecureElementHalCallbackV1_1> clientCBV1_1 = NULL;

// Methods from ::android::hardware::secure_element::V1_0::ISecureElement follow.
Return<void> SecureElement::init(const
    sp<ISecureElementHalCallback> &clientCallback) {
  int result = qpay_open();
  if (result == QPAY_SUCCESS) {
    clientCB = clientCallback;
    clientCB->onStateChange(true);
    return Void();
  }
  ALOGE("SecureElement::init return FAILED");
  return Void();
}

Return<void> SecureElement::getAtr(getAtr_cb _hidl_cb) {
  // There's no ATR available on the p61
  hidl_vec<uint8_t> response;
  _hidl_cb(response);
  return Void();
}

Return<bool> SecureElement::isCardPresent() {
  int result = qpay_open();
  if (result == QPAY_SUCCESS) {
    return true;
  } else if (clientCB != NULL) {
    clientCB->onStateChange(false);
  } else if (clientCBV1_1 != NULL) {
    clientCBV1_1->onStateChange(false);
  }
  return false;
}

Return<void> SecureElement::transmit(const hidl_vec<uint8_t> &data,
                                     transmit_cb _hidl_cb) {
  std::lock_guard<std::mutex> lock(m);
  uint8_t rApdu[QPAY_MAX_APDU_SIZE] = {};
  int rLen = 0;
  hidl_vec<uint8_t> respApdu;

  int result = transmitApdu(data, rApdu, &rLen);

  if (result == QPAY_SUCCESS) {
    respApdu.resize(rLen);
    memscpy(&respApdu[0], rLen, rApdu, rLen);
    print_text("Received :", &respApdu[0], rLen);
  } else {
      ALOGE("Transmit FAILED !");
  }
  _hidl_cb(respApdu);
  return Void();
}


static Return<SecureElementStatus> doCloseChannel(uint8_t channelNumber) {
  int result = closeChannel(channelNumber);
   if (result == QPAY_SUCCESS) {
    return SecureElementStatus::SUCCESS;
  }

  ALOGE("SecureElement::closeChannel FAILED");
  return SecureElementStatus::FAILED;
}

Return<void> SecureElement::openLogicalChannel(const hidl_vec<uint8_t> &aid,
    uint8_t p2, openLogicalChannel_cb _hidl_cb) {
  std::lock_guard<std::mutex> lock(m);
  uint8_t rApdu[QPAY_MAX_APDU_SIZE] = {};
  SecureElementStatus status = SecureElementStatus::FAILED;
  LogicalChannelResponse resApduBuff;
  memset(&resApduBuff, 0x00, sizeof(resApduBuff));
  resApduBuff.channelNumber = 0xff;
  int rLen = 0;
  int cNumber = 0;
  int result = openChannel(aid, p2, &cNumber, rApdu, &rLen, false);

  if ((result == QPAY_SUCCESS) && (rLen > 1)) {
    resApduBuff.channelNumber = cNumber;
    ALOGE("channel number passed to OMAPI = %d", resApduBuff.channelNumber);
    resApduBuff.selectResponse.resize(rLen);
    memscpy(&resApduBuff.selectResponse[0], rLen, &rApdu[1], rLen);
    print_text("OpenLogical Success : Select Response :",
               &resApduBuff.selectResponse[0], rLen);
    status = SecureElementStatus::SUCCESS;
  } else if (result == QPAY_ERROR_OUT_OF_MEMORY) {
    ALOGE("SecureElement::OPENLOGICAL - No more channel available");
    resApduBuff.channelNumber = 0xff;
    status = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
  } else if (result == QPAY_ERROR_ITEM_NOT_FOUND) {
    ALOGE("SecureElement::OPENLOGICAL - Applet not found");
    resApduBuff.channelNumber = cNumber;
    status = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
    doCloseChannel(resApduBuff.channelNumber);
  }

  _hidl_cb(resApduBuff, status);
  return Void();
}

Return<void> SecureElement::openBasicChannel(const hidl_vec<uint8_t> &aid,
    uint8_t p2, openBasicChannel_cb _hidl_cb) {
  std::lock_guard<std::mutex> lock(m);
  uint8_t rApdu[QPAY_MAX_APDU_SIZE] = {};
  SecureElementStatus status = SecureElementStatus::FAILED;
  hidl_vec<uint8_t> resApduBuff;
  int rLen = 0;
  int cNumber = 0;
  int result = openChannel(aid, p2, &cNumber, rApdu, &rLen, true);

  if (result == QPAY_SUCCESS) {
    resApduBuff.resize(rLen);
    memscpy(&resApduBuff[0], rLen, &rApdu[1], rLen);
    print_text("OpenBasic Success : Select Response :", &resApduBuff[0], rLen);
    status = SecureElementStatus::SUCCESS;
  } else if (result == QPAY_ERROR_OUT_OF_MEMORY) {
    ALOGE("SecureElement::OpenBasic - No more channel available");
    status = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
  } else if (result == QPAY_ERROR_ITEM_NOT_FOUND) {
    ALOGE("SecureElement::OpenBasic - Applet not found");
    status = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
    doCloseChannel(0);
  }

  _hidl_cb(resApduBuff, status);
  return Void();
}

Return<SecureElementStatus> SecureElement::closeChannel(uint8_t channelNumber) {
  std::lock_guard<std::mutex> lock(m);
  return doCloseChannel(channelNumber);
}

ISecureElement *HIDL_FETCH_ISecureElement(const char * /* name */) {
  return new SecureElement();
}

// Methods from ::android::hardware::secure_element::V1_1::ISecureElement follow.
Return<void> SecureElement::init_1_1(const
    sp<ISecureElementHalCallbackV1_1> &clientCallback) {
  int result = qpay_open();
  if (result == QPAY_SUCCESS) {
    clientCBV1_1 = clientCallback;
    clientCBV1_1->onStateChange_1_1(true, "QPAY eSE has been initialised");
    return Void();
  }
  ALOGE("%s: QPAY eSE initialisation FAILED", __func__);
  return Void();
}

// Methods from ::android::hardware::secure_element::V1_2::ISecureElement follow.
Return<SecureElementStatus> SecureElement::reset() {
  int result = QPAY_SUCCESS;

  clientCBV1_1->onStateChange_1_1(false, "re-initilaise QPAY eSE");
  result = qpay_close();
  if (result == QPAY_SUCCESS) {
    if (qpay_open() == QPAY_SUCCESS) {
      clientCBV1_1->onStateChange_1_1(true, "QPAY eSE re-initialised");
      return SecureElementStatus::SUCCESS;
    }
  }

  ALOGE("%s: QPAY eSE re-initialisation FAILED", __func__);
  return SecureElementStatus::FAILED;
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace secure_element
}  // namespace hardware
}  // namespace android
