#include "Argparse.H"
#include "Logger.H"
#include "Reader.H"
#include "Writer.H"
#include "Admittance.H"
#include "NewtonRaphson.H"
#include "GaussSeidel.H"

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
