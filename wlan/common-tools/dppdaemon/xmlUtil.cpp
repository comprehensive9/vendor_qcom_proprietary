/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2002-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed, used, and modified under the terms of
 * BSD license:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name(s) of the above-listed copyright holder(s) nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "xmlUtil.h"

#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

/* XML format */
/* XML file enclosing tags */
constexpr auto XML_FILE_ENCLOSING_TAG = "dpp-enrollee-info";
const std::unordered_map<std::string, int> tag_line_idx_map = {
    {"opclass", 3},
    {"channel", 4},
    {"mac-address", 5},
    {"dpp-key", 6},
    {"public-key", 7},
    {"uri", 8},
};

static bool ValidateXmlTagPosition(const DppConfig* dpp_config_p,
                                   const std::string& xml_content,
                                   const std::vector<std::string>& xml_tags) {
    auto count_lines = [](const std::string& text) {
        int new_lines = 0;
        for (auto ch : text) {
            if (ch == '\n') ++new_lines;
        }
        return new_lines;
    };

    for (const auto& tag : xml_tags) {
        auto tag_pos = xml_content.find("</" + tag + '>');
        if (tag_pos == std::string::npos) {
            dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                             "[%s] Missing closing tag for %s",
                             __func__, tag.c_str());
            return false;
        }

        int line_idx_found =
            count_lines(xml_content.substr(0, tag_pos + tag.length() + 4));
        int expected_idx_found = tag_line_idx_map.at(tag);
        if ( line_idx_found != expected_idx_found) {
            dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                             "[%s] Xml line position mismatch for tag %s "
                             "expected in line %d found in line %d",
                             __func__, tag.c_str(), expected_idx_found, line_idx_found);
            return false;
        }
    }
    return true;
}

static bool ValidateXmlTag(const DppConfig* dpp_config_p,
                           const std::string &content,
                           const std::vector<std::string> &mandatory_tags,
                           std::vector<std::string>& xml_tags_present) {
    std::regex tags_start_regex("<((\\w+)|(\\w+-\\w+)|(\\w+-\\w+-\\w+))>");
    std::regex tags_end_regex("</((\\w+)|(\\w+-\\w+)|(\\w+-\\w+-\\w+))>");
    auto first_tags_words_begin =
        std::sregex_iterator(content.begin(), content.end(), tags_start_regex);
    auto closing_tags_words_begin =
        std::sregex_iterator(content.begin(), content.end(), tags_end_regex);
    auto first_tags_words_end = std::sregex_iterator();
    auto closing_tags_words_end = std::sregex_iterator();
    auto start = std::sregex_iterator();
    auto end = std::sregex_iterator();
    std::unordered_map<std::string, bool> mandatory_tags_check;
    for (const auto &tag : mandatory_tags) {
        mandatory_tags_check.insert({tag, false});
    }

    ++first_tags_words_begin; /* skips first XML enclosing tag */
    for (start = first_tags_words_begin, end = closing_tags_words_begin;
         start != first_tags_words_end && end != closing_tags_words_end;
         ++start, ++end) {
        std::smatch match_first = *start;
        std::smatch match_close = *end;
        std::string match_str1  = match_first.str();
        std::string         tag = match_str1.substr(1, match_str1.length() - 2);
        std::string closing_tag = match_close.str();
        if (tag_line_idx_map.find(tag) == tag_line_idx_map.end()) {
            /* Some invalid tag is present in xml */
            dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                             "[%s] Invalid tag %s found in XML",
                             __func__, tag.c_str());
            return false;
        }
        if (mandatory_tags_check.find(tag) != mandatory_tags_check.end()) {
            mandatory_tags_check.at(tag) = true;
        }
        if (closing_tag != "</" + tag + '>') {
            /* opening and closing tags didn't match */
            dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                             "[%s] Missing closing tag for %s",
                             __func__, tag.c_str());
            return false;
        }
        xml_tags_present.emplace_back(tag);
    }
    /* Now match XML enclosing tag */
    if (end == closing_tags_words_end) {
        /* Enclosing tag is not present */
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] closing tag for %s is not present",
                         __func__, XML_FILE_ENCLOSING_TAG);
        return false;
    }
    if ((*end).str() != "</" + std::string(XML_FILE_ENCLOSING_TAG) + '>') {
        dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                         "[%s] Invalid enclosing tag %s found in xml",
                         __func__, (*end).str().c_str());
        return false;
    }
    /* Now check if all mandatory tags are present */
    for (const auto& pai : mandatory_tags_check) {
        if (!pai.second) {
            dpp_daemon_print(dpp_config_p, DPPDAEMON_MSG_ERROR,
                             "[%s] mandatory tag %s is not present in xml",
                             __func__, pai.first.c_str());
            return false;
        }
    }

    return true;
}

static bool ValidateXmlFile(const DppConfig* dpp_config_p,
                            const std::string& text,
                            std::vector<std::string>& xml_tags_present) {
    if (text.find('<' + std::string(XML_FILE_ENCLOSING_TAG) + '>')  ==
        std::string::npos ||
        text.find("</" + std::string(XML_FILE_ENCLOSING_TAG) + '>') ==
       std::string::npos) { /* Enclosing tags are not present */
        return false;
    }

    if (!ValidateXmlTag(dpp_config_p, text, {"opclass", "channel"}, xml_tags_present)) {
        /* Failed to validate each tag */
        return false;
    }

    if (!ValidateXmlTagPosition(dpp_config_p, text, xml_tags_present)) {
        return false;
    }

    /* opclass and channel are mandatory tags */
    return true;
}

static int ieee80211_chan_to_freq_global(int op_class, int chan) {
    /* Table E-4 in IEEE Std 802.11-2012 - Global operating classes */
    switch (op_class) {
        case 81:
            /* channels 1..13 */
            if (chan < 1 || chan > 13) {
                return -1;
            }
            return 2407 + 5 * chan;
        case 82:
            /* channel 14 */
            if (chan != 14) {
                return -1;
            }
            return 2414 + 5 * chan;
        case 83: /* channels 1..9; 40 MHz */
        case 84: /* channels 5..13; 40 MHz */
            if (chan < 1 || chan > 13)
                return -1;
            return 2407 + 5 * chan;
        case 115: /* channels 36,40,44,48; indoor only */
        case 116: /* channels 36,44; 40 MHz; indoor only */
        case 117: /* channels 40,48; 40 MHz; indoor only */
        case 118: /* channels 52,56,60,64; dfs */
        case 119: /* channels 52,60; 40 MHz; dfs */
        case 120: /* channels 56,64; 40 MHz; dfs */
            if (chan < 36 || chan > 64)
                return -1;
            return 5000 + 5 * chan;
        case 121: /* channels 100-140 */
        case 122: /* channels 100-142; 40 MHz */
        case 123: /* channels 104-136; 40 MHz */
            if (chan < 100 || chan > 140)
                return -1;
            return 5000 + 5 * chan;
        case 124: /* channels 149,153,157,161 */
        case 126: /* channels 149,157; 40 MHz */
        case 127: /* channels 153,161; 40 MHz */
            if (chan < 149 || chan > 161)
                return -1;
            return 5000 + 5 * chan;
        case 125: /* channels 149,153,157,161,165,169 */
            if (chan < 149 || chan > 169)
                return -1;
            return 5000 + 5 * chan;
        case 128: /* center freqs 42, 58, 106, 122, 138, 155; 80 MHz */
        case 130: /* center freqs 42, 58, 106, 122, 138, 155; 80 MHz */
            if (chan < 36 || chan > 161)
                return -1;
            return 5000 + 5 * chan;
        case 129: /* center freqs 50, 114; 160 MHz */
            if (chan < 36 || chan > 128)
                return -1;
            return 5000 + 5 * chan;
        case 180: /* 60 GHz band, channels 1..4 */
            if (chan < 1 || chan > 4)
                return -1;
            return 56160 + 2160 * chan;
    }

    return -1;
}


XmlUtil::XmlUtil(const std::string &filename,
                 std::shared_ptr<DppConfig> dpp_config_p) {
    dpp_config_p_ = dpp_config_p;
    filename_ = filename;
    std::ifstream in(filename.empty() ? "default.xml" : filename.c_str());
    if (in.is_open()) {
        std::string line;
        while (getline(in, line)) {
            content_ += line + '\n';
        }
    }
    in.close();
    xml_validation_ = ValidateXmlFile(dpp_config_p_.get(),
                                      content_, xml_tags_present);
}


std::string XmlUtil::GetDataByTag(std::string tag) {
    int len = tag.size();

    auto start = content_.find("<" + tag + ">");
    if (start == std::string::npos) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] xml file is corrupted, no tag named %s",
                         __func__, tag.c_str());
        return "";
    }
    auto end = content_.find("</" + tag);
    if (end == std::string::npos) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] xml file is corrupted, no tag named %s",
                         __func__, tag.c_str());
        return "";
    }
    start += len + 2;

    return content_.substr(start, end - start);
}

void XmlUtil::AddTagToXml(const std::string& tag, const std::string& val) {
    std::string xml_write_str = "\n  <" + tag + '>' + val + "</" + tag + ">\n";
    if (tag_line_idx_map.find(tag) == tag_line_idx_map.end()) {
        return;
    }
    std::fstream file(filename_.empty() ? "default.xml" : filename_.c_str(),
                      std::ios::in | std::ios::out | std::ios::trunc);
    /* skips lines */
    size_t start_pos = 0;
    int new_lines = 1;
    for (auto ch : content_) {
        if (ch == '\n' && ++new_lines == tag_line_idx_map.at(tag)) {
            break;
        }
        start_pos++;
    }
    content_ =
        content_.substr(0, start_pos) + xml_write_str +
        content_.substr(start_pos + 1,
                        content_.find(XML_FILE_ENCLOSING_TAG, start_pos + 1) +
                            std::strlen(XML_FILE_ENCLOSING_TAG));
    file.write(content_.c_str(), content_.size());
    file.close();

    return;
}

void XmlUtil::RemoveTagFromXml(const std::string& tag) {
    auto start_pos = content_.find('<' + tag + '>');
    auto end_pos   = content_.find("</" + tag + '>');
    if (start_pos == std::string::npos || end_pos == std::string::npos) {
        dpp_daemon_print(dpp_config_p_.get(), DPPDAEMON_MSG_ERROR,
                         "[%s] No tag named %s in xml file",
                         __func__, tag.c_str());
        return;
    }
    std::fstream file(filename_.empty() ? "default.xml" : filename_.c_str(),
                      std::ios::in | std::ios::out | std::ios::trunc);
    content_ = content_.erase(start_pos,
                              end_pos + tag.length() + 3 - start_pos + 1);
    file.write(content_.c_str(), content_.size());
    file.close();
    return;
}

int XmlUtil::getChannelFreq(const int channel, const int opclass) {
    return ieee80211_chan_to_freq_global(opclass, channel);
}

int XmlUtil::getChannel() {
    auto channel_str = GetDataByTag("channel");
    if (channel_str.empty()) {
        return -1;
    }
    return std::atoi(channel_str.c_str());
}

int XmlUtil::getOpClass() {
    auto opclass_str = GetDataByTag("opclass");
    if (opclass_str.empty()) {
        return -1;
    }
    return std::atoi(opclass_str.c_str());
}

std::string XmlUtil::getMacAddress() {
    return GetDataByTag("mac-address");
}

std::string XmlUtil::getDppKey() {
    return GetDataByTag("dpp-key");
}

std::string XmlUtil::getPK() {
    return GetDataByTag("public-key");
}

std::string XmlUtil::getURI() {
    return GetDataByTag("uri");
}
