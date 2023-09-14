/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved
 */

/**
 * Utility helper class
 * @brief Utils class performs common error code conversions
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <CommonDefines.hpp>

class Utils {
 public:
  // Validate the input and in case of invalid input request
  // for proper input from user.
  template <typename T>
  static void validateInput(T& input) {
    bool valid = false;
    do {
      if (std::cin.good()) {
        valid = true;
      } else {
        // If an error occurs then an error flag is set and future attempts to get
        // input will fail. Cear the error flag on cin.
        std::cin.clear();
        // Extracts characters from the previous input sequence and discards them,
        // until entire stream have been extracted, or one compares equal to newline.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "ERROR: Invalid input, please re-enter." << std::endl;
        std::cin >> input;
      }
    } while (!valid);
  }

  // Validate input string(Ex: 1, 2, 3) which should contain
  // atleast one number or numbers seperated by either comma, space or both.
  static void validateNumericString(std::string& input);
  /**
   * Get error description for given ErrorCode
   */
  static std::string getErrorCodeAsString(telux::common::ErrorCode error);
};

#endif
