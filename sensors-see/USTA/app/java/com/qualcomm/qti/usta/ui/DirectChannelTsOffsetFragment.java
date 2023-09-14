/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.DirectChannelParamValues;
import com.qualcomm.qti.usta.core.USTALog;

public class DirectChannelTsOffsetFragment extends Fragment {

    private View DirectChannelTsOffsetFragmentView;

    public static DirectChannelTsOffsetFragment createInstance() {
        DirectChannelTsOffsetFragment self = new DirectChannelTsOffsetFragment();
        return self;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        DirectChannelTsOffsetFragmentView =  inflater.inflate(R.layout.direct_channel_ts_offset_fragment, container, false);
        return DirectChannelTsOffsetFragmentView;
    }

}
