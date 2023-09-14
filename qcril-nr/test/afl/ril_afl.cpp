/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <ril_afl.hpp>
#include <Marshal.h>
#include <framework/Log.h>
#include <RilApiSession.hpp>

namespace qti {
namespace ril {
namespace afl {

static std::thread aflProcessingThread;

Status initRilApiSession(RilApiSession& rilSession) {
    Status s = rilSession.initialize(
        [] (Status s) {
            Log::getInstance().d("Error detected in session. Error: " + std::to_string((int) s));
        }
    );

    if (s != Status::SUCCESS) {
        Log::getInstance().d("Failed to initialize API session.");
    } else {
        Log::getInstance().d("Successfully initialized API session.");
    }

    return s;
}

void readStdin(std::string& input) {
    do {
        char inputSegment[128] = { 0 };
        std::cin.clear();
        size_t i = 0;

        while (i < sizeof(inputSegment)) {
            std::cin.get(inputSegment[i]);
            if (std::cin.fail()) {
                break;
            }
            i++;
        }

        input.append(inputSegment, i);

        // No more data to read from the input stream
        if (i != sizeof(inputSegment)) {
            break;
        }
    } while (true);
}

void issueRequest(RilApiSession& rilSession, std::string& input) {
    /* The string must have at least 2 characters (bytes).
       The first two bytes are the request ID. The rest of
       the bytes represent the marshal. */
    if (input.size() < 2) {
        return;
    }

    unsigned short requestId = static_cast<unsigned short>(input[0]);
    requestId <<= 8;
    requestId |= input[1];
    input.erase(0, 2);

    if (requestId == RIL_REQUEST_DISCONNECT) {
        return;
    }

    if (rilSession.reqMgr != nullptr) {
        std::unique_ptr<Marshal> parcel = std::make_unique<Marshal>();
        if (parcel != nullptr) {
            parcel->setData(std::move(input));
            rilSession.reqMgr->issueRequest(
                requestId,
                std::move(parcel),
                [] (RIL_Errno e, std::shared_ptr<Marshal> p) {
                    (void) e;
                    (void) p;
                }
            );
        }
    }
}

void processAflInput(const std::string& rilSocketPath) {
    RilApiSession rilSession(rilSocketPath);
    Status s = initRilApiSession(rilSession);

    if (s != Status::SUCCESS) {
        return;
    }

    while (__AFL_LOOP(1000)) {
        std::string aflInput;
        readStdin(aflInput);
        issueRequest(rilSession, aflInput);
    }
}

void setup(const std::string& rilSocketPath) {
    Log::getInstance().d("Starting AFL input processing thread.");
    aflProcessingThread = std::thread(processAflInput, rilSocketPath);
    if (aflProcessingThread.joinable()) {
        Log::getInstance().d("AFL input processing thread is joinable.");
    } else {
        Log::getInstance().d("AFL input processing thread is NOT joinable.");
    }
    aflProcessingThread.detach(); // TODO: Detach or join in RIL daemon's main()?
}

} // namespace afl
} // namespace ril
} // namespace qti

// Wrapper for C
void ril_afl_setup(const char* rilSocketPath) {
    qti::ril::afl::setup(rilSocketPath);
}
