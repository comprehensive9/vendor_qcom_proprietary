/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.media.secureprocessor;

import android.hardware.HardwareBuffer;
import android.annotation.NonNull;
import android.annotation.Nullable;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IHwBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.os.RemoteException;
import android.util.Log;
import android.util.Singleton;
import dalvik.system.CloseGuard;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

public final class MediaSecureProcessor implements AutoCloseable {
  private static final String TAG = "MediaSecureProcessor";

  private final AtomicBoolean mClosed = new AtomicBoolean();
  private final CloseGuard mCloseGuard = CloseGuard.get();

  private long mNativeContext;

  /**
   * Instantiate a MediaSecureProcessor object for secure data
   * processing on a targeted secure destination. The HAL service
   * facilitating communication with a secure destination is
   * identified by service parameter.
   *
   * @param service HAL service identifier for communication with
   *                secure destination.
   */
  public MediaSecureProcessor(String service) throws Exception {
    native_setup(service);
    mCloseGuard.open("release");
  }

  /**
   * createSession:
   *
   * Method to create a new MediaSecureProcessor session for
   * secure image data processing on a secure destination. It
   * creates and returns a unique session identifier for
   * subsequent interactions to this session.
   *
   * @return session identifier
   *
   */
  public native int createSession();

  /**
   * getConfig:
   *
   * Get session configuration.
   *
   * The input configuration (inConfig) contains set of required
   * tags. The output configuration (referred as outConfig) is
   * populated with set of pairs having <tag, value> entries for
   * requested config tags. Both input and output configuration
   * parameters are expected to an instance of {@link
   * MediaSecureProcessorConfig} common helper
   * class.
   *
   * @param sessionId Session identifier.
   *
   * @param inConfig Input configuration. It contains required
   *                 tags for session config to be queried.
   * @return outConfig Output configuration. Populated with
   *         requested session config <tag, value> pairs.
   *
   */
  public native MediaSecureProcessorConfig getConfig(
      @NonNull int sessionId, @NonNull MediaSecureProcessorConfig inConfig);

  /**
   * setConfig:
   *
   * Set session configuration.
   *
   * The input configuration (inConfig) contains set of pairs
   * having <tag, value> entries. The configuration parameter is
   * expected to be an instance of {@link
   * MediaSecureProcessorConfig} common helper
   * class.
   *
   * @param sessionId Session identifier.
   * @param inConfig Input configuration.
   * @return 0 if the configurations were set successfully.
   *
   */
  public native int setConfig(@NonNull int sessionId,
                              @NonNull MediaSecureProcessorConfig inConfig);

  /**
   * startSession:
   *
   * Start requested session.
   *
   * This API allocates essential resources on secure destination
   * and makes them ready for secure data processing. The
   * mandatory session configs defined in {@link
   * MediaSecureProcessorConfig} class are expected
   * to be set before calling this API.
   *
   * @param sessionId Session identifier.
   * @return 0 if the session started successfully.
   *
   */
  public native int startSession(@NonNull int sessionId);

  /**
   * processImage:
   *
   * Process secure image data on selected secure destination.
   *
   * Additionally, the API allows set/get of the image specific
   * configurations. The input configuration buffer (inConfig)
   * contains config data associated with current image and the
   * output config buffer (referred as outConfig) is expected to
   * be populated with new config requests to be applied to
   * current secure data capture (source) session based on current
   * image data processing on secure destination.
   *
   * The input/output configs are instances of {@link
   * MediaSecureProcessorConfig} common helper
   * class.
   *
   * @param sessionId Session identifier.
   * @param hwBuffer hwBuffer handle for secure image data buffer.
   * @param inConfig Input configuration.
   * @return outConfig Output configuration.
   *
   */
  public native MediaSecureProcessorConfig
  processImage(@NonNull int sessionId, @NonNull HardwareBuffer hwBuffer,
               @NonNull MediaSecureProcessorConfig imageConfig);

  /**
   * stopSession:
   *
   * Stop requested session.
   *
   * This API releases resources on secure destination which were
   * allocated during startSession call. No secure data processing
   * is allowed post this API call. This API is expected to be
   * called after completely stopping the secure data capture
   * requests on source.
   *
   * @param sessionId Session identifier.
   * @return 0 if the session stopped successfully.
   *
   */
  public native int stopSession(@NonNull int sessionId);

  /**
   * deleteSession:
   *
   * Delete a previously allocated session.
   *
   * @param sessionId Session identifier.
   * @return 0 if the session deleted successfully.
   *
   */
  public native int deleteSession(@NonNull int sessionId);

  /**
   * Release resources associated with MediaSecureProcessor
   * object. The object is unusable after calling this method.
   *
   */
  @Override
  public void close() {
    release();
  }

  /**
   * Private methods to release native resources.
   */
  private final void release() {
    mCloseGuard.close();
    if (mClosed.compareAndSet(false, true)) {
      native_release();
    }
  }

  /**
   * Private native methods.
   *
   */
  private native final void native_release();

  private static native final void native_init();

  private native final void native_setup(String service);

  static {
    System.loadLibrary("mediasp_jni");
    native_init();
  }
}
