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
 * @brief Reactive power limit (Q-limit) checking for PV buses.
 *
 * After power flow convergence, this function checks whether any PV bus
 * has exceeded its reactive power generation limits ($$ Q_{gmax} $$ or $$ Q_{gmin} $$).
 * If a violation is detected, the PV bus is converted to a PQ bus and the solver
 * must be re-run with the updated bus types.
 *
 * This follows the standard approach described in the MATLAB Qlim.m reference.
 */

#ifndef QLIM_H
#define QLIM_H

#include <Eigen/Dense>

class BusData;

/**
 * @brief Checks reactive power limits on PV buses after solver convergence.
 *
 * Computes the reactive power at each bus using converged voltages and angles,
 * then checks if PV buses violate their $$ Q_{gmax} $$ or $$ Q_{gmin} $$ limits.
 * Violating PV buses are converted to PQ type (type 3 in C++ convention).
 *
 * @param V Converged voltage magnitudes [p.u.].
 * @param delta Converged voltage angles [rad].
 * @param type_bus (in/out) Bus type vector; PV buses that violate limits are set to PQ.
 * @param G Conductance matrix.
 * @param B Susceptance matrix.
 * @param busData Bus data (for Ql, Qgmax, Qgmin).
 * @param pv_bus_id 0-based indices of PV buses (before any conversion).
 * @param n_bus Total number of buses.
 * @return true if any Q-limit was hit (solver must be re-run), false otherwise.
 */
bool checkQlimits(
    const Eigen::VectorXd& V,
    const Eigen::VectorXd& delta,
    Eigen::VectorXi& type_bus,
    const Eigen::MatrixXd& G,
    const Eigen::MatrixXd& B,
    BusData& busData,
    const std::vector<int>& pv_bus_id,
    int n_bus
);

#endif
