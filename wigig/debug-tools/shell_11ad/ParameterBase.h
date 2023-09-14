/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#pragma once

#include "json/json.h"
#include "CommandLineArgs.h"

#include <string>
#include <memory>

enum SetJsonValueResult {SUCCESS_SET, SUCCESS_NOT_SET, FAIL};

/*
This class defines the code base for the specific type objects:
    string
    int
    bool
*/
class ParameterBase
{
public:
    virtual ~ParameterBase() = default;

    std::string HelpMessage() const;
    virtual bool SetJsonValue(Json::Value &root, const CommandLineArgs& cla) const = 0;
    const std::string& GetName() const { return m_name; };
    const std::string& GetDescription()const { return m_description; };
    const std::string& GetSupportedValues()const { return m_supportedValues; };
    const std::string& GetDefaultValue() const { return m_defaultValue; };
    bool IsMandatory() const { return m_mandatory; };

    ParameterBase(const ParameterBase & po) = default;
    ParameterBase(ParameterBase && po) = default;
    ParameterBase& operator=(const ParameterBase&) = default;

protected:
    ParameterBase(const std::string & name, bool isMandatory, const std::string & description, const std::string & supportedValues = "", const std::string & defaultValue = "");
    std::string m_name;
    bool m_mandatory;
    std::string m_description; // description of the param might be empty
    std::string m_supportedValues;
    std::string m_defaultValue;
    SetJsonValueResult SetJsonValueBase(Json::Value &root, const CommandLineArgs& cla, std::string &outVal) const;

};

inline std::ostream &operator<<(std::ostream &output, const std::unique_ptr<ParameterBase> &po)
{
    output << po->GetName();
    return output;
}

class IntParameter :
    public ParameterBase
{
public:
    IntParameter(const std::string & name, bool isMandatory, const std::string & description,
                const std::string & defaultValue = "");
    bool SetJsonValue(Json::Value &root, const CommandLineArgs& cla) const override;
};

class StringParameter :
    public ParameterBase
{
public:
    StringParameter(const std::string & name, bool isMandatory, const std::string & description,
                    const std::string & supportedValues = "any string",const std::string & defaultValue = "", bool canBeEmpty = false);
    bool SetJsonValue(Json::Value &root, const CommandLineArgs& cla) const override;
private:
    bool m_canBeEmpty;
};


class BoolParameter :
    public ParameterBase
{
public:
    BoolParameter(const std::string & name, bool isMandatory, const std::string & description,
                    const std::string & defaultValue = "");
    bool SetJsonValue(Json::Value &root, const CommandLineArgs& cla) const override;
};
