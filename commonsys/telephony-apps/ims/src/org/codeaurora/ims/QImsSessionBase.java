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
import android.telephony.ims.stub.ImsCallSessionImplBase;
import android.telephony.ims.ImsCallProfile;
import android.telephony.ims.ImsCallSessionListener;
import android.telephony.ims.ImsStreamMediaProfile;

import com.qualcomm.ims.utils.Log;

import java.util.concurrent.CopyOnWriteArrayList;
import java.util.List;

public abstract class QImsSessionBase extends ImsCallSessionImplBase {

    protected ImsSenderRxr mCi;
    protected Context mContext;
    protected ImsServiceClassTracker mTracker = null;
    protected int mPhoneId = QtiCallConstants.INVALID_PHONE_ID;
    public ImsCallSessionCallbackHandler mCallbackHandler;
    protected int mState = ImsCallSessionImplBase.State.IDLE;
    protected List<Listener> mListeners = new CopyOnWriteArrayList<>();

    public interface Listener {
        void onDisconnected(QImsSessionBase session);
        void onClosed(QImsSessionBase session);
        void onUnsolCallModify(QImsSessionBase session, CallModify callModify);
        void onCallModifyInitiated(QImsSessionBase session, CallModify callModify);
        void onCallTypeChanging(QImsSessionBase session, int newCallType);
    }

    public static class ListenerBase implements Listener {
        @Override
        public void onDisconnected(QImsSessionBase session) {}

        @Override
        public void onClosed(QImsSessionBase session) {}

        @Override
        public void onUnsolCallModify(QImsSessionBase session, CallModify callModify) {}

        @Override
        public void onCallModifyInitiated(QImsSessionBase session, CallModify callModify) {}

        @Override
        public void onCallTypeChanging(QImsSessionBase session, int newCallType) {}
    }

    protected QImsSessionBase(Context context, ImsSenderRxr senderRxr, int phoneId,
                               ImsServiceClassTracker tracker) {
        this(context, senderRxr, phoneId, tracker, new ImsCallSessionCallbackHandler());
    }

    protected QImsSessionBase(Context context, ImsSenderRxr senderRxr, int phoneId,
                                ImsServiceClassTracker tracker,
                                ImsCallSessionCallbackHandler handler) {
        mCi = senderRxr;
        mContext = context;
        mTracker = tracker;
        mPhoneId = phoneId;
        mCallbackHandler = handler;
    }

    public void updateFeatureCapabilities(boolean isVideo, boolean isVoice) {
        //no-op
    }

    /**
     * Sets the listener to listen to the session events. A {@link IImsCallSession}
     * can only hold one listener at a time. Subsequent calls to this method
     * override the previous listener.
     *
     * @param listener to listen to the session events of this object
     */
    @Override
    public void setListener(ImsCallSessionListener listener) {
        if (!isSessionValid()) return;
        mCallbackHandler.mListener = listener;
    }

    /**
     * Registers a listener from within IMS Service vendor, for updates. This is
     * not to be confused with the 'setListener' method in this class, that
     * is used by IMS Service clients to set the call session callback
     * listener. The 'setListener' listener is set by Framework.
     * @param listener Listener to registered
     * @see ImsCallSessionImpl#Listener
     */
    //TODO: Have seperate Call and USSD listeneres
    public void addListener(Listener listener) {
        if (!isSessionValid()) return;

        if (listener == null) {
            throw new IllegalArgumentException("listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        synchronized (mListeners) {
            if (!mListeners.contains(listener)) {
                mListeners.add(listener);
            } else {
                Log.e(this, "Duplicate listener, " + listener);
            }
        }
    }

    /**
     * Unregisters listeners from within IMS Service vendor modules.
     * @param listener Listener to unregistered
     * @see ImsCallSessionImpl#Listener
     */
    public void removeListener(Listener listener) {
        if (!isSessionValid()) return;

        if (listener == null) {
            throw new IllegalArgumentException("listener is null.");
        }

        // Note: This will do linear search, O(N).
        // This is acceptable since arrays size is small.
        synchronized (mListeners) {
            if (mListeners.contains(listener)) {
                mListeners.remove(listener);
            } else {
                Log.e(this, "Listener not found, " + listener);
            }
        }
    }

    protected void notifySessionDisconnected() {
        synchronized (mListeners) {
            for( Listener l : mListeners) {
                l.onDisconnected(this);
            }
        }
    }

    protected void notifySessionClosed() {
        synchronized (mListeners) {
            for( Listener l : mListeners) {
                l.onClosed(this);
            }
        }
    }

    public void notifyUnsolCallModify(CallModify callModify) {
        if (!isSessionValid()) return;

        synchronized (mListeners) {
            for (Listener l : mListeners) {
                l.onUnsolCallModify(this, callModify);
            }
        }
    }

    public void notifyCallModifyInitiate(CallModify callModify) {
        if (!isSessionValid()) return;
        synchronized (mListeners) {
            for (Listener l : mListeners) {
                l.onCallModifyInitiated(this, callModify);
            }
        }
    }

    protected void notifyCallTypeChanging(int callType) {
        if (!isSessionValid()) return;
        synchronized (mListeners) {
            for (Listener l : mListeners) {
                l.onCallTypeChanging(this, callType);
            }
        }
    }

    public boolean isSessionValid() {
        boolean isValid = (mState != ImsCallSessionImplBase.State.INVALID);
        if (!isValid) {
            Log.e(this, "Session is closed");
        }
        return isValid;
    }
}
