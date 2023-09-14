/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WIFI_DPP_XMLUTIL_H
#define __WIFI_DPP_XMLUTIL_H

#include "dppCommon.h"

#include <vector>

class XmlUtil {
    public:
        XmlUtil(const std::string& filename,
                std::shared_ptr<DppConfig> dpp_config_p);

        int getChannel();

        int getOpClass();

        int getChannelFreq(const int channel, const int opclass);

        std::string getMacAddress();

        std::string getDppKey();

        std::string getPK();

        std::string getURI();

        void AddTagToXml(const std::string& tag, const std::string& val);

        void RemoveTagFromXml(const std::string& tag);

        bool IsXmlValid() const { return xml_validation_; }

    private:
        std::string GetDataByTag(std::string tag);

        /* data memebers */
        std::string filename_;

        std::string content_;

        bool xml_validation_;

        std::vector<std::string> xml_tags_present;

        std::shared_ptr<DppConfig> dpp_config_p_;
};
#endif /* __WIFI_DPP_XMLUTIL_H */
