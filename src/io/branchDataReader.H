#ifndef BRANCH_DATA_READER_H
#define BRANCH_DATA_READER_H

#include "csvReader.H"
#include "types.H"

class BusDataCsvReader;

class BranchDataCsvReader : public CsvReader {
public:
    BranchDataCsvReader(const std::string&);
    void read() override;

    ComplexVector Y(const BusDataCsvReader&);

private:
    IntegerVector from;
    IntegerVector to;
    DoubleVector resistance;
    DoubleVector reactance;
    DoubleVector conductance;
    DoubleVector susceptance;
    DoubleVector tapRatio;

    ComplexVector admittanceMatrix;

    void compute_admittance_matrix(const BusDataCsvReader&);
};

#endif
