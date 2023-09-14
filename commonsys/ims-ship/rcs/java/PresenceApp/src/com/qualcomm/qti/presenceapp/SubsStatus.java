/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class SubsStatus {
    final public static String imsRegistration = "IMS Registration Status";
    final public static String publishTrigger = "PublishTrigger Type";
    final public static String publishCmdStatus = "Publish Cmd Status";
    final public static String publishResponse = "Publish Response";
    Map<String, String> SubStatusList = new HashMap<String, String>();

    private boolean expanded;


    public void SubsStatus() {
        generateDummyData();
        this.expanded = false;
    }

    private void generateDummyData() {
        SubStatusList.put(imsRegistration, "True");
        SubStatusList.put(publishTrigger, "VOPS");
        SubStatusList.put(publishCmdStatus, "SUCCESS");
        SubStatusList.put(publishResponse, "200");
    }
    public void setData(String s, String status) {
        SubStatusList.put(s, status);
    }

    public List<String> getData() {
        ArrayList<String> strings = new ArrayList<String>();
        for(String key : SubStatusList.keySet()) {
            strings.add(key + ": " + SubStatusList.get(key));
        }
        return strings;
    }

    public boolean isExpanded() {
        return expanded;
    }
    public void setExpanded(boolean x) {
        this.expanded = x;
    }
}
