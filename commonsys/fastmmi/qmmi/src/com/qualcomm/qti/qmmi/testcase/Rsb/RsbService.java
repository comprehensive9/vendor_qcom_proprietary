/*
 * Copyright (c) 2017,2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Rsb;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class RsbService extends BaseService {
    @Override
    public void register() {

    }

    @Override
    public int run(TestCase testCase) {
        LogUtils.logi("rsbservice run");
        return 0;
    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }

}
