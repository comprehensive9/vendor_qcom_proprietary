/*=============================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import android.app.Activity;
import android.content.Intent;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CameraCharacteristics;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import java.util.List;
import java.util.ArrayList;
import android.widget.TextView;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.Face;

import android.graphics.Rect;

import androidx.fragment.app.Fragment;

import java.util.Arrays;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import android.hardware.camera2.params.StreamConfigurationMap;
import android.hardware.camera2.params.SessionConfiguration;
import android.hardware.camera2.params.OutputConfiguration;
import java.util.concurrent.Executor;

import org.json.JSONException;
import org.json.JSONObject;

// Secure and Non-secure capture fragments extend this class
class Capture extends Fragment
{
    static final int USAGE_PROTECTED = 0x4000;
    ConfiguredCamera cam;

    long currentFPS, numFrames, startTimestamp;
    CameraManager cameraManager;
    CameraDevice mCameraDevice;
    CameraCaptureSession mCameraCaptureSession;
    Size mCaptureDimensions;
    CaptureRequest.Builder mCaptureRequestBuilder;
    ImageReader mRawImageReader;

    TextView fps_view;
    TextView timestamp_view;
    int fps_id;
    int timestamp_id;

    protected static final Object mCameraStateLock = new Object();
    protected static final Semaphore mCameraOpenCloseLock = new Semaphore(1);

    //FR66122
    private byte[] mBlinkDetected = null;
    private byte[] mBlinkDegree = null;
    private byte[] mSmileDegree = null;
    private byte[] mSmileConfidence = null;
    private byte[] mGazeAngle = null;
    private int[] mGazeDirection = null;
    private byte[] mGazeDegree = null;

    private final CameraCharacteristics.Key<Byte> BSGC_AVAILABLE = new CameraCharacteristics.Key<>("org.codeaurora.qcamera3.stats.bsgc_available", byte.class);
    private final CaptureResult.Key<byte[]> BLINK_DETECTED = new CaptureResult.Key<>("org.codeaurora.qcamera3.stats.blink_detected", byte[].class);
    private final CaptureResult.Key<byte[]> BLINK_DEGREE = new CaptureResult.Key<>("org.codeaurora.qcamera3.stats.blink_degree", byte[].class);
    private final CaptureResult.Key<byte[]> SMILE_DEGREE = new CaptureResult.Key<>("org.codeaurora.qcamera3.stats.smile_degree", byte[].class);
    private final CaptureResult.Key<byte[]> SMILE_CONFIDENCE = new CaptureResult.Key<>("org.codeaurora.qcamera3.stats.smile_confidence", byte[].class);
    private final CaptureResult.Key<byte[]> GAZE_ANGLE = new CaptureResult.Key<>("org.codeaurora.qcamera3.stats.gaze_angle", byte[].class);
    private final CaptureResult.Key<int[]> GAZE_DIRECTION = new CaptureResult.Key<>("org.codeaurora.qcamera3.stats.gaze_direction", int[].class);
    private final CaptureResult.Key<byte[]> GAZE_DEGREE = new CaptureResult.Key<>("org.codeaurora.qcamera3.stats.gaze_degree", byte[].class);

    //FR66122
    public boolean isFaceDetectionSupported = false;
    public boolean isFDSwitch = false;
    //FR66122

    public static int mSetting = CameraCharacteristics.STATISTICS_FACE_DETECT_MODE_OFF;

    Handler mBackgroundHandler;
    private HandlerThread mHandlerThread;

    public static final CaptureRequest
        .Key<Byte> mBypass_Resource_Check = new CaptureRequest.Key<>(
        "org.codeaurora.qcamera3.sessionParameters.overrideResourceCostValidation",
        byte.class);

    //FR66122
    public JSONObject getFaceBounds(Rect faceBound) {
         JSONObject bounds = new JSONObject();
     try {
         bounds.put("left", faceBound.left);
         bounds.put("right", faceBound.right);
         bounds.put("top", faceBound.top);
         bounds.put("bottom", faceBound.bottom);
         bounds.put("width", faceBound.width());
         bounds.put("height", faceBound.height());
     } catch (JSONException e) {
         Log.d(getLogTag(),"Failed to create a JSON object with face bound stats");
         e.printStackTrace();
         return null;
     } catch (NullPointerException e) {
         Log.d(getLogTag(),"Null pointer obtained while querying face bound stats");
         e.printStackTrace();
         return null;
     }
     return bounds;
     }

      public boolean obtainBSGCStats(CaptureResult result) {
         try {
             mBlinkDetected = result.get(BLINK_DETECTED);
             mBlinkDegree = result.get(BLINK_DEGREE);
             mSmileDegree = result.get(SMILE_DEGREE);
             mSmileConfidence = result.get(SMILE_CONFIDENCE);
             mGazeDirection = result.get(GAZE_DIRECTION);
             mGazeAngle = result.get(GAZE_ANGLE);
             mGazeDegree = result.get(GAZE_DEGREE);
         } catch (NullPointerException e) {
             if (mSetting == CameraCharacteristics.STATISTICS_FACE_DETECT_MODE_FULL) {
                 Log.d(getLogTag(),"BSGC stats are missing in FULL mode");
             }
             return false;
         }
         return true;
     }

     public JSONObject getFdFullStats(Face face, int nFaceIndex, boolean isBSGStatsPopulated) {
         Rect faceBound = face.getBounds();
         JSONObject curPoint;
         JSONObject fullStats = new JSONObject();
         try {
             fullStats.put("Face", nFaceIndex);
             fullStats.put("Face rectangle stats", getFaceBounds(faceBound));
             fullStats.put("face score", face.getScore());
             fullStats.put("face Id", face.getId());
             if (face.getLeftEyePosition() != null) {
                 curPoint = new JSONObject();
                 curPoint.put("x coordinate", face.getLeftEyePosition().x);
                 curPoint.put("y coordinate", face.getLeftEyePosition().y);
                 fullStats.put("Left eye", curPoint);
             }
             if (face.getRightEyePosition() != null) {
                 curPoint = new JSONObject();
                 curPoint.put("x coordinate", face.getRightEyePosition().x);
                 curPoint.put("y coordinate", face.getRightEyePosition().y);
                 fullStats.put("Right eye", curPoint);
             }
             if (face.getMouthPosition() != null) {
                 curPoint = new JSONObject();
                 curPoint.put("x coordinate", face.getMouthPosition().x);
                 curPoint.put("y coordinate", face.getMouthPosition().y);
                 fullStats.put("Mouth position", curPoint);
             }
             if (isBSGStatsPopulated) {
                 JSONObject bsgcStats = getBSGCStats(nFaceIndex-1);
                 fullStats.put("BSGC stats", bsgcStats == null ? "Not available" : bsgcStats);
             }
         } catch (JSONException e) {
             Log.d(getLogTag(),"Failed to create a JSON object with FULL mode stats");
             e.printStackTrace();
             return null;
         } catch (NullPointerException e) {
             Log.d(getLogTag(), "Null pointer obtained while querying FULL mode stats");
             e.printStackTrace();
             return null;
         }
         return fullStats;
     }

    private JSONObject getBSGCStats(int faceIndex) {
         JSONObject bsgcStats = new JSONObject();
         try {
             bsgcStats.put("Blink detected", mBlinkDetected[faceIndex]);
             JSONObject blinkStats = new JSONObject();
             blinkStats.put("left eye", mBlinkDegree[2*faceIndex]);
             blinkStats.put("right eye", mBlinkDegree[2*faceIndex + 1]);
             bsgcStats.put("Blink degree", blinkStats);
             bsgcStats.put("Smile confidence", mSmileConfidence[faceIndex]);
             bsgcStats.put("Smile Degree", mSmileDegree[faceIndex]);
             bsgcStats.put("Gaze angle", mGazeAngle[faceIndex]);
             JSONObject gazeStats = new JSONObject();
             gazeStats.put("up-down", mGazeDirection[3*faceIndex]);
             gazeStats.put("left-right", mGazeDirection[3*faceIndex + 1]);
             gazeStats.put("roll", mGazeDirection[3*faceIndex + 2]);
             bsgcStats.put("Gaze direction", gazeStats);
             gazeStats = new JSONObject();
             gazeStats.put("left-right", mGazeDegree[2*faceIndex]);
             gazeStats.put("top-bottom", mGazeDegree[2*faceIndex + 1]);
             bsgcStats.put("Gaze degree", gazeStats);
         } catch (JSONException e) {
             Log.d(getLogTag(),"Failed to create a JSON object with bsgc stats");
             e.printStackTrace();
             return null;
         } catch (ArrayIndexOutOfBoundsException e) {
             Log.d(getLogTag(),"BSGC stats is missing for face index: " + (faceIndex+1));
             e.printStackTrace();
             return null;
         }
         return bsgcStats;
     }
    //FR66122
    //FR66122
    public void printFaceDetectionDataLog(long timestamp, CaptureResult result) {
         JSONObject logBuilder = new JSONObject();
         long sensorTimestamp = 0;
         Face [] faces = null;
         try {
             sensorTimestamp = result.get(CaptureResult.SENSOR_TIMESTAMP);
         } catch (IllegalArgumentException | NullPointerException  e) {
             Log.d(getLogTag(), "Failed to get SENSOR_TIMESTAMP from capture result");
         }
         try {
             faces = result.get(CaptureResult.STATISTICS_FACES);
         } catch (IllegalArgumentException | NullPointerException e) {
             Log.d(getLogTag(), "Failed to get STATISTICS_FACES from capture result");
         }

         try
         {
             logBuilder.put("Frame", result.getFrameNumber());
             logBuilder.put("onCaptureComplete timestamp", timestamp + "ms");
             logBuilder.put("Sensor capture timestamp", sensorTimestamp == 0 ?
                 "missing" : sensorTimestamp+"ns");
             logBuilder.put("Number of faces detected", faces == null ? "missing": faces.length);
             logBuilder.put("Mode requested",  mCaptureRequestBuilder.get(CaptureRequest.STATISTICS_FACE_DETECT_MODE));
             logBuilder.put("Mode set", result.get(CaptureResult.STATISTICS_FACE_DETECT_MODE));
             Log.d(getLogTag(), "SECCAM_FACE_ENTRY " + logBuilder.toString());
             int nFaceIndex = 0;
             for (Face face : faces) {
                 nFaceIndex++;
                 Rect faceBound = face.getBounds();
                 JSONObject faceDataBuilder = new JSONObject();
                 JSONObject faceStats = new JSONObject();
                 faceDataBuilder.put("Frame", result.getFrameNumber());

                 if (faceBound == null) {
                     faceStats.put("Face", nFaceIndex);
                     faceStats.put("Face rectangle stats", null);
                     faceDataBuilder.put("Face stats", faceStats);
                     Log.d(getLogTag(), "SECCAM_FACE_ENTRY: " + faceDataBuilder.toString());
                     continue;
                 }
                 else if (result.get(CaptureResult.STATISTICS_FACE_DETECT_MODE) ==
                     CameraCharacteristics.STATISTICS_FACE_DETECT_MODE_SIMPLE) {
                     faceStats.put("Face", nFaceIndex);
                     faceStats.put("Face rectangle stats", getFaceBounds(faceBound));
                     faceDataBuilder.put("Face stats", faceStats);
                     Log.d(getLogTag(), "SECCAM_FACE_ENTRY: " + faceDataBuilder.toString());
                 } else if (result.get(CaptureResult.STATISTICS_FACE_DETECT_MODE) ==
                     CameraCharacteristics.STATISTICS_FACE_DETECT_MODE_FULL) {
                     boolean isBSGCStatsPopulated = false;
             boolean mIsBSGCAvailable = false;
             try {
               if (Byte.compare(cameraManager.getCameraCharacteristics(cam.getCamId()).get(BSGC_AVAILABLE), (byte) 0) == 0) {
                           Log.d(getLogTag(), "BSGC stats are not supported on device");
               } else {
                    mIsBSGCAvailable = true;
               }
            } catch (IllegalArgumentException e) {
                Log.d(getLogTag(), "Setprop required to query BSGC vendor tags");
            }catch (CameraAccessException e) {
                Log.d(getLogTag(),"ERROR: Failed to access CAMERA ID " + cam.getCamId());
                e.printStackTrace();
                        }
                     if (mIsBSGCAvailable) {
                         isBSGCStatsPopulated = obtainBSGCStats(result);
                     }
                     faceStats = getFdFullStats(face, nFaceIndex, isBSGCStatsPopulated);
                     faceDataBuilder.put("Face stats", faceStats == null ? "Null FD stats for face index: " +
                         nFaceIndex +" FULL mode" : faceStats);
                     Log.d(getLogTag(), "SECCAM_FACE_ENTRY: " + faceDataBuilder.toString());
                 }

             }
         } catch (JSONException e) {
             Log.d(getLogTag(), "Failed to log JSON data");
             e.printStackTrace();
         } catch (IllegalArgumentException | NullPointerException  e) {
             Log.d(getLogTag(), "Failed to get STATISTICS_FACE_DETECT_MODE from capture result");
         }
     }
     //FR66122

    protected CameraCaptureSession.CaptureCallback
        mSingleCaptureCallback_ = new CameraCaptureSession.CaptureCallback() {
        @Override
        public void onCaptureStarted(CameraCaptureSession session,
                                     CaptureRequest request, long timestamp,
                                     long frameNumber)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraSingleCaptureSession.CaptureCallback::onCaptureStarted");
        }

        @Override
        public void onCaptureCompleted(CameraCaptureSession session,
                                       CaptureRequest request,
                                       TotalCaptureResult result)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraSingleCaptureSession.CaptureCallback::onCaptureCompleted");

            if (!cam.isChosenForSecureCapture() || (cam.isChosenForSecureCapture() && cam.isChosenForSecurePreview())) {

                 setRepeatCapture();
            }
        }
    };

    //FR66122
    private void process(CaptureResult result) {
            Integer mode = result.get(CaptureResult.STATISTICS_FACE_DETECT_MODE);
            Face [] faces = result.get(CaptureResult.STATISTICS_FACES);
            Log.d(getLogTag(), "faces : " + faces.length);
            Log.d(getLogTag(), "mode : " + mode );
            if(faces != null && mode != null){
            Log.d(getLogTag(), "faces : " + faces.length + " , mode : " + mode );
            // synchronized(mCameraStateLock) -> Yet to be implemented if required
            long tsLong = System.currentTimeMillis()/1000;
            printFaceDetectionDataLog(tsLong,result);
            }
        }
        //FR66122

    protected CameraCaptureSession.CaptureCallback
        mRepeatingCaptureCallback_ = new CameraCaptureSession.CaptureCallback() {
        @Override
        public void onCaptureBufferLost(CameraCaptureSession session,
                                        CaptureRequest request, Surface target,
                                        long frameNumber)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraCaptureSession.CaptureCallback::onCaptureBufferLost - Camera #");
        }

        @Override
        public void onCaptureCompleted(CameraCaptureSession session,
                                       CaptureRequest request,
                                       TotalCaptureResult result)
        {
            // unlockFocus();
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraCaptureSession.CaptureCallback::onCaptureCompleted - Camera #");
            //FR66122
            if(isFaceDetectionSupported && isFDSwitch){
                process(result);
            }
            //FR66122
        }

        @Override
        public void onCaptureFailed(CameraCaptureSession session,
                                    CaptureRequest request,
                                    CaptureFailure failure)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraCaptureSession.CaptureCallback::onCaptureFailed - Camera #");
        }

        @Override
        public void onCaptureProgressed(CameraCaptureSession session,
                                        CaptureRequest request,
                                        CaptureResult partialResult)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraCaptureSession.CaptureCallback::onCaptureProgressed - Camera #");
        }

        @Override
        public void onCaptureSequenceAborted(CameraCaptureSession session,
                                             int sequenceId)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraCaptureSession.CaptureCallback::onCaptureSequenceAborted - Camera #");
        }

        @Override
        public void onCaptureSequenceCompleted(CameraCaptureSession session,
                                               int sequenceId, long frameNumber)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraCaptureSession.CaptureCallback::onCaptureSequenceCompleted - Camera #");
        }

        @Override
        public void onCaptureStarted(CameraCaptureSession session,
                                     CaptureRequest request, long timestamp,
                                     long frameNumber)
        {
            Log.d(
                getLogTag(),
                "JavaCamera2::CameraCaptureSession.CaptureCallback::onCaptureStarted - Camera #");
        }
    };

    protected CameraCaptureSession.StateCallback mCaptureStateCallBack =
        new CameraCaptureSession.StateCallback() {
            @Override public void onConfigured(CameraCaptureSession session)
            {
                if (null != mCameraDevice) {
                    mCameraCaptureSession = session;

                    // Session is configured, now start capture
                    Log.d(getLogTag(), "Start single capture");
                    setSingleCapture();
                }
            }

            @Override
            public void onConfigureFailed(CameraCaptureSession session)
            {
                Log.d(getLogTag(),
                      "ERROR: Failed to configure capture session");
            }
        };

    protected int resetSecureCamera()
    {
        // return 0 by default. Override by subclass.
        return 0;
    }

    // Lock & open requested camera
    @SuppressWarnings("MissingPermission") void openCamera()
    {
        try {
            // Acquire semaphore on camera before opening
            acquireSemaphore(mCameraOpenCloseLock);

            // Open camera, handle changes in state using stateCallback
            cameraManager.openCamera(cam.getCamId(), stateCallback,
                                     mBackgroundHandler);

        } catch (Exception e) {
            Log.d(getLogTag(),
                  "ERROR: Unable to open camera ID " + cam.getCamId());
            e.printStackTrace();

            sendErrIntentToMainActivity();
        }
    }

    private CameraDevice.StateCallback stateCallback =
        new CameraDevice.StateCallback() {
            @Override public void onOpened(CameraDevice camera)
            {
                synchronized (mCameraStateLock)
                {
                    Log.d(getLogTag(), "Opened Camera ID " + cam.getCamId());
                    mCameraDevice = camera;

                    // Camera has opened. So start capture if surface texture is
                    // also available
                    if (mCaptureDimensions != null) {
                        startCameraCapture();
                    }

                    releaseSemaphore(mCameraOpenCloseLock);
                }
            }

            @Override public void onDisconnected(CameraDevice camera)
            {
                Log.d(getLogTag(), "Disconnected Camera ID " + cam.getCamId());
                closeCamera();
            }

            @Override public void onError(CameraDevice camera, int error)
            {
                Log.d(getLogTag(), "ERROR in Camera ID " + cam.getCamId());
                closeCamera();
                getActivity().finish();
            }

            @Override public void onClosed(CameraDevice camera)
            {
                Log.d(getLogTag(), "Camera Device closed");
            }
        };

    protected class HandlerExecutor implements Executor
    {
        private final Handler ihandler;
        public HandlerExecutor(Handler handler)
        {
            ihandler = handler;
        }
        @Override

        public void execute(Runnable runCmd)
        {
            ihandler.post(runCmd);
        }
    }

    private void startCameraCapture()
    {
        Surface captureSurface = mRawImageReader.getSurface();

        List<Surface> outputSurfaces = new ArrayList<Surface>();
        outputSurfaces.add(captureSurface);

        try {
            // Create capture request based on chosen fps
            mCaptureRequestBuilder = mCameraDevice.createCaptureRequest(
                CameraDevice.TEMPLATE_PREVIEW);
            mCaptureRequestBuilder.set(
                CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, cam.getFps());
            mCaptureRequestBuilder.set(mBypass_Resource_Check, (byte)0x01);

            List<OutputConfiguration> outConfigurations =
                new ArrayList<>(outputSurfaces.size());

            for (Surface sface : outputSurfaces) {
                outConfigurations.add(new OutputConfiguration(sface));
                mCaptureRequestBuilder.addTarget(sface);
            }

            SessionConfiguration sessionCfg = new SessionConfiguration(
                SessionConfiguration.SESSION_REGULAR, outConfigurations,
                new HandlerExecutor(mBackgroundHandler), mCaptureStateCallBack);
            sessionCfg.setSessionParameters(mCaptureRequestBuilder.build());
            mCameraDevice.createCaptureSession(sessionCfg);

        } catch (CameraAccessException e) {
            Log.d(getLogTag(),
                  "ERROR: Failed to access CAMERA ID " + cam.getCamId());
            e.printStackTrace();
        }
    }

    // After the first time, set repeating requests to keep getting frames
    private void setSingleCapture()
    {
        if (null != mCameraDevice) {
            mCaptureRequestBuilder.set(
                CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, cam.getFps());
            try {
                mCameraCaptureSession.capture(mCaptureRequestBuilder.build(),
                                              mSingleCaptureCallback_,
                                              mBackgroundHandler);
            } catch (Exception e) {
                Log.d(getLogTag(),
                      "ERROR: Failed to access CAMERA ID " + cam.getCamId());
                e.printStackTrace();
            }
        }
    }

    // After the first time, set repeating requests to keep getting frames
    protected void setRepeatCapture()
    {
        if (null != mCameraDevice) {
            mCaptureRequestBuilder.set(
                CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, cam.getFps());
            try {
                mCameraCaptureSession.setRepeatingRequest(
                    mCaptureRequestBuilder.build(), mRepeatingCaptureCallback_,
                    mBackgroundHandler);
            } catch (Exception e) {
                Log.d(getLogTag(),
                      "ERROR: Failed to access CAMERA ID " + cam.getCamId());
                e.printStackTrace();
            }
        }
    }

    void startBackgroundThread()
    {
        mHandlerThread = new HandlerThread("Camera Background");
        mHandlerThread.start();

        Log.d(getLogTag(), "STARTED Background Thread");
        synchronized (mCameraStateLock)
        {
            mBackgroundHandler = new Handler(mHandlerThread.getLooper());
        }
    }

    void stopBackgroundThread()
    {
        mHandlerThread.quitSafely();
        try {
            mHandlerThread.join();
            mBackgroundHandler = null;
            mHandlerThread = null;

            Log.d(getLogTag(), "STOPPED Background Thread " + cam.getCamId());
        } catch (InterruptedException e) {
            Log.d(getLogTag(),
                  "Interrupted while stopping background activity");
            e.printStackTrace();
        }
    }

    // Close camera, imagereader and session
    protected void closeCamera()
    {
        try {
            mCameraOpenCloseLock.acquire();

            // Checks null before closing to avoid crashing if open had failed
            synchronized (mCameraStateLock)
            {
                if (mCameraCaptureSession != null) {
                    mCameraCaptureSession.close();
                    mCameraCaptureSession = null;
                }

                if (mRawImageReader != null) {
                    mRawImageReader.close();
                    mRawImageReader = null;
                }

                if (mCameraDevice != null) {
                    mCameraDevice.close();
                    mCameraDevice = null;
                }

                Log.d(getLogTag(), "Closed Camera ID " + cam.getCamId());
            }
        } catch (InterruptedException e) {
            Log.d(getLogTag(), "ERROR: Interrupted while closing camera");
            e.printStackTrace();
        } finally {
            mCameraOpenCloseLock.release();
        }
    }

    void acquireSemaphore(Semaphore semaphore)
    {
        try {
            // Acquire semaphore on camera before opening
            if (!semaphore.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                Log.d(getLogTag(), "ERROR: Took too long to lock semaphore");
                throw new RuntimeException(
                    "Timeout while waiting to lock semaphore");
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    void releaseSemaphore(Semaphore semaphore)
    {
        semaphore.release();
    }

    protected void sendErrIntentToMainActivity()
    {
        // Returning with error code RESULT_CAMERA_ERROR
        Intent returnIntent = new Intent();
        getActivity().setResult(MainActivity.RESULT_CAMERA_ERROR, returnIntent);
        getActivity().finish();
    }

    String getLogTag()
    {
        return getString(R.string.log_tag);
    }
}
