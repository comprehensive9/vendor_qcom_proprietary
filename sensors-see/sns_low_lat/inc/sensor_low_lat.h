/*============================================================================
  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_qualcomm_qti_sensors_core_sensortest_SensorLowLat */

#ifndef _Included_com_qualcomm_qti_usta_core_SensorLowLatencyJNI
#define _Included_com_qualcomm_qti_usta_core_SensorLowLatencyJNI
#ifdef __cplusplus
extern "C" {
#endif
#undef com_qualcomm_qti_usta_core_SensorLowLatencyJNI_defaultCircBufferSize
#define com_qualcomm_qti_usta_core_SensorLowLatencyJNI_defaultCircBufferSize 4096L
#undef com_qualcomm_qti_usta_core_SensorLowLatencyJNI_defaultSamplePeriodUs
#define com_qualcomm_qti_usta_core_SensorLowLatencyJNI_defaultSamplePeriodUs 1250L
#undef com_qualcomm_qti_usta_core_SensorLowLatencyJNI_defaultFlags
#define com_qualcomm_qti_usta_core_SensorLowLatencyJNI_defaultFlags 0L

/**
 * Initializes the stream state with the framework.
 *
 * @param bufferSize The desired size of the circular buffer for the output samples.
 * @throws
 */
/*
 * Class:     com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    maxrate
 * Signature: ([JII)F
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_getmaxrate
  (JNIEnv *env, jobject obj, jlongArray suids);

/*
 * Class:     Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    getchannel
 * Signature: (I)V
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_getchannel
  (JNIEnv *, jobject, jint);

/*
 * Class:     Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    start
 * Signature: ([JII)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_start
  (JNIEnv *, jobject, jlongArray, jint, jint, jint, jint);

/*
 * Class:     Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_stop
  (JNIEnv *, jobject, jint);

/*
 * Class:     Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI
 * Method:    deinit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_usta_core_SensorLowLatencyJNI_closechannel
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
