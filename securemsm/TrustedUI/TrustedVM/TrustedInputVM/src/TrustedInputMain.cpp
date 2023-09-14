/*===================================================================================
  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <semaphore.h>

#include "CTouchInput.hpp"
#include "ITUICoreService.hpp"
#include "ITUICoreService.hpp"
#include "ITrustedInput.hpp"
#include "ITrustedInput_invoke.hpp"
#include "TUILog.h"
#include "TUIUtils.h"
#include "TouchInput.h"
#include "minkipc.h"

using namespace std;

/******************************************************************************
 *       Constant Definitions And Local Variables
*****************************************************************************/
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TUIVMInput::TrustedInputMain"

static const string coreServiceSocket = "/dev/socket/TUICoreSVC";
static Object TUICoreServiceObj = Object_NULL;
static Object TouchInputObj = Object_NULL;
static MinkIPC *MinkIPCHandle = nullptr;
static unique_ptr<ITUICoreService> mCoreService = nullptr;
static sem_t sigTerm;

/* Description : This API creates a CTouchInput class object pointer
* as the object context and TouchInput mink interface as the invoke.
*
* Out : objOut : Returns the object with invoke and context.
*
* Return : Object_ERROR_KMEM,
*          ITrustedInput::SUCCESS
*/
int32_t CTouchInput_open(Object *objOut)
{
    int32_t ret = ITrustedInput::SUCCESS;
    CTouchInput *me = new CTouchInput();
    if (me == nullptr) {
        TUILOGE("%s::%d - Error: Creating TouchInput pointer object Failed!", __func__, __LINE__);
        ret = Object_ERROR_KMEM;
        goto errorExit;
    }
    *objOut = (Object){ImplBase::invoke, me};
errorExit:
    return ret;
}

/* Description : This API release the MinkIPC handle along
* with TUICoreServiceObj and TouchInputObj.
*/

void cleanUpTrustedInput()
{
    TUILOGE("%s: ++ ", __func__);

    Object_ASSIGN_NULL(TouchInputObj);
    Object_ASSIGN_NULL(TUICoreServiceObj);

    if (MinkIPCHandle != nullptr) {
        MinkIPC_release(MinkIPCHandle);
        MinkIPCHandle = nullptr;
    }

}

void SignalHandler(int signalNum) {

    TUILOGE("%s::%d - TrustedInputService died, received signal:%d",  __func__,
            __LINE__, signalNum);
    //Handle clean up in case of abort
    TrustedInput* inputHandle = new TrustedInput(TouchInputObj);
    if (inputHandle) {
        TUILOGE("%s::%d - Terminate touch session if any", __func__, __LINE__);
        inputHandle->terminate();
    }
    cleanUpTrustedInput();
    sem_post(&sigTerm);
    // terminate program
    exit(signalNum);
}

void registerSignalHandler() {
  struct sigaction action;
  //Register signal handler
  memset(&action, 0, sizeof(action));
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;
  action.sa_handler = SignalHandler;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGABRT, &action, NULL);
  sigaction(SIGSEGV, &action, NULL);
}

/******************************************************************************
 *       Main Entry to TouchInput
*****************************************************************************/

int main()
{
    int32_t ret = ITrustedInput::SUCCESS;

    MinkIPC *MinkIPCHandle =
        MinkIPC_connect(coreServiceSocket.c_str(), &TUICoreServiceObj);

    TUI_CHECK_COND(MinkIPCHandle != nullptr, Object_ERROR);
    TUI_CHECK_COND(!Object_isNull(TUICoreServiceObj), Object_ERROR);

    TUILOGE("%s::%d - Successfully connected to TUI Coreservice",
           __func__, __LINE__);
    mCoreService = make_unique<TUICoreService>(TUICoreServiceObj);
    TUI_CHECK_COND(mCoreService != nullptr, Object_ERROR);

    ret = CTouchInput_open(&TouchInputObj);
    TUI_CHECK_COND(!Object_isNull(TouchInputObj), Object_ERROR_KMEM);

    //TouchInputObj's lifetime is forever, as long as InputService is alive
    ret = mCoreService->registerInputService(CTouchInput_UID, TouchInputObj);
    TUI_CHECK_COND(ret == ITrustedInput::SUCCESS, ret);
    TUILOGE("%s::%d - Successfully registered input service to TUI CoreService",
           __func__, __LINE__);

    sem_init(&sigTerm, 0, 0);
    registerSignalHandler();
    sem_wait(&sigTerm);

errorExit:
    cleanUpTrustedInput();
    return ret;
}
