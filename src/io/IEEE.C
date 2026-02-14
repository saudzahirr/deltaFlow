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

#include <algorithm>
#include <fstream>
#include <map>

#include "IEEE.H"
#include "Logger.H"

static std::string strip(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

void IEEECommonDataFormat::read(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        ERROR("Cannot open input file: {}", filename);
        return;
    }

    DEBUG("Reading IEEE Common Data Format: {}", filename);

    std::string line;
    std::string section;
    int busCount = 0;
    int branchCount = 0;

    std::vector<int> busID;
    std::vector<std::string> busName;
    std::vector<int> busType;
    std::vector<double> V, delta, Pl, Ql, Pg, Qg, Qgmax, Qgmin, Gs, Bs;

    std::vector<int> fromBus, toBus;
    std::vector<double> R, X, G, B, tap;

    std::map<std::string, std::string> busTypes;
    busTypes["0"] = "3";
    busTypes["1"] = "3";
    busTypes["2"] = "2";
    busTypes["3"] = "1";

    std::map<std::string, int> busIndex;

    while (std::getline(file, line)) {
        std::string firstToken = strip(line.substr(0, 4));

        if (line.find("BUS DATA") != std::string::npos) { section = "bus"; DEBUG("Parsing BUS DATA section ..."); continue; }
        if (line.find("BRANCH DATA") != std::string::npos) { section = "branch"; DEBUG("Parsing BRANCH DATA section ..."); continue; }
        if (line.find("-999") != std::string::npos) { section = ""; continue; }

        if (section == "bus" && !firstToken.empty() && std::all_of(firstToken.begin(), firstToken.end(), ::isdigit)) {
            busCount++;
            std::string id = strip(line.substr(0, 4));
            busIndex[id] = busCount;   // Store mapping
            busID.push_back(busCount);
            busName.push_back(strip(line.substr(4, 11)));
            std::string typeStr = strip(line.substr(24, 2));
            busType.push_back(std::stoi(busTypes[typeStr]));
            // Read voltage magnitude from CDF columns 28-33 (1-based), 0-based substr(27,6)
            double vmag = std::stod(strip(line.substr(27, 6)));
            V.push_back(vmag > 0.0 ? vmag : 1.0);
            delta.push_back(0.0);
            Pl.push_back(std::stod(strip(line.substr(40, 9))) / 100.0);
            Ql.push_back(std::stod(strip(line.substr(49, 10))) / 100.0);
            Pg.push_back(std::stod(strip(line.substr(59, 8))) / 100.0);
            Qg.push_back(std::stod(strip(line.substr(67, 8))) / 100.0);
            Qgmax.push_back(std::stod(strip(line.substr(90, 8))) / 100.0);
            Qgmin.push_back(std::stod(strip(line.substr(98, 8))) / 100.0);
            Gs.push_back(std::stod(strip(line.substr(106, 8))));
            Bs.push_back(std::stod(strip(line.substr(114, 8))));
        }

        if (section == "branch" && !firstToken.empty() && std::all_of(firstToken.begin(), firstToken.end(), ::isdigit)) {
            branchCount++;
            std::string from = strip(line.substr(0, 4));
            std::string to   = strip(line.substr(5, 4));
            fromBus.push_back(busIndex[from]);
            toBus.push_back(busIndex[to]);
            R.push_back(std::stod(strip(line.substr(19, 10))));
            X.push_back(std::stod(strip(line.substr(29, 10))));
            G.push_back(0.0); // default 0
            B.push_back(std::stod(strip(line.substr(40, 10))));
            std::string aStr = strip(line.substr(76, 6));
            double aVal = aStr.empty() ? 0.0 : std::stod(aStr);
            tap.push_back(aVal == 0.0 ? 1.0 : aVal);
        }
    }

    int nBus = busID.size();
    busData.ID = Eigen::Map<Eigen::VectorXi>(busID.data(), nBus);
    busData.Type = Eigen::Map<Eigen::VectorXi>(busType.data(), nBus);
    busData.V = Eigen::Map<Eigen::VectorXd>(V.data(), nBus);
    busData.delta = Eigen::Map<Eigen::VectorXd>(delta.data(), nBus);
    busData.Pg = Eigen::Map<Eigen::VectorXd>(Pg.data(), nBus);
    busData.Qg = Eigen::Map<Eigen::VectorXd>(Qg.data(), nBus);
    busData.Pl = Eigen::Map<Eigen::VectorXd>(Pl.data(), nBus);
    busData.Ql = Eigen::Map<Eigen::VectorXd>(Ql.data(), nBus);
    busData.Qgmax = Eigen::Map<Eigen::VectorXd>(Qgmax.data(), nBus);
    busData.Qgmin = Eigen::Map<Eigen::VectorXd>(Qgmin.data(), nBus);
    busData.Gs = Eigen::Map<Eigen::VectorXd>(Gs.data(), nBus);
    busData.Bs = Eigen::Map<Eigen::VectorXd>(Bs.data(), nBus);
    busData.Name = busName;

    int nBranch = fromBus.size();
    branchData.From = Eigen::Map<Eigen::VectorXi>(fromBus.data(), nBranch);
    branchData.To = Eigen::Map<Eigen::VectorXi>(toBus.data(), nBranch);
    branchData.R = Eigen::Map<Eigen::VectorXd>(R.data(), nBranch);
    branchData.X = Eigen::Map<Eigen::VectorXd>(X.data(), nBranch);
    branchData.G = Eigen::Map<Eigen::VectorXd>(G.data(), nBranch);
    branchData.B = Eigen::Map<Eigen::VectorXd>(B.data(), nBranch);
    branchData.tapRatio = Eigen::Map<Eigen::VectorXd>(tap.data(), nBranch);

    DEBUG("IEEE CDF parsing complete: {} bus cards, {} branch cards", nBus, nBranch);
}
