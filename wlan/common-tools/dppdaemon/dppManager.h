/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WIFI_DPP_MANAGER_H
#define __WIFI_DPP_MANAGER_H

#include "dppEnrollee.h"

class DPPManager
{
    public:
        DPPManager(std::shared_ptr<DppConfig> dpp_config_p) :
            authrole_check_(true),
            provrole_check_(true),
            netrole_check_(true),
            dpp_config_p_(dpp_config_p),
            enrollee_p_(nullptr) {}

        ~DPPManager();

        int ProcessCommand(const std::string& cmd);

    private:
        void InitialiseLogging();

        bool IsAuthRoleCheck() const {
            return authrole_check_;
        }

        bool IsNetRoleCheck() const {
            return netrole_check_;
        }

        bool IsProvRoleCheck() const {
            return provrole_check_;
        }

        int StartEnrolleeAsResponder() const;

        void StopEnrolleeAsResponder();

        void CleanUpDpp();

        void SetInterface(const std::string& ifname);

        void SetClientPath(std::string& path);

        void SetNetRole(const std::string& netrole);

        void SetProvRole(const std::string& prov_role);

        void SetAuthRole(const std::string& auth_role);

        void SetXmlPath(const std::string file);

        void SetLogDirPath(const std::string path);

        NetRole GetNetRole() const { return dpp_config_p_->netrole; };

        ProvRole GetProvRole() const { return dpp_config_p_->prov_role; };

        AuthRole GetAuthRole() const { return dpp_config_p_->auth_role; };

        /* data members */
        bool authrole_check_;
        bool provrole_check_;
        bool netrole_check_;

        std::shared_ptr<DppConfig> dpp_config_p_;
        std::unique_ptr<DPPEnrollee> enrollee_p_;
};

#endif /* __WIFI_DPP_MANAGER_H */
