#include "csvReader.H"

CsvReader::~CsvReader() {
    if (file.is_open()) {
        DEBUG("Closing file {} ...", this->csvFilePath);
        file.close();
    }
}
