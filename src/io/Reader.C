#include "Reader.H"

const BusData& Reader::getBusData() const noexcept {
    return this->busData;
}

const BranchData& Reader::getBranchData() const noexcept {
    return this->branchData;
}
