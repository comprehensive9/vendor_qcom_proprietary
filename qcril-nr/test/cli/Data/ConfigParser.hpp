/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2019, The Linux Foundation. All rights reserved
 */

/**
 * @brief ConfigParser class reads config file and caches the app config
 * settings. It provides utility functions to read the config values (key=value pair).
 */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>

extern "C" {
#include <limits.h>
#include <unistd.h>
}

#define DEFAULT_CONFIG_FILE_NAME "/data/Datafilter.conf"

/*
 * ConfigParser class caches the config settings from conf file
 * It provides utility methods to get value from configuration file in key,value form.
 */
class ConfigParser {
 public:
  ConfigParser(std::string section, std::string configFile = DEFAULT_CONFIG_FILE_NAME);
  ~ConfigParser();
  // Get the user defined value for configured key
  std::string getValue(std::map<std::string, std::string> pairMap_, std::string key);
  std::vector<std::map<std::string, std::string>> getFilters();

 private:
  std::string section_;
  // Function to read config file containing key value pairs
  void readConfigFile(std::string configFile);
  // Get the path where config file is located
  std::string getConfigFilePath();
  // Hashmap to store all settings as key-value pairs
  std::vector<std::map<std::string, std::string>> configVector_;
};

#endif  // CONFIGPARSER_HPP
