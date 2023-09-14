/*********************************************************************
 Copyright (c) 2017,2019,2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.translator;


import com.qualcomm.qti.uceservice.V2_0.PresenceCapabilityInfo;
import com.qualcomm.qti.uceservice.V2_0.PresServiceInfo;
import com.qualcomm.qti.uceservice.V2_0.OptionsCapabilityInfo;
import com.qualcomm.qti.uceservice.V2_0.CapabilityInfo;

import com.android.ims.internal.uce.common.CapInfo;
import com.android.ims.internal.uce.presence.PresCapInfo;
import com.android.ims.internal.uce.options.OptionsCapInfo;

import android.util.Log;

import java.util.Arrays;
import java.util.Map;
import java.util.HashMap;


public class AidlToHidlTranslator
{
  private final static String logTAG = "AIDL to HIDL Translator";

/* Translator methods pertaining to PresService class */

public PresenceCapabilityInfo getHidlPresCapInfo(PresCapInfo  aidlPresCapInfo) {
    PresenceCapabilityInfo hidlPresCapInfo = new PresenceCapabilityInfo();

    hidlPresCapInfo.capInfo.imSupported =
      aidlPresCapInfo.getCapInfo().isImSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlPresCapInfo.capInfo.ftSupported =
      aidlPresCapInfo.getCapInfo().isFtSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlPresCapInfo.capInfo.ftThumbSupported =
      aidlPresCapInfo.getCapInfo().isFtThumbSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlPresCapInfo.capInfo.ftSnFSupported =
      aidlPresCapInfo.getCapInfo().isFtSnFSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlPresCapInfo.capInfo.ftHttpSupported =
      aidlPresCapInfo.getCapInfo().isFtHttpSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlPresCapInfo.capInfo.imageShareSupported =
      aidlPresCapInfo.getCapInfo().isIsSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlPresCapInfo.capInfo.videoShareDuringCSSupported =
      aidlPresCapInfo.getCapInfo().isVsDuringCSSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlPresCapInfo.capInfo.videoShareSupported =
      aidlPresCapInfo.getCapInfo().isVsSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlPresCapInfo.capInfo.socialPresenceSupported =
      aidlPresCapInfo.getCapInfo().isSpSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlPresCapInfo.capInfo.capDiscViaPresenceSupported =
      aidlPresCapInfo.getCapInfo().isCdViaPresenceSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlPresCapInfo.capInfo.ipVoiceSupported =
      aidlPresCapInfo.getCapInfo().isIpVoiceSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VOICE);
    hidlPresCapInfo.capInfo.ipVideoSupported =
      aidlPresCapInfo.getCapInfo().isIpVideoSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlPresCapInfo.capInfo.geoPullFtSupported =
      aidlPresCapInfo.getCapInfo().isGeoPullFtSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlPresCapInfo.capInfo.geoPullSupported =
      aidlPresCapInfo.getCapInfo().isGeoPullSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL);
    hidlPresCapInfo.capInfo.geoPushSupported =
      aidlPresCapInfo.getCapInfo().isGeoPushSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPUSH);
    hidlPresCapInfo.capInfo.smSupported =
      aidlPresCapInfo.getCapInfo().isSmSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlPresCapInfo.capInfo.fullSnFGroupChatSupported =
      aidlPresCapInfo.getCapInfo().isFullSnFGroupChatSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlPresCapInfo.capInfo.rcsIpVoiceCallSupported =
      aidlPresCapInfo.getCapInfo().isRcsIpVoiceCallSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoCallSupported =
      aidlPresCapInfo.getCapInfo().isRcsIpVideoCallSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoOnlyCallSupported =
      aidlPresCapInfo.getCapInfo().isRcsIpVideoOnlyCallSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    //hidlPresCapInfo.capInfo.mExts.addAll(Arrays.asList(aidlPresCapInfo.getCapInfo().getExts()));
    hidlPresCapInfo.capInfo.capTimestamp =
      aidlPresCapInfo.getCapInfo().getCapTimestamp();
    hidlPresCapInfo.contactUri = "";
    if(aidlPresCapInfo.getContactUri() !=  null){
       hidlPresCapInfo.contactUri = aidlPresCapInfo.getContactUri();
    }

    return hidlPresCapInfo;
}

public com.qualcomm.qti.uceservice.V2_1.PresenceCapabilityInfo getHidlPresCapInfo_2_1(
                    PresCapInfo  aidlPresCapInfo) {
    com.qualcomm.qti.uceservice.V2_1.PresenceCapabilityInfo hidlPresCapInfo
        = new com.qualcomm.qti.uceservice.V2_1.PresenceCapabilityInfo();

    hidlPresCapInfo.capInfo.imSupported =
      aidlPresCapInfo.getCapInfo().isImSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlPresCapInfo.capInfo.ftSupported =
      aidlPresCapInfo.getCapInfo().isFtSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlPresCapInfo.capInfo.ftThumbSupported =
      aidlPresCapInfo.getCapInfo().isFtThumbSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlPresCapInfo.capInfo.ftSnFSupported =
      aidlPresCapInfo.getCapInfo().isFtSnFSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlPresCapInfo.capInfo.ftHttpSupported =
      aidlPresCapInfo.getCapInfo().isFtHttpSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlPresCapInfo.capInfo.imageShareSupported =
      aidlPresCapInfo.getCapInfo().isIsSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlPresCapInfo.capInfo.videoShareDuringCSSupported =
      aidlPresCapInfo.getCapInfo().isVsDuringCSSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlPresCapInfo.capInfo.videoShareSupported =
      aidlPresCapInfo.getCapInfo().isVsSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlPresCapInfo.capInfo.socialPresenceSupported =
      aidlPresCapInfo.getCapInfo().isSpSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlPresCapInfo.capInfo.capDiscViaPresenceSupported =
      aidlPresCapInfo.getCapInfo().isCdViaPresenceSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlPresCapInfo.capInfo.ipVoiceSupported =
      aidlPresCapInfo.getCapInfo().isIpVoiceSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VOICE);
    hidlPresCapInfo.capInfo.ipVideoSupported =
      aidlPresCapInfo.getCapInfo().isIpVideoSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlPresCapInfo.capInfo.geoPullFtSupported =
      aidlPresCapInfo.getCapInfo().isGeoPullFtSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlPresCapInfo.capInfo.geoPullSupported =
      aidlPresCapInfo.getCapInfo().isGeoPullSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL);
    hidlPresCapInfo.capInfo.geoPushSupported =
      aidlPresCapInfo.getCapInfo().isGeoPushSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPUSH);
    hidlPresCapInfo.capInfo.smSupported =
      aidlPresCapInfo.getCapInfo().isSmSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlPresCapInfo.capInfo.fullSnFGroupChatSupported =
      aidlPresCapInfo.getCapInfo().isFullSnFGroupChatSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlPresCapInfo.capInfo.rcsIpVoiceCallSupported =
      aidlPresCapInfo.getCapInfo().isRcsIpVoiceCallSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoCallSupported =
      aidlPresCapInfo.getCapInfo().isRcsIpVideoCallSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoOnlyCallSupported =
      aidlPresCapInfo.getCapInfo().isRcsIpVideoOnlyCallSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    hidlPresCapInfo.capInfo.geoSmsSupported =
      aidlPresCapInfo.getCapInfo().isGeoSmsSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOSMS);
    hidlPresCapInfo.capInfo.callComposerSupported =
      aidlPresCapInfo.getCapInfo().isCallComposerSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CALLCOMPOSER);
    hidlPresCapInfo.capInfo.postCallSupported =
      aidlPresCapInfo.getCapInfo().isPostCallSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.POSTCALL);
    hidlPresCapInfo.capInfo.sharedMapSupported =
      aidlPresCapInfo.getCapInfo().isSharedMapSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SHAREDMAP);
    hidlPresCapInfo.capInfo.sharedSketchSupported =
      aidlPresCapInfo.getCapInfo().isSharedSketchSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SHAREDSKETCH);
    hidlPresCapInfo.capInfo.chatBotSupported =
      aidlPresCapInfo.getCapInfo().isChatbotSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CHATBOT);
    hidlPresCapInfo.capInfo.chatBotRoleSupported =
      aidlPresCapInfo.getCapInfo().isChatbotRoleSupported() ||
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CHATBOTROLE);
    //hidlPresCapInfo.capInfo.mExts.addAll(Arrays.asList(aidlPresCapInfo.getCapInfo().getExts()));
    hidlPresCapInfo.capInfo.capTimestamp =
      aidlPresCapInfo.getCapInfo().getCapTimestamp();
    hidlPresCapInfo.contactUri = "";
    if(aidlPresCapInfo.getContactUri() !=  null){
       hidlPresCapInfo.contactUri = aidlPresCapInfo.getContactUri();
    }

    return hidlPresCapInfo;
}

public com.qualcomm.qti.uceservice.V2_2.PresenceCapabilityInfo getHidlPresCapInfo_2_2(
                                                                    PresCapInfo  aidlPresCapInfo) {
    com.qualcomm.qti.uceservice.V2_2.PresenceCapabilityInfo hidlPresCapInfo
        = new com.qualcomm.qti.uceservice.V2_2.PresenceCapabilityInfo();

    hidlPresCapInfo.capInfo.imSupported = aidlPresCapInfo.getCapInfo().isImSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlPresCapInfo.capInfo.ftSupported = aidlPresCapInfo.getCapInfo().isFtSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlPresCapInfo.capInfo.ftThumbSupported = aidlPresCapInfo.getCapInfo().isFtThumbSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlPresCapInfo.capInfo.ftSnFSupported = aidlPresCapInfo.getCapInfo().isFtSnFSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlPresCapInfo.capInfo.ftHttpSupported = aidlPresCapInfo.getCapInfo().isFtHttpSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlPresCapInfo.capInfo.imageShareSupported = aidlPresCapInfo.getCapInfo().isIsSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlPresCapInfo.capInfo.videoShareDuringCSSupported = aidlPresCapInfo.getCapInfo().isVsDuringCSSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlPresCapInfo.capInfo.videoShareSupported = aidlPresCapInfo.getCapInfo().isVsSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlPresCapInfo.capInfo.socialPresenceSupported = aidlPresCapInfo.getCapInfo().isSpSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlPresCapInfo.capInfo.capDiscViaPresenceSupported = aidlPresCapInfo.getCapInfo().isCdViaPresenceSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlPresCapInfo.capInfo.ipVoiceSupported = aidlPresCapInfo.getCapInfo().isIpVoiceSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VOICE);
    hidlPresCapInfo.capInfo.ipVideoSupported = aidlPresCapInfo.getCapInfo().isIpVideoSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlPresCapInfo.capInfo.geoPullFtSupported = aidlPresCapInfo.getCapInfo().isGeoPullFtSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlPresCapInfo.capInfo.geoPullSupported = aidlPresCapInfo.getCapInfo().isGeoPullSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL);
    hidlPresCapInfo.capInfo.geoPushSupported = aidlPresCapInfo.getCapInfo().isGeoPushSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPUSH);
    hidlPresCapInfo.capInfo.smSupported = aidlPresCapInfo.getCapInfo().isSmSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlPresCapInfo.capInfo.fullSnFGroupChatSupported = aidlPresCapInfo.getCapInfo().isFullSnFGroupChatSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlPresCapInfo.capInfo.rcsIpVoiceCallSupported = aidlPresCapInfo.getCapInfo().isRcsIpVoiceCallSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoCallSupported = aidlPresCapInfo.getCapInfo().isRcsIpVideoCallSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoOnlyCallSupported = aidlPresCapInfo.getCapInfo().isRcsIpVideoOnlyCallSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    hidlPresCapInfo.capInfo.geoSmsSupported = aidlPresCapInfo.getCapInfo().isGeoSmsSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOSMS);
    hidlPresCapInfo.capInfo.callComposerSupported = aidlPresCapInfo.getCapInfo().isCallComposerSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CALLCOMPOSER);
    hidlPresCapInfo.capInfo.postCallSupported = aidlPresCapInfo.getCapInfo().isPostCallSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.POSTCALL);
    hidlPresCapInfo.capInfo.sharedMapSupported = aidlPresCapInfo.getCapInfo().isSharedMapSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SHAREDMAP);
    hidlPresCapInfo.capInfo.sharedSketchSupported = aidlPresCapInfo.getCapInfo().isSharedSketchSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SHAREDSKETCH);
    hidlPresCapInfo.capInfo.chatBotSupported = aidlPresCapInfo.getCapInfo().isChatbotSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CHATBOT);
    hidlPresCapInfo.capInfo.chatBotRoleSupported = aidlPresCapInfo.getCapInfo().isChatbotRoleSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CHATBOTROLE);
    hidlPresCapInfo.capInfo.standaloneChatbotSupported = aidlPresCapInfo.getCapInfo().isSmChatbotSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.STANDALONE_CHATBOT);
    hidlPresCapInfo.capInfo.mmtelCallComposerSupported = aidlPresCapInfo.getCapInfo().isMmtelCallComposerSupported() ||
                                          aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.MMTEL_CALLCOMPOSER);
    //hidlPresCapInfo.capInfo.mExts.addAll(Arrays.asList(aidlPresCapInfo.getCapInfo().getExts()));
    hidlPresCapInfo.capInfo.capTimestamp = aidlPresCapInfo.getCapInfo().getCapTimestamp();
    hidlPresCapInfo.contactUri = "";
    if(aidlPresCapInfo.getContactUri() !=  null){
       hidlPresCapInfo.contactUri = aidlPresCapInfo.getContactUri();
    }

    return hidlPresCapInfo;
}

public com.qualcomm.qti.uceservice.V2_3.PresenceCapabilityInfo getHidlPresCapInfo_2_3(
            PresCapInfo  aidlPresCapInfo) {
    com.qualcomm.qti.uceservice.V2_3.PresenceCapabilityInfo hidlPresCapInfo
        = new com.qualcomm.qti.uceservice.V2_3.PresenceCapabilityInfo();

    hidlPresCapInfo.capInfo.imSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlPresCapInfo.capInfo.ftSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlPresCapInfo.capInfo.ftThumbSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlPresCapInfo.capInfo.ftSnFSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlPresCapInfo.capInfo.ftHttpSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlPresCapInfo.capInfo.imageShareSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlPresCapInfo.capInfo.videoShareDuringCSSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlPresCapInfo.capInfo.videoShareSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlPresCapInfo.capInfo.socialPresenceSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlPresCapInfo.capInfo.capDiscViaPresenceSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlPresCapInfo.capInfo.ipVoiceSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VOICE);
    hidlPresCapInfo.capInfo.ipVideoSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlPresCapInfo.capInfo.geoPullFtSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlPresCapInfo.capInfo.geoPullSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPULL);
    hidlPresCapInfo.capInfo.geoPushSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOPUSH);
    hidlPresCapInfo.capInfo.smSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlPresCapInfo.capInfo.fullSnFGroupChatSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlPresCapInfo.capInfo.rcsIpVoiceCallSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoCallSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlPresCapInfo.capInfo.rcsIpVideoOnlyCallSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    hidlPresCapInfo.capInfo.geoSmsSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.GEOSMS);
    hidlPresCapInfo.capInfo.callComposerSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CALLCOMPOSER);
    hidlPresCapInfo.capInfo.postCallSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.POSTCALL);
    hidlPresCapInfo.capInfo.sharedMapSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SHAREDMAP);
    hidlPresCapInfo.capInfo.sharedSketchSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.SHAREDSKETCH);
    hidlPresCapInfo.capInfo.chatBotSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CHATBOT);
    hidlPresCapInfo.capInfo.chatBotRoleSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.CHATBOTROLE);
    hidlPresCapInfo.capInfo.standaloneChatbotSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.STANDALONE_CHATBOT);
    hidlPresCapInfo.capInfo.mmtelCallComposerSupported =
      aidlPresCapInfo.getCapInfo().isCapabilitySupported(CapInfo.MMTEL_CALLCOMPOSER);
    //hidlPresCapInfo.capInfo.mExts.addAll(Arrays.asList(aidlPresCapInfo.getCapInfo().getExts()));
    hidlPresCapInfo.capInfo.capTimestamp =
      aidlPresCapInfo.getCapInfo().getCapTimestamp();
    hidlPresCapInfo.contactUri = "";
    if(aidlPresCapInfo.getContactUri() !=  null){
       hidlPresCapInfo.contactUri = aidlPresCapInfo.getContactUri();
    }
 //Printing Map Data
    Map<String, String> tempCapInfoMap = new HashMap<String, String>();
    tempCapInfoMap = aidlPresCapInfo.getCapInfo().getCapInfoMap();

    for(Map.Entry<String, String> entry : tempCapInfoMap.entrySet()) {
        Log.d(logTAG,"getHidlPresCapInfo_2_3:  Map key:[" +
              entry.getKey() + "] value:[" + entry.getValue());
        com.qualcomm.qti.uceservice.V2_3.FeatureDesc hidlFeatureTagDesc =
          new com.qualcomm.qti.uceservice.V2_3.FeatureDesc();
        hidlFeatureTagDesc.featureTag = entry.getKey();
        hidlFeatureTagDesc.version = entry.getValue();
        hidlPresCapInfo.capInfo.featureTagData.add(hidlFeatureTagDesc);
    }

    return hidlPresCapInfo;
}

public PresServiceInfo getHidlPresSvcInfo(
    com.android.ims.internal.uce.presence.PresServiceInfo aidlPresServiceInfo) {
    PresServiceInfo hidlPresSvcInfo = new PresServiceInfo();

    hidlPresSvcInfo.serviceId = aidlPresServiceInfo.getServiceId();
    hidlPresSvcInfo.serviceDesc = aidlPresServiceInfo.getServiceDesc();
    hidlPresSvcInfo.serviceVer = aidlPresServiceInfo.getServiceVer();
    hidlPresSvcInfo.mediaCap = aidlPresServiceInfo.getMediaType();

    return hidlPresSvcInfo;
}

/* Translator methods pertaining to OptionsService class */

public OptionsCapabilityInfo getHidlOptCapInfo(OptionsCapInfo aidlOptCapInfo) {
    OptionsCapabilityInfo hidlOptCapInfo = new OptionsCapabilityInfo();

    getHidlCapInfo(hidlOptCapInfo.capInfo, aidlOptCapInfo.getCapInfo());
    hidlOptCapInfo.sdp = new String();
    hidlOptCapInfo.sdp = aidlOptCapInfo.getSdp();

    return hidlOptCapInfo;
}

public com.qualcomm.qti.uceservice.V2_1.OptionsCapabilityInfo getHidlOptCapInfo_2_1(OptionsCapInfo aidlOptCapInfo) {
    com.qualcomm.qti.uceservice.V2_1.OptionsCapabilityInfo hidlOptCapInfo
         = new com.qualcomm.qti.uceservice.V2_1.OptionsCapabilityInfo();

    getHidlCapInfo_2_1(hidlOptCapInfo.capInfo, aidlOptCapInfo.getCapInfo());
    hidlOptCapInfo.sdp = new String();
    hidlOptCapInfo.sdp = aidlOptCapInfo.getSdp();

    return hidlOptCapInfo;
}

public com.qualcomm.qti.uceservice.V2_2.OptionsCapabilityInfo getHidlOptCapInfo_2_2(OptionsCapInfo aidlOptCapInfo) {
    com.qualcomm.qti.uceservice.V2_2.OptionsCapabilityInfo hidlOptCapInfo
         = new com.qualcomm.qti.uceservice.V2_2.OptionsCapabilityInfo();

    getHidlCapInfo_2_2(hidlOptCapInfo.capInfo, aidlOptCapInfo.getCapInfo());
    hidlOptCapInfo.sdp = new String();
    hidlOptCapInfo.sdp = aidlOptCapInfo.getSdp();

    return hidlOptCapInfo;
}

public com.qualcomm.qti.uceservice.V2_3.OptionsCapabilityInfo getHidlOptCapInfo_2_3(
    OptionsCapInfo aidlOptCapInfo) {
    com.qualcomm.qti.uceservice.V2_3.OptionsCapabilityInfo hidlOptCapInfo
         = new com.qualcomm.qti.uceservice.V2_3.OptionsCapabilityInfo();

    getHidlCapInfo_2_3(hidlOptCapInfo.capInfo, aidlOptCapInfo.getCapInfo());
    hidlOptCapInfo.sdp = new String();
    hidlOptCapInfo.sdp = aidlOptCapInfo.getSdp();

    return hidlOptCapInfo;
}

public void getHidlCapInfo(CapabilityInfo hidlCapInfo, CapInfo aidlCapInfo) {

    hidlCapInfo.imSupported = aidlCapInfo.isImSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlCapInfo.ftSupported = aidlCapInfo.isFtSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlCapInfo.ftThumbSupported = aidlCapInfo.isFtThumbSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlCapInfo.ftSnFSupported = aidlCapInfo.isFtSnFSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlCapInfo.ftHttpSupported = aidlCapInfo.isFtHttpSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlCapInfo.imageShareSupported = aidlCapInfo.isIsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlCapInfo.videoShareDuringCSSupported = aidlCapInfo.isVsDuringCSSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlCapInfo.videoShareSupported = aidlCapInfo.isVsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlCapInfo.socialPresenceSupported = aidlCapInfo.isSpSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlCapInfo.capDiscViaPresenceSupported = aidlCapInfo.isCdViaPresenceSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlCapInfo.ipVoiceSupported = aidlCapInfo.isIpVoiceSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IP_VOICE);
    hidlCapInfo.ipVideoSupported = aidlCapInfo.isIpVideoSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlCapInfo.geoPullFtSupported = aidlCapInfo.isGeoPullFtSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlCapInfo.geoPullSupported = aidlCapInfo.isGeoPullSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL);
    hidlCapInfo.geoPushSupported = aidlCapInfo.isGeoPushSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPUSH);
    hidlCapInfo.smSupported = aidlCapInfo.isSmSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlCapInfo.fullSnFGroupChatSupported = aidlCapInfo.isFullSnFGroupChatSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlCapInfo.rcsIpVoiceCallSupported = aidlCapInfo.isRcsIpVoiceCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlCapInfo.rcsIpVideoCallSupported = aidlCapInfo.isRcsIpVideoCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlCapInfo.rcsIpVideoOnlyCallSupported = aidlCapInfo.isRcsIpVideoOnlyCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    //hidlCapInfo.mExts.addAll(Arrays.asList(aidlCapInfo.getExts()));
    hidlCapInfo.capTimestamp = aidlCapInfo.getCapTimestamp();
}

public void getHidlCapInfo_2_1(com.qualcomm.qti.uceservice.V2_1.CapabilityInfo hidlCapInfo,
                                CapInfo aidlCapInfo) {

    hidlCapInfo.imSupported = aidlCapInfo.isImSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlCapInfo.ftSupported = aidlCapInfo.isFtSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlCapInfo.ftThumbSupported = aidlCapInfo.isFtThumbSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlCapInfo.ftSnFSupported = aidlCapInfo.isFtSnFSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlCapInfo.ftHttpSupported = aidlCapInfo.isFtHttpSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlCapInfo.imageShareSupported = aidlCapInfo.isIsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlCapInfo.videoShareDuringCSSupported = aidlCapInfo.isVsDuringCSSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlCapInfo.videoShareSupported = aidlCapInfo.isVsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlCapInfo.socialPresenceSupported = aidlCapInfo.isSpSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlCapInfo.capDiscViaPresenceSupported = aidlCapInfo.isCdViaPresenceSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlCapInfo.ipVoiceSupported = aidlCapInfo.isIpVoiceSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IP_VOICE);
    hidlCapInfo.ipVideoSupported = aidlCapInfo.isIpVideoSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlCapInfo.geoPullFtSupported = aidlCapInfo.isGeoPullFtSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlCapInfo.geoPullSupported = aidlCapInfo.isGeoPullSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL);
    hidlCapInfo.geoPushSupported = aidlCapInfo.isGeoPushSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPUSH);
    hidlCapInfo.smSupported = aidlCapInfo.isSmSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlCapInfo.fullSnFGroupChatSupported = aidlCapInfo.isFullSnFGroupChatSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlCapInfo.rcsIpVoiceCallSupported = aidlCapInfo.isRcsIpVoiceCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlCapInfo.rcsIpVideoCallSupported = aidlCapInfo.isRcsIpVideoCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlCapInfo.rcsIpVideoOnlyCallSupported = aidlCapInfo.isRcsIpVideoOnlyCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    hidlCapInfo.geoSmsSupported = aidlCapInfo.isGeoSmsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOSMS);
    hidlCapInfo.callComposerSupported = aidlCapInfo.isCallComposerSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CALLCOMPOSER);
    hidlCapInfo.postCallSupported = aidlCapInfo.isPostCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.POSTCALL);
    hidlCapInfo.sharedMapSupported = aidlCapInfo.isSharedMapSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.SHAREDMAP);
    hidlCapInfo.sharedSketchSupported = aidlCapInfo.isSharedSketchSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.SHAREDSKETCH);
    hidlCapInfo.chatBotSupported = aidlCapInfo.isChatbotSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CHATBOT);
    hidlCapInfo.chatBotRoleSupported = aidlCapInfo.isChatbotRoleSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CHATBOTROLE);

    //hidlCapInfo.mExts.addAll(Arrays.asList(aidlCapInfo.getExts()));
    hidlCapInfo.capTimestamp = aidlCapInfo.getCapTimestamp();
}

public void getHidlCapInfo_2_2(com.qualcomm.qti.uceservice.V2_2.CapabilityInfo hidlCapInfo,
                                CapInfo aidlCapInfo) {

    hidlCapInfo.imSupported = aidlCapInfo.isImSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlCapInfo.ftSupported = aidlCapInfo.isFtSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlCapInfo.ftThumbSupported = aidlCapInfo.isFtThumbSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlCapInfo.ftSnFSupported = aidlCapInfo.isFtSnFSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlCapInfo.ftHttpSupported = aidlCapInfo.isFtHttpSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlCapInfo.imageShareSupported = aidlCapInfo.isIsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlCapInfo.videoShareDuringCSSupported = aidlCapInfo.isVsDuringCSSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlCapInfo.videoShareSupported = aidlCapInfo.isVsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlCapInfo.socialPresenceSupported = aidlCapInfo.isSpSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlCapInfo.capDiscViaPresenceSupported = aidlCapInfo.isCdViaPresenceSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlCapInfo.ipVoiceSupported = aidlCapInfo.isIpVoiceSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IP_VOICE);
    hidlCapInfo.ipVideoSupported = aidlCapInfo.isIpVideoSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlCapInfo.geoPullFtSupported = aidlCapInfo.isGeoPullFtSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlCapInfo.geoPullSupported = aidlCapInfo.isGeoPullSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL);
    hidlCapInfo.geoPushSupported = aidlCapInfo.isGeoPushSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOPUSH);
    hidlCapInfo.smSupported = aidlCapInfo.isSmSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlCapInfo.fullSnFGroupChatSupported = aidlCapInfo.isFullSnFGroupChatSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlCapInfo.rcsIpVoiceCallSupported = aidlCapInfo.isRcsIpVoiceCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlCapInfo.rcsIpVideoCallSupported = aidlCapInfo.isRcsIpVideoCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlCapInfo.rcsIpVideoOnlyCallSupported = aidlCapInfo.isRcsIpVideoOnlyCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    hidlCapInfo.geoSmsSupported = aidlCapInfo.isGeoSmsSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.GEOSMS);
    hidlCapInfo.callComposerSupported = aidlCapInfo.isCallComposerSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CALLCOMPOSER);
    hidlCapInfo.postCallSupported = aidlCapInfo.isPostCallSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.POSTCALL);
    hidlCapInfo.sharedMapSupported = aidlCapInfo.isSharedMapSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.SHAREDMAP);
    hidlCapInfo.sharedSketchSupported = aidlCapInfo.isSharedSketchSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.SHAREDSKETCH);
    hidlCapInfo.chatBotSupported = aidlCapInfo.isChatbotSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CHATBOT);
    hidlCapInfo.chatBotRoleSupported = aidlCapInfo.isChatbotRoleSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.CHATBOTROLE);
    hidlCapInfo.standaloneChatbotSupported = aidlCapInfo.isSmChatbotSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.STANDALONE_CHATBOT);
    hidlCapInfo.mmtelCallComposerSupported = aidlCapInfo.isMmtelCallComposerSupported() ||
                              aidlCapInfo.isCapabilitySupported(CapInfo.MMTEL_CALLCOMPOSER);

    //hidlCapInfo.mExts.addAll(Arrays.asList(aidlCapInfo.getExts()));
    hidlCapInfo.capTimestamp = aidlCapInfo.getCapTimestamp();
}

public void getHidlCapInfo_2_3(com.qualcomm.qti.uceservice.V2_3.CapabilityInfo hidlCapInfo,
                                CapInfo aidlCapInfo) {
    hidlCapInfo.imSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.INSTANT_MSG);
    hidlCapInfo.ftSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER);
    hidlCapInfo.ftThumbSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL);
    hidlCapInfo.ftSnFSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF);
    hidlCapInfo.ftHttpSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP);
    hidlCapInfo.imageShareSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.IMAGE_SHARE);
    hidlCapInfo.videoShareDuringCSSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS);
    hidlCapInfo.videoShareSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.VIDEO_SHARE);
    hidlCapInfo.socialPresenceSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.SOCIAL_PRESENCE);
    hidlCapInfo.capDiscViaPresenceSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE);
    hidlCapInfo.ipVoiceSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.IP_VOICE);
    hidlCapInfo.ipVideoSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.IP_VIDEO);
    hidlCapInfo.geoPullFtSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL_FT);
    hidlCapInfo.geoPullSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.GEOPULL);
    hidlCapInfo.geoPushSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.GEOPUSH);
    hidlCapInfo.smSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.STANDALONE_MSG);
    hidlCapInfo.fullSnFGroupChatSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT);
    hidlCapInfo.rcsIpVoiceCallSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL);
    hidlCapInfo.rcsIpVideoCallSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL);
    hidlCapInfo.rcsIpVideoOnlyCallSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
    hidlCapInfo.geoSmsSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.GEOSMS);
    hidlCapInfo.callComposerSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.CALLCOMPOSER);
    hidlCapInfo.postCallSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.POSTCALL);
    hidlCapInfo.sharedMapSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.SHAREDMAP);
    hidlCapInfo.sharedSketchSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.SHAREDSKETCH);
    hidlCapInfo.chatBotSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.CHATBOT);
    hidlCapInfo.chatBotRoleSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.CHATBOTROLE);
    hidlCapInfo.standaloneChatbotSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.STANDALONE_CHATBOT);
    hidlCapInfo.mmtelCallComposerSupported =
      aidlCapInfo.isCapabilitySupported(CapInfo.MMTEL_CALLCOMPOSER);

    //hidlCapInfo.mExts.addAll(Arrays.asList(aidlCapInfo.getExts()));
    hidlCapInfo.capTimestamp = aidlCapInfo.getCapTimestamp();

   //Printing the map
    Map<String, String> aidlCapInfoMap = new HashMap<String, String>();
    aidlCapInfoMap = aidlCapInfo.getCapInfoMap();
    for(Map.Entry<String, String> entry : aidlCapInfoMap.entrySet()) {
        Log.d(logTAG,"getHidlCapInfo_2_3:  Map key:[" +
              entry.getKey() + "] value:[" + entry.getValue());
        com.qualcomm.qti.uceservice.V2_3.FeatureDesc hidlFeatureTagDesc =
          new com.qualcomm.qti.uceservice.V2_3.FeatureDesc();
        hidlFeatureTagDesc.featureTag = entry.getKey();
        hidlFeatureTagDesc.version = entry.getValue();
        hidlCapInfo.featureTagData.add(hidlFeatureTagDesc);
    }
}

}