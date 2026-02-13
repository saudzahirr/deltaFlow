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
 * This file declares the ArgumentParser class for parsing command-line
 * options for power system analysis with deltaFlow.
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
enum class SolverType {
    GaussSeidel,    ///< Gauss-Seidel iterative method
    NewtonRaphson   ///< Newton-Raphson iterative method
};

enum class InputFormat {
  IEEE,
  PSSE
};

/**
  * @class ArgumentParser
  * @brief Parses and stores command-line arguments for deltaFlow.
  *
  * The ArgumentParser extracts command-line arguments related to the input CDF file, job name,
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
         * @brief Get the input CDF file path.
         * @return File path as std::string.
         */
        std::string getInputFile() const noexcept;

        /**
         * @brief Get the job name.
         * @return Job name as std::string.
         */
        std::string getJobName() const noexcept;

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
        SolverType getSolverType() const noexcept;

        InputFormat getInputFormat() const noexcept;

    private:
        std::string inputFile;        ///< Path to input CDF file
        std::string jobName;          ///< Job name (defaults to input filename)
        double tolerance = 1E-8;      ///< Convergence tolerance ($$ \epsilon $$)
        int maxIterations = 1024;     ///< Maximum number of iterations ($$ N_{max} $$)
        double relaxation = 1.0;      ///< Relaxation coefficient ($$ \omega $$)
        SolverType method;                ///< Solver type
        InputFormat format;

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
