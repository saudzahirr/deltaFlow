#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>
#include <iostream>
#include <csignal>
#include <unistd.h>

class SignalException : public std::exception {
private:
    std::string message;

public:
    SignalException(const std::string& msg);
    const char* what() const noexcept override;
};

extern "C" void signalHandler(int signal);

#endif // EXCEPTION_H
