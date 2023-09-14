/* ============================================================
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#include <string>
#include <unordered_map>
#include "qmi_ril_peripheral_mng.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "QtiMutex.h"
#include "PeripheralMgrImpl.h"
#include "PeripheralMgr.h"
#include "AndroidPeripheralMgrImpl.h"
#include <framework/Log.h>
#include <framework/legacy.h>
#define TAG "AndroidPeripheralMgrImpl"

AndroidPeripheralMgrImpl::AndroidPeripheralMgrImpl() {
    QCRIL_LOG_INFO("AndroidPeripheralMgrImpl constructor");
}

AndroidPeripheralMgrImpl::~AndroidPeripheralMgrImpl() {

}

/**
 * load esoc info from pm service
*/
void AndroidPeripheralMgrImpl::loadEsocInfo() {
    QCRIL_LOG_FUNC_ENTRY();
    struct dev_info devinfo;
#ifndef QMI_RIL_UTF
    if (get_system_info(&devinfo) != RET_SUCCESS) {
        QCRIL_LOG_ERROR("Could not retrieve esoc info");
        return;
    }

    if (devinfo.num_modems > 2) {
        QCRIL_LOG_ERROR("Unexpected number of modems %d",devinfo.num_modems);
        return;
    }

    unsigned short int primary_modem_idx = 0;
    unsigned short int secondary_modem_idx = 1;

    /* In the dual modem scenario, an internal modem paired with an
     * external modem is the only supported configuration. The internal
     * modem is considered the "primary" modem and the external modem
     * is considered the "secondary" modem. For example, in 4G-5G fusion
     * setup, the 4G modem is internal and primary, and the 5G modem is
     * external and secondary. */
    if (devinfo.num_modems == 2) {
        if (devinfo.mdm_list[0].type == devinfo.mdm_list[1].type) {
            QCRIL_LOG_ERROR("Invalid configuration. For dual modems, "
                            "an internal and an external modem is the "
                            "only supported configuration.");
            return;
        }

        if (devinfo.mdm_list[0].type == MDM_TYPE_EXTERNAL) {
            secondary_modem_idx = 0;
            primary_modem_idx = 1;
        }

        copySecondaryModemInfo(devinfo.mdm_list[secondary_modem_idx]);

        QCRIL_LOG_INFO("secondary modem name: %s, "
                       "secondary modem link name: %s, "
                       "secondary modem esoc_node: %s, "
                       "secondary modem type: %s",
                       mSecModemInfo.modem_name,
                       mSecModemInfo.link_name,
                       mSecModemInfo.esoc_node,
                       mSecModemInfo.type ? "internal" : "external");
    }

    copyPrimaryModemInfo(devinfo.mdm_list[primary_modem_idx]);

    QCRIL_LOG_INFO("primary modem name: %s, primary modem link name: %s, "
                   "primary modem esoc_node: %s, ",
                   "primary modem type: %s",
                   mPrimaryModemInfo.modem_name, mPrimaryModemInfo.link_name,
                   mPrimaryModemInfo.esoc_node,
                   mPrimaryModemInfo.type ? "internal" : "external");
#endif
    QCRIL_LOG_FUNC_RETURN();
}

/**
 * copy primary modem info from pm service
 **/
void AndroidPeripheralMgrImpl::copyPrimaryModemInfo(struct mdm_info &src) {
    QCRIL_LOG_FUNC_ENTRY();

    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);

    /* Read esoc node, to be used if peripheral manager is not supported */
    strlcpy(mPrimaryModemInfo.esoc_node, src.powerup_node, sizeof(mPrimaryModemInfo.esoc_node));

    /* Read modem name, to be used to register with peripheral manager */
    strlcpy(mPrimaryModemInfo.modem_name, src.mdm_name, sizeof(mPrimaryModemInfo.modem_name));

    /* Read link name to find out the transport medium to decide on qmi port */
    strlcpy(mPrimaryModemInfo.link_name, src.mdm_link, sizeof(mPrimaryModemInfo.link_name));

    mPrimaryModemInfo.type = src.type;
    QCRIL_LOG_FUNC_RETURN();

}

/**
 * copy secondary modem info from pm service
 **/
void AndroidPeripheralMgrImpl::copySecondaryModemInfo(struct mdm_info &src) {
    QCRIL_LOG_FUNC_ENTRY();

    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mSecModemInfo.mdm_mutex);

    /* Read esoc node, to be used if peripheral manager is not supported */
    strlcpy(mSecModemInfo.esoc_node, src.powerup_node, sizeof(mSecModemInfo.esoc_node));

    /* Read modem name, to be used to register with peripheral manager */
    strlcpy(mSecModemInfo.modem_name, src.mdm_name, sizeof(mSecModemInfo.modem_name));

    /* Read link name to find out the transport medium to decide on qmi port */
    strlcpy(mSecModemInfo.link_name, src.mdm_link, sizeof(mSecModemInfo.link_name));

    mSecModemInfo.type = src.type;
    QCRIL_LOG_FUNC_RETURN();

}

/**
 * register QCRIL with pm as client for modem
 **/

void AndroidPeripheralMgrImpl::registerClientWithModem() {
    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO("Handling registerClientWithModem request");

    loadEsocInfo();
#ifndef QMI_RIL_UTF
    const char *pModemName = nullptr;
    pModemName = getPrimaryModemName();

    if (pModemName &&
        !qmi_ril_peripheral_mng_register_pm_client_for_primary_modem(pModemName)) {
        QCRIL_LOG_INFO("Registered peripheral manager client for "
                   "the primary modem [%s].", pModemName);
        mPrimaryModemInfo.pm_feature_supported = true;
    }

    const char *sModemName = nullptr;
    sModemName = getSecondaryModemName();
    if (sModemName &&
        !qmi_ril_peripheral_mng_register_pm_client_for_secondary_modem(sModemName)) {
        QCRIL_LOG_INFO("Registered peripheral manager client for "
                   "the secondary modem [%s].", sModemName);
        mSecModemInfo.pm_feature_supported = true;
    }
#endif
    QCRIL_LOG_FUNC_RETURN();

}

/**
 * vote up request for primary modem
 **/
void AndroidPeripheralMgrImpl::voteUpPrimaryModem() {
    QCRIL_LOG_FUNC_ENTRY();

    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);

    const char *modemName = getPrimaryModemName();
    if (!modemName) {
        QCRIL_LOG_ERROR("Primary modem is not present.");
        return;
    }

#ifndef QMI_RIL_UTF

    if (mPrimaryModemInfo.pm_feature_supported) {
        if (qmi_ril_peripheral_mng_vote_up_primary_modem()) {
            QCRIL_LOG_ERROR("Failed to vote for primary modem [%s] "
                            "using peripheral manager.", modemName);
        }
        else {
            mPrimaryModemInfo.voting_state = 1;
            QCRIL_LOG_INFO("Successfully voted for primary modem [%s] "
                           "using peripheral manager.", modemName);
        }
    } else {

        voteModemUsingEsoc(mPrimaryModemInfo);

        if (mPrimaryModemInfo.voting_state) {
            QCRIL_LOG_INFO("Successfully voted for primary modem [%s] "
                           "using esoc node.", modemName);
        } else {
            QCRIL_LOG_ERROR("Failed to vote for primary modem [%s] "
                            "using esoc node.", modemName);
        }
    }

#endif
    QCRIL_LOG_FUNC_RETURN();

}

/**
 * vote up request for secondary modem
 **/
void AndroidPeripheralMgrImpl::voteUpSecondaryModem() {
    QCRIL_LOG_FUNC_ENTRY();

    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mSecModemInfo.mdm_mutex);

    const char *modemName = getSecondaryModemName();
    if (!modemName) {
        QCRIL_LOG_ERROR("Secondary modem is not present.");
        return; // no-op if the secondary modem is not present
    }

#ifndef QMI_RIL_UTF

    if (mSecModemInfo.pm_feature_supported) {
        if (qmi_ril_peripheral_mng_vote_up_secondary_modem()) {
            QCRIL_LOG_ERROR("Failed to vote for secondary modem [%s] "
                            "using peripheral manager.", modemName);
        } else {
            mSecModemInfo.voting_state = 1;
            QCRIL_LOG_INFO("Successfully voted for secondary modem [%s] "
                           "using peripheral manager.", modemName);
        }
    } else {
        voteModemUsingEsoc(mSecModemInfo);

        if (mSecModemInfo.voting_state) {
            QCRIL_LOG_INFO("Successfully voted for secondary modem [%s] "
                           "using esoc node.", modemName);
        } else {
            QCRIL_LOG_ERROR("Failed to vote for secondary modem [%s] "
                            "using esoc node.", modemName);
        }
    }

#endif
    QCRIL_LOG_FUNC_RETURN();
}

/**
 * vote for primary/secondary modem using ESOC
 **/
void AndroidPeripheralMgrImpl::voteModemUsingEsoc(qcril_mdm_info &modemInfo) {
    char *esoc_node_name = NULL;

    QCRIL_LOG_FUNC_ENTRY();
#ifndef QMI_RIL_UTF
    if (strlen(modemInfo.esoc_node) > 0) {
        esoc_node_name = modemInfo.esoc_node;
    }

    if (esoc_node_name) {
        if (!access(esoc_node_name, F_OK)) {
            QCRIL_LOG_INFO("esoc feature is enabled");
            modemInfo.esoc_feature_supported = true;

            if(modemInfo.type == MDM_TYPE_EXTERNAL) {
                modemInfo.esoc_fd = open(esoc_node_name, O_RDONLY);

                if (RIL_VALID_FILE_HANDLE > modemInfo.esoc_fd) {
                    modemInfo.esoc_feature_supported = false;
                    QCRIL_LOG_ERROR("Cannot open file %s", esoc_node_name);
                } else {
                    modemInfo.voting_state = 1;
                    QCRIL_LOG_INFO("Vote activated for node %s, fd %d",
                                    esoc_node_name, modemInfo.esoc_fd);
                }
            } else {
                modemInfo.esoc_feature_supported = false;
                QCRIL_LOG_INFO("Internal modem - esoc file open not required");
            }

        } else {
            QCRIL_LOG_ERROR("ESOC node %s not accessible", esoc_node_name);
        }
    } else {
        QCRIL_LOG_ERROR("ESOC node is not available");
    }
#endif
    QCRIL_LOG_FUNC_RETURN();
}

/**
 * vote down request for primary modem
 **/
void AndroidPeripheralMgrImpl::voteDownPrimaryModem() {
    QCRIL_LOG_FUNC_ENTRY();

#ifndef QMI_RIL_UTF
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);

    if (mPrimaryModemInfo.pm_feature_supported) {
        qmi_ril_peripheral_mng_vote_down_primary_modem();
        QCRIL_LOG_INFO("Successfully released vote for primary modem [%s] "
                       "using peripheral manager.",
                       getPrimaryModemName()? getPrimaryModemName(): "null");
    } else if (mPrimaryModemInfo.esoc_feature_supported) {
        // TODO: Check if close() was successful
        close(mPrimaryModemInfo.esoc_fd);
        mPrimaryModemInfo.esoc_fd = -1;
        QCRIL_LOG_INFO("Successfully released vote for primary modem [%s] "
                       "using esoc node.", mPrimaryModemInfo.esoc_node);
    }

#endif
    QCRIL_LOG_FUNC_RETURN();

}

/**
 * vote down request for secondary modem
 **/
void AndroidPeripheralMgrImpl::voteDownSecondaryModem() {
    QCRIL_LOG_FUNC_ENTRY();

#ifndef QMI_RIL_UTF
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mSecModemInfo.mdm_mutex);

    if (mSecModemInfo.pm_feature_supported)
    {
        qmi_ril_peripheral_mng_vote_down_secondary_modem();
        QCRIL_LOG_INFO("Successfully released vote for secondary modem [%s] "
                       "using peripheral manager.",
                       getSecondaryModemName() ? getSecondaryModemName() : "null");
    }
    else if (mSecModemInfo.esoc_feature_supported)
    {
        // TODO: Check if close() was successful
        close(mSecModemInfo.esoc_fd);
        mSecModemInfo.esoc_fd = -1;
        QCRIL_LOG_INFO("Successfully released vote for secondary modem [%s] "
                       "using esoc node.", mSecModemInfo.esoc_node);
    }

#endif
    QCRIL_LOG_FUNC_RETURN();
}

/**
 * request to deregister QCRIL from pm service
 **/
void AndroidPeripheralMgrImpl::deregisterClientWithModem() {
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("[PeripheralMgr]: Handling deregisterClientWithModem");
#ifndef QMI_RIL_UTF
    if (mPrimaryModemInfo.pm_feature_supported) {
        qmi_ril_peripheral_mng_deregister_pm_client_for_primary_modem();
    }

    if (mSecModemInfo.pm_feature_supported) {
        qmi_ril_peripheral_mng_deregister_pm_client_for_secondary_modem();
    }
#endif
    QCRIL_LOG_FUNC_RETURN();
}

/**
 * get voting state of primary modem
 **/
int AndroidPeripheralMgrImpl::getPrimaryModemVotingState() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);

    return mPrimaryModemInfo.voting_state;
}

/**
 * set voting state of primary modem
 **/
void AndroidPeripheralMgrImpl::setPrimaryModemVotingState(int state) {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);
    mPrimaryModemInfo.voting_state = state;
}

/**
 * get voting state of secondary modem
 **/
int AndroidPeripheralMgrImpl::getSecondaryModemVotingState() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mSecModemInfo.mdm_mutex);
    return mSecModemInfo.voting_state;
}

/**
 * get esoc link name to for baseband info
 **/
const char *AndroidPeripheralMgrImpl::getEsocLinkName() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);
    if (strlen(mPrimaryModemInfo.link_name) > 0) {
        return mPrimaryModemInfo.link_name;
    }
    return NULL;
}

/**
 * get primary modem name
 **/
const char *AndroidPeripheralMgrImpl::getPrimaryModemName() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);
    if (strlen(mPrimaryModemInfo.modem_name) > 0) {
        return mPrimaryModemInfo.modem_name;
    }

    return NULL;
}

/**
 * get secondary modem name
 **/
const char *AndroidPeripheralMgrImpl::getSecondaryModemName() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mSecModemInfo.mdm_mutex);
    if (strlen(mSecModemInfo.modem_name) > 0) {
        return mSecModemInfo.modem_name;
    }

    return NULL;
}

/**
 * check whether secondary modem is available or not
 **/
bool AndroidPeripheralMgrImpl::isSecondaryModemPresent() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mSecModemInfo.mdm_mutex);
    if (strlen(mSecModemInfo.modem_name) > 0) {
        return true;
    }
    return false;
}

/**
 * get the PM vote support on primary modem
 **/
bool AndroidPeripheralMgrImpl::pmVotingSupportPrimaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);
    return mPrimaryModemInfo.pm_feature_supported;
}

/**
 * get esoc support on primary modem
 **/
bool AndroidPeripheralMgrImpl::esocVotingSupportPrimaryModem() {
    std::lock_guard<qtimutex::QtiRecursiveMutex> lock(mPrimaryModemInfo.mdm_mutex);
    return mPrimaryModemInfo.esoc_feature_supported;
}

/*platform dependent module initialization*/
PeripheralMgrImpl* getPlatformPmModule() {
    QCRIL_LOG_INFO("getPlatformPmModule");
    static AndroidPeripheralMgrImpl mModule{};
    return &mModule;
}
