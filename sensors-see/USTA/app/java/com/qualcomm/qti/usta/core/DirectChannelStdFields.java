/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.usta.core;

public class DirectChannelStdFields {
    private int SensorHandle = -1;
    private boolean isCalibratead = false;
    private boolean isResampled = false;
    private String batchPeriod = null;
    private String flushPeriod = null;
    private boolean isFlushOnly = false;
    private boolean isMaxBatch = false;
    private boolean isPassive = false;
    private boolean isAttributes = false;

    public void resetAllFields() {
        isCalibratead = false;
        isResampled = false;
        batchPeriod = null;
    }

    public int getSensorHandle() {
        return SensorHandle;
    }

    public void setSensorHandle(int sensorHandle) {
        SensorHandle = sensorHandle;
    }

    public boolean isCalibratead() {
        return isCalibratead;
    }

    public void setCalibratead(boolean calibratead) {
        isCalibratead = calibratead;
    }

    public boolean isResampled() {
        return isResampled;
    }

    public void setResampled(boolean resampled) {
        isResampled = resampled;
    }

    public String getBatchPeriod() {
        return batchPeriod;
    }

    public void setBatchPeriod(String batchPeriod) {
        this.batchPeriod = batchPeriod;
    }

    public String getFlushPeriod() {
        return flushPeriod;
    }

    public void setFlushPeriod(String flushPeriod) {
        this.flushPeriod = flushPeriod;
    }

    public boolean isFlushOnly() {
        return isFlushOnly;
    }

    public void setFlushOnly(boolean flushOnly) {
        isFlushOnly = flushOnly;
    }

    public boolean isMaxBatch() {
        return isMaxBatch;
    }

    public void setMaxBatch(boolean maxBatch) {
        isMaxBatch = maxBatch;
    }

    public boolean isPassive() {
        return isPassive;
    }

    public void setPassive(boolean passive) {
        isPassive = passive;
    }

    public boolean isAttributes() {
        return isAttributes;
    }

    public void setAttributes(boolean attributes) {
        isAttributes = attributes;
    }

}
