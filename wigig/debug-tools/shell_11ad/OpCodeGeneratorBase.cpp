/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <sstream>
#include <iomanip>

#include "OpCodeGeneratorBase.h"
#include "Utils.h"
#include "LogHandler.h"
#include "ParameterBase.h"

using namespace std;

string OpCodeGeneratorBase::FormattedOpcodeDescription(const string& rawOpCodeDescription, int alignmentWidth)
{
    std::stringstream ss(rawOpCodeDescription);
    std::string to;
    std::stringstream sf;
    bool space = false;
    sf << left; // text aligned to the left
    while (std::getline(ss, to, '\n')) {
        if (space)
        {
            sf << "\t" << setw(alignmentWidth) << " ";
        }
        else
        {
            space = true;
        }
        sf << to << endl;
    }
    return sf.str();
}


bool OpCodeGeneratorBase::AddParamsToJson(Json::Value &root, const CommandLineArgs & cla)
{
    for (const auto& x : m_parameterObjects)
    {
        if (!x->SetJsonValue(root, cla))
        {
            LOG_ERROR << "Could not construct command. "
                << "Group: " << cla.Group() << ", OpCode: " << cla.OpCode()
                << ", Parameter " << x->GetName() << " is missing or invalid.\n";
            return false;
        }
    }
    return true;
}

bool OpCodeGeneratorBase::ValidateProvidedParams(const CommandLineArgs& cla)
{
    vector<string> keys;
    cla.GetParamsMapKeys(keys);
    for (const auto& claParam : keys)
    {
        bool paramValid = false;
        for (const auto& x : m_parameterObjects)
        {
            if (claParam == Utils::ToLower(x->GetName()))
            {
                paramValid = true;
            }
        }
        if(!paramValid)
        {
            stringstream ssParams;
            bool coma = false;
            for (const auto& x : m_parameterObjects)
            {
                if (coma)
                {
                    ssParams << ", ";
                }
                else
                {
                    coma = true;
                }
                ssParams << x->GetName();
            }

            LOG_ERROR << "Wrong parameter provided: " << claParam << ". Expected parameters: "<< ssParams.str() << std::endl;
            return false;
        }
    }
    return true;

}

string OpCodeGeneratorBase::GenerateHelpMessage()
{
    stringstream ss;
    ss << left; // left make text aligned to the left
    ss << "\nCOMMAND DESCRIPTION\n\t" << FormattedOpcodeDescription(m_opCodeDescription, 10) << "\n";

    ss << "PARAMETERS\n";
    if (m_parameterObjects.empty())
    {
        ss << "\tThere are no additional parameters for this OpCode\n\n";
    }
    else
    {
        for (const auto& x: m_parameterObjects)
        {
            ss << x->HelpMessage() << "\n";
        }
    }

    ss << "EXAMPLES\n";
    int i = 1;
    for (const auto& x: m_examples)
    {
        ss << "\t" << i++ << ") " << x->HelpMessage() << "\n";
    }

    if (!m_notes.empty())
    {
        ss << "NOTE\n";
        for (const auto& x: m_notes)
        {
            ss << "\t" << x << "\n";
        }
    }

    return ss.str();
}

void OpCodeGeneratorBase::RegisterParam(unique_ptr<ParameterBase>&& upParamObject)
{
    m_parameterObjects.push_back(move(upParamObject));
}

void OpCodeGeneratorBase::RegisterDeviceParam()
{
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Device", false, "device name to perform the operation on; if device is not single, parameter is mandatory",
                              "One of device names returned by 'host get_state' command")));
}

void OpCodeGeneratorBase::RegisterExample(const std::string& exampleLine, const std::string& exampleDescription)
{
    m_examples.push_back(unique_ptr<ExampleHelper>(new ExampleHelper(exampleLine, exampleDescription)));
}

void OpCodeGeneratorBase::RegisterNote(const std::string& note)
{
    m_notes.push_back(note);
}

