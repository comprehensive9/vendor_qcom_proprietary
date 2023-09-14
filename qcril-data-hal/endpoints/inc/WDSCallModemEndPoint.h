/******************************************************************************
#  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef WDSCALLMODEMENDPOINT
#define WDSCALLMODEMENDPOINT
#include <string>
#include "modules/qmi/ModemEndPoint.h"
#include "WDSCallModemEndPointModule.h"
#include "framework/Log.h"

class WDSCallModemEndPoint : public ModemEndPoint
{
private:
  std::string mModuleName;
  rildata::IpFamilyType_t mIpType;
  int mEpType;
  int mEpId;
  int mMuxId;
  int mCid;
  std::string mApnName;
  std::string mIfaceName;
  bool mReportLinkStateChange;

public:
  WDSCallModemEndPoint(std::string moduleName, int cid, std::string apnName,
    std::string ifaceName, rildata::IpFamilyType_t iptype) : ModemEndPoint(moduleName) {
    mModuleName = moduleName;
    mCid = cid;
    mApnName = apnName;
    mIfaceName = ifaceName;
    mIpType = iptype;
    mModule = new WDSCallModemEndPointModule(mModuleName, *this, mIpType);
    mModule->init();
    Log::getInstance().d("["+mModuleName+"]: xtor with name " + mModuleName);
  }
  ~WDSCallModemEndPoint() {
    Log::getInstance().d("["+mModuleName+"]: destructor");
    delete mModule;
    mModule = nullptr;
  }
#ifdef RIL_FOR_DYNAMIC_LOADING
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
#else
  void requestSetup(string clientToken, GenericCallback<string>* cb);
#endif
  rildata::IpFamilyType_t getIpType(void){return mIpType;};

  int getCid(void){return mCid;};

  std::string getApnName(void){return mApnName;};

  std::string getIfaceName(void){return mIfaceName;};

  void setBindMuxDataPortParams(int eptype, int epid, int muxid);

  void getBindMuxDataPortParams(int *eptype, int *epid, int *muxid);

  void releaseWDSCallEPModuleQMIClient();

  void setLinkStateChangeReport(bool report);

  void updateLinkStateChangeReport(void);

  bool goDormant(void);
};

#endif