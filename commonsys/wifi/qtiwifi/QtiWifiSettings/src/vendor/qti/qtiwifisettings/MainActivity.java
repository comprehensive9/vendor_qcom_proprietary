/*
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package vendor.qti.qtiwifisettings;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import android.util.Log;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;
import android.os.RemoteException;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.IOException;

import android.net.wifi.WifiManager;
import com.qualcomm.qti.qtiwifi.QtiWifiManager;

public class MainActivity extends Activity implements View.OnClickListener {

    private static final String TAG = "QtiWifiSettingsApp";
    private Intent mServiceIntent;
    //button objects
    private Button buttonStart;
    private Button buttonStop;
    private WifiManager mWifiManager;
    private static QtiWifiManager mUniqueInstance = null;
    private FileOutputStream fileout;
    private OutputStreamWriter outputWriter;

    private QtiWifiManager.CsiCallback mCsiCallback = new QtiWifiManager.CsiCallback() {
        @Override
        public void onCsiUpdate(byte[] info) {
            StringBuilder strBuilder = new StringBuilder();
            for(byte val : info) {
                strBuilder.append(String.format("%02x", val&0xff));
            }
            Log.d(TAG, "onCsiUpdate csi info = " + info);
            try {
                outputWriter.write(strBuilder.toString());
                Log.i(TAG, "Successfully write into the file");
            } catch (IOException e) {
                Log.e(TAG, "Error while writing into the file");
            }
        }
    };

    private QtiWifiManager.ApplicationBinderCallback mApplicationCallback = new QtiWifiManager.ApplicationBinderCallback() {
        @Override
        public void onAvailable(QtiWifiManager qtiWifiManager) {
            Log.d(TAG, "onAvailable called");
            mUniqueInstance = qtiWifiManager;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        QtiWifiManager.initialize(this, mApplicationCallback);
        Log.i(TAG, "initialize called");

        setContentView(R.layout.content_main);
        //getting buttons from xml
        buttonStart = (Button) findViewById(R.id.buttonStart);
        buttonStop = (Button) findViewById(R.id.buttonStop);

        //attaching onclicklistener to buttons
        buttonStart.setOnClickListener(this);
        buttonStop.setOnClickListener(this);
        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
    }

    private void showMessage(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onClick(View view) {
        if (view == buttonStart) {
            //starting service
            if (mUniqueInstance != null) {
                if (!mWifiManager.isWifiEnabled()) {
                    showMessage("Turn on Wifi before capturing CSI data");
                    Log.e(TAG, "Turn on Wifi before capturing CSI data");
                } else if (mWifiManager.isWifiEnabled()) {
                    showMessage("CSI start until user stops");
                    mUniqueInstance.startCsi(mCsiCallback, null);
                    try {
                        fileout=openFileOutput("mytextfile.txt", MODE_PRIVATE);
                        outputWriter=new OutputStreamWriter(fileout);
                        showMessage("File open successfully!");
                    } catch (Exception e) {
                        Log.e(TAG, "Failed to open file");
                    }
                    mUniqueInstance.doDriverCmd("CSI start 0");
                }
            } else {
                showMessage("uniqueInstance is null");
                Log.e(TAG, "Failed to get QtiWifiManager instance");
            }
        } else if (view == buttonStop) {
            //stopping service
            if (mUniqueInstance != null) {
                showMessage("CSI stop");
                mUniqueInstance.doDriverCmd("CSI stop");
                mUniqueInstance.stopCsi(mCsiCallback);
                try {
                    outputWriter.close();
                    Log.i(TAG, "Succseefully close the file");
                } catch (IOException e) {
                    Log.e(TAG, "Failed to close the file");
                }
            } else {
                showMessage("uniqueInstance is null");
                Log.e(TAG, "Failed to get QtiWifiManager instance");
            }
        }
    }

    public static void unbindService(Context context) {
        QtiWifiManager.unbindService(context);
    }

    @Override
    protected void onDestroy() {
        Log.i("MAINACT", "onDestroy!");
        super.onDestroy();
        try {
            unbindService(getApplicationContext());
        } catch (IllegalArgumentException e) {
            Log.e(TAG,"Illegal Argument Exception ",e);
        }
    }

    private void quitApplicationWithAlert(int title, int message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(title)
                .setMessage(message)
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setCancelable(false)
                .setNeutralButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which)  {
                        // Exit the application.
                        finish();
                    }
                });
        AlertDialog alert = builder.create();
        alert.setCanceledOnTouchOutside(false);
        alert.show();
    }
}
