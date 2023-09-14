/*============================================================================
  @file direct_channel.cpp

  @brief
  direct_channel class implementation.

  Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef SNS_SUPPORT_DIRECT_CHANNEL
#include "direct_channel.h"

#include <string>
#include <cutils/native_handle.h>
#include <hardware/hardware.h>
#include <dlfcn.h>

#include "sensors_hal.h"
#include "sensors_timeutil.h"
#include "rpcmem.h"

/* Initialize the android_handle_counter */
int direct_channel::client_channel_handle_counter = 0;
void* direct_channel::llcmhandler = NULL;
direct_channel_stream_init_t direct_channel::direct_channel_stream_init = NULL;
direct_channel_stream_deinit_t direct_channel::direct_channel_stream_deinit = NULL;
direct_channel_stream_config_t direct_channel::direct_channel_stream_config = NULL;
direct_channel_stream_stop_t   direct_channel::direct_channel_stream_stop = NULL;
direct_channel_offset_update_t direct_channel::direct_channel_offset_update = NULL;
sns_remote_handles direct_channel::remote_handles;
bool direct_channel::is_llcm_initialized = false;

using android::hardware::hidl_handle;
using android::hardware::graphics::common::V1_2::BufferUsage;
using IMapper3_0 = ::android::hardware::graphics::mapper::V3_0::IMapper;
using IMapper2_0 = ::android::hardware::graphics::mapper::V2_0::IMapper;
using MapperError3_0 = android::hardware::graphics::mapper::V3_0::Error;
using MapperError2_0 = android::hardware::graphics::mapper::V2_0::Error;
using ExtensionError1_0 = ::vendor::qti::hardware::display::mapperextensions::V1_0::Error;

#define SNS_LOW_LAT_STUB_NAME "libsns_low_lat_stream_stub.so"

direct_channel::direct_channel(const struct native_handle *mem_handle,
    const size_t mem_size)
    :sns_low_lat_handle(-1),
     client_channel_handle(0)
{
    int ret = 0;

    /* Load fastRPC symbols, always load symbols for each inctance to restore from SSR */
    if (init_fastRPC_symbols()) {
      sns_loge("%s: init fastRPC symbols failed!", __FUNCTION__);
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
    mem_native_handle = native_handle_clone(mem_handle);
    if(NULL == mem_native_handle){
        sns_loge("%s: mem_native_handle is NULL", __FUNCTION__);
        return;
    }

    sns_logi("(mem_handle/):hnd %p(%d) (cloned handle/):local_hnd %p(%d)",
               (void *)mem_handle, mem_handle->data[0], mem_native_handle, mem_native_handle->data[0]);

    buffer_size = (int)mem_size;

    if (qtiMapper3 != nullptr) {
        MapperError3_0 error;
        auto result = qtiMapper3->importBuffer(hidl_handle(mem_native_handle),
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
            auto result = qtiMapper1_1->importBuffer(hidl_handle(mem_native_handle),
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
            .width = static_cast<int32_t>(buffer_size),
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
                         buffer_ptr = tmpData;
                         bytesPerPixel = tmpBytesPerPixel;
                         bytesPerStride = tmpBytesPerStride;
                      });

        if (!result.isOk() || error != MapperError3_0::NONE) {
            qtiMapper3->freeBuffer(buffer);
            native_handle_close(mem_native_handle);
            mem_native_handle = NULL;
            sns_loge("%s: lock FAILED ret %d", __FUNCTION__, error);
            return;
        }
    } else {
        IMapper2_0::Rect accessRegion = {
            .left = 0,
            .top = 0,
            .width = static_cast<int32_t>(buffer_size),
            .height = 1
        };
        hidl_handle acquireFenceHandle;
        MapperError2_0 error = MapperError2_0::NONE;

        if (qtiMapper1_1 != nullptr) {
            auto result = qtiMapper1_1->lock(buffer, BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN | BufferUsage::SENSOR_DIRECT_DATA,
                         accessRegion, acquireFenceHandle,
                         [&](const auto& tmpError, const auto& tmpData) {
                             error = tmpError;
                             buffer_ptr = tmpData;
                          });
            if (!result.isOk() || error != MapperError2_0::NONE) {
                qtiMapper1_1->freeBuffer(buffer);
                native_handle_close(mem_native_handle);
                mem_native_handle = NULL;
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
    memset(buffer_ptr, 0, (size_t)buffer_size);

    /* Map the shared memory to a fastRPC addressable file descriptor */

    if(NULL != remote_handles.reg_buf_attr) {
      remote_handles.reg_buf_attr(buffer_ptr, buffer_size, this->get_buffer_fd(), 0);
    } else if(NULL != remote_handles.reg_buf) {
      remote_handles.reg_buf(buffer_ptr, buffer_size, this->get_buffer_fd());
    }

    /* Initialize the buffer with the Low Latency library */
    ret = direct_channel_stream_init(this->get_buffer_fd(), buffer_size, 0, &sns_low_lat_handle);

    if (ret != 0) {
      sns_loge("%s: sns_low_lat_stream_init failed! ret %d", __FUNCTION__, ret);
      sns_low_lat_handle = -1;

      //TODO: shall we call unlock buffer here?
      if (qtiMapper3 != nullptr) {
        qtiMapper3->freeBuffer(buffer);
      } else if (qtiMapper1_1 != nullptr) {
        qtiMapper1_1->freeBuffer(buffer);
      }
      native_handle_close(mem_native_handle);
      mem_native_handle = NULL;
      client_channel_handle = 0;
    }

    sns_logv("%s: complete ret=%d", __FUNCTION__, ret);
    return;
}

/* Copy constructor */
direct_channel::direct_channel(const direct_channel &copy)
{
    mem_native_handle = native_handle_clone(copy.mem_native_handle);
    buffer_ptr = copy.buffer_ptr;
    buffer_size = copy.buffer_size;
    sns_low_lat_handle = copy.sns_low_lat_handle;
    client_channel_handle = copy.client_channel_handle;
}

direct_channel::~direct_channel()
{
    int ret = 0;
    int32_t dummy_var; /* Not really used */

    sns_logd("%s: (cloned handle /):local_hnd %p", __FUNCTION__, mem_native_handle);

    /* Make sure the object is valid by checking the android_handle */
    if (client_channel_handle != 0) {
      ret = direct_channel_stream_deinit(sns_low_lat_handle);
    } else {
      sns_loge("%s: deinit FAILED. direct_channel object is in a bad state.", __FUNCTION__);
    }

    if (mem_native_handle != NULL) {
        auto buffer = const_cast<native_handle_t*>(importedHandle);
        if (qtiMapper3 != nullptr) {
            MapperError3_0 error;
            auto result = qtiMapper3->unlock(buffer, [&](const auto& tmpError, const auto& tmpReleaseFence) {
                                                    error = tmpError;
                                                    sns_logi("unlock buffer, %d", tmpError);
                                                   });
            //TODO: error handling here
            qtiMapper3->freeBuffer(buffer);
        } else if (qtiMapper1_1 != nullptr) {
          MapperError2_0 error = MapperError2_0::NONE;
          auto result = qtiMapper1_1->unlock(buffer, [&](const auto& tmpError, const auto& tmpReleaseFence) {
                                                  error = tmpError;
                                                  sns_logi("unlock buffer, %d", tmpError);
                                                 });
          //TODO: error handling here
          qtiMapper1_1->freeBuffer(buffer);
        }
        native_handle_close(mem_native_handle);
        mem_native_handle = NULL;
    }
   sns_logv("%s: complete ret=%d", __FUNCTION__, ret);
}

int direct_channel::config_channel(int handle, int64 timestamp_offset, const sns_std_suid_t* sensor_suid, unsigned int sample_period_us, unsigned int flags, int sensor_handle)
{
   sns_logv("%s: ", __FUNCTION__);
   return direct_channel_stream_config(handle, timestamp_offset, sensor_suid, sample_period_us, flags, sensor_handle);
}

int direct_channel::stop_channel(int handle)
{
   sns_logv("%s: ", __FUNCTION__);
   return direct_channel_stream_stop(handle);
}

int direct_channel::get_low_lat_handle()
{
    return sns_low_lat_handle;
}

int direct_channel::get_client_channel_handle()
{
    return client_channel_handle;
}

int direct_channel::get_buffer_fd()
{
    if (mem_native_handle) {
        return mem_native_handle->data[0];
    } else {
        return -1;
    }
}

bool direct_channel::is_same_memory(const struct native_handle *mem_handle)
{
  uint64_t id1 = 0, id2 = 0;
  auto buffer1 = const_cast<native_handle_t*>(mem_handle);
  auto buffer2 = const_cast<native_handle_t*>(mem_native_handle);
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

int direct_channel::init_fastRPC_symbols(void)
{
  /* To support direct_channel properly for normal and SSR case
     two requirements should be met
     1. the stub library should not be dynamically linked because it will keep
        reference count as 1 since it's loaded when the process starts and
        will not be unloaded until the process exits.
        so, it should be dynamically loaded however, the stub library should be
        in loaded state until SSR happens. if it's loaded and unloaded every time,
        somehow, fastrpc service in DSP side can't find the right skel library name.
        Therefore, the solution is that loading the stub library once when the first
        direct_channel request comes then, don't unload it until SSR happens.
        the stub library should be loaded again after SSR happens.
     2. sensor pd session also should be opened once and should not be closed until
        SSR happens. if sensor PD is opened and closed every time, fastrpc service in
        ADSP can't find the API symbols correctly

     In order to meet above two requirements, the soltuion implemented here is

     1. open the stub library and initialize  fastrpc resource (sensor pd session will
        be oepened when fastrpc resources are initialized for the first time) when the
        first direct_channel request comes
     2. keep it until reset() is called. reset() will be called when SSR happens

     is_lcm_initialized will be reset by reset() when SSR happens by sensors_hal class
     and all llcm and fastrpc related function pointers and variables are declared as
     static to implement this solution
  */

  if (is_llcm_initialized == false) {
    sns_logi("initialize llcm resources");
    if (0 != sns_rpc_get_handle(&remote_handles)) {
     sns_loge("sns_rpc_get_handle failed");
     return -EINVAL;
    }

    sns_logd("%s: loading stub(%s) llcmhandler=%p!", __FUNCTION__, SNS_LOW_LAT_STUB_NAME, llcmhandler);
    if(NULL == (llcmhandler = dlopen(SNS_LOW_LAT_STUB_NAME, RTLD_NOW))) {
      sns_loge("%s: load stub(%s) failed !", __FUNCTION__, SNS_LOW_LAT_STUB_NAME);
      return -EINVAL;
    }
    typedef void (*sns_init_remote_handles_t)(sns_remote_handles remote_handles);
    sns_init_remote_handles_t init_remote_handles =
      (sns_init_remote_handles_t)dlsym(llcmhandler, "sns_init_remote_handles");
    direct_channel_stream_init = (direct_channel_stream_init_t)dlsym(llcmhandler, "sns_low_lat_stream_init");
    direct_channel_stream_deinit =(direct_channel_stream_deinit_t)dlsym(llcmhandler, "sns_low_lat_stream_deinit");
    direct_channel_stream_stop = (direct_channel_stream_stop_t)dlsym(llcmhandler, "sns_low_lat_stream_stop");
    direct_channel_stream_config = (direct_channel_stream_config_t)dlsym(llcmhandler, "sns_low_lat_stream_config");
    direct_channel_offset_update = (direct_channel_offset_update_t)dlsym(llcmhandler, "sns_low_lat_stream_offset_update");

    if (init_remote_handles == NULL ||
      direct_channel_stream_init == NULL || direct_channel_stream_deinit == NULL ||
      direct_channel_stream_stop == NULL || direct_channel_stream_config == NULL) {
      sns_loge("Init fastRPC symbol failed: remote_handles =%p, init=%p, deinit=%p, stop=%p, config=%p",
        init_remote_handles,
        direct_channel_stream_init,
        direct_channel_stream_deinit,
        direct_channel_stream_stop,
        direct_channel_stream_config);
      dlclose(llcmhandler);
      llcmhandler = NULL;
      return -EINVAL;
    }
    init_remote_handles(remote_handles);
    if (direct_channel_offset_update == NULL) {
      sns_loge("direct_channel_offset_update=NULL! Continue with timestamp offset update disabled.");
    }
    sns_logd("llcm resources are initialized successfully");
    is_llcm_initialized = true;
  } else {
    sns_logi("llcm resource is already initialized");
  }
  return 0;
}

void direct_channel::update_offset(int64_t new_offset)
{
  direct_channel_offset_update(new_offset, 0);
}

void direct_channel::reset(void)
{
  sns_logi("trying to reset llcm rsources");
  if (is_llcm_initialized) {
    dlclose(llcmhandler);
    llcmhandler = NULL;
    sns_rpc_reset();
    sns_logi("llcm resources are reset successfully");
    is_llcm_initialized = false;
  }
}
#endif
