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

#ifndef READER_H
#define READER_H

#include <string>

#include "Data.H"

class Reader {
    public:
        virtual ~Reader() = default;

        virtual void read(const std::string& filename) = 0;

        const BusData& getBusData() const noexcept;

        const BranchData& getBranchData() const noexcept;

    protected:
        BusData busData;
        BranchData branchData;
};

#endif
