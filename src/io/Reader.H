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
 * @brief Abstract base class for power system data file readers.
 */

#ifndef READER_H
#define READER_H

#include <string>

#include "Data.H"

/**
 * @class Reader
 * @brief Abstract reader interface for power system input formats.
 *
 * Subclasses implement the read() method for specific file formats
 * (e.g., IEEE Common Data Format, PSS/E Raw).
 */
class Reader {
    public:
        /** @brief Virtual destructor. */
        virtual ~Reader() = default;

        /**
         * @brief Read and parse the input file.
         * @param filename Path to the input data file.
         */
        virtual void read(const std::string& filename) = 0;

        /**
         * @brief Get the parsed bus data.
         * @return Const reference to BusData.
         */
        const BusData& getBusData() const noexcept;

        /**
         * @brief Get the parsed branch data.
         * @return Const reference to BranchData.
         */
        const BranchData& getBranchData() const noexcept;

    protected:
        BusData busData;          ///< Parsed bus data
        BranchData branchData;    ///< Parsed branch data
};

#endif
