#ifndef BUS_DATA_READER_H
#define BUS_DATA_READER_H

#include "csvReader.H"
#include "types.H"

class BranchDataCsvReader;

class BusDataCsvReader: public CsvReader {
    friend class BranchDataCsvReader;

    public:
        BusDataCsvReader(const std::string&);
        void read() override;

        ComplexVector V() const noexcept;
        DoubleVector P() const noexcept;
        DoubleVector Q() const noexcept;
        DoubleVector getQGenMax() const noexcept;
        DoubleVector getQGenMin() const noexcept;
        IntegerVector getBusType() const noexcept;

    private:
        IntegerVector id;
        IntegerVector type;
        DoubleVector voltage;
        DoubleVector delta;
        DoubleVector pGen;
        DoubleVector qGen;
        DoubleVector pLoad;
        DoubleVector qLoad;
        DoubleVector qGenMax;
        DoubleVector qGenMin;
        DoubleVector conductanceShunt;
        DoubleVector susceptanceShunt;
};

#endif
