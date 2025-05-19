#include "logger.H"


Logger& Logger::getLogger() {
    static Logger instance("deltaFlow.log", Level::DEBUG);
    return instance;
}

Logger::Logger(const std::string& name, Level level) : m_FilePath(name), m_Level(level) {
    file.open(m_FilePath);
    if (not file.is_open()) {
        std::cerr << "Failed to open log file: " << m_FilePath << std::endl;
    }
}

Logger::~Logger() {
    if (file.is_open()) {
        file.close();
    }
}

void Logger::log(const std::string& msg, const Level& level) {
    std::string levelString;
    m_Level = level;
    switch (m_Level) {
        case Level::DEBUG:
            levelString = "DEBUG: ";
            break;
        case Level::INFO:
            levelString = "INFO: ";
            break;
        case Level::WARN:
            levelString = "WARN: ";
            break;
        case Level::ERROR:
            levelString = "ERROR: ";
            break;
        case Level::CRITICAL:
            levelString = "CRITICAL: ";
            break;
        case Level::NOTSET:
        default:
            levelString = "LOG: ";
            break;
    }

    std::string logMessage = fmt::format(
        "{:%d-%m-%Y %H-%M-%S} - {:<8} - {}\n",
        fmt::localtime(std::time(nullptr)),
        levelString,
        msg
    );

    if (file.is_open()) {
        file << logMessage;
    }

    fmt::print("{}", logMessage);
}
