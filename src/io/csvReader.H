#ifndef CSV_READER_H
#define CSV_READER_H

#include <fstream>
#include <string>
#include "logger.H"
#include "exception.H"

class CsvReader {
    public:
        virtual ~CsvReader();
        virtual void read() = 0;

    protected:
        std::ifstream file;
        std::string csvFilePath;
};

#endif
