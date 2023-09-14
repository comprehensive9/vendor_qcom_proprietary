/******************************************************************************
  @file    client.cpp
  @brief   Android performance PerfLock library

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2014,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <shared_mutex>
#include <exception>
#include <thread>

#define LOG_TAG         "ANDR-PERF-CLIENT-SYS"
#include <log/log.h>
#include <cutils/properties.h>
#include "client.h"
#include "mp-ctl.h"
#include <vendor/qti/hardware/perf/2.0/IPerf.h>
#include <vendor/qti/hardware/perf/2.2/IPerf.h>
#include "PerfThreadPool.h"

#define FAILED                  -1
#define SUCCESS                 0
#define THREADS_IN_POOL 4
#define DEFAULT_OFFLOAD_FLAG 0
#define SET_OFFLOAD_FLAG 1

using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_death_recipient;
using android::hardware::hidl_vec;
using android::hardware::hidl_string;
using android::hidl::base::V1_0::IBase;
using V2_0_IPerf = vendor::qti::hardware::perf::V2_0::IPerf;
using V2_2_IPerf = vendor::qti::hardware::perf::V2_2::IPerf;
using ::android::sp;
using ::android::wp;
using std::unique_lock;
using std::shared_lock;
using std::shared_timed_mutex;
using std::timed_mutex;
using std::defer_lock;
using CM = std::chrono::milliseconds;

#define TIMEOUT 10

static sp<V2_0_IPerf> gPerfHal = NULL;
static sp<V2_2_IPerf> gPerfHalV2_2 = NULL;
static shared_timed_mutex gPerf_lock;
static timed_mutex timeoutMutex;

static int perf_hint_private(int hint, const char *pkg, int duration, int type, int offloadFlag, int numArgs, int list[]);
static void perf_event_private(int eventId, const char *pkg, int offloadFlag, int numArgs, int list[]);
struct PerfDeathRecipient : virtual public hidl_death_recipient
{
    virtual void serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) override {
        try {
            unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
            gPerfHal = NULL;
            gPerfHalV2_2 = NULL;
            ALOGE("IPerf serviceDied");
        } catch (std::exception &e) {
            QLOGE("Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE("Exception caught in %s", __func__);
        }
    }
};
static sp<PerfDeathRecipient> perfDeathRecipient = NULL;

static bool getPerfServiceAndLinkToDeath() {
    bool rc = true;
    try {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);

        if (gPerfHal == NULL) {
            read_lock.unlock();
            {
                unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
                if (gPerfHal == NULL) {
                    gPerfHal = V2_0_IPerf::tryGetService();
                    if (gPerfHal != NULL) {
                        perfDeathRecipient = new(std::nothrow) PerfDeathRecipient();
                        if (perfDeathRecipient != NULL) {
                            android::hardware::Return<bool> linked = gPerfHal->linkToDeath(perfDeathRecipient, 0);
                            if (!linked || !linked.isOk()) {
                                gPerfHal = NULL;
                                rc = false;
                                ALOGE("Unable to link to gPerfHal death notifications!");
                            }
                        } else {
                            gPerfHal = NULL;
                            rc = false;
                            ALOGE("Unable to link to gPerfHal death notifications! memory error");
                        }
                    } else {
                        rc = false;
                        QLOGE("IPerf:: Perf HAL Service 2.0 is not available.");
                    }
                }
            }
        }
    } catch (std::exception &e) {
        rc = false;
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        rc = false;
        QLOGE("Exception caught in %s", __func__);
    }
    return rc;
}

static bool getPerfServiceAndLinkToDeathV2_2() {
    bool rc = true;

    try {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHalV2_2 == NULL) {
            read_lock.unlock();
            {
                unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
                if (gPerfHalV2_2 == NULL) {
                    gPerfHalV2_2 = V2_2_IPerf::tryGetService();
                    if (gPerfHalV2_2 != NULL) {
                        perfDeathRecipient = new(std::nothrow) PerfDeathRecipient();
                        if (perfDeathRecipient != NULL) {
                            android::hardware::Return<bool> linked = gPerfHalV2_2->linkToDeath(perfDeathRecipient, 0);
                            if (!linked || !linked.isOk()) {
                                gPerfHalV2_2 = NULL;
                                rc = false;
                                ALOGE("Unable to link to gPerfHalV2_2 death notifications!");
                            }
                        } else {
                            gPerfHalV2_2 = NULL;
                            rc = false;
                            ALOGE("Unable to link to gPerfHal 2.2 death notifications! memory error");
                        }
                    } else {
                        rc = false;
                        QLOGE("IPerf:: Perf HAL Service 2.2 is not available.");
                    }
                }
            }
        }
    } catch (std::exception &e) {
        rc = false;
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        rc = false;
        QLOGE("Exception caught in %s", __func__);
    }
    return rc;
}

static int processIntReturn(const Return<int32_t> &intReturn, const char* funcName) {
    int ret = -1;
    if (!intReturn.isOk()) {
        try {
            unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
            gPerfHal = NULL;
            gPerfHalV2_2 = NULL;
            QLOGE("perf HAL service not available.");
        } catch (std::exception &e) {
            QLOGE("Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE("Exception caught in %s", __func__);
        }
    } else {
        ret = intReturn;
    }
    return ret;
}

static void processVoidReturn(const Return<void> &voidReturn, const char* funcName) {
    if (!voidReturn.isOk()) {
        try {
            unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
            gPerfHal = NULL;
            gPerfHalV2_2 = NULL;
            QLOGE("perf HAL service not available.");
        } catch (std::exception &e) {
            QLOGE("Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE("Exception caught in %s", __func__);
        }
    }
}

int perf_lock_acq(int handle, int duration, int list[], int numArgs)
{
    int rc = -1;
    QLOGI("inside %s of client", __func__);
    if (getPerfServiceAndLinkToDeath() == false) {
        return rc;
    }
    if (duration < 0 || numArgs <= 0 || numArgs > MAX_ARGS_PER_REQUEST)
        return FAILED;
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE("Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> boostsList;
        std::vector<int32_t> paramList;


        paramList.push_back(gettid());
        paramList.push_back(getpid());
        boostsList.assign(list, (list + numArgs));
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<int32_t> intReturn = gPerfHal->perfLockAcquire(handle, duration, boostsList, paramList);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfLockAcquire");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return rc;
}

int perf_lock_rel(int handle)
{
    QLOGI("inside perf_lock_rel of client");
    if (getPerfServiceAndLinkToDeath() == false) {
        return FAILED;
    }
    if (handle == 0)
        return FAILED;

    try {
        std::vector<int32_t> paramList;
        paramList.push_back(gettid());
        paramList.push_back(getpid());

        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<void> voidReturn = gPerfHal->perfLockRelease(handle, paramList);
            read_lock.unlock();
            processVoidReturn(voidReturn, "perfLockRelease");
            return SUCCESS;
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return FAILED;
}

int perf_hint(int hint, const char *pkg, int duration, int type)
{
    return perf_hint_private(hint, pkg, duration, type, DEFAULT_OFFLOAD_FLAG, 0, NULL);
}

int perf_hint_private(int hint, const char *pkg, int duration, int type, int offloadFlag, int numArgs, int list[])
{
    int rc = FAILED;
    QLOGI("inside perf_hint of client");
    if (getPerfServiceAndLinkToDeath() == false) {
        return rc;
    }

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE("Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE("Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> paramList;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        paramList.assign(list, (list + numArgs));
        paramList.emplace(paramList.begin(),offloadFlag);
        paramList.emplace(paramList.begin(),getpid());
        paramList.emplace(paramList.begin(),gettid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<int32_t> intReturn = gPerfHal->perfHint(hint, pkg_s, duration, type, paramList);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfHint");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return rc;
}

int perf_get_feedback(int req, const char *pkg) {
    int rc = -1;

    QLOGI("inside perf_get_feedback of client");
    if (getPerfServiceAndLinkToDeath() == false) {
        return rc;
    }

    try {
        std::vector<int32_t> paramList;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        paramList.push_back(gettid());
        paramList.push_back(getpid());
        paramList.push_back(DEFAULT_OFFLOAD_FLAG);

        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<int32_t> intReturn = gPerfHal->perfHint(req, pkg_s, 0, 0, paramList);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfHint");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return rc;
}

int perf_lock_use_profile(int handle, int profile)
{
    int rc = -1;
    pid_t client_tid = 0;
    if (getPerfServiceAndLinkToDeath() == false) {
        return rc;
    }

    QLOGI("inside perf_lock_use_profile of client");
    client_tid = gettid();
    try {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<int32_t> intReturn = gPerfHal->perfProfile(handle, profile, client_tid);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfProfile");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return rc;

}

void perf_lock_cmd(int cmd)
{
    pid_t client_tid = 0;

    QLOGI("inside perf_lock_cmd of client");
    if (getPerfServiceAndLinkToDeath() == false) {
        return;
    }
    client_tid = gettid();
    try {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<void> voidReturn = gPerfHal->perfLockCmd(cmd, client_tid);
            read_lock.unlock();
            processVoidReturn(voidReturn, "perfLockCmd");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
}

PropVal perf_get_prop(const char *prop , const char *def_val) {

    pid_t client_tid = 0;
    PropVal return_value = {""};
    return_value.value[0] = '\0';
    char *out = NULL;
    if (getPerfServiceAndLinkToDeath() == true) {
        out = (char *)malloc(PROP_VAL_LENGTH * sizeof(char));
    }
    if(out != NULL) {
        out[0] = '\0';
        client_tid = gettid();

        try {
            shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
            if (gPerfHal != NULL) {
                Return<void> voidReturn = gPerfHal->perfGetProp(prop, def_val, [&out](const auto &res) { strlcpy(out,res.c_str(),PROP_VAL_LENGTH);});
                read_lock.unlock();
                processVoidReturn(voidReturn, "perfGetProp");
            } else if (def_val != NULL) {
                strlcpy(out, def_val, PROP_VAL_LENGTH);
            }
            strlcpy(return_value.value, out, PROP_VAL_LENGTH);
            QLOGI("VALUE RETURNED FOR PROPERTY %s :: %s",prop,return_value.value);
        } catch (std::exception &e) {
            QLOGE("Exception caught: %s in %s", e.what(), __func__);
        } catch (...) {
            QLOGE("Exception caught in %s", __func__);
        }
        free(out);
    } else if (def_val != NULL) {
        strlcpy(return_value.value, def_val, PROP_VAL_LENGTH);
    }
    return return_value;
}

PropVal perf_wait_get_prop(const char *prop , const char *def_val) {

    static sp<V2_0_IPerf> gPerfHal_new = NULL;
    gPerfHal_new = V2_0_IPerf::getService();
    pid_t client_tid = 0;
    PropVal return_value = {""};
    return_value.value[0] = '\0';
    char *out = (char *)malloc(PROP_VAL_LENGTH * sizeof(char));

    if (out != NULL) {
        out[0] = '\0';
        client_tid = gettid();
        if (gPerfHal_new != NULL) {
           Return<void> voidReturn = gPerfHal_new->perfGetProp(prop, def_val, [&out](const auto &res) { strlcpy(out,res.c_str(),PROP_VAL_LENGTH);});
        } else if (def_val != NULL) {
           strlcpy(out, def_val, PROP_VAL_LENGTH);
        }
        strlcpy(return_value.value, out, PROP_VAL_LENGTH);
        QLOGI("VALUE RETURNED FOR PROPERTY %s :: %s ",prop,return_value.value);
        free(out);
    } else if (def_val != NULL) {
        strlcpy(return_value.value, def_val, PROP_VAL_LENGTH);
    }
    return return_value;
}

/* this function returns a malloced string which must be freed by the caller */
const char* perf_sync_request(int cmd) {
    QLOGI("perf_sync_request cmd:%d", cmd);
    char *out = NULL;
    if (getPerfServiceAndLinkToDeath() ==false) {
        return out;
    }
    try {
        std::vector<int32_t> paramList;
        paramList.push_back(gettid());
        paramList.push_back(getpid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<void> voidReturn = gPerfHal->perfSyncRequest(cmd, "", paramList,
                    [&out](const auto &res) {
                    out = strdup(res.c_str());
                    });
            read_lock.unlock();
            processVoidReturn(voidReturn, "perf_sync_request");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return out;
}

int perf_lock_acq_rel(int handle, int duration, int list[], int numArgs, int reserveNumArgs) {
    int rc = -1;
    QLOGI("inside %s of client", __func__);
    if (getPerfServiceAndLinkToDeathV2_2() == false) {
        return rc;
    }

    if (duration < 0 || numArgs <= 0 || numArgs > MAX_ARGS_PER_REQUEST ||
            reserveNumArgs < 0 || reserveNumArgs > MAX_RESERVE_ARGS_PER_REQUEST ||
            (reserveNumArgs + numArgs) > (MAX_ARGS_PER_REQUEST + MAX_RESERVE_ARGS_PER_REQUEST) ||
            (reserveNumArgs + numArgs) <= 0) {
        return FAILED;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE("Process is Flooding request to PerfService");
            return rc;
        }
        std::vector<int32_t> boostsList;
        std::vector<int32_t> reservedList;

        boostsList.assign(list, (list + numArgs));
        reservedList.assign(list + numArgs, (list + numArgs+ reserveNumArgs));
        reservedList.emplace(reservedList.begin(),getpid());
        reservedList.emplace(reservedList.begin(),gettid());
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHalV2_2 != NULL) {
            Return<int32_t> intReturn = gPerfHalV2_2->perfLockAcqAndRelease(handle, duration, boostsList, reservedList);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfLockAcqAndRelease");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    return rc;
}

void perf_event(int eventId, const char *pkg, int numArgs, int list[]) {
    QLOGI("inside %s of client", __func__);
    perf_event_private(eventId, pkg, DEFAULT_OFFLOAD_FLAG, numArgs, list);
}

void perf_event_private(int eventId, const char *pkg, int offloadFlag, int numArgs, int list[]) {

    if (getPerfServiceAndLinkToDeathV2_2() == false) {
        return;
    }


    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE("Invalid number of arguments %d", numArgs);
        return;
    }
    try {
        unique_lock<timed_mutex> lck(timeoutMutex,defer_lock);
        bool lck_acquired = lck.try_lock_for(CM(TIMEOUT));
        if (lck_acquired == false) {
            QLOGE("Process is Flooding request to PerfService");
            return;
        }
        std::vector<int32_t> reservedList;
        std::string pkg_s((pkg != NULL ? pkg: ""));
        reservedList.assign(list, (list + numArgs));
        reservedList.emplace(reservedList.begin(),offloadFlag);
        reservedList.emplace(reservedList.begin(),getpid());
        reservedList.emplace(reservedList.begin(),gettid());

        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHalV2_2 != NULL) {
            Return<void> voidReturn = gPerfHalV2_2->perfEvent(eventId, pkg_s, reservedList);
            read_lock.unlock();
            processVoidReturn(voidReturn, "perfEvent");
        }
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
}

//thread based perf hint calls
int perf_hint_offload(int hint, const char *pkg, int duration, int type, int listlen, int list[]) {
    QLOGI("inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (listlen > MAX_RESERVE_ARGS_PER_REQUEST || listlen < 0) {
        QLOGE("Invalid number of arguments %d Max Accepted %d", listlen, MAX_RESERVE_ARGS_PER_REQUEST);
        return FAILED;
    }

    if (getPerfServiceAndLinkToDeath() == false) {
        return rc;
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE("Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len);
        pkg_t[len] = '\0';
    }

    if (listlen > 0) {
        list_t = (int*)calloc(listlen, sizeof(int));
        if (list_t == NULL) {
            QLOGE("Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * listlen);
    }

    try {
        rc = ptp.placeTask([=]() mutable {
                    int handle = perf_hint_private(hint, pkg_t, duration, type, SET_OFFLOAD_FLAG, listlen, list_t);
                    if (pkg_t != NULL) {
                       free(pkg_t);
                       pkg_t = NULL;
                    }
                    if (list_t != NULL) {
                        free(list_t);
                        list_t = NULL;
                    }
                });
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE("Failed to Offload hint: 0x%X", hint);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    }
    return rc;
}

int perf_event_offload(int eventId, const char *pkg, int numArgs, int list[]) {
    QLOGI("inside %s of client", __func__);
    int rc = FAILED;
    char *pkg_t = NULL;
    int *list_t = NULL;

    if (numArgs > MAX_RESERVE_ARGS_PER_REQUEST || numArgs < 0) {
        QLOGE("Invalid number of arguments %d", numArgs);
        return FAILED;
    }
    if (getPerfServiceAndLinkToDeathV2_2() == false) {
        return rc;
    }

    if (pkg != NULL) {
        int len = strlen(pkg);
        pkg_t = (char*)calloc(len + 1,sizeof(char));
        if (pkg_t == NULL) {
            QLOGE("Memory alloc error in %s", __func__);
            return rc;
        }
        strlcpy(pkg_t, pkg, len);
        pkg_t[len] = '\0';
    }
    if (numArgs > 0) {
        list_t = (int*)calloc(numArgs, sizeof(int));
        if (list_t == NULL) {
            QLOGE("Memory alloc error in %s", __func__);
            if (pkg_t != NULL) {
                free(pkg_t);
                pkg_t = NULL;
            }
            return rc;
        }
        memcpy(list_t, list, sizeof(int) * numArgs);
    }

    PerfThreadPool &ptp = PerfThreadPool::getPerfThreadPool(THREADS_IN_POOL);
    try {
        rc = ptp.placeTask([=]() mutable {
                    perf_event_private(eventId, pkg_t, SET_OFFLOAD_FLAG, numArgs, list_t);
                    if (pkg_t != NULL) {
                        free(pkg_t);
                        pkg_t = NULL;
                    }
                    if (list_t != NULL) {
                        free(list_t);
                        list_t = NULL;
                    }
                });
    } catch (std::exception &e) {
        QLOGE("Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE("Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE("Failed to Offload event: 0x%X", eventId);
        if (pkg_t != NULL) {
            free(pkg_t);
            pkg_t = NULL;
        }
        if (list_t != NULL) {
            free(list_t);
            list_t = NULL;
        }
    }
    return rc;
}
