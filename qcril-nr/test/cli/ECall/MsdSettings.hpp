/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

/**
 * @brief MsdSettings class reads config file and caches the config msdSettings
 * It provides utility functions to read the config values
 */

#ifndef MSDSETTINGS_HPP
#define MSDSETTINGS_HPP

#include <map>
#include <string>

//#include <telux/tel/ECallDefines.hpp>

/*
 * MsdSettings class caches the config msdSettings from tel.conf file
 * It provides utility methods to get value of a config setting by passing the key
 */
class MsdSettings {
 public:
  // Get the user defined value for any configuration setting
  static std::string getValue(std::string key);

  // Function to read msdSettings config file containing key value pairs
  static void readMsdSettingsFile();

  // Print all the key value pairs in the cache
  static void printMsdSettings();

  // telux::tel::ECallMsdData readMsdFromFile(std::string filename);

 private:
  // Hashmap to store all msdSettings as key-value pairs
  static std::map<std::string, std::string> msdSettingsMap_;
  static std::string filename_;
};  // end of class MsdSettings

#endif  // MSDSETTINGS_HPP
