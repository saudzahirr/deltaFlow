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
#include "IEEE.H"
#include "PSSE.H"
#include "Writer.H"

int main(int argc, char* argv[]) {
    ArgumentParser args(argc, argv);
    DEBUG("Input file :: {}", args.getInputFile());
    DEBUG("Job name :: {}", args.getJobName());

    Reader* reader = nullptr;

    SolverType solver = args.getSolverType();
    InputFormat format = args.getInputFormat();
    int maxIter = args.getMaxIterations();
    double tolerance = args.getTolerance();

    switch (format) {
    case InputFormat::IEEE:
        reader = dynamic_cast<Reader *>(new(std::nothrow) IEEECommonDataFormat());
        break;
    case InputFormat::PSSE:
        reader = dynamic_cast<Reader *>(new(std::nothrow) PSSERawFormat());
        break;
    default:
        break;
    }

    reader->read(args.getInputFile());

    auto busData = reader->getBusData();
    auto& branchData = reader->getBranchData();

    if (busData.ID.size() == 0 || branchData.From.size() == 0) {
        ERROR("No bus or branch data found in '{}'. Check the file exists and is valid.", args.getInputFile());
        delete reader;
        return 1;
    }

    auto Y = computeAdmittanceMatrix(busData, branchData);

    switch (solver) {
        case SolverType::GaussSeidel: {
            double relaxation_coeff = args.getRelaxationCoefficient();
            GaussSeidel(Y, busData, maxIter, tolerance, relaxation_coeff);
            break;
        }

        case SolverType::NewtonRaphson: {
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
