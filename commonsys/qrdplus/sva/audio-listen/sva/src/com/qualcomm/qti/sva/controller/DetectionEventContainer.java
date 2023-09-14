/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.controller;

import com.qualcomm.listen.ListenTypes;
import com.qualcomm.qti.sva.session.OpaqueDataHelper.DetectionConfLevel;

public class DetectionEventContainer {
    private final String TAG = DetectionEventContainer.class.getSimpleName();

    private int mEventType;
    private ListenTypes.EventData mEventData;
    private ListenTypes.VoiceWakeupDetectionDataV2 mDetectionData;
    private int mSessionId;
    private String mSmFullName;
    private DetectionConfLevel mConfLevel;

    public DetectionEventContainer(int eventType,
                                   ListenTypes.EventData eventData,
                                   ListenTypes.VoiceWakeupDetectionDataV2 detectionData,
                                   int sessionId, String smFullName,
                                   DetectionConfLevel confLevel) {
        this.mEventType = eventType;
        this.mEventData = eventData;
        this.mDetectionData = detectionData;
        this.mSessionId = sessionId;
        this.mSmFullName = smFullName;
        this.mConfLevel = confLevel;
    }

    public int getEventType() {
        return mEventType;
    }

    public ListenTypes.EventData getEventData() {
        return mEventData;
    }

    public int getSessionId() {
        return mSessionId;
    }

    public String getSmFullName() {
        return mSmFullName;
    }

    public ListenTypes.VoiceWakeupDetectionDataV2 getDetectionData() {
        return mDetectionData;
    }

    public DetectionConfLevel getConfLevel() {
        return mConfLevel;
    }
}
