/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <android/log.h>
#include <dlfcn.h>
#include <utils/Log.h>

#include "libsoc_helper.h"
#include "com_qualcomm_qti_LibsocHelper.h"

#define LIBSOC_HELPER_LIB "libsoc_helper.so"

typedef struct dlHandler {
    void *dlHandle;
    void (*getSocInfo)(soc_info_v0_1_t *soc_info);
} dlHandler;

static dlHandler mDlHandler = {
    NULL, NULL};

static const char *className = "com/qualcomm/qti/LibsocHelper";

/*
 * Load native shared library and get the function pointer.
 * returns JNI_TRUE on success, JNI_FALSE on failure.
 */

static int libsoc_helper_init() {
    mDlHandler.dlHandle = dlopen(LIBSOC_HELPER_LIB, RTLD_NOW | RTLD_LOCAL);
    if (NULL == mDlHandler.dlHandle) {
        return JNI_FALSE;
    }

    *(void **)(&mDlHandler.getSocInfo) =
        dlsym(mDlHandler.dlHandle, "get_soc_info");

    if (NULL == mDlHandler.getSocInfo) {
        if (mDlHandler.dlHandle) {
            dlclose(mDlHandler.dlHandle);
            mDlHandler.dlHandle = NULL;
        }
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Class:     com_qualcomm_qti_LibsocHelper
 * Method:    native_getSocInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_LibsocHelper_native_1getSocInfo
    (JNIEnv *env, jobject obj) {

    if (NULL != mDlHandler.getSocInfo) {
        soc_info_v0_1_t soc = {MSM_CPU_UNKNOWN};
        (*mDlHandler.getSocInfo)(&soc);

        jclass clazz = NULL;
        clazz = env->FindClass(className);

        if (NULL != clazz) {
            jfieldID msm_cpu_id = env->GetStaticFieldID(clazz, "msm_cpu", "I");
            if (NULL != msm_cpu_id) {
                env->SetStaticIntField(clazz, msm_cpu_id, soc.msm_cpu);
            }
            env->DeleteLocalRef(clazz);
        }
    }

    return;
}

static JNINativeMethod libsocHelperMethods[] = {
    // nativeMethod,
    // methodSignature,
    // methodPointer
    {    "native_getSocInfo",
         "()V",
         (void *)Java_com_qualcomm_qti_LibsocHelper_native_1getSocInfo}
};

/*
 * Register native methods.
 * returns JNI_TRUE on success, JNI_FALSE on failure.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* libsocHelperMethods, int numMethods) {

    jclass clazz = NULL;
    clazz = env->FindClass(className);

    if (NULL == clazz) {
        ALOGE("registerNativeMethods: FindClass failed for %s\n", className);
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, libsocHelperMethods, numMethods) < 0) {
        ALOGE("registerNativeMethods: RegisterNatives failed\n");
        return JNI_FALSE;
    }

    env->DeleteLocalRef(clazz);

    return JNI_TRUE;
}

/*
 * returns the JNI version on success, JNI_ERR on failure.
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv* env = NULL;

    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("JNI_OnLoad: GetEnv failed\n");
        return JNI_ERR;
    }

    if (NULL == env) {
        ALOGE("JNI_OnLoad: env = NULL");
        return JNI_ERR;
    }

    if (!registerNativeMethods(env, className, libsocHelperMethods,
        sizeof(libsocHelperMethods) / sizeof(libsocHelperMethods[0]))) {
        ALOGE("JNI_OnLoad: registerNativeMethods failed\n");
        return JNI_ERR;
    }

    if (!libsoc_helper_init()) {
        ALOGE("JNI_OnLoad: init failed\n");
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
