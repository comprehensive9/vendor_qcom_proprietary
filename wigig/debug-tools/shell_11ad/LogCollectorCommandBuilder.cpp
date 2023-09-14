/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorCommandBuilder.h"
#include "ParameterBase.h"

using namespace std;

LogCollectorCommandBuilder::LogCollectorCommandBuilder()
{
    SetOpcodeGenerators();
}

void LogCollectorCommandBuilder::SetOpcodeGenerators()
{
    RegisterOpcodeGenerators("get_health", unique_ptr<OpCodeGeneratorBase>(new LCGetHealthGenerator()));
    RegisterOpcodeGenerators("get_state", unique_ptr<OpCodeGeneratorBase>(new LCGetStateGenerator()));
    RegisterOpcodeGenerators("get_config", unique_ptr<OpCodeGeneratorBase>(new LCGetConfigGenerator()));
    RegisterOpcodeGenerators("set_config", unique_ptr<OpCodeGeneratorBase>(new LCSetConfigGenerator()));
    RegisterOpcodeGenerators("reset_config", unique_ptr<OpCodeGeneratorBase>(new LCReSetConfigGenerator()));
    RegisterOpcodeGenerators("set_verbosity", unique_ptr<OpCodeGeneratorBase>(new LCSetVerbosityGenerator()));
    RegisterOpcodeGenerators("get_verbosity", unique_ptr<OpCodeGeneratorBase>(new LCGetVerbosityGenerator()));
    RegisterOpcodeGenerators("get_enforced_verbosity", unique_ptr<OpCodeGeneratorBase>(new LCGetEnforcedVerbosityGenerator()));
    RegisterOpcodeGenerators("start_recording", unique_ptr<OpCodeGeneratorBase>(new LCStartRecordingGenerator()));
    RegisterOpcodeGenerators("stop_recording", unique_ptr<OpCodeGeneratorBase>(new LCStopRecordingGenerator()));
    RegisterOpcodeGenerators("split_recording", unique_ptr<OpCodeGeneratorBase>(new LCSplitRecordingGenerator()));
    RegisterOpcodeGenerators("start_deferred_recording", unique_ptr<OpCodeGeneratorBase>(new LCStartDeferredRecordingGenerator()));
    RegisterOpcodeGenerators("stop_deferred_recording", unique_ptr<OpCodeGeneratorBase>(new LCStopDeferredRecordingGenerator()));
    RegisterOpcodeGenerators("get_deferred_recording", unique_ptr<OpCodeGeneratorBase>(new LCGetDeferredRecordingGenerator()));
    RegisterOpcodeGenerators("strings_dump", unique_ptr<OpCodeGeneratorBase>(new LCDumpFmtStringsGenerator()));
}

LCGetStateGenerator::LCGetStateGenerator():
    OpCodeGeneratorBase("Get recording (TXT and RAW) and publishing state")
{
    RegisterDeviceParam();

    RegisterExample("log_collector get_state", "Get log collector recording and publishing state");
    RegisterExample("log_collector get_state Device=wlan0", "Get log collector recording and publishing state of device wlan0");
}

LCGetHealthGenerator::LCGetHealthGenerator() :
    OpCodeGeneratorBase("Get TXT/RAW logs recorder health for the given CPU")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("CpuType", true, "select specific cpu", "fw / ucode")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("RecordingType", true, "select recording type", "raw / txt")));

    RegisterExample("log_collector get_health CpuType=fw RecordingType=txt", "Get FW logs TXT recorder health report");
    RegisterExample("log_collector get_health CpuType=ucode RecordingType=raw Device=wlan0", "Get uCode logs RAW recorder health report of device wlan0");
}

LCGetConfigGenerator::LCGetConfigGenerator() :
    OpCodeGeneratorBase("Get log collector configuration")
{
    RegisterDeviceParam();

    RegisterExample("log_collector get_config",
        "Get log collector configuration");
    RegisterExample("log_collector get_config Device=wlan0", "Get log collector configuration of device wlan0");
}

LCStartRecordingGenerator::LCStartRecordingGenerator() :
    OpCodeGeneratorBase("Start fw/ucode log recording")
{
    const string moduleParamDescription = "set specific module verbosity - Advanced";
    const string moduleParamValues = "any combination of {V, I, E, W}, e.g V, VI, IE .. or empty(\"\")";

    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("CpuType", false, "select specific cpu", "fw / ucode", "fw and ucode")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("RecordingType", false, "select a recording type", "raw / txt", "raw")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("IgnoreLogHistory", false, "ignore log history when starting the recording", "false")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("Compress", false, "compress fw/ucode logs after collection", "false")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("Upload", false, "upload fw/ucode logs after collection", "false")));

    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("DefaultVerbosity", false, "Specify verbosity level for all non-specified modules", moduleParamValues)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module0", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module1", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module2", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module3", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module4", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module5", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module6", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module7", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module8", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module9", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module10", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module11", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module12", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module13", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module14", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module15", false, moduleParamDescription, moduleParamValues, "", true)));

    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("Debug", false, "turn debug mode on/off", "false")));

    RegisterExample("log_collector start_recording",
        "Start recording FW and UCODE logs in RAW format");
    RegisterExample("log_collector start_recording Device=wlan0 CpuType=fw RecordingType=txt Compress=true Upload=true",
        "Start recording FW logs in TXT format on device wlan0 with compressing and uploading after collection");
    RegisterNote("Recording TXT logs can not be started if strings conversion file doesn't exist at configured location (to change it use set_config)");
    RegisterNote("Compress and Upload are supported only on linux system");
}

LCStopRecordingGenerator::LCStopRecordingGenerator() :
    OpCodeGeneratorBase("Stop fw/ucode log recording")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("CpuType", false, "select specific cpu", "fw / ucode", "fw and ucode")));
    RegisterExample("log_collector stop_recording",
        "Stop all FW and UCODE active log recording");
    RegisterExample("log_collector stop_recording Device=wlan0 CpuType=fw",
        "Stop all FW active log recording on device wlan0");
}

LCSplitRecordingGenerator::LCSplitRecordingGenerator() :
    OpCodeGeneratorBase("Split all recordings")
{
    RegisterExample("log_collector split_recording", "Split all active recordings");
}


LCStartDeferredRecordingGenerator::LCStartDeferredRecordingGenerator():
    OpCodeGeneratorBase("Start deferred log recording (auto start for a new discovered device)")
{
    const string moduleParamDescription = "set specific module verbosity - Advanced";
    const string moduleParamValues = "any combination of {V, I, E, W}, e.g V, VI, IE .. or empty(\"\")";

    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("RecordingType", false, "select a recording type", "raw / txt", "raw")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("Compress", false, "compress fw/ucode logs after collection", "false")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("Upload", false, "compress fw/ucode logs after collection", "false")));

    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("DefaultVerbosity", false, "Specify verbosity level for all non-specified modules", moduleParamValues)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module0", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module1", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module2", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module3", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module4", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module5", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module6", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module7", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module8", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module9", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module10", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module11", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module12", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module13", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module14", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module15", false, moduleParamDescription, moduleParamValues, "", true)));

    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("Debug", false, "turn debug mode on/off", "false")));

    RegisterExample("log_collector start_deferred_recording",
        "Start deferred FW and UCODE log recording");
    RegisterExample("log_collector start_deferred_recording RecordingType=txt Compress=true Upload=true",
        "Start deferred FW and UCODE log recording in TXT format with compressing and uploading after collection");
}


LCStopDeferredRecordingGenerator::LCStopDeferredRecordingGenerator() :
    OpCodeGeneratorBase("Stop deferred recording mode")
{
    RegisterExample("log_collector stop_deferred_recording", "Disable deferred log recording");
}

LCGetDeferredRecordingGenerator::LCGetDeferredRecordingGenerator() :
    OpCodeGeneratorBase("Get deferred recording state")
{
    RegisterExample("log_collector get_deferred_recording", "Get current deferred recording state");
}


LCSetConfigGenerator::LCSetConfigGenerator() :
    OpCodeGeneratorBase("Set log collector configuration- \npolling interval, log file maximal size, file suffix etc.")
{
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("PollingIntervalMs", false, "set the polling interval for log collection")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("MaxSingleFileSizeMb", false, "set max size in MB for a single file")));
    RegisterParam(unique_ptr<ParameterBase>(new IntParameter("MaxNumberOfLogFiles", false, "set max number of files created before overriding")));
    RegisterParam(unique_ptr<ParameterBase>(new BoolParameter("FlushChunk", false, "explicit log file flush for every chunk. Warning: may degrade performance!")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("ConversionFilePath", false, "set the path to strings conversion files (both ucode and fw)")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("LogFilesDirectory", false, "set the path for the log recording directory")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("TargetServer", false, "set the IP or host name of the server to upload logs to")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("UserName", false, "set the user name for the upload server")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("RemotePath", false, "set the remote path on the upload server")));


    RegisterExample("log_collector set_config PollingIntervalMs=100",
        "Set the following configuration for log collector: polling interval: 100 ms");
    RegisterExample("log_collector set_config PollingIntervalMs=100 MaxSingleFileSizeMb=50 MaxNumberOfLogFiles=10",
        "Set the following configuration for log collector: polling interval: 100 ms, max file size: 50MB,  max number of files: 10");
    RegisterExample(
        R"(log_collector set_config ConversionFilePath="/root/fw" LogFilesDirectory="/var/Logs")",
        "Set the following configuration for log collector: path to FW & UCODE strings conversion files: /var/fw, log recording directory: /var/Logs");
    RegisterNote("Upload logs to server supported only on linux system");
}

LCReSetConfigGenerator::LCReSetConfigGenerator():
    OpCodeGeneratorBase("ReSet log collector to default configuration")
{
    RegisterExample("log_collector reset_config",
        "Reset log collector to default configuration");
}

LCSetVerbosityGenerator::LCSetVerbosityGenerator() :
    OpCodeGeneratorBase("Set FW / UCODE log module verbosity. \nV-verbose, I-information, E-error, W-warning")
{
    const string moduleParamDescription = "set specific module verbosity - Advanced";
    const string moduleParamValues = "any combination of {V, I, E, W}, e.g V, VI, IE .. or empty(\"\")";

    RegisterDeviceParam();

    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("CpuType", false, "select specific cpu", "fw / ucode")));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("DefaultVerbosity", false, "set verbosity level for all non-specified modules", moduleParamValues, "",true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module0", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module1", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module2", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module3", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module4", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module5", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module6", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module7", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module8", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module9", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module10", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module11", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module12", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module13", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module14", false, moduleParamDescription, moduleParamValues, "", true)));
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("Module15", false, moduleParamDescription, moduleParamValues, "", true)));

    RegisterExample("log_collector set_verbosity CpuType=fw DefaultVerbosity=EW",
        "Set verbosity error/warning for all FW modules on a default device");
    RegisterExample("log_collector set_verbosity Device=wlan0 CpuType=ucode Module0=EWIV Module10=W Module8=IV",
        "Set specific UCODE log verbosity on device wlan0 for modules 0, 8 and 10.");
    RegisterExample("log_collector set_verbosity CpuType=fw DefaultVerbosity=VIEW Module0=E",
        "Set verbosity error for all FW module 0, all other modules are configured for all levels");
}

LCGetVerbosityGenerator::LCGetVerbosityGenerator() :
    OpCodeGeneratorBase("Get FW / UCODE log module verbosity.\nV-verbose, I-information, E-error, W-warning")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("CpuType", true, "select specific cpu", "fw / ucode")));

    RegisterExample("log_collector get_verbosity CpuType=fw",
        "Get FW log module verbosity from a default device");
    RegisterExample("log_collector get_verbosity Device=wlan0 CpuType=ucode",
        "Get UCODE log module verbosity from device wlan0");
}

LCGetEnforcedVerbosityGenerator::LCGetEnforcedVerbosityGenerator() :
    OpCodeGeneratorBase("Get FW / UCODE enforced log module verbosity.\nV-verbose, I-information, E-error, W-warning\nEnforced verbosity may be set by set_verbosity, start_deferred_recording, start_recording commands and overwrites default verbosity for specific modules")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("CpuType", true, "select specific cpu", "fw / ucode")));

    RegisterExample("log_collector get_enforced_verbosity CpuType=fw",
        "Get enforced FW log module verbosity from a default device");
    RegisterExample("log_collector get_enforced_verbosity Device=wlan0 CpuType=ucode",
        "Get UCODE enforced log module verbosity from device wlan0");
}

LCDumpFmtStringsGenerator::LCDumpFmtStringsGenerator() :
    OpCodeGeneratorBase("Dump string conversion file for given CPU")
{
    RegisterDeviceParam();
    RegisterParam(unique_ptr<ParameterBase>(new StringParameter("CpuType", false, "select specific cpu", "fw / ucode")));

    RegisterExample("log_collector strings_dump",
        "Dump FW and uCode string conversion files from a default device");
    RegisterExample("log_collector strings_dump Device=wlan0 CpuType=ucode",
        "Dump uCode string conversion file from device wlan0");

    RegisterNote("If no CpuType given, dump string conversion files will be created for both FW and uCode");
}
