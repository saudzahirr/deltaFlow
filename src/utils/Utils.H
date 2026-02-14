/*
 * Copyright (c) 2024 Saud Zahir
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with deltaFlow.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Utility functions and helpers for deltaFlow.
 */

#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>

/**
  * @namespace Utilities
  * @brief Collection of utility functions.
  */
namespace Utilities {

    /**
      * @brief Check if filepath is IEEE Common Data Format.
      * @param filePath The filepath.
      * @return True if extension is .cdf or .txt, false otherwise.
      */
    inline bool isCommonDataFormat(const std::string& filePath) {
      auto ext = std::filesystem::path(filePath).extension();
      return ext == ".cdf" || ext == ".txt";
    }

    /**
      * @brief Check if filepath is PSS/E Raw format.
      * @param filePath The filepath.
      * @return True if extension is .raw, false otherwise.
      */
    inline bool isRawFormat(const std::string& filePath) {
        return std::filesystem::path(filePath).extension() == ".raw";
    }

    /**
      * @brief Strip leading and trailing whitespace from a string.
      * @param s The input string.
      * @return Trimmed string.
      */
    inline std::string strip(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    /**
      * @brief Strip surrounding single quotes and whitespace from a string.
      * @param s The input string.
      * @return String with quotes removed.
      */
    inline std::string stripQuotes(const std::string& s) {
        std::string t = strip(s);
        if (t.size() >= 2 && t.front() == '\'' && t.back() == '\'')
            return strip(t.substr(1, t.size() - 2));
        return t;
    }
}

#endif
