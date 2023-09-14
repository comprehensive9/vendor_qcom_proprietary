/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "json/json.h"
#include "ParameterBase.h"
#include "LogHandler.h"

class CommandLineArgs;

class ExampleHelper
{
public:
    ExampleHelper(const std::string& commandLine, const std::string& description) : m_commandLine(commandLine), m_description(description) {};
    std::string HelpMessage() const
    {
        std::ostringstream ss;
        ss << m_description << ":\n\t\t" << g_ApplicationName << " " << m_commandLine << std::endl;
        return ss.str();
    }

private:
    const std::string m_commandLine;
    const std::string m_description;
};


class OpCodeGeneratorBase
{
public:

    static std::string FormattedOpcodeDescription(const std::string& rawOpCodeDescription, int alignmentWidth);
    bool AddParamsToJson(Json::Value &root, const CommandLineArgs &cla);
    bool ValidateProvidedParams(const CommandLineArgs &cla);
    std::string GenerateHelpMessage();
    const char* OpCodeDescription() const { return m_opCodeDescription.c_str(); };

protected:
    explicit OpCodeGeneratorBase(const char* pOpCodeDescription): m_opCodeDescription(pOpCodeDescription) {};
    void RegisterParam(std::unique_ptr<ParameterBase>&& upParamObject);
    void RegisterDeviceParam();
    void RegisterExample(const std::string& exampleLine, const std::string& exampleDescription);
    void RegisterNote(const std::string& note);

private:
    const std::string m_opCodeDescription;
    std::vector<std::unique_ptr<ParameterBase> > m_parameterObjects;
    std::vector<std::unique_ptr<ExampleHelper>> m_examples;
    std::vector<std::string> m_notes;
};