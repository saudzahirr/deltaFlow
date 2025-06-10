#include "Logger.H"


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
    std::string levelStr;
    m_Level = level;
    fmt::color levelColor = fmt::color::white;

    switch (m_Level) {
        case Level::DEBUG:
            levelStr = "DEBUG"; levelColor = fmt::color::light_blue; break;
        case Level::INFO:
            levelStr = "INFO"; levelColor = fmt::color::green; break;
        case Level::WARN:
            levelStr = "WARN"; levelColor = fmt::color::yellow; break;
        case Level::ERROR:
            levelStr = "ERROR"; levelColor = fmt::color::orange_red; break;
        case Level::CRITICAL:
            levelStr = "CRITICAL"; levelColor = fmt::color::red; break;
        default:
            levelStr = "LOG"; break;
    }

    auto now = std::time(nullptr);
    auto timestamp = fmt::format("{:%d-%m-%Y %H-%M-%S}", fmt::localtime(now));

    std::string logMessage = fmt::format(
        "{} - {:<8} - {}\n", timestamp, levelStr, msg
    );

    if (file.is_open()) {
        file << logMessage;
    }

    logMessage = fmt::format(
        "{} - {} - {}\n",
        timestamp,
        fmt::format(fg(levelColor) | fmt::emphasis::bold, "{:<8}", levelStr),
        msg
    );

    fmt::print("{}", logMessage);
}
