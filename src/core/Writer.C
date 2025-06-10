#include <complex>
#include <fstream>
#include <iomanip>

#include <Eigen/Dense>

#include "Reader.H"
#include "Writer.H"

bool writeOutputCSV(const BusData& busData) {
    std::ofstream out("deltaFlow.csv");
    if (!out.is_open()) return false;

    out << "BusID,Name,Type,Voltage,Angle,Pg,Qg,Pl,Ql,Qgmax,Qgmin,Gs,Bs\n";
    for (int i = 0; i < busData.ID.size(); ++i) {
        out << busData.ID[i] << ","
            << busData.Name[i] << ","
            << busData.Type[i] << ","
            << std::fixed << std::setprecision(64)
            << busData.V[i] << ","
            << busData.delta[i] << ","
            << busData.Pg[i] << ","
            << busData.Qg[i] << ","
            << busData.Pl[i] << ","
            << busData.Ql[i] << ","
            << busData.Qgmax[i] << ","
            << busData.Qgmin[i] << ","
            << busData.Gs[i] << ","
            << busData.Bs[i] << "\n";
    }

    out.close();
    return true;
}
