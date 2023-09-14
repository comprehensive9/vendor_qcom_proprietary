/* 
 * Copyright (c) 2014, 2017,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.smq.ui;

import android.app.Activity;
import android.app.NotificationManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.Html;
import android.text.method.LinkMovementMethod;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.smq.R;
import com.qualcomm.qti.smq.receiver.QtiNotificationReceiver;
import com.qualcomm.qti.smq.util.OptInOutHandler;
import com.qualcomm.qti.smq.util.OptInOutProductConstants;
import com.qualcomm.qti.smq.ui.util.SMQUiUtils;

/**
 * Activity used to display the user settings.
 *
 */
public class QtiFeedbackActivity extends AppCompatActivity {

    /** The Constant LOG_TAG. */
    private static final String LOG_TAG = "QCC-TR-UI";
    /**
     * The handle to the helper to get and update DB values.
     */
    private OptInOutHandler mDbHandler = null;
	
	private final String sManufacturer = "QUALCOMM";
    private final String sModel = "for arm";

    private static String mSharedPreferences = "SharedPreferences";
    private static final String enabledModeKey = "enabledMode";

    private static int defaultOptIn = 1;   //1 : default behavior, 2 : all data collections opted-out
    private static int defaultOptOut = 2;
    private static int provisionedCheck = 3;
    private static int defaultAllDisabled = 4;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (! SMQUiUtils.isVendorEnhancedFramework())
        {
            finish();
            return;
        }
        Log.d(LOG_TAG,"QtiFeedbackActivity onCreate");
        disappearNotification(this);
        

        setContentView(R.layout.activity_qtifeedback_setting);
        Toolbar toolbar = (Toolbar)findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.snapdragon_settings_title);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        mDbHandler = new OptInOutHandler(QtiFeedbackActivity.this);


        final CheckBox dataSettingCheckbox = (CheckBox) findViewById(R.id.dataSettings);
        String dataSettingSummary;

        Context sharedContext = this.createDeviceProtectedStorageContext();
        SharedPreferences enabledSet = sharedContext.getSharedPreferences(mSharedPreferences, Context.MODE_PRIVATE );
        int enabled = enabledSet.getInt(enabledModeKey, 1);
        Log.d(LOG_TAG,"QtiFeedbackActivity enabled : "+enabled);
        if(enabled == 0){
            Toast.makeText(this,getResources().getString(R.string.disabledStr), Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        final TextView textView = (TextView)findViewById(R.id.textView1);
        String manufacturer = Build.MANUFACTURER;
        String model = Build.MODEL;
        PackageManager pm = this.getPackageManager();
        Boolean bExtVersion = false;
        try{
            String appVerName = pm.getPackageInfo(this.getPackageName(), 0).versionName;
            if(appVerName != null){
                Log.d(LOG_TAG,"QtiFeedbackActivity appVerName : "+appVerName);
                if(appVerName.startsWith("QCC-TR-UI-EXT")){
                    bExtVersion = true;
                }
            }
        }catch (Exception e){
            Log.e(LOG_TAG,"pm exception : "+e.getMessage());
        }
        Log.d(LOG_TAG,"QtiFeedbackActivity manufacturer : "+manufacturer+" / model : "+model+" / bExtVersion : "+bExtVersion);
        if(sManufacturer.equals(manufacturer) && model != null && model.contains(sModel) && bExtVersion) {
            dataSettingSummary = getResources().getString(R.string.data_setting_summary_ext);
            dataSettingCheckbox.setText(Html.fromHtml(dataSettingSummary, Html.FROM_HTML_MODE_LEGACY));
            textView.setText(Html.fromHtml(getResources().getString(R.string.qti_non_commercial),Html.FROM_HTML_MODE_LEGACY));
        }else{
            dataSettingSummary = getResources().getString(R.string.data_setting_body);
            dataSettingCheckbox.setText(Html.fromHtml(dataSettingSummary, Html.FROM_HTML_MODE_LEGACY));
        }

        dataSettingCheckbox.setClickable(true);
        dataSettingCheckbox.setMovementMethod(LinkMovementMethod.getInstance());


        int dataSettingsValue = mDbHandler.getTypeOptInOut(OptInOutProductConstants.QTR_KEY_DATA_PREFERENCE, 0);
        int optMode = mDbHandler.getDBFromKey(OptInOutProductConstants.KEY_OPERATION_MODE);
        Log.i(LOG_TAG, "dbHandler.getTypeOptInOut dataSettingsValue : "+ dataSettingsValue+" , optMode : "+optMode);
        if((optMode == defaultOptOut) &&
                ((dataSettingsValue == mDbHandler.NOT_INITIAL)||(dataSettingsValue == mDbHandler.SET_OFF))){
            Log.d(LOG_TAG,"QtiFeedbackActivity - opmode 2 and notInitial/setOff");
            dataSettingCheckbox.setChecked(false);
        }else{
            if((dataSettingsValue == mDbHandler.SET_OFF)){
                Log.d(LOG_TAG,"QtiFeedbackActivity - normal opmode and setOff");
                dataSettingCheckbox.setChecked(false);
            }else{
                Log.d(LOG_TAG,"QtiFeedbackActivity - normal opmode and notInitial/setOn");
                dataSettingCheckbox.setChecked(true);
            }
        }

        dataSettingCheckbox
                .setOnCheckedChangeListener(new OnCheckedChangeListener() {

                    @Override
                    public void onCheckedChanged(final CompoundButton buttonView,
                            final boolean isChecked) {
                        Log.d(LOG_TAG,"QtiFeedbackActivity - onCheckedChanged isChecked : "+isChecked);
                        saveDataSettings(isChecked);
                     }
                });
    }
    @Override
    public void onResume(){
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(LOG_TAG,"QtiFeedbackActivity onPause");
    }

    @Override
    protected void onUserLeaveHint() {
        super.onUserLeaveHint();
        Log.d(LOG_TAG,"QtiFeedbackActivity onUserLeaveHint");
        final CheckBox dataSettingCheckbox = (CheckBox) findViewById(R.id.dataSettings);
        CheckBoxClicked(dataSettingCheckbox);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Log.d(LOG_TAG,"QtiFeedbackActivity onBackPressed");
        final CheckBox dataSettingCheckbox = (CheckBox) findViewById(R.id.dataSettings);
        CheckBoxClicked(dataSettingCheckbox);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        if(item.getItemId() == android.R.id.home){
            Log.d(LOG_TAG,"QtiFeedbackActivity toolbar selected 2");
            onBackPressed();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    /**
     * Save location settings.
     * @param value
     */
    private void saveLocationSettings(final boolean value) {
        //
    }

    /**
     * Save data settings.
     * @param value
     */
    private void saveDataSettings(final boolean value) {
        mDbHandler.changeSettingsAsync(
                OptInOutProductConstants.QTR_KEY_DATA_PREFERENCE, value);
    }

    private void CheckBoxClicked(View v){
        CheckBox clickedItem = (CheckBox)v;
        Log.d(LOG_TAG,"QtiFeedbackActivity CheckBoxClicked");
        if(clickedItem.getId() == R.id.dataSettings){
            if(clickedItem.isChecked()){
                //checked by user
                Log.d(LOG_TAG,"QtiFeedbackActivity, clicked dataSettings ");
                saveDataSettings(true);
            } else {
                // determine if user is unchecked or not
                Log.d(LOG_TAG,"QtiFeedbackActivity, unchecked clicked button ");
                saveDataSettings(false);
            }
        }
    }

    private void disappearNotification(Context context){
        Log.d(LOG_TAG,"QtiFeedbackActivity, disappearNotification ");
        NotificationManager motificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        if(motificationManager!=null)
            motificationManager.cancel(QtiNotificationReceiver.QCC_UI_NOTIFICATION_ID);
    }

}
