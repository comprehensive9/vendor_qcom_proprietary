/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @file Metadata.java
 */
package com.qualcomm.qti.mmca.vpt.classification;

import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.Vector;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class Metadata {

    public static String JSON_RECORDS       = "records";
    public static String JSON_SESSION_INFO  = "session_info";
    public static String JSON_FRAME_RATE    = "frame_rate";
    public static String JSON_NUM_BATCHES   = "num_batches";

    private static int JSON_INDENTATION_SPACES = 2;

    private List<ClassificationBatch> mBatches = new Vector();
    private SessionInfo mSessionInfo = new SessionInfo();

    private int mFrameRate;

    public void addBatch(ClassificationBatch r) {
        mBatches.add(r);
    }

    public List<ClassificationBatch> getBatches() {
        return mBatches;
    }

    public SessionInfo getSessionInfo() {
        return mSessionInfo;
    }

    public void setSessionInfo(SessionInfo si) {
        mSessionInfo = si;
    }

    public int getFrameRate() {
        return mFrameRate;
    }

    public void setFrameRate(int newFrameRate) {
        mFrameRate = newFrameRate;
    }

    public void writeToFileAsJSON(String filepath) throws IOException {
        FileWriter fw = new FileWriter(filepath);
        fw.write(asJSONString());
        fw.close();
    }

    public JSONObject asJSONObject() {
        JSONObject j = new JSONObject();
        JSONArray ja = new JSONArray();
        try {
            for (ClassificationBatch r : mBatches) {
                ja.put(r.asJSONObject());
            }
            j.put(JSON_SESSION_INFO, mSessionInfo.asJSONObject())
             .put(JSON_FRAME_RATE, mFrameRate)
             .put(JSON_NUM_BATCHES, mBatches.size())
             .put(JSON_RECORDS, ja);
        } catch (JSONException e) {
            // what do?
        }
        return j;
    }

    public String asJSONString() {
        try {
            return asJSONObject().toString(JSON_INDENTATION_SPACES);
        } catch (JSONException e) {
            return "";
        }
    }
}
