/*=============================================================================
Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.Context;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.os.Bundle;

import android.preference.ListPreference;
import android.preference.MultiSelectListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.EditTextPreference;
import android.preference.SwitchPreference;
import android.util.Log;
import android.util.Range;
import android.util.Size;

import java.util.Collections;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

// This class deals with changes in configurations
public class ConfigActivity extends PreferenceActivity {
    private static final String LOG_TAG = "SecCamTest";
    public static int BACK_CAMERA_ID = -1;
    public static int FRONT_CAMERA_ID = -1;
    public static int MONO_CAMERA_ID = -1;

    static Map<Integer, String> formatMap = new HashMap<>();
    static Map<Integer, String> cameraMap = new HashMap<>();

    private static CameraManager camManager;

    static ListPreference fps_pref;
    static ListPreference format_pref;
    static ListPreference resolution_pref;
    static ListPreference config_pref;
    static ListPreference camera_pref;
    static ListPreference destination_pref;
    static ListPreference optional_preview_format, optional_preview_resolution;
    static MultiSelectListPreference mode_pref;
    static SwitchPreference timestamp_pref;
    static EditTextPreference usecase_pref;

    static int chosenIndx;
    private static boolean toRecreate;
    private static String chosenCamera = "";
    private static String secure_mode, capture_mode, preview_mode, secure_preview_mode;

    // Different available configs
    private static ArrayList<String> config_list;
    private static ArrayList<String> timestamp_list;
    private static ArrayList<String> camera_list;
    private static ArrayList<String> fps_list;
    private static ArrayList<String> resolution_list;
    private static ArrayList<String> mode_list;
    private static ArrayList<String> destination_list;
    private static ArrayList<String> usecase_list;
    private static ArrayList<String> format_list;

    private static Activity activity;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        activity = this;
        toRecreate = false;
        secure_mode = getString(R.string.mode_secure_capture);
        preview_mode = getString(R.string.mode_preview);
        capture_mode = getString(R.string.mode_capture);
        secure_preview_mode = getString(R.string.mode_secure_preview);

        // Check which config tab was open the last time. Reopen the same
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        String config_chosen = sharedPreferences.getString(getString(R.string.key_select_config), null);
        if (config_chosen == null) {
            chosenIndx = 0;
        } else {
            chosenIndx = Integer.parseInt(config_chosen);
        }

        initializeLists();
        initializeCameraAndFormat();
        getFragmentManager().beginTransaction().replace(android.R.id.content, new MainPreferenceFragment()).commit();
    }

    public static class MainPreferenceFragment extends PreferenceFragment {

        @Override
        public void onCreate(final Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            addPreferencesFromResource(R.xml.config_content);

            if (chosenIndx == 0) {
                PreferenceCategory preferenceCategory = (PreferenceCategory) findPreference(getString(R.string.category_2));
                preferenceCategory.removeAll();
            } else {
                PreferenceCategory preferenceCategory = (PreferenceCategory) findPreference(getString(R.string.category_1));
                preferenceCategory.removeAll();
            }

            // Initialize preference objects
            camera_pref = (ListPreference) findPreference(camera_list.get(chosenIndx));
            fps_pref = (ListPreference) findPreference(fps_list.get(chosenIndx));
            format_pref = (ListPreference) findPreference(format_list.get(chosenIndx));
            resolution_pref = (ListPreference) findPreference(resolution_list.get(chosenIndx));
            config_pref = (ListPreference) findPreference(getString(R.string.key_select_config));
            destination_pref = (ListPreference) findPreference(destination_list.get(chosenIndx));
            timestamp_pref = (SwitchPreference) findPreference(timestamp_list.get(chosenIndx));
            usecase_pref = (EditTextPreference) findPreference(usecase_list.get(chosenIndx));
            mode_pref = (MultiSelectListPreference) findPreference(mode_list.get(chosenIndx));
            optional_preview_format = (ListPreference) findPreference(getString(R.string.key_select_preview_format));
            optional_preview_resolution= (ListPreference) findPreference(getString(R.string.key_select_preview_resolution));

            // Query the cameras available in the device and populate the settings
            populateCameraPreferences(getContext());

            usecase_pref.setSummary(usecase_pref.getText());

            // Bind each setting to a listener, to manage the setting changes
            bindListPreferenceSummaryToValue(resolution_pref);
            bindListPreferenceSummaryToValue(format_pref);
            bindListPreferenceSummaryToValue(fps_pref);
            bindListPreferenceSummaryToValue(config_pref);
            bindListPreferenceSummaryToValue(destination_pref);
            bindListPreferenceSummaryToValue(usecase_pref);
            bindListPreferenceSummaryToValue(camera_pref);
            bindListPreferenceSummaryToValue(optional_preview_format);
            bindListPreferenceSummaryToValue(optional_preview_resolution);

            bindMultiListPreferenceSummaryToValue(mode_pref);

            bindTogglePreferenceSummaryToValue(timestamp_pref);
        }

        // When settings open up, this is called to query for available cameras and create dropdown
        private void populateCameraPreferences(final Context context) {
            camManager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
            int cameraId_ = -1;

            try {
                List<CharSequence> camNames = new ArrayList<>();
                camNames.add("None");
                List<CharSequence> camValues = new ArrayList<>();
                camValues.add("");

                // Iterate on available cameras in the device and populate settings dropdown
                int count_front = -1;
                int count_back = -1;
                int count_external = -1;
                int count_front_logical = -1;
                int count_back_logical = -1;
                int count_back_depth = -1;
                int count_front_depth = -1;
                int count_external_logical = -1;
                int count_external_depth = -1;
                boolean rear_detected = false;
                boolean front_detected = false;

                for(String camId: camManager.getCameraIdList()) {
                    Log.d(LOG_TAG, "FOUND Camera ID - " + camId);

                    CameraCharacteristics characteristics = camManager.getCameraCharacteristics(camId);

                    if (characteristics.get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL) == CameraMetadata.INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY) {
                        Log.d(LOG_TAG, "ERROR: Camera ID - " + camId + ". Camera2 API Unsupported");
                        continue;
                    }

                    cameraId_ = Integer.parseInt(camId);
                    boolean isDepthSensor = Arrays.stream(characteristics.get(CameraCharacteristics.REQUEST_AVAILABLE_CAPABILITIES)).anyMatch( c -> c == CameraCharacteristics.REQUEST_AVAILABLE_CAPABILITIES_DEPTH_OUTPUT);
                    boolean isLogicalCamera = Arrays.stream(characteristics.get(CameraCharacteristics.REQUEST_AVAILABLE_CAPABILITIES)).anyMatch(c -> c == CameraCharacteristics.REQUEST_AVAILABLE_CAPABILITIES_LOGICAL_MULTI_CAMERA);

                    try {
                        if (characteristics.get(CameraCharacteristics.DEPTH_DEPTH_IS_EXCLUSIVE)) {
                            Log.d(LOG_TAG, "Detected Depth Camera");
                            isDepthSensor = true;
                        }
                    } catch (NullPointerException ne) {
                        Log.d(LOG_TAG, "Camera Characteristics does not have  DEPTH_DEPTH_IS_EXCLUSIVE set");
                        if (isDepthSensor) {
                            Log.d(LOG_TAG, "Detected Depth Camera");
                        }
                    }

                    if (characteristics.get(CameraCharacteristics.LENS_FACING) == CameraCharacteristics.LENS_FACING_BACK) {
                        count_back++;
                        if (isLogicalCamera) {
                            count_back_logical++;
                        }
                        if (isDepthSensor) {
                            count_back_depth++;
                        }
                        if (!isLogicalCamera && !isDepthSensor && !rear_detected) {
                            rear_detected = true;
                            BACK_CAMERA_ID = cameraId_;
                            camNames.add(context.getResources().getString(R.string.back_cam_name));
                            camValues.add(Integer.toString(BACK_CAMERA_ID));
                        }
                    }  else if (characteristics.get(CameraCharacteristics.LENS_FACING) == CameraCharacteristics.LENS_FACING_FRONT) {
                        count_front++;

                        if (isLogicalCamera) {
                            count_front_logical++;
                        }
                        if (isDepthSensor) {
                            count_front_depth++;
                        }
                        if (!isLogicalCamera && !isDepthSensor && !front_detected) {
                            front_detected = true;
                            FRONT_CAMERA_ID = cameraId_;
                            camNames.add(context.getResources().getString(R.string.front_cam_name));
                            camValues.add(Integer.toString(FRONT_CAMERA_ID));
                        }
                    }  else {
                        Log.d(LOG_TAG, "Unknown facing of Camera");
                    }
                }

                Object[] names = camNames.toArray(), values = camValues.toArray();
                camera_pref.setEntries(Arrays.copyOf(names, names.length, String[].class));
                camera_pref.setEntryValues(Arrays.copyOf(values, values.length, String[].class));

                if (values.length >= 2) {
                    if (camera_pref.getValue() == null) {
                        camera_pref.setValueIndex(chosenIndx);
                    }
                    camera_pref.setSummary(camera_pref.getEntry());
                }
            } catch (CameraAccessException e) {
                Log.d(LOG_TAG, "ERROR: Camera ID - " + cameraId_ + " is not accessible - " + e);
                throw new RuntimeException();
            }
        }
    }

    private static void bindTogglePreferenceSummaryToValue(Preference preference) {  // Bind the toggle-type settings to a listener
        if (preference != null) {
            preference.setOnPreferenceChangeListener(sBindPreferenceSummaryToValueListener);
            sBindPreferenceSummaryToValueListener.onPreferenceChange(preference, PreferenceManager.getDefaultSharedPreferences(preference.getContext()).getBoolean(preference.getKey(), false));
        }
    }

    private static void bindListPreferenceSummaryToValue(Preference preference) {  // Bind the dropdown-type settings to listeners
        if (preference != null) {
            preference.setOnPreferenceChangeListener(sBindPreferenceSummaryToValueListener);
            sBindPreferenceSummaryToValueListener.onPreferenceChange(preference, PreferenceManager.getDefaultSharedPreferences(preference.getContext()).getString(preference.getKey(), ""));
        }
    }

    private static void bindMultiListPreferenceSummaryToValue(Preference preference) {  // Bind the multi-select dropdown-type settings to listeners
        if (preference != null) {
            preference.setOnPreferenceChangeListener(sBindPreferenceSummaryToValueListener);
            sBindPreferenceSummaryToValueListener.onPreferenceChange(preference, PreferenceManager.getDefaultSharedPreferences(preference.getContext()).getStringSet(preference.getKey(), null));
        }
    }

    // On change of any preference, update the displayed selection and update dependent dropdowns
    private static Preference.OnPreferenceChangeListener sBindPreferenceSummaryToValueListener = new Preference.OnPreferenceChangeListener() {
        @Override
        public boolean onPreferenceChange(Preference preference, Object newValue) {
            if (preference instanceof ListPreference) {
                // If a listPreference has changed, update and display the changed value
                String stringValue = newValue.toString();
                ListPreference listPreference = (ListPreference) preference;
                int index = listPreference.findIndexOfValue(stringValue);
                boolean summaryChanged = false;

                if (!stringValue.equals("None") && !stringValue.equals("")) {
                    // Name of the preference that has changed
                    String changedKey = preference.getKey();

                    // If config number has been changed, load only config1 or config2 accordingly
                    if (config_list.contains(changedKey)) {
                        preference.setSummary(((ListPreference) preference).getEntry());
                        summaryChanged = true;
                        chosenIndx = Integer.parseInt(stringValue);
                        if (toRecreate) {
                            toRecreate = false;
                            activity.recreate();
                        } else {
                            toRecreate = true;
                        }
                    }
                    // If camera choice has changed, query fps/formats available & set dropdowns
                    else if (camera_list.contains(changedKey)) {
                        Log.d(LOG_TAG, "Camera changed to " + stringValue);
                        chosenCamera = stringValue;

                        if (!format_pref.isEnabled()) {
                            format_pref.setEnabled(true);
                        }

                        preference.setSummary(cameraMap.get(Integer.parseInt(stringValue)));
                        summaryChanged = true;
                        changeFPSOptions(stringValue);
                        changeFormatOptions(stringValue);
                    }
                    // If format choice has changed, query resolutions available & set dropdowns
                    else if (format_list.contains(changedKey)) {
                        Log.d(LOG_TAG, "Format changed to " + stringValue);

                        if (!resolution_pref.isEnabled()) {
                            resolution_pref.setEnabled(true);
                        }

                        preference.setSummary(formatMap.get(Integer.parseInt(stringValue)));
                        summaryChanged = true;
                        changeResolutionOptions(stringValue);
                    }
                    else if (resolution_list.contains(changedKey)) {
                        Log.d(LOG_TAG, "Resolution changed to " + stringValue);
                    }
                    else if (fps_list.contains(changedKey)) {
                        Log.d(LOG_TAG, "FPS Changed");
                    }

                    if (!summaryChanged) {
                        preference.setSummary(index >= 0 ? stringValue : null);
                    }
                } else {
                    preference.setSummary("None");
                }
            } else if (preference instanceof SwitchPreference) {
                // If a switchPreference has been toggled, update and display the new value
                boolean switched = ((SwitchPreference) preference).isChecked();
                preference.setSummary(!switched ? "Disabled" : "Enabled");
            } else if (preference instanceof MultiSelectListPreference) {
                // If mode has changed
                String stringValue = newValue.toString();
                preference.setSummary(stringValue);

                if (stringValue.contains(secure_mode) && !stringValue.contains(capture_mode)) {
                    toggleSecureOnlyConfig(true);
                } else {
                    toggleSecureOnlyConfig(false);
                }

                // Enable the extra preview resolution & format choices if both preview & capture are chosen together
                if (stringValue.contains(capture_mode) && stringValue.contains(preview_mode)) {
                    optional_preview_format.setEnabled(true);
                    optional_preview_resolution.setEnabled(true);
                } else {
                    optional_preview_format.setEnabled(false);
                    optional_preview_resolution.setEnabled(false);
                }
            } else {
                String stringValue = newValue.toString();
                preference.setSummary(stringValue);
            }

            return true;
        }
    };

    // Disable these options in non-secure mode
    private static void toggleSecureOnlyConfig(boolean value) {
        destination_pref.setEnabled(value);
        timestamp_pref.setEnabled(value);
        usecase_pref.setEnabled(value);
    }

    // Invoked to update the list of resolutions when format is changed
    private static void changeResolutionOptions(final String stringValue) {
        try {
            CameraCharacteristics characteristics = camManager.getCameraCharacteristics(chosenCamera);
            StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

            // Get available resolutions for this format & camera
            Size[] outputSizes = map.getOutputSizes(Integer.parseInt(stringValue));
            List<CharSequence> resolutionList = new ArrayList<>();

            // Add resolutions to dropdown in WIDTHxHEIGHT format
            for (Size outputSize : outputSizes) {
                if(!resolutionList.contains(outputSize.getWidth() + "x" + outputSize.getHeight())) {
                    resolutionList.add(outputSize.getWidth() + "x" + outputSize.getHeight());
                }
            }

            Collections.reverse(resolutionList);
            Object[] obj = resolutionList.toArray();

            // Insert values into concerned dropdown
            resolution_pref.setEntries(Arrays.copyOf(obj, obj.length, String[].class));
            resolution_pref.setEntryValues(Arrays.copyOf(obj, obj.length, String[].class));
            optional_preview_resolution.setEntries(Arrays.copyOf(obj, obj.length, String[].class));
            optional_preview_resolution.setEntryValues(Arrays.copyOf(obj, obj.length, String[].class));
            resolution_pref.setEnabled(true);

            // Initially when nothing is chosen, set a default
            if (obj.length >= 1) {
                if (resolution_pref.getValue() == null) {
                    resolution_pref.setValueIndex(0);
                    optional_preview_resolution.setValueIndex(0);
                }
                resolution_pref.setSummary(resolution_pref.getEntry());
                optional_preview_resolution.setSummary(optional_preview_resolution.getEntry());
            }

        } catch (CameraAccessException e) {
            Log.d(LOG_TAG, "ERROR: Camera ID - " + chosenCamera + " is not accessible - " + e);
            throw new RuntimeException();
        } catch (IllegalArgumentException e) {
            Log.d(LOG_TAG, "ERROR: Camera ID - " + chosenCamera + ": " + e);
        }
    }

    // Invoked to update the list of formats when camera is changed
    private static void changeFormatOptions(final String chosenCam) {
        try {
            CameraCharacteristics characteristics = camManager.getCameraCharacteristics(chosenCam);
            StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

            if(map != null) {
                // Get the formats supported by this camera
                int[] outputFormats = map.getOutputFormats();
                List<CharSequence> formatNames = new ArrayList<>();
                List<CharSequence> formatValues = new ArrayList<>();

                //  Create lists of format names & values, and assign them to dropdowns
                for (int outputFormat : outputFormats) {
                    if (formatMap.get(outputFormat) != null) {
                        formatNames.add(formatMap.get(outputFormat));
                        formatValues.add(Integer.toString(outputFormat));
                    }
                }

                Object[] obj_name = formatNames.toArray(), obj_values = formatValues.toArray();
                format_pref.setEntries(Arrays.copyOf(obj_name, obj_name.length, String[].class));
                format_pref.setEntryValues(Arrays.copyOf(obj_values, obj_values.length, String[].class));
                optional_preview_format.setEntries(Arrays.copyOf(obj_name, obj_name.length, String[].class));
                optional_preview_format.setEntryValues(Arrays.copyOf(obj_values, obj_values.length, String[].class));
                format_pref.setEnabled(true);

                // Initially when nothing is chosen, set a default
                if (obj_values.length >= 1) {
                    if (format_pref.getValue() == null) {
                        format_pref.setValueIndex(0);
                        optional_preview_format.setValueIndex(0);
                    }
                    format_pref.setSummary(format_pref.getEntry());
                    optional_preview_format.setSummary(optional_preview_format.getEntry());
                    changeResolutionOptions(format_pref.getValue());
                }
            }
        } catch (CameraAccessException e) {
            Log.d(LOG_TAG, "ERROR: Camera ID - " + chosenCam + " is not accessible - " + e);
            throw new RuntimeException();
        } catch (IllegalArgumentException e) {
            Log.d(LOG_TAG, "ERROR: Camera ID - " + chosenCam + ": " + e);
        }
    }

    // Invoked to update the list of FPS ranges when camera is changed
    private static void changeFPSOptions(final String chosenCam) {
        try {
            CameraCharacteristics characteristics = camManager.getCameraCharacteristics(chosenCam);

            // Get the available FPS ranges for tge chosen camera
            Range<Integer>[] fps = characteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);

            if (fps != null) {
                List<CharSequence> fpss = new ArrayList<>();
                for (Range<Integer> item: fps) {
                    fpss.add(item.toString());  // Create a list of FPS values and assign it to dropdown
                }

                Object[] fpsValues = fpss.toArray();
                fps_pref.setEntries(Arrays.copyOf(fpsValues, fpsValues.length, String[].class));
                fps_pref.setEntryValues(Arrays.copyOf(fpsValues, fpsValues.length, String[].class));
                fps_pref.setEnabled(true);

                // Initially when nothing is chosen, set a default
                if (fpsValues.length >= 1) {
                    if (fps_pref.getValue() == null) {
                        fps_pref.setValueIndex(0);
                    }
                    fps_pref.setSummary(fps_pref.getEntry());
                }
            }
        } catch (CameraAccessException e) {
            Log.d(LOG_TAG, "ERROR: Camera ID - " + chosenCam + " is not accessible - " + e);
            throw new RuntimeException();
        } catch (IllegalArgumentException e) {

        }
    }

    // To map numbers returned by API to user-friendly text
    private void initializeCameraAndFormat() {
        formatMap.put(34, getResources().getString(R.string.private_format));
        formatMap.put(35, getResources().getString(R.string.yuv_format));
        formatMap.put(32,getResources().getString(R.string.raw_format));
        cameraMap.put(FRONT_CAMERA_ID, getResources().getString(R.string.front_cam_name));
        cameraMap.put(BACK_CAMERA_ID, getResources().getString(R.string.back_cam_name));
        cameraMap.put(MONO_CAMERA_ID, getResources().getString(R.string.mono_cam_name));
    }

    // Lists with names of preferences (for iteration convenience)
    private void initializeLists() {
        timestamp_list = new ArrayList<String>(){
            {
                add(getString(R.string.key_timestamp_1));
                add(getString(R.string.key_timestamp_2));
            }
        };
        format_list= new ArrayList<String>(){
            {
                add(getString(R.string.key_select_format_1));
                add(getString(R.string.key_select_format_2));
            }
        };
        camera_list = new ArrayList<String>(){
            {
                add(getString(R.string.key_select_camera_1));
                add(getString(R.string.key_select_camera_2));
            }
        };
        fps_list = new ArrayList<String>(){
            {
                add(getString(R.string.key_select_fps_1));
                add(getString(R.string.key_select_fps_2));
            }
        };
        resolution_list = new ArrayList<String>(){
            {
                add(getString(R.string.key_select_resolution_1));
                add(getString(R.string.key_select_resolution_2));
            }
        };
        mode_list = new ArrayList<String>(){
            {
                add(getString(R.string.key_select_mode_1));
                add(getString(R.string.key_select_mode_2));
            }
        };
        destination_list = new ArrayList<String>(){
            {
                add(getString(R.string.key_select_destination_1));
                add(getString(R.string.key_select_destination_2));
            }
        };
        usecase_list = new ArrayList<String>() {
            {
                add(getString(R.string.key_select_usecaseID_1));
                add(getString(R.string.key_select_usecaseID_2));
            }
        };
        config_list = new ArrayList<String>() {
            {
                add(getString(R.string.key_select_config));
            }
        };
    }
}
