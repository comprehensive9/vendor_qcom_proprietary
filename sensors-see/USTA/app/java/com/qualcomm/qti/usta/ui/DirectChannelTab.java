/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.usta.ui;

import android.app.FragmentManager;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.DirectChannelContext;
import com.qualcomm.qti.usta.core.DirectChannelParamValues;
import com.qualcomm.qti.usta.core.ModeType;
import com.qualcomm.qti.usta.core.USTALog;

import java.util.Vector;

public class DirectChannelTab extends Fragment {
    private String DIRECT_CHANNEL_FRAGMENT_TAGS_NEW_CHANNEL = "NEW_CHANNEL";
    private String DIRECT_CHANNEL_FRAGMENT_TAGS_SENSORINFO = "SENSOR_INFO";

    private View directChannelTabView;
    private Spinner channelNumberSpinner;
    private Button directChannelSendConfigButton;
    public static TextView sendConfigRequuestStatusTextView;

    DirectChannelContext directChannelContext;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);

        /*Creating instance of directChannelContext*/
        directChannelContext = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI);

        /*Static UI in this Fragment*/
        directChannelTabView = inflater.inflate(R.layout.direct_channel_tab, container, false);
        setUpChannelNumberSpinner();
        setUpSendConfigRequestButton();
        sendConfigRequuestStatusTextView = (TextView) directChannelTabView.findViewById(R.id.direct_channel_send_config_req_status_view_id);

        /*Return the View to activity page*/
        return directChannelTabView;
    }

    private void setUpChannelNumberSpinner() {
        Vector<String> directChannelNumbersArray = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getChannelList();
        ArrayAdapter<String> channelNumberSpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, directChannelNumbersArray);
        channelNumberSpinner = (Spinner) directChannelTabView.findViewById(R.id.direct_channel_channel_number_spinner);
        channelNumberSpinner.setAdapter(channelNumberSpinnerAdapter);
        channelNumberSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                setUpChannelFragment(position);
            }
            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });

    }

    private void clearExistingFragmentByTag(String tag) {
        FragmentManager fm = getActivity().getFragmentManager();
        if(fm.findFragmentByTag(tag) != null) {
            fm.beginTransaction().remove(fm.findFragmentByTag(tag)).commit();
        }
    }

    private void setUpChannelFragment(int position) {
        updateSendConfigButtonStatus("" , Color.BLUE);
        if(0 == position) {
            DirectChannelParamValues.getInstance().setNewChannel(true);
            clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_SENSORINFO);
            setUpNewChannelFragment();
        } else {
            DirectChannelParamValues.getInstance().setNewChannel(false);
            clearExistingFragmentByTag(DIRECT_CHANNEL_FRAGMENT_TAGS_NEW_CHANNEL);
            DirectChannelParamValues.getInstance().setChannelNumber(Long.parseLong(DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).getChannelList().elementAt(position)));
            setUpSensorInfoFragment();
        }
    }

    private void setUpNewChannelFragment() {
        FragmentManager fm = getActivity().getFragmentManager();
        DirectChannelNewChannelFragment newChannelFragment = DirectChannelNewChannelFragment.createInstance();
        if(fm.findFragmentById(R.id.direct_channel_new_channel_fragment) != null ) {
            fm.beginTransaction().replace(R.id.direct_channel_new_channel_fragment, newChannelFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_NEW_CHANNEL).commit();
        } else {
            fm.beginTransaction().add(R.id.direct_channel_new_channel_fragment, newChannelFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_NEW_CHANNEL).commit();
        }
    }

    private void setUpSensorInfoFragment() {
        FragmentManager fm = getActivity().getFragmentManager();
        DirectChannelSensorInfoFragment sensorInfoFragment = DirectChannelSensorInfoFragment.createInstance();
        if(fm.findFragmentById(R.id.direct_channel_sensorInfo_fragment) != null ) {
            fm.beginTransaction().replace(R.id.direct_channel_sensorInfo_fragment, sensorInfoFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_SENSORINFO).commit();
        } else {
            fm.beginTransaction().add(R.id.direct_channel_sensorInfo_fragment, sensorInfoFragment, DIRECT_CHANNEL_FRAGMENT_TAGS_SENSORINFO).commit();
        }
    }

    private void setUpSendConfigRequestButton(){

        directChannelSendConfigButton = (Button) directChannelTabView.findViewById(R.id.direct_channel_send_config_button);

        directChannelSendConfigButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(true == DirectChannelParamValues.getInstance().isNewChannel()) {
                    long channelNumber = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).createNewDirectChannel();
                    if(channelNumber < 0 ) {
                        String status = "Channel creation failed - Please check the logs for further investigation";
                        updateSendConfigButtonStatus(status, Color.RED);
                    } else {
                        String status = "Channel Created with channel Number ";
                        status = status + Long.toString(channelNumber);
                        updateSendConfigButtonStatus(status,Color.BLUE);
                    }

                } else {
                    if(true == DirectChannelParamValues.getInstance().isSetUpConfigReqFragment()) {
                        USTALog.i("Send Button Clicked on SetUpConfigReqFragment");
                        int ret = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).sendConfigRequest();
                    } else if (true == DirectChannelParamValues.getInstance().isRemoveReqFragment()) {
                        int ret = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).sendRemoveConfigRequest();
                        if(ret < 0 ) {
                            String status = "Remove Config Req FAILED ";
                            updateSendConfigButtonStatus(status,Color.RED);
                        } else {
                            String status = "Remove Config Req SUCCESS";
                            updateSendConfigButtonStatus(status,Color.BLUE);
                        }
                    } else if(true == DirectChannelParamValues.getInstance().isTimeStampOffsetFragment()){
                        int ret = DirectChannelContext.getDirectChannelContext(ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI).updateTimeStampOffsetRequest();
                        if(ret < 0 ) {
                            String status = "TimeStamp Offset Req FAILED ";
                            updateSendConfigButtonStatus(status,Color.RED);
                        } else {
                            String status = "TimeStamp Offset Req SUCCESS";
                            updateSendConfigButtonStatus(status,Color.BLUE);
                        }
                    }
                }
            }
        });

    }

    public static void updateSendConfigButtonStatus(CharSequence text, int color) {
        sendConfigRequuestStatusTextView.setText(text);
        sendConfigRequuestStatusTextView.setTextColor(color);
        sendConfigRequuestStatusTextView.setTypeface(null, Typeface.BOLD_ITALIC);
    }

}


