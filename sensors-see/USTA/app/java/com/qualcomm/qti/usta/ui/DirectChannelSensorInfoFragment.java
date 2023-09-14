/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.app.FragmentManager;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Spinner;
import android.widget.TextView;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.DirectChannelContext;
import com.qualcomm.qti.usta.core.DirectChannelParamValues;
import com.qualcomm.qti.usta.core.ModeType;
import com.qualcomm.qti.usta.core.SensorContext;
import com.qualcomm.qti.usta.core.USTALog;

public class DirectChannelSensorInfoFragment extends Fragment {

    private String DIRECT_CHANNEL_FRAGMENT_TAGS_TS_OFFSET_UPDATE_REQ = "TS_OFFSET_UPDATE_REQ";
    private String DIRECT_CHANNEL_FRAGMENT_TAGS_REMOVE_CONFIG_REQ = "REMOVE_CONFIG_REQ";
    private String DIRECT_CHANNEL_FRAGMENT_TAGS_SET_CONFIG_REQ = "SET_CONFIG_REQ";

    private View DirectChannelSensorInfoFragmentView;
    private SensorContext sensorContext; //TODO This should not directly call sensorContext. It should call directChannelContext and get data from it.
    private Spinner sensorListSpinner;
    private Spinner reqMsgListSpinner;

    private Button directChannelDeleteChannelButton;

    public static DirectChannelSensorInfoFragment createInstance() {
        DirectChannelSensorInfoFragment self = new DirectChannelSensorInfoFragment();
        return self;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        DirectChannelSensorInfoFragmentView = inflater.inflate(R.layout.direct_channel_sensor_info_fragment, container, false);
        sensorContext = SensorContext.getSensorContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI);
        setUpDeleteChannelCheckBox();
        setUpDeleteChannelButton();
        setUpSensorListSpinner();
        return DirectChannelSensorInfoFragmentView;
    }

    private void setUpDeleteChannelButton() {

        directChannelDeleteChannelButton = (Button) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_delete_Channel_button);

        directChannelDeleteChannelButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(true == DirectChannelParamValues.getInstance().isDeleteChannel() &&
                        false == DirectChannelParamValues.getInstance().isNewChannel()) {
                    DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).deleteDirectChannel(DirectChannelParamValues.getInstance().getChannelNumber());
                    USTALog.i("Channel Deleted - refresh the spinner ");
                }
            }
        });

        directChannelDeleteChannelButton.setEnabled(false);

    }

    private void setUpDeleteChannelCheckBox() {
        CheckBox calibratedCheckBox    =   (CheckBox) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_delete_channel_checkbox_id);
        calibratedCheckBox.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v) {
                if (((CheckBox)v).isChecked()) {
                    USTALog.i("Delete Channel Checkbox - clicked");
                    DirectChannelParamValues.getInstance().setDeleteChannel(true);
                    directChannelDeleteChannelButton.setEnabled(true);
                    /*Dark out the sensor Info fields  */
                } else {
                    USTALog.i("Delete Channel Checkbox - unclicked");
                    DirectChannelParamValues.getInstance().setDeleteChannel(false);
                    /*Dark out the Button */
                    directChannelDeleteChannelButton.setEnabled(false);
                }
            }
        });
    }

    private void setUpSensorListSpinner() {

        sensorListSpinner = (Spinner) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_sensors_list_spinner);
        ArrayAdapter<String> sensorListSpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, sensorContext.getSensorNames());

        sensorListSpinner.setAdapter(sensorListSpinnerAdapter);

        sensorListSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

                int sensorHandle;
                sensorHandle = sensorListSpinner.getSelectedItemPosition();

                DirectChannelParamValues.getInstance().setSensorHandle(sensorHandle);

                TextView suidLowTextView = (TextView) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_suid_low);
                suidLowTextView.setText(sensorContext.getSensors().get(sensorHandle).getSensorSUIDLow());

                TextView suidHighTextView = (TextView) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_suid_high);
                suidHighTextView.setText(sensorContext.getSensors().get(sensorHandle).getSensorSUIDHigh());

                TextView onChangeTextView = (TextView) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_on_change_value);
                if (true == sensorContext.getSensors().get(sensorHandle).isOnChangeSensor()) {
                    onChangeTextView.setText("True");
                } else {
                    onChangeTextView.setText("False");
                }

                TextView hwIDTextView = (TextView) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_hw_id_value);
                int hwID = sensorContext.getSensors().get(sensorHandle).getHwID();
                if (hwID == -1) {
                    hwIDTextView.setText("NA");
                } else {
                    hwIDTextView.setText(Integer.toString(hwID));
                }

                TextView rigitBodyTypeView = (TextView) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_rigid_body_type);
                int rigidBodyType = sensorContext.getSensors().get(sensorHandle).getRigidBodyType();
                if (rigidBodyType == -1) {
                    rigitBodyTypeView.setText("NA");
                } else {
                    rigitBodyTypeView.setText(Integer.toString(rigidBodyType));
                }

                setUpReqMsgListSpinner();

            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }

    private void setUpReqMsgListSpinner() {

        reqMsgListSpinner = (Spinner) DirectChannelSensorInfoFragmentView.findViewById(R.id.direct_channel_req_msg_spinner);
        ArrayAdapter<CharSequence> reqMsgListSpinnerAdapter = ArrayAdapter.createFromResource(getActivity().getApplicationContext(), R.array.direct_channel_config_array, android.R.layout.simple_spinner_item);
        reqMsgListSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        reqMsgListSpinner.setAdapter(reqMsgListSpinnerAdapter);

        reqMsgListSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getDirectChannelFieldInstance(DirectChannelParamValues.getInstance().getSensorHandle()).resetAllFields();
                int requestMsgType = position;
                switch (position) {
                    case 0:
                        clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_REMOVE_CONFIG_REQ);
                        clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_TS_OFFSET_UPDATE_REQ);
                        DirectChannelTab.updateSendConfigButtonStatus("" , Color.BLUE);
                        setUpConfigReqFragment();
                        DirectChannelParamValues.getInstance().setSetUpConfigReqFragment(true);
                        DirectChannelParamValues.getInstance().setCalibrated(false);
                        DirectChannelParamValues.getInstance().setResampled(false);

                        break;
                    case 1:
                        clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_SET_CONFIG_REQ);
                        clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_TS_OFFSET_UPDATE_REQ);
                        DirectChannelTab.updateSendConfigButtonStatus("" , Color.BLUE);
                        setUpRemoveReqFragment();
                        DirectChannelParamValues.getInstance().setRemoveReqFragment(true);
                        DirectChannelParamValues.getInstance().setCalibrated(false);
                        DirectChannelParamValues.getInstance().setResampled(false);
                        break;
                    case 2:
                        clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_SET_CONFIG_REQ);
                        clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_REMOVE_CONFIG_REQ);
                        DirectChannelTab.updateSendConfigButtonStatus("" , Color.BLUE);
                        setUpTimeStampOffsetReqFragment();
                        DirectChannelParamValues.getInstance().setTimeStampOffsetFragment(true);
                        DirectChannelParamValues.getInstance().setCalibrated(false);
                        DirectChannelParamValues.getInstance().setResampled(false);
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

    }

    private void setUpConfigReqFragment() {
        FragmentManager fm = getActivity().getFragmentManager();
        DirectChannelConfigRequestFragment ConfigRequestFragment = DirectChannelConfigRequestFragment.createInstance();
        if(fm.findFragmentById(R.id.direct_channel_setconfig_request_fragment) != null ) {
            fm.beginTransaction().replace(R.id.direct_channel_setconfig_request_fragment, ConfigRequestFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_SET_CONFIG_REQ).commit();
        } else {
            fm.beginTransaction().add(R.id.direct_channel_setconfig_request_fragment, ConfigRequestFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_SET_CONFIG_REQ).commit();
        }
    }

    private void setUpRemoveReqFragment() {
        FragmentManager fm = getActivity().getFragmentManager();
        DirectChannelRemoveRequestFragment RemoveRequestFragment = DirectChannelRemoveRequestFragment.createInstance();
        if(fm.findFragmentById(R.id.direct_channel_remove_request_fragment) != null ) {
            fm.beginTransaction().replace(R.id.direct_channel_remove_request_fragment, RemoveRequestFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_REMOVE_CONFIG_REQ).commit();
        } else {
            fm.beginTransaction().add(R.id.direct_channel_remove_request_fragment, RemoveRequestFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_REMOVE_CONFIG_REQ).commit();
        }
    }

    private void setUpTimeStampOffsetReqFragment() {
        FragmentManager fm = getActivity().getFragmentManager();
        DirectChannelTsOffsetFragment TsOffsetFragment = DirectChannelTsOffsetFragment.createInstance();
        if(fm.findFragmentById(R.id.direct_channel_timestamp_offset_fragment) != null ) {
            fm.beginTransaction().replace(R.id.direct_channel_timestamp_offset_fragment, TsOffsetFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_TS_OFFSET_UPDATE_REQ).commit();
        } else {
            fm.beginTransaction().add(R.id.direct_channel_timestamp_offset_fragment, TsOffsetFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_TS_OFFSET_UPDATE_REQ).commit();
        }
    }

    private void clearExistingFragmentByTag(String tag) {
        FragmentManager fm = getActivity().getFragmentManager();
        if(fm.findFragmentByTag(tag) != null) {
            fm.beginTransaction().remove(fm.findFragmentByTag(tag)).commit();
        }
    }

}
