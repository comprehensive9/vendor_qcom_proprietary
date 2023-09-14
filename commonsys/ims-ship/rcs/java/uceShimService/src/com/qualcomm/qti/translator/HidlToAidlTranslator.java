/*********************************************************************
 Copyright (c) 2017,2019,2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.translator;


import java.util.ArrayList;
import java.util.Iterator;

import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.common.CapInfo;

import com.android.ims.internal.uce.presence.*;
import com.android.ims.internal.uce.options.OptionsCmdId;
import com.android.ims.internal.uce.options.OptionsSipResponse;
import com.android.ims.internal.uce.options.OptionsCapInfo;
import com.android.ims.internal.uce.options.OptionsCmdStatus;


import com.qualcomm.qti.uceservice.V2_0.PresenceCmdId;
import com.qualcomm.qti.uceservice.V2_0.ResInstantceState;
import com.qualcomm.qti.uceservice.V2_0.UceStatusCode;

import android.util.Log;
import static java.lang.Math.toIntExact;

public class HidlToAidlTranslator
{
    private final static String logTAG = "HIDL to AIDL Translator";

/* Translator method common to PresListener and OptionsListener class  */

//PresListener : serviceAvailable & serviceUnavailable use below method
//Ex : m_statusCode.setStatusCode(hidlToAidlObj.getAidlStatusCode(status));
public int getAidlStatusCode(int statusCode) {
    int retAidlStatusCode = getStatusValue(statusCode);
    return retAidlStatusCode;
}

/* Translator methods pertaining to PresListener class */

//Ex: m_publishTrigger.setPublishTrigeerType(hidlToAidlObj.getAidlPresenceTriggerTypeValue(publishTrigger));
public int getAidlPresenceTriggerTypeValue(int publishTrigger) {
    int retAidlPresTriggerType = getPresenceTriggerTypeValue(publishTrigger);
    return retAidlPresTriggerType;
}

//Ex: m_pCmdStatus = hidlToAidlObj.getAidlPresCmdStatus(pCmdStatus);
public PresCmdStatus getAidlPresCmdStatus(com.qualcomm.qti.uceservice.V2_0.PresCmdStatus
                                          pCmdStatus) {
    PresCmdStatus aidlPresCmdStatus = new PresCmdStatus();
    PresCmdId aidlPresCmdId = new PresCmdId();
    StatusCode aidlStatusCode = new StatusCode();
    // Set Cmd Id
    aidlPresCmdId.setCmdId(getPresCmdIdValue(pCmdStatus.cmdId));
    aidlPresCmdStatus.setCmdId(aidlPresCmdId);
    //Set User data
    aidlPresCmdStatus.setUserData((int)pCmdStatus.userData);
    //Set Status Code
    aidlStatusCode.setStatusCode(getStatusValue(pCmdStatus.status));
    aidlPresCmdStatus.setStatus(aidlStatusCode);
    //Set Request Id
    aidlPresCmdStatus.setRequestId(pCmdStatus.requestId);

    return aidlPresCmdStatus;
}

//Ex: m_pSipResponse = hidlToAidlObj.getAidlPresSipResponse(pSipResponse);
public PresSipResponse getAidlPresSipResponse(com.qualcomm.qti.uceservice.V2_0.PresSipResponse
                                              pSipResponse) {
    PresSipResponse aidlPresSipResponse = new PresSipResponse();
    PresCmdId aidlPresCmdId = new PresCmdId();
    aidlPresCmdId.setCmdId(getPresCmdIdValue(pSipResponse.cmdId));
    aidlPresSipResponse.setCmdId(aidlPresCmdId);
    aidlPresSipResponse.setRequestId(pSipResponse.requestId);
    aidlPresSipResponse.setSipResponseCode(pSipResponse.sipResponseCode);
    aidlPresSipResponse.setRetryAfter(pSipResponse.retryAfter);
    aidlPresSipResponse.setReasonPhrase(pSipResponse.reasonPhrase);

    return aidlPresSipResponse;
}

public PresSipResponse getAidlPresSipResponse_2_3(com.qualcomm.qti.uceservice.V2_3.PresSipResponse
                                              pSipResponse) {
    PresSipResponse aidlPresSipResponse = new PresSipResponse();
    PresCmdId aidlPresCmdId = new PresCmdId();
    aidlPresCmdId.setCmdId(getPresCmdIdValue(pSipResponse.cmdId));
    aidlPresSipResponse.setCmdId(aidlPresCmdId);
    aidlPresSipResponse.setRequestId(pSipResponse.requestId);
    aidlPresSipResponse.setSipResponseCode(pSipResponse.sipResponseCode);
    aidlPresSipResponse.setRetryAfter(pSipResponse.retryAfter);
    aidlPresSipResponse.setReasonPhrase(pSipResponse.reasonPhrase);
    aidlPresSipResponse.setReasonHeader(pSipResponse.reasonHeader);

    return aidlPresSipResponse;
}

//Ex: m_pTupleInfo = hidlToAidlObj.getAidlTupleInfoArray(pTupleInfoArray);
//mAidlPresenceListener.capInfoReceived(m_pPresentityURI, m_pTupleInfo);
public PresTupleInfo[] getAidlTupleInfoArray(ArrayList<com.qualcomm.qti.uceservice.V2_0.PresTupleInfo>
                                           pTupleInfoArray) {
    Iterator i = pTupleInfoArray.iterator();
    int sizeofTupleArray = pTupleInfoArray.size();
    PresTupleInfo aidlTupleInfo [] = new PresTupleInfo[sizeofTupleArray];

    int j = 0;
    while(i.hasNext()) {
        com.qualcomm.qti.uceservice.V2_0.PresTupleInfo pHidlTupleInfo =
            (com.qualcomm.qti.uceservice.V2_0.PresTupleInfo)i.next();
        com.android.ims.internal.uce.presence.PresTupleInfo tupleInfoObj = new
            com.android.ims.internal.uce.presence.PresTupleInfo();
        tupleInfoObj.setFeatureTag(pHidlTupleInfo.featureTag);
        tupleInfoObj.setContactUri(pHidlTupleInfo.contactUri);
        tupleInfoObj.setTimestamp(pHidlTupleInfo.timestamp);
        aidlTupleInfo[j] = tupleInfoObj;
        j++;
    }

    return aidlTupleInfo;
}

public PresTupleInfo[] getAidlTupleInfoArray_2_3(
      ArrayList<com.qualcomm.qti.uceservice.V2_3.PresTupleInfo>
                                           pTupleInfoArray) {
    Iterator i = pTupleInfoArray.iterator();
    int sizeofTupleArray = pTupleInfoArray.size();
    PresTupleInfo aidlTupleInfo [] = new PresTupleInfo[sizeofTupleArray];

    int j = 0;
    while(i.hasNext()) {
        com.qualcomm.qti.uceservice.V2_3.PresTupleInfo pHidlTupleInfo =
            (com.qualcomm.qti.uceservice.V2_3.PresTupleInfo)i.next();
        com.android.ims.internal.uce.presence.PresTupleInfo tupleInfoObj = new
            com.android.ims.internal.uce.presence.PresTupleInfo();
        tupleInfoObj.setFeatureTag(pHidlTupleInfo.featureTag);
        tupleInfoObj.setContactUri(pHidlTupleInfo.contactUri);
        tupleInfoObj.setTimestamp(pHidlTupleInfo.timestamp);
        tupleInfoObj.setVersion(pHidlTupleInfo.ftVersion);
        aidlTupleInfo[j] = tupleInfoObj;
        j++;
    }

    return aidlTupleInfo;
}

//Ex: m_pRlmiInfo = hidlToAidlObj.getAidlPresRlmiInfo(pRlmiInfo);
public PresRlmiInfo getAidlPresRlmiInfo(com.qualcomm.qti.uceservice.V2_0.PresRlmiInfo
                                        pRlmiInfo) {
    PresRlmiInfo aidlPresRlmiInfo = new PresRlmiInfo();
    PresSubscriptionState aidlPresSubscriptionState = new PresSubscriptionState();

    aidlPresRlmiInfo.setUri(pRlmiInfo.uri);
    aidlPresRlmiInfo.setVersion(pRlmiInfo.version);
    aidlPresRlmiInfo.setFullState(pRlmiInfo.fullState);
    aidlPresRlmiInfo.setListName(pRlmiInfo.listname);
    aidlPresRlmiInfo.setRequestId(pRlmiInfo.requestId);
    aidlPresSubscriptionState.setPresSubscriptionState(
        getPresSubscriptionState(pRlmiInfo.presSubscriptionState));
    aidlPresRlmiInfo.setPresSubscriptionState(aidlPresSubscriptionState);
    aidlPresRlmiInfo.setSubscriptionExpireTime(pRlmiInfo.subscriptionExpireTime);
    aidlPresRlmiInfo.setSubscriptionTerminatedReason(pRlmiInfo.subscriptionTerminatedReason);

    return aidlPresRlmiInfo;
}

//Ex: m_pResInfo = hidlToAidlObj.getAidlPresResInfo(pResInfoArray);
public PresResInfo[] getAidlPresResInfo(ArrayList<com.qualcomm.qti.uceservice.V2_0.PresResInfo>
                                      pResInfoArray) {
    Iterator i = pResInfoArray.iterator();
    int sizeOfResInfoArray = pResInfoArray.size();
    PresResInfo aidlPresResInfo[] = new PresResInfo[sizeOfResInfoArray];
    int j = 0;

    while(i.hasNext()) {
        com.qualcomm.qti.uceservice.V2_0.PresResInfo hidlPresResInfo =
            (com.qualcomm.qti.uceservice.V2_0.PresResInfo)i.next();
        PresResInfo resInfoObj = new PresResInfo();
        resInfoObj.setResUri(hidlPresResInfo.resUri);
        resInfoObj.setDisplayName(hidlPresResInfo.displayName);
        PresResInstanceInfo aidlPresResInstanceInfo = new PresResInstanceInfo();
        aidlPresResInstanceInfo.setResInstanceState(getResInstanceStateValue(hidlPresResInfo.instanceInfo.resInstanceState));
        aidlPresResInstanceInfo.setResId(hidlPresResInfo.instanceInfo.id);
        aidlPresResInstanceInfo.setReason(hidlPresResInfo.instanceInfo.reason);
        aidlPresResInstanceInfo.setPresentityUri(hidlPresResInfo.instanceInfo.presentityUri);

        int sizeOfTupleInfoArray = hidlPresResInfo.instanceInfo.mTupleInfoArray.size();
        PresTupleInfo aidlPresTupleInfo[] = new PresTupleInfo[sizeOfTupleInfoArray];
        aidlPresTupleInfo = getAidlTupleInfoArray(hidlPresResInfo.instanceInfo.mTupleInfoArray);
        aidlPresResInstanceInfo.setTupleInfo(aidlPresTupleInfo);

        resInfoObj.setInstanceInfo(aidlPresResInstanceInfo);
        aidlPresResInfo[j] = resInfoObj;
        j++;
    }

    return aidlPresResInfo;
}

public PresResInfo[] getAidlPresResInfo_2_3(ArrayList<com.qualcomm.qti.uceservice.V2_3.PresResInfo>
                                      pResInfoArray) {
    Iterator i = pResInfoArray.iterator();
    int sizeOfResInfoArray = pResInfoArray.size();
    PresResInfo aidlPresResInfo[] = new PresResInfo[sizeOfResInfoArray];
    int j = 0;

    while(i.hasNext()) {
        com.qualcomm.qti.uceservice.V2_3.PresResInfo hidlPresResInfo =
            (com.qualcomm.qti.uceservice.V2_3.PresResInfo)i.next();
        PresResInfo resInfoObj = new PresResInfo();
        resInfoObj.setResUri(hidlPresResInfo.resUri);
        resInfoObj.setDisplayName(hidlPresResInfo.displayName);
        PresResInstanceInfo aidlPresResInstanceInfo = new PresResInstanceInfo();
        aidlPresResInstanceInfo.setResInstanceState(
            getResInstanceStateValue(hidlPresResInfo.instanceInfo.resInstanceState));
        aidlPresResInstanceInfo.setResId(hidlPresResInfo.instanceInfo.id);
        aidlPresResInstanceInfo.setReason(hidlPresResInfo.instanceInfo.reason);
        aidlPresResInstanceInfo.setPresentityUri(hidlPresResInfo.instanceInfo.presentityUri);

        int sizeOfTupleInfoArray = hidlPresResInfo.instanceInfo.mTupleInfoArray.size();
        PresTupleInfo aidlPresTupleInfo[] = new PresTupleInfo[sizeOfTupleInfoArray];
        aidlPresTupleInfo = getAidlTupleInfoArray_2_3(hidlPresResInfo.instanceInfo.mTupleInfoArray);
        aidlPresResInstanceInfo.setTupleInfo(aidlPresTupleInfo);

        resInfoObj.setInstanceInfo(aidlPresResInstanceInfo);
        aidlPresResInfo[j] = resInfoObj;
        j++;
    }

    return aidlPresResInfo;
}

/* Translator methods pertaining to OptionsListener class */

public OptionsSipResponse getAidlOptionsSipResponse(com.qualcomm.qti.uceservice.V2_0.OptionsSipResponse
                                                    hidlOptionsSipResponse) {
    OptionsSipResponse aidlOptionsSipResponse = new OptionsSipResponse();

    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsSipResponse.cmdId));

    aidlOptionsSipResponse.setCmdId(aidlOptionsCmdId);
    aidlOptionsSipResponse.setRequestId(hidlOptionsSipResponse.requestId);
    aidlOptionsSipResponse.setSipResponseCode(toIntExact(hidlOptionsSipResponse.sipResponseCode));
    aidlOptionsSipResponse.setReasonPhrase(hidlOptionsSipResponse.reasonPhrase);
    aidlOptionsSipResponse.setRetryAfter(toIntExact(hidlOptionsSipResponse.retryAfter));

    return aidlOptionsSipResponse;
}

public OptionsSipResponse getAidlOptionsSipResponse_2_3(com.qualcomm.qti.uceservice.V2_3.OptionsSipResponse
                                                    hidlOptionsSipResponse) {
    OptionsSipResponse aidlOptionsSipResponse = new OptionsSipResponse();

    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsSipResponse.cmdId));

    aidlOptionsSipResponse.setCmdId(aidlOptionsCmdId);
    aidlOptionsSipResponse.setRequestId(hidlOptionsSipResponse.requestId);
    aidlOptionsSipResponse.setSipResponseCode(toIntExact(hidlOptionsSipResponse.sipResponseCode));
    aidlOptionsSipResponse.setReasonPhrase(hidlOptionsSipResponse.reasonPhrase);
    aidlOptionsSipResponse.setRetryAfter(toIntExact(hidlOptionsSipResponse.retryAfter));
    aidlOptionsSipResponse.setReasonHeader(hidlOptionsSipResponse.reasonHeader);

    return aidlOptionsSipResponse;
}

public OptionsCapInfo getAidlOptionsCapInfo(com.qualcomm.qti.uceservice.V2_0.OptionsCapabilityInfo
                                            hidlCapInfo) {
    OptionsCapInfo aidlOptionsCapInfo = new OptionsCapInfo();
    aidlOptionsCapInfo.setSdp(hidlCapInfo.sdp);

    CapInfo aidlCapInfo = new CapInfo();
    aidlCapInfo = getAidlCapInfo(hidlCapInfo.capInfo);

    aidlOptionsCapInfo.setCapInfo(aidlCapInfo);

    return aidlOptionsCapInfo;
}

public OptionsCapInfo getAidlOptionsCapInfo_2_1(com.qualcomm.qti.uceservice.V2_1.OptionsCapabilityInfo
                                            hidlCapInfo) {
    OptionsCapInfo aidlOptionsCapInfo = new OptionsCapInfo();
    aidlOptionsCapInfo.setSdp(hidlCapInfo.sdp);

    CapInfo aidlCapInfo = new CapInfo();
    aidlCapInfo = getAidlCapInfo_2_1(hidlCapInfo.capInfo);

    aidlOptionsCapInfo.setCapInfo(aidlCapInfo);

    return aidlOptionsCapInfo;
}

public OptionsCapInfo getAidlOptionsCapInfo_2_2(com.qualcomm.qti.uceservice.V2_2.OptionsCapabilityInfo
                                            hidlCapInfo) {
    OptionsCapInfo aidlOptionsCapInfo = new OptionsCapInfo();
    aidlOptionsCapInfo.setSdp(hidlCapInfo.sdp);

    CapInfo aidlCapInfo = new CapInfo();
    aidlCapInfo = getAidlCapInfo_2_2(hidlCapInfo.capInfo);

    aidlOptionsCapInfo.setCapInfo(aidlCapInfo);

    return aidlOptionsCapInfo;
}

public OptionsCapInfo getAidlOptionsCapInfo_2_3(
    com.qualcomm.qti.uceservice.V2_3.OptionsCapabilityInfo hidlCapInfo) {
    OptionsCapInfo aidlOptionsCapInfo = new OptionsCapInfo();
    aidlOptionsCapInfo.setSdp(hidlCapInfo.sdp);

    CapInfo aidlCapInfo = new CapInfo();
    aidlCapInfo = getAidlCapInfo_2_3(hidlCapInfo.capInfo);

    aidlOptionsCapInfo.setCapInfo(aidlCapInfo);

    return aidlOptionsCapInfo;
}

public OptionsCmdStatus getAidlOptionsCmdStatus(com.qualcomm.qti.uceservice.V2_0.OptionsCmdStatus hidlOptionsCmdStatus) {
    OptionsCmdStatus aidlOptionsCMDStatus = new OptionsCmdStatus();
    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    CapInfo aidlCapInfo = new CapInfo();
    StatusCode aidlStatusCode = new StatusCode();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsCmdStatus.cmdId));
    aidlOptionsCMDStatus.setCmdId(aidlOptionsCmdId);
    aidlOptionsCMDStatus.setUserData(toIntExact(hidlOptionsCmdStatus.userData));
    aidlStatusCode.setStatusCode(getStatusValue(hidlOptionsCmdStatus.status));
    aidlOptionsCMDStatus.setStatus(aidlStatusCode);
    aidlCapInfo = getAidlCapInfo(hidlOptionsCmdStatus.capInfo.capInfo);
    aidlOptionsCMDStatus.setCapInfo(aidlCapInfo);

    return aidlOptionsCMDStatus;
}

public OptionsCmdStatus getAidlOptionsCmdStatus_2_1(com.qualcomm.qti.uceservice.V2_1.OptionsCmdStatus hidlOptionsCmdStatus) {
    OptionsCmdStatus aidlOptionsCMDStatus = new OptionsCmdStatus();
    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    CapInfo aidlCapInfo = new CapInfo();
    StatusCode aidlStatusCode = new StatusCode();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsCmdStatus.cmdId));
    aidlOptionsCMDStatus.setCmdId(aidlOptionsCmdId);
    aidlOptionsCMDStatus.setUserData(toIntExact(hidlOptionsCmdStatus.userData));
    aidlStatusCode.setStatusCode(getStatusValue(hidlOptionsCmdStatus.status));
    aidlOptionsCMDStatus.setStatus(aidlStatusCode);
    aidlCapInfo = getAidlCapInfo_2_1(hidlOptionsCmdStatus.capInfo.capInfo);
    aidlOptionsCMDStatus.setCapInfo(aidlCapInfo);

    return aidlOptionsCMDStatus;
}

public OptionsCmdStatus getAidlOptionsCmdStatus_2_2(com.qualcomm.qti.uceservice.V2_2.OptionsCmdStatus hidlOptionsCmdStatus) {
    OptionsCmdStatus aidlOptionsCMDStatus = new OptionsCmdStatus();
    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    CapInfo aidlCapInfo = new CapInfo();
    StatusCode aidlStatusCode = new StatusCode();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsCmdStatus.cmdId));
    aidlOptionsCMDStatus.setCmdId(aidlOptionsCmdId);
    aidlOptionsCMDStatus.setUserData(toIntExact(hidlOptionsCmdStatus.userData));
    aidlStatusCode.setStatusCode(getStatusValue(hidlOptionsCmdStatus.status));
    aidlOptionsCMDStatus.setStatus(aidlStatusCode);
    aidlCapInfo = getAidlCapInfo_2_2(hidlOptionsCmdStatus.capInfo.capInfo);
    aidlOptionsCMDStatus.setCapInfo(aidlCapInfo);

    return aidlOptionsCMDStatus;
}

public OptionsCmdStatus getAidlOptionsCmdStatus_2_3(com.qualcomm.qti.uceservice.V2_3.OptionsCmdStatus hidlOptionsCmdStatus) {
    OptionsCmdStatus aidlOptionsCMDStatus = new OptionsCmdStatus();
    OptionsCmdId aidlOptionsCmdId = new OptionsCmdId();
    CapInfo aidlCapInfo = new CapInfo();
    StatusCode aidlStatusCode = new StatusCode();
    aidlOptionsCmdId.setCmdId(getOptionsCmdIdValue(hidlOptionsCmdStatus.cmdId));
    aidlOptionsCMDStatus.setCmdId(aidlOptionsCmdId);
    aidlOptionsCMDStatus.setUserData(toIntExact(hidlOptionsCmdStatus.userData));
    aidlStatusCode.setStatusCode(getStatusValue(hidlOptionsCmdStatus.status));
    aidlOptionsCMDStatus.setStatus(aidlStatusCode);
    aidlCapInfo = getAidlCapInfo_2_3(hidlOptionsCmdStatus.capInfo.capInfo);
    aidlOptionsCMDStatus.setCapInfo(aidlCapInfo);

    return aidlOptionsCMDStatus;
}

/* Utility Functions to map HIDL enum value to AIDL enum value */
 private int getStatusValue(int statusCode) {
        int retStatusCode = StatusCode.UCE_SERVICE_UNKNOWN;
        switch(statusCode) {
        case UceStatusCode.SUCCESS:
            retStatusCode = StatusCode.UCE_SUCCESS;
            break;

        case UceStatusCode.FAILURE:
            retStatusCode = StatusCode.UCE_FAILURE;
            break;

        case UceStatusCode.SUCCESS_ASYNC_UPDATE:
            retStatusCode = StatusCode.UCE_SUCCESS_ASYC_UPDATE;
            break;

        case UceStatusCode.INVALID_SERVICE_HANDLE:
            retStatusCode = StatusCode.UCE_INVALID_SERVICE_HANDLE;
            break;

        case UceStatusCode.INVALID_LISTENER_HANDLE:
            retStatusCode = StatusCode.UCE_INVALID_LISTENER_HANDLE;
            break;

        case UceStatusCode.INVALID_PARAM:
            retStatusCode = StatusCode.UCE_INVALID_PARAM;
            break;

        case UceStatusCode.FETCH_ERROR:
            retStatusCode = StatusCode.UCE_FETCH_ERROR;
            break;

        case UceStatusCode.REQUEST_TIMEOUT:
            retStatusCode = StatusCode.UCE_REQUEST_TIMEOUT;
            break;

        case UceStatusCode.INSUFFICIENT_MEMORY:
            retStatusCode = StatusCode.UCE_INSUFFICIENT_MEMORY;
            break;

        case UceStatusCode.LOST_NET:
            retStatusCode = StatusCode.UCE_LOST_NET;
            break;

        case UceStatusCode.NOT_SUPPORTED:
            retStatusCode = StatusCode.UCE_NOT_SUPPORTED;
            break;

        case UceStatusCode.NOT_FOUND:
            retStatusCode = StatusCode.UCE_NOT_FOUND;
            break;

        case UceStatusCode.SERVICE_UNAVAILABLE:
            retStatusCode = StatusCode.UCE_SERVICE_UNAVAILABLE;
            break;

        case UceStatusCode.NO_CHANGE_IN_CAP:
            retStatusCode = StatusCode.UCE_NO_CHANGE_IN_CAP;
            break;

        case UceStatusCode.INVALID_FEATURE_TAG:
            //retStatusCode = StatusCode.UCE_INVALID_FEATURE_TAG;
            break;

        case UceStatusCode.SERVICE_UNKNOWN:
            retStatusCode = StatusCode.UCE_SERVICE_UNKNOWN;
            break;

        default:
            retStatusCode = StatusCode.UCE_SERVICE_UNKNOWN;
            break;
        }

        return retStatusCode;
    }

private int getPresenceTriggerTypeValue(int publishTrigger) {
    int retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_UNKNOWN;

    switch( publishTrigger ) {
    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.ETAG_EXPIRED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_ETAG_EXPIRED;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_LTE_VOPS_DISABLED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_DISABLED;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_LTE_VOPS_ENABLED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_LTE_VOPS_ENABLED;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_EHRPD:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_EHRPD;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_HSPAPLUS:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_HSPAPLUS;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_3G:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_3G;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_2G:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_2G;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_WLAN:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_WLAN;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.MOVE_TO_IWLAN:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_IWLAN;
    break;

    //2.1 extends 2.0, previous enums values stay the same
    case com.qualcomm.qti.uceservice.V2_1.PresPublishTriggerType.MOVE_TO_NR5G_VOPS_DISABLED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_NR5G_VOPS_DISABLED;
    break;

    case com.qualcomm.qti.uceservice.V2_1.PresPublishTriggerType.MOVE_TO_NR5G_VOPS_ENABLED:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_MOVE_TO_NR5G_VOPS_ENABLED;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresPublishTriggerType.UNKNOWN:
    default:
    retPresTriggerType = PresPublishTriggerType.UCE_PRES_PUBLISH_TRIGGER_UNKNOWN;
    break;
    }

    return retPresTriggerType;
}

private int getPresCmdIdValue(int cmdId) {
    int retCmdId = PresCmdId.UCE_PRES_CMD_UNKNOWN;

    switch( cmdId ) {
    //case PresenceCmdId.GET_VERSION:
    //retCmdId = PresCmdId.UCE_PRES_CMD_GET_VERSION;
    //break;

    case PresenceCmdId.PUBLISHMYCAP:
    retCmdId = PresCmdId.UCE_PRES_CMD_PUBLISHMYCAP;
    break;

    case PresenceCmdId.GETCONTACTCAP:
    retCmdId = PresCmdId.UCE_PRES_CMD_GETCONTACTCAP;
    break;

    case PresenceCmdId.GETCONTACTLISTCAP:
    retCmdId = PresCmdId.UCE_PRES_CMD_GETCONTACTLISTCAP;
    break;

    case PresenceCmdId.SETNEWFEATURETAG:
    retCmdId = PresCmdId.UCE_PRES_CMD_SETNEWFEATURETAG;
    break;

    case PresenceCmdId.REENABLE_SERVICE:
    retCmdId = PresCmdId.UCE_PRES_CMD_REENABLE_SERVICE;
    break;

    case PresenceCmdId.UNKNOWN:
    retCmdId = PresCmdId.UCE_PRES_CMD_UNKNOWN;
    break;

    default:
    retCmdId = PresCmdId.UCE_PRES_CMD_UNKNOWN;
    break;
    }

    return retCmdId;
}

private int getPresSubscriptionState(int presSubscriptionState) {
    int retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_UNKNOWN;

    switch( presSubscriptionState ) {
    case com.qualcomm.qti.uceservice.V2_0.PresSubscriptionState.ACTIVE:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_ACTIVE;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresSubscriptionState.PENDING:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_PENDING;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresSubscriptionState.TERMINATED:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_TERMINATED;
    break;

    case com.qualcomm.qti.uceservice.V2_0.PresSubscriptionState.UNKNOWN:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_UNKNOWN;
    break;

    default:
    retPresSubscriptionState = PresSubscriptionState.UCE_PRES_SUBSCRIPTION_STATE_UNKNOWN;
    break;
    }

    return retPresSubscriptionState;
}

private int getResInstanceStateValue(int instanceState) {
    int retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_UNKNOWN;

    switch( instanceState ) {
    case ResInstantceState.ACTIVE:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_ACTIVE;
    break;

    case ResInstantceState.PENDING:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_PENDING;
    break;

    case ResInstantceState.TERMINATED:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_TERMINATED;
    break;

    case ResInstantceState.UNKNOWN:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_UNKNOWN;
    break;

    //case ResInstantceState.UCE_PRES_RES_INSTANCE_UNKNOWN:
    //retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_UNKNOWN;
    //break;

    default:
    retInstanceState = PresResInstanceInfo.UCE_PRES_RES_INSTANCE_STATE_UNKNOWN;
    break;
    }

    return retInstanceState;
}

private int getOptionsCmdIdValue(int cmdId) {
    int retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN;

    switch( cmdId ) {
    case OptionsCmdId.UCE_OPTIONS_CMD_GETMYCDINFO:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_GETMYCDINFO;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_SETMYCDINFO:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_SETMYCDINFO;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_GETCONTACTCAP:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_SETMYCDINFO;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_GETCONTACTLISTCAP:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_GETCONTACTLISTCAP;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_RESPONSEINCOMINGOPTIONS:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_RESPONSEINCOMINGOPTIONS;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_GET_VERSION:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_GET_VERSION;
    break;

    case OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN;
    break;

    default:
    retCmdId = com.android.ims.internal.uce.options.OptionsCmdId.UCE_OPTIONS_CMD_UNKNOWN;
    break;
    }

    return retCmdId;
}
/* utility function to map HIDL CapInfo to AIDL CapInfo */
private CapInfo getAidlCapInfo(com.qualcomm.qti.uceservice.V2_0.CapabilityInfo hidlCapInfo){
    CapInfo aidlCapInfo = new CapInfo();

    aidlCapInfo.setImSupported(hidlCapInfo.imSupported);
    aidlCapInfo.setFtSupported(hidlCapInfo.ftSupported);
    aidlCapInfo.setFtThumbSupported(hidlCapInfo.ftThumbSupported);
    aidlCapInfo.setFtSnFSupported(hidlCapInfo.ftSnFSupported);
    aidlCapInfo.setFtHttpSupported(hidlCapInfo.ftHttpSupported);
    aidlCapInfo.setIsSupported(hidlCapInfo.imageShareSupported);
    aidlCapInfo.setVsDuringCSSupported(hidlCapInfo.videoShareDuringCSSupported);
    aidlCapInfo.setVsSupported(hidlCapInfo.videoShareSupported);
    aidlCapInfo.setSpSupported(hidlCapInfo.socialPresenceSupported);
    aidlCapInfo.setCdViaPresenceSupported(hidlCapInfo.capDiscViaPresenceSupported);
    aidlCapInfo.setIpVoiceSupported(hidlCapInfo.ipVoiceSupported);
    aidlCapInfo.setIpVideoSupported(hidlCapInfo.ipVideoSupported);
    aidlCapInfo.setGeoPullFtSupported(hidlCapInfo.geoPullFtSupported);
    aidlCapInfo.setGeoPullSupported(hidlCapInfo.geoPullSupported);
    aidlCapInfo.setGeoPushSupported(hidlCapInfo.geoPushSupported);
    aidlCapInfo.setSmSupported(hidlCapInfo.smSupported);
    aidlCapInfo.setFullSnFGroupChatSupported(hidlCapInfo.fullSnFGroupChatSupported);
    aidlCapInfo.setRcsIpVoiceCallSupported(hidlCapInfo.rcsIpVoiceCallSupported);
    aidlCapInfo.setRcsIpVideoCallSupported(hidlCapInfo.rcsIpVideoCallSupported);
    aidlCapInfo.setRcsIpVideoOnlyCallSupported(hidlCapInfo.rcsIpVideoOnlyCallSupported);
    //aidlCapInfo.setExts(hidlCapInfo.mExts.toArray(new String[hidlCapInfo.mExts.size()]));
    aidlCapInfo.setCapTimestamp(hidlCapInfo.capTimestamp);

    return aidlCapInfo;
}

private CapInfo getAidlCapInfo_2_1(com.qualcomm.qti.uceservice.V2_1.CapabilityInfo hidlCapInfo){
    CapInfo aidlCapInfo = new CapInfo();

    aidlCapInfo.setImSupported(hidlCapInfo.imSupported);
    aidlCapInfo.setFtSupported(hidlCapInfo.ftSupported);
    aidlCapInfo.setFtThumbSupported(hidlCapInfo.ftThumbSupported);
    aidlCapInfo.setFtSnFSupported(hidlCapInfo.ftSnFSupported);
    aidlCapInfo.setFtHttpSupported(hidlCapInfo.ftHttpSupported);
    aidlCapInfo.setIsSupported(hidlCapInfo.imageShareSupported);
    aidlCapInfo.setVsDuringCSSupported(hidlCapInfo.videoShareDuringCSSupported);
    aidlCapInfo.setVsSupported(hidlCapInfo.videoShareSupported);
    aidlCapInfo.setSpSupported(hidlCapInfo.socialPresenceSupported);
    aidlCapInfo.setCdViaPresenceSupported(hidlCapInfo.capDiscViaPresenceSupported);
    aidlCapInfo.setIpVoiceSupported(hidlCapInfo.ipVoiceSupported);
    aidlCapInfo.setIpVideoSupported(hidlCapInfo.ipVideoSupported);
    aidlCapInfo.setGeoPullFtSupported(hidlCapInfo.geoPullFtSupported);
    aidlCapInfo.setGeoPullSupported(hidlCapInfo.geoPullSupported);
    aidlCapInfo.setGeoPushSupported(hidlCapInfo.geoPushSupported);
    aidlCapInfo.setSmSupported(hidlCapInfo.smSupported);
    aidlCapInfo.setFullSnFGroupChatSupported(hidlCapInfo.fullSnFGroupChatSupported);
    aidlCapInfo.setRcsIpVoiceCallSupported(hidlCapInfo.rcsIpVoiceCallSupported);
    aidlCapInfo.setRcsIpVideoCallSupported(hidlCapInfo.rcsIpVideoCallSupported);
    aidlCapInfo.setRcsIpVideoOnlyCallSupported(hidlCapInfo.rcsIpVideoOnlyCallSupported);
    aidlCapInfo.setGeoSmsSupported(hidlCapInfo.geoSmsSupported);
    aidlCapInfo.setCallComposerSupported(hidlCapInfo.callComposerSupported);
    aidlCapInfo.setPostCallSupported(hidlCapInfo.postCallSupported);
    aidlCapInfo.setSharedMapSupported(hidlCapInfo.sharedMapSupported);
    aidlCapInfo.setSharedSketchSupported(hidlCapInfo.sharedSketchSupported);
    aidlCapInfo.setChatbotSupported(hidlCapInfo.chatBotSupported);
    aidlCapInfo.setChatbotRoleSupported(hidlCapInfo.chatBotRoleSupported);
    //aidlCapInfo.setExts(hidlCapInfo.mExts.toArray(new String[hidlCapInfo.mExts.size()]));
    aidlCapInfo.setCapTimestamp(hidlCapInfo.capTimestamp);

    return aidlCapInfo;
}

private CapInfo getAidlCapInfo_2_2(com.qualcomm.qti.uceservice.V2_2.CapabilityInfo hidlCapInfo){
    CapInfo aidlCapInfo = new CapInfo();

    aidlCapInfo.setImSupported(hidlCapInfo.imSupported);
    aidlCapInfo.setFtSupported(hidlCapInfo.ftSupported);
    aidlCapInfo.setFtThumbSupported(hidlCapInfo.ftThumbSupported);
    aidlCapInfo.setFtSnFSupported(hidlCapInfo.ftSnFSupported);
    aidlCapInfo.setFtHttpSupported(hidlCapInfo.ftHttpSupported);
    aidlCapInfo.setIsSupported(hidlCapInfo.imageShareSupported);
    aidlCapInfo.setVsDuringCSSupported(hidlCapInfo.videoShareDuringCSSupported);
    aidlCapInfo.setVsSupported(hidlCapInfo.videoShareSupported);
    aidlCapInfo.setSpSupported(hidlCapInfo.socialPresenceSupported);
    aidlCapInfo.setCdViaPresenceSupported(hidlCapInfo.capDiscViaPresenceSupported);
    aidlCapInfo.setIpVoiceSupported(hidlCapInfo.ipVoiceSupported);
    aidlCapInfo.setIpVideoSupported(hidlCapInfo.ipVideoSupported);
    aidlCapInfo.setGeoPullFtSupported(hidlCapInfo.geoPullFtSupported);
    aidlCapInfo.setGeoPullSupported(hidlCapInfo.geoPullSupported);
    aidlCapInfo.setGeoPushSupported(hidlCapInfo.geoPushSupported);
    aidlCapInfo.setSmSupported(hidlCapInfo.smSupported);
    aidlCapInfo.setFullSnFGroupChatSupported(hidlCapInfo.fullSnFGroupChatSupported);
    aidlCapInfo.setRcsIpVoiceCallSupported(hidlCapInfo.rcsIpVoiceCallSupported);
    aidlCapInfo.setRcsIpVideoCallSupported(hidlCapInfo.rcsIpVideoCallSupported);
    aidlCapInfo.setRcsIpVideoOnlyCallSupported(hidlCapInfo.rcsIpVideoOnlyCallSupported);
    aidlCapInfo.setGeoSmsSupported(hidlCapInfo.geoSmsSupported);
    aidlCapInfo.setCallComposerSupported(hidlCapInfo.callComposerSupported);
    aidlCapInfo.setPostCallSupported(hidlCapInfo.postCallSupported);
    aidlCapInfo.setSharedMapSupported(hidlCapInfo.sharedMapSupported);
    aidlCapInfo.setSharedSketchSupported(hidlCapInfo.sharedSketchSupported);
    aidlCapInfo.setChatbotSupported(hidlCapInfo.chatBotSupported);
    aidlCapInfo.setChatbotRoleSupported(hidlCapInfo.chatBotRoleSupported);
    aidlCapInfo.setSmChatbotSupported(hidlCapInfo.standaloneChatbotSupported);
    aidlCapInfo.setMmtelCallComposerSupported(hidlCapInfo.mmtelCallComposerSupported);
    //aidlCapInfo.setExts(hidlCapInfo.mExts.toArray(new String[hidlCapInfo.mExts.size()]));
    aidlCapInfo.setCapTimestamp(hidlCapInfo.capTimestamp);

    return aidlCapInfo;
}

private CapInfo getAidlCapInfo_2_3(com.qualcomm.qti.uceservice.V2_3.CapabilityInfo hidlCapInfo){
    CapInfo aidlCapInfo = new CapInfo();

    if(hidlCapInfo.imSupported){
        aidlCapInfo.addCapability(CapInfo.INSTANT_MSG,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.INSTANT_MSG);
    }

    if(hidlCapInfo.ftSupported){
        aidlCapInfo.addCapability(CapInfo.FILE_TRANSFER,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.FILE_TRANSFER);
    }

    if(hidlCapInfo.ftThumbSupported){
        aidlCapInfo.addCapability(CapInfo.FILE_TRANSFER_THUMBNAIL,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.FILE_TRANSFER_THUMBNAIL);
    }

    if(hidlCapInfo.ftSnFSupported){
        aidlCapInfo.addCapability(CapInfo.FILE_TRANSFER_SNF,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.FILE_TRANSFER_SNF);
    }

    if(hidlCapInfo.ftHttpSupported){
        aidlCapInfo.addCapability(CapInfo.FILE_TRANSFER_HTTP,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.FILE_TRANSFER_HTTP);
    }

    if(hidlCapInfo.imageShareSupported){
        aidlCapInfo.addCapability(CapInfo.IMAGE_SHARE,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.IMAGE_SHARE);
    }

    if(hidlCapInfo.videoShareDuringCSSupported){
        aidlCapInfo.addCapability(CapInfo.VIDEO_SHARE_DURING_CS,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.VIDEO_SHARE_DURING_CS);
    }

    if(hidlCapInfo.videoShareSupported){
        aidlCapInfo.addCapability(CapInfo.VIDEO_SHARE,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.VIDEO_SHARE);
    }

    if(hidlCapInfo.socialPresenceSupported){
        aidlCapInfo.addCapability(CapInfo.SOCIAL_PRESENCE,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.SOCIAL_PRESENCE);
    }

    if(hidlCapInfo.capDiscViaPresenceSupported){
        aidlCapInfo.addCapability(CapInfo.CAPDISC_VIA_PRESENCE,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.CAPDISC_VIA_PRESENCE);
    }

    if(hidlCapInfo.ipVoiceSupported){
        aidlCapInfo.addCapability(CapInfo.IP_VOICE,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.IP_VOICE);
    }

    if(hidlCapInfo.ipVideoSupported){
        aidlCapInfo.addCapability(CapInfo.IP_VIDEO,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.IP_VIDEO);
    }

    if(hidlCapInfo.geoPullFtSupported){
        aidlCapInfo.addCapability(CapInfo.GEOPULL_FT,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.GEOPULL_FT);
    }

    if(hidlCapInfo.geoPullSupported){
        aidlCapInfo.addCapability(CapInfo.GEOPULL,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.GEOPULL);
    }

    if(hidlCapInfo.geoPushSupported){
        aidlCapInfo.addCapability(CapInfo.GEOPUSH,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.GEOPUSH);
    }

    if(hidlCapInfo.smSupported){
        aidlCapInfo.addCapability(CapInfo.STANDALONE_MSG,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.STANDALONE_MSG);
    }

    if(hidlCapInfo.fullSnFGroupChatSupported){
        aidlCapInfo.addCapability(CapInfo.FULL_SNF_GROUPCHAT,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.FULL_SNF_GROUPCHAT);
    }

    if(hidlCapInfo.rcsIpVoiceCallSupported){
        aidlCapInfo.addCapability(CapInfo.RCS_IP_VOICE_CALL,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.RCS_IP_VOICE_CALL);
    }

    if(hidlCapInfo.rcsIpVideoCallSupported){
        aidlCapInfo.addCapability(CapInfo.RCS_IP_VIDEO_CALL,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.RCS_IP_VIDEO_CALL);
    }

    if(hidlCapInfo.rcsIpVideoOnlyCallSupported){
        aidlCapInfo.addCapability(CapInfo.RCS_IP_VIDEO_ONLY_CALL,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    }

    if(hidlCapInfo.geoSmsSupported){
        aidlCapInfo.addCapability(CapInfo.GEOSMS,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.GEOSMS);
    }

    if(hidlCapInfo.callComposerSupported){
        aidlCapInfo.addCapability(CapInfo.CALLCOMPOSER,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.CALLCOMPOSER);
    }

    if(hidlCapInfo.postCallSupported){
        aidlCapInfo.addCapability(CapInfo.POSTCALL,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.POSTCALL);
    }

    if(hidlCapInfo.sharedMapSupported){
        aidlCapInfo.addCapability(CapInfo.SHAREDMAP,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.SHAREDMAP);
    }

    if(hidlCapInfo.sharedSketchSupported){
        aidlCapInfo.addCapability(CapInfo.SHAREDSKETCH,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.SHAREDSKETCH);
    }

    if(hidlCapInfo.chatBotSupported){
        Iterator i = hidlCapInfo.featureTagData.iterator();
        String versions = "";
        while(i.hasNext()){
          com.qualcomm.qti.uceservice.V2_3.FeatureDesc pHidlFeatureDesc =
               (com.qualcomm.qti.uceservice.V2_3.FeatureDesc)i.next();
          if(pHidlFeatureDesc.featureTag.equals(
            "+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.chatbot\""))
          {
            versions = pHidlFeatureDesc.version;
            Log.d(logTAG,"getAidlCapInfo_2_3 :chatBotSupported version:" + versions);
          }
        }
        aidlCapInfo.addCapability(CapInfo.CHATBOT,versions);
    }else{
        aidlCapInfo.removeCapability(CapInfo.CHATBOT);
    }

    if(hidlCapInfo.chatBotRoleSupported){
        aidlCapInfo.addCapability(CapInfo.CHATBOTROLE,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.CHATBOTROLE);
    }

    if(hidlCapInfo.standaloneChatbotSupported){
        Iterator i = hidlCapInfo.featureTagData.iterator();
        String versions = "";
        while(i.hasNext()){
         com.qualcomm.qti.uceservice.V2_3.FeatureDesc pHidlFeatureDesc =
               (com.qualcomm.qti.uceservice.V2_3.FeatureDesc)i.next();
         if(pHidlFeatureDesc.featureTag.equals(
         "+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.chatbot.sa\""))
         {
            versions = pHidlFeatureDesc.version;
            Log.d(logTAG,"getAidlCapInfo_2_3 : sa.chatBotSupported version:" + versions);
         }
        }
        aidlCapInfo.addCapability(CapInfo.STANDALONE_CHATBOT,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.STANDALONE_CHATBOT);
    }

    if(hidlCapInfo.mmtelCallComposerSupported){
        aidlCapInfo.addCapability(CapInfo.MMTEL_CALLCOMPOSER,"");
    }else{
        aidlCapInfo.removeCapability(CapInfo.MMTEL_CALLCOMPOSER);
    }
    //aidlCapInfo.setExts(hidlCapInfo.mExts.toArray(new String[hidlCapInfo.mExts.size()]));
    aidlCapInfo.setCapTimestamp(hidlCapInfo.capTimestamp);

    return aidlCapInfo;
}
}
