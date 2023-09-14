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
import android.net.Uri;
import android.os.Bundle;

import android.util.Log;
import android.util.Size;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.sql.Timestamp;

public class NonsecureCaptureFragment extends Capture {
    private static final String ARG_PARAM1 = "cam";
    private static final String ARG_PARAM2 = "fps_display";
    private static final String ARG_PARAM3 = "timestamp_display";

    private OnFragmentInteractionListener mListener;

    public NonsecureCaptureFragment() {
    }

    static NonsecureCaptureFragment newInstance(ConfiguredCamera camera, int fps_display, int timestamp_display) {
        NonsecureCaptureFragment fragment = new NonsecureCaptureFragment();
        Bundle args = new Bundle();
        args.putParcelable(ARG_PARAM1, camera);
        args.putInt(ARG_PARAM2, fps_display);
        args.putInt(ARG_PARAM3, timestamp_display);

        // On fragment creation, receive the arguments provided by CameraActivity
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        Log.d(getLogTag(), "CREATED Capture");
        cameraManager = (CameraManager) getActivity().getSystemService(Context.CAMERA_SERVICE); // Obtain camera service

        // Set capture dimensions based on chosen camera width & height
        mCaptureDimensions = new Size(cam.getIntWidth(), cam.getIntHeight());

        numFrames = 0;
        currentFPS = 0;
        startTimestamp = 0;

        fps_view = getActivity().findViewById(fps_id);  // To display dynamic fps
        timestamp_view = getActivity().findViewById(timestamp_id); // To display timestamp

        // Image reader to listen to ImageAvailable event
        mRawImageReader = ImageReader.newInstance(mCaptureDimensions.getWidth(), mCaptureDimensions.getHeight(), Integer.parseInt(cam.getFormat()), 2);
        mRawImageReader.setOnImageAvailableListener(onRawImageAvailableListener, mBackgroundHandler);

        Log.d(getLogTag(), "CAPTURE IMAGE READER Created" + " - " + "DIMENSIONS " + cam.getIntWidth() + "x" + cam.getIntHeight());
    }

    @Override
    public void onResume() {
        super.onResume();
        startBackgroundThread();
		
		if( mRawImageReader == null){
		 // Image reader to listen to ImageAvailable event
        mRawImageReader = ImageReader.newInstance(mCaptureDimensions.getWidth(), mCaptureDimensions.getHeight(), Integer.parseInt(cam.getFormat()), 2);
        mRawImageReader.setOnImageAvailableListener(onRawImageAvailableListener, mBackgroundHandler);
		}

        Log.d(getLogTag(), "OPENING Camera ID " + cam.getCamId());
        openCamera();
    }

    @Override
    public void onPause() {
        closeCamera();
        super.onPause();
    }

    @Override
    public void onStop() {
        stopBackgroundThread();
        super.onStop();
    }

    private final ImageReader.OnImageAvailableListener onRawImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            Image image =  reader.acquireLatestImage();
            long timestamp = image.getTimestamp();

            // Timestamp object. Takes arg milliseconds, Image gives nanoseconds.
            Timestamp toDisplay = new Timestamp(timestamp / 1000000);
            Log.d(getLogTag(), "CAMERA - " + cam.getCamId() + " - NEW Capture Frame : " + timestamp + " - " + toDisplay.toString() + " - " + image.getFormat() + " - " + image.getWidth() + "x" + image.getHeight());
            image.close();

            // Calculate and update fps value after every frame
            if (numFrames == 0) {
                startTimestamp = timestamp;
            } else {
                currentFPS = (numFrames * 1000000000) / (timestamp - startTimestamp);  // Nanoseconds to seconds for fps

                // Update the fps and timestamp after every frame
                //fps_view.setText(getString(R.string.capture_fps, currentFPS));
                //timestamp_view.setText(getString(R.string.timestamp_text, toDisplay.toString()));
            }

            numFrames++;
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // On creating the layout, receive the parameters from CameraActivity
        if(getArguments() != null) {
            cam = getArguments().getParcelable(ARG_PARAM1);
            fps_id = getArguments().getInt(ARG_PARAM2);
            timestamp_id = getArguments().getInt(ARG_PARAM3);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_capture, container, false);
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnFragmentInteractionListener) {
            mListener = (OnFragmentInteractionListener) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    public interface OnFragmentInteractionListener {
        void onFragmentInteraction(Uri uri);
    }
}
