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
 * @brief Power mismatch computation for Newton-Raphson power flow analysis.
 *
 * Computes the active and reactive power at each bus and returns the mismatch
 * vector $$ [\Delta P; \Delta Q] $$ used in iterative solvers.
 *
 * Active power:
 * $$ P_i = \sum_{j=1}^{n} |V_i||V_j|(G_{ij}\cos(\delta_i - \delta_j) + B_{ij}\sin(\delta_i - \delta_j)) $$
 *
 * Reactive power:
 * $$ Q_i = \sum_{j=1}^{n} |V_i||V_j|(G_{ij}\sin(\delta_i - \delta_j) - B_{ij}\cos(\delta_i - \delta_j)) $$
 */

#ifndef POWER_MISMATCH_H
#define POWER_MISMATCH_H

#include <Eigen/Dense>
#include <vector>

/**
 * @brief Computes the power mismatch vector for use in Newton-Raphson iterations.
 *
 * The mismatch vector is composed of:
 * - $$ \Delta P $$ for all non-slack buses (buses 1..N-1, 0-indexed)
 * - $$ \Delta Q $$ for PQ buses only
 *
 * @param Ps Scheduled active power injection (Pg - Pl) at each bus [p.u.].
 * @param Qs Scheduled reactive power injection (Qg - Ql) at each bus [p.u.].
 * @param G Conductance matrix (real part of $$ Y_{bus} $$).
 * @param B Susceptance matrix (imaginary part of $$ Y_{bus} $$).
 * @param V Voltage magnitudes at each bus [p.u.].
 * @param delta Voltage angles at each bus [rad].
 * @param n_bus Total number of buses.
 * @param pq_bus_id 0-based indices of PQ buses.
 * @param P (output) Computed active power at each bus [p.u.].
 * @param Q (output) Computed reactive power at each bus [p.u.].
 * @return Mismatch vector $$ [\Delta P_{2..N}; \Delta Q_{PQ}] $$.
 */
Eigen::VectorXd powerMismatch(
    const Eigen::VectorXd& Ps,
    const Eigen::VectorXd& Qs,
    const Eigen::MatrixXd& G,
    const Eigen::MatrixXd& B,
    const Eigen::VectorXd& V,
    const Eigen::VectorXd& delta,
    int n_bus,
    const std::vector<int>& pq_bus_id,
    Eigen::VectorXd& P,
    Eigen::VectorXd& Q
);

#endif
