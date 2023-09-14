/*
* Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pWfdInfo;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.NetType;
import com.qualcomm.wfd.client.Whisperer;

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;

public class WifiP2pNetDevice implements NetDeviceIF {
    static final String TAG = "WifiP2pNetDevice";

    private WifiP2pDevice mDevice;
    //WifiP2pInfo mWifiP2pInfo;

    private WifiP2pConfig mConfig;

    WifiP2pNetDevice(WifiP2pDevice device) {
        mDevice = device;
    }

    WifiP2pDevice getDevice() {
        return mDevice;
    }

    void setDevice(WifiP2pDevice device) {
        if (device == null) {
            throw new IllegalArgumentException("device cannot be null");
        }
        mDevice = device;
    }

    @Override
    public NetType getNetType() {
        return NetType.WIFI_P2P;
    }

    @Override
    public WifiP2pNetManager getNetTypeManager() {
        return WifiP2pNetManager.sInstance;
    }

    @Override
    public void setConfig(WifiP2pConfig config) {
        mConfig = config;
    }

    @Override
    public void connect(final ResultListener initListener) {
        Log.v(TAG, "connect");
        Whisperer whisperer = Whisperer.getInstance();
        if (mConfig == null) {
            mConfig = new WifiP2pConfig();
            mConfig.deviceAddress = mDevice.deviceAddress;
            mConfig.wps.setup = WpsInfo.PBC;
            if (whisperer.getSharedPrefs().getBoolean("perform_custom_group_owner_intent_value",
                    false) == true) {
                mConfig.groupOwnerIntent = Integer.parseInt(whisperer.getSharedPrefs().getString(
                        "group_owner_intent_value", "3"));
            }
            Toast.makeText(whisperer.getActivity(), "group owner intent value: "
                    + mConfig.groupOwnerIntent, Toast.LENGTH_LONG);
            Log.d(TAG, "group owner intent value: " + mConfig.groupOwnerIntent);

        } else {
            mConfig.deviceAddress = mDevice.deviceAddress;
        }

        getNetTypeManager().manager.connect(getNetTypeManager().channel, mConfig, new WifiP2pManager.ActionListener() {
            @Override
            public void onSuccess() {
                initListener.onSuccess(null);
            }

            @Override
            public void onFailure(int i) {
                initListener.onFailure(i);
            }
        });
    }

    @Override
    public void disconnect(final ResultListener rl) {
        Log.v(TAG, "disconnect");
        getNetTypeManager().disconnect(rl);
    }

    @Override
    public boolean isStatusConnected() {
        return mDevice.status == WifiP2pDevice.CONNECTED;
    }

    @Override
    public boolean isStatusFailed() {
        return mDevice.status == WifiP2pDevice.FAILED;
    }

    @Override
    public String getStatusString() {
        Log.d(TAG, "getDeviceStatus:" + mDevice.status);
        switch (mDevice.status) {
            case WifiP2pDevice.AVAILABLE:
                return "Available";
            case WifiP2pDevice.INVITED:
                return "Invited";
            case WifiP2pDevice.CONNECTED:
                return "Connected";
            case WifiP2pDevice.FAILED:
                return "Failed";
            case WifiP2pDevice.UNAVAILABLE:
                return "Unavailable";
            default:
                return "Unknown";
        }
    }

    @Override
    public String getWfdInfoString() {
        Log.v(TAG,"Get WFD Info String");
        WifiP2pWfdInfo wfdInfo = mDevice != null? mDevice.getWfdInfo() : null;
        if (wfdInfo != null) {
            WifiP2pWfdInfoHelper wfdInfoHelper = WifiP2pWfdInfoHelper.wrap(wfdInfo);
            if (wfdInfoHelper.isWfdR2Supported()) {
                return new P2pWfdDeviceInfo(wfdInfoHelper.getDeviceInfoHex()+
                        wfdInfoHelper.getR2DeviceInfoHex()).toString();
            }
            else
                return new P2pWfdDeviceInfo(wfdInfoHelper.getDeviceInfoHex()).toString();
        }
        return ("");
    }

    private boolean isLocalDevice() {
        return mDevice != null && mDevice.deviceAddress != null &&
                mDevice.deviceAddress.equals(getNetTypeManager().getLocalDevice().getAddress());
    }

    @Override
    public String getIp() {
        if (isLocalDevice()) {
            // return null for local device
            Log.w(TAG, "getIp for local device, return null");
            return null;
        }
        ConnectionInfoIF localInfo = getNetTypeManager().getLocalConnectionInfo();
        if (localInfo != null && !localInfo.isGroupOwner()) {
            Log.v(TAG, "get ip for group owner");
            return localInfo.getGroupOwnerIp();
        }
        Log.v(TAG, "get ip from arp");
        return WifiP2pNetManager.getPeerIP(mDevice.deviceAddress);
    }

    @Override
    public WfdDevice convertToWfdDevice() {
        return getNetTypeManager().convertToWfdDevice(this);
    }

    @Override
    public String getName() {
        return mDevice.deviceName;
    }

    @Override
    public String getAddress() {
        return mDevice.deviceAddress;
    }

    @Override
    public boolean supportWfd() {
        return mDevice != null && mDevice.getWfdInfo() != null;
    }

    @Override
    public WfdEnums.WFDDeviceType getWfdType() {
        WifiP2pWfdInfo wfdInfo = mDevice != null? mDevice.getWfdInfo() : null;
        if (mDevice == null || wfdInfo == null) {
            return WfdEnums.WFDDeviceType.UNKNOWN;
        }
        int type = wfdInfo.getDeviceType();
        switch (type) {
            case WifiP2pWfdInfo.DEVICE_TYPE_WFD_SOURCE: return WfdEnums.WFDDeviceType.SOURCE;
            case WifiP2pWfdInfo.DEVICE_TYPE_PRIMARY_SINK: return WfdEnums.WFDDeviceType.PRIMARY_SINK;
            case WifiP2pWfdInfo.DEVICE_TYPE_SECONDARY_SINK: return WfdEnums.WFDDeviceType.SECONDARY_SINK;
            case WifiP2pWfdInfo.DEVICE_TYPE_SOURCE_OR_PRIMARY_SINK: return WfdEnums.WFDDeviceType.SOURCE_PRIMARY_SINK;
        }
        return WfdEnums.WFDDeviceType.UNKNOWN;
    }

    @Override
    public boolean hasDeviceInfo() {
        return mDevice != null;
    }

    @Override
    public boolean isWfdSessionAvailable() {
        WifiP2pWfdInfo wfdInfo = mDevice != null? mDevice.getWfdInfo() : null;
        return wfdInfo != null && wfdInfo.isSessionAvailable();
    }

    @Override
    public String toString() {
        return "WifiP2pNetDevice: " + mDevice.toString();
    }

    static class WifiP2pWfdInfoHelper {

        static final String TAG = "WifiP2pWfdInfoHelper";

        static final Constructor sConstructor; // ctor(int devInfo, int ctrlPort, int maxTput)
        static final Method sMethod_isWfdR2Supported; // boolean isWfdR2Supported()
        static final Method sMethod_getDeviceInfoHex; // String getDeviceInfoHex()
        static final Method sMethod_getR2DeviceInfoHex; // String getR2DeviceInfoHex()
        static final Method sMethod_setWfdR2Device; // void setWfdR2Device(int r2DeviceInfo)

        static {
            Constructor constructor = null;
            try {
                constructor = WifiP2pWfdInfo.class.getConstructor(int.class, int.class, int.class);
            } catch (Exception e) {
                Log.e(TAG, "Failed to get WifiP2pWfdInfo constructor(int, int, int)", e);
            }
            sConstructor = constructor;

            Method method_isWfdR2Supported = null;
            try {
                method_isWfdR2Supported = WifiP2pWfdInfo.class.getMethod("isWfdR2Supported");
            } catch (Exception e) {
                Log.e(TAG, "Failed to get WifiP2pWfdInfo.isWfdR2Supported() method", e);
            }
            sMethod_isWfdR2Supported = method_isWfdR2Supported;

            Method method_getDeviceInfoHex = null;
            try {
                method_getDeviceInfoHex = WifiP2pWfdInfo.class.getMethod("getDeviceInfoHex");
            } catch (Exception e) {
                Log.e(TAG, "Failed to get WifiP2pWfdInfo.getDeviceInfoHex() method", e);
            }
            sMethod_getDeviceInfoHex = method_getDeviceInfoHex;

            Method method_getR2DeviceInfoHex = null;
            try {
                method_getR2DeviceInfoHex = WifiP2pWfdInfo.class.getMethod("getR2DeviceInfoHex");
            } catch (Exception e) {
                Log.e(TAG, "Failed to get WifiP2pWfdInfo.getR2DeviceInfoHex() method", e);
            }
            sMethod_getR2DeviceInfoHex = method_getR2DeviceInfoHex;

            Method method_setWfdR2Device = null;
            try {
                method_setWfdR2Device = WifiP2pWfdInfo.class.getMethod("setWfdR2Device", int.class);
            } catch (Exception e) {
                Log.e(TAG, "Failed to get WifiP2pWfdInfo.setWfdR2Device(int) method", e);
            }
            sMethod_setWfdR2Device = method_setWfdR2Device;

            Log.d(TAG, "WifiP2pWfdInfoHelper initialized");
        }

        private final WifiP2pWfdInfo mWfdInfo;

        static WifiP2pWfdInfo createWifiP2pWfdInfo(int devInfo, int ctrlPort, int maxTput) {
            Log.v(TAG, "createWifiP2pWfdInfo");
            WifiP2pWfdInfo wfdInfo = null;
            if (sConstructor != null) {
                try {
                    wfdInfo = (WifiP2pWfdInfo)sConstructor.newInstance(devInfo, ctrlPort, maxTput);
                    return wfdInfo;
                } catch (Exception e) {
                    Log.e(TAG, "Failed to call WifiP2pWfdInfo(int, int, int)", e);
                }
            }
            // XXX: call WifiP2pWfdInfo default constructor, parser devInfo and set each flag
            Log.e(TAG, "Failed to create WifiP2pWfdInfo");
            return null;
        }

        static WifiP2pWfdInfoHelper wrap(WifiP2pWfdInfo wfdInfo) {
            return new WifiP2pWfdInfoHelper(wfdInfo);
        }

        private WifiP2pWfdInfoHelper(WifiP2pWfdInfo wfdInfo) {
            mWfdInfo = wfdInfo;
        }

        public boolean isWfdR2Supported() {
            Log.v(TAG, "isWfdR2Supported");
            if (mWfdInfo == null) {
                Log.e(TAG, "mWfdInfo is null");
                return false;
            }

            if (sMethod_isWfdR2Supported == null) {
                Log.e(TAG, "sMethod_isWfdR2Supported is null");
                return false;
            }

            try {
                boolean result = (Boolean)sMethod_isWfdR2Supported.invoke(mWfdInfo);
                return result;
            } catch (Exception e) {
                Log.e(TAG, "Failed to call WifiP2pWfdInfo.isWfdR2Supported() by reflection", e);
                return false;
            }
        }

        public String getDeviceInfoHex() {
            Log.v(TAG, "getDeviceInfoHex");
            if (mWfdInfo == null) {
                Log.e(TAG, "mWfdInfo is null");
                return "";
            }

            if (sMethod_getDeviceInfoHex == null) {
                Log.e(TAG, "sMethod_getDeviceInfoHex is null");
                return "";
            }

            try {
                String result = (String)sMethod_getDeviceInfoHex.invoke(mWfdInfo);
                return result;
            } catch (Exception e) {
                Log.e(TAG, "Failed to call WifiP2pWfdInfo.getDeviceInfoHex() by reflection", e);
                return "";
            }
        }

        public String getR2DeviceInfoHex() {
            Log.v(TAG, "getR2DeviceInfoHex");
            if (mWfdInfo == null) {
                Log.e(TAG, "mWfdInfo is null");
                return "";
            }

            if (sMethod_getR2DeviceInfoHex == null) {
                Log.e(TAG, "sMethod_getR2DeviceInfoHex is null");
                return "";
            }

            try {
                String result = (String)sMethod_getR2DeviceInfoHex.invoke(mWfdInfo);
                return result;
            } catch (Exception e) {
                Log.e(TAG, "Failed to call WifiP2pWfdInfo.getR2DeviceInfoHex() by reflection", e);
                return "";
            }
        }

        public void setWfdR2Device(int r2DeviceInfo) {
            Log.v(TAG, "setWfdR2Device " + r2DeviceInfo);
            if (mWfdInfo == null) {
                Log.e(TAG, "mWfdInfo is null");
                return;
            }

            if (sMethod_setWfdR2Device == null) {
                Log.e(TAG, "sMethod_setWfdR2Device is null");
                return;
            }

            try {
                sMethod_setWfdR2Device.invoke(mWfdInfo, r2DeviceInfo);
                return;
            } catch (Exception e) {
                Log.e(TAG, "Failed to call WifiP2pWfdInfo.isWfdR2Supported() by reflection", e);
                return;
            }
        }
    }
}

