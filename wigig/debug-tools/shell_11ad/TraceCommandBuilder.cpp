/*
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "TraceCommandBuilder.h"
#include "ParameterBase.h"

using namespace std;

TraceCommandBuilder::TraceCommandBuilder()
{
    TraceCommandBuilder::SetOpcodeGenerators();
}

void TraceCommandBuilder::SetOpcodeGenerators()
{
    RegisterOpcodeGenerators("set_severity", unique_ptr<OpCodeGeneratorBase>(new TraceSetSeverityGenerator()));
    RegisterOpcodeGenerators("get_severity", unique_ptr<OpCodeGeneratorBase>(new TraceGetSeverityGenerator()));
    RegisterOpcodeGenerators("split", unique_ptr<OpCodeGeneratorBase>(new TraceSplitGenerator()));
    RegisterOpcodeGenerators("set_path", unique_ptr<OpCodeGeneratorBase>(new TraceSetPathGenerator()));
    RegisterOpcodeGenerators("get_path", unique_ptr<OpCodeGeneratorBase>(new TraceGetPathGenerator()));
}

TraceSetSeverityGenerator::TraceSetSeverityGenerator() :
    OpCodeGeneratorBase("Set debug trace severity level for the different trace modules.\n")
{
    const string severityParamDescription = "The severity level";
    const string severityParamValues = "{VRB, DBG, INF, WRN, ERR}";
    const string moduleParamDescription = "The module that need to update";
    const string moduleParamValues = "Get the supported modules values from \"get_severity\" command";

    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module", false, moduleParamDescription, moduleParamValues, "", false)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Severity", true, severityParamDescription, severityParamValues, "INF", false)));

    RegisterExample("trace set_severity Severity=VRB",
        "Set all debug trace modules severity to verbose level");
    RegisterExample("trace set_severity Module=RDWR Severity=WRN",
        "Set read/write debug trace module severity to warning level");
}

TraceGetSeverityGenerator::TraceGetSeverityGenerator() :
    OpCodeGeneratorBase("Get Trace log severity level.\nVRB-verbose, DBG-debug, INF-information, WRN-warning, ERR-error")
{
    RegisterExample("trace get_severity",
        "Get debug trace severity level");
}

TraceSplitGenerator::TraceSplitGenerator() :
    OpCodeGeneratorBase("Flush and close current trace file. A new file will be created for further traces. This operation is required for file collection")
{
    RegisterExample("split",
        "Flush and close current trace file");
}

TraceSetPathGenerator::TraceSetPathGenerator() :
    OpCodeGeneratorBase("Set debug trace path.")
{
    const string moduleParamDescription = "Debug trace path";

    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Path", true, moduleParamDescription)));

    RegisterExample("trace set_path Path=/var/host_manager_11ad_data/new_folder/",
        "Set debug trace path to /var/host_manager_11ad_data/new_folder/");
}

TraceGetPathGenerator::TraceGetPathGenerator() :
    OpCodeGeneratorBase("Get debug trace current path.")
{
    RegisterExample("trace get_path",
        "Get debug trace path");
}
