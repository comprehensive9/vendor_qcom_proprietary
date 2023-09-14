/*
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "LogCollectorDeferredRecordingHandler.h"
#include "Host.h"
#include "DeviceManager.h"
#include "LogCollectorService.h"

#include <memory>

using namespace std;

void LogCollectorStartDeferredRecordingHandler::HandleRequest(
    const LogCollectorStartDeferredRecordingRequest& jsonRequest, LogCollectorStartDeferredRecordingResponse& jsonResponse)
{
    bool compress = false;
    const JsonValueBoxed<bool> compressBoxed = jsonRequest.GetCompressBoxed();
    if (compressBoxed.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(compressBoxed.BuildStateReport());
        return;
    }
    if (compressBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        compress = compressBoxed.GetValue();
    }

    bool upload = false;
    const JsonValueBoxed<bool> uploadBoxed = jsonRequest.GetUploadBoxed();
    if (uploadBoxed.GetState() == JsonValueState::JSON_VALUE_MALFORMED)
    {
        jsonResponse.Fail(uploadBoxed.BuildStateReport());
        return;
    }
    if (uploadBoxed.GetState() == JsonValueState::JSON_VALUE_PROVIDED)
    {
        upload = uploadBoxed.GetValue();
    }

    bool debugMode = false;
    const JsonValueBoxed<bool> debugModeBoxed = jsonRequest.GetDebugModeBoxed();
    switch (debugModeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MISSING:
        // Use default settings
        break;
    case JsonValueState::JSON_VALUE_PROVIDED:
        debugMode = debugModeBoxed.GetValue();
        break;
    default:
        jsonResponse.Fail(debugModeBoxed.BuildStateReport());
        return;
    }

    const JsonValueBoxed<RecordingType> recordingTypeBoxed = jsonRequest.GetRecordingTypeBoxed();
    RecordingType recordingType = RECORDING_TYPE_XML;
    switch (recordingTypeBoxed.GetState())
    {
    case JsonValueState::JSON_VALUE_MALFORMED:
        jsonResponse.Fail(recordingTypeBoxed.BuildStateReport());
        return;
    case JsonValueState::JSON_VALUE_MISSING:
        recordingType = RECORDING_TYPE_XML;
        break;
    default:
        // guaranty that value is provided
        recordingType = recordingTypeBoxed;
        break;
    }

    if (recordingType == RECORDING_TYPE_BOTH || recordingType == RECORDING_TYPE_PMC)
    {
        jsonResponse.Fail("RecordingType 'both'/'pmc' is not supported for deferred recording");
        return;
    }

    // convert given recording type to the corresponding logging type, both was already rejected
    const auto loggingType = static_cast<LoggingType>(recordingType);

    std::vector<LogCollectorModuleVerbosity> enforcedVerbosity;
    OperationStatus os = jsonRequest.GetModuleVerbosity(enforcedVerbosity);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
    }

    std::shared_ptr<DeferredRecordingContext> spDeferredRecordingContext =
        make_shared<DeferredRecordingContext>(compress, upload, debugMode, loggingType, enforcedVerbosity);

    os = Host::GetHost().GetLogCollectorService().EnableDeferredRecording(spDeferredRecordingContext);
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
    }
}

void LogCollectorStopDeferredRecordingHandler::HandleRequest(
    const Json::Value& jsonRequest, LogCollectorStopDeferredRecordingResponse& jsonResponse)
{
    (void)jsonRequest;
    LOG_DEBUG << "Stopping deferred recording " << std::endl;

    const OperationStatus os = Host::GetHost().GetLogCollectorService().DisableDeferredLogging();
    if (!os)
    {
        jsonResponse.Fail(os.GetStatusMessage());
        return;
    }

    if (!os.GetStatusMessage().empty())
    {
        // not a failure, only set the response message
        jsonResponse.SetMessage(os.GetStatusMessage());
    }
}

void LogCollectorGetDeferredRecordingHandler::HandleRequest(
    const Json::Value& jsonRequest, LogCollectorGetDeferredRecordingResponse& jsonResponse)
{
    (void)jsonRequest;
    LOG_DEBUG << "Querying deferred recording state" << std::endl;

    const shared_ptr<DeferredRecordingContext> spDeferredRecordingCtx =
        Host::GetHost().GetLogCollectorService().GetDeferredRecordingContext();

    if (spDeferredRecordingCtx)
    {
        LOG_DEBUG << "Reporting deferred recording context: " << *spDeferredRecordingCtx << endl;
        jsonResponse.Set(spDeferredRecordingCtx);
    }
}