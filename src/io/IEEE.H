#ifndef IEEE_H
#define IEEE_H

#include "Reader.H"

class IEEECommonDataFormat: public Reader {
    public:
        ~IEEECommonDataFormat() = default;

        void read(const std::string& filename) override;
};

#endif
