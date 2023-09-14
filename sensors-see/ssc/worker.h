/*
 * Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include <atomic>
#include <pthread.h>
#include <atomic>
#include <queue>
#ifdef SNS_TARGET_LE_PLATFORM
#include <functional>
#include <memory>
#include <string.h>
#endif
#include "sensors_log.h"
#include <wakelock_utils.h>
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

class sns_wakelock;
#define UNUSED(x) (void)(x)
/**
 * type alias for worker's task function
 */
using worker_task = std::function<void()>;

/**
 * Implementation of a worker thread with its own task-queue.
 * Worker thread starts running when constructed and stops when
 * destroyed. Tasks can be assigned to the worker
 * asynchronously and they will be performed in order.
 */
/* set it to 1 to enable debug messages */
#define SNS_DEBUG_WORKER 0
class worker
{
public:
    /**
     * @brief creates a worker thread and starts processing tasks
     */
    worker(): _alive(true)
    {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
        _cond = PTHREAD_COND_INITIALIZER;
        strlcpy(ret, "create", MY_STR_LEN);
        _unsuspendable = false;
        _wake_lock = NULL;
        pthread_create(&_threadhandle, NULL, &worker::thread_execute, this);
#if SNS_DEBUG_WORKER
        _tid = pthread_gettid_np(_threadhandle);
        sns_logi("worker(%p) is created with tid= %d", this, _tid);
#endif
    }

    /**
     * @brief terminates the worker thread, waits until all
     *        outstanding tasks are finished.
     *
     * @note this destructor should never be called from the worker
     *       thread itself.
     */
    ~worker()
    {
        pthread_mutex_lock( &_mutex );
        _alive = false;
        pthread_cond_signal( &_cond );
        pthread_mutex_unlock( &_mutex );
        pthread_join(_threadhandle,(void**)&ret);
        remaining_ind_mem_cleanup();
        size_t num_of_task = _task_q.size();
        if (_wake_lock && num_of_task > 0)
            _wake_lock->put_n_locks(num_of_task);
#if SNS_DEBUG_WORKER
        sns_logi("worker(%p) is destroyed, remaining task = %zu, tid= %d", this, num_of_task, _tid);
#endif
    }

    void setname(const char *name) {
        pthread_setname_np(_threadhandle , name);
    }

    static void *thread_execute(void *param)
    {
        worker *ptr = ((worker *)param);
        if(nullptr != ptr)
          ptr->run();
        strlcpy(ptr->ret, "exit", MY_STR_LEN);
        pthread_exit((void*)ptr->ret);
        return NULL;
    }

    /**
     * @brief helps to make worker thread as unsuspendable.
     *      useful for wakeup sensors
     * @note name of the wakelock must be unique per thread
     */
    int make_unsuspendable(std::string S){
        if (_unsuspendable) {
            sns_loge("already set, must not set more than once");
            return -1;
        }
        _unsuspendable = true;
        UNUSED(S);
        _wake_lock = sns_wakelock::get_instance();
        return 0;
    }

     /**
     * @brief add a new task for the worker to do
     *
     * Tasks are performed in order in which they are added
     *
     * @param task task to perform
     */
    void add_task(void *buf_ptr, const worker_task& task)
    {
        pthread_mutex_lock( &_mutex );
        if(NULL != buf_ptr){
            _ind_mem_tracker.push(buf_ptr);
        }
        try {
            if(_wake_lock)
                _wake_lock->get_n_locks(1);
            _task_q.push(task);
#if SNS_DEBUG_WORKER
            sns_logi("add a task to %p, will be executed by tid= %d", this, _tid);
#endif
        } catch (std::exception& e) {
            sns_loge("failed to add new task, %s", e.what());
            if(_wake_lock)
                _wake_lock->put_n_locks(1);
            pthread_mutex_unlock( &_mutex );
            return;
        }
        pthread_cond_signal( &_cond );
        pthread_mutex_unlock( &_mutex );
    }

    /**
    * @brief free memory pushed to worked thread while calling add_task
    *
    * This task is required to avoid the memory leaks
    *
    */
    void ind_mem_free()
    {
        pthread_mutex_lock( &_mutex );
        if(_ind_mem_tracker.size()){
            void *buf_ptr = _ind_mem_tracker.front();
            free(buf_ptr);
            _ind_mem_tracker.pop();
        }
        pthread_mutex_unlock( &_mutex );
    }

private:
    /* clear remaining ind memory which are in task queue*/
    void remaining_ind_mem_cleanup()
    {
        pthread_mutex_lock( &_mutex );
        while(_ind_mem_tracker.size()){
            void *buf_ptr = _ind_mem_tracker.front();
            free(buf_ptr);
            _ind_mem_tracker.pop();
        }
        pthread_mutex_unlock( &_mutex );
    }

    /* worker thread's mainloop */
    void run()
    {
        while (_alive) {
            pthread_mutex_lock( &_mutex );
            while (_task_q.empty() && _alive) {
                pthread_cond_wait( &_cond, &_mutex );
            }
            if (_alive && !_task_q.empty()) {
                auto task = std::move(_task_q.front());
                _task_q.pop();
                pthread_mutex_unlock( &_mutex );
                try {
                    if(nullptr != task) {
#if SNS_DEBUG_WORKER
                        sns_logi("run task by tid= %d in %p", _tid, this);
#endif
                        task();
                    }
                } catch (const std::exception& e) {
                    /* if an unhandled exception happened when running
                       the task, just log it and move on */
                    sns_loge("task failed, %s", e.what());
                }
                pthread_mutex_lock( &_mutex );
                if(_wake_lock)
                  _wake_lock->put_n_locks(1);
            }
            pthread_mutex_unlock( &_mutex );
        }
    }

    std::atomic<bool> _alive;
    std::queue<worker_task> _task_q;
    std::queue<void *> _ind_mem_tracker;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pthread_t _threadhandle;
    const static int MY_STR_LEN = 20;
    char ret[MY_STR_LEN];
    /* flag to make the worker thread prevent target to go to suspend */
    bool _unsuspendable;
    sns_wakelock* _wake_lock;
#if SNS_DEBUG_WORKER
    pid_t _tid;
#endif
};
