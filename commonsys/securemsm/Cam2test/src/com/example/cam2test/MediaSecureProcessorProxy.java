/*=============================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import android.util.Log;
import android.hardware.HardwareBuffer;

import com.qti.media.secureprocessor.MediaSecureProcessor;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig.ConfigEntry;

import java.util.Map;
import java.util.HashMap;

// Secure and Non-secure capture fragments extend this class
public class MediaSecureProcessorProxy
{
    protected static int mSessionId = -1;
    protected static MediaSecureProcessor mSecureProcessor = null;
    protected static MediaSecureProcessorConfig mInSecConfig = null;
    protected static Map<String, Integer> mTagMap = null;
    private static boolean mSessionStarted = false;
    private static String mLogtag = "SecCamTest";

    final static String SESSION_CONFIG_RESET_CAMERA =
        "secureprocessor.session.config.reset_camera";

    final static int CustomConfigLicense =
        MediaSecureProcessorConfig.CUSTOM_CONFIG_OFFSET + 1;
    final static int CustomConfigResetCamera =
        MediaSecureProcessorConfig.CUSTOM_CONFIG_OFFSET + 2;

    public MediaSecureProcessorProxy()
    {
        mTagMap = new HashMap<>();
        mTagMap.put(SESSION_CONFIG_RESET_CAMERA, CustomConfigResetCamera);
    }

    public int createSession(String dest, int prevSessionID)
    {
        int ret = -1;

        if (mSessionId != -1) {
            Log.d(getLogTag(),
                  "createSession: session ID already existed: " + mSessionId);
            ret = 0;
            return ret;
        }

        synchronized (this)
        {
            try {
                ret = createMediaSecureProcessor(dest, prevSessionID);
                if (ret != 0) {
                    return ret;
                }

                mSessionId = mSecureProcessor.createSession();
                if (mSessionId <= 0) {
                    ret = -1;
                }
                Log.d(getLogTag(),
                      "createSession: session ID created: " + mSessionId);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return ret;
    }

    public int setConfig(MediaSecureProcessorConfig inSecConfig)
    {
        int ret = -1;

        if (mSecureProcessor == null) {
            Log.e(getLogTag(), "setConfig: mSecureProcessor is null!");
            return ret;
        }

        if (mSessionId == -1) {
            Log.d(getLogTag(), "setConfig: session ID is -1!");
            return ret;
        }

        try {
            mInSecConfig = inSecConfig;
            // Set the session configurations after addEntry calls
            ret = mSecureProcessor.setConfig(mSessionId, mInSecConfig);
            if (ret != 0) {
                Log.e(getLogTag(), "setConfig error = " + ret);
            }

            Log.d(getLogTag(),
                  "setConfig called with session ID " + mSessionId);

        } catch (Exception e) {
            e.printStackTrace();
        }

        return ret;
    }

    public int startSession()
    {
        if (mSessionStarted) {
            Log.d(getLogTag(), "startSession: session already started!");
            return 0;
        }

        int ret = -1;
        if (mSecureProcessor == null) {
            Log.e(getLogTag(), "startSession: mSecureProcessor is null!");
            return ret;
        }

        if (mSessionId == -1) {
            Log.d(getLogTag(), "startSession: session ID is -1!");
            return ret;
        }
        // Secure session should atart only once even for 2 sensors
        synchronized (this)
        {
            try {
                ret = mSecureProcessor.startSession(mSessionId);
                if (ret != 0) {
                    Log.e(getLogTag(),
                          "Error: startSession called with session ID " +
                              mSessionId + ". Return val - " + ret);
                    return ret;
                }

                mSessionStarted = true;
                ret = 0;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return ret;
    }

    public MediaSecureProcessorConfig processImage(
        HardwareBuffer hbuf, MediaSecureProcessorConfig frameConfig)
    {
        MediaSecureProcessorConfig outConfig = null;
        if (mSecureProcessor == null) {
            Log.e(getLogTag(), "processImage: mSecureProcessor is null!");
            return outConfig;
        }

        if (mSessionId == -1) {
            Log.d(getLogTag(), "processImage: session ID is -1!");
            return outConfig;
        }

        synchronized (this)
        {
            try {
                outConfig = mSecureProcessor.processImage(mSessionId, hbuf,
                                                          frameConfig);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return outConfig;
    }

    public void stopSession()
    {
        if (mSecureProcessor == null) {
            Log.e(getLogTag(), "stopSession: mSecureProcessor is null!");
            return;
        }

        if (mSessionId == -1) {
            Log.d(getLogTag(), "stopSession: session ID is -1!");
            return;
        }

        synchronized (this)
        {
            if (mSessionStarted) {
                mSessionStarted = false;
                try {
                    int ret = mSecureProcessor.stopSession(mSessionId);
                    if (ret != 0) {
                        // ignore stop session failure
                        Log.d(getLogTag(),
                              "Ssession already stopped: Session ID " +
                                  mSessionId);
                    }
                } catch (Exception e) {
                    Log.e(getLogTag(),
                          "Ssession already stopped: Session ID " + mSessionId);
                }
            }
        }
    }

    public void deleteSession()
    {
        if (mSecureProcessor == null) {
            Log.e(getLogTag(), "deleteSession: mSecureProcessor is null!");
            return;
        }

        if (mSessionId == -1) {
            Log.d(getLogTag(), "deleteSession: session ID is -1!");
            return;
        }

        if (mSessionStarted) {
            stopSession();
        }

        synchronized (this)
        {
            try {
                Log.d(getLogTag(), "deleteSession: Session ID " + mSessionId);
                int ret = mSecureProcessor.deleteSession(mSessionId);
                if (ret != 0) {
                    // ignore stop session failure
                    Log.d(getLogTag(),
                          "Ssession already deleted: Session ID " + mSessionId);
                }
            } catch (Exception e) {
                Log.d(getLogTag(),
                      "Ssession already deleted: Session ID " + mSessionId);
            }
        }
    }

    /* This is for demo purposes. The reset camera API should be called from
       the Trusted Application between users. It will clear the secure
       memory used by the camera */
    public int resetCamera()
    {
        int ret = -1;

        if (mSecureProcessor == null) {
            Log.e(getLogTag(), "resetCamera: mSecureProcessor is null!");
            return ret;
        }

        if (mSessionId == -1) {
            Log.d(getLogTag(), "resetCamera: session ID is -1!");
            return ret;
        }

        synchronized (this)
        {
            if (!mSessionStarted) {
                Log.d(getLogTag(), "resetCamera: session not started!");
                return ret;
            }

            try {
                MediaSecureProcessorConfig resetCameraConfig =
                    new MediaSecureProcessorConfig(2, 10);
                resetCameraConfig.setCustomConfigs(mTagMap);

                // Only the config name RESET_CAMERA is needed.
                // The value passed is not used.
                int[] dummyParam = {0};
                resetCameraConfig.addEntry(SESSION_CONFIG_RESET_CAMERA,
                                           dummyParam, 1);

                ret = mSecureProcessor.setConfig(mSessionId, resetCameraConfig);

                Log.d(getLogTag(), "resetCamera called with session ID: " +
                                       mSessionId + " ret: " + ret);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return ret;
    }

    public void setLogTag(String logtag)
    {
        mLogtag = logtag;
    }

    public MediaSecureProcessorConfig getMediaSecureProcessorConfig()
    {
        return mInSecConfig;
    }

    public int getSessionId()
    {
        return mSessionId;
    }

    public MediaSecureProcessor getMediaSecureProcessor()
    {
        return mSecureProcessor;
    }

    private int createMediaSecureProcessor(String dest, int prevSessionID)
    {
        if (mSecureProcessor != null) {
            return 0;
        }

        int ret = -1;
        try {
            Log.d(getLogTag(), "Destination is " + dest);
            mSecureProcessor = new MediaSecureProcessor(dest);
            Log.d(getLogTag(), "Created SECUREPROCESSOR - " + mSecureProcessor);

            if (mSecureProcessor != null) {
                ret = 0;
            }

            // delete previous session if exists
            cleanUpSessionID(prevSessionID);

        } catch (Exception e) {
            e.printStackTrace();
        }

        return ret;
    }

    private void cleanUpSessionID(int sessionID)
    {
        if (sessionID == -1) {
            return;
        }

        try {
            mSecureProcessor.stopSession(sessionID);
            mSecureProcessor.deleteSession(sessionID);
            Log.d(getLogTag(), "Clean up Session ID " + sessionID);
        } catch (Exception e) {
            Log.e(getLogTag(),
                  "Session already cleaned up: Session ID " + sessionID);
        }
    }

    private String getLogTag()
    {
        return mLogtag;
    }
}
