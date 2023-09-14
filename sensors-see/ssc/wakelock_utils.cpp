/*
 * Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifdef SNS_TARGET_LE_PLATFORM
#include <cstring>
#endif
#include "wakelock_utils.h"

#include "sensors_log.h"

const char* SNS_SENSORS_WACKLOCK_NAME = "SensorsHAL_WAKEUP";
#define SNS_POWER_WAKE_lOCK_ACQUIRE_INDEX 0
#define SNS_POWER_WAKE_lOCK_RELEASE_INDEX 1
const char * const PATHS[] = {
    /*to hold wakelock*/
    "/sys/power/wake_lock",
    /*to release wakelock*/
    "/sys/power/wake_unlock",
};

sns_wakelock* sns_wakelock::_self= nullptr;

sns_wakelock* sns_wakelock::get_instance()
{
  if(nullptr != _self) {
    return _self;
  }
  else {
    _self = new sns_wakelock();
    return _self;
  }
}

sns_wakelock::sns_wakelock()
{
  _is_held = false;
  _accu_count = 0;
  for (int i=0; i<FD_COUNT; i++) {
    _fds[i] = -1;
    int fd = open(PATHS[i], O_RDWR | O_CLOEXEC);
    if (fd < 0) {
      sns_loge("fatal error opening \"%s\": %s\n", PATHS[i],
          strerror(errno));
    }
    _fds[i] = fd;
  }
}

sns_wakelock::~sns_wakelock()
{
  std::lock_guard<std::mutex> lock(_mutex);
  if(true == _is_held) {
    _accu_count = 0;
    release();
  }

  for (int i=0; i<FD_COUNT; i++) {
    if (_fds[i] >= 0)
      close(_fds[i]);
  }
}
void sns_wakelock::wait_for_held()
{
  std::unique_lock<std::mutex> lock(_mutex);
  if(false == _is_held) {
    _cv.wait(lock);
  }
}
void sns_wakelock::get_n_locks(unsigned int _in_count)
{
  std::lock_guard<std::mutex> lock(_mutex);
  if (_in_count > std::numeric_limits<unsigned int>::max() - _accu_count) {
      sns_loge("_in_count %d will overflow max of _accu_count %u", _in_count, _accu_count);
      return;
  }
  if(_accu_count == 0 && false == _is_held)
    acquire();
  _accu_count = _accu_count + _in_count;
}

void sns_wakelock::put_n_locks(unsigned int _in_count)
{
  std::lock_guard<std::mutex> lock(_mutex);
  if (_in_count > _accu_count) {
      sns_loge("_in_count %d is larger than _accu_count %u", _in_count, _accu_count);
      return;
  }
  _accu_count = _accu_count - _in_count;
  if(_accu_count == 0 && true ==_is_held)
    release();
}
void sns_wakelock::put_all() {
  std::lock_guard<std::mutex> lock(_mutex);
  if(true == _is_held) {
    _accu_count = 0;
    release();
  }
}

void sns_wakelock::acquire()
{
  if (_fds[SNS_POWER_WAKE_lOCK_ACQUIRE_INDEX] >= 0) {
    if (write( _fds[SNS_POWER_WAKE_lOCK_ACQUIRE_INDEX],SNS_SENSORS_WACKLOCK_NAME, strlen(SNS_SENSORS_WACKLOCK_NAME)+1) > 0) {
      sns_logv("sucess wakelock acquire:%s", SNS_SENSORS_WACKLOCK_NAME);
      _is_held = true;
      _cv.notify_one();
    } else {
      sns_loge("write fail wakelock acquire:%s", SNS_SENSORS_WACKLOCK_NAME);
    }
  } else {
    sns_loge("Not able to open fd for wakeup:%s", SNS_SENSORS_WACKLOCK_NAME);
  }
}

void sns_wakelock::release()
{
  if (_fds[SNS_POWER_WAKE_lOCK_RELEASE_INDEX] >= 0) {
    if (write( _fds[SNS_POWER_WAKE_lOCK_RELEASE_INDEX],SNS_SENSORS_WACKLOCK_NAME, strlen(SNS_SENSORS_WACKLOCK_NAME)+1) > 0) {
      sns_logv("sucess release %s wakelock", SNS_SENSORS_WACKLOCK_NAME);
      _is_held = false;
    } else {
      sns_loge("failed in write:%s wake unlock", SNS_SENSORS_WACKLOCK_NAME);
    }
  } else {
    sns_loge("Not able to open fd for wakeup:%s", SNS_SENSORS_WACKLOCK_NAME);
  }
}
