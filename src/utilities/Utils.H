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
 * @brief Utility functions and helpers for deltaFlow.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iomanip>
#include <Eigen/Dense>
#include "Define.H"
#include "Logger.H"

/**
  * @namespace Utilities
  * @brief Collection of utility functions for string and matrix operations.
  */
namespace Utilities {

    /**
      * @brief Check if a string ends with a given suffix.
      * @param str The input string.
      * @param suffix The suffix to check.
      * @return True if str ends with suffix, false otherwise.
      */
    inline bool endswith(const std::string& str, const std::string& suffix) {
        if (suffix.size() > str.size()) return false;
        return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    /**
      * @brief Converts a string to uppercase in-place.
      * @param str The string to convert.
      */
    inline void toupper(std::string& str) {
        forEach(c, str) {
            c = std::toupper(static_cast<unsigned char>(c));
        }
    }

    /**
      * @brief Display a complex Eigen matrix with formatted real and imaginary parts.
      * @param Y The complex matrix to display.
      * @param width The width of each element (default 22).
      * @param left_justify If true, left justifies output (default false).
      */
    inline void disp(const Eigen::MatrixXcd& Y, int width = 22, bool left_justify = false) {
        for (int i = 0; i < Y.rows(); ++i) {
            std::stringstream rowStream;
            for (int j = 0; j < Y.cols(); ++j) {
                auto R = Y(i, j).real();
                auto X = Y(i, j).imag();

                std::stringstream cell;
                if (X >= 0) {
                    cell << "(" << R << " + " << X << "i)";
                } else {
                    cell << "(" << R << " - " << -X << "i)";
                }

                if (left_justify)
                    rowStream << std::left;
                else
                    rowStream << std::right;

                rowStream << std::setw(width) << cell.str();

                if (j != Y.cols() - 1)
                    rowStream << "\t";
            }
            DEBUG("{}", rowStream.str().c_str());
        }
    }

    /**
      * @brief Display a real Eigen matrix with formatting.
      * @param J The real matrix to display.
      * @param width The width of each element (default 22).
      * @param left_justify If true, left justifies output (default false).
      */
    inline void disp(const Eigen::MatrixXd& J, int width = 22, bool left_justify = false) {
        for (int i = 0; i < J.rows(); ++i) {
            std::stringstream rowStream;
            for (int j = 0; j < J.cols(); ++j) {
                std::stringstream cell;
                cell << J(i, j);

                if (left_justify)
                    rowStream << std::left;
                else
                    rowStream << std::right;

                rowStream << std::setw(width) << cell.str();

                if (j != J.cols() - 1)
                    rowStream << "\t";
            }
            DEBUG("{}", rowStream.str().c_str());
        }
    }

    /**
      * @brief Display a complex Eigen vector with formatting.
      * @param vec The complex vector to display.
      * @param width The width of each element (default 22).
      * @param left_justify If true, left justifies output (default false).
      */
    inline void disp(const Eigen::VectorXcd& vec, int width = 22, bool left_justify = false) {
        std::stringstream rowStream;
        for (int i = 0; i < vec.size(); ++i) {
            auto R = vec(i).real();
            auto X = vec(i).imag();

            std::stringstream cell;
            if (X >= 0)
                cell << "(" << R << " + " << X << "i)";
            else
                cell << "(" << R << " - " << -X << "i)";

            if (left_justify)
                rowStream << std::left;
            else
                rowStream << std::right;

            rowStream << std::setw(width) << cell.str();

            if (i != vec.size() - 1)
                rowStream << "\t";
        }
        DEBUG("{}", rowStream.str().c_str());
    }

    /**
      * @brief Display a real Eigen vector with formatting.
      * @param vec The real vector to display.
      * @param width The width of each element (default 22).
      * @param left_justify If true, left justifies output (default false).
      */
    inline void disp(const Eigen::VectorXd& vec, int width = 22, bool left_justify = false) {
        std::stringstream rowStream;
        for (int i = 0; i < vec.size(); ++i) {
            std::stringstream cell;
            cell << vec(i);

            if (left_justify)
                rowStream << std::left;
            else
                rowStream << std::right;

            rowStream << std::setw(width) << cell.str();

            if (i != vec.size() - 1)
                rowStream << "\t";
        }
        DEBUG("{}", rowStream.str().c_str());
    }

}

#endif
