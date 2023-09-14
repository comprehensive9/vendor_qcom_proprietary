/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (c) 2017 The Android Open Source Project
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

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.telephony.ims.ImsCallProfile;
import android.telephony.ims.ImsReasonInfo;
import android.telephony.ims.ImsStreamMediaProfile;
import android.telephony.ims.stub.ImsCallSessionImplBase;

import com.qualcomm.ims.utils.Log;
import org.codeaurora.telephony.utils.AsyncResult;

public class ImsUssdSessionImpl extends QImsSessionBase {

    private static final int EVENT_USSD_SENT_RESPONSE = 1;
    private static final int EVENT_CANCEL_USSD = 2;
    private static final int EVENT_USSD_MESSAGE_RECEIVED = 3;
    private static final int EVENT_CLOSE_USSD_SESSION = 4;

    private Handler mHandler = new ImsSessionHandler();
    private UssdInfo mUssdInfo = null;
    private ImsCallProfile mCallProfile = null;

    public ImsUssdSessionImpl(ImsCallProfile profile, Context context,
                        ImsSenderRxr senderRxr, int phoneId,
                        ImsServiceClassTracker tracker, UssdInfo info) {
        super(context, senderRxr, phoneId, tracker);
        mUssdInfo = info;
        mCallProfile = profile;
        mCi.registerForUssdInfo(mHandler, EVENT_USSD_MESSAGE_RECEIVED, null);
    }

    /**
     * Initiates an IMS USSD session with the specified target and call profile. The session
     * listener is called back upon defined session events.
     * @param callee dialed string to make USSD request
     * @param profile call profile to make USSD with the specified service type, call type and
     *            media information
     * @see Listener#callSessionStarted, Listener#callSessionStartFailed
     */
    public void start(String callee, ImsCallProfile profile) {
        if (!isSessionValid()) return;

        if (!mTracker.isUssdSupported()) {
            Log.i (this, "start: USSD is not supported");
            mCallbackHandler.callSessionInitiatedFailed(new ImsReasonInfo(
                    ImsReasonInfo.CODE_LOCAL_SERVICE_UNAVAILABLE,
                    ImsReasonInfo.CODE_UNSPECIFIED, "USSD not supported"));
            return;
        }
        mState = ImsCallSessionImplBase.State.INITIATED;
        mCi.sendUssd(callee, mHandler.obtainMessage(EVENT_USSD_SENT_RESPONSE, this));
    }

    /**
     * Accepts an incoming USSD session.
     *
     * @param callType call type specified in {@link ImsCallProfile} to be answered
     * @param profile stream media profile {@link ImsStreamMediaProfile} to be answered
     * @see Listener#callSessionStarted
     */
    public void accept(int callType, ImsStreamMediaProfile profile) {
        if (!isSessionValid()) return;

        if (mUssdInfo != null) {
            mState = ImsCallSessionImplBase.State.ESTABLISHED;
            mCallbackHandler.callSessionUssdMessageReceived(mUssdInfo.getType(),
                    mUssdInfo.getMessage());
        }
    }

    /**
     * Terminates a USSD session.
     *
     * @see Listener#callSessionTerminated
     */
    public void terminate(int reason) {
        if (!isSessionValid()) return;

        mCi.cancelPendingUssd(mHandler.obtainMessage(EVENT_CANCEL_USSD, this));
    }

    /**
     * Closes the object. This object is not usable after being closed.
     * This function is called when onCallTerminated is triggered from client side.
     */
    public void close() {
        Log.i(this, "Close!");
        if (mHandler != null) {
            mHandler.obtainMessage(EVENT_CLOSE_USSD_SESSION).sendToTarget();
        }
    }

    /**
     * Sends an USSD message.
     *
     * @param ussdMessage USSD message to send
     */
    public void sendUssd(String ussdMessage) {
        if (!isSessionValid()) return;

        mCi.sendUssd(convertNullToEmptyString(ussdMessage),
                mHandler.obtainMessage(EVENT_USSD_SENT_RESPONSE, this));
    }

    private static String convertNullToEmptyString(String msg) {
        return msg != null ? msg : "";
    }

    //Handler for events tracking requests sent to ImsSenderRxr
    private class ImsSessionHandler extends Handler {
        ImsSessionHandler() {
            super(Looper.getMainLooper());
        }

        @Override
        public void handleMessage(Message msg) {
            Log.i (this, "Message received: what = " + msg.what);

            AsyncResult ar;

            switch (msg.what) {
                case EVENT_CLOSE_USSD_SESSION:
                    doClose();
                    break;
                case EVENT_USSD_SENT_RESPONSE:
                    OnUssdSentResponse((AsyncResult) msg.obj);
                    break;
                case EVENT_CANCEL_USSD:
                    onCancelUssd((AsyncResult) msg.obj);
                    break;
                case EVENT_USSD_MESSAGE_RECEIVED:
                    onUssdMessageReceived((AsyncResult) msg.obj);
                    break;
            }
        }
    }

    private void OnUssdSentResponse(AsyncResult ar) {
        if (ar == null) {
            Log.e(this, "USSD: response result is null");
            return;
        }
        if (mState == ImsCallSessionImplBase.State.ESTABLISHED) {
            Log.e(this, "USSD: ongoing USSD session");
            if (ar.exception != null) {
                mState = ImsCallSessionImplBase.State.TERMINATED;
                mCallbackHandler.callSessionTerminated(new ImsReasonInfo(
                        ImsReasonInfo.CODE_SIP_SERVER_ERROR,
                        ImsReasonInfo.CODE_UNSPECIFIED, "USSD terminated"));
            }
            return;
        }
        if (ar.exception != null) {
            Log.e(this, "USSD: response exception: " + ar.exception);
            mState = ImsCallSessionImplBase.State.TERMINATED;
            ImsReasonInfo sipErrorInfo = ImsCallUtils.getImsReasonInfo(ar);
            mCallbackHandler.callSessionInitiatedFailed(sipErrorInfo);
        } else {
            Log.d(this, "USSD: response received");
            mState = ImsCallSessionImplBase.State.ESTABLISHED;
            mCallbackHandler.callSessionInitiated(mCallProfile);
        }
    }

    private void onCancelUssd(AsyncResult ar) {
        if (ar == null) {
            Log.e(this, "USSD: response result is null");
            return;
        }
        if (ar.exception != null) {
            Log.e(this, "Cancel USSD: response exception: " + ar.exception);
            return;
        }
        Log.d(this, "Cancel USSD: response received");
        mState = ImsCallSessionImplBase.State.TERMINATED;
        mCallbackHandler.callSessionTerminated(new ImsReasonInfo(
                ImsReasonInfo.CODE_USER_TERMINATED,
                ImsReasonInfo.CODE_UNSPECIFIED, "USSD Cancelled"));
    }

    private void onUssdMessageReceived(AsyncResult ar) {
        if (ar == null) {
            Log.e(this, "USSD: message result is null");
            return;
        }
        if (ar.exception != null) {
            Log.e(this, "USSD: message exception: " + ar.exception);
            //In case of exception, terminate ongoing USSD session.
            if (mState == ImsCallSessionImplBase.State.ESTABLISHED ||
                mState == ImsCallSessionImplBase.State.INITIATED) {
                mState = ImsCallSessionImplBase.State.TERMINATED;
                mCallbackHandler.callSessionTerminated(new ImsReasonInfo(
                        ImsReasonInfo.CODE_SIP_SERVER_ERROR,
                        ImsReasonInfo.CODE_UNSPECIFIED, "USSD Network Error"));
            }
        } else if (mState == ImsCallSessionImplBase.State.ESTABLISHED ||
                   mState == ImsCallSessionImplBase.State.INITIATED) {
            Log.d(this, "USSD: message received");
            final UssdInfo info = (UssdInfo) ar.result;
            int ussdMode = info.getType();
            String ussdMsg = info.getMessage();
            mCallbackHandler.callSessionUssdMessageReceived(ussdMode, ussdMsg);
            //Close USSD session.
            if (ussdMode != ImsPhoneCommandsInterface.USSD_MODE_REQUEST ||
                ussdMsg.isEmpty()) {
                Log.i(this, "USSD: session closed mode: " + ussdMode);
                doClose();
            }
        }
    }

    private void doClose() {
        Log.i(this, "doClose!");
        if (mCallbackHandler != null) {
            mCallbackHandler.dispose();
            mCallbackHandler = null;
        }
        if (mHandler != null) {
            mCi.unregisterForUssdInfo(mHandler);
            mHandler = null;
        }
        notifySessionClosed();
        synchronized (mListeners) {
            mListeners.clear();
        }
        mCi = null;
        mCallProfile = null;
        mPhoneId = QtiCallConstants.INVALID_PHONE_ID;
        mState = ImsCallSessionImplBase.State.INVALID;
    }
}
