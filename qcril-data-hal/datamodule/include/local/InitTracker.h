/**
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef INITTRACKER
#define INITTRACKER

namespace rildata {

 enum IWLANOperationMode {
   DEFAULT = 0,
   LEGACY,
   AP_ASSISTED
 };
 class InitTracker {
 private:
    bool mAuthServiceReady;
    bool mDsdServiceReady;
    bool mWdsServiceReady;
    bool mOttServiceReady;
    IWLANOperationMode mOperMode;
    bool mIWLANEnabled;
    bool mPartialRetryEnabled;
    bool mModemCapability;
 public:
    InitTracker() : mAuthServiceReady(false), mDsdServiceReady(false),
      mWdsServiceReady(false), mOttServiceReady(false), mIWLANEnabled(false),
      mPartialRetryEnabled(true), mModemCapability(false)
      {
#ifdef RIL_FOR_MDM_LE
        mOperMode = LEGACY;
#else
        mOperMode = DEFAULT;
#endif
      }
    bool allServicesReady() {
      return mAuthServiceReady && mDsdServiceReady && mWdsServiceReady;
    }
    bool isIWLANEnabled () {
      return mIWLANEnabled;
    }
    void setIWLANEnabled(bool enabled) {
      mIWLANEnabled = enabled;
    }
    bool isPartialRetryEnabled () {
      return mPartialRetryEnabled;
    }
    void setPartialRetryEnabled(bool enabled) {
      mPartialRetryEnabled = enabled;
    }
    bool isModeDetermined() {
      return mOperMode != DEFAULT;
    }
    bool isAPAssistMode() {
      return mOperMode == AP_ASSISTED;
    }
    void setIWLANMode(IWLANOperationMode mode)
    {
#ifndef RIL_FOR_MDM_LE
      mOperMode = mode;
#endif
    }
    void setModemCapability(bool capability) {
      mModemCapability = capability;
    }
    bool getModemCapability() {
      return mModemCapability;
    }
    bool getDsdServiceReady() {
      return mDsdServiceReady;
    }
    bool getAuthServiceReady() {
      return mAuthServiceReady;
    }
    bool getWdsServiceReady() {
      return mWdsServiceReady;
    }
    bool getOttServiceReady() {
      return mOttServiceReady;
    }
    void setDsdServiceReady(bool ready) {
      mDsdServiceReady = ready;
    }
    void setAuthServiceReady(bool ready) {
      mAuthServiceReady = ready;
    }
    void setWdsServiceReady(bool ready) {
      mWdsServiceReady = ready;
    }
    void setOttServiceReady(bool ready) {
      mOttServiceReady = ready;
    }
    void dump(std::string padding, std::ostream& os) {
      os << padding << std::boolalpha << "AuthServiceReady=" << mAuthServiceReady << endl;
      os << padding << std::boolalpha << "DsdServiceReady=" << mDsdServiceReady << endl;
      os << padding << std::boolalpha << "WdsServiceReady=" << mWdsServiceReady << endl;
      os << padding << "OperMode=" << mOperMode << endl;
      os << padding << std::boolalpha << "IWLANEnabled=" << mIWLANEnabled << endl;
      os << padding << std::boolalpha << "ModemCapability=" << mModemCapability << endl;
    }
  };

} //namespace

#endif
