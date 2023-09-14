/*============================================================================
  Copyright (c)  2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.usta.core;

import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

public class DirectChannelContext  {

    private static DirectChannelContext[] directChannelContextArray = new DirectChannelContext[ModeType.USTA_MODE_TYPE_COUNT.ordinal()];
    private Vector<DirectChannelinfo> directChannelsInfo; //TODO :: Make use of channel number from this variable instead of getting it from Top .. Get only position Number.. From position, get Channel number

    private Vector<DirectChannelStdFields> directChannelStdFieldsVector; // Per sensor, One field..

    Vector<String> channelList = new Vector<>();
    ModeType currentModeType;

    private SensorContext sensorContext;

    public static DirectChannelContext getDirectChannelContext(ModeType inModeType) {
        switch (inModeType) {
            case USTA_MODE_TYPE_DIRECT_CHANNEL_UI:
                if(null == directChannelContextArray[ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI.ordinal()]) {
                    DirectChannelContext selfContext = new DirectChannelContext(inModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI);
                    directChannelContextArray[ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI.ordinal()] = selfContext;
                }
                return directChannelContextArray[ModeType.USTA_MODE_TYPE_DIRECT_CHANNEL_UI.ordinal()];

            default:
                USTALog.e("Direct Channel Mode Type is Wrong. Please check once");
                return null;
        }
    }

    private DirectChannelContext(ModeType inModeType) {
        currentModeType = inModeType;
        sensorContext = SensorContext.getSensorContext(currentModeType);
        directChannelsInfo = new Vector<DirectChannelinfo>(Arrays.asList(SensorContextJNI.getDirectChannelNumbersWrapper(currentModeType)));

        Vector<Sensor> sensors = sensorContext.getSensors();
        int sensorCount = sensors.size();
        USTALog.i("Sensors Size / sensorCount " + sensorCount);
        directChannelStdFieldsVector = new Vector<DirectChannelStdFields>();
        for(int loopindex =0; loopindex < sensorCount ; loopindex++) {
            DirectChannelStdFields currentInstance = new DirectChannelStdFields();
            directChannelStdFieldsVector.add(currentInstance);
        }
        createChannelList();
    }

    private void createChannelList() {
        channelList.add("Create New one");
        for(int loopindex = 0; loopindex < directChannelsInfo.size() ; loopindex++) {
            channelList.add(Integer.toString(directChannelsInfo.elementAt(loopindex).getChannelNumber()));
        }
    }

    public Vector<String> getChannelList() {
        return channelList;
    }

    private void addChannelToList(long channelNumber) {
        channelList.add(Long.toString(channelNumber));
    }

    public long createNewDirectChannel() {

        long channelNumber = SensorContextJNI.createDirectChannelWrapper(currentModeType, DirectChannelParamValues.getInstance().getChannelType(), DirectChannelParamValues.getInstance().getClientProc());
        USTALog.i("channelNumber is " + channelNumber);
        if(channelNumber < 0) {
            return -1;
        }
        addChannelToList(channelNumber);
        return channelNumber;
    }

    public int updateTimeStampOffsetRequest() {
        return SensorContextJNI.TimeStampOffsetUpdateWrapper(currentModeType, DirectChannelParamValues.getInstance().getChannelNumber());
    }

    public int sendRemoveConfigRequest() {
        return SensorContextJNI.sendRemoveConfigRequestWrapper(currentModeType, DirectChannelParamValues.getInstance().getChannelNumber(), DirectChannelParamValues.getInstance().getSensorHandle(),DirectChannelParamValues.getInstance().isCalibrated(), DirectChannelParamValues.getInstance().isResampled());
    }

    public int deleteDirectChannel(long channelNumber) {
        int ret = SensorContextJNI.deleteDirectChannelWrapper(currentModeType, channelNumber);
        if (ret < 0) {
            USTALog.i("deleteDirectChannel Failed ");
            return ret;
        }

        /* Delete entry from List / Array / Vector at this position */
        int positionToBeDeleted = -1;
        for(int ChannelListLoopIndex = 1; ChannelListLoopIndex < channelList.size(); ChannelListLoopIndex++) {
            if(channelList.elementAt(ChannelListLoopIndex).contentEquals(Long.toString(channelNumber)) == true) {
                positionToBeDeleted = ChannelListLoopIndex;
                break;
            }
        }
        if(positionToBeDeleted == -1 ) {
            USTALog.e("Proper position not detected. failure ");
            return -1;
        }
        channelList.remove(positionToBeDeleted);
        return 0;
    }

    public int sendConfigRequest() {

        DirectChannelStdFields srcFields = directChannelStdFieldsVector.elementAt(DirectChannelParamValues.getInstance().getSensorHandle());

        /*Standard Fields*/
        DirectChannelStdFields dstsStdFields = new DirectChannelStdFields();
        dstsStdFields.setSensorHandle(DirectChannelParamValues.getInstance().getSensorHandle());
        dstsStdFields.setCalibratead(srcFields.isCalibratead());
        dstsStdFields.setResampled(srcFields.isResampled());
        dstsStdFields.setBatchPeriod(srcFields.getBatchPeriod());
        dstsStdFields.setFlushPeriod(srcFields.getFlushPeriod());
        dstsStdFields.setFlushOnly(srcFields.isFlushOnly());
        dstsStdFields.setMaxBatch(srcFields.isMaxBatch());
        dstsStdFields.setPassive(srcFields.isPassive());
        dstsStdFields.setAttributes(srcFields.isAttributes());

        /*Dynamic Fields*/
        Sensor sensor = sensorContext.getSensors().get(DirectChannelParamValues.getInstance().getSensorHandle());
        ReqMsgPayload reqMsg = new ReqMsgPayload();
        reqMsg.setMsgName(sensor.getSensorReqMsgsNames().get(DirectChannelParamValues.getInstance().getReqMsgHandlePosition()));
        reqMsg.setMsgID(sensor.getSensorReqMsgIDs().get(DirectChannelParamValues.getInstance().getReqMsgHandlePosition()));

        Vector<SensorPayloadField> payloadFields = sensor.getSensorPayloadField(DirectChannelParamValues.getInstance().getReqMsgHandlePosition());
        for(int fieldIndex = 0 ; fieldIndex < payloadFields.size() ; fieldIndex++){
            if(payloadFields.get(fieldIndex).isUserDefined() == true) {
                /*TODO: UserDefined Messages support for Direct Channel - later we can see*/
//                updateFiledValuesFromMappingtable(payloadFields.get(fieldIndex).getNestedMsgs());
            }
            else{
                Map widgetIDValueTable = payloadFields.get(fieldIndex).getWidgetIDValueTable();
                Iterator it = widgetIDValueTable.entrySet().iterator();
                while(it.hasNext()){
                    Map.Entry currentMapEntry = (Map.Entry)it.next();
                    if(currentMapEntry.getValue() != null) {
                        String value = currentMapEntry.getValue().toString();
                        payloadFields.get(fieldIndex).addValue(value);
                    }
                }
                payloadFields.get(fieldIndex).setValuesForNative(payloadFields.get(fieldIndex).getValue().toArray(new String[payloadFields.get(fieldIndex).getValue().size()]));
            }
        }

        SensorPayloadField[] payLoadFiledArray = payloadFields.toArray(new SensorPayloadField[payloadFields.size()]);
        reqMsg.setFields(payLoadFiledArray);

        /*reset value content for next send request - corner case where repeated filed occurs */
        for(int fieldIndex = 0 ; fieldIndex < payloadFields.size() ; fieldIndex++){
            if (payloadFields.get(fieldIndex).getAccessModifier() == AccessModifier.REPEATED){
                payloadFields.get(fieldIndex).clearValue();
            }
        }

        return SensorContextJNI.sendConfigRequestWrapper(currentModeType, DirectChannelParamValues.getInstance().getChannelNumber(),dstsStdFields,reqMsg);

    }

    public DirectChannelStdFields getDirectChannelFieldInstance(int position) {
        return directChannelStdFieldsVector.elementAt(position);
    }

    public Vector<String> getReqMsgList() {
        int sensorHandle = DirectChannelParamValues.getInstance().getSensorHandle();
        Vector<String> reqMsgsNames = sensorContext.getSensors().get(sensorHandle).getSensorReqMsgsNames();
        Vector<String> reqMsgIDs = sensorContext.getSensors().get(sensorHandle).getSensorReqMsgIDs();
        Vector<String> reqMsgsNamesWithMsgIDs = new Vector<>();
        for (int reqMsgIndex = 0; reqMsgIndex < reqMsgsNames.size(); reqMsgIndex++) {
            reqMsgsNamesWithMsgIDs.add(reqMsgsNames.get(reqMsgIndex) + " - "+ reqMsgIDs.get(reqMsgIndex));
        }
        return reqMsgsNamesWithMsgIDs;
    }

    public SensorContext getSensorContext() {
        return sensorContext;
    }

}
