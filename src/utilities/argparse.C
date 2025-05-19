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

ArgumentParser::ArgumentParser(int argc, char* argv[]) {
    parse_args(argc, argv);
}

void ArgumentParser::parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "--bus" || arg == "-b") && i + 1 < argc) {
            this->busDataCsv = argv[++i];
        }
        else if ((arg == "--branch" || arg == "-y") && i + 1 < argc) {
            this->branchDataCsv = argv[++i];
        }
        else if ((arg == "--solver" || arg == "-s") && i + 1 < argc) {
            std::string method = argv[++i];
            Utilities::toupper(method);

            if (method == "GAUSS") {
                this->solver = Solver::GAUSS;
            }
            else if (method == "NEWTON") {
                this->solver = Solver::NEWTON;
            }
            else {
                MESSAGE("ERROR: Invalid solver type {}", method);
                help();
                exit(1);
            }
        }
        else if ((arg == "--tol" || arg == "-t") && i + 1 < argc) {
            this->tolerance = std::stold(argv[++i]);
        }
        else if ((arg == "--max" || arg == "-m") && i + 1 < argc) {
            this->maxIterations = std::stoul(argv[++i]);
        }
        else if ((arg == "--ascii" || arg == "-a")) {
            this->ascii = true;
        }
        else if (arg == "--gui" || arg == "-g") {
            this->gui = true;
        }
        else if (arg == "--version" || arg == "-v") {
            version();
            exit(0);
        }
        else if (arg == "--help" || arg == "-h") {
            help();
            exit(0);
        }
        else {
            MESSAGE("Invalid option: {}", arg);
            help();
            exit(1);
        }
    }

    if (not (Utilities::endswith(busDataCsv, ".csv") and Utilities::endswith(branchDataCsv, ".csv"))) {
        MESSAGE("ERROR: Bus & branch data csv file is required.");
        help();
        exit(1);
    }
}


std::string ArgumentParser::getBusDataCsv() const noexcept {
    return this->busDataCsv;
}

std::string ArgumentParser::getBranchDataCsv() const noexcept {
    return this->branchDataCsv;
}

Solver ArgumentParser::getSolverType() const noexcept {
    return this->solver;
}

double ArgumentParser::getTolerance() const noexcept {
    return this->tolerance;
}

unsigned long int ArgumentParser::getMaxIterations() const noexcept {
    return this->maxIterations;
}

bool ArgumentParser::writeAscii() const noexcept {
    return this->ascii;
}

bool ArgumentParser::guiEnabled() const noexcept {
    return this->gui;
}

void ArgumentParser::help() const noexcept {
   MESSAGE(R"(
Usage:
  deltaFlow [OPTIONS]

Required:
  -b, --bus <file>         Path to Bus Data CSV file
  -y, --branch <file>      Path to Branch Data CSV file

Options:
  -s, --solver <type>      Solver type: GAUSS or NEWTON (default: GAUSS)
  -t, --tol <value>        Convergence tolerance (default: 1E-8)
  -m, --max <number>       Maximum number of iterations (default: 128)
  -a, --ascii              Enable ASCII output (in addition to binary)
  -g, --gui                Launch GUI mode (default: off)
  -h, --help               Display this help message and exit
  -v, --version            Show program version and exit
)");
}

void ArgumentParser::version() const noexcept {
    MESSAGE("{}", VERSION);
}
