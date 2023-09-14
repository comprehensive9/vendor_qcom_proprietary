/* Copyright (c) 2020, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a contribution.
 *
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.cam2test;

import android.app.Activity;
import android.content.Intent;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.RectF;
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

import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import android.hardware.camera2.params.StreamConfigurationMap;
import android.hardware.camera2.params.SessionConfiguration;
import android.hardware.camera2.params.OutputConfiguration;
import java.util.concurrent.Executor;

/*
Preview is essentially an extension of Capture
Preview = Capturing frames + Displaying
*/
public class PreviewFragment extends Capture
{
    private static final String ARG_PARAM1 = "cam";
    private static final String ARG_PARAM2 = "fps_display";
    private static final String ARG_PARAM3 = "timestamp_display";

    // To support rotation during preview
    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();
    static
    {
        ORIENTATIONS.append(Surface.ROTATION_0, 0);
        ORIENTATIONS.append(Surface.ROTATION_90, 90);
        ORIENTATIONS.append(Surface.ROTATION_180, 180);
        ORIENTATIONS.append(Surface.ROTATION_270, 270);
    }

    private AutoFitTextureView mTextureView;
    private Size mPreviewDimensions;

    public static final CaptureRequest
        .Key<Byte> mBypass_Resource_Check = new CaptureRequest.Key<>(
        "org.codeaurora.qcamera3.sessionParameters.overrideResourceCostValidation",
        byte.class);
    protected CameraCaptureSession.CaptureCallback captureCallback_ = null;

    private OnFragmentInteractionListener mListener;

    public PreviewFragment()
    {
    }

    static PreviewFragment newInstance(ConfiguredCamera camera, int fps_display,
                                       int timestamp_display)
    {
        PreviewFragment fragment = new PreviewFragment();
        Bundle args = new Bundle();
        args.putParcelable(ARG_PARAM1, camera);
        args.putInt(ARG_PARAM2, fps_display);
        args.putInt(ARG_PARAM3, timestamp_display);

        // On fragment creation, receive the arguments provided by
        // CameraActivity
        fragment.setArguments(args);
        return fragment;
    }

    @Override public void onViewCreated(View view, Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);

        mTextureView = getActivity().findViewById(R.id.texture);
        mTextureView.setSurfaceTextureListener(
            surfaceTextureListener);  // Listen for any changes in the surface
                                      // size/orientation

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
        if (cam.isChosenForCapture()) {
            SharedPreferences sharedPreferences =
                PreferenceManager.getDefaultSharedPreferences(
                    getActivity().getApplicationContext());
            String previewResolution = sharedPreferences.getString(
                getString(R.string.key_select_preview_resolution), null);
            if (previewResolution != null) {
                mPreviewDimensions =
                    ConfiguredCamera.getResolutionFromString(previewResolution);
            }

            Log.d(getLogTag(), "Simultaneous preview resolution: " +
                                   mPreviewDimensions.getWidth() + "x" +
                                   mPreviewDimensions.getHeight());

            // Image reader to listen to ImageAvailable event
            mRawImageReader = ImageReader.newInstance(
                mCaptureDimensions.getWidth(), mCaptureDimensions.getHeight(),
                Integer.parseInt(cam.getFormat()), 2);
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
    }

    // Update fps/timestamp on each frame event
    private final ImageReader
        .OnImageAvailableListener onRawImageAvailableListener =
        new ImageReader.OnImageAvailableListener() {
            @Override public void onImageAvailable(ImageReader reader)
            {
                try {
                    Image image = reader.acquireLatestImage();
                    long timestamp = image.getTimestamp();

                    // Timestamp takes milliseconds. Image gives nanoseconds.
                    Timestamp toDisplay = new Timestamp(timestamp / 1000000);
                    Log.d(getLogTag(),
                          "CAMERA - " + cam.getCamId() +
                              " - NEW Capture Frame : " + timestamp + " - " +
                              toDisplay.toString() + " - " + image.getFormat() +
                              " - " + image.getWidth() + "x" +
                              image.getHeight());
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
                    Log.d(getLogTag(), "ERROR: Frame null");
                }
            }
        };

    private AutoFitTextureView.SurfaceTextureListener
        surfaceTextureListener = new TextureView.SurfaceTextureListener() {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width,
                                              int height)
        {
            // Calculate aspect ratio and create the preview surface accordingly
            configureTransform(width, height);
            openCamera();  // Open the camera as soon as surface is available
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface,
                                                int width, int height)
        {
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface)
        {
            // In a synchronized way (using monitor), remove preview dimensions
            synchronized (mCameraStateLock)
            {
                Log.d(getLogTag(), "DESTROYED PREVIEW");
                mCaptureDimensions = null;
            }
            return true;
        }

        @Override public void onSurfaceTextureUpdated(SurfaceTexture surface)
        {
        }
    };

    @SuppressWarnings("MissingPermission") void openCamera()
    {
        try {
            // Acquire semaphore on camera before opening
            acquireSemaphore(mCameraOpenCloseLock);

            // Open camera, handle changes in state using stateCallback
            cameraManager.openCamera(cam.getCamId(), stateCallback,
                                     mBackgroundHandler);

        } catch (CameraAccessException e) {
            Log.d(getLogTag(),
                  "ERROR: Failed to access CAMERA ID " + cam.getCamId());
            e.printStackTrace();
        } catch (Exception e) {
            Log.d(getLogTag(), "ERROR: Unable to open camera");
            e.printStackTrace();

            Intent returnIntent = new Intent();
            getActivity().setResult(Activity.RESULT_FIRST_USER, returnIntent);
            getActivity().finish();
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
                getActivity().finish();
            }
        };

    @Override public void onResume()
    {
        super.onResume();
        startBackgroundThread();

        if (mTextureView.isAvailable()) {
            Log.d(getLogTag(), "OPENING Camera ID " + cam.getCamId());
            openCamera();
        } else {
            mTextureView.setSurfaceTextureListener(surfaceTextureListener);
        }
    }

    @Override public void onPause()
    {
        closeCamera();
        super.onPause();
    }

    @Override public void onStop()
    {
        stopBackgroundThread();
        super.onStop();
    }

    private void startCameraPreview()
    {
        SurfaceTexture surfaceTexture = mTextureView.getSurfaceTexture();

        Log.d(getLogTag(),
              "DIMENSIONS " + cam.getIntWidth() + "x" + cam.getIntHeight());
        Log.d(getLogTag(), "DIMENSIONS " + mPreviewDimensions.getWidth() + "x" +
                               mPreviewDimensions.getHeight());
        surfaceTexture.setDefaultBufferSize(mPreviewDimensions.getWidth(),
                                            mPreviewDimensions.getHeight());
        Surface surface =
            new Surface(surfaceTexture);  // The surface to receive images

        List<Surface> surfaceTargets = new ArrayList<>();

        try {
            surfaceTargets.add(surface);

            // Create capture request based on chosen fps and set preview to be
            // displayed on surface
            mCaptureRequestBuilder = mCameraDevice.createCaptureRequest(
                CameraDevice.TEMPLATE_PREVIEW);
            mCaptureRequestBuilder.set(
                CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, cam.getFps());
            mCaptureRequestBuilder.set(mBypass_Resource_Check, (byte)0x01);
            // mCaptureRequestBuilder.addTarget(surface);

            if (cam.isChosenForCapture()) {
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
        synchronized (mCameraStateLock)
        {
            if (null == mTextureView) {
                return;
            }

            CameraCharacteristics characteristics;
            try {
                characteristics =
                    cameraManager.getCameraCharacteristics(cam.getCamId());

            } catch (CameraAccessException e) {
                Log.d(getLogTag(),
                      "ERROR: Unable to access Camera ID " + cam.getCamId());
                throw new RuntimeException();
            }
            StreamConfigurationMap map = characteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

            // Find the rotation of the device relative to the native device
            // orientation.
            int deviceRotation = getActivity()
                                     .getWindowManager()
                                     .getDefaultDisplay()
                                     .getRotation();
            Point displaySize = new Point();
            getActivity().getWindowManager().getDefaultDisplay().getSize(
                displaySize);

            // Find the rotation of the device relative to the camera sensor's
            // orientation.
            int totalRotation =
                sensorToDeviceRotation(characteristics, deviceRotation);

            // Swap the view dimensions for calculation as needed if they are
            // rotated relative to the sensor.
            boolean swappedDimensions =
                totalRotation == 90 || totalRotation == 270;
            int rotatedViewWidth = viewWidth;
            int rotatedViewHeight = viewHeight;
            int maxPreviewWidth = displaySize.x;
            int maxPreviewHeight = displaySize.y;

            if (swappedDimensions) {
                rotatedViewWidth = viewHeight;
                rotatedViewHeight = viewWidth;
                maxPreviewWidth = displaySize.y;
                maxPreviewHeight = displaySize.x;
            }

            if (swappedDimensions) {
                mTextureView.setAspectRatio(mPreviewDimensions.getHeight(),
                                            mPreviewDimensions.getWidth());
            } else {
                mTextureView.setAspectRatio(mPreviewDimensions.getWidth(),
                                            mPreviewDimensions.getHeight());
            }

            // Find rotation of device in degrees (reverse device orientation
            // for front-facing cameras).
            int rotation =
                (characteristics.get(CameraCharacteristics.LENS_FACING) ==
                 CameraCharacteristics.LENS_FACING_FRONT)
                    ? (360 + ORIENTATIONS.get(deviceRotation)) % 360
                    : (360 - ORIENTATIONS.get(deviceRotation)) % 360;

            Matrix matrix = new Matrix();
            RectF viewRect = new RectF(0, 0, viewWidth, viewHeight);
            RectF bufferRect = new RectF(0, 0, mPreviewDimensions.getHeight(),
                                         mPreviewDimensions.getWidth());
            float centerX = viewRect.centerX();
            float centerY = viewRect.centerY();

            // Initially, output stream images from the Camera2 API will be
            // rotated to the native device orientation from the sensor's
            // orientation, and the TextureView will default to scaling
            // these buffers to fill it's view bounds.  If the aspect
            // ratios and relative orientations are correct, this is fine.
            //
            // However, if the device orientation has been rotated relative to
            // its native orientation so that the TextureView's dimensions
            // are swapped relative to the native device orientation,
            // we must do the following to ensure the output stream
            // images are not incorrectly scaled by the TextureView:
            //   - Undo the scale-to-fill from the output buffer's dimensions
            //   (i.e. its dimensions in the native device orientation)
            //     to the TextureView's dimension.
            //   - Apply a scale-to-fill from the output buffer's rotated
            //   dimensions
            //     (i.e. its dimensions in the current device orientation) to
            //     the TextureView's dimensions.
            //   - Apply the rotation from the native device orientation to the
            //   current device rotation.

            if (Surface.ROTATION_90 == deviceRotation ||
                Surface.ROTATION_270 == deviceRotation) {
                bufferRect.offset(centerX - bufferRect.centerX(),
                                  centerY - bufferRect.centerY());
                matrix.setRectToRect(viewRect, bufferRect,
                                     Matrix.ScaleToFit.FILL);
                float scale =
                    Math.max((float)viewHeight / mPreviewDimensions.getHeight(),
                             (float)viewWidth / mPreviewDimensions.getWidth());
                matrix.postScale(scale, scale, centerX, centerY);
            }

            matrix.postRotate(rotation, centerX, centerY);
            mTextureView.setTransform(matrix);
        }
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
            timestamp_id = getArguments().getInt(ARG_PARAM3);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        return inflater.inflate(R.layout.fragment_preview, container, false);
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
