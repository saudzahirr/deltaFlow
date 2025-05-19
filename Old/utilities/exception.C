#include "exception.H"

RuntimeError::RuntimeError(const std::string& msg) : message(msg) {}

const char* RuntimeError::what() const noexcept {
    return message.c_str();
}

extern "C" void signalHandler(int signal) {
    switch (signal) {
        case SIGINT:
            throw RuntimeError("Received SIGINT (Ctrl+C). Program interrupted.");
        case SIGTERM:
            throw RuntimeError("Received SIGTERM. Termination signal.");
        case SIGSEGV:
            throw RuntimeError("Received SIGSEGV (Segmentation Fault). Invalid memory access.");
        case SIGABRT:
            throw RuntimeError("Received SIGABRT. Abort signal.");
        case SIGFPE:
            throw RuntimeError("Received SIGFPE (Floating Point Exception). Arithmetic error.");
        case SIGILL:
            throw RuntimeError("Received SIGILL (Illegal Instruction). Invalid instruction.");
        case SIGBUS:
            throw RuntimeError("Received SIGBUS (Bus Error). Hardware fault or invalid memory access.");
        case SIGPIPE:
            throw RuntimeError("Received SIGPIPE. Broken pipe.");
        case SIGALRM:
            throw RuntimeError("Received SIGALRM. Alarm clock.");
        case SIGUSR1:
            throw RuntimeError("Received SIGUSR1. User-defined signal 1.");
        case SIGUSR2:
            throw RuntimeError("Received SIGUSR2. User-defined signal 2.");
        case SIGCHLD:
            throw RuntimeError("Received SIGCHLD. Child process terminated.");
        case SIGCONT:
            throw RuntimeError("Received SIGCONT. Continue stopped process.");
        case SIGSTOP:
            throw RuntimeError("Received SIGSTOP. Stop process (cannot be caught or ignored).");
        case SIGTSTP:
            throw RuntimeError("Received SIGTSTP. Terminal stop signal.");
        case SIGTTIN:
            throw RuntimeError("Received SIGTTIN. Background process attempting to read from terminal.");
        case SIGTTOU:
            throw RuntimeError("Received SIGTTOU. Background process attempting to write to terminal.");
        case SIGWINCH:
            throw RuntimeError("Received SIGWINCH. Window size change.");
        case SIGPWR:
            throw RuntimeError("Received SIGPWR. Power failure.");
        case SIGSYS:
            throw RuntimeError("Received SIGSYS. Bad system call.");
        default:
            throw RuntimeError("Received unknown signal.");
    }
}
