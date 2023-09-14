/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.connmgr;

import android.content.Context;
import android.view.View;
import com.qualcomm.qti.imscmservice.V2_2.IImsCmService;
import com.qualcomm.qti.imscmservice.V2_0.IImsCMConnection;
import com.qualcomm.qti.imscmservice.V2_0.configData;
import com.qualcomm.qti.imscmservice.V2_0.userConfig;
import com.qualcomm.qti.imscmservice.V2_0.deviceConfig;
import com.qualcomm.qti.imscmservice.V2_0.autoConfig;
import com.qualcomm.qti.imscmservice.V2_0.ConfigType;
import com.qualcomm.qti.imscmservice.V2_1.autoconfigTriggerReason;
import com.qualcomm.qti.imscmservice.V2_1.StatusCode;
import com.qualcomm.qti.imscmservice.V2_2.AutoConfigResponse;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;


public class ConnectionGlobalData  {
    final static String TAG = "CMTestApp:ConnectionGlobalData";
    final static String FEATURE_TAG_NAME = "name";
    final static String TAB_INSTANCE = "instance";

    public ConnectionManagerService.CMHalVersionWrapper connMgr = null;
    public int connMgrInitialised = 0 ;
    public static Context mContext = null;
    public int userData = 0;
    public long serviceHandle = 0;
    //random value for sub0 and sub1
    public int userDataArray[] = {6789, 9560};
    public configData configdata = null;
    public AutoConfigResponse acsResponse = null;
    public HashMap<Integer, String> userConfigData = new
                                      HashMap<Integer, String>();
    public HashMap<Integer, String> deviceConfigData = new
                                      HashMap<Integer, String>();
    public autoConfig autoConfigData = null;
    public short responseCode = 404;
    public String callID = "";
    public View primaryScreenView;

    //Service Listener
    public ConnectionManagerListenerImpl mConnMgrListener = null;

    //Map of Connection objects with feature tag
    public HashMap<String, CMConnection> connectionMap = new
                                      HashMap<String, CMConnection>();
    public int acsTriggerReason =
                          autoconfigTriggerReason.AUTOCONFIG_INVALID_TOKEN;
    public int unsupportedStatusCode = StatusCode.UNSUPPORTED;
    public ArrayList<String> parsedFtList = new ArrayList<String>();
    //Map of tag_name and tag_string
    public HashMap<String, String> parsedFeatureTagMap = new
                                      HashMap<String, String>();
    public boolean isFTRegistered = false;
    public HashMap<String, Boolean> featureTagStatusMap = new
                                      HashMap<String, Boolean>();
    //List of Fts for which connections are not yet created
    public List<String> connectionSpinnerList = new
                                      ArrayList<String>();
    //List of registered Fts
    public List<String> registeredConnectionSpinnerList = new
                                      ArrayList<String>();
    public List<String> connMgrStatusList = new ArrayList<String>();

}
