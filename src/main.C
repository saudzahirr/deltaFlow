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
 * @brief Main entry point for the deltaFlow (power flow analysis application).
 */

#include <cmath>
#include <complex>
#include <memory>
#include <vector>

#include "Admittance.H"
#include "Argparse.H"
#include "GaussSeidel.H"
#include "NewtonRaphson.H"
#include "Qlim.H"
#include "Logger.H"
#include "Reader.H"
#include "IEEE.H"
#include "PSSE.H"
#include "Writer.H"

int main(int argc, char* argv[]) {
    ArgumentParser args(argc, argv);
    DEBUG("Input file :: {}", args.getInputFile());
    DEBUG("Job name :: {}", args.getJobName());

    std::unique_ptr<Reader> reader;

    SolverType solver = args.getSolverType();
    InputFormat format = args.getInputFormat();
    int maxIter = args.getMaxIterations();
    double tolerance = args.getTolerance();

    switch (format) {
    case InputFormat::IEEE:
        reader = std::make_unique<IEEECommonDataFormat>();
        break;
    case InputFormat::PSSE:
        reader = std::make_unique<PSSERawFormat>();
        break;
    default:
        break;
    }

    reader->read(args.getInputFile());

    auto busData = reader->getBusData();
    auto branchData = reader->getBranchData();

    if (busData.ID.size() == 0 || branchData.From.size() == 0) {
        ERROR("No bus or branch data found in '{}'. Check the file exists and is valid.", args.getInputFile());
        std::exit(1);
    }

    int N = busData.ID.size();
    auto Y = computeAdmittanceMatrix(busData, branchData);

    // Extract G and B from Y_bus
    Eigen::MatrixXd G = Y.array().real().matrix();
    Eigen::MatrixXd B = Y.array().imag().matrix();

    // Flat start: PQ buses -> V=1.0, delta=0 for all; PV/Slack keep file voltage
    Eigen::VectorXd V(N);
    Eigen::VectorXd delta = Eigen::VectorXd::Zero(N);

    for (int i = 0; i < N; ++i) {
        if (busData.Type(i) == 3) {  // PQ bus
            V(i) = 1.0;
        } else {
            V(i) = busData.V(i);  // PV/Slack keep specified voltage
        }
    }

    // Working copy of bus types (modified during Q-limit loop)
    Eigen::VectorXi type_bus = busData.Type;

    switch (solver) {
        case SolverType::GaussSeidel: {
            double relaxation_coeff = args.getRelaxationCoefficient();

            bool Q_lim_status = true;

            while (Q_lim_status) {
                Eigen::VectorXd Ps = busData.Pg - busData.Pl;
                Eigen::VectorXd Qs = busData.Qg - busData.Ql;

                std::vector<int> pv_indices;
                for (int i = 0; i < N; ++i)
                    if (type_bus(i) == 2) pv_indices.push_back(i);

                bool converged = GaussSeidel(Y, V, delta, type_bus, Ps, Qs, N,
                                              maxIter, tolerance, relaxation_coeff);

                if (!converged) {
                    ERROR("Gauss-Seidel solver failed to converge.");
                    std::exit(1);
                }

                Q_lim_status = checkQlimits(V, delta, type_bus, G, B,
                                             busData, pv_indices, N);

                if (Q_lim_status)
                    DEBUG("Re-running Gauss-Seidel with updated bus types ...");
            }
            break;
        }

        case SolverType::NewtonRaphson:
        default: {
            // =====================================================
            // Outer Q-limit loop (follows MATLAB Qlim.m logic)
            // =====================================================
            bool Q_lim_status = true;

            while (Q_lim_status) {
                // Scheduled power injections (p.u.)
                Eigen::VectorXd Ps = busData.Pg - busData.Pl;
                Eigen::VectorXd Qs = busData.Qg - busData.Ql;

                // Identify PQ and PV buses from current types
                std::vector<int> pq_indices;
                std::vector<int> pv_indices;

                for (int i = 0; i < N; ++i) {
                    if (type_bus(i) == 3) pq_indices.push_back(i);
                    else if (type_bus(i) == 2) pv_indices.push_back(i);
                }

                int n_pq = static_cast<int>(pq_indices.size());

                // Run Newton-Raphson
                bool converged = NewtonRaphson(G, B, Ps, Qs, V, delta,
                    N, n_pq, pq_indices, maxIter, tolerance);

                if (!converged) {
                    ERROR("Newton-Raphson solver failed to converge.");
                    std::exit(1);
                }

                // Q-limit check after convergence
                Q_lim_status = checkQlimits(V, delta, type_bus, G, B,
                    busData, pv_indices, N);

                if (Q_lim_status) {
                    DEBUG("Re-running Newton-Raphson with updated bus types ...");
                    // Converged V and delta are used as initial guess for next round
                }
            }
            break;
        }
    }

    // =====================================================
    // Post-convergence: compute final power and update busData
    // =====================================================
    Eigen::VectorXcd Vc(N);
    for (int i = 0; i < N; ++i)
        Vc(i) = std::polar(V(i), delta(i));

    Eigen::VectorXd P_net = busData.Pg - busData.Pl;
    Eigen::VectorXd Q_net = busData.Qg - busData.Ql;

    // Recalculate slack bus power injection
    for (int i = 0; i < N; ++i) {
        if (busData.Type(i) == 1) {  // Slack
            std::complex<double> Ii = Y.row(i) * Vc;
            std::complex<double> Si = Vc(i) * std::conj(Ii);
            P_net(i) = Si.real();
            Q_net(i) = Si.imag();
        }
    }

    // Recalculate reactive power for PV buses
    for (int i = 0; i < N; ++i) {
        if (busData.Type(i) == 2) {  // PV
            std::complex<double> Ii = Y.row(i) * Vc;
            Q_net(i) = -std::imag(std::conj(Vc(i)) * Ii);
        }
    }

    // Update bus data with results
    for (int i = 0; i < N; ++i) {
        busData.V(i) = std::abs(Vc(i));
        busData.delta(i) = std::arg(Vc(i)) * 180.0 / M_PI;
        busData.Pg(i) = P_net(i) + busData.Pl(i);
        busData.Qg(i) = Q_net(i) + busData.Ql(i);
    }

    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    double QLoss = busData.Qg.sum() - busData.Ql.sum();
    DEBUG("Total real power loss: {}", PLoss);
    DEBUG("Total reactive power loss: {}", QLoss);

    dispBusData(busData);
    dispLineFlow(busData, branchData, Y);

    writeOutputCSV(busData);

    return 0;
}
