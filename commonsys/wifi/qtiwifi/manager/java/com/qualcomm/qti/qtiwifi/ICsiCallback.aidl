/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qtiwifi;

/**
 * Interface for csi callback.
 *
 */

oneway interface ICsiCallback
{
    void onCsiUpdate(in byte[] info);
}
