/******************************************************************************
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include "HalServiceImplFactory.h"
#include "hidl_impl/1.2/lpa_service_1_2.h"

#undef TAG
#define TAG "LpaService"

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_2 {
namespace implementation {

template <>
const HalServiceImplVersion& UimLpaImpl<V1_2::IUimLpa>::getVersion() {
  static HalServiceImplVersion version(1, 2);
  return version;
}

static void __attribute__((constructor)) registerUimLpaImpl_1_2();
void registerUimLpaImpl_1_2() {
  QCRIL_LOG_INFO("Calling registerUimLpaImpl_1_2");
  getHalServiceImplFactory<LpaServiceBase>().registerImpl<UimLpaImpl<V1_2::IUimLpa>>();
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

