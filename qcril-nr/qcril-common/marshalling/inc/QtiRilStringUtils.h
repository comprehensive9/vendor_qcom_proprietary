/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef QTI_RIL_STRING_UTILS_INCLUDED
#define QTI_RIL_STRING_UTILS_INCLUDED

#include <string>

class QtiRilStringUtils{
    public:
        static char fromHex(const unsigned char nibble) {
            char ret = 0;
            if (isxdigit(nibble)) {
                if (nibble < 'a') {
                    ret = nibble-'0';
                }
                else {
                    ret = nibble-'a' + 0xa;
                }
            }
            return ret;
        }

        static char fromHex(const char h, const char l) {
            char ret = 0;
            if (isxdigit(h) && isxdigit(l)) {


                ret = (fromHex(h)<< 4 | fromHex(l));
            }
            return ret;
        }
        static std::string fromHex(std::string in) {
            std::string out;
            std::string::const_iterator it;
            std::transform(in.begin(), in.end(), in.begin(),
                [](char c){ return std::tolower(c); });
            for (it = in.begin(); it != in.end() ; it += 2) {
                if (isxdigit(*it) &&
                        (it + 1) != in.end() &&
                        isxdigit(*(it + 1))) {
                    char c = QtiRilStringUtils::fromHex(*it, *(it + 1));
                    out.push_back(c);
                }
            }
            return out;
        }
        static std::string toHex(char in) {
            const char tohex[] = "0123456789abcdef";
            unsigned char c = (unsigned char)in;
            std::string ret;
            ret.append(1, tohex[c >> 4]);
            ret.append(1, tohex[c & 0xf]);
            return ret;
        }
        static std::string toHex(std::string in) {
            std::string out;
            std::for_each(in.begin(), in.end(), [&out](char c) {out.append(toHex(c));});
            return out;
        }
};

#endif
