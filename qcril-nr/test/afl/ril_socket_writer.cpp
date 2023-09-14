/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <limits>
#include <string>
#include <ril_afl.hpp>
#include <RilApiSession.hpp>

unsigned long parseRilInstanceArg(int argc, char** argv) {
    const char* rilInstanceArg = nullptr;
    for (int i = 1; i < argc; i++) {
        // handles both "-i\S+" and "-i \S+"
        if (argv[i] != nullptr && !strncmp(argv[i], "-i", 2)) {
            // argv[i] is expected to be null-terminated
            if (argv[i][2] != '\0') {    // handle "-i\S+"
                rilInstanceArg = &argv[i][2];
            } else {                     // handle "-i \S+"
                i++;
                if (i < argc) {
                    rilInstanceArg = argv[i];
                }
            }
        }
    }

    unsigned long instance = std::numeric_limits<unsigned long>::max();
    if (rilInstanceArg != nullptr) {
        char* end = nullptr;
        instance = std::strtoul(rilInstanceArg, &end, 10);
        int errsv = errno;
        // conversion error
        if (errsv == ERANGE || end == rilInstanceArg) {
            instance = std::numeric_limits<unsigned long>::max();
        }
    }

    return instance;
}

int main(int argc, char** argv) {
    unsigned long rilInstance = parseRilInstanceArg(argc, argv);
    if (rilInstance == std::numeric_limits<unsigned long>::max()) {
        std::cout
            << "Please provide a valid ID of the RIL instance to connect"
            << " to using the -i option (e.g. -i 0)."
            << std::endl;
        return 1;
    }

    RilApiSession rilSession("/dev/socket/qcrild/rild" + std::to_string(rilInstance));
    Status s = qti::ril::afl::initRilApiSession(rilSession);
    if (s != Status::SUCCESS) {
        return 1;
    }

    std::string input;
    qti::ril::afl::readStdin(input);

    qti::ril::afl::issueRequest(rilSession, input);
}
