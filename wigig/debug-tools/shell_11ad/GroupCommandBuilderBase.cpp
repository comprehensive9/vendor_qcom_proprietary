/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <sstream>
#include <iostream>
#include "json/json.h"
#include "GroupCommandBuilderBase.h"
#include "SharedVersionInfo.h"
#include "ParameterBase.h"
#include "LogHandler.h"

using namespace std;

bool GroupCommandBuilderBase::GetCommandOrHelpMessage(const CommandLineArgs & commandLineArgs, string & messageToSend)
{
    auto fit = m_opcodeGenerators.find(commandLineArgs.OpCode());
    if (!fit->second->ValidateProvidedParams(commandLineArgs))
    {
        return false;
    }

    Json::Value messageRoot;
    SetCommonParams(commandLineArgs, messageRoot);
    if (fit->second->AddParamsToJson(messageRoot, commandLineArgs))
    {
        Json::StyledWriter writer;
        messageToSend = writer.write(messageRoot);
        return true;
    }

    // in this case the message is not sent just help text displayed
    string opcodeHelpMessage = fit->second->GenerateHelpMessage();
    messageToSend = HelpTemplate(opcodeHelpMessage, commandLineArgs.Group(), commandLineArgs.OpCode());
    return false;
}


bool GroupCommandBuilderBase::HandleCommand(const CommandLineArgs & commandLineArgs, string & messageToSendOrHelp)
{
    if ((commandLineArgs.IsGroupHelpRequired()))
    {
        messageToSendOrHelp = GenerateGroupHelpMessage(commandLineArgs);
        return false;
    }
    if (commandLineArgs.OpCode().empty())
    {
        LOG_ERROR << "Missing OpCode. Use \' " << g_ApplicationName << " " << commandLineArgs.Group() << " -h \' for more information." << endl;
        return false;
    }

    auto fit = m_opcodeGenerators.find(commandLineArgs.OpCode());
    if (fit == m_opcodeGenerators.end())
    {
        // OpCode is not in m_opcodeGenerators
        LOG_ERROR << "Invalid command opcode: " << commandLineArgs.OpCode() << endl;
        return false;
    }
    if (commandLineArgs.IsParamHelpRequired())
    {
        // in this case the message is not sent just help text displayed
        messageToSendOrHelp = fit->second->GenerateHelpMessage();
        return false;
    }

    // Returns true and a message to send (JSON)
    // Otherwise return false and messageToSendOrHelp contains help
    return GetCommandOrHelpMessage(commandLineArgs, messageToSendOrHelp);
}


string GroupCommandBuilderBase::GenerateGroupHelpMessage(const CommandLineArgs & commandLineArgs) const
{
    return HelpTemplate(GetAvailableOpCodesHelp(), commandLineArgs.Group());
}


string GroupCommandBuilderBase::HelpTemplate(string helpText, string group, string opcode, bool isSynopsysPrintRequired)
{
    stringstream optionalParamsHelpText;
    optionalParamsHelpText << "OPTIONAL PARAMS\n" << "\t(-t/ --target),    Usage: -t=<machine name or ip>, if not provided default is localhost\n" <<
    "\t(-p/ --port),      Usage: -p=<port number>, if not provided default is 12346\n" <<
    "\t(-h/ --help),      Print relevant help message, can be used anywhere. The command will not be executed\n" <<
    "\t(-j/ --json),      Reply message will be in JSON format\n" <<
    "\t(-d),              Set log verbosity level, usage: -d={0,1,2,3,4}\n\n";

    stringstream ss;
    if (isSynopsysPrintRequired)
    {
        ss << "\nSYNOPSIS\n";
        ss << "\t" << g_ApplicationName << " <Group> <OpCode> <ParmName-1>=<ParmValue-1> ... <ParmName-n>=<ParmValue-n> -<optional params>\n";
        ss << "\nNOTE\n";
        ss << "\tGroups, commands and parameters are case insensitive\n\n";
    }
    else
    {
        ss << "\n##########################################################################\n";
    }

    if (!group.empty())
    {
        ss << "COMMAND GROUP\n\t" << group << "\n\n";
    }
    else
    {
        ss << "SUPPORTED GROUPS\n" << helpText << "\n";
        ss << optionalParamsHelpText.str();
        ss << "MORE INFO\n";
        ss << "\tFor more information about specific Group run: \n";
        ss << "\t\t" <<  g_ApplicationName <<" <Group> -h \n";
        ss << "\tFor example, for more information about log collection capabilities:\n";
        ss << "\t\t" << g_ApplicationName << " log_collector -h\n";
        return ss.str();
    }

    if (!opcode.empty())
    {
        ss << "OPCODE\n\t" << opcode << "\n";
    }
    else
    {
        ss << "SUPPORTED OPCODES\n" << helpText << "\n";
        ss << optionalParamsHelpText.str();
        ss << "MORE INFO\n";
        ss << "\tFor more information about specific OpCode run:\n";
        ss << "\t\t" << g_ApplicationName << " <Group> <OpCode> -h \n";
        ss << "\tFor example, for more information about start log recording:\n";
        ss << "\t\t" << g_ApplicationName << " log_collector start_recording -h\n";
        return ss.str();
    }
    ss << "OPCODE SPECIFIC HELP\n\t" << helpText << "\n";
    ss << optionalParamsHelpText.str();
    return ss.str();
}

void GroupCommandBuilderBase::SetCommonParams(const CommandLineArgs & commandLineArgs, Json::Value & messageRoot) const
{
    messageRoot["KeepConnectionAlive"] = false; // will make host manager close the connection after sending the response.
    messageRoot["Client"] = "shell_11ad";
    messageRoot["ClientVersion"] = GetToolsVersion();
    messageRoot["Group"] = commandLineArgs.Group();
    messageRoot["OpCode"] = commandLineArgs.OpCode();
}

void GroupCommandBuilderBase::RegisterOpcodeGenerators(string opcodeGeneratorName, unique_ptr<OpCodeGeneratorBase>&& upOpcodeGenerator)
{
    m_opcodeGenerators.insert(make_pair(opcodeGeneratorName, move(upOpcodeGenerator)));
}

std::string GroupCommandBuilderBase::GetAvailableOpCodesHelp() const
{
    stringstream availableOpCodesHelp;
    availableOpCodesHelp << left; // left make text aligned to the left
    const auto alignmentWidth = 30;
    for (const auto& x : m_opcodeGenerators)
    {
        availableOpCodesHelp << "\t" << setw(alignmentWidth) << (x.first) // set the width to 30 to make all values aligned
            << OpCodeGeneratorBase::FormattedOpcodeDescription(x.second->OpCodeDescription(), alignmentWidth) << endl;
    }
    return availableOpCodesHelp.str();
}

std::string GroupCommandBuilderBase::GetFullHelp(const std::string& group)
{
    ostringstream ss;
    ss << HelpTemplate(GetAvailableOpCodesHelp(), group, "", false);
    for (const auto& x : m_opcodeGenerators)
    {
        ss << "\n#####################################\n";
        ss << "COMMAND OPCODE\n\t" << x.first << "\n";
        ss << x.second->GenerateHelpMessage();
    }
    return ss.str();
}
