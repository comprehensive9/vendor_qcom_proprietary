/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp.service;

public abstract class TaskListener {
    /**
     * Please note these callbacks will be triggered
     * on UI Context thread.
     * */
    public void onCommandStatus(String Status, int Id) {
        return;
    }
    public void onContactCapabilitesUpdated(String phoneNumber) {
        return;
    }
    public void onNotifyListReceived() {
        return;
    }
    public void onSipResponse(int Reasoncode, String reason) {return;}
    public void onPublishTrigger(String type) {return;}
    public void onRegistrationChange(String x) { return;}
}
