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
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.DirectChannelParamValues;

public class DirectChannelNewChannelFragment extends Fragment {
    private View DirectChannelNewChannelFragment;
    private Spinner channelTypeSpinner;
    private Spinner clientProcTypeSpinner;


    public static DirectChannelNewChannelFragment createInstance() {
        DirectChannelNewChannelFragment self = new DirectChannelNewChannelFragment();
        return self;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        DirectChannelNewChannelFragment = inflater.inflate(R.layout.direct_channel_new_channel_fragment, container, false);
        setUpChannelTypeSpinner();
        setUpClientProcTypeSpinner();
        return DirectChannelNewChannelFragment;
    }

    private void setUpChannelTypeSpinner() {

        channelTypeSpinner = (Spinner) DirectChannelNewChannelFragment.findViewById(R.id.direct_channel_channelType_spinner);
        ArrayAdapter<CharSequence> channelTypeSpinnerAdapter = ArrayAdapter.createFromResource(getActivity().getApplicationContext(), R.array.direct_channel_channelType_array, android.R.layout.simple_spinner_item);
        channelTypeSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        channelTypeSpinner.setAdapter(channelTypeSpinnerAdapter);

        channelTypeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                DirectChannelParamValues.getInstance().setChannelType(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }

    private void setUpClientProcTypeSpinner() {

        clientProcTypeSpinner = (Spinner) DirectChannelNewChannelFragment.findViewById(R.id.direct_channel_clientProc_spinner);
        ArrayAdapter<CharSequence> channelTypeSpinnerAdapter = ArrayAdapter.createFromResource(getActivity().getApplicationContext(), R.array.direct_channel_clientProcType_array, android.R.layout.simple_spinner_item);
        channelTypeSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        clientProcTypeSpinner.setAdapter(channelTypeSpinnerAdapter);

        clientProcTypeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                DirectChannelParamValues.getInstance().setClientProc(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }

        });
    }

}
