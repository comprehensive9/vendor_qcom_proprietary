/*============================================================================
  Copyright (c) 2017-2018 , 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorContextJNI.java
  @brief
   SensorContextJNI class which is used to communicates to the Native library
============================================================================*/
package com.qualcomm.qti.usta.core;

import android.os.Build;

public class SensorContextJNI {

  private static native SensorListInfo[] getSensorList(ModeType inModeType);

  private static native ReqMsgPayload[] getRequestMessages(ModeType inModeType, int inSensorHandle);

  private static native String getSensorAttributes(ModeType inModeType, int inSensorHandle);

  private static native NativeErrorCodes sendRequest(ModeType inModeType, int inSensorHandle, ReqMsgPayload inReqMsg, SendReqStdFields sendReqStdFieldObj, String logFileName);

  private static native String[] getClientProcessors(ModeType inModeType);

  private static native String[] getWakeupDeliveryType(ModeType inModeType);

  private static native void stopRequest(ModeType inModeType, int inSensorHandle, boolean isClientDisableRequestEnabled);

  private static native void destroySensors(ModeType inModeType);

  private static native void removeSensors(ModeType inModeType, int[] inRogueSensors);

  private static native void updateLoggingFlag(boolean isLoggingDisabled);

  private static native void enableStreamingStatus(int sensorHandle);

  private static native void disableStreamingStatus(int sensorHandle);

  private static native String getSamplesFromNative(boolean isRegistrySensor);

  private static native int sendDeviceModeIndication(int ModeType , int ModeState);

  public static int sendDeviceModeIndicationWrapper(int ModeType , int ModeState) {

    if (Build.MODEL.contains("Android SDK")) {
      USTALog.i("sendDeviceModeIndicationWrapper : Stub with ModeType " + ModeType + " State " + ModeState);
      return 0;
    } else {
      return sendDeviceModeIndication(ModeType, ModeState);
    }
  }

  public static SensorListInfo[] getSensorListNativeWrapper(ModeType inModeType) {

    if (Build.MODEL.contains("Android SDK")) {
      SensorListInfo sensorList = new SensorListInfo();
      sensorList.setDataType("resampler");
      sensorList.setVendor("qti");
      sensorList.setSuidLow("0x5643509e974b83c1");
      sensorList.setSuidHigh("0xd233de11b097e7b4");
      SensorListInfo ret[] = {sensorList};
      return ret;
    } else {
      return getSensorList(inModeType);
    }
  }

  public static void enableStreamingStatusNativeWrapper(int sensorHandle) {

    if (Build.MODEL.contains("Android SDK")) {
      USTALog.i("enableStreamingStatusNativeWrapper : Stub with sensor handle " + sensorHandle );
    } else {
      enableStreamingStatus(sensorHandle);
    }
  }

  public static void disableStreamingStatusNativeWrapper(int sensorHandle) {

    if (Build.MODEL.contains("Android SDK")) {
      USTALog.i("disableStreamingStatusNativeWrapper : Stub with sensor handle " + sensorHandle );
    } else {
      disableStreamingStatus(sensorHandle);
    }
  }

  public static String getSensorAttributesNativeWrapper(ModeType inModeType, int sensorHandle) {

    if (Build.MODEL.contains("Android SDK")) {

      return "Sensor attributes stubbed";
    } else {

      return getSensorAttributes(inModeType, sensorHandle);
    }
  }

  public static ReqMsgPayload[] getRequestMessagesNativeWrapper(ModeType inModeType, int sensorHandle) {

    if (Build.MODEL.contains("Android SDK")) {
      USTALog.i("getRequestMessagesNativeWrapper : ReqMsgPayload[] as null as we don't have device ");
      ReqMsgPayload[] regMsgArray = new ReqMsgPayload[1];

      for(int i = 0 ; i < 1; i++){
        ReqMsgPayload currentMsg = new ReqMsgPayload();
        currentMsg.setMsgName("sns_resampler_config");
        currentMsg.setMsgID("513");

        SensorPayloadField fields_0 = new SensorPayloadField();
        fields_0.setAccessModifier(AccessModifier.REQUIRED);
        fields_0.setDataType(DataType.USER_DEFINED);
        fields_0.setUserDefined(true);
        fields_0.setFieldName("sensor_uid");

        NestedMsgPayload nestedMsgs_0 = new NestedMsgPayload();
        nestedMsgs_0.setNestedMsgName("sensor_uid");

        SensorPayloadField nested_fields_0 = new SensorPayloadField();

        nested_fields_0.setAccessModifier(AccessModifier.REQUIRED);
        nested_fields_0.setDataType(DataType.UNSIGNED_INTEGER64);
        nested_fields_0.setUserDefined(false);
        nested_fields_0.setFieldName("suid_low");
        nested_fields_0.setEnumerated(false);
        nested_fields_0.setHasDefaultValue(false);

        SensorPayloadField nested_fields_1 = new SensorPayloadField();
        nested_fields_1.setAccessModifier(AccessModifier.REQUIRED);
        nested_fields_1.setDataType(DataType.UNSIGNED_INTEGER64);
        nested_fields_1.setUserDefined(false);
        nested_fields_1.setFieldName("suid_high");
        nested_fields_1.setEnumerated(false);
        nested_fields_1.setHasDefaultValue(false);

        SensorPayloadField[] nested_fields = {nested_fields_0 , nested_fields_1};
        nestedMsgs_0.setFields(nested_fields);
        NestedMsgPayload[] nestedMsgs = {nestedMsgs_0};

        fields_0.setNestedMsgs(nestedMsgs);

        SensorPayloadField fields_1 = new SensorPayloadField();
        fields_1.setAccessModifier(AccessModifier.REQUIRED);
        fields_1.setDataType(DataType.FLOAT);
        fields_1.setUserDefined(false);
        fields_1.setFieldName("resampled_rate");
        fields_1.setEnumerated(false);
        fields_1.setHasDefaultValue(false);


        SensorPayloadField fields_2 = new SensorPayloadField();
        fields_2.setAccessModifier(AccessModifier.REQUIRED);
        fields_2.setDataType(DataType.ENUM);
        fields_2.setUserDefined(false);
        fields_2.setFieldName("rate_type");
        fields_2.setEnumerated(true);
        String[] enumValues = {"SNS_RESAMPLER_RATE_FIXED", "SNS_RESAMPLER_RATE_MINIMUM"};
        fields_2.setEnumValues(enumValues);
        fields_2.setHasDefaultValue(false);

        SensorPayloadField fields_3 = new SensorPayloadField();
        fields_3.setAccessModifier(AccessModifier.REQUIRED);
        fields_3.setDataType(DataType.BOOLEAN);
        fields_3.setUserDefined(false);
        fields_3.setFieldName("filter");
        fields_3.setEnumerated(false);
        fields_3.setHasDefaultValue(false);

        SensorPayloadField[] fields = {fields_0, fields_1,fields_2,fields_3};
        currentMsg.setFields(fields);
        regMsgArray[0] = currentMsg;
      }

      return regMsgArray;
      }
    else {
      return getRequestMessages(inModeType, sensorHandle);
    }

  }

  public static String[] getClientProcessorsNativeWrapper(ModeType inModeType) {

    if (Build.MODEL.contains("Android SDK")) {

      return new String[]{"SNS_STD_PROCESSOR_APSS", "SNS_STD_PROCESSOR_SSC", "SNS_STD_PROCESSOR_ADSP" , "SNS_STD_PROCESSOR_MDSP"};
    } else {

      return getClientProcessors(inModeType);
    }
  }

  public static String[] getWakeupDeliveryNativeWrapper(ModeType inModeType) {

    if (Build.MODEL.contains("Android SDK")) {

      return new String[]{"SNS_STD_DELIVERY_WAKEUP", "SNS_STD_DELIVERY_NO_WAKEUP", "SNS_STD_DELIVERY_FLUSH"};
    } else {

      return getWakeupDeliveryType(inModeType);
    }
  }

  public static NativeErrorCodes sendRequestNativeWrapper(ModeType inModeType, int inSensorHandle, ReqMsgPayload inReqMsg, SendReqStdFields sendReqStdFieldObj, String logFileName) {

    if (!Build.MODEL.contains("Android SDK")) {

      return(sendRequest(inModeType, inSensorHandle, inReqMsg, sendReqStdFieldObj, logFileName));
    }
    else {
      USTALog.i(" sendRequest has arraived for sensor " + inSensorHandle + "with flush only as " + sendReqStdFieldObj.flushOnly + " with max_batch as " + sendReqStdFieldObj.maxBatch);
      return NativeErrorCodes.USTA_NO_ERROR;
    }

  }


  public static void stopRequestNativeWrapper(ModeType inModeType , int inSensorHandle , boolean isClientDisableRequestEnabled) {

    if (Build.MODEL.contains("Android SDK")) {
      USTALog.i(" Stop Request has arraived for sensor " + inSensorHandle + "with flag as " + isClientDisableRequestEnabled);
    } else {
      stopRequest(inModeType, inSensorHandle , isClientDisableRequestEnabled);
    }
  }

  public static void removeSensorsNativeWrapper(ModeType inModeType , int[] inRogueSensors) {

    if (!Build.MODEL.contains("Android SDK")) {

      removeSensors(inModeType, inRogueSensors);
    } else {
      USTALog.i(" Removed RogueSensors from Native - TestStub ");
    }
  }

  public static void updateLoggingFlagWrapper(){
    if (!Build.MODEL.contains("Android SDK")) {
        updateLoggingFlag(USTALog.isLoggingDisabled);
    } else {
      USTALog.i(" updateLoggingFlag to Native - TestStub ");
    }
  }

  public static String getSamplesFromNativeWrapper(boolean isRegistrySensor){
    if (!Build.MODEL.contains("Android SDK")) {
      return getSamplesFromNative(isRegistrySensor);
    } else {
      return "getSamplesFromNativeWrapper - TestStub ";
    }
  }

  //========================================//
  private static int USTA_STUB_RETURN_VALUE = -1 ; // 0 -- Sucess , -1 Faiure  // Used only for Stub

  private static native boolean isDirectChannelSupported();
  private static native long createDirectChannel(ModeType inModeType, int channelType, int clientType);
  private static native int deleteDirectChannel(ModeType inModeType, long channelNumber);
  private static native int timeStampOffsetUpdate(ModeType inModeType, long channelNumber);
  private static native int sendRemoveConfigRequest(ModeType inModeType, long channelNumber, int sensorHandle, boolean isCalibrated , boolean isResampled);
  private static native int sendConfigRequest(ModeType inModeType, long channelNumber, DirectChannelStdFields stdFields, ReqMsgPayload reqMsg);

  public static boolean isDirectChannelSupportedWrapper() {
    if(!Build.MODEL.contains("Android SDK")) {
      return isDirectChannelSupported();
    } else
      return false;
  }
  public static int sendConfigRequestWrapper(ModeType inModeType, long channelNumber, DirectChannelStdFields stdFields, ReqMsgPayload reqMsg) {
    if(!Build.MODEL.contains("Android SDK")) {
      return sendConfigRequest(inModeType, channelNumber, stdFields, reqMsg);
    } else {
      USTALog.i("sendConfigRequestWrapper Stub with sampling Rate ");
      return USTA_STUB_RETURN_VALUE;
    }
  }

  public static int deleteDirectChannelWrapper(ModeType inModeType, long channelNumber) {
    if(!Build.MODEL.contains("Android SDK")) {
      return deleteDirectChannel(inModeType , channelNumber);
    } else {
      USTALog.i("deleteDirectChannelWrapper Stub");
      return USTA_STUB_RETURN_VALUE;
    }
  }

  public static int sendRemoveConfigRequestWrapper(ModeType inModeType, long channelNumber, int sensorHandle, boolean isCalibrated , boolean isResampled) {
    if(!Build.MODEL.contains("Android SDK")) {
      return sendRemoveConfigRequest(inModeType , channelNumber, sensorHandle, isCalibrated, isResampled);
    } else {
      USTALog.i("sendRemoveConfigRequestWrapper Stub - with channel Number " + channelNumber + " sensorHandle " + sensorHandle + " isCalibrated " + isCalibrated + " isResampled " + isResampled);
      return USTA_STUB_RETURN_VALUE;
    }
  }

  public static int TimeStampOffsetUpdateWrapper(ModeType inModeType,long channelNumber) {
    if(!Build.MODEL.contains("Android SDK")) {
      return timeStampOffsetUpdate(inModeType, channelNumber);
    } else {
      USTALog.i("TimeStampOffsetUpdateWrapper Stub - for channel " + channelNumber);
      return USTA_STUB_RETURN_VALUE;
    }
  }

  public static long createDirectChannelWrapper(ModeType inModeType, int channelType, int clientType) {
    if(!Build.MODEL.contains("Android SDK")) {
      return createDirectChannel(inModeType , channelType, clientType);
    } else {
      USTALog.i("createDirectChannelWrapper - stub :: channelType " + channelType + " clientType " + clientType);
      if(USTA_STUB_RETURN_VALUE < 0)
        return -1;
      else
        return 789065;
    }
  }

  public static DirectChannelinfo[] getDirectChannelNumbersWrapper(ModeType inModeType) {
    if (!Build.MODEL.contains("Android SDK")) {
//            return getDirectChannelInfo(inModeType);
      DirectChannelinfo[] chList = {};
      return chList;
    }
    else{
      USTALog.i("getDirectChannelNumbersWrapper - Stub");
      DirectChannelinfo ch1 = new DirectChannelinfo();
      ch1.setChannelNumber(123);
      ch1.setChannelType(0);
      ch1.setClientProcType(2);

      DirectChannelinfo ch2 = new DirectChannelinfo();
      ch2.setChannelNumber(456);
      ch2.setChannelType(1);
      ch2.setClientProcType(4);

      DirectChannelinfo[] chList = /*{}; */ {ch1, ch2};
      return chList;
    }
  }
  //========================================//


}
