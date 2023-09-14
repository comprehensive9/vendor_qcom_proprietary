/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "TaskExecutor.h"
#include "DebugLogger.h"
#include "Utils.h"

#include <string>

using namespace std;

TaskExecutor::TaskExecutor(std::function<void()> task, std::string name, std::chrono::milliseconds interval)
    : m_task(std::move(task))
    , m_name(std::move(name))
    , m_interval(interval)
    , m_continue(true)
{
    m_thread = thread(&TaskExecutor::ExecutionLoop, this);
}

TaskExecutor::~TaskExecutor()
{
    LOG_DEBUG << m_name << ": Stop request" << endl;
    m_continue.store(false);
    if (m_thread.joinable())
    {
        m_thread.join();
        LOG_DEBUG << m_name << ": Stopped" << endl;
    }
    else
    {
        LOG_DEBUG << m_name << ": Was already stopped" << endl;
    }
}

void TaskExecutor::ExecutionLoop() const
{
    LOG_DEBUG << m_name << ": Starting thread loop" << endl;
    while (m_continue.load())
    {
        LOG_VERBOSE << m_name << ": Task iteration started" << endl;
        auto taskIterationStartTime = chrono::steady_clock::now();
        m_task(); // Run the function of the task
        auto taskIterationStopTime = chrono::steady_clock::now();

        auto taskIterationExecutionTime =
            chrono::time_point_cast<chrono::milliseconds>(taskIterationStopTime) -
            chrono::time_point_cast<chrono::milliseconds>(taskIterationStartTime);

        auto sleepInterval = m_interval.load() - taskIterationExecutionTime;    // May be negative

        LOG_VERBOSE << m_name << ": Task iteration completed"
            << " Execution Time: " << taskIterationExecutionTime.count() << " (msec)"
            << " Interval: " << m_interval.load().count() << " (msec)"
            << " Sleep required: " << sleepInterval.count() << " (msec)"
            << endl;

        if (sleepInterval.count() <= 0)
        {
            // sleep_until/ sleep_for negative value bug was officially fixed in gcc 4.9.3
            LOG_VERBOSE << m_name << ": no need in sleep - interval is " << sleepInterval.count() << endl;
            continue;
        }

        this_thread::sleep_for(sleepInterval);
    }
    LOG_DEBUG << m_name << ": Exit thread loop" << endl;
}