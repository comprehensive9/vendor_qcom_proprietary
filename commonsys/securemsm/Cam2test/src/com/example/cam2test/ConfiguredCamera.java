/*=============================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.cam2test;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;
import android.util.Range;
import android.util.Size;

import java.util.ArrayList;
import java.util.Set;

// This class abstracts a configuration
// Implements parcelable so it can be passed between activities as argument
public class ConfiguredCamera implements Parcelable {
    private final String LOG_TAG = "SecCamTest";

    // Attributes that can be configured
    private String camId;
    private ArrayList<String> mode;
    private int format;
    private Range<Integer> fps;
    private int lower; // Lower limit of fps range
    private int higher; // Upper limit of fps range
    private String usecaseID;
    private String destination;
    private int[] resolution;
    private boolean timestamp;

    ConfiguredCamera() {
        camId = null;
        mode = null;
        format = -1;
        fps = null;
        usecaseID = null;
        destination = null;
        resolution = null;
        timestamp = false;
    }

    // Initialize camera configurations based on stored preferences
    // If the preference is not saved, initialize as null
    // Else initialize based on the saved value
    ConfiguredCamera(Context context, SharedPreferences pref, int objId) {
        int[] cameras = {R.string.key_select_camera_1, R.string.key_select_camera_2};
        int[] modes = {R.string.key_select_mode_1, R.string.key_select_mode_2};
        int[] destinations = {R.string.key_select_destination_1, R.string.key_select_destination_2};
        int[] fpss = {R.string.key_select_fps_1, R.string.key_select_fps_2};
        int[] resolutions = {R.string.key_select_resolution_1, R.string.key_select_resolution_2};
        int[] formats = {R.string.key_select_format_1, R.string.key_select_format_2};
        int[] timestamps = {R.string.key_timestamp_1, R.string.key_timestamp_2};
        int[] usecaseIDs = {R.string.key_select_usecaseID_1, R.string.key_select_usecaseID_2};
        int indx = objId - 1;

        camId = pref.getString(context.getString(cameras[indx]), null);
        if (camId != null && camId.equals("")) {
            camId = null;
        }

        Set<String> modes_chosen = pref.getStringSet(context.getString(modes[indx]), null);
        if (modes_chosen != null) {
            mode = new ArrayList<>();
            mode.addAll(modes_chosen);
        } else {
            mode = null;
        }

        destination = pref.getString(context.getString(destinations[indx]), null);
        usecaseID = pref.getString(context.getString(usecaseIDs[indx]), null);

        String str_format = pref.getString(context.getString(formats[indx]), null);
        format = (str_format == null) ? -1 : Integer.parseInt(str_format);

        String str_fps = pref.getString(context.getString(fpss[indx]),null);
        fps = getFPSInRangeFormat(str_fps);

        String res = pref.getString(context.getString(resolutions[indx]), null);
        if(res == null) {
            resolution = null;
        } else {
            // Remove 'x' from WIDTHxHEIGHT string to obtain width & height values
            String[] resWH = res.split("x");
            resolution = new int[2];
            resolution[0] = Integer.parseInt(resWH[0]);
            resolution[1] = Integer.parseInt(resWH[1]);
        }

        timestamp = pref.getBoolean(context.getString(timestamps[indx]), false);
    }

    // Helps in passing the object from one activity to another
    private ConfiguredCamera(Parcel in) {
        camId = in.readString();
        mode = in.createStringArrayList();
        format = in.readInt();
        destination = in.readString();
        resolution = in.createIntArray();
        lower = in.readInt();
        higher = in.readInt();
        fps = new Range<>(lower, higher);
        timestamp = in.readBoolean();
        usecaseID = in.readString();
    }

    public static final Creator<ConfiguredCamera> CREATOR = new Creator<ConfiguredCamera>() {
        @Override
        public ConfiguredCamera createFromParcel(Parcel in) {
            return new ConfiguredCamera(in);
        }

        @Override
        public ConfiguredCamera[] newArray(int size) {
            return new ConfiguredCamera[size];
        }
    };

    void showConfiguration() {
        Log.d(LOG_TAG, "---------------------------------------------");
        Log.d(LOG_TAG, "Camera ID: " + camId);
        Log.d(LOG_TAG, "Mode: " + mode);
        Log.d(LOG_TAG, "Format: " + format);
        if (resolution != null) {
            Log.d(LOG_TAG, "Resolution: " + resolution[0] + "x" + resolution[1]);
        }
        Log.d(LOG_TAG, "FPS: " + fps);
        Log.d(LOG_TAG, "Usecase Identifier: " + usecaseID);
        Log.d(LOG_TAG, "Destination: " + destination);
        Log.d(LOG_TAG, "Timestamp exchange: " + timestamp);
        Log.d(LOG_TAG, "---------------------------------------------");
    }

    String getFormat() {
        return Integer.toString(format);
    }

    boolean isTimestamp() {
        return timestamp;
    }

    String getResolution() {
        return resolution[0] + "x" + resolution[1];
    }

    int getIntWidth() {
        if(resolution == null) {
            return -1;
        }
        return resolution[0];
    }

    int getIntHeight() {
        if(resolution == null) {
            return -1;
        }
        return resolution[1];
    }

    String getCamId() {
        return camId;
    }

    String getDestination() {
        return destination;
    }

    ArrayList<String> getMode() {
        return mode;
    }

    Range<Integer> getFps() {
        return fps;
    }

    public String getUsecaseID() {
        return usecaseID;
    }

    String isConfigValid(String secure_capture, String preview, String capture) {

        /*
        lATER check secure-secure
        */

        if (getCamId() == null) {
            return null;
        }

        if (mode == null) {
            return "Choose Mode(s)";
        } else if (format == -1) {
            return "Choose Format";
        } else if (destination == null) {
            return "Choose Destination";
        } else if (resolution == null) {
            return "Choose Resolution";
        } else if (fps == null) {
            return "Choose FPS";
        }

        if (mode.size() > 2) {
            return "Cannot run 2+ modes at once";
        } else if (mode.size() == 0) {
            return "Choose Mode(s)";
        } else if (mode.contains(secure_capture) && mode.contains(preview)) {
            return "Cannot run secure capture with non-secure preview";
        } else if (mode.contains(secure_capture) && mode.contains(capture)) {
            return "Cannot run secure & non-secure capture";
        }

        return null;
    }

    // Check for conflicting configurations
    static String verifyMutualValidity(ConfiguredCamera cam1, ConfiguredCamera cam2) {
        if(cam1 == null || cam2 == null)
            return "ERROR: Cameras could not be initialized";

        if(cam1.getCamId() == null && cam2.getCamId() == null) {
            return "Both Cameras cannot be None";
        }

        if(cam1.getCamId() != null && cam2.getCamId() != null) {
            if(cam1.getMode().contains("Non-Secure Preview") && cam2.getMode().contains("Non-Secure Preview")) {
                return "Both Cameras cannot preview at once";
            }

            if(cam1.getCamId().equals(cam2.getCamId() )) {
                return "Both cameras cannot be the same";
            }
        }

        return null;
    }

    // Return whether this camera is configured for preview
    boolean isChosenForPreview() {
        return (getCamId() != null) && (getMode().contains("Non-Secure Preview"));
    }

    // Return whether this camera is configured for capture
    boolean isChosenForCapture() {
        return (getCamId() != null) && getMode().contains("Non-Secure Capture");
    }

    // Return whether this camera is configured for secure capture
    boolean isChosenForSecureCapture() {
        return (getCamId() != null) && getMode().contains("Secure Capture");
    }

    // Return whether this camera is configured for secure preview
    boolean isChosenForSecurePreview() {
        return (getCamId() != null) && getMode().contains("Secure Preview");
    }

    Range<Integer> getFPSInRangeFormat(String str_fps) {
        if(str_fps == null || str_fps.equals("")) {
            return null;
        }

        // Remove '[' and ']' from FPS range string [low, high]
        StringBuilder stringBuilder = new StringBuilder(str_fps);
        stringBuilder.deleteCharAt(str_fps.length() - 1);
        stringBuilder.deleteCharAt(0);
        str_fps = stringBuilder.toString();

        try {
            // Split string "low, high" to get low and high values from FPS range
            String[] fpsLU = str_fps.split(", ");
            return new Range<>(Integer.parseInt(fpsLU[0]), Integer.parseInt(fpsLU[1]));
        } catch (NumberFormatException e) {
            Log.d(LOG_TAG, "ERROR: FPS was invalid " + str_fps);
            return null;
        }
    }

    // Update the ConfiguredCamera object based on changes in settings. Invoked by the listener
    void updateObject(SharedPreferences prefs, String key) {
        if(key.contains("camera")) {
            camId = prefs.getString(key, null);
            if (camId.equals("None") || camId.equals("")) {
                camId = null;
            }
        } else if(key.contains("mode")) {
            Set<String> modes_chosen = prefs.getStringSet(key, null);
            if (modes_chosen != null) {
                mode = new ArrayList<>();
                mode.addAll(modes_chosen);
            } else {
                mode = null;
            }
        } else if(key.contains("destination")) {
            destination = prefs.getString(key, null);
        } else if(key.contains("format")) {
            format = Integer.parseInt(prefs.getString(key, "-1"));
        } else if(key.contains("resolution")) {
            String res = prefs.getString(key, null);

            if(res == null) {
                resolution = null;
            }
            else {
                String[] resWH = res.split("x");
                resolution = new int[2];
                resolution[0] = Integer.parseInt(resWH[0]);
                resolution[1] = Integer.parseInt(resWH[1]);
            }
        } else if(key.contains("timestamp")) {
            timestamp = prefs.getBoolean(key, false);
        } else if (key.contains("fps")) {
            String str_fps = prefs.getString(key, null);

            fps = getFPSInRangeFormat(str_fps);
        }
    }

    static Size getResolutionFromString(String res) {
        if(res == null) {
            return null;
        }

        String[] resWH = res.split("x");
        return new Size(Integer.parseInt(resWH[0]), Integer.parseInt(resWH[1]));
    }

    // Function necessary to be implemented for Parcelable
    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(camId);
        parcel.writeStringList(mode);
        parcel.writeInt(format);
        parcel.writeString(destination);
        parcel.writeIntArray(resolution);
        parcel.writeInt(lower);
        parcel.writeInt(higher);
        parcel.writeBoolean(timestamp);
        parcel.writeString(usecaseID);
    }
}
