/*=============================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentActivity;
import androidx.fragment.app.FragmentTransaction;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.qti.media.secureprocessor.MediaSecureProcessor;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig.ConfigEntry;

import java.util.ArrayList;

// This class starts the capture/preview fragments
public class CameraActivity extends FragmentActivity
    implements SecurePreviewFragment.OnFragmentInteractionListener,
               PreviewFragment.OnFragmentInteractionListener,
               NonsecureCaptureFragment.OnFragmentInteractionListener,
               SecureCaptureFragment.OnFragmentInteractionListener
{
    protected static ArrayList<ConfiguredCamera> cams =
        new ArrayList<>(MainActivity.numSupportedCameras);
    private Button stopBtn;
    private int btnId;
    private int mSessionId;
    protected MediaSecureProcessorProxy mSecureProcessorProxy;

    // Session input configuration
    protected MediaSecureProcessorConfig mInSecConfig;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        stopBtn = findViewById(R.id.stop_capture);

        // Define what happens on clicking the stop button
        stopBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view)
            {
                FragmentManager fm = getSupportFragmentManager();
                for (int i = 0; i < fm.getBackStackEntryCount(); ++i) {
                    // Close the running capture/preview one-by-one
                    fm.popBackStack();
                }

                // Return to main activity homescreen
                finish();
            }
        });

        // Getting the secureprocessor object from main activity
        mSecureProcessorProxy = MainActivity.mSecureProcessorProxy;

        // Getting the configuration value from main activity
        mInSecConfig = MainActivity.mInSecConfig;

        Intent intent = getIntent();
        // Received 2 camera objects as parameters
        cams = intent.getParcelableArrayListExtra("Cameras");

        // dummy_1, fps_1, timestamp_1 represent left-side display
        // dummy_2, fps_2, timestamp_2 represent right-side display
        int[] displays = {R.id.dummy_1, R.id.dummy_2};
        int[] fps_views = {R.id.fps_1, R.id.fps_2};
        int[] timestamp_views = {R.id.timestamp_1, R.id.timestamp_2};
        int[] timestamp_exchanges = {R.id.exchange_timestamp_1,
                                     R.id.exchange_timestamp_2};
        int indx = 0;

        // Check the chosen modes and run capture/preview
        for (ConfiguredCamera cam : cams) {
            if (cam == null) {
                continue;
            }

            if (cam.isChosenForPreview()) {
                Log.d(getLogTag(), "STARTING Preview");
                Fragment fragment = PreviewFragment.newInstance(
                    cam, fps_views[indx], timestamp_views[indx]);
                initiateFragment(fragment, displays[indx]);
                indx++;
            } else if (cam.isChosenForCapture()) {
                Log.d(getLogTag(), "STARTING Capture");
                Fragment fragment = NonsecureCaptureFragment.newInstance(
                    cam, fps_views[indx], timestamp_views[indx]);
                initiateFragment(fragment, displays[indx]);
                indx++;
            } else if (mSecureProcessorProxy.getSessionId() != -1) {
                int ret = -1;
                MainActivity.flag_for_non_secure = 1;

                ret = mSecureProcessorProxy.setConfig(mInSecConfig);
                if (ret != 0) {
                    notify_finish(MainActivity.RESULT_MSECUREPROCESSOR_ERROR);
                    return;
                }
                ret = mSecureProcessorProxy.startSession();
                if (ret != 0) {
                    notify_finish(MainActivity.RESULT_MSECUREPROCESSOR_ERROR);
                    return;
                }

                if (cam.isChosenForSecurePreview()) {
                    Log.d(
                        getLogTag(),
                        "STARTING Secure Preview, session ID = " + mSessionId);
                    Fragment fragment = SecurePreviewFragment.newInstance(
                        cam, fps_views[indx], mSessionId,
                        timestamp_views[indx]);
                    initiateFragment(fragment, displays[indx]);

                } else if (cam.isChosenForSecureCapture()) {
                    Log.d(getLogTag(), "STARTING Secure Capture");
                    Fragment fragment = SecureCaptureFragment.newInstance(
                        cam, fps_views[indx], mSessionId,
                        timestamp_views[indx]);
                    initiateFragment(fragment, displays[indx]);

                    // Check if timestamp exchange was enabled by user
                    if (cam.isTimestamp()) {
                        Button timestampButton =
                            findViewById(timestamp_exchanges[indx]);

                        // Show timestamp exchange button
                        timestampButton.setVisibility(View.VISIBLE);
                        btnId = timestamp_exchanges[indx];
                    }
                }

                indx++;
            }
        }
    }

    private void initiateFragment(Fragment fragment, int display)
    {
        FragmentTransaction transaction =
            getSupportFragmentManager().beginTransaction();
        transaction.add(display, fragment);

        // Add to stack, to return to previous screen with back button
        transaction.addToBackStack(null);
        transaction.commit();  // Starts the required fragment
    }

    @Override
    protected void onStart() {
        Log.d(getLogTag(), "onStart CameraActivity");
        super.onStart();
    }

    @Override
    protected void onRestart() {
        Log.d(getLogTag(), "onRestart CameraActivity");
        super.onRestart();
    }

    @Override
    public void onStop()
    {
        Log.d(getLogTag(), "onStop CameraActivity");
        if(mSecureProcessorProxy != null) {
            mSecureProcessorProxy.stopSession();
        }
        super.onStop();
    }

    @Override
    public void onResume()
    {
        Log.d(getLogTag(), "onResume CameraActivity");
        if (mSecureProcessorProxy != null) {
            mSecureProcessorProxy.startSession();
        }
        super.onResume();
    }

    @Override
    public void onPause()
    {
        Log.d(getLogTag(), "onPause CameraActivity");
        super.onPause();
    }

    @Override
    public void onDestroy()
    {
        Log.d(getLogTag(), "onDestroy CameraActivity");
        super.onDestroy();
    }

    @Override
    public void onFragmentInteraction(Uri uri)
    {
    }

    private String getLogTag()
    {
        return getString(R.string.log_tag);
    }

    private void notify_finish(int result_code)
    {
        // Returning with result_code
        Intent returnIntent = new Intent();
        this.setResult(result_code, returnIntent);
    }
}
