/******************************************************************************
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include "HalServiceImplFactory.h"
#include "hidl_impl/1.1/lpa_service_1_1.h"

#undef TAG
#define TAG "LpaService"

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_1 {
namespace implementation {

template <>
const HalServiceImplVersion& UimLpaImpl<V1_1::IUimLpa>::getVersion() {
  static HalServiceImplVersion version(1, 1);
  return version;
}

static void __attribute__((constructor)) registerUimLpaImpl_1_1();
void registerUimLpaImpl_1_1() {
  QCRIL_LOG_INFO("Calling registerUimLpaImpl_1_1");
  getHalServiceImplFactory<LpaServiceBase>().registerImpl<UimLpaImpl<V1_1::IUimLpa>>();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

