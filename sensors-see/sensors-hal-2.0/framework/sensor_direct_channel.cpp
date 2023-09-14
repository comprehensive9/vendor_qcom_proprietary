/*============================================================================
  @file direct_channel.cpp

  @brief
  direct_channel class implementation.

  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifdef SNS_SUPPORT_DIRECT_CHANNEL
#include <string>
#include "sensor_direct_channel.h"
#include <string>
#include <cutils/native_handle.h>
#include <hardware/hardware.h>
#include <dlfcn.h>

#include "sns_direct_channel.pb.h"
#include "sns_std_type.pb.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sensors_hal.h"
#include "sensors_timeutil.h"
#include "rpcmem.h"


const int32_t SNS_LLCM_FLAG_CALIBRATED_STREAM     = 0x4;
int direct_channel::client_channel_handle_counter = 0;

using android::hardware::hidl_handle;
using android::hardware::graphics::common::V1_2::BufferUsage;
using IMapper3_0 = ::android::hardware::graphics::mapper::V3_0::IMapper;
using IMapper2_0 = ::android::hardware::graphics::mapper::V2_0::IMapper;
using MapperError3_0 = android::hardware::graphics::mapper::V3_0::Error;
using MapperError2_0 = android::hardware::graphics::mapper::V2_0::Error;
using ExtensionError1_0 = ::vendor::qti::hardware::display::mapperextensions::V1_0::Error;

remote_handle64 direct_channel::_remote_handle_fd = -1;
remote_handle64 direct_channel::_sns_rpc_handle64 = -1;

#define US_TO_HZ(var) 1000000/(var)

std::string direct_channel_client::config_req_str(unsigned int sample_period_us)
{
  sns_logi("%s Start" , __func__);
  std::string channel_config_msg_str;
  sns_direct_channel_config_msg channel_config_msg;
  std::string sensor_config_str;
  sns_direct_channel_set_client_req *client_req = channel_config_msg.mutable_set_client_req();
  sns_std_sensor_config sensor_config;

  //get sub fields
  sns_direct_channel_stream_id *stream_id = client_req->mutable_stream_id();
  sns_direct_channel_set_client_req_structured_mux_channel_stream_attributes *stream_attributes = client_req->mutable_attributes();
  sns_std_request *std_request = client_req->mutable_request();

  _sr_hz = US_TO_HZ(sample_period_us);

  //set config request
  client_req->set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);

  // set streamid parameters
  stream_id->set_resampled(true);
  stream_id->set_calibrated(_calibrated);
  stream_id->mutable_suid()->set_suid_high(_suid.high);
  stream_id->mutable_suid()->set_suid_low(_suid.low);

  // set stream sttribute parameters
  stream_attributes->set_sensor_type(_sensor_type);
  stream_attributes->set_sensor_handle(_sensor_handle);

  // set the payload for the request
  sensor_config.set_sample_rate(_sr_hz);
  sensor_config.SerializeToString(&sensor_config_str);
  std_request->set_is_passive(false);
  std_request->set_payload( sensor_config_str);

  channel_config_msg.SerializeToString(&channel_config_msg_str);
  return channel_config_msg_str;
}

std::string direct_channel_client::remove_req_str()
{
  sns_logi("%s Start" , __func__);
  std::string channel_config_msg_str;
  sns_direct_channel_config_msg channel_config_msg;

  sns_direct_channel_remove_client_req *remove_client_req = channel_config_msg.mutable_remove_client_req();

  sns_direct_channel_stream_id *stream_id = remove_client_req->mutable_stream_id();
  stream_id->set_calibrated(_calibrated);
  stream_id->set_resampled(true);
  stream_id->mutable_suid()->set_suid_high(_suid.high);
  stream_id->mutable_suid()->set_suid_low(_suid.low);

  channel_config_msg.SerializeToString(&channel_config_msg_str);
  return channel_config_msg_str;
}

direct_channel::direct_channel(const struct native_handle *mem_handle,
    const size_t mem_size)
{
    sns_logi("%s Start" , __func__);
    int ret = 0;
    _channel_handle = -1;
    client_channel_handle = 0;
    if (init_rpc_channel()) {
      /*communication to remote side is failed*/
      sns_loge("%s: init_rpc_channel failed!", __FUNCTION__);
      return;
    }

    if (init_mapper()) {
      /* Kick out if mapper initialization fails */
      sns_loge("%s: init_ampper failed!", __FUNCTION__);
      return;
    }

    /* Check the native_handle for validity */
    if (mem_handle->numFds < 1) {
      sns_loge("%s: Unexpected numFds. Expected at least 1. Received %d.", __FUNCTION__,
          mem_handle->numFds);
      return;
    }


    /* Copy the native_handle */
    _mem_native_handle = native_handle_clone(mem_handle);
    if(NULL == _mem_native_handle){
        sns_loge("%s: _mem_native_handle is NULL", __FUNCTION__);
        return;
    }

    sns_logi("(mem_handle/):hnd %p(%d) (cloned handle/):local_hnd %p(%d)",
               (void *)mem_handle, mem_handle->data[0], _mem_native_handle, _mem_native_handle->data[0]);

    _buffer_size = (int)mem_size;

    if (qtiMapper3 != nullptr) {
        MapperError3_0 error;
        auto result = qtiMapper3->importBuffer(hidl_handle(_mem_native_handle),
                            [&](const auto& tmpError, const auto& tmpBufferHandle) {
                                error = tmpError;
                                importedHandle = static_cast<buffer_handle_t>(tmpBufferHandle);
                            });

        if (!result.isOk()) {
            sns_loge("fail to import buffer");
            return;
        }
        if (error != MapperError3_0::NONE) {
            sns_loge("fail to import buffer, error = %d", error);
            return;
        }
    } else {
        MapperError2_0 error = MapperError2_0::NONE;
        if (qtiMapper1_1 != nullptr) {
            auto result = qtiMapper1_1->importBuffer(hidl_handle(_mem_native_handle),
                              [&](const auto& tmpError, const auto& tmpBufferHandle) {
                                  error = tmpError;
                                  importedHandle = static_cast<buffer_handle_t>(tmpBufferHandle);
                              });
            if (!result.isOk()) {
                sns_loge("fail to import buffer");
                return;
            }
            if (error != MapperError2_0::NONE) {
                sns_loge("fail to import buffer, error = %d", error);
                return;
            }
        }
    }

    auto buffer = const_cast<native_handle_t*>(importedHandle);
    if (qtiMapper3 != nullptr) {

        IMapper3_0::Rect accessRegion = {
            .left = 0,
            .top = 0,
            .width = static_cast<int32_t>(_buffer_size),
            .height = 1
        };
        hidl_handle acquireFenceHandle;
        size_t bytesPerPixel;
        size_t bytesPerStride;

        MapperError3_0 error;
        auto result = qtiMapper3->lock(buffer, BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN | BufferUsage::SENSOR_DIRECT_DATA,
                     accessRegion, acquireFenceHandle,
                     [&](const auto& tmpError, const auto& tmpData, const auto& tmpBytesPerPixel, const auto& tmpBytesPerStride) {
                         error = tmpError;
                         _buffer_ptr = tmpData;
                         bytesPerPixel = tmpBytesPerPixel;
                         bytesPerStride = tmpBytesPerStride;
                      });

        if (!result.isOk() || error != MapperError3_0::NONE) {
            qtiMapper3->freeBuffer(buffer);
            native_handle_close(_mem_native_handle);
            _mem_native_handle = NULL;
            sns_loge("%s: lock FAILED ret %d", __FUNCTION__, error);
            return;
        }
    } else {
        IMapper2_0::Rect accessRegion = {
            .left = 0,
            .top = 0,
            .width = static_cast<int32_t>(_buffer_size),
            .height = 1
        };
        hidl_handle acquireFenceHandle;
        MapperError2_0 error = MapperError2_0::NONE;

        if (qtiMapper1_1 != nullptr) {
            auto result = qtiMapper1_1->lock(buffer, BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN | BufferUsage::SENSOR_DIRECT_DATA,
                         accessRegion, acquireFenceHandle,
                         [&](const auto& tmpError, const auto& tmpData) {
                             error = tmpError;
                             _buffer_ptr = tmpData;
                          });
            if (!result.isOk() || error != MapperError2_0::NONE) {
                qtiMapper1_1->freeBuffer(buffer);
                native_handle_close(_mem_native_handle);
                _mem_native_handle = NULL;
                sns_loge("%s: lock FAILED ret %d - qtiMapper1_1", __FUNCTION__, error);
                return;
            }
        }
    }

    /* Increment the Android handle counter and check it for validity. Then
        assign the resulting Android handle to this Direct Channel */
    ++client_channel_handle_counter;
    if (client_channel_handle_counter <= 0) {
      client_channel_handle_counter = 1;
    }
    client_channel_handle = client_channel_handle_counter;

    /* Clear out the buffer */
    memset(_buffer_ptr, 0, (size_t)_buffer_size);

#if 0
    /* Map the shared memory to a fastRPC addressable file descriptor */
    reg_buf_attr(_buffer_ptr, _buffer_size, this->get_buffer_fd(), 0);
    reg_buf(_buffer_ptr, _buffer_size, this->get_buffer_fd());
#endif
    /* create channel for further processing */
    std::string req_msg = get_channel_req_str();
    ret = sns_direct_channel_create(_sns_rpc_handle64, (const unsigned char *)req_msg.c_str(), req_msg.length(), &_channel_handle);
    if (ret != 0) {
      sns_loge("%s: sns_low_lat_stream_init failed! ret %d", __FUNCTION__, ret);
      _channel_handle = -1;
      //TODO: shall we call unlock buffer here?
      if (qtiMapper3 != nullptr) {
        qtiMapper3->freeBuffer(buffer);
      } else if (qtiMapper1_1 != nullptr) {
        qtiMapper1_1->freeBuffer(buffer);
      }
      native_handle_close(_mem_native_handle);
      _mem_native_handle = NULL;
    }

    sns_logv("%s: complete ret=%d", __FUNCTION__, ret);
    return;
}

/* Copy constructor */
direct_channel::direct_channel(const direct_channel &copy)
{
    sns_logi("%s Start" , __func__);
    _mem_native_handle = native_handle_clone(copy._mem_native_handle);
    _buffer_ptr = copy._buffer_ptr;
    _buffer_size = copy._buffer_size;
    _channel_handle = copy._channel_handle;
    client_channel_handle = copy.client_channel_handle;
}


direct_channel::~direct_channel()
{
  sns_logi("%s Start" , __func__);
  if ( 0 == sns_direct_channel_delete(_sns_rpc_handle64 , _channel_handle))
    sns_logd("sns_direct_channel_delete success _sns_rpc_handle64/_sns_rpc_handle64 : %llu/%llu \n",
                            (unsigned long long)_remote_handle_fd, (unsigned long long)_sns_rpc_handle64);
  else
    sns_loge("sns_direct_channel_delete failed  _remote_handle_fd/_sns_rpc_handle64 : %llu/%llu \n",
                            (unsigned long long)_remote_handle_fd, (unsigned long long)_sns_rpc_handle64);

  if(_mem_native_handle != NULL) {
    auto buffer = const_cast<native_handle_t*>(importedHandle);
    if (qtiMapper3 != nullptr) {
      MapperError3_0 error;
      auto result = qtiMapper3->unlock(buffer, [&](const auto& tmpError, const auto& tmpReleaseFence) {
        error = tmpError;
        sns_logi("unlock buffer, %d", tmpError);
      });
      qtiMapper3->freeBuffer(buffer);
    } else if (qtiMapper1_1 != nullptr) {
      MapperError2_0 error = MapperError2_0::NONE;
      auto result = qtiMapper1_1->unlock(buffer, [&](const auto& tmpError, const auto& tmpReleaseFence) {
        error = tmpError;
        sns_logi("unlock buffer, %d", tmpError);
      });
      qtiMapper1_1->freeBuffer(buffer);
    }
    native_handle_close(_mem_native_handle);
    _mem_native_handle = NULL;
  }
  sns_logi("%s End " , __func__);
}

int32_t direct_channel::init_rpc_channel()
{
  int ret = 0;
  std::string uri = sns_direct_channel_URI "&_dom=adsp";
  struct stat sb;
  if (!stat("/sys/kernel/boot_slpi", &sb)){
    uri = sns_direct_channel_URI "&_dom=sdsp";
  }

  // below is required to be called per HLOS process and first call before any communication.
  // with out this skel will try to open in root pd instead of sensor pd and can throw hashing related errors
  if ((_remote_handle_fd != -1 ) ||
       (0 == remote_handle64_open(ITRANSPORT_PREFIX "createstaticpd:sensorspd&_dom=sdsp", &_remote_handle_fd))) {
    if ( 0 == sns_direct_channel_open(uri.c_str(), &_sns_rpc_handle64)) {
      sns_logd("sns_direct_channel_open success _remote_handle_fd/_sns_rpc_handle64 : %llu/%llu \n",
                                          (unsigned long long)_remote_handle_fd, (unsigned long long)_sns_rpc_handle64);
    } else {
      sns_loge("sns_direct_channel_open failed  _remote_handle_fd/_sns_rpc_handle64 : %llu/%llu \n",
                                          (unsigned long long)_remote_handle_fd, (unsigned long long)_sns_rpc_handle64);
      ret = -1;
    }
  } else {
    sns_loge("remote_handle64_open failed _remote_handle_fd/_sns_rpc_handle64 : %llu/%llu \n",
                                          (unsigned long long)_remote_handle_fd, (unsigned long long)_sns_rpc_handle64);
    ret = -1;
  }

  return ret;
}
std::string direct_channel::get_channel_req_str()
{
  sns_logi("%s Start" , __func__);
  std::string req_msg_str;
  sns_direct_channel_create_msg req_msg;

  req_msg.set_channel_type(DIRECT_CHANNEL_TYPE_STRUCTURED_MUX_CHANNEL);
  req_msg.set_client_proc(SNS_STD_CLIENT_PROCESSOR_APSS);
  req_msg.mutable_buffer_config()->set_size(_buffer_size);
  req_msg.mutable_buffer_config()->set_fd(this->get_buffer_fd());
  req_msg.SerializeToString(&req_msg_str);

  return req_msg_str;
}

std::string direct_channel::update_offset_req_str(int64_t new_offset)
{
  sns_logi("%s Start" , __func__);
  std::string channel_config_msg_str;
  sns_direct_channel_config_msg channel_config_msg;
  sns_direct_channel_set_ts_offset *ts_offset = channel_config_msg.mutable_set_ts_offset();

  ts_offset->set_ts_offset(new_offset);
  channel_config_msg.SerializeToString(&channel_config_msg_str);
  return channel_config_msg_str;
}

int direct_channel::update_offset(int64_t new_offset)
{
  sns_logi("%s Start" , __func__);
  int ret = 0;
  std::string req_msg = update_offset_req_str(new_offset);
  sns_logd("update_offset with %lld", (long long)new_offset);
  ret = sns_direct_channel_config(_sns_rpc_handle64, _channel_handle, (const unsigned char *)req_msg.c_str(), req_msg.length());
  if (0 != ret)
    sns_loge("sns_direct_channel_config/update_offset failed %d", ret);

  return ret;
}

int direct_channel::config_channel(int handle, int64 timestamp_offset, const sensor_uid* sensor_suid,
                                          unsigned int sample_period_us, unsigned int flags,
                                          int sensor_handle, int sensor_type)
{
  sns_logv("%s: ", __FUNCTION__);
  std::map<int, unique_ptr<direct_channel_client>>::iterator it;

  if (-1== sensor_handle && 0 == sensor_suid->high && 0 == sensor_suid->low && 0 == sensor_type) {
    int idx = 0;
    for (auto it = _clients_map.begin(); it != _clients_map.end(); ++it) {
        std::unique_ptr<direct_channel_client> temp = std::move(it->second);
        sns_logi("delete client(%p) for sensor_handle(%d)/direct channel(%d)", temp.get(), sensor_handle, _channel_handle);
        sns_logi("%s  remove config" , __func__);
        std::string msg = temp->remove_req_str();
        if( 0 != sns_direct_channel_config(_sns_rpc_handle64, _channel_handle, (const unsigned char *)msg.c_str(), msg.length())) {
          sns_loge("config_req failed for existing client(%p)/sensor_handle(%d)/direct channel(%d)", _clients_map[sensor_handle].get(), sensor_handle, _channel_handle);
          return -1;
        }
        temp.reset();
        _clients_map.erase(idx);
        idx++;
    }
  } else {
    it = _clients_map.find(sensor_handle);
    if (it != _clients_map.end()) {
      /*available client so update the existing request*/
      std::string req_msg = _clients_map[sensor_handle]->config_req_str(sample_period_us);
      sns_logi("%s  config req - updating on the existing channel" , __func__);
      if( 0 != sns_direct_channel_config(_sns_rpc_handle64, _channel_handle, (const unsigned char *)req_msg.c_str(), req_msg.length())) {
        sns_loge("config_req failed for existing client(%p)/sensor_handle(%d)/direct channel(%d)", _clients_map[sensor_handle].get(), sensor_handle, _channel_handle);
        return -1;
      }
    } else {
      /*new client , create it and send request , store it if request is success */
      std::unique_ptr<direct_channel_client> client;
      client   = std::make_unique<direct_channel_client>(sensor_handle, *sensor_suid, flags&SNS_LLCM_FLAG_CALIBRATED_STREAM, sensor_type);
      sns_logi("new client(%p) added sensor_handle(%d)/direct channel(%d)", client.get(), sensor_handle, _channel_handle);
      if (client) {
        std::string req_msg = client->config_req_str(sample_period_us);
        sns_logi("%s  config req - Creatting new config requuest " , __func__);
        if( 0 == sns_direct_channel_config(_sns_rpc_handle64, _channel_handle, (const unsigned char *)req_msg.c_str(), req_msg.length())) {
          _clients_map[sensor_handle] = std::move(client);
          //_clients_map.insert(std::pair<int,unique_ptr<direct_channel_client>>(sensor_handle, client));
          } else {
            sns_logi("config_req failed for client(%p)/sensor_handle(%d)/direct channel(%d)", client.get(), sensor_handle, _channel_handle);
            return -1;
        }
      }
      else {
        sns_logi("failed to create client for sensor_handle(%d)/direct channel(%d)", sensor_handle, _channel_handle);
        return -1;
      }
    }

    /*update the offset request*/
    if( 0 != update_offset(timestamp_offset)) {
      sns_loge("update_offset failed for sensor_handle(%d)/direct channel(%d)", sensor_handle, _channel_handle);
      return -1;
    }
  }
  return 0;
}

int direct_channel::stop_channel(int handle)
{
  int ret = 0;
  sns_logv("%s: ", __FUNCTION__);
  std::map<int, unique_ptr<direct_channel_client>>::iterator it;
  int idx = 0;
  for (auto it = _clients_map.begin(); it != _clients_map.end(); ++it) {
    std::unique_ptr<direct_channel_client> temp = std::move(it->second);
    sns_logi("delete client(%p)/direct channel(%d)", temp.get(), _channel_handle);
    if(nullptr == temp.get()) {
      sns_logi("Current client is already removed as part of config request");
      continue;
    }
    sns_logi("%s  Remove  req " , __func__);
    std::string msg = temp->remove_req_str();
    if( 0 != sns_direct_channel_config(_sns_rpc_handle64, _channel_handle, (const unsigned char *)msg.c_str(), msg.length())) {
      sns_loge("config_req remove stream failed client(%p)/direct channel(%d)", temp.get(), _channel_handle);
      return -1;
    }
    temp.reset();
    _clients_map.erase(idx);
    idx++;
  }
  return ret;
}

int direct_channel::get_low_lat_handle()
{
  return _channel_handle;
}

int direct_channel::get_client_channel_handle()
{
  return client_channel_handle;
}

int direct_channel::get_buffer_fd()
{
    if (_mem_native_handle) {
        return _mem_native_handle->data[0];
    } else {
        return -1;
    }
}

bool direct_channel::is_same_memory(const struct native_handle *mem_handle)
{
  uint64_t id1 = 0, id2 = 0;
  auto buffer1 = const_cast<native_handle_t*>(mem_handle);
  auto buffer2 = const_cast<native_handle_t*>(_mem_native_handle);
  if(qtiMapper3 != nullptr) {
    if(qtiMapperExtensions != nullptr) {
      qtiMapperExtensions->getId(buffer1, [&](const auto& tmpError, const auto& tmpId) {
                                              if (tmpError == ExtensionError1_0::NONE) {
                                                id1 = tmpId;
                                              } else {
                                                sns_loge("fail to get id for buffer1");
                                              }});

      qtiMapperExtensions->getId(buffer2, [&](const auto& tmpError, const auto& tmpId) {
                                              if (tmpError == ExtensionError1_0::NONE) {
                                                id2 = tmpId;
                                              } else {
                                                sns_loge("fail to get id for buffer2");
                                              }});
    }
  } else if (qtiMapper1_1 != nullptr) {
      qtiMapper1_1->getId(buffer1,[&](const auto& tmpError, const auto& tmpId) {
                                      if (tmpError == MapperError2_0::NONE) {
                                        id1 = tmpId;
                                      } else {
                                        sns_loge("fail to get id for buffer1");
                                      }});
      qtiMapper1_1->getId(buffer2,[&](const auto& tmpError, const auto& tmpId) {
                                      if (tmpError == MapperError2_0::NONE) {
                                        id2 = tmpId;
                                      } else {
                                        sns_loge("fail to get id for buffer1");
                                      }});
  }

  return (id1 == id2);
}

int direct_channel::init_mapper(void)
{
  int ret = 0;
  qtiMapper3 = IQtiMapper3_0::castFrom(IMapper3_0::getService());
  if (qtiMapper3 == nullptr) {
    sns_logi("%s: IQtiMapper V3 is not available. fall back to V1_1", __FUNCTION__);
    qtiMapper1_1 = IQtiMapper1_1::castFrom(IMapper2_0::getService());
    if(qtiMapper1_1 == nullptr) {
      sns_logi("%s: IQtiMapper V1_1 is not available. failed", __FUNCTION__);
      return -1;
    }
  }

  if (qtiMapper3 != nullptr) {
    MapperError3_0 error = MapperError3_0::NONE;
    qtiMapper3->getMapperExtensions([&](auto const& tmpError, auto const& tmpExtensions) {
                                      error = tmpError;
                                      if (tmpError == MapperError3_0::NONE) {
                                        qtiMapperExtensions = tmpExtensions;
                                      }});
    if (error != MapperError3_0::NONE) {
      sns_loge("%s: fail to get Qti Mapper Extension, %d", __FUNCTION__, error);
      return -1;
    }
  }
  return 0;
}
void direct_channel::reset()
{
  sns_logi("reset - sns_direct_channel_close called");
  if ( 0 == sns_direct_channel_close(_sns_rpc_handle64)) {
    sns_logd("sns_direct_channel_close success _remote_handle_fd/_sns_rpc_handle64 : %llu/%llu \n",
                            (unsigned long long)_remote_handle_fd, (unsigned long long)_sns_rpc_handle64);
  }
  remote_handle64_close(_remote_handle_fd);
  _remote_handle_fd = -1;
  _sns_rpc_handle64 = -1;
  sns_logi("reset  sucessfull ");
}
#endif
