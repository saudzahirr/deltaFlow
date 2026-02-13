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
 * @brief Progress bar for iterative solvers.
 *
 * Provides a colored terminal progress bar that shows solver convergence
 * status in real-time. Green fill indicates progress; red indicates failure.
 */

#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <fmt/color.h>
#include <fmt/core.h>
#include <string>

/**
 * @brief Print an iteration progress line for a solver.
 *
 * Displays a colored progress bar with iteration count and error magnitude.
 * - Green bar: proportional to iterations completed out of maxIter
 * - Red bar at failure point when solver diverges
 *
 * @param solver   Solver name (e.g. "Newton-Raphson", "Gauss-Seidel")
 * @param iter     Current iteration number (1-based)
 * @param maxIter  Maximum allowed iterations
 * @param error    Current mismatch/error value
 * @param tol      Convergence tolerance
 * @param barWidth Width of the progress bar in characters (default: 50)
 */
inline void printIterationProgress(
    const std::string& solver,
    int iter,
    int maxIter,
    double error,
    double tol,
    int barWidth = 50
) {
    float ratio = maxIter == 0 ? 0.0f : static_cast<float>(iter) / maxIter;
    int filled = static_cast<int>(ratio * barWidth);

    // Move cursor up to overwrite previous line (except on first iteration)
    if (iter > 1)
        fmt::print("\033[1F\033[2K");

    // Solver label
    fmt::print(fg(fmt::color::cyan) | fmt::emphasis::bold, "{:<16}", solver);

    // Progress bar
    for (int i = 0; i < filled; ++i)
        fmt::print(bg(fmt::color::green), " ");
    for (int i = filled; i < barWidth; ++i)
        fmt::print(bg(fmt::color::dark_gray), " ");

    // Reset colors
    fmt::print("\033[0m");

    // Stats
    fmt::print("  iter ");
    fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, "{:>4}", iter);
    fmt::print(fg(fmt::color::white), "/{:<4}", maxIter);
    fmt::print("  error ");
    fmt::print(fg(fmt::color::yellow), "{:.6e}", error);
    fmt::print(fg(fmt::color::white), "  tol {:.0e}\n", tol);
}

/**
 * @brief Print the final convergence status line.
 *
 * Overwrites the progress bar with a final status:
 * - Green bar (partial) + "CONVERGED" if converged
 * - Red bar at failure point + "FAILED" if not converged
 *
 * @param solver    Solver name
 * @param converged Whether the solver converged
 * @param iter      Final iteration count
 * @param maxIter   Maximum allowed iterations
 * @param error     Final mismatch/error value
 * @param tol       Convergence tolerance
 * @param barWidth  Width of the progress bar in characters (default: 50)
 */
inline void printConvergenceStatus(
    const std::string& solver,
    bool converged,
    int iter,
    int maxIter,
    double error,
    double tol,
    int barWidth = 50
) {
    // Overwrite the last progress line
    fmt::print("\033[1F\033[2K");

    float ratio = maxIter == 0 ? 0.0f : static_cast<float>(iter) / maxIter;
    int filled = static_cast<int>(ratio * barWidth);

    // Solver label
    fmt::print(fg(fmt::color::cyan) | fmt::emphasis::bold, "{:<16}", solver);

    if (converged) {
        // Green bar up to convergence point, gray for the rest
        for (int i = 0; i < filled; ++i)
            fmt::print(bg(fmt::color::green), " ");
        for (int i = filled; i < barWidth; ++i)
            fmt::print(bg(fmt::color::dark_gray), " ");
        fmt::print("\033[0m");

        fmt::print("  ");
        fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "CONVERGED");
        fmt::print(fg(fmt::color::white), " in ");
        fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, "{}", iter);
        fmt::print(fg(fmt::color::white), " iterations");
        fmt::print(fg(fmt::color::white), "  error {:.6e}\n", error);
    } else {
        // Red bar up to failure point, dark gray for the rest
        for (int i = 0; i < filled; ++i)
            fmt::print(bg(fmt::color::red), " ");
        for (int i = filled; i < barWidth; ++i)
            fmt::print(bg(fmt::color::dark_gray), " ");
        fmt::print("\033[0m");

        fmt::print("  ");
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "FAILED");
        fmt::print(fg(fmt::color::white), " after ");
        fmt::print(fg(fmt::color::yellow) | fmt::emphasis::bold, "{}", iter);
        fmt::print(fg(fmt::color::white), " iterations");
        fmt::print(fg(fmt::color::white), "  error {:.6e}\n", error);
    }
}

#endif // PROGRESS_BAR_H
