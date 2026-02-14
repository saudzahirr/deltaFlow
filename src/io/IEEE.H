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
 * @brief IEEE Common Data Format reader for deltaFlow.
 */

#ifndef IEEE_H
#define IEEE_H

#include "Reader.H"

/**
 * @class IEEECommonDataFormat
 * @brief Reads power system data from IEEE Common Data Format files.
 */
class IEEECommonDataFormat: public Reader {
    public:
        /** @brief Destructor. */
        ~IEEECommonDataFormat() = default;

        /**
         * @brief Parse an IEEE CDF file (.cdf or .txt).
         * @param filename Path to the CDF file.
         */
        void read(const std::string& filename) override;
};

#endif
