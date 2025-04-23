#ifndef CDF_READER_H
#define CDF_READER_H


#include "baseReader.H"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>

#include "logger.H"
#include "matrix.H"
#include "algorithms.H"


enum class IEEECardMode {
    TITLE_DATA,
    BUS_DATA,
    BRANCH_DATA,
    SKIP
};

enum class BusType {
    SLACK = 1,
    PV = 2,
    PQ = 3,
};

static const std::regex g_BusDataPattern("^BUS DATA FOLLOWS");
static const std::regex g_BranchDataPattern("^BRANCH DATA FOLLOWS");
static const std::regex g_SkipPattern("^-9+");

const std::unordered_map<int, int> busTypes = {
    {0, static_cast<int>(BusType::PQ)},
    {1, static_cast<int>(BusType::PQ)},
    {2, static_cast<int>(BusType::PV)},
    {3, static_cast<int>(BusType::SLACK)}
};

class CommonDataFormatReader: public Reader {
    public:
        CommonDataFormatReader(std::string& cdfFilePath) {
            this->m_fileName = cdfFilePath;
            file.open(m_fileName);
            if (not file.is_open()) {
                ERROR("Failed to open log file: {}", m_fileName);
                exit(EXIT_FAILURE);
            }
        }

        CommonDataFormatReader(const CommonDataFormatReader&) = delete;
        CommonDataFormatReader& operator=(const CommonDataFormatReader&) = delete;

        PowerSystemData read() {
            DEBUG("Reading CDF file: {}", m_fileName);
            std::string line;
            IEEECardMode mode = IEEECardMode::TITLE_DATA;
            int N = 0;
            double pBase = 0.0;

            if (getline(file, line)) {
                try {
                    N = std::stoi(line.substr(50, 4));
                    pBase = std::stod(line.substr(31, 5));
                }
                catch (...) {
                    ERROR("Error parsing the title data line :: {}", line);
                    exit(EXIT_FAILURE);
                }
            }

            int busCount = 0;
            int branchCount = 0;
            int busID;
            std::complex<double> I(0, 1);
            std::unordered_map<int, int> busIndex;
            Matrix<int> busType(N, 1);
            Matrix<double> voltage(N, 1), delta(N, 1),
                        pLoad(N, 1), qLoad(N, 1),
                        pGen(N, 1), qGen(N, 1),
                        qMax(N, 1), qMin(N, 1),
                        gShunt(N, 1), bShunt(N, 1),
                        P(N, 1), Q(N, 1);
            Matrix<std::complex<double>> V(N, 1), Y(N, N);

            int i, j;
            double tap;
            std::complex<double> zImpedance, yAdmittance, bChargingSusceptance;

            while (getline(file, line)) {
                try {
                    if (std::regex_search(line, g_BusDataPattern)) {
                        mode = IEEECardMode::BUS_DATA;
                        continue;
                    }
                    else if (std::regex_search(line, g_BranchDataPattern)) {
                        mode = IEEECardMode::BRANCH_DATA;
                        continue;
                    }
                    else if (std::regex_search(line, g_SkipPattern)) {
                        mode = IEEECardMode::SKIP;
                        continue;
                    }

                    switch (mode) {
                        case IEEECardMode::BUS_DATA:
                            if (busCount < N) {
                                busID = std::stoi(line.substr(0, 4));
                                busIndex[busID] = busCount;
                                busType(busCount, 0) = ::busTypes.at(std::stoi(line.substr(24, 2)));
                                if (busType(busCount, 0) == 3) {
                                    voltage(busCount, 0) = 1.0;
                                    delta(busCount, 0) = 0.0;
                                }
                                else {
                                    voltage(busCount, 0) = std::stod(line.substr(27, 6));
                                    delta(busCount, 0) = std::stod(line.substr(33, 7));
                                }
                                pLoad(busCount, 0) = std::stod(line.substr(40, 9)) / pBase;
                                qLoad(busCount, 0) = std::stod(line.substr(49, 10)) / pBase;
                                pGen(busCount, 0) = std::stod(line.substr(59, 8)) / pBase;
                                qGen(busCount, 0) = std::stod(line.substr(67, 8)) / pBase;
                                qMax(busCount, 0) = std::stod(line.substr(90, 8)) / pBase;
                                qMin(busCount, 0) = std::stod(line.substr(98, 8)) / pBase;
                                gShunt(busCount, 0) = std::stod(line.substr(106, 8));
                                bShunt(busCount, 0) = std::stod(line.substr(114, 8));
                                int idx = busIndex[busID];
                                Y(idx,idx) += std::complex<double>( gShunt(idx,0), bShunt(idx,0) );
                                ++busCount;
                            }
                            break;
                        case IEEECardMode::BRANCH_DATA:
                            tap  = std::stod(line.substr(76, 6));
                            if (tap == 0.0) tap = 1.0;

                            i = busIndex.at(std::stoi(line.substr( 0, 4)));
                            j = busIndex.at(std::stoi(line.substr( 5, 4)));

                            zImpedance = std::complex<double>(std::stod(line.substr(19,10)), std::stod(line.substr(29,10)));
                            yAdmittance = 1.0 / zImpedance;
                            bChargingSusceptance = std::complex<double>(0.0, std::stod(line.substr(40,10))/2.0);

                            Y(i,j) -= yAdmittance / tap;
                            Y(j,i)  = Y(i,j);

                            Y(i,i) += yAdmittance / (tap*tap) + bChargingSusceptance;
                            Y(j,j) += yAdmittance           + bChargingSusceptance;
                            ++branchCount;
                            break;
                        default:
                            break;
                    }
                }
                catch (...) {
                    ERROR("Error processing line: {}", line);
                }
            }

            file.close();

            V = voltage * (cos(delta) + I * sin(delta));
            P = pGen - pLoad;
            Q = qGen - qLoad;

            PowerSystemData powerDataframe;
            powerDataframe.pBase = pBase;
            powerDataframe.N = N;
            powerDataframe.busType = busType.data();
            powerDataframe.voltage = voltage.data();
            powerDataframe.delta = delta.data();
            powerDataframe.pLoad = pLoad.data();
            powerDataframe.qLoad = qLoad.data();
            powerDataframe.pGen = pGen.data();
            powerDataframe.qGen = qGen.data();
            powerDataframe.qMax = qMax.data();
            powerDataframe.qMin = qMin.data();
            powerDataframe.gShunt = gShunt.data();
            powerDataframe.bShunt = bShunt.data();

            powerDataframe.Y = Y.data();
            powerDataframe.V = V.data();
            powerDataframe.P = P.data();
            powerDataframe.Q = Q.data();

            return powerDataframe;
        }

    private:
        IEEECardMode mode = IEEECardMode::TITLE_DATA;
};

#endif
