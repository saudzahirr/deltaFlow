#ifndef BASE_READER_H
#define BASE_READER_H

#include "logger.H"
#include "matrix.H"

struct PowerSystemData {
    double pBase;
    int N;
    int* busType;
    double* voltage;
    double* delta;
    double* pLoad;
    double* qLoad;
    double* pGen;
    double* qGen;
    double* qMax;
    double* qMin;
    double* gShunt;
    double* bShunt;
    std::complex<double>* Y;
    std::complex<double>* V;
    double* P;
    double* Q;
};

class Reader {
    public:
        virtual ~Reader();

        /**
         * Pure virtual function
         */
        virtual PowerSystemData read() = 0;

    protected:
        std::ifstream file;
        std::string m_fileName;
};

#endif
