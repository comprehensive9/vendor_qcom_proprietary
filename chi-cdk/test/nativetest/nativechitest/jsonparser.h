//=================================================================================================
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  jsonparser.h
// @brief Header file for NATIVECHI3 json parser
//=================================================================================================

#ifndef __NATIVETEST_JSONPARSER__
#define __NATIVETEST_JSONPARSER__

#include <stdint.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "nativetestlog.h"
#include "json.hpp"

namespace chitests
{
    // A workaround to give to use fifo_map as map of choice
    template<class K, class V, class dummy_compare, class A>
    // Use custom fifo_map_compare to maintain order of tests in Json when parsing
    using my_fifo_map = jsonparser::fifo_map<K, V, jsonparser::fifo_map_compare<K>, A>;
    using json = jsonparser::basic_json<my_fifo_map>;


    //=================================================================================================
    // Class Definition
    //=================================================================================================
    class JsonParser
    {
        public:

            JsonParser() = default;
            ~JsonParser() = default;

            enum DataTypes
            {
                TYPE_INT,
                TYPE_CAPS,
                TYPE_BOOL,
                TYPE_MAP,
                TYPE_FLOAT,
                TYPE_STRING
            };

            struct JsnParamDef
            {
                bool        bHasData;
                DataTypes   nDataType;
                void*       pvData;
                std::vector<char const*> mutuallyExList;
                bool        userSetFlag;
            };

            // TODO: make these variables private post NCX migration
            // buffer related
            static UINT32         sBufferType;
            static bool           sbVideoFDTest;
            //json related
            static bool           bIsJsonDefined;

            /// Do not allow the copy constructor or assignment operator
            JsonParser(const JsonParser&) = delete;
            JsonParser& operator= (const JsonParser&) = delete;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// jsonFileCheck
            ///
            /// @brief checks if Json filename is present & updates bIsJsonDefined
            /// @param string - fileName
            /// @return bool
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static bool jsonFileCheck(std::string);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// PrintCommandLineUsage
            ///
            /// @brief function that prints json flag compatbility
            ///
            /// @return None
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void PrintCommandLineUsage();

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// ParsePipelineAndFlags
            ///
            /// @brief function appply the commonJson & pipelineJsonObj
            ///
            /// @return None
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void ParsePipelineAndFlags();

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// ApplyJsnParamDef
            ///
            /// @brief helper function to apply parsed json values
            /// @param  JsnParamDef, json, string
            /// @return None
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void ApplyJsnParamDef(JsnParamDef jsonParam, json jsonObj, std::string jsonKey);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// ParseJsonObj
            ///
            /// @brief parses through the Json parameters to obtain flags
            /// @param  json
            /// @return None
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void ParseJsonObj(json);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// executeJson
            ///
            /// @brief Parse & execute NCX from the input json
            /// @param string - fileName
            /// @return None
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static void executeJson(std::string);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// jsonParse
            ///
            /// @brief parses the json by obtaining key & value for each pipeline
            /// @param jsonObj
            /// @return bool
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static bool jsonParse(json& jsonObj);

            static UINT32 GetBufferType()
            {
                return sBufferType;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// IsVideoFDTest
            ///
            /// @brief  Check if user enabled video FD database testing
            /// @param  None
            /// @return bool
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static bool IsVideoFDTest()
            {
                return sbVideoFDTest;
            }

            static std::string GetUseCaseName()
            {
                return mUseCaseName;
            }

            static jsonparser::fifo_map<std::string, std::string> GetPipelineLinksMap()
            {
                return mPipelineLinksMap;
            }

            static jsonparser::fifo_map<std::string, std::string> GetNodePropertiesMap()
            {
                return mNodePropertiesMap;
            }

            static UINT32 GetNumPipelines()
            {
                return mNumPipelinesPerUc;
            }

            static std::map<std::string, std::string> GetPipelineFlags()
            {
                return mPipelineFlags;
            }

        private:

            //TODO: to make non-static since used only in this class
            static UINT32         mNumPipelinesPerUc;
            static std::string    mUseCaseName;

            // pipeline related
            static std::map<std::string, std::string>              mPipelineFlags;
            static jsonparser::fifo_map<std::string, std::string>  mPipelineMap;
            static jsonparser::fifo_map<std::string, std::string>  mPipelineLinksMap;
            static jsonparser::fifo_map<std::string, std::string>  mNodePropertiesMap;

    };
}
#endif //__NATIVETEST_CMDPARSER__
