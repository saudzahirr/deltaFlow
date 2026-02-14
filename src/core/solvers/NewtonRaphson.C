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
 * @brief Newton-Raphson power flow solver implementation.
 */

#include <cmath>
#include <limits>
#include <iostream>
#include <vector>

#include "Logger.H"
#include "NewtonRaphson.H"
#include "Jacobian.H"
#include "PowerMismatch.H"
#include "Progress.H"
#include "Data.H"
#include "Utils.H"

bool NewtonRaphson(
    const Eigen::MatrixXd& G,
    const Eigen::MatrixXd& B,
    const Eigen::VectorXd& Ps,
    const Eigen::VectorXd& Qs,
    Eigen::VectorXd& V,
    Eigen::VectorXd& delta,
    int n_bus,
    int n_pq,
    const std::vector<int>& pq_bus_id,
    int maxIter,
    double tolerance,
    std::vector<std::pair<int, double>>* iterHistory
) {
    // Compute initial mismatch
    Eigen::VectorXd P(n_bus), Q(n_bus);
    Eigen::VectorXd mismatch = powerMismatch(Ps, Qs, G, B, V, delta, n_bus, pq_bus_id, P, Q);

    double error = mismatch.cwiseAbs().maxCoeff();
    int iter = 0;

    if (iterHistory) {
        iterHistory->clear();
        iterHistory->emplace_back(0, error);
    }

    while (error >= tolerance) {
        if (iter >= maxIter) {
            printConvergenceStatus("Newton-Raphson", false, iter, maxIter, error, tolerance);
            LOG_WARN("Newton-Raphson did not converge within {} iterations.", maxIter);
            LOG_DEBUG("Final max mismatch was {:.6e}, tolerance is {:.6e}.", error, tolerance);
            return false;
        }
        iter++;

        // Build Jacobian
        Eigen::MatrixXd J = computeJacobian(V, delta, n_bus, n_pq, pq_bus_id, G, B, P, Q);

        // Solve J * correction = mismatch
        Eigen::VectorXd correction = J.colPivHouseholderQr().solve(mismatch);

        // Update delta for non-slack buses (indices 1..N-1)
        for (int i = 1; i < n_bus; ++i) {
            delta(i) += correction(i - 1);
        }

        // Update V for PQ buses only
        for (int k = 0; k < n_pq; ++k) {
            V(pq_bus_id[k]) += correction(n_bus - 1 + k);
        }

        // Recompute mismatch
        mismatch = powerMismatch(Ps, Qs, G, B, V, delta, n_bus, pq_bus_id, P, Q);

        error = mismatch.cwiseAbs().maxCoeff();
        if (iterHistory) iterHistory->emplace_back(iter, error);
        printIterationProgress("Newton-Raphson", iter, maxIter, error, tolerance);
        LOG_DEBUG("NR iteration {}: max mismatch = {:.16e}", iter, error);
    }

    printConvergenceStatus("Newton-Raphson", true, iter, maxIter, error, tolerance);
    LOG_DEBUG("Newton-Raphson converged in {} iterations with max mismatch {:.6e}", iter, error);
    return true;
}
