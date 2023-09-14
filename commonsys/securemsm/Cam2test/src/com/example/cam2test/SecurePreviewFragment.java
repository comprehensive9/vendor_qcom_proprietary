/*=============================================================================
Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import android.app.Activity;
import android.content.Intent;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.net.Uri;
import android.os.Bundle;

import androidx.fragment.app.Fragment;
import android.preference.PreferenceManager;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.HardwareBuffer;
import android.hardware.camera2.params.Face;

import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import android.graphics.Rect;

import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import com.qti.media.secureprocessor.MediaSecureProcessor;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig.ConfigEntry;

import android.hardware.camera2.params.StreamConfigurationMap;
import android.hardware.camera2.params.SessionConfiguration;
import android.hardware.camera2.params.OutputConfiguration;
import java.util.concurrent.Executor;

import org.json.JSONException;
import org.json.JSONObject;

/*
Preview is essentially an extension of Capture
Preview = Capturing frames + Displaying
*/
public class SecurePreviewFragment extends Capture
{
    private static final String ARG_PARAM1 = "cam";
    private static final String ARG_PARAM2 = "fps_display";
    private static final String ARG_PARAM3 = "sessionID";
    private static final String ARG_PARAM4 = "timestamp_display";
    private static Boolean mSurfaceExists = false;

    protected MediaSecureProcessorProxy mSecureProcessorProxy;
    private MediaSecureProcessorConfig mOutSecConfig;
    private ArrayList<TotalCaptureResult> mTotalCaptureResults;

    // To support rotation during preview
    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();
    static
    {
        ORIENTATIONS.append(Surface.ROTATION_0, 0);
        ORIENTATIONS.append(Surface.ROTATION_90, 90);
        ORIENTATIONS.append(Surface.ROTATION_180, 180);
        ORIENTATIONS.append(Surface.ROTATION_270, 270);
    }

    // private AutoFitTextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Size mPreviewDimensions;

    public static final CaptureRequest
        .Key<Byte> mBypass_Resource_Check = new CaptureRequest.Key<>(
        "org.codeaurora.qcamera3.sessionParameters.overrideResourceCostValidation",
        byte.class);
    protected CameraCaptureSession.CaptureCallback captureCallback_ = null;

    private OnFragmentInteractionListener mListener;

    public SecurePreviewFragment()
    {
    }

    static SecurePreviewFragment newInstance(ConfiguredCamera camera,
                                             int fps_display, int sessionID,
                                             int timestamp_display)
    {
        SecurePreviewFragment fragment = new SecurePreviewFragment();
        Bundle args = new Bundle();
        args.putParcelable(ARG_PARAM1, camera);
        args.putInt(ARG_PARAM2, fps_display);
        args.putInt(ARG_PARAM3, sessionID);
        args.putInt(ARG_PARAM4, timestamp_display);

        // On fragment creation, receive the arguments provided by
        // CameraActivity
        fragment.setArguments(args);
        return fragment;
    }

    @Override public void onViewCreated(View view, Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
        //FR66122
        isFDSwitch = MainActivity.FDSwitchState;
        mTotalCaptureResults = new ArrayList<>();
        mSurfaceView = getActivity().findViewById(R.id.camSurfaceView);

        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.setFixedSize(cam.getIntWidth(),cam.getIntHeight());
        mSurfaceHolder.addCallback(previewSurfaceCallback);

        cameraManager = (CameraManager)getActivity().getSystemService(
            Context.CAMERA_SERVICE);

        // Set preview dimensions based on chosen camera width & height
        mCaptureDimensions = new Size(cam.getIntWidth(), cam.getIntHeight());

        Log.d(getLogTag(), "IMAGE READER Created"
                               + " - "
                               + "DIMENSIONS " + cam.getIntWidth() + "x" +
                               cam.getIntHeight());

        numFrames = 0;
        currentFPS = 0;
        startTimestamp = 0;

        String display_text;

        // If capture is also chosen along with preview with same sensor,
        // Retrieve separate config for preview frames & capture frames
        // If not set by user, by default both will be with same configs
        if (cam.isChosenForSecureCapture()) {
            SharedPreferences sharedPreferences =
                PreferenceManager.getDefaultSharedPreferences(
                    getActivity().getApplicationContext());
            String previewResolution = sharedPreferences.getString(
                getString(R.string.key_select_preview_resolution), null);
            if (previewResolution != null) {
                mPreviewDimensions =
                    ConfiguredCamera.getResolutionFromString(previewResolution);
            }

            Log.d(getLogTag(), "Simultaneous capture resolution: " +
                                   mPreviewDimensions.getWidth() + "x" +
                                   mPreviewDimensions.getHeight());

            // Image reader to listen to ImageAvailable event
            mRawImageReader = ImageReader.newInstance(
                mCaptureDimensions.getWidth(), mCaptureDimensions.getHeight(),
                Integer.parseInt(cam.getFormat()), 2, USAGE_PROTECTED);
            mRawImageReader.setOnImageAvailableListener(
                onRawImageAvailableListener, mBackgroundHandler);
            display_text = "Capture:" + mCaptureDimensions.getWidth() + "x" +
                           mCaptureDimensions.getHeight() + " Preview:" +
                           mPreviewDimensions.getWidth() + "x" +
                           mPreviewDimensions.getHeight();
        } else {
            mPreviewDimensions = mCaptureDimensions;
            display_text = "Preview Running";
        }

        fps_view =
            getActivity().findViewById(fps_id);  // To display dynamic fps
        timestamp_view = getActivity().findViewById(timestamp_id);
        fps_view.setText(display_text);
        timestamp_view.setText(mPreviewDimensions.getWidth() + "x" +
                               mPreviewDimensions.getHeight());

        Log.d(getLogTag(), "Preview FPS View ID - " + fps_id);

        // Get secureprocessor object that was creating in MainActivity
        mSecureProcessorProxy = MainActivity.mSecureProcessorProxy;
    }

    // Update fps/timestamp on each frame event
    private final ImageReader
        .OnImageAvailableListener onRawImageAvailableListener =
        new ImageReader.OnImageAvailableListener() {
            // Objects to set frame-configs for each available frame
            MediaSecureProcessorConfig outConfig;
            MediaSecureProcessorConfig frameConfig;

            @Override public void onImageAvailable(ImageReader reader)
            {
                try {
                    frameConfig = new MediaSecureProcessorConfig(10, 100);

                    Image image = reader.acquireLatestImage();
                    if (image == null) {
                        return;
                    }

                    long timestamp = image.getTimestamp();
                    // Timestamp takes milliseconds. Image gives nanoseconds.
                    Timestamp toDisplay = new Timestamp(timestamp / 1000000);
                    Log.d(getLogTag(),
                          "CAMERA - " + cam.getCamId() +
                              " - NEW Capture Frame : " + timestamp + " - " +
                              toDisplay.toString() + " - " + image.getFormat() +
                              " - " + image.getWidth() + "x" +
                              image.getHeight());

                    // Set frame-configs cameraID and timestamp
                    long[] tmpstmp = {timestamp};
                    int[] camid = {Integer.parseInt(cam.getCamId())};
                    int ret = frameConfig.addEntry(
                        frameConfig.IMAGE_CONFIG_CAMERA_ID, camid, 1);
                    ret = frameConfig.addEntry(
                        frameConfig.IMAGE_CONFIG_TIMESTAMP, tmpstmp, 1);

                    HardwareBuffer hbuf = image.getHardwareBuffer();
                    if (hbuf == null) {
                        image.close();
                        return;
                    }

                    outConfig =
                        mSecureProcessorProxy.processImage(hbuf, frameConfig);

                    Log.d(getLogTag(),
                          "ProcessImage: cam: " + camid[0] + " - " +
                              mSecureProcessorProxy.getSessionId() + " - " +
                              hbuf + " - " + frameConfig);

                    hbuf.close();
                    image.close();

                    // Calculate and update fps value after every frame
                    if (numFrames == 0) {
                        startTimestamp = timestamp;
                    } else {
                        currentFPS =
                            (numFrames * 1000000000) /
                            (timestamp -
                             startTimestamp);  // Nanoseconds to seconds for fps

                        // Update the fps and timestamp after every frame
                        timestamp_view.setText(getString(
                            R.string.timestamp_text, toDisplay.toString()));
                    }

                    numFrames++;
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.d(getLogTag(), "ERROR: Frame null");
                }
            }
        };

    private SurfaceHolder.Callback previewSurfaceCallback =
        new SurfaceHolder.Callback() {

            @Override public void surfaceCreated(SurfaceHolder holder)
            {
                Log.d(getLogTag(), "surfaceCreated");
                c_OpenCamera();
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format,
                                       int width, int height)
            {
                Log.d(getLogTag(), "surfaceChanged");
                // refreshCamera();
            }

            @Override public void surfaceDestroyed(SurfaceHolder holder)
            {
                Log.d(getLogTag(), "surfaceDestroyed");

                // Todo: we may need to destroy preview and then re-init in
                // resume.
                // However, disabling this is working now.
                // In a synchronized way (using monitor), remove preview
                // dimensions
                synchronized (mCameraStateLock)
                {
                    Log.d(getLogTag(), "DESTROYED PREVIEW");
                    mCaptureDimensions = null;
                }
            }
        };

    @SuppressWarnings("MissingPermission") void c_OpenCamera()
    {
        try {
            if (mCaptureDimensions == null) {
                // Set preview dimensions based on chosen camera width & height
                mCaptureDimensions =
                    new Size(cam.getIntWidth(), cam.getIntHeight());

                Log.d(getLogTag(), "Capture "
                                       + " - "
                                       + "DIMENSIONS " + cam.getIntWidth() +
                                       "x" + cam.getIntHeight());
            }

            if ((mRawImageReader == null) && (cam.isChosenForSecureCapture())) {
                mRawImageReader = ImageReader.newInstance(
                    mCaptureDimensions.getWidth(),
                    mCaptureDimensions.getHeight(),
                    Integer.parseInt(cam.getFormat()), 2, USAGE_PROTECTED);
                mRawImageReader.setOnImageAvailableListener(
                    onRawImageAvailableListener, mBackgroundHandler);

                Log.d(getLogTag(), "IMAGE READER Created");
            }

            // Todo: need to set mCaptureDimensions before opening camera if it
            // is destroyed.
            Log.d(getLogTag(), "OPENING Camera ID " + cam.getCamId());
            openCamera();
        } catch (Exception e) {
            Log.d(getLogTag(),
                  "ERROR: Unable to open camera ID" + cam.getCamId());
            e.printStackTrace();
        }
    }

    @SuppressWarnings("MissingPermission") void openCamera()
    {
        try {
            Log.d(getLogTag(), "INFO: Calling openCamera");
            acquireSemaphore(mCameraOpenCloseLock);

            // Open camera, handle changes in state using stateCallback
            cameraManager.openCamera(cam.getCamId(), stateCallback,
                                     mBackgroundHandler);

        } catch (Exception e) {
            Log.d(getLogTag(),
                  "ERROR: Unable to open camera ID" + cam.getCamId());
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
            //FR66122
            try{
                CameraCharacteristics characteristics = cameraManager.getCameraCharacteristics(cam.getCamId());
                int max_count = characteristics.get(CameraCharacteristics.STATISTICS_INFO_MAX_FACE_COUNT);
                int[] faceDetectModes = characteristics.get(CameraCharacteristics.STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES);
                if( faceDetectModes.length > 1){
                    isFaceDetectionSupported = true;
                    Log.d(getLogTag(),"FaceDetectModes for " + cam.getCamId() + " are " + Arrays.toString(faceDetectModes));
                    Log.d(getLogTag(),"Max Face Count for " + cam.getCamId() + " is " + String.valueOf(max_count));
                }else{
                    Log.d(getLogTag(),"Face Detection not supported");
                }
            }catch(CameraAccessException e){
                e.printStackTrace();
            }
            //FR66122
                        startCameraPreview();
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
                sendErrIntentToMainActivity();
            }

            @Override public void onClosed(CameraDevice camera)
            {
                Log.d(getLogTag(), "Camera Device closed");
            }

        };

    @Override public void onResume()
    {
        Log.d(getLogTag(), "onResume ");
        startBackgroundThread();

        Log.d(getLogTag(), "setProtected to true!");
        mSurfaceView.setProtected(true);

        mSurfaceView.setVisibility(
            View.VISIBLE);  // This triggers surfaceCreated callback

        super.onResume();
    }

    @Override public void onPause()
    {
        Log.d(getLogTag(), "onPause ");
        Log.d(getLogTag(), "setProtected to false!");
        mSurfaceView.setProtected(false);
        mSurfaceView.setVisibility(View.INVISIBLE);

        closeCamera();

        super.onPause();
    }

    @Override public void onStop()
    {
        Log.d(getLogTag(), "onStop ");

        stopBackgroundThread();
        super.onStop();
    }

    private void startCameraPreview()
    {
        Log.d(getLogTag(),
              "DIMENSIONS " + cam.getIntWidth() + "x" + cam.getIntHeight());
        Log.d(getLogTag(), "DIMENSIONS " + mPreviewDimensions.getWidth() + "x" +
                               mPreviewDimensions.getHeight());

        Surface surface = mSurfaceHolder.getSurface();

        List<Surface> surfaceTargets = new ArrayList<>();

        try {
            Log.d(getLogTag(), "Add secure preview surface");
            surfaceTargets.add(surface);

            // Create capture request based on chosen fps and set preview to be
            // displayed on surface
            mCaptureRequestBuilder = mCameraDevice.createCaptureRequest(
                CameraDevice.TEMPLATE_PREVIEW);
            mCaptureRequestBuilder.set(
                CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, cam.getFps());
            mCaptureRequestBuilder.set(mBypass_Resource_Check, (byte)0x01);
            mCaptureRequestBuilder.addTarget(surface);
            //FR66122
            if(isFaceDetectionSupported && isFDSwitch){
                Log.d(getLogTag(), "Set Builder for FD");
                mCaptureRequestBuilder.set(CaptureRequest.STATISTICS_FACE_DETECT_MODE,CameraMetadata.STATISTICS_FACE_DETECT_MODE_SIMPLE);
                Capture.mSetting = 1;
            }
            //FR66122
            if (cam.isChosenForSecureCapture()) {
                Log.d(getLogTag(), "Add secure capture surface");
                Surface captureSurface = mRawImageReader.getSurface();
                surfaceTargets.add(captureSurface);
                mCaptureRequestBuilder.addTarget(captureSurface);
            }

            List<OutputConfiguration> outConfigurations =
                new ArrayList<>(surfaceTargets.size());

            for (Surface sface : surfaceTargets) {
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

    // If the resolution settings have changed, calculate new aspect ratio and
    // set preview surface
    private void configureTransform(int viewWidth, int viewHeight)
    {
        // Not supported for surfaceView
    }

    // Find what the orientation of the device is
    private static int sensorToDeviceRotation(CameraCharacteristics c,
                                              int deviceOrientation)
    {
        int sensorOrientation = c.get(CameraCharacteristics.SENSOR_ORIENTATION);

        // Get device orientation in degrees
        deviceOrientation = ORIENTATIONS.get(deviceOrientation);

        // Reverse device orientation for front-facing cameras
        if (c.get(CameraCharacteristics.LENS_FACING) ==
            CameraCharacteristics.LENS_FACING_FRONT) {
            deviceOrientation = -deviceOrientation;
        }

        return (sensorOrientation - deviceOrientation + 360) % 360;
    }

    @Override public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {  // On creating the layout, receive the
                                       // parameters from CameraActivity
            cam = getArguments().getParcelable(ARG_PARAM1);
            fps_id = getArguments().getInt(ARG_PARAM2);
            timestamp_id = getArguments().getInt(ARG_PARAM4);

            Log.d(getLogTag(), "FPS View ID - " + fps_id);
            Log.d(getLogTag(), "Timestamp View ID - " + timestamp_id);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        return inflater.inflate(R.layout.fragment_secure_preview, container,
                                false);
    }

    public void onButtonPressed(Uri uri)
    {
        if (mListener != null) {
            mListener.onFragmentInteraction(uri);
        }
    }

    @Override public void onAttach(Context context)
    {
        super.onAttach(context);
        if (context instanceof OnFragmentInteractionListener) {
            mListener = (OnFragmentInteractionListener)context;
        } else {
            throw new RuntimeException(
                context.toString() +
                " must implement OnFragmentInteractionListener");
        }
    }

    @Override public void onDetach()
    {
        super.onDetach();
        mListener = null;
    }

    public interface OnFragmentInteractionListener {
        void onFragmentInteraction(Uri uri);
    }
}
