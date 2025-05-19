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
 * @brief Data structures and utility functions for reading and displaying bus and branch data in power system analysis.
 *
 * This file defines the BusData and BranchData structures, as well as functions for reading data from CSV files
 * and displaying results. These structures are essential for power system modeling, providing all relevant quantities
 * such as voltages, power, admittance, and transformer tap ratios.
 */

#ifndef READER_H
#define READER_H

#include <Eigen/Dense>
#include <vector>
#include <string>

/**
  * @struct BusData
  * @brief Contains all relevant data for each bus in the power system.
  *
  * - ID: Integer bus numbers.
  * - Name: String names of buses.
  * - Type: Integer codes for bus type (e.g., 1=Slack, 2=PV, 3=PQ).
  * - V: Voltage magnitude (p.u.).
  * - delta: Voltage angle (radians or degrees).
  * - Pg: Active power generation (MW or p.u.).
  * - Qg: Reactive power generation (MVAr or p.u.).
  * - Pl: Active power load (MW or p.u.).
  * - Ql: Reactive power load (MVAr or p.u.).
  * - Qgmax: Maximum reactive power generation.
  * - Qgmin: Minimum reactive power generation.
  * - Gs: Shunt conductance ($$ G_{sh} $$, p.u.).
  * - Bs: Shunt susceptance ($$ B_{sh} $$, p.u.).
  */
struct BusData {
    Eigen::VectorXi ID;                ///< Bus numbers
    std::vector<std::string> Name;     ///< Bus names
    Eigen::VectorXi Type;              ///< Bus type (1=Slack, 2=PV, 3=PQ)

    Eigen::VectorXd V;                 ///< Voltage magnitude [p.u.]
    Eigen::VectorXd delta;             ///< Voltage angle [rad or deg]
    Eigen::VectorXd Pg;                ///< Active power generation [MW or p.u.]
    Eigen::VectorXd Qg;                ///< Reactive power generation [MVAr or p.u.]
    Eigen::VectorXd Pl;                ///< Active power load [MW or p.u.]
    Eigen::VectorXd Ql;                ///< Reactive power load [MVAr or p.u.]
    Eigen::VectorXd Qgmax;             ///< Max reactive power generation [MVAr or p.u.]
    Eigen::VectorXd Qgmin;             ///< Min reactive power generation [MVAr or p.u.]
    Eigen::VectorXd Gs;                ///< Shunt conductance ($$ G_{sh} $$) [p.u.]
    Eigen::VectorXd Bs;                ///< Shunt susceptance ($$ B_{sh} $$) [p.u.]
 };

/**
  * @brief Reads bus data from a CSV file and fills a BusData structure.
  * @param filename Path to the bus data CSV file.
  * @param busData Reference to BusData structure to populate.
  * @return true if the file was read and parsed successfully, false otherwise.
  */
bool readBusDataCSV(const std::string& filename, BusData& busData);

/**
  * @struct BranchData
  * @brief Contains all relevant data for each transmission line or transformer branch.
  *
  * - From, To: Bus connectivity (indices).
  * - R: Resistance ($$ R $$, p.u.).
  * - X: Reactance ($$ X $$, p.u.).
  * - G: Line conductance ($$ G $$, p.u.).
  * - B: Line susceptance ($$ B $$, p.u.).
  * - tapRatio: Transformer off-nominal tap ratio ($$ a $$, unitless).
  */
struct BranchData {
    Eigen::VectorXi From;      ///< From bus indices
    Eigen::VectorXi To;        ///< To bus indices
    Eigen::VectorXd R;         ///< Resistance ($$ R $$) [p.u.]
    Eigen::VectorXd X;         ///< Reactance ($$ X $$) [p.u.]
    Eigen::VectorXd G;         ///< Line conductance ($$ G $$) [p.u.]
    Eigen::VectorXd B;         ///< Line susceptance ($$ B $$) [p.u.]
    Eigen::VectorXd tapRatio;  ///< Transformer tap ratio ($$ a $$)
};

/**
  * @brief Reads branch data from a CSV file and fills a BranchData structure.
  * @param filename Path to the branch data CSV file.
  * @param branchData Reference to BranchData structure to populate.
  * @return true if the file was read and parsed successfully, false otherwise.
  */
bool readBranchDataCSV(const std::string& filename, BranchData& branchData);

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

#endif
