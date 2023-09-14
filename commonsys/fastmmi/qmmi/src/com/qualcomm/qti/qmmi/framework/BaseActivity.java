 /*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.view.KeyEvent;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;
import com.qualcomm.qti.qmmi.model.CaseServiceManager;

import java.io.Serializable;

public abstract class BaseActivity extends Activity implements BaseContract.View, View.OnClickListener,Serializable {

    private final static String TAG = "BaseActivity";
    protected abstract int getLayoutId();

    public BasePresenter mPresenter;
    public TestCase mTestCase;
    public TextView mTextView;
    public LinearLayout mLinearLayout;
    public Button mBtPass;
    public Button mBtFail;

    public final class BaseActivityHandler extends Handler{
        public BaseActivityHandler(Looper looper){
            super(looper);
        }

        @Override
        public void handleMessage(Message msg){
            switch(msg.what){
                case CaseServiceManager.MSG_UPDATE_CASE_RESULT_TO_UI:
                    Intent intentResult = (Intent)msg.obj;
                    Bundle bundleResult = intentResult.getExtras();
                    mPresenter.updateView(bundleResult);
                    break;
                case CaseServiceManager.MSG_UPDATE_FOREGROUND_CASE_STATE_TO_UI:
                    Intent intentState = (Intent)msg.obj;
                    Bundle bundleState = intentState.getExtras();
                    updateResult(bundleState.getInt(Utils.BUNDLE_KEY_RESULT));
                    break;
            }
        }
    };

    public void updateResult(int state) {
        mPresenter.updateCaseResult(mTestCase, state);

        //update the screen background to indicate
        LogUtils.logi( "updateResult, state:" + state);
        if (mLinearLayout != null && state == TestCase.STATE_PASS) {
            mLinearLayout.setBackgroundColor(getResources().getColor(R.color.pass));
        } else if (mLinearLayout != null && state == TestCase.STATE_FAIL) {
            mLinearLayout.setBackgroundColor(getResources().getColor(R.color.fail));
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LogUtils.logi( "on create");

        //Keep screen on.
        Utils.keepScreenOn(this);

        setContentView(getLayoutId());
        mBtPass = (Button) findViewById(R.id.assertTrueButton);
        mBtPass.setOnClickListener(this);
        mBtFail = (Button) findViewById(R.id.assertFalseButton);
        mBtFail.setOnClickListener(this);
        mTextView = (TextView) findViewById(R.id.case_detail_title);
        if ( mTextView != null){
            mTextView.setMovementMethod(ScrollingMovementMethod.getInstance());
        }
        mLinearLayout = (LinearLayout) findViewById(R.id.case_top);
        mPresenter = new BasePresenter(this, getApplicationContext());

        String caseName = getIntent().getExtras().getString(Utils.BUNDLE_KEY_CASE_NAME);
        if (caseName != null) {
            LogUtils.logi( "save activity for testcase:" + caseName + ",this:" + this);
            mTestCase = mPresenter.getTestCaseByName(caseName);
            if (mTestCase != null)
                mTestCase.setActivity(this);
                mTestCase.setRunning(true);
        }

        int resId = getResources().getIdentifier(mTestCase.getName(), "string", getPackageName());
        setTitle(getResources().getText(resId));

        BaseActivityHandler baHandler = new BaseActivityHandler(Looper.getMainLooper());
        CaseServiceManager.getInstance().setBaseActivityHandler(baHandler);
        LogUtils.logi( "onCreate key:" + mTestCase.getGroup() + ",caseName:" + mTestCase.getName());
    }

    @Override
    protected void onResume() {
        LogUtils.logi( "on onResume");

        super.onResume();

        mTestCase.setLaunchMode(TestCase.LAUNCH_MODE_FOREGROUND);
        mPresenter.runCase(mTestCase);
    }

    @Override
    protected void onPause() {
        super.onPause();
        /***stop case*/
        mPresenter.stopCase(mTestCase);
    }


    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        mTestCase.setRunning(false);
        CaseServiceManager.getInstance().setBaseActivityHandler(null);
        super.onDestroy();
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.assertTrueButton) {
            feedbackResult(TestCase.STATE_PASS);
        } else if (view.getId() == R.id.assertFalseButton) {
            feedbackResult(TestCase.STATE_FAIL);
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK ){
            /***finish activity*/
            updateResult(mTestCase.getResult());
            setResult(RESULT_OK);
            finish();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    }

    public void feedbackResult(int resultId) {
        LogUtils.logi( "feedbackResult,caseName:" + mTestCase.getName() + " status:" + resultId);
        /***stop case*/
        mPresenter.stopCase(mTestCase);

        /***update result*/
        updateResultForCase(mTestCase.getName(), resultId);

        /***finish activity*/
        setResult(RESULT_OK);
        finish();
    }


    public void updateView(Bundle bundle) {
        String caseName = bundle.getString(Utils.BUNDLE_KEY_CASE_NAME);
        String message  = bundle.getString(Utils.BUNDLE_KEY_MESSAGE);

        //check if current test case is the same case as the one who update view.
        //Only if the same, can update view.
        LogUtils.logi( "update view case name : " + caseName);
        LogUtils.logi( "update view case name : " + mTestCase.getName());
        if ( caseName != null && caseName.equals(mTestCase.getName())) {
            mTextView.setText(message);
        }
    }

    public void updateResultForCase(String caseName, int state) {
        Intent intent = new Intent();
        Bundle bundle = new Bundle();
        bundle.putInt(Utils.BUNDLE_KEY_RESULT, state);
        bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, caseName);
        intent.putExtras(bundle);
        CaseServiceManager.getInstance().sendMsgForMainActivityHander(CaseServiceManager.MSG_UPDATE_BACKGROUND_CASE_STATE, intent);
    }

}
