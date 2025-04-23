#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utilities {
    inline bool endsWith(const std::string& str, const std::string& suffix) {
        if (suffix.size() > str.size()) return false;
        return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    };
}

#endif
