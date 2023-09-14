/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright 2013, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ANDROID_MEDIA_MEDIASECUREPROCESSOR_H_
#define _ANDROID_MEDIA_MEDIASECUREPROCESSOR_H_

#include "jni.h"

#include <vendor/qti/hardware/secureprocessor/config/1.0/types.h>
#include <vendor/qti/hardware/secureprocessor/device/1.0/ISecureProcessor.h>
#include <media/stagefright/foundation/ABase.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/RefBase.h>

using vendor::qti::hardware::secureprocessor::device::V1_0::ISecureProcessor;
using vendor::qti::hardware::secureprocessor::common::V1_0::ErrorCode;
using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigTag;
using android::hardware::hidl_death_recipient;
using android::hidl::base::V1_0::IBase;
using android::hardware::hidl_vec;
using android::hardware::hidl_array;


namespace android
{
struct JMediaSecureProcessor : public RefBase {
   public:
    JMediaSecureProcessor(JNIEnv *env, jobject thiz, const char *service);
    static sp<ISecureProcessor> GetSecureProcessor(JNIEnv *env, jobject obj);

   protected:
    virtual ~JMediaSecureProcessor();

   private:
    jclass mClass;
    jweak mObject;
    sp<ISecureProcessor> mSecureProcessor;

   struct serviceDeathRecipient : public hidl_death_recipient {
       virtual void serviceDied(uint64_t cookie __unused,
          const android::wp<IBase>& who __unused) {
           ALOGE("%s : secureprocessor service died", __func__);
           delete this;
       }
   };

   sp<serviceDeathRecipient> death_recepient = NULL;
   sp<ISecureProcessor> MakeSecureProcessor(const char *service);
   DISALLOW_EVIL_CONSTRUCTORS(JMediaSecureProcessor);
};

}  // namespace android

#endif  // _ANDROID_MEDIA_MEDIASECUREPROCESSOR_H_
