/* ======================================================================
*  Copyright (c) 2018 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.gnssconfig;

import java.com.qti.gnssconfig.IGnssConfigCallback;
import java.com.qti.gnssconfig.NtripConfigData;


interface IGnssConfigService {

    void registerCallback(in IGnssConfigCallback callback);
    void getGnssSvTypeConfig();
    void setGnssSvTypeConfig(in int[] disabledSvTypeList);
    void resetGnssSvTypeConfig();
    void getRobustLocationConfig();
    void setRobustLocationConfig(boolean enable, boolean enableForE911);
    void enablePreciseLocation(in NtripConfigData data);
    void disablePreciseLocation();
    void updateNtripGgaConsent(boolean optIn);
}
