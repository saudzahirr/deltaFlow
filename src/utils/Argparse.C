#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "Argparse.H"
#include "Logger.H"
#include "Utils.H"
#include "Version.H"

ArgumentParser::ArgumentParser(int argc, char* argv[]) {
    parse_args(argc, argv);
}

void ArgumentParser::parse_args(int argc, char* argv[]) {
    bool methodFound = false;
    bool inputFileFound = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "--job" || arg == "-j") && i + 1 < argc) {
            this->jobName = argv[++i];
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
        else if (!inputFileFound) {
            this->inputFile = arg;

            if (Utilities::isCommonDataFormat(arg)) {
                this->format = InputFormat::IEEE;
            }
            else if (Utilities::isRawFormat(arg))
            {
                this->format = InputFormat::PSSE;
            }
            else {
                MESSAGE("ERROR: Invalid format '{}'", arg);
                help();
                std::exit(1);
            }

            inputFileFound = true;
        }
        else if (!methodFound) {
            if (arg == "GAUSS") {
                this->method = SolverType::GaussSeidel;
                methodFound = true;
            }
            else if (arg == "NEWTON") {
                this->method = SolverType::NewtonRaphson;
                methodFound = true;
            }
            else {
                MESSAGE("ERROR: Invalid method '{}'", arg);
                help();
                std::exit(1);
            }
        }
        else {
            MESSAGE("ERROR: Unexpected argument '{}'", arg);
            help();
            std::exit(1);
        }
    }

    if (!inputFileFound) {
        MESSAGE("ERROR: Input CDF file (.txt or .cdf) is required.");
        help();
        std::exit(1);
    }

    if (!methodFound) {
        MESSAGE("ERROR: Missing required solver argument (GAUSS or NEWTON).");
        help();
        std::exit(1);
    }

    if (jobName.empty()) {
        jobName = std::filesystem::path(inputFile).filename().string();
    }

    if (method == SolverType::NewtonRaphson && relaxation != 1.0) {
        MESSAGE("Warning: Relaxation coefficient ignored for method 'NEWTON'");
    }

    DEBUG("deltaFlow v{}", deltaFlow_VERSION);
}

std::string ArgumentParser::getInputFile() const noexcept {
    return this->inputFile;
}

std::string ArgumentParser::getJobName() const noexcept {
    return this->jobName;
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

SolverType ArgumentParser::getSolverType() const noexcept {
    return this->method;
}

InputFormat ArgumentParser::getInputFormat() const noexcept {
    return this->format;
}

void ArgumentParser::help() const noexcept {
    MESSAGE(R"(
Usage:
  deltaFlow [OPTIONS] <input-file> <solver>

Required:
  <input-file>                 Path to input CDF file (.txt or .cdf)
  <solver>                     Solver method: GAUSS | NEWTON

Options:
  -j, --job <name>             Job name (default: input filename)
  -t, --tolerance <value>      Convergence tolerance (default: 1E-8)
  -m, --max-iterations <int>   Maximum number of iterations (default: 1024)
  -h, --help                   Display this help message and exit
  -v, --version                Show program version and exit

Solvers:
  GAUSS                Iterative method suitable for smaller systems.
    -r, --relaxation <value>  Relaxation coefficient (default: 1.0)

  NEWTON              Robust method for large or nonlinear systems. Typically converges faster,
                              but requires more computation per iteration.
)");
}

void ArgumentParser::version() const noexcept {
    MESSAGE("deltaFlow v{}", deltaFlow_VERSION);
}
