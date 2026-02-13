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
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>

#include "Logger.H"
#include "PSSE.H"

static std::string strip(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::string stripQuotes(const std::string& s) {
    std::string t = strip(s);
    if (t.size() >= 2 && t.front() == '\'' && t.back() == '\'')
        return strip(t.substr(1, t.size() - 2));
    return t;
}

static std::vector<std::string> splitFields(const std::string& line) {
    std::vector<std::string> fields;
    std::string data = line;
    auto slashPos = data.find('/');
    if (slashPos != std::string::npos)
        data = data.substr(0, slashPos);
    std::stringstream ss(data);
    std::string field;
    while (std::getline(ss, field, ',')) {
        fields.push_back(strip(field));
    }
    return fields;
}

static bool isSectionEnd(const std::string& line) {
    std::string s = strip(line);
    if (s.empty()) return false;
    return s == "0" || s == "Q"
        || (s.size() >= 2 && s[0] == '0' && (s[1] == ' ' || s[1] == '/' || s[1] == ','));
}

void PSSERawFormat::read(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        ERROR("Cannot open input file: {}", filename);
        return;
    }

    DEBUG("Reading PSS/E Raw Format: {}", filename);

    std::string line;

    std::getline(file, line);
    auto hdr = splitFields(line);

    double sbase = 100.0;
    int version = 33;

    if (hdr.size() >= 2) sbase = std::stod(hdr[1]);
    if (hdr.size() >= 3) version = std::stoi(hdr[2]);

    DEBUG("PSS/E RAW format version {} detected (SBASE = {:.2f} MVA)", version, sbase);

    if (version != 32 && version != 33) {
        WARN("PSS/E version {} is not explicitly supported. Attempting to parse as v{}.",
             version, (version >= 33 ? 33 : 32));
    }

    // Title lines (2 lines)
    std::string title1, title2;
    std::getline(file, title1);
    std::getline(file, title2);
    DEBUG("PSS/E case: {}", strip(title1));

    //  Bus data
    //  v32: I,'NAME',BASKV,IDE,AREA,ZONE,OWNER,VM,VA
    //  v33: I,'NAME',BASKV,IDE,AREA,ZONE,OWNER,VM,VA,NVHI,NVLO,EVHI,EVLO
    std::vector<int> busID;
    std::vector<std::string> busName;
    std::vector<int> busType;
    std::vector<double> V, delta_vec;

    std::map<int, int> busIndex;
    int busCount = 0;

    while (std::getline(file, line)) {
        if (isSectionEnd(line)) break;

        auto f = splitFields(line);
        if (f.size() < 9) continue;

        busCount++;
        int origID = std::stoi(f[0]);
        busIndex[origID] = busCount;

        busID.push_back(busCount);
        busName.push_back(stripQuotes(f[1]));

        int ide = std::stoi(f[3]);
        int type;
        switch (ide) {
            case 3:  type = 1; break;  // Slack
            case 2:  type = 2; break;  // PV
            default: type = 3; break;  // PQ (including isolated IDE=4)
        }
        busType.push_back(type);

        double vmag = std::stod(f[7]);
        V.push_back(vmag > 0.0 ? vmag : 1.0);
        delta_vec.push_back(0.0);
    }

    int nBus = busCount;
    DEBUG("  {} buses read", nBus);

    // Per-bus accumulators
    std::vector<double> Pl(nBus, 0.0), Ql(nBus, 0.0);
    std::vector<double> Pg(nBus, 0.0), Qg(nBus, 0.0);
    std::vector<double> Qgmax(nBus, 0.0), Qgmin(nBus, 0.0);
    std::vector<double> Gs(nBus, 0.0), Bs(nBus, 0.0);

    //  Load data
    //  I,'ID',STATUS,AREA,ZONE,PL,QL,IP,IQ,YP,YQ,OWNER,SCALE[,INTRPT]
    while (std::getline(file, line)) {
        if (isSectionEnd(line)) break;

        auto f = splitFields(line);
        if (f.size() < 8) continue;

        int origBus = std::stoi(f[0]);
        int status  = std::stoi(f[2]);
        if (status == 0) continue;

        int idx = busIndex[origBus] - 1;
        Pl[idx] += std::stod(f[5]) / sbase;
        Ql[idx] += std::stod(f[6]) / sbase;
    }

    //  Fixed shunt data:  I,STATUS,GL,BL
    while (std::getline(file, line)) {
        if (isSectionEnd(line)) break;

        auto f = splitFields(line);
        if (f.size() < 4) continue;

        int origBus = std::stoi(f[0]);
        int status  = std::stoi(f[1]);
        if (status == 0) continue;

        int idx = busIndex[origBus] - 1;
        Gs[idx] += std::stod(f[2]) / sbase;
        Bs[idx] += std::stod(f[3]) / sbase;
    }

    //  Generator data
    //  I,'ID',PG,QG,QT,QB,VS,IREG,MBASE,...
    while (std::getline(file, line)) {
        if (isSectionEnd(line)) break;

        auto f = splitFields(line);
        if (f.size() < 10) continue;

        int origBus = std::stoi(f[0]);
        int idx = busIndex[origBus] - 1;

        Pg[idx]    += std::stod(f[2]) / sbase;
        Qg[idx]    += std::stod(f[3]) / sbase;
        Qgmax[idx] += std::stod(f[4]) / sbase;  // QT
        Qgmin[idx] += std::stod(f[5]) / sbase;  // QB

        // Use generator voltage setpoint for PV/Slack buses
        double vs = std::stod(f[6]);
        if (busType[idx] == 1 || busType[idx] == 2) {
            V[idx] = vs;
        }
    }

    //  Branch data
    //  I,J,'CKT',R,X,B,RATEA,RATEB,RATEC,...
    std::vector<int> fromBus, toBus;
    std::vector<double> R_vec, X_vec, G_vec, B_vec, tap_vec;

    while (std::getline(file, line)) {
        if (isSectionEnd(line)) break;

        auto f = splitFields(line);
        if (f.size() < 6) continue;

        int from = std::stoi(f[0]);
        int to   = std::abs(std::stoi(f[1]));

        fromBus.push_back(busIndex[from]);
        toBus.push_back(busIndex[to]);
        R_vec.push_back(std::stod(f[3]));
        X_vec.push_back(std::stod(f[4]));
        G_vec.push_back(0.0);
        B_vec.push_back(std::stod(f[5]));
        tap_vec.push_back(1.0);
    }

    //  Transformer data (2-winding: K==0, 4-line records)
    //
    //  Line 1: I,J,K,'CKT',CW,CZ,CM,MAG1,MAG2,NMETR,'NAME',STAT,...
    //  Line 2: R1-2, X1-2, SBASE1-2
    //  Line 3: WINDV1, NOMV1, ANG1, RATA1, RATB1, RATC1, ...
    //  Line 4: WINDV2, NOMV2
    //  (3-winding adds a 5th line)
    while (std::getline(file, line)) {
        if (isSectionEnd(line)) break;

        auto f1 = splitFields(line);
        if (f1.size() < 5) continue;

        int I = std::stoi(f1[0]);
        int J = std::stoi(f1[1]);
        int K = std::stoi(f1[2]);
        // f1[3] = CKT, f1[4] = CW, f1[5] = CZ
        int cz = (f1.size() >= 6) ? std::stoi(f1[5]) : 1;

        // Line 2: R1-2, X1-2, SBASE1-2
        std::getline(file, line);
        auto f2 = splitFields(line);

        double r12 = (f2.size() >= 1) ? std::stod(f2[0]) : 0.0;
        double x12 = (f2.size() >= 2) ? std::stod(f2[1]) : 0.0;
        double sbase12 = (f2.size() >= 3) ? std::stod(f2[2]) : sbase;

        // CZ=2: convert from winding base to system base
        if (cz == 2 && sbase12 > 0.0) {
            r12 *= sbase / sbase12;
            x12 *= sbase / sbase12;
        }

        // Line 3: WINDV1 (tap ratio) ...
        std::getline(file, line);
        auto f3 = splitFields(line);
        double windv1 = (f3.size() >= 1) ? std::stod(f3[0]) : 1.0;

        // Line 4: WINDV2, NOMV2
        std::getline(file, line);

        if (K != 0) {
            // 3-winding: skip extra winding line (line 5)
            std::getline(file, line);
            WARN("3-winding transformer ({}-{}-{}) encountered, skipping.", I, J, K);
            continue;
        }

        fromBus.push_back(busIndex[I]);
        toBus.push_back(busIndex[J]);
        R_vec.push_back(r12);
        X_vec.push_back(x12);
        G_vec.push_back(0.0);
        B_vec.push_back(0.0);
        double tapVal = (windv1 == 0.0) ? 1.0 : windv1;
        tap_vec.push_back(tapVal);
    }

    busData.ID    = Eigen::Map<Eigen::VectorXi>(busID.data(), nBus);
    busData.Type  = Eigen::Map<Eigen::VectorXi>(busType.data(), nBus);
    busData.V     = Eigen::Map<Eigen::VectorXd>(V.data(), nBus);
    busData.delta = Eigen::Map<Eigen::VectorXd>(delta_vec.data(), nBus);
    busData.Pg    = Eigen::Map<Eigen::VectorXd>(Pg.data(), nBus);
    busData.Qg    = Eigen::Map<Eigen::VectorXd>(Qg.data(), nBus);
    busData.Pl    = Eigen::Map<Eigen::VectorXd>(Pl.data(), nBus);
    busData.Ql    = Eigen::Map<Eigen::VectorXd>(Ql.data(), nBus);
    busData.Qgmax = Eigen::Map<Eigen::VectorXd>(Qgmax.data(), nBus);
    busData.Qgmin = Eigen::Map<Eigen::VectorXd>(Qgmin.data(), nBus);
    busData.Gs    = Eigen::Map<Eigen::VectorXd>(Gs.data(), nBus);
    busData.Bs    = Eigen::Map<Eigen::VectorXd>(Bs.data(), nBus);
    busData.Name  = busName;

    int nBranch = static_cast<int>(fromBus.size());
    branchData.From     = Eigen::Map<Eigen::VectorXi>(fromBus.data(), nBranch);
    branchData.To       = Eigen::Map<Eigen::VectorXi>(toBus.data(), nBranch);
    branchData.R        = Eigen::Map<Eigen::VectorXd>(R_vec.data(), nBranch);
    branchData.X        = Eigen::Map<Eigen::VectorXd>(X_vec.data(), nBranch);
    branchData.G        = Eigen::Map<Eigen::VectorXd>(G_vec.data(), nBranch);
    branchData.B        = Eigen::Map<Eigen::VectorXd>(B_vec.data(), nBranch);
    branchData.tapRatio = Eigen::Map<Eigen::VectorXd>(tap_vec.data(), nBranch);

    DEBUG("PSS/E v{} file parsed: {} buses, {} branches (incl. transformers)",
          version, nBus, nBranch);
}
