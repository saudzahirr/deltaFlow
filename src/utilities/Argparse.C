#include "Argparse.H"
#include "Utils.H"
#include "Logger.H"
#include "Version.H"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

ArgumentParser::ArgumentParser(int argc, char* argv[]) {
    parse_args(argc, argv);
}

void ArgumentParser::parse_args(int argc, char* argv[]) {
    bool methodFound = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "--bus" || arg == "-b") && i + 1 < argc) {
            this->busDataCsv = argv[++i];
        }
        else if ((arg == "--branch" || arg == "-l") && i + 1 < argc) {
            this->branchDataCsv = argv[++i];
        }
        else if ((arg == "--tolerance" || arg == "-t") && i + 1 < argc) {
            this->tolerance = std::stod(argv[++i]);
        }
        else if ((arg == "--max-iterations" || arg == "-m") && i + 1 < argc) {
            this->maxIterations = std::stoi(argv[++i]);
        }
        else if ((arg == "--relaxation" || arg == "-r") && i + 1 < argc) {
            this->relaxation = std::stod(argv[++i]);
        }
        else if (arg == "--version" || arg == "-v") {
            version();
            std::exit(0);
        }
        else if (arg == "--help" || arg == "-h") {
            help();
            std::exit(0);
        }
        else if (!methodFound) {
            if (arg == "gauss-seidel") {
                this->method = Solver::GaussSeidel;
                methodFound = true;
            }
            else if (arg == "newton-raphson") {
                this->method = Solver::NewtonRaphson;
                methodFound = true;
            }
            else {
                MESSAGE("ERROR: Invalid method '{}'", arg);
                help();
                std::exit(1);
            }
        }
        else {
            MESSAGE("ERROR: Unexpected extra argument '{}'", arg);
            help();
            std::exit(1);
        }
    }

    if (!(Utilities::endswith(busDataCsv, ".csv") && Utilities::endswith(branchDataCsv, ".csv"))) {
        MESSAGE("ERROR: Bus & branch data csv file is required.");
        help();
        std::exit(1);
    }

    if (!methodFound) {
        MESSAGE("ERROR: Missing required method argument (gauss-seidel or newton-raphson).");
        help();
        std::exit(1);
    }

    if (method == Solver::NewtonRaphson && relaxation != 1.0) {
        MESSAGE("Warning: Relaxation coefficient ignored for method 'newton-raphson'");
    }

    DEBUG("deltaFlow v{}", deltaFlow_VERSION);
}

std::string ArgumentParser::getBusDataCsv() const noexcept {
    return this->busDataCsv;
}

std::string ArgumentParser::getBranchDataCsv() const noexcept {
    return this->branchDataCsv;
}

double ArgumentParser::getTolerance() const noexcept {
    return this->tolerance;
}

int ArgumentParser::getMaxIterations() const noexcept {
    return this->maxIterations;
}

double ArgumentParser::getRelaxationCoefficient() const noexcept {
    return this->relaxation;
}

Solver ArgumentParser::getSolver() const noexcept {
    return this->method;
}

void ArgumentParser::help() const noexcept {
    MESSAGE(R"(
Usage:
  deltaFlow [OPTIONS] <method>

Required:
  -b, --bus <file>             Path to Bus Data CSV file
  -l, --branch <file>          Path to Branch Data CSV file
  <solver>                     Solver method: gauss-seidel | newton-raphson

Options:
  -t, --tolerance <value>      Convergence tolerance (default: 1E-8)
  -m, --max-iterations <int>   Maximum number of iterations (default: 1024)
  -r, --relaxation <value>     Relaxation coefficient (only for gauss-seidel, default: 1.0)
  -h, --help                   Display this help message and exit
  -v, --version                Show program version and exit

Solvers:
  gauss-seidel                Iterative method suitable for smaller systems. Supports relaxation.
  newton-raphson              Robust method for large or nonlinear systems. Typically converges faster,
                              but requires more computation per iteration.
)");
}

void ArgumentParser::version() const noexcept {
    MESSAGE("deltaFlow v{}", deltaFlow_VERSION);
}
