/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.location.hidlclient;

import android.location.Location;
import android.util.Log;
import vendor.qti.gnss.V1_0.LocHidlLocation;
import vendor.qti.gnss.V1_0.LocHidlLocationFlagsBits;
import java.lang.Throwable;

public class HidlClientUtils {

    public static final int ULP_LOCATION_IS_FROM_HYBRID = 0x0001;
    public static final int ULP_LOCATION_IS_FROM_GNSS   = 0x0002;


    public static Location translateHidlLocation(LocHidlLocation hidlLocation) {
        Location location = new Location("");

        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.LAT_LONG_BIT) != 0) {
            location.setLatitude(hidlLocation.latitude);
            location.setLongitude(hidlLocation.longitude);
            location.setTime(hidlLocation.timestamp);
        }
        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.ALTITUDE_BIT) != 0) {
            location.setAltitude(hidlLocation.altitude);
        }
        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.SPEED_BIT) != 0) {
            location.setSpeed(hidlLocation.speed);
        }
        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.BEARING_BIT) != 0) {
            location.setBearing(hidlLocation.bearing);
        }
        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.ACCURACY_BIT) != 0) {
            location.setAccuracy(hidlLocation.accuracy);
        }
        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.VERTICAL_ACCURACY_BIT)
                != 0) {
            location.setVerticalAccuracyMeters(hidlLocation.verticalAccuracy);
        }
        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.SPEED_ACCURACY_BIT) != 0) {
            location.setSpeedAccuracyMetersPerSecond(hidlLocation.speedAccuracy);
        }
        if ((hidlLocation.locationFlagsMask & LocHidlLocationFlagsBits.BEARING_ACCURACY_BIT) != 0) {
            location.setBearingAccuracyDegrees(hidlLocation.bearingAccuracy);
        }
        location.makeComplete();
        return location;
    }

    private static void printHidlInfo(String tag, String extraStr) {
        String nameofCurrMethod = new Throwable().getStackTrace()[2].getMethodName();
        int lineNum = new Throwable().getStackTrace()[2].getLineNumber();
        Log.d(tag, "[" + nameofCurrMethod + "][" + lineNum + "] " +  extraStr);
    }

    public static void toHidlService(String tag) {
        printHidlInfo(tag, "[HC] =>> [HS]");
    }

    public static void fromHidlService(String tag) {
        printHidlInfo(tag, "[HC] <<= [HS]");
    }

    public static long hexMacToLong(String mac_hex) {
        String hex_mac_lo = mac_hex.substring(0, 6);
        String hex_mac_hi = mac_hex.substring(6);

        long mac = Long.parseLong(hex_mac_lo, 16);
        mac <<= 24;
        mac |= Long.parseLong(hex_mac_hi, 16);
        return mac;
    }

    public static String longMacToHex(long mac_long) {
        // If mac value goes beyond 6 bytes
        if ((mac_long >> 48) > 0) {
            return String.format("%016X", mac_long);
        } else {
            return String.format("%012X", mac_long);
        }
    }

    public static int RiltoIZatCellTypes(int rilCellType) {
        int res = -1;
        final int LOC_RIL_TECH_CDMA = 0x1;
        final int LOC_RIL_TECH_GSM = 0x2;
        final int LOC_RIL_TECH_WCDMA = 0x4;
        final int LOC_RIL_TECH_LTE = 0x8;

        final int GSM = 0;
        final int WCDMA = 1;
        final int CDMA = 2;
        final int LTE = 3;


        if (rilCellType == LOC_RIL_TECH_CDMA) {
            res = CDMA;
        }

        if (rilCellType == LOC_RIL_TECH_GSM) {
            res = GSM;
        }

        if (rilCellType == LOC_RIL_TECH_WCDMA) {
            res = WCDMA;
        }

        if (rilCellType == LOC_RIL_TECH_LTE) {
            res = LTE;
        }

        return res;
    }

    public static int FDCLtoIZatCellTypes(int rilCellType) {
        int res = -1;
        final int eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_CDMA_V02 = 0;
        final int eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_GSM_V02 = 1;
        final int eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_WCDMA_V02 = 2;
        final int eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_V02 = 3;

        final int GSM = 0;
        final int WCDMA = 1;
        final int CDMA = 2;
        final int LTE = 3;

        if (rilCellType == eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_CDMA_V02) {
            res = CDMA;
        }

        if (rilCellType == eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_GSM_V02) {
            res = GSM;
        }

        if (rilCellType == eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_WCDMA_V02) {
            res = WCDMA;
        }

        if (rilCellType == eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_V02) {
            res = LTE;
        }

        return res;
    }
}
