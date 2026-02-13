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
 * @brief Jacobian matrix computation for Newton-Raphson power flow analysis.
 *
 * Constructs the Jacobian matrix with block structure:
 *
 * $$ J = \begin{bmatrix} J_{11} & J_{12} \\ J_{21} & J_{22} \end{bmatrix} $$
 *
 * where:
 * - $$ J_{11} $$: $$ \frac{\partial P}{\partial \delta} $$ for non-slack buses
 *
 * - $$ J_{12} $$: $$ \frac{\partial P}{\partial |V|} $$ for non-slack rows, PQ columns
 *
 * - $$ J_{21} $$: $$ \frac{\partial Q}{\partial \delta} $$ for PQ rows, non-slack columns
 *
 * - $$ J_{22} $$: $$ \frac{\partial Q}{\partial |V|} $$ for PQ rows and columns
 *
 * Follows the formulation by B. Sereeter, C. Vuik, and C. Witteveen (REPORT 17-07).
 */

#ifndef JACOBIAN_H
#define JACOBIAN_H

#include <Eigen/Dense>
#include <vector>

/**
 * @brief Computes the Jacobian matrix for the Newton-Raphson power flow solver.
 *
 * @param V Voltage magnitudes at each bus [p.u.].
 * @param delta Voltage angles at each bus [rad].
 * @param n_bus Total number of buses.
 * @param n_pq Number of PQ buses.
 * @param pq_bus_id 0-based indices of PQ buses.
 * @param G Conductance matrix (real part of $$ Y_{bus} $$).
 * @param B Susceptance matrix (imaginary part of $$ Y_{bus} $$).
 * @param P Computed active power at each bus (from power mismatch) [p.u.].
 * @param Q Computed reactive power at each bus (from power mismatch) [p.u.].
 * @return The assembled Jacobian matrix.
 */
Eigen::MatrixXd computeJacobian(
    const Eigen::VectorXd& V,
    const Eigen::VectorXd& delta,
    int n_bus,
    int n_pq,
    const std::vector<int>& pq_bus_id,
    const Eigen::MatrixXd& G,
    const Eigen::MatrixXd& B,
    const Eigen::VectorXd& P,
    const Eigen::VectorXd& Q
);

#endif
