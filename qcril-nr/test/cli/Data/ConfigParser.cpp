/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2019, The Linux Foundation. All rights reserved
 */

#include "ConfigParser.hpp"

/**
 * Check if a file exists.
 */
inline bool fileExists(const std::string& configFile) {
  std::ifstream f(configFile.c_str());
  return f.good();
}

std::string trim(const std::string& str) {
  return std::regex_replace(str, std::regex("^ +| +$|( ) +"), "$1");
}

bool isEqual(std::string& str1, std::string& str2) {
  return ((str1.size() == str2.size()) &&
          std::equal(str1.begin(), str1.end(), str2.begin(), [](char& c1, char& c2) {
            return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
          }));
}

/**
 * @param [in] section       - Parse the config file for given section and build the hashmap with
 *                             key=value pair
 */
ConfigParser::ConfigParser(std::string section, std::string configFile) {
  std::string configFilePath = configFile;
  if (fileExists(configFilePath)) {
    section_ = section;
    readConfigFile(configFilePath);
  } else {
    std::cout << "Config file does not exists: " << configFilePath << std::endl;
  }
}

ConfigParser::~ConfigParser() {
}

/**
 * Returns the vector of key,value map from config file
 *
 */
std::vector<std::map<std::string, std::string>> ConfigParser::getFilters() {
  return configVector_;  // return an empty string when the setting is not configured.
}

/**
 * Returns the corresponding value for provided key in map
 * param [in] pairMap       - map returned via @ref getFilters
 * param [in] key           - key for matching value is required
 *
 * @returns valid value or empty string in case of Error.
 */
std::string ConfigParser::getValue(std::map<std::string, std::string> pairMap, std::string key) {
  auto settingsIterator = pairMap.find(key);
  if (settingsIterator != pairMap.end()) {
    return settingsIterator->second;
  } else {
    // return an empty string when the setting is not configured.
    return std::string("");
  }
}

/**
 * Get the config file path. Config file is expected to be present in the same
 * location from where application is running.
 *
 */
std::string ConfigParser::getConfigFilePath() {
  char path[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
  std::string fullPath = std::string(path, (count > 0) ? count : 0);
  auto const pos = fullPath.find_last_of('/');
  return fullPath.substr(0, pos);
}

/**
 * Utility function to read config file with key value pairs
 * Prepares a map of key value pairs from Key=Value format
 * Discards leading spaces, blank lines and lines starting with #
 * Removes any leading or trailing spaces around Key and Value if any.
 */
void ConfigParser::readConfigFile(std::string configFile) {
  std::string current;

  // regular expressions to process configuration files filter section
  std::regex sectionMatch_("\\[(.*?)\\]");
  // regular expressions to process configuration files key=value pairs
  std::regex keyValueMatch_("(\\w+)=([^\\+]+(?!\\+{3}))");

  // Create a file stream from the file name
  std::ifstream configFileStream(configFile);

  // Iterate through each parameter in the file and read the key value pairs
  std::string param;

  bool addFilter = false;
  // assuming we've opened the file ok into a
  // filestream object called "mapfile"

  std::map<std::string, std::string> configMap_;

  while (std::getline(configFileStream, param)) {
    param = trim(param);

    // Trim all the spaces between fields for regex to work
    std::string::iterator end_pos = std::remove(param.begin(), param.end(), ' ');
    param.erase(end_pos, param.end());

    if (param.length() > 0) {
      std::smatch match;

      if (std::regex_search(param, match, sectionMatch_)) {
        // any key-value pairs from here to be attributed
        // to this new name
        current = trim(match.str(1));
        if (isEqual(current, section_)) {
          if (addFilter) {
            configVector_.push_back(configMap_);
            configMap_.clear();
          }
          addFilter = true;
        } else {
          if (addFilter) {
            configVector_.push_back(configMap_);
            addFilter = false;
            configMap_.clear();
          }
        }

      } else if (std::regex_search(param, match, keyValueMatch_)) {
        // trim the leading and trailing spaces
        std::string value1 = trim(match.str(1));
        std::string value2 = trim(match.str(2));

        if (addFilter) {
          configMap_[value1] = value2;
        }
      }
    }
  }

  if (addFilter) {
    configVector_.push_back(configMap_);
    addFilter = false;
  }
}
