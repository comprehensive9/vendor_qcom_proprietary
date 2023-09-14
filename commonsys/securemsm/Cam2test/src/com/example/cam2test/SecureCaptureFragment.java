/*=============================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import android.content.Context;
import android.hardware.camera2.CameraManager;
import android.media.Image;
import android.media.ImageReader;
import android.hardware.HardwareBuffer;

import com.qti.media.secureprocessor.MediaSecureProcessor;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig.ConfigEntry;

import android.net.Uri;
import android.os.Bundle;

import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import java.sql.Timestamp;

public class SecureCaptureFragment extends Capture
{
    private static final String ARG_PARAM1 = "cam";
    private static final String ARG_PARAM2 = "fps_display";
    private static final String ARG_PARAM3 = "sessionID";
    private static final String ARG_PARAM4 = "timestamp_display";

    protected MediaSecureProcessorProxy mSecureProcessorProxy;
    private MediaSecureProcessorConfig mOutSecConfig;

    private OnFragmentInteractionListener mListener;

    protected static ConfiguredCamera[] mConfiguredCameras = null;

    public SecureCaptureFragment()
    {
    }

    static SecureCaptureFragment newInstance(ConfiguredCamera camera,
                                             int fps_display, int sessionID,
                                             int timestamp_display)
    {
        SecureCaptureFragment fragment = new SecureCaptureFragment();
        Bundle args = new Bundle();
        args.putParcelable(ARG_PARAM1, camera);
        args.putInt(ARG_PARAM2, fps_display);
        args.putInt(ARG_PARAM3, sessionID);
        args.putInt(ARG_PARAM4, timestamp_display);
        fragment.setArguments(args);

        mConfiguredCameras = MainActivity.cams;
        return fragment;
    }

    boolean isSecurePreviewSelected()
    {
        boolean securePreviewIsSelected = false;

        for (ConfiguredCamera cam : mConfiguredCameras) {
            if (cam == null) {
                continue;
            }

            if (cam.isChosenForSecurePreview()) {
                Log.d(getLogTag(),
                      "Secure preview is selected. Skipping reset camera.");
                securePreviewIsSelected = true;
                break;
            }
        }

        return securePreviewIsSelected;
    }

    @Override public void onViewCreated(View view, Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);

        Log.d(getLogTag(), "CREATED Secure Capture");
        cameraManager = (CameraManager)getActivity().getSystemService(
            Context.CAMERA_SERVICE);
        Log.d(getLogTag(), "CREATED cam manager");

        // Set surface dimensions based on chosen camera width & height
        mCaptureDimensions = new Size(cam.getIntWidth(), cam.getIntHeight());
        Log.d(getLogTag(), "SECURE CAPTURE IMAGE READER Created"
                               + " - "
                               + "DIMENSIONS " + cam.getIntWidth() + "x" +
                               cam.getIntHeight());

        numFrames = 0;
        currentFPS = 0;
        startTimestamp = 0;

        Log.d(getLogTag(), "CREATED view " + fps_view);

        // Image reader to listen to ImageAvailable event
        mRawImageReader = ImageReader.newInstance(
            mCaptureDimensions.getWidth(), mCaptureDimensions.getHeight(),
            Integer.parseInt(cam.getFormat()), 2, USAGE_PROTECTED);
        mRawImageReader.setOnImageAvailableListener(onRawImageAvailableListener,
                                                    mBackgroundHandler);
        Log.d(getLogTag(), "IMAGE READER IS: " + mRawImageReader.toString());

        // Get secureprocessor object that was creating in MainActivity
        mSecureProcessorProxy = MainActivity.mSecureProcessorProxy;
    }

    @Override public void onResume()
    {
        Log.d(getLogTag(), "onResume SecureCapture");
        startBackgroundThread();

        if (mRawImageReader == null) {
            mRawImageReader = ImageReader.newInstance(
                mCaptureDimensions.getWidth(), mCaptureDimensions.getHeight(),
                Integer.parseInt(cam.getFormat()), 2, USAGE_PROTECTED);
            mRawImageReader.setOnImageAvailableListener(
                onRawImageAvailableListener, mBackgroundHandler);
        }
        Log.d(getLogTag(), "OPENING Camera ID " + cam.getCamId());
        openCamera();
        super.onResume();
    }

    @Override public void onPause()
    {
        Log.d(getLogTag(), "onPause SecureCapture");
        closeCamera();
        super.onPause();
    }

    @Override public void onStop()
    {
        Log.d(getLogTag(), "onStop SecureCapture");
        stopBackgroundThread();
        super.onStop();
    }

    private final ImageReader
        .OnImageAvailableListener onRawImageAvailableListener =
        new ImageReader.OnImageAvailableListener() {

            // Objects to set frame-configs for each available frame
            MediaSecureProcessorConfig outConfig = null;
            MediaSecureProcessorConfig frameConfig = null;

            @Override public void onImageAvailable(ImageReader reader)
            {
                frameConfig = new MediaSecureProcessorConfig(10, 100);

                Image image = reader.acquireLatestImage();
                if (image == null) {
                    return;
                }

                int[] camid = {Integer.parseInt(cam.getCamId())};
                long timestamp = image.getTimestamp();
                Timestamp toDisplay =
                    new Timestamp(timestamp / 1000000);  // Timestamp takes
                                                         // milliseconds. Image
                                                         // gives nanoseconds.
                Log.d(getLogTag(),
                      "CAMERA - " + cam.getCamId() + " - NEW Secure Frame : " +
                          timestamp + " - " + toDisplay.toString() + " - " +
                          image.getFormat() + " - " + image.getWidth() + "x" +
                          image.getHeight());

                // Set frame-configs cameraID and timestamp
                long[] tmpstmp = {timestamp};
                int ret = frameConfig.addEntry(
                    frameConfig.IMAGE_CONFIG_CAMERA_ID, camid, 1);
                ret = frameConfig.addEntry(frameConfig.IMAGE_CONFIG_TIMESTAMP,
                                           tmpstmp, 1);

                HardwareBuffer hbuf = image.getHardwareBuffer();
                if (hbuf == null) {
                    image.close();
                    return;
                }

                if (numFrames == 0) {
                    // Demonstrate reseting secure memory between sessions
                    int retVal = resetSecureCamera();
                    if (ret != 0) {
                        closeCamera();
                        sendErrIntentToMainActivity();
                    }
                }
                setRepeatCapture();

                outConfig =
                    mSecureProcessorProxy.processImage(hbuf, frameConfig);

                Log.d(getLogTag(), "ProcessImage: cam: " + camid[0] + " - " +
                                       mSecureProcessorProxy.getSessionId() +
                                       " - " + hbuf + " - " + frameConfig);

                hbuf.close();
                image.close();

                // Get timestamp from TA
                ConfigEntry entry = null;
                if (outConfig != null) {
                    entry =
                        outConfig.getEntry(MainActivity.IMAGE_CONFIG_TIMESTAMP);
                }
                String ta_timestamp = "NONE";
                if (entry != null) {
                    Timestamp timestamp_obj;
                    if (entry.i64Data != null) {
                        timestamp_obj = new Timestamp(entry.i64Data[0]);
                        ta_timestamp = timestamp_obj.toString();
                    }
                }

                // Print NONE if TA unavailable or responded null
                Log.d(getLogTag(), "Timestamp from TA: " + ta_timestamp);

                if (numFrames == 0) {
                    startTimestamp = timestamp;
                } else {
                    currentFPS =
                        (numFrames * 1000000000) /
                        (timestamp - startTimestamp);  // Nanoseconds to seconds
                    // fps_view.setText(getString(R.string.secure_capture_fps,
                    // currentFPS));
                    // timestamp_view.setText(getString(R.string.ta_timestamp_text,
                    // ta_timestamp));
                }

                numFrames++;
            }
        };

    @Override public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
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
        return inflater.inflate(R.layout.fragment_secure_capture, container,
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

    @Override protected int resetSecureCamera()
    {
        int ret = 0;

        // Reset camera should not be called while
        // any secure preview is in process as it
        // resets all protected cameras.
        // This is called here for demo purpose.
        // It should be called in TA.
        if (!isSecurePreviewSelected()) {
            ret = mSecureProcessorProxy.resetCamera();
            Log.d(getLogTag(), "reset camera ret = " + ret);
        }
        return ret;
    }

    public interface OnFragmentInteractionListener {
        void onFragmentInteraction(Uri uri);
    }
}
