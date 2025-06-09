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
 * @brief Output utilities for writing power system analysis results to CSV files.
 *
 * This file declares functions for exporting simulation results (such as bus voltages, power flows, etc.)
 * to CSV files for further analysis or reporting.
 */

#ifndef WRITER_H
#define WRITER_H

#include <string>

// Forward declaration
class BusData;
class BranchData;

/**
  * @brief Writes bus data results to a CSV file.
  *
  * This function exports the results contained in the \ref BusData structure to a CSV file.
  * The output may include bus voltages, angles, power generation, loads, and other calculated quantities.
  *
  * @param busData The bus data structure containing simulation results.
  * @return true if the output was successfully written, false otherwise.
  */
bool writeOutputCSV(const BusData& busData);

#endif
