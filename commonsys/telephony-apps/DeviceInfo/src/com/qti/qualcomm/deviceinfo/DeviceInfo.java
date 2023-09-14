/*
 * Copyright (c) 2012-2013, 2015-2017, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qti.qualcomm.deviceinfo;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.Resources;
import android.content.SharedPreferences;
import android.os.AsyncResult;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.preference.PreferenceManager;
import android.telephony.CellLocation;
import android.telephony.cdma.CdmaCellLocation;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousFileChannel;
import java.nio.channels.CompletionHandler;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharacterCodingException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.io.IOException;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.IccCard;
import com.android.internal.telephony.uicc.IccUtils;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyProperties;
import com.android.internal.telephony.uicc.IccConstants;
import com.android.internal.telephony.uicc.IccFileHandler;
import com.android.internal.telephony.uicc.IccRecords;
import com.android.internal.telephony.uicc.UiccController;
import com.android.internal.telephony.PhoneConstants;

import org.codeaurora.internal.IExtTelephony;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * Display the following information # Battery Strength : TODO # Uptime # Awake
 * Time # XMPP/buzz/tickle status : TODO
 */
public class DeviceInfo extends PreferenceActivity {
    private static final String TAG = DeviceInfo.class.getSimpleName();

    private static final String KEY_DEVICE_MODEL = "device_model";
    private static final String KEY_HW_VERSION = "hardware_version";
    private static final String KEY_MEID = "meid_number";
    private static final String KEY_ESN = "esn_number";
    private static final String KEY_BASEBAND_VERSION = "baseband_version";
    private static final String KEY_PRL_VERSION = "prl_version";
    private static final String KEY_ANDROID_VERSION = "android_version";
    private static final String KEY_SOFTWARE_VERSION = "software_version";
    private static final String KEY_PHONE_NUMBER = "number";
    private static final String KEY_IMSI = "imsi";
    private static final String KEY_UIM_ID = "uim_id";
    private static final String KEY_SID_NUMBER = "sid_number";
    private static final String KEY_NID_NUMBER = "nid_number";
    private static final String KEY_EPRL_NUMBER = "eprl_number";
    private static final String KEY_MSPL_NUMBER = "mspl_number";
    private static final String KEY_MLPL_NUMBER = "mlpl_number";
    private static final String KEY_ICCID_NUMBER = "iccid_number";
    private static final String KEY_IMEI_NUMBER_1 = "imei_number_1";
    private static final String KEY_IMEI_NUMBER_2 = "imei_number_2";
    private static final String KEY_META_VERSION = "meta_info";

    private static final String SP_MEID = "sp_meid";

    // Please get these values from IccConstants
    // CDMA RUIM file ids from 3GPP2 C.S0023-0
    // RUIM EF stores the (up to) 56-bit electronic identification
    // number (ID) unique to the R-UIM. (Removable UIM_ID)
    static final int EF_RUIM_ID = 0x6f31;

    // 3GPP2 C.S0065
    static final int EF_CSIM_PRL = 0x6F30;

    // C.S0074-Av1.0 Section 4
    static final int EF_CSIM_MLPL = 0x4F20;
    static final int EF_CSIM_MSPL = 0x4F21;
    //End of IccConstants value

    private Phone mPhone = null;
    private String mUnknown = null;

    private ObtainIdHandler mHandler = new ObtainIdHandler();
    private IExtTelephony mExtTelephony = IExtTelephony.Stub.
            asInterface(ServiceManager.getService("qti.radio.extphone"));
    private SharedPreferences mSharedPreferences;
    private MetaInfoFetcherProxy mMetaInfoFetcherProxy;

    private static abstract class StringFilePraserBase
            implements CompletionHandler<Integer, ByteBuffer> {
        private AsynchronousFileChannel mAsynchronousFileChannel;
        private ByteBuffer mBuffer;
        private boolean mIsStarted = false;

        protected abstract String getFileName ();
        protected abstract void onCompleted (String ret);

        public StringFilePraserBase() {
            Path path = Paths.get(getFileName());
            try {
                mAsynchronousFileChannel =
                        AsynchronousFileChannel.open(path, StandardOpenOption.READ);
                mBuffer = ByteBuffer.allocate((int)mAsynchronousFileChannel.size());
            } catch (IOException e) {
                Rlog.e(TAG, " failed to open " + getFileName());
            }
        }

        public void start() {
            if (mAsynchronousFileChannel != null && !mIsStarted) {
                mIsStarted = true;
                mAsynchronousFileChannel.read(mBuffer, 0, mBuffer, this);
            }
        }

        public void stop() {
            if (mIsStarted) {
                mIsStarted = false;
                close();
            }
        }

        @Override
        public void completed(Integer result, ByteBuffer attachment) {
            close();
            attachment.flip();
            try {
                CharsetDecoder decoder = Charset.forName("UTF-8").newDecoder();
                CharBuffer charBuffer = decoder.decode(attachment);
                onCompleted(charBuffer.toString());
            } catch (CharacterCodingException e) {
                Rlog.e(TAG, " decoded with failure");
            }
        }

        @Override
        public void failed(Throwable exc, ByteBuffer attachment) {
            close();
            onCompleted(null);
        }

        private void close() {
            try {
                if (mAsynchronousFileChannel != null) {
                    mAsynchronousFileChannel.close();
                }
            } catch (IOException e) {
                Rlog.e(TAG, " failed to close");
            }
        }
    }

    /*
     * Parse version info based on json format below
     * {
     *   "Image_Build_IDs": {
     *     "apps": "LA.UM.9.12.r1-06402-SMxx50.1-6",
     *     "qssi": "LA.QSSI.11.0.r1-03602-qssi.1-2",
     *      ....
     *    },
     *    "Metabuild_Info": {
     *       "Meta_Build_ID": "SM8250_SDX55.LA_TN.2-0_2-0-00106-STD_LP5.INT-1",
     *        ...
     *     },
     *    "Version": "1.0"
     * }
     */
    private static final class MetaInfoFetcherProxy extends StringFilePraserBase {
        private static final String FILENAME_META_VERSION_INFO =
                "/data/vendor/modem_config/ver_info.txt";
        private static final String KEY_META_BUILD_INFO = "Metabuild_Info";
        private static final String KEY_IMAGE_BUILD_INFO = "Image_Build_IDs";
        private static final String KEY_META_BUILD_ID = "Meta_Build_ID";
        private static final String KEY_IMAGE_BUILD_APPS_ID = "apps";

        private DeviceInfo mListener;

        public MetaInfoFetcherProxy (DeviceInfo deviceInfo) {
            super();
            mListener = deviceInfo;
        }

        @Override
        protected String getFileName () {
            return FILENAME_META_VERSION_INFO;
        }

        @Override
        protected  void onCompleted (String ret) {
            String metaId = null;
            try {
                if (TextUtils.isEmpty(ret)) {
                    Rlog.d(TAG, "result is null or empty");
                    return;
                }

                final JSONObject jsonVerInfo = new JSONObject(ret);
                metaId = jsonVerInfo.getJSONObject(KEY_META_BUILD_INFO)
                        .getString(KEY_META_BUILD_ID);
                final JSONObject jsonImageIds = jsonVerInfo.getJSONObject(KEY_IMAGE_BUILD_INFO);
                final String apps = jsonImageIds.getString(KEY_IMAGE_BUILD_APPS_ID);
                Rlog.i(TAG, "Meta ID = " + metaId + " AU = " + apps);
            } catch (JSONException e) {
                Rlog.e(TAG, "parsing failed as json format");
                metaId = ret;
            } finally {
                String model = null;
                if (!TextUtils.isEmpty(metaId)) {
                    String[] metaArray = metaId.split("\\.");
                    if (metaArray.length > 0) {
                        model = metaArray[0];
                    }
                }
                mListener.onMetaInfoLoaded(metaId, model);
            }
        }
    }

    private class ObtainIdHandler extends Handler {
        private static final int MESSAGE_GET_EF_RUIM = 1;
        private static final int MESSAGE_GET_EF_PRL = 2;
        private static final int MESSAGE_GET_EF_MSPL = 3;
        private static final int MESSAGE_GET_EF_MLPL = 4;
        private static final int MESSAGE_GET_DEVICE_IDENTITY = 5;
        //RUIM ID is 8 bytes data
        private static final int NUM_BYTES_RUIM_ID = 8;
        //True PRL ID is x bytes data
        private static final int NUM_BYTES_PRL_ID = 4;
        //MSPL ID is x bytes data
        private static final int NUM_BYTES_MSPL_ID =5;
        //MLPL ID is 8 bytes data
        private static final int NUM_BYTES_MLPL_ID = 5;

        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_GET_EF_RUIM:
                    handleGetEFRuim(msg);
                    break;
                case MESSAGE_GET_EF_PRL:
                    handleGetEFPrl(msg);
                    break;
                case MESSAGE_GET_EF_MSPL:
                    handleGetEFMspl(msg);
                    break;
                case MESSAGE_GET_EF_MLPL:
                    handleGetEFMlpl(msg);
                    break;
                case MESSAGE_GET_DEVICE_IDENTITY:
                    handleGetDeviceIdentity(msg);
                    break;
            }
        }

        private void handleGetEFRuim(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            byte[] data = (byte[])ar.result;
            if (ar.exception == null) {
                //for RUIM ID data, the first byte represent the num bytes of valid data. From
                //the second byte to num+1 byte, it is valid RUIM ID data. And the second
                //byte is the lowest-order byte, the num+1 byte is highest-order
                int numOfBytes = data[0];
                if (numOfBytes < NUM_BYTES_RUIM_ID) {
                    byte[] decodeData = new byte[numOfBytes];
                    for (int i = 0; i < numOfBytes; i++) {
                        decodeData[i] = data[numOfBytes - i];
                    }
                    String ruimid = IccUtils.bytesToHexString(decodeData);
                    setSummaryText(KEY_UIM_ID, ruimid);
                }
            }
        }

        private void handleGetEFPrl(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            byte[] data = (byte[]) ar.result;
            if (ar.exception == null) {
                if (data.length > NUM_BYTES_PRL_ID - 1) {
                    int prlId = ((data[2] & 0xFF) << 8) | (data[3] & 0xFF);
                    setSummaryText(KEY_PRL_VERSION, String.valueOf(prlId));
                }
            }
        }

        private void handleGetEFMspl(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            byte[] data = (byte[])ar.result;
            if (ar.exception == null) {
                if (data.length > NUM_BYTES_MSPL_ID -1) {
                    int msplId = ((data[3] & 0xFF) << 8 ) | (data[4] & 0xFF);
                    setSummaryText(KEY_MSPL_NUMBER, String.valueOf(msplId));
                }
            }
        }

        private void handleGetEFMlpl(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            byte[] data = (byte[])ar.result;
            if (ar.exception == null) {
                if (data.length > NUM_BYTES_MLPL_ID - 1) {
                    int mlplId = ((data[3] & 0xFF) << 8) | (data[4] & 0xFF);
                    setSummaryText(KEY_MLPL_NUMBER, String.valueOf(mlplId));
                }
            }
        }

        private void handleGetDeviceIdentity(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar.exception == null) {
                String[] respId = (String[])ar.result;
                setSummaryText(KEY_MEID, respId[3]);
                // Need clear calling identity due to run in phone wiht system uid
                final long token = Binder.clearCallingIdentity();
                mSharedPreferences.edit().putString(SP_MEID, respId[3]).apply();
            }
        }

        public void GetRuimId() {
            UiccController controller = UiccController.getInstance();
            if (controller != null && mPhone != null) {
                IccFileHandler fh = controller.getIccFileHandler(mPhone.getPhoneId(), UiccController.APP_FAM_3GPP2);
                if (fh != null) {
                    //fh.loadEFTransparent(IccConstants.EF_RUIM_ID, NUM_BYTES_RUIM_ID,
                    fh.loadEFTransparent(EF_RUIM_ID, NUM_BYTES_RUIM_ID,
                            mHandler.obtainMessage(ObtainIdHandler.MESSAGE_GET_EF_RUIM));
                }
            }
        }

        public void getPrlVersion() {
            UiccController controller = UiccController.getInstance();
            if (controller != null && mPhone != null) {
                IccFileHandler fh = controller.getIccFileHandler(mPhone.getPhoneId(), UiccController.APP_FAM_3GPP2);
                if (fh != null) {
                    //fh.loadEFTransparent(IccConstants.EF_CSIM_PRL, NUM_BYTES_PRL_ID,
                    fh.loadEFTransparent(EF_CSIM_PRL, NUM_BYTES_PRL_ID,
                            mHandler.obtainMessage(ObtainIdHandler.MESSAGE_GET_EF_PRL));
                }
            }
        }

        public void getMsplVersion() {
            UiccController controller = UiccController.getInstance();
            if (controller != null && mPhone != null) {
                IccFileHandler fh = controller.getIccFileHandler(mPhone.getPhoneId(), UiccController.APP_FAM_3GPP2);
                if (fh != null) {
                    //fh.loadEFTransparent(IccConstants.EF_CSIM_MSPL, NUM_BYTES_MSPL_ID,
                    fh.loadEFTransparent(EF_CSIM_MSPL, NUM_BYTES_MSPL_ID,
                            mHandler.obtainMessage(ObtainIdHandler.MESSAGE_GET_EF_MSPL));
                }
            }
        }

        public void getMlplVersion() {
            UiccController controller = UiccController.getInstance();
            if (controller != null && mPhone != null) {
                IccFileHandler fh = controller.getIccFileHandler(mPhone.getPhoneId(), UiccController.APP_FAM_3GPP2);
                if (fh != null) {
                    //fh.loadEFTransparent(IccConstants.EF_CSIM_MLPL, NUM_BYTES_MLPL_ID,
                    fh.loadEFTransparent(EF_CSIM_MLPL, NUM_BYTES_MLPL_ID,
                            mHandler.obtainMessage(ObtainIdHandler.MESSAGE_GET_EF_MLPL));
                }
            }
        }

        public String getBasebandVersionForPhone(int phoneId, String defaultVal) {
            if (SubscriptionManager.isValidPhoneId(phoneId)) {
                return TelephonyManager.getDefault().getBasebandVersionForPhone(phoneId);
            }
            return defaultVal;
        }

        public void getDeviceIdentity() {
            if (mPhone != null) {
                mPhone.mCi.getDeviceIdentity(obtainMessage(MESSAGE_GET_DEVICE_IDENTITY));
            }
        }

    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        addPreferencesFromResource(R.xml.device_info);

        mMetaInfoFetcherProxy = new MetaInfoFetcherProxy(this);
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);

        int phoneId = getCdmaPhoneId();
        if (phoneId != Integer.MAX_VALUE) {
            mPhone = PhoneFactory.getPhone(phoneId);
        }

        if (mUnknown == null) {
            mUnknown = getResources().getString(R.string.device_info_unknown);
        }

        setPreferenceSummary();
    }


    private int getCdmaPhoneId() {
        int phoneId = Integer.MAX_VALUE;
        try {
            if (mExtTelephony != null) {
                phoneId = mExtTelephony.getPrimaryStackPhoneId();
            }
        } catch (RemoteException ex) {
            int numPhone = TelephonyManager.getDefault().getPhoneCount();
            for (int i=0; i<numPhone; i++) {
                Phone phone = PhoneFactory.getPhone(i);
                if (phone.getPhoneType() == TelephonyManager.PHONE_TYPE_CDMA) {
                    phoneId = i;
                    break;
                }
            }
        }
        return phoneId;
    }

    private void setPreferenceSummary() {

        final PreferenceScreen prefSet = getPreferenceScreen();
        // get hardware
        String hwVersion = SystemProperties.get("ro.hw_version", mUnknown);
        if (TextUtils.isEmpty(hwVersion) || hwVersion.equals(mUnknown)) {
            hwVersion = "PVT2.0";
        }
        setSummaryText(KEY_HW_VERSION, hwVersion);

        // get android version, get property of ro.build.version.release_or_codename.
        setSummaryText(KEY_ANDROID_VERSION, Build.VERSION.RELEASE_OR_CODENAME);

        // get my phone number
        if (mPhone != null) {
            // get baseband version
            String basebandVersion =
                    mHandler.getBasebandVersionForPhone(mPhone.getPhoneId(), null);
            setSummaryText(KEY_BASEBAND_VERSION, basebandVersion);

            // get MEID
            setSummaryText(KEY_MEID, getMeid());

            // get ESN
            setSummaryText(KEY_ESN, mPhone.getEsn());

            // get PRL version
            mHandler.getPrlVersion();

            // get IMSI
            String imsi = mPhone.getSubscriberId();
            if (imsi == null || imsi.length() == 0) {
                IccCard card = mPhone.getIccCard();
                if (card != null) {
                    IccRecords record = card.getIccRecords();
                    if (record != null) {
                        imsi = record.getIMSI();
                    }
                }
            }
            setSummaryText(KEY_IMSI, imsi);

            // get UIM ID
            mHandler.GetRuimId();

            // get EPrl number
            setSummaryText(KEY_EPRL_NUMBER, mPhone.getCdmaPrlVersion());

            // get Mspl number
            mHandler.getMsplVersion();

            // get Mlpl number
            mHandler.getMlplVersion();

            // get NID and SID
            CellLocation cellLocation = mPhone.getCellIdentity().asCellLocation();
            if (cellLocation != null && cellLocation instanceof CdmaCellLocation) {
                CdmaCellLocation cdmaCellLoc = (CdmaCellLocation)cellLocation;
                setSummaryText(KEY_SID_NUMBER, cdmaCellLoc.getSystemId() + "");
                setSummaryText(KEY_NID_NUMBER, cdmaCellLoc.getNetworkId() + "");
            }

            // get iccid
            setSummaryText(KEY_ICCID_NUMBER, mPhone.getFullIccSerialNumber());

            //get imei
            int numPhone = TelephonyManager.getDefault().getPhoneCount();
            for (int i = 0; i < PhoneConstants.MAX_PHONE_COUNT_DUAL_SIM; i++) {
                Phone phone = PhoneFactory.getPhone(i);
                String imei = null;
                if (i < numPhone) {
                    if (phone != null) {
                        imei = phone.getImei();
                    }
                    switch (i) {
                        case PhoneConstants.SUB1:
                             setSummaryText(KEY_IMEI_NUMBER_1, imei);
                        case PhoneConstants.SUB2:
                            setSummaryText(KEY_IMEI_NUMBER_2, imei);
                    }
                } else {
                    removePreference(KEY_IMEI_NUMBER_2, prefSet);
                }
            }
        }
    }

    private void setSummaryText(String preferenceKey, String value) {
        Preference preference = findPreference(preferenceKey);
        if (preference == null)
            return;

        if (TextUtils.isEmpty(value))
            preference.setSummary(mUnknown);
        else
            preference.setSummary(value);
    }

    private void removePreference(String preferenceKey, PreferenceScreen prefSet) {
        Preference preference = findPreference(preferenceKey);
        if (preference == null)
            return;

        prefSet.removePreference(preference);
    }

    private String getMeid() {
        String meid = mSharedPreferences.getString(SP_MEID, "");
        if (invalidMeid(meid)) {
            if (mPhone != null) {
                meid = mPhone.getMeid();
                if (invalidMeid(meid)) {
                    meid = null;
                    mHandler.getDeviceIdentity();
                } else {
                    mSharedPreferences.edit().putString(SP_MEID, meid).apply();
                }
            }
        }
        return meid;
    }

    private boolean invalidMeid(String meid) {
        return TextUtils.isEmpty(meid) || meid.equals("0")
                || meid.startsWith("00000000");
    }

    @Override
    public void onStart() {
        super.onStart();
        mMetaInfoFetcherProxy.start();
    }

    @Override
    public void onStop() {
        super.onStop();
        mMetaInfoFetcherProxy.stop();
    }

    // running in work thread
    private void onMetaInfoLoaded(String metaVer, String model) {
        runOnUiThread(()-> {
            setSummaryText(KEY_SOFTWARE_VERSION, metaVer);
            setSummaryText(KEY_DEVICE_MODEL, model);
        });
    }

}
