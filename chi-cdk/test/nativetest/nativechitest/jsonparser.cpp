//=================================================================================================
// Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  jsonparser.cpp
// @brief Source file for nativetest json parser
//=================================================================================================

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <regex>
#include <stdexcept>
#include <sstream>
#include <csetjmp>
#include <csignal>
#include <cstdlib>
#include "jsonparser.h"

namespace chitests
{
    //Global Definitions
    jmp_buf enj;
    sig_atomic_t erjSignal            = 0;
    std::string jsonExt               = ".json";
    std::string folderPath            = "/vendor/etc/test/";
    bool bIsAbort                     = false; //Check if a test has aborted
    char cmd[MAX_PATH];             //cmd flag which iterates through j_cmdDef & verifies mutual exclusitivity

    // static members
    UINT32 JsonParser::sBufferType         = BufferHeapIon;
    UINT32 JsonParser::mNumPipelinesPerUc  = 0;
    bool   JsonParser::sbVideoFDTest       = false;
    bool   JsonParser::bIsJsonDefined      = false;
    std::string  JsonParser::mUseCaseName  = "";
    jsonparser::fifo_map<std::string, std::string>  JsonParser::mPipelineMap        = {};
    jsonparser::fifo_map<std::string, std::string>  JsonParser::mNodePropertiesMap  = {};
    jsonparser::fifo_map<std::string, std::string>  JsonParser::mPipelineLinksMap   = {};
    // Flags that are required for specific pipeline
    std::map<std::string, std::string>              JsonParser::mPipelineFlags      = {};


    struct StrCompare : public std::binary_function<const char*, const char*, bool> {
    public:
        bool operator() (const char* str1, const char* str2) const
        {
            return strncmp(str1, str2, strlen(str2)) < 0;
        }
    };

    static std::map<char const*, JsonParser::JsnParamDef, StrCompare> j_cmdDef =
    {
        // cmd     : actual cmd flag passed by user
        // HasData : if the cmd is accompanied by a value
        // type    : if the cmd is a bool/int/string type
        // data    : variable to store the value for the cmd if cmd has data
        // mutuallyExList: if cmd flag has an associated flag that is mutually exclusive
        // userSetFlag: if the corresponding flag has been set
        // | cmd              | HasData     | type                 | data               | mutuallyExList | userSetFlag |
        {"frames",          {true, JsonParser::TYPE_INT,    &CmdLineParser::m_nFrames,            {}, false}},
        {"fd",              {true, JsonParser::TYPE_INT,    &CmdLineParser::m_flushDelay,         {}, false}},
        {"testgen",         {true, JsonParser::TYPE_BOOL,   &CmdLineParser::m_isTestGenMode,      {}, false}},
        {"timeout",         {true, JsonParser::TYPE_INT,    &CmdLineParser::m_timeout,            {}, false}},
        {"no_image_dump",   {true, JsonParser::TYPE_BOOL,   &CmdLineParser::m_bNoImageDump,       {}, false}},
        {"camList",         {true, JsonParser::TYPE_STRING, &CmdLineParser::m_camList,            {}, false}},
        {"camera",          {true, JsonParser::TYPE_INT,    &CmdLineParser::m_nCamera,            {}, false}},
        {"sensormode",      {true, JsonParser::TYPE_INT,    &CmdLineParser::m_sensorMode,         {}, false}},
        {"zoom",            {true, JsonParser::TYPE_FLOAT,  &CmdLineParser::m_zoom,               {}, false}},
        {"normal",          {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"hdr",             {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"hfr",             {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"pdaf",            {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"depth",           {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"fs",              {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"internal",        {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"insensorzoom",    {true, JsonParser::TYPE_CAPS,   &CmdLineParser::m_sensorCapsList,     {}, false}},
        {"inputimage",      {true, JsonParser::TYPE_STRING, &CmdLineParser::m_inputImage,         {}, false}},
        {"inputwidth",      {true, JsonParser::TYPE_INT,    &CmdLineParser::m_inputWidth,         {}, false}},
        {"inputheight",     {true, JsonParser::TYPE_INT,    &CmdLineParser::m_inputHeight,        {}, false}},
        {"inputformat",     {true, JsonParser::TYPE_INT,    &CmdLineParser::m_inputFormat,        {}, false}},
        {"inputport",       {true, JsonParser::TYPE_INT,    &CmdLineParser::m_inputPort,          {}, false}},
        {"outputwidth",     {true, JsonParser::TYPE_INT,    &CmdLineParser::m_outputWidth,        {}, false}},
        {"outputheight",    {true, JsonParser::TYPE_INT,    &CmdLineParser::m_outputHeight,       {}, false}},
        {"rdioutputwidth",  {true, JsonParser::TYPE_INT,    &CmdLineParser::m_rdiOutputWidth,     {}, false}},
        {"rdioutputheight", {true, JsonParser::TYPE_INT,    &CmdLineParser::m_rdiOutputHeight,    {}, false}},
        {"outputformat",    {true, JsonParser::TYPE_INT,    &CmdLineParser::m_outputFormat,       {}, false}},
        {"outputport",      {true, JsonParser::TYPE_INT,    &CmdLineParser::m_outputPort,         {}, false}},
        {"hwfdtype",        {true, JsonParser::TYPE_INT,    &CmdLineParser::m_hwFdType,           {}, false}},
        {"fdtestimage",     {true, JsonParser::TYPE_STRING, &CmdLineParser::m_fdTestImage,        {}, false}},
        {"flushframe",      {true, JsonParser::TYPE_INT,    &CmdLineParser::m_flushFrame,         {}, false}},
        {"flushpipeline",   {true, JsonParser::TYPE_INT,    &CmdLineParser::m_flushPipelineId,    {}, false}},
        {"zsl",             {true, JsonParser::TYPE_INT,    &CmdLineParser::m_bZslMode,           {}, false}},
        {"snaps",           {true, JsonParser::TYPE_INT,    &CmdLineParser::m_nSnaps,             {}, false}},
        {"early_pcr",       {true, JsonParser::TYPE_INT,    &CmdLineParser::m_numEarlyPCRs,       {}, false}},
        {"lc",              {true, JsonParser::TYPE_BOOL,   &CmdLineParser::m_enableLatencyCheck, {}, false}},
        {"sf",              {true, JsonParser::TYPE_INT,    &CmdLineParser::m_snapshotFormat,     {}, false}},
        {"sw",              {true, JsonParser::TYPE_INT,    &CmdLineParser::m_snapshotWidth,      {}, false}},
        {"sh",              {true, JsonParser::TYPE_INT,    &CmdLineParser::m_snapshotHeight,     {}, false}},
        {"dfh",             {true, JsonParser::TYPE_INT,    &CmdLineParser::m_dfh,                {}, false}},
        {"dfw",             {true, JsonParser::TYPE_INT,    &CmdLineParser::m_dfw,                {}, false}},
        {"dff",             {true, JsonParser::TYPE_INT,    &CmdLineParser::m_dff,                {}, false}},
        {"vf",              {true, JsonParser::TYPE_INT,    &CmdLineParser::m_videoFormat,        {}, false}},
        {"vw",              {true, JsonParser::TYPE_INT,    &CmdLineParser::m_videoWidth,         {}, false}},
        {"vh",              {true, JsonParser::TYPE_INT,    &CmdLineParser::m_videoHeight,        {}, false}},
        {"rsn",             {true, JsonParser::TYPE_BOOL,   &CmdLineParser::m_bRemoveSensor,      {}, false}},
        // NCX ported/NCX only
        {"buffer_type",          {true,    JsonParser::TYPE_STRING, nullptr,                               {}, false}},
        {"videofd",              {true,    JsonParser::TYPE_BOOL,   &JsonParser::sbVideoFDTest,            {}, false}},
        //Node properties [id, val] map
        {"nodeInstance_properties_map",               {  true,   JsonParser::TYPE_MAP,    nullptr,   {}, false}},
        //Pipeline links map to buffer format
        {"pipeline_links_buffer_properties_map",      {  true,   JsonParser::TYPE_MAP,    nullptr,   {},  false}}
    };

    void jsonSignalHandler(int signum)
    {
        erjSignal = signum;
        signal(signum, SIG_DFL);
        longjmp(enj, 1);
    }

    int catchJsonErrorSignal(int(*func)())
    {
        int result = -1;
        if (setjmp(enj) == 0) //Initially env is set to 0 and continues with normal execution
        {
            signal(SIGABRT, &jsonSignalHandler); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, &jsonSignalHandler);
            signal(SIGFPE,  &jsonSignalHandler);
            signal(SIGILL,  &jsonSignalHandler);
            result = (*func)();
            signal(SIGABRT, SIG_DFL); //Registers custom SignalHandler when specified signals are called
            signal(SIGSEGV, SIG_DFL);
            signal(SIGFPE,  SIG_DFL);
            signal(SIGILL,  SIG_DFL);
        }
        else
        {
            switch (erjSignal)
            {
            case SIGILL:
            {
                NT_LOG_ERROR("Illegal Instruction Called !");
                NT_LOG_ERROR("%s Stopped", CmdLineParser::m_fullName.c_str());
                ShutDown();
                break;
            }
            case SIGABRT:
            {
                NT_LOG_ERROR("Abort Called !");
                NT_LOG_ERROR("%s Stopped", CmdLineParser::m_fullName.c_str());
                ShutDown();
                break;
            }
            case SIGSEGV:
            {
                NT_LOG_ERROR("Segmentation Violation Called  !");
                NT_LOG_ERROR("%s Stopped", CmdLineParser::m_fullName.c_str());
                ShutDown();
                break;
            }
            case SIGFPE:
            {
                NT_LOG_ERROR("Floating-Point arithmetic Exception !");
                NT_LOG_ERROR("%s Stopped", CmdLineParser::m_fullName.c_str());
                ShutDown();
                break;
            }
            }
            bIsAbort = true;
        }
        return result;
    }

    bool verifyMutuallyExclusive()
    {
        for (size_t i = 0; i < j_cmdDef.at(cmd).mutuallyExList.size(); i++)
        {
            if (j_cmdDef.at(j_cmdDef.at(cmd).mutuallyExList.at(i)).userSetFlag != FALSE)
            {
                NT_LOG_ERROR("%s is mutually exclusive to %s. Cannot be used together.", cmd, j_cmdDef.at(cmd).mutuallyExList.at(i));
                return true;
            }
            else
            {
                // Reset the defaults of the mutually exclusive command for the runtime parameter found
                // For example: Reset --duration to -1 if --frames is set by the user
                switch (j_cmdDef.at(j_cmdDef.at(cmd).mutuallyExList.at(i)).nDataType)
                {
                case JsonParser::TYPE_INT:
                    *reinterpret_cast<int*>(j_cmdDef.at(j_cmdDef.at(cmd).mutuallyExList.at(i)).pvData) = -1;
                    break;
                case JsonParser::TYPE_STRING:
                    strlcpy(reinterpret_cast<char*>(j_cmdDef.at(j_cmdDef.at(cmd).mutuallyExList.at(i)).pvData), "", MAX_PATH - 1);
                    break;
                case JsonParser::TYPE_BOOL:
                    *reinterpret_cast<bool*>(j_cmdDef.at(j_cmdDef.at(cmd).mutuallyExList.at(i)).pvData) = false;
                    break;
                default:
                    NT_LOG_ERROR("Command line has unsupported datatype [%d] for [%s]",
                        j_cmdDef.at(j_cmdDef.at(cmd).mutuallyExList.at(i)).nDataType, j_cmdDef.at(cmd).mutuallyExList.at(i));
                }
            }
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// jsonFileCheck
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool JsonParser::jsonFileCheck(std::string fileName)
    {
        std::string filePath = folderPath + fileName + jsonExt;
        std::ifstream fileObject(filePath);
        if (fileObject.is_open())
        {
            fileObject.close();
            return true;
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// jsonParse
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool JsonParser::jsonParse(json& jsonObj)
    {
        for (auto element = jsonObj.begin(); element != jsonObj.end(); ++element)
        {
            // TODO: handle multiple usecases & plan for usecaseCommonFlags
            if(strcmp(element.key().c_str(), "") == 0)
            {
                NT_LOG_ERROR("No usecase/test entered!");
                return false;
            }

            JsonParser::mUseCaseName = element.key();
            json pipeObj = json::parse(element.value().dump());
            JsonParser::mNumPipelinesPerUc = pipeObj.size() - pipeObj.count("flags");

            //Value (Pipeline Name, flag parameters) are parsed
            for (auto& pipeline : pipeObj.items())
            {
                if(strcmp(pipeline.key().c_str(), "") == 0)
                {
                    NT_LOG_ERROR("No value entered for pipeline name");
                    return false;
                }
                else if (strcmp(pipeline.key().c_str(), "flags") == 0)
                {
                    try //try - catch for common pipeline flags
                    {
                        ParseJsonObj(json::parse(pipeline.value().dump()));
                    }
                    catch (json::exception& e)
                    {
                        NT_LOG_ERROR("Invalid flag input in %s flags", element.key().c_str());
                        NT_LOG_ERROR("%s\n", e.what());
                        return false;
                    }
                }
                else
                {
                    //Insert name of pipeline as key and its flags as value
                    mPipelineMap.insert({ pipeline.key(),  pipeline.value().dump() });
                }
            }
        }
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// executeJson
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void JsonParser::executeJson(std::string fileName)
    {
        json masterObj;
        bool bErrorInput           = false;
        JsonParser::bIsJsonDefined = true;

        //Create json file path
        std::string filePath = folderPath + fileName + jsonExt;
        std::ifstream fileObject(filePath);

        //check if json file opened
        if (fileObject.is_open())
        {
            //parse json file to JSON object type
            try
            {
                fileObject >> masterObj;
            }
            catch (jsonparser::detail::other_error& o)
            {
                NT_LOG_ERROR("Duplicate key found in Json, Stopped parsing !");
                fileObject.close();
                return;
            }
            catch (json::exception& e)
            {
                NT_LOG_ERROR("Invalid Json input -- %s", e.what());
                fileObject.close();
                return;
            }

            fileObject.close();
            if (!jsonParse(masterObj)) {
                return;
            }
        }

        std::string delimiter      = ".";
        std::string testSuiteValue = fileName.substr(0, fileName.find(delimiter));
        CmdLineParser::m_suiteName = testSuiteValue;
        CmdLineParser::m_caseName  = fileName.erase(0, testSuiteValue.size() + 1);
        CmdLineParser::m_fullName  = CmdLineParser::m_suiteName + "." + CmdLineParser::m_caseName;

        try //try - catch for parsing json common & pipeline map
        {
            ParsePipelineAndFlags();
        }
        catch (json::exception& err)
        {
            NT_LOG_ERROR("Invalid flag input in %s ", CmdLineParser::m_fullName.c_str());
            NT_LOG_ERROR("%s\n", err.what());
            bErrorInput = true;
        }

        int returnVal = catchJsonErrorSignal(&RunTests);
        if (bIsAbort)
        {
            bIsAbort  = false;
            ShutDown();
        }

        CmdLineParser::resetDefaultFlags();
        //Clear userSetFlag in JsnParamDef
        for (auto it = j_cmdDef.begin(); it != j_cmdDef.end(); it++)
        {
            j_cmdDef.at(it->first).userSetFlag = false;
        }
        if (bErrorInput) // Prints Json flag usage
        {
            PrintCommandLineUsage();
        }

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ApplyJsnParamDef
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void JsonParser::ApplyJsnParamDef(JsnParamDef jsonParam, json jsonObj, std::string jsonKey)
    {
        const char* jsonKeyChar = jsonKey.c_str();
        auto jsonFileValue = jsonObj[jsonKey];
        switch (jsonParam.nDataType)
        {
            case TYPE_INT:
                *reinterpret_cast<int*>(jsonParam.pvData) = jsonFileValue;
                break;
            case TYPE_FLOAT:
                *reinterpret_cast<float*>(jsonParam.pvData) = jsonFileValue;
                break;
            case TYPE_CAPS:
            {
                std::vector<CmdLineParser::SensorCaps> &sensorCaps = *static_cast<std::vector<CmdLineParser::SensorCaps>*>(jsonParam.pvData);
                if (strcmp("normal", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::NORMAL);
                }
                else if (strcmp("hdr", jsonKeyChar) == 0)
                {
                CmdLineParser::SensorCaps userCap = CmdLineParser::GetSensorCapEnum(jsonFileValue);
                    if(userCap != CmdLineParser::MaxSensorCaps)
                    {
                        sensorCaps.push_back(userCap);
                    }
                    else
                    {
                        CmdLineParser::m_bHelp = true;
                    }
                }
                else if (strcmp("hfr", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::HFR);
                }
                else if (strcmp("pdaf", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::PDAF);
                }
                else if (strcmp("qcfa", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::QCFA);
                }
                else if (strcmp("depth", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::DEPTH);
                }
                else if (strcmp("fs", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::FS);
                }
                else if (strcmp("insensorzoom", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::INSENSORZOOM);
                }
                else if (strcmp("internal", jsonKeyChar) == 0)
                {
                    sensorCaps.push_back(CmdLineParser::INTERNAL);
                }
                break;
            }
            case TYPE_STRING:
                if (strcmp("buffer_type", jsonKeyChar) == 0)
                {
                    if(strcmp("EGL", std::string(jsonFileValue).c_str()) == 0)
                    {
                        JsonParser::sBufferType = BufferHeapEGL;
                    }
                    else if(strcmp("DSP", std::string(jsonFileValue).c_str()) == 0)
                    {
                        JsonParser::sBufferType = BufferHeapDSP;
                    }
                    else if(strcmp("SYS", std::string(jsonFileValue).c_str()) == 0)
                    {
                        JsonParser::sBufferType = BufferHeapSystem;
                    }
                    else if(strcmp("PSL", std::string(jsonFileValue).c_str()) == 0)
                    {
                        JsonParser::sBufferType = 0xFFFFFFFF;
                    }
                    else
                    {
                        JsonParser::sBufferType = BufferHeapIon;
                    }
                    break;
                }
                strlcpy(reinterpret_cast<char*>(jsonParam.pvData), std::string(jsonFileValue).c_str(), MAX_PATH-1);
                break;
            case TYPE_BOOL:
                *reinterpret_cast<bool*>(jsonParam.pvData) = jsonFileValue;
                break;
            case TYPE_MAP:
            {
                if (strcmp("pipeline_links_buffer_properties_map", jsonKeyChar) == 0)
                {
                    for (auto element : jsonFileValue.items())
                    {
                        JsonParser::mPipelineLinksMap.insert({ element.key(), element.value().dump() });
                    }
                }
                else  if (strcmp("nodeInstance_properties_map", jsonKeyChar) == 0)
                {
                    for (auto element : jsonFileValue.items())
                    {
                        JsonParser::mNodePropertiesMap.insert({ element.key(), element.value().dump() });
                    }
                }
                break;
            }
            default:
                NT_LOG_ERROR("Json has unsupported datatype [%d] for [%s]", jsonParam.nDataType, jsonParam.pvData);
                break;
        }
        jsonParam.userSetFlag = true;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseJsonObj
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void JsonParser::ParseJsonObj(json jsonObj)
    {
        std::string assign = "";
        strlcpy(cmd, "", MAX_PATH-1); // reset flag iterator to check mutual exclusivity
        std::map<char const*, JsonParser::JsnParamDef>::iterator it;
        for (auto it : jsonObj.items())
        {
            std::string jsonKey = it.key();
            const char* jsonKeyChar = jsonKey.c_str(); // runtime flags & pipelineFlags
            auto jsonFileValue = jsonObj[jsonKey];
            if (j_cmdDef.find(jsonKeyChar) != j_cmdDef.end())
            {
                ApplyJsnParamDef(j_cmdDef.at(jsonKeyChar), jsonObj, jsonKey);
                strlcpy(cmd, jsonKeyChar, MAX_PATH - 1);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParsePipelineAndFlags
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void JsonParser::ParsePipelineAndFlags()
    {
        for (auto pipelineItr = mPipelineMap.begin(); pipelineItr != mPipelineMap.end(); pipelineItr++)
        {
            // Loop through flag params (defined as cmd) inside j_cmdDef
            json jsonPipelineObj = json::parse(pipelineItr->second);
            for (auto it : jsonPipelineObj.items())
            {
                std::string jsonKey = it.key();
                const char* jsonKeyChar = jsonKey.c_str(); //runtime flags & pipelineFlags
                if(strcmp(jsonKey.c_str(), "flags") == 0)
                {
                    //Insert name of pipeline as key and flags defined as value
                    JsonParser::mPipelineFlags.insert({ pipelineItr->first, it.value().dump() });
                }
                else
                {
                    auto jsonFileValue = jsonPipelineObj[jsonKey];
                    if (j_cmdDef.find(jsonKeyChar) != j_cmdDef.end())
                    {
                        ApplyJsnParamDef(j_cmdDef.at(jsonKeyChar), jsonPipelineObj, jsonKey);
                        strlcpy(cmd, jsonKeyChar, MAX_PATH - 1);
                        if (verifyMutuallyExclusive())
                        {
                            NT_LOG_INFO("%s did not run due to mutually exclusive flags\n\n", CmdLineParser::m_fullName.c_str());
                            return;
                        }
                    }
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PrintCommandLineUsage
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void JsonParser::PrintCommandLineUsage()
    {
        printf("********************************** JSON FLAG USAGE *********************************\n");
        printf(" frames                [INT]    : override default number of frames to be dumped\n");
        printf(" camera                [INT]    : camera to run test on\n");
        printf(" timeout               [INT]    : timeout in seconds while waiting for buffers/results (0 disables timeout)\n");
        printf(" testgen               [BOOL]   : run any sensor related test in testgen mode\n");
        printf(" noimagedump (nid)     [BOOL]   : disable result image dumping to device\n");
        printf(" sm      (no alias)    [INT]    : override the sensor mode for given camera\n");
        printf(" zm      (no alias)    [FLOAT]  : enable zoom factor for entire usecase \n");
        printf(" camList (no alias)    [STRING] : list of camera IDs to run \n");
        printf(" inputimage  (ii)      [STRING] : set the input image for offline recipe tests\n");
        printf("\n");
        printf(" inputwidth  (iw)      [INT]    : set the input width for recipe tests\n");
        printf(" inputheight (ih)      [INT]    : set the input height for recipe tests\n");
        printf(" inputformat (if)      [INT]    : set the input stream format for recipe tests\n");
        printf(" inputport   (ip)      [INT]    : set the input port on IFE/IPE/BPS for recipe tests\n");
        printf("\n");
        printf(" sf      (no alias)    [INT]    : set the output format for snapshot streams\n");
        printf(" sw      (no alias)    [INT]    : set the output width  for snapshot streams\n");
        printf(" sh      (no alias)    [INT]    : set the output height for snapshot streams\n");
        printf("\n");
        printf(" vf      (no alias)    [INT]    : set the output format for video streams\n");
        printf(" vw      (no alias)    [INT]    : set the output width  for video streams\n");
        printf(" vh      (no alias)    [INT]    : set the output height for video streams\n");
        printf("\n");
        printf(" outputwidth    (ow)   [INT]    : set the output width for recipe tests\n");
        printf(" outputheight   (oh)   [INT]    : set the output height for recipe tests\n");
        printf(" rdioutputwidth (row)  [INT]    : Set the RDI output width for recipe tests and few mixed pipeline tests \n");
        printf(" rdioutputheight(roh)  [INT]    : Set the RDI output height for recipe tests and few mixed pipeline tests\n");
        printf(" outputformat   (of)   [INT]    : set the output stream format for recipe tests\n");
        printf(" outputport     (op)   [INT]    : set the output port on IFE/IPE/BPS for recipe tests\n");
        printf("\n");
        printf(" df         (no alias) [BOOL]   : disables flush operation for given flush test\n");
        printf(" fd         (no alias) [INT]    : delay before triggering flush (microseconds)\n");
        printf(" flushframe     (ff)   [INT]    : frame number to trigger flush at\n");
        printf(" flushpipeline  (fpid) [INT]    : pipeline ID to trigger flush on \n");
        printf(" hwfdtype       (hwfd) [INT]    : set the HW FD Type for FD HW tests\n");
        printf(" fdtestimage    (fdti) [STRING] : set the FD input scene name for FD tests\n");
        printf(" epcr       (no alias) [INT]    : number of early PCRs before activating pipeline\n");
        printf(" dfw        (no alias) [INT]    : override for dual fov / display stream width\n");
        printf(" dfh        (no alias) [INT]    : override for dual fov / display stream height\n");
        printf(" dff        (no alias) [INT]    : override for dual fov / display stream format\n");
        printf(" lc         (no alias) [BOOL]   : enables latency check for given test\n");
        printf(" sn         (no alias) [INT]    : number of snaps to be taken in a test \n");
        printf(" zsl        (no alias) [BOOL]   : enable ZSL mode for preview + snapshot usecase \n");
        printf("\n");
        printf(" pdaf         (no alias)        : Pick PDAF  sensor capability \n ");
        printf(" qcfa         (no alias)        : Pick QCFA  sensor capability \n ");
        printf(" fs           (no alias)        : Pick FS    sensor capability \n ");
        printf(" insensorzoom (no alias)        : Pick InSensorZoom sensor capability \n ");
        printf(" hdr [STRING = ihdr, zzhdr, qhdr, shdr] : Pick HDR sensor capability from given values\n ");
        printf("************************************************************************\n");
    }

}
