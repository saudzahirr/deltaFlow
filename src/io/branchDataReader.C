#include "branchDataReader.H"
#include "busDataReader.H"
#include <sstream>
#include <iostream>

BranchDataCsvReader::BranchDataCsvReader(const std::string& csv_filepath) {
    csvFilePath = csv_filepath;
    file.open(csvFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open " + csvFilePath);
    }
}

void BranchDataCsvReader::read() {
    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int from_, to_;
        double r, x, g, b, tap;

        char comma;
        ss >> from_ >> comma >> to_ >> comma >> r >> comma >> x >> comma >>
              g >> comma >> b >> comma >> tap;

        from.push_back(from_);
        to.push_back(to_);
        resistance.push_back(r);
        reactance.push_back(x);
        conductance.push_back(g);
        susceptance.push_back(b);
        tapRatio.push_back(tap);
    }
}

ComplexVector BranchDataCsvReader::Y(const BusDataCsvReader& busReader) {
    try {
        this->compute_admittance_matrix(busReader);
    }
    catch (std::exception& exc) {
        throw RuntimeError("{}", exc.what());
    }

    return this->admittanceMatrix;
}

void BranchDataCsvReader::compute_admittance_matrix(const BusDataCsvReader& busData) {
    int N = *std::max_element(from.begin(), from.end());
    N = std::max(N, *std::max_element(to.begin(), to.end()));

    admittanceMatrix = ComplexVector(N * N, {0.0, 0.0});

    size_t nLine = from.size();
    ComplexVector Z(nLine), Y(nLine), B(nLine);
    DoubleVector aAdjusted = tapRatio;

    for (size_t i = 0; i < nLine; ++i) {
        Z[i] = DOUBLE_COMPLEX(resistance[i], reactance[i]);
        Y[i] = 1.0 / Z[i];
        B[i] = DOUBLE_COMPLEX(0.0, 0.5 * susceptance[i]);

        if (aAdjusted[i] == 0.0) aAdjusted[i] = 1.0;
    }

    auto getIndex = [N](int row, int col) -> int {
        return row + col * N;  // Column-major
    };

    for (size_t k = 0; k < nLine; ++k) {
        int i = from[k] - 1;
        int j = to[k] - 1;

        DOUBLE_COMPLEX Yij = Y[k] / aAdjusted[k];
        DOUBLE_COMPLEX Yii = Y[k] / (aAdjusted[k] * aAdjusted[k]) + B[k];
        DOUBLE_COMPLEX Yjj = Y[k] + B[k];

        // Off-diagonal (symmetric)
        admittanceMatrix[getIndex(i, j)] -= Yij;
        admittanceMatrix[getIndex(j, i)] = admittanceMatrix[getIndex(i, j)];

        // Diagonal
        admittanceMatrix[getIndex(i, i)] += Yii;
        admittanceMatrix[getIndex(j, j)] += Yjj;
    }

    for (size_t n = 0; n < busData.conductanceShunt.size(); ++n) {
        admittanceMatrix[getIndex(n, n)] += std::complex<double>(
            busData.conductanceShunt[n], busData.susceptanceShunt[n]);
    }
}
