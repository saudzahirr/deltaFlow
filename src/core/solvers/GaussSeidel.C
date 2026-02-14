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
 * @brief Gauss-Seidel power flow solver implementation.
 */

#include <cmath>
#include <complex>
#include <iostream>
#include <limits>

#include "GaussSeidel.H"
#include "Logger.H"
#include "Progress.H"

bool GaussSeidel(
    const Eigen::MatrixXcd& Y,
    Eigen::VectorXd& Vmag,
    Eigen::VectorXd& delta,
    const Eigen::VectorXi& type_bus,
    const Eigen::VectorXd& P,
    const Eigen::VectorXd& Q,
    int N,
    int maxIter,
    double tolerance,
    double omega,
    std::vector<std::pair<int, double>>* iterHistory
) {
    // Store scheduled voltage magnitudes for PV buses
    Eigen::VectorXd Vmag_sched = Vmag;

    // Initialize complex voltage vector
    Eigen::VectorXcd V(N);
    for (int i = 0; i < N; ++i)
        V(i) = std::polar(Vmag(i), delta(i));

    int iteration = 0;
    double error = std::numeric_limits<double>::infinity();

    if (omega <= 0.0 || omega >= 2.0) {
        LOG_WARN("Invalid input: Relaxation coefficient must be between 0 and 2.");
        LOG_DEBUG("Setting Relaxation coefficient to 1.");
        omega = 1.0;
    }

    if (omega < 1.0) {
        LOG_CRITICAL("Under-relaxation enabled (omega < 1), this will slow down convergence.");
    }
    else if (omega == 1.0) {
        LOG_DEBUG("Standard Gauss-Seidel enabled (omega = 1).");
    }
    else if (omega > 1.0) {
        LOG_DEBUG("Over-relaxation enabled (omega > 1), this will accelerate convergence.");
    }

    LOG_DEBUG("Relaxation Coefficient :: {}", omega);

    while (error >= tolerance && iteration < maxIter) {
        Eigen::VectorXcd dV = Eigen::VectorXcd::Zero(N);

        for (int n = 0; n < N; ++n) {
            if (type_bus(n) == 1) continue;  // Skip slack bus

            std::complex<double> In = Y.row(n) * V;

            if (type_bus(n) == 2) {  // PV Bus
                // Compute Q from current solution (no clamping)
                double Qn = -std::imag(std::conj(V(n)) * In);

                // GS update maintaining scheduled voltage magnitude
                std::complex<double> I_excl = In - Y(n, n) * V(n);
                std::complex<double> V_updated = ((P(n) - std::complex<double>(0, 1) * Qn) / std::conj(V(n)) - I_excl) / Y(n, n);
                std::complex<double> V_corrected = Vmag_sched(n) * V_updated / std::abs(V_updated);
                dV(n) = V_corrected - V(n);
                V(n) = V_corrected;

            } else if (type_bus(n) == 3) {  // PQ Bus
                std::complex<double> I_excl = In - Y(n, n) * V(n);
                std::complex<double> V_updated = ((P(n) - std::complex<double>(0, 1) * Q(n)) / std::conj(V(n)) - I_excl) / Y(n, n);
                std::complex<double> V_relaxed = V(n) + omega * (V_updated - V(n));
                dV(n) = V_relaxed - V(n);
                V(n) = V_relaxed;
            }
        }

        error = dV.norm();
        iteration++;
        if (iterHistory) iterHistory->emplace_back(iteration, error);
        printIterationProgress("Gauss-Seidel", iteration, maxIter, error, tolerance);
    }

    if (iteration >= maxIter) {
        printConvergenceStatus("Gauss-Seidel", false, iteration, maxIter, error, tolerance);
        LOG_WARN("Gauss-Seidel did not converge within max iterations ({}).", maxIter);
        LOG_DEBUG("Final error norm was {:.6e}, tolerance is {:.6e}.", error, tolerance);
        return false;
    }

    // Extract converged V magnitudes and angles
    for (int i = 0; i < N; ++i) {
        Vmag(i) = std::abs(V(i));
        delta(i) = std::arg(V(i));
    }

    printConvergenceStatus("Gauss-Seidel", true, iteration, maxIter, error, tolerance);
    LOG_DEBUG("Gauss-Seidel converged in {} iterations with error norm {:.6e}.", iteration, error);

    return true;
}
