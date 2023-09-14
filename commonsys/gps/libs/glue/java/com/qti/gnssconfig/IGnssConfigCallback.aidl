/*  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.gnssconfig;

import java.com.qti.gnssconfig.RLConfigData;

oneway interface IGnssConfigCallback
{
    void getGnssSvTypeConfigCb(in int[] disabledSvTypeList);
    void getRobustLocationConfigCb(in RLConfigData rlConfigData);
}
