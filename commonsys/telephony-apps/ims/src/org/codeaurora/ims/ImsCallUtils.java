/*
 * Copyright (c) 2015-2016, 2018-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.os.SystemProperties;
import android.telecom.VideoProfile;
import android.telecom.Connection.VideoProvider;
import android.telephony.ims.ImsCallProfile;
import android.telephony.ims.ImsReasonInfo;
import android.telephony.SubscriptionManager;
import android.telephony.ims.ProvisioningManager;
import com.qualcomm.ims.utils.Log;
import org.codeaurora.telephony.utils.AsyncResult;

import org.codeaurora.ims.ImsRilException;
import org.codeaurora.ims.QtiCallConstants;

public class ImsCallUtils {
    private static String TAG = "ImsCallUtils";
    private static final int MIN_VIDEO_CALL_PHONE_NUMBER_LENGTH = 7;
    public static final int CONFIG_ITEM_INVALID = -1;
    public static final int CONFIG_TYPE_INVALID = -1;
    public static final int CONFIG_TYPE_INT = 1;
    public static final int CONFIG_TYPE_STRING = 2;

    public static final int SESSION_MERGE_NOT_INITIATED = 0;
    public static final int SESSION_MERGE_STARTED = 1;
    public static final int SESSION_MERGE_COMPLETED = 2;
    public static final int SESSION_MERGE_FAILED = 3;

    public static final int CLIR_RESPONSE_LENGTH = 2;


    /** Checks if a call type is any valid video call type with or without direction
     */
    public static boolean isVideoCall(int callType) {
        return callType == CallDetails.CALL_TYPE_VT
                || callType == CallDetails.CALL_TYPE_VT_TX
                || callType == CallDetails.CALL_TYPE_VT_RX
                || callType == CallDetails.CALL_TYPE_VT_PAUSE
                || callType == CallDetails.CALL_TYPE_VT_RESUME
                || callType == CallDetails.CALL_TYPE_VT_NODIR;
    }

    public static boolean isVoiceCall(int callType) {
        return callType == CallDetails.CALL_TYPE_VOICE;
    }

    /** Check if call type is valid for lower layers
     */
    public static boolean isValidRilModifyCallType(int callType){
        return callType == CallDetails.CALL_TYPE_VT
                || callType == CallDetails.CALL_TYPE_VT_TX
                || callType == CallDetails.CALL_TYPE_VT_RX
                || callType == CallDetails.CALL_TYPE_VOICE
                || callType == CallDetails.CALL_TYPE_VT_NODIR;
    }

    /** Checks if videocall state transitioned to Video Paused state
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isVideoPaused(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        boolean isHoldingPaused = isVideoCall(currCallType)
                && (currCallState == DriverCallIms.State.HOLDING)
                && isVideoCallTypeWithoutDir(nextCallType)
                && (nextCallState == DriverCallIms.State.ACTIVE);
        boolean isActivePaused =  (isVideoCall(currCallType)
                && (currCallState == DriverCallIms.State.ACTIVE)
                && isVideoCallTypeWithoutDir(nextCallType)
                && (nextCallState == DriverCallIms.State.ACTIVE));
        return isHoldingPaused || isActivePaused;
    }

    /** Detects active video call
     */
    public static boolean canVideoPause(ImsCallSessionImpl conn) {
        return isVideoCall(conn.getInternalCallType()) && conn.isCallActive();
    }

    /** Checks if videocall state transitioned to Video Resumed state
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isVideoResumed(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        return (isVideoCallTypeWithoutDir(currCallType)
                && (currCallState == DriverCallIms.State.ACTIVE)
                && isVideoCall(nextCallType)
                && (nextCallState == DriverCallIms.State.ACTIVE));
    }

    /** Checks if AVP Retry needs to be triggered during dialing
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isAvpRetryDialing(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        boolean dialingAvpRetry = (isVideoCall(currCallType)
                    && (currCallState == DriverCallIms.State.DIALING || currCallState == DriverCallIms.State.ALERTING)
                    && isVideoCallTypeWithoutDir(nextCallType)
                    && nextCallState == DriverCallIms.State.ACTIVE);
        return (conn.getImsCallModification().isAvpRetryAllowed() && dialingAvpRetry);
    }

    /** Checks if AVP Retry needs to be triggered during upgrade
     * @param conn - Current connection object
     * @param dc - Latest DriverCallIms instance
     */
    public static boolean isAvpRetryUpgrade(ImsCallSessionImpl conn, DriverCallIms dc) {
        int currCallType = conn.getInternalCallType();
        DriverCallIms.State currCallState = conn.getInternalState();
        int nextCallType = dc.callDetails.call_type;
        DriverCallIms.State nextCallState = dc.state;

        boolean upgradeAvpRetry = (currCallType == CallDetails.CALL_TYPE_VOICE
                    && currCallState == DriverCallIms.State.ACTIVE
                    && isVideoCallTypeWithoutDir(nextCallType)
                    && nextCallState == DriverCallIms.State.ACTIVE);
        return (conn.getImsCallModification().isAvpRetryAllowed() && upgradeAvpRetry);
    }

    /** Checks if a call type is video call type with direction
     * @param callType
     */
    public static boolean isVideoCallTypeWithDir(int callType) {
        return callType == CallDetails.CALL_TYPE_VT
                || callType == CallDetails.CALL_TYPE_VT_RX
                || callType == CallDetails.CALL_TYPE_VT_TX;
    }

    /** Checks if a incoming call is video call
     * @param callSession - Current connection object
     */
    public static boolean isIncomingVideoCall(ImsCallSessionImpl callSession) {
        return (isVideoCall(callSession.getInternalCallType()) &&
                (callSession.getInternalState() == DriverCallIms.State.INCOMING ||
                callSession.getInternalState() == DriverCallIms.State.WAITING));
    }

   /** Checks if a call is incoming call
     * @param callSession - Current connection object
     */
    public static boolean isIncomingCall(ImsCallSessionImpl callSession) {
        return (callSession.getInternalState() == DriverCallIms.State.INCOMING ||
                callSession.getInternalState() == DriverCallIms.State.WAITING);
    }

    /** Checks if a call is not CS video call
     * @param details - Current call details
     */
    public static boolean isNotCsVideoCall(CallDetails details) {
        return isVideoCall(details.call_type) &&
                (details.call_domain != CallDetails.CALL_DOMAIN_CS);
    }

    /** Checks if a call type is video call type without direction
     * @param callType
     */
    public static boolean isVideoCallTypeWithoutDir(int callType) {
        return callType == CallDetails.CALL_TYPE_VT_NODIR;
    }

    public static int convertVideoStateToCallType(int videoState) {
        int callType = CallDetails.CALL_TYPE_VOICE;
        switch (videoState) {
            case VideoProfile.STATE_AUDIO_ONLY:
                callType = CallDetails.CALL_TYPE_VOICE;
                break;
            case VideoProfile.STATE_RX_ENABLED:
                callType = CallDetails.CALL_TYPE_VT_RX;
                break;
            case VideoProfile.STATE_TX_ENABLED:
                callType = CallDetails.CALL_TYPE_VT_TX;
                break;
            case VideoProfile.STATE_BIDIRECTIONAL:
                callType = CallDetails.CALL_TYPE_VT;
                break;
            case VideoProfile.STATE_PAUSED:
                callType = CallDetails.CALL_TYPE_VT_NODIR;
                break;
        }
        return callType;
    }

    public static int convertCallTypeToVideoState(int callType) {
        int videoState = VideoProfile.STATE_AUDIO_ONLY;
        switch (callType) {
            case CallDetails.CALL_TYPE_VOICE:
                videoState = VideoProfile.STATE_AUDIO_ONLY;
                break;
            case CallDetails.CALL_TYPE_VT_RX:
                videoState = VideoProfile.STATE_RX_ENABLED;
                break;
            case CallDetails.CALL_TYPE_VT_TX:
                videoState = VideoProfile.STATE_TX_ENABLED;
                break;
            case CallDetails.CALL_TYPE_VT:
                videoState = VideoProfile.STATE_BIDIRECTIONAL;
                break;
            case CallDetails.CALL_TYPE_VT_PAUSE:
            case CallDetails.CALL_TYPE_VT_NODIR:
                videoState = VideoProfile.STATE_PAUSED;
                break;
        }
        return videoState;
    }

    public static int convertToInternalCallType(int imsCallProfileCallType) {
        int internalCallType = CallDetails.CALL_TYPE_UNKNOWN;
        switch (imsCallProfileCallType) {
            case ImsCallProfile.CALL_TYPE_VOICE:
            case ImsCallProfile.CALL_TYPE_VOICE_N_VIDEO:
                internalCallType = CallDetails.CALL_TYPE_VOICE;
                break;
            case ImsCallProfile.CALL_TYPE_VT:
            case ImsCallProfile.CALL_TYPE_VIDEO_N_VOICE:
                internalCallType = CallDetails.CALL_TYPE_VT;
                break;
            case ImsCallProfile.CALL_TYPE_VT_NODIR:
                internalCallType = CallDetails.CALL_TYPE_VT_NODIR;
                break;
            case ImsCallProfile.CALL_TYPE_VT_TX:
                internalCallType = CallDetails.CALL_TYPE_VT_TX;
                break;
            case ImsCallProfile.CALL_TYPE_VT_RX:
                internalCallType = CallDetails.CALL_TYPE_VT_RX;
                break;
            default:
                Log.e(TAG, "convertToInternalCallType invalid calltype " + imsCallProfileCallType);
                break;
        }
        return internalCallType;
    }

    public static VideoProfile convertToVideoProfile(int callType, int callQuality) {
        VideoProfile videoCallProfile = new VideoProfile(
                convertCallTypeToVideoState(callType), callQuality);
        // TODO in future - add quality to CallDetails
        return videoCallProfile;
    }

    public static int convertImsErrorToUiError(int error) {
        if (error == CallModify.E_CANCELLED) {
            return VideoProvider.SESSION_MODIFY_REQUEST_TIMED_OUT;
        } else if (error == CallModify.E_SUCCESS || error == CallModify.E_UNUSED) {
            return VideoProvider.SESSION_MODIFY_REQUEST_SUCCESS;
        } else if (error == QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY) {
            return QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY;
        } else {
            return VideoProvider.SESSION_MODIFY_REQUEST_FAIL;
        }
    }

    /**
     * Utility method to detect if call type has changed
     * @return boolean - true if both driver calls are not null and call type has changed
     */
    public static boolean hasCallTypeChanged(DriverCallIms dc, DriverCallIms dcUpdate) {
        return (dc != null && dcUpdate != null && dc.callDetails != null
                && dcUpdate.callDetails != null
                && dc.callDetails.call_type != dcUpdate.callDetails.call_type);
    }

    /**
     * Utility method to get modify call error code from exception
     * @return int - session modify request errors. Valid values are
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_FAIL},
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_INVALID},
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_TIMED_OUT},
     *         {@link VideoProvider#SESSION_MODIFY_REQUEST_REJECTED_BY_REMOTE},
     *         {@link QtiCallConstants#SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY}
     *
     */
    public static int getUiErrorCode(Throwable ex) {
        int status = VideoProvider.SESSION_MODIFY_REQUEST_FAIL;
        if (ex instanceof ImsRilException) {
            ImsRilException imsRilException = (ImsRilException) ex;
            status = getUiErrorCode(imsRilException.getErrorCode());
        }
        return status;
    }

    public static int getUiErrorCode(int imsErrorCode) {
        int status = VideoProvider.SESSION_MODIFY_REQUEST_SUCCESS;
        switch (imsErrorCode) {
            case ImsErrorCode.SUCCESS:
            case ImsErrorCode.UNUSED:
                status = VideoProvider.SESSION_MODIFY_REQUEST_SUCCESS;
                break;
            case ImsErrorCode.CANCELLED:
                status = VideoProvider.SESSION_MODIFY_REQUEST_TIMED_OUT;
                break;
            case ImsErrorCode.REJECTED_BY_REMOTE:
                status = VideoProvider.SESSION_MODIFY_REQUEST_REJECTED_BY_REMOTE;
                break;
            case ImsErrorCode.INVALID_PARAMETER:
                status = VideoProvider.SESSION_MODIFY_REQUEST_INVALID;
                break;
            case QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY:
                status = QtiCallConstants.SESSION_MODIFY_REQUEST_FAILED_LOW_BATTERY;
                break;
            default:
                status = VideoProvider.SESSION_MODIFY_REQUEST_FAIL;
        }
        return status;
    }

    public static boolean isConfigRequestValid(int item, int requestType) {
        int configType = CONFIG_TYPE_INVALID;
        /** Checks if item is within range including vendor configuration
        */
        try {
            if (convertImsConfigToImsConfigItem(item) != CONFIG_ITEM_INVALID) {
                switch (item) {
                    case ProvisioningManager.KEY_AMR_CODEC_MODE_SET_VALUES:
                    case ProvisioningManager.KEY_AMR_WB_CODEC_MODE_SET_VALUES:
                    case ProvisioningManager.KEY_REGISTRATION_DOMAIN_NAME:
                    case ProvisioningManager.KEY_LOCAL_BREAKOUT_PCSCF_ADDRESS:
                    case ProvisioningManager.KEY_SMS_PUBLIC_SERVICE_IDENTITY:
                        configType = CONFIG_TYPE_STRING;
                        break;
                    default:
                        configType = CONFIG_TYPE_INT;
                }
            }
        } catch (IllegalArgumentException e) {
            Log.w(TAG, "Invalid config - setting CONFIG_TYPE_INVALID");
            return false;
        }

        return configType == requestType;
    }

    public static int convertImsConfigToImsConfigItem(int config) {
        switch(config) {
            case ProvisioningManager.KEY_AMR_CODEC_MODE_SET_VALUES:
                return ImsConfigItem.VOCODER_AMRMODESET;
            case ProvisioningManager.KEY_AMR_WB_CODEC_MODE_SET_VALUES:
                return ImsConfigItem.VOCODER_AMRWBMODESET;
            case ProvisioningManager.KEY_SIP_SESSION_TIMER_SEC:
                return ImsConfigItem.SIP_SESSION_TIMER;
            case ProvisioningManager.KEY_MINIMUM_SIP_SESSION_EXPIRATION_TIMER_SEC:
                return ImsConfigItem.MIN_SESSION_EXPIRY;
            case ProvisioningManager.KEY_SIP_INVITE_CANCELLATION_TIMER_MS:
                return ImsConfigItem.CANCELLATION_TIMER;
            case ProvisioningManager.KEY_TRANSITION_TO_LTE_DELAY_MS:
                return ImsConfigItem.T_DELAY;
            case ProvisioningManager.KEY_ENABLE_SILENT_REDIAL:
                return ImsConfigItem.SILENT_REDIAL_ENABLE;
            case ProvisioningManager.KEY_T1_TIMER_VALUE_MS:
                return ImsConfigItem.SIP_T1_TIMER;
            case ProvisioningManager.KEY_T2_TIMER_VALUE_MS:
                return ImsConfigItem.SIP_T2_TIMER;
            case ProvisioningManager.KEY_TF_TIMER_VALUE_MS:
                return ImsConfigItem.SIP_TF_TIMER;
            case ProvisioningManager.KEY_VOLTE_PROVISIONING_STATUS:
                return ImsConfigItem.VLT_SETTING_ENABLED;
            case ProvisioningManager.KEY_VT_PROVISIONING_STATUS:
                return ImsConfigItem.LVC_SETTING_ENABLED;
            case ProvisioningManager.KEY_REGISTRATION_DOMAIN_NAME:
                return ImsConfigItem.DOMAIN_NAME;
            case ProvisioningManager.KEY_SMS_FORMAT:
                return ImsConfigItem.SMS_FORMAT;
            case ProvisioningManager.KEY_SMS_OVER_IP_ENABLED:
                return ImsConfigItem.SMS_OVER_IP;
            case ProvisioningManager.KEY_RCS_PUBLISH_TIMER_SEC:
                return ImsConfigItem.PUBLISH_TIMER;
            case ProvisioningManager.KEY_RCS_PUBLISH_OFFLINE_AVAILABILITY_TIMER_SEC:
                return ImsConfigItem.PUBLISH_TIMER_EXTENDED;
            case ProvisioningManager.KEY_RCS_CAPABILITIES_CACHE_EXPIRATION_SEC:
                return ImsConfigItem.CAPABILITIES_CACHE_EXPIRATION;
            case ProvisioningManager.KEY_RCS_AVAILABILITY_CACHE_EXPIRATION_SEC:
                return ImsConfigItem.AVAILABILITY_CACHE_EXPIRATION;
            case ProvisioningManager.KEY_RCS_CAPABILITIES_POLL_INTERVAL_SEC:
                return ImsConfigItem.CAPABILITIES_POLL_INTERVAL;
            case ProvisioningManager.KEY_RCS_PUBLISH_SOURCE_THROTTLE_MS:
                return ImsConfigItem.SOURCE_THROTTLE_PUBLISH;
            case ProvisioningManager.KEY_RCS_MAX_NUM_ENTRIES_IN_RCL:
                return ImsConfigItem.MAX_NUM_ENTRIES_IN_RCL;
            case ProvisioningManager.KEY_RCS_CAPABILITY_POLL_LIST_SUB_EXP_SEC:
                return ImsConfigItem.CAPAB_POLL_LIST_SUB_EXP;
            case ProvisioningManager.KEY_USE_GZIP_FOR_LIST_SUBSCRIPTION:
                return ImsConfigItem.GZIP_FLAG;
            case ProvisioningManager.KEY_EAB_PROVISIONING_STATUS:
                return ImsConfigItem.EAB_SETTING_ENABLED;
            case ProvisioningManager.KEY_VOICE_OVER_WIFI_ROAMING_ENABLED_OVERRIDE:
                return ImsConfigItem.VOICE_OVER_WIFI_ROAMING;
            case ProvisioningManager.KEY_VOICE_OVER_WIFI_MODE_OVERRIDE:
                return ImsConfigItem.VOICE_OVER_WIFI_MODE;
            case ProvisioningManager.KEY_VOICE_OVER_WIFI_ENABLED_OVERRIDE:
                return ImsConfigItem.VWF_SETTING_ENABLED;
            case ProvisioningManager.KEY_LOCAL_BREAKOUT_PCSCF_ADDRESS:
                return ImsConfigItem.LBO_PCSCF_ADDRESS;
            case ProvisioningManager.KEY_MOBILE_DATA_ENABLED:
                return ImsConfigItem.MOBILE_DATA_ENABLED;
            case ProvisioningManager.KEY_VOLTE_USER_OPT_IN_STATUS:
                return ImsConfigItem.VOLTE_USER_OPT_IN_STATUS;
            case ProvisioningManager.KEY_SIP_KEEP_ALIVE_ENABLED:
                return ImsConfigItem.KEEP_ALIVE_ENABLED;
            case ProvisioningManager.KEY_REGISTRATION_RETRY_BASE_TIME_SEC:
                return ImsConfigItem.REGISTRATION_RETRY_BASE_TIME_SEC;
            case ProvisioningManager.KEY_REGISTRATION_RETRY_MAX_TIME_SEC:
                return ImsConfigItem.REGISTRATION_RETRY_MAX_TIME_SEC;
            case ProvisioningManager.KEY_RTP_SPEECH_START_PORT:
                return ImsConfigItem.SPEECH_START_PORT;
            case ProvisioningManager.KEY_RTP_SPEECH_END_PORT:
               return ImsConfigItem.SPEECH_END_PORT;
            case ProvisioningManager.KEY_SIP_INVITE_REQUEST_TRANSMIT_INTERVAL_MS:
               return ImsConfigItem.SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ProvisioningManager.KEY_SIP_INVITE_ACK_WAIT_TIME_MS:
               return ImsConfigItem.SIP_INVITE_RSP_WAIT_TIME_MSEC;
            case ProvisioningManager.KEY_SIP_INVITE_RESPONSE_RETRANSMIT_WAIT_TIME_MS:
               return ImsConfigItem.SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ProvisioningManager.KEY_SIP_NON_INVITE_REQUEST_RETRANSMIT_INTERVAL_MS:
               return ImsConfigItem.SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
            case ProvisioningManager.KEY_SIP_NON_INVITE_TRANSACTION_TIMEOUT_TIMER_MS:
               return ImsConfigItem.SIP_NON_INVITE_TXN_TIMEOUT_TIMER_MSEC;
            case ProvisioningManager.KEY_SIP_INVITE_RESPONSE_RETRANSMIT_INTERVAL_MS:
               return ImsConfigItem.SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
            case ProvisioningManager.KEY_SIP_ACK_RECEIPT_WAIT_TIME_MS:
               return ImsConfigItem.SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
            case ProvisioningManager.KEY_SIP_ACK_RETRANSMIT_WAIT_TIME_MS:
               return ImsConfigItem.SIP_ACK_RETX_WAIT_TIME_MSEC;
            case ProvisioningManager.KEY_SIP_NON_INVITE_REQUEST_RETRANSMISSION_WAIT_TIME_MS:
               return ImsConfigItem.SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
            case ProvisioningManager.KEY_SIP_NON_INVITE_RESPONSE_RETRANSMISSION_WAIT_TIME_MS:
               return ImsConfigItem.SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
            case ProvisioningManager.KEY_AMR_WB_OCTET_ALIGNED_PAYLOAD_TYPE:
               return ImsConfigItem.AMR_WB_OCTET_ALIGNED_PT;
            case ProvisioningManager.KEY_AMR_WB_BANDWIDTH_EFFICIENT_PAYLOAD_TYPE:
               return ImsConfigItem.AMR_WB_BANDWIDTH_EFFICIENT_PT;
            case ProvisioningManager.KEY_AMR_OCTET_ALIGNED_PAYLOAD_TYPE:
               return ImsConfigItem.AMR_OCTET_ALIGNED_PT;
            case ProvisioningManager.KEY_AMR_BANDWIDTH_EFFICIENT_PAYLOAD_TYPE:
               return ImsConfigItem.AMR_BANDWIDTH_EFFICIENT_PT;
            case ProvisioningManager.KEY_DTMF_WB_PAYLOAD_TYPE:
               return ImsConfigItem.DTMF_WB_PT;
            case ProvisioningManager.KEY_DTMF_NB_PAYLOAD_TYPE:
               return ImsConfigItem.DTMF_NB_PT;
            case ProvisioningManager.KEY_AMR_DEFAULT_ENCODING_MODE:
               return ImsConfigItem.AMR_DEFAULT_MODE;
            case ProvisioningManager.KEY_SMS_PUBLIC_SERVICE_IDENTITY:
               return ImsConfigItem.SMS_PSI;
            case ProvisioningManager.KEY_VIDEO_QUALITY:
                return ImsConfigItem.VIDEO_QUALITY;
            case ProvisioningManager.KEY_LTE_THRESHOLD_1:
                return ImsConfigItem.THRESHOLD_LTE1;
            case ProvisioningManager.KEY_LTE_THRESHOLD_2:
                return ImsConfigItem.THRESHOLD_LTE2;
            case ProvisioningManager.KEY_LTE_THRESHOLD_3:
                return ImsConfigItem.THRESHOLD_LTE3;
            case ProvisioningManager.KEY_1X_THRESHOLD:
                return ImsConfigItem.THRESHOLD_1x;
            case ProvisioningManager.KEY_WIFI_THRESHOLD_A:
                return ImsConfigItem.THRESHOLD_WIFI_A;
            case ProvisioningManager.KEY_WIFI_THRESHOLD_B:
                return ImsConfigItem.THRESHOLD_WIFI_B;
            case ProvisioningManager.KEY_LTE_EPDG_TIMER_SEC:
                return ImsConfigItem.T_EPDG_LTE;
            case ProvisioningManager.KEY_WIFI_EPDG_TIMER_SEC:
                return ImsConfigItem.T_EPDG_WIFI;
            case ProvisioningManager.KEY_1X_EPDG_TIMER_SEC:
                return ImsConfigItem.T_EPDG_1x;
            case ProvisioningManager.KEY_MULTIENDPOINT_ENABLED:
                return ImsConfigItem.VCE_SETTING_ENABLED;
            case ProvisioningManager.KEY_RTT_ENABLED:
                return ImsConfigItem.RTT_SETTING_ENABLED;
            case QtiCallConstants.QTI_CONFIG_SMS_APP:
                return ImsConfigItem.SMS_APP;
            case QtiCallConstants.QTI_CONFIG_VVM_APP:
                return ImsConfigItem.VVM_APP;
            case ProvisioningManager.KEY_RCS_CAPABILITY_DISCOVERY_ENABLED:
                return ImsConfigItem.CAPABILITY_DISCOVERY_ENABLED;
            case QtiCallConstants.QTI_CONFIG_VOWIFI_ROAMING_MODE_PREFERENCE:
                return ImsConfigItem.VOICE_OVER_WIFI_ROAMING_MODE;
            case QtiCallConstants.AUTO_REJECT_CALL_MODE:
                return ImsConfigItem.AUTO_REJECT_CALL_MODE;
            case QtiCallConstants.CALL_COMPOSER_MODE:
                return ImsConfigItem.CALL_COMPOSER_MODE;
            default:
                throw new IllegalArgumentException();
        }
    }

    public static boolean isActive(DriverCallIms dc) {
        return (dc != null && dc.state == DriverCallIms.State.ACTIVE);
    }

    /**
     * Maps the proto exception error codes passed from lower layers (RIL) to internal error codes.
     */
    public static int toUiErrorCode(ImsRilException ex) {
        switch (ex.getErrorCode()) {
            case ImsErrorCode.HOLD_RESUME_FAILED:
                return QtiCallConstants.ERROR_CALL_SUPP_SVC_FAILED;
            case ImsErrorCode.HOLD_RESUME_CANCELED:
                return QtiCallConstants.ERROR_CALL_SUPP_SVC_CANCELLED;
            case ImsErrorCode.REINVITE_COLLISION:
                return QtiCallConstants.ERROR_CALL_SUPP_SVC_REINVITE_COLLISION;
            default:
                return QtiCallConstants.ERROR_CALL_CODE_UNSPECIFIED;
        }
    }

    /**
     * Maps the proto exception error codes passed from lower layers (RIL) to Ims error codes.
     */
    public static int toImsErrorCode(ImsRilException ex) {
        switch (ex.getErrorCode()) {
            case ImsErrorCode.HOLD_RESUME_FAILED:
                return ImsReasonInfo.CODE_SUPP_SVC_FAILED;
            case ImsErrorCode.HOLD_RESUME_CANCELED:
                return ImsReasonInfo.CODE_SUPP_SVC_CANCELLED;
            case ImsErrorCode.REINVITE_COLLISION:
                return ImsReasonInfo.CODE_SUPP_SVC_REINVITE_COLLISION;
            case ImsErrorCode.USSD_CS_FALLBACK:
                return ImsReasonInfo.CODE_LOCAL_SERVICE_UNAVAILABLE;
            default:
                return ImsReasonInfo.CODE_UNSPECIFIED;
        }
    }

    /**
     * Check is carrier one supported or not
     */
    public static boolean isCarrierOneSupported() {
        String property = SystemProperties.get("persist.vendor.radio.atel.carrier");
        return "405854".equals(property);
    }

    /**
     * Checks if the vidoCall number is valid,ignore the special characters and invalid length
     *
     * @param number the number to call.
     * @return true if the number is valid
     *
     * @hide
     */
     public static boolean isVideoCallNumValid(String number) {
         if (number == null || number.isEmpty()
                 || number.length() < MIN_VIDEO_CALL_PHONE_NUMBER_LENGTH) {
             Log.w(TAG, "Phone number invalid!");
             return false;
         }

         /*if the first char is "+",then remove it,
          * this for support international call
         */
         if(number.startsWith("+")) {
            number = number.substring(1, number.length());
         }
         //Check non-digit characters '#', '+', ',', and ';'
         if (number.contains("#") || number.contains("+") ||
                 number.contains(",") || number.contains(";") ||
                 number.contains("*")) {
             return false;
         }
         return true;
     }

    /**
     * Creates ImsReasonInfo object from async result.
     */
    public static ImsReasonInfo getImsReasonInfo(AsyncResult ar)
    {
        ImsReasonInfo sipErrorInfo = null;
        int code = ImsReasonInfo.CODE_UNSPECIFIED;

        if (ar != null) {
            sipErrorInfo = (ImsReasonInfo) ar.result;
            if(ar.exception != null) {
                code = ImsCallUtils.toImsErrorCode((ImsRilException)ar.exception);
            }
        }
        if (sipErrorInfo == null) {
            return new ImsReasonInfo(code, ImsReasonInfo.CODE_UNSPECIFIED, null);
        }
        Log.d(TAG, "Sip error code:" + sipErrorInfo.getExtraCode() +
                " error string :" + sipErrorInfo.getExtraMessage());
        return new ImsReasonInfo(code, sipErrorInfo.getExtraCode(),
               sipErrorInfo.getExtraMessage());
    }

    public static boolean isVideoCall(ImsCallSessionImpl session) {
        return (session != null) && isVideoCall(session.getInternalCallType());
    }

    public static boolean isVideoCall(CallModify callModify) {
        return callModify != null && callModify.call_details != null &&
                isVideoCall(callModify.call_details.call_type);
    }

    /**
     * Helper method to update the verstat info in ImsCallProfile.
     */
    public static QImsCallProfile toImsCallProfile(VerstatInfo verstatInfo,
            QImsCallProfile profile) {
        if (verstatInfo == null || profile == null) {
            return profile;
        }
        //No need to add Extra fields for default values.
        if (verstatInfo.canMarkUnwantedCall()) {
            profile.setCallExtraBoolean(
                    QtiCallConstants.CAN_MARK_UNWANTED_CALL,
                    true);
        }
        int verstatVerificationStatus = verstatInfo.getVerstatVerificationStatus();
        if (verstatVerificationStatus == VerstatInfo.VERSTAT_VERIFICATION_NONE) {
            profile.setCallerNumberVerificationStatus(
                ImsCallProfile.VERIFICATION_STATUS_NOT_VERIFIED);
        } else if (verstatVerificationStatus == VerstatInfo.VERSTAT_VERIFICATION_PASS){
            profile.setCallerNumberVerificationStatus(
                ImsCallProfile.VERIFICATION_STATUS_PASSED);
        } else {
            profile.setCallerNumberVerificationStatus(
                ImsCallProfile.VERIFICATION_STATUS_FAILED);
        }

        if (verstatVerificationStatus != VerstatInfo.VERSTAT_VERIFICATION_NONE) {
            profile.setCallExtraInt(
                    QtiCallConstants.VERSTAT_VERIFICATION_STATUS,
                    verstatVerificationStatus);
        }
        return profile;
    }
}
