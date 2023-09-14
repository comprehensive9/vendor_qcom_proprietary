//=================================================================================================
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  haljsonparser.h
// @brief Header file for NATIVEHAL Json Parser
//=================================================================================================

#ifndef __NATIVETEST_HALJSONPARSER__
#define __NATIVETEST_HALJSONPARSER__

#include <stdint.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "nativetestlog.h"
#include "json.hpp"
#include <hardware/camera3.h>

#define PATTERN_REQUEST_MOD(frame, pattern_size) (((frame) - 1) % pattern_size)
namespace haltests
{

    static const int      UNSET_INT         = -1;
    static const uint32_t EXPOSURES_RANGE[] = {1, 2, 3};

    // A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
    template<class K, class V, class dummy_compare, class A>
    // Use custom fifo_map_compare to maintain order of tests in Json when parsing
    using my_fifo_map = jsonparser::fifo_map<K, V, jsonparser::fifo_map_compare<K>, A>;
    using json = jsonparser::basic_json<my_fifo_map>;

    //=================================================================================================
    // Class Definition
    //=================================================================================================
    class HalJsonParser
    {
        public:
            enum DataTypes
            {
                TYPE_INT,
                TYPE_STRING,
                TYPE_BOOL,
                TYPE_INT_LIST,
                TYPE_STRING_LIST,
                TYPE_INLINE_PARSE,
                TYPE_DIRECTIONS
            };

            struct JsnParamDef
            {
                bool        bHasData;
                DataTypes   nDataType;
                void*       pvData;
                std::vector<char const*> mutuallyExList;
                bool        userSetFlag;
            };

            // HDR related
            static bool                  sAutoHDR;
            static bool                  sMFHDR;
            static bool                  sQHDR;
            static bool                  sRandExpPattern;
            static bool                  sSHDR;
            static int                   sNumExposures;
            static std::vector<uint32_t> sExpPattern;

            // MFNR related
            static bool sMFNR;

            // MCX related
            static bool sMCBOnly;

            // streams related
            static int                                  sNumStreams;
            static std::vector<int>                     sRequestTypes;
            static std::vector<int>                     sUsageFlags;
            static std::vector<int>                     sDataspaces;
            static std::vector<camera3_stream_type_t>   sDirections;
            static std::vector<int>                     sFormats;
            static std::vector<std::string>             sResolutions;
            static std::vector<int>                     sStreamIntervals;

            // snaps/frames related
            static int sDuration;
            static int sFrames;
            static int sSnaps;

            //session related
            static int sSessionMode;

            // ZSL related
            static bool sZSL;

            HalJsonParser()  = default;
            ~HalJsonParser() = default;

            /// Do not allow the copy constructor or assignment operator
            HalJsonParser(const HalJsonParser&) = delete;
            HalJsonParser& operator= (const HalJsonParser&) = delete;

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ///  JsonRecurse
            ///  @brief
            ///     Recurses the json by obtaining key & value at each level
            ///  @return
            ///     None
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void jsonRecurse(json& jsonObj, int *level);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ///  PrintCommandLineUsage()
            ///  @brief
            ///     Print the Json usage instructions to stdout
            ///     Usage instructions are printed when a json error is detected
            ///  @return
            ///     None
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void PrintCommandLineUsage();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ///  JsonFileCheck
            ///  @brief
            ///     Checks if json file from the command line is present
            ///  @return
            ///     1 on success, 0 on failure
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static bool jsonFileCheck(std::string);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ///   jsonParsing
            ///  @brief
            ///     Parse the input json file from the command line using jsonparser::JSON
            ///  @return
            ///     0 on success, 1 on failure
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static int jsonParsing(std::string);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ///   executeTest
            ///  @brief
            ///      Parses through the Json parameters to obtain flags
            ///  @return
            ///      None
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void executeTest(json, int);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ///   testCheck
            ///  @brief
            ///     Checks if the TestSuiteName.TestCaseName is registered
            ///  @return
            ///     1 - complete match, 0 - partial match & -1 - no match
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static int  testCheck(std::string, std::string);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// ResetDefaultFlags
            ///
            /// @brief resets Default flags
            ///
            /// @return None
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void ResetDefaultFlags();
    };

}
#endif //__NATIVETEST_HALJSONPARSER__
