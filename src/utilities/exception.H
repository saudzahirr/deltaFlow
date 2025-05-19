#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>
#include "logger.H"

#define RuntimeError(msg, ...) Exception(fmt::format(FMT_STRING(msg), ##__VA_ARGS__))

class Exception : public std::exception {
    public:
        Exception(const std::string&);
        const char* what() const noexcept override;

    private:
        std::string errorMessage;
};

#endif // EXCEPTION_H
