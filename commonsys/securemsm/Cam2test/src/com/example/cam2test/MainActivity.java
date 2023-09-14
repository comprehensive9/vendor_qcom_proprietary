/*=============================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import com.qualcomm.qti.cam2test.MediaSecureProcessorProxy;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig;
import com.qti.media.secureprocessor.MediaSecureProcessorConfig.ConfigEntry;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Switch;

import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.lang.Thread;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

import static com.qualcomm.qti.cam2test.ConfiguredCamera.verifyMutualValidity;

public class MainActivity extends AppCompatActivity
{
    private static final int REQUEST_CAMERA_PERMISSION = 0;
    static final int numSupportedCameras = 2;
    private boolean isSessionStarted_ = false;
    // TODO:::will probably add Intnts to maintain status info when secure Dual
    // cam use case is Added
    public static int flag_for_non_secure = 0;

    // Custom result code
    final static int RESULT_CAMERA_ERROR = Activity.RESULT_FIRST_USER;
    final static int RESULT_MSECUREPROCESSOR_ERROR =
        Activity.RESULT_FIRST_USER + 1;

    final static int REQUEST_CODE_CAMERAACTIVITY = 1;

    // Texts
    final static String SESSION_ID = "sessionID";
    final static String MSECUREPROCESSOR = "MEDIA_SECURE_PROCESSOR";
    final static String DEFAULT_DEST = "qti-tee";

    // Custom tags for addEntry
    final static String IMAGE_CONFIG_CAMERA_ID =
        "secureprocessor.image.config.camera_id";
    final static String IMAGE_CONFIG_FRAME_NUMBER =
        "secureprocessor.image.config.frame_number";
    final static String IMAGE_CONFIG_TIMESTAMP =
        "secureprocessor.image.config.timestamp";
    final static String IMAGE_CONFIG_FRAME_BUFFER_WIDTH =
        "secureprocessor.image.config.frame_buffer_width";
    final static String IMAGE_CONFIG_FRAME_BUFFER_HEIGHT =
        "secureprocessor.image.config.frame_buffer_height";
    final static String IMAGE_CONFIG_FRAME_BUFFER_STRIDE =
        "secureprocessor.image.config.frame_buffer_stride";
    final static String IMAGE_CONFIG_FRAME_BUFFER_FORMAT =
        "secureprocessor.image.config.frame_buffer_format";
    final static String SESSION_CONFIG_NUM_SENSOR =
        "secureprocessor.session.config.num_sensors";
    final static String SESSION_CONFIG_USECASE_IDENTIFIER =
        "secureprocessor.session.config.usecase_id";
    final static String SESSION_CONFIG_LICENSE =
        "secureprocessor.session.license";

    public static Boolean FDSwitchState = false;
    protected Switch FDSwitch;

    protected static MediaSecureProcessorConfig mInSecConfig = null;
    protected static MediaSecureProcessorProxy mSecureProcessorProxy = null;

    Menu settings;
    SharedPreferences
        sharedPreferences;  // To store configs and access across all activities

    // Objects to represent the configurations of the 2 cameras
    protected static ConfiguredCamera[] cams = new ConfiguredCamera[numSupportedCameras];

    private Thread.UncaughtExceptionHandler androidDefaultUEH;

    private Thread.UncaughtExceptionHandler handler =
        new Thread.UncaughtExceptionHandler() {
            public void uncaughtException(Thread thread, Throwable ex)
            {
                Log.e(getLogTag(), "Uncaught exception is: ", ex);

                if (mSecureProcessorProxy != null) {
                    Log.d(getLogTag(), "uncaughtException: delete session");
                    mSecureProcessorProxy.deleteSession();
                    alertAndClose("Exception encountered. Closing app.", true);
                }
            }
        };

    @Override protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        androidDefaultUEH = Thread.getDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(handler);

        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle("Start Capture/Preview");
        setSupportActionBar(toolbar);
        FDSwitch = (Switch)findViewById(R.id.FDSwitch);

        // Check if user has given camera access permission
        int permissionCheck = ContextCompat.checkSelfPermission(
            MainActivity.this, android.Manifest.permission.CAMERA);
        if (permissionCheck != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                MainActivity.this,
                new String[] {android.Manifest.permission.CAMERA},
                REQUEST_CAMERA_PERMISSION);
        }

        // The start button, onclick starts the preview/capture
        Button startBtn = findViewById(R.id.startCapture);
        startBtn.setOnClickListener(startListener);

        // Whenever any preference changes, this listener triggers and changes
        // the cam1 & cam2
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(
            getApplicationContext());
        sharedPreferences.registerOnSharedPreferenceChangeListener(listener);

        // Initialize objects using saved preferences
        for (int i = 0; i < numSupportedCameras; i++) {
            Log.d(getLogTag(), "Initialized camera object " + i);
            cams[i] = new ConfiguredCamera(this, sharedPreferences, i + 1);
            cams[i].showConfiguration();
        }

        // delete previous session if exists
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(
            getApplicationContext());
        sharedPreferences.registerOnSharedPreferenceChangeListener(listener);

        int prevSessionID = sharedPreferences.getInt(SESSION_ID, -1);

        Log.d(getLogTag(), "onCreate: prevSessionId = " + prevSessionID);

        // Create a SecureProcessor object with default destination
        mSecureProcessorProxy = new MediaSecureProcessorProxy();
        int ret =
            mSecureProcessorProxy.createSession(DEFAULT_DEST, prevSessionID);
        if (ret != 0) {
            alertAndClose(
                "Unable to create MediaSecureProcessor. setenforce 0 or provide correct destination.",
                true);
        }
        mSecureProcessorProxy.setLogTag(getLogTag());
    }

    @Override protected void onDestroy()
    {
        super.onDestroy();
        Log.d(getLogTag(), "onDestroy Main");

        // Before exiting the app, delete the secure session
        if (mSecureProcessorProxy != null) {
            mSecureProcessorProxy.deleteSession();
            mSecureProcessorProxy = null;
        }
    }

    @Override protected void onStart()
    {
        Log.d(getLogTag(), "onStart Main");
        super.onStart();
    }

    @Override protected void onRestart()
    {
        Log.d(getLogTag(), "onRestart Main");
        super.onRestart();
    }

    @Override protected void onResume()
    {
        Log.d(getLogTag(), "onResume Main");
        super.onResume();
    }

    @Override protected void onStop()
    {
        Log.d(getLogTag(), "onStop Main");
        super.onStop();
    }

    @Override protected void onPause()
    {
        Log.d(getLogTag(), "onPause Main");

        int sessionId = mSecureProcessorProxy.getSessionId();
        SharedPreferences.Editor ed = sharedPreferences.edit();
        ed.putInt(SESSION_ID, sessionId);
        ed.commit();

        Log.d(getLogTag(), "onPause write session ID = " + sessionId);

        super.onPause();
    }

    // Change the ConfiguredCamera objects to reflect setting changes
    SharedPreferences.OnSharedPreferenceChangeListener listener =
        new SharedPreferences.OnSharedPreferenceChangeListener() {
            public void onSharedPreferenceChanged(SharedPreferences prefs,
                                                  String key)
            {
                for (int i = 0; i < numSupportedCameras; i++) {
                    if (key.charAt(key.length() - 1) == (i + 1) + '0') {
                        Log.d(getLogTag(), "Updated object");
                        cams[i].updateObject(prefs, key);  // Update object
                                                           // corresponding to
                                                           // whichever
                                                           // preference was
                                                           // changed by user
                    }
                }

                if (key.contains("destination")) {  // If destination changed,
                                                    // delete and recreate
                                                    // SecureProcessor object
                                                    // and session
                    String new_dest = prefs.getString(key, null);

                    if (!new_dest.equals(DEFAULT_DEST)) {
                        Log.d(getLogTag(), "Now creating with " + new_dest);
                        if (mSecureProcessorProxy == null) {
                            mSecureProcessorProxy =
                                new MediaSecureProcessorProxy();
                        }

                        mSecureProcessorProxy.deleteSession();
                        mSecureProcessorProxy.createSession(new_dest, -1);
                    }
                }
            }
        };

    // Respond to a click on the Start button
    View.OnClickListener startListener = new View.OnClickListener() {
        @Override
        public void onClick(View v)
        {
        FDSwitchState = FDSwitch.isChecked();
            if (isSessionStarted_) {
                Log.d(getLogTag(), "ENDED Capture Session");
                isSessionStarted_ = false;
            } else {
                for (int i = 0; i < numSupportedCameras; i++) {
                    if (cams[i] == null) {
                        alertAndClose("ERROR: Config " + (i + 1) +
                                          "could not be initialized",
                                      false);
                    }

                    // Check if config is not chosen, or invalid
                    String reason = cams[i].isConfigValid(
                        getString(R.string.mode_secure_capture),
                        getString(R.string.mode_preview),
                        getString(R.string.mode_capture));
                    if (reason != null) {
                        alertAndClose(
                            "ERROR: Config " + (i + 1) + ": " + reason, false);
                        return;
                    }
                }

                String result = verifyMutualValidity(
                    cams[0], cams[1]);  // Check for conflicting/invalid
                                        // preferences between the two configs

                if (result != null) {
                    Log.d(getLogTag(), "ERROR: " + result);
                    alertAndClose("ERROR: " + result, false);
                } else {
                    int num_secure_sensors = 0;
                    ArrayList<ConfiguredCamera> camList =
                        new ArrayList<ConfiguredCamera>();

                    // Display all configurations
                    for (ConfiguredCamera cam : cams) {
                        cam.showConfiguration();

                        if (cam.getCamId() != null) {
                            camList.add(cam);

                            if (cam.isChosenForSecureCapture() ||
                                cam.isChosenForSecurePreview()) {
                                num_secure_sensors++;
                            }
                        }
                    }

                    mInSecConfig = new MediaSecureProcessorConfig(
                        10,
                        4096);  // New object to add number of sensors to config
                    Log.d(getLogTag(), "Opened new MediaSecureProcessorConfig");

                    // To inform TA about number of sensors to be used in this
                    // session
                    int[] numCams = {num_secure_sensors};
                    mInSecConfig.addEntry(
                        mInSecConfig.SESSION_CONFIG_NUM_SENSOR, numCams, 1);
                    Log.d(getLogTag(),
                          "addEntry NUM_SENSOR " + num_secure_sensors);

                    // Insert custom config for license
                    Map<String, Integer> lic_entry =
                        new HashMap<String, Integer>();
                    lic_entry.put(SESSION_CONFIG_LICENSE,
                                  MediaSecureProcessorProxy.CustomConfigLicense);
                    mInSecConfig.setCustomConfigs(lic_entry);

                    // Read License file from device
                    try {
                        String license_path = "/vendor/etc/pfm_lic.txt";
                        String content = new String(
                            Files.readAllBytes(Paths.get(license_path)));
                        mInSecConfig.addEntry(SESSION_CONFIG_LICENSE, content);
                    } catch (IOException e) {
                        Log.e(getLogTag(),
                              "License file read failed " + e.toString());
                    }
                    // Put the camera objects in the intent, and pass it on to
                    // CameraActivity
                    Intent intent =
                        new Intent(MainActivity.this, CameraActivity.class);
                    intent.putExtra("Cameras", camList);

                    Log.d(getLogTag(), "STARTED Session");
                    startActivityForResult(
                        intent,
                        REQUEST_CODE_CAMERAACTIVITY);  // Start CameraActivity
                }
            }
        }
    };

    @Override
    protected void onActivityResult(int requestCode, int resultCode,
                                    Intent data)
    {
        if (requestCode == REQUEST_CODE_CAMERAACTIVITY) {
            String err_mesg = "";

            switch (resultCode) {
                case Activity.RESULT_OK:
                case Activity.RESULT_CANCELED:
                    break;
                case RESULT_CAMERA_ERROR:
                    err_mesg = "ERROR: Camera has stopped working.";
                    break;
                case RESULT_MSECUREPROCESSOR_ERROR:
                    err_mesg =
                        "ERROR: Media secure processor has stopped working.";
                    break;
                default:
                    err_mesg =
                        "ERROR: Generic error resultCode = " + resultCode;
                    break;
            }

            flag_for_non_secure = 0;

            if (err_mesg != "" && mSecureProcessorProxy != null) {
                Log.d(getLogTag(), "onActivityResult: deleteSession");
                mSecureProcessorProxy.deleteSession();
                alertAndClose(err_mesg, true);
            }
        }
    }

    @Override public boolean onCreateOptionsMenu(Menu menu)
    {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        settings = menu;

        return true;
    }

    @Override public boolean onOptionsItemSelected(MenuItem item)
    {
        int id = item.getItemId();

        if (id == R.id.action_settings) {
            Intent configIntent =
                new Intent(MainActivity.this, ConfigActivity.class);
            startActivity(configIntent);  // Launch settings
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions,
                                           int[] grantResults)
    {
        super.onRequestPermissionsResult(requestCode, permissions,
                                         grantResults);

        if (requestCode == REQUEST_CAMERA_PERMISSION) {
            if (grantResults.length == 0 ||
                grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                alertAndClose(
                    "This app cannot run without camera access permission",
                    true);
            }
        }
    }

    // If fatal error, close app, else simply dismiss alert
    public void alertAndClose(String reason, final boolean toExit)
    {
        AlertDialog.Builder builder =
            new AlertDialog.Builder(MainActivity.this);
        builder.setMessage(reason);
        builder.setPositiveButton("CLOSE", null);
        final AlertDialog dialog = builder.show();

        final Button closeButton =
            dialog.getButton(AlertDialog.BUTTON_POSITIVE);
        closeButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                if (toExit) {
                    finish();
                    System.exit(0);
                } else {
                    dialog.dismiss();
                }
            }
        });
    }

    private String getLogTag()
    {
        return getString(R.string.log_tag);
    }
}
