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
 * @brief Declaration of functions and classes for constructing the bus admittance matrix ($$ Y_{bus} $$) in power system analysis.
 *
 * This header provides the interface for computing the admittance matrix using bus and branch data.
 * The admittance matrix $$ Y_{bus} $$ is a fundamental element in load flow studies,
 * relating bus currents $$ I $$ and bus voltages $$ V $$ via the equation:
 *
 * $$ I = Y_{bus} V $$
 *
 * where:
 *   - $$ I $$ is the vector of injected currents,
 *   - $$ Y_{bus} $$ is the complex bus admittance matrix,
 *   - $$ V $$ is the vector of bus voltages.
 */

#ifndef ADMITTANCE_H
#define ADMITTANCE_H

#include <Eigen/Dense>

struct BranchData;
struct BusData;

/**
 * @brief Computes the complex bus admittance matrix ($$ Y_{bus} $$).
 *
 * This function generates the bus admittance matrix, which is essential in solving
 * power flow equations in electrical networks. The admittance matrix is computed
 * using the provided bus and branch data.
 *
 * @param busData Data representing the buses in the network.
 * @param branchData Data representing the branches (lines/transformers) in the network.
 * @return Eigen::MatrixXcd The computed $$ Y_{bus} $$ matrix as a complex matrix.
 */
Eigen::MatrixXcd computeAdmittanceMatrix(const BusData& busData, const BranchData& branchData);
 
#endif
