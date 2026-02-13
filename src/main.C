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
 * @brief Main entry point for the deltaFlow (power flow analysis application).
 */

#include <chrono>
#include <cmath>
#include <complex>
#include <memory>
#include <utility>
#include <vector>

#include "Admittance.H"
#include "Argparse.H"
#include "Banner.H"
#include "GaussSeidel.H"
#include "IEEE.H"
#include "Logger.H"
#include "NewtonRaphson.H"
#include "OutputFile.H"
#include "PSSE.H"
#include "Qlim.H"
#include "Reader.H"
#include "Writer.H"

int main(int argc, char* argv[]) {
    Banner::printTerminalBanner();

    auto startTime = std::chrono::high_resolution_clock::now();

    ArgumentParser args(argc, argv);

    std::string jobName = args.getJobName();
    std::string inputFile = args.getInputFile();
    SolverType solver = args.getSolverType();
    InputFormat format = args.getInputFormat();
    int maxIter = args.getMaxIterations();
    double tolerance = args.getTolerance();

    std::string solverName = (solver == SolverType::GaussSeidel) ? "Gauss-Seidel" : "Newton-Raphson";
    std::string formatName = (format == InputFormat::IEEE) ? "IEEE Common Data Format" : "PSS/E Raw Format";

    DEBUG("Job name     :: {}", jobName);
    DEBUG("Input file   :: {}", inputFile);
    DEBUG("Input format :: {}", formatName);
    DEBUG("Solver       :: {}", solverName);
    DEBUG("Tolerance    :: {:.6e}", tolerance);
    DEBUG("Max iter     :: {}", maxIter);

    std::unique_ptr<Reader> reader;

    switch (format) {
    case InputFormat::IEEE:
        reader = std::make_unique<IEEECommonDataFormat>();
        INFO("Reading IEEE Common Data Format file: {}", inputFile);
        break;
    case InputFormat::PSSE:
        reader = std::make_unique<PSSERawFormat>();
        INFO("Reading PSS/E Raw Format file: {}", inputFile);
        break;
    default:
        break;
    }

    reader->read(inputFile);

    auto busData = reader->getBusData();
    auto branchData = reader->getBranchData();

    if (busData.ID.size() == 0 || branchData.From.size() == 0) {
        ERROR("No bus or branch data found in '{}'. Check the file exists and is valid.", inputFile);
        std::exit(1);
    }

    int N = busData.ID.size();
    int nBranch = branchData.From.size();

    INFO("Model: {} buses, {} branches", N, nBranch);

    int nSlack = 0, nPV = 0, nPQ = 0;
    for (int i = 0; i < N; ++i) {
        if (busData.Type(i) == 1) nSlack++;
        else if (busData.Type(i) == 2) nPV++;
        else nPQ++;
    }
    DEBUG("Bus types: {} Slack, {} PV, {} PQ", nSlack, nPV, nPQ);

    auto Y = computeAdmittanceMatrix(busData, branchData);
    DEBUG("Admittance matrix computed ({}x{})", N, N);

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

    Eigen::VectorXi type_bus = busData.Type;

    bool finalConverged = false;
    int totalIterations = 0;
    double finalError = 0.0;
    std::vector<std::pair<int, double>> iterationHistory;

    INFO("Starting {} solver ...", solverName);

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
                                              maxIter, tolerance, relaxation_coeff,
                                              &iterationHistory);

                finalConverged = converged;

                if (!converged) {
                    ERROR("Gauss-Seidel solver failed to converge.");
                    break;
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
            bool Q_lim_status = true;

            while (Q_lim_status) {
                Eigen::VectorXd Ps = busData.Pg - busData.Pl;
                Eigen::VectorXd Qs = busData.Qg - busData.Ql;

                std::vector<int> pq_indices;
                std::vector<int> pv_indices;

                for (int i = 0; i < N; ++i) {
                    if (type_bus(i) == 3) pq_indices.push_back(i);
                    else if (type_bus(i) == 2) pv_indices.push_back(i);
                }

                int n_pq = static_cast<int>(pq_indices.size());

                bool converged = NewtonRaphson(G, B, Ps, Qs, V, delta,
                    N, n_pq, pq_indices, maxIter, tolerance, &iterationHistory);

                finalConverged = converged;

                if (!converged) {
                    ERROR("Newton-Raphson solver failed to converge.");
                    break;
                }

                Q_lim_status = checkQlimits(V, delta, type_bus, G, B,
                    busData, pv_indices, N);

                if (Q_lim_status) {
                    DEBUG("Re-running Newton-Raphson with updated bus types ...");
                }
            }
            break;
        }
    }

    if (!iterationHistory.empty()) {
        totalIterations = iterationHistory.back().first;
        finalError = iterationHistory.back().second;
    }

    if (!finalConverged) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedSec = std::chrono::duration<double>(endTime - startTime).count();
        OutputFile::writeStatusFile(jobName, inputFile, solverName, formatName,
            N, nBranch, maxIter, 0.0, tolerance, false, elapsedSec);
        std::exit(1);
    }

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

    for (int i = 0; i < N; ++i) {
        busData.V(i) = std::abs(Vc(i));
        busData.delta(i) = std::arg(Vc(i)) * 180.0 / M_PI;
        busData.Pg(i) = P_net(i) + busData.Pl(i);
        busData.Qg(i) = Q_net(i) + busData.Ql(i);
    }

    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    double QLoss = busData.Qg.sum() - busData.Ql.sum();
    DEBUG("Total real power loss: {:.6f} p.u.", PLoss);
    DEBUG("Total reactive power loss: {:.6f} p.u.", QLoss);

    dispBusData(busData);
    dispLineFlow(busData, branchData, Y);

    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedSec = std::chrono::duration<double>(endTime - startTime).count();

    writeOutputCSV(busData);

    OutputFile::writeOutputFile(jobName, inputFile, solverName, formatName,
        busData, branchData, Y, totalIterations, finalError, tolerance, elapsedSec);

    OutputFile::writeStatusFile(jobName, inputFile, solverName, formatName,
        N, nBranch, totalIterations, finalError, tolerance, finalConverged, elapsedSec);

    OutputFile::writeDatFile(jobName, inputFile, solverName, formatName,
        busData, branchData, iterationHistory, totalIterations, finalError,
        tolerance, finalConverged, elapsedSec);

    OutputFile::writeMessageFile(jobName, solverName, iterationHistory, tolerance, finalConverged);

    // =====================================================
    // Final summary to terminal
    // =====================================================
    fmt::print("\n");
    fmt::print(fg(Banner::BRAND_COLOR) | fmt::emphasis::bold,
        "   THE ANALYSIS HAS BEEN COMPLETED SUCCESSFULLY\n");
    fmt::print("\n");
    fmt::print("   Elapsed time : {:.3f} sec\n", elapsedSec);
    fmt::print("\n");

    return 0;
}
