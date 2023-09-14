/**
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSORFACTORY_H
#define VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSORFACTORY_H

#include <vendor/qti/hardware/secureprocessor/device/1.0/ISecureProcessor.h>
#include <string>

namespace vendor
{
namespace qti
{
namespace hardware
{
namespace secureprocessor
{
namespace device
{
namespace V1_0
{
namespace implementation
{
using namespace std;

class SecureProcessorFactory
{
   public:
    static ISecureProcessor *CreateSecureProcessor(const string &type);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace device
}  // namespace secureprocessor
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_SECUREPROCESSOR_V1_0_SECUREPROCESSORFACTORY_H
