/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <arpa/inet.h>
#include <framework/Log.h>
#include <record_stream.h>
#include <RequestManager.hpp>
#include "Logger.h"
#define TAG "[RequestManager]"

RequestManager::RequestManager(int socketFd, const ErrorCallback& errorCb)
        : socketFd(socketFd), errorCallback(errorCb) {
    requestListener = std::thread(
        [this] {
            this->listenForRequests();
        }
    );

    responseListener = std::thread(
        [this] {
            this->listenForResponses();
        }
    );
}

RequestManager::~RequestManager() {
    Status s = issueRequest(
        RIL_REQUEST_DISCONNECT,
        nullptr,
        [] (RIL_Errno e, std::shared_ptr<Marshal> /*p*/) {
            if (e == RIL_E_SUCCESS) {
                Log::getInstance().d("Disconnection request successful.");
            } else {
                Log::getInstance().d("Disconnection request failed.");
            }
        }
    );

    if (s != Status::SUCCESS) {
        Log::getInstance().d("Failed to issue disconnection request.");
    }

    disconnectionRequested.store(true, std::memory_order_relaxed);

    requestListener.join();
}

void RequestManager::recordSocketError() {
    socketErrorDetected.store(true, std::memory_order_relaxed);
}

bool RequestManager::isSocketErrorDetected() {
    return socketErrorDetected.load(std::memory_order_relaxed);
}

bool RequestManager::isDisconnectionRequested() {
    return disconnectionRequested.load(std::memory_order_relaxed);
}

void RequestManager::invokeErrorCallback() {
    std::call_once(
        errorCallbackInvocationFlag,
        [this] () {
            if (this->errorCallback && !this->isDisconnectionRequested() &&
                    this->isSocketErrorDetected()) {
                this->errorCallback(Status::SOCKET_FAILURE);
            }
        }
    );
}

void RequestManager::detectSocketError() {
    struct pollfd fds;
    fds.fd = socketFd;
    fds.events = POLLIN | POLLOUT;

    int ret;

    do {
        fds.revents = 0;

        ret = poll(&fds, 1, 500);

        if (ret == 0) {
            QCRIL_HAL_LOG_DEBUG("Polling socket timed out. Assuming socket is still alive.");
        } else if (ret < 0) {
            if (ret != EINTR) {
               QCRIL_HAL_LOG_DEBUG("Error in polling socket. Assuming socket is dead.");
               recordSocketError();
            }
        } else if (fds.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            QCRIL_HAL_LOG_DEBUG("Socket error detected.");
            recordSocketError();
        } else if (fds.revents & (POLLIN | POLLOUT)) {
            QCRIL_HAL_LOG_DEBUG("Socket is still alive!");
        }
    } while (ret == EINTR);
}

void RequestManager::listenForRequests() {
    Log::getInstance().d("[RequestManager]: Starting loop to listen for requests.");

    std::queue<Request> requests;

    while (true) {
        // BEGIN unique_lock block with requestsWaitingToBeIssuedMutex
        {
            std::unique_lock<std::mutex> lock(requestsWaitingToBeIssuedMutex);
            if (requestsWaitingToBeIssued.empty()) {
                requestNotifier.wait(
                    lock,
                    [this] {
                        return !(this->requestsWaitingToBeIssued).empty() || isSocketErrorDetected();
                    }
                );
            }

            if (isSocketErrorDetected()) {
                break;
            }

            requests = std::move(requestsWaitingToBeIssued);
            requestsWaitingToBeIssued = std::queue<Request>();
        }
        // END unique_lock block with requestsWaitingToBeIssuedMutex

        Log::getInstance().d("[RequestManager]: Got one or more requests.");

        while (!requests.empty()) {
            Log::getInstance().d("[RequestManager]: Sending request to the server.");

            Request& req = requests.front();
            GenericResponseCallback* token = nullptr;

            // BEGIN scope boundary for "cb"
            {
                std::unique_ptr<GenericResponseCallback> cb(std::move(std::get<2>(req)));
                if (cb == nullptr) {
                    requests.pop();
                    continue;
                }

                if (isSocketErrorDetected()) {
                    (*(cb))(RIL_E_SYSTEM_ERR, nullptr);
                    requests.pop();
                    continue;
                }

                token = cb.get();

                // BEGIN lock_guard block with requestsWaitingForResponseMutex
                {
                    std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);
                    requestsWaitingForResponse[cb.get()] = std::move(cb);
                }
                // END lock_guard block with requestsWaitingForResponseMutex
            }
            // END scope boundary for "cb"

            Status s = sendRequestToServer(
                reinterpret_cast<uint64_t>(token),
                std::get<0>(req),
                std::move(std::get<1>(req))
            );

            if (s == Status::SOCKET_WRITE_FAILURE) {
                detectSocketError();
            }

            if (s != Status::SUCCESS) {
                std::unique_ptr<GenericResponseCallback> callback = nullptr;

                // BEGIN lock_guard block with requestsWaitingForResponseMutex
                {
                    std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);
                    auto search = requestsWaitingForResponse.find(token);
                    if (search != requestsWaitingForResponse.end()) {
                        callback = std::move(search->second);
                        requestsWaitingForResponse.erase(search);
                    }
                }
                // END lock_guard block with requestsWaitingForResponseMutex

                if (callback) {
                    (*(callback))(RIL_E_SYSTEM_ERR, nullptr);
                }
            }

            requests.pop();
        }

        if (isSocketErrorDetected()) {
            break;
        }
    }

    flushRequestsWaitingToBeIssued();
    //Wait for the response thread to finish before calling the error callback
    responseListener.join();
    invokeErrorCallback();
}

Status RequestManager::sendRequestToServer(uint64_t token, int32_t requestId, std::unique_ptr<Marshal> parcel) {
    if (socketFd < 0) {
        Log::getInstance().d("Socket FD not valid: " + std::to_string(socketFd));
        return Status::FAILURE;
    }

    // TODO: Introduce mutex to prevent concurrent writes to the socket

    // TODO: assert parcel size is not larger than the maximum 32-bit unsigned integer
    uint32_t bufferSize = htonl(static_cast<uint32_t>(sizeof(token) + sizeof(requestId) + (parcel ? parcel->dataSize() : 0)));
    Log::getInstance().d("[RequestManager]: Sending buffer of size "
                         + std::to_string(sizeof(token) + sizeof(requestId) + (parcel ? parcel->dataSize() : 0)));

    if (writeToSocket(socketFd, &bufferSize, sizeof(bufferSize)) < 0) {
        // TODO: return a more specific error
        Log::getInstance().d("[RequestManager]: Failed to transmit the size of the buffer over the socket.");
        return Status::SOCKET_WRITE_FAILURE;
    }

    // write request ID
    if (writeToSocket(socketFd, &requestId, sizeof(requestId)) < 0) {
        // TODO: return a more specific error
        Log::getInstance().d("[RequestManager]: Failed to transmit request ID over the socket.");
        return Status::SOCKET_WRITE_FAILURE;
    }

    // write token
    if (writeToSocket(socketFd, &token, sizeof(token)) < 0) {
        // TODO: return a more specific error
        Log::getInstance().d("[RequestManager]: Failed to transmit token over the socket.");
        return Status::SOCKET_WRITE_FAILURE;
    }

    if (parcel) {
        // write request parameter parcel
        if (writeToSocket(socketFd, parcel->data(), parcel->dataSize()) < 0) {
            // TODO: return a more specific error
            Log::getInstance().d("[RequestManager]: Failed to transmit request parameter parcel over the socket.");
            return Status::SOCKET_WRITE_FAILURE;
        }
    }

    return Status::SUCCESS;
}

void printBuffer(const void* buffer, size_t bufferLength, std::string& s) {
    char hex[4];
    for (int i = 0; i < bufferLength; i++) {
        std::snprintf(hex, sizeof(hex), "%-3.2hhX", *(static_cast<const uint8_t*>(buffer) + i));
        s += hex;
    }
}

ssize_t writeToSocket(int socketFd, const void* buffer, size_t bufferLength) {
    // TODO: Should writes to the socket timeout?
    std::string s;
    printBuffer(buffer, bufferLength, s);
    Log::getInstance().d("[RequestManager]: Buffer contents: " + s);

    ssize_t totalBytesWritten = 0;
    while (totalBytesWritten < bufferLength) {
        ssize_t bytesWritten = write(socketFd, static_cast<const uint8_t*>(buffer) + totalBytesWritten,
                                     bufferLength - totalBytesWritten);
        if (bytesWritten < 0) {
            Log::getInstance().d("[RequestManager]: Failed to write to the socket: " + std::string(strerror(errno)));
            return bytesWritten;
        }
        Log::getInstance().d("[RequestManager]: Wrote " + std::to_string(bytesWritten) + " bytes to the socket.");
        totalBytesWritten += bytesWritten;
    }

    return totalBytesWritten;
}

void RequestManager::listenForResponses() {
    Log::getInstance().d("[RequestManager]: Starting loop to listen for responses.");

    // Start polling on sockets and exit socket
    struct pollfd fds;
    fds.fd = socketFd;
    fds.events = POLLIN;

    std::unique_ptr<RecordStream, void (*)(RecordStream *)> rs(
        record_stream_new(socketFd, MAX_COMMAND_BYTES), record_stream_free);

    int ret;
    void *record;
    size_t recordLen;

    while (true) {
        // revents is set by the poll() implementation to signify which events caused the poll to return
        fds.revents = 0;

        ret = poll(&fds, 1, -1);

        if (ret < 0) {
            Log::getInstance().d("Error in polling socket, exiting " + std::to_string(ret));
            break;
        }

        if (fds.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            Log::getInstance().d("Error in poll, exiting listener thread");
            break;
        }

        if (fds.revents != POLLIN) {
            continue;
        }

        int errsv = 0;

        while (true) {
            errno = 0;
            ret = record_stream_get_next(rs.get(), &record, &recordLen);
            errsv = errno;

            Log::getInstance().d("record_stream_get_next: recordLen: " + std::to_string(recordLen)
                                 + ", ret: " + std::to_string(ret)
                                 + ", errno: " + std::to_string(ret ? errsv : 0)
                                 + ", errStr: " + strerror(errsv)
                                 + ", socket fd: " + std::to_string(socketFd));

            if(ret == 0 && record == nullptr) {
                // EOS - Done reading data from socket, client has disconnected
                break;
            } else if(ret < 0 && errsv != EAGAIN) {
                // error condition
                break;
            } else if(ret < 0 && errsv == EAGAIN) {
                // client has not finished writing to socket, wait for more data before processing
                break;
            } else if(ret == 0) {
                if(recordLen > MAX_COMMAND_BYTES) {
                    ret = -1;
                    Log::getInstance().d("Received parcel is too big to be valid; not handling");
                    break;
                } else {
                    Log::getInstance().d("Got a response parcel");
                    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
                    // TODO: check if we could end up with a dangling pointer inside
                    // parcel if this thread exits before RIL has a chance to process parcel
                    // (because that will destroy the RecordStream instance and thus free "record")
                    if (p) {
                        p->setData(std::string(reinterpret_cast<const char*>(record), recordLen));

                        uint32_t responseType;
                        p->read(responseType);
                        Status s;
                        //TODO: Use macro RESPONSE_SOLICITED instead of 0
                        if (responseType == 0) {
                            // TODO: Handle response in a separate thread?
                            s = handleResponse(p);
                            if (s != Status::SUCCESS) {
                                Log::getInstance().d("Failed to process response.");
                            }
                        } else if (responseType == 1) {
                            handleIndication(p);
                        }
                    }
                }
            }
        }

        // Fatal error or end-of-stream
        if (ret == 0 || !(errsv == EAGAIN || errsv == EINTR)) {
            if(ret != 0) {
                Log::getInstance().d("Error on reading socket recordLen: " + std::to_string(recordLen)
                                     + ", ret: " + std::to_string(ret) +
                                     + ", errno: " + std::to_string(ret ? errsv : 0)
                                     + ", errStr: " + strerror(errsv)
                                     + ", socket fd:" + std::to_string(socketFd));
            } else {
                Log::getInstance().d("EOS on socket recordLen: " + std::to_string(recordLen)
                                     + ", ret: " + std::to_string(ret)
                                     + ", errno: " + std::to_string(ret ? errsv : 0)
                                     + ", errStr: " + strerror(errsv)
                                     + ", socket fd: " + std::to_string(socketFd));
            }
            break;
        }
    }

    recordSocketError();
    requestNotifier.notify_one();
    flushRequestsWaitingForResponse();
}

Status RequestManager::handleResponse(std::shared_ptr<Marshal> p) {
    if (p == nullptr) {
        return Status::FAILURE;
    }

    // TODO: static assert sizeof(uint64_t) == sizeof(GenericResponseCallback*)
    // TODO: check for errors during unmarshalling
    uint64_t token;
    uint32_t errorCode;
    p->read(token);
    p->read(errorCode);

    std::unique_ptr<GenericResponseCallback> cb;

    {
        std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);

        auto iter = requestsWaitingForResponse.find(reinterpret_cast<GenericResponseCallback*>(token));
        if (iter == requestsWaitingForResponse.end()) {
            Log::getInstance().d("Cannot find request with token " +
                std::to_string(token));
            return Status::FAILURE;
        } else {
            if (iter->second == nullptr) {
                Log::getInstance().d(
                    "Response callback is null for request with token " +
                    std::to_string(token));
                return Status::FAILURE;
            } else {
                cb = std::move(iter->second);
                //(*(iter->second))(static_cast<RIL_Errno>(errorCode), p);
            }
            requestsWaitingForResponse.erase(iter);
        }
    }

    if (cb != nullptr) {
        (*cb)(static_cast<RIL_Errno>(errorCode), p);
    }

    return Status::SUCCESS;
}

Status RequestManager::handleIndication(std::shared_ptr<Marshal> p) {
    if (p == nullptr) {
        return Status::FAILURE;
    }

    // TODO: check for errors during unmarshalling
    uint32_t indicationId;
    p->read(indicationId);

    GenericIndicationHandler indicationHandler;

    {
        std::lock_guard<std::mutex> lock(indicationHandlersMutex);
        auto iter = indicationHandlers.find(indicationId);
        if (iter == indicationHandlers.end()) {
            return Status::FAILURE;
        } else {
            if (iter->second == nullptr) {
                return Status::FAILURE;
            } else {
                //GenericIndicationHandler& indHandler = *(iter->second);
                //indicationHandler = indHandler;
                indicationHandler = *(iter->second);
            }
        }
    }

    if (indicationHandler) {
        indicationHandler(p);
    }

    return Status::SUCCESS;
}

Status RequestManager::issueRequest(int32_t requestId, std::unique_ptr<Marshal> p, const GenericResponseCallback& cb) {
    if (isSocketErrorDetected() || isDisconnectionRequested()) {
        Log::getInstance().d("[RequestManager]: Requests aren't currently allowed.");
        return Status::FAILURE;
    }

    Log::getInstance().d("[RequestManager]: Pushing a request to the queue.");

    std::unique_ptr<GenericResponseCallback> cbCopy = std::make_unique<GenericResponseCallback>(cb);
    if (cbCopy == nullptr) {
        return Status::FAILURE;
    }

    {
        std::lock_guard<std::mutex> lock(requestsWaitingToBeIssuedMutex);
        requestsWaitingToBeIssued.push(std::make_tuple(requestId, std::move(p), std::move(cbCopy)));
    }
    requestNotifier.notify_one();
    Log::getInstance().d("[RequestManager]: Pushed a request to the queue.");

    return Status::SUCCESS;
}

Status RequestManager::registerIndicationHandler(int32_t indicationId, const GenericIndicationHandler& indicationHandler) {
    Log::getInstance().d("[RequestManager]: Registering indication handler for indication " + std::to_string(indicationId));

    std::unique_ptr<GenericIndicationHandler> indicationHandlerCopy = std::make_unique<GenericIndicationHandler>(indicationHandler);
    if (indicationHandlerCopy == nullptr) {
        return Status::FAILURE;
    }

    {
        std::lock_guard<std::mutex> lock(indicationHandlersMutex);
        indicationHandlers[indicationId] = std::move(indicationHandlerCopy);
    }

    Log::getInstance().d("[RequestManager]: Registered indication handler for indication " + std::to_string(indicationId));
    return Status::SUCCESS;
}

void RequestManager::flushRequestsWaitingForResponse() {
    {
        std::lock_guard<std::mutex> lock(requestsWaitingForResponseMutex);
        std::for_each(
            requestsWaitingForResponse.begin(),
            requestsWaitingForResponse.end(),
            [] (const auto& iter) {
                if (iter.second != nullptr) {
                    (*(iter.second))(RIL_E_SYSTEM_ERR, nullptr);
                }
            }
        );
        requestsWaitingForResponse.clear();
    }
}

void RequestManager::flushRequestsWaitingToBeIssued() {
    {
        std::lock_guard<std::mutex> lock(requestsWaitingToBeIssuedMutex);
        std::unique_ptr<GenericResponseCallback> cb;
        while (!requestsWaitingToBeIssued.empty()) {
            cb = std::move(std::get<2>(requestsWaitingToBeIssued.front()));
            if (cb != nullptr) {
                (*cb)(RIL_E_SYSTEM_ERR, nullptr);
            }
            requestsWaitingToBeIssued.pop();
        }
    }
}
