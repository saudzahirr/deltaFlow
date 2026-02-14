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
 * @brief PSS/E Raw Data Format reader for deltaFlow.
 */

#ifndef PSSE_H
#define PSSE_H

#include "Reader.H"

/**
 * @class PSSERawFormat
 * @brief Reads power system data from PSS/E Raw format files (v32/v33).
 */
class PSSERawFormat: public Reader {
    public:
        /** @brief Destructor. */
        ~PSSERawFormat() = default;

        /**
         * @brief Parse a PSS/E .raw file.
         * @param filename Path to the .raw file.
         */
        void read(const std::string& filename) override;

};

#endif
