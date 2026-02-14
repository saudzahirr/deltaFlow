/*
 * Copyright (c) 2024 Saud Zahir
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with deltaFlow.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Logger utilities for deltaFlow, providing logging macros and a singleton Logger class.
 *
 * This file declares the logging macros and the Logger class for handling logging at various severity levels.
 * The logger supports logging to a file and console with different levels:
 * NOTSET, DEBUG, INFO, WARN, ERROR, CRITICAL.
 * 
 * Macros (e.g., DEBUG, INFO) simplify use throughout the codebase.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
  #ifdef ERROR
    #undef ERROR
  #endif
#endif

/**
  * @enum Level
  * @brief Log severity levels.
  *
  * Supported levels:
  * - NOTSET
  * - DEBUG
  * - INFO
  * - WARN
  * - ERROR
  * - CRITICAL
  */
enum class Level : uint8_t {
    NOTSET,    ///< No level set
    DEBUG,     ///< Debug messages
    INFO,      ///< Informational messages
    WARN,      ///< Warning conditions
    ERROR,     ///< Error conditions
    CRITICAL   ///< Critical conditions
};

/**
  * @def LOG_DEBUG(msg, ...)
  * @brief Macro for logging a debug-level message.
  * @def LOG_INFO(msg, ...)
  * @brief Macro for logging an info-level message.
  * @def LOG_WARN(msg, ...)
  * @brief Macro for logging a warning-level message.
  * @def LOG_ERROR(msg, ...)
  * @brief Macro for logging an error-level message.
  * @def LOG_CRITICAL(msg, ...)
  * @brief Macro for logging a critical-level message.
  * @def LOG_MESSAGE(msg, ...)
  * @brief Macro for printing messages to stdout.
  *
  * All macros use fmt formatting and forward the formatted message to the logger.
  */
#define LOG_DEBUG(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::DEBUG)
#define LOG_INFO(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::INFO)
#define LOG_WARN(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::WARN)
#define LOG_ERROR(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::ERROR)
#define LOG_CRITICAL(msg, ...) Logger::getLogger().log(fmt::format(FMT_STRING(msg), ##__VA_ARGS__), Level::CRITICAL)
#define LOG_MESSAGE(msg, ...) fmt::print("{}\n", fmt::format(FMT_STRING(msg), ##__VA_ARGS__))

 /**
  * @class Logger
  * @brief Singleton logger class for deltaFlow.
  *
  * Provides methods for logging messages at various severity levels to a file and/or console.
  * Usage is typically via the provided macros.
  */
class Logger final {
    public:
        /**
         * @brief Get the singleton Logger instance.
         * @return Reference to the Logger instance.
         */
        static Logger& getLogger();

        /**
         * @brief Log a message at a given severity level.
         * @param msg The message to log.
         * @param level The severity level.
         */
        void log(const std::string& msg, const Level& level);

    private:
        Level m_Level;              ///< Minimum logging level
        std::string m_FilePath;     ///< Path to the log file
        std::ofstream file;         ///< Output file stream

        /**
         * @brief Construct a logger with file name and log level.
         * @param name Name or file path for the logger.
         * @param level Minimum severity level to log.
         */
        Logger(const std::string& name, Level level);

        /**
         * @brief Destructor flushes and closes log file.
         */
        ~Logger();

        // Disable copying
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
};

#endif
