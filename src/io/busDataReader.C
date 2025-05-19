#include "busDataReader.H"
#include "exception.H"
#include "matrix.H"
#include "define.H"
#include <sstream>
#include <iostream>

BusDataCsvReader::BusDataCsvReader(const std::string& csv_filepath) {
    csvFilePath = csv_filepath;
    file.open(csvFilePath);
    if (not file.is_open()) {
        throw RuntimeError("Failed to open file: {}", csvFilePath);
    }
}

void BusDataCsvReader::read() {
    std::string line;
    std::getline(file, line);  // Skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;

        std::getline(ss, field, ','); int id_ = std::stoi(field);
        std::getline(ss, field, ','); std::string name = field;
        std::getline(ss, field, ','); int type_ = std::stoi(field);
        std::getline(ss, field, ','); double v = std::stod(field);
        std::getline(ss, field, ','); double d = std::stod(field);
        std::getline(ss, field, ','); double pg = std::stod(field);
        std::getline(ss, field, ','); double qg = std::stod(field);
        std::getline(ss, field, ','); double pl = std::stod(field);
        std::getline(ss, field, ','); double ql = std::stod(field);
        std::getline(ss, field, ','); double qmax = std::stod(field);
        std::getline(ss, field, ','); double qmin = std::stod(field);
        std::getline(ss, field, ','); double gs = std::stod(field);
        std::getline(ss, field, ','); double bs = std::stod(field);

        id.push_back(id_);
        type.push_back(type_);
        voltage.push_back(v);
        delta.push_back(d);
        pGen.push_back(pg);
        qGen.push_back(qg);
        pLoad.push_back(pl);
        qLoad.push_back(ql);
        qGenMax.push_back(qmax);
        qGenMin.push_back(qmin);
        conductanceShunt.push_back(gs);
        susceptanceShunt.push_back(bs);
    }
}


ComplexVector BusDataCsvReader::V() const noexcept {
    return voltage * (cos(delta) + I * sin(delta));
}

DoubleVector BusDataCsvReader::P() const noexcept {
    return pGen - pLoad;
}

DoubleVector BusDataCsvReader::Q() const noexcept {
    return qGen - qLoad;
}

DoubleVector BusDataCsvReader::getQGenMax() const noexcept {
    return qGenMax;
}

DoubleVector BusDataCsvReader::getQGenMin() const noexcept {
    return qGenMin;
}

IntegerVector BusDataCsvReader::getBusType() const noexcept {
    return type;
}
