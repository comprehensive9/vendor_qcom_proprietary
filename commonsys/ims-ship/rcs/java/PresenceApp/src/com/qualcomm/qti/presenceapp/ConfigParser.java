/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.content.Context;
import android.util.Log;

import com.android.ims.internal.uce.common.CapInfo;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;

public class ConfigParser {
    final String FILE_NAME = "PresenceAppConfig.xml";
    static XmlPullParser mParser = null;
    static CapabilityInfo publishCapabilities;
    static CapabilityInfo modifyPublishCapabilities;

    public void init(Context ctx) {
        File txtFile = new File(ctx.getExternalFilesDir(null), FILE_NAME);

        // Read the file Contents in a StringBuilder Object
        StringBuilder text = new StringBuilder();

        try {
            BufferedReader reader = new BufferedReader(new FileReader(txtFile));
            String line;
            while ((line = reader.readLine()) != null) {
                text.append(line + '\n');
            }
            reader.close();
        } catch (IOException e) {
            Log.e("C2c", "Error occured while reading text file!!");
        }

        try {
            XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
            factory.setNamespaceAware(true);
            mParser = factory.newPullParser();
            mParser.setInput(new StringReader(text.toString()));
            starParse();
        } catch (XmlPullParserException e) {
            //
        }
    }
    public void starParse()  {
        XmlPullParser xpp = mParser;
        try {

            int eventType = xpp.getEventType();
            while (eventType != XmlPullParser.END_DOCUMENT) {
                if (eventType == XmlPullParser.START_TAG) {
                    String TagName = xpp.getName();
                    if(TagName.equals("publish_capabilities")) {
                        publishCapabilities = parseCapabilites(xpp, "publish_capabilities");
                    }
                    else if(TagName.equals("modify_publish_capabilities")) {
                        modifyPublishCapabilities = parseCapabilites(xpp,"modify_publish_capabilities");
                    }
                }
                eventType = xpp.next();
            }
        }catch (XmlPullParserException| IOException e) {
            //@TODO: error reading the parser (make a pop-up)
        }
    }

    private CapabilityInfo parseCapabilites(XmlPullParser parser,
                                            String endTag) throws XmlPullParserException, IOException {
        CapInfo caps = new CapInfo();
        int eventType = parser.getEventType();
        String TagName ="";
        String Data = "";
        while (eventType != XmlPullParser.END_DOCUMENT) {
            switch (eventType) {
                case XmlPullParser.START_TAG:
                    TagName = parser.getName();
                    break;
                case XmlPullParser.TEXT:
                    Data = parser.getText();
                    break;
                case XmlPullParser.END_TAG:
                    String engTagName = parser.getName();
                    if(engTagName.equals(endTag)) {
                        return new CapabilityInfo(caps);
                    }
                    else if(TagName.equals("instant_message")) {
                        if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.INSTANT_MSG,"");
                        else
                          caps.removeCapability(CapInfo.INSTANT_MSG);
                    }
                    else if(TagName.equals("file_transfer")) {
                        if(Boolean.parseBoolean(Data))
                           caps.addCapability(CapInfo.FILE_TRANSFER,"");
                        else
                           caps.removeCapability(CapInfo.FILE_TRANSFER);
                    }
                    else if(TagName.equals("file_transfer_thumb")) {
                       if(Boolean.parseBoolean(Data))
                           caps.addCapability(CapInfo.FILE_TRANSFER_THUMBNAIL,"");
                       else
                           caps.removeCapability(CapInfo.FILE_TRANSFER_THUMBNAIL);
                    }
                    else if(TagName.equals("file_transfer_snf")) {
                       if(Boolean.parseBoolean(Data))
                           caps.addCapability(CapInfo.FILE_TRANSFER_SNF,"");
                       else
                           caps.removeCapability(CapInfo.FILE_TRANSFER_SNF);
                    }
                    else if(TagName.equals("file_transfer_http")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.FILE_TRANSFER_HTTP,"");
                      else
                          caps.removeCapability(CapInfo.FILE_TRANSFER_HTTP);
                    }
                    else if(TagName.equals("image_share")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.IMAGE_SHARE,"");
                      else
                          caps.removeCapability(CapInfo.IMAGE_SHARE);
                    }
                    else if(TagName.equals("video_share_in_cs_call")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.VIDEO_SHARE_DURING_CS,"");
                      else
                          caps.removeCapability(CapInfo.VIDEO_SHARE_DURING_CS);
                    }
                    else if(TagName.equals("video_share")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.VIDEO_SHARE,"");
                      else
                          caps.removeCapability(CapInfo.VIDEO_SHARE);
                    }
                    else if(TagName.equals("social_presence")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.SOCIAL_PRESENCE,"");
                      else
                          caps.removeCapability(CapInfo.SOCIAL_PRESENCE);
                    }
                    else if(TagName.equals("presence")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.CAPDISC_VIA_PRESENCE,"");
                      else
                          caps.removeCapability(CapInfo.CAPDISC_VIA_PRESENCE);
                    }
                    else if(TagName.equals("volte")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.IP_VOICE,"");
                      else
                          caps.removeCapability(CapInfo.IP_VOICE);
                    }
                    else if(TagName.equals("video_telephony")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.IP_VIDEO,"");
                      else
                          caps.removeCapability(CapInfo.IP_VIDEO);
                    }
                    else if(TagName.equals("geo_pull_ft")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.GEOPULL_FT,"");
                      else
                          caps.removeCapability(CapInfo.GEOPULL_FT);
                    }
                    else if(TagName.equals("geo_pull")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.GEOPULL,"");
                      else
                          caps.removeCapability(CapInfo.GEOPULL);
                    }
                    else if(TagName.equals("geo_push")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.GEOPUSH,"");
                      else
                          caps.removeCapability(CapInfo.GEOPUSH);
                    }
                    else if(TagName.equals("short_message")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.STANDALONE_MSG,"");
                      else
                          caps.removeCapability(CapInfo.STANDALONE_MSG);
                    }
                    else if(TagName.equals("full_snf_group_chat")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.FULL_SNF_GROUPCHAT,"");
                      else
                          caps.removeCapability(CapInfo.FULL_SNF_GROUPCHAT);
                    }
                    else if(TagName.equals("geo_sms")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.GEOSMS,"");
                      else
                          caps.removeCapability(CapInfo.GEOSMS);
                    }
                    else if(TagName.equals("call_composer")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.CALLCOMPOSER,"");
                      else
                          caps.removeCapability(CapInfo.CALLCOMPOSER);
                    }
                    else if(TagName.equals("post_call")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.POSTCALL,"");
                      else
                          caps.removeCapability(CapInfo.POSTCALL);
                    }
                    else if(TagName.equals("shared_map")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.SHAREDMAP,"");
                      else
                          caps.removeCapability(CapInfo.SHAREDMAP);
                    }
                    else if(TagName.equals("shared_sketch")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.SHAREDSKETCH,"");
                      else
                          caps.removeCapability(CapInfo.SHAREDSKETCH);
                    }
                    else if(TagName.equals("chat_bot")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.CHATBOT,"#=1");
                      else
                          caps.removeCapability(CapInfo.CHATBOT);
                    }
                    else if(TagName.equals("chat_bot_role")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.CHATBOTROLE,"");
                      else
                          caps.removeCapability(CapInfo.CHATBOTROLE);
                    }
                    else if(TagName.equals("sm_chat_bot")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.STANDALONE_CHATBOT,"#=1");
                      else
                          caps.removeCapability(CapInfo.STANDALONE_CHATBOT);
                    }
                    else if(TagName.equals("mmtel_call_composer")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.MMTEL_CALLCOMPOSER,"");
                      else
                          caps.removeCapability(CapInfo.MMTEL_CALLCOMPOSER);
                    }
                    else if(TagName.equals("rcs_ip_video_call")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.RCS_IP_VIDEO_CALL,"");
                      else
                          caps.removeCapability(CapInfo.RCS_IP_VIDEO_CALL);
                    }
                    else if(TagName.equals("rcs_ip_voice_call")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.RCS_IP_VOICE_CALL,"");
                      else
                          caps.removeCapability(CapInfo.RCS_IP_VOICE_CALL);
                    }
                    else if(TagName.equals("rcs_ip_voice_only_call")) {
                      if(Boolean.parseBoolean(Data))
                          caps.addCapability(CapInfo.RCS_IP_VIDEO_ONLY_CALL,"");
                      else
                          caps.removeCapability(CapInfo.RCS_IP_VIDEO_ONLY_CALL);
                    }
                    //@TODO: parse Custom Extensions
            }
            eventType = parser.next();
        }
        return  new CapabilityInfo(caps);
    }
}
