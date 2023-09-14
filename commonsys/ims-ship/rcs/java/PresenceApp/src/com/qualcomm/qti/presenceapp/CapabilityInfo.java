/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.util.Log;

import com.android.ims.internal.uce.common.CapInfo;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CapabilityInfo {

    CapInfo mCaps = new CapInfo();
    private final static String LOG_TAG = MainActivity.LOG_TAG + ": CapabilityInfo:";
    public CapabilityInfo() {
    }

    public CapabilityInfo(CapInfo c) {
        mCaps = c;
    }

    public CapInfo getCapInfo() {
        return mCaps;
    }

    public List<String> getCapsInString() {
        ArrayList<String> caps = new ArrayList<String>();

        for(int i = 0; i < mCaps.getExts().length; i++) {
            if(mCaps.getExts()[i] != null && mCaps.getExts()[i].length() > 0) {
                caps.add(mCaps.getExts()[i] + ": True");
            }
        }

        caps.add("Im: " + mCaps.isCapabilitySupported(CapInfo.INSTANT_MSG));
        caps.add("Presence: " + mCaps.isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE));
        caps.add("VT: " + mCaps.isCapabilitySupported(CapInfo.IP_VIDEO));
        caps.add("VOLTE: " + mCaps.isCapabilitySupported(CapInfo.IP_VOICE));
        caps.add("Chatbot Role: " + mCaps.isCapabilitySupported(CapInfo.CHATBOTROLE));
        if(mCaps.isCapabilitySupported(CapInfo.CHATBOT)) {
          caps.add("Chatbot: " +
             mCaps.isCapabilitySupported(CapInfo.CHATBOT) +
             " Versions: " +
             mCaps.getCapabilityVersions(CapInfo.CHATBOT));
        }else{
          caps.add("Chatbot: " + mCaps.isCapabilitySupported(CapInfo.CHATBOT));
        }
        caps.add("FT over Http: " + mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP));
        caps.add("FT store and Forward: " + mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF));
        caps.add("FT: " + mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER));
        caps.add("FT Thumb: " + mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL));
        caps.add("Store and Forward Group Chat: " +
          mCaps.isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT));
        caps.add("GeoPull FT: " + mCaps.isCapabilitySupported(CapInfo.GEOPULL_FT));
        caps.add("Geo Pull: " + mCaps.isCapabilitySupported(CapInfo.GEOPULL));
        caps.add("Geo Push: " + mCaps.isCapabilitySupported(CapInfo.GEOPUSH));
        caps.add("Geo SMS: " + mCaps.isCapabilitySupported(CapInfo.GEOSMS));
        caps.add("Image Share: " + mCaps.isCapabilitySupported(CapInfo.IMAGE_SHARE));
        caps.add("mmtel Callcomposer: " +
          mCaps.isCapabilitySupported(CapInfo.MMTEL_CALLCOMPOSER));
        caps.add("Post Call: " + mCaps.isCapabilitySupported(CapInfo.POSTCALL));
        caps.add("Rcs IP Video: " + mCaps.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL));
        caps.add("Rcs IP Voice: " + mCaps.isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL));
        caps.add("Rcs IP Video only: " +
          mCaps.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL));
        caps.add("Shared Map: " + mCaps.isCapabilitySupported(CapInfo.SHAREDMAP));
        caps.add("Shared Sketch: " + mCaps.isCapabilitySupported(CapInfo.SHAREDSKETCH));
        if(mCaps.isCapabilitySupported(CapInfo.STANDALONE_CHATBOT)){
          caps.add("SM Chat Bot: " +
            mCaps.isCapabilitySupported(CapInfo.STANDALONE_CHATBOT) +
            " Versions: " +
            mCaps.getCapabilityVersions(CapInfo.STANDALONE_CHATBOT));
        } else {
          caps.add("SM Chat Bot: " +
            mCaps.isCapabilitySupported(CapInfo.STANDALONE_CHATBOT));
        }
        caps.add("SM: " + mCaps.isCapabilitySupported(CapInfo.STANDALONE_MSG));
        caps.add("Social Presence: " +
          mCaps.isCapabilitySupported(CapInfo.SOCIAL_PRESENCE));
        caps.add("Video Share during CS: " +
          mCaps.isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS));
        caps.add("Video Share: " + mCaps.isCapabilitySupported(CapInfo.VIDEO_SHARE));
        caps.add("CallComposer: " + mCaps.isCapabilitySupported(CapInfo.CALLCOMPOSER));
        return caps;
    }
    public void printCaps() {
        List<String> sCaps = getCapsInString();
        for(int i=0; i< sCaps.size(); i++) {
            Log.i(LOG_TAG, sCaps.get(i));
        }
    }
    public void setCap(String CapName, boolean status) {
        if(CapName.equals("Im") ){
            if(status)
              mCaps.addCapability(CapInfo.INSTANT_MSG,"");
            else
              mCaps.removeCapability(CapInfo.INSTANT_MSG);
        }

        if(CapName.equals("Presence") ){
            if(status)
              mCaps.addCapability(CapInfo.CAPDISC_VIA_PRESENCE,"");
            else
              mCaps.removeCapability(CapInfo.CAPDISC_VIA_PRESENCE);
        }
        if(CapName.equals("VT") ){
            if(status)
              mCaps.addCapability(CapInfo.IP_VIDEO,"");
            else
              mCaps.removeCapability(CapInfo.IP_VIDEO);
        }
        if(CapName.equals("VOLTE") ){
            if(status)
              mCaps.addCapability(CapInfo.IP_VOICE,"");
            else
              mCaps.removeCapability(CapInfo.IP_VOICE);
        }
        if(CapName.equals("Chatbot Role") ){
            if(status)
              mCaps.addCapability(CapInfo.CHATBOTROLE,"");
            else
              mCaps.removeCapability(CapInfo.CHATBOTROLE);
        }
        if(CapName.equals("Chatbot") ){
            if(status)
              mCaps.addCapability(CapInfo.CHATBOT,"");
            else
              mCaps.removeCapability(CapInfo.CHATBOT);
        }
        if(CapName.equals("FT over Http") ){
            if(status)
              mCaps.addCapability(CapInfo.FILE_TRANSFER_HTTP,"");
              else
              mCaps.removeCapability(CapInfo.FILE_TRANSFER_HTTP);
        }
        if(CapName.equals("FT store and Forward") ){
            if(status)
              mCaps.addCapability(CapInfo.FILE_TRANSFER_SNF,"");
              else
              mCaps.removeCapability(CapInfo.FILE_TRANSFER_SNF);
        }
        if(CapName.equals("FT") ){
            if(status)
              mCaps.addCapability(CapInfo.FILE_TRANSFER,"");
              else
              mCaps.removeCapability(CapInfo.FILE_TRANSFER);
        }
        if(CapName.equals("FT Thumb") ){
            if(status)
              mCaps.addCapability(CapInfo.FILE_TRANSFER_THUMBNAIL,"");
            else
              mCaps.removeCapability(CapInfo.FILE_TRANSFER_THUMBNAIL);
        }
        if(CapName.equals("Store and Forward Group Chat") ){
            if(status)
              mCaps.addCapability(CapInfo.FULL_SNF_GROUPCHAT,"");
            else
              mCaps.removeCapability(CapInfo.FULL_SNF_GROUPCHAT);
        }
        if(CapName.equals("GeoPull FT") ){
            if(status)
              mCaps.addCapability(CapInfo.GEOPULL_FT,"");
            else
              mCaps.removeCapability(CapInfo.GEOPULL_FT);
        }
        if(CapName.equals("Geo Pull") ){
            if(status)
              mCaps.addCapability(CapInfo.GEOPULL,"");
            else
              mCaps.removeCapability(CapInfo.GEOPULL);
        }
        if(CapName.equals("Geo Push") ){
            if(status)
              mCaps.addCapability(CapInfo.GEOPUSH,"");
            else
              mCaps.removeCapability(CapInfo.GEOPUSH);
        }
        if(CapName.equals("Geo SMS") ){
            if(status)
              mCaps.addCapability(CapInfo.GEOSMS,"");
            else
              mCaps.removeCapability(CapInfo.GEOSMS);
        }
        if(CapName.equals("Image Share") ){
            if(status)
              mCaps.addCapability(CapInfo.IMAGE_SHARE,"");
            else
              mCaps.removeCapability(CapInfo.IMAGE_SHARE);
        }
        if(CapName.equals("mmtel Callcomposer") ){
            if(status)
              mCaps.addCapability(CapInfo.MMTEL_CALLCOMPOSER,"");
            else
              mCaps.removeCapability(CapInfo.MMTEL_CALLCOMPOSER);
        }
        if(CapName.equals("Post Call") ){
            if(status)
              mCaps.addCapability(CapInfo.POSTCALL,"");
            else
              mCaps.removeCapability(CapInfo.POSTCALL);
        }
        if(CapName.equals("Rcs IP Video") ){
            if(status)
              mCaps.addCapability(CapInfo.RCS_IP_VIDEO_CALL,"");
            else
              mCaps.removeCapability(CapInfo.RCS_IP_VIDEO_CALL);
        }
        if(CapName.equals("Rcs IP Voice") ){
            if(status)
              mCaps.addCapability(CapInfo.RCS_IP_VOICE_CALL,"");
            else
              mCaps.removeCapability(CapInfo.RCS_IP_VOICE_CALL);
        }
        if(CapName.equals("Rcs IP Video only") ){
            if(status)
              mCaps.addCapability(CapInfo.RCS_IP_VIDEO_ONLY_CALL,"");
            else
              mCaps.removeCapability(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
        }
        if(CapName.equals("Shared Map") ){
            if(status)
              mCaps.addCapability(CapInfo.SHAREDMAP,"");
            else
              mCaps.removeCapability(CapInfo.SHAREDMAP);
        }
        if(CapName.equals("Shared Sketch") ){
            if(status)
              mCaps.addCapability(CapInfo.SHAREDSKETCH,"");
            else
              mCaps.removeCapability(CapInfo.SHAREDSKETCH);
        }
        if(CapName.equals("SM Chat Bot") ){
            if(status)
              mCaps.addCapability(CapInfo.STANDALONE_CHATBOT,"");
            else
              mCaps.removeCapability(CapInfo.STANDALONE_CHATBOT);
        }
        if(CapName.equals("SM") ){
            if(status)
              mCaps.addCapability(CapInfo.STANDALONE_MSG,"");
            else
              mCaps.removeCapability(CapInfo.STANDALONE_MSG);
        }
        if(CapName.equals("Social Presence") ){
            if(status)
              mCaps.addCapability(CapInfo.SOCIAL_PRESENCE,"");
            else
              mCaps.removeCapability(CapInfo.SOCIAL_PRESENCE);
        }
        if(CapName.equals("Video Share during CS") ){
            if(status)
              mCaps.addCapability(CapInfo.VIDEO_SHARE_DURING_CS,"");
            else
              mCaps.removeCapability(CapInfo.VIDEO_SHARE_DURING_CS);
        }
        if(CapName.equals("Video Share") ){
            if(status)
              mCaps.addCapability(CapInfo.VIDEO_SHARE,"");
            else
              mCaps.removeCapability(CapInfo.VIDEO_SHARE);
        }
        if(CapName.equals("CallComposer") ){
            if(status)
              mCaps.addCapability(CapInfo.CALLCOMPOSER,"");
            else
              mCaps.removeCapability(CapInfo.CALLCOMPOSER);
        }
    }

    public Map<String, Boolean> getCapsInMap() {
        Map<String, Boolean> caps = new HashMap<String, Boolean>();

        caps.put("Im", mCaps.isCapabilitySupported(CapInfo.INSTANT_MSG));
        caps.put("Presence", mCaps.isCapabilitySupported(CapInfo.CAPDISC_VIA_PRESENCE));
        caps.put("VT", mCaps.isCapabilitySupported(CapInfo.IP_VIDEO));
        caps.put("VOLTE", mCaps.isCapabilitySupported(CapInfo.IP_VOICE));
        caps.put("Chatbot Role", mCaps.isCapabilitySupported(CapInfo.CHATBOTROLE));
        caps.put("Chatbot", mCaps.isCapabilitySupported(CapInfo.CHATBOT));
        caps.put("FT over Http",
                 mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER_HTTP));
        caps.put("FT store and Forward",
                 mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER_SNF));
        caps.put("FT", mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER));
        caps.put("FT Thumb",
                 mCaps.isCapabilitySupported(CapInfo.FILE_TRANSFER_THUMBNAIL));
        caps.put("Store and Forward Group Chat",
                 mCaps.isCapabilitySupported(CapInfo.FULL_SNF_GROUPCHAT));
        caps.put("GeoPull FT", mCaps.isCapabilitySupported(CapInfo.GEOPULL_FT));
        caps.put("Geo Pull", mCaps.isCapabilitySupported(CapInfo.GEOPULL));
        caps.put("Geo Push", mCaps.isCapabilitySupported(CapInfo.GEOPUSH));
        caps.put("Geo SMS", mCaps.isCapabilitySupported(CapInfo.GEOSMS));
        caps.put("Image Share", mCaps.isCapabilitySupported(CapInfo.IMAGE_SHARE));
        caps.put("mmtel Callcomposer",
                 mCaps.isCapabilitySupported(CapInfo.MMTEL_CALLCOMPOSER));
        caps.put("Post Call", mCaps.isCapabilitySupported(CapInfo.POSTCALL));
        caps.put("Rcs IP Video",
                 mCaps.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_CALL));
        caps.put("Rcs IP Voice",
                 mCaps.isCapabilitySupported(CapInfo.RCS_IP_VOICE_CALL));
        caps.put("Rcs IP Video only",
                 mCaps.isCapabilitySupported(CapInfo.RCS_IP_VIDEO_ONLY_CALL));
        caps.put("Shared Map" , mCaps.isCapabilitySupported(CapInfo.SHAREDMAP));
        caps.put("Shared Sketch" , mCaps.isCapabilitySupported(CapInfo.SHAREDSKETCH));
        caps.put("SM Chat Bot" ,
                 mCaps.isCapabilitySupported(CapInfo.STANDALONE_CHATBOT));
        caps.put("SM" , mCaps.isCapabilitySupported(CapInfo.STANDALONE_MSG));
        caps.put("Social Presence" ,
                 mCaps.isCapabilitySupported(CapInfo.SOCIAL_PRESENCE));
        caps.put("Video Share during CS" ,
                 mCaps.isCapabilitySupported(CapInfo.VIDEO_SHARE_DURING_CS));
        caps.put("Video Share" , mCaps.isCapabilitySupported(CapInfo.VIDEO_SHARE));
        caps.put("CallComposer" , mCaps.isCapabilitySupported(CapInfo.CALLCOMPOSER));

        return caps;
    }
    public void decodeFeatureTagToCap(String featureTag, String version) {
        if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.im\"")) {
            mCaps.addCapability(CapInfo.INSTANT_MSG,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.gsma-is\"")) {
            mCaps.addCapability(CapInfo.IMAGE_SHARE,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.ft\"")) {
            mCaps.addCapability(CapInfo.FILE_TRANSFER,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.gsma-vs\"")) {
            mCaps.addCapability(CapInfo.VIDEO_SHARE,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.sp\"")) {
            mCaps.addCapability(CapInfo.SOCIAL_PRESENCE,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.dp\"")) {
            mCaps.addCapability(CapInfo.CAPDISC_VIA_PRESENCE,"");
        }else if(featureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\"")) {
            mCaps.addCapability(CapInfo.IP_VOICE,"");
        }else if(featureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel\";video")) {
            mCaps.addCapability(CapInfo.IP_VIDEO,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.geopull\"")) {
            mCaps.addCapability(CapInfo.GEOPULL,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.geopush\"")) {
            mCaps.addCapability(CapInfo.GEOPUSH,"");
        }else if(featureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.oma.cpm.msg;urn%3Aurn-7%3A3gpp-service.ims.icsi.oma.cpm.largemsg\"")) {
            mCaps.addCapability(CapInfo.STANDALONE_MSG,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.ftthumb\"")) {
            mCaps.addCapability(CapInfo.FILE_TRANSFER_THUMBNAIL,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.ftstandfw\"")) {
            mCaps.addCapability(CapInfo.FILE_TRANSFER_SNF,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.fthttp\"")) {
            mCaps.addCapability(CapInfo.FILE_TRANSFER_HTTP,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.fullsfgroupchat\"")) {
            mCaps.addCapability(CapInfo.FULL_SNF_GROUPCHAT,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.geosms\"")) {
            mCaps.addCapability(CapInfo.GEOSMS,"");
        }else if(featureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.gsma.callcomposer\"")) {
            mCaps.addCapability(CapInfo.CALLCOMPOSER,"");
        }else if(featureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.gsma.callunanswered\"")) {
            mCaps.addCapability(CapInfo.POSTCALL,"");
        }else if(featureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.gsma.sharedmap\"")) {
            mCaps.addCapability(CapInfo.SHAREDMAP,"");
        }else if(featureTag.equals("+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.gsma.sharedsketch\"")) {
            mCaps.addCapability(CapInfo.SHAREDSKETCH,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.chatbot\"")) {
            mCaps.addCapability(CapInfo.CHATBOT,version);
        }else if(featureTag.equals("+g.gsma.rcs.isbot")) {
            mCaps.addCapability(CapInfo.CHATBOTROLE,"");
        }else if(featureTag.equals("+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcs.chatbot.sa\"")) {
            mCaps.addCapability(CapInfo.STANDALONE_CHATBOT,version);
        }else if(featureTag.equals("+g.gsma.callcomposer")) {
            mCaps.addCapability(CapInfo.MMTEL_CALLCOMPOSER,"");
        }
    }

    public void setVolteStatus(boolean t) {
        mCaps.setIpVoiceSupported(t);
    }

    public void setVtStatus(boolean t) {
        if(t)
          mCaps.addCapability(CapInfo.IP_VIDEO,"");
        else
          mCaps.removeCapability(CapInfo.IP_VIDEO);
    }

    public void setCallComposerStatus(boolean t) {
        if(t)
          mCaps.addCapability(CapInfo.CALLCOMPOSER,"");
        else
          mCaps.removeCapability(CapInfo.CALLCOMPOSER);
    }

    public void enableDisableFeatures(boolean supported) {
        if(supported) {
          enableAllFeatures();
        }
        else {
          mCaps.removeCapability(CapInfo.INSTANT_MSG);
          mCaps.removeCapability(CapInfo.IMAGE_SHARE);
          mCaps.removeCapability(CapInfo.FILE_TRANSFER);
          mCaps.removeCapability(CapInfo.VIDEO_SHARE);
          mCaps.removeCapability(CapInfo.SOCIAL_PRESENCE);
          mCaps.removeCapability(CapInfo.CAPDISC_VIA_PRESENCE);
          mCaps.removeCapability(CapInfo.IP_VOICE);
          mCaps.removeCapability(CapInfo.IP_VIDEO);
          mCaps.removeCapability(CapInfo.GEOPULL);
          mCaps.removeCapability(CapInfo.GEOPUSH);
          mCaps.removeCapability(CapInfo.STANDALONE_MSG);
          mCaps.removeCapability(CapInfo.FILE_TRANSFER_THUMBNAIL);
          mCaps.removeCapability(CapInfo.FILE_TRANSFER_SNF);
          mCaps.removeCapability(CapInfo.FILE_TRANSFER_HTTP);
          mCaps.removeCapability(CapInfo.FULL_SNF_GROUPCHAT);
          mCaps.removeCapability(CapInfo.GEOSMS);
          mCaps.removeCapability(CapInfo.CALLCOMPOSER);
          mCaps.removeCapability(CapInfo.POSTCALL);
          mCaps.removeCapability(CapInfo.SHAREDMAP);
          mCaps.removeCapability(CapInfo.SHAREDSKETCH);
          mCaps.removeCapability(CapInfo.CHATBOT);
          mCaps.removeCapability(CapInfo.CHATBOTROLE);
          mCaps.removeCapability(CapInfo.STANDALONE_CHATBOT);
          mCaps.removeCapability(CapInfo.MMTEL_CALLCOMPOSER);
        }
    }
    public void enableAllFeatures() {
        mCaps.addCapability(CapInfo.INSTANT_MSG,"");
        mCaps.addCapability(CapInfo.IMAGE_SHARE,"");
        mCaps.addCapability(CapInfo.FILE_TRANSFER,"");
        mCaps.addCapability(CapInfo.VIDEO_SHARE,"");
        mCaps.addCapability(CapInfo.SOCIAL_PRESENCE,"");
        mCaps.addCapability(CapInfo.CAPDISC_VIA_PRESENCE,"");
        mCaps.addCapability(CapInfo.IP_VOICE,"");
        mCaps.addCapability(CapInfo.IP_VIDEO,"");
        mCaps.addCapability(CapInfo.GEOPULL,"");
        mCaps.addCapability(CapInfo.GEOPUSH,"");
        mCaps.addCapability(CapInfo.STANDALONE_MSG,"");
        mCaps.addCapability(CapInfo.FILE_TRANSFER_THUMBNAIL,"");
        mCaps.addCapability(CapInfo.FILE_TRANSFER_SNF,"");
        mCaps.addCapability(CapInfo.FILE_TRANSFER_HTTP,"");
        mCaps.addCapability(CapInfo.FULL_SNF_GROUPCHAT,"");
        mCaps.addCapability(CapInfo.GEOSMS,"");
        mCaps.addCapability(CapInfo.CALLCOMPOSER,"");
        mCaps.addCapability(CapInfo.POSTCALL,"");
        mCaps.addCapability(CapInfo.SHAREDMAP,"");
        mCaps.addCapability(CapInfo.SHAREDSKETCH,"");
        mCaps.addCapability(CapInfo.CHATBOT,"");
        mCaps.addCapability(CapInfo.CHATBOTROLE,"");
        mCaps.addCapability(CapInfo.STANDALONE_CHATBOT,"");
        mCaps.addCapability(CapInfo.MMTEL_CALLCOMPOSER,"");
    }
}
