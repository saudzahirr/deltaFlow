#include "baseReader.H"

Reader::~Reader() {
    if (file.is_open()) {
        DEBUG("Closing file {} ...", m_fileName);
        file.close();
    }
}
