/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.DirectChannelParamValues;
import com.qualcomm.qti.usta.core.USTALog;

public class DirectChannelRemoveRequestFragment extends Fragment {

    private View DirectChannelRemoveRequestFragmentView;

    public static DirectChannelRemoveRequestFragment createInstance() {
        DirectChannelRemoveRequestFragment DirectChannelRemoveRequestFragmentInstance = new DirectChannelRemoveRequestFragment();
        return DirectChannelRemoveRequestFragmentInstance;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        DirectChannelRemoveRequestFragmentView = inflater.inflate(R.layout.direct_channel_remove_req_fragment, container, false);
        setUpCalibratedCheckBox();
        setUpResampledCheckBox();
        return DirectChannelRemoveRequestFragmentView;
    }

    private void setUpCalibratedCheckBox() {
        CheckBox calibratedCheckBox    =   (CheckBox) DirectChannelRemoveRequestFragmentView.findViewById(R.id.direct_channel_rmv_req_calibrated_id);
        calibratedCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    USTALog.i("Remove Req Fragment calibrated Check boc - clicked ");
                    DirectChannelParamValues.getInstance().setCalibrated(true);
                } else {
                    USTALog.i("Remove Req Fragment calibrated Check boc - un Clicked  ");
                    DirectChannelParamValues.getInstance().setCalibrated(false);
                }
            }
        });
    }

    private void setUpResampledCheckBox() {
        CheckBox fixedRateCheckBox    =   (CheckBox) DirectChannelRemoveRequestFragmentView.findViewById(R.id.direct_channel_rmv_req_resampled_id);
        fixedRateCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    DirectChannelParamValues.getInstance().setResampled(true);
                } else {
                    DirectChannelParamValues.getInstance().setResampled(false);
                }
            }
        });
    }

}
