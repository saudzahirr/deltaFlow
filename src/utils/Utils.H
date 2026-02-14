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
      * @param str The filepath.
      * @return True if extension is .cdf, false otherwise.
      */
    inline bool isCommonDataFormat(const std::string& filePath) {
      auto ext = std::filesystem::path(filePath).extension();
      return ext == ".cdf" || ext == ".txt";
    }

    inline bool isRawFormat(const std::string& filePath) {
        return std::filesystem::path(filePath).extension() == ".raw";
    }
}

#endif
