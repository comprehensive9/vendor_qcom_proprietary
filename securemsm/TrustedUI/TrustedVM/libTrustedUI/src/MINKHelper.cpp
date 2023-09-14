/*========================================================================
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/******************************************************************************
 *                   Header Inclusions
*****************************************************************************/
#include <cstdint>

#include "impl_base.hpp"
#include "minkipc.h"
#include "object.h"
#include "proxy_base.hpp"
#include "TUILog.h"
#include "ITUICoreService.hpp"
#include "ITUISystemSession.hpp"

#define LOG_TAG "libTrustedUI"

static MinkIPC *TUIServiceMinkIPC = NULL;

#define TUI_CORESVC_SOCKET "/dev/socket/TUICoreSVC"

TUICoreService *getTUIService() {
  TUICoreService *tuiCoreSvc;
  Object remoteTUICoreSvcObj;

  TUIServiceMinkIPC = MinkIPC_connect(TUI_CORESVC_SOCKET, &remoteTUICoreSvcObj);
  if (!TUIServiceMinkIPC) {
    TUILOGE("Failed to connect to tui service");
    return NULL;
  } else if (Object_isNull(remoteTUICoreSvcObj)) {
    // Not sure about why this would happen, but it's possible.
    // Go ahead and close s_ipc so we can retry next time.
    MinkIPC_release(TUIServiceMinkIPC);
    TUIServiceMinkIPC = NULL;
    TUILOGE("Failed to get opener object from tui service");
    return NULL;
  }

  TUILOGE("Success in connecting to TUI service");
  tuiCoreSvc = new TUICoreService(remoteTUICoreSvcObj);
  return tuiCoreSvc;
}

void releaseTUIService(TUICoreService *tuiCoreSvc) {

    TUILOGD("%s:++", __func__);
    delete tuiCoreSvc;

    if (TUIServiceMinkIPC) {

        TUILOGD("Release socket connection with core service");
        MinkIPC_release(TUIServiceMinkIPC);
        TUIServiceMinkIPC = NULL;
    }
    TUILOGD("%s:--", __func__);
}
