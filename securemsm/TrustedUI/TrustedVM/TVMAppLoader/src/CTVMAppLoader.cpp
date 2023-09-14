/*========================================================================
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#include <linux/mem-buf.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <object.h>

#include "TUILog.h"
#include "TUIUtils.h"
#include "minkipc.h"
#include "IRegisterApp.h"
#include "openssl/err.h"
#include "openssl/hmac.h"
#include "openssl/sha.h"

#include "CAppClient.h"
#include "CTUIGetAuthInfo.h"
#include "IAppClient.h"
#include "IClientEnv.h"
#include "IOpener.h"
#include "ITUIGetAuthInfo.h"
#include "ITunnelInvokeService.h"
#include "ITVMAppLoader_invoke.hpp"

#define TUI_SECRET_KEY_SIZE 8
#define QSEE_HMAC_DIGEST_SIZE_SHA256 32

// membuf doesn't map physical pages without 2mb alignment
#define SIZE_2MB 0x200000

extern Object tvmAppOpener;
Object mTUIAppObject = Object_NULL;
void *mAppHandle = nullptr;

typedef int32_t (*opener)(Object *);
typedef int32_t (*cleanup)(void);

struct authinfo {
    uint8_t key[TUI_SECRET_KEY_SIZE];
    uint8_t tzHmac[QSEE_HMAC_DIGEST_SIZE_SHA256];
};

class CTVMAppLoader : public TVMAppLoaderImplBase {
   public:
    CTVMAppLoader() = default;
    virtual ~CTVMAppLoader() = default;
    virtual int32_t loadApp(uint32_t UID_val, uint32_t memparcel_handle_val,
                            uint32_t mem_size_val, Object &appObject);
    virtual int32_t unloadApp(uint32_t UID_val);

   private:
    int32_t saveBufToFile(void *library, uint32_t libSize);
    int32_t getAppObject(uint32_t UID_val);
    int32_t getTUIAuthAppInfo(authinfo *authHandle, uint32_t UID_val);
    int32_t computeHMAC(authinfo authHandle, uint8_t *leHmac,
                        uint32_t mem_size_val, void *library,
                        size_t len);
    bool    isAppImageValid(uint8_t *leHmac, uint8_t *tzHmac);

    struct acl_entry mAclEntries = {
        .vmid = MEM_BUF_VMID_TRUSTED_VM,
        .perms = MEM_BUF_PERM_FLAG_READ,
    };
    struct mem_buf_import_ioctl_arg importArgs = {};
    constexpr static char *mbinPath = "/tmp/libTUIApp.so";
};

int32_t CTVMAppLoader::getTUIAuthAppInfo(authinfo *authHandle, uint32_t UID_val)
{
    int32_t ret = 0;
    const char *tunnelSocketPath = "/dev/socket/le_tisock";
    const char *tuiauth = "tuiauthapp";
    Object tunnelInvokeOpener = Object_NULL;
    Object clientEnvTunnel = Object_NULL;
    Object appClient = Object_NULL;
    Object appObj = Object_NULL;
    Object tuiAuthAppObj = Object_NULL;
    size_t keyLenout = 0;
    size_t hmacLenout = 0;

    MinkIPC *tunnelMinkIPC =
      MinkIPC_connect(tunnelSocketPath, &tunnelInvokeOpener);
    if (!tunnelMinkIPC || Object_isNull(tunnelInvokeOpener)) {
        TUILOGE("%s::%d Error: Failed to get Tunnel Invoke object", __func__,
              __LINE__);
        ret = Object_ERROR;
        goto end;
    }

    ret =
        ITunnelInvokeService_getClientEnv(tunnelInvokeOpener, &clientEnvTunnel);

    if (Object_isERROR(ret) || Object_isNull(clientEnvTunnel)) {
        TUILOGE("%s::%d Failed to get Client Env from Tunnel Invoke, ret - %d",
              __func__, __LINE__, ret);
        ret = Object_ERROR;
        goto end;
    }

    ret = IClientEnv_open(clientEnvTunnel, CAppClient_UID, &appClient);
    if (Object_isERROR(ret) || Object_isNull(appClient)) {
        TUILOGE("%s::%d Failed to get app client object, ret - %d", __func__,
              __LINE__, ret);
        ret = Object_ERROR;
        goto end;
    }

    ret = IAppClient_getAppObject(appClient, tuiauth, strlen(tuiauth), &appObj);
    if (Object_isERROR(ret) || Object_isNull(appObj)) {
        TUILOGE("%s::%d Failed to get App Object, ret - %d", __func__, __LINE__,
              ret);
        ret = Object_ERROR;
        goto end;
    }

    ret = IOpener_open(appObj, CTUIGetAuthInfo_UID, &tuiAuthAppObj);
    if (Object_isERROR(ret) || Object_isNull(tuiAuthAppObj)) {
        TUILOGE("%s::%d Failed to get tui auth app object, ret - %d", __func__,
                __LINE__, ret);
        ret = Object_ERROR;
        goto end;
    }

    memset(authHandle->key, sizeof(authHandle->key), 0);
    memset(authHandle->tzHmac, sizeof(authHandle->tzHmac), 0);

    // Get Key and HMAC from authapp
    ret = ITUIGetAuthInfo_getAuthInfo(tuiAuthAppObj, UID_val, authHandle->key,
                                      sizeof(authHandle->key), &keyLenout,
                                      authHandle->tzHmac, sizeof(authHandle->tzHmac),
                                      &hmacLenout);
    TUI_CHECK_ERR(ret == 0, ret);

end:
    Object_ASSIGN_NULL(tuiAuthAppObj);
    Object_ASSIGN_NULL(appObj);
    Object_ASSIGN_NULL(appClient);
    Object_ASSIGN_NULL(clientEnvTunnel);
    Object_ASSIGN_NULL(tunnelInvokeOpener);

    if (tunnelMinkIPC) {
        MinkIPC_release(tunnelMinkIPC);
        tunnelMinkIPC = NULL;
    }

    return ret;
}

int32_t CTVMAppLoader::computeHMAC(authinfo authHandle, uint8_t *leHmac,
                                   uint32_t mem_size_val, void *library,
                                   size_t len)
{
    unsigned int md_len = QSEE_HMAC_DIGEST_SIZE_SHA256;
    int32_t ret = 0;
    if (!HMAC(EVP_sha256(), authHandle.key, sizeof(authHandle.key),
        (unsigned const char *)library, len, leHmac, &md_len)) {
        TUILOGE("error computing hmac using openssl");
        ret = -1;
    }
    return ret;
}

bool CTVMAppLoader::isAppImageValid(uint8_t *leHmac, uint8_t *tzHmac)
{
    if (memcmp(tzHmac, leHmac, sizeof(leHmac))) {
        TUILOGE("tz hmac and le hmac dosen't match, exit");
        return false;
    }
    return true;
}

int32_t CTVMAppLoader::saveBufToFile(void *library, uint32_t libSize)
{
    int32_t libFd;
    int32_t ret = 0;

    // opens /tmp/libTUIApp.so with 750 permissions
    libFd = open(mbinPath, O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP);
    TUI_CHECK_ERR(libFd > 0, libFd);

    ret = write(libFd, library, libSize);
    TUI_CHECK_ERR(ret == libSize, -1);

    TUILOGD("%s: wrote %d bytes to libFd", __func__, ret);

end:
    if (ret) {
      if (libFd > 0) {
        close(libFd);
      }
    }

    TUILOGD("%s::%d ret = %d", __func__, __LINE__, ret);
    return ret;
}

int32_t CTVMAppLoader::getAppObject(uint32_t UID_val)
{
    opener app_init = nullptr;
    int32_t ret = 0;

    mAppHandle = dlopen(mbinPath, RTLD_NOW);
    if (mAppHandle == nullptr) {
        TUILOGE(dlerror());
        goto end;
    }

    app_init = (opener)dlsym(mAppHandle, "CApp_init");
    if (app_init == nullptr) {
        TUILOGE(dlerror());
        goto end;
    }

    ret = app_init(&mTUIAppObject);
    if ((ret != 0) || Object_isNull(mTUIAppObject)) {
        TUILOGE("Failed to get TUI App object");
        goto end;
    }

    return Object_OK;

end:
    if (mAppHandle != NULL) {
        dlclose(mAppHandle);
        mAppHandle = NULL;
    }
    Object_ASSIGN_NULL(mTUIAppObject);
    return Object_ERROR_BADOBJ;
}

int32_t CTVMAppLoader::loadApp(uint32_t UID_val, uint32_t memparcel_handle_val,
                               uint32_t mem_size_val, Object &appObject)
{
    int32_t ret = 0;
    int32_t memBufFd = -1;
    void *library = nullptr;
    uint8_t leHmac[QSEE_HMAC_DIGEST_SIZE_SHA256];
    bool isValid;
    size_t len = 0;
    authinfo authHandle;

    memBufFd = open("/dev/membuf", O_RDONLY);
    TUI_CHECK_ERR(memBufFd > 0, memBufFd);

    importArgs.memparcel_hdl = memparcel_handle_val;
    importArgs.nr_acl_entries = 1;
    importArgs.acl_list = (__u64)&mAclEntries;

    TUILOGD("%s: importing handle 0x%x\n", __func__, memparcel_handle_val);
    ret = ioctl(memBufFd, MEM_BUF_IOC_IMPORT, &importArgs);
    TUI_CHECK_ERR(ret == 0, ret);

    len = (mem_size_val + (SIZE_2MB -1)) & (~(SIZE_2MB - 1));
    // map library
    library = mmap(
            nullptr, len, PROT_READ, MAP_SHARED, importArgs.dma_buf_import_fd, 0);

    TUI_CHECK_ERR((library != MAP_FAILED), -1);

    // get key and hash from tuiauthapp via tunelled invoke
    ret = getTUIAuthAppInfo(&authHandle, UID_val);
    TUI_CHECK_ERR(ret == 0, ret);

    // compute HMAC on LE side via openssl
    ret = computeHMAC(authHandle, leHmac, mem_size_val, library, len);
    TUI_CHECK_ERR(ret == 0, ret);

    // match leHmac and tzHmac, proceed only when it matches
    isValid = isAppImageValid(leHmac, authHandle.tzHmac);
    TUI_CHECK_ERR(isValid, -1);

    TUILOGD("App binary is successfully authenticated");

    ret = saveBufToFile(library, mem_size_val);
    TUI_CHECK_ERR(ret > 0, ret);

    TUILOGD("%s: Saved imported membuf to %s", __func__, mbinPath);

    ret = getAppObject(UID_val);
    TUI_CHECK_ERR(ret == 0, ret);

    // return app object to the client
    appObject = mTUIAppObject;

end:
    if (memBufFd > 0) {
        close(memBufFd);
    }
    if (importArgs.dma_buf_import_fd > 0) {
        close(importArgs.dma_buf_import_fd);
    }
    if (library != nullptr) {
        munmap((void *)library, mem_size_val);
    }

    TUILOGD("%s::%d ret = %d", __func__, __LINE__, ret);
    return ret;
}

int32_t CTVMAppLoader::unloadApp(uint32_t UID_val)
{
    int32_t ret = 0;
    int i = 0;

    ret = dlclose(mAppHandle);
    if (ret < 0) {
        TUILOGE("%s: dlclose failed for mAppHandle", __func__);
    }

    ret = unlink(mbinPath);
    if (ret < 0) {
        TUILOGE("%s: Failed to remove file [%s]", __func__, mbinPath);
    }

end:
    return ret;
}

int32_t CTVMAppLoader_open(Object *objOut)
{
    int32_t ret = 0;
    TUILOGD("%s ++", __func__);
    CTVMAppLoader *me = new CTVMAppLoader();
    TUI_CHECK_ERR(me, Object_ERROR_KMEM);

    *objOut = (Object) { ImplBase::invoke, me };
    TUILOGD("%s --", __func__);
end:
    return Object_OK;
}

int32_t CTVMAppLoader_cleanup()
{
    cleanup app_deinit = nullptr;
    if (mAppHandle != NULL) {
        app_deinit = (cleanup)dlsym(mAppHandle, "CApp_deinit");
        if (app_deinit == nullptr) {
            TUILOGE(dlerror());
            goto end;
        }
        app_deinit();
    }
    return Object_OK;

end:
    if (mAppHandle != NULL) {
        dlclose(mAppHandle);
        mAppHandle = NULL;
    }

    return Object_ERROR_BADOBJ;
}
