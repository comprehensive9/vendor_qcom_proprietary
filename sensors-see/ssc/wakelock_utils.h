/*
 * Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once


#include <stdio.h>
#include <mutex>
#include <pthread.h>
#include <atomic>
#ifdef SNS_TARGET_LE_PLATFORM
#include <condition_variable>
#endif

using namespace std;

const int FD_COUNT = 2;

/*Singleton class which will interact with sns_wakelock
 * to acuire / release the wakelock based on the number of
 * wakeup events processed from FMQ */
class sns_wakelock {
public:
  /**
   * @brief Static function to get instance of sns_wakelock
   */
  static sns_wakelock* get_instance();

  /**
   * @brief Number of wakeup samples pushed to framework
   */
  void get_n_locks(unsigned int _in_count);

  /**
   * @brief Number of wakeup samples processed by framework
   */
  void put_n_locks(unsigned int _in_count);

  /**
   * @brief Framework processed all wakeup samples.
   *  This is used to release the wakelock in hal
   *  without any wakeup count conditions
   */
  void put_all();

  /**
   * @brief returns the status of wakelock
   */
  inline bool get_status() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _is_held;
  }

  /**
   * @brief wait for wakelock to be held
   *  in wakelock FMQ thread in h/w module
   */
  void wait_for_held();
private:
  sns_wakelock();
  ~sns_wakelock();

  /* @brief acquire the wakelock
   * */
  void acquire();

  /* @brief release the wakelock based on processed count
   * */
  void release();

  /* @brief Instance to singleton class
   * */
  static sns_wakelock* _self;

  /* @brief Total number of unprocessed wakeup samples by framework
   * */
  unsigned int _accu_count;
  std::mutex _mutex;
  std::condition_variable _cv;
  int _fds[FD_COUNT];
  bool _is_held;
};
