/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include <memory>
#include "gtest/gtest.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/legacy.h"

#include "framework/Util.h"
#include "framework/ModuleLooper.h"
#include "framework/PolicyManager.h"
#include "framework/ThreadPoolManager.h"
#include "framework/SingleDispatchAndFamilyPairRestriction.h"

#include "telephony/ril.h"

using namespace std;

#if 0
TEST(BasicFeatureSanity, TestRegisteredMessageProcessing) {
    std::shared_ptr<RilRequestDialSyncMessage> msgPtr = createTestMsg();
    std::shared_ptr<string> sync_shared_response;
    Message::Callback::Status status = msgPtr->dispatchSync(sync_shared_response);
    ASSERT_TRUE(status == Message::Callback::Status::SUCCESS);
}
#endif

TEST(BasicFeatureSanity, TestSingleDispatchAndFamilyPairRestriction) {
    class TestFamilyPair1Message : public SolicitedMessage<string> {
    public:
      inline TestFamilyPair1Message():
            SolicitedMessage<string>(REG_MSG("TEST_PAIR1_MESSAGE"))
      {
        mName = "TEST_PAIR1_MESSAGE";
      }

      inline ~TestFamilyPair1Message() {}
      inline string dump() override {
        return mName + "{}";
      }
    };

    class TestFamilyPair2Message : public SolicitedMessage<string> {
    public:
      inline TestFamilyPair2Message():
            SolicitedMessage<string>(REG_MSG("TEST_PAIR2_MESSAGE"))
      {
        mName = "TEST_PAIR2_MESSAGE";
      }

      inline ~TestFamilyPair2Message() {}
      inline string dump() override {
        return mName + "{}";
      }
    };

    class TestOtherMessage : public SolicitedMessage<string> {
    public:
      inline TestOtherMessage():
            SolicitedMessage<string>(REG_MSG("TEST_OTHER_MESSAGE"))
      {
        mName = "TEST_OTHER_MESSAGE";
      }

      inline ~TestOtherMessage() {}
      inline string dump() override {
        return mName + "{}";
      }
    };

    class SampleModule : public Module {
    public:
      inline SampleModule() {
        mName = "SampleModule";
        mNumPair1MsgCame = 0;
        mNumPair2MsgCame = 0;
        mNumOtherMsgsCame = 0;
        mNumNewMessagesUnRead = 0;
        mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
        using std::placeholders::_1;
        mMessageHandler = {
          { REG_MSG("TEST_PAIR1_MESSAGE"),
            std::bind(&SampleModule::handlePair1Message, this, _1) },
          { REG_MSG("TEST_PAIR2_MESSAGE"),
            std::bind(&SampleModule::handlePair2Message, this, _1) },
          { REG_MSG("TEST_OTHER_MESSAGE"),
            std::bind(&SampleModule::handleOtherMessage, this, _1) },
        };
      }
      inline ~SampleModule() {}

      inline void init() {
        Module::init();
        std::vector<std::string> ring {
          "TEST_PAIR1_MESSAGE",
          "TEST_PAIR2_MESSAGE",
          "TEST_OTHER_MESSAGE",
        };
        auto rest = std::make_shared<SingleDispatchAndFamilyPairRestriction>();
        if (rest) {
          rest->setMessagePair("TEST_PAIR1_MESSAGE", "TEST_PAIR2_MESSAGE");
        }
        PolicyManager::getInstance().setMessageRestriction(ring, rest);
      }

#define HANLE_MESSAGE(msgCount) do { \
    { \
      usleep(5000); \
      std::unique_lock<std::mutex> lock(mLock); \
      mCv.wait_for(lock, std::chrono::seconds(2), [=] { return mNumNewMessagesUnRead == 0; }); \
      mNumNewMessagesUnRead++; \
      msgCount++; \
    } \
    mCv.notify_all(); \
  } while (0)

#define CONSUME_MESSAGE() do { \
    { \
      std::unique_lock<std::mutex> lock(mLock); \
      mCv.wait_for(lock, std::chrono::seconds(2), [=] {return mNumNewMessagesUnRead > 0; }); \
      mNumNewMessagesUnRead--; \
    } \
    mCv.notify_all(); \
  } while (0)

      inline void handlePair1Message(std::shared_ptr<Message> msg) {
        (void)msg;
        HANLE_MESSAGE(mNumPair1MsgCame);
      }
      inline void handlePair2Message(std::shared_ptr<Message> msg) {
        (void)msg;
        HANLE_MESSAGE(mNumPair2MsgCame);
      }
      inline void handleOtherMessage(std::shared_ptr<Message> msg) {
        (void)msg;
        HANLE_MESSAGE(mNumOtherMsgsCame);
      }

      inline int WaitNextAndGetNumbersOfPair1Message() {
        CONSUME_MESSAGE();
        return mNumPair1MsgCame;
      }
      inline int WaitNextAndGetNumbersOfPair2Message() {
        CONSUME_MESSAGE();
        return mNumPair2MsgCame;
      }
      inline int WaitNextAndGetNumbersOfOtherMessages() {
        CONSUME_MESSAGE();
        return mNumOtherMsgsCame;
      }
    private:
      std::condition_variable mCv;
      std::mutex              mLock;
      int                     mNumPair1MsgCame;
      int                     mNumPair2MsgCame;
      int                     mNumOtherMsgsCame;
      int                     mNumNewMessagesUnRead;
    }; // end of class definition

    SampleModule *module = new SampleModule();
    module->init();

    auto pair1Msg = std::make_shared<TestFamilyPair1Message>();
    auto pair2Msg = std::make_shared<TestFamilyPair2Message>();
    auto otherMsg = std::make_shared<TestOtherMessage>();

    // no callbacks, the informMessageFinished will be called after handler completed
    // dispatcher order: pair1, other, pair1, pair2, other, pair2
    pair1Msg->dispatch();
    otherMsg->dispatch();
    pair1Msg->dispatch();
    pair2Msg->dispatch();
    otherMsg->dispatch();
    pair2Msg->dispatch();

    // executed order should be: pair1, pair2, pair1, pair2, other, other
    ASSERT_EQ(1, module->WaitNextAndGetNumbersOfPair1Message());
    ASSERT_EQ(1, module->WaitNextAndGetNumbersOfPair2Message());
    ASSERT_EQ(2, module->WaitNextAndGetNumbersOfPair1Message());
    ASSERT_EQ(2, module->WaitNextAndGetNumbersOfPair2Message());
    ASSERT_EQ(1, module->WaitNextAndGetNumbersOfOtherMessages());
    ASSERT_EQ(2, module->WaitNextAndGetNumbersOfOtherMessages());

    delete module;
    module = nullptr;
}

TEST(BasicFeatureSanity, DISABLED_TestThreadPool) {
    std::thread::id payload1 = std::this_thread::get_id();
    std::thread::id payload2 = std::this_thread::get_id();
    // define the callback
    void (*func_cb1)(void* cb_data) = [](void* cb_data) {
        if (cb_data)
            *((std::thread::id*)cb_data) = std::this_thread::get_id();
        threadSleep(2);
    };
    void (*func_cb2)(void* cb_data) = [](void* cb_data) {
        if (cb_data)
            *((std::thread::id*)cb_data) = std::this_thread::get_id();
        threadSleep(4);
    };

    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb1, &payload1);
    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb2, &payload2);
    threadSleep(1);

    // handlers get executed and they are different threads for cb1 and cb2
    ASSERT_NE(payload1, std::this_thread::get_id());
    ASSERT_NE(payload2, std::this_thread::get_id());
    ASSERT_NE(payload1, payload2);
    // should be 2 threads totally, and no idle thread
    ASSERT_EQ(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 2);

    threadSleep(2);
    // should be one idle thread and not freed
    ASSERT_NE(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 2);

    // No task active. Only Idle thread left
    threadSleep(2);
    ASSERT_NE(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 1);

    // When enqueue one new task, the idle thread should be used directly
    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb1, &payload1);
    threadSleep(1);
    ASSERT_EQ(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 1);

    // test clean up threads
    (void)ThreadPoolManager::getInstance().scheduleExecution(func_cb2, &payload2);
    ThreadPoolManager::getInstance().cleanupThreads();
    ASSERT_EQ(ThreadPoolManager::getInstance().getIdleModule(), nullptr);
    ASSERT_EQ(ThreadPoolManager::getInstance().getTotalThreads(), 0);
}

#if 0
extern "C" void *qcril_malloc_adv ( size_t size, const char* func_name, int line_num);

TEST(LegacyQcril, TestQcrilMalloc) {
    char *buffer = (char *)qcril_malloc(10);
    ASSERT_TRUE(buffer != nullptr);
}
#endif
#endif
