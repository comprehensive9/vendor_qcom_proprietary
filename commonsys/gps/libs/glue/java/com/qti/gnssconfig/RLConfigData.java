/* ======================================================================
*  Copyright (c) 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.gnssconfig;

import android.os.Parcel;
import android.os.Parcelable;

public final class RLConfigData implements Parcelable {
    public int validMask;
    public boolean enableStatus;
    public boolean enableStatusForE911;
    public int major;
    public int minor;

    public RLConfigData() {
    }

    public static final Parcelable.Creator<RLConfigData> CREATOR =
        new Parcelable.Creator<RLConfigData>() {
        public RLConfigData createFromParcel(Parcel in) {
            return new RLConfigData(in);
        }

        public RLConfigData[] newArray(int size) {
            return new RLConfigData[size];
        }
    };

    private RLConfigData(Parcel in) {
        validMask = in.readInt();
        enableStatus = in.readBoolean();
        enableStatusForE911 = in.readBoolean();
        major = in.readInt();
        minor = in.readInt();
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(validMask);
        out.writeBoolean(enableStatus);
        out.writeBoolean(enableStatusForE911);
        out.writeInt(major);
        out.writeInt(minor);
    }

    public int describeContents() {
        return 0;
    }
}
