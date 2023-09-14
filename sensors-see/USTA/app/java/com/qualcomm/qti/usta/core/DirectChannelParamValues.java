/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.usta.core;

public class DirectChannelParamValues {

    private static DirectChannelParamValues self = null;

    private int currentChannelTypePosition;
    private int currentClientTypeSpinnerPosition;

    private boolean isDeleteChannel = false;

    private boolean isSetUpConfigReqFragment = false;
    private boolean isRemoveReqFragment = false;
    private boolean isCalibrated = false;
    private boolean isResampled = false;
    private boolean isTimeStampOffsetFragment = true;

    private boolean isNewChannel;
    private int channelType;
    private int clientProc;

    private boolean isSensorInfoFragment;
    private long channelNumber;
    private int sensorHandle;


    private int reqMsgHandlePosition;

    public boolean isDeleteChannel() {
        return isDeleteChannel;
    }

    public void setDeleteChannel(boolean deleteChannel) {
        isDeleteChannel = deleteChannel;
    }

    public int getSensorHandle() {
        return sensorHandle;
    }

    public void setSensorHandle(int sensorHandle) {
        this.sensorHandle = sensorHandle;
    }

    public boolean isCalibrated() {
        return isCalibrated;
    }

    public void setCalibrated(boolean calibrated) {
        isCalibrated = calibrated;
    }

    public void setResampled(boolean resampled) {
        isResampled = resampled;
    }

    public boolean isResampled() {
        return isResampled;
    }

    public boolean isRemoveReqFragment() {
        return isRemoveReqFragment;
    }

    public void setRemoveReqFragment(boolean removeReqFragment) {
        isRemoveReqFragment = removeReqFragment;
        if(true == removeReqFragment) {
            isSetUpConfigReqFragment = false;
            isRemoveReqFragment = true;
            isTimeStampOffsetFragment = false;
        }
    }

    public boolean isSetUpConfigReqFragment() {
        return isSetUpConfigReqFragment;
    }

    public void setSetUpConfigReqFragment(boolean setUpConfigReqFragment) {
        isSetUpConfigReqFragment = setUpConfigReqFragment;

        if(true == setUpConfigReqFragment) {
            isSetUpConfigReqFragment = true;
            isRemoveReqFragment = false;
            isTimeStampOffsetFragment = false;
        }
    }

    public boolean isTimeStampOffsetFragment() {
        return isTimeStampOffsetFragment;
    }

    public void setTimeStampOffsetFragment(boolean timeStampOffsetFragment) {
        isTimeStampOffsetFragment = timeStampOffsetFragment;
        if(true == timeStampOffsetFragment) {
            isSetUpConfigReqFragment = false;
            isRemoveReqFragment = false;
            isTimeStampOffsetFragment = true;
        }
    }

    public void setChannelType(int channelType) {
        this.channelType = channelType;
    }

    public int getChannelType() {
        return channelType;
    }

    public void setClientProc(int clientProc) {
        this.clientProc = clientProc;
    }

    public int getClientProc() {
        return clientProc;
    }

    public long getChannelNumber() {
        return channelNumber;
    }

    public void setChannelNumber(long channelNumber) {
        this.channelNumber = channelNumber;
    }


    public int getCurrentChannelTypePosition() {
        return currentChannelTypePosition;
    }

    public void setCurrentChannelTypePosition(int currentChannelTypePosition) {
        this.currentChannelTypePosition = currentChannelTypePosition;
    }

    public int getCurrentClientTypeSpinnerPosition() {
        return currentClientTypeSpinnerPosition;
    }

    public void setCurrentClientTypeSpinnerPosition(int currentClientTypeSpinnerPosition) {
        this.currentClientTypeSpinnerPosition = currentClientTypeSpinnerPosition;
    }

    public boolean isNewChannel() {
        return isNewChannel;
    }

    public void setNewChannel(boolean newChannel) {
        isNewChannel = newChannel;
        if(true == newChannel) {
        /*All are false related to existing channel - Resetting the values */
    }
}

    public static DirectChannelParamValues getInstance() {
        if(null == self) {
            self = new DirectChannelParamValues();
        }
        return self;
    }

    public int getReqMsgHandlePosition() {
        return reqMsgHandlePosition;
    }

    public void setReqMsgHandlePosition(int reqMsgHandlePosition) {
        this.reqMsgHandlePosition = reqMsgHandlePosition;
    }
}
