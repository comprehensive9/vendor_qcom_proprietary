/* ======================================================================
*  Copyright (c) 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.content.Context;
import android.os.Bundle;

import com.qti.debugreport.IZatDebugConstants;
import java.util.List;
import java.util.Set;
import java.util.Map;
import java.util.HashMap;

/** @addtogroup IZatGnssConfigServices
    @{ */
public interface IZatGnssConfigServices {

    /**
     * Get IZatSvConfigService instance.
     * <p>
     * This function get the instance of IZatSvConfigService</p>
     *
     * @param
     * None
     *
     * @return
     * The instance of IZatSvConfigService.
     * @throws IZatServiceUnavailableException The feature is not available in this device.
     */
    public IZatSvConfigService getSvConfigService() throws IZatIllegalArgumentException;

    /**
     * Get IZatRobustLocationConfigService instance.
     * <p>
     * This function get the instance of
     * IZatRobustLocationConfigService</p>
     *
     * @param
     * None
     *
     * @return
     * The instance of IZatRobustLocationConfigService.
     * @throws IZatServiceUnavailableException The feature is not available in this device.
     */
    public IZatRobustLocationConfigService getRobustLocationConfigService()
            throws IZatServiceUnavailableException;


    /**
     * Get IZatPreciseLocationConfigService instance.
     * <p>
     * This function get the instance of
     * IZatPreciseLocationConfigService</p>
     *
     * @param
     * None
     *
     * @return
     * The instance of IZatPreciseLocationConfigService.
     * @throws IZatServiceUnavailableException The feature is not available in this device.
     */
    public IZatPreciseLocationConfigService getPreciseLocationConfigService()
            throws IZatServiceUnavailableException;

    /** @addtogroup IZatSvConfigService
        @{ */
    public interface IZatSvConfigService {

        /**
         * Enum IzatGnssSvType.
         *
         * IzatGnssSvType specified constellations which can be enabled or
         * disabled via:
         * {@link com.qti.location.sdk.IZatGnssConfigService.setGnssSvTypeConfig()}
         */

        public enum IzatGnssSvType {

            UNKNOWN(0),
            GPS(1),
            SBAS(2),
            GLONASS(3),
            QZSS(4),
            BEIDOU(5),
            GALILEO(6),
            NAVIC(7);

            private final int value;
            private IzatGnssSvType(int value) {
                this.value = value;
            }
            public int getValue() {
                return value;
            }

            private static final Map<Integer, IzatGnssSvType> valueMap =
                    new HashMap<Integer, IzatGnssSvType>();

            static {
                for (IzatGnssSvType type : IzatGnssSvType.values()) {
                    valueMap.put(type.value, type);
                }
            }

            protected static IzatGnssSvType fromInt(int value) {
                return valueMap.get(value);
            }
        }

        /**
         * Requests the current GNSS SV Type Configuration.
         * <p>
         * This function sends a request to GNSS engine to
         * fetch the current GNSS SV Type Configuration </p>
         *
         * @param gnssConfigCb Callback to receive the SV Type configuration.
         *         This parameter cannot be NULL, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException The gnssConfigCb parameter is NULL.
         *
         * @return
         * None.
         */

        void getGnssSvTypeConfig(IZatSvConfigCallback gnssConfigCb)
                throws IZatIllegalArgumentException;

        /**
         * Sets the GNSS SV Type configuration.
         * <p>
         * This function specifies the GNSS SV Types (constellations) to be
         * enabled and disabled.</p>
         *
         * @param enabledSvTypeSet Set of IzatGnssSvType to be enabled.
         *         Pass null or an empty set if no enablement required.
         * @param disabledSvTypeSet Set of IzatGnssSvType to be disabled.
         *         Pass null or an empty set if no disablement required.
         * @throws IZatIllegalArgumentException The enabledSvTypeSet and
         *          disabledSvTypeSet parameters both are NULL.
         *          Both enabledSvTypeSet and disabledSvTypeSet contain
         *          the same element.
         *
         * @return
         * None.
         */

        void setGnssSvTypeConfig(Set<IzatGnssSvType> enabledSvTypeSet,
                                 Set<IzatGnssSvType> disabledSvTypeSet)
                throws IZatIllegalArgumentException;

        /**
         * Resets the GNSS SV Type configuration.
         * <p>
         * This function resets the GNSS SV Type configuration to
         * default values in the underlying service.</p>
         *
         * @param
         * None
         *
         * @return
         * None.
         */

        void resetGnssSvTypeConfig();

        /**
         * Class IZatGnssConfigCallback.
         *
         * IZatGnssConfigCallback provides the GNSS Config fetched
         * from the underlying service.
         */

        interface IZatSvConfigCallback {

            /**
             * GNSS SV Type Config Callback.
             * <p>
             * This API is called by the underlying service back
             * to applications in response to the getGnssSvTypeConfig
             * request.</p>
             *
             * @param enabledSvTypeSet Set of IzatGnssSvType.
             * @param disabledSvTypeSet Set of IzatGnssSvType.
             */

            void getGnssSvTypeConfigCb(Set<IzatGnssSvType> enabledSvTypeSet,
                                       Set<IzatGnssSvType> disabledSvTypeSet);
        }
    }
    /** @} */ /* end_addtogroup IZatSvConfigService */

    /** @addtogroup IZatRobustLocationConfigService
        @{ */
    public interface IZatRobustLocationConfigService {
        /**
         * Class IzatRLConfigInfo.
         */

        public class IzatRLConfigInfo {
            public static final int ENABLE_STATUS_VALID = 1;
            public static final int ENABLE_FORE911_STATUS_VALID = 2;
            public static final int VERSION_INFO_VALID = 4;

            public int mValidMask;
            public boolean mEnableStatus;
            public boolean mEnableForE911Status;
            public int major;
            public int minor;
        }

        /**
         * Requests the robust location Configuration.
         * <p>
         * This function sends a request to GNSS engine to
         * fetch the current robust location Configuration </p>
         *
         * @param IZatRLConfigCallback Callback to receive the robust location configuration.
         *         This parameter cannot be NULL, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException The gnssConfigCb parameter is NULL.
         *         SecurityException if the device bootloader is unlocked.
         * @return
         * None.
         */

        void getRobustLocationConfig(IZatRLConfigCallback callback);

        /**
         * Sets the robust location configuration.
         * <p>
         * This function enable/disable the robust location feature</p>
         *
         * @param enable set true/false to enable/disable the robust location feature.
         * @param enableForE911 set true/false to enable/disable the robust location
         *        feature for E911 emergency call.
         * @throws SecurityException if the device bootloader is unlocked.
         *
         * @return
         * None.
         */

        void setRobustLocationConfig(boolean enable, boolean enableForE911);

        /**
         * Class IZatRLConfigCallback.
         *
         * IZatGnssRLCallback provides the robust location Config fetched
         * from the underlying service.
         */

        interface IZatRLConfigCallback {

            /**
             * Robust location Config Callback.
             * <p>
             * This API is called by the underlying service back
             * to applications in response to the getRobustLocationConfig
             * request.</p>
             *
             * @param info robust location config info.
             */

            void getRobustLocationConfigCb(IzatRLConfigInfo info);
        }
    }
    /** @} */ /* end_addtogroup IZatRobustLocationConfigService */

    /** @addtogroup IZatPreciseLocationConfigService
        @{ */
    public interface IZatPreciseLocationConfigService {

        /** Some NTRIP mount points requires NMEA GGA.
         *  Due to privacy considerations, this setting indicates
         *  that end user agreed to send location to NTRIP server.
         */
        enum IZatPreciseLocationOptIn {
            OPTED_IN_FOR_LOCATION_REPORT,
            NOT_OPTED_IN_FOR_LOCATION_REPORT
        }

        /**
         * Class IZatPreciseLocationNTRIPSettings
         */
        public class IZatPreciseLocationNTRIPSettings {
            protected String mHostNameOrIP;
            protected String mMountPointName;
            protected int mPort;
            protected String mUserName;
            protected String mPassword;

            /**
             * Configuration settings for NTRIP caster connection.
             * <p>
             * All the parameters are mandatory </p>
             *
             * @param hostNameOrIP Caster host name or IP address.
             * @param mountPointName Name of the caster mount point to connect to.
             * @param port Caster port to connect to.
             * @param userName Caster mount point credentials user name.
             * @param password Caster mount point credentials password.
             * @throws IZatIllegalArgumentException Any of the parameters is NULL.
             *
             * @return
             * IZatPreciseLocationNTRIPSettings.
             */
            public IZatPreciseLocationNTRIPSettings(
                    String hostNameOrIP,
                    String mountPointName,
                    int port,
                    String userName,
                    String password)
                    throws IZatIllegalArgumentException {

                mHostNameOrIP = validateString(hostNameOrIP);
                mMountPointName = validateString(mountPointName);
                mUserName = validateString(userName);
                mPassword = validateString(password);
                mPort = port;
            }

            private String validateString(String param) {
                if (null == param || param.isEmpty()) {
                    throw new IZatIllegalArgumentException();
                }

                return param;
            }
        }

        /**
         * Set the user consent for sharing location to NTRIP mount point.
         * Shall be called before enablePreciseLocation.
         * @param consent Consent is granted or not.
         *
         * It is the responsibility of the OEM to comply with all applicable privacy and data
         * protection laws, rules, and regulation (for example, obtaining consumer opt-in consent).
         * To enable Qualcomm SW device to report its location to the NTRIP mount point, OEM App
         * shall explicitly configure Qualcomm SW.
         */
        void setPreciseLocationOptIn(IZatPreciseLocationOptIn optin);

        /**
         * Enables the Precise Location data stream using the desired NTRIP caster settings.
         * <p>
         * This is system-global and will affect any ongoing or future GNSS tracking session. </p>
         *
         * It is the responsibility of the OEM to comply with all applicable privacy and data
         * protection laws, rules, and regulation (for example, obtaining consumer opt-in consent).
         * To enable Qualcomm SW device to report its location to the NTRIP mount point, OEM App
         * shall explicitly configure Qualcomm SW.
         *
         * @param ntripSettings Settings for the NTRIP connection as defined in
         *                      IZatPreciseLocationNTRIPSettings.
         * @param requiresInitialNMEA Boolean indicating if the correction data server requires
         *                      device location in the form of GGA NMEA string.
         * @throws IZatIllegalArgumentException The settings are not properly initialized.
         *
         * @return
         * None.
         */
        void enablePreciseLocation(IZatPreciseLocationNTRIPSettings ntripSettings,
                boolean requiresInitialNMEA)
                throws IZatIllegalArgumentException;

        /**
         * Disables the Precise Location data stream.
         * <p>
         * This is system-global and will affect any ongoing or future GNSS tracking session. </p>
         *
         * @return
         * None.
         */
        void disablePreciseLocation();
    }
    /** @} */ /* end_addtogroup IZatPreciseLocationConfigService */
}
/** @} */ /* end_addtogroup IZatGnssConfigServices */
