/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package org.codeaurora.ims;

import android.os.Bundle;
import android.telephony.ims.ImsCallProfile;
import android.telephony.ims.ImsStreamMediaProfile;
import java.util.List;

/**
 * Helper/wrapper class to synchronize calls to ImsCallProfile APIs
 *
 * @See ImsCallProfile
 */
public class QImsCallProfile {
    private ImsCallProfile mCallProfile;

    public QImsCallProfile() {
        mCallProfile = new ImsCallProfile();
    }

    public QImsCallProfile(int serviceType, int callType) {
        mCallProfile = new ImsCallProfile(serviceType, callType);
    }

    public QImsCallProfile(int serviceType, int callType,
            Bundle callExtras, ImsStreamMediaProfile mediaProfile) {
        mCallProfile = new ImsCallProfile(serviceType, callType,
                callExtras, mediaProfile);
    }

    public ImsCallProfile newImsCallProfile() {
        return newImsCallProfile(mCallProfile);
    }

    // deep copies the ImsCallProfile passed in along with ImsStreamMediaProfile
    // Refer to ImsCallProfile: mRestrictCause and mCallerNumberVerificationStatus
    // are not passed through the constructor, so explicitly set the values
    public ImsCallProfile newImsCallProfile(ImsCallProfile profile) {
        ImsCallProfile newImsCallProfile = new ImsCallProfile(profile.getServiceType(),
                profile.getCallType(),
                profile.getCallExtras().deepCopy(),
                newImsStreamMediaProfile(profile.getMediaProfile()));
        newImsCallProfile.setCallerNumberVerificationStatus(
                profile.getCallerNumberVerificationStatus());
        newImsCallProfile.setCallRestrictCause(profile.getRestrictCause());
        return newImsCallProfile;
    }

    // deep copies the ImsStreamMediaProfile passed in
    public ImsStreamMediaProfile newImsStreamMediaProfile(ImsStreamMediaProfile profile) {
        return new ImsStreamMediaProfile(profile.getAudioQuality(),
                profile.getAudioDirection(), profile.getVideoQuality(),
                profile.getVideoDirection(), profile.getRttMode());
    }

    public void updateProfile(ImsCallProfile profile) {
        synchronized(mCallProfile) {
            if (profile != null) {
                mCallProfile = newImsCallProfile(profile);
            }
        }
    }

    public void updateMediaProfile(ImsCallProfile profile) {
        synchronized(mCallProfile) {
            if (profile != null) {
                mCallProfile.mMediaProfile = newImsStreamMediaProfile(profile.getMediaProfile());
            }
        }
    }

    public void updateMediaProfile(ImsStreamMediaProfile profile) {
        synchronized(mCallProfile) {
            if (profile != null) {
                mCallProfile.mMediaProfile = newImsStreamMediaProfile(profile);
            }
        }
    }

    public void updateCallType(ImsCallProfile profile) {
        synchronized(mCallProfile) {
            if (profile != null) {
                mCallProfile.updateCallType(profile);
            }
        }
    }

    public void setCallExtra(String name, String value) {
        synchronized(mCallProfile) {
            mCallProfile.setCallExtra(name, value);
        }
    }

    public void setCallExtraBoolean(String name, boolean value) {
        synchronized(mCallProfile) {
            mCallProfile.setCallExtraBoolean(name, value);
        }
    }

    public void setCallExtraInt(String name, int value) {
        synchronized(mCallProfile) {
            mCallProfile.setCallExtraInt(name, value);
        }
    }

    public void setCallRestrictCause(int cause) {
        synchronized(mCallProfile) {
            mCallProfile.setCallRestrictCause(cause);
        }
    }

    public void setCallerNumberVerificationStatus(int status) {
        synchronized(mCallProfile) {
            mCallProfile.setCallerNumberVerificationStatus(status);
        }
    }

    public int getCallType() {
        synchronized(mCallProfile) {
            return mCallProfile.getCallType();
        }
    }

    public int getServiceType() {
        synchronized(mCallProfile) {
            return mCallProfile.getServiceType();
        }
    }

    public ImsStreamMediaProfile getMediaProfile() {
        synchronized(mCallProfile) {
            return newImsStreamMediaProfile(mCallProfile.getMediaProfile());
        }
    }

    public Bundle getCallExtras() {
        synchronized(mCallProfile) {
            return mCallProfile.getCallExtras();
        }
    }

    public String getCallExtra(String name) {
        synchronized(mCallProfile) {
            return mCallProfile.getCallExtra(name);
        }
    }

    public boolean getCallExtraBoolean(String name) {
        synchronized(mCallProfile) {
            return mCallProfile.getCallExtraBoolean(name);
        }
    }

    public void removeExtras(List<String> extras) {
        synchronized(mCallProfile) {
            Bundle callExtras = getCallExtras();
            if (extras == null) {
                return;
            }
            for (String extra : extras) {
                callExtras.remove(extra);
            }
        }
    }

    @Override
    public String toString() {
        synchronized(mCallProfile) {
            return mCallProfile.toString();
        }
    }
}
