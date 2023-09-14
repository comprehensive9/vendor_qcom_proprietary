/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2012 The Android Open Source Project
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

/**
 * The concept for configuration data marshaling and
 * de-marshaling is borrowed from AOSP Camera MetaData.
 * Reference:
 *   platform/system/media/camera/src/camera_metadata.h[/c]
 *
 */

// #define LOG_NDEBUG 0

#define LOG_TAG "MediaSecureProcessor-JNI"

#include "mediaSecureProcessor.h"
#include <android_runtime/AndroidRuntime.h>
#include <android_runtime/Log.h>
#include "jni.h"

#include <android_runtime/android_hardware_HardwareBuffer.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <cutils/native_handle.h>
#include <cutils/properties.h>
#include <media/stagefright/foundation/ADebug.h>
#include <utils/Log.h>
#include <vndk/hardware_buffer.h>

#include "SecureProcessorConfig.h"
#include "SecureProcessorUtils.h"

using vendor::qti::hardware::secureprocessor::config::V1_0::ConfigType;
using vendor::qti::hardware::secureprocessor::common::V1_0::SecureProcessorConfig;
using vendor::qti::hardware::secureprocessor::common::V1_0::SecureProcessorCfgBuf;

/**
 * MediaSecureProcessorConfig map:
 *   The map is used for converting configuration strings
 *   to corresponding HAL defined config TAGs.
 */
std::map<std::string, int32_t> MediaSecureProcessorConfigMap = {
    {"secureprocessor.image.config.camera_id",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_CAMERA_ID)},
    {"secureprocessor.image.config.frame_number",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_NUMBER)},
    {"secureprocessor.image.config.timestamp",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_TIMESTAMP)},
    {"secureprocessor.image.config.frame_buffer_width",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_WIDTH)},
    {"secureprocessor.image.config.frame_buffer_height",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_HEIGHT)},
    {"secureprocessor.image.config.frame_buffer_stride",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_STRIDE)},
    {"secureprocessor.image.config.frame_buffer_format",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_FORMAT)},
    {"secureprocessor.session.config.num_sensors",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_NUM_SENSOR)},
    {"secureprocessor.session.config.usecase_id",
     ((int32_t)ConfigTag::SECURE_PROCESSOR_SESSION_CONFIG_USECASE_IDENTIFIER)}};

namespace android
{
#define CALL_OBJECT_METHOD(var, obj, mid)  \
    var = env->CallObjectMethod(obj, mid); \
    LOG_FATAL_IF(!(var), "Unable to call object method: " mid);

#define GET_METHOD_ID(var, clazz, fieldName, fieldDescriptor)  \
    var = env->GetMethodID(clazz, fieldName, fieldDescriptor); \
    LOG_FATAL_IF(!(var), "Unable to find method: " fieldName);

#define GET_FIELD_ID(var, clazz, fieldName, fieldDescriptor)  \
    var = env->GetFieldID(clazz, fieldName, fieldDescriptor); \
    LOG_FATAL_IF(!(var), "Unable to find field: " fieldName);

struct MapFields {
    jmethodID entrySet;
    jmethodID iterator;
    jmethodID hasNext;
    jmethodID next;
    jmethodID getKey;
    jmethodID getValue;
    jmethodID stringval;
    jmethodID intval;
    jobject obj_entryset;
    jobject obj_iter;
};

static MapFields mField;
static jfieldID gContext;

static sp<JMediaSecureProcessor> getMediaSecureProcessor(JNIEnv *env,
                                                         jobject thiz)
{
    return reinterpret_cast<JMediaSecureProcessor *>(
        env->GetLongField(thiz, gContext));
}

JMediaSecureProcessor::JMediaSecureProcessor(JNIEnv *env, jobject thiz,
                                             const char *service)
{
    jclass clazz = env->GetObjectClass(thiz);
    CHECK(clazz != NULL);

    mClass = (jclass)env->NewGlobalRef(clazz);
    mObject = env->NewWeakGlobalRef(thiz);
    mSecureProcessor = MakeSecureProcessor(service);
    if (mSecureProcessor == NULL) {
        ALOGE("Failed to get SecureProcessor HAL service handle.");
    }
}

JMediaSecureProcessor::~JMediaSecureProcessor()
{
    if (death_recepient != NULL) {
        mSecureProcessor->unlinkToDeath(death_recepient);
        death_recepient.clear();
    }
    mSecureProcessor.clear();
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    env->DeleteWeakGlobalRef(mObject);
    mObject = NULL;
    env->DeleteGlobalRef(mClass);
    mClass = NULL;
}

sp<ISecureProcessor> JMediaSecureProcessor::MakeSecureProcessor(
    const char *service)
{
    sp<ISecureProcessor> secureprocessor =
        ISecureProcessor::getService(service);
    if (secureprocessor == NULL) {
        ALOGE("GetService failed for %s", service);
    }

    death_recepient = new serviceDeathRecipient();
    if (death_recepient == NULL) {
        ALOGE("%s : Failed to register death recepient", __func__);
    } else {
        hardware::Return<bool> linked =
            secureprocessor->linkToDeath(death_recepient, 0);
        if (!linked) {
            ALOGE("%s: Unable to link to secure processor death notification",
                  __func__);
        } else if (!linked.isOk()) {
            ALOGE("%s: Transaction error in linking", __func__);
        }
    }

    return secureprocessor;
}

sp<ISecureProcessor> JMediaSecureProcessor::GetSecureProcessor(JNIEnv *env,
                                                               jobject obj)
{
    jclass clazz =
        env->FindClass("com/qti/media/secureprocessor/MediaSecureProcessor");
    CHECK(clazz != NULL);

    if (!env->IsInstanceOf(obj, clazz)) {
        ALOGE("Cannot find MediaSecureProcessor instance");
        return NULL;
    }

    sp<JMediaSecureProcessor> jMediaSP = getMediaSecureProcessor(env, obj);
    if (jMediaSP == NULL) {
        ALOGE("Cannot get JMediaSecureProcessor instance");
        return NULL;
    }

    return jMediaSP->mSecureProcessor;
}

static String8 JStringToString8(JNIEnv *env, jstring const &jstr)
{
    String8 result;

    const char *str = env->GetStringUTFChars(jstr, NULL);
    if (str) {
        result = str;
        env->ReleaseStringUTFChars(jstr, str);
    }
    return result;
}

static SecureProcessorConfig *GetNativeHandle(JNIEnv *env, jobject thiz)
{
    jclass cls = env->FindClass(
        "com/qti/media/secureprocessor/MediaSecureProcessorConfig");
    jmethodID nMethodID = NULL;
    GET_METHOD_ID(nMethodID, cls, "getNativehandle", "()J");
    jlong nHandle = env->CallLongMethod(thiz, nMethodID);
    return reinterpret_cast<SecureProcessorConfig *>(nHandle);
}

static void PopulateMapFields(JNIEnv *env, jobject map)
{
    jclass clazz = env->GetObjectClass(map);
    CHECK(clazz != NULL);
    GET_METHOD_ID(mField.entrySet, clazz, "entrySet", "()Ljava/util/Set;");

    jclass entryset = env->FindClass("java/util/Set");
    GET_METHOD_ID(mField.iterator, entryset, "iterator",
                  "()Ljava/util/Iterator;");

    jclass iter = env->FindClass("java/util/Iterator");
    GET_METHOD_ID(mField.hasNext, iter, "hasNext", "()Z");
    GET_METHOD_ID(mField.next, iter, "next", "()Ljava/lang/Object;");

    jclass centry = env->FindClass("java/util/Map$Entry");
    GET_METHOD_ID(mField.getKey, centry, "getKey", "()Ljava/lang/Object;");
    GET_METHOD_ID(mField.getValue, centry, "getValue", "()Ljava/lang/Object;");

    jclass cstring = env->FindClass("java/lang/String");
    GET_METHOD_ID(mField.stringval, cstring, "toString",
                  "()Ljava/lang/String;");

    jclass cint = env->FindClass("java/lang/Integer");
    GET_METHOD_ID(mField.intval, cint, "intValue", "()I");

    CALL_OBJECT_METHOD(mField.obj_entryset, map, mField.entrySet);
    CALL_OBJECT_METHOD(mField.obj_iter, mField.obj_entryset, mField.iterator);
}

static jobject PopulateReturnConfig(JNIEnv *env,
                                    SecureProcessorCfgBuf outConfig)
{
    SecureProcessorConfig *outConfigBuf = nullptr;

    jclass cls = env->FindClass(
        "com/qti/media/secureprocessor/MediaSecureProcessorConfig");
    jmethodID constructor = NULL;
    GET_METHOD_ID(constructor, cls, "<init>", "()V");
    jobject obj = env->NewObject(cls, constructor);

    convertFromHidl(&outConfig, &outConfigBuf);
    jlong oHandle = reinterpret_cast<jlong>(outConfigBuf);
    jmethodID nMethodID = NULL;
    GET_METHOD_ID(nMethodID, cls, "setNativehandle", "(J)I");
    jint result = env->CallIntMethod(obj, nMethodID, oHandle);
    if (result < 0) {
        ALOGE("Failed to set the native handle on MediaSecureProcessorConfig");
        return nullptr;
    }
    return obj;
}

}  // namespace android

using namespace android;

template <typename T>
static jobject updateConfigEntryBuffer(JNIEnv *env, T buffer)
{
    jclass cls = env->FindClass(
        "com/qti/media/secureprocessor/MediaSecureProcessorConfig$ConfigEntry");

    if (buffer.type == (int32_t)ConfigType::INT32) {
        jmethodID constructor = NULL;
        GET_METHOD_ID(constructor, cls, "<init>", "(III[I)V");
        jintArray intVal = env->NewIntArray(buffer.count);
        env->SetIntArrayRegion(intVal, 0, (buffer.count), (buffer.data.i32));
        jobject obj = env->NewObject(cls, constructor, (buffer.tag),
                                     (buffer.type), (buffer.count), intVal);
        return obj;
    } else if (buffer.type == (int32_t)ConfigType::BYTE) {
        jmethodID constructor = NULL;
        GET_METHOD_ID(constructor, cls, "<init>", "(III[B)V");
        jbyteArray byteVal = env->NewByteArray(buffer.count);
        env->SetByteArrayRegion(byteVal, 0, (buffer.count),
                                (jbyte *)(buffer.data.u8));
        jobject obj = env->NewObject(cls, constructor, (buffer.tag),
                                     (buffer.type), (buffer.count), byteVal);
        return obj;
    } else if (buffer.type == (int32_t)ConfigType::INT64) {
        jmethodID constructor = NULL;
        GET_METHOD_ID(constructor, cls, "<init>", "(III[J)V");
        jlongArray longVal = env->NewLongArray(buffer.count);
        env->SetLongArrayRegion(longVal, 0, (buffer.count), (buffer.data.i64));
        jobject obj = env->NewObject(cls, constructor, (buffer.tag),
                                     (buffer.type), (buffer.count), longVal);
        return obj;
    }

    return nullptr;
}

static jlong mediaSecureProcessorConfig_createConfigBuffer(JNIEnv *env,
                                                           jobject thiz,
                                                           jint entryLimit,
                                                           jint dataLimit)
{
    SecureProcessorConfig *nConfig;

    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return -1;
    }

    nConfig = new SecureProcessorConfig(entryLimit, dataLimit);
    return reinterpret_cast<jlong>(nConfig);
}

static jint mediaSecureProcessorConfig_insertCustomTags(JNIEnv *env,
                                                        jobject thiz,
                                                        jobject map)
{
    int result = -1;

    if (env == NULL || thiz == NULL || map == NULL) {
        ALOGE("Invalid parameters");
        return result;
    }
    int custom_offset =
        (int32_t)ConfigTag::SECURE_PROCESSOR_CUSTOM_CONFIG_START;
    PopulateMapFields(env, map);

    bool hasNext =
        (bool)env->CallBooleanMethod(mField.obj_iter, mField.hasNext);

    while (hasNext) {
        jobject entry = NULL;
        CALL_OBJECT_METHOD(entry, mField.obj_iter, mField.next);

        jobject key = NULL;
        CALL_OBJECT_METHOD(key, entry, mField.getKey);
        jobject value = NULL;
        CALL_OBJECT_METHOD(value, entry, mField.getValue);

        jstring jstrKey = (jstring)env->CallObjectMethod(key, mField.stringval);

        const char *strKey = env->GetStringUTFChars(jstrKey, 0);
        int Keyvalue = env->CallIntMethod(value, mField.intval);

        if (Keyvalue < custom_offset) {
            ALOGE("Invalid tag value");
            return result;
        }

        MediaSecureProcessorConfigMap.insert({strKey, Keyvalue});

        hasNext = (bool)env->CallBooleanMethod(mField.obj_iter, mField.hasNext);
    }

    return 0;
}

static jint mediaSecureProcessorConfig_clear(JNIEnv *env, jobject thiz)
{
    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return -1;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present");
        return -1;
    }

    configBuf->clear();
    return 0;
}

static jboolean mediaSecureProcessorConfig_isEmpty(JNIEnv *env, jobject thiz)
{
    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return JNI_FALSE;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present");
        return JNI_FALSE;
    }

    return configBuf->isEmpty();
}

static jint mediaSecureProcessorConfig_getSize(JNIEnv *env, jobject thiz)
{
    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return 0;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present");
        return 0;
    }

    return configBuf->getSize();
}

static jboolean mediaSecureProcessorConfig_checkEntry(JNIEnv *env, jobject thiz,
                                                      jstring jtag)
{
    bool result = JNI_FALSE;
    std::map<std::string, int32_t>::iterator ptr;

    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return result;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present to check entry");
        return result;
    }

    std::string tag = (JStringToString8(env, jtag)).string();
    ptr = MediaSecureProcessorConfigMap.find(tag);

    if (ptr != MediaSecureProcessorConfigMap.end()) {
        auto buffer = configBuf->checkEntry(ptr->second);
        if (buffer) {
            result = JNI_TRUE;
        }
    } else {
        ALOGE("Invalid Tag: %s", tag.c_str());
    }

    return result;
}

static jint mediaSecureProcessorConfig_getEntryCount(JNIEnv *env, jobject thiz)
{
    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return 0;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present to get entry count");
        return 0;
    }

    return configBuf->entryCount();
}

static jint mediaSecureProcessorConfig_addEntry__ILjava_lang_String_2(
    JNIEnv *env, jobject thiz, jstring jtag, jstring jdata)
{
    status_t result = -1;
    std::map<std::string, int32_t>::iterator ptr;

    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return result;
    }

    std::string tag = (JStringToString8(env, jtag)).string();
    String8 data = JStringToString8(env, jdata);

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present to add entry");
        return result;
    }

    ptr = MediaSecureProcessorConfigMap.find(tag);
    if (ptr != MediaSecureProcessorConfigMap.end()) {
        result = configBuf->addEntry(ptr->second, data);
    } else {
        ALOGE("Not a valid Tag value: %s", tag.c_str());
    }

    return result;
}

static jint mediaSecureProcessorConfig_addEntry__I_3II(
    JNIEnv *env, jobject thiz, jstring jtag, jintArray jdata, jint jdataCount)
{
    status_t result = -1;
    std::map<std::string, int32_t>::iterator ptr;
    int32_t *data = NULL;

    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return result;
    }

    std::string tag = (JStringToString8(env, jtag)).string();
    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present to add entry");
        return result;
    }

    data = env->GetIntArrayElements(jdata, NULL);

    ptr = MediaSecureProcessorConfigMap.find(tag);
    if (ptr != MediaSecureProcessorConfigMap.end()) {
        result = configBuf->addEntry(ptr->second, data, jdataCount);
    } else {
        ALOGE("Not a valid Tag value: %s", tag.c_str());
    }

    return result;
}

static jint mediaSecureProcessorConfig_addEntry__I_3BI(
    JNIEnv *env, jobject thiz, jstring jtag, jbyteArray jdata, jint jdataCount)
{
    status_t result = -1;
    uint8_t *data = NULL;
    jboolean isCopy = JNI_FALSE;
    std::map<std::string, int32_t>::iterator ptr;

    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return result;
    }

    std::string tag = (JStringToString8(env, jtag)).string();

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present to add entry");
        return result;
    }

    jbyte *const tmpData = env->GetByteArrayElements(jdata, &isCopy);
    data = reinterpret_cast<uint8_t *>(tmpData);

    ptr = MediaSecureProcessorConfigMap.find(tag);
    if (ptr != MediaSecureProcessorConfigMap.end()) {
        result = configBuf->addEntry(ptr->second, data, jdataCount);
    } else {
        ALOGE("Not a valid Tag value: %s", tag.c_str());
    }

    return result;
}

static jint mediaSecureProcessorConfig_addEntry__I_3JI(
    JNIEnv *env, jobject thiz, jstring jtag, jlongArray jdata, jint jdataCount)
{
    status_t result = -1;
    int64_t *data = NULL;
    std::map<std::string, int32_t>::iterator ptr;

    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return result;
    }

    std::string tag = (JStringToString8(env, jtag)).string();

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present to add entry");
        return result;
    }

    data = reinterpret_cast<int64_t *>(env->GetLongArrayElements(jdata, 0));

    ptr = MediaSecureProcessorConfigMap.find(tag);
    if (ptr != MediaSecureProcessorConfigMap.end()) {
        result = configBuf->addEntry(ptr->second, data, jdataCount);
    } else {
        ALOGE("Not a valid Tag value: %s", tag.c_str());
    }

    return result;
}

static jobject mediaSecureProcessorConfig_getEntry(JNIEnv *env, jobject thiz,
                                                   jstring jtag)
{
    std::map<std::string, int32_t>::iterator ptr;

    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return NULL;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
    if (configBuf == NULL) {
        ALOGE("No configuration backing buffer present to get entry");
        return NULL;
    }

    std::string tag = (JStringToString8(env, jtag)).string();
    ptr = MediaSecureProcessorConfigMap.find(tag);

    if (ptr != MediaSecureProcessorConfigMap.end()) {
        auto buffer = configBuf->getEntry(ptr->second);
        return updateConfigEntryBuffer(env, buffer);
    } else {
        ALOGE("Not a valid Tag value: %s", tag.c_str());
    }

    return NULL;
}

static jobject mediaSecureProcessorConfig_getEntryByIndex(JNIEnv *env,
                                                          jobject thiz,
                                                          jint index)
{
    if (env == NULL || thiz == NULL) {
        ALOGE("Invalid parameters");
        return NULL;
    }

    if (index >= 0) {
        SecureProcessorConfig *configBuf = GetNativeHandle(env, thiz);
        if (configBuf == NULL) {
            ALOGE("No configuration backing buffer present to get entry");
            return NULL;
        }

        auto entry = configBuf->getEntryByIndex(index);
        return updateConfigEntryBuffer(env, entry);
    }
    return NULL;
}

static sp<JMediaSecureProcessor> setMediaSecureProcessor(
    JNIEnv *env, jobject thiz, const sp<JMediaSecureProcessor> &jMediaSP)
{
    sp<JMediaSecureProcessor> old = reinterpret_cast<JMediaSecureProcessor *>(
        env->GetLongField(thiz, gContext));
    if (jMediaSP != NULL) {
        jMediaSP->incStrong(thiz);
    }

    if (old != NULL) {
        old->decStrong(thiz);
    }

    env->SetLongField(thiz, gContext, reinterpret_cast<jlong>(jMediaSP.get()));
    return old;
}

static void mediaSecureProcessor_native_release(JNIEnv *env, jobject thiz)
{
    setMediaSecureProcessor(env, thiz, NULL);
}

static void mediaSecureProcessor_native_init(JNIEnv *env)
{
    jclass clazz =
        env->FindClass("com/qti/media/secureprocessor/MediaSecureProcessor");
    CHECK(clazz != NULL);
    GET_FIELD_ID(gContext, clazz, "mNativeContext", "J");
}

static void mediaSecureProcessor_native_setup(JNIEnv *env, jobject thiz,
                                              jstring jservice)
{
    std::string tmp = (JStringToString8(env, jservice)).string();
    const char *service = tmp.c_str();
    sp<JMediaSecureProcessor> jMediaSP =
        new JMediaSecureProcessor(env, thiz, service);
    if (jMediaSP == NULL) {
        ALOGE("Failed to instantiate JMediaSecureProcessor object.");
        return;
    }

    setMediaSecureProcessor(env, thiz, jMediaSP);
}

static jint mediaSecureProcessor_createSession(JNIEnv *env, jobject thiz)
{
    sp<ISecureProcessor> secureprocessor =
        JMediaSecureProcessor::GetSecureProcessor(env, thiz);
    if (secureprocessor == NULL) {
        ALOGE("%s: failed to get secureproccessor", __func__);
        return 0;
    }

    uint32_t sessionId = 0;
    ErrorCode retCode = ErrorCode::SECURE_PROCESSOR_OK;
    auto err = secureprocessor->createSession(
        [&retCode, &sessionId](ErrorCode code, uint32_t sid) {
            retCode = code;
            sessionId = sid;
        });

    if (!err.isOk() || retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        String8 msg("createSession failed");
        if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
            msg += ": retCode.";
        } else {
            msg.appendFormat(": general failure (retCode = %d)", retCode);
        }
        ALOGE("%s", msg.string());
        return 0;
    }

    ALOGD("%s: session created with sessionId = %d", __func__, sessionId);
    return sessionId;
}

static jobject mediaSecureProcessor_getConfig(JNIEnv *env, jobject thiz,
                                              jint jsessionId,
                                              jobject jSessionConfig)
{
    ErrorCode retCode = ErrorCode::SECURE_PROCESSOR_OK;
    SecureProcessorCfgBuf inConfig;
    SecureProcessorCfgBuf outConfig;

    if (jsessionId == 0) {
        return NULL;
    }

    sp<ISecureProcessor> secureprocessor =
        JMediaSecureProcessor::GetSecureProcessor(env, thiz);
    if (secureprocessor == NULL) {
        return NULL;
    }

    SecureProcessorConfig *inconfigBuf = GetNativeHandle(env, jSessionConfig);
    convertToHidl(inconfigBuf, &inConfig);

    auto err = secureprocessor->getConfig(
        jsessionId, inConfig,
        [&retCode, &outConfig](ErrorCode code, SecureProcessorCfgBuf outCfg) {
            retCode = code;
            outConfig = outCfg;
        });

    if (!err.isOk() || retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        String8 msg("getConfig failed");
        if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
            msg += ": retCode.";
        } else {
            msg.appendFormat(": general failure (retCode = %d)", retCode);
        }
        ALOGE("%s", msg.string());
    } else {
        jobject obj = PopulateReturnConfig(env, outConfig);
        return obj;
    }

    return NULL;
}

static jint mediaSecureProcessor_setConfig(JNIEnv *env, jobject thiz,
                                           jint jsessionId,
                                           jobject jSessionConfig)
{
    if (jsessionId == 0) {
        return -1;
    }

    sp<ISecureProcessor> secureprocessor =
        JMediaSecureProcessor::GetSecureProcessor(env, thiz);
    if (secureprocessor == NULL) {
        return -1;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, jSessionConfig);
    SecureProcessorCfgBuf inConfig;
    convertToHidl(configBuf, &inConfig);

    ErrorCode retCode = secureprocessor->setConfig(jsessionId, inConfig);

    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        ALOGE("setConfig failed");
        return -1;
    }

    return 0;
}

static jint mediaSecureProcessor_startSession(JNIEnv *env, jobject thiz,
                                              jint jsessionId)
{
    if (jsessionId == 0) {
        return -1;
    }

    sp<ISecureProcessor> secureprocessor =
        JMediaSecureProcessor::GetSecureProcessor(env, thiz);
    if (secureprocessor == NULL) {
        return -1;
    }

    ErrorCode retCode = secureprocessor->startSession(jsessionId);
    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        String8 msg("startSession failed");
        if (retCode == ErrorCode::SECURE_PROCESSOR_FAIL) {
            msg += ": session not found";
        } else {
            msg.appendFormat(": general failure (%d)", retCode);
        }
        ALOGE("%s", msg.string());
        return -1;
    }

    return 0;
}

static status_t _populateImageBufferConfig(AHardwareBuffer *hwBuf,
                                           SecureProcessorConfig *cfg)
{
    if (!hwBuf || !cfg) {
        return BAD_VALUE;
    }

    AHardwareBuffer_Desc bufDesc;
    AHardwareBuffer_describe(hwBuf, &bufDesc);

    // Set frame buffer width
    uint32_t tag =
        (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_WIDTH;
    int32_t width = bufDesc.width;
    cfg->addEntry(tag, &width, 1);

    // Set frame buffer height
    tag =
        (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_HEIGHT;
    int32_t height = bufDesc.height;
    cfg->addEntry(tag, &height, 1);

    // Set frame buffer stride
    tag =
        (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_STRIDE;
    int32_t stride = bufDesc.stride;
    cfg->addEntry(tag, &stride, 1);

    // Set frame buffer height
    tag =
        (uint32_t)ConfigTag::SECURE_PROCESSOR_IMAGE_CONFIG_FRAME_BUFFER_FORMAT;
    int32_t format = bufDesc.format;
    cfg->addEntry(tag, &format, 1);

    return OK;
}

static jobject mediaSecureProcessor_processImage(JNIEnv *env, jobject thiz,
                                                 jint jsessionId,
                                                 jobject jhwBuffer,
                                                 jobject jImageConfig)
{
    ErrorCode retCode = ErrorCode::SECURE_PROCESSOR_OK;
    SecureProcessorCfgBuf HIDLInCfg;
    SecureProcessorCfgBuf HIDLOutCfg;

    if (jsessionId == 0) {
        return NULL;
    }

    sp<ISecureProcessor> secureprocessor =
        JMediaSecureProcessor::GetSecureProcessor(env, thiz);
    if (secureprocessor == NULL) {
        return NULL;
    }

    SecureProcessorConfig *configBuf = GetNativeHandle(env, jImageConfig);

    // Convert HardwareBuffer jobject to AHardwareBuffer
    AHardwareBuffer *hwBuf =
        android_hardware_HardwareBuffer_getNativeHardwareBuffer(env, jhwBuffer);

    // Convert AHardwareBuffer to nativeHandle
    const native_handle_t *nativeHandle =
        AHardwareBuffer_getNativeHandle(hwBuf);

    // Add mandatory Image Frame Buffer configs
    status_t status = _populateImageBufferConfig(hwBuf, configBuf);
    if (status != OK) {
        ALOGE("failed to populate mandatory Image Buffer config");
        return NULL;
    }

    // Convert to HIDL vector
    convertToHidl(configBuf, &HIDLInCfg);

    // Call the HIDL API
    auto ret = secureprocessor->processImage(
        jsessionId, nativeHandle, HIDLInCfg,
        [&retCode, &HIDLOutCfg](ErrorCode _status,
                                SecureProcessorCfgBuf HIDLOutCfg_) {
            retCode = _status;
            if (retCode == ErrorCode::SECURE_PROCESSOR_OK) {
                HIDLOutCfg = HIDLOutCfg_;
            }
        });
    if (!ret.isOk() || retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        String8 msg("processImage failed");
        if (retCode == ErrorCode::SECURE_PROCESSOR_FAIL) {
            msg += ": retCode";
        } else {
            msg.appendFormat(": general failure (retCode = %d)", retCode);
        }
        ALOGE("%s", msg.string());
    } else {
        jobject obj = PopulateReturnConfig(env, HIDLOutCfg);
        return obj;
    }

    // Delete configBuf object
    delete configBuf;

    return nullptr;
}

static jint mediaSecureProcessor_stopSession(JNIEnv *env, jobject thiz,
                                             jint jsessionId)
{
    if (jsessionId == 0) {
        return -1;
    }

    sp<ISecureProcessor> secureprocessor =
        JMediaSecureProcessor::GetSecureProcessor(env, thiz);
    if (secureprocessor == NULL) {
        return -1;
    }

    ErrorCode retCode = secureprocessor->stopSession(jsessionId);
    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        String8 msg("stopSession failed");
        if (retCode == ErrorCode::SECURE_PROCESSOR_FAIL) {
            msg += ": session not found";
        } else {
            msg.appendFormat(": general failure (%d)", retCode);
        }
        ALOGE("%s", msg.string());
        return -1;
    }

    return 0;
}

static jint mediaSecureProcessor_deleteSession(JNIEnv *env, jobject thiz,
                                               jint jsessionId)
{
    if (jsessionId == 0) {
        return -1;
    }

    sp<ISecureProcessor> secureprocessor =
        JMediaSecureProcessor::GetSecureProcessor(env, thiz);
    if (secureprocessor == NULL) {
        return -1;
    }

    ErrorCode retCode = secureprocessor->deleteSession(jsessionId);
    if (retCode != ErrorCode::SECURE_PROCESSOR_OK) {
        String8 msg("deleteSession failed");
        if (retCode == ErrorCode::SECURE_PROCESSOR_FAIL) {
            msg += ": session not found ";
        } else {
            msg.appendFormat(": general failure (%d)", retCode);
        }
        ALOGE("%s", msg.string());
        return -1;
    }

    return 0;
}

static const JNINativeMethod gMethods[] = {
    {"native_release", "()V", (void *)mediaSecureProcessor_native_release},
    {"native_init", "()V", (void *)mediaSecureProcessor_native_init},

    {"native_setup", "(Ljava/lang/String;)V",
     (void *)mediaSecureProcessor_native_setup},

    {"createSession", "()I", (void *)mediaSecureProcessor_createSession},

    {"getConfig",
     "(ILcom/qti/media/secureprocessor/MediaSecureProcessorConfig;)Lcom/qti/"
     "media/secureprocessor/MediaSecureProcessorConfig;",
     (void *)mediaSecureProcessor_getConfig},

    {"setConfig",
     "(ILcom/qti/media/secureprocessor/MediaSecureProcessorConfig;)I",
     (void *)mediaSecureProcessor_setConfig},

    {"startSession", "(I)I", (void *)mediaSecureProcessor_startSession},

    {"processImage",
     "(ILandroid/hardware/HardwareBuffer;Lcom/qti/media/secureprocessor/"
     "MediaSecureProcessorConfig;)Lcom/qti/media/secureprocessor/"
     "MediaSecureProcessorConfig;",
     (void *)mediaSecureProcessor_processImage},

    {"stopSession", "(I)I", (void *)mediaSecureProcessor_stopSession},

    {"deleteSession", "(I)I", (void *)mediaSecureProcessor_deleteSession},
};

static const JNINativeMethod cMethods[] = {
    {"createConfigBuffer", "(II)J",
     (void *)mediaSecureProcessorConfig_createConfigBuffer},

    {"addEntry", "(Ljava/lang/String;Ljava/lang/String;)I",
     (void *)mediaSecureProcessorConfig_addEntry__ILjava_lang_String_2},

    {"addEntry", "(Ljava/lang/String;[II)I",
     (void *)mediaSecureProcessorConfig_addEntry__I_3II},

    {"addEntry", "(Ljava/lang/String;[BI)I",
     (void *)mediaSecureProcessorConfig_addEntry__I_3BI},

    {"addEntry", "(Ljava/lang/String;[JI)I",
     (void *)mediaSecureProcessorConfig_addEntry__I_3JI},

    {"getEntry",
     "(Ljava/lang/String;)Lcom/qti/media/secureprocessor/"
     "MediaSecureProcessorConfig$ConfigEntry;",
     (void *)mediaSecureProcessorConfig_getEntry},

    {"insertCustomTags", "(Ljava/util/Map;)I",
     (void *)mediaSecureProcessorConfig_insertCustomTags},

    {"getEntryCount", "()I", (void *)mediaSecureProcessorConfig_getEntryCount},

    {"getEntryByIndex",
     "(I)Lcom/qti/media/secureprocessor/"
     "MediaSecureProcessorConfig$ConfigEntry;",
     (void *)mediaSecureProcessorConfig_getEntryByIndex},

    {"clear", "()I", (void *)mediaSecureProcessorConfig_clear},

    {"isEmpty", "()Z", (void *)mediaSecureProcessorConfig_isEmpty},

    {"checkEntry", "(Ljava/lang/String;)Z",
     (void *)mediaSecureProcessorConfig_checkEntry},

    {"getSize", "()I", (void *)mediaSecureProcessorConfig_getSize},
};

int register_mediaSecureProcessor(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(
        env, "com/qti/media/secureprocessor/MediaSecureProcessor", gMethods,
        NELEM(gMethods));
}
int register_mediaSecureProcessorConfig(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(
        env, "com/qti/media/secureprocessor/MediaSecureProcessorConfig",
        cMethods, NELEM(cMethods));
}

jint JNI_OnLoad(JavaVM *vm, void * /* reserved */)
{
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("Failed to GetEnv\n");
        goto exit;
    }
    assert(env != NULL);

    if (register_mediaSecureProcessor(env) < 0) {
        ALOGE("ERROR: MediaSecureProcessor native registration failed");
        goto exit;
    }

    if (register_mediaSecureProcessorConfig(env) < 0) {
        ALOGE("ERROR: MediaSecureProcessorConfig native registration failed");
        goto exit;
    }

    result = JNI_VERSION_1_4;

exit:
    return result;
}
