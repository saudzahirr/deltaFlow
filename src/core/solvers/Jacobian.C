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

#include <cmath>

#include "Jacobian.H"

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
) {
    // J11: (n_bus-1) x (n_bus-1) - dP/ddelta, non-slack buses
    Eigen::MatrixXd J11 = Eigen::MatrixXd::Zero(n_bus - 1, n_bus - 1);

    for (int i = 1; i < n_bus; ++i) {
        for (int k = 1; k < n_bus; ++k) {
            if (i == k) {
                J11(i - 1, k - 1) = -Q(i) - V(i) * V(i) * B(i, i);
            } else {
                double dik = delta(i) - delta(k);
                J11(i - 1, k - 1) = V(i) * V(k) * (G(i, k) * std::sin(dik)
                    - B(i, k) * std::cos(dik));
            }
        }
    }

    // J21: n_pq x (n_bus-1) - dQ/ddelta, PQ rows, non-slack columns
    Eigen::MatrixXd J21 = Eigen::MatrixXd::Zero(n_pq, n_bus - 1);

    for (int i = 0; i < n_pq; ++i) {
        int j = pq_bus_id[i];
        for (int k = 1; k < n_bus; ++k) {
            if (j == k) {
                J21(i, k - 1) = P(j) - V(j) * V(j) * G(j, j);
            } else {
                double djk = delta(j) - delta(k);
                J21(i, k - 1) = -V(j) * V(k) * (G(j, k) * std::cos(djk)
                    + B(j, k) * std::sin(djk));
            }
        }
    }

    // J12: (n_bus-1) x n_pq - dP/dV, non-slack rows, PQ columns
    Eigen::MatrixXd J12 = Eigen::MatrixXd::Zero(n_bus - 1, n_pq);

    for (int i = 1; i < n_bus; ++i) {
        for (int k = 0; k < n_pq; ++k) {
            int j = pq_bus_id[k];
            if (i == j) {
                J12(i - 1, k) = P(j) / V(j) + V(j) * G(j, j);
            } else {
                double dij = delta(i) - delta(j);
                J12(i - 1, k) = V(i) * (G(i, j) * std::cos(dij)
                    + B(i, j) * std::sin(dij));
            }
        }
    }

    // J22: n_pq x n_pq - dQ/dV, PQ rows and columns
    Eigen::MatrixXd J22 = Eigen::MatrixXd::Zero(n_pq, n_pq);

    for (int i = 0; i < n_pq; ++i) {
        int j = pq_bus_id[i];
        for (int k = 0; k < n_pq; ++k) {
            int l = pq_bus_id[k];
            if (j == l) {
                J22(i, k) = Q(j) / V(j) - V(j) * B(j, j);
            } else {
                double djl = delta(j) - delta(l);
                J22(i, k) = V(j) * (G(j, l) * std::sin(djl)
                    - B(j, l) * std::cos(djl));
            }
        }
    }

    // Assemble full Jacobian: [J11 J12; J21 J22]
    int dim = (n_bus - 1) + n_pq;
    Eigen::MatrixXd J(dim, dim);
    J.topLeftCorner(n_bus - 1, n_bus - 1) = J11;
    J.topRightCorner(n_bus - 1, n_pq) = J12;
    J.bottomLeftCorner(n_pq, n_bus - 1) = J21;
    J.bottomRightCorner(n_pq, n_pq) = J22;

    return J;
}
