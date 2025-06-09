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
 * @brief Command-line argument parsing utilities for deltaFlow.
 *
 * This file declares the ArgumentParser class for parsing command-line options for power system analysis with deltaFlow.
 * Supported options include selecting the solver (Gauss-Seidel or Newton-Raphson), file names for bus and branch data,
 * convergence tolerance, maximum iterations, and relaxation coefficient.
 */

#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <string>

/**
  * @enum Solver
  * @brief Types of solvers supported by deltaFlow.
  *
  * - GaussSeidel: Gauss-Seidel iterative method.
  * - NewtonRaphson: Newton-Raphson iterative method.
  */
enum class Solver {
    GaussSeidel,    ///< Gauss-Seidel iterative method
    NewtonRaphson   ///< Newton-Raphson iterative method
};

/**
  * @class ArgumentParser
  * @brief Parses and stores command-line arguments for deltaFlow.
  *
  * The ArgumentParser extracts command-line arguments related to bus/branch data files,
  * solver selection, convergence tolerance ($$ \epsilon $$), maximum iterations ($$ N_{max} $$), and relaxation coefficient ($$ \omega $$).
  */
class ArgumentParser final {
    public:
        /**
         * @brief Constructor: parses command-line arguments.
         * @param argc Argument count.
         * @param argv Argument vector.
         */
        ArgumentParser(int argc, char* argv[]);

        /**
         * @brief Destructor (default).
         */
        ~ArgumentParser() = default;

        // Deleted copy/move semantics
        ArgumentParser(const ArgumentParser&) = delete;
        ArgumentParser& operator=(const ArgumentParser&) = delete;
        ArgumentParser(const ArgumentParser&&) = delete;
        ArgumentParser& operator=(ArgumentParser&&) = delete;

        /**
         * @brief Get the bus data CSV file name.
         * @return File path as std::string.
         */
        std::string getBusDataCsv() const noexcept;

        /**
         * @brief Get the branch data CSV file name.
         * @return File path as std::string.
         */
        std::string getBranchDataCsv() const noexcept;

        /**
         * @brief Get the convergence tolerance ($$ \epsilon $$).
         * @return Tolerance as double.
         */
        double getTolerance() const noexcept;

        /**
         * @brief Get the maximum number of iterations ($$ N_{max} $$).
         * @return Maximum iterations as int.
         */
        int getMaxIterations() const noexcept;

        /**
         * @brief Get the relaxation coefficient ($$ \omega $$).
         * @return Relaxation coefficient as double.
         */
        double getRelaxationCoefficient() const noexcept;

        /**
         * @brief Get the solver type.
         * @return Solver enum (GaussSeidel or NewtonRaphson).
         */
        Solver getSolver() const noexcept;

    private:
        std::string busDataCsv;       ///< Path to bus data CSV file
        std::string branchDataCsv;    ///< Path to branch data CSV file
        double tolerance = 1E-8;      ///< Convergence tolerance ($$ \epsilon $$)
        int maxIterations = 1024;     ///< Maximum number of iterations ($$ N_{max} $$)
        double relaxation = 1.0;      ///< Relaxation coefficient ($$ \omega $$)
        Solver method;                ///< Solver type

        /**
         * @brief Parse the provided arguments.
         * @param argc Argument count.
         * @param argv Argument vector.
         */
        void parse_args(int argc, char* argv[]);

        /**
         * @brief Print help message to stdout.
         */
        void help() const noexcept;

        /**
         * @brief Print version information to stdout.
         */
        void version() const noexcept;
};

#endif
