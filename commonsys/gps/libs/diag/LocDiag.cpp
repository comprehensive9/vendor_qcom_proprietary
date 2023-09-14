/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <string.h>
#include <jni.h>
#include "IZatServiceDiagLog.h"
#include <diag_lsm.h>

static jclass class_location;
static jclass class_bundle;

bool isZero(double val) {
    if (val <= 1e-6 && val >= -1e-6) {
        return true;
    }
    return false;
}

void initDiag() {
    bool bResult  = Diag_LSM_Init(NULL);
    if (bResult == FALSE) {
        LOC_LOGe("initDiag() failed on Diag_LSM_Init()");
    }
}

void populateFlpBatch(JNIEnv* env,  jobject obj, jobjectArray locations, void* pData) {
    DiagGnssLocationData diagLocation;
    DiagGnssLocationStructType* diagGnssLocationPtr = (DiagGnssLocationStructType*)pData;
    int len = env->GetArrayLength(locations);
    if (len <= 0 || nullptr == diagGnssLocationPtr) {
        LOC_LOGe("Either batch is empty or diag buffer is unavailable, len: %d", len);
        return;
    }
    diagGnssLocationPtr->version = LOG_IZAT_FLP_DIAG_MSG_VERSION;
    diagGnssLocationPtr->count = len;
    jmethodID method_getTime = env->GetMethodID(class_location, "getTime", "()J");
    jmethodID method_getLatitude = env->GetMethodID(class_location, "getLatitude", "()D");
    jmethodID method_getLongitude = env->GetMethodID(class_location, "getLongitude", "()D");
    jmethodID method_getAltitude = env->GetMethodID(class_location, "getAltitude", "()D");
    jmethodID method_getSpeed = env->GetMethodID(class_location, "getSpeed", "()F");
    jmethodID method_getBearing = env->GetMethodID(class_location, "getBearing", "()F");
    jmethodID method_getAccuracy = env->GetMethodID(class_location, "getAccuracy", "()F");
    jmethodID method_getVerticalAccuracyMeters = env->GetMethodID(class_location,
            "getVerticalAccuracyMeters", "()F");
    jmethodID method_getSpeedAccuracyMetersPerSecond = env->GetMethodID(class_location,
            "getSpeedAccuracyMetersPerSecond", "()F");
    jmethodID method_getBearingAccuracyDegrees = env->GetMethodID(class_location,
            "getBearingAccuracyDegrees", "()F");
    jmethodID method_getElapsedRealtimeNanos = env->GetMethodID(class_location,
            "getElapsedRealtimeNanos", "()J");
    jmethodID method_getElapsedRealtimeUncertaintyNanos = env->GetMethodID(class_location,
            "getElapsedRealtimeUncertaintyNanos", "()D");
    jmethodID method_getExtras = env->GetMethodID(class_location,
            "getExtras", "()Landroid/os/Bundle;");
    jmethodID method_getInt = env->GetMethodID(class_bundle, "getInt", "(Ljava/lang/String;)I");

    for (int i=0; i<len; ++i) {
        memset(&diagLocation, 0, sizeof(diagLocation));
        diagLocation.size = sizeof(diagLocation);
        diagLocation.qualityType = (DiagLocationQualityType)-1;
        jobject location_obj = env->GetObjectArrayElement(locations, i);
        diagLocation.timestamp = env->CallLongMethod(location_obj, method_getTime);
        diagLocation.latitude = env->CallDoubleMethod(location_obj, method_getLatitude);
        diagLocation.longitude = env->CallDoubleMethod(location_obj, method_getLongitude);
        diagLocation.altitude = env->CallDoubleMethod(location_obj, method_getAltitude);
        diagLocation.speed = env->CallFloatMethod(location_obj, method_getSpeed);
        diagLocation.bearing = env->CallFloatMethod(location_obj, method_getBearing);
        diagLocation.accuracy = env->CallFloatMethod(location_obj, method_getAccuracy);
        diagLocation.verticalAccuracy = env->CallFloatMethod(location_obj,
                method_getVerticalAccuracyMeters);
        diagLocation.speedAccuracy = env->CallFloatMethod(location_obj,
                method_getSpeedAccuracyMetersPerSecond);
        diagLocation.bearingAccuracy = env->CallFloatMethod(location_obj,
                method_getBearingAccuracyDegrees);
        diagLocation.elapsedRealTime = env->CallLongMethod(location_obj,
                method_getElapsedRealtimeNanos);
        diagLocation.elapsedRealTimeUnc = (uint64_t)env->CallDoubleMethod(location_obj,
                method_getElapsedRealtimeUncertaintyNanos);

        jobject extras_obj = env->CallObjectMethod(location_obj, method_getExtras);
        jstring qualitytype_str = env->NewStringUTF("QUALITY_TYPE");
        if (extras_obj != nullptr) {
            diagLocation.qualityType = (DiagLocationQualityType)env->CallIntMethod(extras_obj,
                    method_getInt, qualitytype_str);
        }

        if (!isZero(diagLocation.latitude) && !isZero(diagLocation.longitude)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_LAT_LONG_BIT;
        }
        if (!isZero(diagLocation.altitude)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_ALTITUDE_BIT;
        }
        if (!isZero(diagLocation.speed)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_SPEED_BIT;
        }
        if (!isZero(diagLocation.bearing)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_BEARING_BIT;
        }
        if (!isZero(diagLocation.accuracy)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_ACCURACY_BIT;
        }
        if (!isZero(diagLocation.verticalAccuracy)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_VERTICAL_ACCURACY_BIT;
        }
        if (!isZero(diagLocation.speedAccuracy)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_SPEED_ACCURACY_BIT;
        }
        if (!isZero(diagLocation.bearingAccuracy)) {
            diagLocation.flags |= DIAG_LOCATION_HAS_BEARING_ACCURACY_BIT;
        }
        if (diagLocation.elapsedRealTime != 0) {
            diagLocation.flags |= DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_BIT;
        }
        if (diagLocation.qualityType >= DIAG_LOCATION_STANDALONE_QUALITY_TYPE &&
                diagLocation.qualityType <= DIAG_LOCATION_FIXED_QUALITY_TYPE) {
            diagLocation.flags |= DIAG_LOCATION_HAS_QUALITY_TYPE_BIT;
        }

        if (diagGnssLocationPtr != nullptr) {
            diagGnssLocationPtr->locations[i] = diagLocation;
        }
        env->DeleteLocalRef(qualitytype_str);
        env->DeleteLocalRef(extras_obj);
    }
}

void nativeDiagLogFlpBatch(JNIEnv* env,  jobject obj, jobjectArray locations) {
    int diagId = LOG_GNSS_IZAT_FLP_REPORT_C;
    void* pData = log_alloc(diagId, sizeof(DiagGnssLocationStructType));
    LOC_LOGv("log_alloc %s, diagId: %x", nullptr==pData? "fail":"succeed", diagId);
    if (pData != nullptr) {
        populateFlpBatch(env, obj, locations, pData);
        log_commit(pData);
    }
}

static JNINativeMethod diagLogMethods[] = {
    {"native_diag_log_flp_batch", "([Landroid/location/Location;)V", (void*)nativeDiagLogFlpBatch},
};

/*
 *
 * Returns the JNI version on success, -1 on failure.
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {

    JNIEnv* env = NULL;
    jint result = -1;

    LOC_LOGi("JNI_OnLoad called");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOC_LOGe("JNI_OnLoad -  GetEnv failed");
        return result;
    }

    jclass clazz = env->FindClass("com/qualcomm/location/utils/IZatServiceContext");
    if (clazz == NULL) {
        LOC_LOGe("Native registration failed find com/qualcomm/location/utils/IZatServiceContext");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, diagLogMethods,
                sizeof(diagLogMethods)/sizeof(diagLogMethods[0])) < 0) {
        LOC_LOGe("RegisterNatives failed for IZatServiceContext");
        return JNI_FALSE;
    }

    jclass locationClass = env->FindClass("android/location/Location");
    class_location = (jclass) env->NewGlobalRef(locationClass);

    jclass bundleClass = env->FindClass("android/os/Bundle");
    class_bundle = (jclass) env->NewGlobalRef(bundleClass);
    initDiag();
    return JNI_VERSION_1_4;
}
