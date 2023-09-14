/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WIFI_DPP_ENROLLEE_H
#define __WIFI_DPP_ENROLLEE_H


#include "xmlUtil.h"
#include "dppWpaCtrlIface.h"
#include <memory>

class DPPEnrollee {
    public:
        DPPEnrollee(std::shared_ptr<DppConfig> config_p) :
            xml_util_p_(std::make_unique<XmlUtil> (config_p->xml_file, config_p)),
            dpp_config_p_(config_p),
            ctrl_iface_p_(std::make_unique<DPPWpaCtrlIface>(config_p)) {}

        int startDppListen();

        int generateBootstrap();

        int removeBootstrap(unsigned int index);

    private:
        int addConfigurator();

        int getConfiguratorkey(std::string& key, const int configurator_idx);

        int getURIandPk(unsigned int index);

        /* data members */
        std::unique_ptr<XmlUtil> xml_util_p_;

        std::shared_ptr<DppConfig> dpp_config_p_;

        std::unique_ptr<DPPWpaCtrlIface> ctrl_iface_p_;

        std::vector<std::string> dpp_uris_list;
};

#endif /* __WIFI_DPP_ENROLLEE_H */
