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
#include <Eigen/Dense>

// Forward declaration
class BusData;
class BranchData;

/**
  * @brief Displays bus data in a human-readable format.
  * @param busData The bus data to display.
  */
void dispBusData(const BusData& busData);

/**
  * @brief Displays the line flow results, including power flow and losses.
  * @param busData The bus data structure.
  * @param branchData The branch data structure.
  * @param Y The bus admittance matrix ($$ Y_{bus} $$).
  * @param basemva The base MVA for per-unit system (default: 100).
  */
void dispLineFlow(
    const BusData& busData,
    const BranchData& branchData,
    const Eigen::MatrixXcd& Y,
    double basemva = 100
);

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
