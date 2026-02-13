#ifndef PSSE_H
#define PSSE_H

#include "Reader.H"

class PSSERawFormat: public Reader {
    public:
        ~PSSERawFormat() = default;

        void read(const std::string& filename) override;

};

#endif
