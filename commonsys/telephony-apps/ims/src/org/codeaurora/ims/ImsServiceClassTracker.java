/*
 * Copyright (c) 2015-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
  * Not a Contribution.
  *
  * Copyright (C) 2013 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
 */

package org.codeaurora.ims;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.List;
import java.util.Iterator;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.telephony.ims.feature.MmTelFeature;
import android.telephony.ims.ImsCallProfile;
import android.telephony.ims.ImsReasonInfo;
import android.telephony.ims.ImsSuppServiceNotification;
import android.telephony.ims.stub.ImsCallSessionImplBase;
import android.util.SparseIntArray;

import androidx.annotation.VisibleForTesting;

import com.qualcomm.ims.utils.Log;
import org.codeaurora.telephony.utils.AsyncResult;

import org.codeaurora.ims.utils.CallComposerInfoUtils;
import org.codeaurora.ims.utils.QtiImsExtUtils;

public class ImsServiceClassTracker {
    // Call Id, Dial String --> Call Session
    private final Map<String, ImsCallSessionImpl> mCallList;
    private final ArrayList<ImsCallSessionImpl> mPendingSessionList;
    private ArrayList<ImsUssdSessionImpl> mUssdList;
    private List<ICallListListener> mCallListListeners;
    private SparseIntArray mCallComposerTokenList;

    private ImsSenderRxr mCi = null;
    private Context mContext;

    private boolean mIsVideoSupported = false;
    private boolean mIsVoiceSupported = false;

    private Handler mHandler;
    private static final int EVENT_CONFERENCE = 1;
    private ImsCallSessionCallbackHandler mMergeHostListener;
    private static final int INVALID_CALL_ID = -1;
    private ConferenceResult mConferenceResult;
    private ImsServiceSub mServiceSub;

    public static final String CONF_URI_DC_NUMBER = "Conference Call";

    private static int sToken = -1;

    private QImsSessionBase.ListenerBase mCallListener = new QImsSessionBase.ListenerBase() {
        @Override
        public void onClosed(QImsSessionBase s) {
            ImsServiceClassTracker.this.onCallClosed(s);
        }
    };

    private QImsSessionBase.ListenerBase mUssdListener = new QImsSessionBase.ListenerBase() {
        @Override
        public void onClosed(QImsSessionBase s) {
            ImsServiceClassTracker.this.onUssdClosed(s);
        }
    };

    // Constructor
    public ImsServiceClassTracker(ImsSenderRxr ci, Context context, ImsServiceSub serviceSub) {
        mCi = ci;
        mContext = context;
        mCallList = new HashMap<String, ImsCallSessionImpl>();
        mPendingSessionList = new ArrayList<ImsCallSessionImpl>();
        mUssdList = new ArrayList<ImsUssdSessionImpl>();
        mCallListListeners = new CopyOnWriteArrayList<>();
        mCallComposerTokenList = new SparseIntArray();
        mServiceSub = serviceSub;
    }

    /**
     * Get the call Count associated with this tracker object
     * @return call Count
     */
    @VisibleForTesting
    public int getCallCount() {
        return mCallList.size();
    }

    /**
     * Creates a Bunde for passing incoming call data.
     * @param isUssd - If this is a USSD request
     * @param isUnknown - If this is a Phantom call
     *
     * @return Bundle instance containing the params.
     */
    private Bundle createIncomingCallBundle(boolean isUssd, boolean isUnknown) {
        Bundle extras = new Bundle();
        extras.putBoolean(MmTelFeature.EXTRA_IS_USSD, isUssd);
        extras.putBoolean(MmTelFeature.EXTRA_IS_UNKNOWN_CALL, isUnknown);
        return extras;
    }

    public void updateFeatureCapabilities(boolean isVideoSupported, boolean isVoiceSupported) {
        Log.i(this,"updateFeatureCapabilities video " + isVideoSupported + " voice " + isVoiceSupported);
        mIsVideoSupported = isVideoSupported;
        mIsVoiceSupported = isVoiceSupported;
        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                session.updateFeatureCapabilities(isVideoSupported, isVoiceSupported);
            }
        }
    }

    public void updateLowBatteryStatus() {
        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                session.updateLowBatteryStatus();
            }
        }
    }

    private class ConferenceResult {
        ConferenceResult() {
        }

        ImsCallSessionImpl activeCall;
        ImsCallSessionImpl heldCall;
        ImsCallSessionImpl confHostCall;
        boolean shouldHaveTransientSession = true;
    }

    private void cleanupConferenceAttempt() {
        mMergeHostListener = null;
        mConferenceResult = null;
    }

    public void handleConferenceResult(AsyncResult ar) {
        if (ar != null) {
            if (ar.exception != null) {
                // If the response is a failure,  we know that both the calls
                // have failed to get merged into the conference call.
                ImsReasonInfo reason = ImsCallUtils.getImsReasonInfo(ar);
                mMergeHostListener.callSessionMergeFailed(reason);
                cleanupConferenceAttempt();
                for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                        Map.Entry<String, ImsCallSessionImpl> e = it.next();
                        ImsCallSessionImpl cs = e.getValue();
                        cs.unMuteStateReporting();
                        cs.updateMergeStatus(ImsCallUtils.SESSION_MERGE_FAILED);
                        cs.mIsMergeHostSession = false;
                }
                return;
            } else {
                processConferenceResult();
            }
        }
    }

    public void processConferenceResult() {
        Log.i(this, "Conference response received. Processing final result.");

        boolean foundActiveCall = false;
        boolean foundHeldCall = false;

        // Check the states of existing calls. By this point of time, the call list
        // should be up to date with the final states of all the call sessions. If
        // sessions have ended as part of the merge process, they will not be present.
        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                mCallList.entrySet().iterator(); it.hasNext();) {
            Map.Entry<String, ImsCallSessionImpl> e = it.next();
            ImsCallSessionImpl cs = e.getValue();

            if (cs.getDcState() == DriverCallIms.State.ACTIVE) {
                //If the conference host terminates before conference call session is
                //created conference info xml file will be processed with host ConfInfo
                //object which has to be updated to conference call
                if (mConferenceResult.shouldHaveTransientSession &&
                        (mConferenceResult.activeCall != null)) {
                    cs.setConfInfo(mConferenceResult.activeCall.getConfInfo());
                }

                // Check for the active call. If scenarios where the calls have swapped,
                // the swapped active call will be detected in the Holding state check.
                mConferenceResult.activeCall = cs;

                if (mConferenceResult.shouldHaveTransientSession) {
                    cs.mIsConferenceHostSession = true;
                    Log.v(this, "Setting mIsConferenceHostSession to true");
                }
                foundActiveCall = true;
            } else if (cs.getDcState() == DriverCallIms.State.HOLDING) {
                foundHeldCall = true;
            }
        }

        if (!foundActiveCall) {
            //TODO: The only possible valid scenario might be all calls ending.
            //      Discussion required on how to handle this scenario.
        }

        // If there are no held calls, it implies both the calls got merged into the
        // conference, and hence ended.
        // NOTE: We cannot figure out genuine call drops here.
        if (!foundHeldCall) {
            mConferenceResult.heldCall = null;
        }

        // Call the merge success callback with the active call for 3 way conference
        // This active call is used to create transient session
        // For all other scenario pass null as the session would already exist
        if (mConferenceResult.shouldHaveTransientSession) {
            mMergeHostListener.callSessionMergeComplete(mConferenceResult.activeCall);
            // When conference host call is ended as part of merge into conference,
            // the ImsCallSession is not closed from telephony framework. Instead
            // the ImsCallSessiom is replaced with the transient conference call
            // session created. Hence we close the conference host session here
            // explicitly so that the onClosed() listeners can do the required cleanup.
            // For example CameraController can close camera if conf host session is the
            // camera owner.
            if (mConferenceResult.confHostCall != null &&
                    !mConferenceResult.confHostCall.isImsCallSessionAlive()) {
                Log.i(this, "processConferenceResult: close conf host session");
                mConferenceResult.confHostCall.close();
            }
        } else {
            mMergeHostListener.callSessionMergeComplete(null);
        }

        // Un-mute all call sessions still present.
        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                mCallList.entrySet().iterator(); it.hasNext();) {
            Map.Entry<String, ImsCallSessionImpl> e = it.next();

            ImsCallSessionImpl cs = e.getValue();
            cs.unMuteStateReporting();
            cs.updateMergeStatus(ImsCallUtils.SESSION_MERGE_COMPLETED);
            cs.mIsMergeHostSession = false;

        }

        // Reset all conference call flow variables.
        cleanupConferenceAttempt();
    }

    public void sendConferenceRequest(ImsCallSessionImpl hostSession) {
        Log.i(this, "Conference request being requested by session = " + hostSession);

        mConferenceResult = new ConferenceResult();
        mConferenceResult.confHostCall = hostSession;
        hostSession.mIsMergeHostSession = true;

        // Cache the host session's listener before sending the request. If the host
        // session ends as part of the conference call flow, its listener will still
        // be required to call the callSessionMerged callback. Simply having a reference
        // will not work as it will be set to null once the call session ends.
        mMergeHostListener = hostSession.mCallbackHandler;

        // Block the intermediate call state reporting for all the call sessions
        // till conference merge is in progress. Also, cache a snapshot of the call
        // sessions. This will be used while processing the final call session states
        // after the conference merge process ends.
        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                mCallList.entrySet().iterator(); it.hasNext();) {

            Map.Entry<String, ImsCallSessionImpl> e = it.next();
            ImsCallSessionImpl cs = e.getValue();

            cs.muteStateReporting();

            if (cs.getDcState() == DriverCallIms.State.ACTIVE) {
                mConferenceResult.activeCall = cs;
            } else if (cs.getDcState() == DriverCallIms.State.HOLDING) {
                mConferenceResult.heldCall = cs;
            }
            //If a call session is a conference host, we should not have a transient session
            if (cs.mIsConferenceHostSession) {
                Log.v(this,"Setting should have transient session to false ");
                mConferenceResult.shouldHaveTransientSession = false;
            }
            cs.updateMergeStatus(ImsCallUtils.SESSION_MERGE_STARTED);
        }

        // Send the conference request to service sub.
        mServiceSub.sendConferenceRequest(this);
    }

    /**
     * Method to provide the currently available calls related information
     * and explictly marking the individual call state as END.
     */
    public Object getCallsListToClear() {
        ArrayList<DriverCallIms> response = null;
        DriverCallIms dc;

        if (mCallList.size() > 0) {
            response = new ArrayList<DriverCallIms> ();

            synchronized(mCallList) {
                for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                        mCallList.entrySet().iterator(); it.hasNext();) {
                    Map.Entry<String, ImsCallSessionImpl> e = it.next();
                    ImsCallSessionImpl callSession = (ImsCallSessionImpl) e.getValue();
                    dc = new DriverCallIms();
                    dc.state = DriverCallIms.State.END;
                    dc.index = Integer.parseInt(callSession.getCallId());
                    dc.callDetails = new CallDetails();
                    dc.callDetails.call_type = callSession.getInternalCallType();
                    dc.callDetails.call_domain = callSession.getCallDomain();
                    dc.callFailCause = new ImsReasonInfo(ImsReasonInfo.
                            CODE_LOCAL_SERVICE_UNAVAILABLE, ImsReasonInfo.CODE_UNSPECIFIED, null);
                    response.add(dc);
                }
            }
            Collections.sort(response);
        }

        return response;
    }

    /**
     * Handle a list of calls updated by the IMS stack
     * @param dcList
     */
    public void handleCalls(ArrayList<DriverCallIms> dcList) {
        Map <String, DriverCallIms> dcMap = new HashMap<String, DriverCallIms>();
        // First pass is to look at every call in dc and update the Call Session List
        final boolean disableVideo = shallDisableVideo(dcList);
        for (int i = 0; dcList!= null && i < dcList.size(); i++) {
            ImsCallSessionImpl callSession = null;
            DriverCallIms dc = maybeDisableVideo(dcList.get(i), disableVideo);
            if (mPendingSessionList != null) {
                synchronized(mPendingSessionList) {
                    for (Iterator<ImsCallSessionImpl> it = mPendingSessionList.iterator();
                            it.hasNext();) {
                        ImsCallSessionImpl s = it.next();
                        // Match the DIALING call which is not already added to mCallList
                        if (dc.state == DriverCallIms.State.DIALING
                                && mCallList.get(Integer.toString(dc.index)) == null) {
                            // Add to call list as we now have call id, remove from
                            // temp list
                            Log.i(this, "Found match call session in temp list, s = " + s);
                            Log.i(this, "Index in call list is " + dc.index);
                            addCall(dc.index, s, false);
                            // Remove from mPendingSessionList
                            it.remove();
                        }
                    }
                }
            }
            synchronized(mCallList) {
                callSession = mCallList.get(Integer.toString(dc.index));
            }
            if (callSession != null){
                // Pending MO, active call updates
                // update for a existing call - no callID but MO number for a dial request
                // Call collision scenario
                callSession.updateCall(dc);
            } else {
                boolean isUnknown = false;
                if (dc.state == DriverCallIms.State.END) {
                    //This is an unknown call probably already cleaned up as part of SRVCC
                    //just ignore this dc and continue with the dc list
                    continue;
                }
                callSession = new ImsCallSessionImpl(dc, mCi, mContext, this, mIsVideoSupported,
                        mServiceSub.getPhoneId());
                callSession.addListener(mCallListener);
                callSession.updateFeatureCapabilities(mIsVideoSupported, mIsVoiceSupported);
                mServiceSub.onNewCall();
                // TODO: This functionality of informing ServiceSub of a new call needs to
                // be revisited from a design point of view.
                if (dc.isMT) {
                    Log.i(this, "MT Call creating a new call session");
                    reportIncomingCall(callSession, dc.index, false);
                } else if (dc.isMpty) {
                    Log.i(this, "Conference Call creating a new call session");
                    isUnknown = true;
                    //Get the old session
                    synchronized(mCallList) {
                        for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                                mCallList.entrySet().iterator(); it.hasNext();) {
                            Map.Entry<String, ImsCallSessionImpl> e = it.next();
                            ImsCallSessionImpl oldSession = (ImsCallSessionImpl) e.getValue();
                            if ( oldSession.mIsMergeHostSession ) {
                                Log.i(this, "Set New Session as Id " + callSession.getCallId());
                                callSession.setConfInfo(oldSession.getConfInfo());
                                oldSession.setNewSession(callSession);
                                oldSession.reportNewConferenceCallSession(callSession);
                                // This is new conference call created as part of 3 way merge.
                                isUnknown = false;
                                break;
                            } else if (oldSession.getMergeStatus() ==
                                    ImsCallUtils.SESSION_MERGE_STARTED) {
                                isUnknown = false;
                            }
                        }
                    }
                    if (isUnknown) {
                        Log.i(this, "Phantom conference call Scenario");
                    } else {
                        addCall(dc.index, callSession, true);
                    }
                    callSession.updateConfSession(dc);
                } else if (dc.state != DriverCallIms.State.END) {
                   Log.i(this, "MO phantom Call Scenario. State = " + dc.state);
                   isUnknown = true;
                }
                if (isUnknown) {
                    reportIncomingCall(callSession, dc.index, true);
                }
            }
            // If state is not END then add call to list of active calls
            if (dc.state != DriverCallIms.State.END) {
                dcMap.put(Integer.toString(dc.index), dc);
            }
            if (dc.state == DriverCallIms.State.END) {
                // broadcast call composer call end
                // get mapped value from Integer.toString(dc.index)
                int token = mCallComposerTokenList.get(dc.index, CallComposerInfo.INVALID_TOKEN);
                if (token != CallComposerInfo.INVALID_TOKEN) {
                    broadcastCallComposerIntent(token);
                    mCallComposerTokenList.delete(dc.index);
                }
            }
        }

        // Second pass to check if all Call Sessions are still active, dc will not contain
        // a disconnected call in the dc List, remove any call sessions that are not present
        // in dc List.
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                if (dcMap.get(e.getKey()) == null) { // Call Dropped!
                    // CallStartFailed/CallTerminated are triggered during updateCall
                    // when call state is END. Also callsession close is triggered by the
                    // component that creates the session.
                    // Here just remove the session from tracker.
                    Log.i(this, "handleCalls: removing dropped/ended call id:" +
                            e.getKey());
                    it.remove();
                    notifyCallRemoved(e.getValue());
                }
            }
        }
    }

    /**
     * Handle USSD message received by the IMS stack
     * @param profile that is passed to AOSP.
     * @param info that contains USSD related information.
     */
    public void onUssdMessageReceived(UssdInfo info) {
        //In case of empty message from lower layer, treat
        //it like an error case.
        boolean isErrorCase = info.getMessage().isEmpty();

        //As in this case we have an ongoing USSD session,
        //we prevent creation of another.
        if (!mUssdList.isEmpty() && !isErrorCase) {
            Log.d(this, "onUssdMessageReceived: ongoing USSD session");
            return;
        }

        if (isErrorCase) {
            Log.d(this, "onUssdMessageReceived: received empty message");
            return;
        }

        ImsCallProfile profile = new ImsCallProfile(ImsCallProfile.SERVICE_TYPE_NORMAL,
                                                    ImsCallProfile.CALL_TYPE_VOICE);
        mServiceSub.notifyIncomingCall(createCallSession(profile, info),
                createIncomingCallBundle(true, false));
    }

    private boolean shallDisableVideo(ArrayList<DriverCallIms> dcList) {
        if (QtiImsExtUtils.canHoldVideoCall(getPhoneId(), mContext)) {
            return false;
        }
        boolean hasActiveVoiceCall = false;
        boolean hasHeldVoiceCall = false;
        boolean isMergeStarted = false;
        ImsCallSessionImpl callSession = null;

        for (DriverCallIms dc : dcList ) {
            hasActiveVoiceCall |= dc.state == DriverCallIms.State.ACTIVE &&
                    ImsCallUtils.isVoiceCall(dc.callDetails.call_type);
            hasHeldVoiceCall |= dc.state == DriverCallIms.State.HOLDING &&
                    ImsCallUtils.isVoiceCall(dc.callDetails.call_type);
            synchronized(mCallList) {
                callSession = mCallList.get(Integer.toString(dc.index));
                isMergeStarted |= (callSession != null &&
                        callSession.getMergeStatus() == ImsCallUtils.SESSION_MERGE_STARTED);
            }
        }

        return (hasActiveVoiceCall && hasHeldVoiceCall) || isMergeStarted;
    }

    private DriverCallIms maybeDisableVideo(DriverCallIms dc, final boolean disableVideo) {
        if (!disableVideo || dc == null || dc.callDetails == null ||
                dc.callDetails.localAbility == null) {
            return dc;
        }

        boolean isVideoDisabled = !ImsCallSessionImpl.isServiceAllowed(
                CallDetails.CALL_TYPE_VT, dc.callDetails.localAbility);
        if (isVideoDisabled == disableVideo) {
            return dc;
        }

        for (ServiceStatus srv : dc.callDetails.localAbility) {
            if (srv.type == CallDetails.CALL_TYPE_VT) {
                srv.status = ServiceStatus.STATUS_DISABLED;
            }
        }
        return dc;
    }

    public void onCallClosed(QImsSessionBase session) {
        ImsCallSessionImpl callSession = (ImsCallSessionImpl)session;
        if (callSession.getState() != ImsCallSessionImplBase.State.TERMINATED) {
            Log.i(this, "onCallClosed: call session not in terminated state. Ignore.");
            return;
        }

        // Remove the terminated session if present in pending list.
        // If a pending session is closed by AOSP framework due to dial error,
        // we remove the session from the list here.
        synchronized(mPendingSessionList) {
            if (mPendingSessionList.remove(callSession)) {
                Log.i(this, "Removing pending session on close " + callSession);
                notifyCallRemoved(callSession);
            }
        }
    }

    private void removeAndNotifyCallSessions(
            Collection <ImsCallSessionImpl> list, String listName) {
        if (list == null) {
            return;
        }
        for (Iterator<ImsCallSessionImpl> it = list.iterator(); it.hasNext(); ) {
            ImsCallSessionImpl session = it.next();
            it.remove();
            Log.i(this, "Removing session: " + session + " from " + listName);
            notifyCallRemoved(session);
        }
    }

    private void removeCallSessionsAfterSrvcc() {
        Log.i(this, "removeCallSessionsAfterSrvcc");
        // For all the states after DIALING state is reported by RIL
        synchronized(mCallList) {
            removeAndNotifyCallSessions(mCallList.values(), "call list");
        }

        // When there is a pending session waiting for a call object with DIALING state
        synchronized(mPendingSessionList) {
            removeAndNotifyCallSessions(mPendingSessionList, "pending list");
        }
    }

    public void onUssdClosed(QImsSessionBase session) {
        Log.i(this, "onUssdClosed for session " + session);
        if (mUssdList != null) {
            synchronized(mUssdList) {
                if (mUssdList.remove((ImsUssdSessionImpl)session)) {
                    Log.i(this, "Removing session on close " + session);
                    notifyUssdRemoved((ImsUssdSessionImpl)session);
                }
            }
        }
    }

    public void handleModifyCallRequest(CallModify cm) {
        if (cm != null) {
            ImsCallSessionImpl callSession = null;
            synchronized(mCallList) {
                callSession = mCallList.get(Integer.toString(cm.call_index));
            }
            if (callSession != null) {
                callSession.onReceivedModifyCall(cm);
            } else {
                Log.e(this,"handleModifyCallRequest Error: callSession is null");
            }
        } else {
            Log.e(this,"handleModifyCallRequest Error: Null Call Modify request ");
        }
    }

    /**
     * Create a call session
     * @param profile - ImsCallProfile associated with this call
     * @param listener - listner for the call session
     * @return IImsCallSession object or null on failure
     */
    public ImsCallSessionImplBase createCallSession(ImsCallProfile profile,
            UssdInfo info) {
        QImsSessionBase session =
            ((profile.getCallExtraInt(ImsCallProfile.EXTRA_DIALSTRING) ==
                      ImsCallProfile.DIALSTRING_USSD) || (info != null)) ?
            createUssdSession(profile, info) :
            createCallSession(profile);
        return (ImsCallSessionImplBase)session;
    }

    private QImsSessionBase createCallSession(ImsCallProfile profile) {
        ImsCallSessionImpl session = new ImsCallSessionImpl(profile, mCi, mContext,
                this, mIsVideoSupported, mServiceSub.getPhoneId());
        session.addListener(mCallListener);
        session.updateFeatureCapabilities(mIsVideoSupported, mIsVoiceSupported);
        mServiceSub.onNewCall();
        synchronized(mPendingSessionList) {
            mPendingSessionList.add(session);
            notifyCallAdded(session);
        }
        return session;
    }

    private QImsSessionBase createUssdSession(ImsCallProfile profile, UssdInfo info) {
        ImsUssdSessionImpl session = new ImsUssdSessionImpl(profile, mContext,
                mCi, mServiceSub.getPhoneId(), this, info);
        session.addListener(mUssdListener);
        session.updateFeatureCapabilities(mIsVideoSupported, mIsVoiceSupported);
        mServiceSub.onNewCall();
        synchronized(mUssdList) {
            mUssdList.add(session);
            notifyUssdAdded(session);
        }
        return session;
    }

    /**
     * Get a call session associated with the callId
     * @param callId
     * @return IImsCallSession object
     */
    public ImsCallSessionImpl getCallSession(String callId) {
        ImsCallSessionImpl session = null;
        synchronized(mCallList) {
            session = mCallList.get(callId);
        }
        return session;
    }

    /**
     * Handle the call state changes for incoming (MT) Hold/Resume as part of
     * the UNSOL_SUPP_SVC_NOTIFICATION message.
     * TODO: Handle other supp_svc info here?
     *
     * @param info
     */
    public void handleSuppSvcUnsol(SuppNotifyInfo info) {
        Log.i(this, "handleSuppSvcUnsol connId= " + info.getConnId());
        synchronized (mCallList) {
            ImsCallSessionImpl callSession =
                    callSession = mCallList.get(Integer.toString(info.getConnId()));
            if (callSession != null) {
                boolean startOnHoldLocalTone = false;
                final String forwardedCallHistory = info.getHistoryInfo();
                String[] callHistory = null;
                if (forwardedCallHistory != null && !forwardedCallHistory.isEmpty() ) {
                    callHistory = forwardedCallHistory.split("\r\n");
                }
                ImsSuppServiceNotification suppServiceInfo = new ImsSuppServiceNotification(
                        info.getNotificationType(), info.getCode(), info.getIndex(),
                        info.getNotificationType(), info.getNumber(), callHistory);

                if (info.hasHoldTone()) {
                    startOnHoldLocalTone = info.getHoldTone();
                }
                Log.i(this, "handleSuppSvcUnsol suppNotification= " + suppServiceInfo +
                      " startOnHoldLocalTone = " + startOnHoldLocalTone);
                callSession.updateSuppServiceInfo(suppServiceInfo, startOnHoldLocalTone);
            } else {
                Log.e(this, "No call session found for number: "/* + info.getNumber()*/);
            }
        }
    }

    /**
     * Handles Conference refresh Info notified through UNSOL_REFRESH_CONF_INFO message
     * @param ar - the AsyncResult object that contains the refresh Info information
     */
    public void handleRefreshConfInfo(ConfInfo confRefreshInfo) {
        byte[] confInfoBytes = null;
        int state = ConfInfo.INVALID;
        ImsCallSessionImpl callSession = null;
        if (confRefreshInfo != null) {
            confInfoBytes = confRefreshInfo.getConfInfoUri();
            if (confInfoBytes != null
                    && confInfoBytes.length >= 1) {
                final int confCallState = confRefreshInfo.getConfCallState();
                /*
                 * defaulting to foreground call for backward compatibility before call state
                 * was added to the interface
                 */
                state = (confCallState != ConfInfo.INVALID) ?
                        confCallState : ConfInfo.FOREGROUND;
                callSession = getCallSessionWithMptyBitSet(state);
            }
        }

        Log.i(this, "handleRefreshConfInfo: confCallState = " + state + ", callSession = "
                + callSession /*+ ", confInfoBytes: " + confInfoBytes*/);
        /*
         * UE subscribes for conference xml as soon as it establishes session with conference
         * server.Multiparty bit will be updated only through Get_current_calls after all the
         * participants are merged to the call. So refresh info can be received during the interval
         * in which the conference request is sent and before the conference call reflects in the
         * Get_current-calls
         */
        if (confInfoBytes != null && callSession != null) {
            Log.i(this, "Update UI for Conference");
            callSession.notifyConfInfo(confInfoBytes);
        } else {
            Log.e(this, "No CallSession with Multiparty bit set is found. Some Error!!!");
        }
    }

    /**
     * Handle the TTY mode changes as part of the UNSOL_TTY_NOTIFICATION message.
     *
     * @param mode the mode informed via the indication
     */
    public void handleTtyModeChangeUnsol(int mode) {
        ImsCallSessionImpl callSession = null;

        // Check if any call session is active.
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                // Call active
                if (e.getValue().getState() == ImsCallSessionImplBase.State.ESTABLISHED) {
                    callSession = (ImsCallSessionImpl) e.getValue();
                    callSession.notifyTtyModeChange(mode);
                    break;
                }
            }
        }

        if (callSession == null) {
            Log.e(this, "No Active call session found for TTY mode change");
        }
    }

    private ImsCallSessionImpl getCallSessionWithMptyBitSet(int state) {
        synchronized(mCallList) {

            // Use the cached active session as during conference one of the sessions
            // will receive END as part of merge
            if ((mConferenceResult != null) && mConferenceResult.shouldHaveTransientSession) {
                Log.i(this, "getCallSessionWithMptyBitSet session = "
                        + mConferenceResult.activeCall);
                return mConferenceResult.activeCall;
            }

            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it = mCallList.entrySet()
                    .iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                ImsCallSessionImpl session = (ImsCallSessionImpl) e.getValue();
                DriverCallIms.State dcState = session.getDriverCallState();
                Log.i(this, "getCallSessionWithMptyBitSet:: ImsCallSession state = "
                        + session.getState() + ", isMultiparty = " + session.isMultiparty());

                if (session.isMultiparty() == true) {
                    Log.i(this, "ImsCallSession found with Multiparty bit set");
                    if ((dcState == DriverCallIms.State.DIALING ||
                            dcState == DriverCallIms.State.ALERTING ||
                            dcState == DriverCallIms.State.ACTIVE)
                            && (state == ConfInfo.FOREGROUND)) {
                        Log.i(this, "Foreground Conference callSession found");
                        return session;
                    } else if ((dcState == DriverCallIms.State.HOLDING)
                            && (state == ConfInfo.BACKGROUND)) {
                        Log.i(this, "Background Conference callSession found");
                        return session;
                    } else if ((dcState == DriverCallIms.State.INCOMING ||
                            dcState == DriverCallIms.State.WAITING)
                            && (state == ConfInfo.RINGING)) {
                        Log.i(this, "Ringing Conference callSession found");
                        return session;
                    }
                }
            }

            // Deprecated code
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it = mCallList.entrySet()
                    .iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                ImsCallSessionImpl session = (ImsCallSessionImpl) e.getValue();
                if (session.isConfInProgress()) {
                    if (state == ConfInfo.FOREGROUND) {
                        Log.i(this, "Foreground ImsCallSession found during Conference setup");
                        return session;
                    }
                } else {
                    if (state == ConfInfo.BACKGROUND) {
                        Log.i(this, "Background ImsCallSession found");
                    }
                }
            }
            return null;
        }
    }

    /**
     * Gets list of call sessions that are in the given state.
     * @param state State of the call.
     * @return List of call session objects that have {@code state}
     */
    public List<ImsCallSessionImpl> getCallSessionByState(DriverCallIms.State state) {
        List<ImsCallSessionImpl> sessionList = new ArrayList<ImsCallSessionImpl>();
        if (state == null) return sessionList;

        synchronized(mPendingSessionList) {
            for (ImsCallSessionImpl session : mPendingSessionList) {
                if (session.getInternalState() == state) {
                    sessionList.add(session);
                }
            }
        }

        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                if (session.getInternalState() == state) {
                    sessionList.add(session);
                }
            }
        }
        return sessionList;
    }

    /**
     * Gets a call session with give media id.
     * @param mediaId Media id of the session to be searched.
     * @return Call session with {@code mediaId}
     */
    public ImsCallSessionImpl findSessionByMediaId(int mediaId) {
        synchronized(mCallList) {
            for (Map.Entry<String, ImsCallSessionImpl> e : mCallList.entrySet()) {
                final ImsCallSessionImpl session = e.getValue();
                if (session.getMediaId() == mediaId) {
                    return session;
                }
            }
        }
        return null;
    }

    // For SRVCC indication source and target RATs will be invalid.
    private static boolean isSrvcc(HoInfo hoInfo) {
        return hoInfo.getSrcTech() == RadioTech.RADIO_TECH_INVALID &&
                hoInfo.getTargetTech() == RadioTech.RADIO_TECH_INVALID;
    }

    public void handleHandover(HoInfo handover) {
        Log.i(this, "in handleHandover");
        if (isSrvcc(handover)) {
            if (handover.getType() == HoInfo.COMPLETE_SUCCESS) {
                removeCallSessionsAfterSrvcc();
            }
            // srvcc indications are not notified to ImsCallSessionImpl
            return;
        }

        Log.i(this, "hoState: " + handover.getType() +
                " srcTech: " + handover.getSrcTech() +
                " tarTech: " + handover.getTargetTech() +
                " extraType: " + handover.getExtraType() +
                " extraInfo: " + handover.getExtraInfo() +
                " ErrorCode: " + handover.getErrorCode() +
                " errorMessage: " + handover.getErrorMessage());

        boolean showHandoverToast = false;
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it = mCallList.entrySet()
                    .iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                ImsCallSessionImpl callSession = (ImsCallSessionImpl) e.getValue();
                if (callSession == null) {
                    Log.i(this, "handleHandover: null callsession. Key = " + e.getKey());
                    continue;
                }
                callSession.handleHandover(handover.getType(), handover.getSrcTech(),
                        handover.getTargetTech(), handover.getExtraType(),
                        handover.getExtraInfo(),
                        handover.getErrorCode(), handover.getErrorMessage());
                if (!showHandoverToast && callSession.isInCall()) {
                    showHandoverToast = true;
                }
            }
        }

        if (showHandoverToast &&
                handover.getType() == HoInfo.COMPLETE_SUCCESS &&
                (handover.getSrcTech() == RadioTech.RADIO_TECH_WIFI ||
                 handover.getSrcTech() == RadioTech.RADIO_TECH_IWLAN) &&
                 (handover.getTargetTech() == RadioTech.RADIO_TECH_LTE ||
                   handover.getTargetTech() == RadioTech.RADIO_TECH_NR5G)) {
            Log.i(this, "Switching to LTE network for better quality");
        }
    }

    /**
     * Registers call list listener.
     * @param listener Listener to registered
     * @see ICallListListener
     */
    /* package */void addListener(ICallListListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("addListener error: listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        if (!mCallListListeners.contains(listener)) {
            mCallListListeners.add(listener);
        } else {
            Log.i(this,"addListener: Listener already added, " + listener);
        }
    }

    /**
     * Unregisters call list listener. Note: Only {@code ImsServiceClass.MMTEL} is supported.
     * @param listener Listener to unregistered
     * @see ICallListListener
     */
    /* package */void removeListener(ICallListListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("removeListener error: listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        if (mCallListListeners.contains(listener)) {
            mCallListListeners.remove(listener);
        } else {
            Log.e(this,"removeListener error: Listener not found, " + listener);
        }
    }

    // TODO Create CallList class and hide listeners, registration, notification in that class.
    private void notifyCallAdded(ImsCallSessionImpl session) {
        for (ICallListListener listener : mCallListListeners) {
            listener.onSessionAdded(session);
        }
    }

    private void addCall(Integer id, ImsCallSessionImpl session, boolean notify) {
        synchronized (mCallList) {
            mCallList.put(id.toString(), session);
        }
        if (notify) {
            notifyCallAdded(session);
        }
    }

    private void notifyCallRemoved(ImsCallSessionImpl session) {
        for (ICallListListener listener : mCallListListeners) {
            listener.onSessionRemoved(session);
        }
    }

    private void notifyUssdAdded(ImsUssdSessionImpl session) {
        mCallListListeners.forEach(listener -> {
            listener.onSessionAdded(session);
        });
    }

    private void notifyUssdRemoved(ImsUssdSessionImpl session) {
        mCallListListeners.forEach(listener -> {
            listener.onSessionRemoved(session);
        });
    }

    public void reportIncomingCall(ImsCallSessionImpl session, int index, boolean isUnknown) {
        Log.d(this, "reportIncomingCall :: session=" + session + " index=" + index
                + " isUnknown=" + isUnknown);
        mServiceSub.notifyIncomingCall((ImsCallSessionImplBase)session,
                createIncomingCallBundle(false, isUnknown));
        addCall(index, session, true);
    }

    /**
     * Update the WiFi quality indication to call sessions
     *
     * @param quality the quality value received via the indication
     */
    public void updateVoWiFiCallQuality(int quality) {
        ImsCallSessionImpl callSession = null;

        /* Update the existing call sessions with quality value */
        synchronized(mCallList) {
            for (Iterator<Map.Entry<String, ImsCallSessionImpl>> it =
                    mCallList.entrySet().iterator(); it.hasNext();) {
                Map.Entry<String, ImsCallSessionImpl> e = it.next();
                callSession = (ImsCallSessionImpl) e.getValue();
                callSession.updateVoWiFiCallQuality(quality);
            }
        }
    }

    public boolean isVoiceSupportedOverWifi() {
        return mServiceSub.isVoiceSupportedOverWifi();
    }

    public boolean isVideoSupportedOverWifi() {
        return mServiceSub.isVideoSupportedOverWifi();
    }

    public boolean isUssdSupported() {
        return mServiceSub.isUssdSupported();
    }

    public int getPhoneId() {
        return mServiceSub.getPhoneId();
    }

    @VisibleForTesting
    public SparseIntArray getCallComposerTokenList() {
        return mCallComposerTokenList;
    }

    private static int getToken() {
        sToken = (sToken == Integer.MAX_VALUE) ? 0 : sToken + 1;
        return sToken;
    }

    public void onCallComposerDataAvailable(int callId, Bundle ccExtra) {
        int token = getToken();
        ccExtra.putInt(QtiCallConstants.EXTRA_CALL_COMPOSER_TOKEN, token);
        mCallComposerTokenList.put(callId, token);
        broadcastCallComposerIntent(ccExtra, token);
    }

    // utility function to broadcast intent when call composer call ends
    private void broadcastCallComposerIntent(int token) {
        broadcastCallComposerIntent(null, token);
    }

    private void broadcastCallComposerIntent(Bundle ccExtra, int token) {
        Intent intent = new Intent(QtiCallConstants.ACTION_CALL_COMPOSER_INFO);
        intent.putExtra(QtiCallConstants.EXTRA_CALL_COMPOSER_TOKEN, token);
        if (ccExtra != null) {
            intent.putExtra(QtiCallConstants.EXTRA_CALL_COMPOSER_INFO, ccExtra);
        } else {
            Log.d(this, "broadcastCallComposerIntent for end state");
            intent.putExtra(QtiCallConstants.EXTRA_CALL_COMPOSER_CALL_ENDED, true);
        }

        mContext.sendBroadcast(intent, "com.qti.permission.RECEIVE_CALLCOMPOSER_INFO");
    }
}
