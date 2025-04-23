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

#include "cmd.H"

using namespace Utilities;

Options::Options(int argc, char* argv[]) {
    parseArguments(argc, argv);
}

void Options::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "--sim" || arg == "-s") && i + 1 < argc) {
            this->m_simFile = argv[++i];
        }
        else if (arg == "--gui" || arg == "-g") {
            gui = true;
        }
        else if (arg == "--version" || arg == "-v") {
            displayVersion();
            exit(0);
        }
        else if (arg == "--help" || arg == "-h") {
            displayHelp();
            exit(0);
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            displayHelp();
            exit(1);
        }
    }

    if (m_simFile.empty() and not endsWith(m_simFile, ".sim")) {
        ERROR("Error: Simulation file (-s, --sim) is required.");
        displayHelp();
        exit(1);
    }
}

void Options::displayHelp() const {
    std::cout << R"(
Usage:
  deltaFlow [OPTIONS]

Options:
  -s, --sim <file>       Path to the “.sim” configuration file (required unless using -t, -h or -v)
  -g, --gui              Launch in GUI mode
  -t, --template         Print the “.sim” file template and exit
  -h, --help             Show this help message and exit
  -v, --version          Show program version and exit

Note:
  All other simulation parameters (analysis types, methods, tolerances, iterations, etc.)  
  must be defined inside your “.sim” file.  
)" << std::endl;
}

std::string Options::getSimFile() const {
    return m_simFile;
}

void Options::displayVersion() const {
    std::cout << "v0.0.1" << std::endl;
}
