/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include <memory>
#include "gtest/gtest.h"

#include "framework/Dispatcher.h"
#include "framework/Module.h"
#include "framework/Util.h"
#include "framework/SolicitedMessage.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/Util.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/android/LegacyHandlerSubModule.h"
#include "modules/android/LegacyHandlerMessage.h"

#define RIL_SHLIB
#include "telephony/ril.h"

using namespace std;

bool callback_executed;

extern "C" void
RIL_register (const RIL_RadioFunctions *callbacks);

void RIL_onRequestComplete(RIL_Token t, RIL_Errno e,
        void *response, size_t responselen) {
    Log::getInstance().d("[RIL_onRequestComplete]");
    callback_executed = true;
}
void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
        size_t datalen) {
    Log::getInstance().d("[RIL_onUnsolicitedResponse] unsol id = " + std::to_string(unsolResponse));
}
void RIL_requestTimedCallback (RIL_TimedCallback callback,
        void *param, const struct timeval *relativeTime) {
    Log::getInstance().d("[RIL_requestTimedCallback]");
}
void RIL_onRequestAck(RIL_Token t) {
    Log::getInstance().d("[RIL_onRequestAck]");
}

static struct RIL_Env s_rilEnv = {
    RIL_onRequestComplete,
    RIL_onUnsolicitedResponse,
    RIL_requestTimedCallback,
    RIL_onRequestAck
};

extern "C" const RIL_RadioFunctions *RIL_Init
(
  const struct RIL_Env *env,
  int argc,
  char **argv
);

extern "C" void onRequest
(
  int request,
  void *data,
  size_t datalen,
  RIL_Token t
);

/* Verify that dispatcher properly removes all the references to the module upon module
  destructor..
*/
const RIL_RadioFunctions *funcs = nullptr;

struct local_token
{
    uint32_t sequence;
    int req_id;
};

TEST(AndroidRilModuleTest, VerifyRilInit) {

    char *argv[] = {
        "-c", "0"
    };
    funcs = RIL_Init(&s_rilEnv, sizeof(argv)/sizeof(*argv), argv);

    ASSERT_NE(funcs, nullptr);
    ASSERT_GE(funcs->version, 12);
    ASSERT_NE(funcs->onRequest, nullptr);
    ASSERT_NE(funcs->onStateRequest, nullptr);
    ASSERT_NE(funcs->supports, nullptr);
    ASSERT_NE(funcs->onCancel, nullptr);
    ASSERT_NE(funcs->getVersion, nullptr);
    const char *version = funcs->getVersion();
    ASSERT_NE(version, nullptr);
    //threadSleep(30);
}

TEST(AndroidRilModuleTest, VerifyDispatchRequest) {

    int requested = 1;
    local_token *t = new local_token{ 1, RIL_REQUEST_RADIO_POWER };
    funcs->onRequest(RIL_REQUEST_RADIO_POWER, &requested, sizeof(requested), t);

    local_token *t2 = new local_token{ 2, RIL_REQUEST_GET_CELL_INFO_LIST };
    funcs->onRequest(RIL_REQUEST_GET_CELL_INFO_LIST, NULL, 0, t);

}

TEST(BasicFeatureSanity, TestLegacyHandlerSubModule) {
    std::thread::id payload1 = std::this_thread::get_id();
    std::thread::id payload2 = std::this_thread::get_id();
    qcril_request_params_type params1 = {QCRIL_DEFAULT_INSTANCE_ID,
                                        QCRIL_DEFAULT_MODEM_ID,
                                        0, nullptr, &payload1, sizeof(payload1),
                                        0, nullptr};
    qcril_request_params_type params2 = {QCRIL_DEFAULT_INSTANCE_ID,
                                        QCRIL_DEFAULT_MODEM_ID,
                                        0, nullptr, &payload2, sizeof(payload2),
                                        0, nullptr};

    // could not define function inside GTEST... use functor without capture list
    void (*test_legacy_handler)(const qcril_request_params_type *const, qcril_request_return_type *const) =
        [](const qcril_request_params_type *const params_ptr, qcril_request_return_type *const ret_ptr) {
            (void)ret_ptr;
            if (params_ptr && params_ptr->data)
              *((std::thread::id*)params_ptr->data) = std::this_thread::get_id();
        };

    // message1 with dedicated thread
    LegacyHandlerSubModule *subModule1= new LegacyHandlerSubModule();
    subModule1->init();
    // true means clean up the subModule after complete
    auto subMessage1 = std::make_shared<LegacyHandlerMessage>(&params1,
        test_legacy_handler, subModule1, true);
    subModule1->dispatch(subMessage1);

    // message2 with dedicated thread
    LegacyHandlerSubModule *subModule2= new LegacyHandlerSubModule();
    subModule2->init();
    // true means clean up the subModule after complete
    auto subMessage2 = std::make_shared<LegacyHandlerMessage>(&params2,
        test_legacy_handler, subModule2, true);
    subModule2->dispatch(subMessage2);

    threadSleep(2);

    // handler got executed
    ASSERT_NE(payload1, std::this_thread::get_id());
    ASSERT_NE(payload2, std::this_thread::get_id());
    // the handlers are in different thread
    ASSERT_NE(payload1, payload2);
    // dedicated thread
    //ASSERT_NE(payload1, Dispatcher::getInstance().getDispatcherLooperThreadId());
    //ASSERT_NE(payload2, Dispatcher::getInstance().getDispatcherLooperThreadId());
}
#endif
