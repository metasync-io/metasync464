#pragma once

#include <stdint.h>

#include <string>

namespace Skeleton {

    uint64_t PlayerNameToInt64(const std::string& s)
    {

        uint64_t l = 0;

        size_t len = s.length() > 12 ? 12 : s.length();

        for (size_t i = 0; i < len; ++i) {

            char c = s[i];

            l *= 37L;

            if (c >= 'A' && c <= 'Z') {

                l += (1 + c) - 65;

            }

            else if (c >= 'a' && c <= 'z') {

                l += (1 + c) - 97;
            }

            else if (c >= '0' && c <= '9') {

                l += (27 + c) - 48;
            }

        }

        while (l % 37L == 0L && l != 0L) {

            l /= 37L;
        }

        return l;
    }
}
