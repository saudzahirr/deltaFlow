#ifndef READER_H
#define READER_H

#include <string>
#include "Data.H"

class Reader {
    public:
        virtual ~Reader() = default;

        virtual void read(const std::string& filename) = 0;

        const BusData& getBusData() const noexcept;

        const BranchData& getBranchData() const noexcept;

    protected:
        BusData busData;
        BranchData branchData;
};

#endif
