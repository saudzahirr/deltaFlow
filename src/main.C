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

#include "Admittance.H"
#include "Argparse.H"
#include "GaussSeidel.H"
#include "NewtonRaphson.H"
#include "Logger.H"
#include "Reader.H"
#include "Writer.H"

int main(int argc, char* argv[]) {
    ArgumentParser opts(argc, argv);
    DEBUG("Bus Data csv :: {}", opts.getBusDataCsv());
    DEBUG("Branch Data csv :: {}", opts.getBranchDataCsv());

    BusData busData;
    BranchData branchData;

    Solver solver = opts.getSolver();
    int maxIter = opts.getMaxIterations();
    double tolerance = opts.getTolerance();

    readBusDataCSV(opts.getBusDataCsv(), busData);
    readBranchDataCSV(opts.getBranchDataCsv(), branchData);

    auto Y = computeAdmittanceMatrix(busData, branchData);

    switch (solver) {
        case Solver::GaussSeidel: {
            double relaxation_coeff = opts.getRelaxationCoefficient();
            GaussSeidel(Y, busData, maxIter, tolerance, relaxation_coeff);
            break;
        }

        case Solver::NewtonRaphson: {
            NewtonRaphson(Y, busData, maxIter, tolerance);
            break;
        }

        default: {
            NewtonRaphson(Y, busData, maxIter, tolerance);
            break;
        }
    }

    dispBusData(busData);
    dispLineFlow(busData, branchData, Y);

    writeOutputCSV(busData);

    return 0;
}
