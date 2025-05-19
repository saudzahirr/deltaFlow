#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <fmt/chrono.h>

enum class Level : uint8_t {
    NOTSET,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

#define DEBUG(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::DEBUG)
#define INFO(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::INFO)
#define WARN(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::WARN)
#define ERROR(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::ERROR)
#define CRITICAL(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::CRITICAL)

class Logger final {
    public:
        static Logger& getLogger();
        void log(const std::string& msg, const Level& level);

    private:
        Level m_Level;
        std::string m_FilePath;
        std::ofstream file;

        Logger(const std::string& name, Level level);
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
};

#endif
