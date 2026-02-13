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

#include <complex>

#include "Admittance.H"
#include "Logger.H"
#include "Data.H"

Eigen::MatrixXcd computeAdmittanceMatrix(const BusData& busData, const BranchData& branchData) {
    int nLine = branchData.From.size();
    int N = std::max(branchData.From.maxCoeff(), branchData.To.maxCoeff());  // 1-based bus indexing

    Eigen::MatrixXcd Ybus = Eigen::MatrixXcd::Zero(N, N);  // Size N x N, 0-based indexing internally

    for (int k = 0; k < nLine; ++k) {
        int from = branchData.From(k) - 1;  // Convert to 0-based
        int to   = branchData.To(k) - 1;    // Convert to 0-based

        std::complex<double> Z(branchData.R(k), branchData.X(k));
        std::complex<double> Y = 1.0 / Z;

        std::complex<double> B(0.0, 0.5 * branchData.B(k));

        double a = branchData.tapRatio(k);
        if (a == 0.0) {
            a = 1.0;
        }

        // Off-diagonal
        Ybus(from, to) -= Y / a;
        Ybus(to, from) = Ybus(from, to);  // Symmetric

        // Diagonal
        Ybus(from, from) += Y / (a * a) + B;
        Ybus(to, to)     += Y + B;
    }

    // Add shunt admittances
    int nBuses = busData.ID.size();  // Should match Gs and Bs size

    for (int n = 0; n < nBuses; ++n) {
        int busIndex = busData.ID(n) - 1;  // Convert to 0-based

        if (busIndex < 0 || busIndex >= Ybus.rows()) {
            ERROR("Warning: Bus ID {} out of bounds in Ybus", busIndex + 1);
            continue;
        }

        std::complex<double> Y_shunt(busData.Gs(n), busData.Bs(n));
        Ybus(busIndex, busIndex) += Y_shunt;
    }

    return Ybus;
}
