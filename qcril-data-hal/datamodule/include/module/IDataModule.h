/**
* Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef IDATAMODULE
#define IDATAMODULE
#include "framework/Module.h"

namespace rildata {
/**
    IDataModule is the public interface to create Data Module
*/
class IDataModule : public Module {
 public:
  static IDataModule* makeModule();
  virtual void init() = 0;
  virtual ~IDataModule() {}
  IDataModule(const IDataModule&) =delete;
  IDataModule& operator=(const IDataModule&) =delete;
  // void setImsDataModuleInterface(
  //     std::shared_ptr<ImsDataModuleInterface> imsDataModuleInterface);
protected:
  IDataModule() = default;

};

#if (defined (QMI_RIL_UTF) || defined(RIL_FOR_MDM_LE))
void dump_data_module(int fd);
#endif

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_data_module_cleanup();
#endif

} //namespace


#endif