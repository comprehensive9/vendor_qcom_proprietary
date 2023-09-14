/******************************************************************************
  @file    PerfThreadPool.h
  @brief   Implementation of PerfThreadPool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef __PERFTHREADPOOL_H__
#define __PERFTHREADPOOL_H__

#include <functional>
#include <vector>
#include <pthread.h>
#include <mutex>

using namespace std;

#define NUM_THREADS 4
#define MAX_NUM_THREADS 16
#define MAX_BUF_SIZE 16
#define FAILED -1
#define SUCCESS 0
#define ONE_THREAD 1

typedef enum {
    INVALID_STATE = -1,
    SLEEPING_STATE,
    RUNNING_STATE
}ThreadState_t;


typedef enum {
    REGULAR = 0,
    DEDICATED,
}ThreadType_t;

class ThreadPoolData {
    private:
        int mThreadId;
        pthread_t mThread;
        atomic<ThreadState_t> mState;
        atomic<ThreadType_t> mType;
        atomic<bool> mHasSlept;
        function<void()> *mFunc;
        atomic<bool> mGotTask;
        mutex mTaskAvailable;
        condition_variable mCondVar;
        mutex mStateMutex;
        ThreadPoolData();
        ThreadPoolData(int thread_id);
        ~ThreadPoolData();

        void wait();
        void signal();

        ThreadState_t getState() {
            return mState;
        }

        int getIfAvailable(ThreadType_t type);
        void setState(ThreadState_t state);

        int getIfAvailable() {
            return getIfAvailable(REGULAR);
        }

    public:
        friend class PerfThreadPool;
};

class PerfThreadPool {

    private:
        mutex mSizeMutex;
        atomic<int>  mPoolSize; //number of threads in pool
        atomic<bool> mPoolCreated;
        int mLastThreadIndex;
        vector<ThreadPoolData*> mThreadPool; //vector of threads.

        //ctor, copy ctor, assignment overloading
        PerfThreadPool();
        PerfThreadPool(PerfThreadPool const& oh);
        PerfThreadPool& operator=(PerfThreadPool const& oh);
        void create(int size); //create thread initially.
        static void *executeTask(void *);
        int addThreads(int size, ThreadType_t type); //create threads in pool
        int resize(int size, ThreadType_t type);
        static int setNonRT();

    public:
        ~PerfThreadPool();
        int resize(int size); //add more thread to pool
        int placeTask(std::function<void()> &&lambda);
        static PerfThreadPool &getPerfThreadPool(int size = NUM_THREADS);
};

#endif /*__PERFTHREADPOOL_H__*/
