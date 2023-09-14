/* ======================================================================
*  Copyright (c) 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.gnssconfig;

import android.os.Parcel;
import android.os.Parcelable;

public final class NtripConfigData implements Parcelable {
    public String mHostNameOrIP;
    public String mMountPointName;
    public int mPort;
    public String mUserName;
    public String mPassword;
    public boolean mRequiresInitialNMEA;

    public NtripConfigData() {
    }

    public static final Parcelable.Creator<NtripConfigData> CREATOR =
        new Parcelable.Creator<NtripConfigData>() {
        public NtripConfigData createFromParcel(Parcel in) {
            return new NtripConfigData(in);
        }

        public NtripConfigData[] newArray(int size) {
            return new NtripConfigData[size];
        }
    };

    private NtripConfigData(Parcel in) {
        mHostNameOrIP = in.readString();
        mMountPointName = in.readString();
        mPort = in.readInt();
        mUserName = in.readString();
        mPassword = in.readString();
        mRequiresInitialNMEA = in.readBoolean();
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeString(mHostNameOrIP);
        out.writeString(mMountPointName);
        out.writeInt(mPort);
        out.writeString(mUserName);
        out.writeString(mPassword);
        out.writeBoolean(mRequiresInitialNMEA);
    }

    public int describeContents() {
        return 0;
    }
}
