#include "exception.H"

SignalException::SignalException(const std::string& msg) : message(msg) {}

const char* SignalException::what() const noexcept {
    return message.c_str();
}

extern "C" void signalHandler(int signal) {
    switch (signal) {
        case SIGINT:
            throw SignalException("Received SIGINT (Ctrl+C). Program interrupted.");
        case SIGTERM:
            throw SignalException("Received SIGTERM. Termination signal.");
        case SIGSEGV:
            throw SignalException("Received SIGSEGV (Segmentation Fault). Invalid memory access.");
        case SIGABRT:
            throw SignalException("Received SIGABRT. Abort signal.");
        case SIGFPE:
            throw SignalException("Received SIGFPE (Floating Point Exception). Arithmetic error.");
        case SIGILL:
            throw SignalException("Received SIGILL (Illegal Instruction). Invalid instruction.");
        case SIGBUS:
            throw SignalException("Received SIGBUS (Bus Error). Hardware fault or invalid memory access.");
        case SIGPIPE:
            throw SignalException("Received SIGPIPE. Broken pipe.");
        case SIGALRM:
            throw SignalException("Received SIGALRM. Alarm clock.");
        case SIGUSR1:
            throw SignalException("Received SIGUSR1. User-defined signal 1.");
        case SIGUSR2:
            throw SignalException("Received SIGUSR2. User-defined signal 2.");
        case SIGCHLD:
            throw SignalException("Received SIGCHLD. Child process terminated.");
        case SIGCONT:
            throw SignalException("Received SIGCONT. Continue stopped process.");
        case SIGSTOP:
            throw SignalException("Received SIGSTOP. Stop process (cannot be caught or ignored).");
        case SIGTSTP:
            throw SignalException("Received SIGTSTP. Terminal stop signal.");
        case SIGTTIN:
            throw SignalException("Received SIGTTIN. Background process attempting to read from terminal.");
        case SIGTTOU:
            throw SignalException("Received SIGTTOU. Background process attempting to write to terminal.");
        case SIGWINCH:
            throw SignalException("Received SIGWINCH. Window size change.");
        case SIGPWR:
            throw SignalException("Received SIGPWR. Power failure.");
        case SIGSYS:
            throw SignalException("Received SIGSYS. Bad system call.");
        default:
            throw SignalException("Received unknown signal.");
    }
}
