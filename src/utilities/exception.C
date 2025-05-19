#include "exception.H"

Exception::Exception(const std::string& msg) : errorMessage(msg) {
    ERROR("{}", msg);
}

const char* Exception::what() const noexcept {
    return errorMessage.c_str();
}
