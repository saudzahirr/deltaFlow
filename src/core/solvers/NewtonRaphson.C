#include <cmath>
#include <limits>
#include <iostream>
#include <vector>

#include "Logger.H"
#include "NewtonRaphson.H"
#include "Data.H"
#include "Utils.H"

bool NewtonRaphson(const Eigen::MatrixXcd& Y, BusData& busData, int maxIter, double tolerance) {
    int N = Y.rows();
    Eigen::VectorXi& type_bus = busData.Type;  // non-const because we modify it on PV→PQ conversion

    Eigen::VectorXd Vmag = busData.V;
    Eigen::VectorXd delta = busData.delta * M_PI / 180.0;  // degrees to radians

    Eigen::VectorXd Pg = busData.Pg;
    Eigen::VectorXd Qg = busData.Qg;
    const Eigen::VectorXd& Pl = busData.Pl;
    const Eigen::VectorXd& Ql = busData.Ql;
    const Eigen::VectorXd& Qmax = busData.Qgmax;
    const Eigen::VectorXd& Qmin = busData.Qgmin;

    int iteration = 0;
    double error = std::numeric_limits<double>::infinity();

    Eigen::VectorXd P_net = Pg - Pl;
    Eigen::VectorXd Q_net = Qg - Ql;

    // Precompute admittance magnitude and angle
    Eigen::MatrixXd Ymag = Y.cwiseAbs();
    Eigen::MatrixXd theta = Y.cwiseArg();

    while (error > tolerance && iteration < maxIter) {
        Eigen::VectorXd P_estimate = Eigen::VectorXd::Zero(N);
        Eigen::VectorXd Q_estimate = Eigen::VectorXd::Zero(N);

        // Calculate P and Q at all buses except slack (bus 0)
        for (int i = 1; i < N; ++i) {
            for (int n = 0; n < N; ++n) {
                double angle_diff = theta(i, n) + delta(n) - delta(i);
                P_estimate(i) += Vmag(i) * Vmag(n) * Ymag(i, n) * std::cos(angle_diff);
                Q_estimate(i) -= Vmag(i) * Vmag(n) * Ymag(i, n) * std::sin(angle_diff);
            }

            // PV to PQ conversion based on Q limits
            if (Qmax(i) != 0.0) {
                if (Q_estimate(i) > Qmax(i)) {
                    Q_estimate(i) = Qmax(i);
                    type_bus(i) = 3;  // PV bus converted to PQ bus
                } else if (Q_estimate(i) < Qmin(i)) {
                    Q_estimate(i) = Qmin(i);
                    type_bus(i) = 3;  // PV bus converted to PQ bus
                } else {
                    type_bus(i) = 2;  // keep as PV bus
                    Vmag(i) = busData.V(i);  // voltage magnitude fixed for PV buses
                }
            }
        }

        // Rebuild index lists after possible PV→PQ conversion
        std::vector<int> pq_indices;
        for (int i = 0; i < N; ++i)
            if (type_bus(i) == 3) pq_indices.push_back(i);

        std::vector<int> pv_pq_indices; // all except slack bus (type 1)
        for (int i = 1; i < N; ++i)
            if (type_bus(i) != 1) pv_pq_indices.push_back(i);

        // Compute mismatches dP (exclude slack bus)
        Eigen::VectorXd dP(pv_pq_indices.size());
        for (std::size_t i = 0; i < pv_pq_indices.size(); ++i)
            dP(i) = P_net(pv_pq_indices[i]) - P_estimate(pv_pq_indices[i]);

        // Compute mismatches dQ for PQ buses only
        Eigen::VectorXd dQ(pq_indices.size());
        for (std::size_t i = 0; i < pq_indices.size(); ++i)
            dQ(i) = Q_net(pq_indices[i]) - Q_estimate(pq_indices[i]);

        // Construct full Jacobian matrices J1, J2, J3, J4 (size N×N)
        Eigen::MatrixXd J1 = Eigen::MatrixXd::Zero(N, N);
        Eigen::MatrixXd J2 = Eigen::MatrixXd::Zero(N, N);
        Eigen::MatrixXd J3 = Eigen::MatrixXd::Zero(N, N);
        Eigen::MatrixXd J4 = Eigen::MatrixXd::Zero(N, N);

        for (int i = 0; i < N; ++i) {
            for (int n = 0; n < N; ++n) {
                double angle_diff = theta(i, n) + delta(n) - delta(i);
                if (i != n) {
                    // Off-diagonal elements
                    J1(i, i) += Vmag(i) * Vmag(n) * Ymag(i, n) * std::sin(angle_diff);
                    J1(i, n) = -Vmag(i) * Vmag(n) * Ymag(i, n) * std::sin(angle_diff);

                    J2(i, i) += Vmag(n) * Ymag(i, n) * std::cos(angle_diff);
                    J2(i, n) = Vmag(i) * Ymag(i, n) * std::cos(angle_diff);

                    J3(i, i) += Vmag(i) * Vmag(n) * Ymag(i, n) * std::cos(angle_diff);
                    J3(i, n) = -Vmag(i) * Vmag(n) * Ymag(i, n) * std::cos(angle_diff);

                    J4(i, i) -= Vmag(n) * Ymag(i, n) * std::sin(angle_diff);
                    J4(i, n) = -Vmag(i) * Ymag(i, n) * std::sin(angle_diff);
                } else {
                    // Diagonal elements
                    J2(i, i) += 2.0 * Vmag(i) * Ymag(i, i) * std::cos(theta(i, i));
                    J4(i, i) -= 2.0 * Vmag(i) * Ymag(i, i) * std::sin(theta(i, i));
                }
            }
        }

        // Zero rows and columns in Jacobian related to slack bus to avoid singularities
        // Slack bus index assumed to be 0
        for (int i = 0; i < N; ++i) {
            J1(0, i) = 0.0;
            J1(i, 0) = 0.0;
            J2(0, i) = 0.0;
            J2(i, 0) = 0.0;
            J3(0, i) = 0.0;
            J3(i, 0) = 0.0;
            J4(0, i) = 0.0;
            J4(i, 0) = 0.0;
        }

        // Build reduced Jacobian matrices for the unknown variables
        Eigen::MatrixXd J11(pv_pq_indices.size(), pv_pq_indices.size());
        Eigen::MatrixXd J12(pv_pq_indices.size(), pq_indices.size());
        Eigen::MatrixXd J21(pq_indices.size(), pv_pq_indices.size());
        Eigen::MatrixXd J22(pq_indices.size(), pq_indices.size());

        for (std::size_t i = 0; i < pv_pq_indices.size(); ++i) {
            for (std::size_t j = 0; j < pv_pq_indices.size(); ++j) {
                J11(i, j) = J1(pv_pq_indices[i], pv_pq_indices[j]);
            }
            for (std::size_t j = 0; j < pq_indices.size(); ++j) {
                J12(i, j) = J2(pv_pq_indices[i], pq_indices[j]);
            }
        }

        for (std::size_t i = 0; i < pq_indices.size(); ++i) {
            for (std::size_t j = 0; j < pv_pq_indices.size(); ++j) {
                J21(i, j) = J3(pq_indices[i], pv_pq_indices[j]);
            }
            for (std::size_t j = 0; j < pq_indices.size(); ++j) {
                J22(i, j) = J4(pq_indices[i], pq_indices[j]);
            }
        }

        // Assemble full Jacobian matrix
        Eigen::MatrixXd J(J11.rows() + J21.rows(), J11.cols() + J12.cols());
        J << J11, J12,
             J21, J22;

        // Assemble mismatch vector (dP followed by dQ)
        Eigen::VectorXd mismatch(J.rows());
        mismatch << dP, dQ;

        // Solve for state updates Δx = [Δδ; ΔV]
        Eigen::VectorXd DX = J.colPivHouseholderQr().solve(mismatch);

        // Update angles (δ) and voltage magnitudes (V)
        for (std::size_t i = 0; i < pv_pq_indices.size(); ++i) {
            delta(pv_pq_indices[i]) += DX(i);
        }
        for (std::size_t i = 0; i < pq_indices.size(); ++i) {
            Vmag(pq_indices[i]) += DX(pv_pq_indices.size() + i);
        }

        // Compute error as max absolute mismatch (infinity norm)
        error = mismatch.cwiseAbs().maxCoeff();
        iteration++;
    }

    if (iteration >= maxIter) {
        WARN("Newton-Raphson did not converge within the maximum iterations ({}).", maxIter);
        DEBUG("Final error norm was {:.6e}, tolerance is {:.6e}.", error, tolerance);
        return false;
    }

    Eigen::VectorXcd V(N);
    for (int i = 0; i < N; ++i)
        V(i) = std::polar(Vmag(i), delta(i));

    // Recalculate slack bus power injection
    std::complex<double> I_slack = Y.row(0) * V;
    std::complex<double> S_slack = V(0) * std::conj(I_slack);
    P_net(0) = S_slack.real();
    Q_net(0) = S_slack.imag();

    // Recalculate reactive power for PV buses
    for (int i = 0; i < N; ++i) {
        if (type_bus(i) == 2) {
            std::complex<double> In = Y.row(i) * V;
            Q_net(i) = -std::imag(std::conj(V(i)) * In);
        }
    }

    // Update bus data: V magnitude, angle (degrees), Pg
    for (int i = 0; i < N; ++i) {
        busData.V(i) = std::abs(V(i));
        busData.delta(i) = std::arg(V(i)) * 180.0 / M_PI;
        busData.Pg(i) = P_net(i) + Pl(i);
        busData.Qg(i) = Q_net(i) + Ql(i);
    }

    // Calculate total power losses in the system
    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    double QLoss = busData.Qg.sum() - busData.Ql.sum();

    DEBUG("Total real power loss: {}", PLoss);
    DEBUG("Total reactive power loss: {}", QLoss);

    DEBUG("Newton-Raphson converged in {} iterations with max mismatch error {}", iteration, error);

    return true;
}
