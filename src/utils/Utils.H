/*
 * Copyright (c) 2024 saudzahirr
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with deltaFlow; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
