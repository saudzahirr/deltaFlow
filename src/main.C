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

#include <string>

#include "cmd.H"
#include "config.H"
#include "algorithms.H"
#include "cdfReader.H"


int main(int argc, char* argv[]) {
    Utilities::Options opts(argc, argv);
    std::string simFile = opts.getSimFile();
    Config config(simFile);

    switch (config.getAnalysisType()) {
        case AnalysisType::STATIC:
            switch (config.getFormat()) {
                case Format::CDF: {
                    std::string name = config.getIncludeFile();
                    Reader* reader = new CommonDataFormatReader(name);
                    PowerSystemData pData = reader->read();
                    DEBUG("Number of buses: {}", pData.N);
                    DEBUG("VOLTAGE :: {}", pData.voltage[0]);
                    // solve(config, pData);

                    for (int i = 0; i < pData.N; ++i) {
                        auto& v = pData.V[i];
                        DEBUG("v[{}] = {}", i, std::abs(v));
                    }
                    delete reader;
                }
                break;
            }

        case AnalysisType::TRANSIENT:
            break;

        default:
            break;
    }

    return 0;
}
