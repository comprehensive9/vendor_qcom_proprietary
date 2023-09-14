/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.usta.core;

public class DirectChannelinfo {
    private int channelNumber;
    private int channelType;
    private int clientProcType;

    public int getChannelNumber() {
        return channelNumber;
    }

    public void setChannelNumber(int channelNumber) {
        this.channelNumber = channelNumber;
    }

    public void setChannelType(int channelType) {
        this.channelType = channelType;
    }

    public void setClientProcType(int clientProcType) {
        this.clientProcType = clientProcType;
    }


}
