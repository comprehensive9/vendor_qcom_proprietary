/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Spinner;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.DirectChannelContext;
import com.qualcomm.qti.usta.core.DirectChannelParamValues;
import com.qualcomm.qti.usta.core.ModeType;
import com.qualcomm.qti.usta.core.SensorContext;
import com.qualcomm.qti.usta.core.USTALog;

import java.util.Vector;

public class DirectChannelConfigRequestFragment extends Fragment {
    private View DirectChannelConfigRequestFragmentView;
    private Spinner reqMsgSpinner;
    private EditText batchPeriodEditText;
    private EditText flushPeriodEditText;

    public static DirectChannelConfigRequestFragment createInstance() {
        DirectChannelConfigRequestFragment self = new DirectChannelConfigRequestFragment();
        return self;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        DirectChannelConfigRequestFragmentView = inflater.inflate(R.layout.direct_channel_setconfig_req_fragment, container, false);
        setUpMsgIDSpinner();
        setUpCalibratedCheckBox();
        setUpResampledCheckBox();
        setupBatchPeriodEditText();
        setupFlushPeriodEditText();
        setupFlushOnlyCheckBox();
        setupMaxBatchCheckBox();
        setupIsPassiveCheckBox();
        setUpAttributesCheckBox();
        return DirectChannelConfigRequestFragmentView;
    }

    private void setUpMsgIDSpinner() {
        Vector<String> reqMsgArray = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getReqMsgList();
        reqMsgSpinner = (Spinner) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_msgID_id);
        ArrayAdapter<String> reqMsgSpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, reqMsgArray);
        reqMsgSpinner.setAdapter(reqMsgSpinnerAdapter);
        reqMsgSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                DirectChannelParamValues.getInstance().setReqMsgHandlePosition(position);
                setupSensorPayloadFragment(position);
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });
    }

    private void setupSensorPayloadFragment(int reqMsgHandle) {
        FragmentManager fm = getActivity().getFragmentManager();
        int sensorHandle = DirectChannelParamValues.getInstance().getSensorHandle();
        SensorContext sensorContext = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getSensorContext();
        USTALog.i("sensorsHandle " + sensorHandle);
        USTALog.i("reqhandlePosition " + reqMsgHandle);
        Fragment directChannelSensorPayloadFragment = SensorPayloadFragment.CreateSensorPayloadFragmentInstance(sensorHandle, reqMsgHandle, sensorContext, true);
        if (fm.findFragmentById(R.id.direct_channel_sensor_req_payload) != null) {
            fm.beginTransaction().replace(R.id.direct_channel_sensor_req_payload, directChannelSensorPayloadFragment).commit();
        } else {
            fm.beginTransaction().add(R.id.direct_channel_sensor_req_payload, directChannelSensorPayloadFragment).commit();
        }
    }

    private void setUpCalibratedCheckBox() {
        CheckBox calibratedCheckBox    =   (CheckBox) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_confg_req_calibrated_id);
        calibratedCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setCalibratead(true);
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setCalibratead(false);
                }
            }
        });
    }

    private void setUpResampledCheckBox() {
        CheckBox fixedRateCheckBox    =   (CheckBox) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_resampled_id);
        fixedRateCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setResampled(true);
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setResampled(false);
                }
            }
        });
    }

    private void setUpAttributesCheckBox() {
        CheckBox attributeCheckBox    =   (CheckBox) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_is_attributes);
        attributeCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setAttributes(true);
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setAttributes(false);
                }
            }
        });
    }

    private void setupFlushOnlyCheckBox() {
        CheckBox flushOnlyCheckBox    =   (CheckBox) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_flush_only_id);
        flushOnlyCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setFlushOnly(true);
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setFlushOnly(false);
                }
            }
        });
    }

    private void setupMaxBatchCheckBox() {
        CheckBox maxBatchCheckBox    =   (CheckBox) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_max_batch_id);
        maxBatchCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setMaxBatch(true);
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setMaxBatch(false);
                }
            }
        });
    }

    private void setupIsPassiveCheckBox() {
        CheckBox isPassiveCheckBox    =   (CheckBox) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_is_passive);
        isPassiveCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setPassive(true);
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setPassive(false);
                }
            }
        });
    }


    private void setupBatchPeriodEditText() {

        batchPeriodEditText = (EditText) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_batch_period_id);
        batchPeriodEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {

                if (v.getId() == R.id.direct_channel_config_req_batch_period_id && !hasFocus) {
                    InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
                }
            }
        });

        TextWatcher batchPeriodTextWatcher = new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

                if (s.length() != 0) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setBatchPeriod(s.toString());
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setBatchPeriod(null);
                }
            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        };

        batchPeriodEditText.addTextChangedListener(batchPeriodTextWatcher);
    }

    private void setupFlushPeriodEditText() {

        flushPeriodEditText = (EditText) DirectChannelConfigRequestFragmentView.findViewById(R.id.direct_channel_config_req_flush_period_id);
        flushPeriodEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {

                if (v.getId() == R.id.direct_channel_config_req_flush_period_id && !hasFocus) {
                    InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
                }
            }
        });

        TextWatcher flushPeriodTextWatcher = new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

                if (s.length() != 0) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setFlushPeriod(s.toString());
                } else {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).setFlushPeriod(null);
                }
            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        };

        flushPeriodEditText.addTextChangedListener(flushPeriodTextWatcher);
    }

}
