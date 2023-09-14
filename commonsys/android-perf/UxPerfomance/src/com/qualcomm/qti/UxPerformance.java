/*
 * Copyright (c) 2015,2017,2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2011-2014, The Linux Foundation. All rights reserved.
 */

package com.qualcomm.qti;

import android.util.Log;
import java.util.ArrayList;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.RandomAccessFile;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import android.util.Slog;
import java.io.FileDescriptor;
import java.io.IOException;
import java.lang.reflect.Method;
import android.os.Trace;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.Context;
import static android.os.PowerManager.ACTION_DEVICE_IDLE_MODE_CHANGED;
import android.content.pm.IPackageManager;
import android.app.AppGlobals;
import android.os.SystemProperties;

public class UxPerformance
{
    private static final String TAG_TASKS = "UxPerf";

    private static boolean isUxPerfInitialized = false;
    private static boolean isUxPerfContextInitialized = false;
    private static boolean EnablePrefetch = false;
    private static boolean EnableDebug = false;
    private static boolean EnablePAppsSpeed = false;
    private DexPrefetchThread PreDexThread = null;

    private DozeReceiver mDozeReceiver;
    private static boolean dozeSetup = false;
    private static Object mUxPerf;
    private static Method sUxeTrigFunc;
    private static Method sUxeEventFunc;
    private static IPackageManager pm;
    private PAppsSpeedThread pAppsThread;
    private static final int UXE_SPEED_ADD = 11;
    private static final int UXE_SPEED_DEL = 12;
    private static final int UXE_SPEED_TRIGGER = 13;

    /** @hide */
    public UxPerformance() {
        if (isUxPerfInitialized || isUxPerfContextInitialized) return;
        QLogD( "UX Perf module initialized");

        try {
              Class sPerfClass = Class.forName("com.qualcomm.qti.Performance");
              Class[] argClasses = new Class[] {String.class, String.class};
              Method sPerfGetPropFunc = sPerfClass.getMethod("perfGetProp", argClasses);
              Object mPerf = sPerfClass.newInstance();
              Object retVal = sPerfGetPropFunc.invoke(mPerf, "vendor.perf.iop_v3.enable", "false");
              EnablePrefetch = Boolean.parseBoolean(retVal.toString());
              Object retVal_2 = sPerfGetPropFunc.invoke(mPerf, "vendor.perf.iop_v3.enable.debug",
                                                               "false");
              EnableDebug = Boolean.parseBoolean(retVal_2.toString());
              isUxPerfInitialized = true;

        } catch(Exception e) {
              Log.e(TAG_TASKS, "Couldn't load Performance Class");
        }
    }
    public UxPerformance(Context context) {
        if (isUxPerfContextInitialized) return;
        QLogD( "UX Perf module initialized w/ context");

        try {
              Class sPerfClass = Class.forName("com.qualcomm.qti.Performance");
              Class[] argClasses = new Class[] {String.class, String.class};
              Method sPerfGetPropFunc = sPerfClass.getMethod("perfGetProp", argClasses);
              argClasses = new Class[] {int.class};
              sUxeTrigFunc = sPerfClass.getMethod("perfUXEngine_trigger", argClasses);
              argClasses = new Class[] {int.class, int.class, String.class, int.class,
                                        String.class};
              sUxeEventFunc = sPerfClass.getMethod("perfUXEngine_events", argClasses);
              mUxPerf = sPerfClass.newInstance();
              if (!isUxPerfInitialized) {
                  Object retVal = sPerfGetPropFunc.invoke(mUxPerf,
                          "vendor.perf.iop_v3.enable", "false");
                  EnablePrefetch = Boolean.parseBoolean(retVal.toString());
                  Object retVal_2 = sPerfGetPropFunc.invoke(mUxPerf,
                          "vendor.perf.iop_v3.enable.debug", "false");
                  EnableDebug = Boolean.parseBoolean(retVal_2.toString());
              }
              Object retVal_3 = sPerfGetPropFunc.invoke(mUxPerf, "vendor.iop.enable_speed",
                                                                 "false");
              EnablePAppsSpeed = Boolean.parseBoolean(retVal_3.toString());

              if (!UxPerformance.dozeSetup && EnablePAppsSpeed) {
                  IntentFilter pfilter = new IntentFilter();
                  pfilter.addAction(ACTION_DEVICE_IDLE_MODE_CHANGED);
                  pfilter.addAction(Intent.ACTION_SCREEN_ON);
                  pfilter.addAction(Intent.ACTION_SCREEN_OFF);
                  this.mDozeReceiver = new DozeReceiver();
                  context.registerReceiver(this.mDozeReceiver, pfilter);
                  UxPerformance.dozeSetup = true;
              }
              pm = AppGlobals.getPackageManager();
              isUxPerfContextInitialized = true;
        } catch(Exception e) {
              Log.e(TAG_TASKS, "Couldn't load Performance Class w/ context");
        }
    }
    /** @hide */
    private void QLogI(String msg)
    {
        if(EnableDebug) Slog.i(TAG_TASKS, msg);
        return;
    }
     /** @hide */
    private void QLogD(String msg)
    {
        if(EnableDebug) Slog.d(TAG_TASKS, msg);
        return;
    }
    /** @hide */
    private void QLogV(String msg)
    {
        if(EnableDebug) Slog.v(TAG_TASKS, msg);
        return;
    }
    /** @hide */
    private void QLogE(String msg)
    {
        if(EnableDebug) Slog.e(TAG_TASKS, msg);
        return;
    }


    /* The following are the PerfLock API return values*/
    /** @hide */ public static final int REQUEST_FAILED = -1;
    /** @hide */ public static final int REQUEST_SUCCEEDED = 0;

    /* The following functions are the UX APIs*/
    /** @hide */
    public int perfIOPrefetchStart(int PId, String PkgName, String CodePath)
    {
        if(EnablePrefetch)
        {
           QLogI("DexPrefetchThread Feature is Enable ");
           PreDexThread = new DexPrefetchThread(CodePath);
            if(PreDexThread != null)
                (new Thread(PreDexThread)).start();
        }
        else
        {
            QLogI("DexPrefetchThread Feature is disabled ");
            return REQUEST_FAILED;
        }
        return REQUEST_SUCCEEDED;
    }

    private class DexPrefetchThread implements Runnable {
        public String CodePath;
        public DexPrefetchThread(String CodePath) {
            this.CodePath = CodePath;
        }

        public void LoadFiles(String path,String FileName)
        {
            String[] Files = {".art", ".odex", ".vdex"};
            QLogD(" LoadFiles() path is " + path);
            for ( int i = 0; i< Files.length; i++ )
            {
                File MyFile = new File(path + FileName + Files[i]);
                if(MyFile.exists())
                {
                    try {
                        FileChannel fileChannel = new RandomAccessFile(MyFile, "r").getChannel();
                        MappedByteBuffer buffer = fileChannel.map(FileChannel.MapMode.READ_ONLY, 0, fileChannel.size());
                        if(EnableDebug)
                            QLogD( "Before Is Buffer Loaded " + buffer.isLoaded());
                        QLogD( "File is " + path + FileName + Files[i]);
                        buffer.load();
                        if(EnableDebug)
                            QLogD( "After Is Buffer Loaded " + buffer.isLoaded());
                    } catch (FileNotFoundException e) {
                        QLogE( "DexPrefetchThread Can not find file " + FileName + Files[i] + "at " + path);
                    } catch (IOException e) {
                        QLogE( "DexPrefetchThread IO Error file " + FileName + Files[i] + "at " + path);
                    }
                }
            }
        }
        public void run() {
            //Fix me : This loop is added just to comply with thread run functions.
            while(true) {
                Trace.traceBegin(Trace.TRACE_TAG_ACTIVITY_MANAGER, "DexPrefetchThread");
                if ( CodePath.startsWith("/data") )
                {
                    //Check for 32/64 bit apps
                    QLogD( "Data pkg ");
                    if((new File(CodePath + "/oat/arm64/")).exists())
                    {
                        LoadFiles(CodePath + "/oat/arm64/", "base");
                    }
                    else if((new File(CodePath + "/oat/arm/")).exists())
                    {
                        LoadFiles(CodePath + "/oat/arm/", "base");
                    }
                }
                else
                {
                    // Get the name of package
                    QLogD( "system/vendor pkg ");
                    String[] SplitPath = CodePath.split("/");
                    String PkgName = SplitPath[SplitPath.length-1];
                    QLogD( "PKgNAme : " + PkgName);

                    //Check for 32/64 bit apps
                    if((new File(CodePath + "/oat/arm64/")).exists())
                    {
                        LoadFiles(CodePath + "/oat/arm64/", PkgName);
                    }
                    else if((new File(CodePath + "/oat/arm/")).exists())
                    {
                        LoadFiles(CodePath + "/oat/arm/", PkgName);
                    }
                }
                Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
                return;

            }
        }
    }

    private class DozeReceiver extends BroadcastReceiver {

        private boolean screenOff = false;
        private boolean pAppsThreadCreated = false;

        public void onReceive(Context context, Intent intent) {
            switch (intent.getAction()) {
                case ACTION_DEVICE_IDLE_MODE_CHANGED:
                    try {
                        QLogD("Calling PMS broadcast receiver. Entered device idle mode");
                        if(screenOff && !pAppsThreadCreated) {
                            pAppsThread = new PAppsSpeedThread();
                            if (pAppsThread != null) {
                                (new Thread(pAppsThread)).start();
                            }
                            pAppsThreadCreated = true;
                        }
                    } catch (Exception e) {
                        Log.d(TAG_TASKS, "Caught exception: " + e);
                    }
                    break;
                case Intent.ACTION_SCREEN_OFF:
                    screenOff = true;
                    break;
                case Intent.ACTION_SCREEN_ON:
                    screenOff = false;
                    if (pAppsThread != null) {
                        pAppsThread.setStopThread();
                    }
                    pAppsThreadCreated = false;
                    break;
                default:
                    break;
            }
        }
    }

    private class PAppsSpeedThread implements Runnable {

        private boolean stopPAppsThread = false;

        public void setStopThread() {
            this.stopPAppsThread = true;
        }

        public void run() {
            try {
                boolean s_low = false;
                boolean checkProfiles = SystemProperties.getBoolean("dalvik.vm.usejitprofiles",
                                                                     false);
                String defaultFilter = SystemProperties.get("pm.dexopt.bg-dexopt");
                s_low = pm.isStorageLow();
                if (defaultFilter.equals("speed"))
                    return;
                Object retVal = sUxeTrigFunc.invoke(mUxPerf, UXE_SPEED_TRIGGER);
                String ret = retVal.toString();
                if (ret == null || ret.equals(""))
                   return;
                QLogD( "Obtained PApps: " + ret);
                String[] pApps_del = ret.split("=");
                for (int i = 1; i < pApps_del.length; i++) {
                    if (stopPAppsThread) {
                        break;
                    }
                    boolean tmp = pm.performDexOptMode(pApps_del[i], checkProfiles, "speed-profile",
                                                       true, true, null);
                    QLogD("Removed pApp: " + pApps_del[i] + " to speed mode. Result: " + tmp);
                    sUxeEventFunc.invoke(mUxPerf, UXE_SPEED_DEL, -1, pApps_del[i], -1, null);
                }

                String[] pApps_add = pApps_del[0].split("/");
                for (int i = 0; i < pApps_add.length; i++) {
                    if(s_low || stopPAppsThread) {
                        break;
                    }
                    boolean tmp = pm.performDexOptMode(pApps_add[i], checkProfiles, "speed",
                                                       true, true, null);
                    QLogD("Added pApp: " + pApps_add[i] + " to speed mode. Result: " + tmp);
                    if (tmp)
                        sUxeEventFunc.invoke(mUxPerf, UXE_SPEED_ADD, -1, pApps_add[i], -1, null);
                    else
                        sUxeEventFunc.invoke(mUxPerf, UXE_SPEED_DEL, -1, pApps_add[i], -1, null);
                }
            } catch (Exception e) {
                Log.e(TAG_TASKS, "Exception caught. " + e);
            }
        }
    }
}
