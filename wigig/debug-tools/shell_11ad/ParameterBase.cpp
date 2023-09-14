/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "ParameterBase.h"
#include "Utils.h"
#include "LogHandler.h"

#include <sstream>
#include <locale>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

ParameterBase::ParameterBase(const string & name, bool isMandatory, const string &  description, const string& supportedValues, const string & defaultValue) :
    m_name(name), m_mandatory(isMandatory), m_description(description), m_supportedValues(supportedValues), m_defaultValue(defaultValue) {};

// Prints specific help message for every parameter object.
string ParameterBase::HelpMessage() const
{
    stringstream ss;
    const auto alignmentWidth = 22;
    ss << std::left;
    ss << "\t" << m_name << endl;
    ss << std::setw(alignmentWidth) << "\t\tDescription: " << m_description << endl;
    ss << std::setw(alignmentWidth) << "\t\tSupported values: " << m_supportedValues << endl;
    if (!m_defaultValue.empty())
    {
        ss << std::setw(alignmentWidth) << "\t\tDefault value: " << m_defaultValue << endl;
    }
    ss << std::setw(alignmentWidth) << "\t\tMandatory: " << (m_mandatory ? "yes" : "no") << endl;

    return ss.str();
}

SetJsonValueResult ParameterBase::SetJsonValueBase(Json::Value & root, const CommandLineArgs & cla, string &outVal) const
{
    if (root.isMember(m_name))
    {
        LOG_ERROR << "ERROR: the key: " << m_name << " already exists, please check the command line." << endl;
        return FAIL;
    }

    // Check if key was  in the cli arguments then if the parameter is mandatory return false. otherwise return true;
    bool tmpFindRes = cla.FindParam(Utils::ToLower(m_name), outVal);
    if (!tmpFindRes)
    {
        if(m_mandatory)
        {
            LOG_ERROR << "ERROR: the key: " << m_name << " is mandatory but not provided." << endl;
            return FAIL;
        }
        else
        {
            return SUCCESS_NOT_SET;
        }
    }
    return SUCCESS_SET;
}


StringParameter::StringParameter(const std::string & name, bool isMandatory, const std::string & description, const std::string & supportedValues,
                                            const std::string & defaultValue, bool canBeEmpty):
    ParameterBase(name, isMandatory, description, supportedValues, defaultValue)
{
    m_canBeEmpty = canBeEmpty;
};

bool StringParameter::SetJsonValue(Json::Value & root, const CommandLineArgs & cla) const
{
    string outVal;
    SetJsonValueResult res = SetJsonValueBase(root, cla, outVal);
    if (res == SUCCESS_SET)
    {
        if (outVal.empty() && !m_canBeEmpty)
        {
            return false;
        }
        root[m_name] = Json::Value(outVal);
    }
    return res != FAIL;
}

BoolParameter::BoolParameter(const std::string & name, bool isMandatory, const std::string & description, const std::string & defaultValue):
    ParameterBase(name, isMandatory, description, "true / false", defaultValue) {};

bool BoolParameter::SetJsonValue(Json::Value & root, const CommandLineArgs & cla) const
{
    string outVal;
    SetJsonValueResult res = SetJsonValueBase(root, cla, outVal);
    transform(outVal.begin(), outVal.end(), outVal.begin(), [](char c) { return static_cast<char>(::tolower(c)); });
    if (res == SUCCESS_SET)
    {
        bool tmpVal = (outVal == "true");
        if(!tmpVal && outVal != "false")
        {
            LOG_ERROR << "For key: " << m_name << ". The provided value:" << outVal << " cannot be parsed to a boolean value." << endl;
            return false;
        }
        root[m_name] = Json::Value(tmpVal);
    }
    return res != FAIL;
}



IntParameter::IntParameter(const std::string & name, bool isMandatory, const std::string & description, const std::string & defaultValue):
    ParameterBase(name, isMandatory, description, "any number: DEC (no prefix) or HEX (prefix 0x / 0X)", defaultValue) {};

bool IntParameter::SetJsonValue(Json::Value & root, const CommandLineArgs & cla) const
{
    string outVal;
    SetJsonValueResult res = SetJsonValueBase(root, cla, outVal);
    if (res == SUCCESS_SET)
    {
        // can take decimal (no prefix), octal (prefix 0), Hexadecimal (prefix 0x / 0X)
        char *endptr;
        long int tmpVal = strtol(outVal.c_str(), &endptr, 0);
        if (endptr == outVal.c_str())
        {
            LOG_ERROR << "For key: " << m_name << ". The provided value is empty." << endl;
            return false;
        }
        if (*endptr != '\0')
        {
            LOG_ERROR << "For key: " << m_name << ". The provided value:" << outVal << " can not be parsed to a numeric value." << endl;
            return false;
        }
        root[m_name] = Json::Value(static_cast<int64_t>(tmpVal));
    }
    return res != FAIL;
}
