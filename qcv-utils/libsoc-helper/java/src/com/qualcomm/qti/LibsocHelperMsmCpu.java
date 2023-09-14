/*
 * Copyright (c) 2020-21 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

public enum LibsocHelperMsmCpu {
    MSM_CPU_SDM845(321),
    MSM_CPU_SDM670(336),
    MSM_CPU_SM8150(339),
    MSM_CPU_KONA(356),
    MSM_CPU_SDM710(360),
    MSM_CPU_LITO(400),
    MSM_CPU_LAHAINA(415),
    APQ_CPU_LAHAINA(439),
    MSM_CPU_SHIMA(450),
    MSM_CPU_SM8325(501),
    APQ_CPU_SM8325P(502),
    MSM_CPU_YUPIK(475),
    APQ_CPU_YUPIK(499),
    MSM_CPU_YUPIK_IOT(497),
    APQ_CPU_YUPIKP_IOT(498),
    MSM_CPU_YUPIK_LTE(515),
    MSM_CPU_UNKNOWN(-1);

    private static final Map<Integer, LibsocHelperMsmCpu> lookup
        = new HashMap<Integer, LibsocHelperMsmCpu>();

    static {
        for (LibsocHelperMsmCpu MsmCpu :
            EnumSet.allOf(LibsocHelperMsmCpu.class))
            lookup.put(MsmCpu.getSocId(), MsmCpu);
    }

    private final int socId;

    private LibsocHelperMsmCpu(int socId) {
        this.socId = socId;
    }

    private int getSocId() {
        return socId;
    }

    public static LibsocHelperMsmCpu getMsmCpu(int socId) {
        return lookup.get(socId);
    }
}
