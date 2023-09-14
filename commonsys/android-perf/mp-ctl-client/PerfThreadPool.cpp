/******************************************************************************
  @file    PerfThreadPool.cpp
  @brief   Implementation of PerfThreadPool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOG_TAG "ANDR-PERF-TP-SYS"
#include "PerfThreadPool.h"
#include <unistd.h>
#include <log/log.h>
#include <pthread.h>
#include <sys/resource.h>

#define THREAD_POOL_NAME "%d-%.*s-%d"
#define PROCESS_NAME_CHARS 6
#define QC_DEBUG_ERRORS 1
#ifdef QC_DEBUG_ERRORS
#define QLOGE(x,...) ALOGE("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#else
#define QLOGE(...)
#endif

#if QC_DEBUG
#define QLOGV(x,...) ALOGV("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define QLOGI(x,...) ALOGI("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define QLOGD(x,...) ALOGD("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define QLOGW(x,...) ALOGW("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define QLOGE(x,...) ALOGE("%s() %d: " x "", __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#else
#define QLOGV(x,...)
#define QLOGI(x,...)
#define QLOGD(x,...)
#define QLOGW(x,...) ALOGW("%s() %d: " x "",  __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#define QLOGE(x,...) ALOGE("%s() %d: " x "",  __FUNCTION__ , __LINE__, ##__VA_ARGS__);
#endif

ThreadPoolData::ThreadPoolData(int thread_id) {
    mThreadId = thread_id;
    mFunc = NULL;
    mGotTask = false;
    mState = RUNNING_STATE;
    mHasSlept = false;
    mType = REGULAR;
}

ThreadPoolData::~ThreadPoolData() {
    if (mFunc != NULL) {
        delete mFunc;
        mFunc = NULL;
    }
}

void ThreadPoolData::wait() {
    try {
        unique_lock<mutex> lck(mTaskAvailable);
        setState(SLEEPING_STATE);
        mCondVar.wait(lck,[&]{mHasSlept = true;return mGotTask.load();});
        mGotTask = false;
        mHasSlept = false;
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
}

void ThreadPoolData::signal() {
    try {
        unique_lock<mutex> lck(mTaskAvailable);
        mGotTask = true;
        mCondVar.notify_one();
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
}

int ThreadPoolData::getIfAvailable(ThreadType_t type) {
    int available = FAILED;
    try {
        std::unique_lock<std::mutex> lock(mStateMutex, std::try_to_lock);
        if (lock.owns_lock()) {
            if (getState() == SLEEPING_STATE && mHasSlept && mType == type) {
                mState = RUNNING_STATE;
                available = SUCCESS;
            }
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return available;
}

void ThreadPoolData::setState(ThreadState_t state) {
    try {
        std::unique_lock<std::mutex> lock(mStateMutex);
        if (mState != INVALID_STATE) {
            mState = state;
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
}

PerfThreadPool::PerfThreadPool() {
    QLOGI("Constructor %s", __func__);
    mPoolSize = 0;
    mPoolCreated = false;
    mLastThreadIndex = -1;

}

int PerfThreadPool::setNonRT() {

    struct sched_param sparam_old;
    struct sched_param sparam;
    int policy_old, priority_old;
    int policy, priority;
    int rc;

    policy = SCHED_NORMAL;
    priority = sched_get_priority_min(policy);
    sparam.sched_priority = priority;

    rc = pthread_getschedparam(pthread_self(), &policy_old, &sparam_old);
    if (rc) {
        QLOGE("Failed to get sched params %d", rc);
        return FAILED;
    }
    if (policy_old != policy || sparam_old.sched_priority != priority) {
        rc = pthread_setschedparam(pthread_self(), policy, &sparam);
        if (rc != SUCCESS) {
            QLOGE("Failed to set sched params %d", rc);
            return FAILED;
        }
    }
    return SUCCESS;
}

void *PerfThreadPool::executeTask(void *ptr) {
    int task_executed = 0;
    int ret_code = 0;
    ThreadPoolData *thread_data = (ThreadPoolData *)ptr;
    if(thread_data == NULL) {
        QLOGE( "Thread exiting NULL data");
        return NULL;
    }
    ret_code = setNonRT();
    if (ret_code != SUCCESS) {
        QLOGE("Failed to set priority of thread Thread-%d", thread_data->mThreadId);
        return NULL;
    }

    do {
        QLOGI("Thread-%d executed [%d] tasks", thread_data->mThreadId, task_executed);
        thread_data->wait();
        QLOGI("Thread-%d got task", thread_data->mThreadId);
        if (thread_data->getState() == RUNNING_STATE) {
            try {
                if (thread_data->mFunc != NULL) {
                    (*(thread_data->mFunc))();
                    delete thread_data->mFunc;
                    thread_data->mFunc = NULL;
                    task_executed++;
                }
            } catch (std::exception &e) {
                QLOGE("Exception caught: %s in %s", e.what(), __func__);
            } catch (...) {
                QLOGE("Exception caught in %s", __func__);
            }
            QLOGI("setting Thread-%d to sleep state", thread_data->mThreadId);
        }
    } while(thread_data->getState() != INVALID_STATE);
    QLOGI("Thread-%d exiting, Total Task executed[%d]", thread_data->mThreadId, task_executed);
    return NULL;
}

int PerfThreadPool::addThreads(int size,ThreadType_t type) {
    int rc = -1;
    char buf[MAX_BUF_SIZE] = {0};
    int pid = getpid();
    pthread_getname_np(pthread_self(), buf, MAX_BUF_SIZE);

    for (int i = 0; i < size; i++) {
        char tname[MAX_BUF_SIZE] = {0};
        ThreadPoolData *tmp = new(std::nothrow) ThreadPoolData(mPoolSize);
        if (tmp != NULL) {
            rc = pthread_create(&(tmp->mThread),NULL,executeTask, tmp);
            if (rc == 0) {

                snprintf(tname, MAX_BUF_SIZE, THREAD_POOL_NAME , pid, PROCESS_NAME_CHARS, buf, mPoolSize.load());
                rc = pthread_setname_np(tmp->mThread, tname);
                if (rc != 0) {
                    QLOGE("Failed to name Thread-%d for %s process",mPoolSize.load(), buf);
                }
                tmp->mType = type;
                try {
                    mThreadPool.push_back(tmp);
                    mPoolSize++;
                } catch (std::exception &e) {
                    QLOGE("Exception caught: %s in %s", e.what(), __func__);
                    QLOGE("Error in Thread Creation");
                    if (tmp != NULL) {
                        tmp->setState(INVALID_STATE);
                        tmp->signal();
                        pthread_join(tmp->mThread, NULL);
                        delete tmp;
                    }
                } catch (...) {
                    QLOGE("Exception caught in %s", __func__);
                    QLOGE("Error in Thread Creation");
                    if (tmp != NULL) {
                        tmp->setState(INVALID_STATE);
                        tmp->signal();
                        pthread_join(tmp->mThread, NULL);
                        delete tmp;
                    }
                }
            } else {
                QLOGE("Error in Thread Creation");
                if (tmp != NULL) {
                    delete tmp;
                }
            }
        } else {
            QLOGE("Error in Thread Creation");
        }
    }
    return mPoolSize;
}

/*
using this method for creating threads at runtime only for clients using perf_hint_offload API.
clients can resize the pool. only 2 threads will be created on first API call.
*/
void PerfThreadPool::create(int size) {
    try {
        std::scoped_lock lck(mSizeMutex);

        if (mPoolCreated || mPoolSize > 0) {
            return;
        } else if(size <= 0) {
            size = NUM_THREADS;
            QLOGE("Invalid Size creating %d Threads in Pool", size);
        }
        size = size < MAX_NUM_THREADS ? size : MAX_NUM_THREADS;
        addThreads(size, REGULAR);
        if (mPoolSize > 0) {
            mPoolCreated = true;
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
}

/*
resize/extend number of threads in ThreadPool.
*/
int PerfThreadPool::resize(int size) {
    return resize(size,REGULAR);
}

int PerfThreadPool::resize(int size,ThreadType_t type) {
    int pool_new_size = 0;
    try {
        std::scoped_lock lck(mSizeMutex);
        pool_new_size = mPoolSize;
        if (!mPoolCreated || !(mPoolSize > 0)) {
            create(size);
            return mPoolSize;
        } else if (size <= 0 || (size + mPoolSize) > MAX_NUM_THREADS || (size + mPoolSize) < 0) {
            QLOGE("Invalid size: %d Max Threads allowed = %d", size, MAX_NUM_THREADS);
            return pool_new_size;
        }
        pool_new_size = addThreads(size, type);
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return pool_new_size;
}

//method to offload task to a running thread
int PerfThreadPool::placeTask(std::function<void()> &&lambda) {
    int rc = FAILED;
    int pool_size = mPoolSize;
    int currIndex = 0;
    if (pool_size == 0 || !mPoolCreated) {
        QLOGE("Pool not created use resize() to add threads to pool");
    }
    try {
        for(int i = 0 ;i < pool_size; i++) {
            {
                std::unique_lock<std::mutex> lock(mSizeMutex, std::try_to_lock);
                if (lock.owns_lock() == false) {
                    continue;
                }
                mLastThreadIndex = (mLastThreadIndex + 1) % pool_size;
                currIndex = mLastThreadIndex;
            }
            QLOGI("Trying to Get Thread %d", currIndex);
            if (mThreadPool[currIndex] != NULL && mThreadPool[currIndex]->getIfAvailable() == SUCCESS) {
                QLOGI("Got Thread %d", currIndex);
                mThreadPool[currIndex]->mFunc = new(std::nothrow) std::function<void()>(lambda);
                if (mThreadPool[currIndex]->mFunc != NULL) {
                    mThreadPool[currIndex]->signal();
                    QLOGI("Waking up Thread-%d", currIndex);
                    QLOGI("Task placed on thread-%d", currIndex);
                    rc = SUCCESS;
                    break;
                } else {
                    mThreadPool[currIndex]->setState(SLEEPING_STATE);
                    QLOGE("Failed to offload task to Thread-%d Memory allocation error", currIndex);
                }
            }
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE("Failed to offload task all threads are busy.");
    }
    return rc;
}

PerfThreadPool &PerfThreadPool::getPerfThreadPool(int size) {
    static PerfThreadPool singleton_pool_obj;

    if (!singleton_pool_obj.mPoolCreated) {
        singleton_pool_obj.create(size);
    }
    return singleton_pool_obj;
}

PerfThreadPool::~PerfThreadPool() {
    QLOGI("%s ", __func__);
    try {
        std::scoped_lock lck(mSizeMutex);
        int pool_size = mPoolSize;
        mPoolSize = 0;
        for(int i = 0; i < pool_size; i++) {
            if (mThreadPool[i] != NULL) {
                mThreadPool[i]->setState(INVALID_STATE);
                mThreadPool[i]->signal();
                pthread_join(mThreadPool[i]->mThread,NULL);
                if (mThreadPool[i]->mFunc != NULL) {
                    delete mThreadPool[i]->mFunc;
                    mThreadPool[i]->mFunc = NULL;
                }
                QLOGI("Thread-%d join", i);
                delete  mThreadPool[i];
                mThreadPool[i] = NULL;
            }
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
}
