/*---------------------------------------------------------------------------*\

                                  /\\
                                 /  \\
                                /    \\
                               /      \\
                               =========

                               deltaFlow
                      Power System Analysis Tool
                          Copyright (c) 2025
--------------------------------------------------------------------------------
License
    This file is part of deltaFlow.

   deltaFlow is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   deltaFlow is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with deltaFlow.  If not, see <https://www.gnu.org/licenses/>.

Author
    Saud Zahir

Date
    09 December 2024
\*---------------------------------------------------------------------------*/

#include "argparse.H"
#include "busDataReader.H"
#include "branchDataReader.H"
#include "algorithms.H"


int main(int argc, char* argv[]) {
    ArgumentParser opts(argc, argv);
    ComplexVector Y, V;
    DoubleVector P, Q, Qmax, Qmin;
    IntegerVector busType;
    INTEGER N;
    INTEGER maxIter = opts.getMaxIterations();
    DOUBLE tolerance = opts.getTolerance();
    DOUBLE omega = 1.0;
    INTEGER iterOut = 0;
    DOUBLE error = 0.0;

    try {
        BusDataCsvReader busReader(opts.getBusDataCsv());
        busReader.read();

        BranchDataCsvReader branchReader(opts.getBranchDataCsv());
        branchReader.read();

        Y = branchReader.Y(busReader);
        V = busReader.V();
        P = busReader.P();
        Q = busReader.Q();
        N = V.size();
        Qmax = busReader.getQGenMax();
        Qmin = busReader.getQGenMin();
        busType = busReader.getBusType();

    } catch (const std::exception& e) {
        ERROR("{}", e.what());
    }

    switch (opts.getSolverType()) {
        case Solver::GAUSS:
            GAUSS_SEIDEL(V.data(),
                         P.data(),
                         Q.data(),
                         Y.data(),
                         &N, &maxIter,
                         &tolerance);
            break;
        case Solver::NEWTON:
            NEWTON_RAPHSON(V.data(),
                           P.data(),
                           Q.data(),
                           Y.data(),
                           &N, &maxIter,
                           &tolerance);
            break;
        default:
            break;
    }

    forAll(i, V) {
        DEBUG("{} {} {} {}", std::abs(V[i]), (180.0 / M_PI) * std::arg(V[i]), P[i], Q[i]);
    }

    return 0;
}
