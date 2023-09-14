/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <telephony/ril.h>
#include <Marshal.h>
#include <Status.hpp>

// TODO: Use a common define for the server and the client
#define MAX_COMMAND_BYTES (8 * 1024)

ssize_t writeToSocket(int socketFd, const void* buffer, size_t bufferLength);

class RequestManager {
    public:
        typedef std::function<void(RIL_Errno, std::shared_ptr<Marshal>)> GenericResponseCallback;
        typedef std::function<void(std::shared_ptr<Marshal>)> GenericIndicationHandler;
        typedef std::function<void(Status)> ErrorCallback;

    private:
        typedef std::tuple<int32_t, std::unique_ptr<Marshal>, std::unique_ptr<GenericResponseCallback>> Request;

    /* Responsible for receiving requests, forwarding them to the server
     * and handling responses from the server. */
    public:
        RequestManager(int socketFd, const ErrorCallback& errorCb);
        ~RequestManager();
        Status issueRequest(int32_t requestId, std::unique_ptr<Marshal> p, const GenericResponseCallback& cb);
        Status registerIndicationHandler(int32_t indicationId, const GenericIndicationHandler& indicationHandler);

    private:
        void listenForRequests();
        void listenForResponses();
        Status sendRequestToServer(uint64_t token, int32_t requestId, std::unique_ptr<Marshal> parcel);
        Status handleResponse(std::shared_ptr<Marshal> p);
        Status handleIndication(std::shared_ptr<Marshal> p);
        bool isSocketErrorDetected();
        bool isDisconnectionRequested();
        void detectSocketError();
        void recordSocketError();
        void invokeErrorCallback();
        void flushRequestsWaitingForResponse();
        void flushRequestsWaitingToBeIssued();

    private:
        int socketFd;
        std::atomic_bool socketErrorDetected = false;
        std::atomic_bool disconnectionRequested = false;

        std::once_flag errorCallbackInvocationFlag;
        const ErrorCallback errorCallback;

        std::thread requestListener;
        std::thread responseListener;

        std::queue<Request> requestsWaitingToBeIssued;
        std::mutex requestsWaitingToBeIssuedMutex;
        std::condition_variable requestNotifier;

        std::unordered_map<GenericResponseCallback*, std::unique_ptr<GenericResponseCallback>> requestsWaitingForResponse;
        std::mutex requestsWaitingForResponseMutex;

        std::unordered_map<int32_t, std::unique_ptr<GenericIndicationHandler>> indicationHandlers;
        std::mutex indicationHandlersMutex;
};
